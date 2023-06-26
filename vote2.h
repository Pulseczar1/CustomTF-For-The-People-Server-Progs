#ifndef VOTE2_H
#define VOTE2_H

namespace Progs {

// PZ: Submenus for the "votebot" menu.
#define PR_VOTEBOT_MENU_MAIN               0
#define PR_VOTEBOT_MENU_BOTCOUNT          10
#define PR_VOTEBOT_MENU_BOTSKILL          20
#define PR_VOTEBOT_MENU_BOTSKILL_SUB1     21
#define PR_VOTEBOT_MENU_BOTCLASSES        30
#define PR_VOTEBOT_MENU_BOTCLASSES_SUB1   31
#define PR_VOTEBOT_MENU_BOTCLASSES_SUB2   32
#define PR_VOTEBOT_MENU_BOTBEHAVIOR       40
#define PR_VOTEBOT_MENU_BOTBEHAVIOR_SUB1  41

// PZ: This is what really triggered me to want to convert all QuakeC code to a 'real' programming language.
//     This stores the bot vote menu's default values, as well as the currently selected value of each player in the menu.
struct BotVoteMenuData
{
	// [Data for the vote options. Values of 255 indicate no value entered.]
	// player/bot count                         [for 4 teams]
	unsigned char          idealNumPlayers, percentPlyrsPerTeam[4], forceHumansToTeam;
	// bot skill level                     [for overall, and all 4 teams]
	unsigned char          botMinSkillLevel[5], botMaxSkillLevel[5], botLockedSkillLevel[5];
	// bot classes         [for overall, and all 4 teams] [for all 9 classes, and random class]
	unsigned char          botClassPercentage[5][10];
	// bot behavior                                  [for overall, and all 4 teams]
	unsigned char          botDefensePercent[5], botOffensePercent[5], botNeitherPercent[5], botRandomPercent[5];

	// [Data for menu and vote inner-working operation.]
	// menu navigation
	unsigned char          menuTeamSelected;    // 0 is Overall, 1 is Team 1, 2 is Team 2, etc.
	// numerical entry
	bool                   gettingNumInput;     // is player typing a number for input?
	unsigned char          inputForOptNum;      // the option the player is currently inputing for
	int                    inputNum;            // the number being input

	// Constructor
	BotVoteMenuData()
	{
		idealNumPlayers = forceHumansToTeam = 255;
		for (int i = 0; i < 4; i++) percentPlyrsPerTeam[i] = 255;
		for (int i = 0; i < 5; i++) botMinSkillLevel[i] = botMaxSkillLevel[i] = botLockedSkillLevel[i] = 255;
		for (int i = 0; i < 5; i++) for (int j = 0; j < 10; j++) botClassPercentage[i][j] = 255;
		for (int i = 0; i < 5; i++) botDefensePercent[i] = botOffensePercent[i] = botNeitherPercent[i] = botRandomPercent[i] = 255;
	}
};

#define VOTEBOTDATALEN  (MAX_CLIENTS * 3 + 2)            // I tried to make this a const variable, but got errors saying it wasn't a constant when I tried to use it.
extern BotVoteMenuData botVoteMenuData[VOTEBOTDATALEN];
enum {DEFAULTS = 0, CURRENT = VOTEBOTDATALEN - 1};

// Functions
void initVoteBotMenuData();
void copyVoteBotData(int from, int to, int menu, bool alsoCopyToCommittedData = false);
void Player_VoteBot();
void Menu_VoteBot();
void Menu_VoteBot_Input(float input);
void voteBotAutoComplete(entity player);
void voteBotAutoPercent(unsigned char* data, unsigned char* humanEntries, int length, int newValIdx);
void voteBotValueEntryDone(entity player);
int voteBotInputLimits(int input, int minValue, int maxValue);
void StartVoteBot();
void voteBotTellRequestedChanges();
bool shouldWeTellChange(unsigned char newValue, unsigned char oldValue, bool isPercentage = false);

} // END namespace Progs

#endif // END VOTE2_H
