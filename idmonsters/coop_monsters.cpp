/*
	coop_monsters.qc - standard functions for monsters
*/

#include "progs.h"

#ifndef PR_COOP_MODE_ENHANCED
//#pragma DONT_COMPILE_THIS_FILE
#else

#include "coop_defs.h"
#include "tfortmap.h"
#include "custom.h"
#include "gweapons.h"
#include "monsters.h"
#include "debug.h"

namespace Progs {

float COOP_AttackTarget(entity en, float check);

/* ALL MONSTERS SHOULD BE 1 0 0 IN COLOR */

// name =[framenum,	nexttime, nextthink] {code}
// expands to:
// name ()
// {
//		self.frame=framenum;
//		self.nextthink = time + nexttime;
//		self.think = nextthink
//		<code>
// };

/*
	coop_debugtargetprint
*/
void coop_debugtargetprint()
{
	dprint ("Monster ");
	dprint (self->classname);
	dprint (" can't find target ");
	dprint (self->target);
	dprint (" at ");
	dprint (vtos(self->origin));
	dprint ("\n");
}

/*
================
monster_use

Using a monster makes it angry at the current activator
================
*/
void coop_monster_use()
{
	if (self->enemy != world)
		return;
	if (self->health <= 0)
		return;
	if (activator->items & PR_IT_INVISIBILITY)
		return;
	if (activator->flags & PR_FL_NOTARGET)
		return;
	if (activator->classname != "player")
		return;

// delay reaction so if the monster is teleported, its sound is still
// heard
	COOP_AttackTarget( activator, PR_TRUE );
}

/*
================
monster_death_use

When a monster dies, it fires all of its targets with the current
enemy as activator.
================
*/
void coop_monster_death_use()
{
// fall to ground
	if (self->flags & PR_FL_FLY)
		self->flags = self->flags - PR_FL_FLY;
	if (self->flags & PR_FL_SWIM)
		self->flags = self->flags - PR_FL_SWIM;

	if (self->target == "")
		return;

	activator = self->enemy;
	SUB_UseTargets ();
}


//============================================================================

/*
================
COOP_FindMonsterPath

This is a hack to allow monsters to have a path AND targets
it fires at death. Only call this if self.target is set.
================
*/
void COOP_FindMonsterPath() {
	entity	te;

	self->goalentity = self->movetarget = te = find( world, "targetname", self->target );
	while ( te  != world) {
		if ( te->classname == "path_corner" ) {
			self->goalentity = self->movetarget = te;
			return;
		}

		te = find( te, "targetname", self->target );
	}

	// if none are any path_corners, then goalentity and movetarget contain the first
	// target found, if any, like the standard behavior
}

void coop_walkmonster_start_go()
{
	self->origin[Z] = self->origin[Z] + 1;	// raise off floor a bit
	droptofloor();

	// save original spawn angles
	self->PR_npc_originalAngles = self->angles;

	if (!walkmove(0,0))
	{
		dprint( "walkmonster " );
		dprint( self->classname );
		dprint( " in wall at: " );
		dprint( vtos( self->origin ) );
		dprint( "\n" );
	}

	self->takedamage = PR_DAMAGE_AIM;

	self->ideal_yaw = self->angles * V({0, 1, 0});
	if (!self->yaw_speed)
		self->yaw_speed = 20;
	self->view_ofs = V({0, 0, 25});
	self->use = coop_monster_use;

	self->flags = self->flags | PR_FL_MONSTER;

	if (self->target != "")
	{
		COOP_FindMonsterPath();
		self->ideal_yaw = vectoyaw(self->goalentity->origin - self->origin);
		if (self->movetarget == world)
		{
			coop_debugtargetprint ();
		}
// this used to be an objerror
		if (self->movetarget->classname == "path_corner")
			self->th_walk ();
		else {
			self->pausetime = 99999999;
			self->th_stand ();
		}
	}
	else
	{
		self->pausetime = 99999999;
		self->th_stand ();
	}

// spread think times so they don't all happen at same time
	self->nextthink = self->nextthink + random()*0.5;
}


void coop_walkmonster_start()
{
	if (CheckExistence() == PR_FALSE) {
		remove(self);
		return;
	}

	if ( !self->PR_npc_isCoopMonster )
		self->PR_npc_isCoopMonster = PR_TRUE;
	self->PR_npc_originalloc = self->origin;

// delay drop to floor to make sure all doors have been spawned
// spread think times so they don't all happen at same time
	self->nextthink = self->nextthink + random()*0.5;
	self->think = coop_walkmonster_start_go;

	// make up for monster respawns on total monsters
	if (!server_spawned) {
		self->PR_npc_useTargetsOnRespawn = PR_TRUE;

		if ( !( self->spawnflags & PR_SPAWN_DONTCOUNT ) )
		if ( number_of_teams > 1 || !Teammate( self->team_no, 1 ) )		// don't count friendly monsters
			total_monsters = total_monsters + (self->lives + 1);
	}
}



void coop_flymonster_start_go()
{
	// save original spawn angles
	self->PR_npc_originalAngles = self->angles;

	self->takedamage = PR_DAMAGE_AIM;

	self->ideal_yaw = self->angles * V({0, 1, 0});
	if (!self->yaw_speed)
		self->yaw_speed = 10;
	self->view_ofs = V({0, 0, 25});
	self->use = coop_monster_use;

	self->flags = self->flags | PR_FL_FLY;
	self->flags = self->flags | PR_FL_MONSTER;

	if (!walkmove(0,0))
	{
		dprint( "flymonster " );
		dprint( self->classname );
		dprint( " in wall at: " );
		dprint( vtos( self->origin ) );
		dprint( "\n" );
	}

	if (self->target != "")
	{
		COOP_FindMonsterPath();
		if (self->movetarget == world)
		{
			coop_debugtargetprint ();
		}
// this used to be an objerror
		if (self->movetarget->classname == "path_corner")
			self->th_walk ();
		else {
			self->pausetime = 99999999;
			self->th_stand ();
		}
	}
	else
	{
		self->pausetime = 99999999;
		self->th_stand ();
	}
}

void coop_flymonster_start()
{
	if (CheckExistence() == PR_FALSE) {
		remove(self);
		return;
	}

	if ( !self->PR_npc_isCoopMonster )
		self->PR_npc_isCoopMonster = PR_TRUE;
	self->PR_npc_originalloc = self->origin;

// spread think times so they don't all happen at same time
	self->nextthink = self->nextthink + random()*0.5;
	self->think = coop_flymonster_start_go;

	// make up for monster respawns on total monsters
	if (!server_spawned) {
		self->PR_npc_useTargetsOnRespawn = PR_TRUE;

		if ( !( self->spawnflags & PR_SPAWN_DONTCOUNT ) )
		if ( number_of_teams > 1 || !Teammate( self->team_no, 1 ) )		// don't count friendly monsters
			total_monsters = total_monsters + (self->lives + 1);
	}
}


void coop_swimmonster_start_go()
{
	// save original spawn angles
	self->PR_npc_originalAngles = self->angles;

	self->takedamage = PR_DAMAGE_AIM;

	self->ideal_yaw = self->angles * V({0, 1, 0});
	if (!self->yaw_speed)
		self->yaw_speed = 10;
	self->view_ofs = V({0, 0, 10});
	self->use = coop_monster_use;

	self->flags = self->flags | PR_FL_SWIM;
	self->flags = self->flags | PR_FL_MONSTER;

	if (self->target != "")
	{
		COOP_FindMonsterPath();
		if (self->movetarget == world)
		{
			coop_debugtargetprint ();
		}
// this used to be an objerror
		self->ideal_yaw = vectoyaw(self->goalentity->origin - self->origin);
		if (self->movetarget->classname == "path_corner")
			self->th_walk ();
		else {
			self->pausetime = 99999999;
			self->th_stand ();
		}
	}
	else
	{
		self->pausetime = 99999999;
		self->th_stand ();
	}

// spread think times so they don't all happen at same time
	self->nextthink = self->nextthink + random()*0.5;
}

void coop_swimmonster_start()
{
	if (CheckExistence() == PR_FALSE) {
		remove(self);
		return;
	}

	if ( !self->PR_npc_isCoopMonster )
		self->PR_npc_isCoopMonster = PR_TRUE;
	self->PR_npc_originalloc = self->origin;

// spread think times so they don't all happen at same time
	self->nextthink = self->nextthink + random()*0.5;
	self->think = coop_swimmonster_start_go;

	// make up for monster respawns on total monsters
	if (!server_spawned) {
		self->PR_npc_useTargetsOnRespawn = PR_TRUE;

		if ( !( self->spawnflags & PR_SPAWN_DONTCOUNT ) )
		if ( number_of_teams > 1 || !Teammate( self->team_no, 1 ) )		// don't count friendly monsters
			total_monsters = total_monsters + (self->lives + 1);
	}
}


//============================================================================

#define PR_DEFAULT_SPAWNER_TARGETNAME	"monster_spawner"

float	spawner_id;	// next id of a spawner, so we can tell them apart for ones without targetnames and duplicate targetnames

void monster_spawner_spawn() {
	entity	e, oself;
	string	oldTarget;
	float		maxCount, ftmp, rchance;

	// if lives are up, then all monsters are or have been spawned
	// only time this can happen is if this gets called too much
	// because we check below after we spawn if lives went to 0 and when a monster dies we also check then
	if ( !self->lives || self->cnt == self->count ) {
		// if OFF_AT_COUNT then make sure it's off
		if ( self->spawnflags & 32 ) {
			self->state = 0;
			self->think = SUB_Null;
		}
		return;
	}

	if ( !self->currentammo ) {
		objerror( "no valid monsters specified" );
		return;
	}

	// increase monster count
	self->cnt = self->cnt + 1;
	// subtract a life when a monster spawns
	self->lives = self->lives - 1;

	// if the new monster reaches our limit then stop spawning
	if ( self->cnt == self->count || !self->lives ) {
		// if OFF_AT_COUNT then turn it off
		if ( self->spawnflags & 32 )
			self->state = 0;
		self->think = SUB_Null;
	}

	e = spawn();
	e->target = self->targetname;
	e->angles = self->angles;
	if ( self->netname  != "")
		e->targetname = self->netname;

	setorigin( e, self->origin );

	// find a monster to spawn
	oself = self;
	self = e;

	maxCount = 100;

	// .currentammo is checked for being zero before this is performed
	rchance = 1 / oself->currentammo;

	do {
		ftmp = 0;

		// call random multiple times to keep things easy and easy to add more monsters
		if ( oself->aflag & 1 && random() < rchance )
			monster_zombie();
		else if ( oself->aflag & 2 && random() < rchance )
			monster_shambler();
		else if ( oself->aflag & 4 && random() < rchance )
			monster_knight();
		else if ( oself->aflag & 8 && random() < rchance )
			monster_enforcer();
		else if ( oself->aflag & 16 && random() < rchance )
			monster_demon1();
		else if ( oself->aflag & 32 && random() < rchance )
			monster_army();
		else if ( oself->aflag & 64 && random() < rchance )
			monster_ogre();
		else if ( oself->aflag & 128 && random() < rchance )
			monster_shalrath();
		else if ( oself->aflag & 256 && random() < rchance )
			monster_wizard();
		else if ( oself->aflag & 512 && random() < rchance )
			monster_dog();
		else if ( oself->aflag & 1024 && random() < rchance )
			monster_fish();
		else if ( oself->aflag & 2048 && random() < rchance )
			monster_hell_knight();
		else if ( oself->aflag & 4096 && random() < rchance )
			monster_tarbaby();
		else {
			maxCount = maxCount - 1;

			// if we failed to spawn one then force one to spawn
			if ( !maxCount )
				rchance = 1;

			ftmp = 1;
		}
	} while ( ftmp );

	self = oself;

	// set monster specific things
	e->PR_npc_isCoopMonster = self->ammo_detpack;	// so we can tell spawned monsters from map monsters, and if two spawners have the same classname
	e->PR_npc_useTargetsOnRespawn = PR_TRUE;	// make sure we always fire our targets
	e->lives = 0;
	e->endtime = self->endtime;
	if ( self->team_no )
		e->team_no = self->team_no;

	// check for randomizing the spawn position
	if ( self->spawnflags & 1 || self->spawnflags & 2 || InSolid( e ) ) {
		maxCount = 100;

		while ( maxCount ) {
			if ( random() < 0.5 )
				e->origin[X] = e->origin[X] - random() * self->t_length;
			else
				e->origin[X] = e->origin[X] + random() * self->t_length;

			if ( random() < 0.5 )
				e->origin[Y] = e->origin[Y] + random() * self->t_length;
			else
				e->origin[Y] = e->origin[Y] - random() * self->t_length;

			setorigin( e, e->origin );

			// check for invalid positions
			traceline( e->origin, e->origin + V({0, 0, -8192}), PR_TL_BSP_ONLY, e );

			// if spawnflags 2 is set, then the position must be visible to the spawner
			if ( trace_fraction == 1 || InSolid( e ) || ( self->spawnflags & 2 && !visible( e ) ) ) {
				setorigin( e, self->origin );
				maxCount = maxCount - 1;
			} else
				maxCount = 0;
		}
	}

	if ( self->spawnflags & 64 )
		e->angles = vectoangles( self->origin - e->origin );
	else if ( self->spawnflags & 128 )
		e->angles = vectoangles( e->origin - self->origin );

	// play the teleport effect if we're suppose to
	if ( !( self->spawnflags & 1024 ) )
		spawn_tfog( e->origin );

	// fire event
	oldTarget = self->target;
	self->target = self->event;
	activator = e;
	SUB_UseTargets();
	self->target = oldTarget;

	// setup to spawn another monster if needed
	// don't worry about zero waits because those are done as they die
	if ( self->wait > 0 ) {
		if ( self->think  != SUB_Null)
			self->nextthink = time + self->wait;
	}
}

/*
================
monster_spawner_use

Gets called whenever this trigger gets fired or when
a monster that this trigger spawned has died.
================
*/
void monster_spawner_use() {
	if ( self->classname != "monster_spawner" )
		return;

	// check if we're being used because of a monster death
	// map monsters can have a monster_spawner as a target and things will work fine
	if ( COOP_IsCoopMonster( other ) && other->enemy == activator && other->PR_npc_isCoopMonster == self->ammo_detpack ) {
		self->cnt = self->cnt - 1;

		// since one died, the only way we can't spawn another one is
		// if our lives are up
		if ( !self->lives ) {
			// only remove it if all monsters are dead
			if ( !self->cnt ) {
				self->targetname = "";
				self->use = SUB_Null;
				self->nextthink = time + 0.1;
				self->think = SUB_Remove;
			} else
				self->think = SUB_Null;

			// fire our targets when a monster dies
			SUB_UseTargets();
			return;
		}

		// if spawning is enabled then spawn a new monster after the wait time
		if ( self->wait > 0 ) {
			if ( self->targetname == PR_DEFAULT_SPAWNER_TARGETNAME ) {
				self->nextthink = time + self->wait;
				self->think = monster_spawner_spawn;
			} else {
				if ( self->state ) {
					self->nextthink = time + self->wait;
					self->think = monster_spawner_spawn;
				} else
					self->think = SUB_Null;
			}
		} else if ( self->targetname == PR_DEFAULT_SPAWNER_TARGETNAME )
			monster_spawner_spawn();

		// fire our targets when a monster dies
		SUB_UseTargets();
		return;
	}

	// this should never happen, except if the map is trying to be funny
	// if targetname isn't present then the only time this should be fired is when one of our monsters die
	if ( self->targetname == PR_DEFAULT_SPAWNER_TARGETNAME )
		return;

	// check if we should be toggled on/off
	if ( self->wait > 0 ) {
		if ( !self->state ) {
			self->state = 1;
			self->nextthink = time + self->wait;
			self->think = monster_spawner_spawn;

			// the code below, if used, won't break as nextthink is reset when a spawned monster dies
			// and monster_spawner_spawn checks count and lives before spawning a monster
			// also state is managed in monster_spawner_spawn()

			// check if SPAWN_COUNT_TOGGLE or SPAWN_TOGGLE are set
			if ( self->spawnflags & 512 ) {
				// spawn up to count now that we're toggled on, this may not spawn any at all
				while ( self->cnt < self->count && self->lives )
					monster_spawner_spawn();
			} else if ( self->spawnflags & 256 ) {
				// spawn one right away (if we're suppose to)
				monster_spawner_spawn();
			}
		} else {
			self->state = 0;
			self->think = SUB_Null;
		}
	} else
		monster_spawner_spawn();
}

/*QUAKED monster_spawner (0 .5 .8) (-8 -8 -8) (8 8 8) RANDOM_POS VISIBLE_RANDOM_POS START_ON SPAWN_FIRST SPAWN_COUNT_FIRST OFF_AT_COUNT LOOK_AT LOOK_AWAY SPAWN_TOGGLE SPAWN_COUNT_TOGGLE NO_TELEPORT_EFFECT
Spawns random monsters at random positions with a maximum and current limit.
If "targetname" and "wait" are set, then the trigger is toggled on/off, and defaults to off unless START_ON spawnflag is set.
If just "targetname" is set, then the only way for a monster to spawn is when the monster_spawner is fired.
If just "wait" is set, then monsters are continuously spawned according to "count" and "lives".
If neither "targetname" nor "wait" are set, then monsters spawn continuously and instantly according to "count" and "lives".
"count" specifies how many monsters are allowed to be spawned at once.
"lives" specifies the overall total number of monsters this can spawn.
"count" and "lives" always apply no matter how a monster is spawned.
"t_length" specifies the radius of the random monster placement if one of the random placement spawnflags are present or a monster spawns inside another.
"team_no" specifies the team the monsters will be on, if set to 1 then the monster count is not increased.
If "netname" is set, it will be the spawned monsters's "targetname" field.
If "endtime" is set, it will be copied to a monster when it spawns.
If "event" is set, it will fire all entities that have a matching "targetname" field when a monster spawns from the monster_spawner.
"aflag" specifies the monster(s) to spawn, if not set or zero then every monster type can potentially be spawned.
The monsters will face the angle of the monster_spawner.
The monster_spawner will fire it's targets every time a monster dies.
The monster_spawner removes itself once "lives" are used up and all spawned monsters have died.
*/
void monster_spawner() {
	entity	temp, oself;
	float		ftmp;

	// this is stored in the #isCoopMonster field, so make sure it's greater than 1
	if ( !spawner_id )
		spawner_id = 2;

	if ( CheckExistence() == PR_FALSE ) {
		dremove( self );
		return;
	}

	if ( deathmatch ) {
		remove( self );
		return;
	}

	// this is needed because of collisions with the flags that specific if the item should exist
	// if heat is set then set spawnflags to that value after CheckExistence() so that function will work and we can still reuse those bitflags for our own purposes
	// if heat isn't set, then do nothing and this will act the same as 1.0.7.1 and older versions
	if ( self->heat )
		self->spawnflags = self->heat;

	self->cnt = 0;
	self->state = 0;

	// make sure the spawner can't interact with anything
	self->solid = PR_SOLID_NOT;
	self->movetype = PR_MOVETYPE_NONE;

	// check for invalids
	if ( self->spawnflags & 8 )
	if ( self->spawnflags & 16 )
		objerror( "spawn_first and spawn_count_first doesn't make sense" );

	if ( self->spawnflags & 4 ) {
		// only valid if targetname and wait are present
		if ( self->wait <= 0 || self->targetname == "" )
			objerror( "start_on is only valid if targetname and wait are set" );
	} else {
		if ( self->wait > 0 && self->targetname != "" )
		if ( self->spawnflags & 8 || self->spawnflags & 16 )
			objerror( "spawn_first and spawn_count_first are not valid without start_on when wait and targetname are set" );
	}

	if ( self->targetname  != "")
	if ( self->target  != "")
	if ( self->target == self->targetname )
		objerror( "target == targetname" );

	if ( self->wait <= 0 || self->targetname == "" ) {
		if ( self->spawnflags & 32 )
			objerror( "off_at_count is only valid if targetname and wait are set" );
		if ( self->spawnflags & 256 || self->spawnflags & 512 )
			objerror( "spawn_toggle and spawn_count_toggle are only valid if targetname and wait are set" );
	}

	// set the default random placement range
	if ( self->t_length <= 0 )
		self->t_length = 256;
	// set the default amount of monsters at a time
	if ( self->count <= 0 )
		self->count = 1;
	// set default lives
	if ( self->lives <= 0 )
		self->lives = 1;
	// endtime shouldn't be negative
	if ( self->endtime < 0 )
		self->endtime = 0;
	// default to all monsters
	if ( !self->aflag )
		self->aflag = 8191;
	// set the default targetname to notify us of monster deaths
	if ( self->targetname == PR_DEFAULT_SPAWNER_TARGETNAME )
		objerror( "targetname cannot be \"" PR_DEFAULT_SPAWNER_TARGETNAME "\"\n" );
	if ( self->targetname  == "")
		self->targetname = PR_DEFAULT_SPAWNER_TARGETNAME;

	// because there could be two spawners with the same targetname...
	// make sure each spawner has a unique id
	self->ammo_detpack = spawner_id;
	spawner_id = spawner_id + 1;

	// only increase monster count if we're not on a team or we're not on team one
	if ( !self->team_no || !Teammate( self->team_no, 1 ) )
		total_monsters = total_monsters + self->lives;

	// first precache sounds and models
	temp = spawn();

	// preserve total_monsters
	ftmp = total_monsters;

	oself = self;
	self = temp;

	oself->currentammo = 0;
	if ( oself->aflag & 1 ) {
		monster_zombie();
		oself->currentammo = oself->currentammo + 1;
	}
	if ( oself->aflag & 2 ) {
		monster_shambler();
		oself->currentammo = oself->currentammo + 1;
	}
	if ( oself->aflag & 4 ) {
		monster_knight();
		oself->currentammo = oself->currentammo + 1;
	}
	if ( oself->aflag & 8 ) {
		monster_enforcer();
		oself->currentammo = oself->currentammo + 1;
	}
	if ( oself->aflag & 16 ) {
		monster_demon1();
		oself->currentammo = oself->currentammo + 1;
	}
	if ( oself->aflag & 32 ) {
		monster_army();
		oself->currentammo = oself->currentammo + 1;
	}
	if ( oself->aflag & 64 ) {
		monster_ogre();
		oself->currentammo = oself->currentammo + 1;
	}
	if ( oself->aflag & 128 ) {
		monster_shalrath();
		oself->currentammo = oself->currentammo + 1;
	}
	if ( oself->aflag & 256 ) {
		monster_wizard();
		oself->currentammo = oself->currentammo + 1;
	}
	if ( oself->aflag & 512 ) {
		monster_dog();
		oself->currentammo = oself->currentammo + 1;
	}
	if ( oself->aflag & 1024 ) {
		monster_fish();
		oself->currentammo = oself->currentammo + 1;
	}
	if ( oself->aflag & 2048 ) {
		monster_hell_knight();
		oself->currentammo = oself->currentammo + 1;
	}
	if ( oself->aflag & 4096 ) {
		monster_tarbaby();
		oself->currentammo = oself->currentammo + 1;
	}

	self = oself;
	remove( temp );

	// setup to be triggered, even if no targetname, because we need to know when a monster dies
	self->use = monster_spawner_use;

	// check for START_ON
	if ( self->spawnflags & 4 ) {
		// state is only used when wait and targetname (toggle mode) are set
		self->state = 1;

		if ( self->spawnflags & 16 ) {
			do {
				monster_spawner_spawn();
			} while ( self->cnt < self->count && self->lives );
		} else {
			// spawn one right away
			if ( self->spawnflags & 8 )
				monster_spawner_spawn();

			self->nextthink = time + self->wait;
			self->think = monster_spawner_spawn;
		}
	} else if ( self->wait > 0 ) {	// if self.wait > 0 and self.targetname is set and START_ON isn't set, then we don't need to do anything
		if ( self->targetname == PR_DEFAULT_SPAWNER_TARGETNAME ) {
			if ( self->spawnflags & 16 ) {
				do {
					monster_spawner_spawn();
				} while ( self->cnt < self->count && self->lives );
			} else {
				// spawn one right away
				if ( self->spawnflags & 8 )
					monster_spawner_spawn();

				self->nextthink = time + self->wait;
				self->think = monster_spawner_spawn;
			}
		}
	} else if ( self->targetname == PR_DEFAULT_SPAWNER_TARGETNAME ) {
		do {
			monster_spawner_spawn();
		} while ( self->cnt < self->count && self->lives );
	} else {
		if ( self->spawnflags & 16 ) {
			do {
				monster_spawner_spawn();
			} while ( self->cnt < self->count && self->lives );
		} else {
			// spawn one right away
			if ( self->spawnflags & 8 )
				monster_spawner_spawn();
		}
	}

	// reset total_monsters
	total_monsters = ftmp;
}

} // END namespace Progs

#endif
