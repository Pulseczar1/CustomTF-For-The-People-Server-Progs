/* ALL MONSTERS SHOULD BE 1 0 0 IN COLOR */

#include "progs.h"
#include "ofndefs.h"
#include "cpstuff.h"
#include "warlock.h"
#include "debug.h"
#include "army.h"
#include "idmonsters/common.h"

namespace Progs {

// name =[framenum,	nexttime, nextthink] {code}
// expands to:
// name ()
// {
//		self.frame=framenum;
//		self.nextthink = time + nexttime;
//		self.think = nextthink
//		<code>
// };

//- OfN -
entity CreateWaypoint(const vector& location, float life, float type, entity sld);
string GetMonsterName(entity themonster);
void KillSoul(entity warlock, float soulnum);

/*
=============
visible

returns 1 if the entity is visible to self, even if not infront ()
=============
*/
float visible(entity targ)
{
	vector	spot1, spot2;

	spot1 = self->origin + self->view_ofs;
	spot2 = targ->origin + targ->view_ofs;
	traceline (spot1, spot2, PR_TL_BSP_ONLY, self);	// see through other monsters

	if (trace_inopen && trace_inwater)
		return PR_FALSE;			// sight line crossed contents

	if (trace_fraction == 1)
		return PR_TRUE;
	return PR_FALSE;
}

//CH so you can check other then self
float visible2(entity targ, entity check)
{
	vector	spot1, spot2;

	spot1 = check->origin + check->view_ofs;
	spot2 = targ->origin + targ->view_ofs;
	traceline (spot1, spot2, PR_TL_BSP_ONLY, check);	// see through other monsters

	if (trace_inopen && trace_inwater)
		return PR_FALSE;			// sight line crossed contents

	if (trace_fraction == 1)
		return PR_TRUE;
	return PR_FALSE;
}

//- OfN - Used for haxxx and sentrygun targetting
float visible2x(entity targ, entity check)
{
	vector	spot1, spot2;

	spot1 = check->origin + check->view_ofs;
	spot2 = targ->origin + targ->view_ofs;

    if (check->classname == "building_sentrygun")
    {
        if (check->tf_items & PR_NIT_TURRET)
            spot1 = check->origin + check->view_ofs - V({0, 0, 20});
        else
            spot1 = check->origin + check->view_ofs + V({0, 0, 20});
    }

    traceline (spot1, spot2, PR_TL_BSP_ONLY, check);	// see through other monsters

	if (trace_inopen && trace_inwater)
		return PR_FALSE;			// sight line crossed contents

	if (trace_fraction == 1)
		return PR_TRUE;
	return PR_FALSE;
}


//#ifndef COOP_MODE
#ifdef PR_NEVER_DEFINED
/*
void() monster_ogre =
{
	dremove(self);
};

void() monster_knight =
{
	dremove(self);
};

void() monster_shambler =
{
	dremove(self);
};

void() monster_demon1 =
{
	dremove(self);
};

void() monster_wizard =
{
	dremove(self);
};

void() monster_zombie =
{
	dremove(self);
};

void() monster_dog =
{
	dremove(self);
};

void() monster_hell_knight =
{
	dremove(self);
};

void() monster_tarbaby =
{
	dremove(self);
};

void() monster_vomit =
{
	dremove(self);
};

void() monster_enforcer =
{
	dremove(self);
};

void() monster_shalrath =
{
	dremove(self);
};

void() monster_dragon =
{
	dremove(self);
};

void() monster_army =
{
	dremove(self);
};

*/

/*
==============================================================================

MOVETARGET CODE

The angle of the movetarget effects standing and bowing direction, but has no effect on movement, which allways heads to the next target.

targetname
must be present.  The name of this movetarget.

target
the next spot to move to.  If not present, stop here for good.

pausetime
The number of seconds to spend standing or bowing for path_stand or path_bow

==============================================================================
*/

/*
void() t_movetarget;

void() movetarget_f =
{
	if (!self.targetname)
		objerror ("monster_movetarget: no targetname");

	self.solid = #SOLID_TRIGGER;
	self.touch = t_movetarget;
	setsize (self, '-8 -8 -8', '8 8 8');
};

*/

/*QUAKED path_corner (0.5 0.3 0) (-8 -8 -8) (8 8 8)
Monsters will continue walking towards the next target corner.
*/

/*
void() path_corner =
{
	if (CheckExistence() == #FALSE)
	{
		dremove(self);
		return;
	}

	movetarget_f ();
};
*/

/*
=============
t_movetarget

Something has bumped into a movetarget.  If it is a monster
moving towards it, change the next destination and continue.
==============
*/

/*
void() t_movetarget =
{
	local entity	temp;

	if (other.movetarget != self)
		return;

	if (other.enemy)
		return;		// fighting, not following a path

	temp = self;
	self = other;
	other = temp;
*/
/*
	if (self.classname == "monster_ogre")
		sound (self, #CHAN_VOICE, "ogre/ogdrag.wav", 1, #ATTN_IDLE);// play chainsaw drag sound
*/
/*
//RPrint ("t_movetarget\n");
	self.goalentity = self.movetarget = find (world, targetname, other.target);
	self.ideal_yaw = vectoyaw(self.goalentity.origin - self.origin);
	if (!self.movetarget)
	{
		self.pausetime = time + 999999;
		self.th_stand ();
		return;
	}
};

float(float v) anglemod =
{
	while (v >= 360)
		v = v - 360;
	while (v < 0)
		v = v + 360;
	return v;
};

float(entity targ) range =
{
	local vector	spot1, spot2;
	local float		r;

	spot1 = self.origin + self.view_ofs;
	spot2 = targ.origin + targ.view_ofs;

	r = vlen (spot1 - spot2);
	if (r < 120)
		return #RANGE_MELEE;
	if (r < 500)
		return #RANGE_NEAR;
	if (r < 1000)
		return #RANGE_MID;
	return #RANGE_FAR;
};

*/
/*
=============
infront

returns 1 if the entity is in front (in sight) of self
=============
*/
/*
float(entity targ) infront =
{
	local vector	vec;
	local float		dot;

	makevectors (self.angles);
	vec = normalize (targ.origin - self.origin);
	dot = vec * v_forward;

	if ( dot > 0.3)
	{
		return #TRUE;
	}
	return #FALSE;
};



//============================================================================

void() HuntTarget =
{
	self.goalentity = self.enemy;
	self.think = self.th_run;
	self.ideal_yaw = vectoyaw(self.enemy.origin - self.origin);
	self.nextthink = time + 0.1;
	SUB_AttackFinished (1);	// wait a while before first attack
};


/COMMENT*
===========
FindTarget

Self is currently not attacking anything, so try to find a target

Returns #TRUE if an enemy was sighted

When a player fires a missile, the point of impact becomes a fakeplayer so
that monsters that see the impact will respond as if they had seen the
player.

To avoid spending too much time, only a single client (or fakeclient) is
checked each frame.  This means multi player games will have slightly
slower noticing monsters.
============
*/

/*
float() FindTarget =
{
//WK THIS CODE IS #DEF-ed out!!
/COMMENT* WK Replace this with the code from sentry
	local entity	client;

	client = checkclient ();

	if (!client)
		return #FALSE;	// current check entity isn't in PVS

	if (client.flags & #FL_NOTARGET)
		return #FALSE;
	if (client.items & #IT_INVISIBILITY)
		return #FALSE;

	if (!visible (client))
		return #FALSE;

	if (client.classname != "player")
		return #FALSE;

	self.enemy = client;

	HuntTarget ();

	return #TRUE;
*/
/*
	local entity client;
	local float	r, gotone, loopc;

	//WK Hack to get floating sentry working
	if (self.tf_items & #NIT_FLOATING_SENTRY) {
		self.origin_z = self.origin_z - 40;
	}

	// Try a few checks to make it react faster
	r = 0;
	loopc = 0;
	gotone = #FALSE;
	while (loopc < 5 && gotone == #FALSE) //WK 3
	{
		client = checkclient();

		gotone = #TRUE;

		if (!client)
			gotone = #FALSE;
/COMMENT*
		if (teamplay)
		{
			// Only attack enemies
			if (client.team_no == self.team_no && self.team_no != 0)
				gotone = #FALSE;

			// Cant see Undercover spies
			if (client.undercover_team == self.team_no && self.team_no != 0)
				gotone = #FALSE;
		}

//		if (client == self.real_owner)
//			gotone = #FALSE;
*/
/*
		if (client.is_feigning)
			gotone = #FALSE;

		if (client.flags & #FL_NOTARGET)
			gotone = #FALSE;
		if (client.items & #IT_INVISIBILITY)
			gotone = #FALSE;

		if (!visible (client))
			gotone = #FALSE;

		r = range (client);
		if (r == #RANGE_FAR)
			gotone = #FALSE;

		if (r == #RANGE_NEAR)
		{
			if (client.show_hostile < time && !infront (client))
				gotone = #FALSE;
		}
		else if (r == #RANGE_MID)
		{
			if ( /COMMENT* client.show_hostile < time || */ !infront (client))
/COMMENT*		gotone = #FALSE;
		}
		loopc = loopc + 1;
		if (gotone) loopc = 1000;
	}

	if (!gotone)
		return #FALSE;

	// Found a Target
	self.enemy = client;
	if (self.enemy.classname != "player")
	{
		self.enemy = self.enemy.enemy;
		if (self.enemy.classname != "player")
		{
			self.enemy = world;
			return #FALSE;
		}
	}

	HuntTarget ();

	return #TRUE;
};*/
#endif

void FoundTarget();

/* WK We are already including these
ai.qc
fight.qc
demon.qc
*/
#ifdef PR_COOP_MODE
// include all the monsters
// WK In normal QW we only need demons
// SB and soldiers, and shamblers

#include "dog.c"
#include "enforcer.c"
#include "fish.c"
#include "hknight.c"
#include "knight.c"
#include "ogre.c"
#include "oldone.c"
#include "shalrath.c"
#include "tarbaby.c"
#include "wizard.c"
#include "zombie.c"

#endif

/*
================
monster_use

Using a monster makes it angry at the current activator
================
*/
void monster_use()
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
	self->enemy = activator;
	self->nextthink = time + 0.1;
	self->think = FoundTarget;
}

/*===========================
set_monster_health

Increases the monsters health
for skill levels above 3
doesnt work
===========================*/

void set_monster_health()
{
#ifndef PR_COOP_MODE_ENHANCED
	skill = cvar("skill");
#endif

	if (skill > 2)
	{
		self->lives = ((skill - 2) * 10) - 1;
		skill = 3;
	}
}


/*
================
monster_death_use

When a mosnter dies, it fires all of its targets with the current
enemy as activator.
================
*/
#ifdef PR_COOP_MODE
//WK This is doubly declared in combat.qc
void monster_death_use()
{
	entity 	ent, otemp;

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
#endif

//============================================================================

void walkmonster_start_go()
{
//	local entity	etemp;
//	local float		failure;
//	local vector	test;

	self->origin[Z] = self->origin[Z] + 1;	// raise off floor a bit
	droptofloor();

/*	failure = 1;

	test = self.origin;
	test_z = test_z - self.mins_z - 1;

	if (pointcontents(test) == #CONTENT_SOLID)
		failure = 0;

	test = self.origin - self.mins;
	test_z = test_z - 1;

	if (pointcontents(test) == #CONTENT_SOLID)
		failure = 0;

	test_x = test_x - self.mins_x + self.maxs_x;

	if (pointcontents(test) == #CONTENT_SOLID)
		failure = 0;

	test_y = test_y - self.mins_y + self.maxs_y;

	if (pointcontents(test) == #CONTENT_SOLID)
		failure = 0;

	test_x = test_x - self.maxs_x + self.mins_x;

	if (pointcontents(test) == #CONTENT_SOLID)
		failure = 0;
*/

	if (!walkmove(0,0))
//	if (failure)
	{
/*		RPrint ("walkmonster in wall at: ");
		RPrint (vtos(self.origin));
		RPrint ("\n"); */

        if (IsOwnedMonster(self))
			if (self->real_owner->classname == "player")
			{
				//self.real_owner.job = self.real_owner.job - (self.real_owner.job & #JOB_DEMON_OUT);
				string MName;
                MName=GetMonsterName(self);
                sprint(self->real_owner,PR_PRINT_HIGH,"Your ");
                sprint(self->real_owner,PR_PRINT_HIGH,MName);
                sprint(self->real_owner,PR_PRINT_HIGH," was beamed into a wall and died.\n");

				/* TODO:
                if (self.classname == "monster_shambler") //- ofn
				{
					self.real_owner.demon_blood = self.real_owner.demon_blood + 4;
					if (self.real_owner.demon_blood > #MAX_KNIFE_BLOOD)
						self.real_owner.demon_blood = #MAX_KNIFE_BLOOD;
				}
                else if (self.classname == "monster_demon1") //- ofn
				{
					self.real_owner.demon_blood = self.real_owner.demon_blood + 2;
					if (self.real_owner.demon_blood > #MAX_KNIFE_BLOOD)
						self.real_owner.demon_blood = #MAX_KNIFE_BLOOD;
				}*/

                /*else if (self.classname == "monster_wizard") //- ofn
				{
					self.real_owner.demon_blood = self.real_owner.demon_blood + ? ;
					if (self.real_owner.demon_blood > #MAX_KNIFE_BLOOD)
						self.real_owner.demon_blood = #MAX_KNIFE_BLOOD;
				}*/

                //kill_his_demons(self.real_owner);
                KillSoul(self->real_owner,self->increase_team1);
				// PZ: give warlock resources (hearts, etc.) back (TODO: meat, heads?)
				float knifekills, neededhearts;
				knifekills = GetHearts(self->real_owner);
				neededhearts = GetNeededHearts(self->real_owner, self->PR_monster_type);	// type should be same as input
				SetHearts(self->real_owner, knifekills + neededhearts);
                return;
			}
		dremove(self);
		return;
	}

	self->takedamage = PR_DAMAGE_AIM;

	self->ideal_yaw = self->angles * V({0, 1, 0});
	if (!self->yaw_speed)
		self->yaw_speed = 20;

	self->view_ofs = V({0, 0, 25});
	self->use = monster_use;

	self->flags = self->flags | PR_FL_MONSTER;

	if (self->target != "")
	{
		self->goalentity = self->movetarget = find(world, "targetname", self->target);
		self->ideal_yaw = vectoyaw(self->goalentity->origin - self->origin);
		if (self->movetarget == world)
		{
			RPrint ("Monster can't find target at ");
			RPrint (vtos(self->origin));
			RPrint ("\n");
		}
// this used to be an objerror
		if (self->movetarget->classname == "path_corner")
			self->th_walk ();
		else
			self->pausetime = 99999999;
			self->th_stand ();
	}
	else
	{
		self->pausetime = 99999999;
		self->th_stand ();
	}

	if (self->classname == "monster_army")
	{
        self->martyr_enemy = CreateWaypoint(self->origin,PR_WAYPOINT_LIFE,PR_WAYPOINT_TYPE_PRIMARY, self);
		self->martyr_enemy->goalentity = world;

        // OFTEN
        self->demon_two=world;
        self->demon_one=world;
        // OFTEN

		self->goalentity = world;
	}

// spread think times so they don't all happen at same time
	self->nextthink = self->nextthink + random()*0.5;
}


void walkmonster_start()
{
// delay drop to floor to make sure all doors have been spawned
// spread think times so they don't all happen at same time
	self->nextthink = self->nextthink + 1; //WK Give time to run from demons
	//WK self.nextthink = self.nextthink + random()*0.5;
	self->think = walkmonster_start_go;
	// Gizmo - don't count summoned monsters in the total monster count
	if ( !IsOwnedMonster( self ) )
		total_monsters = total_monsters + 1;
	set_monster_health();
}



void flymonster_start_go()
{
	self->takedamage = PR_DAMAGE_AIM;

	self->ideal_yaw = self->angles * V({0, 1, 0});
	if (!self->yaw_speed)
		self->yaw_speed = 10;
	self->view_ofs = V({0, 0, 25});
	self->use = monster_use;

	self->flags = self->flags | PR_FL_FLY;
	self->flags = self->flags | PR_FL_MONSTER;

	if (!walkmove(0,0))
	{
		RPrint ("flymonster in wall at: ");
		RPrint (vtos(self->origin));
		RPrint ("\n");
	}

	if (self->target != "")
	{
		self->goalentity = self->movetarget = find(world, "targetname", self->target);
		if (self->movetarget == world)
		{
			RPrint ("Monster can't find target at ");
			RPrint (vtos(self->origin));
			RPrint ("\n");
		}
// this used to be an objerror
		if (self->movetarget->classname == "path_corner")
			self->th_walk ();
		else
			self->pausetime = 99999999;
			self->th_stand ();
	}
	else
	{
		self->pausetime = 99999999;
		self->th_stand ();
	}
}

void flymonster_start()
{
// spread think times so they don't all happen at same time
	self->nextthink = self->nextthink + random()*0.5;

	self->think = flymonster_start_go;
	// Gizmo - don't count summoned monsters in the total monster count
	if ( !IsOwnedMonster( self ) )
		total_monsters = total_monsters + 1;
	set_monster_health();
}


void swimmonster_start_go()
{
	/*if (deathmatch)
	{
		dremove(self);
		return;
	}*/

	self->takedamage = PR_DAMAGE_AIM;

	self->ideal_yaw = self->angles * V({0, 1, 0});
	if (!self->yaw_speed)
		self->yaw_speed = 25; // Original 10
	self->view_ofs = V({0, 0, 10});
	self->use = monster_use;

	self->flags = self->flags | PR_FL_SWIM;
	self->flags = self->flags | PR_FL_MONSTER;

	if (self->target != "")
	{
		self->goalentity = self->movetarget = find(world, "targetname", self->target);
		if (self->movetarget == world)
		{
			RPrint ("Monster can't find target at ");
			RPrint (vtos(self->origin));
			RPrint ("\n");
		}
// this used to be an objerror
		self->ideal_yaw = vectoyaw(self->goalentity->origin - self->origin);
		self->th_walk ();
	}
	else
	{
		self->pausetime = 99999999;
		self->th_stand ();
	}

// spread think times so they don't all happen at same time
	self->nextthink = self->nextthink + random()*0.5;
}

void swimmonster_start()
{
// spread think times so they don't all happen at same time
	self->nextthink = self->nextthink + random()*0.5;
	self->think = swimmonster_start_go;
	// Gizmo - don't count summoned monsters in the total monster count
	if ( !IsOwnedMonster( self ) )
		total_monsters = total_monsters + 1;
	set_monster_health();
}
// WK #endif

//===========================================================================
// Declares a monster dead

void MonsterDead()
{
    self->th_die = SUB_Null;
    self->classname = "monster_corpse";
    SpawnBloodEx(self->origin,0.75,9);
}

//============================================================================
// Sets corpse removal time and flags the entity for corpse damage

void MonsterCorpse()
{
#ifdef PR_COOP_MODE_ENHANCED
	float r;

	if ( self->endtime > 0 && COOP_IsCoopMonster( self ) ) {
		r = random();
		if ( r < 0.5 )
			r = 0.5 + random()*0.5;

		self->nextthink = time + self->endtime*r;
	} else
#endif
		self->nextthink = time + PR_MIN_CORPSE_TIME + (PR_MAX_CORPSE_TIME - PR_MIN_CORPSE_TIME)*random();
	self->think = SUB_Remove;

    #ifdef PR_GIBABLE_CORPSES
    if (self->PR_monster_type == PR_MONSTER_SCRAG)
        MakeCorpse(self,PR_CORPSE_HP_SCRAG);
    else if (self->PR_monster_type == PR_MONSTER_FISH)
        MakeCorpse(self,PR_CORPSE_HP_FISH);
    else if (self->PR_monster_type == PR_MONSTER_DEMON)
        MakeCorpse(self,PR_CORPSE_HP_DEMON);
    else if (self->PR_monster_type == PR_MONSTER_GREMLIN)
        MakeCorpse(self,PR_CORPSE_HP_GREMLIN);
    else if (self->PR_monster_type == PR_MONSTER_SHAMBLER)
        MakeCorpse(self,PR_CORPSE_HP_SHAMBLER);
    else if (self->PR_monster_type == PR_MONSTER_ARMY)
        MakeCorpse(self,PR_CORPSE_HP_ARMY);
    else
        MakeCorpse(self,200); // Shouldnt happen, if it does, bug
    #endif
}

//=======================================================================
// Plays aura of power sound effect when needed for the monster

void MonsterAuraPower()
{
    if (self->aura == PR_AURA_POWER && self->super_sound < time)
    {
        sound (self, PR_CHAN_BODY, "auras/aura2b.wav", 1, PR_ATTN_NORM);
        self->super_sound = time + 1;
    }
}

} // END namespace Progs
