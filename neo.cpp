// Module: Neo Mode                                                    Version:  January 29, 2009
// Author: Pulseczar                                Ported to CuTF 4TP and C++:   August 25, 2020
//
// Based on an idea by =STG=Gnarler and [cp]Snake, where one player is given all four runes.
//
// Everyone is on the Agent team, except Neo; he's by himself. Neo alternates from player to
// player, such that the first player to get a kill (starts in deathmatch mode) becomes Neo, and
// from there on, every time a player kills Neo, the player becomes Neo. Neo has the equivalent
// property of having all CustomTF runes at once (triple damage, damage resistance, regeneration,
// faster firing, and increased movement speed), though Neo's powers vary according to the number
// of players in the server. However, the speed rune does not scale according to the number of
// players.
//
// There are no flags in Neo mode. The sole objective is to be Neo as much as you can and keep
// others from being Neo, while fighting the onslaught of Agents.
//
// Using the 'inv' command will tell you what Neo's enhancement stats are, which change according
// to the number of players.
//
// Other rules:
// > Neo can't use the thief skill.
// > Neo can't hide from other players. Doors and teleporters are open to all players.
// > If Neo kills himself or the environment kills him, he becomes an Agent and the first player
//   to kill another player becomes Neo.
// > Neo can't use resupply packs. Grenades and detpacks have been made to slowly regenerate
//   along with the usual regeneration.
// > No one can change their color with the spykit.
// > Agents get a lightning bolt indication of the direction towards Neo.
//
// I tried to fit everything I could into this file. The things that could not go here can be
// found by searching for 'neo', case-insensitive.
//
// This code uses the already-existing rune code by Often. Furthermore, runes cannot be added by
// admins to a game of Neo, because, currently, adding runes will not work right with Neo mode.
//
// Thanks to Major Major Major Major for most of the event messages.
//
// TODO:
// -How about making bots out of grunts to play with in Neo mode? (Yay! Don't need this anymore!)
//   -Rip navigation code from Reaper bot or something?
// -Give multiple frags for killing Neo?
//   -Speaking of killing Neo, shouldn't the player that did the most damage to Neo become Neo
//    when Neo dies, not just the player that scored the final hit? Also, perhaps it should be
//    random who becomes Neo, since it is debatable who should become Neo, I suppose. Or maybe a
//    switchable setting between the two options.
// -Verify that teleporters work right (let everyone pass through).
// -Check that when armor reaches 0 and regen adds more to it, that the armor actually works and
//  isn't disabled by something when it hits 0.
// -Is there a way to allow Neo to customize or change classes? It really sucks to use a high
//  damage class to become Neo, only to be stuck using that class once you are Neo, since you
//  don't need so much damage anymore, and it limits what you can do. Allow Neo a moment to change
//  classes. Neo should only be allowed to use 'kill' every so often. And it only allows the
//  player to stay Neo if they are changing class or customizing.
//
// DONE:
// -Rename everywhere that "Smith" is written to the more-generic "Agent"?

#include "progs.h"
#include "neo.h"
#include "cpstuff.h"
#include "spectate.h"
#include "jobs.h"
#include "admin.h"
#include "runes.h"
#include "warlock.h"

namespace Progs {

// The Neo mode object.
Neo neo;

//=================================================================================================
// Called by the localinfo key initializing function.
//=================================================================================================
void Neo::initialize()
{
	string st;

	timeLastLightningDisplayed = 0;

	st = infokey(world, "neo");
	if (st == string_null) st = "0"; // sets default
	modeActive = stof(st);
	if (modeActive != 1) modeActive = 0; // has to be set to 1 to engage Neo mode

	neoExists = world;

	if (modeActive)
	{
		// basically just makes sure friendly-fire is on 
		// and team half damage and such is off
		oldTeamplayValue = cvar("teamplay");
		cvar_set("teamplay", "1");

		st = infokey(world, "neo_base_dmg");
		if (st == string_null) st = "1"; // sets default
		neo_base_dmg = stof(st);

		st = infokey(world, "neo_dmg_factor");
		if (st == string_null) st = "0.5"; // sets default
		neo_dmg_factor = stof(st);

		st = infokey(world, "neo_base_res");
		if (st == string_null) st = "1"; // sets default  (changed from 0.8. 8-30-2020)
		neo_base_res = stof(st);

		// currently not being used in resist calculation
		st = infokey(world, "neo_res_factor");
		if (st == string_null) st = "0.016"; // sets default
		neo_res_factor = stof(st);

		st = infokey(world, "neo_base_regen");
		if (st == string_null) st = "0.4"; // sets default
		neo_base_regen = stof(st);

		st = infokey(world, "neo_regen_factor");
		if (st == string_null) st = "0.15"; // sets default
		neo_regen_factor = stof(st);
	}

	lastNadeRegen = lastDetRegen = timeLastLightningDisplayed = 0;
	neoCanChangeClass = true;
	neoFragsAtLastClassChange = neoTimeAtLastClassChange = 0;
}
//=================================================================================================
// Alerts joining players of Neo mode being active.
//=================================================================================================
void Neo::introducePlayer()
{
	sprint(self, PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^> ^bNeo Mode^b Enabled ^<^-^-^-^-^-^-^-^>\n"));
	//DELETE this once out of 'test period'
	string temp; 
	temp = colstr("This is a TEST version.\nPlease report bugs to " + CUTF_URL + ".\n", PR_COLSTR_RED);
	sprint(self, PR_PRINT_HIGH, temp);
	//DELETE end
	sprint(self, PR_PRINT_HIGH, string("Welcome to the Matrix, Agent ") + self->netname + ".\n");
	if (neoExists != world)
	{
		sprint(self, PR_PRINT_HIGH, neoExists->netname);
		sprint(self, PR_PRINT_HIGH, " is currently Neo.\n");
		sprint(self, PR_PRINT_HIGH, "Find and destroy him! (look for gibs amongst a blue haze)\n");
	}
	else
		sprint(self, PR_PRINT_HIGH, "First player to kill another player becomes Neo.\n");

	sprint(self, PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
	stuffcmd(self, "play boss1/sight1.wav\n");
}
//=================================================================================================
// This gets called every frame in StartFrame(). Does general stuff for Neo Mode.
//=================================================================================================
void Neo::mainThink()
{
	// Calculate Neo's "buffs".
	calcNeoFactors();

	if (neoExists != world)
	{
		// Determine whether Neo can change class at the moment.
		if (neoExists->real_frags - neoFragsAtLastClassChange >= NEO_KILLS_FOR_CLASS_CHANGE)
			neoCanChangeClass = true;
		if (time - neoTimeAtLastClassChange >= NEO_MINUTES_FOR_CLASS_CHANGE * 60)
			neoCanChangeClass = true;

		// For all non-bot Agents, give the player a visual indication of the direction towards Neo, when Neo exists.
		entity player;
		vector vecToNeo, dirToNeo, lightningVec, src;
		if (time >= timeLastLightningDisplayed + 0.5) // show it every half second
		{
			player = find(world, "classname", "player");
			while (player != world)
			{
				if (player->ishuman && player != neoExists)
				{
					// PZ NOTE: I took this from scanner code to duplicate scanner effect (tweaked it some).
					vecToNeo = neoExists->origin - player->origin;
					dirToNeo = normalize(vecToNeo); // direction, unit vector
					// Only give visual indication when player can't see Neo due to an obstruction. (Took this part from TeamFortress_ID().)
					src = player->origin + dirToNeo*10;
					src[Z] = player->absmin[Z] + player->size[Z] * 0.7; // PZ NOTE: Can't we just use |player.view_ofs[Z]| here?
					traceline(src, src + dirToNeo*2048, PR_TL_ANY_SOLID, player);
					// If it traces to any other entities, skip over them to see if we can see Neo past them.
					while (trace_ent != world)
					{
						if (trace_ent == neoExists) break;      // We found Neo. So we don't need to keep searching for Neo in line-of-sight.
						src = trace_ent->origin + dirToNeo*10;
						traceline(src, src + dirToNeo*2048, PR_TL_ANY_SOLID, trace_ent);
					}
					if (trace_ent != neoExists)
					{
						lightningVec  = dirToNeo * (vlen(vecToNeo) / 5);
						lightningVec += player->origin;
						// Create the lightning.
						msg_entity = player;
						WriteByte(PR_MSG_ONE, PR_SVC_TEMPENTITY);
						WriteByte(PR_MSG_ONE, PR_TE_LIGHTNING1);
						WriteEntity(PR_MSG_ONE, player);
						WriteCoord(PR_MSG_ONE, player->origin[X]);
						WriteCoord(PR_MSG_ONE, player->origin[Y]);
						WriteCoord(PR_MSG_ONE, player->origin[Z] + 8);
						WriteCoord(PR_MSG_ONE, lightningVec[X]);
						WriteCoord(PR_MSG_ONE, lightningVec[Y]);
						WriteCoord(PR_MSG_ONE, lightningVec[Z] + 8);
						timeLastLightningDisplayed = time;
					}
				}
				player = find(player, "classname", "player");
			}
		}
	}

	// Perform error checking.
	errorCheck();
}
//=================================================================================================
// Calculates global Neo factor values (for scaling Neo's power according to number of players).
//=================================================================================================
void Neo::calcNeoFactors()
{
	float playerCount;
	playerCount = GetNoPlayers(PR_TRUE, PR_TRUE);

	/**************************
	  calculate damage factor
	**************************/
	neoDamageFactor = neo_base_dmg + (playerCount * neo_dmg_factor);

	/******************************
	  calculate resistance factor
	  - simplified: neo_base_res * (1 - (playerCount / 32))
	******************************/
	//neoResistFactor = 1 - (playerCount / 32);
	//neoResistFactor = neo_base_res * neoResistFactor;
	neoResistFactor = neo_base_res * (1 - (playerCount / 32));
	if (neoResistFactor < 0.05)
		neoResistFactor = 0.05; // make sure he never ends up impossible to kill because of Resist

	/********************************
	  calculate regeneration factor
	  - simplified: neo_base_regen + (playerCount * neo_regen_factor)
	********************************/
	// NOTE: I'm doing all the multiplying and dividing by 1000 because the server will round the float values, otherwise.
	//   --> This was probably a QuakeC thing..
	//neoRegenFactor = playerCount * (neo_regen_factor * 1000);
	//neoRegenFactor = (neo_base_regen * 1000) + neoRegenFactor;
	//neoRegenFactor = neoRegenFactor / 1000;
	// No longer need to do things the dumb way for QuakeC...
	//neoRegenFactor = neo_base_regen + (playerCount * neo_regen_factor);
	// Neo is too hard to kill with a lot of players. Currently there is no added regeneration with player count.
	neoRegenFactor = 1;
}
//=================================================================================================
// Called in obits every time a player dies. Handles death events.
//=================================================================================================
void Neo::deathThink(entity targ, entity attacker)
{
	if (!modeActive) return;

	float rnum, playerCount;
	entity oldSelf;
	entity realAttacker;

	playerCount = GetNoPlayers(PR_FALSE, PR_FALSE);
	rnum = random();

	/*
	there's no Neo
	*/

	if (neoExists == world && targ->classname == "player")
	{
		// something killed an Agent
		if (attacker->classname == "player" && attacker != targ)
		{
			// an Agent killed an Agent; make killer Neo
			realAttacker = attacker;
		}
		else if ((attacker->flags & PR_FL_MACHINE || attacker->flags & PR_FL_MONSTER)
		         && attacker->real_owner != targ)
		{
			// an Agent's object killed an Agent; make the object's owner Neo
			realAttacker = attacker->real_owner;
		}
		else
			return; // keep it from executing the following

		bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^-^-^-^-^-^-^> ^bNeo Mode^b ^<^-^-^-^-^-^-^-^-^-^>\n"));
		if      (rnum < 0.333)
		{
			bprint(PR_PRINT_HIGH, realAttacker->netname); bprint(PR_PRINT_HIGH, " knows kung fu!\n");
		}
		else if (rnum < 0.666)
		{
			bprint(PR_PRINT_HIGH, realAttacker->netname); bprint(PR_PRINT_HIGH, " frees his mind!\n");
		}
		else // rnum >= 0.666
		{
			bprint(PR_PRINT_HIGH, realAttacker->netname); 
			bprint(PR_PRINT_HIGH, " learns that there is no spoon!\n");
		}
		bprint(PR_PRINT_HIGH, realAttacker->netname); bprint(PR_PRINT_HIGH, " becomes Neo.\n");
		bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
		oldSelf = self;
		assignNeo(realAttacker);
		self = oldSelf;
		BroadcastSound("boss1/sight1.wav");
		rnum = random();
		if (rnum < (1 / playerCount) * WHOA_LIKELINESS)
			stuffcmd(realAttacker, "say Whoa..\n");
		return;
	}

	/*
	Neo exists
	*/

	if (neoExists != world)
	{
		// Neo was killed
		if (targ == neoExists)
		{
			// a player killed Neo
			if (attacker->classname == "player")
			{
				// Neo killed himself; make him an Agent
				if (attacker == targ)
				{
					removeNeo(attacker);
					bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^-^-^-^-^-^-^> ^bNeo Mode^b ^<^-^-^-^-^-^-^-^-^-^>\n"));
					bprint(PR_PRINT_HIGH, "Neo ("); bprint(PR_PRINT_HIGH, attacker->netname);
					bprint(PR_PRINT_HIGH, ") learns that the body cannot live\nwithout the mind.");
					bprint(PR_PRINT_HIGH, " Kill someone to become Neo!\n");
					bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
					BroadcastSound("boss1/sight1.wav");
					this->centerPrint();
					return;
				}
				// an Agent killed Neo; make the Agent Neo
				else
				{
					bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^-^-^-^-^-^-^> ^bNeo Mode^b ^<^-^-^-^-^-^-^-^-^-^>\n"));
					if (rnum < 0.5)
					{
						bprint(PR_PRINT_HIGH, targ->netname); bprint(PR_PRINT_HIGH, " has been deleted.\n");
					}
					else
					{
						bprint(PR_PRINT_HIGH, "Bullet-time was not slow enough for Neo ("); 
						bprint(PR_PRINT_HIGH, targ->netname); bprint(PR_PRINT_HIGH, ").\n");
					}
					bprint(PR_PRINT_HIGH, attacker->netname); bprint(PR_PRINT_HIGH, " is... THE ONE!\n");
					bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
					oldSelf = self;
					assignNeo(attacker);
					self = oldSelf;
					BroadcastSound("boss1/sight1.wav");
					rnum = random();
					if (rnum < (1 / playerCount) * WHOA_LIKELINESS)
						stuffcmd(attacker, "say Whoa..\n");
					return;
				}
			}

			// Neo was killed by an engineer building.
			else if (attacker->classname == "building_sentrygun"  ||
			         attacker->classname == "building_tesla"      ||
			         attacker->classname == "building_fieldgen"   ||
			         attacker->classname == "force_field"         ||   // not sure if this is necessary
			         attacker->classname == "building_dispenser")
			{
				// Neo's own building killed him. Make Neo an Agent.
				if (attacker->real_owner == targ)
				{
					removeNeo(targ);
					bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^-^-^-^-^-^-^> ^bNeo Mode^b ^<^-^-^-^-^-^-^-^-^-^>\n"));
					bprint(PR_PRINT_HIGH, "Neo ("); bprint(PR_PRINT_HIGH, targ->netname); 
					bprint(PR_PRINT_HIGH, ") was killed by his own devices!\n");
					bprint(PR_PRINT_HIGH, "Kill someone to become Neo!\n");
					bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
					BroadcastSound("boss1/sight1.wav");
					this->centerPrint();
					return;
				}
				// an Agent's building killed Neo; make the Agent Neo
				else
				{
					removeNeo(targ);
					bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^-^-^-^-^-^-^> ^bNeo Mode^b ^<^-^-^-^-^-^-^-^-^-^>\n"));
					bprint(PR_PRINT_HIGH, "Neo ("); bprint(PR_PRINT_HIGH, targ->netname); 
					bprint(PR_PRINT_HIGH, ") was killed by "); bprint(PR_PRINT_HIGH, attacker->real_owner->netname);
					bprint(PR_PRINT_HIGH, "'s building!\n");
					bprint(PR_PRINT_HIGH, attacker->real_owner->netname); bprint(PR_PRINT_HIGH, " becomes Neo!\n");
					bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
					oldSelf = self;
					assignNeo(attacker->real_owner);
					self = oldSelf;
					BroadcastSound("boss1/sight1.wav");
					rnum = random();
					if (rnum < (1 / playerCount) * WHOA_LIKELINESS)
						stuffcmd(attacker->real_owner, "say Whoa..\n");
					return;
				}
			}

			// Neo was killed by a summon
			else if (attacker->classname == "monster_wizard" ||
			         attacker->classname == "monster_fish" ||
			         attacker->classname == "monster_demon1" ||
			         attacker->classname == "monster_gremlin" ||
			         attacker->classname == "monster_shambler" ||
			         attacker->classname == "monster_army")
			{
				// Neo's own summon killed him; make Neo an Agent
				if (attacker->real_owner == targ)
				{
					removeNeo(targ);
					bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^-^-^-^-^-^-^> ^bNeo Mode^b ^<^-^-^-^-^-^-^-^-^-^>\n"));
					bprint(PR_PRINT_HIGH, "Neo ("); bprint(PR_PRINT_HIGH, targ->netname);
					bprint(PR_PRINT_HIGH, ") was killed by his own creature!\n");
					bprint(PR_PRINT_HIGH, "Kill someone to become Neo!\n");
					bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
					BroadcastSound("boss1/sight1.wav");
					this->centerPrint();
					return;
				}
				// an Agent's summon killed Neo; make the Agent Neo
				else
				{
					removeNeo(targ);
					bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^-^-^-^-^-^-^> ^bNeo Mode^b ^<^-^-^-^-^-^-^-^-^-^>\n"));
					bprint(PR_PRINT_HIGH, "Neo ("); bprint(PR_PRINT_HIGH, targ->netname);
					bprint(PR_PRINT_HIGH, ") was killed by "); bprint(PR_PRINT_HIGH, attacker->real_owner->netname);
					bprint(PR_PRINT_HIGH, "'s summon!\n");
					bprint(PR_PRINT_HIGH, attacker->real_owner->netname); bprint(PR_PRINT_HIGH, " becomes Neo!\n");
					bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
					oldSelf = self;
					assignNeo(attacker->real_owner);
					self = oldSelf;
					BroadcastSound("boss1/sight1.wav");
					rnum = random();
					if (rnum < (1 / playerCount) * WHOA_LIKELINESS)
						stuffcmd(attacker->real_owner, "say Whoa..\n");
					return;
				}
			}

			// Neo was killed by the environment; make Neo an Agent
			else if (attacker->classname != "player")
			{
				removeNeo(targ);
				bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^-^-^-^-^-^-^> ^bNeo Mode^b ^<^-^-^-^-^-^-^-^-^-^>\n"));
				if (rnum < 0.5)
				{
					bprint(PR_PRINT_HIGH, "Neo ("); bprint(PR_PRINT_HIGH, targ->netname);
					bprint(PR_PRINT_HIGH, ") went too far down the rabbit hole...\n");
				}
				else // rnum >= 0.5
				{
					bprint(PR_PRINT_HIGH, "Neo ("); bprint(PR_PRINT_HIGH, targ->netname);
					bprint(PR_PRINT_HIGH, ") proves that you should never send a human to do a machine's job...\n");
				}
				bprint(PR_PRINT_HIGH, "No one is Neo. Kill someone to become Neo!\n");
				bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
				BroadcastSound("boss1/sight1.wav");
				this->centerPrint();
				return;
			}
		}
		// Neo killed an Agent
		else if (attacker == neoExists && targ != neoExists && !this->teammate(attacker, targ))
		{
			// just randomly cause Neo to say "Whoa.."
			rnum = random();
			if (rnum < (1 / playerCount) * WHOA_LIKELINESS)
				stuffcmd(attacker, "say Whoa..\n");
		}
	}
}
//=================================================================================================
// Makes player `attacker` Neo. If there's currently a player with Neo status, this function will
// remove Neo status from that player.
//=================================================================================================
void Neo::assignNeo(entity attacker)
{
	// if a player is already Neo, remove Neo from that player
	if (neoExists != world) removeNeo(neoExists);

	// if player is dead when they become Neo, don't give them Neo status (no one will be Neo)
	if (attacker->health <= 0 || attacker->deadflag != PR_DEAD_NO || attacker->penance_time > time
		|| !attacker->is_connected)
	{
		bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^-^-^-^-^-^-^> ^bNeo Mode^b ^<^-^-^-^-^-^-^-^-^-^>\n"));
		bprint(PR_PRINT_HIGH, "Unfortunately, Neo ("); bprint(PR_PRINT_HIGH, attacker->netname); 
		bprint(PR_PRINT_HIGH, ") was dead,\nor just not available. Kill someone to become Neo!\n");
		bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
		this->centerPrint();
		return;
	}

	// unthief `attacker` if he's thieving
	if (attacker->job & PR_JOB_THIEF && (attacker->job & PR_JOB_ACTIVE || attacker->job & PR_JOB_FULL_HIDE))
		RevealThief(attacker, PR_FALSE);

	neoExists = attacker;
	string temp; temp = ftos(oldTeamplayValue);
	cvar_set("teamplay", temp); // return the server to its 'actual' teamplay setting

	// centerprint "YOU ARE NEO!", surrounded by white boxes
	temp =              S_("\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\n");
	temp = strcat(temp, S_("\x0B ^bYOU ARE NEO!^b \x0B\n"));
	temp = strcat(temp, S_("\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\n"));
	CenterPrint(attacker, temp);

	// put attacker on blue team alone
	self = attacker;
	self->admin_kick = attacker;
	Admin_Assign(1, false, true);
	possessionsChangeTeam(attacker, 1);

	// give attacker (Neo) all the CuTF runes
	SpawnNewRune(PR_RUNE_TRIAD); SpawnNewRune(PR_RUNE_RESIS);
	SpawnNewRune(PR_RUNE_SPEED); SpawnNewRune(PR_RUNE_REGEN);

	entity rune;
	rune = find(world, "classname", "rune");
	while (rune != world)
	{
		PlayerTakesRune(attacker, rune);
		rune = find(rune, "classname", "rune");
	}
	rune = find(world, "classname", "runewrong");
	while (rune != world)
	{
		PlayerTakesRune(attacker, rune);
		rune = find(rune, "classname", "runewrong");
	}
	attacker->effects = attacker->effects | (PR_EF_BLUE | PR_EF_BRIGHTLIGHT);

	// Allow Neo to change class immediately.
	neoCanChangeClass = true;

	// Remember his current frag count and the current game time.
	neoFragsAtLastClassChange = neoExists->real_frags;
	neoTimeAtLastClassChange = time;
}
//=================================================================================================
// Removes Neo status from `player`.
//=================================================================================================
void Neo::removeNeo(entity player)
{
	// put player back on Agent team (team 2)
	neoExists = world;
	cvar_set("teamplay", "1"); // make sure settings are right for 'deathmatch'

	entity oldSelf;
	oldSelf = self;
	self = player;
	self->admin_kick = player;
	Admin_Assign(2, false, true);
	self = oldSelf;
	possessionsChangeTeam(player, 2);

	// remove player's runes
	EndRunes();
	player->effects = player->effects - (player->effects & (PR_EF_BRIGHTLIGHT | PR_EF_BLUE));
}
//=================================================================================================
// Find `player`'s possessions, and change the team of their possessions to `teamNumber`.
//=================================================================================================
void Neo::possessionsChangeTeam(entity player, float teamNumber)
{
	entity ent;

	// buildings
	ent = nextent(world);
	while (ent != world)
	{
		if (ent->flags & PR_FL_MACHINE) // is it a building?
		if (ent->real_owner == player)  // is it owned by `player`?
		{
			ent->team_no = teamNumber;
			if (teamNumber == 1)
			{
				if (ent->classname == "building_sentrygun")
					ent->skin = 0;
				else
					ent->skin = 1;
			}
			else
				ent->skin = 3;
		}
		ent = nextent(ent);
	}

	// summons
	if (player->job & PR_JOB_WARLOCK)
	{
		float i; i = 0;
		while (i < 3)
		{
			ent = GetSummon(player, i);
			if (ent != world)
			{
				ent->team_no = teamNumber;
				if (teamNumber == 1)
					ent->effects = ent->effects | (PR_EF_BRIGHTLIGHT | PR_EF_BLUE);
				else
					ent->effects = ent->effects - (ent->effects & (PR_EF_BRIGHTLIGHT | PR_EF_BLUE));
			}
			i = i + 1;
		}
	}

	// army grunts
	else if (player->job & PR_JOB_ARMY)
	{
		ent = find(world, "classname", "monster_army");
		while (ent != world)
		{
			if (ent->real_owner == player)
			{
				ent->team_no = teamNumber;
				if (teamNumber == 1)
					ent->skin = 0;
				else
					ent->skin = 3;
			}
			ent = find(ent, "classname", "monster_army");
		}
	}

	// mines
	else if (player->job & PR_JOB_GUERILLA)
	{
		ent = find(world, "netname", "land_mine");
		while (ent != world)
		{
			if (ent->real_owner == player)
				ent->team_no = teamNumber;
			ent = find(ent, "netname", "land_mine");
		}
	}

	// holograms (not sure this is necessary)
	if (player->cutf_items & PR_CUTF_HOLO)
	{
		ent = find(world, "classname", "holo");
		while (ent != world)
		{
			if (ent->real_owner == player)
				ent->team_no = teamNumber;
			ent = find(ent, "classname", "holo");
		}
	}
}
//=================================================================================================
// Tests whether `a` and `b` are teammates (for Neo mode only).
// No longer using this. This is handled inside Teammate() with help of areFamilyMembers(). (8-25-2020)
//=================================================================================================
float Neo::teammate(entity a, entity b)
{
	if (!modeActive)
		return PR_TRUE;

	if (a == b) return PR_TRUE;

	// basically act normal if there's a Neo
	if (neoExists != world)
	{
		if (a->team_no == b->team_no) return PR_TRUE;
		else return PR_FALSE;
	}
	
	// no one is a teammate when there's no Neo, except objects and their owners
	else
	{
		if (a->classname == "player" && b->classname == "player")
		{
			return PR_FALSE;
		}
		else if (a->classname == "player" && b->classname != "player")
		{
			// if b's owner is a, return true.. else return false
			{
				if (b->real_owner == a) return PR_TRUE;
				else return PR_FALSE;
			}
		}
		else if (a->classname != "player" && b->classname == "player")
		{
			// if a's owner is b, return true.. else return false
			{
				if (a->real_owner == b) return PR_TRUE;
				else return PR_FALSE;
			}
		}
		else if (a->classname != "player" && b->classname != "player")
		{
			// whatever it is.. if it has the same owner, it's on same team
			if (a->real_owner == b->real_owner) return PR_TRUE;
			else return PR_FALSE;
		}
	}

	bprint(PR_PRINT_HIGH, "Error: Neo::teammate() had no answer\n");
	return PR_FALSE;
}
//=================================================================================================
// Prints Neo's stats when 'inv' command is used.
//=================================================================================================
void Neo::invPrint()
{
	float integer, fraction;
	string temp;

	sprint(self, PR_PRINT_HIGH, S_("\n^bNeo Mode^b: "));
	// damage
	sprint(self, PR_PRINT_HIGH, "damage: x");
	integer = floor(neoDamageFactor); fraction = neoDamageFactor - integer; fraction = fraction * 10;
	fraction = floor(fraction); temp = ftos(integer); temp = colstr(temp, PR_COLSTR_NUMBER);
	sprint(self, PR_PRINT_HIGH, temp); sprint(self, PR_PRINT_HIGH, "."); temp = ftos(fraction);
	temp = colstr(temp, PR_COLSTR_NUMBER); sprint(self, PR_PRINT_HIGH, temp);
	// resistance
	sprint(self, PR_PRINT_HIGH, ", resist: /");
	integer = floor(neoResistFactor); fraction = neoResistFactor - integer; fraction = fraction * 1000;
		// invert it, for people to read it more easily
		neoResistFactor = 1000 / fraction; integer = floor(neoResistFactor); fraction = neoResistFactor - integer; 
		fraction = fraction * 100; fraction = floor(fraction);
	temp = ftos(integer); temp = colstr(temp, PR_COLSTR_NUMBER); sprint(self, PR_PRINT_HIGH, temp);
	sprint(self, PR_PRINT_HIGH, "."); temp = ftos(fraction); 
	temp = colstr(temp, PR_COLSTR_NUMBER); sprint(self, PR_PRINT_HIGH, temp);
	// regeneration
	sprint(self, PR_PRINT_HIGH, ", regen: x");
	integer = floor(neoRegenFactor); fraction = neoRegenFactor - integer;
	fraction = fraction * 100; fraction = floor(fraction);
	temp = ftos(integer); temp = colstr(temp, PR_COLSTR_NUMBER); sprint(self, PR_PRINT_HIGH, temp);
	sprint(self, PR_PRINT_HIGH, "."); temp = ftos(fraction);
	temp = colstr(temp, PR_COLSTR_NUMBER); sprint(self, PR_PRINT_HIGH, temp);
	// firing rate

	// movement speed

}
//=================================================================================================
// When there's no Neo, this function is used to centerprint the message to everyone, "THERE'S NO
// NEO! KILL SOMEONE TO BE NEO!"
//=================================================================================================
void Neo::centerPrint()
{
	entity player;
	string temp;

	temp =              "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
	temp = strcat(temp, "\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\n");
	temp = strcat(temp, S_("\x0B     ^bTHERE'S NO NEO!^b     \x0B\n"));
	temp = strcat(temp, S_("\x0B ^bKILL SOMEONE TO BE NEO!^b \x0B\n"));
	temp = strcat(temp, "\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\n");

	player = find(world, "classname", "player");
	while (player != world)
	{
		CenterPrint(player, temp);
		player = find(player, "classname", "player");
	}
}

//=================================================================================================
// General error checking. Broadcasts error messages to all players if an error is detected. Called
// every frame in StartFrame().
//=================================================================================================
void Neo::errorCheck()
{
	// TODO: Devise a better system that doesn't spam messages to players.
	if (neoExists != world)
	{
		// Neo should always have all four runes.
		if (GetPlayerNoRunes(neoExists) != 4)
		{
			bprint(PR_PRINT_HIGH, NEO_ERROR_MESSAGE);
			bprint(PR_PRINT_HIGH, "Error: Neo does not have all 4 runes.\n");
		}
	}
}

} // END namespace Progs
