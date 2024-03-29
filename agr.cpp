// agr.qc                                     ver. March 14, 2010
// Author: Pulseczar
//
// - Attackers Go Red -
// For the idea of red always attacking and blue always defending,
// credit goes to OneManClan. He also came up with the name.

// This mode should work on virtually all standard TF maps
// containing 2 teams, with 1 flag each.
//
// Sounds for AGR / Invade modes came from Unreal Tournament 2004.
// You can open UAX sound pack files with Dragon UnPACKer.

#include "progs.h"
#include "agr.h"
#include "tforttm.h"
#include "spectate.h"
#include "client.h"
#include "tfortmap.h"

namespace Progs {

#ifdef PR_0
//=================================================================
// globals defined in defs.qc; change in defs.qc and mirror here
//=================================================================

// cvars - requires 'localinfo' to set these
float agr;                        // globar cvar, just on or off (requires 'localinfo agr 1' to be on)
float agr_frags;                  // frags blue gets for successfully defending for agr_minutes minutes
float agr_minutes;                // number of minutes blue has to successfully defend in order to be
                                  // rewarded frags
float agr_vote_max;               // overrides default maximum number of players allowed to vote for AGR
float agr_teamfrags;              // overrides the serverinfo 'teamfrags' (or 't') var when AGR is active,
                                  // which controls whether or not each players' frags are equal to their
                                  // team's score
// globals
float agrTimeToRewardBlue;           // don't try to set it (it's set by functions)
float agrTimerRunning;            // boolean
float agrTimeOfLastSound;         // in seconds, what the timer was on last timer sound
float agrFlagRespawnWait;         // is flag currently waiting to spawn? (done after captures)

#define PR_AGR_ROTATION_OFF PR_TRUE  // on a map triggered by map rotation, AGR is never on
#define PR_AGR_VOTE_MAX 0            // default maximum number of players allowed for an AGR vote (0 means no max)
#define PR_AGR_BLUE_RESPAWN_DELAY 5  // how long blue has to wait to respawn
                                     // (to make up for time red has to travel to contested area)
#define PR_AGR_FLAG_RESPAWN_DELAY 15 // when red captures the flag, how long does it take the flag to respawn?
// #define AGR_WHEN_BLUE_SCORES_RETURN_FLAG

#endif

//=================================================================
// called by the localinfo key initializing function
//=================================================================
void AGR_initialize()
{
	string st;
	float force_agr;

	st = infokey(world, "agr"); // ARE YOU HAPPY, OMC?? :P
	if (st == string_null) st = "0"; // sets default
	agr = stof(st);
	if (agr != 1) agr = 0; // has to be set to 1 to engage AGR

	//WK 5-15-08 Added command to force on AGR regardless of vote setting, for Phrosty
	st = infokey(world, "force_agr");
	if (st == string_null) st = "0"; // sets default
	force_agr = stof(st);
	if (force_agr == 1) agr = 1; // Override AGR setting if 1

	if (agr)
	{
		st = infokey(world, "agr_frags");
		if (st == string_null) st = "10"; // sets default
		agr_frags = stof(st);
		if (agr_frags < 0) agr_frags = 0; // the allowed minimum

		st = infokey(world, "agr_minutes");
		if (st == string_null) st = "5"; // sets default
		agr_minutes = stof(st);
		if (agr_minutes < 0) agr_minutes = 0; // the allowed minimum

		st = infokey(world, "agr_teamfrags");
		if (st == string_null) st = "1"; //WK 5-15-08 Teamfrags on by default now in AGR mode
		agr_teamfrags = stof(st);
		if (agr_teamfrags != 1) agr_teamfrags = 0;
	}
}
//=================================================================
// called every frame in StartFrame(); maintains flag
// capture timer; awards blue team points for successfully
// defending for required time; stops the AGR timer if red or blue
// have 0 players
//=================================================================
void AGR_think()
{
	string temp;
	entity player;
	float timeLeft; // time left on the map
	float timeAccumulated; // amount of time blue has defended this 'clock cycle'
	float amtBlueCanScore; // max amount blue can score with time left
	float timesBlueCanScore; // max number of times blue can score with time left

	if (agr && !agrFlagRespawnWait)
	{
		if (playersOnTeam1 && playersOnTeam2)
		{
			if (agrTimerRunning)
			{
				if (agrTimeToRewardBlue <= time) // has blue successfully defended for agr_minutes minutes?
				{
					// give frags to blue players
					TeamFortress_TeamIncreaseScore(1, agr_frags, world);
					player = find(world, "classname", "player");
					while (player != world)
					{
						if (player->team_no == 1)
						{
							player->real_frags = player->real_frags + agr_frags;
							if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
								player->frags = player->real_frags;
						}
						CenterPrint(player, S_("^] Blue Successfully Defends! ^[\n"));
						player = find(player, "classname", "player");
					}
					bprint(PR_PRINT_HIGH, S_("\n^<^-^-^> Blue Successfully Defends ^<^-^-^-^>\n"));
					bprint(PR_PRINT_HIGH, "Blue is awarded ");
					temp = ftos(agr_frags); temp = colstr(temp, PR_COLSTR_NUMBER);
					bprint(PR_PRINT_HIGH, temp);
					bprint(PR_PRINT_HIGH, " frags for\nsuccessfully defending for ");
					temp = ftos(agr_minutes); temp = colstr(temp, PR_COLSTR_NUMBER);
					bprint(PR_PRINT_HIGH, temp); bprint(PR_PRINT_HIGH, " minutes!\n");
					bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
					TeamFortress_TeamShowScores(2, 1, agr_frags);
					BroadcastSound("boss2/pop2.wav");
					AGR_resetTimer();
				}
				AGR_playTimerSounds();

				// check to see if the game should end because blue
				// doesn't have enough time to catch up to red in score
				if (timelimit != 0)
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
				}
			}
			else if (prematch < time)
				AGR_startTimer();
		}
		else if (agrTimerRunning)
			AGR_stopTimer();
	}
}
//=================================================================
// starts the flag capture timer
//=================================================================
void AGR_startTimer()
{
	if (prematch >= time) return;

	string temp;
	entity player;

	agrTimeToRewardBlue = time + agr_minutes*60; // set the defender's reward timer
	agrTimerRunning = 1;
	temp = ftos(agr_minutes);
	temp = colstr(temp, PR_COLSTR_NUMBER);
	bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^-^-^-^> ^bAGR^b Timer Started ^<^-^-^-^-^-^-^-^>\n"));
	bprint(PR_PRINT_HIGH, "Blue must successfully defend their\nflag for ");
	bprint(PR_PRINT_HIGH, temp); bprint(PR_PRINT_HIGH, " minutes, in order to score.\n");
	bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
	BroadcastSound("misc/runekey.wav");
	player = find(world, "classname", "player");
	while (player != world)
	{
		CenterPrint(player, S_("^] ^bAGR^b Timer Started ^[\n"));
		player = find(player, "classname", "player");
	}
}
//=================================================================
// stops the flag capture timer; returns blue flag to its base
//=================================================================
void AGR_stopTimer()
{
	entity player;
	entity blueFlag;

	agrTimerRunning = 0;
	bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^-^-^-^> ^bAGR^b Timer Stopped ^<^-^-^-^-^-^-^-^>\n"));
	bprint(PR_PRINT_HIGH, "The flag capture timer has been stopped.\n");
	if (playersOnTeam1 == 0)
		bprint(PR_PRINT_HIGH, "Blue has no players.\n");
	else if (playersOnTeam2 == 0)
		bprint(PR_PRINT_HIGH, "Red has no players.\n");
	bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
	BroadcastSound("doors/runeuse.wav");
	player = find(world, "classname", "player");
	while (player != world)
	{
		CenterPrint(player, S_("^] ^bAGR^b Timer Stopped ^[\n"));
		player = find(player, "classname", "player");
	}
	// return blue flag to its base
	blueFlag = find(world, "classname", "item_tfgoal");
	while (blueFlag != world)
	{
		if (blueFlag->team_no == 2)
		{
			self = blueFlag;
			// both ways returns the flag
			if (self->goal_state == PR_TFGS_ACTIVE && self->owner != world) // a player has it
				tfgoalitem_RemoveFromPlayer(self, self->owner, 1);
			else
				tfgoalitem_remove();
			blueFlag = world;  //break;  // PZ: break statements don't work (changed 9/5/15)
		}
		else // PZ: added 9/5/15
			blueFlag = find(blueFlag, "classname", "item_tfgoal");
	}
}
//=================================================================
// used when teams score; resets the timer for which blue has to
// successfully defend; (may return blue flag to its base)
//=================================================================
void AGR_resetTimer()
{
	string temp;

	agrTimeToRewardBlue = time + agr_minutes*60;
	bprint(PR_PRINT_HIGH, S_("\n^<^-^-^-^-^-^-^-^> ^bAGR^b Timer Reset ^<^-^-^-^-^-^-^-^-^>\n"));
	bprint(PR_PRINT_HIGH, "The flag capture timer has been reset.\nRed has ");
	temp = ftos(agr_minutes); temp = colstr(temp, PR_COLSTR_NUMBER);
	bprint(PR_PRINT_HIGH, temp); bprint(PR_PRINT_HIGH, " minutes to capture Blue's flag.\n");
	bprint(PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
	BroadcastSound("doors/runeuse.wav");
  #ifdef PR_AGR_WHEN_BLUE_SCORES_RETURN_FLAG
	entity blueFlag;
	// return blue flag to its base
	blueFlag = find(world, "classname", "item_tfgoal");
	while (blueFlag != world)
	{
		if (blueFlag->team_no == 2)
		{
			self = blueFlag;
			// both ways returns the flag
			if (self->goal_state == PR_TFGS_ACTIVE && self->owner != world) // a player has it
				tfgoalitem_RemoveFromPlayer(self, self->owner, 1);
			else
				tfgoalitem_remove();
			blueFlag = world;  //break;  // PZ: break statements don't work (changed 9/5/15)
		}
		else // PZ: added 9/5/15
			blueFlag = find(blueFlag, "classname", "item_tfgoal");
	}
  #endif
}
//=================================================================
// checks to see if it's time to play a timer sound, and if so,
// plays it
//=================================================================
void AGR_playTimerSounds()
{
	if (!agrTimerRunning) return;
	float minutes, seconds; // minutes:seconds of the flag timer
	float totalSeconds;

	totalSeconds = agrTimeToRewardBlue - floor(time);
	if (agrTimeOfLastSound == totalSeconds) return;

	minutes = floor(totalSeconds / 60);
	seconds = totalSeconds - (60 * floor(totalSeconds / 60)); // sec = totalSec mod 60
	seconds = floor(seconds);

	/*if    (minutes == 5 && seconds == 0)
				{BroadcastSound("agr/cd5min.wav");  agrTimeOfLastSound = totalSeconds;}*/
	if      (minutes == 3 && seconds == 0)
				{BroadcastSound("agr/cd3min.wav");  agrTimeOfLastSound = totalSeconds;}
	else if (minutes == 1 && seconds == 0)
				{BroadcastSound("agr/cd1min.wav");  agrTimeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 30)
				{BroadcastSound("agr/cd30sec.wav"); agrTimeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 20)
				{BroadcastSound("agr/cd20sec.wav"); agrTimeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 10)
				{BroadcastSound("agr/cd10.wav");    agrTimeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 9)
				{BroadcastSound("agr/cd9.wav");     agrTimeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 8)
				{BroadcastSound("agr/cd8.wav");     agrTimeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 7)
				{BroadcastSound("agr/cd7.wav");     agrTimeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 6)
				{BroadcastSound("agr/cd6.wav");     agrTimeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 5)
				{BroadcastSound("agr/cd5.wav");     agrTimeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 4)
				{BroadcastSound("agr/cd4.wav");     agrTimeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 3)
				{BroadcastSound("agr/cd3.wav");     agrTimeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 2)
				{BroadcastSound("agr/cd2.wav");     agrTimeOfLastSound = totalSeconds;}
	else if (minutes == 0 && seconds == 1)
				{BroadcastSound("agr/cd1.wav");     agrTimeOfLastSound = totalSeconds;}
}
//=================================================================
// alerts the joining player of AGR being active
//=================================================================
void AGR_introducePlayer()
{
	sprint(self, PR_PRINT_HIGH, S_("^<^-^-^-^> ^bAttackers Go Red^b enabled ^<^-^-^-^>\n"));
	sprint(self, PR_PRINT_HIGH, "To be an attacker, go Red, else go Blue and defend.\n");
	if (!agrTimerRunning)
		sprint(self, PR_PRINT_HIGH, "The timer for flag capture has not started yet.\n");
	sprint(self, PR_PRINT_HIGH, S_("^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n"));
	stuffcmd(self, "play agr/redonoffense.wav\n");
}
//=================================================================
// prints the remaining time on the flag capture timer, when the
// 'inv' function is called
//=================================================================
void AGR_invPrint()
{
	string temp;
	float x, y;

	sprint(self, PR_PRINT_HIGH, S_("\n^bAGR^b Capture Timer: "));
	if (agrTimerRunning)
	{
		x = agrTimeToRewardBlue - time; // in seconds
		// convert to min:sec
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
		sprint(self, PR_PRINT_HIGH, "timer stopped");
}
//=================================================================
// called in WorldSpawn()
//=================================================================
void AGR_precacheSounds()
{
	precache_sound2("agr/redonoffense.wav");
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
