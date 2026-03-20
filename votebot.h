#ifndef VOTE2_H
#define VOTE2_H

namespace Progs {

// PZ: Submenus for the "votebot" menu.
const int PR_VOTEBOT_MENU_MAIN             = 0;
const int PR_VOTEBOT_MENU_BOTCOUNT         = 10;
const int PR_VOTEBOT_MENU_BOTSKILL         = 20;
const int PR_VOTEBOT_MENU_BOTSKILL_SUB1    = 21;
const int PR_VOTEBOT_MENU_BOTCLASSES       = 30;
const int PR_VOTEBOT_MENU_BOTCLASSES_SUB1  = 31;
const int PR_VOTEBOT_MENU_BOTCLASSES_SUB2  = 32;
const int PR_VOTEBOT_MENU_BOTBEHAVIOR      = 40;
const int PR_VOTEBOT_MENU_BOTBEHAVIOR_SUB1 = 41;

class VoteBot
{
  private:

	struct VoteBotMenuData
	{
		// [Data for the vote options (Values of 255 indicate no value entered.)]
		// Player/Bot Count                         [for 4 teams]
		unsigned char          idealNumPlayers, percentPlyrsPerTeam[4], forceHumansToTeam;
		// Bot Skill Level                     [for overall, and all 4 teams]
		unsigned char          botMinSkillLevel[5], botMaxSkillLevel[5], botLockedSkillLevel[5];
		// Bot Classes         [for overall, and all 4 teams] [for all 9 classes, and random class]
		unsigned char          botClassPercentage[5][10];
		// Bot Behavior                                  [for overall, and all 4 teams]
		unsigned char          botDefensePercent[5], botOffensePercent[5], botNeitherPercent[5], botRandomPercent[5];

		// [Data for menu and vote inner-working operation]
		// menu navigation
		unsigned char          menuTeamSelected;    // 0 is Overall, 1 is Team 1, 2 is Team 2, etc.
		// numerical entry
		bool                   gettingNumInput;     // is player typing a number for input?
		unsigned char          inputForOptNum;      // the option the player is currently inputing for
		int                    inputNum;            // the number being input

		// Constructor
		VoteBotMenuData()
		{
			idealNumPlayers = forceHumansToTeam = 255;
			for (int i = 0; i < 4; i++) percentPlyrsPerTeam[i] = 255;
			for (int i = 0; i < 5; i++)
			{
				botMinSkillLevel[i] = botMaxSkillLevel[i] = botLockedSkillLevel[i] = 255;
				for (int j = 0; j < 10; j++) botClassPercentage[i][j] = 255;
				botDefensePercent[i] = botOffensePercent[i] = botNeitherPercent[i] = botRandomPercent[i] = 255;
			}
		}
	};

	// PZ: This array holds the data for the bot vote menu.
	// Index 0 is the default data (normal settings).
	// Index 1 to 32 (or MAX_CLIENTS) are for each player's current uncommitted values while in the menu.
	// Index 33 to 64 (depending on MAX_CLIENTS) are for each player's committed (okay'd) values while in the menu.
	// Index 65 to 96 (depending on MAX_CLIENTS) are flags that indicate whether the user entered the value, themselves.
	// Index 97 (depending on MAX_CLIENTS) holds the current data in use in the game (what has passed a `votebot` vote).
	static const unsigned int VOTEBOTDATALEN = MAX_CLIENTS * 3 + 2; // I tried to make this a const variable, but got errors saying it wasn't a constant when I tried to use it.
	VoteBotMenuData voteBotMenuData[VOTEBOTDATALEN];

	// Functions
	void voteBotAutoComplete(entity player);
	void voteBotAutoPercent(unsigned char* data, unsigned char* humanEntries, int length, int newValIdx);
	int  voteBotInputLimits(int input, int minValue, int maxValue);
	void StartVoteBot();
	void voteBotTellRequestedChanges();
	bool shouldWeTellChange(unsigned char newValue, unsigned char oldValue, bool isPercentage = false);
	string getVoteBotTeamStr(int ndx);

  public:

	enum Settings {DEFAULT = 0, CURRENT = VOTEBOTDATALEN - 1};

	void initVoteBotMenuData();
	void Player_VoteBot();
	void Menu_VoteBot();
	void Menu_VoteBot_Input(float input);
	bool isGettingNumInput(entity player)
	{
		int ndx = ENT_TO_NUM(player);
		if (ndx >= 1 && ndx <= MAX_CLIENTS)
			return voteBotMenuData[ndx].gettingNumInput;
		else
		{
			dprint(string("VoteBot::isGettingNumInput(): Error: Was passed a player entity (with classname \"") + player->classname +
			       "\") that resulted in an out-of-bound `voteBotMenuData[]` index (" + to_string(ndx) + ")\n");
			return false;
		}
	}
	void voteBotValueEntryDone(entity player);

	unsigned char getIdealNumPlayers(int settingsIndex)
	{
		if (settingsIndex >= 0 && static_cast<unsigned int>(settingsIndex) < VOTEBOTDATALEN)
			return voteBotMenuData[settingsIndex].idealNumPlayers;
		else
		{
			dprint(string("VoteBot::getIdealNumPlayers(): Error: Was passed a settings index (") + to_string(settingsIndex) +
			       ") that resulted in an out-of-bound `voteBotMenuData[]` index\n");
			return 255;
		}
	}
	unsigned char getBotMinSkillLevel(int settingsIndex, int team)
	{
		if (settingsIndex >= 0 && static_cast<unsigned int>(settingsIndex) < VOTEBOTDATALEN && team >= 0 && team < 5)
			return voteBotMenuData[settingsIndex].botMinSkillLevel[team];
		else
		{
			dprint(string("VoteBot::getBotMinSkillLevel(): Error: Was passed a settings index (") + to_string(settingsIndex) +
			       ") or team index (" + to_string(team) + ") that was out-of-bounds\n");
			return 255;
		}
	}
	unsigned char getBotMaxSkillLevel(int settingsIndex, int team)
	{
		if (settingsIndex >= 0 && static_cast<unsigned int>(settingsIndex) < VOTEBOTDATALEN && team >= 0 && team < 5)
			return voteBotMenuData[settingsIndex].botMaxSkillLevel[team];
		else
		{
			dprint(string("VoteBot::getBotMaxSkillLevel(): Error: Was passed a settings index (") + to_string(settingsIndex) +
			       ") or team index (" + to_string(team) + ") that was out-of-bounds\n");
			return 255;
		}
	}
	unsigned char getBotLockedSkillLevel(int settingsIndex, int team)
	{
		if (settingsIndex >= 0 && static_cast<unsigned int>(settingsIndex) < VOTEBOTDATALEN && team >= 0 && team < 5)
			return voteBotMenuData[settingsIndex].botLockedSkillLevel[team];
		else
		{
			dprint(string("VoteBot::getBotLockedSkillLevel(): Error: Was passed a settings index (") + to_string(settingsIndex) +
			       ") or team index (" + to_string(team) + ") that was out-of-bounds\n");
			return 255;
		}
	}
	unsigned char getForceHumansToTeam(int settingsIndex)
	{
		if (settingsIndex >= 0 && static_cast<unsigned int>(settingsIndex) < VOTEBOTDATALEN)
			return voteBotMenuData[settingsIndex].forceHumansToTeam;
		else
		{
			dprint(string("VoteBot::getForceHumansToTeam(): Error: Was passed a settings index (") + to_string(settingsIndex) +
			       ") that resulted in an out-of-bound `voteBotMenuData[]` index\n");
			return 255;
		}
	}
	unsigned char getPercentPlyrsPerTeam(int settingsIndex, int team)
	{
		if (settingsIndex >= 0 && static_cast<unsigned int>(settingsIndex) < VOTEBOTDATALEN && team >= 0 && team < 4)
			return voteBotMenuData[settingsIndex].percentPlyrsPerTeam[team];
		else
		{
			dprint(string("VoteBot::getPercentPlyrsPerTeam(): Error: Was passed a settings index (") + to_string(settingsIndex) +
			       ") or team index (" + to_string(team) + ") that was out-of-bounds\n");
			return 255;
		}
	}
	unsigned char getBotClassPercentage(int settingsIndex, int team, int playerClass)
	{
		if (settingsIndex >= 0 && static_cast<unsigned int>(settingsIndex) < VOTEBOTDATALEN &&
		    team >= 0 && team < 5 && playerClass >= 0 && playerClass < 10)
			return voteBotMenuData[settingsIndex].botClassPercentage[team][playerClass];
		else
		{
			dprint(string("VoteBot::getBotClassPercentage(): Error: Was passed a settings index (") + to_string(settingsIndex) +
			       ") or team index (" + to_string(team) + ") or class index (" + to_string(playerClass) + ") that was out-of-bounds\n");
			return 255;
		}
	}

	void copyVoteBotData(int from, int to, int menu, bool alsoCopyToCommittedData = false); // I'll let this be public for now. Be careful with it.
};

extern VoteBot voteBot;

} // END namespace Progs

#endif // END VOTE2_H
