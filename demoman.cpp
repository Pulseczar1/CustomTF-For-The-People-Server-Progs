/*======================================================
	DEMOMAN.QC

	TeamFortress v2.5	 29/2/97
========================================================
Functions for the DEMOMAN class and associated weaponry
========================================================*/

#include "progs.h"
#include "ofndefs.h"
#include "combat.h"
#include "weapons.h"
#include "debug.h"
#include "cpstuff.h"
#include "custom.h"

namespace Progs {

// Functions outside this file
void NormalGrenadeTouch();
void NormalGrenadeExplode();

// Functions in this file
// Pipebomb Functions
void TeamFortress_DetonatePipebombs();
void PipebombTouch();
// Mirv Grenade Functions
void MirvGrenadeTouch();
void MirvGrenadeExplode();
#ifdef PR_MIRV_VELOCITY
void MirvGrenadeLaunch(const vector& org, const vector& initVelocity, entity shooter);
#else
void MirvGrenadeLaunch(const vector& org, entity shooter);
#endif
// Detpack Functions
void TeamFortress_SetDetpack(float timer);
void TeamFortress_DetpackSet();
void TeamFortress_DetpackStop(float krac);
void TeamFortress_DetpackExplode();
void TeamFortress_DetpackTouch();
void TeamFortress_DetpackDisarm();
void TeamFortress_DetpackCountDown();

//=========================================================================
// Detonate all thrown pipebombs
void TeamFortress_DetonatePipebombs()
{
	entity e;

	// Find all this players pipebombs
	e = find(world, "classname", "pipebomb");
	while (e != world)
	{
    	if(e->owner == self)
			e->nextthink = time;

		e = find(e, "classname", "pipebomb");
	}
}

//=========================================================================
// Pipebomb touch function
void PipebombTouch()
{
	sound (self, PR_CHAN_WEAPON, "weapons/bounce.wav", 1, PR_ATTN_NORM);	// bounce sound
	if (self->velocity == V({0, 0, 0}))
		self->avelocity = V({0, 0, 0});
}

//=========================================================================
// Touch Function for Mirv Grenade
// Mirv Grenade heavily influenced by the Firewall Grenade by Steve Bond (wedge@nuc.net)
void MirvGrenadeTouch()
{
	// If the Mirv Grenade hits a player, it just bounces off
	sound (self, PR_CHAN_WEAPON, "weapons/bounce.wav", 1, PR_ATTN_NORM);
	if (self->velocity == V({0, 0, 0}))
		self->avelocity = V({0, 0, 0});
}

//=========================================================================
// Mirv Grenade explode function, for when the PRIMETIME runs out
void MirvGrenadeExplode()
{
	float i, nummirvs;

	deathmsg = PR_DMSG_GREN_MIRV;
	T_RadiusDamage (self, self->owner, 100, world);

	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
	WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
#ifdef PR_QUAKE_WORLD
	multicast (self->origin, PR_MULTICAST_PHS);
#endif

	self->solid = PR_SOLID_NOT;

	// Launch mirvs
    if (self->netname != "special_mine")
        nummirvs = PR_GR_TYPE_MIRV_NO;
    else
        nummirvs = PR_MINE_MIRV_NUM;

	i = 0;
	while (i < nummirvs)
	{
#ifdef PR_MIRV_VELOCITY
		// Gizmo - allow for the spamlets to inherit the mirv's velocity
		MirvGrenadeLaunch (self->origin + V({0, 0, -1}), self->velocity, self->owner);
#else
		MirvGrenadeLaunch (self->origin + V({0, 0, -1}),self->owner);
#endif
		i = i + 1;
	}

#ifdef PR_DEMO_STUFF
	// Remove any camera's locks on this missile
	if (self->enemy != world)
		CamProjectileLockOff();
#endif

	BecomeExplosion();
}

//=========================================================================
// Launch a Mirv
#ifdef PR_MIRV_VELOCITY
void MirvGrenadeLaunch(const vector& org, const vector& initVelocity, entity shooter)
#else
void MirvGrenadeLaunch(const vector& org, entity shooter)
#endif
{
	float xdir,ydir,zdir;

	xdir = 150 * random() - 75;
	ydir = 150 * random() - 75;
	zdir = 40 * random();

	newmis = spawn ();
	newmis->owner = shooter;
	newmis->movetype = PR_MOVETYPE_BOUNCE;
	newmis->solid = PR_SOLID_BBOX;

	newmis->classname = "grenade";
	newmis->weapon = PR_DMSG_GREN_MIRV;

#ifdef PR_NET_SERVER
	newmis->touch = NormalGrenadeTouch;
	newmis->think = NormalGrenadeExplode;
#else
	newmis->touch = GrenadeTouch;
	newmis->think = GrenadeExplode;
#endif

	//WK 2 + random
	newmis->nextthink = time + 0.75 + random();

	newmis->velocity[X] = xdir * 2;
	newmis->velocity[Y] = ydir * 2;
	newmis->velocity[Z] = zdir * 15;

#ifdef PR_MIRV_VELOCITY
	newmis->velocity = newmis->velocity + initVelocity;
#endif

	newmis->avelocity = V({100, 100, 400});

	setmodel (newmis, "progs/grenade2.mdl");
	setsize (newmis, PR_VEC_ORIGIN, PR_VEC_ORIGIN);
	setorigin (newmis, org);
}

//=========================================================================
// Handles the Setting of Detpacks
void TeamFortress_SetDetpack(float timer)
{
	string stimer;

	// prevent detpack impulse from triggering anything else
	self->impulse = 0;
	self->last_impulse = 0;

	if (!(self->cutf_items & PR_CUTF_DETPACK))
		return;

	if (self->ammo_detpack <= 0)
		return;

	// Cant set detpack if you're in the air
	if (!(self->flags & PR_FL_ONGROUND))
	{
		sprint (self, PR_PRINT_HIGH, "You can't set detpacks in the air!\n");
		return;
	}
	//WK Prevents spy sliding bug
	if (self->is_feigning)
	{
		sprint (self, PR_PRINT_HIGH, "You can't set detpacks while playing dead!\n");
		return;
	}
	if (self->is_haxxxoring)
	{
		sprint (self, PR_PRINT_HIGH, "You can't set a detpack while you're hacking something.\n");
		return;
	}
	if (timer < 5)
	{
		sprint (self, PR_PRINT_HIGH, "You can't set detpacks for less that 5 seconds.\n");
		return;
	}

	if (self->is_detpacking || self->is_toffingadet)
		return;

	self->is_detpacking = 1;
	self->ammo_detpack = self->ammo_detpack - 1;

	makeImmune(self,time + 2);
	//self.immune_to_check = time + 2;
	self->tfstate = self->tfstate | PR_TFSTATE_CANT_MOVE;
	// Save the current weapon and remove it
	self->weapon = self->current_weapon;
	self->current_weapon = 0;
	self->weaponmodel = "";
	self->weaponframe = 0;

#ifdef PR_QUAKE_WORLD
	TeamFortress_SetSpeed(self);
#endif

	self->pausetime = time + PR_WEAP_DETPACK_SETTIME;

	stimer = ftos(timer);

	sprint(self, PR_PRINT_HIGH, "Setting detpack for ");
	sprint(self, PR_PRINT_HIGH, stimer);
	sprint(self, PR_PRINT_HIGH, " seconds...\n");

	newmis = spawnServerSide(); // PZ: make it a server-side only entity
	newmis->owner = self;
	newmis->classname = "timer";
	newmis->netname = "detpack_timer";
	newmis->movetype = PR_MOVETYPE_NONE; //WK M3 Bug Hunt
	newmis->nextthink = time + PR_WEAP_DETPACK_SETTIME;
	newmis->think = TeamFortress_DetpackSet;
	newmis->health = timer;

//CH because its on the sbar :)
	self->StatusRefreshTime = time + 0.2;
	self->StatusBarScreen = 3;
}

//=========================================================================
// Stops the setting of the detpack
void TeamFortress_DetpackStop(float krac)
{
	entity detpack_timer;

	detpack_timer = find(world, "netname", "detpack_timer");
	while ((detpack_timer->owner != self) && (detpack_timer != world))
		detpack_timer = find(detpack_timer, "netname", "detpack_timer");

	if (detpack_timer == world)
		return;

	if (krac == PR_TRUE) {
		sprint(self, PR_PRINT_HIGH, "Your detpack got Kraced!\n");
	} else {
		sprint(self, PR_PRINT_HIGH, "Detpack retrieved.\n");
		self->ammo_detpack = self->ammo_detpack + 1;
	}

	dremove(detpack_timer);

	self->tfstate = self->tfstate - (self->tfstate & PR_TFSTATE_CANT_MOVE);

	self->is_detpacking = 0;
	self->current_weapon = self->weapon;
	W_SetCurrentAmmo();

#ifdef PR_QUAKE_WORLD
	TeamFortress_SetSpeed(self);
#endif

	self->pausetime = time;
}

//=========================================================================
// The detpack is set, let the player go and start timer
void TeamFortress_DetpackSet()
{
	entity countd, oldself;

	self->is_detpacking = 0;
	self->owner->tfstate = self->owner->tfstate - (self->owner->tfstate & PR_TFSTATE_CANT_MOVE);
    self->owner->is_detpacking = 0;

#ifdef PR_QUAKE_WORLD
	TeamFortress_SetSpeed(self->owner);
#endif

#ifdef PR_SPEECH
	stuffcmd(self->owner, "play speech/demo_dp.wav\n");
#endif

	oldself = self;
	self = self->owner;
	//self.is_detpacking = 0;
	self->current_weapon = self->weapon;
	W_SetCurrentAmmo();
	self = oldself;

	newmis = spawn ();
	newmis->owner = self->owner;
    newmis->origin = self->owner->origin - V({0, 0, 23});
	newmis->movetype = PR_MOVETYPE_BOUNCE;
#ifdef PR_QUAKE_WORLD
	newmis->solid = PR_SOLID_BBOX;
#else
	newmis->solid = PR_SOLID_TRIGGER;
#endif
	newmis->classname = "detpack";
    newmis->flags = PR_FL_ITEM;

	newmis->angles = V({90, 0, 0});
	newmis->angles[Y] = self->owner->angles[Y];
	newmis->velocity = V({0, 0, 0});
	newmis->avelocity = V({0, 0, 0});

	newmis->weaponmode = 0;  // Detpack weaponmode = 1 when disarming
   	newmis->touch = TeamFortress_DetpackTouch;

	setmodel (newmis, "progs/detpack.mdl");
	setsize (newmis, V({-16, -16, 0}), V({16, 16, 8}));
	setorigin (newmis, self->owner->origin);

	sound (newmis, PR_CHAN_WEAPON, "doors/medtry.wav", 1, PR_ATTN_NORM);  //CH play set detpack sound


	// Create the CountDown entity
	countd = spawnServerSide(); // PZ: make it a server-side only entity
	newmis->linked_list = countd;    // attach count to its detpack
	countd->think = TeamFortress_DetpackCountDown;
	countd->health = self->health - 1;
	countd->owner = self->owner;
	countd->movetype = PR_MOVETYPE_NONE; //WK M3 Bug Hunt
	countd->classname = "countdown_timer"; // Don't call it timer, because we
							  // don't want it removed if player dies
	countd->enemy = newmis;
	newmis->oldenemy = countd;
	if (self->health <= 10)
		countd->nextthink = time + 1;
	else
	{
		countd->nextthink = time + self->health - 10;
		countd->health = 9;
	}
	newmis->nextthink = time + self->health;
	newmis->think = TeamFortress_DetpackExplode;

	sprint(self->owner, PR_PRINT_HIGH, "Detpack set!\n");

    teamprefixsprint(self->owner->team_no,self->owner);
    teamsprint6(self->owner,self->owner->netname," sets a detpack!\n","","","","");

	dremove(self);
}

//- OfN - Used for extra det explosions
float vis2orig(const vector& targ, const vector& check);


//=========================================================================
// The detpack goes BOOM!
void TeamFortress_DetpackExplode()
{
	float pos;

	bprint(PR_PRINT_MEDIUM, "FIRE IN THE HOLE!\n");

	// Check the pointcontents to prevent detpack outside the world
	pos = pointcontents(self->origin);
	if (pos != PR_CONTENT_SOLID && pos != PR_CONTENT_SKY)
	{
		deathmsg = PR_DMSG_DETPACK;
		T_RadiusDamage (self, self->owner, PR_WEAP_DETPACK_SIZE, world);

		sound(self, PR_CHAN_MISC + PR_CHAN_NO_PHS_ADD, "weapons/detpack.wav", 1, PR_ATTN_NONE); // PZ: really make det sound world-wide

		WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
		WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
	#ifdef PR_QUAKE_WORLD
		multicast (self->origin, PR_MULTICAST_PHS);
	#endif

        WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_BROADCAST, PR_TE_LAVASPLASH);
		WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
	#ifdef PR_QUAKE_WORLD
		multicast (self->origin, PR_MULTICAST_PHS);
	#endif

    //- OfN - Should a detpack make the same explosion a gren does? dont think so...
    #ifdef PR_EXTRA_DETPACK_EXPLOSIONS

        float loopc;
        vector rexp;

        loopc = 0;

        while (loopc < PR_EXTRA_DETEXPLOSIONS)
        {
            rexp[X] = self->origin[X] + random()*400 - 200;
            rexp[Y] = self->origin[Y] + random()*400 - 200;
            rexp[Z] = self->origin[Z] + random()*100;

            if (vis2orig(self->origin,rexp))
            {
                WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
                WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
                WriteCoord (PR_MSG_BROADCAST, rexp[X]);
                WriteCoord (PR_MSG_BROADCAST, rexp[Y]);
                WriteCoord (PR_MSG_BROADCAST, rexp[Z]);
            #ifdef PR_QUAKE_WORLD
                multicast (rexp, PR_MULTICAST_PHS);
            #endif
            }
            loopc = loopc + 1;
        }
    #endif

    //- OfN - Moves a bit the players that are near the exploding detpack
    #ifdef PR_DETPACK_EARTHQUAKE

        entity head;

        vector v;

        head = findradius(self->origin, PR_DETPACK_EARTHQUAKE_RANGE);

        while (head != world)
        {
            if (head->classname == "player")
            if (head->health > 0)
            if (head->is_connected)
            if (head->playerclass != PR_PC_UNDEFINED)
            if (head->flags & PR_FL_ONGROUND || head->waterlevel)
            {
                v[X] = 100 * crandom();
            	v[Y] = 100 * crandom();
        	    v[Z] = 150 + 200 * random();
                head->velocity = head->velocity + v;
            }

            head = head->chain;
        }

    #endif


	}
	else
	{
//		bprint(#PRINT_HIGH, "Your detpack fizzled out.\n");
		sprint(self->owner, PR_PRINT_HIGH, "Your detpack fizzled out.\n");
	}

	// This code handles a disarming scout with protection
	if (self->weaponmode == 1)     	  // Detpack was being disarmed
	{
#ifdef PR_QUAKE_WORLD
		TeamFortress_SetSpeed(self->enemy);
#else
		self->enemy->pausetime = time;
#endif

		dremove(self->oldenemy);  	 // CountDown
		dremove(self->observer_list); //	Disarm timer
	}

	BecomeExplosion ();
}

//=========================================================================
// The detpack touch function. Scouts can disarm it.
void TeamFortress_DetpackTouch()
{
	entity disarm;

	if (other->classname != "player")
		return;

	//WK if (other.playerclass != #PC_SCOUT)
	if (!(other->tf_items & PR_NIT_SCANNER))
		return;

	if (self->weaponmode == 1)
	    return;

	if (Teammate(self->owner, other))
	    return;

	makeImmune(other,time + 2);
	//other.immune_to_check = time + 2;
	other->tfstate = other->tfstate | PR_TFSTATE_CANT_MOVE;

	sprint(other, PR_PRINT_HIGH, "Disarming detpack...\n");
#ifdef PR_QUAKE_WORLD
	TeamFortress_SetSpeed(other);
#else
	other->pausetime = time + PR_WEAP_DETPACK_DISARMTIME;
#endif

	// Spawn disarming entity
	disarm = spawnServerSide(); // PZ: make it a server-side only entity
	disarm->movetype = PR_MOVETYPE_NONE; //WK M3 Bug Hunt
	disarm->owner = other;		// the scout
	disarm->enemy = self;		// the detpack
	disarm->classname = "timer";
	disarm->nextthink = time + PR_WEAP_DETPACK_DISARMTIME;
	disarm->think = TeamFortress_DetpackDisarm;

	self->weaponmode = 1;  // indicates disarming
	self->enemy = other;   // points to scout
	self->observer_list = disarm;
}

//=========================================================================
// The detpack disarm function. Scout has finished disarming it
// .enemy is the detpack
// .owner is the scout
void TeamFortress_DetpackDisarm()
{
	if (self->owner->health <= 0) {
		dremove(self);
		return;
	}

	bprint (PR_PRINT_MEDIUM, self->enemy->owner->netname);
	bprint (PR_PRINT_MEDIUM, "'s detpack was defused by ");
	bprint (PR_PRINT_MEDIUM, self->owner->netname);
	bprint (PR_PRINT_MEDIUM, "\n");

    // Give disarmer a frag for doing the job - OfN
    self->owner->real_frags = self->owner->real_frags + 1;
    if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
        self->owner->frags = self->owner->real_frags;

    //if (!(self.owner.is_feigning))
	//- OfN already checked on TeamFortress_SetSpeed()
    //{
        self->owner->tfstate = self->owner->tfstate - (self->owner->tfstate & PR_TFSTATE_CANT_MOVE);

	// Reset speeds of scout
#ifdef PR_QUAKE_WORLD
    	TeamFortress_SetSpeed(self->owner);
#else
    	self->owner->pausetime = time;
#endif
	//}

	dremove(self->enemy->oldenemy);	// remove count down
	dremove(self->enemy);        	// remove detpack
	dremove(self);					// remove this timer
}

//=========================================================================
// The Detpack CountDown function. Displays the seconds left before the
// detpack detonates to the owner of the detpack, if <10
void TeamFortress_DetpackCountDown()
{
	string cd;

	cd = ftos(self->health);

	sprint(self->owner, PR_PRINT_HIGH, cd);
	sprint(self->owner, PR_PRINT_HIGH, "...\n");

	self->nextthink = time + 1;
	self->health = self->health - 1;

	// Flash the red light
	if (self->health <= 4)
		self->enemy->skin = 1;

	if (self->health <= 3 && self->health > 2) { //WK Detpack alerts
		sound (self->owner, PR_CHAN_VOICE, "weapons/fith.wav", 1, PR_ATTN_NORM);
		sound (self->enemy, PR_CHAN_WEAPON, "doors/baseuse.wav", 1, PR_ATTN_NORM);  //CH play set detpack sound
	}

	if (self->health == 0)
		dremove(self);
}

} // END namespace Progs
