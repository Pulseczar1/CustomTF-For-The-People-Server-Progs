#include "progs.h"
#include "tfortmap.h"
#include "debug.h"
#include "subs.h"
#include "frikbot/bot_ai.h"
#include "combat.h"

namespace Progs {

//WK
float IsBuilding(entity tester);
void makeImmune(entity foo,float bar);

entity stemp, otemp, s, old;

void trigger_reactivate()
{
	self->solid = PR_SOLID_TRIGGER;
}

//=============================================================================

#define PR_SPAWNFLAG_NOMESSAGE	1
#define PR_SPAWNFLAG_NOTOUCH	1

// the wait time has passed, so set back up for another activation
void multi_wait()
{
	if (self->max_health)
	{
		self->health = self->max_health;
		self->takedamage = PR_DAMAGE_YES;
		self->solid = PR_SOLID_BBOX;
	}
}

// the trigger was just touched/killed/used
// self.enemy should be set to the activator so it can be held through a delay
// so wait for the delay time before firing
void multi_trigger()
{
	if (self->nextthink > time)
	{
		return;		// allready been triggered
	}

	if (self->classname == "trigger_secret")
	{
		if (self->enemy->classname != "player")
			return;
		found_secrets = found_secrets + 1;
		WriteByte (PR_MSG_ALL, PR_SVC_FOUNDSECRET);
	}

	if (self->noise != "")
		sound (self, PR_CHAN_VOICE, self->noise, 1, PR_ATTN_NORM);

// don't trigger again until reset
	self->takedamage = PR_DAMAGE_NO;

	activator = self->enemy;

	SUB_UseTargets();

	if (self->wait > 0)
	{
		self->think = multi_wait;
		self->nextthink = time + self->wait;
	}
	else
	{	// we can't just remove (self) here, because this is a touch function
		// called while C code is looping through area links...
		self->touch = SUB_Null;
		self->nextthink = time + 0.1;
		self->think = SUB_Remove;
	}
}

void multi_killed()
{
	self->enemy = damage_attacker;
	multi_trigger();
}

void multi_use()
{
	self->enemy = activator;
	multi_trigger();
}

void multi_touch()
{
	entity te;

    if (other->classname != "player")
		return;

    // OfN - Dont let triggers go for invalid players
    if (other->done_custom & PR_CUSTOM_BUILDING) // skip ppl customizing
        return;
    if (other->playerclass == PR_PC_UNDEFINED) // skip observers
        return;
    if (other->health <= 0) // dead players out
        return;
    // Teamkillers
	if (other->penance_time >= time)
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

// if the trigger has an angles field, check player's facing direction
	if (self->movedir != V({0, 0, 0}))
	{
		makevectors (other->angles);
		if (v_forward * self->movedir < 0)
			return;		// not facing the right way
	}

	self->enemy = other;
	multi_trigger ();
}

/*QUAKED trigger_multiple (.5 .5 .5) ? notouch
Variable sized repeatable trigger.  Must be targeted at one or more entities.  If "health" is set, the trigger must be killed to activate each time.
If "delay" is set, the trigger waits some time after activating before firing.
"wait" : Seconds between triggerings. (.2 default)
If notouch is set, the trigger is only fired by other entities, not by touching.
NOTOUCH has been obsoleted by trigger_relay!
sounds
1)	secret
2)	beep beep
3)	large switch
4)
set "message" to text string
*/
void trigger_multiple()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	if (self->sounds == 1)
	{
		precache_sound ("misc/secret.wav");
		self->noise = "misc/secret.wav";
	}
	else if (self->sounds == 2)
	{
		precache_sound ("misc/talk.wav");
		self->noise = "misc/talk.wav";
	}
	else if (self->sounds == 3)
	{
		precache_sound ("misc/trigger1.wav");
		self->noise = "misc/trigger1.wav";
	}

	if (!self->wait)
		self->wait = 0.2;
	self->use = multi_use;

	InitTrigger ();

	if (self->health)
	{
		if (self->spawnflags & PR_SPAWNFLAG_NOTOUCH)
			objerror ("health and notouch don't make sense\n");
		self->max_health = self->health;
		self->th_die = multi_killed;
		self->takedamage = PR_DAMAGE_YES;
		self->solid = PR_SOLID_BBOX;
		setorigin (self, self->origin);	// make sure it links into the world
	}
	else
	{
		if ( !(self->spawnflags & PR_SPAWNFLAG_NOTOUCH) )
		{
			self->touch = multi_touch;
		}
	}
}


/*QUAKED trigger_once (.5 .5 .5) ? notouch
Variable sized trigger. Triggers once, then removes itself.  You must set the key "target" to the name of another object in the level that has a matching
"targetname".  If "health" is set, the trigger must be killed to activate.
If notouch is set, the trigger is only fired by other entities, not by touching.
if "killtarget" is set, any objects that have a matching "target" will be removed when the trigger is fired.
if "angle" is set, the trigger will only fire when someone is facing the direction of the angle.  Use "360" for an angle of 0.
sounds
1)	secret
2)	beep beep
3)	large switch
4)
set "message" to text string
*/
void trigger_once()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	self->wait = -1;
	trigger_multiple();
}

//=============================================================================

/*QUAKED trigger_relay (.5 .5 .5) (-8 -8 -8) (8 8 8)
This fixed size trigger cannot be touched, it can only be fired by other events.  It can contain killtargets, targets, delays, and messages.
*/
void trigger_relay()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	self->use = SUB_UseTargets;
}


//=============================================================================

/*QUAKED trigger_secret (.5 .5 .5) ?
secret counter trigger
sounds
1)	secret
2)	beep beep
3)
4)
set "message" to text string
*/
void trigger_secret()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	total_secrets = total_secrets + 1;
	self->wait = -1;
	if (self->message == "")
		self->message = "You found a secret area!";
	if (!self->sounds)
		self->sounds = 1;

	if (self->sounds == 1)
	{
		precache_sound ("misc/secret.wav");
		self->noise = "misc/secret.wav";
	}
	else if (self->sounds == 2)
	{
		precache_sound ("misc/talk.wav");
		self->noise = "misc/talk.wav";
	}

	trigger_multiple ();
}

//=============================================================================


void counter_use()
{
	//local string junk;

	self->count = self->count - 1;
	if (self->count < 0)
		return;

	if (self->count != 0)
	{
		if (activator->classname == "player"
		&& (self->spawnflags & PR_SPAWNFLAG_NOMESSAGE) == 0)
		{
			if (self->count >= 4)
				CenterPrint (activator, "There are more to go...");
			else if (self->count == 3)
				CenterPrint (activator, "Only 3 more to go...");
			else if (self->count == 2)
				CenterPrint (activator, "Only 2 more to go...");
			else
				CenterPrint (activator, "Only 1 more to go...");
		}
		return;
	}

	if (activator->classname == "player"
	&& (self->spawnflags & PR_SPAWNFLAG_NOMESSAGE) == 0)
		CenterPrint(activator, "Sequence completed!");
	self->enemy = activator;
	multi_trigger ();
}

/*QUAKED trigger_counter (.5 .5 .5) ? nomessage
Acts as an intermediary for an action that takes multiple inputs.

If nomessage is not set, t will print "1 more.. " etc when triggered and "sequence complete" when finished.

After the counter has been triggered "count" times (default 2), it will fire all of it's targets and remove itself.
*/
void trigger_counter()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	self->wait = -1;
	if (!self->count)
		self->count = 2;

	self->use = counter_use;
}


/*
==============================================================================

TELEPORT TRIGGERS

==============================================================================
*/

#define PR_PLAYER_ONLY	1
#define PR_SILENT	2

void play_teleport()
{
	float v;
	string tmpstr;

	v = random() * 5;
	if (v < 1)
		tmpstr = "misc/r_tele1.wav";
	else if (v < 2)
		tmpstr = "misc/r_tele2.wav";
	else if (v < 3)
		tmpstr = "misc/r_tele3.wav";
	else if (v < 4)
		tmpstr = "misc/r_tele4.wav";
	else
		tmpstr = "misc/r_tele5.wav";

	sound (self, PR_CHAN_VOICE, tmpstr, 1, PR_ATTN_NORM);
	remove (self);
}

void spawn_tfog(const vector& org)
{
	s = spawn ();
	s->origin = org;
	s->nextthink = time + 0.2;
	s->think = play_teleport;

	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_TELEPORT);
	WriteCoord (PR_MSG_BROADCAST, org[X]);
	WriteCoord (PR_MSG_BROADCAST, org[Y]);
	WriteCoord (PR_MSG_BROADCAST, org[Z]);
#ifdef PR_QUAKE_WORLD
	multicast (org, PR_MULTICAST_PHS);
#endif
}


void tdeath_touch()
{
	//local entity spot;
	entity other2;

	if (other == self->owner)
		return;

    //- OfN Hack to avoid turretized teles to be telefragged when beeing used
    if (other->classname == "building_teleporter" && other->tf_items & PR_NIT_TURRET)
        return;

// frag anyone who teleports in on top of an invincible player
	if (other->classname == "player")
	{
		// Gizmo - enable double 666 telefrags
		if (other->invincible_finished > time &&
			self->owner->invincible_finished > time) {
			self->classname = "teledeath3";
			other->invincible_finished = 0;
			self->owner->invincible_finished = 0;
			T_Damage (other, self, self, 50000);
			other2 = self->owner;
			self->owner = other;
			T_Damage (other2, self, self, 50000);
		}

		if (other->invincible_finished > time) {
			self->classname = "teledeath2";
			T_Damage (self->owner, self, self, 50000);
			return;
		}

		if (self->owner->classname != "player")
		{
			T_Damage (self->owner, self, self, 5000);
			return;
		}
	}

	if (other->health)
	{
		T_Damage(other, self, self, 5000);
	}
}


void spawn_tdeath(const vector& org, entity death_owner)
{
	entity	death;

	death = spawn(); // PZ: this could probably be a server-side entity, but I'm not entirely sure; so, leaving it client-available for right now
	death->classname = "teledeath";
	death->movetype = PR_MOVETYPE_NONE;
	death->solid = PR_SOLID_TRIGGER;
	death->angles = V({0, 0, 0});
	setsize (death, death_owner->mins - V({1, 1, 1}), death_owner->maxs + V({1, 1, 1}));
	setorigin (death, org);
	death->touch = tdeath_touch;
	death->nextthink = time + 0.2;
	death->think = SUB_Remove;
	death->owner = death_owner;

	force_retouch = 2;		// make sure even still objects get hit
}

void teleport_touch()
{
	entity	t, te;
	vector	org;

	if (self->targetname != "")
	{
		if (self->nextthink < time)
		{
			return;		// not fired yet
		}
	}

	if (self->spawnflags & PR_PLAYER_ONLY)
	{
		if (other->classname != "player")
			return;
	}

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

// only teleport living creatures
	if (other->health <= 0 || other->solid != PR_SOLID_SLIDEBOX)
		return;

	SUB_UseTargets ();

// put a tfog where the player was
	spawn_tfog (other->origin);

/*
	t = find (world, targetname, self.target);
	if (!t)
		objerror ("couldn't find target");
*/
	// Gizmo - teleports now cycle through their .targets
	t = self->oldenemy = find( self->oldenemy, "targetname", self->target );
	if ( t  == world) {
		t = self->oldenemy = find( world, "targetname", self->target );
		if (t == world)
			objerror ("couldn't find target");
	}

// spawn a tfog flash in front of the destination
	makevectors (t->mangle);
	org = t->origin + 32 * v_forward;

	spawn_tfog (org);
	spawn_tdeath(t->origin, other);

// move the player and lock him down for a little while
	if (!other->health)
	{
		other->origin = t->origin;
		other->velocity = (v_forward * other->velocity[X]) + (v_forward * other->velocity[Y]);
		return;
	}

	setorigin (other, t->origin);
	other->angles = t->mangle;

	// PZ: Bot should remove the teleporter from his target stack.
	if (other->classname == "player" && !other->ishuman)
	{
		entity oldSelf;
		oldSelf = self;
		self = other;
/*local string strtemp;
strtemp = etos(oldSelf);
bprint(2, "Removing from target stack, teleporter, with entity number: ", strtemp, "\n");*/
		frik_removeFromTargetStack(oldSelf);
		self = oldSelf;
	}

	if (other->classname == "player")
	{
		// Teleporting - detach hook (wedge)
		if (other->weapons_carried & PR_WEAP_HOOK && other->hook_out)
		{
			sound (other, PR_CHAN_WEAPON, "weapons/bounce2.wav", 1, PR_ATTN_NORM);
			other->on_hook = PR_FALSE;
			other->hook_out = PR_FALSE;
			other->fire_held_down = PR_FALSE;
			other->weaponframe = 0;
			other->attack_finished = time + 0.75;	// avoid instant rehook
		}

		//other.immune_to_check = time + 2; //WK Don't zap people for teleporting
		makeImmune(other,time+2);

		other->fixangle = 1;		// turn this way immediately
		other->teleport_time = time + 0.7;
		if (other->flags & PR_FL_ONGROUND)
			other->flags = other->flags - PR_FL_ONGROUND;
		other->velocity = v_forward * 300;
	}
	other->flags = other->flags - (other->flags & PR_FL_ONGROUND);
}

/*QUAKED info_teleport_destination (.5 .5 .5) (-8 -8 -8) (8 8 32)
This is the destination marker for a teleporter.  It should have a "targetname" field with the same value as a teleporter's "target" field.
*/
void info_teleport_destination()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

// this does nothing, just serves as a target spot
	self->mangle = self->angles;
	self->angles = V({0, 0, 0});
	self->model = "";
	self->origin = self->origin + V({0, 0, 27});
	if (self->targetname == "")
		objerror ("no targetname");
}

void teleport_use()
{
	self->nextthink = time + 0.2;
	force_retouch = 2;		// make sure even still objects get hit
	self->think = SUB_Null;
}

/*QUAKED trigger_teleport (.5 .5 .5) ? #PLAYER_ONLY #SILENT
Any object touching this will be transported to the corresponding info_teleport_destination entity. You must set the "target" field, and create an object with a "targetname" field that matches.

If the trigger_teleport has a targetname, it will only teleport entities when it has been fired.
*/
void trigger_teleport()
{
	vector o;

	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	InitTrigger ();
	self->touch = teleport_touch;
	// find the destination
	if (self->target == "")
		objerror ("no target");
	self->use = teleport_use;

	if (!(self->spawnflags & PR_SILENT))
	{
		precache_sound ("ambience/hum1.wav");
		o = (self->mins + self->maxs)*0.5;
		ambientsound (o, "ambience/hum1.wav",0.5 , PR_ATTN_STATIC);
	}
}

/*
==============================================================================

trigger_setskill

==============================================================================
*/

#ifdef PR_COOP_MODE_ENHANCED
void trigger_skill_touch()
{
	entity te;

	if (other->classname != "player")
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

	// Gizmo - if in singleplayer then save the new skill, coop is always in nightmare skill
	if ( !deathmatch && !coop ) {
		skill = stof( self->message );
		localcmd( "serverinfo skill " );
		localcmd( self->message );
		localcmd( "\n" );

		// Gizmo - quakeworld doesn't have this cvar, though some third party servers do
		cvar_set ("skill", self->message);
	}
}
#endif

/*QUAKED trigger_setskill (.5 .5 .5) ?
sets skill level to the value of "message".
Only used on start map.
*/
void trigger_setskill()
{
#ifdef PR_COOP_MODE_ENHANCED
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	if (deathmatch)
	{
		dremove(self);
		return;
	}
	else
	{
		InitTrigger ();
		self->touch = trigger_skill_touch;
	}
#else
	dremove( self );
	return;
#endif
}


/*
==============================================================================

ONLY REGISTERED TRIGGERS

==============================================================================
*/

void trigger_onlyregistered_touch()
{
	entity te;

	if (other->classname != "player")
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

	if (self->attack_finished > time)
		return;

	self->attack_finished = time + 2;
	if (cvar("registered"))
	{
		self->message = "";
		SUB_UseTargets ();

		// if == it's going to be removed somewhere else
		if (self->killtarget != self->targetname)
			remove (self);
	}
	else
	{
		if (self->message != "")
		{
			CenterPrint (other, self->message);
			sound (other, PR_CHAN_BODY, "misc/talk.wav", 1, PR_ATTN_NORM);
		}
	}
}

/*QUAKED trigger_onlyregistered (.5 .5 .5) ?
Only fires if playing the registered version, otherwise prints the message
*/
void trigger_onlyregistered()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	precache_sound ("misc/talk.wav");
	InitTrigger ();
	self->touch = trigger_onlyregistered_touch;
}

//============================================================================
void hurt_on()
{
	self->solid = PR_SOLID_TRIGGER;
	self->nextthink = -1;
}

void hurt_touch()
{
	entity te;

	if (other->takedamage)
	{
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

		self->solid = PR_SOLID_NOT;
		deathmsg = PR_DMSG_TRIGGER;
		TF_T_Damage (other, self, self, self->dmg, PR_TF_TD_IGNOREARMOUR | PR_TF_TD_IGNORERESIST, PR_TF_TD_OTHER);
		self->think = hurt_on;
		self->nextthink = time + 1;
	}
}

/*QUAKED trigger_hurt (.5 .5 .5) ?
Any object touching this will be hurt
set dmg to damage amount
defalt dmg = 5
*/
void trigger_hurt()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	InitTrigger ();
	self->touch = hurt_touch;
	if (!self->dmg)
		self->dmg = 5;
}

//============================================================================

#define PR_PUSH_ONCE	1

void trigger_push_touch()
{
	entity te;

	if (IsBuilding(other))
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

	if (other->classname == "grenade" || other->classname == "pipebomb") //CH can throw pipes now :)
		other->velocity = self->speed * self->movedir * 10;
	else if (other->health > 0)
	{
		other->velocity = self->speed * self->movedir * 10;
		if (other->classname == "player")
		{
			if (other->fly_sound < time)
			{
				other->fly_sound = time + 1.5;
				sound (other, PR_CHAN_AUTO, "ambience/windfly.wav", 1, PR_ATTN_NORM);
			}
		}
	}
	if (self->spawnflags & PR_PUSH_ONCE)
		dremove(self);
}


/*QUAKED trigger_push (.5 .5 .5) ? #PUSH_ONCE
Pushes the player
*/
void trigger_push()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	InitTrigger ();
	precache_sound ("ambience/windfly.wav");
	self->touch = trigger_push_touch;
	if (!self->speed)
		self->speed = 1000;
}

//============================================================================

void trigger_monsterjump_touch()
{
	if ( (other->flags & (PR_FL_MONSTER | PR_FL_FLY | PR_FL_SWIM)) != PR_FL_MONSTER )
		return;

// set XY even if not on ground, so the jump will clear lips
	other->velocity[X] = self->movedir[X] * self->speed;
	other->velocity[Y] = self->movedir[Y] * self->speed;

	if ( !(other->flags & PR_FL_ONGROUND) )
		return;

	other->flags = other->flags - PR_FL_ONGROUND;

	other->velocity[Z] = self->height;
}

/*QUAKED trigger_monsterjump (.5 .5 .5) ?
Walking monsters that touch this will jump in the direction of the trigger's angle
"speed" default to 200, the speed thrown forward
"height" default to 200, the speed thrown upwards
*/
void trigger_monsterjump()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	if (!self->speed)
		self->speed = 200;
	if (!self->height)
		self->height = 200;
	if (self->angles == V({0, 0, 0}))
		self->angles = V({0, 360, 0});
	InitTrigger ();
	self->touch = trigger_monsterjump_touch;
}

} // END namespace Progs
