/*======================================================
	CLIENT.QC			Custom TeamFortress v3.2

	(c) TeamFortress Software Pty Ltd	29/2/97
	(c) William Kerney			16/9/00
	(c) Craig Hauser				19/3/00
========================================================
Handles obituaries, what happens when a client dies, respawns,
connects & disconnects, and when a map changes levels
======================================================*/

#include "progs.h"
#include "ofndefs.h"
#include "coop_defs.h"
#include "tfdefs.h"
#include "debug.h"
#include "tfortmap.h"
#include "weapons.h"
#include "menu.h"
#include "environ.h"
#include "subs.h"
#include "world.h"
#include "warlock.h"
#include "army.h"
#include "tforttm.h"
#include "cpstuff.h"
#include "frikbot/bot_qw.h"
#include "admin.h"
#include "weapons.h"
#include "prozac.h"
#include "status.h"
#include "vote2.h"
#include "neo.h"

namespace Progs {

// prototypes
void W_WeaponFrame();
void W_SetCurrentAmmo();
void player_pain(entity attacker, float damage);
void player_stand1();
void spawn_tfog(const vector& org);
void spawn_tdeath(const vector& org, entity death_owner);
float modelindex_eyes, modelindex_player, modelindex_null;
float anglemod(float v);

//void () SetUpChrisRound;//-
void PrematchBegin();
void TeamAllPlayers();
//void (float winner) RoundStop;//-
//void () UpdateScores;
//void () roundtimer_think;//-
//void () EndRound;//-

// TeamFortress prototypes
void TeamFortress_MOTD();
void TeamFortress_CheckTeamCheats();
//float(float tno) TeamFortress_TeamGetColor; //- OfN - not used here
void TeamFortress_PrintClassName(entity Viewer, float pc, float rpc);
void TeamFortress_PrintJobName(entity Viewer, float pc);
void TeamFortress_RemoveTimers();
void TeamFortress_SetupRespawn(float Suicided);
void TeamFortress_ShowTF();
float IsLegalClass(float pc);
void SetupTeamEqualiser();
#ifdef PR_QUAKE_WORLD
void SetTeamName(entity p);
#endif
void decrement_team_ammoboxes(float number);
void TeamFortress_DetonatePipebombs();
void decrement_team_pipebombs(float f);
void PlayerObserverMode();

// Hook prototypes
void Service_Grapple();

// TeamFortressMap prototypes
void ParseTFDetect(entity AD);
entity Finditem(float ino);
void tfgoalitem_GiveToPlayer(entity Item, entity AP, entity Goal);
void AttemptToActivate(entity Goal, entity AP, entity ActivatingGoal);
void CTF_FlagCheck();

//WK
void DropToCustomClassGen();
void TeamFortress_SetSpeed(entity p);
void kill_my_demons();
void DetonateAllGuns(); // SB
void Boot_Flamer_stream_touch();
void makeImmune(entity foo,float bar);

// SB Extern
void GetRank(entity targ);
//void() SwitchFromCamera;

//==============// ofn
void DetonateMines(entity mine_owner);
void RemoveHolo(entity player);
string GetBuildingName(entity thebuilding);
string GetMonsterName(entity themonster);
void MonsterKill(entity attacker);
void DetonateAllGunsForced();

float ValidVoteEnt();
void ResetVoteEnt();
void BroadcastSound(const string& thesound);

void WarlockKnifeKill(entity attacker);
void SetMeatUsage(entity player, float usage);
void SetHeadsUsage(entity player, float usage);

void PlayerJoined();
void PlayerQuit();

string GetClientDescription(entity client);
string GetBracedClDesc(entity client);

float ServerUpdating();
void PrintGameSettings(float broadcastmsg);
void PlayerDropRunes(entity player);
void FinalizedPerformAction();

entity GetSleepVote();
void VoteToSleep();
void SleepVoteResume(entity sleepent, entity targetclient);
void LaunchMap(const string& nfmap);

/*
=============================================================================

				LEVEL CHANGING / INTERMISSION

=============================================================================
*/

string nextmap;

//float intermission_running; // OfN - Moved to defs.qc for use on environ.qc
float	intermission_exittime;

/*QUAKED info_intermission (1 0.5 0.5) (-16 -16 -16) (16 16 16)
This is the camera point for the intermission.
Use mangle instead of angle, so you can set pitch or roll as well as yaw.  'pitch roll yaw'
*/
void info_intermission()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}
}


void SetChangeParms()
{
	if (self->health <= 0)
	{
		SetNewParms ();
		return;
	}

	// remove items
	self->items = self->items - (self->items & (PR_IT_KEY1 | PR_IT_KEY2 | PR_IT_INVISIBILITY | PR_IT_INVULNERABILITY | PR_IT_SUIT | PR_IT_QUAD) );

	// cap super health
	if (self->health > 100)
		self->health = 100;
	if (self->health < 50)
		self->health = 50;
	parm1 = self->items;
	parm2 = self->health;
	parm3 = self->armorvalue;
	if (self->ammo_shells < 25)
		parm4 = 25;
	else
		parm4 = self->ammo_shells;
	parm5 = self->ammo_nails;
	parm6 = self->ammo_rockets;
	parm7 = self->ammo_cells;
	parm8 = self->current_weapon;
	parm9 = self->armortype * 100;

	// TeamFortress Parameters
	parm10 = toggleflags;  // Store the global ToggleFlag settings
	parm11 = 0; 		   // Random Playerclass
	parm12 = 0;

#ifdef PR_COOP_MODE
	if (toggleflags & PR_TFLAG_CLASS_PERSIST)
	{
		if (self->tfstate & PR_TFSTATE_RANDOMPC)
		{
			parm11 = (parm11 | PR_TFSTATE_RANDOMPC);
			self->playerclass = 1 + floor(random() * (PR_PC_RANDOM - 1));
			while(!IsLegalClass(self->playerclass))
				self->playerclass = 1 + floor(random() * (PR_PC_RANDOM - 1));
		}

		parm12 = self->playerclass;				// save the playerclass between levels in parm12
	}											// if the toggleflag CLASS_PERSIST is set
#endif

#ifdef PR_STATUSBAR
	parm13 = self->StatusBarRes;
	parm14 = self->StatusBarSize;
#endif
#ifdef PR_QUAKE_WORLD
	parm15 = self->admin_flag; //CH Pass along admin status :)
#endif
}

void SetNewParms()
{
	parm1 = 0;
	parm2 = 100;
	parm3 = 0;
	parm4 = 25;
	parm5 = 0;
	parm6 = 0;
	parm6 = 0;
	parm8 = 1;
	parm9 = 0;

	// TeamFortress Parameters
	parm10 = 0;
	parm11 = 0;
	parm12 = 0;
	parm13 = 0;
	parm14 = 0;
	parm15 = 0;
}

// This think kicks in 30 seconds into the map to
// turn autoteam on.
void autoteam_think()
{
	//local float loopc;
	//? local entity other;//,oself;

//	if (chris)
//		SetUpChrisRound();
//	if (!chris)
		//PrematchBegin(); // PZ: no

	toggleflags = toggleflags | PR_TFLAG_AUTOTEAM;
	dremove(self);
}

// PZ: This function actually ends the prematch. Go figure.
void PrematchBegin()
{
	if (prematch)
	{
		bprint(PR_PRINT_HIGH,"\x9C\x11\x10 The game has now started! \x11\x10\x9C\n");
		other = find(world, "classname", "player");
		while (other != world)
		{
			other->is_abouttodie = PR_TRUE;
			other->items = other->items - (other->items & PR_IT_INVULNERABILITY);
			other->invincible_time = 0;
			other->invincible_finished = 0;
			other->effects = other->effects - (other->effects & PR_EF_DIMLIGHT);
			TF_T_Damage(other, world, world, other->health + 20, PR_TF_TD_IGNOREARMOUR, PR_TF_TD_OTHER);
			other = find(other, "classname", "player");
		}

		// - OfN - wtf
		/*other = find(world, classname, "grunty");
		while (other != world)
		{
			other.is_abouttodie = #TRUE;
			other.items = other.items - (other.items & #IT_INVULNERABILITY);
			other.invincible_time = 0;
			other.invincible_finished = 0;
			other.effects = other.effects - (other.effects & #EF_DIMLIGHT);
			TF_T_Damage(other, world, world, other.health + 20, #TF_TD_IGNOREARMOUR, #TF_TD_OTHER);
			other = find(other, classname, "grunty");
		}*/
	}
	dremove(self); // PZ: removes timer entity that called this function

/*	if (chris)
	{
		if (self.classname == "prematch_timer")
			dremove(self);
		else
			SetUpChrisRound();
	}*/
}

/*void () SetUpChrisRound =
{
	local entity other, oself;

	// Make sure everyone is on a team
	TeamAllPlayers();

	// Clean up team player counts
	team1total = 0;
	team2total = 0;
	team3total = 0;
	team4total = 0;

	oself = self;

	other = find(world, classname, "player");
	while (other != world)
	{
		if (livesperguy > 0)
		{
			if (other.team_no == 1) team1total = team1total + livesperguy;
			if (other.team_no == 2) team2total = team2total + livesperguy;
			if (other.team_no == 3) team3total = team3total + livesperguy;
			if (other.team_no == 4) team4total = team4total + livesperguy;
			other.lives = livesperguy;
		}

		self = other;
		kill_my_demons();
		DetonateAllGuns();

		TF_T_Damage(other, world, world, other.health + 20, #TF_TD_IGNOREARMOUR, #TF_TD_OTHER);

		other = find(other, classname, "player");
	}

	newmis = spawn();
	newmis.classname = "chris_round_timer";
	newmis.nextthink = time + roundtime;
	newmis.think = roundtimer_think;
};*/

/*void() roundtimer_think =
{

	RoundStop(0);
	dremove(self);

};*/

void TeamAllPlayers()
{
	entity other, oself;

	other = find(world, "classname", "player");
	while (other != world)
	{
		if (other->team_no <= 0 && other->is_connected)
		{
			oself = self;
			self = other;
			Menu_Team_Input(5);
			self = oself;
		}
		other = find(other, "classname", "player");
	}
}

/*void (float winner) RoundStop =
{
	if (winner == 0)
		bprint(#PRINT_MEDIUM, "The round has ended due to time.\n");
	else
	{
		if (winner == 1)
		{
			team1rounds = team1rounds + 1;
			bprint(#PRINT_MEDIUM, "The blue team wins the round.\n");
		}
		if (winner == 2)
		{
			team2rounds = team2rounds + 1;
			bprint(#PRINT_MEDIUM, "This round goes to the red team.\n");
		}
		if (winner == 3)
		{
			team3rounds = team3rounds + 1;
			bprint(#PRINT_MEDIUM, "The yellow team are sponsored by SB-1 Tech and have won this round.\n");
		}
		if (winner == 4)
		{
			team4rounds = team4rounds + 1;
			bprint(#PRINT_MEDIUM, "The green team are the winners of this round.\n");
		}
	}

	UpdateScores();

	newmis = spawn();
	newmis.nextthink = time + 1;
	newmis.think = EndRound;
};*/

/*void() UpdateScores =
{
	local entity guy;

	guy = find(other, classname, "player");
	while (guy != world)
	{
		if (guy.team_no == 1)
		{
			guy.frags = team1rounds * 10;
			guy.real_frags = team1rounds * 10;
		}
		else if (guy.team_no == 2)
		{
			guy.frags = team2rounds * 10;
			guy.real_frags = team2rounds * 10;
		}
		else if (guy.team_no == 3)
		{
			guy.frags = team3rounds * 10;
			guy.real_frags = team3rounds * 10;
		}
		else if (guy.team_no == 4)
		{
			guy.frags = team4rounds * 10;
			guy.real_frags = team4rounds * 10;
		}
		guy = find(guy, classname, "other");
	}
};*/ /////////////??/////////////7

/*void() EndRound =
{
	local entity roundtimer, prematchtimer;
	local float winner, prematchtime;
	local string st;

	// Kill the round timer
	other = find(world, classname, "chris_round_timer");
	// I deliberately don't do a contingency check here - if it screws up I wanna know about it
	dremove(other);

	st = infokey(world, "pm");
	if (st == string_null) // if 'pm' isn't set, try 'prematch'
		st = infokey(world, "prematch");
	if (st == "on") // if it reads 'on', do a 30 second prematch
		prematchtime = time + 30;
	else // if it doesn't read 'on'...
		prematchtime = stof(st); // turn the string into a float
	if (prematchtime) // if we have prematch
	{
		prematch = time + prematchtime; // set it
	}
	else
		prematch = #FALSE; // otherwise, no prematch

	// Kill everyone
	other = find(world, classname, "player");
	while (other != world)
	{
		TF_T_Damage(other, world, world, other.health + 20, #TF_TD_IGNOREARMOUR, #TF_TD_OTHER);
		other = find(other, classname, "player");
	} // Don't kill everyone here or it drops the flag

	// Set up the timer
	if (team1rounds < roundstowin && team2rounds < roundstowin && team3rounds < roundstowin && team4rounds < roundstowin)
	{
		roundtimer = spawn();
		roundtimer.think = SetUpChrisRound;
		roundtimer.nextthink = time + 5;

		prematchtimer = spawn();
		prematchtimer.classname == "prematch_timer";
		prematchtimer.nextthink = time + prematch;
		prematchtimer.think = PrematchBegin;
	}
	else
	{
		newmis = spawn();
		newmis.nextthink = time + 5;
		newmis.think = execute_changelevel;
	}
};*/

void DecodeLevelParms()
{
	string st;
	entity ent;
	float prematchtime;

	if (serverflags)
	{
		if (world->model == "maps/start.bsp")
			SetNewParms (); 	// take away all stuff on starting new episode
	}

	self->items = parm1;
	self->health = parm2;
	self->armorvalue = parm3;
	self->ammo_shells = parm4;
	self->ammo_nails = parm5;
	self->ammo_rockets = parm6;
	self->ammo_cells = parm7;
	self->current_weapon = parm8;
	self->armortype = parm9 * 0.01;

	// TeamFortress Parameters
	// Detect whether this is the first entrance into a map
	if (toggleflags == 0)
	{
		toggleflags = parm10;
		allow_hook = 0;
		invis_only = PR_SPY_INVIS_ONLY;

		if (coop || !deathmatch)
			toggleflags = toggleflags | PR_TFLAG_CLASS_PERSIST;

		nextmap = mapname;

	#ifdef PR_GRAPPLING_HOOK
		allow_hook = PR_TRUE;
	#endif

		// Is this a FortressMap?
		ent = find(world, "classname", "info_tfdetect");
		if (ent != world)
		{
			// Turn on Teamplay
			if (teamplay == 0)
				cvar_set("teamplay","21?TeamFortress");

			// Parse the rest of the Detection details
			ParseTFDetect(ent);

			// If the number_of_teams wasn't set, then there's not TF
			// spawnpoints on this lvl... so guess at 4 teams.
#ifdef PR_COOP_MODE_ENHANCED
			if ( deathmatch ) {
				if ( number_of_teams <= 0 || number_of_teams >= 5 )
					number_of_teams = 4;
			} else
				number_of_teams = 1;	// Gizmo - only one team on singleplayer/coop
#else
			if (number_of_teams <= 0 || number_of_teams >= 5)
				number_of_teams = 4;
#endif
		}
		else
		{
			// Is this a CTF map?
			ent = find(world, "classname", "info_player_team1");
			if ((ent != world) || (CTF_Map == PR_TRUE))
			{
				// Turn on CTF MAP
				CTF_Map = PR_TRUE;

				// Turn on Teamplay
				if (teamplay == 0)
					cvar_set("teamplay","21?TeamFortress");

				// Setup the CTF FlagCheck Timer
				ent = spawnServerSide(); // PZ: make it a server-side only entity
				ent->nextthink = time + 30;
				ent->think = CTF_FlagCheck;

#ifdef PR_COOP_MODE_ENHANCED
				if ( !deathmatch )
					number_of_teams = 1;
				else
					number_of_teams = 2;
#else
				number_of_teams = 2;
#endif
			}
			else // Normal map
			{
				normalQuakeMap = PR_TRUE; // PZ
#ifdef PR_COOP_MODE_ENHANCED
				if ( !deathmatch )
					number_of_teams = 1;
				else
					number_of_teams = 4;
#else
				number_of_teams = 4;
#endif
			}

			// Set speed limits
#ifndef PR_QUAKE_WORLD
			cvar_set("sv_maxspeed", "1000");
#endif
			// set aiming level
#ifndef PR_NET_SERVER
			cvar_set("sv_aim", "1");
#endif
			// Set life limits
			team1lives = -1;
			team2lives = -1;
			team3lives = -1;
			team4lives = -1;

			// WK Clear our nextspam counters
			team1nextspam = -1;
			team2nextspam = -1;
			team3nextspam = -1;
			team4nextspam = -1;

			// Set illegal playerclasses
			illegalclasses1 = 0;
			illegalclasses2 = 0;
			illegalclasses3 = 0;
			illegalclasses4 = 0;

			// Set Team Limits
			team1maxplayers = 100;
			team2maxplayers = 100;
			team3maxplayers = 100;
			team4maxplayers = 100;
			civilianteams = 0;
		}

		// Ofn
		PrintGameSettings(PR_TRUE);

		SetupTeamEqualiser();

		if (PR_NEVER_TEAMFRAGS)
		{
			toggleflags = toggleflags - (toggleflags & PR_TFLAG_TEAMFRAGS);
		}

		if (PR_ALWAYS_TEAMFRAGS)
		{
			toggleflags = toggleflags | PR_TFLAG_TEAMFRAGS;
		}

		if (PR_CHECK_SPEEDS)
		{
			toggleflags = toggleflags | PR_TFLAG_CHEATCHECK;
		}

	#ifdef PR_QUAKE_WORLD
		st = infokey(world, "temp1");
		toggleflags = (toggleflags | PR_TFLAG_FIRSTENTRY | (int)stof(st));
	#else
		toggleflags = (toggleflags | PR_TFLAG_FIRSTENTRY | cvar("temp1"));
	#endif

		float autoteam_time;
		autoteam_time = 30;

	#ifdef PR_QUAKE_WORLD
		// check all serverinfo settings, to set the appropriate toggleflags
		//local string st;

		// AUTOTEAM
		st = infokey(world, "a");
		if (st == string_null)
			st = infokey(world, "autoteam");
		if (st == "on")
			toggleflags = toggleflags | PR_TFLAG_AUTOTEAM;
		else if (st == "off")
			toggleflags = toggleflags - (toggleflags & PR_TFLAG_AUTOTEAM);
		else if (stof(st) != 0)
		{
			toggleflags = toggleflags | PR_TFLAG_AUTOTEAM;
			autoteam_time = stof(st);
		}

		// TEAMFRAGS
		st = infokey(world, "t");
		if (st == string_null)
			st = infokey(world, "teamfrags");
		if (st == "on")
			toggleflags = toggleflags | PR_TFLAG_TEAMFRAGS;
		else if (st == "off")
			toggleflags = toggleflags - (toggleflags & PR_TFLAG_TEAMFRAGS);
		// PZ - for AGR mode
		if (agr)
		{
			if (agr_teamfrags)
				toggleflags = toggleflags | PR_TFLAG_TEAMFRAGS;
			else
				toggleflags = toggleflags - (toggleflags & PR_TFLAG_TEAMFRAGS);
		}
		// PZ - for Invade mode
		if (invade)
		{
			if (invade_teamfrags)
				toggleflags = toggleflags | PR_TFLAG_TEAMFRAGS;
			else
				toggleflags = toggleflags - (toggleflags & PR_TFLAG_TEAMFRAGS);
		}

		//WK JELLO WATER
		st = infokey(world, "j");
		if (st == string_null)
			st = infokey(world, "jello");
		if (st == "on")
			jello = PR_TRUE;
		else {
			float numba;
			numba = stof(st);
			if (numba)
				jello = numba;
			else
				jello = PR_FALSE;
		}

		//WK JELLO WATER
		light_damage = PR_FALSE;
		st = infokey(world, "ld");
		if (st == string_null)
			st = infokey(world, "lightdamage");
		if (st == "on")
			light_damage = PR_TRUE;

		// SB New, improved TF 2.9 fake prematch mode!
		// We can make a class, come in, run around, play tag the flag, but can't do anything
		// useful until the prematch is over!

		st = infokey(world, "pm");
		if (st == string_null) // if 'pm' isn't set, try 'prematch'
			st = infokey(world, "prematch");
		if (st == "on") // if it reads 'on', do a 30 second prematch
			prematchtime = time + 30;
		else // if it doesn't read 'on'...
			prematchtime = stof(st); // turn the string into a float
		if (prematchtime) // if we have prematch
		{
			prematch = time + prematchtime; // set it
			// PZ: why require autoteam after prematch?? (no more)
			//autoteam_time = prematchtime;
			//toggleflags = toggleflags | #TFLAG_AUTOTEAM;
			// PZ: for some reason, prematch was riding on autoteam code (no more)
			ent = spawnServerSide(); // PZ: make it a server-side only entity
			ent->nextthink = time + prematchtime;
			ent->think = PrematchBegin;
		}
		else
			prematch = PR_FALSE; // otherwise, no prematch

		//WK Bounty System
		st = infokey(world, "bounty");
		if (st == string_null)
			st = infokey(world, "moola");
		if (st == "on")
			bounty = PR_TRUE;
		else
			bounty = PR_FALSE;

		//CH Sets the starting amount of money :)
		/*st = infokey(world, "m");
		if (st == string_null)
			st = infokey(world, "money");
		local float numba;
		numba = stof(st);
		if (numba)
			custom_money = numba;
		else
			custom_money = #SPENDING_LIMIT; // --> DEFAULT_MONEY
		MOVED TO UDPATEINFOS()*/

		// GRAPPLING HOOK
		st = infokey(world, "g");
		if (st == string_null)
			st = infokey(world, "grapple");
		if (st == "off")
			allow_hook = PR_FALSE;
		if (!(toggleflags & PR_TFLAG_GRAPPLE) && st != "on")
			allow_hook = PR_FALSE;

		// SPY OFF
		st = infokey(world, "spy");
		if (st == "off")
			spy_off = PR_TRUE;

		// SPY INVIS ONLY
		st = infokey(world, "s");
		if (st == string_null)
			st = infokey(world, "spyinvis");
		if (st == "on" || toggleflags & PR_TFLAG_SPYINVIS)
			invis_only = PR_TRUE;
		else if (st == "off")
			invis_only = PR_FALSE;

		// RespawnDelay
		st = infokey(world, "rd");
		if (st == string_null)
			st = infokey(world, "respawn_delay");
		respawn_delay_time = stof(st);
		if (respawn_delay_time)
			toggleflags = toggleflags | PR_TFLAG_RESPAWNDELAY;
	#else
		// GRAPPLING HOOK
		if (!(toggleflags & PR_TFLAG_GRAPPLE) && (CTF_Map == PR_FALSE))
			allow_hook = PR_FALSE;
	#endif

		// If no Respawndelay has been specified, set the default
		if (toggleflags & PR_TFLAG_RESPAWNDELAY && respawn_delay_time == 0)
			respawn_delay_time = PR_RESPAWN_DELAY_TIME;

		// Prevent autoteam from kicking in for 30 seconds.
		// Allows restructuring of the teams from the last map nicely.
		if (toggleflags & PR_TFLAG_AUTOTEAM)
		{
			toggleflags = toggleflags - (toggleflags & PR_TFLAG_AUTOTEAM);
			ent = spawnServerSide(); // PZ: make it a server-side only entity
			ent->nextthink = time + autoteam_time;
			ent->think = autoteam_think;
		}
	}

	if (parm11)
		self->tfstate = parm11;
	if (self->playerclass == 0)
		self->playerclass = parm12;

#ifdef PR_STATUSBAR
	if (parm13)
		self->StatusBarRes = parm13;
	if (parm14)
		self->StatusBarSize = parm14;
#endif
#ifdef PR_QUAKE_WORLD
	if (parm15)
		self->admin_flag = parm15; //CH Admin status :)
#endif
}

/*
============
FindIntermission

Returns the entity to view from
============
*/
entity FindIntermission()
{
	entity spot;
	float cyc;

	// look for info_intermission first
	spot = find (world, "classname", "info_intermission");
	if (spot != world)
	{	// pick a random one
		cyc = random() * 1;

		// Following removed for the observer code
		/*while (cyc > 1)
		{
			spot = find (spot, classname, "info_intermission");
			if (!spot)
				spot = find (spot, classname, "info_intermission");
			cyc = cyc - 1;
		}*/

		return spot;
	}

// then look for the start position
	spot = find (world, "classname", "info_player_start");
	if (spot != world)
		return spot;

// then look through the deathmatch starts
	spot = find (world, "classname", "info_player_deathmatch");
	if (spot != world)
	{
		// pick a random one
		cyc = random() * 6;
		while (cyc > 1)
		{
			spot = find (spot, "classname", "info_player_deathmatch");
			if (spot == world)
				spot = find (spot, "classname", "info_player_deathmatch");
			cyc = cyc - 1;
		}
		return spot;
	}

	objerror ("FindIntermission: no spot");
	return world;  // PZ: Added this line.
}

/*===========================
FindNextIntermission

returns the next intermission point
===========================*/
entity FindNextIntermission(entity start_point)
{
	entity spot;
	//local float cyc;

	if (deathmatch)
	{
	// look through info_intermission first
		if (start_point->classname == "info_intermission" || start_point == world)
		{
			spot = find (start_point, "classname", "info_intermission");
			if (spot != world)
				return spot;
			else
				start_point = world;
		}


	// then look through the deathmatch starts
		if (start_point->classname == "info_player_deathmatch" || start_point == world)
		{
			spot = find (start_point, "classname", "info_player_deathmatch");
			if (spot != world)
				return spot;
		}

		// at the end of the list
		spot = find (world, "classname", "info_intermission");
		if (spot != world)
			return spot;

		spot = find (world, "classname", "info_player_deathmatch");
		if (spot != world)
			return spot;
	}
	else // do not cycle though in co-op or single
	{
		spot = find (world, "classname", "info_player_start");
		if (spot != world)
			return spot;
	}

	// it should never reach this point
	return FindIntermission();
}


/*==================================================

TF_MovePlayer
Moves the player to another intermission viewpoint

====================================================*/

void TF_MovePlayer()
{
	entity place;

	place = FindNextIntermission(self->observer_list);

	self->observer_list = place;

	setorigin(self, place->origin + V({0, 0, 1}));
	self->angles = place->angles;
	self->fixangle = PR_TRUE;		// turn this way immediately

}

//- OfN - Map cycling code modified to match TF2.8 standard
float GetNoPlayers(float checkIP, float noIdlers);
void SetCycleTimeoutTimer();
void SetCycleErrorCheck();
void SetCycleTimer();

float DoExtraCycle()
{
	string nfmap, temp;
	float minplayers, maxplayers, itsok, currentpl;
//bprint(2, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! DoExtraCycle 1\n");
	nfmap = infokey(world, "nmap");

	// if null then return
	if (nfmap == string_null)
		return PR_FALSE;

	if (infokey(world,"minp")!="")
		minplayers = stof(infokey(world,"minp"));
	else
		minplayers = 0;

	if (infokey(world,"maxp")!="")
		maxplayers = stof(infokey(world,"maxp"));
	else
		maxplayers = 32;

	itsok = PR_TRUE;

	currentpl = GetNoPlayers(PR_FALSE, PR_FALSE);
//bprint(2, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! DoExtraCycle 2\n");
	// check conditions
	if (minplayers > currentpl) {
		bprint(PR_PRINT_HIGH,"Map ");
		nfmap = infokey(world, "nmap");
		bprint(PR_PRINT_HIGH, nfmap);
		bprint(PR_PRINT_HIGH,S_(" skipped ^b-^b minimum players "));
		temp = ftos(minplayers);
		bprint(PR_PRINT_HIGH, temp);
		bprint(PR_PRINT_HIGH," \xA8""currently ");
		temp = ftos(currentpl);
		bprint(PR_PRINT_HIGH, temp);
		bprint(PR_PRINT_HIGH,"\xA9\n");
		itsok = PR_FALSE;
	} else if (maxplayers < currentpl) {
		bprint(PR_PRINT_HIGH,"Map ");
		nfmap = infokey(world, "nmap");
		bprint(PR_PRINT_HIGH, nfmap);
		bprint(PR_PRINT_HIGH,S_(" skipped ^b-^b maximum players "));
		temp = ftos(maxplayers);
		bprint(PR_PRINT_HIGH,temp);
		bprint(PR_PRINT_HIGH," \xA8""currently ");
		temp = ftos(currentpl);
		bprint(PR_PRINT_HIGH,temp);
		bprint(PR_PRINT_HIGH,"\xA9\n");
		itsok = PR_FALSE;
	}

	// cleanup
	localcmd("localinfo minp \"\"\n");
	localcmd("localinfo maxp \"\"\n");
	//locals clean
//bprint(2, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! DoExtraCycle 3\n");
	// execute map conditions ok
	if (itsok) {
		nfmap = infokey(world, "nmap");
		LaunchMap(nfmap);
//bprint(2, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! DoExtraCycle 4\n");
		return PR_TRUE;
	} else {
		localcmd("localinfo nmap \"\"\n");
//bprint(2, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! DoExtraCycle 5\n");
		return PR_FALSE;
	}
//bprint(2, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! DoExtraCycle 6\n");
	return PR_FALSE;
}

void GotoNextMap();

void cycle_timer_think()
{
	GotoNextMap();
	dremove(self);
}

void cycle_timeout_think()
{
	// Silently report to server console and remote debugger
	//RPrint("Warning: Error executing map cfg file, it doesn't exist or it doesn't make an explicit map change\n\n Attempting direct map launch...\n\n");

	// Direct map launch
	string tmap;

	tmap = infokey(world,"tmap");
	localcmd("map ");
	localcmd(tmap);
	localcmd("\n");

	// If map is really launched the following timer never runs
	SetCycleErrorCheck();
	dremove(self);
}

void cycle_error_think()
{
	// Get name of problematic map
	string tmap;
	tmap = infokey(world,"tmap");

	// And display it to every1 on server
	bprint(PR_PRINT_HIGH,"Server failed loading map \"");
	bprint(PR_PRINT_HIGH,tmap);
	bprint(PR_PRINT_HIGH,"\" (Skipping...)\n");

	// Also log it to server console
	RPrint("ERROR TRYING TO LAUNCH \"");
	RPrint(tmap);
	RPrint("\" MAP!\n");

	// Clean up, not need anyway as it is done on worlspawn
	localcmd("localinfo tmap \"\"\n");

	already_chosen_map = PR_FALSE;
	SetCycleTimer();

	dremove(self);
}

void SetCycleTimeoutTimer()
{
	newmis = spawnServerSide(); // PZ: make it a server-side only entity
	newmis->classname = "cycletimeout_timer";
	newmis->nextthink = time + PR_CYCLE_TIMEOUT;
	newmis->think = cycle_timeout_think;
}

void SetCycleTimer()
{
	newmis = spawnServerSide(); // PZ: make it a server-side only entity
	newmis->classname = "cyclemap_timer";
	newmis->nextthink = time + PR_MAPCYCLE_SPEED;
	newmis->think = cycle_timer_think;
}

void SetCycleErrorCheck()
{
	newmis = spawnServerSide(); // PZ: make it a server-side only entity
	newmis->classname = "cycleerror_timer";
	newmis->nextthink = time + PR_CYCLE_TIMEOUT;
	newmis->think = cycle_error_think;
}


/*
	CheckFailedMapChange - used to check if a changelevel() call failed

	By Gizmo
*/

void fail_think()
{
	bprint (PR_PRINT_HIGH, "Map change failed! Defaulting to start...\n");
	//changelevel ("start");
	// fix changelevel bug?
	localcmd ("map start\n");
	remove (self);
}

void CheckFailedMapChange()
{
	newmis = spawnServerSide(); // PZ: make it a server-side only entity
	newmis->nextthink = time + 5;
	newmis->think = fail_think;
}

// PZ NOTE: the map change code is so damn convoluted
void GotoNextMap()
{
	float nextlevel;
	string sl;
	//local entity te;
	string cyc;
	string nmap;
//if (already_chosen_map) bprint(2, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ALREADY CHOSEN MAP\n");
//else bprint(2, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! _NOT_ ALREADY CHOSEN MAP\n");
//bprint(2, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 1\n");
	// Gizmo - this enables nextmap voting on coop
	nmap = infokey (world, "nmap");
	localcmd ("localinfo nmap \"\"\n");

	// Gizmo - check if the samelevel cvar is set to 1
	if ( cvar( "samelevel" ) == 1 )
	{
		changelevel( mapname );
		already_chosen_map = PR_TRUE;
	}
	else if (nextmap != mapname || nmap != string_null)
	{
		if (nmap != string_null)
			nextmap = nmap;
//bprint(2, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! RUNNING CHANGELEVEL()\n");
		changelevel (nextmap);
		CheckFailedMapChange ();
		already_chosen_map = PR_TRUE;
	}
//if (already_chosen_map) bprint(2, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ALREADY CHOSEN MAP\n");
//else bprint(2, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! _NOT_ ALREADY CHOSEN MAP\n");
	if ( !already_chosen_map )
		already_chosen_map = DoExtraCycle();
//bprint(2, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 2\n");
	if (!already_chosen_map)
	{
		if (PR_AGR_ROTATION_OFF) // PZ - AGR off on map triggered by rotation
			localcmd("localinfo agr \"\"\n");
		localcmd("localinfo invade \"\"\n");
		// PZ - make sure Neo mode is off on map rotation
		localcmd("localinfo neo \"\"\n");
//bprint(2, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 3\n");

		sl = infokey (world, "n");
		nextlevel = stof (sl);

		nextlevel = nextlevel + 1;	// next level
		sl = ftos (nextlevel);
		localcmd ("serverinfo n ");
		localcmd (sl);
		localcmd ("\n");

		// PZ NOTE: This performs map rotation, using the map#.cfg files.
		// change map
		cyc = infokey (world, "cycledir");

		if (cyc == string_null)
			localcmd ("exec " TO_STR(PR_DEFAULT_CYCLEDIR) "/map");
		else {
			localcmd ("exec ");
			localcmd (cyc);
			localcmd ("/map");
		}

		localcmd (sl);
		localcmd (".cfg\n");

		// PZ: execute any <mapname>.cfg file in the cycledir
		// localcmd() doesn't immediately execute, so I made flushcmd().
		flushcmd();                               // make it execute the previous localcmds now
		nmap = infokey(world, "nmap");            // nmap doesn't get set until the above cfg is ran
		if (cyc == string_null)
			localcmd("exec " TO_STR(PR_DEFAULT_CYCLEDIR) "/");
		else
		{
			localcmd("exec ");
			localcmd(cyc);
			localcmd("/");
		}
		localcmd(nmap);
		localcmd(".cfg\n");
	}

	// PZ: execute any <mapname>.cfg file in the cycledir
	if (already_chosen_map)
	{
		// localcmd() doesn't immediately execute, so I made flushcmd().
		//flushcmd();                             // make it execute the previous localcmds now
		nmap = infokey(world, "nmap");            // nmap doesn't get set until the above cfg is ran
		cyc = infokey (world, "cycledir");
		if (cyc == string_null)
			localcmd("exec " TO_STR(PR_DEFAULT_CYCLEDIR) "/");
		else
		{
			localcmd("exec ");
			localcmd(cyc);
			localcmd("/");
		}
		localcmd(nmap);
		localcmd(".cfg\n");
	}

/*
	// Gizmo - if the above didn't produce a serverinfo/localinfo nmap then default to start
	if (!already_chosen_map && infokey(world, "nmap") != string_null)
		already_chosen_map = DoExtraCycle ();
	else if (!already_chosen_map && infokey(world, "nmap") == string_null)
		changelevel ("start");
*/

	if (infokey(world, "n") == "0")
		already_chosen_map = PR_FALSE;

	// Gizmo - this is only called if the map wasn't changed to for min/max player reasons
	// Gizmo - all this does is set a timer to call GotoNextMap again and try the next map from the infokey(world, "n") number
	if (already_chosen_map == PR_FALSE) // nothing was done yet, so set the damn timer..
		SetCycleTimer ();
}


void ExitIntermission()
{
	RPrint("Exiting intermission...\n");
// skip any text in deathmatch
	if (deathmatch)
	{
		RPrint("Exit Intermission in Deathmatch.\n");
		GotoNextMap ();
		return;
	}

	intermission_exittime = time + 1;
	intermission_running = intermission_running + 1;

//
// run some text if at the end of an episode
//
	if (intermission_running == 2)
	{
		if (world->model == "maps/e1m7.bsp")
		{
			WriteByte (PR_MSG_ALL, PR_SVC_CDTRACK);
			WriteByte (PR_MSG_ALL, 2);
			WriteByte (PR_MSG_ALL, 3);
			if (!cvar("registered"))
			{
				WriteByte (PR_MSG_ALL, PR_SVC_FINALE);
				WriteString (PR_MSG_ALL, "As the corpse of the monstrous entity\nChthon sinks back into the lava whence\nit rose, you grip the Rune of Earth\nMagic tightly. Now that you have\nconquered the Dimension of the Doomed,\nrealm of Earth Magic, you are ready to\ncomplete your task in the other three\nhaunted lands of Quake. Or are you? If\nyou don't register Quake, you'll never\nknow what awaits you in the Realm of\nBlack Magic, the Netherworld, and the\nElder World!");
			}
			else
			{
				WriteByte (PR_MSG_ALL, PR_SVC_FINALE);
				WriteString (PR_MSG_ALL, "As the corpse of the monstrous entity\nChthon sinks back into the lava whence\nit rose, you grip the Rune of Earth\nMagic tightly. Now that you have\nconquered the Dimension of the Doomed,\nrealm of Earth Magic, you are ready to\ncomplete your task. A Rune of magic\npower lies at the end of each haunted\nland of Quake. Go forth, seek the\ntotality of the four Runes!");
			}
			return;
		}
		else if (world->model == "maps/e2m6.bsp")
		{
			WriteByte (PR_MSG_ALL, PR_SVC_CDTRACK);
			WriteByte (PR_MSG_ALL, 2);
			WriteByte (PR_MSG_ALL, 3);

			WriteByte (PR_MSG_ALL, PR_SVC_FINALE);
			WriteString (PR_MSG_ALL, "The Rune of Black Magic throbs evilly in\nyour hand and whispers dark thoughts\ninto your brain. You learn the inmost\nlore of the Hell-Mother; Shub-Niggurath!\nYou now know that she is behind all the\nterrible plotting which has led to so\nmuch death and horror. But she is not\ninviolate! Armed with this Rune, you\nrealize that once all four Runes are\ncombined, the gate to Shub-Niggurath's\nPit will open, and you can face the\nWitch-Goddess herself in her frightful\notherworld cathedral.");
			return;
		}
		else if (world->model == "maps/e3m6.bsp")
		{
			WriteByte (PR_MSG_ALL, PR_SVC_CDTRACK);
			WriteByte (PR_MSG_ALL, 2);
			WriteByte (PR_MSG_ALL, 3);

			WriteByte (PR_MSG_ALL, PR_SVC_FINALE);
			WriteString (PR_MSG_ALL, "The charred viscera of diabolic horrors\nbubble viscously as you seize the Rune\nof Hell Magic. Its heat scorches your\nhand, and its terrible secrets blight\nyour mind. Gathering the shreds of your\ncourage, you shake the devil's shackles\nfrom your soul, and become ever more\nhard and determined to destroy the\nhideous creatures whose mere existence\nthreatens the souls and psyches of all\nthe population of Earth.");
			return;
		}
		else if (world->model == "maps/e4m7.bsp")
		{
			WriteByte (PR_MSG_ALL, PR_SVC_CDTRACK);
			WriteByte (PR_MSG_ALL, 2);
			WriteByte (PR_MSG_ALL, 3);

			WriteByte (PR_MSG_ALL, PR_SVC_FINALE);
			WriteString (PR_MSG_ALL, "Despite the awful might of the Elder\nWorld, you have achieved the Rune of\nElder Magic, capstone of all types of\narcane wisdom. Beyond good and evil,\nbeyond life and death, the Rune\npulsates, heavy with import. Patient and\npotent, the Elder Being Shub-Niggurath\nweaves her dire plans to clear off all\nlife from the Earth, and bring her own\nfoul offspring to our world! For all the\ndwellers in these nightmare dimensions\nare her descendants! Once all Runes of\nmagic power are united, the energy\nbehind them will blast open the Gateway\nto Shub-Niggurath, and you can travel\nthere to foil the Hell-Mother's plots\nin person.");
			return;
		}

		GotoNextMap();
	}

	if (intermission_running == 3)
	{
		if (!cvar("registered"))
		{	// shareware episode has been completed, go to sell screen
			WriteByte (PR_MSG_ALL, PR_SVC_SELLSCREEN);
			return;
		}

		if ( (serverflags&15) == 15)
		{
			WriteByte (PR_MSG_ALL, PR_SVC_FINALE);
			WriteString (PR_MSG_ALL, "Now, you have all four Runes. You sense\ntremendous invisible forces moving to\nunseal ancient barriers. Shub-Niggurath\nhad hoped to use the Runes Herself to\nclear off the Earth, but now instead,\nyou will use them to enter her home and\nconfront her as an avatar of avenging\nEarth-life. If you defeat her, you will\nbe remembered forever as the savior of\nthe planet. If she conquers, it will be\nas if you had never been born.");
			return;
		}

	}

	RPrint("Exit Intermission.\n");

	GotoNextMap();
}

/*
============
IntermissionThink

When the player presses attack or jump, change to the next level
============
*/
void IntermissionThink()
{
	if (time < intermission_exittime)
		return;

	if (!self->PR_BUTTON_FIRE && !self->PR_BUTTON_USE && !self->PR_BUTTON_JUMP)
		return;

	if (ServerUpdating())
		return;

	if (!triggered_cycle)
	{
		RPrint("Intermission think.\n");
		triggered_cycle = PR_TRUE;
		GotoNextMap();
	}
}

/*
============
execute_changelevel

The global "nextmap" has been set previously.
Take the players to the intermission spot
============
*/
void PrintResults();
void execute_changelevel()
{
	entity	pos;
	RPrint("execute_changelevel()\n");

	PrintResults();

	World_FadeEnd(2.9, 2.9, 0);

	intermission_running = 1;

	// enforce a wait time before allowing changelevel
	intermission_exittime = time + PR_INTERMISSION_LENFORCED;

	pos = FindIntermission ();

	// play intermission music
	// OfN TODO: remove this?
	//WriteByte (#MSG_ALL, #SVC_CDTRACK);
	//WriteByte (#MSG_ALL, 3);

	WriteByte (PR_MSG_ALL, PR_SVC_INTERMISSION);
	WriteCoord (PR_MSG_ALL, pos->origin[X]);
	WriteCoord (PR_MSG_ALL, pos->origin[Y]);
	WriteCoord (PR_MSG_ALL, pos->origin[Z]);
	WriteAngle (PR_MSG_ALL, pos->mangle[X]);
	WriteAngle (PR_MSG_ALL, pos->mangle[Y]);
	WriteAngle (PR_MSG_ALL, pos->mangle[Z]);

	other = find (world, "classname", "player");
	while (other != world)
	{
		other->takedamage = PR_DAMAGE_NO;
		other->solid = PR_SOLID_NOT;
		other->movetype = PR_MOVETYPE_NONE;
		other->modelindex = 0;
		other = find (other, "classname", "player");
	}

}


// Gizmo
void changelevel_use()
{
	other = activator;

	if (cvar("samelevel") == 2 || (cvar("samelevel") == 3 && mapname != "start")) {
		T_Damage (other, self, self, 50000);
		return;
	}

	if ( self->message  != "") {
		if ( !( self->spawnflags & 4 ) )
			bprint( PR_PRINT_HIGH, other->netname );
		bprint( PR_PRINT_HIGH, self->message );
		bprint( PR_PRINT_HIGH, "\n" );
	} else {
		bprint (PR_PRINT_HIGH, other->netname);
		bprint (PR_PRINT_HIGH, " exited the level\n");
	}

	nextmap = self->map;

	SUB_UseTargets ();

	if ( (self->spawnflags & 1) && (deathmatch == 0) )
	{	// NO_INTERMISSION
		GotoNextMap();
		return;
	}

	self->use = SUB_Null;
	self->touch = SUB_Null;

	// we can't move people right now, because touch functions are called
	// in the middle of C movement code, so set a think time to do it
	self->think = execute_changelevel;
	self->nextthink = time + 0.1;
}

void changelevel_touch()
{
	//local entity pos;
	//local float ne;
	//local string st;

	if (other->classname != "player")
		return;

	if (cvar("samelevel") == 2 || (cvar("samelevel") == 3 && mapname != "start")) {
		T_Damage (other, self, self, 50000);
		return;
	}

	bprint (PR_PRINT_HIGH, other->netname);
	bprint (PR_PRINT_HIGH, " exited the level\n");

	nextmap = self->map;

	SUB_UseTargets ();

	if ( (self->spawnflags & 1) && (deathmatch == 0) )
	{	// NO_INTERMISSION
		GotoNextMap();
		return;
	}

	self->use = SUB_Null;
	self->touch = SUB_Null;

	// we can't move people right now, because touch functions are called
	// in the middle of C movement code, so set a think time to do it
	self->think = execute_changelevel;
	self->nextthink = time + 0.1;
}

/*QUAKED trigger_changelevel (0.5 0.5 0.5) ? NO_INTERMISSION NOTOUCH NOPLAYERNAME
When the player touches this, he gets sent to the map listed in the "map" variable.  Unless the NO_INTERMISSION flag is set, the view will go to the info_intermission spot and display stats.
*/
void trigger_changelevel()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	if (self->map == "")
		objerror ("changelevel trigger doesn't have map");

	InitTrigger ();

	if ( !( self->spawnflags & 2 ) )
		self->touch = changelevel_touch;

	// Gizmo - can now be triggered
	if ( self->targetname  != "")
		self->use = changelevel_use;
}


/*
=============================================================================

				PLAYER GAME EDGE FUNCTIONS

=============================================================================
*/

void set_suicide_frame();

// called by ClientKill and DeadThink
void respawn()
{
/*	if (!(self.done_custom == 0 || (self.done_custom & #CUSTOM_ON_SPAWN))) // do not spawn in prematch
		if (prematch < time)
		{
			sprint(self, #PRINT_HIGH, "Sorry, there are still ");
			sprint(self, #PRINT_HIGH, prematch - time);
			sprint(self, #PRINT_HIGH, " seconds of prematch remaining. Type custom to recreate your class.\n");
			return;
		}
*/
#ifdef PR_COOP_MODE_ENHANCED
	if (coop)
	{
		// Gizmo - if in coop 3, then do not allow respawning
		if ( coop == 3 ) {
#ifndef PR_NO_REVIVE
			if ( self->modelindex == modelindex_player ) {
				if ( self->attack_finished < time ) {
					if ( self->last_saveme_sound < time )
						sprint( self, PR_PRINT_HIGH, "You must wait for a medic!\n" );
					self->impulse = PR_TF_MEDIC_HELPME;
					DeadImpulses();
					self->attack_finished = time + 0.25;
				}
			} else {
				sprint( self, PR_PRINT_HIGH, "You're dead for good!\n" );
				PlayerObserverMode();
				self->PR_is_dead_observer = PR_TRUE;
			}
#else
			sprint( self, PR_PRINT_HIGH, "You're dead for good!\n" );
			PlayerObserverMode();
			self->PR_is_dead_observer = PR_TRUE;
#endif
			self->tfstate = self->tfstate - ( self->tfstate & PR_TFSTATE_RESPAWN_READY );
		} else {
			// make a copy of the dead body for appearances sake
			CopyToBodyQue (self);

			self->velocity = V({0, 0, 0}); // Thanks catscratch
			self->PR_BUTTON_FIRE = PR_FALSE;
			self->PR_BUTTON_USE = PR_FALSE;
			self->PR_BUTTON_JUMP = PR_FALSE;

			// get the spawn parms as they were at level start
			setspawnparms (self);
			// respawn
			PutClientInServer ();
		}
	}
	else
#endif
    if (deathmatch)
	{
		// make a copy of the dead body for appearances sake
		CopyToBodyQue (self);

		self->velocity = V({0, 0, 0}); // Thanks catscratch
		self->PR_BUTTON_FIRE = PR_FALSE;
		self->PR_BUTTON_USE = PR_FALSE;
		self->PR_BUTTON_JUMP = PR_FALSE;

		// set default spawn parms
		SetNewParms ();
		// respawn
		PutClientInServer ();
	}
	else
	{
#ifdef PR_QUAKE_WORLD
		// restart the level
		changelevel( mapname );
#else
		localcmd ("restart\n");
#endif
	}
}

/*
============
ClientKill

Player entered the suicide command
============
*/
void ClientKill()
{
	if (intermission_running)
		return;

	if (ceasefire)
	{
		sprint(self,PR_PRINT_HIGH,"You can't suicide during a ceasefire!\n");
		return;
	}

	if (self->flags & PR_FL_FINALIZED)
		return;

	// for Neo mode
	if (neo.isModeActive() && self == neo.getNeo())
	{
		if (!neo.getNeoCanChangeClass() || self->nextpc == 0 || (self->nextpc == self->playerclass && self->done_custom & PR_CUSTOM_FINISHED))
		{
			neo.removeNeo(self);
			bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^-^-^-^-^-^-^> ^bNeo Mode^b ^<^-^-^-^-^-^-^-^-^-^>\n"));
			bprint(PR_PRINT_HIGH, "Neo ("); bprint(PR_PRINT_HIGH, self->netname); 
			bprint(PR_PRINT_HIGH, ") desired to return to a state of bliss/ignorance.\n");
			bprint(PR_PRINT_HIGH, "Kill someone to become Neo!\n");
			bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
			BroadcastSound("boss1/sight1.wav");
			neo.centerPrint();
		}
	}
	// end Neo

	float finished;
	entity te;
	//WK Stop music
	sound (self, PR_CHAN_MUSIC, "misc/null.wav", 0.1, PR_ATTN_NORM);

	if (self->suicide_time > time)
		return;

	if (self->deadflag)
		return;

	// players can't suicide again for 10 seconds
	self->suicide_time = time + 5 + (random() * 5);

	//WK If building class, remove flag and start over
	if (self->playerclass == PR_PC_CUSTOM && (self->done_custom & PR_CUSTOM_BUILDING))
	{
		sprint(self, PR_PRINT_HIGH,"You can type \xA2""custom\xA2 again to start over!\n");
		return;
		//WK self.done_custom = #CUSTOM_ON_SPAWN | #CUSTOM_FINISHED;
	}
	if (self->playerclass == PR_PC_UNDEFINED)
	{
		sprint(self, PR_PRINT_HIGH,"You aren't alive!\n");
		return;
	}

	if (prematch < time)
	{
		bprint (PR_PRINT_MEDIUM, self->netname);
		bprint (PR_PRINT_MEDIUM, " suicides\n");
	}

	set_suicide_frame();
	self->modelindex = modelindex_player;

	// If infected, give the medic a frag
	if (self->tfstate & PR_TFSTATE_INFECTED)
	{
		finished = PR_FALSE;
		te = find(world, "classname", "timer");
		while (te != world)
		{
			if (te->owner == self && te->think == BioInfection_Decay)
			{
			#ifdef PR_QUAKE_WORLD
				logfrag(te, self);
			#endif
				te->enemy->real_frags = te->enemy->real_frags + 1;
				if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
					te->enemy->frags = te->enemy->real_frags;
				finished = PR_TRUE; //Thanks lostman
			}

			te = find(te, "classname", "timer");
			if (finished) te = world;
		}
	}
#ifdef PR_QUAKE_WORLD
	else
		logfrag (self, self);
#endif

	self->real_frags = self->real_frags - 1;
	if (teamplay & PR_TEAMPLAY_VAMPIRE) //WK
		self->real_frags = self->real_frags - 1;

	if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
		self->frags = self->real_frags;

	self->weaponmodel= "";
	self->view_ofs = V({0, 0, -8});
	self->movetype = PR_MOVETYPE_NONE;

	//WK If building class, remove flag and start over
	if (self->playerclass == PR_PC_CUSTOM && (self->done_custom & PR_CUSTOM_BUILDING))
	{
		sprint(self, PR_PRINT_HIGH,"You can type \xA2""custom\xA2 again to start over!\n");
		self->done_custom = PR_CUSTOM_ON_SPAWN | PR_CUSTOM_FINISHED;
		return; //ch
	}

	// Remove all timers for this player
	TeamFortress_RemoveTimers();
	TeamFortress_SetupRespawn(PR_TRUE);

	// Drop any runes he would carry
	PlayerDropRunes(self);

	self->health = -1;
	self->th_die();
//	self.deadflag = #DEAD_RESPAWNABLE;		// Gizmo - commented this out, th_die() takes care of it
	self->tfstate = self->tfstate | PR_TFSTATE_RESPAWN_READY;
//	self.respawn_time = time;
}

////////////////////////////////////
// FindTeamSpawnPoint
//////////
entity lastspawn_team1;
entity lastspawn_team2;
entity lastspawn_team3;
entity lastspawn_team4;

entity FindTeamSpawnPoint(float team_num)
{
	entity spot;
	entity at_spot;
	float spot_found;
	float attempts;

	if (team_num == 1)
	{
		spot = lastspawn_team1;

		attempts = 0;
		// search through until found or end-of-list
		while (1)
		{
			attempts = attempts + 1;

			spot = find(spot, "team_str_home", "ts1");

			if (spot == world)
				spot = find(world, "team_str_home", "ts1");

			if (spot == world)
				return world;

			at_spot = findradius(spot->origin, 40);
			spot_found = PR_TRUE;

			while (at_spot != world)
			{
				if (at_spot->classname == "player" && at_spot->deadflag == PR_DEAD_NO)
				{
					spot_found = PR_FALSE;
				}

				at_spot = at_spot->chain;
			}

			// Check the Criteria of the spawnpoint
			if (!Activated(spot, self))
				spot_found = PR_FALSE;

			if ((spot_found) || (attempts >= 30))
			{
				lastspawn_team1 = spot;
				return spot;
			}
		}

	}
	else if (team_num == 2)
	{
		spot = lastspawn_team2;

		attempts = 0;
		// search through until found or end-of-list
		while (1)
		{
			attempts = attempts + 1;

			spot = find(spot, "team_str_home", "ts2");

			if (spot == world)
				spot = find(world, "team_str_home", "ts2");

			if (spot == world)
				return world;

			at_spot = findradius(spot->origin, 40);
			spot_found = PR_TRUE;

			while (at_spot != world)
			{
				if (at_spot->classname == "player" && at_spot->deadflag == PR_DEAD_NO)
				{
					spot_found = PR_FALSE;
				}

				at_spot = at_spot->chain;
			}

			// Check the Criteria of the spawnpoint
			if (!Activated(spot, self))
				spot_found = PR_FALSE;

			if ((spot_found) || (attempts >= 30))
			{
				lastspawn_team2 = spot;
				return spot;
			}
		}
	}
	else if (team_num == 3)
	{
		spot = lastspawn_team3;

		attempts = 0;
		// search through until found or end-of-list
		while (1)
		{
			attempts = attempts + 1;

			spot = find(spot, "team_str_home", "ts3");

			if (spot == world)
				spot = find(world, "team_str_home", "ts3");

			if (spot == world)
				return world;

			at_spot = findradius(spot->origin, 40);
			spot_found = PR_TRUE;

			while (at_spot != world)
			{
				if (at_spot->classname == "player" && at_spot->deadflag == PR_DEAD_NO)
				{
					spot_found = PR_FALSE;
				}

				at_spot = at_spot->chain;
			}

			// Check the Criteria of the spawnpoint
			if (!Activated(spot, self))
				spot_found = PR_FALSE;

			if ((spot_found) || (attempts >= 30))
			{
				lastspawn_team3 = spot;
				return spot;
			}
		}
	}
	else if (team_num == 4)
	{
		spot = lastspawn_team4;

		attempts = 0;
		// search through until found or end-of-list
		while (1)
		{
			attempts = attempts + 1;

			spot = find(spot, "team_str_home", "ts4");

			if (spot == world)
				spot = find(world, "team_str_home", "ts4");

			if (spot == world)
				return world;

			at_spot = findradius(spot->origin, 40);
			spot_found = PR_TRUE;

			while (at_spot != world)
			{
				if (at_spot->classname == "player" && at_spot->deadflag == PR_DEAD_NO)
				{
					spot_found = PR_FALSE;
				}

				at_spot = at_spot->chain;
			}

			// Check the Criteria of the spawnpoint
			if (!Activated(spot, self))
				spot_found = PR_FALSE;

			if ((spot_found) || (attempts >= 30))
			{
				lastspawn_team4 = spot;
				return spot;
			}
		}
	}

	// failure
	return world;
}

/*
============
SelectSpawnPoint

Returns the entity to spawn at
============
*/
entity SelectSpawnPoint()
{
	entity spot;
	entity at_spot;
	float  spot_found;
	float  attempts;

	// testinfo_player_start is only found in regioned levels
#ifdef PR_COOP_MODE_ENHANCED
	if ( !deathmatch ) {
		spot = find (world, "classname", "testplayerstart");
		if (spot != world)
			return spot;
	}
#endif

	// If FortressMap option is on, we want to make the player spawn on a
	// spawnpoint marked as being one for his/her team.
	// The team that owns a spawnpoint is kept in the spawnpoints's teamno
	if (self->team_no != 0)
	{
		//entity oldSelf = self, fakeSelf = world;   // PZ: For Neo mode --> Apparently none of this is needed.

		// PZ: In Neo Mode, give the player the spawn point of a random team.
		//     --> Apparently none of this is needed. Works without it.
		/*if (neo.isModeActive())
		{
			// NOTE: rand() should be seeded by now because it's used by the random() built-in.
			const int MINIMUM_TEAM_NUMBER = 1;
			int teamNum = MINIMUM_TEAM_NUMBER + (rand() % (static_cast<int>(number_of_teams) - MINIMUM_TEAM_NUMBER + 1));
			// Create a fake entity copy of `self` that is on the team selected, to trick TF code into spawning players on a random /team/ spawnpoint.
			// Without this, players will spawn in non-team spawnpoints, typically in the middle of the map.
			fakeSelf = spawnServerSide();
			// WARNING: This is risky, but should only affect Neo Mode if it breaks something. We delete the entity at the end of this function, 
			//          to greatly minimize the risk.
			auto correct_edict_ptr = fakeSelf->_edict;
			*fakeSelf = *self; // NOTE: This messes up its _edict, which the engine does not like.
			fakeSelf->_edict = correct_edict_ptr;
			fakeSelf->team_no = teamNum;
			self = fakeSelf;
		}*/

		spot = FindTeamSpawnPoint(self->team_no);

		// PZ: For Neo mode. Get rid of the fake entity. It causes problems with other code if it's allowed to continue
		//     to exist after this function returns.
		//     --> Apparently none of this is needed. Works without it.
		//if (fakeSelf != world) remove(fakeSelf);
		//if (neo.isModeActive()) self = oldSelf;

		if (spot != world)
			return spot;

		// failure to find a team spawn point for that player
		// just move on
	}

	// choose a info_player_deathmatch point
#ifdef PR_COOP_MODE_ENHANCED
	if (coop)
	{
		lastspawn = find(lastspawn, "classname", "info_player_coop");
		if (lastspawn == world)
			lastspawn = find (world, "classname", "info_player_coop");

		if (lastspawn != world)
			return lastspawn;
	}
	else
#endif

	if (deathmatch)
	{
		// search through
		spot = find(lastspawn, "classname", "info_player_deathmatch");
		if (spot == world)
			spot = find(world, "classname", "info_player_deathmatch");

		attempts = 0;
		while (spot != world && attempts < 100)
		{
			attempts = attempts + 1;

			// reject spot if other players are found at point
			at_spot = findradius(spot->origin, 40);
			spot_found = PR_TRUE;
			while (at_spot != world)
			{
				if (at_spot->classname == "player" && at_spot->deadflag == PR_DEAD_NO)
				{
					spot_found = PR_FALSE;
				}

				at_spot = at_spot->chain;
			}

			// Make sure we don't get locked by people standing on all
			// the spawnpoints.
			if ((spot_found) || (attempts >= 10))
			{
				lastspawn = spot;
				return spot;
			}

			spot = find(spot, "classname", "info_player_deathmatch");

			if (spot == world)
				spot = find(world, "classname", "info_player_deathmatch");
		}
	}

	if (serverflags)
	{	// return with a rune to start
		spot = find (world, "classname", "info_player_start2");
		if (spot != world)
			return spot;
	}

	spot = find (world, "classname", "info_player_start");
	if (spot == world)
		error ("PutClientInServer: no info_player_start on level\n");

	return spot;
}

//================================================
// Cleans up everything ^_^

void CleanUpEverything()
{
	SetMeatUsage(self,4);
	SetHeadsUsage(self,4);
	SetCurrentSoul(self,0);

	if (self->has_holo > 0 ) RemoveHolo(self);

	kill_my_demons();
	RemoveArmyTimer();

	DetonateMines(self);
	DetonateAllGunsForced();

	if (self->job & PR_JOB_ARMY)
		self->delay_time = PR_FALSE;
	else
		self->delay_time = PR_TRUE; // Avoids setting an armytimer when not needed by switching/rebuilding class*/

	if (!(self->flags & PR_FL_FINALIZED)) // Only if not a "finalized" player
		self->dont_do_triggerwork = 0; // warlock points/guerilla mine kills or army rating

	self->increase_team4 = 0; // used by army/warlock status bar and guerilla stuff

	// Clean up pointers
	// fixes like when having hacked something and getting army/warlock, phantom monsters
	self->demon_one = world;
	self->demon_two = world;
	self->demon_three = world;

	TeamFortress_RemoveTimers();

	RemoveArmyTimer();

	// Reset our "job initialization done" flag to false
	self->goal_activation = PR_FALSE;
}

void DecodeLevelParms();
void PlayerDie();
void TeamFortress_SetHealth();
void TeamFortress_SetEquipment();
void player_touch();
void TeamFortress_SetSpeed(entity p);
void TeamFortress_SetSkin(entity p);

/*
===========
SetClientAlive

Sets a player so that he is alive.
Their health and armor are set to the standard respawning amounts.
Note that this sets the player's frame also.
===========
*/
void SetClientAlive() {
	entity	te;

	self->th_pain = player_pain;
	self->th_die = PlayerDie;
	self->touch = player_touch;
	self->classname = "player";
	self->takedamage = PR_DAMAGE_AIM;
	self->solid = PR_SOLID_SLIDEBOX;
	self->movetype = PR_MOVETYPE_WALK;
	self->show_hostile = 0;
	self->is_malfunctioning = 0;
	self->is_abouttodie = 0;

	// OfN
	self->has_holo = 0;
	self->is_killed = PR_FALSE;

	self->message = ""; // reset for gremlin and cool gibs (heads)
	self->FlashTime = 0;
	self->flags = PR_FL_CLIENT;
	self->aura = 0; //- OfN
	self->crusader_inspirator=world; // OfN - needed?
	self->gravity = 1; //WK
	self->air_finished = time + 12;
	self->dmg = 2;			// initial water damage
	self->super_damage_finished = 0;
	self->radsuit_finished = 0;
	self->invisible_finished = 0;
	self->invincible_finished = 0;
	self->effects = 0;
	self->invincible_time = 0;
	self->reload_shotgun = 0;
	self->reload_super_shotgun = 0;
	self->reload_grenade_launcher = 0;
	self->reload_rocket_launcher = 0;
	self->reload_light_assault = 0;
	self->reload_laser_cannon = 0;
	self->last_attacked_time = 0; //WK For chaplan healing purposes

	self->deadflag = PR_DEAD_NO;
	// pausetime is set by teleporters to keep the player from moving a while
	self->pausetime = 0;
	self->view_ofs = V({0, 0, 22});

	// grapple stuff
	self->on_hook = PR_FALSE;
	self->hook_out = PR_FALSE;
	self->fire_held_down = PR_FALSE;

	// Reset all tfstate flags, except for RANDOMPC and ZOOMOFF
	self->tfstate = self->tfstate & ( PR_TFSTATE_RANDOMPC | PR_TFSTATE_ZOOMOFF );

	makeImmune( self, time + 5 );

	if (self->playerclass != PR_PC_CUSTOM)
	{
		self->max_health = 100;
		self->health = 100;
	}

	// OfN - FIx grenade on spawn
	te = find(world,"classname","grenade_timer");
	while (te != world)
	{
		if (te->owner == self)
		{
			te->nextthink = time + 0.1;
			te->think = SUB_Remove;
		}

		te = find(te,"classname","grenade_timer");
	}

	// Set the weapons and ammo for the player based on class
	TeamFortress_SetEquipment();
	// Set the health for the player based on class
	TeamFortress_SetHealth();
	// Set the speed for the player based on class
	TeamFortress_SetSpeed(self);
	// Set the skin for the player based on class
	TeamFortress_SetSkin(self);
	stuffcmd(self, "v_idlescale 0\n");
	stuffcmd(self, "v_cshift 0 0 0 0\n");
	//WK Clear Bastard Rotation
	stuffcmd(self, "-left;-right;cl_yawspeed 140\n");

#ifdef PR_QUAKE_WORLD
	setmodel (self, string_null);
	modelindex_null = self->modelindex;
#endif

	setmodel (self, "progs/eyes.mdl");
	modelindex_eyes = self->modelindex;

	setmodel (self, "progs/player.mdl");
	modelindex_player = self->modelindex;

	setsize (self, PR_VEC_HULL_MIN, PR_VEC_HULL_MAX);
}

/*
===========
PutClientInServer

called each time a player is spawned
============
*/
void PutClientInServer()
{
#ifdef PR_MAP_DEBUG
	string st;
#endif
	float iszoom, oldclass;
	entity spot, te;
	string rjtmp;

	DecodeLevelParms ();

/*	if (self.is_cameraviewing)
		SwitchFromCamera();*/

#ifdef PR_COOP_MODE_ENHANCED
	// Force all players to be on the same team in Coop mode
	if ( coop ) {
		// teamplay must be at least set to something
		if ( !teamplay ) {
			teamplay = 21;
			cvar_set( "teamplay", "21?TeamFortress" );
		}

		// only set team if we're not already on a team
		if ( !self->team_no )
			TeamFortress_TeamSet( 1, 1 );
	}
#endif

	// Set the Civilian Class of anyone in a Civilian Team
	if (self->playerclass == PR_PC_UNDEFINED)
	{
		if (TeamFortress_TeamIsCivilian(self->team_no))
		{
			self->impulse = 1;
			TeamFortress_ChangeClass();
		}
	}

	// For players who've changed their classes in deathmatch 3,
	// their class may be PC_RANDOM, in which case we set the toggleflag
#ifdef PR_COOP_MODE_ENHANCED
	if ( ( coop && self->nextpc != 0 ) || ( deathmatch == 3 && self->nextpc != 0 ) ) {
#else
	if ((deathmatch == 3) && (self->nextpc != 0)) {
#endif
		// Did we switch our player class?
		if (self->playerclass != self->nextpc)
		if (!(self->done_custom & PR_CUSTOM_ON_SPAWN)) // and not going to customize, which will call it on DropToCustom
		{
			CleanUpEverything(); // if so, kill all our stuff
			ResetStripedAndGiven(self);

			// PZ: if we used "changeclass", display the class help on spawn
			if (PR_DISPLAY_CLASS_HELP)
				self->current_menu = PR_MENU_CLASSHELP;
			self->menu_displaytime = 0;
			// PZ: END
		}

		self->playerclass = self->nextpc;

		if (self->nextpc == PR_PC_RANDOM)
			self->tfstate = self->tfstate | PR_TFSTATE_RANDOMPC;
		else
			self->tfstate = self->tfstate - (self->tfstate & PR_TFSTATE_RANDOMPC);
	}
	// PZ: If the player used the "changeteam" command, handle it here, on respawn.
	if (self->new_team_no)
	{
		TeamFortress_TeamSet(self->new_team_no, 0); // PZ: don't perform checks, just add him
		self->new_team_no = 0;                      // set to 0 once handled
	}

	// some states are kept
	iszoom = 0;

	if (self->tfstate & PR_TFSTATE_ZOOMOFF)
		iszoom = 1;

	// Reset all tfstate flags, except for RANDOMPC
	if (self->tfstate & PR_TFSTATE_RANDOMPC)
	{
		oldclass = self->playerclass;
		self->playerclass = 1 + floor(random() * (PR_PC_RANDOM - 1));
		while(!IsLegalClass(self->playerclass) || (self->playerclass == oldclass))
			self->playerclass = 1 + floor(random() * (PR_PC_RANDOM - 1));
		self->tfstate = PR_TFSTATE_RANDOMPC;
	}
	else
		self->tfstate = 0;

	if (iszoom == 1)
		self->tfstate = self->tfstate | PR_TFSTATE_ZOOMOFF;

	// Gizmo - created SetClientAlive() so it can be used in other places as well
	SetClientAlive();

	// Display the Player's Class
	if (!(self->done_custom == 0 || (self->done_custom & PR_CUSTOM_ON_SPAWN)))
	{
		if (self->playerclass != PR_PC_CUSTOM)
			TeamFortress_PrintClassName(self,self->playerclass, (self->tfstate & PR_TFSTATE_RANDOMPC));
		else
			TeamFortress_PrintJobName(self,self->job);
	}

	//WK Again, since they could lose their sentrygun in SetEquip...
	DetonateAllGuns();

#ifdef PR_QUAKE_WORLD
	SetTeamName(self);
#endif

	self->attack_finished = time + 0.3;

	// make sure that autozoom is reset
	if (self->height != 0)
	{
		self->height = 0;
		TF_zoom(90);
	}

	spot = SelectSpawnPoint ();

	self->observer_list = spot;
	self->origin = spot->origin + V({0, 0, 1});
	self->angles = spot->angles;
	self->fixangle = PR_TRUE;		// turn this way immediately

#ifdef PR_MAP_DEBUG
	RPrint(self->netname);
	RPrint(" spawned at a ");
	RPrint(spot->classname);
	if (spot->team_no != 0)
	{
		RPrint(", team_no of ");
		st = ftos(spot->team_no);
		RPrint(st);
	}
	RPrint("\n");
#endif

	// If this is a TeamSpawnpoint, check to see if it
	// gives out a GoalItem, or displays a message
	if (spot->classname == "info_player_teamspawn")
	{
		if (spot->items != 0)
		{
			te = Finditem(spot->items);
			if (te != world)
				tfgoalitem_GiveToPlayer(te, self, self);

			if (!(spot->goal_activation & PR_TFSP_MULTIPLEITEMS))
				spot->items = 0;
		}

		// Display teamspawn message
		if (spot->message != "")
		{
			CenterPrint(self, spot->message);

			if (!(spot->goal_activation & PR_TFSP_MULTIPLEMSGS))
				spot->message = string_null;
		}

		// Activate a Goal if needed
		if (spot->activate_goal_no != 0)
		{
			te = Findgoal(spot->activate_goal_no);
			if (te != world)
				AttemptToActivate(te, self, spot);
		}

		// TeamSpawn points can remove themselves after being spawned on
		if (spot->goal_effects == PR_TFSP_REMOVESELF)
		{
			spot->classname = "deadpoint";
			spot->team_str_home = string_null;
			spot->nextthink = time + 1;
			spot->think = SUB_Remove;
		}
	}

	if (self->playerclass == PR_PC_UNDEFINED)
	{
#ifdef PR_QUAKE_WORLD
		self->modelindex = modelindex_null;
#else
		setmodel(self, string_null);
#endif
		self->current_menu = 1;
		self->gravity = 0;
	}

	//WK DropIntoCustomClassGeneration
	if (self->playerclass == PR_PC_CUSTOM)
	{
//WK Done_custom is initialized to 0, hackish yes.
//There are two entries, unintialized 0, or a person having issued a 'custom' command
//which sets the #CUSTOM_ON_SPAWN flag
		if (self->done_custom == 0 || (self->done_custom & PR_CUSTOM_ON_SPAWN))
		{
			DropToCustomClassGen();
		}
		else
		{
			self->maxspeed = self->custom_speed; //Reset our speed
		}
	}
	else {
		self->done_custom = 0; //Unitialize this. Maybe set to ON_SPAWN...
	}

	player_stand1 ();

	if (deathmatch || coop)
	{
		makevectors(self->angles);
		if (self->playerclass != PR_PC_UNDEFINED) {
			spawn_tfog (self->origin + v_forward*10);
			spawn_tdeath (self->origin, self);
		}
	}

#ifdef PR_QUAKE_WORLD
	// Set Rocket Jump Modifiers

	rjtmp = infokey(world, "rj");
	if (stof(rjtmp) != 0)
		rj = stof(rjtmp);
	else
		rj = 1;
#endif

	//This code is in three places. client,custom & tfort.qc
	//WK Give them invincibility if they are a normal class or bought it
	if (((self->playerclass >= PR_PC_SCOUT && self->playerclass <= PR_PC_RANDOM) ||
	    self->tf_items & PR_NIT_RESPAWN_GUARD) && !normalQuakeMap) // PZ: respawn guard is annoying as hell on DM maps
	{
		self->items = self->items + PR_IT_INVULNERABILITY;
		self->invincible_time = 1;
#ifdef PR_COOP_MODE_ENHANCED
		if ( !deathmatch )
			self->invincible_finished = time + PR_COOP_RESPAWN_GUARD_TIME;
		else
#endif
		self->invincible_finished = time + PR_RESPAWN_GUARD_TIME;
		if (self->custom_speed > 300)
			self->invincible_finished = self->invincible_finished - 1;
		if (self->custom_speed > 400)
			self->invincible_finished = self->invincible_finished - 1;
	}
	// PZ: give everyone full armor in deathmatch, if they are a stock class
	if (/*(self.playerclass >= #PC_SCOUT && self.playerclass <= #PC_RANDOM) && */normalQuakeMap) // PZ: give to custom players too
	{
		self->cutf_items = self->cutf_items | PR_CUTF_FULLARMOUR; // not working; I'm just going to do the below  // PZ: re-added this
		self->armorvalue = self->maxarmor;
		// Should probably use MakeFree() on this for custom class editing. Only problem is if menus ever change. (MakeFree() relies on menu order.)
	}

	// PZ: give them a message every time they spawn to remind them to press 9 to view the help
	CenterPrint(self, S_("Press ^9 to view the help,\nfor commands, etc.       \n"));

	// PZ: for Neo mode
	if (neo.isModeActive() && self == neo.getNeo())
		self->effects = self->effects | (PR_EF_BLUE | PR_EF_DIMLIGHT);
}


/*
=============================================================================

				QUAKED FUNCTIONS

=============================================================================
*/


#ifdef PR_COOP_MODE_ENHANCED
/*
==============
ValidSpawnPoint

Checks if the given spawn point is stuck inside the world.
For maps with bad coops spawns like terra6.
==============
*/
float ValidSpawnPoint( entity point) {
	entity	tent, oself;
	float 	ret;

	tent = spawnServerSide(); // PZ: make it a server-side only entity
	tent->solid = PR_SOLID_SLIDEBOX;
	tent->movetype = PR_MOVETYPE_STEP;
	setsize( tent, PR_VEC_HULL_MIN, PR_VEC_HULL_MAX );
	setorigin( tent, point->origin );

	oself = self;
	self = tent;

	self->origin[Z] = self->origin[Z] + 1;
	droptofloor();

	ret = walkmove( 0, 0 );

	self = oself;

	remove( tent );
	return ret;
}
#endif


/*QUAKED info_player_start (1 0 0) (-16 -16 -24) (16 16 24)
The normal starting point for a level.
*/
void info_player_start()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}
}


/*QUAKED info_player_start2 (1 0 0) (-16 -16 -24) (16 16 24)
Only used on start map for the return point from an episode.
*/
void info_player_start2()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}
}


/*
saved out by quaked in region mode
*/
void testplayerstart()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

}

/*QUAKED info_player_deathmatch (1 0 1) (-16 -16 -24) (16 16 24)
potential spawning position for deathmatch games
*/
void info_player_deathmatch()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

#ifdef PR_COOP_MODE_ENHANCED
	// if we're in singleplayer or coop, then remove the spawn to save entities
	if ( !deathmatch ) {
		remove( self );
		return;
	}
#endif
}

/*QUAKED info_player_coop (1 0 1) (-16 -16 -24) (16 16 24)
potential spawning position for coop games
*/
void info_player_coop()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

#ifdef PR_COOP_MODE_ENHANCED
	// if we're not in coop mode, then remove the spawn to save entities
	if ( !coop ) {
		remove( self );
		return;
	}
	if ( !ValidSpawnPoint( self ) ) {
		dprint( "Bad placement of coop spawn at: " );
		dprint( vtos( self->origin ) );
		dprint( "\n" );

		remove( self );
		return;
	}
#endif
}

/*
===============================================================================

RULES

===============================================================================
*/

void PrintClientScore(entity c)
{
	if (c->frags > -10 && c->frags < 0)
		bprint (PR_PRINT_MEDIUM, " ");
	else if (c->frags >= 0)
	{
		if (c->frags < 100)
			bprint (PR_PRINT_MEDIUM, " ");
		if (c->frags < 10)
			bprint (PR_PRINT_MEDIUM, " ");
	}
	bprint (PR_PRINT_MEDIUM, ftos(c->frags));
	bprint (PR_PRINT_MEDIUM, " ");
	bprint (PR_PRINT_MEDIUM, c->netname);
	bprint (PR_PRINT_MEDIUM, "\n");
}

void DumpScore()
{
	entity	e, sort, walk;

	if (world->chain != world)
		error ("DumpScore: world.chain is set");

// build a sorted lis
	e = find(world, "classname", "player");
	sort = world;
	while (e != world)
	{
		if (sort == world)
		{
			sort = e;
			e->chain = world;
		}
		else
		{
			if (e->frags > sort->frags)
			{
				e->chain = sort;
				sort = e;
			}
			else
			{
				walk = sort;
				do
				{
					if (walk->chain == world)
					{
						e->chain = world;
						walk->chain = e;
					}
					else if (walk->chain->frags < e->frags)
					{
						e->chain = walk->chain;
						walk->chain = e;
					}
					else
						walk = walk->chain;
				} while (walk->chain != e);
			}
		}

		e = find(e, "classname", "player");
	}

// print the list

	bprint (PR_PRINT_MEDIUM, "\n");
	while (sort != world)
	{
		PrintClientScore (sort);
		sort = sort->chain;
	}
	bprint (PR_PRINT_MEDIUM, "\n");
}

/*
go to the next level for deathmatch
*/
float already_cycled;

void NextLevel()
{
	if (ServerUpdating())
		return;

	entity o;
	//local string st;

	if (already_cycled)
		return;

	agr = 0;    // PZ - makes timer sounds and such stop
	invade = 0; // PZ - for Invade mode

	already_cycled = PR_TRUE;

	o = spawnServerSide(); // PZ: make it a server-side only entity
	o->map = nextmap;
	o->think = execute_changelevel;
	o->nextthink = time + 0.1;
}

/*
============
CheckRules

Exit deathmatch games upon conditions
============
*/
void CheckRules()
{
	/*if (ServerUpdating())
		return;*/ //OPTIMIZED: checked on NextLevel, because this crap is called every frame by all clients..

	if (timelimit && time >= timelimit)
		NextLevel ();
	else if (fraglimit && self->frags >= fraglimit)
		NextLevel ();
}

//============================================================================

void PlayerDeathThink()
{
	if (self->flags & PR_FL_FINALIZED)
		return;

	//local entity	old_self;
	float 	forward;

	if ((self->flags & PR_FL_ONGROUND) && self->PR_coolflag != PR_STRFLAG_COOL)
	{
		forward = vlen (self->velocity);
		forward = forward - 20;
		if (forward <= 0)
			self->velocity = V({0, 0, 0});
		else
			self->velocity = forward * normalize(self->velocity);
	}

	// wait for all buttons released
	if (self->deadflag == PR_DEAD_DEAD)
	{
		if (self->PR_BUTTON_JUMP || self->PR_BUTTON_USE || self->PR_BUTTON_FIRE)
			return;
		self->deadflag = PR_DEAD_RESPAWNABLE;
		// make sure that respawn flag has not been set
		self->tfstate = self->tfstate - (self->tfstate & PR_TFSTATE_RESPAWN_READY);
		return;
	}
	// wait for any button down
	if (!self->PR_BUTTON_JUMP && !self->PR_BUTTON_USE && !self->PR_BUTTON_FIRE)
	{
		// if no buttons, but respawn_ready flag is set, respawn
		if (self->tfstate & PR_TFSTATE_RESPAWN_READY)
		{
			if (self->respawn_time <= time)
			{
				self->PR_BUTTON_FIRE = 0;
				self->PR_BUTTON_USE = 0;
				self->PR_BUTTON_JUMP = 0;
				respawn();
			}
		}
		return;
	}
	else
	{
		// button has been pressed, player is ready to respawn
		self->tfstate = self->tfstate | PR_TFSTATE_RESPAWN_READY;

		if (self->respawn_time <= time)
		{
			self->PR_BUTTON_FIRE = 0;
			self->PR_BUTTON_USE = 0;
			self->PR_BUTTON_JUMP = 0;
			respawn();
		}
		return;
	}
}

void PlayerJump()
{
	if (self->done_custom & PR_CUSTOM_BUILDING) // OfN
	{
		self->velocity = V({0, 0, 0});
		return; // Dont jump if customizing
	}

	if (self->flags & PR_FL_FINALIZED)
		return;

	// OfN - Psionized guys can't jump
	if (self->tfstate & PR_TFSTATE_PSIONIZED)
	{
		if (self->velocity[Z] > 0)
			self->velocity[Z] = 0;

		return;
	}

	//local vector start, end;

	if (self->flags & PR_FL_WATERJUMP)
		return;

	if (self->waterlevel >= 2)
	{
		// PZ: This may break something. I put this here to get the FrikBot 'Teleport to Waypoint' option working right in water.
		//self.#BUTTON_JUMP = 1; // Didn't work.

		if (self->watertype == PR_CONTENT_WATER)
			self->velocity[Z] = 100;
		else if (self->watertype == PR_CONTENT_SLIME)
			self->velocity[Z] = 80;
		else
			self->velocity[Z] = 50;
		//WK
		if (self->tf_items & PR_NIT_SCUBA)
			self->velocity[Z] = 250;

		// play swiming sound
		if (self->cutf_items & PR_CUTF_STEALTH) // SB no noise if we have stealth
			return;

		if (self->swim_flag < time)
		{
			self->swim_flag = time + 1;
			if (random() < 0.5)
				sound (self, PR_CHAN_BODY, "misc/water1.wav", 1, PR_ATTN_NORM);
			else
				sound (self, PR_CHAN_BODY, "misc/water2.wav", 1, PR_ATTN_NORM);
		}

		return;
	}

	if (!(self->flags & PR_FL_ONGROUND))
		return;

	if (!(self->flags & PR_FL_JUMPRELEASED) )
		return; 	// don't pogo stick

	// Gizmo - jump bug fix
	if (self->velocity[Z] < 0)
		self->velocity[Z] = 0;

	self->flags = self->flags - (self->flags & PR_FL_JUMPRELEASED);
	self->PR_BUTTON_JUMP = 0;
	// player jumping sound
	if (!(self->cutf_items & PR_CUTF_STEALTH)) // Only play it if we don't have stealth
		sound (self, PR_CHAN_BODY, "player/plyrjmp8.wav", 1, PR_ATTN_NORM);

	//WK Sprinters jump higher
	//SB No, people with high jump do
	if (self->cutf_items & PR_CUTF_HIGHJUMP)
		self->velocity[Z] = self->velocity[Z] + 400;

	// PZ: I made the bots use the server's C code physics, just like a real player. So, this is no longer needed. Reverted. (8-15-15)
	// PZ: FrikBot was made for normal Quake. So, it's expecting the jumping to be done in the QuakeC, as you can see below.
	//if (!self.ishuman) // in QW, player physics is done by the client (and checked by the server) for anti-lag purposes
	//	self.velocity_z = self.velocity_z + 270;
#ifndef PR_QUAKE_WORLD
	//else // PZ: added the 'else'
		self->velocity[Z] = self->velocity[Z] + 270;
#endif

	// PZ: added this
	self->timeLastJumped = time;
}


/*
===========
WaterMove

============
*/
FIELD(".float	dmgtime;")

void WaterMove()
{
//	RPrint (ftos(self.waterlevel));
	if (self->movetype == PR_MOVETYPE_NOCLIP)
		return;
	if (self->health < 0)
		return;

	if (self->flags & PR_FL_FINALIZED)
		return;

	//WK Poison attacks
	if (self->tfstate & (PR_TFSTATE_HALLUCINATING | PR_TFSTATE_TRANQUILISED))
	{
		if (self->air_finished < time)
		{	// drown!
			if (self->pain_finished < time)
			{
				self->dmg = self->dmg + 0.5;
				if (self->dmg == 2.5) //Initial damage is 2
					sprint(self,PR_PRINT_HIGH,"Your lungs are still paralyzed. Try to minimize your exposure to poison\n");
				if (self->dmg > 6)
					self->dmg = 2.2;
				if (self->dmg < self->health - 1)
					TF_T_Damage (self, world, world, self->dmg, PR_TF_TD_IGNOREARMOUR, PR_TF_TD_OTHER);
				self->pain_finished = time + 1;
			}
		}
	}
	else if (self->waterlevel != 3)
	{
		if (self->air_finished < time)
			sound (self, PR_CHAN_VOICE, "player/gasp2.wav", 1, PR_ATTN_NORM);
		else if (self->air_finished < time + 9)
			sound (self, PR_CHAN_VOICE, "player/gasp1.wav", 1, PR_ATTN_NORM);
		self->air_finished = time + 12;
		self->dmg = 2;
	}
	else if (self->air_finished < time)
	{	// drown!
		if (self->pain_finished < time)
		{
			self->dmg = self->dmg + 2;
			if (self->dmg > 15)
				self->dmg = 10;
			TF_T_Damage (self, world, world, self->dmg, PR_TF_TD_IGNOREARMOUR, PR_TF_TD_OTHER);
			self->pain_finished = time + 1;
		}
	}

	if (!self->waterlevel)
	{
		if (self->flags & PR_FL_INWATER)
		{
			// play leave water sound
			if (!(self->cutf_items & PR_CUTF_STEALTH)) // SB only if no stealth
				sound (self, PR_CHAN_BODY, "misc/outwater.wav", 1, PR_ATTN_NORM);
			self->flags = self->flags - PR_FL_INWATER;
			//WK Setspeed for Scuba commando
			if (self->tf_items & PR_NIT_SCUBA)
				TeamFortress_SetSpeed(self);
		}
		return;
	}

	if (self->watertype == PR_CONTENT_LAVA)
	{	// do damage
		if (self->dmgtime < time)
		{
			if (self->radsuit_finished > time)
				self->dmgtime = time + 1;
			else
				self->dmgtime = time + 0.2;

			// Asbestos armor helps against lava, but I doubt it'll save you :)
			TF_T_Damage (self, world, world, 10*self->waterlevel, 0, PR_TF_TD_FIRE);
		}
	}
	else if (self->watertype == PR_CONTENT_SLIME)
	{	// do damage
		if (self->dmgtime < time && self->radsuit_finished < time)
		{
			self->dmgtime = time + 1;
			//T_Damage (self, world, world, 4*self.waterlevel);
			TF_T_Damage (self, world, world, 4*self->waterlevel, 0, PR_TF_TD_ELECTRICITY);
		}
	}

	if ( !(self->flags & PR_FL_INWATER) )
	{
		// player enter water sound
		if (!(self->cutf_items & PR_CUTF_STEALTH))
		{
			if (self->watertype == PR_CONTENT_LAVA)
				sound (self, PR_CHAN_BODY, "player/inlava.wav", 1, PR_ATTN_NORM);
			else if (self->watertype == PR_CONTENT_WATER)
				sound (self, PR_CHAN_BODY, "player/inh2o.wav", 1, PR_ATTN_NORM);
			else if (self->watertype == PR_CONTENT_SLIME)
				sound (self, PR_CHAN_BODY, "player/slimbrn2.wav", 1, PR_ATTN_NORM);
		}
		self->flags = self->flags + PR_FL_INWATER;
		self->dmgtime = 0;
		//WK Setspeed for scuba commando
		if (self->tf_items & PR_NIT_SCUBA)
			TeamFortress_SetSpeed(self);
	}

#ifndef PR_QUAKE_WORLD
	if ( !(self->flags & PR_FL_WATERJUMP) )
		self->velocity = self->velocity - 0.8*self->waterlevel*frametime*self->velocity;
#endif
}

void CheckWaterJump()
{
	vector start, end;

	// check for a jump-out-of-water
	makevectors (self->angles);
	start = self->origin;
	start[Z] = start[Z] + 8;
	v_forward[Z] = 0;
	normalize(v_forward);
	end = start + v_forward*24;
	traceline (start, end, PR_TL_BSP_ONLY, self);
	if (trace_fraction < 1)
	{	// solid at waist
		start[Z] = start[Z] + self->maxs[Z] - 8;
		end = start + v_forward*24;
		self->movedir = trace_plane_normal * -50;
		traceline (start, end, PR_TL_BSP_ONLY, self);
		if (trace_fraction == 1)
		{	// open at eye level
			self->flags = self->flags | PR_FL_WATERJUMP;
			self->velocity[Z] = 225;
			self->flags = self->flags - (self->flags & PR_FL_JUMPRELEASED);
			self->teleport_time = time + 2;	// safety net
			return;
		}
	}
}


/*
================
PlayerPreThink

Called every frame before physics are run
================
*/
void PlayerPreThink()
{

//bprint(2, "called PlayerPreThink()\n");
	if (!frik_botPreThink()) // FrikBot // PZ NOTE: this is extra code for the bot; bots will still run all that follows
		return; // the rest of this function doesn't run if self.b_clientno == -1

	if (self->flags & PR_FL_FINALIZED)
		return;

	// PZ: We need bots to be able to see (traceline()) dead players (especially for medic's revive ability). So, we need to set a flag to allow that here.
	if (self->solid == PR_SOLID_NOT && self->health <= 0 && self->deadflag != PR_DEAD_NO) self->flags = self->flags | PR_FL_FINDABLE_NONSOLID;
	else                                                                                  self->flags = self->flags - (self->flags & PR_FL_FINDABLE_NONSOLID);

	//local float	mspeed, aspeed, r;
	vector src;
	//WK -- For LPB calculation
	string foo;
	float ping;

	if (self->is_feigning && self->waterlevel == 1)
	{
		self->watertype = PR_CONTENT_WATER;
		self->waterlevel = 3;
	}

	if (self->cheat_level > 0)
		self->cheat_level = self->cheat_level - 1;
	if (self->speed_level > 0) //Cyto
		self->speed_level = self->speed_level - 1;
	if (intermission_running)
	{
		IntermissionThink ();	// otherwise a button could be missed between
		return; 				// the think tics
	}

	makevectors (self->v_angle); 	// is this still used?

#ifdef PR_QUAKE_WORLD // culled by KK. was QUAKE_WORLD
	//if (infokey(world,"ceasefire")=="on") //Cyto
	if (ceasefire)
	{
		#ifndef PR_CEASEFIRE_ALLOWS_TO_MOVE
		if (self->lip)
			self->origin = self->oldorigin;
		else
		{
			self->velocity[Z] = -5000;
			self->velocity[X] = 0;
			self->velocity[Y] = 0;
			if (self->flags & PR_FL_ONGROUND)
			{
				self->oldorigin = self->origin;
				self->lip = PR_TRUE;
			}
		}
		#endif

		if (self->current_menu == PR_MENU_DEFAULT || self->current_menu == 0)
		if (self->duration < time)
		{
			if (self->PR_cprint_fx & 1)
				CenterPrint(self,S_("\x9D\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9F\n^bCeasefire Applies^b\n\x9D\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9F"));
			else
				CenterPrint(self,"\x9D\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9F\nCeasefire Applies\n\x9D\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9F");

			self->duration = time + PR_CEASEFIRE_CPRINT_RATE;

			self->PR_cprint_fx = self->PR_cprint_fx + 1;
		}

		if (!GoodCeasefireImpulse(self->impulse)) //See admin.qc
			self->impulse = 0;

		self->PR_BUTTON_FIRE = 0;
		self->PR_BUTTON_USE = 0;
		self->PR_BUTTON_JUMP = 0;
	}
	else if (self->lip)
	{
		self->lip = PR_FALSE;
		centerprint(self,"");
	}
#endif
// End Cyt0

	CheckRules ();

	//xxxx
	//WK Supercharged observer mode!
	if (self->playerclass == PR_PC_UNDEFINED)
	{
		if (self->PR_BUTTON_FIRE)
		{
			//WK Nifty code to push through walls only if we really want to
			if (vlen(self->velocity) < 200)
			{
				src = self->origin + normalize(v_forward) * 5;
				setorigin(self,src);
			}
			self->velocity = normalize(v_forward) * 400;
		}
		else
		{
			foo = infokey(self,"ping");
			ping = 200;
			if (foo != string_null)
				ping = stof(foo);
			if (ping < 300) {
				self->velocity[X] = floor((3 * self->velocity[X]) / 4);
				self->velocity[Y] = floor((3 * self->velocity[Y]) / 4);
				self->velocity[Z] = 0; //WK 1-7-7 Change so that observers don't fall at all
			}
		}
		if (self->PR_BUTTON_JUMP)
			self->velocity = V({0, 0, 0});
		return;
	//WK Removed jump == autoteam
	//WK Removed all the demo stuff stuff.
	}

	if (self->view_ofs == V({0, 0, 0}))
		return; 	// intermission or finale

	if (self->playerclass != PR_PC_UNDEFINED)
		WaterMove();

	// PZ: I made the bots use the server's C code physics, just like a real player. So, this is no longer needed. Reverted. (8-15-15)
	// PZ: Commented out defines and added check for bot instead. Bot doesn't have a client to do his physics for him. (<-Old comment.)
#ifndef PR_QUAKE_WORLD
	if (/*!self.ishuman &&*/ self->waterlevel == 2 && self->playerclass != PR_PC_UNDEFINED)
		CheckWaterJump ();
#endif

	if (self->deadflag >= PR_DEAD_DEAD)
	{
		PlayerDeathThink ();
		return;
	}

	if (self->undercover_team || self->undercover_skin || self->is_undercover)
	{
		if (self->effects & (PR_EF_DIMLIGHT | PR_EF_BRIGHTLIGHT))
		{
			sprint(self, PR_PRINT_MEDIUM, "The glowing removes your disguise.\n");
			Spy_RemoveDisguise(self);
		}
	}
	/*if (self.job & #JOB_THIEF && self.job & #JOB_FULL_HIDE)
	{
		if (self.effects & (#EF_DIMLIGHT | #EF_BRIGHTLIGHT))
		{
			sprint(self, #PRINT_MEDIUM, "The glowing removes your full invisibility.\n");
			self.job = self.job - (self.job & #JOB_FULL_HIDE);
			self.job = self.job | #JOB_ACTIVE;
		}
	} */

	if (self->deadflag == PR_DEAD_DYING)
		return; // dying, so do nothing

	// PZ: For the votebot menu. To detect termination of numerical input with Enter (jump button).
	if (self->PR_BUTTON_JUMP && botVoteMenuData[ENT_TO_NUM(self)].gettingNumInput)
	{
        voteBotValueEntryDone(self);
        self->PR_BUTTON_JUMP = 0;     // eat the jump (doesn't work because one press likely hits several frames)
	}

	if (!self->is_feigning)
	{
		if (self->PR_BUTTON_JUMP)
		{
			PlayerJump(); //WK This just does noises for jumps
		}
		else
			self->flags = self->flags | PR_FL_JUMPRELEASED;
	}

	//WK Scuba commando treads water :)
	if ((self->tf_items & PR_NIT_SCUBA) && self->waterlevel > 1) {
		if (self->velocity[Z] >= -25 && self->velocity[Z] < 0)
			self->velocity[Z] = 10; //Tread water if near a stop
	}

	//Jello Water :)
	else if (jello && self->waterlevel > 1) {
		if (jello == PR_TRUE) //Binary on/off
			self->velocity[Z] = 1000; //900 is a better number for 2forts
		else
			self->velocity[Z] = jello;
	}

// PZ: I took BH restriction out 10-19-2015, because it sucks not being able to BH and fly fast with boots, when bots at high level are still going to be very tough.
#ifdef PR_USE_BH_RESTRICTIONS_ON_FLAG_CARRIER
	// PZ: added part of WK's BH restriction code (only affects flag carriers)
	//WK 5-15-08 The bunny hop caps only apply while the player is on the ground or water or firing our hover boots
	// Can't use custom_speed since it's only set on Custom PCs
	vector tempvel;
	float tempspeed;
	if (self->flags & PR_FL_ONGROUND || self->waterlevel > 1 || (self->tf_items & PR_NIT_HOVER_BOOTS && self->PR_BUTTON_JUMP))
	{
		/*local string foobar;
		tempspeed = vlen(self.velocity);
		foobar = ftos(tempspeed);
		if (tempspeed > 100) {
			sprint(self,#PRINT_HIGH,"Speed: ");
			sprint(self,#PRINT_HIGH,foobar);
			sprint(self,#PRINT_HIGH,"Maxspeed: ");
			tempspeed = self.maxspeed;
			foobar = ftos(tempspeed);
			sprint(self,#PRINT_HIGH,foobar);
			sprint(self,#PRINT_HIGH,"\n");
		}*/

		// PZ: no Bunny Hopping for people with the flag
		// PZ: remove_spawnpoint keeps track of number of goals attached (don't restrict all glowers, just flag carriers)
		if ((self->effects & (PR_EF_DIMLIGHT | PR_EF_BRIGHTLIGHT)) && self->remove_spawnpoint)
		{
			if (self->maxspeed < 0) self->maxspeed = 0;
			tempvel[X] = self->velocity[X]; //We only care about horizontal movement
			tempvel[Y] = self->velocity[Y];
			tempvel[Z] = 0;
			tempspeed = vlen(tempvel);
			if (tempspeed > self->maxspeed)
			{
				tempspeed = self->maxspeed / tempspeed;
				self->velocity[X] = self->velocity[X] * tempspeed; //Only slow down horizontal movement
				self->velocity[Y] = self->velocity[Y] * tempspeed;
			}
		}
	}
#endif

	// teleporters can force a non-moving pause time
	if (time < self->pausetime)
		self->velocity = V({0, 0, 0});

	// Gizmo - fixed this happening while building
	if (time > self->attack_finished && self->currentammo == 0 && self->weapon > PR_WEAP_AXE && !self->is_building)
	{
		self->weapon = W_BestWeapon ();
		W_SetCurrentAmmo ();
	}

	// Do grapple stuff if I'm on a hook
	if (self->on_hook)
	{
		Service_Grapple ();
		if (self->PR_BUTTON_JUMP && self->velocity[Z] > 10)
			self->velocity[Z] = 10;	//WK Allow more creative physics with hook
	}

	//WK Add Hover Boot Support
	//WK 1/7/7 Allow people to jump when wearing boots
	if (self->tf_items & PR_NIT_HOVER_BOOTS) {
		if (self->PR_BUTTON_JUMP && self->velocity[Z] < 0 && self->hover_time > 0 && self->waterlevel == 0 && !(self->flags & PR_FL_ONGROUND))
		{	//Try to hover
			self->velocity[Z] = 100; //+10 == hover
			self->hover_time = self->hover_time - 0.25; //0.1 == tick time
			if (self->hover_time <= 0) {
				self->hover_time = -5; //3 second penalty for draining it
				sprint(self,PR_PRINT_HIGH,"Your boots are out of fuel, let them recharge\n");
			}
			//Spit out fire from them!
			if (self->search_time < time) {
			sound (self, PR_CHAN_AUTO, "weapons/flmfire2.wav", 1, PR_ATTN_NORM);
			entity flame;
			flame = spawn();
			flame->owner = self;flame->movetype = PR_MOVETYPE_FLYMISSILE;
			flame->solid = PR_SOLID_BBOX;flame->classname = "flamerflame";
			makevectors (self->v_angle);
			flame->velocity = -1 * v_up; flame->velocity = flame->velocity * 600;
			flame->touch = Boot_Flamer_stream_touch;flame->think = s_explode1;
			flame->nextthink = time + 0.1;setmodel(flame, "progs/s_explod.spr");
			setsize (flame, V({0, 0, 0}), V({0, 0, 0}));
			setorigin (flame, self->origin + v_up * -16);
			self->search_time = time + 0.15;
			}
		}
		else { //Recharger <- That's French for "recharge"
			self->hover_time = self->hover_time + 0.01; //0.05 is a half-tick
			if (self->hover_time > PR_MAX_HOVER_FUEL && (!(self->tf_items & PR_NIT_HOVER_BOOTS_UPGRADE)))
				self->hover_time = PR_MAX_HOVER_FUEL;
			if (self->hover_time > (PR_MAX_HOVER_FUEL * 2) && self->tf_items & PR_NIT_HOVER_BOOTS_UPGRADE)
				self->hover_time = (PR_MAX_HOVER_FUEL * 2);
		}
	}
}

/*
================
CheckPowerups

Check for turning off powerups
================
*/
void CheckPowerups()
{
	float lighton;
	entity te;

	if (self->health <= 0)
		return;

	// Invisibility
	//WK Made people invis during building, and during thief's full hide
	//WK And while being inspired by chaplan
	if (self->done_custom & PR_CUSTOM_BUILDING || //We are building a custom class
		  self->playerclass == PR_PC_UNDEFINED || //Or we're in observer mode
		 (self->job & PR_JOB_THIEF && self->job & PR_JOB_FULL_HIDE) || //Or we are fully hiding thieves
	   //(self.tfstate & #TFSTATE_INSPIRED && !(self.job & #JOB_CHAPLAN)) || //ofN comented by
	   self->aura == PR_AURA_INVIS) //- OfN
	{
		self->modelindex = modelindex_null;	// don't use eyes
	}
	else if ((self->is_undercover == 1 && invis_only == PR_TRUE) || (self->job & PR_JOB_THIEF && self->job & PR_JOB_ACTIVE))
	{
		self->frame = 0;
		self->modelindex = modelindex_eyes;
	}
	else if (self->invisible_finished)
	{
		// If this is being given by a goalitem, extend the time
		if (self->tfstate & PR_TFSTATE_INVISIBLE)
		{
			if (self->invisible_finished < time + 10)
				self->invisible_finished = time + 666;
		}

		// sound and screen flash when items starts to run out
		if (self->invisible_sound < time)
		{
			sound (self, PR_CHAN_AUTO, "items/inv3.wav", 0.5, PR_ATTN_IDLE);
			self->invisible_sound = time + ((random() * 3) + 1);
		}


		if (self->invisible_finished < time + 3)
		{
			if (self->invisible_time == 1)
			{
				sprint(self, PR_PRINT_HIGH, "Ring of Shadows magic is fading\n");
				stuffcmd (self, "bf\n");
				sound (self, PR_CHAN_AUTO, "items/inv2.wav", 1, PR_ATTN_NORM);
				self->invisible_time = time + 1;
			}

			if (self->invisible_time < time)
			{
				self->invisible_time = time + 1;
				stuffcmd (self, "bf\n");
			}
		}

		if (self->invisible_finished < time)
		{	// just stopped
			self->items = self->items - PR_IT_INVISIBILITY;
			self->invisible_finished = 0;
			self->invisible_time = 0;
		}

		// use the eyes
		self->frame = 0;
		self->modelindex = modelindex_eyes;
	}
	else
		self->modelindex = modelindex_player;	// don't use eyes

	// invincibility
	if (self->invincible_finished)
	{
		// If this is being given by a goalitem, extend the time
		if (self->tfstate & PR_TFSTATE_INVINCIBLE)
		{
			if (self->invincible_finished < time + 10)
				self->invincible_finished = time + 666;
		}

		// sound and screen flash when items starts to run out
		if (self->invincible_finished < time + 3)
		{
			if (self->invincible_time == 1)
			{
				sprint (self, PR_PRINT_HIGH, "Protection is almost burned out\n");

				stuffcmd (self, "bf\n");
				sound (self, PR_CHAN_AUTO, "items/protect2.wav", 1, PR_ATTN_NORM);
				self->invincible_time = time + 1;
			}

			if (self->invincible_time < time)
			{
				self->invincible_time = time + 1;
				stuffcmd (self, "bf\n");
			}
		}

		if (self->invincible_finished < time)
		{	// just stopped
			self->items = self->items - PR_IT_INVULNERABILITY;
			self->invincible_time = 0;
			self->invincible_finished = 0;
		}
		if (self->invincible_finished > time)
			self->effects = self->effects | PR_EF_DIMLIGHT;
		else
		{
			// Only remove dimlight if it's not being supplied by a GoalItem
			lighton = PR_FALSE;
			te = find (world, "classname", "item_tfgoal");
			while (te != world)
			{
				if (te->owner == self)
				{
					if (te->goal_activation & PR_TFGI_GLOW)
						lighton = PR_TRUE;
				}
				te = find(te, "classname", "item_tfgoal");
			}

			if (!lighton)
				self->effects = self->effects - (self->effects & PR_EF_DIMLIGHT);
		}
	}

	// super damage
	if (self->super_damage_finished)
	{
		// If this is being given by a goalitem, extend the time
		if (self->tfstate & PR_TFSTATE_QUAD)
		{
			if (self->super_damage_finished == time + 10)
				self->super_damage_finished = time + 666;
		}

		// sound and screen flash when items starts to run out
		if (self->super_damage_finished < time + 3 && !(self->tfstate & PR_TFSTATE_INSPIRED))
		{
			if (self->super_time == 1)
			{
				sprint (self, PR_PRINT_HIGH, "Quad Damage is wearing off\n");
				stuffcmd (self, "bf\n");
				sound (self, PR_CHAN_AUTO, "items/damage2.wav", 1, PR_ATTN_NORM);
				self->super_time = time + 1;
			}

			if (self->super_time < time)
			{
				self->super_time = time + 1;
				stuffcmd (self, "bf\n");
			}
		}

		if (self->super_damage_finished < time)
		{	// just stopped
			self->items = self->items - PR_IT_QUAD;
			self->super_damage_finished = 0;
			self->super_time = 0;
			//WK If inspired, remove inspiration
			self->tfstate = self->tfstate - (self->tfstate & PR_TFSTATE_INSPIRED);
		}
		if (self->super_damage_finished > time)
			self->effects = self->effects | PR_EF_DIMLIGHT;
		else
		{
			// Only remove dimlight if it's not being supplied by a GoalItem
			lighton = PR_FALSE;
			te = find (world, "classname", "item_tfgoal");
			while (te != world)
			{
				if (te->owner == self)
				{
					if (te->goal_activation & PR_TFGI_GLOW)
						lighton = PR_TRUE;
				}
				te = find(te, "classname", "item_tfgoal");
			}

			if (!lighton)
				self->effects = self->effects - (self->effects & PR_EF_DIMLIGHT);
		}
	}

	// suit
	if (self->radsuit_finished)
	{
		self->air_finished = time + 12;		// don't drown

		// If this is being given by a goalitem, extend the time
		if (self->tfstate & PR_TFSTATE_RADSUIT || self->tf_items & PR_NIT_SCUBA)
		{
//WK Do we need this?	if (self.radsuit_finished == time + 10)
				self->radsuit_finished = time + 666;
		}

		// sound and screen flash when items starts to run out
		if (self->radsuit_finished < time + 3)
		{
			if (self->rad_time == 1)
			{
				sprint (self, PR_PRINT_HIGH, "Air supply in Biosuit expiring\n");
				stuffcmd (self, "bf\n");
				sound (self, PR_CHAN_AUTO, "items/suit2.wav", 1, PR_ATTN_NORM);
				self->rad_time = time + 1;
			}

			if (self->rad_time < time)
			{
				self->rad_time = time + 1;
				stuffcmd (self, "bf\n");
			}
		}

		if (self->radsuit_finished < time)
		{	// just stopped
			self->items = self->items - PR_IT_SUIT;
			self->rad_time = 0;
			self->radsuit_finished = 0;
		}
	}

	if (self->aura == PR_AURA_INVIS)
		if (self->aura_time < time)
		{
			if (self->invisible_finished < time)
				self->items = self->items - (self->items & PR_IT_INVISIBILITY);
			self->aura = 0;
		}

}

void DeadImpulses();

/*
================


Called every frame after physics are run
================
*/
void PlayerPostThink()
{
	/*local float	mspeed, aspeed;
	local	float	r;*/

	// PZ: This only does stuff for human players, involving the FrikBot.
	if (!frik_botPostThink()) // FrikBot
		return; // don't run this function if self.b_clientno == -1

//bprint(2, "Called PlayerPostThink()\n");

	if (self->flags & PR_FL_FINALIZED)
	{
		if (self->dont_do_triggerwork == 4)
			return;

		if ((self->ex_skill_max + PR_FINALIZED_DELAY_TOACTION) < time)
		{
			FinalizedPerformAction();
			return;
		}

		if (self->ex_skill_min < time)
		{
			if (self->option2 == PR_TRUE)
			{
				if (self->dont_do_triggerwork == 1)
					// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
					//     themselves, to keep from having to kick them when they do.
					//stuffcmd(self,"team kick\n");
					setinfo(self, "team", "kick");  // PZ
				else if (self->dont_do_triggerwork == 2)
					//stuffcmd(self,"team ban\n");
					setinfo(self, "team", "ban");   // PZ
				else
					//stuffcmd(self,"team serm\n");
					setinfo(self, "team", "serm");  // PZ

				self->option2 = PR_FALSE;
			}
			else
			{
				if (self->dont_do_triggerwork == 1)
					//stuffcmd(self,"team \"\bkick\b\"\n");
					setinfo(self, "team", S_("^bkick^b"));  // PZ
				else if (self->dont_do_triggerwork == 2)
					//stuffcmd(self,"team \"\bban\b\"\n");
					setinfo(self, "team", S_("^bban^b"));   // PZ
				else
					//stuffcmd(self,"team \"\bserm\b\"\n");
					setinfo(self, "team", S_("^bserm^b"));  // PZ

				self->option2 = PR_TRUE;
			}

			self->ex_skill_min = time + PR_FINALIZED_RATE_TEAMFLASH;
		}

		return;
	}

	if (self->view_ofs == V({0, 0, 0}))
		return; 	// intermission or finale
#ifdef PR_QUAKE_WORLD // culled by KK. was QUAKE_WORLD
	//if (infokey(world,"ceasefire")=="on") //Cyto
	if (ceasefire)
		if (!GoodCeasefireImpulse(self->impulse)) //See admin.qc
			self->impulse = 0;
#endif
	if (self->deadflag)
	{
		DeadImpulses();  // check for dead-only commands
		self->impulse = 0;
		return;
	}

	// PZ: I had broken 'fall landing' detection by changing this. So, I put things back the way they were, but renamed 'jump_flag' to 'prev_velocity_z'. (8-15-15)
	// check to see if player landed and play landing sound  // (Old comment. ->) PZ: repurposed .jump_flag (all it was, was .velocity_z, before; so just use .velocity_z)
	if (self->takedamage && (self->prev_velocity_z < -300) && (self->flags & PR_FL_ONGROUND) && (self->health > 0))
	{
		if (self->watertype == PR_CONTENT_WATER)
		{
			if (!(self->cutf_items & PR_CUTF_STEALTH))
				sound (self, PR_CHAN_BODY, "player/h2ojump.wav", 1, PR_ATTN_NORM);
		}
		else if (self->prev_velocity_z < -650)
		{
			if (!(self->cutf_items & PR_CUTF_STEALTH)) { //WK Judo teaches falling... SB ceaf judo
				self->deathtype = "falling";
				T_Damage (self, world, world, 5);
				sound (self, PR_CHAN_VOICE, "player/land2.wav", 1, PR_ATTN_NORM);
			}
		}
		else
			if (!(self->cutf_items & PR_CUTF_STEALTH)) //WK Judo teaches falling... SB ceaf judo
				sound (self, PR_CHAN_VOICE, "player/land.wav", 1, PR_ATTN_NORM);
	}

#ifndef PR_NO_ZEROGRAVGUN
	// Gizmo - check if we were going to be teamkilled by a friendly engy and we made it out of the lava
	if ( self->PR_teleporter_teamkiller )
	if ( self->watertype != PR_CONTENT_LAVA )
	if ( pointcontents( self->origin ) != PR_CONTENT_LAVA )
	if ( self->flags & PR_FL_ONGROUND || self->flags & PR_FL_PARTIALGROUND )	// we're out of lava and we're on solid ground so we're safe
		self->PR_teleporter_teamkiller = world;
#endif

	// PZ: repurposed .jump_flag (all it was was .velocity_z, before; so just use .velocity_z)  (<- Old comment.)
	// PZ: I brought back 'jump_flag' (see comment above), but renamed it to 'prev_velocity_z'). (8-15-15)
	self->prev_velocity_z = self->velocity[Z];

	CheckPowerups ();
	W_WeaponFrame ();

	//	Display MOTD
	// Sync this with tforthlp.qc and menu.qc
	if (self->motd < PR_MOTD_FINISHED)
		TeamFortress_MOTD();
	else if (self->cheat_check == 0)
		self->cheat_check = time + 10;
#ifdef PR_STATUSBAR
	else if (self->current_menu == 0 && time > self->StatusRefreshTime && self->StatusBarSize != 0)
	{
		if (!ceasefire)
		{
			if (self->StatusBarScreen == 1)
				RefreshStatusBar1(self); //Sentry screen
			else if (self->StatusBarScreen == 2)
				RefreshStatusBar2(self); //Spy screen
			else if (self->StatusBarScreen == 3)
				RefreshStatusBar3(self); //Misc screen
			else if (self->StatusBarScreen == 4)
				RefreshStatusBar4(self); //Tesla screen
			else if (self->StatusBarScreen == 5)
				RefreshStatusBar5(self); //Scanner screen
			else
				RefreshStatusBar(self); //Normal scores and clip
		}
	}
#endif

	// Check for Team Cheats
	if (self->cheat_check <= time)
	{
		TeamFortress_CheckTeamCheats();

	#ifdef PR_QUAKE_WORLD
		self->cheat_check = time + 5;
	#else
		self->cheat_check = time + 1;
	#endif
	}
}


/*
===========
ClientConnect

called when a player connects to a server
============
*/

void ClientConnect()
{
	string st;

	frik_setupNewClient(); // FrikBot

	st = GetBracedClDesc(self);
	// PZ: print the name by itself, so that the server can color the name (etc.) on its console
	bprint(PR_PRINT_HIGH, self->netname);
	bprint(PR_PRINT_HIGH, " ", st, " has joined the server\n");

	// PZ: make their team name and color blank when they join the server
	if (teamplay) // PZ: only if we have teams
	{
		// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
		//     themselves, to keep from having to kick them when they do.
		//stuffcmd(self, "team \"\"\n");
		setinfo(self, "team", "");
		//stuffcmd(self, "color \"0 0\"\n");
		setinfo(self, "topcolor", "0");
		setinfo(self, "bottomcolor", "0");
	}

	//- OfN
	self->admin_kick = world;

	self->ex_skill_min = 0; // OfN - Used as password attempt count
	self->ex_skill_max = time; // OfN - Used on cmnd.qc
	self->option2 = time; // OfN - Used on cmnd.qc

	self->goal_activation = PR_FALSE; // Job initialized flag
	self->PR_runes = 0; // Runes, 0 = none

	self->PR_cprint_fx = 0; // Centerprint effect counter

	// Set Default autozoom
	if (PR_DEFAULT_AUTOZOOM == PR_OFF)
		self->tfstate = self->tfstate | PR_TFSTATE_ZOOMOFF;

	// Set the MOTD on
	self->motd = 0;
	self->worldtype = 1; // PZ: this sets their intro/MOTD to page 1
	// Clear the Alias Flag
	self->got_aliases = 0;
	self->ff_count = 0; //WK Clear the friendly-fire counter


#ifdef PR_QUAKE_WORLD
//RJM
	st = infokey(self, "sbr");
	if (st == string_null)
//RJM
		st = infokey(self, "sbar_res");
	if (st == "768")
		self->StatusBarRes = 8;
	else if (st == "600")
		self->StatusBarRes = 7;
	else if (st == "480")
		self->StatusBarRes = 6;
	else if (st == "400")
		self->StatusBarRes = 5;
	else if (st == "384")
		self->StatusBarRes = 4;
	else if (st == "350")
		self->StatusBarRes = 3;
	else if (st == "300")
		self->StatusBarRes = 2;
	else if (st == "240")
		self->StatusBarRes = 1;
	else
		self->StatusBarRes = 0;
//RJM
	st = infokey(self, "sbs");
	if (st == string_null)
//RJM
		st = infokey(self, "sbar_size");
	self->StatusBarSize = stof(st);
	if (self->StatusBarSize > 2 || self->StatusBarSize < 0)
		self->StatusBarSize = 0;
#endif

	//self.has_disconnected = #FALSE;
	// PZ NOTE: Technically, the server considers the client connected before this point -- while checking files, loading, downloading, etc.
	//          At this point, the player is joining the game. So this really means "are they in the game?", not "are they connected?".
	self->is_connected = PR_TRUE;

	//PlayerObserverMode(); //ofn already commented out

	self->gravity = 0;
	self->movetype = PR_MOVETYPE_FLY;

	// a client connecting during an intermission can cause problems
	if (intermission_running)
		GotoNextMap();

	//- OfN Player joined event
	PlayerJoined();
}

/*
===========
ClientDisconnect

called when a player disconnects from a server
============
*/

void GibPlayerOut();
void set_suicide_frame_disc();

void ClientDisconnect()
{
	//local float teamsup;
	entity te;
	string st;

#ifndef PR_QUAKE_WORLD
	if (gameover)
		return;
#endif

	// for Neo mode
	if (neo.isModeActive() && self == neo.getNeo())
	{
		neo.removeNeo(self);
		bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^-^-^-^-^-^-^> ^bNeo Mode^b ^<^-^-^-^-^-^-^-^-^-^>\n"));
		bprint(PR_PRINT_HIGH, "Neo ("); bprint(PR_PRINT_HIGH, self->netname); 
		bprint(PR_PRINT_HIGH, ") unplugged.\n");
		bprint(PR_PRINT_HIGH, "Kill someone to become Neo!\n");
		bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
		BroadcastSound("boss1/sight1.wav");
		neo.centerPrint();
	}
	// end Neo

	st = ftos (floor(self->real_frags));
	bprint (PR_PRINT_HIGH, self->netname);
	bprint (PR_PRINT_HIGH, " has left the game with ");
	bprint (PR_PRINT_HIGH, st);
	bprint (PR_PRINT_HIGH, " frags\n");// and ");

	/*st = ftos (floor(self.ff_count));
	bprint (#PRINT_HIGH, st);
	bprint (#PRINT_HIGH, " teamkills\n");*/

	if (self->admin_flag)
	{
		AdminLoggedOut(self);
		self->admin_flag = 0;
	}

	self->message = "";

	// Throw gib/head stuff if appropiate only
	if (!(self->done_custom & PR_CUSTOM_BUILDING)) // customizing, so nope
	if (!(self->playerclass == PR_PC_UNDEFINED))	// observing, so nope
	if (self->modelindex == modelindex_player)	// are we visible and on a player model? (not a head)
		GibPlayerOut(); // ok, gib him

	// PZ: MOVED THIS STUFF TO END OF THIS FUNCTION  1-9-15
	// PZ: moved down here so that bots gib when they are removed
	/*if (!self.ishuman)
	{
		frik_handleBotDisconnection(self);  // FrikBot // PZ: put this here instead
	}
	frik_handleClientDisconnection(); // FrikBot     // PZ: took this out of above condition (8-3-14)*/

	// OfN - Adjust army rating of our teammates as needed
	ArmyRatingLeave(self);

	//- he got significant score?
	/*local float temp_scr;
	local float avr_team_scr;
	avr_team_scr = (team1score + team2score + team3score + team4score) / number_of_teams;
	temp_scr = fabs (self.real_frags) + self.ff_count;

	if ( (temp_scr > time || time > 20) && time < 60*60 && deathmatch == 3)
	{
		local float final_score;
		final_score = self.real_frags - self.ff_count*3 - avr_team_scr/2 ;

		if ( final_score < -50 )
			bprint (#PRINT_HIGH, "bad enough to ban him! hehe\n");
		else if (final_score < 0)
			bprint (#PRINT_HIGH, "damn newbie!\n");
		else if (final_score < 10)
			bprint (#PRINT_HIGH, "he was not doing much\n");
		else if (final_score < 30)
			bprint (#PRINT_HIGH, "a regular player\n");
		else if (final_score < 50)
			bprint (#PRINT_HIGH, "not bad!\n");
		else if (final_score < 75)
			bprint (#PRINT_HIGH, "good score\n");
		else if (final_score < 100)
			bprint (#PRINT_HIGH, "cool score!\n");
		else if (final_score < 125)
			bprint (#PRINT_HIGH, "he is a real master of customTF!\n");
		else bprint (#PRINT_HIGH, "omg! did he cheat? awesome score!\n");
		Fortunatelly for his team
	}*/

	//- OfN


	sound (self, PR_CHAN_BODY, "player/tornoff2.wav", 1, PR_ATTN_NONE);
	//self.has_disconnected = #TRUE;
	self->is_connected = PR_FALSE;

	if (debug_target == self)
		debug_target = world;

	// Remove Timers
	//TeamFortress_RemoveTimers(); Moved inside CleanUpEverything

	CleanUpEverything();

	// Drop any runes he would carry
	PlayerDropRunes(self);

	// Remove Buildings
	////DetonateAllGunsForced();
	/*Find_And_Dmg("building_dispenser", self, 1);
	Find_And_Dmg("building_sentrygun", self, 1);
	Find_And_Dmg("building_tesla", self, 1);
	Find_And_Dmg("building_camera", self, 1);
	Find_And_Dmg("building_teleporter", self, 1);
	Find_And_Dmg("building_sensor", self, 1); //sb*/

	//WK Added demon cleanup
	////kill_my_demons();
	//WK Added ammobox/pipebomb fix
	te = find(world, "classname", "ammobox");
	while (te != world)
	{
		if (te->enemy == self) {
			num_world_ammoboxes = num_world_ammoboxes - 1;
			if (te->enemy->team_no != 0)
				decrement_team_ammoboxes(self->team_no);

		}
		te->think = SUB_Remove;
		te->nextthink = time + 0.1;

		te = find(te, "classname", "ammobox");
	}
	te = find(world, "classname", "pipebomb");
	while (te != world)
	{
		if (te->owner == self) {
			num_world_pipebombs = num_world_pipebombs - 1;
			decrement_team_pipebombs(self->team_no);
			te->think = SUB_Remove;
			te->nextthink = time + 0.1;
		}

		te = find(te, "classname", "pipebomb");
	}

	// OfN - Remove holograph if player disconnects!
	////if (self.has_holo > 0 ) RemoveHolo(self);
	////DetonateMines(self);
	////RemoveArmyTimer();

	// Remove Detpacks
	te = find(world, "classname", "detpack");
	while (te != world)
	{
		if (te->owner == self)
		{
			if (te->weaponmode == 1) 		  // Detpack was being disarmed
			{
				te->enemy->tfstate = te->enemy->tfstate - (te->enemy->tfstate & PR_TFSTATE_CANT_MOVE);
		#ifdef PR_QUAKE_WORLD
				TeamFortress_SetSpeed(te->enemy);
		#else
				te->enemy->pausetime = time;
		#endif

				dremove(te->oldenemy);	 // CountDown
				dremove(te->observer_list); //	Disarm timer
			}

			dremove(te);
			te = world;
		}

		te = find(te, "classname", "detpack");
	}

	//- OfN - Player quit event
	PlayerQuit();

	set_suicide_frame_disc ();
	self->netname = string_null;
	self->team_no = 0;
	self->solid = PR_SOLID_NOT;
	self->movetype = PR_MOVETYPE_NONE; //WK Stop crashing MOVETYPE_WALK bug?
	setsize(self, V({0, 0, 0}), V({0, 0, 0}));

	self->classname = "emptyclient";

	// Gizmo - this used to be above ArmyRatingLeave() and under GibPlayerOut(), for whoever put it up there
	// Remove our permanent strings (if any in use by us)
	if (self->PR_sname1 != "")
		delstr(self->PR_sname1);
	if (self->PR_sname2 != "")
		delstr(self->PR_sname2);
	if (self->PR_sname3 != "")
		delstr(self->PR_sname3);

	// PZ: For bots. This stuff completely clears the bot's entity. So, it needs to be done last.
	if (!self->ishuman)
	{
		frik_handleBotDisconnection(self);  // FrikBot // PZ: put this here instead
	}
	// PZ NOTE: also done for humans
	frik_handleClientDisconnection(); // FrikBot     // PZ: took this out of above condition (8-3-14)
}

//==============================================================
// "Event" functions when a player joins or leaves

void UpdateMapSettings();
void ResetStripedAndGiven(entity player);

void PlayerJoined()
{
	num_players = num_players + 1;

	UpdateMapSettings();

	if (num_players == 1)
		World_FadeIn(5.2,0,2); // 4.5, 2

	ResetStripedAndGiven(self);

	self->owned_by = 0; // VOTED_JUSTJOINED

	// Check if this player is hiding from a mute voting
	if (votesleeping)
	{
		entity sleepvote;

		sleepvote = GetSleepVote();

		if (sleepvote != world)
		{
			string foo;
			foo = infokey(self,"ip");

			if (foo == sleepvote->message)
			{
				if (ValidVoteEnt()) // But.. shit we have another vote running!
					sleepvote->admin_kick = self; // Will check in its thinks
				else
					SleepVoteResume(sleepvote,self); // Resume it!
			}
		}
		else
		{
			RPrint("BUG: Unable check for resume of vote, GetSleepVote() failed!\n");
		}
	}
	// Bastard Memory
	// Is this player still serving curse time? -Pulseczar
	entity te;
	string IP;
	float  brk; // PZ: added so that I can remove 'break;'
	IP = infokey(self, "ip");
	te = find(world, "netname", "bastardtimer");
	brk = 0;
	while (te != world && !brk)
	{
		if (IP == te->t_s_h)
		{
			self->penance_time = te->penance_time;
			te->owner = self;
			makeImmune(self, time + 65);
			bprint(PR_PRINT_HIGH, self->netname);
			bprint(PR_PRINT_HIGH, " discovered that he/she can no longer escape Curse by reconnecting\n");
			brk = 1;   //break;
		}
		te = find(te, "netname", "bastardtimer");
	}
}

void PlayerQuit()
{
	string foo;
	entity te;

	num_players = num_players - 1;

	// PZ - created playersOnTeam# for AGR mode
	if		(self->team_no == 1) playersOnTeam1 = playersOnTeam1 - 1;
	else if (self->team_no == 2) playersOnTeam2 = playersOnTeam2 - 1;
	else if (self->team_no == 3) playersOnTeam3 = playersOnTeam3 - 1;
	else if (self->team_no == 4) playersOnTeam4 = playersOnTeam4 - 1;

	UpdateMapSettings();

	// Trying to escape from banning? not today..
	if (self->flags & PR_FL_FINALIZED)
	if (self->dont_do_triggerwork == 2)
	{
		bprint(PR_PRINT_HIGH, self->netname);
		bprint(PR_PRINT_HIGH, " escapes the sermon but not his banning\n");

		sprint(self,PR_PRINT_HIGH,S_("\nYou have been ^bBANNED^b from the server!\n"));

		foo = infokey(self,"ip");
		localcmd("addip ");
		localcmd(foo);
		localcmd("\n");

		self->dont_do_triggerwork = 4;
	}

	if (votesleeping)
	{
		te = GetSleepVote();

		if (te != world)
		{
			if (te->admin_kick == self)
				te->admin_kick = world;
		}
	}

	if (ValidVoteEnt())
	{
		if (current_voteent->has_sentry == 0) //PUNISH
		if (current_voteent->admin_kick == self)
		{
			if (current_voteent->g_a == 2) // PUNISH_BAN
			{
				bprint(PR_PRINT_HIGH,"Voting\x8D ");
				bprint(PR_PRINT_HIGH,self->netname);
				bprint(PR_PRINT_HIGH," leaves the server but votes to ban him may continue..\n"); // only IP in fact, but better to not inform every1 about it, dont ya think?

				// You will be remembered here, pal
				current_voteent->dont_do_triggerwork = PR_TRUE;

				current_voteent->t_s_h = infokey(self,"ip");
				current_voteent->t_s_h = makestr(current_voteent->t_s_h);
				current_voteent->t_s_m = makestr(self->netname);

				BroadcastSound("zombie/z_hit");
			}
			else if (current_voteent->g_a == 3 || current_voteent->g_a == 4) // PUNISH_MUTE or PUNISH_CUFF
			{
				bprint(PR_PRINT_HIGH,"Voting\x8D ");
				bprint(PR_PRINT_HIGH,self->netname);
				bprint(PR_PRINT_HIGH," leaves the server, but keeping client data\n"); // only IP in fact, but better to not inform every1 about it, dont ya think?

				// You will be remembered here, pal
				VoteToSleep();

				BroadcastSound("zombie/z_hit");
			}
			else // anything else should fail (kick and curse)
			{
				bprint(PR_PRINT_HIGH,"Voting\x8D Punishment failed. User ");
				bprint(PR_PRINT_HIGH,self->netname);
				bprint(PR_PRINT_HIGH," has left the server\n");

				BroadcastSound("zombie/z_hit");
				ResetVoteEnt();
			}
		}
	}

	// Reset version and clienttype
	self->cltype = 0;
	self->clversion = 0;

	if (num_players < 0)
		num_players = 0;

	if (!num_players)
	{
		if (!num_specs)
		{
			if (ceasefire)
			{
				ceasefire = PR_FALSE;
				bprint(PR_PRINT_HIGH,"No clients on server, ceasefire ends automatically..\n");
			}

			World_FadeOut(0,0,0);	  // TOCHECK: If this is 2, and u reconnect weird fade occurs
		}
		else
			World_FadeMid(2,0,2);
	}
}

void UpdateMapSettings()
{
	float result;

	if (mapname == "huntedr")
	{
		result = floor(TeamFortress_TeamGetNoPlayers(2) * PR_HUNTED_YELLOWTEAM_FACTOR);
		team3maxplayers = result;
		if (team3maxplayers < 1) team3maxplayers = 1;
	}
	else if (mapname == "border1" || mapname == "border1r")
	{
		result = floor(TeamFortress_TeamGetNoPlayers(2) * PR_BORDER_FACTOR);
		team3maxplayers = result;
		team1maxplayers = result;
		if (team3maxplayers < 1) team3maxplayers = 1;
		if (team1maxplayers < 1) team1maxplayers = 1;
	}
}

} // END namespace Progs
