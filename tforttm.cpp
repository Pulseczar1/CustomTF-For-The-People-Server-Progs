/*
	TeamFortress 2.1	-	22/1/97

	TeamFortress Software
	Robin Walker, John Cook, Ian Caughley.

	Functions handling all the help displaying for TeamFortress.
*/

#include "progs.h"
#include "ofndefs.h"
#include "custom.h"
#include "tfort.h"
#include "army.h"
#include "admin.h"
#include "weapons.h"
#include "debug.h"
#include "neo.h"

namespace Progs {

// Prototypes
// Team Functions
float TeamFortress_TeamPutPlayerInTeam(float performChecks);
float TeamFortress_TeamSet(float tno, float performChecks);
float TeamFortress_TeamGetColor(float tno);
void TeamFortress_TeamSetColor(float tno);
void TeamFortress_CheckTeamCheats();
void TeamFortress_TeamIncreaseScore(float tno, float scoretoadd, entity playerThatTriggeredScore);
float TeamFortress_TeamGetScore(float tno);
float TeamFortress_TeamGetLives(float tno);
float TeamFortress_TeamGetNoPlayers(float tno);
float TeamFortress_TeamGetMaxPlayers(float tno);
float TeamFortress_TeamGetWinner();
void TeamFortress_TeamShowScores(float all, float teamscored, float scorepoints);
string TeamFortress_TeamGetColorString(float tno);
void TeamFortress_TeamShowMemberClasses(entity Player);
int TeamFortress_TeamGetIllegalClasses(float tno);
float TeamFortress_TeamIsCivilian(float tno);
#ifdef PR_QUAKE_WORLD
void SetTeamName(entity p);
#endif
string GetTrueTeamName(float tno);

//-------- OfN ---------------------------------//
void teamprefixsprint(float tno, entity ignore);

//- OfN - Extra cheat checking...
float Is_TF_Skin(const string& skin_str);
float IsValidTopColor(float tno, float theColor);


/*
//The QuakeForge server will call this function on user info changes. The
//expected return value is 0 if the server is to handle the change as normal
//(server sends changes to clients) or non-zero if the progs handled the change
//and the server is to ignore it (by not sending the changes).
float (string key, string oldvalue, string value) UserInfoChanged =
{
	local string realcolor;	// color sent to non-team players
	local string teamcolor;	// color sent to team players
	local float myteam;		// team this player is on
	local float clno;		// client number of this player
	local entity cl;		// client entity

	if (key != "topcolor" && key != "bottomcolor") {
		// let the server handle all other keys
		return 0;
	}
	//
	realcolor = value;
	myteam = self.team_no;
	teamcolor = ftos (TeamFortress_TeamGetColor (myteam));
	// teamkillers are always pink
	if (realcolor == ftos (#PINK))
		teamcolor = realcolor;

	// find our client number (hmm, maybe this should be passed in as a third
	// parameter, but then there's the issue of integer vs float)
	clno = 0;
	cl = nextent (world);
	while (cl != self) {
		clno = clno + 1;
		cl = nextent (cl);
	}

	// spam the new color to the clients
#ifdef NEVER_DEFINED
	cl = nextent (world);
	while (cl) {
		if (cl.classname == "player" || cl.classname == "spec") {
			msg_entity = cl;
			WriteByte (#MSG_ONE, 51);	// svc_setinfo
			WriteByte (#MSG_ONE, clno);
			WriteString (#MSG_ONE, key);
			if (cl.team_no == myteam)
				WriteString (#MSG_ONE, teamcolor);
			else
				WriteString (#MSG_ONE, realcolor);
		}
		cl = nextent (cl);
	}
#endif
	// Gizmo - faster way to send out to all clients (since it's always faster to let C do all the heavy stuff)
	cl = find( world, classname, "player" );
	while ( cl ) {
		msg_entity = cl;
		WriteByte (#MSG_ONE, 51);	// svc_setinfo
		WriteByte (#MSG_ONE, clno);
		WriteString (#MSG_ONE, key);
		if (cl.team_no == myteam)
			WriteString (#MSG_ONE, teamcolor);
		else
			WriteString (#MSG_ONE, realcolor);

		cl = find( cl, classname, "player" );
	}
	cl = find( world, classname, "spec" );
	while ( cl ) {
		msg_entity = cl;
		WriteByte (#MSG_ONE, 51);	// svc_setinfo
		WriteByte (#MSG_ONE, clno);
		WriteString (#MSG_ONE, key);
		if (cl.team_no == myteam)
			WriteString (#MSG_ONE, teamcolor);
		else
			WriteString (#MSG_ONE, realcolor);

		cl = find( cl, classname, "spec" );
	}
	//tell the server we handled the change
	return 1;
}
*/


//=========================================================================
// TEAM FUNCTIONS
//=========================================================================
// Put the player into the team with the least number of players in it. Return #TRUE if successful
// PZ: `performChecks` is just passed on to TeamFortress_TeamSet(). See that function for what it is.
float TeamFortress_TeamPutPlayerInTeam(float performChecks)
{
	float i, j, lowest, likely_team;
	//local string st;

	//WK 1/7/7 Use KKMay's Server Side function to do this??

	i = 1;
	likely_team = random() * number_of_teams;
	likely_team = ceil(likely_team);
	if (likely_team == 0)
		likely_team = number_of_teams;
	lowest = 33;
	while (i < (number_of_teams + 1))
	{
		j = TeamFortress_TeamGetNoPlayers(i);
		if (j <= lowest) //WK j < lowest
		{
			// Is the team full?
			if (TeamFortress_TeamGetMaxPlayers(i) > j)
			{
				if (lowest == j) { //WK tie
					if (random() < (1/number_of_teams)) {
						lowest = j;
						likely_team = i;
					}
				}
				else {
					lowest = j;
					likely_team = i;
				}
			}
		}
		i = i + 1;
	}

	// Put the player in likely_team
	return TeamFortress_TeamSet(likely_team, performChecks);
}

//=========================================================================
// Return the color for the team corresponding to the no passed in
float TeamFortress_TeamGetColor(float tno)
{
	if (tno == 1)
		return PR_DARKBLUE;
	if (tno == 2)
	{
		// Neo mode
		if (neo.isModeActive()) return PR_WHITE;

		return PR_RED;
	}
	if (tno == 3)
		return PR_YELLOW;
	if (tno == 4)
		return PR_DARKGREEN;

	return 0;
}

//=========================================================================
// Set the color for the team corresponding to the no passed in, to self.team_no
void TeamFortress_TeamSetColor(float tno)
{
	if (tno == 1)
	{
		team1col = PR_DARKBLUE;
		return;
	}
	if (tno == 2)
	{
		if (neo.isModeActive()) team2col = PR_WHITE; // Neo mode
		else     team2col = PR_RED;
		return;
	}
	if (tno == 3)
	{
		team3col = PR_YELLOW;
		return;
	}
	if (tno == 4)
	{
		team4col = PR_DARKGREEN;
		return;
	}
}

#ifdef PR_QUAKE_WORLD
string GetTeamName(float tno)
{
	string st;

	if (tno == 1)
	{
		/*if (world.b_b != "" && world.b_b != string_null)
		{
			return world.b_b;
		}*/

		// for Neo mode
		if (neo.isModeActive()) return S_("^bNeo^b");

		st = infokey(world, "team1");
		if (st == string_null)
			st = infokey(world, "t1");

		if (st == string_null || st == "")
			return "blue";
	}
	else if (tno == 2)
	{
		/*if (world.b_t != "" && world.b_t != string_null)
		{
			return world.b_t;
		}*/

		// for Neo mode
		if (neo.isModeActive()) return "Agnt";   // PZ: I renamed all instances of "Smith" to "Agent". (8-25-2020)
		
		st = infokey(world, "team2");
		if (st == string_null)
			st = infokey(world, "t2");

		if (st == string_null || st == "")
			return "red";
	}
	else if (tno == 3)
	{
		/*if (world.b_n != "" && world.b_n != string_null)
		{
			return world.b_n;
		}*/

		st = infokey(world, "team3");
		if (st == string_null)
			st = infokey(world, "t3");

		if (st == string_null || st == "")
			return "yell";
	}
	else if (tno == 4)
	{
		/*if (world.b_o != "" && world.b_o != string_null)
		{
			return world.b_o;
		}*/

		st = infokey(world, "team4");
		if (st == string_null)
			st = infokey(world, "t4");

		if (st == string_null || st == "")
			return "teal";
	}
	else
		//st = "\"\""; // PZ: This line was making setinfo() complain. Can't have quote characters in key or value.
		st = "";       //     Let's hope changing this doesn't break anything.

	return st;
}
//returns only the team color
string GetTrueTeamName(float tno)
{
	if (tno == 1)
		return S_("^bBlue^b");
	else if (tno == 2)
		return S_("^bRed^b");
	else if (tno == 3)
		return S_("^bYellow^b");
	else if (tno == 4)
		return S_("^bGreen^b");
	else
		return "ERROR";
}
#endif

#ifdef PR_QUAKE_WORLD
void SetTeamName(entity p)
{
	string st;

	// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
	//     themselves, to keep from having to kick them when they do.
	//stuffcmd(p, "team ");
	st = GetTeamName(p->team_no);
	setinfo(p, "team", st);
	//stuffcmd(p, st);
	//stuffcmd(p, "\n");
}
#endif

//=========================================================================
// Set the current player's.team_no to self.impulse. Return #TRUE if successful
// PZ: `performChecks`: 0 means don't perform any checking; just add to the team
//                      1 means run the function as normal with checks and everything
//                      2 means only perform checks; don't add to the team yet
float TeamFortress_TeamSet(float tno, float performChecks)
{
	string st;
	float tc;
	float result;
	float playerUsedChangeTeamCommand; // PZ
	float oldTeamNumber;               // PZ

	if (performChecks) // PZ
	{
		if (teamplay < 1)
		{
			sprint (self, PR_PRINT_HIGH, "Teamplay is not On, so FortressTeams are inactive.\n");
			return PR_FALSE;
		}

		/*if (chris)
			if (prematch > time)
			{
				sprint(self, #PRINT_HIGH, "Sorry, Chris' Teamplay Plus Mode is active and a round is in progress. Please wait for the next round to begin.\n");
				PlayerObserverMode();
			}*/

		if (tno > number_of_teams && number_of_teams != 0)
		{
			sprint (self, PR_PRINT_HIGH, "There can be only ");
			st = ftos(number_of_teams);
			sprint (self, PR_PRINT_HIGH, st);
			sprint (self, PR_PRINT_HIGH, " teams on this map.\nTry again\n");
			return PR_FALSE;
		}

		// PZ: If we are going to allow people to switch teams with "changeteam", then we can't have this. Changed it so that it only blocks
		//     selecting a team we're already on.
		/* PZ: took this out so that people can cancel team changes by reselecting the team they are already on
		if (self.team_no == tno) // was "> 0"
		{
			sprint(self, #PRINT_HIGH, "You're already on Team No ");
			st = ftos(self.team_no);
			sprint(self, #PRINT_HIGH, st);
			sprint(self, #PRINT_HIGH, ".\n");
			return #FALSE;
		}*/

		tc = TeamFortress_TeamGetNoPlayers(tno);
		if (tc >= TeamFortress_TeamGetMaxPlayers(tno))
		{
			//- OfN
			if (mapname == "huntedr")
			{
				if (tno == 3)
					sprint (self, PR_PRINT_HIGH, "The game has already enough assassins, be a bodyguard.\n");
				else if (tno == 1)
					sprint (self, PR_PRINT_HIGH, "You wanna be the vicepresident or what?\n");
			}
			else
				sprint (self, PR_PRINT_HIGH, "That team is full. Pick another.\n");

			return PR_FALSE;
		}
	}

	// PZ: 2 means only perform checks; don't add to the team yet
	if (performChecks == 2)
	{
		self->new_team_no = tno; // when we run this function later, we'll know what to set his team to
		sprint(self, PR_PRINT_HIGH, S_("After dying, you will return on ^bTeam^b "));
		st = ftos(self->new_team_no);
		st = colstr(st, PR_COLSTR_RED);
		sprint(self, PR_PRINT_HIGH, st, "\n");
		return PR_TRUE;           // returning true to indicate that we can add the player to the team
	}

	// ###############################################################
	// PZ NOTE: This is where it adds them to a team.
	// ###############################################################

	// PZ: remember whether player used the "changeteam" command
	if (self->team_no > 0)
		playerUsedChangeTeamCommand = PR_TRUE;
	else
		playerUsedChangeTeamCommand = PR_FALSE;

	oldTeamNumber = self->team_no; // PZ: added

	// PZ NOTE: I don't think this branch is ever used.
	// If the color for the team this player is changing to is 0,
	// then this is the first player in this team, and we set the
	// teamcolor to this player's pants color.
	// If not, we change this player's color to this team.
	if (TeamFortress_TeamGetColor(tno) == 0)
	{
		TeamFortress_TeamSetColor(tno);
		// If the color wasn't set
		if (TeamFortress_TeamGetColor(tno) == 0)
		{
			sprint (self, PR_PRINT_HIGH, "You can't start a new team with your color, since another ");
			sprint (self, PR_PRINT_HIGH, "already using that color. Change your pants color, then try again.\n");
			return PR_FALSE;
		}

		// Announce the new team
		bprint(PR_PRINT_HIGH, self->netname);
		bprint(PR_PRINT_HIGH, " has started Team No ");
		st = ftos(tno);
		bprint(PR_PRINT_HIGH, st);
		bprint(PR_PRINT_HIGH, ".\n");

		// Prevent the cheatchecking mechanism from nabbing them b4 the
		// color command works.
		makeImmune(self,time+10);
		//self.immune_to_check = time + 10;

		// Set the player's color
		// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
		//     themselves, to keep from having to kick them when they do.
		//stuffcmd(self, "color ");
		tc = TeamFortress_TeamGetColor(tno) - 1;
		st = ftos(tc);
		//- OfN - Nice colors
		if (nicecolors==1) st =TeamGetNiceColor(tno);
		setinfo(self, "topcolor", st);    // PZ
		setinfo(self, "bottomcolor", st); // PZ
		//stuffcmd(self, st);
		//stuffcmd(self, "\n");

		self->team_no = tno;
		self->lives = TeamFortress_TeamGetLives(tno);

	#ifdef PR_QUAKE_WORLD
		SetTeamName(self);
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

		return PR_TRUE;
	}

	// Announce the new team member
	#ifndef PR_OLD_TEAM_ANNOUNCEMENTS
	bprint(PR_PRINT_HIGH, self->netname);
	bprint(PR_PRINT_HIGH, " has joined Team No ");
	st = ftos(tno);
	bprint(PR_PRINT_HIGH, st);
	st = GetTrueTeamName(tno);
	bprint(PR_PRINT_HIGH, " (", st, " team)\n");

	#else

	bprint(PR_PRINT_HIGH, self->netname);
	bprint(PR_PRINT_HIGH, " has joined Team No ");
	st = ftos(tno);
	bprint(PR_PRINT_HIGH, ".\n");
	#endif

	// Set the player's color
	// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
	//     themselves, to keep from having to kick them when they do.
	//stuffcmd(self, "color ");
	tc = TeamFortress_TeamGetColor(tno) - 1;
	st = ftos(tc);
	//- OfN - Nice colors
	if (nicecolors==1) st =TeamGetNiceColor(tno);
	setinfo(self, "topcolor", st);    // PZ
	setinfo(self, "bottomcolor", st); // PZ
	//stuffcmd(self, st);
	//stuffcmd(self, "\n");

	self->team_no = tno;
	// Prevent the cheatchecking mechanism from nabbing them b4 the
	// color command works.
	makeImmune(self,time+10);
	//self.immune_to_check = time + 10;
	self->lives = TeamFortress_TeamGetLives(tno);

	// Tell them what class the other members of their team are
	TeamFortress_TeamShowMemberClasses(self);

#ifdef PR_QUAKE_WORLD
	SetTeamName(self);
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
//RJM
	if (toggleflags & PR_TFLAG_TEAMFRAGS)
		self->frags = TeamFortress_TeamGetScore(self->team_no);
//RJM

	//- OfN Team3 should have less players than red in Hunted
	if (mapname == "huntedr")
	{
		result = floor(TeamFortress_TeamGetNoPlayers(2) * PR_HUNTED_YELLOWTEAM_FACTOR);
		team3maxplayers = result;
		if (team3maxplayers < 1) team3maxplayers = 1;
	}
	else
	if (mapname == "border1" || mapname == "border1r")
	{
		result = floor(TeamFortress_TeamGetNoPlayers(2) * PR_BORDER_FACTOR);
		team3maxplayers = result;
		team1maxplayers = result;
		if (team3maxplayers < 1) team3maxplayers = 1;
		if (team1maxplayers < 1) team1maxplayers = 1;
	}

	// OfN - Adjust his teammates army rating as needed
	ArmyRatingJoin(self);

	// PZ - created playersOnTeam# for AGR mode
	if      (self->team_no == 1) playersOnTeam1 = playersOnTeam1 + 1;
	else if (self->team_no == 2) playersOnTeam2 = playersOnTeam2 + 1;
	else if (self->team_no == 3) playersOnTeam3 = playersOnTeam3 + 1;
	else if (self->team_no == 4) playersOnTeam4 = playersOnTeam4 + 1;

	// PZ: If the player used "changeteam", we are changing his team on-the-fly. So, we need to use Admin_Assign() to make sure everything,
	//     like the player's belongings, is cleaned up somehow.
	if (playerUsedChangeTeamCommand)
	{
		self->admin_kick = self;            // who is being assigned a team
		Admin_Assign(self->team_no, PR_TRUE);
		if      (oldTeamNumber == 1) playersOnTeam1 = playersOnTeam1 - 1;
		else if (oldTeamNumber == 2) playersOnTeam2 = playersOnTeam2 - 1;
		else if (oldTeamNumber == 3) playersOnTeam3 = playersOnTeam3 - 1;
		else if (oldTeamNumber == 4) playersOnTeam4 = playersOnTeam4 - 1;
	}

	return PR_TRUE;
}

//define KICK_FOR_CHANGING_BAD_STUFF  // PZ: added; uncomment this to kick people for changing team, color, and skin console variables

//=========================================================================
// Make sure no-one is changing their colors, team, and skin.
// PZ: I made it so that players can no longer change these three things from the console (or scripts). These changes are now done by
//     the added function, setinfo(), rather than stuffcmd(). So, this color/team/skin checking code should no longer be needed.
void TeamFortress_CheckTeamCheats()
{
	string st, sk;
	float tc;
	float rate;
	vector knockDirection; // PZ

	if (self->immune_to_chec > time)
		return;

	// PZ: I'm putting this here only because this function isn't ran very often. Making the players use the "skins" command will
	//     make sure they have all the skins that players are using. The game only makes you download the TF skins that are in use
	//     when you connect. Using the "skins" command makes you download whatever skins are in use that you don't have.
	//stuffcmd(self, "skins\n"); // nevermind, this will make "checking skins" print out to the player every time this executes

#ifdef PR_QUAKE_WORLD
	//WK Limit rate to 10k?
	st = infokey(self,"rate");
	rate = stof(st);
	if (rate > 10000) {
		stuffcmd(self,"rate 10000\n");
	}
#endif

	if (self->deadflag)
		return;

#ifdef PR_COOP_MODE
	if (coop && !deathmatch)	// don't worry about it if in coop game
		return;
#endif

	//WK Add in hack for "skins" cheat. This is as good a place as any
	if (self->netname == string_null) {
		stuffcmd(self,"name \"I'm a cheater even when RENAMED\"\n");
		stuffcmd(self,"disconnect\n");
	}

	// Have they changed their skin? (PZ: moved this out of the teamplay > 0 section; don't want skin changes in deathmatch)
#ifdef PR_QUAKE_WORLD
	if (self->playerclass != PR_PC_UNDEFINED)
	{
		//WK For some reason, this isn't reading right for el PC_CUSTOM
		st = infokey(self, "skin");
		tc = 0;

		// OfN kick custom player classes using a non TF-skin
		if (self->playerclass == PR_PC_CUSTOM && !Is_TF_Skin(st))
		{
			TeamFortress_SetSkin(self);

			// PZ: Don't kick them anymore. Just set it back and annoy them for changing it, so that they won't want to keep changing it.
		#ifdef PR_KICK_FOR_CHANGING_BAD_STUFF
			bprint(PR_PRINT_MEDIUM, self->netname, " has been kicked for changing skin.\n");
			sprint(self, PR_PRINT_HIGH, "You have been kicked for changing your skin. Don't do it.\n");
			sprint(self, PR_PRINT_HIGH, "Check that you don't have a .cfg file that is changing your skin.\n"); // PZ: added
			stuffcmd(self, "disconnect\n");
		#else
			bprint(PR_PRINT_MEDIUM, self->netname, " has been punched for changing skin.\n");
			sprint(self, PR_PRINT_HIGH, "You have been punched for changing your skin. Don't do it.\n");
			sprint(self, PR_PRINT_HIGH, "Check that you don't have a .cfg file that is changing your skin.\n"); // PZ: added
			TF_T_Damage(self, world, world, 50, 0, 0);
		#endif
		}

		if ((self->cutf_items & PR_CUTF_SPY_KIT) && self->undercover_skin != 0)
		{
			//WK tc = #PC_SPY;
			tc = self->playerclass;
			self->playerclass = self->undercover_skin;
		}

		if ( self->playerclass == PR_PC_SCOUT )
			sk = "tf_scout";
		else if ( self->playerclass == PR_PC_SNIPER )
			sk = "tf_snipe";
		else if ( self->playerclass == PR_PC_SOLDIER )
			sk = "tf_sold";
		else if ( self->playerclass == PR_PC_DEMOMAN )
			sk = "tf_demo";
		else if ( self->playerclass == PR_PC_MEDIC )
			sk = "tf_medic";
		else if ( self->playerclass == PR_PC_HVYWEAP )
			sk = "tf_hwguy";
		else if ( self->playerclass == PR_PC_PYRO )
			sk = "tf_pyro";
		else if ( self->playerclass == PR_PC_SPY )
			sk = "tf_spy";
		else if ( self->playerclass == PR_PC_ENGINEER )
			sk = "tf_eng";
		else
		{
			if ((self->cutf_items & PR_CUTF_SPY_KIT) && self->undercover_skin != 0)
			{
				self->playerclass = tc;
			}
			return; //WK Don't check as a safety precaution.
		}

		if (self->cutf_items & PR_CUTF_SPY_KIT && self->undercover_skin != 0)
		{
			//sprint(self,#PRINT_HIGH,"Mommy Wuvs You!\n");
			self->playerclass = tc;
		}

		//WK Ignore our custom, spy-kitting friends. Kick everyone else
		if ((st != sk) && !((self->playerclass == PR_PC_CUSTOM) && (self->cutf_items & PR_CUTF_SPY_KIT)))
		{
			#ifdef PR_CHEAT_WARNINGS
				RPrint(self->netname);
				RPrint(" had his skin reset.\n");
			#endif

			TeamFortress_SetSkin(self);

			// PZ: I THINK IT WOULD BE BETTER IF I JUST DON'T ALLOW PLAYERS TO CHANGE THEIR USERINFO FOR SKIN, TEAM, AND COLOR.
			// PZ: Don't kick them anymore. Just set it back and annoy them for changing it, so that they won't want to keep changing it.
		#ifdef PR_KICK_FOR_CHANGING_BAD_STUFF
			bprint(PR_PRINT_MEDIUM, self->netname, " has been kicked for changing skin.\n");
			sprint(self, PR_PRINT_HIGH, "You have been kicked for changing your skin. Don't do it.\n");
			sprint(self, PR_PRINT_HIGH, "Check that you don't have a .cfg file that is changing your skin.\n"); // PZ: added
			stuffcmd(self, "disconnect\n");
		#else
			bprint(PR_PRINT_MEDIUM, self->netname, " has been punched for changing skin.\n");
			sprint(self, PR_PRINT_HIGH, "You have been punched for changing your skin. Don't do it.\n");
			sprint(self, PR_PRINT_HIGH, "Check that you don't have a .cfg file that is changing your skin.\n"); // PZ: added
			TF_T_Damage(self, world, world, 60, 0, 0);
			knockDirection[X] = crandom() * 900; // random float between -1 and +1
			knockDirection[Y] = crandom() * 900;
			knockDirection[Z] = 150;
			self->velocity = self->velocity + knockDirection;
		#endif
		}
	}
#endif

	//- OfN - Added checking for topcolor changes... (see IsValidTopColor in OfteN.qc)
	float tTopColor;
	st = infokey(self,"topcolor");
	tTopColor=stof(st);

	// Have they changed color?
	if (self->team_no > 0 && teamplay > 0)
	{
	#ifndef PR_QUAKE_WORLD
		// if they're a spy, check to see if they've changed colors manually
		if ((self->cutf_items & PR_CUTF_SPY_KIT) && self->undercover_team != 0)
		{
			if (TeamFortress_TeamGetColor(self->undercover_team) != self->team) //- OfN
			//if ((TeamFortress_TeamGetColor(self.undercover_team) != self.team) || !IsValidTopColor(self.undercover_team,tTopColor))
			//  || !IsValidTopColor(self.undercover_team,tTopColor)
			{
				// Set their color
				// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
				//     themselves, to keep from having to kick them when they do.
				//stuffcmd(self, "color ");
				tc = TeamFortress_TeamGetColor(self->undercover_team) - 1;
				st = ftos(tc);
				//- OfN - Nice colors
				if (nicecolors==1) st =TeamGetNiceColor(self->undercover_team);
				setinfo(self, "topcolor", st);    // PZ
				setinfo(self, "bottomcolor", st); // PZ
				//stuffcmd(self, st);
				//stuffcmd(self, "\n");

				bprint2(PR_PRINT_MEDIUM, self->netname, " has been kicked for changing color.\n");
				sprint(self, PR_PRINT_HIGH, "You have been kicked for changing your color. Don't do it.\n");
				sprint(self, PR_PRINT_HIGH, "Check that you don't have a .cfg file that is changing your color.\n"); // PZ: added
				stuffcmd(self, "disconnect\n");
				return;
			}
		}
		else if (TeamFortress_TeamGetColor(self->team_no) != self->team)
		{
			// Set their color
			// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
			//     themselves, to keep from having to kick them when they do.
			//stuffcmd(self, "color ");
			tc = TeamFortress_TeamGetColor(self->team_no) - 1;
			st = ftos(tc);
			//- OfN - Nice colors
			if (nicecolors==1) st =TeamGetNiceColor(self->team_no);
			setinfo(self, "topcolor", st);    // PZ
			setinfo(self, "bottomcolor", st); // PZ
			//stuffcmd(self, st);
			//stuffcmd(self, "\n");

			bprint2(PR_PRINT_MEDIUM, self->netname, " has been kicked for changing color.\n");
			sprint(self, PR_PRINT_HIGH, "You have been kicked for changing your color. Don't do it.\n");
			sprint(self, PR_PRINT_HIGH, "Check that you don't have a .cfg file that is changing your color.\n"); // PZ: added
			stuffcmd(self, "disconnect\n");
			return;
		}
	#else
		// QuakeWorld
		st = infokey(self, "bottomcolor");
		tc = stof(st);
		// if they're a spy, check to see if they've changed colors manually
		if ((self->cutf_items & PR_CUTF_SPY_KIT) && self->undercover_team != 0)
		{
			if ((TeamFortress_TeamGetColor(self->undercover_team) - 1) != tc || !IsValidTopColor(self->undercover_team,tTopColor))
			{
			#ifdef PR_CHEAT_WARNINGS
				RPrint(self->netname);
				RPrint(" had his colors reset.\n");
			#endif

				// Set the player's color
				// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
				//     themselves, to keep from having to kick them when they do.
				//stuffcmd(self, "color ");
				tc = TeamFortress_TeamGetColor(self->undercover_team) - 1;
				st = ftos(tc);
				//- OfN - Nice colors
				if (nicecolors==1) st =TeamGetNiceColor(self->undercover_team);
				setinfo(self, "topcolor", st);    // PZ
				setinfo(self, "bottomcolor", st); // PZ
				//stuffcmd(self, st);
				//stuffcmd(self, "\n");

				bprint(PR_PRINT_MEDIUM, self->netname, " has been kicked for changing color.\n");
				sprint(self, PR_PRINT_HIGH, "You have been kicked for changing your color. Don't do it.\n");
				sprint(self, PR_PRINT_HIGH, "Check that you don't have a .cfg file that is changing your color.\n"); // PZ: added
				stuffcmd(self, "disconnect\n");
				return;
			}
		}
		else if (tc != (TeamFortress_TeamGetColor(self->team_no) - 1) || !IsValidTopColor(self->team_no,tTopColor))
		{
		#ifdef PR_CHEAT_WARNINGS
			RPrint(self->netname);
			RPrint(" had his colors reset.\n");
		#endif

			// Set the player's color
			// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
			//     themselves, to keep from having to kick them when they do.
			//stuffcmd(self, "color ");
			tc = TeamFortress_TeamGetColor(self->team_no) - 1;
			st = ftos(tc);
			//- OfN - Nice colors
			if (nicecolors==1) st =TeamGetNiceColor(self->team_no);
			setinfo(self, "topcolor", st);    // PZ
			setinfo(self, "bottomcolor", st); // PZ
			//stuffcmd(self, st);
			//stuffcmd(self, "\n");

			bprint(PR_PRINT_MEDIUM, self->netname, " has been kicked for changing color.\n");
			sprint(self, PR_PRINT_HIGH, "You have been kicked for changing your color. Don't do it.\n");
			sprint(self, PR_PRINT_HIGH, "Check that you don't have a .cfg file that is changing your color.\n"); // PZ: added
			stuffcmd(self, "disconnect\n");
			return;
		}
	#endif

	#ifdef PR_QUAKE_WORLD
		// Have they Changed their Team?
		st = GetTeamName(self->team_no);
		if (st != infokey(self, "team"))
		{
			if (self->penance_time < time) { //Ignore if we're cursed

			#ifdef PR_CHEAT_WARNINGS
				RPrint(self->netname);
				RPrint(" had his team reset.\n");
			#endif

				// Set the player's team
				SetTeamName(self);
				bprint(PR_PRINT_MEDIUM, self->netname, " has been kicked for changing team.\n");
				sprint(self, PR_PRINT_HIGH, "You have been kicked for changing your team. Don't do it.\n");
				sprint(self, PR_PRINT_HIGH, "Check that you don't have a .cfg file that is changing your team.\n"); // PZ: added
				stuffcmd(self, "disconnect\n");
				return;
			}
		}
	#endif
	}
}

//=========================================================================
// Increase the score of a team
// PZ: Added `playerThatTriggeredScore`. This can potentially no-one (world). So keep that in mind.
void TeamFortress_TeamIncreaseScore(float tno, float scoretoadd, entity playerThatTriggeredScore)
{
	entity e;

	if (tno == 0 || scoretoadd == 0)
		return;

	if (tno == 1)
		team1score = team1score + scoretoadd;
	if (tno == 2)
		team2score = team2score + scoretoadd;
	if (tno == 3)
		team3score = team3score + scoretoadd;
	if (tno == 4)
		team4score = team4score + scoretoadd;

	// If TeamFrags is on, update all the team's player's frags.
	if (toggleflags & PR_TFLAG_TEAMFRAGS)
	{
		e = find(world, "classname", "player");
		while (e != world)
		{
			if (e->team_no == tno)
				e->frags = TeamFortress_TeamGetScore(tno);
			e = find(e, "classname", "player");
		}
	}

	// PZ: Make it so that flag captures and such affect the bot's skill level. If someone is capping a lot, bots should
	// get harder. A score increase by 10 will increase the skill of bots by 5, spread across all bots. An increase of 1
	// won't do anything.
	// DONE? TODO: it should only increase collectiveHumanScore when a human captures the flag
	if (playerThatTriggeredScore != world && playerThatTriggeredScore->ishuman)
	{
		scoretoadd = floor(scoretoadd / 2);
		collectiveHumanScore = collectiveHumanScore + scoretoadd;
	}
}

//=========================================================================
// Return the score of a team
float TeamFortress_TeamGetScore(float tno)
{
	if (tno == 1)
		return team1score;
	if (tno == 2)
		return team2score;
	if (tno == 3)
		return team3score;
	if (tno == 4)
		return team4score;

	return 0;
}

//=========================================================================
// Return the number of lives each team member in a team has
float TeamFortress_TeamGetLives(float tno)
{
	if (tno == 1)
		return team1lives;
	if (tno == 2)
		return team2lives;
	if (tno == 3)
		return team3lives;
	if (tno == 4)
		return team4lives;

	return 0;
}

//=========================================================================
// Return the number of players in a team
float TeamFortress_TeamGetNoPlayers(float tno)
{
	float size_team;
	entity search;
	//local string st;

	size_team = 0;

	search = find (world, "classname", "player");
	while (search != world)
	{
		if (search->team_no == tno)
			size_team = size_team + 1;
		search = find (search, "classname", "player");
	}

	return size_team;
}

//=========================================================================
// Return the number of lives each team member in a team has
float TeamFortress_TeamGetMaxPlayers(float tno)
{
	if (tno == 1)
		return team1maxplayers;
	if (tno == 2)
		return team2maxplayers;
	if (tno == 3)
		return team3maxplayers;
	if (tno == 4)
		return team4maxplayers;

	return 0;
}

//=========================================================================
// Return the team that's winning
float TeamFortress_TeamGetWinner()
{
	float i,j,highest,highestteam;

	i = 1;
	highest = 0;
	highestteam = 0;
	while (i < (number_of_teams + 1))
	{
		j = TeamFortress_TeamGetScore(i);
		if (j > highest)
		{
			highest = j;
			highestteam = i;
		}
		i = i + 1;
	}

	return highestteam;
}

//=========================================================================
// Return the team that's winning
/*CH i saw no uses
float() TeamFortress_TeamGetSecond =
{
	local float i,j,highest,highestteam, secondteam, second;

	i = 1;
	highestteam = TeamFortress_TeamGetWinner();
	highest = TeamFortress_TeamGetScore(highestteam);
	secondteam = 0;
	second = 0;
	while (i < (number_of_teams + 1))
	{
		j = TeamFortress_TeamGetScore(i);
		if (j < highest && j > second)
		{
			second = j;
			secondteam = i;
		}
		i = i + 1;
	}

	return secondteam;
};
*/
//=========================================================================
// Display all the Team Scores
void TeamFortress_TeamShowScores(float all, float teamscored, float scorepoints)
{
	string st;
	float i,j;

	i = 1;

	// OfN - End of map team scores
	if (all == 3)
	{
		// long scores
		while (i <= number_of_teams)
		{
			if (TeamFortress_TeamGetColor(i) > 0)
			{
				st = ftos(i);
				bprint (PR_PRINT_HIGH, "Team ", st, " (");

				st = GetTrueTeamName(i);
				st = strcat(st,") ");
				st = padstr(st,8);
				st = strcat(st,": ");
				bprint (PR_PRINT_HIGH, st);

				j = TeamFortress_TeamGetScore(i);
				st = ftos(j);
				st = colstr(st,PR_COLSTR_NUMBER);

				if (j < 10)
					bprint(PR_PRINT_HIGH,"  ");
				else if (j < 100)
					bprint(PR_PRINT_HIGH," ");

				bprint (PR_PRINT_HIGH, st, " points\n");
			}
			i = i + 1;
		}
		return;
	}

	// short scores
	if (all == 2)
	{
		while (i <= number_of_teams)
		{
			if (TeamFortress_TeamGetColor(i) > 0)
			{
				j = TeamFortress_TeamGetScore(i);
				st = TeamFortress_TeamGetColorString(i);

				bprint(PR_PRINT_HIGH, st);
				bprint(PR_PRINT_HIGH, ": ");
				st = ftos(j);
				bprint(PR_PRINT_HIGH, st);
				bprint(PR_PRINT_HIGH, " ");
			}
			i = i + 1;
		}

		if (!teamscored)
			bprint(PR_PRINT_HIGH," (mixed scoring)\n");
		else
		{
			st = GetTrueTeamName(teamscored);
			bprint(PR_PRINT_HIGH," [",st," scores ");
			st = ftos(scorepoints);
			st = colstr(st,PR_COLSTR_NUMBER);
			bprint(PR_PRINT_HIGH,st,"]\n");
		}

		return;
	}

	// long scores
	while (i <= number_of_teams)
	{
		if (TeamFortress_TeamGetColor(i) > 0)
		{
			if (all)
				bprint (PR_PRINT_HIGH, "Team ");
			else
				sprint (self, PR_PRINT_HIGH, "Team ");

			st = ftos(i);
			if (all)
				bprint (PR_PRINT_HIGH, st);
			else
				sprint (self, PR_PRINT_HIGH, st);

			if (all)
				bprint (PR_PRINT_HIGH, " (");
			else
				sprint (self, PR_PRINT_HIGH, " (");

			st = TeamFortress_TeamGetColorString(i);
			if (all)
				bprint (PR_PRINT_HIGH, st);
			else
				sprint (self, PR_PRINT_HIGH, st);

			if (all)
				bprint (PR_PRINT_HIGH, ") : ");
			else
				sprint (self, PR_PRINT_HIGH, ") : ");

			j = TeamFortress_TeamGetScore(i);
			st = ftos(j);

			if (all)
				bprint (PR_PRINT_HIGH, st);
			else
				sprint (self, PR_PRINT_HIGH, st);

			if (all)
				bprint (PR_PRINT_HIGH, "\n");
			else
				sprint (self, PR_PRINT_HIGH, "\n");
		}
		i = i + 1;
	}
}

//=========================================================================
// Return a string containing the color of the team passed in tno
string TeamFortress_TeamGetColorString(float tno)
{
	float col;

	col = TeamFortress_TeamGetColor(tno);
	if (col == 1)
		return "White";
	if (col == 2)
		return "Brown";
	if (col == 3)
		return "LightBlue"; //- OfN was blue
	if (col == 4)
		return "Green";
	if (col == 5)
		return "Red";
	if (col == 6)
		return "Tan";
	if (col == 7)
		return "Pink";
	if (col == 8)
		return "Orange";
	if (col == 9)
		return "Purple";
	if (col == 10)
		return "DarkPurple";
	if (col == 11)
		return "Grey";
	if (col == 12)
		return "Teal";//"DarkGreen";
	if (col == 13)
		return "Yellow";

	return "Blue"; // was DarkBlue
}

//=========================================================================
// Print to the Player's screen a list of all the members of his/her
// team, and the class they've chosen
void TeamFortress_TeamShowMemberClasses(entity Player)
{
	/*local string st;
	st = infokey(world, "no_showmembers");	*/

	entity e;
	float found;

	found = PR_FALSE;

	e = find(world, "classname", "player");
	while (e != world)
	{
		if (((e->team_no == Player->team_no) || (e->team_no == 0)) && (e != Player))
		{
			if (e->model != string_null) // check if valid player
			{
				if (!found)
				{
					found = PR_TRUE;
					sprint (self, PR_PRINT_HIGH, "The other members of your team are:\n");
				}

				sprint (Player, PR_PRINT_HIGH, e->netname);
				sprint (Player, PR_PRINT_HIGH, " : ");
				if (e->playerclass != PR_PC_CUSTOM)
					TeamFortress_PrintClassName(Player,e->playerclass, (e->tfstate & PR_TFSTATE_RANDOMPC));
				else
					TeamFortress_PrintJobName(Player,e->job);

			}
		}
		e = find(e, "classname", "player");
	}

	if (!found)
	{
		sprint (Player, PR_PRINT_HIGH, "There are no other players on your team.\n");
	}
}


void CalculateTeamEqualiser()
{
	float t1, t2, t3, t4;
	float t_ave, calc;

	team1advantage = 1;
	team2advantage = 1;
	team3advantage = 1;
	team4advantage = 1;

	if (number_of_teams < 2)
	{
		// not enought teams!
		// wait a while, then try again (more teams may be created)
		self->nextthink = time + 60;
		return;
	}

	if (teamplay & PR_TEAMPLAY_LESSPLAYERSHELP)
	{

		t1 = TeamFortress_TeamGetNoPlayers(1);
		t2 = TeamFortress_TeamGetNoPlayers(2);
		t3 = TeamFortress_TeamGetNoPlayers(3);
		t4 = TeamFortress_TeamGetNoPlayers(4);

		if ((t1 + t2 + t3 + t4) < 1)  // is there any players at all?
		{
			self->nextthink = time + 30;
			return;
		}

		t_ave = (t1 + t2 + t3 + t4) / number_of_teams;

		// calulate teams equalisation ratio
		if (t1 > 0)
		{
			calc = (t_ave / t1) - 1;

			if (calc != 0)
				calc = (calc / 3) + 1;
			else
				calc = calc + 1;

			team1advantage = calc;
		}

		if (t2 > 0 && number_of_teams >= 2)
		{
			calc = (t_ave / t2) - 1;

			if (calc != 0)
				calc = (calc / 3) + 1;
			else
				calc = calc + 1;

			team2advantage = calc;
		}

		if (t3 > 0 && number_of_teams >= 3)
		{
			calc = (t_ave / t3) - 1;

			if (calc != 0)
				calc = (calc / 3) + 1;
			else
				calc = calc + 1;

			team3advantage = calc;
		}

		if (t4 > 0 && number_of_teams >= 4)
		{
			calc = (t_ave / t4) - 1;

			if (calc != 0)
				calc = (calc / 3) + 1;
			else
				calc = calc + 1;

			team4advantage = calc;
		}
	}

	if (teamplay & PR_TEAMPLAY_LESSSCOREHELP)
	{
		t1 = team1score + PR_TEAM_HELP_RATE;

		if (number_of_teams >= 2)
				t2 = team2score + PR_TEAM_HELP_RATE;
		else
				t2 = 0;

		if (number_of_teams >= 3)
			t3 = team3score + PR_TEAM_HELP_RATE;
		else
			t3 = 0;

		if (number_of_teams >= 4)
			t4 = team4score + PR_TEAM_HELP_RATE;
		else
			t4 = 0;


		// calulate teams equalisation ratio
		t_ave = (t2 + t3 + t4) / (number_of_teams - 1);

		calc = (t_ave / t1) - 1;

		if (calc != 0)
			calc = (calc / 3) + 1;
		else
			calc = calc + 1;

		team1advantage = calc * team1advantage;

		if (number_of_teams >= 2)
		{
			t_ave = (t1 + t3 + t4) / (number_of_teams - 1);

			calc = (t_ave / t2) - 1;

			if (calc != 0)
				calc = (calc / 3) + 1;
			else
				calc = calc + 1;

			team2advantage = calc * team2advantage;
		}

		if (number_of_teams >= 3)
		{
			t_ave = (t2 + t1 + t4) / (number_of_teams - 1);

			calc = (t_ave / t3) - 1;

			if (calc != 0)
				calc = (calc / 3) + 1;
			else
				calc = calc + 1;

			team3advantage = calc * team3advantage;
		}

		if (number_of_teams >= 4)
		{
			t_ave = (t2 + t3 + t1) / (number_of_teams - 1);

			calc = (t_ave / t4) - 1;

			if (calc != 0)
				calc = (calc / 3) + 1;
			else
				calc = calc + 1;

			team4advantage = calc * team4advantage;
		}
	}

	self->nextthink = time + 10;
}


void SetupTeamEqualiser()
{
	team1advantage = 1;
	team2advantage = 1;
	team3advantage = 1;
	team4advantage = 1;

	if (!(teamplay & (PR_TEAMPLAY_LESSPLAYERSHELP | PR_TEAMPLAY_LESSSCOREHELP)))
		return;

	// setup teamplay timer
	entity TE;

	TE = spawnServerSide(); // PZ: make it a server-side only entity
	TE->classname = "Team Equaliser";
	TE->think = CalculateTeamEqualiser;
	TE->nextthink = time + 30;
}


float TeamEqualiseDamage(entity targ, entity attacker, float damage)
{
	if (targ->classname != "player" || attacker->classname != "player")
		return damage;

	if (Teammate(targ, attacker))
		return damage;

	float adv, newdam;

	// increase damage done by attacker's team advantage
	if (attacker->team_no == 1)
		adv = team1advantage;
	else if (attacker->team_no == 2)
		adv = team2advantage;
	else if (attacker->team_no == 3)
		adv = team3advantage;
//	else if (attacker.team_no == 4)
	else
		adv = team4advantage;

	if (adv == 0)
	{
		RPrint("There is a team with an advantage of 0\n");
		adv = 0.1;
	}

	newdam = damage * adv;

	// reduce damage done by target's team advantage
	if (targ->team_no == 1)
		adv = team1advantage;
	else if (targ->team_no == 2)
		adv = team2advantage;
	else if (targ->team_no == 3)
		adv = team3advantage;
//	else if (targ.team_no == 4)
	else
		adv = team4advantage;

	if (adv == 0)
	{
		RPrint("There is a team with an advantage of 0\n");
		adv = 0.1;
	}

	newdam = newdam * (1 / adv);

	return newdam;
}

//=========================================================================
// StatusQuery
void TeamFortress_StatusQuery()
{
	float ft;
	string st;

	sprint (self, PR_PRINT_HIGH, "players per team: ");

	ft = TeamFortress_TeamGetNoPlayers(1);
	st = ftos(ft);
	sprint (self, PR_PRINT_HIGH, st);
	sprint (self, PR_PRINT_HIGH, "  ");
	ft = TeamFortress_TeamGetNoPlayers(2);
	st = ftos(ft);
	sprint (self, PR_PRINT_HIGH, st);

	sprint (self, PR_PRINT_HIGH, "  ");
	ft = TeamFortress_TeamGetNoPlayers(3);
	st = ftos(ft);
	sprint (self, PR_PRINT_HIGH, st);
	sprint (self, PR_PRINT_HIGH, "  ");
	ft = TeamFortress_TeamGetNoPlayers(4);
	st = ftos(ft);
	sprint (self, PR_PRINT_HIGH, st);
	sprint (self, PR_PRINT_HIGH, "\n");

	sprint (self, PR_PRINT_HIGH, "    equalisation: ");

	st = ftos(team1advantage);
	sprint (self, PR_PRINT_HIGH, st);
	sprint (self, PR_PRINT_HIGH, "  ");
	st = ftos(team2advantage);
	sprint (self, PR_PRINT_HIGH, st);
	sprint (self, PR_PRINT_HIGH, "  ");
	st = ftos(team3advantage);
	sprint (self, PR_PRINT_HIGH, st);
	sprint (self, PR_PRINT_HIGH, "  ");
	st = ftos(team4advantage);
	sprint (self, PR_PRINT_HIGH, st);

	sprint (self, PR_PRINT_HIGH, "\n");

	st = ftos(teamplay);
	sprint (self, PR_PRINT_HIGH, "Teamplay is ");
	sprint (self, PR_PRINT_HIGH, st);
	sprint (self, PR_PRINT_HIGH, "\n");

}

//=========================================================================
// Return the illegal classes for this team
int TeamFortress_TeamGetIllegalClasses(float tno)
{
	if (tno == 1)
		return illegalclasses1;
	if (tno == 2)
		return illegalclasses2;
	if (tno == 3)
		return illegalclasses3;
	if (tno == 4)
		return illegalclasses4;

	return 0;
}

//=========================================================================
// Return TRUE if this team is restricted to Civilian class
float TeamFortress_TeamIsCivilian(float tno)
{
	//local entity te;

	if (tno == 1)
	{
		if (civilianteams & PR_TEAM1_CIVILIANS)
			return PR_TRUE;
	}
	else if (tno == 2)
	{
		if (civilianteams & PR_TEAM2_CIVILIANS)
			return PR_TRUE;
	}
	else if (tno == 3)
	{
		if (civilianteams & PR_TEAM3_CIVILIANS)
			return PR_TRUE;
	}
	else // if (tno == 4)
	{
		if (civilianteams & PR_TEAM4_CIVILIANS)
			return PR_TRUE;
	}

	return PR_FALSE;
}

//=========================================================================
// Sprints to all the members on one team except one
void teamsprint(float tno, entity ignore, const string& st)
{
	// Don't do teamprints in DM
	if (tno == 0)
		return;

	entity te;

	te = find(world, "classname", "player");
	while (te != world)
	{
		if (Teammate(te, tno) && te != ignore)
		{
			sprint(te, PR_PRINT_HIGH, st);
		}

		te = find(te, "classname", "player");
	}
}

} // END namespace Progs
