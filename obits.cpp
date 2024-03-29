#include "progs.h"
#include "tfdefs.h"
#include "coop_defs.h"
#include "idmonsters/common.h"
#include "optimize.h"
#include "cpstuff.h"
#include "custom.h"
#include "grunty.h"
#include "guerilla.h"
#include "warlock.h"
#include "neo.h"

namespace Progs {

/*
===========
ClientObituary

called when a player dies
============
*/
void ClientObituary(entity targ, entity attacker)
{
	float rnum;
	string deathstring, deathstring2;
	string tst;
	//local float attackerteam, targteam;
	//local entity te;
	//local entity oself;

#ifdef PR_COOP_MODE_ENHANCED
	// Gizmo - don't bother in single player
	if ( !deathmatch && !coop )
	if ( COOP_IsCoopMonster( targ ) )
		return;
#endif

	// FIXME:

	//- OfN - Unidentified kill ---------//
	deathstring=" is killed by ";
	deathstring2="\n";
	// I still have no idea when this is used - OfN martyr does
	//-----------------------------------//

	if (prematch >= time)
		return;

	// Neo mode: handles death events
	if (neo.isModeActive()) neo.deathThink(targ, attacker);
	
	rnum = random();

#ifdef PR_COOP_MODE_ENHANCED
	if ( targ->classname == "player" || COOP_IsCoopMonster( targ ) ) {
#else
	if (targ->classname == "player") {
#endif
		//WK No Obituary for the cursed!
		if (targ->penance_time > time - 1)
		{
			Give_Frags_Out(targ, targ, -1, 0, 1, 1, 0);
/*			targ.real_frags = targ.real_frags - 1;
			if (!(toggleflags & #TFLAG_TEAMFRAGS))
				targ.frags = targ.real_frags;
*/			return;
		}

#ifndef PR_NO_ZEROGRAVGUN
		// Gizmo - if we were teamkilled by a friendly engy using his teleporter
		if ( targ->PR_teleporter_teamkiller ) {
			if ( ( attacker == world && targ->watertype == PR_CONTENT_LAVA ) || ( attacker == targ && deathmsg == PR_DMSG_TELEOUTWORLD ) ) {
				deathmsg = 0;		// prevent any possible incorrect teamkill messages
				attacker = targ->PR_teleporter_teamkiller;
			}

			// we've died for other reasons or the above teamkill was successful so make sure we reset it
			targ->PR_teleporter_teamkiller = world;
		}
#endif

		// Gizmo - added world, easier that way since all entities are init'd to it
#ifdef PR_COOP_MODE_ENHANCED
		if (targ->martyr_enemy != targ && targ->martyr_enemy != world)
#else
		if (targ->martyr_enemy != targ)
#endif
			deathmsg = targ->stored_deathmsg;

		if (deathmsg==PR_DMSG_CYBERNET)
		{
			tst=GetBuildingName(attacker);

			if (rnum < 0.4)
			{
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " will abort the hack next time\n");
			}
			else if (rnum < 0.7)
			{
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " has been lost in the cybernet!\n");
			}
			else
			{
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " wastes his life hacking a ");
				bprint (PR_PRINT_MEDIUM, tst);
				bprint (PR_PRINT_MEDIUM, "\n");
			}

			Give_Frags_Out(targ, targ, -1, 0, 1, 1, 0);

			return;
		}

		if (deathmsg==PR_DMSG_ANTIHACK)
		{
			tst=GetBuildingName(attacker);

			if (rnum < 0.5)
			{
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " is electrocuted trying to hack ");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s ");
				bprint (PR_PRINT_MEDIUM, tst);
				bprint (PR_PRINT_MEDIUM, "\n");
			}
			else
			{
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " should've never tried to hack ");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s ");
				bprint (PR_PRINT_MEDIUM, tst);
				bprint (PR_PRINT_MEDIUM, "\n");
			}

			Give_Frags_Out(targ, targ, -1, 0, 1, 1, 0);

			return;
		}

		if (deathmsg==PR_DMSG_ANTIDISM)
		{
			tst=GetBuildingName(attacker);

			if (rnum < 0.5)
			{
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " tries to dismantle ");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s ");
				bprint (PR_PRINT_MEDIUM, tst);
				bprint (PR_PRINT_MEDIUM, " with the wrong tool\n");
			}
			else
			{
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " is electrocuted trying to dismantle ");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s ");
				bprint (PR_PRINT_MEDIUM, tst);
				bprint (PR_PRINT_MEDIUM, "\n");
			}

			Give_Frags_Out(targ, targ, -1, 0, 1, 1, 0);

			return;
		}

		//- OfN - fish death stuff
		if (attacker->classname == "monster_fish")
		{
			//custom_demon_name(attacker); //CH
			//If our own demon killed us, kill demon and cost 2 frags
			if (attacker->real_owner == targ) {
				Give_Frags_Out(attacker->real_owner, attacker->real_owner, -1, 0, 1, 1, 0);
																		//-2
				bprint (PR_PRINT_MEDIUM, "The piranha ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " eats his owner, "); //lol
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, "\n");

				//kill_my_demons(); //Demon dies too
				return;

			}
			else if ((!Teammate(targ, attacker->real_owner)) || attacker->real_owner->team_no < 1)
			{
				if ( rnum > 0.6)
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " was eaten by the piranha ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ")\n");
				}
				else if (rnum > 0.3)
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " didn't survive ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, "'s (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ") bites\n");

				}
				else
				{
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s piranha, ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, ", eats ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, "\n");
				}

				Give_Frags_Out(attacker->real_owner, attacker->real_owner, 1, 0, 1, 1, 0);

				return;
			}
			else if (Teammate(targ, attacker->real_owner))
			{
				if (rnum < 0.5)
				{
					bprint (PR_PRINT_MEDIUM, "The supposed friendly piranha ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ")");
					bprint (PR_PRINT_MEDIUM, " eats ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, "\n");
				}
				else
				{
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ") proves that piranhas are able to teamkill with ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, "'s help\n");
				}
				//Give_Frags_Out(attacker.real_owner, attacker.real_owner, -1, 0, 1, 1, 0);

				return;
			}
		}


		//- OfN - Wizard death stuff
		if (attacker->classname == "monster_wizard")
		{
			//custom_demon_name(attacker); //CH
			//If our own demon killed us, kill demon and cost 2 frags
			if (attacker->real_owner == targ) {
				Give_Frags_Out(attacker->real_owner, attacker->real_owner, -1, 0, 1, 1, 0);
																		//-2
				bprint (PR_PRINT_MEDIUM, "The scrag ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " kills his owner, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, "\n");

				//kill_my_demon(); //Demon dies too
				return;

			}
			else if ((!Teammate(targ, attacker->real_owner)) || attacker->real_owner->team_no < 1)
			{
				if ( rnum > 0.6)
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " was nullified by the scrag ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ")\n");
				}
				else if (rnum > 0.3)
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " didn't survive ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, "'s (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ") acid\n");

				}
				else
				{
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s scrag, ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, ", removes ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, "\n");
				}

				Give_Frags_Out(attacker->real_owner, attacker->real_owner, 1, 0, 1, 1, 0);

				return;
			}
			else if (Teammate(targ, attacker->real_owner))
			{
				if (rnum < 0.5)
				{
					bprint (PR_PRINT_MEDIUM, "The supposed friendly scrag ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ")");
					bprint (PR_PRINT_MEDIUM, " killed ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, "\n");
				}
				else
				{
					//bprint (#PRINT_MEDIUM, "");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ") proves that scrags are able to teamkill with ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, "'s help\n");
				}
				//Give_Frags_Out(attacker.real_owner, attacker.real_owner, -1, 0, 1, 1, 0);

				return;
			}
		}

		//WK
		if (attacker->classname == "monster_demon1")
		{
			//custom_demon_name(attacker); //CH
			//If our own demon killed us, kill demon and cost 2 frags
			if (attacker->real_owner == targ) {
				Give_Frags_Out(attacker->real_owner, attacker->real_owner, -1, 0, 1, 1, 0);
																	   //-2
/*				attacker.real_owner.real_frags = attacker.real_owner.real_frags - 2;
				if (!(toggleflags & #TFLAG_TEAMFRAGS))
					attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
				if (deathmsg == PR_DMSG_DEMON_FIRE)
				{
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " threw his rage to his owner ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, "\n");
				}
				else
				{
					if (rnum < 0.5) {
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " exacts revenge on its summoner, ");
						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, "\n");
					}
					else {
						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, " is dragged into the pit by his own demon, ");
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, "\n");
					}
				}

				//kill_my_demon(); //Demon dies too
				return;
			}
			if (deathmsg == PR_DMSG_DEMON_FIRE)
			{
				if (rnum < 0.2) {
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " was burned alive by ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ")\n");
				}
				else if (rnum < 0.5) {
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " thought he was safe from ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ")\n");
				}
				else if (rnum < 0.75) {
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " died from ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, "'s (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ") fire balls\n");
				}
				else {
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s demon, ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, ", burned ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " to a crisp\n");
				}
			}
			else
			{
				if (rnum < 0.1) {
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " was eviscerated by ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ")\n");
				}
				else if (rnum < 0.50) {
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " was ripped limb from limb by ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ")\n");
				}
				else {
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s demon, ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					if (rnum < 0.75)
						bprint (PR_PRINT_MEDIUM, ", claws apart ");
					else
						bprint (PR_PRINT_MEDIUM, ", rends apart ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, "\n");
				}
			}

			if (!Teammate(targ, attacker->real_owner) || attacker->real_owner->team_no < 1)
				Give_Frags_Out(attacker->real_owner, attacker->real_owner, 1, 0, 1, 1, 0);

/*				attacker.real_owner.real_frags = attacker.real_owner.real_frags + 1;
			if (!(toggleflags & #TFLAG_TEAMFRAGS))
				attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
			return;
		}

		// Gremlin death stuff
		if (attacker->classname == "monster_gremlin")
		{
			if (attacker->real_owner == targ)
			{
				Give_Frags_Out(attacker->real_owner, attacker->real_owner, -1, 0, 1, 1, 0);
																	   //-2
/*				attacker.real_owner.real_frags = attacker.real_owner.real_frags - 2;
				if (!(toggleflags & #TFLAG_TEAMFRAGS))
					attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " is killed by his own gremlin ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, "\n");
				return;
			}

			if (Teammate(targ, attacker->real_owner))
			{
				bprint (PR_PRINT_MEDIUM, "The gremlin ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);

				if (rnum < 0.25)
					bprint (PR_PRINT_MEDIUM, " mows down a teammate by killing ");
				else if (rnum < 0.50)
					bprint (PR_PRINT_MEDIUM, " just made a strange noise after killing ");
				else if (rnum < 0.75)
					bprint (PR_PRINT_MEDIUM, " gets a frag for the other team by killing ");
				else
					bprint (PR_PRINT_MEDIUM, " decreases teammate count by killing");

				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, "\n");

				return;
			}

			if (deathmsg == PR_DMSG_SHOTGUN)
			{
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " is bumped off by the shotgun of the gremlin ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else if (deathmsg == PR_DMSG_NAILGUN)
			{
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " is nailed by the gremlin ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, "'s (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else if (deathmsg == PR_DMSG_ROCKETL)
			{
				bprint (PR_PRINT_MEDIUM, "The gremlin ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") puts ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " away with a rocket\n");
			}
			else if (deathmsg == PR_DMSG_SSHOTGUN)
			{
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " takes 2 loads of ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, "'s (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") buckshot\n");
			}
			else if (deathmsg == PR_DMSG_LIGHTNING)
			{
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " is deadly shocked by the gremlin "); //electrified
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else // default death and meele
			{
				bprint (PR_PRINT_MEDIUM,targ->netname);
				bprint (PR_PRINT_MEDIUM," is outsmarted by ");
				bprint (PR_PRINT_MEDIUM,attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM,"'s gremlin, ");
				bprint (PR_PRINT_MEDIUM,attacker->netname);
				bprint (PR_PRINT_MEDIUM,"\n");
			}

			if (!Teammate(targ, attacker->real_owner) || attacker->real_owner->team_no < 1)
			{
				Give_Frags_Out(attacker->real_owner, attacker->real_owner, 1, 0, 1, 1, 0);
				Give_Frags_Out(attacker, attacker, 1, 0, 1, 1, 0);
			}
/*				attacker.real_owner.real_frags = attacker.real_owner.real_frags + 1;
			if (!(toggleflags & #TFLAG_TEAMFRAGS))
				attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
			return;
		}

		// Grunt demon death stuff

		if (attacker->classname == "monster_army") //SB-1 Tech - Share and Enjoy!
		{
		//custom_demon_name(attacker); //CH
		GetRank(attacker); // Sponsored by SB-1 Tech
			//If our own demon killed us, kill demon and cost 2 frags
			if (attacker->real_owner == targ)
			{
				Give_Frags_Out(attacker->real_owner, attacker->real_owner, -1, 0, 1, 1, 0);
																	   //-2
/*				attacker.real_owner.real_frags = attacker.real_owner.real_frags - 2;
				if (!(toggleflags & #TFLAG_TEAMFRAGS))
					attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
				/*if (rnum < 0.5)
				{
					bprint (#PRINT_MEDIUM, attacker.undercover_name);
					bprint (#PRINT_MEDIUM, attacker.netname);
					bprint (#PRINT_MEDIUM, " greets his client ");
					bprint (#PRINT_MEDIUM, targ.netname);
					bprint (#PRINT_MEDIUM, " with a bullet in the back of the head\n");
				}
				else
				{*/
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " is killed by his own mercenary ");
					bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, "\n");
				//}
				return;
			}

			if (Teammate(targ, attacker->real_owner))
			{
				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);

				if (rnum < 0.25)
					bprint (PR_PRINT_MEDIUM, " mows down a teammate by killing ");
				else if (rnum < 0.50)
					bprint (PR_PRINT_MEDIUM, " checks his glasses after killing ");
				else if (rnum < 0.75)
					bprint (PR_PRINT_MEDIUM, " gets a frag for the other team by killing ");
				else
					bprint (PR_PRINT_MEDIUM, " loses another friend by killing ");

				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, "\n");

				return;
			}
			if (deathmsg == PR_DMSG_AXE)
			{
				if (attacker->cutf_items & PR_CUTF_KNIFE)
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " is sliced and diced by ");
					bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, "'s (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ") blade\n");
				}
				else
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " is split in two by a powerful axe blow from ");
					bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ")\n");
				}
			}
			else if (deathmsg == PR_DMSG_BACKSTAB)
			{
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " is filleted by ");
				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else if (deathmsg == PR_DMSG_SHOTGUN)
			{
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " practices being ");
				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, "'s (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") clay pigeon\n");
			}
			else if (deathmsg == PR_DMSG_NAILGUN)
			{
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " is turned into ");
				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, "'s (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") pincushion\n");
			}
			else if (deathmsg == PR_DMSG_ROCKETL)
			{
				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " declares that ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " IS the rocket man\n");
			}
			else if (deathmsg == PR_DMSG_SSHOTGUN)
			{
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " is turned into swiss cheese by ");
				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, "'s (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") buckshot\n");
			}

			if (!Teammate(targ, attacker->real_owner) || attacker->real_owner->team_no < 1)
			{
				Give_Frags_Out(attacker->real_owner, attacker->real_owner, 1, 0, 1, 1, 0);
				Give_Frags_Out(attacker, attacker, 1, 0, 1, 1, 0);
			}
/*				attacker.real_owner.real_frags = attacker.real_owner.real_frags + 1;
			if (!(toggleflags & #TFLAG_TEAMFRAGS))
				attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
			return;
		}

		if (attacker->classname == "monster_shambler")
		{
		//custom_demon_name(attacker); //CH
			//If our own demon killed us, kill demon and cost 2 frags
			if (attacker->real_owner == targ) {
				Give_Frags_Out(attacker->real_owner, attacker->real_owner, -1, 0, 1, 1, 0);
																	   //-2
/*				attacker.real_owner.real_frags = attacker.real_owner.real_frags - 2;
				if (!(toggleflags & #TFLAG_TEAMFRAGS))
					attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
				if (deathmsg == PR_DMSG_LIGHTNING)
				{
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " annihilates its owner ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " with a bolt of lightning\n");
				}
				else if (deathmsg == PR_DMSG_DEMON_FIRE)
				{
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " detonates its owner ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " with a fireball\n");
				}
				else
				{
					if (rnum < 0.5) {
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " gets sick of its owner ");
						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, "\n");
					}
					else {
						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, " screams as ");
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " turns on his owner\n");
					}
				}

				//kill_my_demon(); //Demon dies too - OfN nope!
				return;
			}

			#ifdef PR_XMAS_STUFF
			if (deathmsg == PR_DMSG_XMAS)
			{
				if (rnum < 0.5)
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " joins ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, "'s christmas dance\n");
				}
				else
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " succumbs to ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, "'s christmas dance\n");
				}
			}
			else
			#endif
			if (deathmsg == PR_DMSG_LIGHTNING)
			{
				if (rnum < 0.2) {
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, "'s body crackles with electricity as ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ") claims another victim\n");
				}
				else if (rnum < 0.5) {
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " meets a bolt of lightning directed by ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ")\n");
				}
				else if (rnum < 0.75) {
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, "'s goose is cooked by ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, "'s (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ") lightning\n");
				}
				else {
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s shambler, ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, ", teaches ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " to cook without gas\n");
				}
			}
			else if (deathmsg == PR_DMSG_DEMON_FIRE)
			{
				if (rnum < 0.5)
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " is obliterated by ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ")'s fireball\n");
				}
				else
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " is turned into little ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, "s by ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ")'s fireball\n");
				}
			}
			else
			{
				if (rnum < 0.05)
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " screams as ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ")'s claws slash down\n");
				}
				else if (rnum < 0.1) {
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " was torn asunder by ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ")\n");
				}
				else if (rnum < 0.50) {
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " is smashed into next week by ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " (");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, ")\n");
				}
				else {
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s shambler, ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					if (rnum < 0.75)
						bprint (PR_PRINT_MEDIUM, ", tears ");
					else
						bprint (PR_PRINT_MEDIUM, ", rips ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " to shreds\n");
				}
			}

			if (!Teammate(targ, attacker->real_owner) || attacker->real_owner->team_no < 1)
				Give_Frags_Out(attacker->real_owner, attacker->real_owner, 1, 0, 1, 1, 0);

/*				attacker.real_owner.real_frags = attacker.real_owner.real_frags + 1;
			if (!(toggleflags & #TFLAG_TEAMFRAGS))
				attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
			return;
		}

		if (attacker->classname == "teledeath")
		{
			bprint (PR_PRINT_MEDIUM, targ->netname);
			bprint (PR_PRINT_MEDIUM, " was telefragged by ");
			bprint (PR_PRINT_MEDIUM, attacker->owner->netname);
			bprint (PR_PRINT_MEDIUM, "\n");

			if (!Teammate(targ, attacker->owner) || attacker->owner->team_no < 1)
				Give_Frags_Out(attacker->owner, attacker->owner, 1, 0, 1, 1, 0);

/*				attacker.owner.real_frags = attacker.owner.real_frags + 1;
			if (!(toggleflags & #TFLAG_TEAMFRAGS))
				attacker.owner.frags = attacker.owner.real_frags;
*/
			return;
		}

		if (attacker->classname == "teledeath2")
		{
			bprint (PR_PRINT_MEDIUM, "Satan's power deflects ");
			bprint (PR_PRINT_MEDIUM, targ->netname);
			bprint (PR_PRINT_MEDIUM, "'s telefrag\n");

			Give_Frags_Out(targ, targ, -1, 1, 1, 1, 0);

/*			targ.real_frags = targ.real_frags - 1;
			if (!(toggleflags & #TFLAG_TEAMFRAGS))
				targ.frags = targ.real_frags;

#ifdef QUAKE_WORLD
			logfrag (targ, targ);
#endif
*/
			return;
		}

		// Gizmo - added idsoftware qw deathmatch code for double 666 telefrag, see tdeath_touch also
		// double 666 telefrag (can happen often in deathmatch 4)
		if (attacker->classname == "teledeath3")
		{
			bprint (PR_PRINT_MEDIUM, targ->netname);
			bprint (PR_PRINT_MEDIUM, " was telefragged by ");
			bprint (PR_PRINT_MEDIUM, attacker->owner->netname);
			bprint (PR_PRINT_MEDIUM, "'s Satan's power\n");

			Give_Frags_Out (targ, targ, -1, 1, 1, 1, 0);
/*
			targ.frags = targ.frags - 1;
			logfrag (targ, targ);
*/
			return;
		}

		// Killed by a goal
		if (attacker->classname == "info_tfgoal")
		{
			if (attacker->deathtype != "")
			{
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, attacker->deathtype);
			}
#ifdef PR_QUAKE_WORLD
			logfrag (targ, targ);
#endif
			return;
		}

		if (attacker->classname == "player")
		{
			if (targ == attacker)
			{
/*				attacker.real_frags = attacker.real_frags - 1;
				if (teamplay & #TEAMPLAY_VAMPIRE) //WK
					attacker.real_frags = attacker.real_frags - 1;

				if (!(toggleflags & #TFLAG_TEAMFRAGS))
					attacker.frags = attacker.real_frags;
*/
				bprint (PR_PRINT_MEDIUM, targ->netname);

				deathstring = " kills himself!\n";

				if (deathmsg == PR_DMSG_GREN_HAND)
					deathstring = " grenades himself\n";
				else if (deathmsg == PR_DMSG_MARTYR)
				{

					//if (rnum <= 0.5)
						deathstring = " dies for the cause\n";
					//else
					//	deathstring = " is persecuted to death\n";

					/*
					//if (!(targ.job & #JOB_MARTYR_ENEMY)) {
						if (rnum <= 0.5)
							deathstring = " dies for the heck of it\n";
						else
							deathstring = " dies with style\n";

					else {
						//Give last person who attacked him a frag
						if (targ.martyr_enemy.classname == "player" && targ.martyr_enemy.has_disconnected == #FALSE) {
							Give_Frags_Out(targ.martyr_enemy, targ, 1, 1, 1, 1, 0);
						}
						if (rnum <= 0.5)
							deathstring = " dies for the cause\n";
						else
							deathstring = " is persecuted to death\n";
					} */
				}
				/*else if (deathmsg == #DMSG_EXPBODY) - OfN - No need!
				{
					deathstring = "'s body explodes!\n";
				}*/
				else if (deathmsg == PR_DMSG_BERSERK)
				{
					if (rnum < 0.5)
						deathstring = " collapses from exhaustion\n";
					else
						deathstring = " goes down fighting\n";
				}
				else if (deathmsg == PR_DMSG_GREN_NAIL)
					deathstring = " hammers himself\n";
				else if (deathmsg == PR_DMSG_GREN_FRAG)
					deathstring = " fragments himself\n";
				else if (deathmsg == PR_DMSG_LAND_MINE)
				{
					if (rnum < 0.5)
						deathstring = " forgot where he put his minefield\n";
					else
					{
						if (rnum < 0.75)
							deathstring = " blows up along with his mines\n";
						else
							deathstring = " explodes with his own mine\n";
					}
				}
				else if (deathmsg == PR_DMSG_GREN_MIRV)
					deathstring = " goes to pieces\n";
				else if (deathmsg == PR_DMSG_GREN_PIPE)
					deathstring = " ambushes himself with his own pipebombs\n";
				else if (deathmsg == PR_DMSG_GREN_GAS)
					deathstring = " chokes on his own gas\n";
				else if (deathmsg == PR_DMSG_GREN_EMP)
					deathstring = " explodes his ammo and body\n";
				else if (deathmsg == PR_DMSG_GREN_FLASH)
					deathstring = " is charred by his own flash grenade\n";
				else if (deathmsg == PR_DMSG_GREN_EMP_AMMO)
					deathstring = " detonates an ammo box too close to him\n";
				else if (deathmsg == PR_DMSG_DETPACK)
					deathstring = " set the detpack and forgot to run\n";
				else if (deathmsg == PR_DMSG_BIOWEAPON)
					deathstring = " died by his own infection!\n";
				else if (deathmsg == PR_DMSG_FORCEFIELD)
					deathstring = " is fried in his own force field\n";
				else if (deathmsg == PR_DMSG_STUCK_FORCEFIELD)
					deathstring = " is caught and shocked to death in his own force field\n";
				else if (deathmsg == PR_DMSG_SPANNERFIELD)
				{
					if (rnum < 0.5)
						deathstring = " dies learning something about spanners and force fields\n";
					else
						deathstring = " learns something about spanners and force fields..\n";
				}
				else if (deathmsg == PR_DMSG_NAPALM)
					deathstring = " burns with his own napalm\n";
				else if (deathmsg == PR_DMSG_TELEOUTWORLD)
					deathstring = " is teleported outside the world!\n";
				else if (deathmsg == PR_DMSG_MAUSER)
					deathstring = " assassinated himself!\n";
				else if (deathmsg == PR_DMSG_DAEDALUS)
					deathstring = " flew too close to the sun!\n";
				else if (deathmsg == PR_DMSG_AIRG || deathmsg == PR_DMSG_AIRG_WATER)
					deathstring = " airfists his own ass!\n";
				else if (deathmsg == PR_DMSG_JUDOKA)
					deathstring = " broke his own arms!\n";
				else if (deathmsg == PR_DMSG_AXE)
					deathstring = " drove an axe through his own head!\n";
				else if (deathmsg == PR_DMSG_BACKSTAB)
					deathstring = " drove a knife into his own heart!\n";
				else if (deathmsg == PR_DMSG_FGTRAP)
					deathstring = " is electrified tweaking a field generator\n";
				else if (deathmsg == PR_DMSG_CALTROP)
					deathstring = " stepped on too many of his own caltrops\n";
				else if (deathmsg == PR_DMSG_FLYCALTROP)
					deathstring = " was spiked by his own catlrop\n";
				else if (deathmsg == PR_DMSG_ROCKETL)
				{
					if (rnum < 0.5)
						deathstring = " becomes bored with life\n";
					else
						deathstring = " checks if his weapon is loaded\n";
				}
				else if (deathmsg == PR_DMSG_CLUSTER_ROCKET)
					deathstring = " eats his clusters\n";
				else if (deathmsg == PR_DMSG_LASERCANNON)
					deathstring = " blasts himself\n";
				else if (deathmsg == PR_DMSG_BIOEXPLOSION)
					deathstring = " killed himself with a biological grenade\n";
				else if (deathmsg == PR_DMSG_INCENDIARY)
					deathstring = " chars himself with an incendiary rocket\n";
				else if (deathmsg == PR_DMSG_GRENADEL)
					deathstring = " tries to put the pin back in\n";
				else if (deathmsg == PR_DMSG_FLAME)
					deathstring = " torches himself\n";
				else if (deathmsg == PR_DMSG_HOVER) //WK
					deathstring = " dies from uncomfortable boots\n";
				else if (deathmsg == PR_DMSG_LIGHTNING)
					deathstring = " electrocutes himself\n";
				else if (deathmsg == PR_DMSG_STORM)
				{
					bprint(PR_PRINT_MEDIUM," is killed by the storm lightning!\n");
					return;
				}
				else if (deathmsg == PR_DMSG_LIGHTNING && targ->waterlevel > 1)
				{
					Give_Frags_Out(attacker, attacker, -1, 0, 1, 1, 1);
					bprint (PR_PRINT_MEDIUM, " discharges into the water.\n");
					return;
				}

				// killed self
				Give_Frags_Out(attacker, attacker, -1, 0, 1, 1, 1);

				bprint(PR_PRINT_MEDIUM, deathstring);
				return;
			}
			else if (teamplay && Teammate(targ, attacker))
			{
				// killed a team member
				if (Teammate(targ, attacker) )
					Give_Frags_Out(attacker, attacker, -1, 0, 1, 1, 0);
//					attacker.real_frags = attacker.real_frags - 1; // killed a team member
				else
					Give_Frags_Out(attacker, attacker, 1, 0, 1, 1, 0);
//					attacker.real_frags = attacker.real_frags + 1;

//				if (!(toggleflags & #TFLAG_TEAMFRAGS))
//					attacker.frags = attacker.real_frags;
				if (deathmsg == PR_DMSG_MEDIKIT)
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " didn't survive the operation.\n");
					return;
				}

				if (deathmsg == PR_DMSG_EXPBODY) //- OfN
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " has been killed as a side effect of ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, "'s body explosion\n");
					return;
				}

				if (deathmsg == PR_DMSG_FORCEFIELD) //- OfN
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " is fried trying to pass thru ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, "'s force field\n");
					return;
				}

				if (deathmsg == PR_DMSG_STUCK_FORCEFIELD) //- OfN
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " died stuck inside of ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, "'s force field\n");
					return;
				}

				if (deathmsg == PR_DMSG_LASERCANNON) //- OfN
				{
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " laser-bolt bounces on ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, "\n");
					return;
				}

				if (deathmsg == PR_DMSG_DISPEL) //- OfN
				{
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " dispels a teammate. (");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, ")\n");
					return;
				}

				/*if (deathmsg == #DMSG_BIOEXPLOSION)
				{
					bprint(#PRINT_MEDIUM,targ.netname);
					bprint(#PRINT_MEDIUM," dies due to ");
					bprint(#PRINT_MEDIUM,attacker.netname);
					bprint(#PRINT_MEDIUM,"'s infection grenade exploding near\n");
					return;
				}*/

				// OfN - As you can see i was tired of the same 4 teamkill deathmessages
				// so i added lots, some are great IMHO :P
				if (rnum < 0.1)
				{
					if (random() > 0.5)
					{
						if (random() > 0.25)
						{
							if (random() > 0.5)
							{
								if (random() < 0.4)
								{
									bprint (PR_PRINT_MEDIUM, attacker->netname);
									bprint (PR_PRINT_MEDIUM, " considers ");
									bprint (PR_PRINT_MEDIUM, targ->netname);
									bprint (PR_PRINT_MEDIUM, " completely expendable");
								}
								else
								{
									bprint (PR_PRINT_MEDIUM, attacker->netname);
									bprint (PR_PRINT_MEDIUM, " doesn't consider ");
									bprint (PR_PRINT_MEDIUM, targ->netname);
									bprint (PR_PRINT_MEDIUM, " essential anymore");
								}
							}
							else
							{
								bprint (PR_PRINT_MEDIUM, attacker->netname);
								if (random() > 0.2)
									bprint (PR_PRINT_MEDIUM, " thinks ");
								else
									bprint (PR_PRINT_MEDIUM, " notes ");
								bprint (PR_PRINT_MEDIUM, targ->netname);
								bprint (PR_PRINT_MEDIUM, " isn't needed anymore");
							}
						}
						else
						{
							bprint (PR_PRINT_MEDIUM, attacker->netname);
							bprint (PR_PRINT_MEDIUM, " sacrifices ");
							bprint (PR_PRINT_MEDIUM, targ->netname);
							bprint (PR_PRINT_MEDIUM, " for the team");
						}
					}
					else
					{
						if (random() > 0.5)
						{
							if (random() > 0.5)
							{
								bprint (PR_PRINT_MEDIUM, targ->netname);
								bprint (PR_PRINT_MEDIUM, " doesn't fit in ");
								bprint (PR_PRINT_MEDIUM, attacker->netname);
								bprint (PR_PRINT_MEDIUM, "'s new philosophy");
							}
							else
							{
								bprint (PR_PRINT_MEDIUM, "From ");
								bprint (PR_PRINT_MEDIUM, attacker->netname);
								bprint (PR_PRINT_MEDIUM, "'s point of view ");
								bprint (PR_PRINT_MEDIUM, targ->netname);
								bprint (PR_PRINT_MEDIUM, " isn't really needed");
							}
						}
						else
						{
							if (random() > 0.5)
							{
								bprint (PR_PRINT_MEDIUM, attacker->netname);
								bprint (PR_PRINT_MEDIUM, " sees ");
								bprint (PR_PRINT_MEDIUM, targ->netname);
								bprint (PR_PRINT_MEDIUM, "'s death as an unavoidable collateral damage");
							}
							else
							{
								bprint (PR_PRINT_MEDIUM, attacker->netname);
								bprint (PR_PRINT_MEDIUM, " last decision takes ");
								bprint (PR_PRINT_MEDIUM, targ->netname);
								bprint (PR_PRINT_MEDIUM, "'s death into account");
							}
						}
					}
				}
				else if (rnum < 0.2)
				{
					if (random() > 0.5)
					{
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " doesn't tolerate ");
						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, " in his way anymore");
					}
					else
					{
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " didn't accept ");
						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, " in his way");
					}
				}
				else if (rnum < 0.3)
				{
					if (random() > 0.5)
					{
						bprint (PR_PRINT_MEDIUM,targ->netname);
						bprint (PR_PRINT_MEDIUM, " describes ");
						bprint (PR_PRINT_MEDIUM,attacker->netname);
						bprint (PR_PRINT_MEDIUM, "'s mother with his last words");
					}
					else
					{
						bprint (PR_PRINT_MEDIUM,targ->netname);
						bprint (PR_PRINT_MEDIUM, " mentions the family of ");
						bprint (PR_PRINT_MEDIUM,attacker->netname);
						bprint (PR_PRINT_MEDIUM, " in his last sentence");
					}
				}
				else if (rnum < 0.4)
				{
					if (random() > 0.75)
					{
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " destroys the love between ");
						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, " and him");
					}
					else
					{
						bprint (PR_PRINT_MEDIUM, "Things aren't like before between ");
						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, " and ");
						bprint (PR_PRINT_MEDIUM, attacker->netname);
					}
				}
				else if (rnum < 0.5)
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " dies and ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " loses another friend");
				}
				else if (rnum < 0.6)
				{
					if (random() > 0.5)
					{
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " kills ");
						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, " and the baby Jesus cries");
					}
					else
					{
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " made the baby Jesus cry by killing ");
						bprint (PR_PRINT_MEDIUM, targ->netname);
					}
				}
				else if (rnum < 0.7)
				{
					if (random() > 0.5)
					{
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " mows down teammate ");
						bprint (PR_PRINT_MEDIUM, targ->netname);
					}
					else
					{
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " decreases teammate count by killing ");
						bprint (PR_PRINT_MEDIUM, targ->netname);
					}
				}
				else if (rnum < 0.8)
				{
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " checks his glasses after killing ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
				}
				else if (rnum < 0.9)
				{
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " gets a frag for the other team with ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, "'s death");
				}
				else
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " doesn't wuv ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, " any more! Wah!");
				}

				bprint (PR_PRINT_MEDIUM, "\n");
				return;
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 1, 1, 1, 1);
//CH
//				attacker.real_frags = attacker.real_frags + 1;
				if (attacker->tfstate & PR_TFSTATE_INSPIRED)
				if (attacker->inspirator != world && attacker->inspirator->is_connected)
				{
					attacker->inspirator->real_frags =
					attacker->inspirator->real_frags + 0.5;
					if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
						attacker->inspirator->frags =
						attacker->inspirator->real_frags;
				}

				if (attacker->aura && attacker->crusader_inspirator != world && attacker->crusader_inspirator->is_connected)
				{
					if (Teammate(attacker, attacker->crusader_inspirator)) // cause he could be a spy..
					{
						attacker->crusader_inspirator->real_frags = attacker->crusader_inspirator->real_frags + 0.5;
						if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
							attacker->crusader_inspirator->frags = attacker->crusader_inspirator->real_frags;
					}
					else // Punishment for idiot crusaders.. :)
					{
						attacker->crusader_inspirator->real_frags = attacker->crusader_inspirator->real_frags - 0.5;
						if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
							attacker->crusader_inspirator->frags = attacker->crusader_inspirator->real_frags;
					}
				}

				//WK
/*				if (teamplay & #TEAMPLAY_VAMPIRE) //Frag vamp
					targ.real_frags = targ.real_frags - 1;

				#ifdef QUAKE_WORLD
					logfrag (attacker, targ);
				#endif

				if (!(toggleflags & #TFLAG_TEAMFRAGS)) {
					attacker.frags = attacker.real_frags;
					targ.frags = targ.real_frags; //In case of vampire
				}

*/
				if (deathmsg == PR_DMSG_CLUSTER_ROCKET)
				{
					if (rnum < 0.5)
					{
						deathstring = " is put away by ";
						deathstring2 = "'s cluster rockets\n";
					}
					else
					{
						deathstring = " is destroyed by ";
						deathstring2 = "'s cluster rockets\n";
					}
				}
				else if (deathmsg == PR_DMSG_EXPBODY)
				{
					deathstring = " is killed by ";
					deathstring2 = "'s body explosion\n";
				}
				else if (deathmsg == PR_DMSG_GREN_HAND)
				{
					deathstring = " surfs on a grenade from ";
					deathstring2 = "\n";
				}
				else if (deathmsg == PR_DMSG_GREN_NAIL)
				{
					deathstring = " gets flayed by ";
					deathstring2 = "'s nail grenade\n";
				}
				else if (deathmsg == PR_DMSG_GREN_FRAG) //WK
				{
					if (rnum < 0.5) {
						deathstring = " is slashed apart by ";
						deathstring2 = "'s frag grenade\n";
					}
					else if (rnum < 0.75) {
						deathstring = "'s jugular artery is severed by ";
						deathstring2 = "'s frag grenade.\n";
					}
					else {
						deathstring = "'s carotid artery is severed by ";
						deathstring2 = "'s frag grenade.\n";
					}
				}
				else if (deathmsg == PR_DMSG_GREN_MIRV)
				{
					deathstring = " gets spammed by ";
					deathstring2 = "'s Mirv grenade\n";
				}
				else if (deathmsg == PR_DMSG_CALTROP)
				{
					deathstring = " collects one too many of ";
					deathstring2 = "'s caltrops\n";
				}
				else if (deathmsg == PR_DMSG_FLYCALTROP)
				{
					deathstring = " was spiked by ";
					deathstring2 = "'s caltrops\n";
				}
				else if (deathmsg == PR_DMSG_GREN_PIPE)
				{
					deathstring = " is caught by ";
					deathstring2 = "'s pipebomb trap\n";
				}
				else if (deathmsg == PR_DMSG_GREN_GAS)
				{
					deathstring = " gags on ";
					deathstring2 = "'s noxious gasses\n";
				}
				else if (deathmsg == PR_DMSG_GREN_EMP)
				{
					deathstring = "'s ammo detonates him as ";
					deathstring2 = "'s EMP fries it.\n";
				}
				else if (deathmsg == PR_DMSG_GREN_FLASH)
				{
					deathstring = " is charred by ";
					deathstring2 = "'s flash grenade.\n";
				}
				else if (deathmsg == PR_DMSG_GREN_EMP_AMMO)
				{
					deathstring = " stands near some ammo as ";
					deathstring2 = "'s EMP nukes it\n";
				}
				else if (deathmsg == PR_DMSG_DETPACK)
				{
					deathstring = " reaches orbit via ";
					deathstring2 = "'s detpack\n";
				}
				else if (deathmsg == PR_DMSG_DETPACK_DIS)
				{
					deathstring = " cut the red wire of ";
					deathstring2 = "'s detpack\n";
				}
				else if (deathmsg == PR_DMSG_BIOWEAPON)
				{
					deathstring = " dies from ";
					deathstring2 = "'s mysterious tropical disease\n";
				}
				else if (deathmsg == PR_DMSG_MAUSER)
				{
				/*	deathstring = " is assassinated by ";
					deathstring2 = "'s Mauser WK-77\n"; */
					sprint(attacker, PR_PRINT_HIGH, "You mercilessly slaughter ");
					sprint(attacker, PR_PRINT_HIGH, targ->netname);
					sprint(attacker, PR_PRINT_HIGH, "!\n");
					return;
				}
				else if (deathmsg == PR_DMSG_DAEDALUS)
				{
					deathstring = " is melted by ";
					deathstring2 = "'s Daedalus impulse rifle\n";
				}
				else if (deathmsg == PR_DMSG_BIOWEAPON_ATT)
				{
					deathstring = " escapes infection from ";
					deathstring2 = " by dying first\n";
				}
				else if (deathmsg == PR_DMSG_GRENADEL)
				{
					deathstring = " eats ";
					deathstring2 = "'s pineapple\n";
					if (targ->health < -40)
					{
						deathstring = " was gibbed by ";
						deathstring2 = "'s grenade\n";
					}
				}
				else if (deathmsg == PR_DMSG_STUCK_FORCEFIELD)
				{
					deathstring = " is fried in ";
					deathstring2 = "'s force field\n";
				}
				else if (deathmsg == PR_DMSG_FORCEFIELD)
				{
					deathstring = " had a shocking experience on ";
					deathstring2 = "'s force field\n";
				}
				else if (deathmsg == PR_DMSG_FGTRAP)
				{
					deathstring = " explodes with ";
					deathstring2 = "'s trapped field generator\n";
				}
				else if (deathmsg == PR_DMSG_ROCKETL)
				{
					deathstring = " rides ";
					deathstring2 = "'s rocket\n";
					if (targ->health < -40)
					{
						deathstring = " was gibbed by ";
						deathstring2 = "'s rocket\n" ;
					}
				}
				else if (deathmsg == PR_DMSG_HOVER) //WK
				{
					deathstring = " choked on ";
					deathstring2 = "'s hover exhaust\n";
				}
				else if (deathmsg == PR_DMSG_LAND_MINE) //WK
				{
					deathstring2 = "'s land mine\n";

					if (rnum < 0.8)
						deathstring = " walked over ";
					else
						deathstring = " was launched 40 ft into the air by ";

					if (targ->health < -40)
					{
						if (rnum < 0.8)
							deathstring = " was turned into a shower of gibs by ";
						else
							deathstring = " was launched 80 ft into the air by ";
					}

					// We killed one poor guy with our mines:
					MineKill(attacker);
				}
				else if (deathmsg == PR_DMSG_FLAME)
				{
					if (rnum < 0.2)
					{
						deathstring =  " is burnt up by ";
						deathstring2 = "'s flame\n";
					}
					else if (rnum < 0.4)
					{
						deathstring =  " is fried by ";
						deathstring2 = "'s fire\n";
					}
					else if (rnum < 0.6)
					{
						deathstring =  " feels ";
						deathstring2 = "'s fire of wrath\n";
					}
					else if (rnum < 0.8)
					{
						deathstring =  " is reduced to ashes by ";
						deathstring2 = "\n";
					}
					else
					{
						deathstring = " is grilled by ";
						deathstring2 = "'s flame\n";
					}
				}
				else if (deathmsg == PR_DMSG_AXE)
				{
					deathstring2 = "\n";

					if (attacker->cutf_items & PR_CUTF_KNIFE)
					{
						if (attacker->job & PR_JOB_WARLOCK)
						{
#ifdef PR_COOP_MODE_ENHANCED
							if ( COOP_IsCoopMonster( targ ) && targ->monsterflags & PR_MFLAG_NOHEART )
								deathstring = " was knife-murdered by ";
							else
#endif
							if (rnum < 0.33)
								deathstring = " is forced to donate his blood by ";
							else if (rnum < 0.66)
							{
								bprint(PR_PRINT_MEDIUM,attacker->netname);
								bprint(PR_PRINT_MEDIUM," grabs ");
								bprint(PR_PRINT_MEDIUM,targ->netname);
								bprint(PR_PRINT_MEDIUM,"'s heart for his collection\n");
								WarlockKnifeKill(attacker);
								return;
							}
							else
								deathstring = "'s heart is now a prized possession of ";
						}
						else
						{
							deathstring = " was knife-murdered by ";
						}
					}
					else
						deathstring = " was axe-murdered by ";

					//if (attacker.demon_blood < 0) //WK Sanity Check
					//	attacker.demon_blood = 0;
					// if the above ever happens you've got a serious bug that needs fixing
					// don't get around it by cheap hacks - SB

					/*if (attacker.demon_blood < #MAX_KNIFE_BLOOD) //SB - OfN defined max now
						attacker.demon_blood = attacker.demon_blood + 1;*/

#ifdef PR_COOP_MODE_ENHANCED
					if ( ( COOP_IsCoopMonster( targ ) && !( targ->monsterflags & PR_MFLAG_NOHEART ) ) || !COOP_IsCoopMonster( targ ) )
#endif
						WarlockKnifeKill(attacker);
				}
				else if (deathmsg == PR_DMSG_SPANNER)
				{
					deathstring = " was spanner-murdered by ";
					deathstring2 = "\n";
				}
				else if (deathmsg == PR_DMSG_SHOTGUN)
				{
					deathstring = " chewed on ";
					deathstring2 = "'s boomstick\n";
				}
				else if (deathmsg == PR_DMSG_SSHOTGUN)
				{
					deathstring = " ate 2 loads of ";
					deathstring2 = "'s buckshot\n";
				}
				else if (deathmsg == PR_DMSG_NAILGUN)
				{
					deathstring = " was nailed by ";
					deathstring2 = "\n";
				}
				else if (deathmsg == PR_DMSG_SNG)
				{
					deathstring = " was spiked by ";
					deathstring2 = "'s super nailgun\n";
				}
				else if (deathmsg == PR_DMSG_LIGHT_ASSAULT)
				{
					if (rnum <= 0.1) {
						deathstring = " wishes ";
						deathstring2 = "'s gun was still an SNG\n";
					}
					else if (rnum <= 0.5) {
						deathstring = " gets felled by ";
						deathstring2 = "'s light assault cannon\n";
					}
					else {
						deathstring = " gets sawn almost in half by ";
						deathstring2 = "\n";
					}
				}
				else if (deathmsg == PR_DMSG_LIGHTNING)
				{
					if (rnum <= 0.1) {
						deathstring = " has never seen a lightning gun in TF before. (";
						deathstring2 = " has.)\n";
					}
					else if (rnum <= 0.2) {
						deathstring = " is thunderstruck by ";
						deathstring2 = "'s gun\n";
					}
					else if (rnum <= 0.4) {
						deathstring = " is electrocuted by ";
						deathstring2 = "'s thunderbolt\n";
					}
					else if (rnum <= 0.7) {
						deathstring = " is shocked to death by ";
						deathstring2 = "'s lightning gun\n";
					}
					else {
						deathstring = " accepts ";
						if (attacker->waterlevel > 1)
							deathstring2 = "'s discharge\n";
						else
							deathstring2 = "'s shaft\n";
					}
				}
				else if (deathmsg == PR_DMSG_HOOK)
				{
					deathstring = " grappled with ";
					deathstring2 = "\n";
				}
				else if (deathmsg == PR_DMSG_NAPALM)
				{
					deathstring = " is fried with ";
					deathstring2 = "'s napalm fireworks\n";
				}
				else if (deathmsg == PR_DMSG_JUDOKA)
				{
					deathstring = " had his arms broken by ";
					deathstring2 = "\n";
				}
				else if (deathmsg == PR_DMSG_AIRG)
				{
					if (rnum < 0.5)
						deathstring = " is ventilated by ";
					else
						deathstring = " receives fresh air from ";

					deathstring2 = "\n";
				}
				else if (deathmsg == PR_DMSG_LASERCANNON)
				{
					deathstring = " was blasted by ";
					deathstring2 = "'s laser cannon\n";
				}
				else if (deathmsg == PR_DMSG_BIOEXPLOSION)
				{
					deathstring = " dies by the toxic explosion of ";
					deathstring2 = "'s infection grenade\n";
				}
				else if (deathmsg == PR_DMSG_DISPEL)
				{
					deathstring = " is banished to the netherworld by ";
					deathstring2 = "\n";
				}
				else if (deathmsg == PR_DMSG_AIRG_WATER)
				{
					if (rnum < 0.5)
					{
						deathstring = " learns to swim backwards with ";
						deathstring2 = "'s help\n";
					}
					else
					{
						deathstring = " is underwater-airfisted by ";
						deathstring2 = "\n";
					}
				}
				else if (deathmsg == PR_DMSG_SNIPERRIFLE)
				{
					if (attacker->cutf_items & PR_CUTF_OTR && random() > 0.25)
					{
						if (targ->health < -40 && random() > 0.75)
						{
							deathstring = " becomes some pieces of meat with ";
							deathstring2 = "'s OTR shot\n";
						}
						else
						{
							if (rnum < 0.3)
							{
								deathstring = " will never be the same after ";
								deathstring2 = "'s OTR shot\n";
							}
							else if (rnum < 0.6)
							{
								deathstring = " learns from ";
								deathstring2 = " what an OTR bullet is\n";
							}
							else
							{
								deathstring = " gets a new OTR implant from ";
								deathstring2 = "\n";
							}
						}
					}
					else if (rnum < 0.5)
					{
						deathstring = " takes a bullet in the chest from ";
						deathstring2 = "\n";
					}
					else
					{
						deathstring = " succumbs to sniperfire from ";
						deathstring2 = "\n";
					}
				}
				else if (deathmsg == PR_DMSG_SNIPERHEADSHOT)
				{
					if (attacker->cutf_items & PR_CUTF_OTR && random() > 0.25)
					{
						deathstring = "'s head flies away due to ";
						deathstring2 = " and his OTR bullets\n";
					}
					else if (rnum < 0.5)
					{
						deathstring = " gets a third eye from ";
						deathstring2 = "\n";
					}
					else
					{
						deathstring = " gets his head blown off by ";
						deathstring2 = "\n";
					}
				}
				else if (deathmsg == PR_DMSG_SNIPERLEGSHOT)
				{
					if (attacker->cutf_items & PR_CUTF_OTR && random() > 0.25)
					{
						deathstring = " owns a metallic replacement for his legs from ";
						deathstring2 = "\n";
					}
					else if (rnum < 0.5)
					{
						deathstring = " is made legless by ";
						deathstring2 = "\n";
					}
					else
					{
						deathstring = " gets his legs blown off by ";
						deathstring2 = "\n";
					}
				}
				else if (deathmsg == PR_DMSG_AUTORIFLE)
				{
					deathstring = " collects ";
					deathstring2 = "'s bullet spray.\n";
				}
				else if (deathmsg == PR_DMSG_ASSAULTCANNON)
				{
					deathstring = " gets sawn in half by ";
					deathstring2 = "\n";
				}
				else if (deathmsg == PR_DMSG_BACKSTAB)
				{
#ifdef PR_COOP_MODE_ENHANCED
					if (attacker->job & PR_JOB_WARLOCK && random() < 0.33 && ((COOP_IsCoopMonster( targ ) && !(targ->monsterflags & PR_MFLAG_NOHEART)) || !COOP_IsCoopMonster( targ )))
#else
					if (attacker->job & PR_JOB_WARLOCK && random() < 0.33)
#endif
					{
						bprint(PR_PRINT_MEDIUM,attacker->netname);
						bprint(PR_PRINT_MEDIUM," extirpates ");
						bprint(PR_PRINT_MEDIUM,targ->netname);
						bprint(PR_PRINT_MEDIUM,"'s heart\n");
						WarlockKnifeKill(attacker);
						return;
					}

					if (rnum < 0.5)
					{
						deathstring = " gets assassinated by ";
						deathstring2 = "\n";
					}
					else
					{
						deathstring = " is knifed from behind by ";
						deathstring2 = "\n";
					}

					//if (attacker.demon_blood < 0) //WK Sanity Check
					//	attacker.demon_blood = 0;
					// See previous comment - SB
					/*if (attacker.demon_blood < #MAX_KNIFE_BLOOD) //SB
						attacker.demon_blood = attacker.demon_blood + 1;*/

#ifdef PR_COOP_MODE_ENHANCED
					if ((COOP_IsCoopMonster( targ ) && !(targ->monsterflags & PR_MFLAG_NOHEART)) || !COOP_IsCoopMonster( targ ))
#endif
						WarlockKnifeKill(attacker);
				}
				else if (deathmsg == PR_DMSG_TRANQ)
				{
					deathstring = " is put to sleep by ";
					deathstring2 = "\n";
				}
				else if (deathmsg == PR_DMSG_LASERBOLT)
				{
					deathstring = " gets a hole in his heart from ";
					deathstring2 = "'s railgun\n";
				}
				else if (deathmsg == PR_DMSG_INCENDIARY)
				{
					deathstring = " gets well done by ";
					deathstring2 = "'s incendiary rocket\n";
				}
				else if ( deathmsg == PR_DMSG_ADMINKILL )
				{
					deathstring = " was struck down by the admin ";
					deathstring2 = "\n";
				}

				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, deathstring);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, deathstring2);
			}
			return;
		}
		else if (attacker->classname == "building_dispenser")
		{
			if (!(attacker->is_malfunctioning & PR_SCREWUP_TWO) && !(attacker->is_malfunctioning & PR_SCREWUP_FOUR))
			{
				if (targ == attacker->real_owner)
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " dispenses with himself\n");
					Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
				}
				else
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " didn't insert the correct change into ");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s dispenser\n");
					if (Teammate(targ, attacker))
						Give_Frags_Out(attacker, attacker, -1, 0, 1, 1, 0);
					else
						Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
				}
			}
			else
			{
				if (targ == attacker->real_owner)
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " is blown apart by his trapped dispenser\n");
					Give_Frags_Out(attacker->real_owner, attacker, -1, 0, 1, 1, 0);
				}
				else if (targ == attacker->martyr_enemy)
				{
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " should've stayed away from ");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s dispenser\n");
					Give_Frags_Out(attacker->martyr_enemy, attacker, -1, 0, 1, 1, 0);
				}
				else
				{
					bprint (PR_PRINT_MEDIUM, attacker->martyr_enemy->netname);
					bprint (PR_PRINT_MEDIUM, " laughs as ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, " is blown to pieces by ");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s trapped dispenser\n");
					Give_Frags_Out(attacker->martyr_enemy, targ, 1, 0, 1, 1, 0);
				}
			}
			return;
		}
		else if (attacker->classname == "building_sentrygun" || attacker->classname == "building_tesla")
		{
			if (targ == attacker->real_owner)
			{
				if (deathmsg == PR_DMSG_SENTRYGUN_ROCKET)
					deathstring = " intercepts his sentry gun's rocket\n";
				else if (deathmsg == PR_DMSG_SENTRYGUN_BULLET)
					deathstring = " crossed his sentry gun's line of fire\n";
				else if (deathmsg == PR_DMSG_BUG_ZAPPER)
					deathstring = " was electrocuted by his own sentry\n";
				else
					deathstring = " let his sentry turn against him\n"; //CH

				bprint(PR_PRINT_MEDIUM, targ->netname);
				bprint(PR_PRINT_MEDIUM, deathstring);
			}
			else
			{
				if (Teammate(targ, attacker)) {//remove frags if team member
				//CH its not the sentry/tesla's fault so only take from owner
				//	Give_Frags_Out(attacker, targ, -1, 0, 2, 0, 0); //gun
					Give_Frags_Out(attacker->real_owner, targ, -1, 1, 1, 1, 0);
				} else {
					Give_Frags_Out(attacker, targ, 1, 0, 2, 0, 0); //gun
					Give_Frags_Out(attacker->real_owner, targ, 1, 1, 1, 1, 0);
				}

				if (deathmsg == PR_DMSG_SENTRYGUN_ROCKET)
				{
					deathstring = " hates ";
					deathstring2 = "'s sentry gun\n";
				}
				else if (deathmsg == PR_DMSG_SENTRYGUN_BULLET)
				{
					deathstring = " is mown down by ";
					deathstring2 = "'s sentry gun\n";
				}
				else if (deathmsg == PR_DMSG_TESLA)
				{
					if (rnum < 0.5) {
						deathstring = " was electrocuted by ";
						deathstring2 = "'s tesla coil\n";
					}
					else if (rnum < 0.75) {
						deathstring = " was shocked to pieces by ";
						deathstring2 = "'s tesla sentry\n";
					}
					else {
						deathstring = " was annihilated by ";
						deathstring2 = "'s tesla gun\n";
					}
				}
				else if (deathmsg == PR_DMSG_BUG_ZAPPER)
				{
					if (rnum < 0.25) {
						deathstring = " made a frying sound after running into ";
						deathstring2 = "'s Sentry Point Defense System\n";
					}
					else if (rnum < 0.5) {
						deathstring = " is cooked at 400 degrees by ";
						deathstring2 = "'s sentry gun\n";
					}
					else if (rnum < 0.75) {
						deathstring = " is disintegrated by ";
						deathstring2 = "'s Sentry Point Defense System\n";
					}
					else {
						deathstring = " is obliterated by ";
						deathstring2 = "'s sentry gun\n";
					}
				}
				bprint(PR_PRINT_MEDIUM, targ->netname);
				bprint(PR_PRINT_MEDIUM, deathstring);
				bprint(PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint(PR_PRINT_MEDIUM, deathstring2);
			}
		}
		else if (attacker->classname == "building_teleporter")
		{
			sprint(attacker->martyr_enemy, PR_PRINT_HIGH, targ->netname);
			sprint(attacker->martyr_enemy, PR_PRINT_HIGH, " disappears into the void.\n");
			return;
		}
		else
		{
			Give_Frags_Out(targ, targ, -1, 1, 1, 1, 0);
/*
			#ifdef QUAKE_WORLD
				logfrag (targ, targ);
			#endif

			targ.real_frags = targ.real_frags - 1;	 // killed self
			if (!(toggleflags & #TFLAG_TEAMFRAGS))
				targ.frags = targ.real_frags;
*/
			rnum = targ->watertype;

			bprint (PR_PRINT_HIGH, targ->netname);
#ifdef PR_COOP_MODE_ENHANCED
/*
			if ( COOP_IsCoopMonster( targ ) ) {
				bprint (#PRINT_MEDIUM, " (");
				bprint (#PRINT_MEDIUM, ftos(targ.frags));
				bprint (#PRINT_MEDIUM, " frags)");
			}
*/
#endif

#ifdef PR_COOP_MODE_ENHANCED
			// Gizmo - moved this first for obvious reasons
			if ( COOP_IsCoopMonster( attacker ) ) {
				if (attacker == targ)
					deathstring = " killed itself\n";
				else if (attacker->classname == "idmonster_army")
					deathstring = " was shot by a Grunt\n";
				else if (attacker->classname == "idmonster_demon1")
					deathstring = " was eviscerated by a Fiend\n";
				else if (attacker->classname == "idmonster_dog")
					deathstring = " was mauled by a Rottweiler\n";
				else if (attacker->classname == "idmonster_dragon")
					deathstring = " was fried by a Dragon\n";
				else if (attacker->classname == "idmonster_enforcer")
					deathstring = " was blasted by an Enforcer\n";
				else if (attacker->classname == "idmonster_fish")
					deathstring = " was fed to the Rotfish\n";
				else if (attacker->classname == "idmonster_hell_knight")
					deathstring = " was slain by a Death Knight\n";
				else if (attacker->classname == "idmonster_knight")
					deathstring = " was slashed by a Knight\n";
				else if (attacker->classname == "idmonster_ogre")
					deathstring = " was destroyed by an Ogre\n";
				else if (attacker->classname == "idmonster_oldone")
					deathstring = " became one with Shub-Niggurath\n";
				else if (attacker->classname == "idmonster_shalrath")
					deathstring = " was exploded by a Vore\n";
				else if (attacker->classname == "idmonster_shambler")
					deathstring = " was smashed by a Shambler\n";
				else if (attacker->classname == "idmonster_tarbaby")
					deathstring = " was slimed by a Spawn\n";
				else if (attacker->classname == "idmonster_vomit")
					deathstring = " was vomited on by a Vomitus\n";
				else if (attacker->classname == "idmonster_wizard")
					deathstring = " was scragged by a Scrag\n";
				else if (attacker->classname == "idmonster_zombie")
					deathstring = " joins the Zombies\n";
			}

			else if (rnum == -3)
#else
			if (rnum == -3)
#endif
			{
				if (random() < 0.5)
					deathstring = " sleeps with the fishes\n";
				else
					deathstring = " ate before swimming\n"; //CH
			}
			else if (rnum == -4)
			{
				if (random() < 0.5)
					deathstring = " gulped a load of slime\n";
				else
					deathstring = " can't exist on slime alone\n";
			}
			else if (rnum == -5)
			{
				if (targ->health < -15)
				{
					deathstring = " burst into flames\n";
				}
				else if (random() < 0.5)
					deathstring = " turned into hot slag\n";
				else
					deathstring = " visits the Volcano God\n";
			}
			else if (attacker->classname == "explo_box")
			{
				deathstring = " blew up\n";
			}
			else if (attacker->solid == PR_SOLID_BSP && attacker != world)
			{
				deathstring = " was squished\n";
			}
			else if (attacker->classname == "trap_shooter" || attacker->classname == "trap_spikeshooter" || attacker->classname == "trap_tf_spikeshooter" || attacker->classname == "trap_tf_shooter")
			{
				if (attacker->deathtype != string_null)
					deathstring = attacker->deathtype;
				else if (attacker->spawnflags == PR_SPAWNFLAG_LASER)
					deathstring = " had surgery from a laser\n";
				else if (attacker->spawnflags == PR_SPAWNFLAG_TFROCKET)
				{
					//if (rnum < 0.9)
						deathstring = " took a ride on a rocket\n";
					//else
					//	deathstring = " proves for once and for all that he IS the rocket man\n";
				}
				else if (attacker->spawnflags == PR_SPAWNFLAG_TFGRENADE)
					deathstring = " tried to eat a wild pineapple\n";
				else if (attacker->spawnflags == PR_SPAWNFLAG_TFFLAME)
					deathstring = " was burned alive\n";
				else if (attacker->spawnflags == PR_SPAWNFLAG_SUPERSPIKE)
					deathstring = " was nailed to the ground\n";
				else
					deathstring = " was spiked\n";
			}
			else if (attacker->classname == "fireball")
			{
				deathstring = " ate a lavaball\n";
			}
			else if (attacker->classname == "trigger_changelevel")
			{
				if (random() < 0.5)
					deathstring = " tried to leave\n";
				else
					deathstring = " thought he could SLIP through unnoticed\n";
			}
			else if (targ->deathtype == "falling" && attacker == world)
			{
				targ->deathtype = "";
				deathstring = " fell to his death\n";
			}
#ifdef PR_COOP_MODE_ENHANCED
			else if (targ->deathtype == "monster_respawn" && attacker == world) {
				targ->deathtype = "";
				deathstring = " died in a transporter accident\n";
			}
#endif
			else //- OfN - unknown death by himself
			{
				rnum = random();

				if (rnum < 0.25)
					deathstring = " died happily\n";
				else if (rnum < 0.5)
					deathstring = " knows how to die with style\n";
				else if (rnum < 0.75)
					deathstring = " simply dies\n";
				else deathstring = " had an original death\n";
			}

			bprint(PR_PRINT_MEDIUM, deathstring);
			return; //- ofn
		}
	}
	else if (IsOwnedMonster(targ) && attacker->classname == "building_sentrygun")
	{
		//custom_demon_name(targ); //CH

		if ((teamplay) && (attacker->real_owner->team_no > 0) && (targ->real_owner->team_no > 0))
		{
			if (!Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, 1, 0, 2, 0, 0); //Tesla
				Give_Frags_Out(attacker->real_owner, targ, 1, 0, 1, 1, 0);

				//bprint (#PRINT_MEDIUM, targ.real_owner.netname);
				//bprint (#PRINT_MEDIUM, "'s demon, ");
				deathstring = GetMonsterName(targ);

				bprint (PR_PRINT_MEDIUM, "The ");
				bprint (PR_PRINT_MEDIUM,deathstring);
				bprint (PR_PRINT_MEDIUM, " ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") was killed by ");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s sentry gun\n");
				return;
			}
			else
			{
				Give_Frags_Out(attacker, targ, -1, 0, 2, 0, 0); //Tesla
				Give_Frags_Out(attacker->real_owner, targ, -1, 0, 1, 1, 0);

				//bprint (#PRINT_MEDIUM, targ.real_owner.netname);
				//bprint (#PRINT_MEDIUM, " friendly demon, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") was killed by ");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s friendly sentry gun\n");
				return;
			}
		}
	}
	else if (targ->classname == "monster_wizard") // OfN
	{
		//custom_demon_name(targ);
		if (attacker->classname == "building_tesla")
		{
			if ((teamplay) && (attacker->real_owner->team_no > 0) && (targ->real_owner->team_no > 0))
			{
				if (!Teammate(targ->real_owner, attacker->real_owner))
				{
					Give_Frags_Out(attacker, targ, 1, 0, 2, 0, 0); //Tesla
					Give_Frags_Out(attacker->real_owner, targ, 1, 0, 1, 1, 0);

/*					attacker.frags = attacker.frags + 1; //Tesla
					attacker.real_owner.real_frags = attacker.real_owner.real_frags + 1; //Player
					if (!(toggleflags & #TFLAG_TEAMFRAGS))
						attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
					bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s scrag, ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, ", was shocked to death by ");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s tesla sentry\n");
					return;
				}
				else
				{
					Give_Frags_Out(attacker, targ, -1, 0, 2, 0, 0); //Tesla
					Give_Frags_Out(attacker->real_owner, targ, -1, 0, 1, 1, 0);
/*
					attacker.frags = attacker.frags - 1; //Tesla
					attacker.real_owner.real_frags = attacker.real_owner.real_frags - 1; //Player
					if (!(toggleflags & #TFLAG_TEAMFRAGS))
						attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
					bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s friendly scrag, ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, ", was shocked to death by ");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s tesla sentry\n");
					return;
				}
			}
		}
#ifdef PR_COOP_MODE_ENHANCED
		else if (attacker->classname == "player" || COOP_IsCoopMonster( attacker ))
#else
		else if (attacker->classname == "player")
#endif
		{
			if ((teamplay) && (attacker->team_no > 0) && (targ->real_owner->team_no > 0))
			{
				if (Teammate(targ->real_owner, attacker))
				{
					if (attacker == targ->real_owner)
					{
						Give_Frags_Out(attacker, targ, -2, 0, 1, 1, 0);
//						attacker.real_frags = attacker.real_frags - 2; //Owner -2
						bprint (PR_PRINT_MEDIUM, attacker->netname);

						if (deathmsg == PR_DMSG_LASERCANNON)
							bprint (PR_PRINT_MEDIUM, " cuts his own scrag in half!\n");
						else
							bprint (PR_PRINT_MEDIUM, " kills his own scrag!\n");
					}
					else
					{
						Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//						attacker.real_frags = attacker.real_frags - 1; // killed a team demon

						bprint (PR_PRINT_MEDIUM, attacker->netname);

						if (deathmsg == PR_DMSG_LASERCANNON)
							bprint (PR_PRINT_MEDIUM, " blasts the friendly scrag ");
						else
							bprint (PR_PRINT_MEDIUM, " mows down the friendly scrag ");

						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, " (");
						bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
						bprint (PR_PRINT_MEDIUM, ")\n");
					}
				}
				else
				{
					Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//					attacker.real_frags = attacker.real_frags + 1;

					if (deathmsg == PR_DMSG_LASERCANNON)
					{
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " cuts ");
						bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
						bprint (PR_PRINT_MEDIUM, "'s scrag in several parts\n");
					}
					else
					{
						bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
						bprint (PR_PRINT_MEDIUM, "'s scrag, ");
						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, ", was killed by ");
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, "\n");
					}

					MonsterKill(attacker); // adds knife kill to warlock if needed
				}
//				if (!(toggleflags & #TFLAG_TEAMFRAGS))
//					attacker.frags = attacker.real_frags;
				return;
			}
		}
		else if (attacker->classname == "monster_army")
		{
			//custom_demon_name(attacker);
			GetRank(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") mows down the friendly scrag ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s scrag, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", is killed by ");
				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_demon1")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") eviscerates the friendly scrag ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s scrag, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", is returned to hell by ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_shambler")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, "The shambler ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") annihilates the friendly scrag ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s scrag, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", learns a lesson from the shambler ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_wizard")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") annihilates the colleague ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s scrag, ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, ", shows to ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") who is the best\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_fish")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, "The piranha ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") enjoys eating the friendly scrag ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s piranha, ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, ", eats ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") and enjoys doing it\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_gremlin")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, "The gremlin ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") mows down the friendly scrag ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s gremlin, ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, ", kills the scrag ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
	}//- End scrag death stuff
	else if (targ->classname == "monster_gremlin") // OfN
	{
		//custom_demon_name(targ);
		if (attacker->classname == "building_tesla")
		{
			if ((teamplay) && (attacker->real_owner->team_no > 0) && (targ->real_owner->team_no > 0))
			{
				if (!Teammate(targ->real_owner, attacker->real_owner))
				{
					Give_Frags_Out(attacker, targ, 1, 0, 2, 0, 0); //Tesla
					Give_Frags_Out(attacker->real_owner, targ, 1, 0, 1, 1, 0);

/*					attacker.frags = attacker.frags + 1; //Tesla
					attacker.real_owner.real_frags = attacker.real_owner.real_frags + 1; //Player
					if (!(toggleflags & #TFLAG_TEAMFRAGS))
						attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
					bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s gremlin, ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, ", was shocked to death by ");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s tesla sentry\n");
					return;
				}
				else
				{
					Give_Frags_Out(attacker, targ, -1, 0, 2, 0, 0); //Tesla
					Give_Frags_Out(attacker->real_owner, targ, -1, 0, 1, 1, 0);
/*
					attacker.frags = attacker.frags - 1; //Tesla
					attacker.real_owner.real_frags = attacker.real_owner.real_frags - 1; //Player
					if (!(toggleflags & #TFLAG_TEAMFRAGS))
						attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
					bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s gremlin, ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, ", was shocked to death by ");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s friendly tesla sentry\n");
					return;
				}
			}
		}
#ifdef PR_COOP_MODE_ENHANCED
		else if (attacker->classname == "player" || COOP_IsCoopMonster( attacker ))
#else
		else if (attacker->classname == "player")
#endif
		{
			if ((teamplay) && (attacker->team_no > 0) && (targ->real_owner->team_no > 0))
			{
				if (Teammate(targ->real_owner, attacker))
				{
					if (attacker == targ->real_owner)
					{
						Give_Frags_Out(attacker, targ, -2, 0, 1, 1, 0);
//						attacker.real_frags = attacker.real_frags - 2; //Owner -2
						bprint (PR_PRINT_MEDIUM, attacker->netname);

						if (deathmsg == PR_DMSG_LASERCANNON)
							bprint (PR_PRINT_MEDIUM, " cuts his own gremlin in half!\n");
						else
							bprint (PR_PRINT_MEDIUM, " kills his own gremlin!\n");
					}
					else
					{
						Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//						attacker.real_frags = attacker.real_frags - 1; // killed a team demon

						bprint (PR_PRINT_MEDIUM, attacker->netname);

						if (deathmsg == PR_DMSG_LASERCANNON)
							bprint (PR_PRINT_MEDIUM, " blasts the friendly gremlin ");
						else
							bprint (PR_PRINT_MEDIUM, " mows down the friendly gremlin ");

						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, " (");
						bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
						bprint (PR_PRINT_MEDIUM, ")\n");
					}
				}
				else
				{
					Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//					attacker.real_frags = attacker.real_frags + 1;

					if (deathmsg == PR_DMSG_LASERCANNON)
					{
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " cuts ");
						bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
						bprint (PR_PRINT_MEDIUM, "'s gremlin in several parts\n");
					}
					else
					{
						bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
						bprint (PR_PRINT_MEDIUM, "'s gremlin, ");
						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, ", was killed by ");
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, "\n");
					}

					MonsterKill(attacker); // adds knife kill to warlock if needed
				}
//				if (!(toggleflags & #TFLAG_TEAMFRAGS))
//					attacker.frags = attacker.real_frags;
				return;
			}
		}
		else if (attacker->classname == "monster_army")
		{
			//custom_demon_name(attacker);
			GetRank(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") mows down the friendly gremlin ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s gremlin, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", is killed by ");
				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_demon1")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") eviscerates the friendly gremlin ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s gremlin, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", is returned to hell by ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_shambler")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, "The shambler ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") annihilates the friendly gremlin ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s gremlin, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", learns a lesson from the shambler ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_wizard")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") kills the friendly gremlin ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s scrag, ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, ", gives death to the gremlin ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_fish")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, "The piranha ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") enjoys eating the friendly gremlin ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s piranha, ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, ", eats the gremlin");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") and enjoys doing it\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_gremlin")
		{
			if (attacker == targ)
			{
				bprint(PR_PRINT_MEDIUM,"The gremlin ");
				bprint(PR_PRINT_MEDIUM,targ->netname);
				bprint(PR_PRINT_MEDIUM," explodes with his own ammo\n");
				return;
			}

			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, "The gremlin ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") mows down his companion ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s gremlin, ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, ", beats the opposite soul ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
	}//- End gremlin death stuff
	else if (targ->classname == "monster_demon1") //CH
	{
		//custom_demon_name(targ); //CH
		if (attacker->classname == "building_tesla")
		{
			if ((teamplay) && (attacker->real_owner->team_no > 0) && (targ->real_owner->team_no > 0))
			{
				if (!Teammate(targ->real_owner, attacker->real_owner))
				{
					Give_Frags_Out(attacker, targ, 1, 0, 2, 0, 0); //Tesla
					Give_Frags_Out(attacker->real_owner, targ, 1, 0, 1, 1, 0);

/*					attacker.frags = attacker.frags + 1; //Tesla
					attacker.real_owner.real_frags = attacker.real_owner.real_frags + 1; //Player
					if (!(toggleflags & #TFLAG_TEAMFRAGS))
						attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
					bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s demon, ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, ", was shocked to death by ");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s tesla sentry\n");
					return;
				}
				else
				{
					Give_Frags_Out(attacker, targ, -1, 0, 2, 0, 0); //Tesla
					Give_Frags_Out(attacker->real_owner, targ, -1, 0, 1, 1, 0);
/*
					attacker.frags = attacker.frags - 1; //Tesla
					attacker.real_owner.real_frags = attacker.real_owner.real_frags - 1; //Player
					if (!(toggleflags & #TFLAG_TEAMFRAGS))
						attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
					bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s friendly demon, ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, ", was shocked to death by ");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s tesla sentry\n");
					return;
				}
			}
		}
#ifdef PR_COOP_MODE_ENHANCED
		else if (attacker->classname == "player" || COOP_IsCoopMonster( attacker ))
#else
		else if (attacker->classname == "player")
#endif
		{
			if ((teamplay) && (attacker->team_no > 0) && (targ->real_owner->team_no > 0))
			{
				if (Teammate(targ->real_owner, attacker))
				{
					if (attacker == targ->real_owner)
					{
						Give_Frags_Out(attacker, targ, -2, 0, 1, 1, 0);
//						attacker.real_frags = attacker.real_frags - 2; //Owner -2
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " kills his own fiend!\n");
					}
					else
					{
						Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//						attacker.real_frags = attacker.real_frags - 1; // killed a team demon

						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " mows down the friendly demon ");
						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, " (");
						bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
						bprint (PR_PRINT_MEDIUM, ")\n");
					}
				}
				else
				{
					Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//					attacker.real_frags = attacker.real_frags + 1;
					bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s demon, ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, ", was killed by ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, "\n");

					MonsterKill(attacker); // adds knife kills to warlock
				}
//				if (!(toggleflags & #TFLAG_TEAMFRAGS))
//					attacker.frags = attacker.real_frags;
				return;
			}
		}
		else if (attacker->classname == "monster_army")
		{
			//custom_demon_name(attacker);
			GetRank(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") mows down the friendly demon ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s demon, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", didn't have a prayer against ");
				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_demon1")
		{
			if (attacker == targ)
			{
				bprint(PR_PRINT_MEDIUM,"The fiend ");
				bprint(PR_PRINT_MEDIUM,targ->netname);
				bprint(PR_PRINT_MEDIUM," eats his own fireballs\n");
				return;
			}

			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") eviscerates the friendly demon ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s demon, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", realises too late that ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") wasn't just trying to be friendly\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_shambler")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") annihilates the friendly demon ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s demon, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", is given a firm farewell to the netherworld by ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_wizard")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") kills the friendly demon ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s demon, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", is killed by the scrag ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_fish")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") kills the friendly demon ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s demon, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", is killed by the piranha ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_gremlin")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") kills the friendly demon ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s demon, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", is killed by the gremlin ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
	}
	else if (targ->classname == "monster_army") //CH
	{
		//custom_demon_name(targ); //CH
		GetRank(targ);
		if (attacker->classname == "building_tesla")
		{
			if ((teamplay) && (attacker->real_owner->team_no > 0) && (targ->real_owner->team_no > 0))
			{
				if (!Teammate(targ->real_owner, attacker->real_owner))
				{
					Give_Frags_Out(attacker, targ, 1, 0, 2, 0, 0); //Tesla
					Give_Frags_Out(attacker->real_owner, targ, 1, 0, 1, 1, 0);

/*					attacker.frags = attacker.frags + 1; //Tesla
					attacker.real_owner.real_frags = attacker.real_owner.real_frags + 1; //Player
					if (!(toggleflags & #TFLAG_TEAMFRAGS))
						attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
					bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s mercenary soldier, ");
					bprint (PR_PRINT_MEDIUM, targ->undercover_name);
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, ", was shocked to death by ");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s tesla sentry\n");
					return;
				}
				else
				{
					Give_Frags_Out(attacker, targ, -1, 0, 2, 0, 0); //Tesla
					Give_Frags_Out(attacker->real_owner, targ, -1, 0, 1, 1, 0);
/*
					attacker.frags = attacker.frags - 1; //Tesla
					attacker.real_owner.real_frags = attacker.real_owner.real_frags - 1; //Player
					if (!(toggleflags & #TFLAG_TEAMFRAGS))
						attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
					bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s friendly soldier, ");
					bprint (PR_PRINT_MEDIUM, targ->undercover_name);
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, ", was shocked to death by ");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s tesla sentry\n");
					return;
				}
			}
		}
#ifdef PR_COOP_MODE_ENHANCED
		if (attacker->classname == "player" || COOP_IsCoopMonster( attacker ))
#else
		if (attacker->classname == "player")
#endif
		{
			if ((teamplay) && (attacker->team_no > 0) && (targ->real_owner->team_no > 0))
			{
				if (Teammate(targ->real_owner, attacker))
				{
					if (attacker == targ->real_owner)
					{
						Give_Frags_Out(attacker, targ, -2, 0, 1, 1, 0);
//						attacker.real_frags = attacker.real_frags - 2; //Owner -2
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " kills his own soldier!\n");
					}
					else
					{
						Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//						attacker.real_frags = attacker.real_frags - 1; // killed a team demon

						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " mows down the friendly soldier ");
						bprint (PR_PRINT_MEDIUM, targ->undercover_name);
						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, " (");
						bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
						bprint (PR_PRINT_MEDIUM, ")\n");
					}
				}
				else
				{
					Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//					attacker.real_frags = attacker.real_frags + 1;
					bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s mercenary soldier, ");
					bprint (PR_PRINT_MEDIUM, targ->undercover_name);
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, ", was killed by ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, "\n");
				}
//				if (!(toggleflags & #TFLAG_TEAMFRAGS))
//					attacker.frags = attacker.real_frags;
				return;
			}
		}
		else if (attacker->classname == "monster_shambler")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") slaps the friendly soldier ");
				bprint (PR_PRINT_MEDIUM, targ->undercover_name);
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") into oblivion\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s soldier, ");
				bprint (PR_PRINT_MEDIUM, targ->undercover_name);
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", is smacked down by ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_demon1")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") disembowels the friendly soldier ");
				bprint (PR_PRINT_MEDIUM, targ->undercover_name);
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s soldier, ");
				bprint (PR_PRINT_MEDIUM, targ->undercover_name);
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", is now listed as KIA thanks to ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_army")
		{
			//custom_demon_name(attacker);
			GetRank(attacker);

			if (targ==attacker) //- OfN - It *may* happen
			{
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s soldier, ");
				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, ", explodes with his own rockets\n");
				return;
			}

			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") mows down comrade-in-arms ");
				bprint (PR_PRINT_MEDIUM, targ->undercover_name);
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s soldier, ");
				bprint (PR_PRINT_MEDIUM, targ->undercover_name);
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", takes a bullet in the chest from ");
				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_wizard")
		{
			//custom_demon_name(attacker);
			//GetRank(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, "The scrag ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") kills the friendly soldier ");
				bprint (PR_PRINT_MEDIUM, targ->undercover_name);
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s soldier, ");
				bprint (PR_PRINT_MEDIUM, targ->undercover_name);
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", is killed by the scrag ");
				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_fish")
		{
			//custom_demon_name(attacker);
			//GetRank(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, "The piranha ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") kills the friendly soldier ");
				bprint (PR_PRINT_MEDIUM, targ->undercover_name);
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s soldier, ");
				bprint (PR_PRINT_MEDIUM, targ->undercover_name);
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", is killed by the piranha ");
				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_gremlin")
		{
			//custom_demon_name(attacker);
			//GetRank(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, "The gremlin ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") mows down the friendly soldier ");
				bprint (PR_PRINT_MEDIUM, targ->undercover_name);
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s soldier, ");
				bprint (PR_PRINT_MEDIUM, targ->undercover_name);
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", is destroyed by the gremlin ");
				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
	}
	else if (targ->classname == "monster_shambler") //CH
	{
		//custom_demon_name(targ); //CH
		if (attacker->classname == "building_tesla")
		{
			if ((teamplay) && (attacker->real_owner->team_no > 0) && (targ->real_owner->team_no > 0))
			{
				if (!Teammate(targ->real_owner, attacker->real_owner))
				{
					Give_Frags_Out(attacker, targ, 1, 0, 2, 0, 0); //Tesla
					Give_Frags_Out(attacker->real_owner, targ, 1, 0, 1, 1, 0);

/*					attacker.frags = attacker.frags + 1; //Tesla
					attacker.real_owner.real_frags = attacker.real_owner.real_frags + 1; //Player
					if (!(toggleflags & #TFLAG_TEAMFRAGS))
						attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
					bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s shambler, ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, ", was given a shocking surprise by ");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s tesla sentry\n");
					return;
				}
				else
				{
					Give_Frags_Out(attacker, targ, -1, 0, 2, 0, 0); //Tesla
					Give_Frags_Out(attacker->real_owner, targ, -1, 0, 1, 1, 0);
/*
					attacker.frags = attacker.frags - 1; //Tesla
					attacker.real_owner.real_frags = attacker.real_owner.real_frags - 1; //Player
					if (!(toggleflags & #TFLAG_TEAMFRAGS))
						attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
					bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s friendly shambler, ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, ", shouldn't play so close to ");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s tesla sentry\n");
					return;
				}
			}
		}
#ifdef PR_COOP_MODE_ENHANCED
		if (attacker->classname == "player" || COOP_IsCoopMonster( attacker ))
#else
		if (attacker->classname == "player")
#endif
		{
			if ((teamplay) && (attacker->team_no > 0) && (targ->real_owner->team_no > 0))
			{
				if (Teammate(targ->real_owner, attacker))
				{
					if (attacker == targ->real_owner)
					{
						Give_Frags_Out(attacker, targ, -2, 0, 1, 1, 0);
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " kills his own shambler!\n");
//						attacker.real_frags = attacker.real_frags - 2; //Owner -2
					}
					else
					{
						Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//						attacker.real_frags = attacker.real_frags - 1; // killed a team demon

						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " mows down the friendly shambler ");
						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, " (");
						bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
						bprint (PR_PRINT_MEDIUM, ")\n");
					}
				}
				else
				{
					Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//					attacker.real_frags = attacker.real_frags + 1;
					bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s shambler, ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, ", was felled by ");
					bprint (PR_PRINT_MEDIUM, attacker->netname);
					bprint (PR_PRINT_MEDIUM, "\n");

					MonsterKill(attacker); // adds knife kills to warlock
				}
//				if (!(toggleflags & #TFLAG_TEAMFRAGS))
//					attacker.frags = attacker.real_frags;
				return;
			}
		}
		else if (attacker->classname == "monster_demon1")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") checks its aura after killing ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s shambler, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", is torn apart by ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_shambler")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") electrifies the friendly shambler ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s shambler, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", finds losing against shambler ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") to be a fatal experience\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_army")
		{
			//custom_demon_name(attacker);
			GetRank(attacker);
			if ( Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") is caught in the crossfire from ");
				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") proves to ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") that size doesn't matter\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_wizard")
		{
			//custom_demon_name(attacker);
			//GetRank(attacker);

			// condition reversed forconvenience
			if (!Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, "The scrag ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") kills the surprised shambler ");
				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, "The scrag ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") kills the friendly shambler ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_fish")
		{
			//custom_demon_name(attacker);
			//GetRank(attacker);

			// condition reversed forconvenience
			if (!Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, "The piranha ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") kills weirdly the shambler ");
				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, "The piranha ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") kills the friendly shambler ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_gremlin")
		{
			//custom_demon_name(attacker);
			//GetRank(attacker);

			// condition reversed forconvenience
			if (!Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, "The gremlin ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") destroys the shambler ");
				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, "The gremlin ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") mows down the friendly shambler ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}

	}
	else if (targ->classname == "monster_fish") //CH
	{
		//custom_demon_name(targ); //CH
		if (attacker->classname == "building_tesla")
		{
			if ((teamplay) && (attacker->real_owner->team_no > 0) && (targ->real_owner->team_no > 0))
			{
				if (!Teammate(targ->real_owner, attacker->real_owner))
				{
					Give_Frags_Out(attacker, targ, 1, 0, 2, 0, 0); //Tesla
					Give_Frags_Out(attacker->real_owner, targ, 1, 0, 1, 1, 0);

/*					attacker.frags = attacker.frags + 1; //Tesla
					attacker.real_owner.real_frags = attacker.real_owner.real_frags + 1; //Player
					if (!(toggleflags & #TFLAG_TEAMFRAGS))
						attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
					bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s piranha, ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, ", was electrocuted by ");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s tesla sentry\n");
					return;
				}
				else
				{
					Give_Frags_Out(attacker, targ, -1, 0, 2, 0, 0); //Tesla
					Give_Frags_Out(attacker->real_owner, targ, -1, 0, 1, 1, 0);
/*
					attacker.frags = attacker.frags - 1; //Tesla
					attacker.real_owner.real_frags = attacker.real_owner.real_frags - 1; //Player
					if (!(toggleflags & #TFLAG_TEAMFRAGS))
						attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
					bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s friendly piranha, ");
					bprint (PR_PRINT_MEDIUM, targ->netname);
					bprint (PR_PRINT_MEDIUM, ", shouldn't play so close to ");
					bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
					bprint (PR_PRINT_MEDIUM, "'s tesla sentry\n");
					return;
				}
			}
		}
#ifdef PR_COOP_MODE_ENHANCED
		if (attacker->classname == "player" || COOP_IsCoopMonster( attacker ))
#else
		if (attacker->classname == "player")
#endif
		{
			if ((teamplay) && (attacker->team_no > 0) && (targ->real_owner->team_no > 0))
			{
				if (Teammate(targ->real_owner, attacker))
				{
					if (attacker == targ->real_owner)
					{
						Give_Frags_Out(attacker, targ, -2, 0, 1, 1, 0);
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " kills his own piranha!\n");
//						attacker.real_frags = attacker.real_frags - 2; //Owner -2
					}
					else
					{
						Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//						attacker.real_frags = attacker.real_frags - 1; // killed a team demon

						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " kills the friendly piranha ");
						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, " (");
						bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
						bprint (PR_PRINT_MEDIUM, ")\n");
					}
				}
				else
				{
					Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);

					if (rnum < 0.5)
					{
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " returns ");
						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, " (");
						bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
						bprint (PR_PRINT_MEDIUM, ") to the aquarium\n");
					}
					else
					{
						bprint (PR_PRINT_MEDIUM, attacker->netname);
						bprint (PR_PRINT_MEDIUM, " kills the piranha ");
						bprint (PR_PRINT_MEDIUM, targ->netname);
						bprint (PR_PRINT_MEDIUM, " (");
						bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
						bprint (PR_PRINT_MEDIUM, ")\n");
					}

					MonsterKill(attacker); // adds knife kills to warlock
				}
//				if (!(toggleflags & #TFLAG_TEAMFRAGS))
//					attacker.frags = attacker.real_frags;
				return;
			}
		}
		else if (attacker->classname == "monster_demon1")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") checks its aura after killing ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s piranha, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", is killed evilly by ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_shambler")
		{
			//custom_demon_name(attacker);
			if (Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") kills the friendly piranha ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s piranha, ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, ", isn't enough for the shambler ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_army")
		{
			//custom_demon_name(attacker);
			GetRank(attacker);
			if ( Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") is caught in the crossfire from ");
				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") kills the piranha ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_fish")
		{
			//custom_demon_name(attacker);
			//GetRank(attacker);

			// condition reversed forconvenience
			if (!Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, "The piranha ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") kills ");
				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") after an underwater fight\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, "The piranha ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") kills its friendly companion fish ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_wizard")
		{
			//custom_demon_name(attacker);
			//GetRank(attacker);

			// condition reversed forconvenience
			if (!Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, "The scrag ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") kills the piranha ");
				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, "The scrag ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") kills the friendly piranha ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}
		else if (attacker->classname == "monster_gremlin")
		{
			//custom_demon_name(attacker);
			//GetRank(attacker);

			// condition reversed forconvenience
			if (!Teammate(targ->real_owner, attacker->real_owner))
			{
				Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags - 1; // killed a team demon

				bprint (PR_PRINT_MEDIUM, "The gremlin ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") kills the piranha ");
				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
			else
			{
				Give_Frags_Out(attacker, targ, -1, 0, 1, 1, 0);
//				attacker.real_frags = attacker.real_frags + 1;
				//bprint (#PRINT_MEDIUM, attacker.undercover_name);
				bprint (PR_PRINT_MEDIUM, "The gremlin ");
				bprint (PR_PRINT_MEDIUM, attacker->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ") mows down the friendly piranha ");
				bprint (PR_PRINT_MEDIUM, targ->netname);
				bprint (PR_PRINT_MEDIUM, " (");
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, ")\n");
			}
//			if (!(toggleflags & #TFLAG_TEAMFRAGS))
//				attacker.frags = attacker.real_frags;
		return;
		}

	}
	else if (IsBuilding(targ) && targ->classname != "building_sentrygun_base" && attacker->classname == "building_tesla") //Evil Tesla!
	{
		if (!Teammate(targ->real_owner, attacker->real_owner))
		{
			if (targ->classname == "building_sentrygun" ||
				targ->classname == "building_tesla")
			{
				Give_Frags_Out(attacker, targ, 1, 0, 2, 0, 0); //Tesla
				Give_Frags_Out(attacker->real_owner, targ, 1, 0, 1, 1, 0);
/*
				attacker.frags = attacker.frags + 1; //Tesla
				attacker.real_owner.real_frags = attacker.real_owner.real_frags + 1; //Player
				if (!(toggleflags & #TFLAG_TEAMFRAGS))
					attacker.real_owner.frags = attacker.real_owner.real_frags;
*/
			}
		}
		bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
		bprint (PR_PRINT_MEDIUM, "'s ");
			if (targ->classname == "building_dispenser")
				bprint (PR_PRINT_MEDIUM, "dispenser was zapped by");
			else if (targ->classname == "building_sentrygun")
				bprint (PR_PRINT_MEDIUM, "sentrygun was no match for");
			else if (targ->classname == "building_camera")
				bprint (PR_PRINT_MEDIUM, "security camera was vaporized by");
			else if (targ->classname == "building_sensor")
				bprint (PR_PRINT_MEDIUM, "motion sensor was obliterated by");
			else if (targ->classname == "building_teleporter")
				bprint (PR_PRINT_MEDIUM, "teleporter pad was short circuted by");
			else if (targ->classname == "building_tesla" && attacker == targ)
			{
				if (deathmsg == PR_DMSG_EARTHQUAKE)
				{
					bprint(PR_PRINT_MEDIUM,"tesla is destroyed by the earthquake!\n");
					return;
				}

				if (deathmsg == PR_DMSG_STORM)
				{
					bprint(PR_PRINT_MEDIUM,"tesla is destroyed by the storm lightning!\n");
					return;
				}

				bprint (PR_PRINT_MEDIUM, "tesla coil destroys itself.\n");
				return;
			}
			else if (targ->classname == "building_tesla")
				bprint (PR_PRINT_MEDIUM, "tesla sentry lost the battle against");
			else if (targ->classname == "building_sensor")
				bprint (PR_PRINT_MEDIUM, "motion sensor was fucked up by");
			else if (targ->classname == "building_fieldgen")
				bprint (PR_PRINT_MEDIUM, "field generator was destroyed by");
			else
				bprint (PR_PRINT_MEDIUM, "??Building??");
		bprint (PR_PRINT_MEDIUM, " ");
		bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
		bprint (PR_PRINT_MEDIUM, "'s tesla sentry\n");
		return;
	}
	else if (IsBuilding(targ) && targ->classname != "building_sentrygun_base")
	{
		//- OfN - Weird but may happen.. like when a sentry is blown up by its own rocket
		if (attacker == targ)
		{
			if (deathmsg == PR_DMSG_EARTHQUAKE)
			{
				tst = GetBuildingName(targ);
				bprint(PR_PRINT_MEDIUM,targ->real_owner->netname,"'s ",tst," is destroyed by the earthquake!\n");
				return;
			}

			if (deathmsg == PR_DMSG_STORM)
			{
				tst = GetBuildingName(targ);
				bprint(PR_PRINT_MEDIUM,targ->real_owner->netname,"'s ",tst," is destroyed by the storm lightning!\n");
				return;
			}

			bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
			bprint (PR_PRINT_MEDIUM, "'s ");

				if (targ->classname == "building_dispenser")
					bprint (PR_PRINT_MEDIUM, "dispenser");
				else if (targ->classname == "building_sentrygun")
					bprint (PR_PRINT_MEDIUM, "sentrygun");
				else if (targ->classname == "building_camera")
					bprint (PR_PRINT_MEDIUM, "security camera");
				else if (targ->classname == "building_teleporter")
					bprint (PR_PRINT_MEDIUM, "teleporter pad");
				else if (targ->classname == "building_tesla")
					bprint (PR_PRINT_MEDIUM, "tesla sentry");
				else if (targ->classname == "building_sensor")
					bprint (PR_PRINT_MEDIUM, "motion sensor");
				else if (targ->classname == "building_fieldgen")
					bprint (PR_PRINT_MEDIUM, "field generator");
				else
					bprint (PR_PRINT_MEDIUM, "??Building??");

			bprint (PR_PRINT_MEDIUM, " destroys itself\n");
			return;
		}

		if (attacker->classname == "teledeath")
		{
			bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
			bprint (PR_PRINT_MEDIUM, "'s ");

			//bprint (#PRINT_MEDIUM,GetBuildingName(targ));
				if (targ->classname == "building_dispenser")
					bprint (PR_PRINT_MEDIUM, "dispenser");
				else if (targ->classname == "building_sentrygun")
					bprint (PR_PRINT_MEDIUM, "sentrygun");
				else if (targ->classname == "building_camera")
					bprint (PR_PRINT_MEDIUM, "security camera");
				else if (targ->classname == "building_teleporter")
					bprint (PR_PRINT_MEDIUM, "teleporter pad");
				else if (targ->classname == "building_tesla")
					bprint (PR_PRINT_MEDIUM, "tesla sentry");
				else if (targ->classname == "building_sensor")
					bprint (PR_PRINT_MEDIUM, "motion sensor");
				else if (targ->classname == "building_fieldgen")
					bprint (PR_PRINT_MEDIUM, "field generator");
				else
					bprint (PR_PRINT_MEDIUM, "??Building??");

			bprint (PR_PRINT_MEDIUM, " was telefragged by ");
			bprint (PR_PRINT_MEDIUM, attacker->owner->netname);
			bprint (PR_PRINT_MEDIUM, "\n");
			return;
		}

#ifdef PR_COOP_MODE_ENHANCED
		if (attacker->classname == "player" || COOP_IsCoopMonster( attacker ))
#else
		if (attacker->classname == "player")
#endif
		{
			if (attacker == targ->real_owner)
			{
				bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
				bprint (PR_PRINT_MEDIUM, " destroys his ");

				//bprint (#PRINT_MEDIUM,GetBuildingName(targ));
				if (targ->classname == "building_dispenser")
					bprint (PR_PRINT_MEDIUM, "dispenser");
				else if (targ->classname == "building_sentrygun")
					bprint (PR_PRINT_MEDIUM, "sentrygun");
				else if (targ->classname == "building_camera")
					bprint (PR_PRINT_MEDIUM, "security camera");
				else if (targ->classname == "building_teleporter")
					bprint (PR_PRINT_MEDIUM, "teleporter pad");
				else if (targ->classname == "building_tesla")
					bprint (PR_PRINT_MEDIUM, "tesla sentry");
				else if (targ->classname == "building_sensor")
					bprint (PR_PRINT_MEDIUM, "motion sensor");
				else if (targ->classname == "building_fieldgen")
					bprint (PR_PRINT_MEDIUM, "field generator");
				else
					bprint (PR_PRINT_MEDIUM, "??Building??");
				bprint (PR_PRINT_MEDIUM, "\n");
				return;
			}

			bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
			bprint (PR_PRINT_MEDIUM, "'s ");

			//bprint (#PRINT_MEDIUM,GetBuildingName(targ)); //- OfN - FIXED?

			if (targ->classname == "building_dispenser")
				bprint (PR_PRINT_MEDIUM, "dispenser");
			else if (targ->classname == "building_sentrygun")
				bprint (PR_PRINT_MEDIUM, "sentrygun");
			else if (targ->classname == "building_camera")
				bprint (PR_PRINT_MEDIUM, "security camera");
			else if (targ->classname == "building_teleporter")
				bprint (PR_PRINT_MEDIUM, "teleporter pad");
			else if (targ->classname == "building_tesla")
				bprint (PR_PRINT_MEDIUM, "tesla sentry");
			else if (targ->classname == "building_sensor")
				bprint (PR_PRINT_MEDIUM, "motion sensor");
			else if (targ->classname == "building_fieldgen")
				bprint (PR_PRINT_MEDIUM, "field generator");
			else
				bprint (PR_PRINT_MEDIUM, "??Building??");

			bprint (PR_PRINT_MEDIUM, " was destroyed by ");
			bprint (PR_PRINT_MEDIUM, attacker->netname);
			bprint (PR_PRINT_MEDIUM, "\n");

			if (!Teammate(targ->real_owner, attacker))
			{
				if (targ->classname == "building_sentrygun" ||
					targ->classname == "building_tesla")
				{
					Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
/*
					attacker.real_frags = attacker.real_frags + 1;
					if (!(toggleflags & #TFLAG_TEAMFRAGS))
						attacker.frags = attacker.real_frags;
*/
				}
			}

			return;
		}
		else if (attacker->classname == "monster_army") // SB grunty kills building
		{
			//custom_demon_name(attacker);
			GetRank(attacker);

			bprint (PR_PRINT_MEDIUM, targ->real_owner->netname);
			bprint (PR_PRINT_MEDIUM, "'s ");

			//bprint(#PRINT_MEDIUM,GetBuildingName(targ));
			if (targ->classname == "building_dispenser")
				bprint (PR_PRINT_MEDIUM, "dispenser");
			else if (targ->classname == "building_sentrygun")
				bprint (PR_PRINT_MEDIUM, "sentrygun");
			else if (targ->classname == "building_camera")
				bprint (PR_PRINT_MEDIUM, "security camera");
			else if (targ->classname == "building_teleporter")
				bprint (PR_PRINT_MEDIUM, "teleporter pad");
			else if (targ->classname == "building_tesla")
				bprint (PR_PRINT_MEDIUM, "tesla sentry");
			else if (targ->classname == "building_sensor")
				bprint (PR_PRINT_MEDIUM, "motion sensor");
			else if (targ->classname == "building_fieldgen")
				bprint (PR_PRINT_MEDIUM, "field generator");
			else
				bprint (PR_PRINT_MEDIUM, "??Building??");

			bprint (PR_PRINT_MEDIUM, " was destroyed by ");
			bprint (PR_PRINT_MEDIUM, attacker->undercover_name);
			bprint (PR_PRINT_MEDIUM, attacker->netname);
			bprint (PR_PRINT_MEDIUM, " (");
			bprint (PR_PRINT_MEDIUM, attacker->real_owner->netname);
			bprint (PR_PRINT_MEDIUM, ")\n");

			if (!Teammate(targ->real_owner, attacker->real_owner))
			{
				if (targ->classname == "building_sentrygun" ||
					targ->classname == "building_tesla")
				{
					Give_Frags_Out(attacker, targ, 1, 0, 1, 1, 0);
/*
					attacker.real_frags = attacker.real_frags + 1;
					if (!(toggleflags & #TFLAG_TEAMFRAGS))
						attacker.frags = attacker.real_frags;
*/
				}
			}
			return;
		}
	}

	//- Miscellaneous monster/army deaths
#ifdef PR_COOP_MODE_ENHANCED
	else if (IsOwnedMonster(targ)) {
#else
	if (IsOwnedMonster(targ))
	{
#endif
		if (attacker->classname == "explo_box")
		{
			deathstring = " blew up\n";
		}
		else if (attacker->solid == PR_SOLID_BSP && attacker != world)
		{
			deathstring = " was squished\n";
		}
		else if (attacker->classname == "trap_shooter" || attacker->classname == "trap_spikeshooter" || attacker->classname == "trap_tf_spikeshooter" || attacker->classname == "trap_tf_shooter")
		{
			if (attacker->deathtype != string_null)
				deathstring = attacker->deathtype;
			else if (attacker->spawnflags == PR_SPAWNFLAG_LASER)
				deathstring = " had surgery from a laser\n";
			else if (attacker->spawnflags == PR_SPAWNFLAG_TFROCKET)
				deathstring = " took a ride on a rocket\n";
			else if (attacker->spawnflags == PR_SPAWNFLAG_TFGRENADE)
				deathstring = " tried to eat a wild pineapple\n";
			else if (attacker->spawnflags == PR_SPAWNFLAG_TFFLAME)
				deathstring = " was burned alive\n";
			else if (attacker->spawnflags == PR_SPAWNFLAG_SUPERSPIKE)
				deathstring = " was nailed to the ground\n";
			else
				deathstring = " was spiked\n";
		}
		else if (attacker->classname == "fireball")
		{
			deathstring = " ate a lavaball\n";
		}
		else if (attacker->classname == "teledeath")
		{
			deathstring = " was telefragged by ";
		}
		else
		{
			deathstring = " dies\n";
		}

		deathstring2=GetMonsterName(targ);

		bprint(PR_PRINT_HIGH,"The ");
		bprint(PR_PRINT_HIGH, deathstring2);
		bprint(PR_PRINT_HIGH," ");
		bprint(PR_PRINT_HIGH, targ->netname);
		bprint(PR_PRINT_HIGH," (");
		bprint(PR_PRINT_HIGH, targ->real_owner->netname);
		bprint(PR_PRINT_HIGH,")");
		bprint(PR_PRINT_HIGH, deathstring);
		if (attacker->classname == "teledeath")
		{
			bprint(PR_PRINT_HIGH, attacker->owner->netname);
			bprint(PR_PRINT_HIGH,"\n");
		}

		return;
	}
}

} // END namespace Progs
