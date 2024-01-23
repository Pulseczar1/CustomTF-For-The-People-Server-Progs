/*=======================================================//
// prozac.QC - CustomTF 3.2.OfN           - 15/11/2004 - //
// by Sergio Fuma�a Grunwaldt - OfteN [cp]               //
=========================================================//
 ProZac Message Of The Day (MOTD) and un/related stuff :)
=========================================================*/

#include "progs.h"
#include "prozac.h"
#include "ofndefs.h"
#include "agr.h"
#include "invade.h"
#include "neo.h"

namespace Progs {

#define PR_MSG_OFTEN_CUTFVERSION S_("Running Coop progs version 1.0.9.0 ^b[^b1^b/^b30^b/^b2009^b]^b (the baseline/trunk of CuTF For The People)\n")

#define PR_MSG_INFO S_("Type ^b\"^bcmd help^b\"^b to see additional client commands.\n\n")

//#define MSG_CUTFVERSION "Custom�� version �3�2�OfN"
//WK #define MSG_CUTFVERSION   "������ Custom�� �����OfN�" //<-- 3.2.OfN6

//#ifdef COOP_MODE_ENHANCED
//#define MSG_CUTFVERSION		"������ Custom�� COOP VERSION 1.0.9.0 (1-30-09)"
//#else
#define PR_MSG_CUTFVERSION		S_("CustomTF ^bFor The People^b")
//#endif

// PZ: WARNING: Pre-processor doesn't like "//" in defines. It seems to think they are comments. Feel free to fix it. :P
//     EditPlus doesn't display some "high" characters. The yellow '[' before 'facebook.com' had to be hex-edited in. EditPlus won't display anything for it.
//     ## Use the QCCX shortcuts, like \b, \0 -> \9, etc. See its manual.txt.
// Welcome to CustomTF For The People (4tp)\n .... Please give your input at:\n\nfacebook.com/groups/quaketf\n\n             (��������) (�������������������������)
//#define PR_MSG_INTRO  S_("Welcome to\nCu^bstom^bTF ^bFor The People^b (4tp)\n\nwhere the PLAYERS decide\nhow the game is played!\n\nPlease give your input at:\n\n\x90 facebook^b.^bcom^b/^bgroups^b/^bquaketf \x91\n\n")
// Now using Discord instead of Facebook.
#define PR_MSG_INTRO  S_("Welcome to\nCu^bstom^bTF ^bFor The People^b (4tp)\n\nwhere the PLAYERS decide\nhow the game is played!\n\nPlease give your input at:\n\n\x90 https^b://^bdiscord^b.^bgg^b/^bfp9sSZ5 \x91\n\n")
#define PR_MSG_INTRO2 S_("Based on The Gizmo's Coop mod\nwhich is based on Prozac CuTF Classic\nBots are ^bheavily^b modified FrikBots\nby Ryan \"Frika C\" Smith\n\nWith special thanks to...\n")
#define PR_MSG_INTRO3 "Shaka, for the original CustomTF\nand Often, for Prozac CustomTF\n\nThis server is a work in progress...\n\n"                                                // had "Hosted by no one, yet" at the end of this
#define PR_MSG_INTRO4 S_("\nPress '^0' to skip")
//http:��kickme�to�customtf"

// PZ: This is now page 1 of the intro. I made it so that there are multiple pages.
void PrintProzacMOTD()
{
	const float CURRENT_YEAR = 2024; // PZ: now we just have to update this date :) (FIXME: get the current year from a standard library)
    string msgIntro, st, st2, st3, updates;
    float x;

	// PZ: I'm making this easier to read, rather than using the defines above.
	// PZ NOTE: the standard client has a max width of 40 characters for CenterPrint(), and 1024 total characters
	msgIntro = S_("Welcome to\n"
	           "Cu^bstom^bTF ^bFor The People^b (4tp)\n\n"
	           "where the PLAYERS decide\n"
	           "how the game is played!\n\n"
	           "Please give your input at:\n\n"
	           "\x90 https^b://^bdiscord^b.^bgg^b/^bfp9sSZ5 \x91\n\n"  // Was: "\x90 facebook^b.^bcom^b/^bgroups^b/^bquaketf \x91\n\n"
	           "Based on The Gizmo's Coop mod\n"
	           "which is based on Prozac CuTF Classic\n\n"
	           //"Bots are \bheavily\b modified FrikBots\n"
	           //"by Ryan \"Frika C\" Smith\n\n"
	           "With special thanks to...\n"
	           "Shaka, for the original CustomTF\n"
	           "Often, for Prozac CustomTF\n"
	           "and FrikaC, for the FrikBot, which was\n"
	           "an awesome guide for the creation of\n"
	           "these bots\n\n");

    //st = "Celebrating �� years of CustomTF!!\n\n";
	x = CURRENT_YEAR - 1998; // PZ: according to http://wiki.quakeworld.nu/CustomTF, version 1.0 of CustomTF was released on 6/3/98 (June 3rd?).
	st = "Celebrating ";
	st2 = ftos(x);
	st2 = colstr(st2, PR_COLSTR_NUMBER);
	st = strcat(st, st2);
	st = strcat(st, " years of CustomTF!!\n\n");

    if ((self->PR_cprint_fx & 3) == 3)
        st = colstr(st,PR_COLSTR_MIX1);
    else if (self->PR_cprint_fx & 2)
        st = colstr(st,PR_COLSTR_RED);
    else if (self->PR_cprint_fx & 1)
        st = colstr(st,PR_COLSTR_MIX2);

	// PZ
    st2 = S_("\nPress '^0' to skip\n\n\n");
    x = floor(time);
    if (x - (2 * floor(x / 2)) == 0) // x mod 2 == 0
        st2 = colstr(st2, PR_COLSTR_RED);

//	           "\{141} Complete \boverhaul\b of bot AI is in\n"
//	           "\{141} the works... \bComing soon!\b        \n\n";
/*	updates =  "\{141} \bMay 9, 2015\b : The first version of the\n"
	           "new AI is now live. Updates and improve-\n"
	           "ments should come much faster now.      \n\n"
	           "\{141} \bMay 27, 2015\b : Fixed several problems \n"
	           "with deathmatch maps, mostly dealing    \n"
	           "with the engineer.                      \n\n"
	           "\{141} \bJuly 18, 2015\b : Players should no     \n"
	           "longer get kicked for changing team,    \n"
	           "color, or skin. Also fixed several bugs.\n\n";          */
	/*updates =  "This is a temporary test preview, for\n"
	           "the sole purpose of running the\n"
	           "Halloween map. Do not expect a bugless\n"
	           "experience. Happy Halloween!\n\n";*/
	             // PZ NOTE: For boxing strings at 40 characters in length, try padstr(str, 40). Either, the server,
	             // client, or both will just chop off all characters on a line, after the 40th character.
	             //1234567890123456789012345678901234567890
	updates =  S_("Jan 17, 2024:                           \n"
	              "^bThe default bot count was lowered. These\n"
	              "bots seem more fun when they supplement,\n"
	              "rather than dominate, a game. Use       \n"
	              "`^bvotebot^b` if you prefer more bots.^b      \n\n");

	st3     =  S_("Press '^9' to continue to the next page\n");

/*

A lot of things were changed. This list will only include things that I think players will be interested in.

-Bots now use the exact same physics code that is ran on human players. This makes them more player-like. It also fixes everything that wasn't working dealing with bots touching objects, like being killed by touching a tesla.
-Custom class menu now remembers your previous class, so that you can simply edit your class.
-Bot skill level never goes to 4 until all bots are level 3.
-Greatly improved bots' ablity to go up ladders.
-Limited bubbles and gibs when object count is high.
-Fixed spectator not being able to use vote commands.

*/


    //CenterPrint5(self,#MSG_INTRO,#MSG_INTRO2,#MSG_INTRO3,st,st2);
	CenterPrint5(self, msgIntro, st, st2, updates, st3);

	// PZ: people sometimes complain that their number keys don't work; number keys are crucial to CuTF; so let's make sure they work
	stuffcmd(self, "bind 1 \"impulse 1\"; bind 2 \"impulse 2\"; bind 3 \"impulse 3\"; bind 4 \"impulse 4\"; bind 5 \"impulse 5\"; bind 6 \"impulse 6\"; bind 7 \"impulse 7\"; bind 8 \"impulse 8\"; bind 9 \"impulse 9\"; bind 0 \"impulse 10\"\n");

    self->PR_cprint_fx = self->PR_cprint_fx + 1;
}

void PrintProzacMOTD_Page2()
{                                //1234567890123456789012345678901234567890
	string heading =           S_("^8: Prev Page    ^9: Next Page    ^0: Skip \n\n");
	centerprint(self, heading, S_("Dec 8, 2022:                            \n"
	                              "^bBots now default to skill level 3 and   \n"
	                              "more quickly adjust their skill level   \n"
	                              "to the skill of the human players.^b      \n"
	                              "\n"
	                              "October 2016:                           \n"
	                              "^bThis server is finally running entirely \n"
	                              "in C++ (no QuakeC). Expect glitches.    \n"
	                              "Please report glitches to -^bPulse^bczar^b-^b.  \n"
	                              "\n"
	                              "\n"
	                              "\n"
	                              "\n"
	                              "\n"
	                              "\n"
	                              "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"));
}

void PrintProzacMOTD_Page3()
{
	CenterPrint(self, S_("^8: Prev Page    ^9: Next Page    ^0: Skip\n\n\n\n\n\n\n\n\n\n\n\nPage 3\n\n\n\n\n\n\n\n\n\n\n\n\n"));
}

void PrintProzacMOTD_Page4()
{

}

void PrintProzacMOTD_Page5()
{

}

void PrintProzacIntro()
{
	string temp, temp2;
	//sprint (self, #PRINT_HIGH, #MSG_INFO, "\n",#MSG_OFTEN_CUTFVERSION, "\n\{157}\{158}\{158}\{159} ", #MSG_CUTFVERSION, " \{157}\{158}\{158}\{158}\{159}\n       ����������� ��������:\n     www�telefragged�com�shaka\n\{157}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{159}\n\n");
	//sprint (self, #PRINT_HIGH, #MSG_INFO, "\n",#MSG_OFTEN_CUTFVERSION, "\n\{157}\{158}\{158}\{159} ", #MSG_CUTFVERSION, " \{157}\{158}\{158}\{159}\n   Please give your input at:\n   facebook�com�groups�quaketf\n\{157}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{159}\n");
	//sprint(self, PR_PRINT_HIGH, PR_MSG_OFTEN_CUTFVERSION, "\n\x9D\x9E\x9E\x9F ", PR_MSG_CUTFVERSION, S_(" \x9D\x9E\x9E\x9F\n   Please give your input at:\n   facebook^b.^bcom^b/^bgroups^b/^bquaketf\n\x9D\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9F\n"));
	// Was using the previous one for Facebook. Now using Discord.
	sprint(self, PR_PRINT_HIGH, PR_MSG_OFTEN_CUTFVERSION, "\n\x9D\x9E\x9E\x9F ", PR_MSG_CUTFVERSION, S_(" \x9D\x9E\x9E\x9F\n   Please give your input at:\n   https^b://^bdiscord^b.^bgg^b/^bfp9sSZ5\n\x9D\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9F\n"));

	// PZ: people need to see all the commands in one place
	/* PZ: made it so that commands show up in the class help you get when you spawn or press "9"
	sprint(self, #PRINT_HIGH, "\nCommands:\n");
	temp2 = colstr("scout", #COLSTR_RED);    temp = strcat(temp, temp2); temp = strcat(temp, ", ");
	temp2 = colstr("sniper", #COLSTR_RED);   temp = strcat(temp, temp2); temp = strcat(temp, ", ");
	temp2 = colstr("soldier", #COLSTR_RED);  temp = strcat(temp, temp2); temp = strcat(temp, ", ");
	temp2 = colstr("demoman", #COLSTR_RED);  temp = strcat(temp, temp2); temp = strcat(temp, ", ");
	temp2 = colstr("medic", #COLSTR_RED);    temp = strcat(temp, temp2); temp = strcat(temp, ", ");
	temp2 = colstr("hwguy", #COLSTR_RED);    temp = strcat(temp, temp2); temp = strcat(temp, ", ");
	temp2 = colstr("pyro", #COLSTR_RED);     temp = strcat(temp, temp2); temp = strcat(temp, ", ");
	temp2 = colstr("spy", #COLSTR_RED);      temp = strcat(temp, temp2); temp = strcat(temp, ", ");
	temp2 = colstr("engineer", #COLSTR_RED); temp = strcat(temp, temp2);
	sprint(self, #PRINT_HIGH, temp, " \{141} change your class to a Team Fortress stock class\n");
	temp = colstr("custom", #COLSTR_RED);
	sprint(self, #PRINT_HIGH, temp, " \{141} customize your class\n");
	temp = colstr("votemap", #COLSTR_RED);
	sprint(self, #PRINT_HIGH, temp, " \{141} start a vote to play a map\n");
	temp = colstr("punish", #COLSTR_RED);
	sprint(self, #PRINT_HIGH, temp, " \{141} start a vote to punish a player\n");
	temp = colstr("reconnect", #COLSTR_RED);
	sprint(self, #PRINT_HIGH, temp, " \{141} the only way to switch your team, currently\n");
	sprint(self, #PRINT_HIGH, "\n");*/

	//sprint(self, #PRINT_HIGH, #MSG_INFO); // moved this to the class help, that shows after spawning

	/*sprint(self, #PRINT_HIGH, "\{141} ", temp, " : change your class to a Team Fortress stock class\n");
	temp = colstr("custom", #COLSTR_RED);
	sprint(self, #PRINT_HIGH, "\{141} ", temp, " : customize your class\n");
	temp = colstr("votemap", #COLSTR_RED);
	sprint(self, #PRINT_HIGH, "\{141} ", temp, " : start a vote to play a map\n");
	temp = colstr("punish", #COLSTR_RED);
	sprint(self, #PRINT_HIGH, "\{141} ", temp, " : start a vote to punish a player\n");
	temp = colstr("reconnect", #COLSTR_RED);
	sprint(self, #PRINT_HIGH, "\{141} ", temp, " : the only way to switch your team, currently\n");
	sprint(self, #PRINT_HIGH, "\n");*/

	// PZ: this was the classic message.
	/*if (!no_votemap && !no_punish)
		sprint(self,#PRINT_HIGH,"Voting\{141} You can issue a vote by using the �VoteMap� and �Punish� commands.\n");
	else
	{
		if (!no_punish)
			sprint(self,#PRINT_HIGH,"Voting\{141} You can issue a map vote by using the �VoteMap� command.\n");
		else if (!no_votemap)
			sprint(self,#PRINT_HIGH,"Voting\{141} You can issue a punishment vote by using the �Punish� command.\n");
	}*/

	// Gizmo
#ifdef PR_COOP_MODE_ENHANCED
	if ( coop == 3 && !deathmatch ) {
		string	str;

		str = colstr( "\x9CThis server is in Insane Coop mode\x9C\n", PR_COLSTR_RED );
		sprint( self, PR_PRINT_HIGH, str );
	}
#endif

	// PZ - Attackers Go Red mode
	if (agr) { AGR_introducePlayer(); sprint(self, PR_PRINT_HIGH, "\n"); }

	// PZ - Invade mode
	if (invade) { Invade_introducePlayer(); sprint(self, PR_PRINT_HIGH, "\n"); }
	
	// PZ - Neo mode
	if (neo.isModeActive()) { neo.introducePlayer(); sprint(self, PR_PRINT_HIGH, "\n"); }
}

void Reminder()
{
	bprint(PR_PRINT_HIGH, S_("\x9D\x9E\x9E\x9F CustomTF ^bFor The People^b \x9D\x9E\x9E\x9F\n"));
	bprint(PR_PRINT_HIGH, "This server needs your help to make it the way you want it to be.\n");
	//bprint(PR_PRINT_HIGH, S_("Please give your input at: facebook^b.^bcom^b/^bgroups^b/^bquaketf\n"));
	bprint(PR_PRINT_HIGH, S_("Please give your input at: https^b://^bdiscord^b.^bgg^b/^bfp9sSZ5\n"));
	bprint(PR_PRINT_HIGH, "\x9D\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9F\n");

	self->nextthink = time + PR_REMINDER_RATE;
}

} // END namespace Progs
