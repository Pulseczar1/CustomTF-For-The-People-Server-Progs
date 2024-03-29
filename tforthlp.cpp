/*======================================================
	TFORTHLP.QC			CustomTeamFortress v3.1

	(c) TeamFortress Software Pty Ltd 	29/2/97
	(c) William Kerney			5/22/00
	parts (c) Craig Hauser
========================================================
Functions handling all the help displaying for TeamFortress.
========================================================*/

#include "progs.h"
#include "ofndefs.h"
#include "prozac.h"
#include "menu.h"
#include "tfort.h"
#include "cpstuff.h"
#include "debug.h"
#include "neo.h"

namespace Progs {

// Prototypes
// Help Functions
void TeamFortress_MOTD();
void TeamFortress_HelpMap();
void AliasTimer();

//ofN
void bprintline();
void PlayClientSound(entity player, const string& thesound);

//=========================================================================
// HELP FUNCTIONS
//=========================================================================
void TeamFortress_MOTD()
{
	string ya;
	string st;
	entity atimer;

	// Neo mode
	// for forcing the new player to team 2 (Agents)
	if (neo.isModeActive() && self->team_no == 0) Menu_Team_Input(2);
	
	// check if player already has aliases
	if (self->motd == 4) {
		stuffcmd(self,"prozac2\n");
		self->show_hostile = 0;
	}

	if (self->motd <= PR_MOTD_FINISHED - 2)
	{
		if (self->show_hostile < time)
		{
			//CenterPrint3(self,#MSG_INTRO,#MSG_INTRO2,#MSG_INTRO3);//,CP_MOTD);
			// PZ: I'm using .worldtype here on players, for the sole purpose of allowing multiple pages on the intro screen.
			if      (self->worldtype <= 1)
				PrintProzacMOTD();
			else if (self->worldtype == 2)
				PrintProzacMOTD_Page2();
			else if (self->worldtype == 3)
				PrintProzacMOTD_Page3();
			else if (self->worldtype == 4)
				PrintProzacMOTD_Page4();
			else if (self->worldtype == 5)
				PrintProzacMOTD_Page5();
			self->show_hostile = time + PR_MOTD_REFRESHRATE; //was 1 //.5?
		}
		self->menu_count = PR_MENU_REFRESH_RATE;
		self->current_menu = PR_MENU_DEFAULT;

		self->motd = self->motd + 1;
		return;
	}

	if (self->motd == PR_MOTD_FINISHED - 1)
	{
		#ifdef PR_QUAKE_WORLD
			//WK CHECK TO SEE IF WE ARE REDIRECTING PLAYERS
			st = infokey(world, "redir");
			if (st != string_null && st != "off") {
				stuffcmd(self,"connect ");
				stuffcmd(self,st);
				stuffcmd(self,"\n");
			}
		#endif

		// Set Default autozoom
		if (PR_DEFAULT_AUTOZOOM == PR_OFF)
			self->tfstate = self->tfstate | PR_TFSTATE_ZOOMOFF;

		// Set their color to white, so they stand out from teams
		if ((teamplay != 0) && (self->team_no == 0))
		{
			// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
			//     themselves, to keep from having to kick them when they do.
			//stuffcmd(self, "color ");
			ya = ftos(PR_WHITE - 1);
			setinfo(self, "topcolor", ya);
			setinfo(self, "bottomcolor", ya);
			//stuffcmd(self, ya);
			//stuffcmd(self, "\n");
		}

/*		if (self.got_aliases == #FALSE)
			sprint(self, #PRINT_HIGH, "\n\n\n");*/

//CH prints customTF version.  Edit in messages.qc
        PrintProzacIntro();

        self->current_menu = PR_MENU_DEFAULT;
		self->menu_count = PR_MENU_REFRESH_RATE;

		self->motd = PR_MOTD_FINISHED;

		//WK CustomTF Commands
		TeamFortress_Alias("custom", (PR_TF_CHANGEPC + PR_PC_CUSTOM), 0);
		TeamFortress_Alias("upgrade", (PR_TF_SELL), 0);
		TeamFortress_Alias("skill", (PR_TF_SKILL), 0);
		TeamFortress_Alias("throwdet", (PR_IMPULSE_THROWDETPACK), 0);
        TeamFortress_Alias("holo", (PR_IMPULSE_HOLO), 0);
        TeamFortress_Alias("range", PR_IMPULSE_TESTRANGE, 0);
        //TeamFortress_Alias("debug", (#IMPULSE_DEBUG), 0); 193

		// check if player already has aliases
	   	if (self->got_aliases == PR_TRUE)
			return;

		//WK If not, spawn off a timer to slowly send the aliases to
		//prevent overflow
		atimer = spawnServerSide(); // PZ: make it a server-side only entity
		atimer->classname = "timer";
        atimer->netname = "alias"; // OfN - For not removing on removetimers func
		atimer->owner = self;
		atimer->nextthink = time + 0.5;
		atimer->heat = 0;
		atimer->think = AliasTimer;
	}
}

//=========================================================================
// Help displayed when someone types the alias "maphelp"
void TeamFortress_HelpMap()
{
    // Anti-abuse code
    if (self->option2 > time)
        return;

    self->option2 = time + 1.25;

    // OfN - Display settings
    PrintGameSettings(PR_FALSE);

    sprint(self,PR_PRINT_HIGH,S_("^bMap Help^b:\n"));

	entity te;

	te = find(world, "classname", "info_tfdetect");
	if (te != world)
	{
		if (te->non_team_broadcast != string_null)
		{
			sprint (self, PR_PRINT_HIGH, te->non_team_broadcast);
			return;
		}
	}

    // sprint (self, #PRINT_HIGH, "There is no help for you.\n");
    sprint (self, PR_PRINT_HIGH, "There is no map-specific help.\n");
}

void AliasTimer() {

    if (!self->owner->is_connected)
    {
        dremove(self);
        return;
    }

    entity oself;
	float myheat;
	myheat = self->heat = self->heat + 1;
	if (self->heat > 5) {
		self->think = SUB_Remove;
		self->nextthink = time + 0.1;
	}
	oself = self;
	self = self->owner;

	if (myheat == 1) {
		sprint (self, PR_PRINT_HIGH, "binding aliases...\n");

		// Stuff Aliases

		// PZ: added 'changeteam' and 'changeclass'
		TeamFortress_Alias("changeteam", PR_TF_GRENADE_1, 0);
		TeamFortress_Alias("changeclass", PR_TF_GRENADE_2, 0);

		// Grenades
		TeamFortress_Alias("primeone", PR_TF_GRENADE_1, 0);
		TeamFortress_Alias("primetwo", PR_TF_GRENADE_2, 0);
		TeamFortress_Alias("throwgren", PR_TF_GRENADE_T, 0);
		TeamFortress_Alias("+gren1", PR_TF_GRENADE_1, 0);
		TeamFortress_Alias("+gren2", PR_TF_GRENADE_2, 0);
		TeamFortress_Alias("-gren1", PR_TF_GRENADE_T, 0);
		TeamFortress_Alias("-gren2", PR_TF_GRENADE_T, 0);

		// Pipebombs
		TeamFortress_Alias("detpipe", PR_TF_PB_DETONATE, 0);
		TeamFortress_Alias("uncustom", PR_TF_UNCUSTOM, 0);


        #ifdef PR_ALIASES_TEST
        sprint(self,PR_PRINT_HIGH,"Finished alias pack number 1\n");
        #endif
	}
	else if (myheat == 2) {

        #ifdef PR_ALIASES_TEST
        sprint(self,PR_PRINT_HIGH,"Starting alias pack number 2\n");
        #endif

		// Reloading
		TeamFortress_Alias("reload", PR_TF_RELOAD, 0);

		// Actions
		TeamFortress_Alias("special", PR_TF_SPECIAL_SKILL, 0);
		TeamFortress_Alias("saveme", PR_TF_MEDIC_HELPME, 0);
		TeamFortress_Alias("discard", PR_TF_DISCARD, 0);
		TeamFortress_Alias("id", PR_TF_ID, 0);
		TeamFortress_Alias("inv",PR_TF_INVENTORY, 0);

		// Class Ability Impulses
		TeamFortress_Alias("disguise", PR_TF_SPY_SPY, 0);
		TeamFortress_Alias("feign", PR_TF_SPY_DIE, 0);
		TeamFortress_Alias("sfeign", PR_TF_SPY_DIE2, 0);
		TeamFortress_Alias("build", PR_TF_ENGINEER_BUILD, 0);
		TeamFortress_Alias("detdispenser", (PR_TF_ENGINEER_BUILD), 7);

		// General
		TeamFortress_Alias("dropammo", PR_TF_DROP_AMMO, 0);
		TeamFortress_Alias("flaginfo", PR_FLAG_INFO, 0);
		TeamFortress_Alias("autozoom", PR_TF_AUTOZOOM, 0);
		//TeamFortress_Alias("showloc", #TF_DISPLAYLOCATION, 0);
		TeamFortress_Alias("changeclass", PR_TF_CHANGECLASS, 0);   // PZ: Why would someone take this out?? I put it back in. (I had to also finish it.)
                                    //- OfN - it was(#TF_CHANGEPC + #PC_CUSTOM)
		TeamFortress_Alias("changeteam", PR_TF_CHANGETEAM, 0);     // PZ: Added this.

        #ifdef PR_ALIASES_TEST
        sprint(self,PR_PRINT_HIGH,"Finished alias pack number 2\n");
        #endif
	}
	else if (myheat == 3) {

        #ifdef PR_ALIASES_TEST
        sprint(self,PR_PRINT_HIGH,"Starting alias pack number 3\n");
        #endif

		//Taunts
		TeamFortress_Alias("taunt1", PR_TF_TAUNT, 0);
		TeamFortress_Alias("taunt2", PR_TF_TAUNT2, 0);
		TeamFortress_Alias("taunt3", PR_TF_TAUNT3, 0);
		TeamFortress_Alias("taunt4", PR_TF_TAUNT4, 0);
		TeamFortress_Alias("taunt5", PR_TF_TAUNT5, 0);

		// Class Changing
		TeamFortress_Alias("scout",(PR_TF_CHANGEPC + PR_PC_SCOUT), 0);
		TeamFortress_Alias("sniper",(PR_TF_CHANGEPC + PR_PC_SNIPER), 0);
		TeamFortress_Alias("soldier",(PR_TF_CHANGEPC + PR_PC_SOLDIER), 0);
		TeamFortress_Alias("demoman",(PR_TF_CHANGEPC + PR_PC_DEMOMAN), 0);
		TeamFortress_Alias("medic",(PR_TF_CHANGEPC + PR_PC_MEDIC), 0);
		TeamFortress_Alias("hwguy",(PR_TF_CHANGEPC + PR_PC_HVYWEAP), 0);
		TeamFortress_Alias("pyro",(PR_TF_CHANGEPC + PR_PC_PYRO), 0);
		TeamFortress_Alias("spy",(PR_TF_CHANGEPC + PR_PC_SPY), 0);
		TeamFortress_Alias("engineer",(PR_TF_CHANGEPC + PR_PC_ENGINEER), 0);
		TeamFortress_Alias("randompc",(PR_TF_CHANGEPC + PR_PC_RANDOM), 0);

		TeamFortress_Alias("showtf",PR_TF_SHOWTF, 0);
		TeamFortress_Alias("maphelp", PR_TF_HELP_MAP, 0);
        #ifdef PR_COOP_MODE
        if (coop)
            TeamFortress_Alias("dropkey", PR_TF_DROPKEY, 0);
        #endif

        #ifdef PR_ALIASES_TEST
        sprint(self,PR_PRINT_HIGH,"Finished alias pack number 3\n");
        #endif
	}
	else if (myheat == 4) {

        #ifdef PR_ALIASES_TEST
        sprint(self,PR_PRINT_HIGH,"Starting alias pack number 4\n");
        #endif

		// Sample aliases for the Scanner
		TeamFortress_Alias("scan10", PR_TF_SCAN_10, 0);
		TeamFortress_Alias("scan30", PR_TF_SCAN_30, 0);
		TeamFortress_Alias("scan100", PR_TF_SCAN_100, 0);
		TeamFortress_Alias("scanf", PR_TF_SCAN_FRIENDLY, 0);
		TeamFortress_Alias("scane", PR_TF_SCAN_ENEMY, 0);

		// Team handlers
		TeamFortress_Alias("showscores",PR_TF_TEAM_SCORES, 0);
		TeamFortress_Alias("showclasses",PR_TF_TEAM_CLASSES, 0);
		TeamFortress_Alias("query", PR_TF_STATUS_QUERY, 0);

		// Sample aliases for the Detpack
		TeamFortress_Alias("det5", PR_TF_DETPACK_5, 0);
		TeamFortress_Alias("det20", PR_TF_DETPACK_20, 0);
		TeamFortress_Alias("det50", PR_TF_DETPACK_50, 0);
		TeamFortress_Alias("+det5", PR_TF_DETPACK_5, 0);
		TeamFortress_Alias("-det5", PR_TF_DETPACK_STOP, 0);
		TeamFortress_Alias("+det20", PR_TF_DETPACK_20, 0);
		TeamFortress_Alias("-det20", PR_TF_DETPACK_STOP, 0);
		TeamFortress_Alias("+det50", PR_TF_DETPACK_50, 0);
		TeamFortress_Alias("-det50", PR_TF_DETPACK_STOP, 0);

        #ifdef PR_ALIASES_TEST
        sprint(self,PR_PRINT_HIGH,"Finished alias pack number 4\n");
        #endif
	}
	else if (myheat == 5) {

        #ifdef PR_ALIASES_TEST
        sprint(self,PR_PRINT_HIGH,"Starting alias pack number 5\n");
        #endif

        //Get rid of silly MegaTF 10 year olds
		//TeamFortress_Alias("lay", #TF_LAY, 0);   // PZ: let's not be 10-year-olds, ourselves
		//CH allow dropitems
		TeamFortress_Alias("dropitems", PR_TF_DROPITEMS, 0);

        //TeamFortress_Alias("get", #TF_ADMIN_KICK_CYCLE, 0);
		//TeamFortress_Alias("kick", #TF_ADMIN_KICK_PERSON, 0);
		//TeamFortress_Alias("ban", #TF_ADMIN_BAN_PERSON, 0);
		//TeamFortress_Alias("ceasefire", #TF_ADMIN_CEASEFIRE, 0);
        //TeamFortress_Alias("cmnd", #IMPULSE_STUFFCMD, 0);

        //- OfN
		TeamFortress_Alias("punish", PR_IMPULSE_PUNISH, 0);
		TeamFortress_Alias("votemap", PR_IMPULSE_VOTEMAP, 0);
		TeamFortress_Alias("votebot", PR_IMPULSE_VOTEBOT, 0);  // PZ: for "votebot"
		TeamFortress_Alias("voteyes", PR_IMPULSE_VOTEYES, 0);
		TeamFortress_Alias("voteno", PR_IMPULSE_VOTENO, 0);


#ifdef PR_STATUSBAR
		TeamFortress_Alias("sbar_on", PR_TF_STATUSBAR_ON, 0);
		TeamFortress_Alias("sbar_off", PR_TF_STATUSBAR_OFF, 0);
		TeamFortress_Alias("sbar_200", PR_TF_STATUSBAR_RES_START, 0);
		TeamFortress_Alias("sbar_240", PR_TF_STATUSBAR_RES_START + 1, 0);
		TeamFortress_Alias("sbar_300", PR_TF_STATUSBAR_RES_START + 2, 0);
		TeamFortress_Alias("sbar_350", PR_TF_STATUSBAR_RES_START + 3, 0);
		TeamFortress_Alias("sbar_384", PR_TF_STATUSBAR_RES_START + 4, 0);
		TeamFortress_Alias("sbar_400", PR_TF_STATUSBAR_RES_START + 5, 0);
		TeamFortress_Alias("sbar_480", PR_TF_STATUSBAR_RES_START + 6, 0);
		TeamFortress_Alias("sbar_600", PR_TF_STATUSBAR_RES_START + 7, 0);
		TeamFortress_Alias("sbar_768", PR_TF_STATUSBAR_RES_START + 8, 0);
#endif
		// Set up the alias check
		TeamFortress_Alias("prozac2", PR_TF_ALIAS_CHECK, 0);

		// Do the binds
		stuffcmd(self, "bind 9 \"impulse 9\"\n");
		stuffcmd(self, "bind 0 \"impulse 10\"\n");

		//WK INTRO THUNDER
		// OfN - now we have four :P

        myheat = rint(random()*3);

        if (myheat == 0)
            stuffcmd(self, "play ambience/thunder1\n");
        else if (myheat == 1)
            stuffcmd(self, "play environ/thunder2\n");
        else if (myheat == 2)
            stuffcmd(self, "play environ/thunder3\n");
        else
            stuffcmd(self, "play environ/thunder4\n");

        sprint(self,PR_PRINT_HIGH,"The aliases have been set. Welcome!\n");
	}

	self = oself;
	self->nextthink = time + 1;
}

} // END namespace Progs
