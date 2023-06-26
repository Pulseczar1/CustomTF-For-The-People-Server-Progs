// invade.qc                                                                 ver. April 29, 2020
// Author: Pulseczar
//
// - Invade Mode -
// This mode is similar to AGR, but teams switch roles just like on invade4a.bsp. When the game
// starts, initial roles are randomly chosen. When a team caps the flag, the roles switch. Every
// time the roles switch, game enters a short prematch to give defenders time to prepare. The
// offensive team's flag is disabled. The main major benefit of alternating roles (unlike AGR)
// is that teams are in perfect balance on every map. Both teams have the exact same opportunity
// to score.
//
// This mode should work on virtually all standard TF maps, containing 2 teams, with 1 flag each.
//
// Sounds for AGR / Invade modes came from Unreal Tournament 2004. You can open UAX sound pack
// files with Dragon UnPACKer.

#include "progs.h"
#include "invade.h"
#include "spectate.h"
#include "cpstuff.h"
#include "tforttm.h"
#include "debug.h"

namespace Progs {

//=================================================================
// Globals
//=================================================================

// Globals for 'localinfo' variables (see below).
// NOTE: NEED TO MAKE SURE THESE VARIABLES ARE UPDATED IN progdefs.h AND progdefs.cpp, SO THAT THE ENGINE CAN SEE THEM.
float invade;                      // just on or off (requires 'localinfo invade 1' to be on)
float invade_frags;                // frags defense gets for successfully defending for `invade_minutes` minutes
float invade_minutes;              // how long offensive team has to try to cap the flag; 0 means there is no flag capture time limit
float invade_teamfrags;            // This overrides the serverinfo 'teamfrags' (or 't') var when Invade is active, which
                                   // controls whether or not each players' frags are equal to their team's score.
float invade_prematchTime;         // how long Invade prematches will last (in seconds)

// non-cvar globals
//enum class InvadeGameState { STOPPED, IN_PREMATCH, GAME_RUNNING };  // MOVED TO progdefs.h. Should probably be in invade.h.
InvadeGameState invade_gameState; // NOTE: When game is running, if we are using a capture timer, it is running.
float invade_teamOnDef;
float invade_swapRoles;             // flag is set when it's time to swap team roles
float invade_timePrematchEnds;      // when `time` reaches this time, Invade prematch ends
float invade_timeOfLastSound;       // last time we played a timer sound
float invade_timeToRewardDef;       // don't try to set it (it's set by functions); only used when `invade_minutes` > 0

// Globals not currently used outside this file.
string offenseTeamName, defenseTeamName;

//=================================================================
// Called by the localinfo key initializing function.
//=================================================================
void Invade_initialize()
{
	string st;
	float force_invade;

	st = infokey(world, "invade");
	if (st == string_null) st = "0"; // sets default
	invade = stof(st);
	if (invade != 1) invade = 0;     // has to be set to 1 to engage Invade Mode

	st = infokey(world, "force_invade");
	if (st == string_null) st = "0";
	force_invade = stof(st);
	if (force_invade == 1) invade = 1; // override Invade setting if 1

	if (invade)
	{
		prematch = PR_FALSE; // don't use regular prematch stuff during Invade Mode (only use Invade Prematch)

		st = infokey(world, "invade_frags");
		if (st == string_null) st = "10";       // sets default
		invade_frags = stof(st);
		if (invade_frags < 0) invade_frags = 0; // the allowed minimum

		st = infokey(world, "invade_minutes");
		if (st == string_null) st = "5";            // sets default
		invade_minutes = stof(st);
		if (invade_minutes < 0) invade_minutes = 0; // the allowed minimum

		st = infokey(world, "invade_prematch_time");
		if (st == string_null) st = "90";                     // sets default (in seconds)
		invade_prematchTime = stof(st);
		if (invade_prematchTime < 0) invade_prematchTime = 0; // the allowed minimum

		st = infokey(world, "invade_teamfrags");
		if (st == string_null) st = "1";
		invade_teamfrags = stof(st);
		if (invade_teamfrags != 1) invade_teamfrags = 0;
	}
}
//=================================================================
// Called every frame in StartFrame(). Manages Invade Mode.
//=================================================================
void Invade_think()
{
	string temp;
	entity player;

	// Update the team names.
	if (invade_teamOnDef == 1) { defenseTeamName = "Blue"; offenseTeamName = "Red"; }
	else                       { defenseTeamName = "Red";  offenseTeamName = "Blue"; }

	// Check to see whether we need to perform a team role swap.
	if (invade_swapRoles)
	{
		invade_swapRoles = PR_FALSE;

		if (invade_teamOnDef == 1)
			invade_teamOnDef = 2;
		else
			invade_teamOnDef = 1;

		// Update the team names.
		if (invade_teamOnDef == 1) { defenseTeamName = "Blue"; offenseTeamName = "Red"; }
		else                       { defenseTeamName = "Red";  offenseTeamName = "Blue"; }

		bprint(PR_PRINT_HIGH, S_("\n^<^-^> ^bInvade^b Mode \x8D Swapping Roles ^<^-^>\n"));
		if (invade_teamOnDef == 1)
		{
			bprint(PR_PRINT_HIGH, S_("^bRED^b is Offense ^b/^b BLUE is Defense\n"));
			PlayDelayedSound(world, "agr/redonoffense.wav", 4);
		}
		else
		{
			bprint(PR_PRINT_HIGH, S_("^bBLUE^b is Offense ^b/^b RED is Defense\n"));
			PlayDelayedSound(world, "agr/blueonoffense.wav", 4);
		}
		bprint(PR_PRINT_HIGH, "When attacking team captures the flag, or capture timer runs out, roles will switch.\n");
		bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));

		// [Trigger an Invade 'prematch'.]

		/*
		When roles are swapped, all players return to their base and the game enters a short prematch to give the defenders time to build builds.
		During this time, builds can be immediately built with no cells cost. Hacks also are immediate. Builds also take a 10th of the damage they usually take.
		Players on D take a third of damage and can't be knocked. And of course, neither flag can be taken during this time.
		TODO?: Should you get frags for kills during prematch?
		*/

		// Create timer entity to start the prematch.
		entity te;
		te = spawnServerSide();
		te->netname = "Invade Prematch Timer";
		te->nextthink = time + 7;
		te->think = Invade_startPrematch;
	}
	// When we have players on both teams, start the game.
	if (playersOnTeam1 && playersOnTeam2)
	{
		if (invade_gameState != InvadeGameState::STOPPED)
		{
			// If we're currently in Invade prematch, check whether it's time to end the prematch.
			if (invade_gameState == InvadeGameState::IN_PREMATCH)
			{
				// End prematch once prematch time is over.
				if (time >= invade_timePrematchEnds)
				{
					invade_gameState = InvadeGameState::GAME_RUNNING;
					player = find(world, "classname", "player");
					while (player != world)
					{
						CenterPrint(player, S_("^] Invade Prematch is over ^[\n^bFight^b!\n"));
						PlayClientSound(player, "agr/fight.wav");
						player = find(player, "classname", "player");
					}
					Invade_startTimer();
				}
			}
			// Else if the game is running, manage things for a running game.
			else if (invade_gameState == InvadeGameState::GAME_RUNNING)
			{
				// If the capture timer is running (`invade_minutes` > 0), has the defense successfully defended for
				// `invade_minutes` minutes?
				if (invade_minutes > 0 && invade_timeToRewardDef <= time)
				{
					// Give frags to defensive players.
					TeamFortress_TeamIncreaseScore(invade_teamOnDef, invade_frags, world);
					player = find(world, "classname", "player");
					while (player != world)
					{
						if (player->team_no == invade_teamOnDef)
						{
							player->real_frags = player->real_frags + invade_frags;
							if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
								player->frags = player->real_frags;
						}
						CenterPrint(player, S_(string("^] " + defenseTeamName + " Successfully Defends! ^[\n")));
						player = find(player, "classname", "player");
					}
					if (invade_teamOnDef == 1)
						bprint(PR_PRINT_HIGH, S_(string("\n^<^-^-^> " + defenseTeamName + " Successfully Defends ^<^-^-^-^>\n")));
					else // Red requires another character to get everything to line up nicely.
						bprint(PR_PRINT_HIGH, S_(string("\n^<^-^-^-^> " + defenseTeamName + " Successfully Defends ^<^-^-^-^>\n")));
					bprint(PR_PRINT_HIGH, string(defenseTeamName + " is awarded "));
					temp = ftos(invade_frags); temp = colstr(temp, PR_COLSTR_NUMBER);
					bprint(PR_PRINT_HIGH, temp);
					bprint(PR_PRINT_HIGH, " frags for\nsuccessfully defending for ");
					temp = ftos(invade_minutes); temp = colstr(temp, PR_COLSTR_NUMBER);
					bprint(PR_PRINT_HIGH, temp); bprint(PR_PRINT_HIGH, " minutes!\n");
					bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
					TeamFortress_TeamShowScores(2, invade_teamOnDef, invade_frags);
					BroadcastSound("boss2/pop2.wav");
					// Reset capture timer and swap roles.
					Invade_resetTimer();
					invade_swapRoles = PR_TRUE;
				}

				// TODO? This doesn't seem to really apply to Invade Mode.
				// Check to see if the game should end because blue/defense
				// doesn't have enough time to catch up to red/offense in score.
				/*if (timelimit != 0)
				{
					timeLeft = (timelimit - time) / 60;
					timeAccumulated = (agr_minutes*60 - (agrTimeToRewardBlue - time)) / 60;
					timesBlueCanScore = (timeLeft + timeAccumulated) / agr_minutes;
					timesBlueCanScore = floor(timesBlueCanScore);
					amtBlueCanScore = timesBlueCanScore * agr_frags;

					if (team1score + amtBlueCanScore < team2score)
					{
						float integer, fraction;
						bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^-^-^-^> Red Wins The Game! ^<^-^-^-^-^-^-^>\n"));
						bprint(PR_PRINT_HIGH, "Blue can't catch up to Red's score in the ");
						integer = floor(timeLeft); fraction = timeLeft - integer;
						fraction = fraction * 10; fraction = floor(fraction);
						temp = ftos(integer); temp = colstr(temp, PR_COLSTR_NUMBER);
						bprint(PR_PRINT_HIGH, temp); bprint(PR_PRINT_HIGH, ".");
						temp = ftos(fraction); temp = colstr(temp, PR_COLSTR_NUMBER);
						bprint(PR_PRINT_HIGH, temp);
						bprint(PR_PRINT_HIGH, "\nminutes remaining. Blue can only score ");
						temp = ftos(amtBlueCanScore); temp = colstr(temp, PR_COLSTR_NUMBER);
						bprint(PR_PRINT_HIGH, temp); bprint(PR_PRINT_HIGH, " points.\n");
						bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
						BroadcastSound("boss2/pop2.wav");
						NextLevel(); // end the map
					}
				}*/
			}
			Invade_playTimerSounds();
		}
		else // invade_gameState == InvadeGameState::STOPPED
		{
			invade_gameState = InvadeGameState::STARTING_PREMATCH;
			string str;
			str = "Both teams have players.\nStarting the game...\n";
			ExtendedCPrint(world, str, 4, 1.5, PR_TRUE);
			// Create timer entity to start the prematch.
			entity te;
			te = spawnServerSide(); // PZ: Make it a server-side only entity.
			te->netname = "Invade Prematch Timer";
			te->nextthink = time + 7;
			te->think = Invade_startPrematch;
		}
	}
	// If we don't have at least one player on each team, stop running the game.
	else if (invade_gameState != InvadeGameState::STOPPED)
	{
		invade_gameState = InvadeGameState::STOPPED;
		Invade_stopTimer();
	}
}
//=================================================================
// Starts an Invade Mode Prematch.
//=================================================================
void Invade_startPrematch()
{
	if (self->netname == "Invade Prematch Timer")
		dremove(self); // Remove the timer ent that called this function, if called by timer.

	invade_timePrematchEnds = time + invade_prematchTime;
	invade_gameState = InvadeGameState::IN_PREMATCH;

	// Return players to their spawns.
	entity player;
	string str;
	str = ftos(invade_prematchTime);
	str = colstr(str, PR_COLSTR_NUMBER);
	player = find(world, "classname", "player"); // TODO: specs too for text and sound?
	while (player != world)
	{
		self = player; // for calling PutClientInServer()
		string str2;
		str2 = S_("^] Starting Prematch for ");
		str2 = strcat(str2, str);
		str2 = strcat(str2, S_(" seconds ^[\n"));
		ExtendedCPrint(self, str2, 4, 1.5, PR_FALSE);
		self->velocity = V({0, 0, 0});
		// Return only those players that are active in the game (on a team, not in menu, etc.).
		if (self->modelindex != modelindex_null || ((self->job & PR_JOB_THIEF) && (self->job & PR_JOB_FULL_HIDE)))
			PutClientInServer();
		stuffcmd(self, "play agr/prepare.wav\n"); // "Prepare for battle!"
		/*if      (invade_teamOnDef == 1)
			stuffcmd(self, "play agr/redonoffense.wav\n");
		else if (invade_teamOnDef == 2)
			stuffcmd(self, "play agr/blueonoffense.wav\n");*/
		player = find(player, "classname", "player");
	}
}
//=================================================================
// Used to determine whether player during Invade Mode prematch gets
// buffs (used to tidy code a bit).
//=================================================================
float Invade_buffPlayer(entity player)
{
	if (invade && invade_gameState == InvadeGameState::IN_PREMATCH && player->team_no == invade_teamOnDef)
		return PR_TRUE;
	return PR_FALSE;
}
//=================================================================
// Alerts the joining player of Invade Mode being active.
//=================================================================
void Invade_introducePlayer()
{
	sprint(self, PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^> ^bInvade Mode^b enabled ^<^-^-^-^-^-^-^>\n"));
	if (invade_teamOnDef == 1)
	{
		sprint(self, PR_PRINT_HIGH, S_("^bRED^b is Offense ^b/^b BLUE is Defense\n"));
		stuffcmd(self, "play agr/redonoffense.wav\n");
	}
	else
	{
		sprint(self, PR_PRINT_HIGH, S_("^bBLUE^b is Offense ^b/^b RED is Defense\n"));
		stuffcmd(self, "play agr/blueonoffense.wav\n");
	}
	sprint(self, PR_PRINT_HIGH, "When attacking team captures the flag, or capture timer runs out, roles will switch.\n");
	if (invade_gameState == InvadeGameState::IN_PREMATCH)
		sprint(self, PR_PRINT_HIGH, S_("^bInvade Mode Prematch is currently active.^b\n"));
	sprint(self, PR_PRINT_HIGH, S_("Use the '^binv^b' command to view Invade Mode game state information.\n"));
	sprint(self, PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
}
//=================================================================
// Invade Mode status information, for 'inv' command.
//=================================================================
void Invade_invPrint()
{
	//Invade_startPrematch(); // for testing, triggered by 'inv' command
	string temp;
	float x, y;

	sprint(self, PR_PRINT_HIGH, S_("\n^bInvade^b Mode: "));
	if (invade_teamOnDef == 1)
		sprint(self, PR_PRINT_HIGH, S_("^bRED^b is Offense. "));
	else
		sprint(self, PR_PRINT_HIGH, S_("^bBLUE^b is Offense. "));
	if (invade_gameState == InvadeGameState::IN_PREMATCH)
	{
		string temp;
		float timeLeft;
		timeLeft = invade_timePrematchEnds - time;
		timeLeft = floor(timeLeft);
		temp = ftos(timeLeft);
		temp = colstr(temp, PR_COLSTR_NUMBER);
		sprint(self, PR_PRINT_HIGH, S_("Invade Mode ^bPrematch^b ends in "), temp);
		if (timeLeft == 1)
			sprint(self, PR_PRINT_HIGH, " second.");
		else
			sprint(self, PR_PRINT_HIGH, " seconds.");
	}
	else if (invade_gameState == InvadeGameState::GAME_RUNNING && invade_minutes > 0)
	{
		sprint(self, PR_PRINT_HIGH, S_("Capture Timer: "));
		x = invade_timeToRewardDef - time; // in seconds
		// Convert to min:sec.
		y = floor(x / 60);
		temp = ftos(y); temp = colstr(temp, PR_COLSTR_NUMBER);
		sprint(self, PR_PRINT_HIGH, temp, ":"); // minutes
		y = x - (60 * floor(x / 60)); // y = x mod 60
		y = floor(y);
		if (y < 10)
			sprint(self, PR_PRINT_HIGH, S_("^0")); // padding zero
		temp = ftos(y); temp = colstr(temp, PR_COLSTR_NUMBER);
		sprint(self, PR_PRINT_HIGH, temp); // seconds
		sprint(self, PR_PRINT_HIGH, " remaining");
	}
	else
		sprint(self, PR_PRINT_HIGH, "No timer is running.");
}
//=================================================================
// Starts the flag capture timer.
//=================================================================
void Invade_startTimer()
{
	if (time < invade_timePrematchEnds) return;

	string temp;
	entity player;

	invade_timeToRewardDef = time + invade_minutes*60; // Set the defender's reward timer.
	invade_gameState = InvadeGameState::GAME_RUNNING;
	temp = ftos(invade_minutes);
	temp = colstr(temp, PR_COLSTR_NUMBER);
	bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^> ^bInvade Mode^b Timer Started ^<^-^-^-^>\n"));
	bprint(PR_PRINT_HIGH, string("Defenders (" + defenseTeamName + ") must successfully defend their\nflag for "));
	bprint(PR_PRINT_HIGH, temp); bprint(PR_PRINT_HIGH, " minutes, in order to score.\n");
	bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
	BroadcastSound("misc/runekey.wav");
	player = find(world, "classname", "player");
	while (player != world)
	{
		CenterPrint(player, S_("^] ^bInvade Mode^b Timer Started ^[\n"));
		player = find(player, "classname", "player");
	}
}
//=================================================================
// Stops the flag capture timer.
//=================================================================
void Invade_stopTimer()
{
	entity player;
	//entity blueFlag;

	invade_timeToRewardDef = 0;
	bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^> ^bInvade Mode^b Timer Stopped ^<^-^-^-^>\n"));
	bprint(PR_PRINT_HIGH, "The flag capture timer has been stopped.\n");
	if (playersOnTeam1 == 0)
		bprint(PR_PRINT_HIGH, "Blue has no players.\n");
	if (playersOnTeam2 == 0)
		bprint(PR_PRINT_HIGH, "Red has no players.\n");
	bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
	BroadcastSound("doors/runeuse.wav");
	player = find(world, "classname", "player");
	while (player != world)
	{
		CenterPrint(player, S_("^] ^bInvade Mode^b Timer Stopped ^[\n"));
		player = find(player, "classname", "player");
	}
	// Return blue flag to its base.
	// THIS IS ONLY DONE IN AGR. REMOVE?
	/*blueFlag = find(world, "classname", "item_tfgoal");
	while (blueFlag != world)
	{
		if (blueFlag->team_no == 2)
		{
			self = blueFlag;
			// Both ways returns the flag.
			if (self->goal_state == PR_TFGS_ACTIVE && self->owner != world) // a player has it
				tfgoalitem_RemoveFromPlayer(self, self->owner, 1);
			else
				tfgoalitem_remove();
			blueFlag = world;  //break;  // PZ: break statements don't work (changed 9/5/15)
		}
		else // PZ: Added 9/5/15.
			blueFlag = find(blueFlag, "classname", "item_tfgoal");
	}*/
}
//=================================================================
// Used when teams score. Resets the time that Offense has to capture the flag.
//=================================================================
void Invade_resetTimer()
{
	string temp;

	invade_timeToRewardDef = time + invade_minutes*60;
	bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^-^> ^bInvade Mode^b Timer Reset ^<^-^-^-^-^>\n"));
	bprint(PR_PRINT_HIGH, string("The flag capture timer has been reset.\nThe offense (" + offenseTeamName + ") has "));
	temp = ftos(invade_minutes); temp = colstr(temp, PR_COLSTR_NUMBER);
	bprint(PR_PRINT_HIGH, temp); bprint(PR_PRINT_HIGH, string(" minutes to capture the defense's (" + defenseTeamName + "'s) flag.\n"));
	bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
	BroadcastSound("doors/runeuse.wav");
	// THIS IS ONLY DONE IN AGR. REMOVE?
  /*#ifdef PR_AGR_WHEN_BLUE_SCORES_RETURN_FLAG
	entity blueFlag;
	// Return blue flag to its base.
	blueFlag = find(world, "classname", "item_tfgoal");
	while (blueFlag != world)
	{
		if (blueFlag->team_no == 2)
		{
			self = blueFlag;
			// Both ways returns the flag.
			if (self->goal_state == PR_TFGS_ACTIVE && self->owner != world) // a player has it
				tfgoalitem_RemoveFromPlayer(self, self->owner, 1);
			else
				tfgoalitem_remove();
			blueFlag = world;  //break;  // PZ: break statements don't work (changed 9/5/15)
		}
		else // PZ: Added 9/5/15.
			blueFlag = find(blueFlag, "classname", "item_tfgoal");
	}
  #endif*/
}
//=================================================================
// Checks to see if it's time to play a timer sound, and if so, plays it.
// This works on both the prematch timer and the flag capture timer.
// TODO?: Would be a good place to also put in centerprint notifications if I wanted to.
//        Would be nice to not overwrite player's important menus, though, like the custom menu, maybe.
//=================================================================
void Invade_playTimerSounds()
{
	float minutes, seconds; // minutes:seconds of the timer (whichever timer is running)
	float totalSeconds;

	if      (invade_gameState == InvadeGameState::IN_PREMATCH)
		totalSeconds = invade_timePrematchEnds - floor(time);
	else if (invade_gameState == InvadeGameState::GAME_RUNNING && invade_minutes > 0)
		totalSeconds = invade_timeToRewardDef - floor(time);
	else
		return;
	if (invade_timeOfLastSound == totalSeconds) return;

	minutes = floor(totalSeconds / 60);
	seconds = totalSeconds - (60 * floor(totalSeconds / 60)); // sec = totalSec mod 60
	seconds = floor(seconds);

	/*if    (minutes == 5 && seconds == 0)
				{BroadcastSound("agr/cd5min.wav");  invade_timeOfLastSound = totalSeconds;}*/
	if      (minutes == 3 && seconds == 0)
				{BroadcastSound("agr/cd3min.wav");  invade_timeOfLastSound = totalSeconds;}
	else if (minutes == 1 && seconds == 0)
				{BroadcastSound("agr/cd1min.wav");  invade_timeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 30)
				{BroadcastSound("agr/cd30sec.wav"); invade_timeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 20)
				{BroadcastSound("agr/cd20sec.wav"); invade_timeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 10)
				{BroadcastSound("agr/cd10.wav");    invade_timeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 9)
				{BroadcastSound("agr/cd9.wav");     invade_timeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 8)
				{BroadcastSound("agr/cd8.wav");     invade_timeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 7)
				{BroadcastSound("agr/cd7.wav");     invade_timeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 6)
				{BroadcastSound("agr/cd6.wav");     invade_timeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 5)
				{BroadcastSound("agr/cd5.wav");     invade_timeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 4)
				{BroadcastSound("agr/cd4.wav");     invade_timeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 3)
				{BroadcastSound("agr/cd3.wav");     invade_timeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 2)
				{BroadcastSound("agr/cd2.wav");     invade_timeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 1)
				{BroadcastSound("agr/cd1.wav");     invade_timeOfLastSound = totalSeconds;}
}
//=================================================================
// Called in WorldSpawn().
//=================================================================
void Invade_precacheSounds()
{
	precache_sound2("agr/redonoffense.wav");
	precache_sound2("agr/blueonoffense.wav");
	precache_sound2("agr/prepare.wav");
	precache_sound2("agr/fight.wav");
	//precache_sound2("agr/cd5min.wav");
	precache_sound2("agr/cd3min.wav");
	precache_sound2("agr/cd1min.wav");
	precache_sound2("agr/cd30sec.wav");
	precache_sound2("agr/cd20sec.wav");
	precache_sound2("agr/cd10.wav");
	precache_sound2("agr/cd9.wav");
	precache_sound2("agr/cd8.wav");
	precache_sound2("agr/cd7.wav");
	precache_sound2("agr/cd6.wav");
	precache_sound2("agr/cd5.wav");
	precache_sound2("agr/cd4.wav");
	precache_sound2("agr/cd3.wav");
	precache_sound2("agr/cd2.wav");
	precache_sound2("agr/cd1.wav");
}

} // END namespace Progs
