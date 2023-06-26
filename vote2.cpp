// Created by Pulseczar on Oct 24, 2016.
// I created this file because my new vote menus were making an already large file (vote.cpp),
// into a much larger file.

#include "progs.h"
#include "vote2.h"
#include "vote.h"
#include "menu.h"
#include "optimize.h"
#include "cpstuff.h"
#include "spectate.h"

namespace Progs {

// PZ: This array holds the data for the bot vote menu.
// Index 0 is the default data (normal settings).
// Index 1 to 32 (or MAX_CLIENTS) are for each player's current uncommitted values while in the menu.
// Index 33 to 64 (depending on MAX_CLIENTS) are for each player's committed values while in the menu.
// Index 65 to 96 (depending on MAX_CLIENTS) are flags that indicate whether the user entered the value, themselves.
// Index 97 (depending on MAX_CLIENTS) holds the current data in use in the game.
//const unsigned int VOTEBOTDATALEN = MAX_CLIENTS * 3 + 2;
BotVoteMenuData botVoteMenuData[VOTEBOTDATALEN];

// =======================
// BEGIN VOTEGEN FUNCTIONS
// =======================
// TODO
// =======================
// END VOTEGEN FUNCTIONS
// =======================

// =======================
// BEGIN VOTEBOT FUNCTIONS
// =======================

// PZ: Clears the votebot settings on map load.
void initVoteBotMenuData()
{
    for (int i = DEFAULTS + 1; i < VOTEBOTDATALEN; i++)
		copyVoteBotData(DEFAULTS, i, PR_VOTEBOT_MENU_MAIN);
}

// PZ: Helper function, for votebot menu. Used to copy an entire BotVoteMenuData structure from one index
//     to another. Index DEFAULTS means default settings. CURRENT means current settings.
// `from` and `to` : indexes
// `menu` : only this menu and submenus are changed
// `alsoCopyToCommittedData` : when used on player data, also copies to their committed data
void copyVoteBotData(int from, int to, int menu, bool alsoCopyToCommittedData /*= false*/)
{
	switch (menu)
	{
		case PR_VOTEBOT_MENU_MAIN:
		{
			copyVoteBotData(from, to, PR_VOTEBOT_MENU_BOTCOUNT, alsoCopyToCommittedData);
			copyVoteBotData(from, to, PR_VOTEBOT_MENU_BOTSKILL, alsoCopyToCommittedData);
			copyVoteBotData(from, to, PR_VOTEBOT_MENU_BOTCLASSES, alsoCopyToCommittedData);
			copyVoteBotData(from, to, PR_VOTEBOT_MENU_BOTBEHAVIOR, alsoCopyToCommittedData);
			return; // committed data has already been cleared at this point
		}
		case PR_VOTEBOT_MENU_BOTCOUNT:
		{
			botVoteMenuData[to].idealNumPlayers = botVoteMenuData[from].idealNumPlayers;
			for (int i = 0; i < 4; i++)
				botVoteMenuData[to].percentPlyrsPerTeam[i] = botVoteMenuData[from].percentPlyrsPerTeam[i];
			botVoteMenuData[to].forceHumansToTeam = botVoteMenuData[from].forceHumansToTeam;
		}
		case PR_VOTEBOT_MENU_BOTSKILL:
		{
			for (int i = 0; i < 5; i++)
			{
				botVoteMenuData[to].botMinSkillLevel[i]    = botVoteMenuData[from].botMinSkillLevel[i];
				botVoteMenuData[to].botMaxSkillLevel[i]    = botVoteMenuData[from].botMaxSkillLevel[i];
				botVoteMenuData[to].botLockedSkillLevel[i] = botVoteMenuData[from].botLockedSkillLevel[i];
			}
		}
		case PR_VOTEBOT_MENU_BOTSKILL_SUB1:
		{
			int i = botVoteMenuData[to].menuTeamSelected;
			botVoteMenuData[to].botMinSkillLevel[i]    = botVoteMenuData[from].botMinSkillLevel[i];
			botVoteMenuData[to].botMaxSkillLevel[i]    = botVoteMenuData[from].botMaxSkillLevel[i];
			botVoteMenuData[to].botLockedSkillLevel[i] = botVoteMenuData[from].botLockedSkillLevel[i];
		}
		case PR_VOTEBOT_MENU_BOTCLASSES:
		{
			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < 10; j++)
					botVoteMenuData[to].botClassPercentage[i][j] = botVoteMenuData[from].botClassPercentage[i][j];
			}
		}
		case PR_VOTEBOT_MENU_BOTCLASSES_SUB1:
		{
			int i = botVoteMenuData[to].menuTeamSelected;
			for (int j = 0; j < 5; j++)
				botVoteMenuData[to].botClassPercentage[i][j] = botVoteMenuData[from].botClassPercentage[i][j];
		}
		case PR_VOTEBOT_MENU_BOTCLASSES_SUB2:
		{
			int i = botVoteMenuData[to].menuTeamSelected;
			for (int j = 5; j < 10; j++)
				botVoteMenuData[to].botClassPercentage[i][j] = botVoteMenuData[from].botClassPercentage[i][j];
		}
		case PR_VOTEBOT_MENU_BOTBEHAVIOR:
		{
			for (int i = 0; i < 5; i++)
			{
				botVoteMenuData[to].botDefensePercent[i] = botVoteMenuData[from].botDefensePercent[i];
				botVoteMenuData[to].botOffensePercent[i] = botVoteMenuData[from].botOffensePercent[i];
				botVoteMenuData[to].botNeitherPercent[i] = botVoteMenuData[from].botNeitherPercent[i];
				botVoteMenuData[to].botRandomPercent[i]  = botVoteMenuData[from].botRandomPercent[i];
			}
		}
		case PR_VOTEBOT_MENU_BOTBEHAVIOR_SUB1:
		{
			int i = botVoteMenuData[to].menuTeamSelected;
			botVoteMenuData[to].botDefensePercent[i] = botVoteMenuData[from].botDefensePercent[i];
			botVoteMenuData[to].botOffensePercent[i] = botVoteMenuData[from].botOffensePercent[i];
			botVoteMenuData[to].botNeitherPercent[i] = botVoteMenuData[from].botNeitherPercent[i];
			botVoteMenuData[to].botRandomPercent[i]  = botVoteMenuData[from].botRandomPercent[i];
		}
	}
	// If copying to a player's uncommitted (non-Okay'd) data, also copy to the player's committed data,
	// if we haven't already.
	if (to >= 1 && to <= MAX_CLIENTS && alsoCopyToCommittedData)
		copyVoteBotData(from, to+MAX_CLIENTS, menu);
}

// PZ: Helper function, for votebot menu.
string getVoteBotTeamStr(int ndx)
{
	switch (ndx)
	{
		case 0:  return "Overall";
		case 1:  return "Team 1";
		case 2:  return "Team 2";
		case 3:  return "Team 3";
		case 4:  return "Team 4";
		default: return "Team ?";
	}
}

// PZ: For displaying the "votebot" menu.
// Things I want to allow voting on:
//   -bot count (overall and per team)
//   -humans versus bots mode (humans are forced to a team by themselves); voter selects a team for the humans
//   -bot difficulty (min, max, locked,
//                    also, potentially allow adjusting the kill-to-death ratio that the game tries to maintain, with skill level)
//   -bot classes (percentage of each class?, would be cool if it could be per team, as well)
//        example:
//            scout    0%
//            sniper   50%
//            soldier  50%          (and so on...)
//   -bot modes? (like defense versus offense)
//   -"random"? (as choice for some of these things)
void Menu_VoteBot()
{
	string st1, st2, st3, st4, st5, st6, st7;
	const string noEntryDenoter = S_("^b--^b");
	int plyrNum = ENT_TO_NUM(self);
	int teamSel = botVoteMenuData[plyrNum].menuTeamSelected;

	if (self->goal_state == PR_VOTEBOT_MENU_MAIN)
	{
		st1 = S_("^bBot Options^b:                  \n"
		         "                                  \n"
		         " ^1.. >> Player/Bot Count         \n"
		         " ^2.. >> Bot Skill Level          \n"
		         " ^3.. >> Bot Classes              \n"
		         " ^4.. >> Bot Behavior             \n"
		         "                                  \n"
		         "                                  \n"
		         " ^7.. ^bReset to Current^b        \n"
		         " ^8.. ^bReset to Defaults^b       \n"
		         " ^9.. ^bStart Vote^b              \n"
		         " ^0.. ^bCancel^b                  \n");
	}
	else if (self->goal_state == PR_VOTEBOT_MENU_BOTCOUNT)
	{
		st1 = S_("^bPlayer/Bot Count^b:             \n"
		         "                                  \n"
		         "  ^1.. Number Of Players/Bots @1  \n"
		         "  ^2.. Players/Bots On Team 1 @2% \n"
		         "  ^3.. Players/Bots On Team 2 @3% \n"
		         "  ^4.. Players/Bots On Team 3 @4% \n"
		         "  ^5.. Players/Bots On Team 4 @5% \n"
		         "  ^6.. Force Humans To Team   @6  \n"
		         "  ^7.. ^bReset to Current^b       \n"
		         "  ^8.. ^bReset to Defaults^b      \n"
		         "  ^9.. ^b<< Okay^b                \n"
		         "  ^0.. ^b<< Cancel^b              \n");
		//replaceStr(st1, "@1", infokey(world, "idealp"));
		replaceStr(st1, "@1", padstr(to_string(botVoteMenuData[plyrNum].idealNumPlayers), 3, 1));
		replaceStr(st1, "@2", padstr(to_string(botVoteMenuData[plyrNum].percentPlyrsPerTeam[0]), 3, 1));
		replaceStr(st1, "@3", padstr(to_string(botVoteMenuData[plyrNum].percentPlyrsPerTeam[1]), 3, 1));
		replaceStr(st1, "@4", padstr(to_string(botVoteMenuData[plyrNum].percentPlyrsPerTeam[2]), 3, 1));
		replaceStr(st1, "@5", padstr(to_string(botVoteMenuData[plyrNum].percentPlyrsPerTeam[3]), 3, 1));
		replaceStr(st1, "@6", padstr(to_string(botVoteMenuData[plyrNum].forceHumansToTeam), 3, 1));
		replaceStr(st1, "255", padstr(noEntryDenoter, 3, 1)); // value of 255 means no value set
	}
	else if (self->goal_state == PR_VOTEBOT_MENU_BOTSKILL)
	{
		st1 = S_("^bBot Skill Level^b:              \n"
		         "                                  \n"
		         "  ^1.. >> Overall                 \n"
		         "  ^2.. >> Team 1                  \n"  // settings in any Team Skill menu will override Overall settings
		         "  ^3.. >> Team 2                  \n"
		         "  ^4.. >> Team 3                  \n"
		         "  ^5.. >> Team 4                  \n"
		         "                                  \n"
		         "  ^7.. ^bReset to Current^b       \n"
		         "  ^8.. ^bReset to Defaults^b      \n"
		         "  ^9.. ^b<< Okay^b                \n"
		         "  ^0.. ^b<< Cancel^b              \n");
	}
	else if (self->goal_state == PR_VOTEBOT_MENU_BOTSKILL_SUB1)
	{
		st1 = S_("^bBot Skill Level^b: @T           \n"
		         "                                  \n"
		         "  ^1.. Minimum   @1               \n"
		         "  ^2.. Maximum   @2               \n"
		         "  ^3.. Lock To   @3               \n"
		         "                                  \n"
		         "                                  \n"
		         "                                  \n"
		         "  ^7.. ^bReset to Current^b       \n"
		         "  ^8.. ^bReset to Defaults^b      \n"
		         "  ^9.. ^b<< Okay^b                \n"
		         "  ^0.. ^b<< Cancel^b              \n");
		replaceStr(st1, "@T", getVoteBotTeamStr(teamSel));
		replaceStr(st1, "@1", padstr(to_string(botVoteMenuData[plyrNum].botMinSkillLevel[teamSel]), 3, 1));
		replaceStr(st1, "@2", padstr(to_string(botVoteMenuData[plyrNum].botMaxSkillLevel[teamSel]), 3, 1));
		replaceStr(st1, "@3", padstr(to_string(botVoteMenuData[plyrNum].botLockedSkillLevel[teamSel]), 3, 1));
		replaceStr(st1, "255", padstr(noEntryDenoter, 3, 1)); // value of 255 means no value set
	}
	else if (self->goal_state == PR_VOTEBOT_MENU_BOTCLASSES)
	{
		st1 = S_("^bBot Classes^b:                  \n"
		         "                                  \n"
		         "  ^1.. >> Overall                 \n"
		         "  ^2.. >> Team 1                  \n"  // settings in any Team Skill menu will override Overall settings
		         "  ^3.. >> Team 2                  \n"
		         "  ^4.. >> Team 3                  \n"
		         "  ^5.. >> Team 4                  \n"
		         "                                  \n"
		         "  ^7.. ^bReset to Current^b       \n"
		         "  ^8.. ^bReset to Defaults^b      \n"
		         "  ^9.. ^b<< Okay^b                \n"
		         "  ^0.. ^b<< Cancel^b              \n");
	}
	else if (self->goal_state == PR_VOTEBOT_MENU_BOTCLASSES_SUB1)
	{
		st1 = S_("^bBot Classes^b: @T               \n"
		         "                                  \n"
		         "  ^1.. Scout        @1%           \n"
		         "  ^2.. Sniper       @2%           \n"  // settings in any Team Skill menu will override Overall settings
		         "  ^3.. Soldier      @3%           \n"
		         "  ^4.. Demoman      @4%           \n"
		         "  ^5.. Medic        @5%           \n"
		         "  ^6.. ^b>> Next^b                \n"
		         "  ^7.. ^bReset to Current^b       \n"
		         "  ^8.. ^bReset to Defaults^b      \n"
		         "  ^9.. ^b<< Okay^b                \n"
		         "  ^0.. ^b<< Cancel^b              \n");
		replaceStr(st1, "@T", getVoteBotTeamStr(teamSel));
		replaceStr(st1, "@1", padstr(to_string(botVoteMenuData[plyrNum].botClassPercentage[teamSel][0]), 3, 1));
		replaceStr(st1, "@2", padstr(to_string(botVoteMenuData[plyrNum].botClassPercentage[teamSel][1]), 3, 1));
		replaceStr(st1, "@3", padstr(to_string(botVoteMenuData[plyrNum].botClassPercentage[teamSel][2]), 3, 1));
		replaceStr(st1, "@4", padstr(to_string(botVoteMenuData[plyrNum].botClassPercentage[teamSel][3]), 3, 1));
		replaceStr(st1, "@5", padstr(to_string(botVoteMenuData[plyrNum].botClassPercentage[teamSel][4]), 3, 1));
		replaceStr(st1, "255", padstr(noEntryDenoter, 3, 1)); // value of 255 means no value set
	}
	else if (self->goal_state == PR_VOTEBOT_MENU_BOTCLASSES_SUB2)
	{
		st1 = S_("^bBot Classes^b: @T               \n"
		         "                                  \n"
		         "  ^1.. Hwguy        @1%           \n"
		         "  ^2.. Pyro         @2%           \n"  // settings in any Team Skill menu will override Overall settings
		         "  ^3.. Spy          @3%           \n"
		         "  ^4.. Engineer     @4%           \n"
		         "  ^5.. Random       @5%           \n"  // might want civilian
		         "  ^6.. ^b<< Previous^b            \n"
		         "  ^7.. ^bReset to Current^b       \n"
		         "  ^8.. ^bReset to Defaults^b      \n"
		         "  ^9.. ^b<< Okay^b                \n"
		         "  ^0.. ^b<< Cancel^b              \n");
		replaceStr(st1, "@T", getVoteBotTeamStr(teamSel));
		replaceStr(st1, "@1", padstr(to_string(botVoteMenuData[plyrNum].botClassPercentage[teamSel][5]), 3, 1));
		replaceStr(st1, "@2", padstr(to_string(botVoteMenuData[plyrNum].botClassPercentage[teamSel][6]), 3, 1));
		replaceStr(st1, "@3", padstr(to_string(botVoteMenuData[plyrNum].botClassPercentage[teamSel][7]), 3, 1));
		replaceStr(st1, "@4", padstr(to_string(botVoteMenuData[plyrNum].botClassPercentage[teamSel][8]), 3, 1));
		replaceStr(st1, "@5", padstr(to_string(botVoteMenuData[plyrNum].botClassPercentage[teamSel][9]), 3, 1));
		replaceStr(st1, "255", padstr(noEntryDenoter, 3, 1)); // value of 255 means no value set
	}
	else if (self->goal_state == PR_VOTEBOT_MENU_BOTBEHAVIOR)
	{
		st1 = S_("^bBot Behavior^b:                 \n"
		         "                                  \n"
		         "  ^1.. >> Overall                 \n"
		         "  ^2.. >> Team 1                  \n"  // settings in any Team Skill menu will override Overall settings
		         "  ^3.. >> Team 2                  \n"
		         "  ^4.. >> Team 3                  \n"
		         "  ^5.. >> Team 4                  \n"
		         "                                  \n"
		         "  ^7.. ^bReset to Current^b       \n"
		         "  ^8.. ^bReset to Defaults^b      \n"
		         "  ^9.. ^b<< Okay^b                \n"
		         "  ^0.. ^b<< Cancel^b              \n");
	}
	else if (self->goal_state == PR_VOTEBOT_MENU_BOTBEHAVIOR_SUB1)
	{
		st1 = S_("^bBot Behavior^b: @T              \n"
		         "                                  \n"
		         "  ^1.. Defense   @1%              \n"
		         "  ^2.. Offense   @2%              \n"
		         "  ^3.. Neither   @3%              \n"
		         "  ^4.. Random    @4%              \n"
		         "                                  \n"
		         "                                  \n"
		         "  ^7.. ^bReset to Current^b       \n"
		         "  ^8.. ^bReset to Defaults^b      \n"
		         "  ^9.. ^b<< Okay^b                \n"
		         "  ^0.. ^b<< Cancel^b              \n");
		replaceStr(st1, "@T", getVoteBotTeamStr(teamSel));
		replaceStr(st1, "@1", padstr(to_string(botVoteMenuData[plyrNum].botDefensePercent[teamSel]), 3, 1));
		replaceStr(st1, "@2", padstr(to_string(botVoteMenuData[plyrNum].botOffensePercent[teamSel]), 3, 1));
		replaceStr(st1, "@3", padstr(to_string(botVoteMenuData[plyrNum].botNeitherPercent[teamSel]), 3, 1));
		replaceStr(st1, "@4", padstr(to_string(botVoteMenuData[plyrNum].botRandomPercent[teamSel]), 3, 1));
		replaceStr(st1, "255", padstr(noEntryDenoter, 3, 1)); // value of 255 means no value set
	}

	st1 = alignTextBlock(st1, PR_ALIGN_LEFT, PR_TRUE);

if (self->goal_state == PR_VOTEBOT_MENU_BOTBEHAVIOR)
{
	st1 += S_("\n^bThe effects of this menu are not\nprogrammed yet. So, it has no effect.^b\n");
}

	if (botVoteMenuData[plyrNum].gettingNumInput)
	{
		st2 = "\nType value, and press Enter (jump).\n";
		st3 = to_string(botVoteMenuData[plyrNum].inputNum) + "\n";
	}

	CenterPrint7(self, st1, st2, st3, st4, st5, st6, st7);
}

#include <arpa/inet.h> // TODO: remove when done with ntohs() below

// PZ: For input in the "votebot" menu.
void Menu_VoteBot_Input(float input)
{
	int plyrNum = ENT_TO_NUM(self);
	int teamSel = botVoteMenuData[plyrNum].menuTeamSelected;

	// numerical entry input
	if (botVoteMenuData[plyrNum].gettingNumInput)
	if (input >= 1 && input <= 10)
	{
		if (input == 10) input = 0;
		botVoteMenuData[plyrNum].inputNum = botVoteMenuData[plyrNum].inputNum * 10 + input;
		goto input_completion; // don't do normal input, below
	}

	// menu selection input
	if      (self->goal_state == PR_VOTEBOT_MENU_MAIN)
	{
		if      (input == 1)
		{
			self->goal_state = PR_VOTEBOT_MENU_BOTCOUNT;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 2)
		{
			self->goal_state = PR_VOTEBOT_MENU_BOTSKILL;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 3)
		{
			self->goal_state = PR_VOTEBOT_MENU_BOTCLASSES;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 4)
		{
			self->goal_state = PR_VOTEBOT_MENU_BOTBEHAVIOR;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 7)
		{
			copyVoteBotData(CURRENT, plyrNum, self->goal_state, true);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the current settings in play.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 8)
		{
			copyVoteBotData(DEFAULTS, plyrNum, self->goal_state, true);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the server default settings.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 9) // Start Vote
		{
			if (current_voteent != world)
			{
				sprint(self, PR_PRINT_HIGH, "There is currently a vote already running.\n");
				CuTFMenuSound(PR_MENUSOUND_WRONG);
			}
			/*else if (ntohs(net_local_adr.port) == 27500)
			{
				sprint(self, PR_PRINT_HIGH, "I have disabled this on the public server until it is working sufficiently well. -Pulse\n");
				CuTFMenuSound(PR_MENUSOUND_WRONG);
			}*/
			else
			{
				// copy data from uncommitted to committed section
				copyVoteBotData(plyrNum, plyrNum+MAX_CLIENTS, self->goal_state);
				StartVoteBot();
			}
			AllResetMenu();
			self->impulse = 0;
			return;
		}
		else if (input == 10) // Cancel
		{
			// Copying here is kind of redundant, because this will be done when you go back in menu,
			// but I'll leave it to make sure it's done.
			copyVoteBotData(CURRENT, plyrNum, self->goal_state, true);
			AllResetMenu();
			CuTFMenuSound(PR_MENUSOUND_SELL);
			BackFlash(self, 1);
			self->impulse = 0;
			return;
		}
	}
	else if (self->goal_state == PR_VOTEBOT_MENU_BOTCOUNT)
	{
		if      (input == 1)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 2)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 3)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 4)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 5)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 6)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 7)
		{
			copyVoteBotData(CURRENT, plyrNum, self->goal_state);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the current settings in play.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 8)
		{
			copyVoteBotData(DEFAULTS, plyrNum, self->goal_state);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the server default settings.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 9) // Okay
		{
			// copy data from uncommitted to committed section
			copyVoteBotData(plyrNum, plyrNum+MAX_CLIENTS, self->goal_state);
			self->goal_state = PR_VOTEBOT_MENU_MAIN;
			CuTFMenuSound(PR_MENUSOUND_BUY);
		}
		else if (input == 10) // Cancel
		{
			// copy data from committed to uncommitted section
			copyVoteBotData(plyrNum+MAX_CLIENTS, plyrNum, self->goal_state);
			self->goal_state = PR_VOTEBOT_MENU_MAIN;
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
	}
	else if (self->goal_state == PR_VOTEBOT_MENU_BOTSKILL)
	{
		if      (input == 1)
		{
			teamSel = botVoteMenuData[plyrNum].menuTeamSelected = 0;
			self->goal_state = PR_VOTEBOT_MENU_BOTSKILL_SUB1;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 2)
		{
			teamSel = botVoteMenuData[plyrNum].menuTeamSelected = 1;
			self->goal_state = PR_VOTEBOT_MENU_BOTSKILL_SUB1;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 3)
		{
			teamSel = botVoteMenuData[plyrNum].menuTeamSelected = 2;
			self->goal_state = PR_VOTEBOT_MENU_BOTSKILL_SUB1;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 4)
		{
			teamSel = botVoteMenuData[plyrNum].menuTeamSelected = 3;
			self->goal_state = PR_VOTEBOT_MENU_BOTSKILL_SUB1;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 5)
		{
			teamSel = botVoteMenuData[plyrNum].menuTeamSelected = 4;
			self->goal_state = PR_VOTEBOT_MENU_BOTSKILL_SUB1;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 7)
		{
			copyVoteBotData(CURRENT, plyrNum, self->goal_state, true);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the current settings in play.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 8)
		{
			copyVoteBotData(DEFAULTS, plyrNum, self->goal_state, true);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the server default settings.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 9) // Okay
		{
			// copy data from uncommitted to committed section
			copyVoteBotData(plyrNum, plyrNum+MAX_CLIENTS, self->goal_state);
			self->goal_state = PR_VOTEBOT_MENU_MAIN;
			CuTFMenuSound(PR_MENUSOUND_BUY);
		}
		else if (input == 10) // Cancel
		{
			// copy data from committed to uncommitted section
			copyVoteBotData(plyrNum+MAX_CLIENTS, plyrNum, self->goal_state);
			self->goal_state = PR_VOTEBOT_MENU_MAIN;
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
	}
	else if (self->goal_state == PR_VOTEBOT_MENU_BOTSKILL_SUB1)
	{
		if      (input == 1)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 2)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 3)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 7)
		{
			copyVoteBotData(CURRENT, plyrNum, self->goal_state);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the current settings in play.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 8)
		{
			copyVoteBotData(DEFAULTS, plyrNum, self->goal_state);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the server default settings.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 9) // Okay
		{
			// copy data from uncommitted to committed section
			copyVoteBotData(plyrNum, plyrNum+MAX_CLIENTS, self->goal_state);
			self->goal_state = PR_VOTEBOT_MENU_BOTSKILL;
			CuTFMenuSound(PR_MENUSOUND_BUY);
		}
		else if (input == 10) // Cancel
		{
			// copy data from committed to uncommitted section
			copyVoteBotData(plyrNum+MAX_CLIENTS, plyrNum, self->goal_state);
			self->goal_state = PR_VOTEBOT_MENU_BOTSKILL;
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
	}
	else if (self->goal_state == PR_VOTEBOT_MENU_BOTCLASSES)
	{
		if      (input == 1)
		{
			teamSel = botVoteMenuData[plyrNum].menuTeamSelected = 0;
			self->goal_state = PR_VOTEBOT_MENU_BOTCLASSES_SUB1;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 2)
		{
			teamSel = botVoteMenuData[plyrNum].menuTeamSelected = 1;
			self->goal_state = PR_VOTEBOT_MENU_BOTCLASSES_SUB1;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 3)
		{
			teamSel = botVoteMenuData[plyrNum].menuTeamSelected = 2;
			self->goal_state = PR_VOTEBOT_MENU_BOTCLASSES_SUB1;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 4)
		{
			teamSel = botVoteMenuData[plyrNum].menuTeamSelected = 3;
			self->goal_state = PR_VOTEBOT_MENU_BOTCLASSES_SUB1;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 5)
		{
			teamSel = botVoteMenuData[plyrNum].menuTeamSelected = 4;
			self->goal_state = PR_VOTEBOT_MENU_BOTCLASSES_SUB1;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 7)
		{
			copyVoteBotData(CURRENT, plyrNum, self->goal_state, true);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the current settings in play.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 8)
		{
			copyVoteBotData(DEFAULTS, plyrNum, self->goal_state, true);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the server default settings.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 9) // Okay
		{
			// copy data from uncommitted to committed section
			copyVoteBotData(plyrNum, plyrNum+MAX_CLIENTS, self->goal_state);
			self->goal_state = PR_VOTEBOT_MENU_MAIN;
			CuTFMenuSound(PR_MENUSOUND_BUY);
		}
		else if (input == 10) // Cancel
		{
			// copy data from committed to uncommitted section
			copyVoteBotData(plyrNum+MAX_CLIENTS, plyrNum, self->goal_state);
			self->goal_state = PR_VOTEBOT_MENU_MAIN;
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
	}
	else if (self->goal_state == PR_VOTEBOT_MENU_BOTCLASSES_SUB1)
	{
		if      (input == 1)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 2)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 3)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 4)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 5)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 6)
		{
			self->goal_state = PR_VOTEBOT_MENU_BOTCLASSES_SUB2;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 7)
		{
			copyVoteBotData(CURRENT, plyrNum, self->goal_state);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the current settings in play.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 8)
		{
			copyVoteBotData(DEFAULTS, plyrNum, self->goal_state);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the server default settings.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 9) // Okay
		{
			// copy data from uncommitted to committed section
			copyVoteBotData(plyrNum, plyrNum+MAX_CLIENTS, self->goal_state);
			self->goal_state = PR_VOTEBOT_MENU_BOTCLASSES;
			CuTFMenuSound(PR_MENUSOUND_BUY);
		}
		else if (input == 10) // Cancel
		{
			// copy data from committed to uncommitted section
			copyVoteBotData(plyrNum+MAX_CLIENTS, plyrNum, self->goal_state);
			self->goal_state = PR_VOTEBOT_MENU_BOTCLASSES;
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
	}
	else if (self->goal_state == PR_VOTEBOT_MENU_BOTCLASSES_SUB2)
	{
		if      (input == 1)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 2)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 3)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 4)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 5)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 6)
		{
			self->goal_state = PR_VOTEBOT_MENU_BOTCLASSES_SUB1;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 7)
		{
			copyVoteBotData(CURRENT, plyrNum, self->goal_state);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the current settings in play.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 8)
		{
			copyVoteBotData(DEFAULTS, plyrNum, self->goal_state);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the server default settings.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 9) // Okay
		{
			// copy data from uncommitted to committed section
			copyVoteBotData(plyrNum, plyrNum+MAX_CLIENTS, self->goal_state);
			self->goal_state = PR_VOTEBOT_MENU_BOTCLASSES;
			CuTFMenuSound(PR_MENUSOUND_BUY);
		}
		else if (input == 10) // Cancel
		{
			// copy data from committed to uncommitted section
			copyVoteBotData(plyrNum+MAX_CLIENTS, plyrNum, self->goal_state);
			self->goal_state = PR_VOTEBOT_MENU_BOTCLASSES;
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
	}
	else if (self->goal_state == PR_VOTEBOT_MENU_BOTBEHAVIOR)
	{
		if      (input == 1)
		{
			teamSel = botVoteMenuData[plyrNum].menuTeamSelected = 0;
			self->goal_state = PR_VOTEBOT_MENU_BOTBEHAVIOR_SUB1;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 2)
		{
			teamSel = botVoteMenuData[plyrNum].menuTeamSelected = 1;
			self->goal_state = PR_VOTEBOT_MENU_BOTBEHAVIOR_SUB1;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 3)
		{
			teamSel = botVoteMenuData[plyrNum].menuTeamSelected = 2;
			self->goal_state = PR_VOTEBOT_MENU_BOTBEHAVIOR_SUB1;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 4)
		{
			teamSel = botVoteMenuData[plyrNum].menuTeamSelected = 3;
			self->goal_state = PR_VOTEBOT_MENU_BOTBEHAVIOR_SUB1;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 5)
		{
			teamSel = botVoteMenuData[plyrNum].menuTeamSelected = 4;
			self->goal_state = PR_VOTEBOT_MENU_BOTBEHAVIOR_SUB1;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		else if (input == 7)
		{
			copyVoteBotData(CURRENT, plyrNum, self->goal_state, true);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the current settings in play.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 8)
		{
			copyVoteBotData(DEFAULTS, plyrNum, self->goal_state, true);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the server default settings.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 9) // Okay
		{
			// copy data from uncommitted to committed section
			copyVoteBotData(plyrNum, plyrNum+MAX_CLIENTS, self->goal_state);
			self->goal_state = PR_VOTEBOT_MENU_MAIN;
			CuTFMenuSound(PR_MENUSOUND_BUY);
		}
		else if (input == 10) // Cancel
		{
			// copy data from committed to uncommitted section
			copyVoteBotData(plyrNum+MAX_CLIENTS, plyrNum, self->goal_state);
			self->goal_state = PR_VOTEBOT_MENU_MAIN;
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
	}
	else if (self->goal_state == PR_VOTEBOT_MENU_BOTBEHAVIOR_SUB1)
	{
		if      (input == 1)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 2)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 3)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 4)
		{
			botVoteMenuData[plyrNum].gettingNumInput = true;
			botVoteMenuData[plyrNum].inputForOptNum = input;
			botVoteMenuData[plyrNum].inputNum = 0;
		}
		else if (input == 7)
		{
			copyVoteBotData(CURRENT, plyrNum, self->goal_state);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the current settings in play.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 8)
		{
			copyVoteBotData(DEFAULTS, plyrNum, self->goal_state);
			sprint(self, PR_PRINT_HIGH, "Reset menu and submenus to the server default settings.\n");
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
		else if (input == 9) // Okay
		{
			// copy data from uncommitted to committed section
			copyVoteBotData(plyrNum, plyrNum+MAX_CLIENTS, self->goal_state);
			self->goal_state = PR_VOTEBOT_MENU_BOTBEHAVIOR;
			CuTFMenuSound(PR_MENUSOUND_BUY);
		}
		else if (input == 10) // Cancel
		{
			// copy data from committed to uncommitted section
			copyVoteBotData(plyrNum+MAX_CLIENTS, plyrNum, self->goal_state);
			self->goal_state = PR_VOTEBOT_MENU_BOTBEHAVIOR;
			CuTFMenuSound(PR_MENUSOUND_SELL);
		}
	}

input_completion:

	if (self->impulse >= 1 && self->impulse <= 10)
	{
		Menu_VoteBot(); // immediately update the menu display
	}

	self->impulse = 0;


return;	// BELOW IS THE OLD VOTEMAP CODE, FOR REFERENCE
	float voteType, voteMapType;

	if (input >= 1 && input <= 6)
	{
		if (current_voteent != world)
		{
			sprint(self, PR_PRINT_HIGH, "Already running a vote!\n");
			AllResetMenu();
			self->impulse = 0;
			return;
		}

		float nummap;

		nummap = input + (self->goal_state*6);

		StartVoteMap(nummap,"");
		AllResetMenu();
	}
	else if (input == 7)
	{
		voteType = GetByte1( self->group_no );
		voteMapType = GetByte2( self->group_no );

		if      (voteMapType == PR_VOTEMAP_DEFAULT)
			voteMapType = PR_VOTEMAP_AGR;
		else if (voteMapType == PR_VOTEMAP_AGR)
			voteMapType = PR_VOTEMAP_INVADE;
		else if (voteMapType == PR_VOTEMAP_INVADE)
			voteMapType = PR_VOTEMAP_NEO;
#ifdef PR_COOP_MODE_ENHANCED
		else if ( voteMapType == PR_VOTEMAP_NEO )
			voteMapType = PR_VOTEMAP_TF;
		else if ( voteMapType == PR_VOTEMAP_TF )
			voteMapType = PR_VOTEMAP_COOP;
		else if ( voteMapType == PR_VOTEMAP_COOP )
			voteMapType = PR_VOTEMAP_INSANECOOP;
#endif
		else {
			voteMapType = PR_VOTEMAP_DEFAULT;

			if ( voteType == PR_VOTE_NEXTMAP )
				voteType = PR_VOTE_SWITCHMAP;
			else
				voteType = PR_VOTE_NEXTMAP;
		}

		self->group_no = voteType;
		self->group_no = AssignByte2( self->group_no, voteMapType );

		CuTFMenuSound(PR_MENUSOUND_BUY);

		NoCprintFX();
		Menu_VoteMap();
	}
	else if (input == 8)
	{
		self->goal_state = self->goal_state - 1;
		if (self->goal_state < 0)
			self->goal_state = 5;

		CuTFMenuSound(PR_MENUSOUND_BROWSE);

		NoCprintFX();
		Menu_VoteMap();
	}
	else if (input == 9)
	{
		self->goal_state = self->goal_state + 1;
		if (self->goal_state > 5)
			self->goal_state = 0;

		CuTFMenuSound(PR_MENUSOUND_BROWSE);

		NoCprintFX();
		Menu_VoteMap();
	}
	else if (input == 10)
	{
		AllResetMenu();
		CuTFMenuSound(PR_MENUSOUND_SELL);
		BackFlash(self, 1);
	}

	self->impulse = 0;
}

void voteBotValueEntryDone(entity player)
{
	int plyrNum = ENT_TO_NUM(player);
	int teamSel = botVoteMenuData[plyrNum].menuTeamSelected;
	int input   = botVoteMenuData[plyrNum].inputNum;
    switch (player->goal_state)
    {
		case PR_VOTEBOT_MENU_BOTCOUNT:
		{
			switch (botVoteMenuData[plyrNum].inputForOptNum)
			{
				case 1: botVoteMenuData[plyrNum].idealNumPlayers        = botVoteMenuData[plyrNum*3].idealNumPlayers        = voteBotInputLimits(input, 0, MAX_CLIENTS); break;
				case 2: botVoteMenuData[plyrNum].percentPlyrsPerTeam[0] = botVoteMenuData[plyrNum*3].percentPlyrsPerTeam[0] = voteBotInputLimits(input, 0, 100); break;
				case 3: botVoteMenuData[plyrNum].percentPlyrsPerTeam[1] = botVoteMenuData[plyrNum*3].percentPlyrsPerTeam[1] = voteBotInputLimits(input, 0, 100); break;
				case 4: botVoteMenuData[plyrNum].percentPlyrsPerTeam[2] = botVoteMenuData[plyrNum*3].percentPlyrsPerTeam[2] = voteBotInputLimits(input, 0, 100); break;
				case 5: botVoteMenuData[plyrNum].percentPlyrsPerTeam[3] = botVoteMenuData[plyrNum*3].percentPlyrsPerTeam[3] = voteBotInputLimits(input, 0, 100); break;
				case 6: botVoteMenuData[plyrNum].forceHumansToTeam      = botVoteMenuData[plyrNum*3].forceHumansToTeam      = voteBotInputLimits(input, 1, number_of_teams); break;
			}
			break;
		}
		case PR_VOTEBOT_MENU_BOTSKILL_SUB1:
		{
			switch (botVoteMenuData[plyrNum].inputForOptNum)
			{
				case 1: botVoteMenuData[plyrNum].botMinSkillLevel[teamSel]    = botVoteMenuData[plyrNum*3].botMinSkillLevel[teamSel]    = voteBotInputLimits(input, 1, 4); break;
				case 2: botVoteMenuData[plyrNum].botMaxSkillLevel[teamSel]    = botVoteMenuData[plyrNum*3].botMaxSkillLevel[teamSel]    = voteBotInputLimits(input, 1, 4); break;
				case 3: botVoteMenuData[plyrNum].botLockedSkillLevel[teamSel] = botVoteMenuData[plyrNum*3].botLockedSkillLevel[teamSel] = voteBotInputLimits(input, 1, 4); break;
			}
			break;
		}
		case PR_VOTEBOT_MENU_BOTCLASSES_SUB1:
		{
			switch (botVoteMenuData[plyrNum].inputForOptNum)
			{
				case 1: botVoteMenuData[plyrNum].botClassPercentage[teamSel][0] = botVoteMenuData[plyrNum*3].botClassPercentage[teamSel][0] = voteBotInputLimits(input, 0, 100); break;
				case 2: botVoteMenuData[plyrNum].botClassPercentage[teamSel][1] = botVoteMenuData[plyrNum*3].botClassPercentage[teamSel][1] = voteBotInputLimits(input, 0, 100); break;
				case 3: botVoteMenuData[plyrNum].botClassPercentage[teamSel][2] = botVoteMenuData[plyrNum*3].botClassPercentage[teamSel][2] = voteBotInputLimits(input, 0, 100); break;
				case 4: botVoteMenuData[plyrNum].botClassPercentage[teamSel][3] = botVoteMenuData[plyrNum*3].botClassPercentage[teamSel][3] = voteBotInputLimits(input, 0, 100); break;
				case 5: botVoteMenuData[plyrNum].botClassPercentage[teamSel][4] = botVoteMenuData[plyrNum*3].botClassPercentage[teamSel][4] = voteBotInputLimits(input, 0, 100); break;
			}
			break;
		}
		case PR_VOTEBOT_MENU_BOTCLASSES_SUB2:
		{
			switch (botVoteMenuData[plyrNum].inputForOptNum)
			{
				case 1: botVoteMenuData[plyrNum].botClassPercentage[teamSel][5] = botVoteMenuData[plyrNum*3].botClassPercentage[teamSel][5] = voteBotInputLimits(input, 0, 100); break;
				case 2: botVoteMenuData[plyrNum].botClassPercentage[teamSel][6] = botVoteMenuData[plyrNum*3].botClassPercentage[teamSel][6] = voteBotInputLimits(input, 0, 100); break;
				case 3: botVoteMenuData[plyrNum].botClassPercentage[teamSel][7] = botVoteMenuData[plyrNum*3].botClassPercentage[teamSel][7] = voteBotInputLimits(input, 0, 100); break;
				case 4: botVoteMenuData[plyrNum].botClassPercentage[teamSel][8] = botVoteMenuData[plyrNum*3].botClassPercentage[teamSel][8] = voteBotInputLimits(input, 0, 100); break;
				case 5: botVoteMenuData[plyrNum].botClassPercentage[teamSel][9] = botVoteMenuData[plyrNum*3].botClassPercentage[teamSel][9] = voteBotInputLimits(input, 0, 100); break;
			}
			break;
		}
		case PR_VOTEBOT_MENU_BOTBEHAVIOR_SUB1:
		{
			switch (botVoteMenuData[plyrNum].inputForOptNum)
			{
				case 1: botVoteMenuData[plyrNum].botDefensePercent[teamSel] = botVoteMenuData[plyrNum*3].botDefensePercent[teamSel] = voteBotInputLimits(input, 0, 100); break;
				case 2: botVoteMenuData[plyrNum].botOffensePercent[teamSel] = botVoteMenuData[plyrNum*3].botOffensePercent[teamSel] = voteBotInputLimits(input, 0, 100); break;
				case 3: botVoteMenuData[plyrNum].botNeitherPercent[teamSel] = botVoteMenuData[plyrNum*3].botNeitherPercent[teamSel] = voteBotInputLimits(input, 0, 100); break;
				case 4: botVoteMenuData[plyrNum].botRandomPercent[teamSel]  = botVoteMenuData[plyrNum*3].botRandomPercent[teamSel]  = voteBotInputLimits(input, 0, 100); break;
			}
			break;
		}
	}
	// TODO DONE: Somewhere at the bottom here, put the code that autocomputes percentages. So, if someone enters 100% for one team,
	//       it automatically makes all other existing teams 0%.
	voteBotAutoComplete(player);

	CuTFMenuSound(PR_MENUSOUND_BUY);
	botVoteMenuData[plyrNum].gettingNumInput = false;
	// immediately update the menu display
	entity oldSelf = self;  self = player;
	Menu_VoteBot();         self = oldSelf;
}

int voteBotInputLimits(int input, int minValue, int maxValue)
{
	if (input == 255)     return input;     // 255 is the 'unset' value, which makes the server use the default setting
	if (input < minValue) return minValue;
	if (input > maxValue) return maxValue;
	return input;
}

// This is used when entering percentages. So, if you choose, say, 100% for team 1, all other teams get 0%,
// automatically.
void voteBotAutoComplete(entity player)
{
	int plyrNum = ENT_TO_NUM(player);
	int teamSel = botVoteMenuData[plyrNum].menuTeamSelected;
	int newValIdx;
	switch (player->goal_state)
	{
		case PR_VOTEBOT_MENU_BOTCOUNT:
		{
			switch (botVoteMenuData[plyrNum].inputForOptNum)
			{
				case 1: // ideal number of players/bots
					break;
				case 2: case 3: case 4: case 5:  // percent of bots on teams 1 to 4
					newValIdx = botVoteMenuData[plyrNum].inputForOptNum - 2;
					voteBotAutoPercent(botVoteMenuData[plyrNum].percentPlyrsPerTeam, botVoteMenuData[plyrNum*3].percentPlyrsPerTeam, number_of_teams, newValIdx);
					break;
				case 6: // force humans to team
					break;
			}
			break;
		}
		case PR_VOTEBOT_MENU_BOTSKILL_SUB1:
		{
			switch (botVoteMenuData[plyrNum].inputForOptNum)
			{
				case 1: // minimum skill level
					if (botVoteMenuData[plyrNum].botMinSkillLevel[teamSel] > botVoteMenuData[plyrNum].botMaxSkillLevel[teamSel])
						botVoteMenuData[plyrNum].botMaxSkillLevel[teamSel] = botVoteMenuData[plyrNum].botMinSkillLevel[teamSel];
					break;
				case 2: // maximum skill level
					if (botVoteMenuData[plyrNum].botMaxSkillLevel[teamSel] < botVoteMenuData[plyrNum].botMinSkillLevel[teamSel])
						botVoteMenuData[plyrNum].botMinSkillLevel[teamSel] = botVoteMenuData[plyrNum].botMaxSkillLevel[teamSel];
					break;
				case 3: // locked skill level
					break;
			}
			break;
		}
		case PR_VOTEBOT_MENU_BOTCLASSES_SUB1:
		{
			switch (botVoteMenuData[plyrNum].inputForOptNum)
			{
				case 1: case 2: case 3: case 4: case 5: // bot classes (page 1)
					newValIdx = botVoteMenuData[plyrNum].inputForOptNum - 1;
					voteBotAutoPercent(botVoteMenuData[plyrNum].botClassPercentage[teamSel], botVoteMenuData[plyrNum*3].botClassPercentage[teamSel], 10, newValIdx);
					break;
			}
			break;
		}
		case PR_VOTEBOT_MENU_BOTCLASSES_SUB2:
		{
			switch (botVoteMenuData[plyrNum].inputForOptNum)
			{
				case 1: case 2: case 3: case 4: case 5: // bot classes (page 2)
					newValIdx = botVoteMenuData[plyrNum].inputForOptNum - 1 + 5;
					voteBotAutoPercent(botVoteMenuData[plyrNum].botClassPercentage[teamSel], botVoteMenuData[plyrNum*3].botClassPercentage[teamSel], 10, newValIdx);
					break;
			}
			break;
		}
		case PR_VOTEBOT_MENU_BOTBEHAVIOR_SUB1:
		{
			switch (botVoteMenuData[plyrNum].inputForOptNum)
			{
				case 1: case 2: case 3: case 4: // bot behavior percentages
					newValIdx = botVoteMenuData[plyrNum].inputForOptNum - 1;
					// Oops. This doesn't fit the mold of all values being in an array, and I don't feel like having to modify all of the other code to make it fit.
					unsigned char values[8];
					values[0] = botVoteMenuData[plyrNum].botDefensePercent[teamSel]; values[4] = botVoteMenuData[plyrNum*3].botDefensePercent[teamSel];
					values[1] = botVoteMenuData[plyrNum].botOffensePercent[teamSel]; values[5] = botVoteMenuData[plyrNum*3].botOffensePercent[teamSel];
					values[2] = botVoteMenuData[plyrNum].botNeitherPercent[teamSel]; values[6] = botVoteMenuData[plyrNum*3].botNeitherPercent[teamSel];
					values[3] = botVoteMenuData[plyrNum].botRandomPercent[teamSel];  values[7] = botVoteMenuData[plyrNum*3].botRandomPercent[teamSel];
					voteBotAutoPercent(values, &values[4], 4, newValIdx);
					botVoteMenuData[plyrNum].botDefensePercent[teamSel] = values[0];   // the entry flags ([plyrNum*3]) will not be changed
					botVoteMenuData[plyrNum].botOffensePercent[teamSel] = values[1];
					botVoteMenuData[plyrNum].botNeitherPercent[teamSel] = values[2];
					botVoteMenuData[plyrNum].botRandomPercent[teamSel]  = values[3];
					break;
			}
			break;
		}
	}
}

// This attempts to intelligently total up percent values for the user, when they enter percentage values, so that values equal 100%.
// `data` is the array to be modified. `playerEntries` is the array marking values that were entered by the user, so that we can try to preserve those values.
// `length` is the length of the arrays. `newValIdx` is the index of the most recent player entry.
// ### TODO ###: Make sure player entry flags are reset when they should be.
// ### TODO ###: Percentage stuff locked up at one point when I entered 255. (stuck in loop in this function)
// ### TODO ###: Also, starting a votebot vote on a really large list of changes, caused server to crash.
void voteBotAutoPercent(unsigned char* data, unsigned char* playerEntries, int length, int newValIdx)
{
	start:
	// [determine points remaining to be distributed]
	int totalEnteredSoFar = 0;
	for (int i = 0; i < length; i++) if (playerEntries[i] != 255 && i != newValIdx) totalEnteredSoFar += data[i];
	int remainingPoints = 100 - totalEnteredSoFar - data[newValIdx];
	// if the player enters a value that adds up to more than 100, when added with his/her previous entries...
	if (remainingPoints < 0)
	{
		// invalidate all previous entries and start over, keeping the latest entry
		// (Tried to make this a single goto at the bottom of the function, like a function within a function,
		// but got a stupid "jump crosses variable initialization" error.)
		for (int i = 0; i < length; i++) if (i != newValIdx) data[i] = playerEntries[i] = 255;
		goto start;
	}
	// [distribute remaining points]
	// determine how many to distribute over
	int slotsRemaining = 0;
	for (int i = 0; i < length; i++) if (playerEntries[i] == 255) slotsRemaining++;
	// if there is no slot un-entered by the player to put remaining value in...
	if (slotsRemaining < 1)
	{
		if (remainingPoints > 0)
		{
			// invalidate all previous entries and start over, keeping the latest entry
			for (int i = 0; i < length; i++) if (i != newValIdx) data[i] = playerEntries[i] = 255;
			goto start;
		}
		else return;
	}
	// distribute over the ones that haven't already been entered by player
	int share = remainingPoints / slotsRemaining; // TODO DONE?: what if slotsRemaining is 0?
	remainingPoints -= (share * slotsRemaining);
	for (int i = 0; i < length; i++) if (playerEntries[i] == 255) data[i] = share;
	for (int i = 0;  remainingPoints > 0 && i < length;  i++)
	{
		if (playerEntries[i] == 255) { data[i]++;  remainingPoints--; }
	}
}

void Player_VoteBot()
{
	if (self->playerclass == PR_PC_UNDEFINED && self->classname == "player") // Observers
		return;

	if (current_voteent != world)
	{
		sprint(self, PR_PRINT_HIGH, "Already running a vote!\n");
		return;
	}

	if (time < PR_VOTE_MIN_TIME)
#ifdef PR_COOP_MODE_ENHANCED
	if (deathmatch || mapname != "start")
#endif
	{
		sprint(self, PR_PRINT_HIGH, "Wait a moment for all players to enter the game.\n");
		return;
	}

	if (self->goal_no > time)
	{
		sprint(self, PR_PRINT_HIGH, "You just ran a vote!\n");
		return;
	}

	if (self->classname == "player")
		self->current_menu = PR_MENU_VOTEBOT;
	else if (self->classname == "spec")
		self->current_menu = PR_SPEC_MENU_VOTEBOT;
	else
		return;

	self->group_no = PR_VOTE_BOT;
	self->goal_state = PR_VOTEBOT_MENU_MAIN;

	// reset the player's votebot menu data
	copyVoteBotData(CURRENT, ENT_TO_NUM(self), PR_VOTEBOT_MENU_MAIN, true);

	CuTFMenuSound(PR_MENUSOUND_BUY);
	Menu_VoteBot();
	BackFlash(self, 1);
}

// Start vote on bot settings. `self` : player who initiated the vote
void StartVoteBot()
{
	if (self->classname != "player" && self->classname != "spec")
	{
		sprint(self, PR_PRINT_HIGH, "You can't vote. Join the game, first.\n");
		return;
	}
	if (mutedtime(self))
	{
		sprint(self, PR_PRINT_HIGH, "You can't start a vote while muted!\n");
		return;
	}

	BackFlash(self, 1);

	entity voteent;
	voteent = spawnServerSide(); // make it a server-side only entity
	voteent->think = VoteMap_Think;
	voteent->nextthink = time + PR_VOTE_TIME;
	voteent->classname = "voteent";
	voteent->owner = self;       // PZ: was = world

	CleanClientVotes();      // resets all clients to ready-to-vote state

	voteent->all_active = 1; // number of yes votes
	voteent->has_holo = 0;   // number of no votes

	voteent->has_sentry = PR_VOTE_BOT;

	current_voteent = voteent;

	self->goal_no = time + PR_VOTE_DELAY;  // how soon player can initiate vote again
	self->owned_by = PR_VOTED_YES;         // count their vote as a yes

	voteent->dont_do_triggerwork = PR_FALSE; // don't think I need this, but leaving just in case

	BroadcastSound("misc/medkey");

	last_vote_starter = self->netname;
	bprint(PR_PRINT_HIGH, "Voting\x8D ", self->netname);
	bprint(PR_PRINT_HIGH, " requests the following changes to the bot settings: ");
	voteBotTellRequestedChanges();
	bprint(PR_PRINT_HIGH, S_(". Type ^b\"^bVoteYes^b\"^b or ^b\"^bVoteNo^b\"^b to vote."));

	// Gizmo
	bprintVotesNeeded(1, 0);
	bprint(PR_PRINT_HIGH, "\n");

	ResetClientsMenu();    // clear menus on players/specs which are currently in a vote menu
	CheckVoting(PR_FALSE); // PZ: Count up the current votes and see if it passes. (for when there is only one player)
}

// PZ: For the votebot menu.
void voteBotTellRequestedChanges()
{
	int voteStarter = ENT_TO_NUM(current_voteent->owner) + MAX_CLIENTS; // we want the index to their committed data
	string output = ""; // I am putting output into a string first, so that I can remove ending comma.
	std::vector<string> teamStrs = {"Overall", "Team 1", "Team 2", "Team 3", "Team 4"};
	std::vector<string> classStrs = {"Scout", "Sniper", "Soldier", "Demoman", "Medic", "Hwguy", "Pyro", "Spy", "Engineer", "Random"};

	// Player/Bot Count
	if (shouldWeTellChange(botVoteMenuData[voteStarter].idealNumPlayers, botVoteMenuData[CURRENT].idealNumPlayers))
		output += "Overall Player/Bot Count: " + to_string(botVoteMenuData[voteStarter].idealNumPlayers) + ", ";
	for (int i = 0; i < 4; i++)
	if (shouldWeTellChange(botVoteMenuData[voteStarter].percentPlyrsPerTeam[i], botVoteMenuData[CURRENT].percentPlyrsPerTeam[i], true))
		output += "Players/Bots On Team " + to_string(i+1) + ": " + to_string(botVoteMenuData[voteStarter].percentPlyrsPerTeam[i]) + "%, ";
	if (shouldWeTellChange(botVoteMenuData[voteStarter].forceHumansToTeam, botVoteMenuData[CURRENT].forceHumansToTeam))
		output += "Force Humans To Team: " + to_string(botVoteMenuData[voteStarter].forceHumansToTeam) + ", ";
	// Bot Skill Level
	for (int i = 0; i < 5; i++)
	{
		if (shouldWeTellChange(botVoteMenuData[voteStarter].botMinSkillLevel[i], botVoteMenuData[CURRENT].botMinSkillLevel[i]))
			output += teamStrs[i] + " Minimum Skill Level: " + to_string(botVoteMenuData[voteStarter].botMinSkillLevel[i]) + ", ";
		if (shouldWeTellChange(botVoteMenuData[voteStarter].botMaxSkillLevel[i], botVoteMenuData[CURRENT].botMaxSkillLevel[i]))
			output += teamStrs[i] + " Maximum Skill Level: " + to_string(botVoteMenuData[voteStarter].botMaxSkillLevel[i]) + ", ";
		if (shouldWeTellChange(botVoteMenuData[voteStarter].botLockedSkillLevel[i], botVoteMenuData[CURRENT].botLockedSkillLevel[i]))
			output += teamStrs[i] + " Locked Skill Level: " + to_string(botVoteMenuData[voteStarter].botLockedSkillLevel[i]) + ", ";
	}
	// Bot Classes
	for (int i = 0; i < 5; i++) // Example: Overall Class Sniper: 100%
	{
		for (int j = 0; j < 10; j++)
			if (shouldWeTellChange(botVoteMenuData[voteStarter].botClassPercentage[i][j], botVoteMenuData[CURRENT].botClassPercentage[i][j], true))
				output += teamStrs[i] + " Class " + classStrs[j] + ": " + to_string(botVoteMenuData[voteStarter].botClassPercentage[i][j]) + "%, ";
	}
	// Bot Behavior
	for (int i = 0; i < 5; i++)
	{
		if (shouldWeTellChange(botVoteMenuData[voteStarter].botDefensePercent[i], botVoteMenuData[CURRENT].botDefensePercent[i], true))
			output += teamStrs[i] + " Bots On Defense: " + to_string(botVoteMenuData[voteStarter].botDefensePercent[i]) + "%, ";
		if (shouldWeTellChange(botVoteMenuData[voteStarter].botOffensePercent[i], botVoteMenuData[CURRENT].botOffensePercent[i], true))
			output += teamStrs[i] + " Bots On Offense: " + to_string(botVoteMenuData[voteStarter].botOffensePercent[i]) + "%, ";
		if (shouldWeTellChange(botVoteMenuData[voteStarter].botNeitherPercent[i], botVoteMenuData[CURRENT].botNeitherPercent[i], true))
			output += teamStrs[i] + " Bots On Neither Off/Def: " + to_string(botVoteMenuData[voteStarter].botNeitherPercent[i]) + "%, ";
		if (shouldWeTellChange(botVoteMenuData[voteStarter].botRandomPercent[i],  botVoteMenuData[CURRENT].botRandomPercent[i], true))
			output += teamStrs[i] + " Bots On Random Off/Def: " + to_string(botVoteMenuData[voteStarter].botRandomPercent[i]) + "%, ";
	}

	// replace 255 values with "def"
	replaceStr(output, "255", "def");

	// remove ending ", "
	if (output.length() >= 2)
	{
		if (output.substr(output.length()-2) == ", ")
			output.erase(output.length()-2);
	}
	bprint(PR_PRINT_HIGH, output);
}

// helper function
bool shouldWeTellChange(unsigned char newValue, unsigned char oldValue, bool isPercentage /*= false*/)
{
	if (newValue == oldValue) return false;

	if (isPercentage)
	{
		if (newValue == 0) return false;
	}

	return true;
}

// =======================
// END VOTEBOT FUNCTIONS
// =======================

} // END namespace Progs
