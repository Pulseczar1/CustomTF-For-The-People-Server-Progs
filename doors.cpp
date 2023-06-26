#include "progs.h"
#include "tfortmap.h"
#include "debug.h"
#include "subs.h"
#include "neo.h"

namespace Progs {

#define PR_DOOR_START_OPEN	1
#define PR_DOOR_DONT_LINK	4
#define PR_DOOR_GOLD_KEY	8
#define PR_DOOR_SILVER_KEY	16
#define PR_DOOR_TOGGLE	32

/*

Doors are similar to buttons, but can spawn a fat trigger field around them
to open without a touch, and they link together to form simultanious
double/quad doors.

Door.owner is the master door.  If there is only one door, it points to itself.
If multiple doors, all will point to a single one.

Door.enemy chains from the master door through all doors linked in the chain.

*/

/*
=============================================================================

THINK FUNCTIONS

=============================================================================
*/

void door_go_down();
void door_go_up();

void door_blocked()
{
	T_Damage (other, self, self, self->dmg);

// if a door has a negative wait, it would never come back if blocked,
// so let it just squash the object to death real fast
	if (self->wait >= 0)
	{
		if (self->state == PR_STATE_DOWN)
			door_go_up ();
		else
			door_go_down ();
	}
}


void door_hit_top()
{
	sound (self, PR_CHAN_NO_PHS_ADD + PR_CHAN_VOICE, self->noise1, 1, PR_ATTN_NORM);
	self->state = PR_STATE_TOP;
	if (self->spawnflags & PR_DOOR_TOGGLE)
		return;		// don't come down automatically
	self->think = door_go_down;
	self->nextthink = self->ltime + self->wait;
}

void door_hit_bottom()
{
	self->goal_state = PR_TFGS_INACTIVE;

	sound (self, PR_CHAN_NO_PHS_ADD + PR_CHAN_VOICE, self->noise1, 1, PR_ATTN_NORM);
	self->state = PR_STATE_BOTTOM;
}

void door_go_down()
{
	sound (self, PR_CHAN_VOICE, self->noise2, 1, PR_ATTN_NORM);
	if (self->max_health)
	{
		self->takedamage = PR_DAMAGE_YES;
		self->health = self->max_health;
	}

	self->state = PR_STATE_DOWN;
	SUB_CalcMove (self->pos1, self->speed, door_hit_bottom);
}

void door_go_up()
{
	if (self->state == PR_STATE_UP)
		return;		// allready going up

	if (self->state == PR_STATE_TOP)
	{	// reset top wait time
		self->nextthink = self->ltime + self->wait;
		return;
	}

	sound (self, PR_CHAN_VOICE, self->noise2, 1, PR_ATTN_NORM);
	self->state = PR_STATE_UP;
	SUB_CalcMove (self->pos2, self->speed, door_hit_top);

	SUB_UseTargets();
}


/*
=============================================================================

ACTIVATION FUNCTIONS

=============================================================================
*/

void door_fire()
{
	entity 	oself;
	entity	starte;

	if (self->owner != self)
		objerror ("door_fire: self.owner != self");

// play use key sound

	if (self->items)
		sound (self, PR_CHAN_VOICE, self->noise4, 1, PR_ATTN_NORM);

	self->message = string_null;		// no more message
	oself = self;

	if (self->spawnflags & PR_DOOR_TOGGLE)
	{
		if (self->state == PR_STATE_UP || self->state == PR_STATE_TOP)
		{
			starte = self;
			do
			{
				door_go_down ();
				self = self->enemy;
			} while ( (self != starte) && (self != world) );
			self = oself;
			return;
		}
	}

// trigger all paired doors
	starte = self;
	do
	{
		door_go_up ();
		self = self->enemy;
	} while ( (self != starte) && (self != world) );
	self = oself;
}


void door_use()
{
	entity oself;

	self->message = "";			// door message are for touch only
	self->owner->message = "";
	self->enemy->message = "";
	oself = self;
	self = self->owner;
	door_fire ();
	self = oself;
}


void door_trigger_touch()
{
	entity te;

	if (other->health <= 0)
		return;

    // OfN - Dont let doors go for invalid players
    if (other->classname == "player")
    {
        if (other->done_custom & PR_CUSTOM_BUILDING) // skip ppl customizing
            return;
        if (other->playerclass == PR_PC_UNDEFINED) // skip observers
            return;
    }

	if (!Activated(self,other))
	{
		if (self->else_goal != 0)
		{
			te = Findgoal(self->else_goal);
			if (te != world)
				DoResults(te, other, (self->goal_result & PR_TFGR_ADD_BONUSES));
		}

		return;
	}

	if (time < self->attack_finished)
		return;
	self->attack_finished = time + 1;

	activator = other;

	self = self->owner;
	door_use ();
}


void door_killed()
{
	entity oself;

	oself = self;
	self = self->owner;
	self->health = self->max_health;
	self->takedamage = PR_DAMAGE_NO;	// wil be reset upon return
	door_use ();
	self = oself;
}


/*
================
door_touch

Prints messages and opens key doors
================
*/
float DoorShouldOpen();

void door_touch()
{
	entity te;

	if (other->classname != "player")
		return;

	if (self->owner->attack_finished > time)
		return;

    // OfN - Dont let doors go for invalid players
    if (other->done_custom & PR_CUSTOM_BUILDING) // skip ppl customizing
        return;
    if (other->playerclass == PR_PC_UNDEFINED) // skip observers
        return;

	if (!Activated(self,other))
	{
		// If an else goal should be activated, activate it
		if (self->else_goal != 0)
		{
			te = Findgoal(self->else_goal);
			if (te != world)
				DoResults(te, other, (self->goal_result & PR_TFGR_ADD_BONUSES));
		}

		return;
	}

	self->owner->attack_finished = time + 2;

	if (self->owner->message != "")
	{
		CenterPrint (other, self->owner->message);
		sound (other, PR_CHAN_VOICE, "misc/talk.wav", 1, PR_ATTN_NORM);
	}

// key door stuff
	if (!self->items)
		return;

// FIXME: blink key on player's status bar
	if ( (self->items & other->items) != self->items )
	{
		if (self->owner->items == PR_IT_KEY1)
		{
			if (world->worldtype == 2)
			{
				CenterPrint (other, "You need the silver keycard");
				sound (self, PR_CHAN_VOICE, self->noise3, 1, PR_ATTN_NORM);
			}
			else if (world->worldtype == 1)
			{
				CenterPrint (other, "You need the silver runekey");
				sound (self, PR_CHAN_VOICE, self->noise3, 1, PR_ATTN_NORM);
			}
			else if (world->worldtype == 0)
			{
				CenterPrint (other, "You need the silver key");
				sound (self, PR_CHAN_VOICE, self->noise3, 1, PR_ATTN_NORM);
			}
		}
		else
		{
			if (world->worldtype == 2)
			{
				CenterPrint (other, "You need the gold keycard\n");
				sound (self, PR_CHAN_VOICE, self->noise3, 1, PR_ATTN_NORM);
			}
			else if (world->worldtype == 1)
			{
				CenterPrint (other, "You need the gold runekey");
				sound (self, PR_CHAN_VOICE, self->noise3, 1, PR_ATTN_NORM);
			}
			else if (world->worldtype == 0)
			{
				CenterPrint (other, "You need the gold key");
				sound (self, PR_CHAN_VOICE, self->noise3, 1, PR_ATTN_NORM);
			}
		}
		return;
	}

#ifdef PR_COOP_MODE_ENHANCED
	// Gizmo - only print this if in coop mode, don't bother in singleplayer
	else if ( coop ) {
		// FIXME: should this be a team message?
		bprint (PR_PRINT_MEDIUM, other->netname);
		bprint (PR_PRINT_MEDIUM, " unlocked the ");
		if (self->owner->items == PR_IT_KEY1)
			bprint (PR_PRINT_MEDIUM, "silver");
		else
			bprint (PR_PRINT_MEDIUM, "gold");
		bprint (PR_PRINT_MEDIUM, " door\n");
	}
#endif

	other->items = other->items - self->items;
	other->tf_items = other->tf_items | self->items;

	if (neo.isModeActive() || DoorShouldOpen()) // in Neo mode, doors always open for anyone
	{
		self->touch = SUB_Null;
		if (self->enemy != world)
			self->enemy->touch = SUB_Null;	// get paired door
		door_use ();
	}
}

/*
=============================================================================

SPAWNING FUNCTIONS

=============================================================================
*/


entity spawn_field(const vector& fmins, const vector& fmaxs)
{
	entity	trigger;
	vector	t1, t2;

	trigger = spawn();  // PZ: This might should be a server-only entity, but can't tell, on quick inspection.
	trigger->movetype = PR_MOVETYPE_NONE;
	trigger->solid = PR_SOLID_TRIGGER;
	trigger->owner = self;
	trigger->touch = door_trigger_touch;

	// TeamFortress goal code
	trigger->team_no = self->team_no;
	trigger->playerclass = self->playerclass;
	trigger->items_allowed = self->items_allowed;
	trigger->activate_goal_no = self->activate_goal_no;
	trigger->inactivate_goal_no = self->inactivate_goal_no;
	trigger->remove_goal_no = self->remove_goal_no;
	trigger->restore_goal_no = self->restore_goal_no;
	trigger->activate_group_no =	self->activate_group_no;
	trigger->inactivate_group_no = self->inactivate_group_no;
	trigger->remove_group_no	= self->remove_group_no;
	trigger->restore_group_no = self->restore_group_no;
	trigger->goal_activation = self->goal_activation;
	trigger->goal_effects = self->goal_effects;
	trigger->goal_result = self->goal_result;
	trigger->goal_group = self->goal_group;

	t1 = fmins;
	t2 = fmaxs;
	setsize (trigger, t1 - V({60, 60, 8}), t2 + V({60, 60, 8}));
	return (trigger);
}


float EntitiesTouching(entity e1, entity e2)
{
	if (e1->mins[X] > e2->maxs[X])
		return PR_FALSE;
	if (e1->mins[Y] > e2->maxs[Y])
		return PR_FALSE;
	if (e1->mins[Z] > e2->maxs[Z])
		return PR_FALSE;
	if (e1->maxs[X] < e2->mins[X])
		return PR_FALSE;
	if (e1->maxs[Y] < e2->mins[Y])
		return PR_FALSE;
	if (e1->maxs[Z] < e2->mins[Z])
		return PR_FALSE;
	return PR_TRUE;
}


/*
=============
LinkDoors


=============
*/
void LinkDoors()
{
	entity	t, starte;
	vector	cmins, cmaxs;

	if (self->enemy != world)
		return;		// already linked by another door
	if (self->spawnflags & 4)
	{
		self->owner = self->enemy = self;
		return;		// don't want to link this door
	}

	cmins = self->mins;
	cmaxs = self->maxs;

	starte = self;
	t = self;

	do
	{
		self->owner = starte;			// master door

		if (self->health)
			starte->health = self->health;
		if (self->targetname != "")
			starte->targetname = self->targetname;
		if (self->message != "")
			starte->message = self->message;

		t = find (t, "classname", self->classname);
		if (t == world)
		{
			self->enemy = starte;		// make the chain a loop

		// shootable, fired, or key doors just needed the owner/enemy links,
		// they don't spawn a field

			self = self->owner;

			if (self->health)
				return;
			if (self->targetname != "")
				return;
			if (self->items)
				return;

			self->owner->trigger_field = spawn_field(cmins, cmaxs);

			return;
		}

		if (EntitiesTouching(self,t))
		{
			if (t->enemy != world)
				objerror ("cross connected doors");

			self->enemy = t;
			self = t;

			if (t->mins[X] < cmins[X])
				cmins[X] = t->mins[X];
			if (t->mins[Y] < cmins[Y])
				cmins[Y] = t->mins[Y];
			if (t->mins[Z] < cmins[Z])
				cmins[Z] = t->mins[Z];
			if (t->maxs[X] > cmaxs[X])
				cmaxs[X] = t->maxs[X];
			if (t->maxs[Y] > cmaxs[Y])
				cmaxs[Y] = t->maxs[Y];
			if (t->maxs[Z] > cmaxs[Z])
				cmaxs[Z] = t->maxs[Z];
		}
	} while (1 );

}


/*QUAKED func_door (0 .5 .8) ? START_OPEN x #DOOR_DONT_LINK GOLD_KEY SILVER_KEY TOGGLE
if two doors touch, they are assumed to be connected and operate as a unit.

TOGGLE causes the door to wait in both the start and end states for a trigger event.

START_OPEN causes the door to move to its destination when spawned, and operate in reverse.  It is used to temporarily or permanently close off an area when triggered (not usefull for touch or takedamage doors).

Key doors are allways wait -1.

"message"	is printed when the door is touched if it is a trigger door and it hasn't been fired yet
"angle"		determines the opening direction
"targetname" if set, no touch field will be spawned and a remote button or trigger field activates the door.
"health"	if set, door must be shot open
"speed"		movement speed (100 default)
"wait"		wait before returning (3 default, -1 = never return)
"lip"		lip remaining at end of move (8 default)
"dmg"		damage to inflict when blocked (2 default)
"sounds"
0)	no sound
1)	stone
2)	base
3)	stone chain
4)	screechy metal
*/

void func_door()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	if (world->worldtype == 0)
	{
		precache_sound ("doors/medtry.wav");
		precache_sound ("doors/meduse.wav");
		self->noise3 = "doors/medtry.wav";
		self->noise4 = "doors/meduse.wav";
	}
	else if (world->worldtype == 1)
	{
		precache_sound ("doors/runetry.wav");
		precache_sound ("doors/runeuse.wav");
		self->noise3 = "doors/runetry.wav";
		self->noise4 = "doors/runeuse.wav";
	}
	else if (world->worldtype == 2)
	{
		precache_sound ("doors/basetry.wav");
		precache_sound ("doors/baseuse.wav");
		self->noise3 = "doors/basetry.wav";
		self->noise4 = "doors/baseuse.wav";
	}
	/*else // - OfN - USEFULL?
	{
		RPrint ("no worldtype set!\n");
	}*/

    //- OfN - Support for mega-tf door sounds //
    if (self->armorclass == 1)
    {
		precache_sound ("doors/creekdr2.wav");
		precache_sound ("doors/creekdr1.wav");
		self->noise1 = "doors/creekdr2.wav";
		self->noise2 = "doors/creekdr1.wav";
    }
    else if (self->armorclass == 3)
    {
		precache_sound ("doors/electdr2.wav");
		precache_sound ("doors/electdr1.wav");
		self->noise1 = "doors/electdr2.wav";
		self->noise2 = "doors/electdr1.wav";
    }
    else if (self->armorclass == 2)
    {
		precache_sound ("doors/metaldr2.wav");
		precache_sound ("doors/metaldr1.wav");
		self->noise1 = "doors/metaldr2.wav";
		self->noise2 = "doors/metaldr1.wav";
    }
    else if (self->armorclass == 100) // OfN - Custom door sounds
    {
        if (self->noise1 == "" || self->noise2 == "")
            RPrint("Map Bug: Missing filenames of custom sounds for door!\n");

        precache_sound (self->noise1);
		precache_sound (self->noise2);
    }
    else if (self->sounds == 0)
	{
		precache_sound ("misc/null.wav");
		precache_sound ("misc/null.wav");
		self->noise1 = "misc/null.wav";
		self->noise2 = "misc/null.wav";
	}
    else if (self->sounds == 1)
	{
		precache_sound ("doors/drclos4.wav");
		precache_sound ("doors/doormv1.wav");
		self->noise1 = "doors/drclos4.wav";
		self->noise2 = "doors/doormv1.wav";
	}
	else if (self->sounds == 2)
	{
		precache_sound ("doors/hydro1.wav");
		precache_sound ("doors/hydro2.wav");
		self->noise2 = "doors/hydro1.wav";
		self->noise1 = "doors/hydro2.wav";
	}
	else if (self->sounds == 3)
	{
		precache_sound ("doors/stndr1.wav");
		precache_sound ("doors/stndr2.wav");
		self->noise2 = "doors/stndr1.wav";
		self->noise1 = "doors/stndr2.wav";
	}
	else if (self->sounds == 4)
	{
		precache_sound ("doors/ddoor1.wav");
		precache_sound ("doors/ddoor2.wav");
		self->noise1 = "doors/ddoor2.wav";
		self->noise2 = "doors/ddoor1.wav";
	}

    //- OfN - avoid doors having armor.. mega-tf bug?? =)
    self->armorclass=0;
    //--------------------------------------------------------//

	SetMovedir ();

	self->max_health = self->health;
	self->solid = PR_SOLID_BSP;
	self->movetype = PR_MOVETYPE_PUSH;
	setorigin (self, self->origin);
	setmodel (self, self->model);
	self->classname = "door";

	self->blocked = door_blocked;
	self->use = door_use;

	if (self->spawnflags & PR_DOOR_SILVER_KEY)
		self->items = PR_IT_KEY1;
	if (self->spawnflags & PR_DOOR_GOLD_KEY)
		self->items = PR_IT_KEY2;

	if (!self->speed)
		self->speed = 100;
	if (!self->wait)
		self->wait = 3;
	if (!self->lip)
		self->lip = 8;
	if (!self->dmg)
		self->dmg = 2;

	self->pos1 = self->origin;
	self->pos2 = self->pos1 + self->movedir*(fabs(self->movedir*self->size) - self->lip);

// #DOOR_START_OPEN is to allow an entity to be lighted in the closed position
// but spawn in the open position
	if (self->spawnflags & PR_DOOR_START_OPEN)
	{
		setorigin (self, self->pos2);
		self->pos2 = self->pos1;
		self->pos1 = self->origin;
	}

	self->state = PR_STATE_BOTTOM;

	if (self->health)
	{
		self->takedamage = PR_DAMAGE_YES;
		self->th_die = door_killed;
	}

	if (self->items)
		self->wait = -1;

	self->touch = door_touch;

// LinkDoors can't be done until all of the doors have been spawned, so
// the sizes can be detected properly.
	self->think = LinkDoors;
	self->nextthink = self->ltime + 0.1;
}

/*
=============================================================================

SECRET DOORS

=============================================================================
*/

void fd_secret_move1();
void fd_secret_move2();
void fd_secret_move3();
void fd_secret_move4();
void fd_secret_move5();
void fd_secret_move6();
void fd_secret_done();

#define PR_SECRET_OPEN_ONCE	1 		// stays open
#define PR_SECRET_1ST_LEFT	2 		// 1st move is left of arrow
#define PR_SECRET_1ST_DOWN	4 		// 1st move is down from arrow
#define PR_SECRET_NO_SHOOT	8 		// only opened by trigger
#define PR_SECRET_YES_SHOOT	16 	// shootable even if targeted


void fd_secret_use()
{
	float temp;

	self->health = 10000;

	// exit if still moving around...
	if (self->origin != self->oldorigin)
		return;
	// TF Disable doors with spawnflags of DOOR_TOGGLE
	if (self->spawnflags & PR_DOOR_TOGGLE)
		return;

	self->message = string_null;		// no more message

	SUB_UseTargets();				// fire all targets / killtargets

	if (!(self->spawnflags & PR_SECRET_NO_SHOOT))
	{
		self->th_pain = SUB_PainNull;
		self->takedamage = PR_DAMAGE_NO;
	}
	self->velocity = V({0, 0, 0});

	// Make a sound, wait a little...

	sound(self, PR_CHAN_VOICE, self->noise1, 1, PR_ATTN_NORM);
	self->nextthink = self->ltime + 0.1;

	temp = 1 - (self->spawnflags & PR_SECRET_1ST_LEFT);	// 1 or -1
	makevectors(self->mangle);

	if (!self->t_width)
	{
		if (self->spawnflags & PR_SECRET_1ST_DOWN)
			self-> t_width = fabs(v_up * self->size);
		else
			self-> t_width = fabs(v_right * self->size);
	}

	if (!self->t_length)
		self-> t_length = fabs(v_forward * self->size);

	if (self->spawnflags & PR_SECRET_1ST_DOWN)
		self->dest1 = self->origin - v_up * self->t_width;
	else
		self->dest1 = self->origin + v_right * (self->t_width * temp);

	self->dest2 = self->dest1 + v_forward * self->t_length;
	SUB_CalcMove(self->dest1, self->speed, fd_secret_move1);
	sound(self, PR_CHAN_VOICE, self->noise2, 1, PR_ATTN_NORM);
}

void fd_secret_pain(entity attacker, float damage)
{
	fd_secret_use ();
}

// Wait after first movement...
void fd_secret_move1()
{
	self->nextthink = self->ltime + 1.0;
	self->think = fd_secret_move2;
	sound(self, PR_CHAN_VOICE, self->noise3, 1, PR_ATTN_NORM);
}

// Start moving sideways w/sound...
void fd_secret_move2()
{
	sound(self, PR_CHAN_VOICE, self->noise2, 1, PR_ATTN_NORM);
	SUB_CalcMove(self->dest2, self->speed, fd_secret_move3);
}

// Wait here until time to go back...
void fd_secret_move3()
{
	sound(self, PR_CHAN_VOICE, self->noise3, 1, PR_ATTN_NORM);
	if (!(self->spawnflags & PR_SECRET_OPEN_ONCE))
	{
		self->nextthink = self->ltime + self->wait;
		self->think = fd_secret_move4;
	}
}

// Move backward...
void fd_secret_move4()
{
	sound(self, PR_CHAN_VOICE, self->noise2, 1, PR_ATTN_NORM);
	SUB_CalcMove(self->dest1, self->speed, fd_secret_move5);
}

// Wait 1 second...
void fd_secret_move5()
{
	self->nextthink = self->ltime + 1.0;
	self->think = fd_secret_move6;
	sound(self, PR_CHAN_VOICE, self->noise3, 1, PR_ATTN_NORM);
}

void fd_secret_move6()
{
	sound(self, PR_CHAN_VOICE, self->noise2, 1, PR_ATTN_NORM);
	SUB_CalcMove(self->oldorigin, self->speed, fd_secret_done);
}

void fd_secret_done()
{
	if (self->targetname  == ""|| self->spawnflags&PR_SECRET_YES_SHOOT)
	{
		self->health = 10000;
		self->takedamage = PR_DAMAGE_YES;
		self->th_pain = fd_secret_pain;
		self->th_die = fd_secret_use;
	}
	sound(self, PR_CHAN_NO_PHS_ADD + PR_CHAN_VOICE, self->noise3, 1, PR_ATTN_NORM);
}

void secret_blocked()
{
	if (time < self->attack_finished)
		return;
	self->attack_finished = time + 0.5;
	T_Damage (other, self, self, self->dmg);
}

/*
================
secret_touch

Prints messages
================
*/
void secret_touch()
{
	if (other->classname != "player")
		return;
	if (self->attack_finished > time)
		return;
	// TF Disable doors with spawnflags of DOOR_TOGGLE
	if (self->spawnflags & PR_DOOR_TOGGLE)
		return;

	self->attack_finished = time + 2;

	if (self->message != "")
	{
		CenterPrint (other, self->message);
		sound (other, PR_CHAN_BODY, "misc/talk.wav", 1, PR_ATTN_NORM);
	}
}


/*QUAKED func_door_secret (0 .5 .8) ? open_once 1st_left 1st_down no_shoot always_shoot
Basic secret door. Slides back, then to the side. Angle determines direction.
wait  = # of seconds before coming back
1st_left = 1st move is left of arrow
1st_down = 1st move is down from arrow
always_shoot = even if targeted, keep shootable
t_width = override WIDTH to move back (or height if going down)
t_length = override LENGTH to move sideways
"dmg"		damage to inflict when blocked (2 default)

If a secret door has a targetname, it will only be opened by it's botton or trigger, not by damage.
"sounds"
1) medieval
2) metal
3) base
*/

void func_door_secret()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	if (self->sounds == 0)
		self->sounds = 3;
	if (self->sounds == 1)
	{
		precache_sound ("doors/latch2.wav");
		precache_sound ("doors/winch2.wav");
		precache_sound ("doors/drclos4.wav");
		self->noise1 = "doors/latch2.wav";
		self->noise2 = "doors/winch2.wav";
		self->noise3 = "doors/drclos4.wav";
	}
	if (self->sounds == 2)
	{
		precache_sound ("doors/airdoor1.wav");
		precache_sound ("doors/airdoor2.wav");
		self->noise2 = "doors/airdoor1.wav";
		self->noise1 = "doors/airdoor2.wav";
		self->noise3 = "doors/airdoor2.wav";
	}
	if (self->sounds == 3)
	{
		precache_sound ("doors/basesec1.wav");
		precache_sound ("doors/basesec2.wav");
		self->noise2 = "doors/basesec1.wav";
		self->noise1 = "doors/basesec2.wav";
		self->noise3 = "doors/basesec2.wav";
	}

	if (!self->dmg)
		self->dmg = 2;

	// Magic formula...
	self->mangle = self->angles;
	self->angles = V({0, 0, 0});
	self->solid = PR_SOLID_BSP;
	self->movetype = PR_MOVETYPE_PUSH;
	self->classname = "door";
	setmodel (self, self->model);
	setorigin (self, self->origin);

	self->touch = secret_touch;
	self->blocked = secret_blocked;
	self->speed = 50;
	self->use = fd_secret_use;
	if ( self->targetname  == ""|| self->spawnflags&PR_SECRET_YES_SHOOT)
	{
		self->health = 10000;
		self->takedamage = PR_DAMAGE_YES;
		self->th_pain = fd_secret_pain;
	}
	self->oldorigin = self->origin;
	if (!self->wait)
		self->wait = 5;		// 5 seconds before closing
}

} // END namespace Progs
