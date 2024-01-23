/*
	gweapons.qc - Gizmo's new Coop weapons

	FIXME: rename file?
*/

#include "progs.h"
#include "qw.h"
#include "custom.h"

namespace Progs {

entity solidTestEntity;

// PZ: It's so nice in C++ to be able to create data for players in places outside of entity fields.
class ZGG_UserData
{
  public:
	float playerYawWhileHoldingObjectLastFrame;
};

ZGG_UserData zggUserData[MAX_CLIENTS]; // For 32 possible players.

/*
===============
RemoveTestEntity

This is just used to remove the test entity after half a second,
to avoid creating and removing a bunch of entities and causing
the server's sv.num_edicts to increase a great deal.
===============
*/
void RemoveTestEntity()
{
	solidTestEntity = world;
	remove(self);
}

/*
===============
InSolid

Checks if the entity is inside the world or another entity.
===============
*/
float InSolid(entity ent)
{
	entity	oself;
	vector	oldownerorg, oldorg, neworg;
	float		ret;

/*
	local float oldsolid;

	// first make sure the entity won't get in the way
	oldsolid = ent.solid;
	ent.solid = #SOLID_NOT;

	// create an entity to test with
	tent = spawn ();
	tent.solid = #SOLID_SLIDEBOX;
	tent.movetype = #MOVETYPE_STEP;
	setsize (tent, ent.mins, ent.maxs);
	setorigin (tent, ent.origin);

	oself = self;
	self = tent;

	self.origin_z = self.origin_z + 1;
	ret = droptofloor ();

	//ret = walkmove (0, 0);

	self = oself;

	// reset the entity's solid state
	ent.solid = oldsolid;

	remove (tent);
	return ret;
*/

	// 2/28/08: since we can't use ent.owner as the owner for solidTestEntity.owner, link ent.owner out of the way
	if (ent->owner != world)
	{
		oldownerorg = ent->owner->origin;
		// PZ NOTE: I guess we move the ent's owner way up in the air to get it out of the way temporarily.
		ent->owner->origin[Z] = ent->owner->origin[Z] + ent->owner->maxs[Z] * 2;
		setorigin(ent->owner, ent->owner->origin + V({0, 0, 200}));
	}

	// don't spam a bunch of entity spawn() and remove() calls if this is called often
	if (solidTestEntity == world)
	{
		solidTestEntity = spawnServerSide();      // PZ: make it a server-side only entity
		solidTestEntity->think = RemoveTestEntity;
	}

	solidTestEntity->nextthink = time + 0.6;

	solidTestEntity->owner = ent;
	solidTestEntity->solid = PR_SOLID_SLIDEBOX;
	solidTestEntity->movetype = PR_MOVETYPE_STEP;
	//solidTestEntity.flags = #FL_FLY;
	solidTestEntity->flags = PR_FL_ONGROUND | PR_FL_PARTIALGROUND;
	setsize(solidTestEntity, ent->mins, ent->maxs);
	setorigin(solidTestEntity, ent->origin);

	// check if we're inside something
	oself = self;
	self = solidTestEntity;

	oldorg = solidTestEntity->origin;
	ret = walkmove(0, 0);
	neworg = solidTestEntity->origin;
	setorigin(solidTestEntity, oldorg);

	if (oldorg != neworg && !droptofloor())
		ret = PR_FALSE;

	self = oself;

	// set origin back to normal if needed
	if (ent->owner != world)
		setorigin(ent->owner, oldownerorg);

	// make sure the test entity can't interact with anything now
	solidTestEntity->solid = PR_SOLID_NOT;
	solidTestEntity->movetype = PR_MOVETYPE_NONE;
	solidTestEntity->owner = world;

	return !ret;
	//return qc_droptofloor (ent, 4096, #TRUE);
}

//==============================================================

#ifndef PR_NO_ZEROGRAVGUN

/*
=============================
Zero-Gravity gun
=============================
*/

// PZ: TheGizmo used the names 'pickuper' and 'pickupee'. I couldn't stand those names (too similar, for one thing). So, I renamed them to 'user' and 'theObject'.

#define PR_ZG_PICKUPDIST_EXTRA  48                  // extra distance added to theObject's maxs, was 32
#define PR_ZGG_SOUND_EFFECT     "items/suit.wav"    // PZ: added

// FIXME: change some of these to macros
FIELD(".entity pickedupobject_hands;")
FIELD(".entity pickupthink_hands;")
FIELD(".entity _oldowner;")
FIELD(".entity pickedupby;")          // set if the object is being picked up
FIELD(".float  oldmovetype;")

float   unlockedZeroGrav;    // set to TRUE if unlocked

void zg_drop(entity user, float toss);
float zg_getpickupdist(entity theObject);

//=======================

// PZ: Let's give the ZGG some visual and sound effects, similar to the lightning gun.
void ZGG_precache()
{
	precache_sound(PR_ZGG_SOUND_EFFECT);
}

// PZ: I only want the two lightning gun player model shoot frames -- light1 and light2. But in order to use those two macros, I have to include
// the whole list of player model frames here. Instead of doing that, I'm just going to manually write the below two functions, doing the parts
// that would normally be done automatically for you with frame/state functions, and I'm going to set the player's frame number to the actual
// number rather than the macro, since I can't access the macro that is listed in player.qc, and don't want to include the full thing here.
//$frame light1 light2
void player_run(); void player_zgg2();
void player_zgg1() /* [$light1, player_zgg2] */ // doing this part manually
{
	// doing this manually
	self->frame = 105;             // the model frame to displayed ($light1)
	self->nextthink = time + 0.1;  // next frame happens in 1/10 of second
	self->think = player_zgg2;     // the function to call at the next frame

	muzzleflash();

	if (/*!self.#BUTTON_FIRE*/ self->pickedupobject_hands == world || intermission_running)
		{player_run(); return;}
	self->weaponframe = self->weaponframe + 1;
	if (self->weaponframe == 5)
		self->weaponframe = 1;
	//SuperDamageSound();
	//W_FireLightning();
	//Attack_Finished(0.2);
}
void player_zgg2() /* [$light2, player_zgg1] */ // doing this part manually
{
	// doing this manually
	self->frame = 106;             // the model frame to displayed ($light2)
	self->nextthink = time + 0.1;  // next frame happens in 1/10 of second
	self->think = player_zgg1;     // the function to call at the next frame

	if (/*!self.#BUTTON_FIRE*/ self->pickedupobject_hands == world || intermission_running)
		{player_run(); return;}
	self->weaponframe = self->weaponframe + 1;
	if (self->weaponframe == 5)
		self->weaponframe = 1;
	//SuperDamageSound();
	//W_FireLightning();
	//Attack_Finished(0.2);
}

// PZ NOTE: This just makes the object move towards `user`, right?
void zg_movetowards(entity user, entity theObject, const vector& destination)
{
	float vel, distfromuser;
	vector oldOrg;

	vel = vlen(theObject->origin - destination);
	distfromuser = vlen(user->origin - theObject->origin);

	if (distfromuser > zg_getpickupdist(theObject) + 32)
	{
		theObject->velocity = V({0, 0, 0});
		zg_drop(user, PR_FALSE);
		return;
	}

	if (theObject->origin != destination)
		theObject->velocity = user->velocity + ((normalize(destination - theObject->origin) * vel) * 10);
	else
		theObject->velocity = user->velocity;

	// PZ: Let's make it so that if this is the sentry gun, that its base/stand is also moved with the top/turret.
	// PZ NOTE: I've tried to make the turret's collision detection box cover the sentry gun's stand, but when I try to move the bounding box down,
	//          the turret starts acting weird and improper. So, I'm having to handle the stand in a non-ideal way. It would be so easy if the collision
	//          detection for both was done at the same time as a group.
	if (theObject->classname == "building_sentrygun")
	{
		theObject->trigger_field->velocity = theObject->velocity;               // without this, movement of the base is jittery
		oldOrg = theObject->trigger_field->origin;
		setorigin(theObject->trigger_field, theObject->origin - V({0, 0, 8}));  // without this, base will keep moving when turret collides with objects
		// ### For some reason, this code causes the engine to lock up in a loop.
		/*if (CheckArea(theObject.trigger_field, theObject) == #FALSE)
		{
			setorigin(theObject.trigger_field, oldOrg);
			//theObject.velocity = '0 0 0';
		}*/
	}
}

float zg_getpickupdist(entity theObject)
{
	float sizex, sizey;

	if (theObject->solid == PR_SOLID_NOT || theObject->solid == PR_SOLID_TRIGGER || (!theObject->mins[X] && !theObject->maxs[X]))
		return PR_ZG_PICKUPDIST_EXTRA;

	sizex = theObject->maxs[X] + PR_ZG_PICKUPDIST_EXTRA/*(theObject.mins_x * -1) + theObject.maxs_x*/;
	sizey = theObject->maxs[Y] + PR_ZG_PICKUPDIST_EXTRA/*(theObject.mins_y * -1) + theObject.maxs_y*/;

	// go for the biggest side
	if (sizex >= sizey)
		return sizex;
	else
		return sizey;
}

float zg_infront(entity source, entity targ)
{
	vector vec;
	float  dot;

	makevectors (source->angles);	// BUG: this makes up and down not work, use .v_angle for players instead

	vec = normalize (targ->origin - source->origin);
	dot = vec * v_forward;

	if (dot > 0.3)
	{
		return PR_TRUE;
	}
	return PR_FALSE;
}

float zg_invalidtarget(entity tester, entity test)
{
	if (test->solid == PR_SOLID_BSP || test == world /*|| test.movetype == #MOVETYPE_NONE*/ || test->pickedupby != world)
		return PR_TRUE;
	if (/*test.flags & #FL_MONSTER ||*/ !zg_infront(tester, test))
		return PR_TRUE;
	if (IsBuilding(test) && test->real_owner != tester)
		return 2;

	if (!unlockedZeroGrav) {
		if (test->flags & PR_FL_ITEM)
			return 3;
		if (test->flags & PR_FL_CLIENT || test->classname == "player")
			return PR_TRUE;
	}

	if (unlockedZeroGrav < 2) {
		if (test->movetype == PR_MOVETYPE_NONE || test->flags & PR_FL_MONSTER)
			return PR_TRUE;
	}

	return PR_FALSE;
}

entity zg_findtarget(entity source)
{
	entity radiusent;
	float ret = 0, badpickup;

	badpickup = 0;

	makevectors (source->v_angle);

	// try directly at what we're looking at
	traceline (source->origin, (source->origin + V({0, 0, 16})) + v_forward*64, PR_TL_ANY_SOLID, source);

	if (trace_fraction != 1.0 && !(ret = zg_invalidtarget(source, trace_ent)))
		return trace_ent;

	if (ret > 1)
		badpickup = ret;

	// if nothing's directly infront try scanning for something
	radiusent = findradius (source->origin, 64);

	while (radiusent != world)
	{
		if (!(ret = zg_invalidtarget(source, radiusent)))
			return radiusent;

		if (ret > 1)
			badpickup = ret;

		radiusent = radiusent->chain;
	}

	if (source->flags & PR_FL_CLIENT) {
		if (badpickup == 2)
			sprint (source, PR_PRINT_HIGH, "Sorry, only the owner can pickup his builds!\n");
		else if (badpickup == 3)
			sprint (source, PR_PRINT_HIGH, "Sorry, picking up items is disabled!\n");
		else
			sprint (source, PR_PRINT_HIGH, "There's nothing there to pickup\n");
	}

	return world;
}

//=======================
// PZ NOTE: This mostly is responsible for performing necessary functions while a ZGG is holding an object.

void zg_think()
{
	entity user, theObject;

	user = self->owner;
	theObject = self->owner->pickedupobject_hands;

	// PZ: Added for being able to rotate objects with the ZGG.
	//bprint(2, string("user entity number = ") + to_string(ENT_TO_NUM(user)) + "\n");
	int userNdx = ENT_TO_NUM(user)-1;   // `world`'s entity number is 0. We don't include a slot for world in the data structure.

	// remove onground flag
	theObject->flags = theObject->flags - (theObject->flags & (PR_FL_ONGROUND | PR_FL_PARTIALGROUND));

	// if the user doesn't have anything picked up then remove ourself
	// FIXME: this should never happen, zg_drop should be the only thing that messes with theObject and removes the thinker
	if (theObject == world)
		remove(self);

	if ( theObject->classname == "player" ) {
		if ( theObject->gravity ) {
			theObject->oldmovetype = theObject->gravity;
			theObject->gravity = 0;
		}
	} else {
		// 10/12/07: if the theObject's movetype has changed, save it and reset it back to MOVETYPE_FLY, can happen
		// FIXME: if the object's new movetype is actually set somewhere to MOVETYPE_FLY, there's no way to know it changed, and the old movetype will still be set when dropped instead of fly
		if ( theObject->movetype != PR_MOVETYPE_FLY ) {
			theObject->oldmovetype = theObject->movetype;
			theObject->movetype = PR_MOVETYPE_FLY;
		}
	}

	// check if the user is dead
	if (user->health <= 0 || (theObject->takedamage && theObject->health <= 0) || theObject->model == "" || !user->is_connected) {	// *
		zg_drop(user, PR_FALSE);
		return;
	}

	makevectors(user->v_angle);

	// this is based upon the entity's size
/*
	// there's a bug in cpqwsv, for some reason the trace_endpos is screwed on this, works fine on other qwsv's
	// PZ NOTE: I think your problem is that you are calling a function inside a function call. Too bad most QC compilers just happily go on and compile code like that without
	//          complaining. If I'm not mistaken, QC function arguments/parameters (max of 8) and return values (1, of course) are all shared in one global variable -- one global each,
	//          for each argument and for the return value. This is why you can't do "if (Teammate(self.team_no, player.team_no) || Teammate(self.team_no, player.undercover_team)).
	//          The return value of the second one will overwrite the return value of the first, because there is only one place in memory for the return value to live, shared by
	//          all QC function calls. I think the same happened below when TheGizmo tried the below. One of the below function calls stepped on argument 1 of the other's function call.
	//          There may have also been a return value collision here. All he needed to do was store the result of zg_getpickupdist(theObject), first, in a separate variable.
	traceline (user.origin, (user.origin + '0 0 16') + v_forward*zg_getpickupdist(theObject), #TL_BSP_ONLY, user);
*/
	trace_endpos = (user->origin + V({0, 0, 16})) + v_forward*zg_getpickupdist(theObject);

	zg_movetowards(user, theObject, trace_endpos);

	// PZ: Rotate the object on the yaw axis, as the player rotates.
	//bprint(2, string("user->v_angle[YAW] - zggUserData[userNdx].playerYawWhileHoldingObjectLastFrame = ") +
	//          to_string(user->v_angle[YAW] - zggUserData[userNdx].playerYawWhileHoldingObjectLastFrame) + "\n");
	// NOTE: Sentry guns need to have their turret's yaw center rotated.
	if (theObject->classname == "building_sentrygun")
	{
		theObject->waitmin += user->v_angle[YAW] - zggUserData[userNdx].playerYawWhileHoldingObjectLastFrame;
		theObject->waitmax  = theObject->waitmin + PR_SENTRY_ROTATIONWIDTH * 2;
	}
	else
		theObject->angles[YAW] += user->v_angle[YAW] - zggUserData[userNdx].playerYawWhileHoldingObjectLastFrame;

	// PZ: Give the ZGG a sound.
	if (time >= self->t_width) // `t_width` just holds a time for sound playing
	{
		sound(user, PR_CHAN_WEAPON, PR_ZGG_SOUND_EFFECT, 1, PR_ATTN_NORM);
		self->t_width = time + 1;
	}

	self->nextthink = time + 0.05;

	// PZ: Remember the player's yaw angle for next frame.
	zggUserData[userNdx].playerYawWhileHoldingObjectLastFrame = user->v_angle[YAW];
}

// PZ NOTE: `toss` seems to always be false with current code.
void zg_drop(entity user, float toss)
{
	entity theObject, oldself;
	float isObjectASentryGunTurret; // PZ: for sentry guns

	theObject = user->pickedupobject_hands;

	makevectors(user->v_angle);

	// checked based upon the entity's size
	//traceline (user.origin, (user.origin + '0 0 16') + v_forward*64, #TL_ANY_SOLID, user);

	// make sure we have something picked up
	if (user->pickedupobject_hands == world)
		return;

	// PZ: for sentry guns
	if (theObject->classname == "building_sentrygun") isObjectASentryGunTurret = PR_TRUE;
	else                                              isObjectASentryGunTurret = PR_FALSE;

	// PZ: Making it so that field generators are easy to realign when moving with ZGG.
	if (theObject->classname == "building_fieldgen")  theObject->trigger_field = user;

	/*
	if ((trace_fraction != 1.0 || zg_insolid(theObject)) && !(user.health <= 0 || (theObject.takedamage && theObject.health <= 0) || theObject.model == "")) {
		if (user.flags & #FL_CLIENT)
			sprint (user, #PRINT_HIGH, "Not enough room here\n");

		return;
	}
	*/

	// fix user and theObject
	remove(user->pickupthink_hands);  // PZ NOTE: removes think timer entity

	if (toss) {
		theObject->velocity = v_forward * 500;

		if (theObject->oldmovetype != PR_MOVETYPE_FLY && theObject->oldmovetype != PR_MOVETYPE_FLYMISSILE)
			theObject->velocity = theObject->velocity + v_up * 200;
	} else
		theObject->velocity[Z] = theObject->velocity[Z] + 10;	// *

	// PZ: Let's make it so that if this is the sentry gun, that its base/stand is also moved with the top/turret.
	if (isObjectASentryGunTurret)
	{
		theObject->trigger_field->velocity = theObject->velocity;               // without this, movement of the base is jittery
		setorigin(theObject->trigger_field, theObject->origin - V({0, 0, 8}));  // without this, base will keep moving when turret collides with objects
	}

	// 1/18/07: Gizmo - added this above InSolid() so it can check properly, and also if it fails
	user->owner = user->_oldowner;

	// before we reset owner, check whether theObject is inside of the user
	if (InSolid(theObject))
	{
		oldself = self;
		self = theObject;
		self->velocity = V({0, 0, 0});
		user->owner = theObject;
		droptofloor();
		self = oldself;
	}
	// PZ: Let's make it so that if this is the sentry gun, that its base/stand is also moved with the top/turret.
	if (isObjectASentryGunTurret)
	{
		if (InSolid(theObject->trigger_field))
		{
			oldself = self;
			self = theObject->trigger_field;
			self->velocity = V({0, 0, 0});
			user->owner = theObject->trigger_field;
			droptofloor();
			self = oldself;
		}
	}

	user->owner = user->_oldowner;

	// players are handled differently
	if (theObject->classname == "player")
		theObject->gravity = theObject->oldmovetype;
	else
	{
		theObject->movetype = theObject->oldmovetype;
		// PZ: Let's make it so that if this is the sentry gun, that its base/stand is also moved with the top/turret.
		if (isObjectASentryGunTurret)
			theObject->trigger_field->movetype = theObject->trigger_field->oldmovetype;
	}

	theObject->pickedupby = world;					// *
	// PZ: Let's make it so that if this is the sentry gun, that its base/stand is also moved with the top/turret.
	if (isObjectASentryGunTurret)
		theObject->trigger_field->pickedupby = world;

	user->pickedupobject_hands = world;

	// reset weaponmodel
	if (user->health > 0)
	{
		oldself = self;
		self = user;
		W_SetCurrentAmmo();
		self = oldself;
	}
}

//#define ZGG_DETPACK_RESTRICTION_TESTING
void zg_pickup(entity user)
{
	entity thinker;
	bool   objectIsSentryGun;

	// PZ: Added for being able to rotate objects with the ZGG.
	//bprint(2, string("user entity number = ") + to_string(ENT_TO_NUM(user)) + "\n");
	int userNdx = ENT_TO_NUM(user)-1;   // `world`'s entity number is 0. We don't include a slot for world in the data structure.

	trace_ent = zg_findtarget(user);

	if (trace_ent == world)
		return;

	// make sure we're not already picking something up
	if (user->pickedupobject_hands != world)
		return;

#ifndef PR_ZGG_DETPACK_RESTRICTION_TESTING
	// PZ: with the bots, picking up everything is kind of okay, at least for right now
	/*if (deathmatch && (trace_ent.classname == "detpack" || trace_ent.classname == "building_dispenser"))
	{
		sprint(user, #PRINT_HIGH, "For the time being, can't pickup detpacks/dispensers at all.\n");
		return;
	}*/
#endif

	// setup user and theObject
	user->_oldowner = user->owner;   // *
	user->owner = trace_ent;        // *

	if (trace_ent->classname == "building_sentrygun") objectIsSentryGun = PR_TRUE;
	else                                              objectIsSentryGun = PR_FALSE;

	// players are handled differently, since .gravity only works on players
	if (trace_ent->classname == "player")
	{
		trace_ent->oldmovetype = trace_ent->gravity;
		trace_ent->gravity = 0;
	}
	else
	{
		// !! changing the size isn't working for some reason; this seems to have no effect. If you give it a negative mins_z, then it loses all collision detection.
		//if (objectIsSentryGun) {/*setmodel(trace_ent, trace_ent.mdl);*/ setsize(trace_ent, trace_ent.mins - '0 0 16', trace_ent.maxs - '0 0 16'); /*setorigin(trace_ent, trace_ent.origin);*/}

		trace_ent->oldmovetype = trace_ent->movetype;
		trace_ent->movetype = PR_MOVETYPE_FLY;

		// !! changing the size isn't working for some reason; this seems to have no effect. If you give it a negative mins_z, then it loses all collision detection.
		//if (objectIsSentryGun) {/*setmodel(trace_ent, trace_ent.mdl);*/ setsize(trace_ent, trace_ent.mins - '0 0 16', trace_ent.maxs - '0 0 16'); /*setorigin(trace_ent, trace_ent.origin);*/}

		// PZ: Let's make it so that if this is the sentry gun, that its base/stand is also moved with the top/turret.
		if (objectIsSentryGun)
		{
			trace_ent->trigger_field->oldmovetype = trace_ent->trigger_field->movetype;
			trace_ent->trigger_field->movetype = PR_MOVETYPE_FLY;
			// set the size of the turret to include the size of the base, to make the bounds checking performed on the base as well, as a group
			//setsize(trace_ent, trace_ent.mins - '0 0 48', trace_ent.maxs);
		}
		//force_retouch = 2;		// make sure even still objects get hit // PZ: Testing
	}

	trace_ent->pickedupby = user;                              // * for knowing if it's picked up
	trace_ent->flags = trace_ent->flags - PR_FL_ONGROUND;      // *
	// PZ: Let's make it so that if this is the sentry gun, that its base/stand is also moved with the top/turret.
	if (objectIsSentryGun)
	{
		trace_ent->trigger_field->pickedupby = user;
		trace_ent->trigger_field->flags = trace_ent->trigger_field->flags - PR_FL_ONGROUND;
	}

	// fire the onpickup event on the theObject
	//if (trace_ent.onpickup)
	//	trace_ent.onpickup (user, trace_ent);

	user->pickedupobject_hands = trace_ent;

#ifdef PR_ZGG_DETPACK_RESTRICTION_TESTING
 	float ran;

	ran = random();

sprint(user, PR_PRINT_HIGH, "ran = ");
string tmp; tmp = ftos(ran);
sprint(user, PR_PRINT_HIGH, tmp); sprint(user, PR_PRINT_HIGH, "\n");

	if (deathmatch && trace_ent->classname == "detpack")
	{
		sprint(user, PR_PRINT_HIGH, "Careful. Detpacks and zero gravity guns don't get along very well..\n");
		// calc random number
		if ((trace_ent->owner->team_no == user->team_no && ran < PR_ZGG_FRIENDLY_DETPACK_EXPLODE_CHANCE) ||
			(trace_ent->owner->team_no != user->team_no && ran < PR_ZGG_ENEMY_DETPACK_EXPLODE_CHANCE))
		{
if (trace_ent->owner->team_no == user->team_no)
	sprint(user, PR_PRINT_HIGH, "FRIENDLY DET: BEEEEEEEP!\n");
else
	sprint(user, PR_PRINT_HIGH, "ENEMY DET: BEEEEEEEP!\n");
			// beep error sound
			sound(trace_ent, PR_CHAN_WEAPON, "sub/locked.wav", 1, PR_ATTN_NORM);
			// explode 2 seconds later
			if (trace_ent->nextthink > time + 2)
				trace_ent->nextthink = time + 2;
			trace_ent->linked_list->health = 0; // stop the detpack's timer
		}
	}
#endif

	// PZ: Remember the yaw angle the player was looking when they picked up the object.
	zggUserData[userNdx].playerYawWhileHoldingObjectLastFrame = user->v_angle[YAW];

	// create the thinker
	thinker = spawnServerSide(); // PZ: make it a server-side only entity
	thinker->owner = user;

	thinker->nextthink = time + 0.01;
	thinker->think = zg_think;

	// PZ: If theObject is a sentrygun, let's make the thinker entity double as a 'bounding box' entity for turret-base pair, so that we can have collision detection
	//     done on the two as a group. ### I don't think I like this idea. Even if I did get it working, it seems like it would require dirtying up the ZGG code a lot.
	/*if (objectIsSentryGun)
	{
		thinker.
	}*/

	user->pickupthink_hands = thinker;

	// PZ: Give the ZGG some visual effects, similar to the lighting gun.
	entity oldSelf = self;
	self = user;
	player_zgg1();
	self = oldSelf;
}

void zg_togglepickup(entity user, float toss)
{
	// check for unlocking the gravity gun here instead of every frame
	if (infokey(world, "zg_unlocked") != "")
	{
		unlockedZeroGrav = stof(infokey(world, "zg_unlocked"));
		localcmd("localinfo zg_unlocked \"\"\n");
		localcmd("serverinfo zg_unlocked \"\"\n");
	}

	if (user->pickedupobject_hands == world)
		zg_pickup(user);
	else
		zg_drop(user, toss);
}
#endif

} // END namespace Progs
