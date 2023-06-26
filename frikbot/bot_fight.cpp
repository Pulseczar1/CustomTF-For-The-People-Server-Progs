/***********************************************
*                                              *
*             FrikBot Fight Code               *
*      "Because I ain't no Ghandi code"        *
*                                              *
***********************************************/
/*
This program is in the Public Domain. My crack legal
team would like to add:

RYAN "FRIKAC" SMITH IS PROVIDING THIS SOFTWARE "AS IS"
AND MAKES NO WARRANTY, EXPRESS OR IMPLIED, AS TO THE
ACCURACY, CAPABILITY, EFFICIENCY, MERCHANTABILITY, OR
FUNCTIONING OF THIS SOFTWARE AND/OR DOCUMENTATION. IN
NO EVENT WILL RYAN "FRIKAC" SMITH BE LIABLE FOR ANY
GENERAL, CONSEQUENTIAL, INDIRECT, INCIDENTAL,
EXEMPLARY, OR SPECIAL DAMAGES, EVEN IF RYAN "FRIKAC"
SMITH HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
DAMAGES, IRRESPECTIVE OF THE CAUSE OF SUCH DAMAGES.

You accept this software on the condition that you
indemnify and hold harmless Ryan "FrikaC" Smith from
any and all liability or damages to third parties,
including attorney fees, court costs, and other
related costs and expenses, arising out of your use
of this software irrespective of the cause of said
liability.

The export from the United States or the subsequent
reexport of this software is subject to compliance
with United States export control and munitions
control restrictions. You agree that in the event you
seek to export this software, you assume full
responsibility for obtaining all necessary export
licenses and approvals and for assuring compliance
with applicable reexport restrictions.

Any reproduction of this software must contain
this notice in its entirety.
*/

#include "progs.h"
#include "ofndefs.h"
#include "frikbot/bot_qw.h"
#include "frikbot/bot_misc.h"
#include "frikbot/bot_ai.h"
#include "frikbot/bot_move.h"
#include "custom.h"

namespace Progs {

//float modelindex_eyes;    // PZ: apparently, declaring globals in multiple files is like using "extern"

// PZ: Basically, can I use this weapon to check for spies and thieves?
float frik_doesWeaponHurtTeammates(float weap)
{
	if (weap == PR_WEAP_HOOK)    return PR_FALSE;
	if (weap == PR_WEAP_MEDIKIT) return PR_FALSE;
	if (weap == PR_WEAP_SPANNER) return PR_FALSE;
	if (weap == PR_WEAP_AIRF)    return PR_FALSE;
	return PR_TRUE;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_sizeUpPlayer    (PZ: was "bot_size_player")

PZ NOTE: This function doesn't take into account the fact
that some weapons are only dangerous at certain ranges
(not sure whether it should).

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_sizeUpPlayer(entity e)
{
	float playerSize;

	playerSize = e->health + e->armorvalue * e->armortype;

	// PZ: used .current_weapon instead for CuTF (was using .weapon here); check ammo too?
	if      (e->current_weapon == PR_WEAP_ROCKET_LAUNCHER)   // IT_ROCKET_LAUNCHER
		playerSize = playerSize + 60;
	else if (e->current_weapon == PR_WEAP_LIGHTNING)         // IT_LIGHTNING
		playerSize = playerSize + 60;
	else if (e->current_weapon == PR_WEAP_GRENADE_LAUNCHER)  // IT_GRENADE_LAUNCHER
		playerSize = playerSize + 50;
	else if (e->current_weapon == PR_WEAP_SNG)               // IT_SUPER_NAILGUN
		playerSize = playerSize + 50;
	else if (e->current_weapon == PR_WEAP_NAILGUN)           // IT_NAILGUN
		playerSize = playerSize + 40;
	else if (e->current_weapon == PR_WEAP_SUPER_SHOTGUN)     // IT_SUPER_SHOTGUN
		playerSize = playerSize + 40;
	else if (e->current_weapon == PR_WEAP_SHOTGUN)           // IT_SHOTGUN
		playerSize = playerSize + 10;
	else if (e->current_weapon == PR_WEAP_AXE)               // IT_AXE
		playerSize = playerSize - 50;
	// PZ: CuTF weapons
	else if (e->current_weapon == PR_WEAP_HOOK)
		playerSize = playerSize + 5;
	else if (e->current_weapon == PR_WEAP_MEDIKIT)
		playerSize = playerSize + 40;
	else if (e->current_weapon == PR_WEAP_SPANNER)
		playerSize = playerSize - 50;
	else if (e->current_weapon == PR_WEAP_SNIPER_RIFLE)
		playerSize = playerSize + 60;
	else if (e->current_weapon == PR_WEAP_AUTO_RIFLE)
		playerSize = playerSize + 20;
	else if (e->current_weapon == PR_WEAP_LIGHT_ASSAULT)
		playerSize = playerSize + 60;
	else if (e->current_weapon == PR_WEAP_FLAMETHROWER)
		playerSize = playerSize + 50;
	else if (e->current_weapon == PR_WEAP_INCENDIARY)
		playerSize = playerSize + 50;
	else if (e->current_weapon == PR_WEAP_ASSAULT_CANNON)
		playerSize = playerSize + 80;
	else if (e->current_weapon == PR_WEAP_DAEDALUS)
		playerSize = playerSize + 40;
	else if (e->current_weapon == PR_WEAP_TRANQ)
		playerSize = playerSize + 40;
	else if (e->current_weapon == PR_WEAP_RAILGUN)
		playerSize = playerSize + 40;
	else if (e->current_weapon == PR_WEAP_MAUSER)
		playerSize = playerSize + 60;
	else if (e->current_weapon == PR_WEAP_AIRF)
		{}//playerSize = playerSize + 0;
	else if (e->current_weapon == PR_WEAP_LASERCANNON)
		playerSize = playerSize + 50;
	else if (e->current_weapon == PR_WEAP_ZEROGRAVITY)
		{}//playerSize = playerSize + 0;
	// PZ: END

	if (e->items & PR_IT_QUAD)
		playerSize = playerSize + 200;
	if (e->items & PR_IT_INVULNERABILITY)
		playerSize = playerSize + 300;
	if (e->items & PR_IT_INVISIBILITY)     // PZ: do I need to add spy and thief here?
		playerSize = playerSize + 100;  // PZ: why was this 250?

	return playerSize;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_lookForHazardousObject    (PZ: was "bot_dodge_stuff")

This looks for something to make the bot's `.b_entityToAvoid`.

`self` = the current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// PZ: This function contained the target acquisition code. Had to put that in its own function for sake of being organized.
void frik_lookForHazardousObject()
{
	entity foe;
	float nearest, dist;

	if (waypoint_mode > PR_WM_LOADED) // if we're in the waypoint editor, return
		return;

	self->b_entityToAvoid = world;
	nearest = 256;

	// PZ TODO: done adding CuTF stuff?
	// PZ NOTE: all gren1 and gren2 seem to have a classname "grenade"; so, this should be working on those nades too
	// PZ TODO: but I don't think bots are dodging any grenades (they do, I think, but not if there's a lot going on)
	// PZ TODO: WE COULD PROBABLY SPEED THIS UP A LOT; WHY IS IT LOOKING THROUGH ALL GRENADES? WHY NOT JUST DO A RADIUS SEARCH?
	// unfortunately, findradius() looks through the entire list of entities, too
	// PZ TODO: add caltrops
	foe = find(world, "classname", "grenade");
	while (foe != world)
	{
		dist = vlen(foe->origin - self->origin);
		if (dist < nearest)
		{
			nearest = dist;
			self->b_entityToAvoid = foe;
		}
		foe = find(foe, "classname", "grenade");
	}
	if (self->b_entityToAvoid == world)
	{
		foe = find(world, "classname", "missile");
		while (foe != world)
		{
			if (foe->owner != self)
			{
				dist = vlen(foe->origin - self->origin);
				if (dist < nearest)
				{
					nearest = dist;
					self->b_entityToAvoid = foe;
				}
			}
			foe = find(foe, "classname", "missile");
		}
		if (self->b_entityToAvoid == world)
		{
			foe = find(world, "classname", "spike");
			while(foe != world)
			{
				if (foe->owner != self)
				{
					dist = vlen(foe->origin - self->origin);
					if (dist < nearest)
					{
						nearest = dist;
						self->b_entityToAvoid = foe;
					}
				}
				foe = find(foe, "classname", "spike");
			}
		}
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_acquireTargetForAttacking    (PZ: created this function)

This looks for something to make the bot's `.enemy` --
the entity that the bot will attempt to attack.

`self` = the current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// PZ: This code was in frik_lookForHazardousObject(). Moved to its own function.
// PZ: Called this function right after where frik_lookForHazardousObject() is called.
void frik_acquireTargetForAttacking()
{
	entity foe;
	float foeSize, newFoeSize, distanceToFoe;
	vector distanceToEnemy;

	// calculate the power of our current enemy (.enemy is the entity that we are attempting to engage in battle)
	if (self->enemy != world) // PZ FIXME: this is pointless if the below areas can only be entered if you have no .enemy
	{
		distanceToEnemy = self->origin - frik_getRealOriginOfEntity(self->enemy);

		foeSize = frik_sizeUpPlayer(self->enemy);
		foeSize = foeSize + vlen(distanceToEnemy) * 0.5;
	}
	else
		foeSize = 9999999;

	if (coop)
	{
		if (self->enemy == world)
		{
			foe = findradius(self->origin, 9999);
			while (foe != world)
			{
				if (foe->flags & PR_FL_MONSTER)
				{
					if (foe->health > 0)
					{
						// the weaker he is and the closer he is, the more we chase him
						newFoeSize = frik_sizeUpPlayer(foe) + vlen(foe->origin - self->origin) * 0.5;
						if (newFoeSize < foeSize)
						{
							if (frik_isVisible_Complex(foe))
							{
								self->enemy = foe;
								foeSize = newFoeSize;
							}
						}
					}
				}
				foe = foe->chain;
			}
		}
	}
	else // PZ: THIS SEEMS TO BE ITS TARGET ACQUISITION CODE (WHY IT'S IN frik_lookForHazardousObject() I HAVE NO CLUE)
	{
		// PZ: using the coop code above as model, since it doesn't just look at players (old code is commented below)
		/*foe = player_head;
		while (foe)
		{
			//bprint(2, "foe = ", foe.netname, "\n");
			if (foe != self)
			{
				if (foe.modelindex != 0)
				{
					if (foe.health > 0)
					{
						//if (!(teamplay && self.team == foe.team))
						if (!(teamplay && self.team_no == foe.team_no)) // PZ: CuTF uses team_no
						{
							newFoeSize = frik_sizeUpPlayer(foe) + vlen(foe.origin - self.origin) * 0.5;
							if (newFoeSize < foeSize)
							{
								if (frik_isInFovOfSelf(foe) || foe.b_sound > time || self.b_skill == #SKILL_NIGHTMARE)
								{
									if (frik_isVisible_Complex(foe))
									{
										// PZ: I don't know why this is in the dodge function
										self.enemy = foe;
										foeSize = newFoeSize;
									}
								}
							}
						}
					}
				}
			}
			foe = foe._next;
		}*/

		// PZ: new code: we want bots to attack more than just players and each other
		/*local float cont; // emulate C 'continue' statement
		if (!self.enemy)
		{
			foe = findradius(self.origin, 9999); // TODO: JUST USE NEXTENT(). IT'S A LITTLE FASTER nextent(world);
			while (foe)
			{
				cont = #FALSE;
				// PZ: if foe is partially invisible, lower the chance that bot will see him
				if (foe.items & #IT_INVISIBILITY ||
				    (foe.job & #JOB_THIEF && foe.job & #JOB_ACTIVE && !(foe.job & #JOB_FULL_HIDE)))
				{
					if (vlen(foe.origin - self.origin) > 300)
						cont = #TRUE;
					if (random() > 0.01) // 99% chance we'll ignore this player on this check
						cont = #TRUE; // can't remember for sure, but I don't think the compiler's 'continue' always works
				}
				if (!cont)
				if (foe.classname == "player" || foe.flags & #FL_MONSTER || foe.flags & #FL_MACHINE || foe.classname == "monster_army")
				{
					if (foe != self && foe.modelindex != 0 && foe.health > 0)
					{
						if (!(teamplay && (self.team_no == foe.team_no || self.team_no == foe.undercover_team))) // PZ: added disguise check
						{
							// would be great here if I could check avelocity for rotation but I can't (for feigning)
							if (!(foe.job & #JOB_THIEF && foe.job & #JOB_FULL_HIDE) && !(foe.is_feigning && foe.velocity == '0 0 0')) // PZ: added thief check and feign check
							{
								newFoeSize = frik_sizeUpPlayer(foe) + vlen(foe.origin - self.origin) * 0.5;
								if (newFoeSize < foeSize)
								{
									if (frik_isInFovOfSelf(foe) || foe.b_sound > time || self.b_skill == #SKILL_NIGHTMARE)
									{
										if (frik_isVisible_Complex(foe))
										{
											self.enemy = foe;
											foeSize = newFoeSize;
										}
									}
								}
							}
						}
					}
				}
				foe = foe.chain;
			}
		}*/

		// PZ: trying to speed up the above code
		float ignorePlayer; // emulate C 'continue' statement
		if (self->enemy == world)
		{
			//foe = findradius(self.origin, 9999); // PZ: using nextent(); it's a little faster
			foe = nextent(world);
			while (foe != world)
			{
				distanceToFoe = vlen(foe->origin - self->origin);
				float isTeammateOrDisguisedAsTeammate;          isTeammateOrDisguisedAsTeammate = PR_FALSE;
				if      (Teammate(self->team_no, foe->team_no))         isTeammateOrDisguisedAsTeammate = PR_TRUE;
				else if (Teammate(self->team_no, foe->undercover_team)) isTeammateOrDisguisedAsTeammate = PR_TRUE;

				ignorePlayer = PR_FALSE;
				if (foe->classname != "player" && !(foe->flags & PR_FL_MONSTER) && !(foe->flags & PR_FL_MACHINE) && foe->classname != "monster_army"
				    && foe->netname != "land_mine" && foe->classname != "holo")
					ignorePlayer = PR_TRUE;
				else if (foe == self /*|| foe.modelindex == 0*/ || (foe->health <= 0 && !(self->weapons_carried & PR_WEAP_MEDIKIT && teamplay && isTeammateOrDisguisedAsTeammate))) // PZ: took out on 1-24-15; already have cases for invis
					ignorePlayer = PR_TRUE;
				if (!ignorePlayer) if (teamplay && isTeammateOrDisguisedAsTeammate) // PZ: added disguise check
				{
					// PZ: Hack to get medic to attack his teammates with the medikit. Added 10-4-15.
					if (!(self->weapons_carried & PR_WEAP_MEDIKIT && foe->classname == "player"))
						ignorePlayer = PR_TRUE;
				}
				// would be great here if I could check avelocity for rotation but I can't (when players look around while feigning, their model turns)
				// PZ: added thief check and feign check (checking velocity, because players can jump while feigning) // remove_.. indicates having flag
				if (!ignorePlayer) if (((foe->job & PR_JOB_THIEF && foe->job & PR_JOB_FULL_HIDE) || (foe->is_feigning && foe->velocity == V({0, 0, 0}))) && !foe->remove_spawnpoint)
				{
					if (!frik_doesWeaponHurtTeammates(self->current_weapon))
					{
						if (random() <= 0.90)
							ignorePlayer = PR_TRUE;
					}
					else ignorePlayer = PR_TRUE;
				}
				// PZ: if foe is partially invisible, lower the chance that bot will see him
				// PZ TODO: does invisibility make you completely invis when you stop moving? could check .velocity here, if so // remove_.. indicates having flag
				if (!ignorePlayer) if ((foe->items & PR_IT_INVISIBILITY || (foe->job & PR_JOB_THIEF && foe->job & PR_JOB_ACTIVE)) && !foe->remove_spawnpoint &&
				         frik_doesWeaponHurtTeammates(self->current_weapon))
				{
					if (distanceToFoe > (40*PR_ONE_FOOT))  // was just "300"
						ignorePlayer = PR_TRUE;
					else if (random() > 0.05) // 95% chance we'll ignore this player on this check
						ignorePlayer = PR_TRUE; // can't remember for sure, but I don't think the compiler's 'continue' always works
				}
				// PZ: We want disguised spies to ignore players more often.
				if (!ignorePlayer) if (self->is_undercover && !foe->remove_spawnpoint)
				{
					if (distanceToFoe > (10*PR_ONE_FOOT))
						ignorePlayer = PR_TRUE;
					else if (random() > 0.05) // 95% chance we'll ignore this player on this check
						ignorePlayer = PR_TRUE;
				}
//if (foe.classname == "holo")
//if (ignorePlayer)
//bprint(2, self.netname, " ignored the holograph!!!!\n");
				// Now that we have determined to not flat out ignore this entity, determine its attractiveness as a target for attack.
				if (!ignorePlayer)
				{
					newFoeSize = frik_sizeUpPlayer(foe) + distanceToFoe * 0.5;
					// PZ: Hack to get medic to attack his teammates with the medikit. Added 10-4-15.
					entity goalTarget; goalTarget = frik_getGoalTarget(); // the highest numbered target
					if (foe == goalTarget) newFoeSize = -999999;  // PZ: added this on 10-27-15
					if (foe->tfstate & PR_TFSTATE_INFECTED && self->weapons_carried & PR_WEAP_MEDIKIT)
						if (teamplay && isTeammateOrDisguisedAsTeammate)
							newFoeSize = -999999;
					if (foe->remove_spawnpoint && foe->classname == "player") newFoeSize = newFoeSize - 300; // PZ: want you to always go for flag carriers
					// Snipers and engineers usually own bots really badly, because bots don't perceive them as a high threat that needs to be
					// handled. Hopefully now they will.
					if (foe->flags & PR_FL_MACHINE)                    newFoeSize = newFoeSize - 300; // PZ: get the bots to focus more on engineer buildings
					if (foe->weapons_carried & PR_WEAP_SNIPER_RIFLE)   newFoeSize = newFoeSize - 300; // PZ: get the bots to focus more on snipers
					if (self->weapons_carried & PR_WEAP_SNIPER_RIFLE)
					{
						if (foe->flags & PR_FL_MACHINE || foe->weapons_carried & PR_WEAP_SNIPER_RIFLE) newFoeSize = -999998;
						if (foe->remove_spawnpoint && foe->classname == "player")                  newFoeSize = -999999; // flag carrier
					}
					if (newFoeSize < foeSize) // we go for the weakest/nearest target
					{                                  // foe just made a sound
						if (frik_isInFovOfSelf(foe) || foe->b_sound > time || self->b_skill >= PR_SKILL_NIGHTMARE)
						{
							if (frik_isVisible_Complex(foe))
							{
//if (foe.classname == "holo")
//bprint(2, self.netname, " SET HOLO AS ENEMY!!!!\n");
								self->enemy = foe;
								foeSize = newFoeSize;
							}
						}
					}
				}
				foe = nextent(foe);
			}
		}
	}
//if (self.enemy.classname == "holo")
//bprint(2, self.netname, " SET HOLO AS ___FINAL___ ENEMY!!!!\n");
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_getRangeOfWeapon       (PZ: was called "weapon_range")

Returned vector:
_x = "sweet spot range"; try to maintain this range if possible
_y = minimum range bot can be to be effective (rl/gl) (move away)
_z = maximum range bot can be to be effective (lg/axe) (move in)

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
vector frik_getRangeOfWeapon(float wpn)
{
	// ##REMEMBER##: bots won't select the weapon if their current range to target is outside of min and max ranges
	// PZ: added CuTF weapons and used defines corresponding to .current_weapon
	// PZ TODO: don't just guess; look up the range of weapons
	// PZ NOTE: Melee weapons give 64 range. 96 if you have Close Combat. TODO: Account for Close Combat in these.
	if      (wpn == PR_WEAP_AXE)              // IT_AXE
		return V({0, 0, 64});                    // PZ: was '48 0 512'
	else if (wpn == PR_WEAP_SHOTGUN)          // IT_SHOTGUN
		return V({512, 0, 99999});
	else if (wpn == PR_WEAP_SUPER_SHOTGUN)    // IT_SUPER_SHOTGUN
		return V({128, 0, 99999});
	else if (wpn == PR_WEAP_NAILGUN)          // IT_NAILGUN
		return V({180, 0, 99999});
	else if (wpn == PR_WEAP_SNG)              // IT_SUPER_NAILGUN
		return V({180, 0, 99999});
	else if (wpn == PR_WEAP_GRENADE_LAUNCHER) // IT_GRENADE_LAUNCHER
		return V({180, 128, 1000});              // PZ: 128 was 48 (10-6-14)
	else if (wpn == PR_WEAP_ROCKET_LAUNCHER)  // IT_ROCKET_LAUNCHER
		return V({180, 128, 99999});             // PZ: 128 was 48 (10-6-14)
	else if (wpn == PR_WEAP_LIGHTNING)        // IT_LIGHTNING              // PZ NOTE: lightning gun has a range of 600 units
		return V({350, 0, 600});                                              // PZ: was 512
	// PZ: CuTF weapons
	else if (wpn == PR_WEAP_HOOK)
		return V({0, 0, 99999});
	else if (wpn == PR_WEAP_MEDIKIT)
		return V({0, 0, 128});                   // was '48 0 512'
	else if (wpn == PR_WEAP_SPANNER)
		return V({0, 0, 64});
	else if (wpn == PR_WEAP_SNIPER_RIFLE)
		return V({2000, 0, 99999});              // keep ideal high, to keep bot away from his target?
	else if (wpn == PR_WEAP_AUTO_RIFLE)
		return V({2000, 0, 99999});
	else if (wpn == PR_WEAP_LIGHT_ASSAULT)
		return V({180, 0, 1000});
	else if (wpn == PR_WEAP_FLAMETHROWER)
		return V({128, 0, 500});
	else if (wpn == PR_WEAP_INCENDIARY)
		return V({180, 128, 99999});             // 128 was 48 (10-6-14)
	else if (wpn == PR_WEAP_ASSAULT_CANNON)
		return V({180, 0, 1000});
	else if (wpn == PR_WEAP_DAEDALUS)
		return V({180, 128, 99999});             // 128 was 48 (10-6-14)
	else if (wpn == PR_WEAP_TRANQ)
		return V({180, 0, 99999});
	else if (wpn == PR_WEAP_RAILGUN)
		return V({180, 0, 99999});
	else if (wpn == PR_WEAP_MAUSER)
		return V({0, 0, 100});                   // has a range of 100
	else if (wpn == PR_WEAP_AIRF)
		return V({48, 0, 256});
	else if (wpn == PR_WEAP_LASERCANNON)
		return V({180, 0, 99999});
	else if (wpn == PR_WEAP_ZEROGRAVITY)
		return V({0, 0, 64});
	// PZ: END
	else return V({0, 0, 0});
}

entity swapInWeaponList(entity e1, entity e2, entity firstWeapon);

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_makeWeaponSelection     (PZ: was called "bot_weapon_switch")

Pick best weapon based on ammo and range, depending on your level of skill.

`rangeToTarget`: set to -1 if range isn't a concern
`self` = the current bot

Base value range: 0 to 5    (0 means useless (won't get selected))
We can have a value larger than base value. Think of it as extra credit.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// PZ: added CuTF weapons and made weapon selection much more sophisticated
// PZ TODO DONE?: what happens with weapons that use the same impulse and require multiple key presses to select them? Use a timer ent to select again?
// PZ TODO: convert the linked list here to the generic one, called "LinkedList"
void frik_makeWeaponSelection(float rangeToTarget, float onlyAllowBetterWeapon)
{
	int weapons/*, weap, mpulse*/;
	vector weaponRange;
	float valueOfCurrentWeapon = 0;
	bool forceBestWeapon = false;   // this flag means no chance for bot to choose non-best weapon

	float isTeammateOrDisguisedAsTeammate;                          isTeammateOrDisguisedAsTeammate = PR_FALSE;
	if      (Teammate(self->team_no, self->enemy->team_no))         isTeammateOrDisguisedAsTeammate = PR_TRUE;
	else if (Teammate(self->team_no, self->enemy->undercover_team)) isTeammateOrDisguisedAsTeammate = PR_TRUE;

	//weapons = self.items & 127;
	weapons = self->weapons_carried;  // PZ: for CuTF

	//mpulse = 1; // PZ: axe is last option

	// we are going to make a linked list of ranked weapons that the bot can choose from
	entity firstWeapon = world, ptr = world;

//bprint(#PRINT_HIGH, self.netname, ": ranking weapons...\n");

	// Place a value on all the weapons that we have. .heat is the value.
	// Only bother placing a value on the weapon, if we have ammo for the weapon.
	// If a weapon is currently useless, don't add it to the list of weapon values.
	// Higher values are better choices, at least for the current situation.
	// A weapon with a value of 0 will always not be chosen.
	while (weapons)
	{
		if (firstWeapon == world) // for the first in the list
		{
			ptr = spawnServerSide(); // PZ: make it a server-side only entity
			firstWeapon = ptr;       // hold on to the first one
		}
		else
		{
			ptr->_next = spawnServerSide(); // PZ: make it a server-side only entity
			ptr->_next->_prev = ptr;
			ptr = ptr->_next;
		}
		ptr->classname = "weapon_list_element";

		// PZ: copied and pasted this from W_BestWeapon(), replacing the original code which was basically the same
		// PZ TODO DONE?: change this so that each weapon has a rank; that way weapons with similar ranks can be randomly selected
		if (weapons & PR_WEAP_LIGHTNING)
		{
			ptr->weapon = PR_WEAP_LIGHTNING;
			ptr->impulse = 8;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 5;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				ptr->heat = 5;
			}
			if (self->waterlevel > 1) // PZ NOTE: waterlevel of 3 is right when the face goes under; 2 is around the knees or waist
			{
				if (self->b_skill > PR_SKILL_EASY) ptr->heat = 0;
				else                            ptr->heat = ptr->heat - 3;
			}
			// never use in these situations
			if (self->ammo_cells < 1) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_SNIPER_RIFLE)
		{
			ptr->weapon = PR_WEAP_SNIPER_RIFLE;
			ptr->impulse = 2;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 5;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				// check whether it's within a certain range of the ideal range (weaponRange_x)
				if (rangeToTarget >= weaponRange[X] - 200)
					ptr->heat = 5;
				else
					ptr->heat = 3; // we should probably switch weapons if target gets close
			}
			if (self->waterlevel > 2 && !(self->tf_items & PR_NIT_SCUBA)) // his feet have to be on the ground or he has to have scuba gear
			{
				if (self->b_skill > PR_SKILL_EASY) ptr->heat = 0;
				else                            ptr->heat = ptr->heat - 3;
			}
			// never use in these situations
			if (self->ammo_shells < 1) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_ROCKET_LAUNCHER)
		{
			ptr->weapon = PR_WEAP_ROCKET_LAUNCHER;
			ptr->impulse = 7;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 5;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				// check whether it's within a certain range of the ideal range (weaponRange_x)
				if (rangeToTarget <= weaponRange[X] + 400)
					ptr->heat = 5;
				else
					ptr->heat = 4; // value less with far away targets
			}
			// never use in these situations
			if (self->ammo_rockets < 1) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_DAEDALUS)
		{
			ptr->weapon = PR_WEAP_DAEDALUS;
			ptr->impulse = 8;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 3;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				// check whether it's within a certain range of the ideal range (weaponRange_x)
				if (rangeToTarget <= weaponRange[X] + 400)
					ptr->heat = 3;
				else
					ptr->heat = 2; // value less with far away targets
			}
			// if target is a building or tanky player, weapon's value is really good
			if (self->enemy->flags & PR_FL_MACHINE || self->enemy->maxarmor >= 200)
				ptr->heat = ptr->heat + 3;
			// never use in these situations
			if (self->ammo_cells < 5) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_ASSAULT_CANNON)
		{
			ptr->weapon = PR_WEAP_ASSAULT_CANNON;
			ptr->impulse = 7;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 5;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				// check whether it's within a certain range of the ideal range (weaponRange_x)
				if (rangeToTarget <= weaponRange[X] + 400)
					ptr->heat = 5;
				else
					ptr->heat = 4; // value less with far away targets
			}
			if (self->waterlevel > 2)
			{
				if (self->b_skill > PR_SKILL_EASY) ptr->heat = 0;
				else                               ptr->heat = ptr->heat - 3;
			}
			// never use in these situations
			if (self->ammo_cells < 6 || self->ammo_shells < 1) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_LIGHT_ASSAULT)
		{
			ptr->weapon = PR_WEAP_LIGHT_ASSAULT;
			ptr->impulse = 5;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 4;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				// check whether it's within a certain range of the ideal range (weaponRange_x)
				if (rangeToTarget <= weaponRange[X] + 400)
					ptr->heat = 4;
				else
					ptr->heat = 3; // value less with far away targets
			}
			// never use in these situations
			if (self->ammo_nails < 10) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_INCENDIARY)
		{
			ptr->weapon = PR_WEAP_INCENDIARY;
			ptr->impulse = 7;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 4;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				// check whether it's within a certain range of the ideal range (weaponRange_x)
				if (rangeToTarget <= weaponRange[X] + 400)
					ptr->heat = 4;
				else
					ptr->heat = 3; // value less with far away targets
			}
			// not that good in water, but not terrible
			if (self->waterlevel > 1) ptr->heat = ptr->heat - 2;
			// TODO: subtract value if target has asbestos armor? do similarly for other weapons and armor types? only for smart bots?
			// never use in these situations
			if (self->ammo_rockets < 3) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_GRENADE_LAUNCHER)
		{
			ptr->weapon = PR_WEAP_GRENADE_LAUNCHER;
			ptr->impulse = 6;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 4;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				// check whether it's within a certain range of the ideal range (weaponRange_x)
				if (rangeToTarget <= weaponRange[X] + 400)
					ptr->heat = 4;
				else
					ptr->heat = 3; // value less with far away targets
			}
			// never use in these situations
			if (self->enemy->solid == PR_SOLID_BSP) ptr->heat = 1; // use as last resort on buttons/doors
			if (self->ammo_rockets < 1) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_LASERCANNON)
		{
			ptr->weapon = PR_WEAP_LASERCANNON;
			ptr->impulse = 4;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 3;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				// check whether it's within a certain range of the ideal range (weaponRange_x)
				if (rangeToTarget <= weaponRange[X] + 400)
					ptr->heat = 3;
				else
					ptr->heat = 2; // value less with far away targets
			}
			// never use in these situations
			if (self->ammo_cells < 1) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_SNG)
		{
			// TODO: I really should buff up the SNG; make it a badass again, like it was in vanilla Quake (I think its firing rate is slower in CuTF)
			ptr->weapon = PR_WEAP_SNG;
			ptr->impulse = 5;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 4;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				// check whether it's within a certain range of the ideal range (weaponRange_x)
				if (rangeToTarget <= weaponRange[X] + 400)
					ptr->heat = 4;
				else
					ptr->heat = 3; // value less with far away targets
			}
			// never use in these situations
			if (self->ammo_nails < 2) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_FLAMETHROWER)
		{
			ptr->weapon = PR_WEAP_FLAMETHROWER;
			ptr->impulse = 6;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 4;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				ptr->heat = 4;
			}
			if (self->waterlevel > 1)
			{
				if (self->b_skill > PR_SKILL_EASY) ptr->heat = 0;
				else                               ptr->heat = ptr->heat - 3;
			}
			// never use in these situations
			if (self->enemy->solid == PR_SOLID_BSP) ptr->heat = 0; // it won't work on buttons/doors
			if (self->ammo_cells < 1) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_SUPER_SHOTGUN)
		{
			ptr->weapon = PR_WEAP_SUPER_SHOTGUN;
			ptr->impulse = 3;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 3;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				// check whether it's within a certain range of the ideal range (weaponRange_x)
				if (rangeToTarget <= weaponRange[X])           // value less with far away targets
					ptr->heat = 5;
				else if (rangeToTarget <= weaponRange[X] * 2)
					ptr->heat = 4;
				else if (rangeToTarget <= weaponRange[X] * 3)
					ptr->heat = 3;
				else
					ptr->heat = 2;
			}
			// never use in these situations
			if (self->ammo_shells < 2) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_RAILGUN)
		{
			ptr->weapon = PR_WEAP_RAILGUN;
			ptr->impulse = 2;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 3;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				// check whether it's within a certain range of the ideal range (weaponRange_x)
				if (rangeToTarget <= weaponRange[X] + 400)
					ptr->heat = 3;
				else
					ptr->heat = 2; // value less with far away targets
			}
			// TODO: bot won't try to shoot at enemy through the walls, but if he would, this weapon would be more valuable when enemy behind wall
			// never use in these situations
			if (self->ammo_nails < 1) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_NAILGUN)
		{
			ptr->weapon = PR_WEAP_NAILGUN;
			ptr->impulse = 4;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 3;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				// check whether it's within a certain range of the ideal range (weaponRange_x)
				if (rangeToTarget <= weaponRange[X] + 400)
					ptr->heat = 3;
				else
					ptr->heat = 2; // value less with far away targets
			}
			// never use in these situations
			if (self->ammo_nails < 1) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_AUTO_RIFLE)
		{
			ptr->weapon = PR_WEAP_AUTO_RIFLE;
			ptr->impulse = 2;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 2;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				// check whether it's within a certain range of the ideal range (weaponRange_x)
				if (rangeToTarget >= weaponRange[X] - 200)
					ptr->heat = 3;
				else
					ptr->heat = 2; // we should probably switch weapons if target gets close
			}
			// never use in these situations
			if (self->ammo_shells < 1) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_SHOTGUN)
		{
			ptr->weapon = PR_WEAP_SHOTGUN;
			ptr->impulse = 2;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 3;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				// check whether it's within a certain range of the ideal range (weaponRange_x)
				if (rangeToTarget <= weaponRange[X] + 400)
					ptr->heat = 3;
				else
					ptr->heat = 2; // value less with far away targets
			}
			// never use in these situations
			if (self->ammo_shells < 1) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_MAUSER)
		{
			ptr->weapon = PR_WEAP_MAUSER;
			ptr->impulse = 5;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 5;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon); // for the end of the loop
				ptr->heat = 5;
			}
			if (self->undercover_team != 0 || self->undercover_skin != 0) // if we are disguised, mauser is great because it won't remove our disguise
				ptr->heat = 10;
			if (self->enemy->tfstate & PR_TFSTATE_TRANQUILISED)
				ptr->heat = ptr->heat + 2;
			// never use in these situations
			// need? -> if (self.enemy.solid == #SOLID_BSP) ptr.heat = 0; // won't open buttons/doors
			if (self->ammo_nails < 1) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_AIRF)
		{
			ptr->weapon = PR_WEAP_AIRF;
			ptr->impulse = 8;
			if (rangeToTarget == -1) // if we don't care about range to target
				ptr->heat = 0;        // then we probably are thinking about using it on a button (which won't work)
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				// check whether it's within a certain range of the ideal range (weaponRange_x)
				if (rangeToTarget <= 64) // '48 0 128'
					ptr->heat = 10;       // you are probably a scout trying to break through
				else
					ptr->heat = 5;        // value less with far away targets (of course, it won't use it at all if outside of its max range)
			}
			if (self->waterlevel > 2)
			{
				if (self->b_skill > PR_SKILL_EASY) ptr->heat = 0;
				else                            ptr->heat = 1;
			}
			if (self->enemy->modelindex == modelindex_eyes && self->b_skill > PR_SKILL_EASY) // use this weapon to test whether they are teammates
				ptr->heat = 15;
			// never use in these situations
			if (self->ammo_cells < PR_AIRFIST_CELLS_COST) ptr->heat = 0;
			if (self->enemy->solid == PR_SOLID_BSP) ptr->heat = 0; // can't use on buttons/doors
		}
		else if (weapons & PR_WEAP_TRANQ)
		{
			ptr->weapon = PR_WEAP_TRANQ;
			ptr->impulse = 2;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 3;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon);
				// check whether it's within a certain range of the ideal range (weaponRange_x)
				if (rangeToTarget <= weaponRange[X] + 400) // 180 0 99999
					ptr->heat = 3;
				else
					ptr->heat = 2; // value less with far away targets
			}
			// if we have a powerful melee weapon, tranquilizer is great
			if ((self->cutf_items & PR_CUTF_KNIFE) || (self->weapons_carried & PR_WEAP_MAUSER) || (self->weapons_carried & PR_WEAP_MEDIKIT))
				ptr->heat = ptr->heat + 2; // don't make it so high that he won't switch to the powerful melee weapon
			if (self->enemy->tfstate & PR_TFSTATE_TRANQUILISED)
				ptr->heat = ptr->heat - 2;
			// never use in these situations
			if (self->ammo_nails < 1) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_MEDIKIT)
		{
			ptr->weapon = PR_WEAP_MEDIKIT;
			ptr->impulse = 1;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 3;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon); // we'll use this at the end of the loop to check hard range limits
				ptr->heat = 5;
			}
			if (self->enemy->tfstate & PR_TFSTATE_TRANQUILISED)
				ptr->heat = ptr->heat + 2;
			if (self->enemy->modelindex == modelindex_eyes && self->b_skill > PR_SKILL_EASY) // use this weapon to test whether they are teammates
				ptr->heat = 15;
			// PZ: if a teammate's health is pretty low, heal him (select medikit)
			//if (self.enemy.tfstate & #TFSTATE_INFECTED)
			if (teamplay && isTeammateOrDisguisedAsTeammate)
				{ ptr->heat = 999999; forceBestWeapon = PR_TRUE; }
			// never use in these situations
			if (self->ammo_medikit < 1) ptr->heat = 0;
		}
		else if (weapons & PR_WEAP_SPANNER)
		{
			ptr->weapon = PR_WEAP_SPANNER;
			ptr->impulse = 1;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 1;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon); // we'll use this at the end of the loop to check hard range limits
				ptr->heat = 1;
			}
			if (self->enemy->flags & PR_FL_MACHINE)
				ptr->heat = 10; // we can dismantle with one hit
			if (self->enemy->modelindex == modelindex_eyes && self->b_skill > PR_SKILL_EASY) // use this weapon to test whether they are teammates
				ptr->heat = 15;
			// TODO: if a teammate's armor is pretty low, fix it (select spanner), if we have ammo_cells
			// TODO: for engineer build AI, use spanner
		}
		else if (weapons & PR_WEAP_AXE) // this case is also for |self.cutf_items & #CUTF_KNIFE|
		{
			ptr->weapon = PR_WEAP_AXE;
			ptr->impulse = 1;
			if (rangeToTarget == -1) // don't care about range to target
				ptr->heat = 1;
			else
			{
				weaponRange = frik_getRangeOfWeapon(ptr->weapon); // we'll use this at the end of the loop to check hard range limits
				ptr->heat = 1;
			}
			// if this is actually a knife, it's worth a lot more
			if (self->cutf_items & PR_CUTF_KNIFE)
			{
				ptr->heat = 5;
				// TODO: check to see whether enemy's back is facing me?
				if (self->enemy->tfstate & PR_TFSTATE_TRANQUILISED)
					ptr->heat = ptr->heat + 2;
			}
		}
		else if (weapons & PR_WEAP_HOOK) // don't select for right now; just handle it (TODO)
		{
			ptr->weapon = PR_WEAP_HOOK;
			ptr->heat = 0;
			if (self->enemy->modelindex == modelindex_eyes && self->b_skill > PR_SKILL_EASY) // use this weapon to test whether they are teammates
				ptr->heat = 15;
			// never use in these situations
			if (self->enemy->solid == PR_SOLID_BSP) ptr->heat = 0; // can't use on buttons/doors
		}
		else if (weapons & PR_WEAP_ZEROGRAVITY) // don't select for right now; just handle it (TODO)
		{
			ptr->weapon = PR_WEAP_ZEROGRAVITY;
			ptr->heat = 0;
			// never use in these situations
			if (self->enemy->solid == PR_SOLID_BSP) ptr->heat = 0; // can't use on buttons/doors
		}

		// One-time check here for range limits. If the weapon is useless at this range, make its value 0.
		if (rangeToTarget >= 0 && (rangeToTarget < weaponRange[Y] || rangeToTarget > weaponRange[Z]))   // y = min; z = max
		{
			// !! This should work, but it's so costly to processor for a function that runs a lot. Let's just set the value to 0.
			ptr->heat = 0; // this will keep this weapon from being selected
			// out of range; take back out of the list
			/*local entity removeMe;
			removeMe = ptr;
			if (ptr._prev) // not first in list
			{
				ptr = ptr._prev;
				ptr._next = world;
			}
			else
				firstWeapon = ptr = world;
			remove(removeMe);*/
		}

		if (self->current_weapon == ptr->weapon) valueOfCurrentWeapon = ptr->heat;

		// go to the next weapon in the bot's inventory
		weapons = weapons - ptr->weapon;
	}

//bprint(#PRINT_HIGH, self.netname, ": sorting list of weapons...\n");
	// [Pick the best weapon from our list of weapon values.]
/*
bprint(#PRINT_HIGH, self.netname, ": my weapon list, PRE sort : ");
ptr = firstWeapon;
while (ptr != world)
{
	local string temp;
	temp = ftos(ptr.weapon);
	bprint(#PRINT_HIGH, "( ", temp);
	temp = ftos(ptr.heat);
	bprint(#PRINT_HIGH, " ", temp, " ) ");
	ptr = ptr._next;
}
bprint(#PRINT_HIGH, "\n");*/

	// Sort list by value (.heat), highest value first. Bubble (swap) sort. If values are equal, 50% chance we swap them.
	ptr = firstWeapon;
	bool run = false;
	while (ptr != world)
	{
		if (ptr == firstWeapon) run = PR_FALSE;
		if (ptr->_next != world) // not last one
		{
			if      (ptr->_next->heat > ptr->heat)
			{
				firstWeapon = swapInWeaponList(ptr, ptr->_next, firstWeapon);
				run = PR_TRUE;
			}
			// PZ TODO: This isn't the best way to deal with weapons of equal value. It still biases towards weapons at
			// the end of the list, in the code, being at the end of this value list.
			else if (ptr->heat > 0 && ptr->_next->heat == ptr->heat && random() >= 0.5)
				firstWeapon = swapInWeaponList(ptr, ptr->_next, firstWeapon);
			else
				ptr = ptr->_next; // only if we didn't swap this time
		}
		else           // on last one in the list
		{
			if (run) ptr = firstWeapon;
			else ptr = world;
		}
	}

/*bprint(#PRINT_HIGH, self.netname, ": my weapon list, POST sort : ");
ptr = firstWeapon;
while (ptr != world)
{
	local string temp;
	temp = ftos(ptr.weapon);
	bprint(#PRINT_HIGH, "( ", temp);
	temp = ftos(ptr.heat);
	bprint(#PRINT_HIGH, " ", temp, " ) ");
	ptr = ptr._next;
}
bprint(#PRINT_HIGH, "\n");*/

	// Randomly decide how far down the list we will go to pick a weapon.
	// The higher the bot's skill, the less likely he is to go down the list.
	// The further down the list we go, the more rare of a random number chance.
	float descend = 0, randomNumber;
	randomNumber = random();
//bprint(#PRINT_HIGH, self.netname, ": selecting a weapon from the list...\n");
	if      (forceBestWeapon) descend = 0;
	else if (self->b_skill == PR_SKILL_NIGHTMARE) descend = 0; // NIGHTMARE is elite and cut-throat (will always select the best weapon)
	else if (self->b_skill == PR_SKILL_HARD)
	{
		if      (randomNumber <= 0.85) descend = 0;   // 85% chance he will go to the best weapon
		else if (randomNumber <= 0.95) descend = 1;   // 10% chance he will go to the second best weapon
		else                           descend = 2;   // 05% chance he will go to the third best weapon
	}
	else if (self->b_skill == PR_SKILL_MEDIUM)
	{
		if      (randomNumber <= 0.52) descend = 0;   // 52% chance he will go to the best weapon
		else if (randomNumber <= 0.82) descend = 1;   // 30% chance he will go to the second best weapon
		else                           descend = 2;   // 18% chance he will go to the third best weapon
	}
	else if (self->b_skill == PR_SKILL_EASY)
	{
		if      (randomNumber <= 0.33) descend = 0;   // 33% chance he will go to the best weapon
		else if (randomNumber <= 0.62) descend = 1;   // 29% chance he will go to the second best weapon
		else if (randomNumber <= 0.85) descend = 2;   // 23% chance he will go to the third best weapon
		else                           descend = 3;   // 15% chance he will go to the fourth best weapon
	}

	// select the weapon
	ptr = firstWeapon;
	while (ptr != world && descend > 0)
	{
		if (ptr->_next != world && ptr->_next->heat >= 1) // don't select a weapon of 0 value
		{
			ptr = ptr->_next;
			descend = descend - 1;
		}
		//else break; // I'm not sure that 'break' works in QuakeC
		else descend = 0;
	}
	if (ptr != world)
	{
		// only switch if the weapon we are thinking of switching to is more valuable right now than our current weapon
		if (onlyAllowBetterWeapon)
		{
			if (ptr->heat > valueOfCurrentWeapon)
				self->impulse = ptr->impulse; // PZ TODO: handle situations where an impulse is used by more than one weapon
		}
		else
			self->impulse = ptr->impulse; // PZ TODO: handle situations where an impulse is used by more than one weapon
	}

	// If we select a weapon, set up a timer entity that will continuously set the same impulse, until the desired weapon is selected.
	if (self->impulse == ptr->impulse)
	{
		//local entity timer;
		// if this bot already has a timer out, use that timer and supercede the previous weapon request
		/*timer = find(world, classname, "bot_weapon_selection_timer");
		while (timer != world)
		{
			if (timer.owner == self) break;  // PZ NOTE: if you ever use this code again, don't use the break statement.. it doesn't work
			timer = find(timer, classname, "bot_weapon_selection_timer");
		}
		if (!timer)*/
		if (self->PR_b_bot_weaponCycler == world)
			self->PR_b_bot_weaponCycler = spawnServerSide();
		self->PR_b_bot_weaponCycler->classname = "bot_weapon_selection_timer";
		self->PR_b_bot_weaponCycler->owner = self;
		self->PR_b_bot_weaponCycler->impulse = ptr->impulse;  // the impulse required to select weapon
		self->PR_b_bot_weaponCycler->weapon = ptr->weapon;    // the weapon we want
		self->PR_b_bot_weaponCycler->think = timerThink_botEnsureWeaponSelection;
		self->PR_b_bot_weaponCycler->nextthink = time + 0.25;
		self->PR_b_bot_weaponCycler->deadflag = PR_REMOVE_TIMER_ON_DISCONNECT | PR_REMOVE_TIMER_ON_DEATH; // when bot leaves server or dies, remove this timer
		self->PR_b_bot_weaponCycler->th_die = timerCleanup_botEnsureWeaponSelection; // when timer is auto removed, this cleanup function will run
	}

//bprint(#PRINT_HIGH, self.netname, ": deleting the list of weapons...\n");
	// Weapon is now selected. Clean up. Delete the list of weapons.
	ptr = firstWeapon;
	while (ptr != world)
	{
		if (ptr->_next != world) // not last one
		{
			ptr = ptr->_next;
			remove(ptr->_prev);
		}
		else           // last one
		{
			remove(ptr); ptr = world;
			// `firstWeapon` will get deallocated anyway; no need to set to world
		}
	}
}
// PZ: For frik_makeWeaponSelection().
// Swaps e1 and e2 in the weapons list, and returns the first weapon in the list.
entity swapInWeaponList(entity e1, entity e2, entity firstWeapon)
{
	entity e1_prev_backup;

	e1_prev_backup = e1->_prev;
	//e1_next_backup = e1._next;

	// let's go ahead and do any outer nodes first
	if (e1->_prev != world) e1->_prev->_next = e2;
	if (e2->_next != world) e2->_next->_prev = e1;

	e1->_prev = e2;
	e1->_next = e2->_next;

	e2->_prev = e1_prev_backup;
	e2->_next = e1;

	if      (firstWeapon == e1) firstWeapon = e2;
	else if (firstWeapon == e2) firstWeapon = e1;

	return firstWeapon;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_makeWeaponSelection     (PZ: was called "bot_weapon_switch")

Pick best weapon based on ammo and range.
PZ TODO: maybe make it so that it has a random chance that it won't pick the best weapon, especially for lower-skill bots

`rangeToTarget`: set to -1 if range isn't a concern
`self` = the current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// PZ: added CuTF weapons
// PZ TODO: what happens with weapons that use the same impulse and require multiple key presses to select them?
#ifdef PR_USE_OLD_ONE
void frik_makeWeaponSelection(float rangeToTarget)
{
	float weapons, weap, mpulse;
	vector range;

	//weapons = self.items & 127;
	weapons = self->weapons_carried;  // PZ: for CuTF

	mpulse = 1; // PZ: axe is last option

	// pick best weapon
	while (weapons)
	{
#ifdef PR_ORIGINAL_CODE
		if ((self->ammo_rockets >= 1) && (weapons & 32))                            // IT_ROCKET_LAUNCHER
		{
			weap = 32;
			mpulse = 7;
		}
		else if (self->waterlevel <= 1 && self->ammo_cells >= 1 && (weapons & 64))   // IT_LIGHTNING
		{
			weap = 64;
			mpulse = 8;
		}
		else if (self->ammo_nails >= 2 && (weapons & 8))                            // IT_SUPER_NAILGUN
		{
			weap = 8;
			mpulse = 5;
		}
		else if ((self->ammo_rockets >= 1) && (weapons & 16))                       // IT_GRENADE_LAUNCHER
		{
			weap = 16;
			mpulse = 6;
		}
		else if (self->ammo_shells >= 2 && (weapons & 2))                           // IT_SUPER_SHOTGUN
		{
			weap = 2;
			mpulse = 3;
		}
		else if (self->ammo_nails >= 1 && (weapons & 4))                            // IT_NAILGUN
		{
			weap = 4;
			mpulse = 4;
		}
		else if (self->ammo_shells >= 1 && (weapons & 1))                           // IT_SHOTGUN
		{
			weap = 1;
			mpulse = 2;
		}
#endif
		// PZ: copied and pasted this from W_BestWeapon(), replacing the original code which was basically the same
		// PZ TODO: change this so that each weapon has a rank; that way weapons with similar ranks can be randomly selected
		if (self->ammo_cells >= 1 && (weapons & PR_WEAP_LIGHTNING) && self->waterlevel <= 1)
		{
			weap = PR_WEAP_LIGHTNING;
			mpulse = 8;
		}
		else if (self->ammo_shells >= 1 && (weapons & PR_WEAP_SNIPER_RIFLE) )
		{
			weap = PR_WEAP_SNIPER_RIFLE;
			mpulse = 2;
		}
		else if (self->ammo_rockets >= 1 && (weapons & PR_WEAP_ROCKET_LAUNCHER) )
		{
			weap = PR_WEAP_ROCKET_LAUNCHER;
			mpulse = 7;
		}
		else if (self->ammo_cells >= 5 && (weapons & PR_WEAP_DAEDALUS) )
		{
			weap = PR_WEAP_DAEDALUS;
			mpulse = 8;
		}
		else if (self->ammo_cells >= 6 && (self->ammo_shells >= 1) && (weapons & PR_WEAP_ASSAULT_CANNON))
		{
			weap = PR_WEAP_ASSAULT_CANNON;
			mpulse = 7;
		}
		else if (self->ammo_nails >= 10 && (weapons & PR_WEAP_LIGHT_ASSAULT) )
		{
			weap = PR_WEAP_LIGHT_ASSAULT;
			mpulse = 5;
		}
		else if (self->ammo_rockets >= 3 && (weapons & PR_WEAP_INCENDIARY) )
		{
			weap = PR_WEAP_INCENDIARY;
			mpulse = 7;
		}
		else if (self->ammo_rockets >= 1 && (weapons & PR_WEAP_GRENADE_LAUNCHER) )
		{
			weap = PR_WEAP_GRENADE_LAUNCHER;
			mpulse = 6;
		}
		else if (self->ammo_cells >= 1 && (weapons & PR_WEAP_LASERCANNON))
		{
			weap = PR_WEAP_LASERCANNON;
			mpulse = 4;
		}
		else if (self->ammo_nails >= 2 && (weapons & PR_WEAP_SNG))
		{
			weap = PR_WEAP_SNG;
			mpulse = 5;
		}
		else if (self->ammo_cells >= 1 && (weapons & PR_WEAP_FLAMETHROWER))
		{
			weap = PR_WEAP_FLAMETHROWER;
			mpulse = 6;
		}
		else if (self->ammo_shells >= 2 && (weapons & PR_WEAP_SUPER_SHOTGUN) )
		{
			weap = PR_WEAP_SUPER_SHOTGUN;
			mpulse = 3;
		}
		else if (self->ammo_nails >= 1 && (weapons & PR_WEAP_RAILGUN) )
		{
			weap = PR_WEAP_RAILGUN;
			mpulse = 2;
		}
		else if (self->ammo_nails >= 1 && (weapons & PR_WEAP_NAILGUN) )
		{
			weap = PR_WEAP_NAILGUN;
			mpulse = 4;
		}
		else if (self->ammo_shells >= 1 && (weapons & PR_WEAP_SHOTGUN) )
		{
			weap = PR_WEAP_SHOTGUN;
			mpulse = 2;
		}
		else if (self->ammo_nails >= 1 && (weapons & PR_WEAP_MAUSER) )
		{
			weap = PR_WEAP_MAUSER;
			mpulse = 5;
		}
		else if (self->ammo_cells >= PR_AIRFIST_CELLS_COST && (weapons & PR_WEAP_AIRF))
		{
			weap = PR_WEAP_AIRF;
			mpulse = 8;
		}
		else if (self->ammo_nails >= 1 && (weapons & PR_WEAP_TRANQ) )
		{
			weap = PR_WEAP_TRANQ;
			mpulse = 2;
		}
		else if (weapons & PR_WEAP_MEDIKIT)
		{
			weap = PR_WEAP_MEDIKIT;
			mpulse = 1;
		}
		else if (weapons & PR_WEAP_SPANNER)
		{
			weap = PR_WEAP_SPANNER;
			mpulse = 1;
		}
		//else if (weapons & #WEAP_AXE)
		//{
		//	weap = #WEAP_AXE;
		//	mpulse = 1;
		//}
		// we are out of choices; we must select this one
		else
		{
			if (mpulse)
				self->impulse = mpulse;
			return;
		}

		// PZ TODO: sometimes pick a random weapon
		// sometimes randomly eliminate a weapon just so that you will sometimes use a 'lesser' weapon

		if (rangeToTarget == -1) // don't care about range to target
		{
			if (mpulse)
				self->impulse = mpulse;
			return;
		}
		// we do care about range; so pick the best weapon, based on range
		range = frik_getRangeOfWeapon(weap); // PZ TODO: don't just check min and max; look for weapon with ideal range closest to current range
		if (rangeToTarget < range[Y] || rangeToTarget > range[Z])   // y = min; z = max
			weapons = weapons - weap; // out of range; skip this weapon
		else
		{
			if (mpulse)
				self->impulse = mpulse;
			return;
		}
	}
}
#endif

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botFireWeaponIfShotGood  (PZ: was called "bot_shoot")

Fires the selected weapon.

`self` = the current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_botFireWeaponIfShotGood()
{
	// PZ: don't shoot if you're currently cycling through weapons to select a weapon that shares an impulse with another weapon
	if (self->PR_b_bot_weaponCycler != world) return;

	// PZ: Hack to get medics to heal teammates. Don't let them shoot teammates unless they have their medikit selected.
	float isTeammateOrDisguisedAsTeammate = PR_FALSE;
	if      (Teammate(self->team_no, self->enemy->team_no))         isTeammateOrDisguisedAsTeammate = PR_TRUE;
	else if (Teammate(self->team_no, self->enemy->undercover_team)) isTeammateOrDisguisedAsTeammate = PR_TRUE;

	if (teamplay && isTeammateOrDisguisedAsTeammate)
		if (self->current_weapon != PR_WEAP_MEDIKIT)
			{ self->PR_BUTTON_FIRE = PR_FALSE;  return; }  // check your fire, soldier

	// quick little function to stop making him shoot the wrong way ! Argh
	// PZ NOTE: only shoot if you are aiming near the target
	// (PZ: these limits are mainly here for SKILL_EASY, right? (NO, DON'T THINK SO) should probably adjust these for difficulty; sniper should go for more accuracy?)
// PZ TODO: (PUT THIS SOMEWHERE) let's not do this here; this is dirty; leaving here for right now, so that bot isn't degraded
	float accuracy = 30;   // lower is better (default for EASY skill level)
	switch (self->b_skill)
	{
		case PR_SKILL_MEDIUM:     accuracy = 17;  break;
		case PR_SKILL_HARD:       accuracy = 7;   break;
		case PR_SKILL_NIGHTMARE:  accuracy = 1;   break;
	}
	float g;
	g = frik_angleSubtraction(self->v_angle[X], self->b_commandedViewAngle[X]); // pitch
	if (fabs(g) > accuracy)
		return; // argh, too far away (PZ: as in angular distance, right?)
	g = frik_angleSubtraction(self->v_angle[Y], self->b_commandedViewAngle[Y]); // yaw
	if (fabs(g) > accuracy)
		return; // not again!
// PZ: END COMMENT

	// PZ: The goal here is to make skilled bots shoot at the enemy's feet, if they are using radial damage projectiles. So, if they miss, it will more than likely
	//     still damage the enemy, when it explodes on the ground.
	if (self->b_skill >= PR_SKILL_HARD && !(self->tfstate & PR_TFSTATE_RELOADING) && (self->origin[Z] + self->view_ofs[Z] + 20) >= (self->enemy->origin[Z] - 24) &&
	    (self->current_weapon == PR_WEAP_ROCKET_LAUNCHER || self->current_weapon == PR_WEAP_INCENDIARY || self->current_weapon == PR_WEAP_DAEDALUS) &&
	    self->enemy->solid != PR_SOLID_BSP) // don't jump when shooting buttons
	{
		if (self->flags & PR_FL_ONGROUND)
			frik_makeBotJump();
		else
		{
			// shoot but only once you are at least a certain height off the ground
			if (time >= self->timeLastJumped + 0.225 /*&& time <= self.timeLastJumped + 0.30*/)
			//if (!(self.tfstate & #TFSTATE_RELOADING))
				self->PR_BUTTON_FIRE = PR_TRUE;
		}
	}
	// otherwise, use old shooting code
	else
	{
		if (self->tfstate & PR_TFSTATE_RELOADING) return;

		self->PR_BUTTON_FIRE = PR_TRUE; // TODO FIXME !!! WHY DON"T SNIPERS SHOOT????

		// PZ: to shoot the sniper rifle, he has to let go of the fire button after pressing it
		if (self->current_weapon == PR_WEAP_SNIPER_RIFLE)
		{
			if (self->heat && (time - self->heat >= PR_WEAP_SNIPER_RIFLE_MAX_TIME)) // for right now, we will make him charge full shots
			if (self->b_skill < PR_SKILL_HARD || self->flags & PR_FL_ONGROUND) // no point in wasting your charge while in the air
				self->PR_BUTTON_FIRE = PR_FALSE;
		}
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botThrowHandGrenade

PZ: Throws a hand grenade.

`self` = the current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_botThrowHandGrenade()
{
	float dist, timeLeftOnGrenade, timeNeededToReachEnemy;

	// throw the grenade if it's about to blow up in our hands
	if (self->tfstate & PR_TFSTATE_GRENPRIMED)
	{
		timeLeftOnGrenade = PR_GR_PRIMETIME - (time - self->last_grenade_primed);
		if (timeLeftOnGrenade <= 1.0)
		{
			self->impulse = PR_TF_GRENADE_T;
			return;
		}
	}

	// don't throw grenades at buttons, but don't hold onto grenades that are cooking, either
	if (self->enemy->solid == PR_SOLID_BSP && !(self->tfstate & PR_TFSTATE_GRENPRIMED))
		return;

	// quick little function to stop making him shoot the wrong way ! Argh
	// PZ NOTE: only shoot if you are aiming near the target
	// (PZ: these limits are mainly here for SKILL_EASY, right? (NO, DON'T THINK SO) should probably adjust these for difficulty; sniper should go for more accuracy?)
// PZ TODO: (PUT THIS SOMEWHERE) let's not do this here; this is dirty; leaving here for right now, so that bot isn't degraded
	float g;
	g = frik_angleSubtraction(self->v_angle[X], self->b_commandedViewAngle[X]);
	if (fabs(g) > 30)
		return; // argh, too far away (PZ: as in angular distance, right?)
	g = frik_angleSubtraction(self->v_angle[Y], self->b_commandedViewAngle[Y]);
	if (fabs(g) > 30)
		return; // not again!
// PZ: END COMMENT

	// for priming
	if (!(self->tfstate & PR_TFSTATE_GRENPRIMED || self->tfstate & PR_TFSTATE_GRENTHROWING))
	{
		// determine whether to prime a grenade
		float boolTryToThrowGrenade;
		if (self->tfstate & PR_TFSTATE_RELOADING && random() <= 0.02) // always see if you have a grenade to throw while reloading
			boolTryToThrowGrenade = PR_TRUE;
		else                                    // if not reloading, you have a small chance of throwing a grenade
		{
			float chance;
			// [determine chance]
			// never realized until now just how few weapons require reloading (add any weapons that require reloading)
			if (self->current_weapon == PR_WEAP_SHOTGUN || self->current_weapon == PR_WEAP_SUPER_SHOTGUN || self->current_weapon == PR_WEAP_LIGHT_ASSAULT ||
			    self->current_weapon == PR_WEAP_GRENADE_LAUNCHER || self->current_weapon == PR_WEAP_ROCKET_LAUNCHER)
				chance = 0.01;// 0.01
			else
				chance = 0.01;
			if (random() <= chance)
				boolTryToThrowGrenade = PR_TRUE;
			else
				boolTryToThrowGrenade = PR_FALSE;
		}
		// prime a grenade
		if (boolTryToThrowGrenade)
		{
			// determine which grenade to prime
			float gren1_or_gren2;
			if      ( self->no_grenades_1 && !self->no_grenades_2) gren1_or_gren2 = 1;
			else if (!self->no_grenades_1 &&  self->no_grenades_2) gren1_or_gren2 = 2;
			else    /* have both */
			{
				if (random() <= 0.75) gren1_or_gren2 = 1;
				else                  gren1_or_gren2 = 2; // these tend to be spammy.. so, less chance for these
			}
			// prime it
			if (gren1_or_gren2 == 1)
				self->impulse = PR_TF_GRENADE_1;
			else
				self->impulse = PR_TF_GRENADE_2;
		}
	}
	// for throwing (CALTROPS ARE TRICKY, BECAUSE YOU CAN'T PRIME THEM. THEY ALWAYS EXPLODE ON BOTS)
	else if (self->tfstate & PR_TFSTATE_GRENPRIMED)
	{
		// throw it
		// Higher-skilled bots don't throw the grenade until the time left on the grenade <= the amount of time it will take
		// the grenade to make it to the enemy.
		// #define GR_PRIMETIME 3 (seconds) // .last_grenade_primed (time it was primed) // grenades fly at horizontal velocity of 600
		if (self->b_skill >= PR_SKILL_MEDIUM)
		{
			dist = vlen(self->enemy->origin - self->origin);
			timeLeftOnGrenade = PR_GR_PRIMETIME - (time - self->last_grenade_primed);
			timeNeededToReachEnemy = dist / 600; // in seconds; velocity, at least in Quake, is how far something can travel in 1 second
			// TODO: differentiate skill levels in timing the grenade; mess up timeNeededToReachEnemy and the time to ditch the grenade
			// at the last second, if not thrown yet, for MEDIUM and HARD
			if ((timeLeftOnGrenade <= timeNeededToReachEnemy) || (timeLeftOnGrenade <= 1.0)) // PZ: at 0.3, it was killing them a lot
				self->impulse = PR_TF_GRENADE_T;        // (remember that this code doesn't execute at a very high rate / timing resolution, either)
		}
		else
			self->impulse = PR_TF_GRENADE_T;
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_engageTheEnemy   (PZ: was named "bot_fight_style")

This is the core of the bot's thinking when
attacking the `.enemy`.

Handles shooting and moving while engaging `.enemy`.
The bot has a fight mode and a pursuit mode. In fight
mode, he basically ignores waypoints and attacks
`.enemy`. In pursuit mode, he mostly uses waypoints,
to seek something out.

Bot can attack a target (.enemy) while pursuing
another entity (his .target1).

This also handles shooting buttons and other such
triggers. They become his .enemy, temporarily.

`.enemy` = the bot's currently-acquired target
`self` = the bot that we are directing

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// PZ TODO: make him intelligently reload here
// Is there a better way to reload when in a fight? Perhaps instead of reloading, switch weapons. It requires the ability to count shots left,
// which a higher-skilled bot would have. The game should allow you to change weapons while reloading to avoid reloading in the middle of a fight,
// but I don't think CuTF has ever allowed this. It would either partially reload at that point, or it would cancel reload altogether, in case of
// reloading a clip.
void frik_engageTheEnemy()
{
	vector v, v1, v2, positionOfEnemy, distForGren;
	float distanceToFoe = 0, strengthOfSelf, strengthOfEnemy, stopPursuingEnemy;

	float isEnemyTeammateOrDisguisedAsTeammate = PR_FALSE;
	if      (Teammate(self->team_no, self->enemy->team_no))         isEnemyTeammateOrDisguisedAsTeammate = PR_TRUE;
	else if (Teammate(self->team_no, self->enemy->undercover_team)) isEnemyTeammateOrDisguisedAsTeammate = PR_TRUE;

//traceon();

	// [Determine whether we should stop pursuing the enemy, and stop pursuing if needed.]
	stopPursuingEnemy = PR_FALSE;
	if      (self->enemy == world)                                                     stopPursuingEnemy = PR_TRUE;
//CHECK_THAT_THE_FOLLOWING_LINE_IS_GOOD // and check that everything in this function is good for medics, getting them to heal and stop healing and such
	else if (self->enemy->health <= 0 && !(self->weapons_carried & PR_WEAP_MEDIKIT && teamplay && Teammate(self->team_no, self->enemy->team_no)))      stopPursuingEnemy = PR_TRUE;
	else if (self->enemy->health >= (self->enemy->max_health + PR_WEAP_MEDIKIT_OVERHEAL) && teamplay && Teammate(self->team_no, self->enemy->team_no)) stopPursuingEnemy = PR_TRUE;
	else if (!self->enemy->takedamage && self->enemy->classname != "holo")             stopPursuingEnemy = PR_TRUE;
	// PZ FIXME: This isn't good because bots lose their targets about as soon as the target is no longer visible. Bot never chases a target around
	//           corners, hardly. Maybe we could make bots go to the location they last saw the player and look again for him.
	//           So, create a temporary entity to put on the target stack -- the last seen location of the enemy. Once there, look again for the enemy.
	//           If you can't then find him, give up. Nightmare bots should just get a route to the enemy.
	else if (!frik_isVisible_Complex(self->enemy))                                     stopPursuingEnemy = PR_TRUE;
	// stop pursuing the enemy
	if (stopPursuingEnemy)
	{
		self->enemy = world;
		if (self->b_pursuingEnemy) // PZ: Trying to make bot not run into walls so much. (10-26-15)
		{
			self->keys = self->keys & PR_KEY_MASK_LOOKING_KEYS; // stop movement
			self->b_pursuingEnemy = PR_FALSE;
		}
		return;
	}

	// [Grab some data.]
	positionOfEnemy = frik_getRealOriginOfEntity(self->enemy);
	makevectors(self->v_angle);

	// [Fire weapon or throw grenade if you can.]

	// shoot if in range, otherwise, switch weapons if you can
	// PZ: Trying to get the medic to switch to the damn axe when he's trying to heal teammates.
	if (random() <= 0.20) // don't run too often
		frik_makeWeaponSelection(/*distanceToFoe*/-1, PR_TRUE);  // only allow switching weapons if there is a better weapon
	// PZ: END
	distanceToFoe = vlen(positionOfEnemy - self->origin);
	v = frik_getRangeOfWeapon(self->current_weapon);    // PZ: changed .weapon to .current_weapon
	if (distanceToFoe >= v[Y] && distanceToFoe <= v[Z])  // PZ: added ='s
	{
		/*if (random() <= 0.05) // don't run too often
			frik_makeWeaponSelection(distanceToFoe, #TRUE);  // only allow switching weapons if there is a better weapon
		else*/
		{
			traceline(self->origin + self->view_ofs, self->origin + self->view_ofs + v_forward * v[Z], PR_TL_ANY_SOLID, self);
			if (vlen(trace_endpos - (self->origin + self->view_ofs)) >= v[Y])
			{
				// try to avoid teammates that are in the line of fire
				if (trace_ent->classname == "player")
				{
					//if ((trace_ent.team_no == self.team_no && teamplay) || coop)                  // PZ: TF uses .team_no rather than .team
					// PZ: try not to hit enemies disguised as our teammate, too
					float isTeammateOrDisguisedAsTeammate = PR_FALSE;
					if      (Teammate(self->team_no, trace_ent->team_no))         isTeammateOrDisguisedAsTeammate = PR_TRUE;
					else if (Teammate(self->team_no, trace_ent->undercover_team)) isTeammateOrDisguisedAsTeammate = PR_TRUE;

					if ((teamplay && isTeammateOrDisguisedAsTeammate) || coop)
						if (!(self->weapons_carried & PR_WEAP_MEDIKIT))
							return;
				}
				// PZ TODO: add aiming code from frik_botFireWeaponIfShotGood() here?? probably not; need to keep the aiming code in one place as much as possible
				frik_botFireWeaponIfShotGood();
			}
		}
	}
	else
		frik_makeWeaponSelection(distanceToFoe, PR_FALSE);

	// PZ: throw hand grenade, if we have one, and we're in range
	if (self->no_grenades_1 || self->no_grenades_2 || self->tfstate & PR_TFSTATE_GRENPRIMED)
	{
		// PZ: In case our .enemy is a teammate, don't throw nades (can be a teammate if we are trying to 'attack' it with a helpful weapon).
		if (!(teamplay && isEnemyTeammateOrDisguisedAsTeammate))
		{
			//distanceToFoe = vlen(positionOfEnemy - self.origin); // PZ: we already computed this above (took out 10-4-15)
			distForGren = frik_getRangeOfWeapon(PR_WEAP_GRENADE_LAUNCHER);   // probably about the same range as a hand grenade
			if (distanceToFoe >= distForGren[Y] && distanceToFoe <= distForGren[Z])    // inbetween min and max range?
			{
				traceline(self->origin + self->view_ofs, self->origin + self->view_ofs + v_forward * distForGren[Z], PR_TL_ANY_SOLID, self);
				if (vlen(trace_endpos - (self->origin + self->view_ofs)) >= distForGren[Y])
				{
					// try to avoid teammates that are in the line of fire
					if (trace_ent->classname == "player")
					{
						//if ((trace_ent.team_no == self.team_no && teamplay) || coop)                  // PZ: TF uses .team_no rather than .team
						// PZ: try not to hit enemies disguised as our teammate, too
						float isTeammateOrDisguisedAsTeammate;                isTeammateOrDisguisedAsTeammate = PR_FALSE;
						if      (Teammate(self->team_no, trace_ent->team_no))         isTeammateOrDisguisedAsTeammate = PR_TRUE;
						else if (Teammate(self->team_no, trace_ent->undercover_team)) isTeammateOrDisguisedAsTeammate = PR_TRUE;

						if ((teamplay && isTeammateOrDisguisedAsTeammate) || coop)
							return;
					}
					// PZ TODO: add aiming code from frik_botFireWeaponIfShotGood() here?? probably not; need to keep the aiming code in one place as much as possible
					frik_botThrowHandGrenade();
				}
			}
		}
	}

	// [Determine whether to change your movement direction while fighting, and if so, change your direction.]
	// PZ NOTE: It appears that if it returns and doesn't choose a direction, the bot just keeps following waypoints.

	// PZ: We don't care about strength differences if this is a teammate (for beneficial 'attacks', like medic).
	if (!(teamplay && isEnemyTeammateOrDisguisedAsTeammate))
	{
		strengthOfEnemy = frik_sizeUpPlayer(self->enemy);
		//strengthOfSelf = frik_sizeUpPlayer(self) + 5;               // PZ: make this 5 (bravery) instead according to bot skill
		strengthOfSelf = frik_sizeUpPlayer(self) + self->b_skill * 5;  // PZ TODO: introduce randomness? not all bad players will run away

		// if enemy is stronger than me, don't chase them
		if (strengthOfEnemy > strengthOfSelf)
		{
			if (teamplay)
			{
				if (random() < 0.002) // PZ: was 0.02
				{
					/*frik_botConsiderStartingConversation(#TOPIC_SELF_INFORMS_TEAM_NEED_BACKUP, 0.2);
					self.b_chattime = 1;*/ // PZ NOTE: make bot speak immediately, without simulating typing
				}
			}
			return;
		}
		// otherwise, if I'm just too weak to be chasing anyone, don't chase
		if (strengthOfSelf < 140)
			return;
	}
	// PZ: If I have the flag, capture it! Don't chase enemies. TODO: Maybe dumb EASY bots should still sometimes chase enemies.
	// YEAH, THIS FLAG DOES WORK FOR BOTS. ######
	if (self->remove_spawnpoint) // TODO: DOES THIS GET SET WHEN BOT GETS FLAG??????? IT'S NOT WORKING. DOES PHYS_OBJ GET THIS FLAG INSTEAD?
		return;
	// PZ: Don't allow cappers to pursue enemies.
	if (isVectorBitFlagSet(PR_ROLE_CAPPER, V({0, 0, 0}), self->b_botRole))
		return;

	if (!(self->b_aiflags & (PR_AI_PRECISION | PR_AI_BLIND | PR_AI_HANDLING_OBSTRUCTION))) // PZ NOTE: if none of these AI flags are set for this bot
	{
		/*float oldKeys; // PZ
		oldKeys = self->keys;*/

		// otherwise, if there is a hazardous object that I need to avoid, move away from the object(?)
		// PZ TODO: LOOK CLOSELY AT THIS; this is likely why the bot rarely avoids; this doesn't execute if one of the two conditions above does)
		if (0 /*self.b_entityToAvoid*/) // PZ: they currently don't detect objects to avoid (like nades)
		{
			if (self->b_entityToAvoid->velocity != V({0, 0, 0}))
				v = self->b_entityToAvoid->velocity;
			else
				v = normalize(self->b_entityToAvoid->origin - self->origin);
			v1[X] = v[Y] * -1;
			v1[Y] = v[X];
			v2[X] = v[Y];
			v2[Y] = v[X] * -1;
			distanceToFoe = vlen(self->b_entityToAvoid->origin - (self->origin + v1));
			if (distanceToFoe < vlen(self->b_entityToAvoid->origin - (self->origin + v2)))
				frik_botMoveThisWay(v2);
			else
				frik_botMoveThisWay(v1);
		}
		// Otherwise, if the enemy is not a monster, position yourself at the ideal range to the enemy, given your current weapon.
		else /*if (!(self.enemy.flags & #FL_MONSTER))*/  // PZ: inner parens weren't there; I think they need to be (why are we even making the distinction of enemy is monster??)
		{
			// PZ: [Decide whether to pursue or not. We don't want them to pursue enemies so much because then they neglect the flag too much. (10-4-15)]
			// We don't want the bots to quickly change their minds back and forth about pursuing enemies. So, we make it toggle based on a low chance.
			if (teamplay && isEnemyTeammateOrDisguisedAsTeammate)  // Hack for medic and such.
				self->b_pursuingEnemy = PR_TRUE;
			else if (random() <= 0.02)
				self->b_pursuingEnemy = !self->b_pursuingEnemy;

			if (self->b_pursuingEnemy)
			{
				self->b_routeInvalid = PR_TRUE; // next time they pursue something using a route, they need to recompute the route, because bot may have moved too far off route
				// PZ NOTE: The offset number to the distance is a buffer that keeps the bots from sticking too rigidly to the exact ideal range.
				//          I changed them from 32 to 64, on 10-27-15. Decided to make it a variable, because sometimes we need the bots to be exact, like with melee weapons.
				float bufferZone;
				if (v[X] == 0) bufferZone = 0;  // we need bots to get as close as possible with melee weapons
				else          bufferZone = 64; // WARNING: Should this be only if v_x is at least 64?
				if (distanceToFoe + bufferZone < v[X])           // closer than ideal range
				{
					frik_botMoveThisWay(self->origin - positionOfEnemy); /*local string teststr;
					teststr = "i'm moving back. ";
					local string obstr;
					obstr = ftos(self.b_aiflags & #AI_HANDLING_OBSTRUCTION);
					teststr = strcat(teststr, "my obstruction flag: "); teststr = strcat(teststr, obstr);
					teststr = strcat(teststr, "\n"); frik_botSay(teststr);*/
				}
				else if (distanceToFoe - bufferZone > v[X])      // further than ideal range
				{
					frik_botMoveThisWay(positionOfEnemy - self->origin); /*local string teststr;
					teststr = "i'm moving forward. ";
					local string obstr;
					obstr = ftos(self.b_aiflags & #AI_HANDLING_OBSTRUCTION);
					teststr = strcat(teststr, "my obstruction flag: "); teststr = strcat(teststr, obstr);
					teststr = strcat(teststr, "\n"); frik_botSay(teststr);*/
				}
				// PZ NOTE: Bot is within a good range. So, it circle strafes, instead.
				// PZ: Easy bot shall not circle-strafe. Medium bot shall sometimes.
				else if (self->b_skill >= PR_SKILL_HARD || (self->b_skill == PR_SKILL_MEDIUM && (random() <= 0.10)))
				{
					// PZ NOTE: When .wallhug is set, we strafe right. Otherwise, we go left.
					if (random() <= 0.10) // PZ: added toggle here (10-4-15)
						self->wallhug = !self->wallhug;
					if (self->wallhug)                      // PZ NOTE: what are we doing if at ideal range, just strafing left or right?
						frik_botMoveThisWay(v_right);      //          I'm guessing this is for circle strafing.
					else                                   // PZ TODO: he should probably strafe left and right while moving forward and back,
						frik_botMoveThisWay(v_right * -1); //          to make himself a harder target, especially when fighting a sniper.
				}
			}
		}
		// Drop our current goal target if movement was taken over above, so that once target1 pursuit is restored, we won't try to continue our
		// route from the current random location.
		/*if (self.keys != oldKeys)
		{
			self.b_pursuingEnemy = #TRUE;
			local entity goalTarget;
			goalTarget = frik_getGoalTarget();
			frik_removeFromTargetStack(goalTarget);
		}*/
		// The above didn't work well. Bots would be heading for enemy base, shoot a button to go through a door, drop their current goal target,
		// and then turn around and head back to base. Was terrible on well6. Instead, we now keep the target and just get a new route, once we
		// start pursuing it again.
		/*if (self.keys != oldKeys) // moved this up on 10-4-15
		{
			self.b_pursuingEnemy = #TRUE;
			self.b_routeInvalid = #TRUE;
		}*/
	}
	// bot's movement needs to be precise; PZ NOTE: what this is doing exactly, though, I'm not sure; I think it's making the bot not pursue
	// its enemy if it needs to be careful about running into lava such
	else
	{
		// PZ: Took the below out, because it's done above. (10-4-15)
		/*strengthOfEnemy = frik_sizeUpPlayer(self.enemy);
		//strengthOfSelf = frik_sizeUpPlayer(self) + 5;               // PZ: make this 5 (bravery) instead according to bot skill
		strengthOfSelf = frik_sizeUpPlayer(self) + self.b_skill * 5;  // PZ TODO: introduce randomness? not all bad players will run away

		if (strengthOfEnemy > strengthOfSelf)
			return;
		else if (strengthOfSelf < 140)
			return;*/
		// I guess this means the bot stops moving and engages the enemy without running towards his .target1.
		self->keys = self->keys & PR_KEY_MASK_LOOKING_KEYS;    // turns off all but looking keys (TODO: why not be able to continue moving towards its target1?)
	}
//traceoff();
}

} // END namespace Progs
