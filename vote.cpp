/*=======================================================//
// Vote.QC - CustomTF 3.2.OfN			  - 04/11/2002 - //
// by Sergio Fumaña Grunwaldt - OfteN [cp]				 //
=========================================================//
 Voting system functions and stuff...
 (Requires PROZAC qwsv, uses special built-ins)
=========================================================*/
// Modified vote system provided by randomer. -Pulseczar (1/15/07)
//=======================================================

#include "progs.h"
#include "vote.h"
#include "vote2.h"       // PZ: Added.
#include "ofndefs.h"
#include "spectate.h"
#include "menu.h"
#include "admin.h"
#include "cpstuff.h"
#include "optimize.h"
#include "debug.h"
#include "custom.h"
#include "neo.h"

namespace Progs {

// PZ: #defines were here. Now in vote.h.

/*===============================================================================================

EXPLANATION OF HOW THE ENTITY FIELDS ARE USED (thnx? np.. :P)
---------------------------------------------

Global vars used:
-----------------

current_voteent   - Points to current vote entity, if any, if not it points to world

For player entity:
------------------

.g_a              - Contains the type of punishment selected on menu
.goal_no          - Controls delay between same client votes
.group_no         - Gizmo - first byte is #VOTE_NEXTMAP, etc.. second byte is one of the #VOTEMAP_ modes above
.goal_state       - Determines what page of maps to be shown for voting, submenu for "votebot" menu
.owned_by         - Contains #VOTED_XXXX, yes, no or none

For vote entity:
----------------

.has_sentry          - Type of vote
.g_a                 - Type of punishment to vote
.admin_kick          - Player to be punished
.all_active          - Number of affirmative votes
.has_holo            - Number of negative votes
.group_no            - Gizmo - one of the #VOTEMAP_ modes above
.has_tesla           - Number of map being voted
.dont_do_triggerwork - If TRUE, means the player to be banned has left
when banning a user that left:
.t_s_h               - Permanent string containing IP to be banned
.t_s_m               - Permanent string with the name of user to be banned
when map voting:
.netname             - Permanent string that is the name of map
                     - PZ: for 'cmd admin poll <question>' netname is <question>

===============================================================================================*/

// PZ: This holds the data for the votemap menu.
MapVoteMenuData mapVoteMenuData;

void PunishClientCycle();
void StartPunishmentVote();
void CleanClientVotes();
void ResetClientsMenu();
float ValidVoteEnt();
float CheckVoting(float final);
void StartVoteMap(float nummap, string themap);
string GetMapString(float themap);
void bprintVotesLeft();
//float() GetRemainingVotes;
void CleanUpSleepVote();
void VoteSleep_think();
void bprintVotesNeeded( float num_for, float num_against);

// External
void CmdMsg(const string& text);

//====================================================================================================

/*
============
IsMapVote

Determines if the current vote is a map related vote.
============
*/
float IsMapVote() {
	if ( current_voteent->has_sentry == PR_VOTE_NEXTMAP )
		return PR_TRUE;
	if ( current_voteent->has_sentry == PR_VOTE_SWITCHMAP )
		return PR_TRUE;

	return PR_FALSE;
}

/*
============
PrintVoteMapType

May print nothing at all.
============
*/
void PrintVoteMapType(const string& preString, float voteMapType, const string& postString)
{
	if      (voteMapType == PR_VOTEMAP_AGR)
	{
		bprint(PR_PRINT_HIGH, preString);
		bprint(PR_PRINT_HIGH, S_("^bAttackers Go Red^b"));
		bprint(PR_PRINT_HIGH, postString);
	}
	else if (voteMapType == PR_VOTEMAP_INVADE)
	{
		bprint(PR_PRINT_HIGH, preString);
		bprint(PR_PRINT_HIGH, S_("^bInvade^b"));
		bprint(PR_PRINT_HIGH, postString);
	}
	else if (voteMapType == PR_VOTEMAP_NEO)
	{
		bprint(PR_PRINT_HIGH, preString);
		bprint(PR_PRINT_HIGH, S_("^bNeo Mode^b"));
		bprint(PR_PRINT_HIGH, postString);
	}
#ifdef PR_COOP_MODE_ENHANCED
	else if ( voteMapType == PR_VOTEMAP_TF ) {
		bprint( PR_PRINT_HIGH, preString );
		bprint( PR_PRINT_HIGH, S_("^bTeam Fortress^b") );
		bprint( PR_PRINT_HIGH, postString );
	} else if ( voteMapType == PR_VOTEMAP_COOP ) {
		bprint( PR_PRINT_HIGH, preString );
		bprint( PR_PRINT_HIGH, S_("^bCoop^b") );
		bprint( PR_PRINT_HIGH, postString );
	} else if ( voteMapType == PR_VOTEMAP_INSANECOOP ) {
		bprint( PR_PRINT_HIGH, preString );
		bprint( PR_PRINT_HIGH, S_("^bInsane Coop^b") );
		bprint( PR_PRINT_HIGH, postString );
	}
#endif
}

//====================================================================================================
// both player and spectators share the same menu handlers for voting, so we need this

void AllResetMenu()
{
	if (self->classname == "spec")
		SpecResetMenu();
	else if (self->classname == "player")
		ResetMenu();
}

void Player_Punish()
{
	if (self->playerclass == PR_PC_UNDEFINED && self->classname == "player") // Observers
		return;

	if (current_voteent != world)
	{
		sprint(self,PR_PRINT_HIGH,"Already running a vote!\n");
		return;
	}

	if (self->goal_no > time)
	{
		sprint(self,PR_PRINT_HIGH,"You just ran a vote!\n");
		return;
	}

	if (self->classname == "player")
		self->current_menu = PR_MENU_PUNISH;
	else if (self->classname == "spec")
		self->current_menu = PR_SPEC_MENU_PUNISH;
	else
		return;
		//RPrint("BUG: Non-spec/player on Player_Punish()\n");

	self->g_a = PR_PUNISH_CURSE;		// Gizmo - fix this stupid bug

	CuTFMenuSound(PR_MENUSOUND_BUY);
	Menu_Punish();
	BackFlash(self,1);
}

void Menu_Punish()
{
	string st, st2, st3;

	// if we didn't disconnect and we are a valid class, player or spectator..
	if (HasValidAdminTarget2(self))
	{
		st2 = self->admin_kick->netname;

		if (self->admin_kick->classname == "spec")
			st=S_("^bUser^b: ^b(^bspectator^b)^b \n\n");
		else
			st=S_("^bUser^b:             \n\n");
	}
	else
	{
		st = "";
		st2 = S_("^bUser^b:             \n\nNo user selected! ");
	}

	if (self->g_a == PR_PUNISH_BAN)
		st3 = S_("^3.. Ban^b!^b          ");
	else if (self->g_a == PR_PUNISH_KICK)
		st3 = S_("^3.. Kick^b!^b         ");
	else if (self->g_a == PR_PUNISH_MUTE)
		st3 = S_("^3.. Mute^b!^b         ");
	else if (self->g_a == PR_PUNISH_CUFF)
		st3 = S_("^3.. Cuff^b!^b         ");
	else
		st3 = S_("^3.. Curse^b!^b        ");

	CenterPrint5(self,st,st2,S_("\n\n^1.. Next User^b...^b  \n\n^2.. ^bChoose Action^b:\n\n"),st3,S_("\n\n^0^b.. Exit^b          \n"));
}

void Menu_Punish_Input(float input)
{
	if (input == 1)
	{
		PunishClientCycle();
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		Menu_Punish();
		self->impulse = 0;
		return;
	}
	else if (input == 2)
	{
		if (self->g_a == PR_PUNISH_CURSE)
			self->g_a = PR_PUNISH_KICK;
		else if (self->g_a == PR_PUNISH_KICK)
			self->g_a = PR_PUNISH_MUTE;
		else if (self->g_a == PR_PUNISH_MUTE)
			self->g_a = PR_PUNISH_BAN;
		else if (self->g_a == PR_PUNISH_BAN)
			self->g_a = PR_PUNISH_CUFF;
		else
			self->g_a = PR_PUNISH_CURSE;

		CuTFMenuSound(PR_MENUSOUND_BUY);
		Menu_Punish();
		self->impulse = 0;
		return;
	}
	else if (input == 3)
	{
		self->impulse = 0;

		if (current_voteent != world)
		{
			sprint(self,PR_PRINT_HIGH,"Already running a vote!\n");
			AllResetMenu();
			return;
		}
		else if (self->admin_kick == world)
			sprint(self,PR_PRINT_HIGH,"No user selected!\n");
		else if (!self->admin_kick->is_connected)
		{
			sprint(self,PR_PRINT_HIGH,"User has left the server!\n");
			PunishClientCycle();
		}
		else if (self->admin_kick->classname=="spec" && self->g_a == PR_PUNISH_CURSE)
			sprint(self,PR_PRINT_HIGH,"Can't curse spectators!\n");
		else if (self->admin_kick->classname=="spec" && self->g_a == PR_PUNISH_CUFF)
			sprint(self,PR_PRINT_HIGH,"Can't cuff spectators!\n");
		else if (self->admin_kick->classname != "spec" && self->admin_kick->classname != "player")
			sprint(self,PR_PRINT_HIGH,"Invalid user selected!\n");
		else
		{
			StartPunishmentVote();
			AllResetMenu();
			return;
		}

		CuTFMenuSound(PR_MENUSOUND_WRONG);
	}
	else if (input == 10)
	{
		AllResetMenu();
		CuTFMenuSound(PR_MENUSOUND_SELL);
		BackFlash(self,1);
		self->impulse = 0;
		return;
	}

	self->impulse = 0;
}

void Player_VoteMap()
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
		sprint(self, PR_PRINT_HIGH, "Too early for a map vote!\n");
		return;
	}

	if (self->goal_no > time)
	{
		sprint(self, PR_PRINT_HIGH, "You just ran a vote!\n");
		return;
	}

	if (ServerUpdating())
	{
		sprint(self, PR_PRINT_HIGH, "Server update is taking place, no map changes are allowed!\n");
		return;
	}

	if (self->classname == "player")
		self->current_menu = PR_MENU_VOTEMAP;
	else if (self->classname == "spec")
		self->current_menu = PR_SPEC_MENU_VOTEMAP;
	else
		return;
	//RPrint("BUG: Non-spec/player on Player_VoteMap()\n");

	// PZ: Update the list of maps.
	static float timeLastUpdated = 0; // PZ: Added.
	// if 2 seconds have passed since last update of the list, allow update
	float timeDiff = abs(time - timeLastUpdated); // doing this because static variable won't ever reset to 0
	if (timeLastUpdated == 0 || timeDiff >= 2)
	{
		timeLastUpdated = time;
		int file = fopen("votemaplist.txt", PR_FILE_READ);
		if (file >= 0)
		{
			mapVoteMenuData.mapList.clear();
			string fileName = "";
			while ((fileName = fgets(file)) != "")
				mapVoteMenuData.mapList.push_back(fileName);
			std::sort(mapVoteMenuData.mapList.begin(), mapVoteMenuData.mapList.end());
			mapVoteMenuData.numOfVoteMapPages = ceil(mapVoteMenuData.mapList.size() / 6.0);
			fclose(file);
		}
	}

	// Gizmo
	self->group_no = PR_VOTE_SWITCHMAP; //#VOTE_NEXTMAP; // PZ: make it default to switch now

	CuTFMenuSound(PR_MENUSOUND_BUY);
	Menu_VoteMap();
	BackFlash(self, 1);
}

void Menu_VoteMap()
{
	string paddedstr, st1, st2, st3, st4, st5, st6, st7, keyst, color;
	float  voteMapType;

	keyst = GetMapString(self->goal_state*6+1);
	if (mapname == keyst)
	{
		color = colstr(keyst, PR_COLSTR_RED);
		if (self->PR_cprint_fx & 1)
			keyst = strcat(color,S_(" [^bplaying^b]"));
		else
			keyst = strcat(color,S_(" ^b[^bplaying^b]^b"));
		color = keyst;
	}
	else
		color = keyst;
	paddedstr = padstr(color, 18);
	/* PZ: Made this so that it can work with any number of pages.
	if (self->goal_state == 0)
		st1 = strcat(S_("^bAvailable maps^b: ^b(^bpage 1^b/^b6^b)^b\n\n^1.. "), paddedstr);
	else if (self->goal_state == 1)
		st1 = strcat(S_("^bAvailable maps^b: ^b(^bpage 2^b/^b6^b)^b\n\n^1.. "), paddedstr);
	else if (self->goal_state == 2)
		st1 = strcat(S_("^bAvailable maps^b: ^b(^bpage 3^b/^b6^b)^b\n\n^1.. "), paddedstr);
	else if (self->goal_state == 3)
		st1 = strcat(S_("^bAvailable maps^b: ^b(^bpage 4^b/^b6^b)^b\n\n^1.. "), paddedstr);
	else if (self->goal_state == 4)
		st1 = strcat(S_("^bAvailable maps^b: ^b(^bpage 5^b/^b6^b)^b\n\n^1.. "), paddedstr);
	else
		st1 = strcat(S_("^bAvailable maps^b: ^b(^bpage 6^b/^b6^b)^b\n\n^1.. "), paddedstr);*/
	// PZ: New way:
	st1 = strcat(S_("^bAvailable maps^b: ^b(^bpage @1^b/^b@2^b)^b\n\n^1.. "), paddedstr);
	replaceStr(st1, "@1", ftos(self->goal_state + 1));
	replaceStr(st1, "@2", to_string(mapVoteMenuData.numOfVoteMapPages));

	keyst = GetMapString(self->goal_state*6+2);
	if (mapname == keyst)
	{
		color = colstr(keyst,PR_COLSTR_RED);
		if (self->PR_cprint_fx & 1)
			keyst = strcat(color,S_(" [^bplaying^b]"));
		else
			keyst = strcat(color,S_(" ^b[^bplaying^b]^b"));
		color = keyst;
	}
	else
		color = keyst;
	paddedstr = padstr(color,18);
	st2 = strcat(S_("\n^2.. "),paddedstr);

	keyst = GetMapString(self->goal_state*6+3);
	if (mapname == keyst)
	{
		color = colstr(keyst,PR_COLSTR_RED);
		if (self->PR_cprint_fx & 1)
			keyst = strcat(color,S_(" [^bplaying^b]"));
		else
			keyst = strcat(color,S_(" ^b[^bplaying^b]^b"));
		color = keyst;
	}
	else
		color = keyst;
	paddedstr = padstr(color,18);
	st3 = strcat(S_("\n^3.. "),paddedstr);

	keyst = GetMapString(self->goal_state*6+4);
	if (mapname == keyst)
	{
		color = colstr(keyst,PR_COLSTR_RED);
		if (self->PR_cprint_fx & 1)
			keyst = strcat(color,S_(" [^bplaying^b]"));
		else
			keyst = strcat(color,S_(" ^b[^bplaying^b]^b"));
		color = keyst;
	}
	else
		color = keyst;
	paddedstr = padstr(color,18);
	st4 = strcat(S_("\n^4.. "),paddedstr);

	keyst = GetMapString(self->goal_state*6+5);
	if (mapname == keyst)
	{
		color = colstr(keyst,PR_COLSTR_RED);
		if (self->PR_cprint_fx & 1)
			keyst = strcat(color,S_(" [^bplaying^b]"));
		else
			keyst = strcat(color,S_(" ^b[^bplaying^b]^b"));
		color = keyst;
	}
	else
		color = keyst;
	paddedstr = padstr(color,18);
	st5 = strcat(S_("\n^5.. "),paddedstr);

	keyst = GetMapString(self->goal_state*6+6);
	if (mapname == keyst)
	{
		color = colstr(keyst,PR_COLSTR_RED);
		if (self->PR_cprint_fx & 1)
			keyst = strcat(color,S_(" [^bplaying^b]"));
		else
			keyst = strcat(color,S_(" ^b[^bplaying^b]^b"));
		color = keyst;
	}
	else
		color = keyst;
	paddedstr = padstr(color,18);
	st6 = strcat(S_("\n^6.. "),paddedstr);

	// Gizmo
	voteMapType = GetByte2( self->group_no );

	// Gizmo - modified AGR vote code
	if ( GetByte1( self->group_no ) == PR_VOTE_NEXTMAP ) {
		if      ( voteMapType == PR_VOTEMAP_AGR )
			st7 =S_("\n\n^7.. ^bMode^b: as next map \n              ^bAttackers Go Red^b\n\n^8.. ^bPrev Page^b         \n^9.. ^bNext Page^b         \n\n^0^b.. Exit^b              \n");
		else if ( voteMapType == PR_VOTEMAP_INVADE )
			st7 =S_("\n\n^7.. ^bMode^b: as next map \n              ^bInvade^b          \n\n^8.. ^bPrev Page^b         \n^9.. ^bNext Page^b         \n\n^0^b.. Exit^b              \n");
		else if ( voteMapType == PR_VOTEMAP_NEO )
			st7 =S_("\n\n^7.. ^bMode^b: as next map \n              ^bNeo Mode^b        \n\n^8.. ^bPrev Page^b         \n^9.. ^bNext Page^b         \n\n^0^b.. Exit^b              \n");
#ifdef PR_COOP_MODE_ENHANCED
		else if ( voteMapType == PR_VOTEMAP_TF )
			st7 =S_("\n\n^7.. ^bMode^b: as next map \n              ^bTeam Fortress^b   \n\n^8.. ^bPrev Page^b         \n^9.. ^bNext Page^b         \n\n^0^b.. Exit^b              \n");
		else if ( voteMapType == PR_VOTEMAP_COOP )
			st7 =S_("\n\n^7.. ^bMode^b: as next map \n              ^bCoop^b            \n\n^8.. ^bPrev Page^b         \n^9.. ^bNext Page^b         \n\n^0^b.. Exit^b              \n");
		else if ( voteMapType == PR_VOTEMAP_INSANECOOP )
			st7 =S_("\n\n^7.. ^bMode^b: as next map \n              ^bInsane Coop^b     \n\n^8.. ^bPrev Page^b         \n^9.. ^bNext Page^b         \n\n^0^b.. Exit^b              \n");
#endif
		else
			st7 =S_("\n\n^7.. ^bMode^b: as next map \n\n^8.. ^bPrev Page^b         \n^9.. ^bNext Page^b         \n\n^0^b.. Exit^b              \n");
	} else { //if ( self.group_no == #VOTE_SWITCHMAP )
		if      ( voteMapType == PR_VOTEMAP_AGR )
			st7 =S_("\n\n^7.. ^bMode^b: play now^b!^b   \n              ^bAttackers Go Red^b\n\n^8.. ^bPrev Page^b         \n^9.. ^bNext Page^b         \n\n^0^b.. Exit^b              \n");
		else if ( voteMapType == PR_VOTEMAP_INVADE )
			st7 =S_("\n\n^7.. ^bMode^b: play now^b!^b   \n              ^bInvade^b          \n\n^8.. ^bPrev Page^b         \n^9.. ^bNext Page^b         \n\n^0^b.. Exit^b              \n");
		else if ( voteMapType == PR_VOTEMAP_NEO )
			st7 =S_("\n\n^7.. ^bMode^b: play now^b!^b   \n              ^bNeo Mode^b        \n\n^8.. ^bPrev Page^b         \n^9.. ^bNext Page^b         \n\n^0^b.. Exit^b              \n");
#ifdef PR_COOP_MODE_ENHANCED
		else if ( voteMapType == PR_VOTEMAP_TF )
			st7 =S_("\n\n^7.. ^bMode^b: play now^b!^b   \n              ^bTeam Fortress^b   \n\n^8.. ^bPrev Page^b         \n^9.. ^bNext Page^b         \n\n^0^b.. Exit^b              \n");
		else if ( voteMapType == PR_VOTEMAP_COOP )
			st7 =S_("\n\n^7.. ^bMode^b: play now^b!^b   \n              ^bCoop^b            \n\n^8.. ^bPrev Page^b         \n^9.. ^bNext Page^b         \n\n^0^b.. Exit^b              \n");
		else if ( voteMapType == PR_VOTEMAP_INSANECOOP )
			st7 =S_("\n\n^7.. ^bMode^b: play now^b!^b   \n              ^bInsane Coop^b     \n\n^8.. ^bPrev Page^b         \n^9.. ^bNext Page^b         \n\n^0^b.. Exit^b              \n");
#endif
		else
			st7 =S_("\n\n^7.. ^bMode^b: play now^b!^b   \n\n^8.. ^bPrev Page^b         \n^9.. ^bNext Page^b         \n\n^0^b.. Exit^b              \n");
	}

	CenterPrint7(self,st1,st2,st3,st4,st5,st6,st7);

	self->PR_cprint_fx = self->PR_cprint_fx + 1;
}

void Menu_VoteMap_Input(float input)
{
	float	voteType, voteMapType;

	if (input >= 1 && input <= 6)
	{
		if (current_voteent != world)
		{
			sprint(self,PR_PRINT_HIGH,"Already running a vote!\n");
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
			self->goal_state = mapVoteMenuData.numOfVoteMapPages - 1;  // PZ: was = 5

		CuTFMenuSound(PR_MENUSOUND_BROWSE);

		NoCprintFX();
		Menu_VoteMap();
	}
	else if (input == 9)
	{
		self->goal_state = self->goal_state + 1;
		if (self->goal_state > (mapVoteMenuData.numOfVoteMapPages - 1))  // PZ: was > 5
			self->goal_state = 0;

		CuTFMenuSound(PR_MENUSOUND_BROWSE);

		NoCprintFX();
		Menu_VoteMap();
	}
	else if (input == 10)
	{
		AllResetMenu();
		CuTFMenuSound(PR_MENUSOUND_SELL);
		BackFlash(self,1);
	}

	self->impulse = 0;
}

//===============================================================
// bprints the current voting issue

void bprintVoteIssue()
{
	if (current_voteent->has_sentry == PR_VOTE_PUNISH)
	{
		if (current_voteent->dont_do_triggerwork) // Special ban (client has left)
		{
			bprint(PR_PRINT_HIGH,S_(" ^b(^bban "));
			bprint(PR_PRINT_HIGH,current_voteent->t_s_m);
			bprint(PR_PRINT_HIGH,S_("^b)^b"));
		}
		else
		{
			if (current_voteent->g_a == PR_PUNISH_CURSE)
				bprint(PR_PRINT_HIGH,S_(" ^b(^bcurse "));
			else if (current_voteent->g_a == PR_PUNISH_KICK)
				bprint(PR_PRINT_HIGH,S_(" ^b(^bkick "));
			else if (current_voteent->g_a == PR_PUNISH_BAN)
				bprint(PR_PRINT_HIGH,S_(" ^b(^bban "));
			else if (current_voteent->g_a == PR_PUNISH_MUTE)
				bprint(PR_PRINT_HIGH,S_(" ^b(^bmute "));
			else if (current_voteent->g_a == PR_PUNISH_CUFF)
				bprint(PR_PRINT_HIGH,S_(" ^b(^bcuff "));

			bprint(PR_PRINT_HIGH,current_voteent->admin_kick->netname);
			bprint(PR_PRINT_HIGH,S_("^b)^b"));
		}
	}
	else if (current_voteent->has_sentry == PR_VOTE_NEXTMAP) // as next map
	{
		bprint(PR_PRINT_HIGH,S_(" ^b(^b")); // FIXME: CPQCCX compiler says 'Not a name' if only "(" is used ???
		bprint(PR_PRINT_HIGH, current_voteent->netname);
		bprint(PR_PRINT_HIGH," as next map");

		PrintVoteMapType( ", in ", current_voteent->group_no, " mode" );

		bprint( PR_PRINT_HIGH, S_("^b)^b") );
	}
	else if (current_voteent->has_sentry == PR_VOTE_SWITCHMAP)
	{
		bprint(PR_PRINT_HIGH,S_(" ^b(^bswitch to "));
		bprint(PR_PRINT_HIGH,current_voteent->netname);

		PrintVoteMapType( ", in ", current_voteent->group_no, " mode" );

		bprint( PR_PRINT_HIGH, S_("^b)^b") );
	} else if ( current_voteent->has_sentry == PR_VOTE_USEOBJECT ) {
		string objectName;

		objectName = GetUseObjectName( current_voteent->enemy );
		bprint( PR_PRINT_HIGH, S_(" ^b(^buse object ") );
		bprint( PR_PRINT_HIGH, objectName );
		bprint( PR_PRINT_HIGH, S_("^b)^b") );
	}
}

//==============================================================
// user has used the VoteYes impulse command

void Player_VoteYes()
{
	if (self->playerclass == PR_PC_UNDEFINED && self->classname == "player") // Observers
		return;

	if (current_voteent == world)
	{
		sprint(self,PR_PRINT_HIGH,"Not voting currently!\n");
		return;
	}

	// PZ: I don't see a reason to not allow them to vote. I made them automatically vote no to their own punishment.
	/* if (current_voteent.admin_kick == self)
	{
		sprint(self,#PRINT_HIGH,"You are not allowed to vote!\n");
		return;
	} */

	if (self->owned_by == PR_VOTED_NO || self->owned_by == PR_VOTED_YES)
	{
		// FIXME: Allow the person to change their vote. -randomer 2006-02-24
		sprint(self,PR_PRINT_HIGH,"You already voted!\n");
		return;
	}

	// PZ: Don't allow more than one vote from the same IP. People can just connect with multiple clients or computers to vote punish people.
	//     Apologies to the legitimate players sharing an IP behind a router. Blame Doc for vote banning Decimus using two clients.
	string myIP, theirIP;   entity player;   float num;
	myIP = infokey(self, "ip");
	myIP = strcat(myIP, ""); // This is simply to get around Quake string buffer issue -- a problem which I don't completely understand at the moment.
//bprint(2, "myIP = ", myIP, "\n");
	player = nextent(world);
	num = 0;
	while (num < 32)
	{
		theirIP = infokey(player, "ip");
		theirIP = strcat(theirIP, ""); // This is simply to get around Quake string buffer issue -- a problem which I don't completely understand at the moment.
//bprint(2, "theirIP = ", theirIP, "\n");
		if (theirIP == myIP && (player->owned_by == PR_VOTED_NO || player->owned_by == PR_VOTED_YES) && player->is_connected && player->ishuman)
		{
			sprint(self, PR_PRINT_HIGH, "Someone with your IP address already voted!\n");
			return;
		}
		player = nextent(player);
		num = num + 1;
	}

	// Passed checks. Cast the vote.
	BackFlash(self,1);

	bprint(PR_PRINT_HIGH,"Voting\x8D ");
	bprint(PR_PRINT_HIGH,self->netname);
	bprint(PR_PRINT_HIGH," agrees");

	self->owned_by = PR_VOTED_YES;

	bprintVoteIssue();

	BroadcastSound("misc/basekey");

	current_voteent->all_active = current_voteent->all_active + 1;

	if (current_voteent->nextthink < time + PR_VOTE_TIME_REFRESH)
		current_voteent->nextthink = time + PR_VOTE_TIME_REFRESH;

	bprintVotesLeft();
	CheckVoting(PR_FALSE);
}

//==============================================================
// user has used the VoteNo impulse command
// PZ NOTE: Why didn't they just make one function and pass it a 'yes' or 'no'?
void Player_VoteNo()
{
	if (self->playerclass == PR_PC_UNDEFINED && self->classname == "player") // Observers
		return;

	if (current_voteent == world)
	{
		sprint(self,PR_PRINT_HIGH,"Not voting currently!\n");
		return;
	}

	// PZ: I don't see a reason to not allow them to vote. I made them automatically vote no to their own punishment.
	/* if (current_voteent.admin_kick == self)
	{
		sprint(self,#PRINT_HIGH,"You are not allowed to vote!\n");
		return;
	} */

	if (self->owned_by == PR_VOTED_NO || self->owned_by == PR_VOTED_YES)
	{
		sprint(self,PR_PRINT_HIGH,"You already voted!\n");
		return;
	}

	// PZ: Don't allow more than one vote from the same IP. People can just connect with multiple clients or computers to vote punish people.
	//     Apologies to the legitimate players sharing an IP behind a router. Blame Doc for vote banning Decimus using two clients.
	string myIP, theirIP;   entity player;   float num;
	myIP = infokey(self, "ip");
	myIP = strcat(myIP, ""); // This is simply to get around Quake string buffer issue -- a problem which I don't completely understand at the moment.
//bprint(2, "myIP = ", myIP, "\n");
	player = nextent(world);
	num = 0;
	while (num < 32)
	{
		theirIP = infokey(player, "ip");
		theirIP = strcat(theirIP, ""); // This is simply to get around Quake string buffer issue -- a problem which I don't completely understand at the moment.
//bprint(2, "theirIP = ", theirIP, "\n");
		if (theirIP == myIP && (player->owned_by == PR_VOTED_NO || player->owned_by == PR_VOTED_YES) && player->is_connected && player->ishuman)
		{
			sprint(self, PR_PRINT_HIGH, "Someone with your IP address already voted!\n");
			return;
		}
		player = nextent(player);
		num = num + 1;
	}

	// Passed checks. Cast the vote.
	BackFlash(self,1);

	bprint(PR_PRINT_HIGH,"Voting\x8D ");
	bprint(PR_PRINT_HIGH,self->netname);
	bprint(PR_PRINT_HIGH," disagrees");

	self->owned_by = PR_VOTED_NO;

	bprintVoteIssue();

	BroadcastSound("doors/basetry.wav"); // PZ - was misc/basekey just like voteyes
	// changed to differentiate vote sounds from affirmative to negative

	current_voteent->has_holo = current_voteent->has_holo + 1;

	if (current_voteent->nextthink < time + PR_VOTE_TIME_REFRESH)
		current_voteent->nextthink = time + PR_VOTE_TIME_REFRESH;

	bprintVotesLeft();
	CheckVoting(PR_FALSE);
}

//========================================================================
// cycles through all clients as target for punishment

void PunishClientCycle()
{
	entity te;
	float num;
	num = PR_FALSE;

	te = world; // none

	te = find(self->admin_kick, "classname", "player");
	while (te != world && num == PR_FALSE)
	{
		num = PR_TRUE;

		if (!te->is_connected)
			num = PR_FALSE;

		if (self->admin_kick==te)
			num = PR_FALSE;

		#ifndef PR_VOTING_TEST
		if (te == self)
			num = PR_FALSE;
		#endif

		if (num == PR_FALSE) te = find(te, "classname", "player");
	}

	if (te == world) // if out of players on our search lets scan for spectators
	{
		te = find(self->admin_kick, "classname", "spec");
		while (te != world && num == PR_FALSE)
		{
			num = PR_TRUE;

			if (!te->is_connected)
				num = PR_FALSE;

			if (self->admin_kick==te)
				num = PR_FALSE;

			#ifndef PR_VOTING_TEST
			if (te == self)
				num = PR_FALSE;
			#endif

			if (num == PR_FALSE) te = find(te, "classname", "spec");
		}
	}

	if (te == world)
		self->admin_kick=world; // No clients found!
	else
		self->admin_kick = te;
}

//========================================================================
// deletes current vote entity, if it exists, and performs clean up

void ResetVoteEnt()
{
	if (current_voteent->classname == "voteent")
	{
		if (IsMapVote() || current_voteent->has_sentry == PR_VOTE_POLL)
			delstr(current_voteent->netname);

		if (current_voteent->dont_do_triggerwork)
		{
			delstr(current_voteent->t_s_h);
			delstr(current_voteent->t_s_m);
		}

		dremove(current_voteent);
		current_voteent = world;
	}
	else current_voteent = world;
}

//==========================================================================
// gets number of clients currently on server, including spectators

float GetNoClients()
{
	return num_players + num_specs;

	/*local entity te;
	local float clients;
	clients = 0;

	te = find(world, classname, "player");
	while (te != world)
	{
		if (!te.has_disconnected)
			clients = clients + 1;

		te = find(te, classname, "player");
	}

	te = find(world, classname, "spec");
	while (te != world)
	{
		if (!te.has_disconnected)
			clients = clients + 1;

		te = find(te, classname, "spec");
	}

	return clients;*/
}

void PunishVote_Think();

//=================================================================
// Start punishment voting, self = player issuer of vote

void StartPunishmentVote()
{
	if (no_punish)
	{
		CmdMsg("Punishment votes are disabled, sorry!\n");
		return;
	}

	if (self->classname != "player" && self->classname != "spec")
	{
		sprint(self,PR_PRINT_HIGH,"You can't vote still, join the game first!\n");
		return;
	}

	if (self->admin_kick->admin_flag) // PZ: why was this never added?
	{
		sprint(self, PR_PRINT_HIGH, "You can't vote to punish admins!\n");
		return;
	}

	if (mutedtime(self))
	{
		sprint(self,PR_PRINT_HIGH,"You can't start a vote while muted!\n");
		return;
	}

	if (!self->admin_kick->ishuman && self->g_a == PR_PUNISH_BAN) // PZ: banning bots causes a ban on 0.0.0.0, which seems to ban all players
	{
		sprint(self, PR_PRINT_HIGH, "You can't vote to ban bots.\n");
		return;
	}

	BackFlash(self,1);

	CleanClientVotes();

	entity voteent;

	voteent = spawnServerSide(); // PZ: make it a server-side only entity
	voteent->g_a = self->g_a;
	voteent->think = PunishVote_Think;
	voteent->nextthink = time + PR_VOTE_TIME;
	voteent->classname = "voteent";
	voteent->owner = self;       // PZ: was = world

	voteent->all_active = 1;
	voteent->has_holo = 0;

	voteent->demon_one = self;

	voteent->admin_kick = self->admin_kick;

	voteent->has_sentry = PR_VOTE_PUNISH;

	self->goal_no = time + PR_VOTE_DELAY;

	self->owned_by = PR_VOTED_YES; // we already voted yes
	// PZ: Don't know why players weren't being allowed to vote against punishment
	// towards them. This fixes players in a 2 player game being able to ban one another
	// automatically by just casting a vote.
	voteent->admin_kick->owned_by = PR_VOTED_NO;

	BroadcastSound("misc/medkey");

	bprint(PR_PRINT_HIGH,"Voting\x8D ");

	bprint(PR_PRINT_HIGH,self->netname);
	last_vote_starter = self->netname; // for AGR -PZ
	bprint(PR_PRINT_HIGH," proposes to ");

	if (self->g_a == PR_PUNISH_CURSE)
		bprint(PR_PRINT_HIGH,"curse ");
	else if (self->g_a == PR_PUNISH_KICK)
		bprint(PR_PRINT_HIGH,"kick ");
	else if (self->g_a == PR_PUNISH_MUTE)
		bprint(PR_PRINT_HIGH,"mute ");
	else if (self->g_a == PR_PUNISH_BAN)
		bprint(PR_PRINT_HIGH,"ban ");
	else if (self->g_a == PR_PUNISH_CUFF)
		bprint(PR_PRINT_HIGH,"cuff ");

	bprint(PR_PRINT_HIGH,self->admin_kick->netname);

	bprint(PR_PRINT_HIGH,S_(", type ^b\"^bVoteYes^b\"^b or ^b\"^bVoteNo^b\"^b to vote"));
	// Gizmo
	bprintVotesNeeded( 1, 0 );
	bprint( PR_PRINT_HIGH, "\n" );

	current_voteent = voteent;

	current_voteent->dont_do_triggerwork = PR_FALSE;

	ResetClientsMenu(); // reset menus on players/specs which r on MENU_PUNISH or MENU_VOTEMAP

	CheckVoting(PR_FALSE); // If we are the only player on server, checks
}

void PunishVote_Think()
{
	CheckVoting(PR_TRUE);

#ifdef PR_NOTDEFINED
	bprint(PR_PRINT_HIGH,"Voting\x8D Punishment vote has failed");
	bprintVoteIssue();
	bprint(PR_PRINT_HIGH,"\n");

	BroadcastSound("misc/runekey");

	ResetVoteEnt();
#endif
}

void VoteMap_Think();

//=================================================================
// Start map voting, self = player issuer of vote

void StartVoteMap(float nummap, string themap)
{
	string	temp;
	float		voteType, voteMapType;

	if (no_votemap)
	{
		CmdMsg("Map voting within the mod is disabled, sorry!\n");
		return;
	}

	if (self->classname != "player" && self->classname != "spec")
	{
		sprint(self,PR_PRINT_HIGH,"You can't vote still, join the game first!\n");
		return;
	}

	if (time < PR_VOTE_MIN_TIME)
#ifdef PR_COOP_MODE_ENHANCED
	if ( deathmatch || mapname != "start" )
#endif
	{
		sprint(self,PR_PRINT_HIGH,"Too early for a map change!\n");
		return;
	}

	if (mutedtime(self))
	{
		sprint(self,PR_PRINT_HIGH,"You can't start a vote while muted!\n");
		return;
	}

	if (ServerUpdating())
	{
		sprint(self,PR_PRINT_HIGH,"Server update is taking place, no map changes are allowed!\n");
		return;
	}

	voteType = GetByte1( self->group_no );
	voteMapType = GetByte2( self->group_no );

	// PZ - AGR begin
	if (voteMapType == PR_VOTEMAP_AGR)
	if (voteType == PR_VOTE_NEXTMAP || voteType == PR_VOTE_SWITCHMAP)
	{
		float x;
		x = GetNoPlayers(PR_TRUE, PR_TRUE);
		if (agr_vote_max != 0 && x > agr_vote_max)
		{
			sprint(self, PR_PRINT_HIGH, S_("Voting\x8D Max number of players allowed for an ^bAGR^b vote is "));
			temp = ftos(agr_vote_max);
			temp = colstr(temp, PR_COLSTR_NUMBER);
			sprint(self, PR_PRINT_HIGH, temp);
			sprint(self, PR_PRINT_HIGH, " players.\n");
			return;
		}
	}
	// AGR end

	BackFlash(self,1);

	entity voteent;

	voteent = spawnServerSide(); // PZ: make it a server-side only entity
	voteent->g_a = self->g_a;
	voteent->think = VoteMap_Think;
	voteent->nextthink = time + PR_VOTE_TIME;
	voteent->classname = "voteent";
	voteent->owner = self;       // PZ: was = world

	if (!nummap)
	{
		voteent->netname = themap;

		#ifdef PR_UNABLE_TOVOTE_FORSAME_MAP
		// Don't let votes to switch to current map
		// PZ ..unless they are changing to/from AGR
		if (voteent->netname == mapname && voteType == PR_VOTE_SWITCHMAP)
		if (!((voteMapType == PR_VOTEMAP_AGR && !agr) || (voteMapType != PR_VOTEMAP_AGR && agr)))
		if (!((voteMapType == PR_VOTEMAP_INVADE && !invade) || (voteMapType != PR_VOTEMAP_INVADE && invade)))
		if (!((voteMapType == PR_VOTEMAP_NEO && !neo.isModeActive()) || (voteMapType != PR_VOTEMAP_NEO && neo.isModeActive())))
		{
			sprint(self,PR_PRINT_HIGH,"You can't start voting for a map that is currently being played!\n");
			CuTFMenuSound(PR_MENUSOUND_WRONG);
			dremove(voteent);
			voteent = world;
			return;
		}
		#endif

		voteent->netname = makestr(voteent->netname);
		voteent->has_tesla = 0;
	}
	else
	{
		voteent->netname = GetMapString(nummap);

		// Abort if empty map name on localinfo
		if (voteent->netname == "")
		{
			sprint(self,PR_PRINT_HIGH,"You can't choose an empty map slot!\n");
			CuTFMenuSound(PR_MENUSOUND_WRONG);
			dremove(voteent);
			voteent = world;
			return;
		}

		#ifdef PR_UNABLE_TOVOTE_FORSAME_MAP
		// Don't let votes to switch to current map
		// PZ ..unless they are changing to/from AGR
		if (voteent->netname == mapname && voteType == PR_VOTE_SWITCHMAP)
		if (!((voteMapType == PR_VOTEMAP_AGR && !agr) || (voteMapType != PR_VOTEMAP_AGR && agr)))
		if (!((voteMapType == PR_VOTEMAP_INVADE && !invade) || (voteMapType != PR_VOTEMAP_INVADE && invade)))
		if (!((voteMapType == PR_VOTEMAP_NEO && !neo.isModeActive()) || (voteMapType != PR_VOTEMAP_NEO && neo.isModeActive())))
		{
			sprint(self,PR_PRINT_HIGH,"You can't start voting for a map that is currently being played!\n");
			CuTFMenuSound(PR_MENUSOUND_WRONG);
			dremove(voteent);
			voteent = world;
			return;
		}
		#endif

		voteent->netname = makestr(voteent->netname);
		voteent->has_tesla = nummap;
	}

	CleanClientVotes();

	voteent->all_active = 1;
	voteent->has_holo = 0;

	// Gizmo - check for vote nextmap switchmap
	voteent->has_sentry = voteType;		// has_sentry is the generic vote type field

	current_voteent = voteent;

	self->goal_no = time + PR_VOTE_DELAY;

	self->owned_by = PR_VOTED_YES; // we already voted yes

	voteent->group_no = voteMapType;		// group_no on the vote entity is the map vote type

	current_voteent->dont_do_triggerwork = PR_FALSE;

	BroadcastSound("misc/medkey");

	themap = colstr(current_voteent->netname,PR_COLSTR_RED);

	bprint(PR_PRINT_HIGH,"Voting\x8D ",self->netname);
	last_vote_starter = self->netname; // for AGR -PZ

	if ( voteType == PR_VOTE_NEXTMAP ) {
		bprint(PR_PRINT_HIGH," suggests ",themap," as next map");

		PrintVoteMapType( ", in ", voteMapType, " mode" );
	} else /*if ( voteType == #VOTE_SWITCHMAP )*/ {
		bprint(PR_PRINT_HIGH," wants to play ",themap," right now");

		PrintVoteMapType( ", in ", voteMapType, " mode" );
	}

	bprint(PR_PRINT_HIGH,S_(", type ^b\"^bVoteYes^b\"^b or ^b\"^bVoteNo^b\"^b to vote"));
	// Gizmo
	bprintVotesNeeded( 1, 0 );
	bprint( PR_PRINT_HIGH, "\n" );

	ResetClientsMenu(); // reset menus on players/specs which are on MENU_PUNISH or MENU_VOTEMAP

	CheckVoting(PR_FALSE); // If we are the only player on server must check
}

// PZ NOTE: If you change this function, please bear in mind that "cmd admin poll" and "votebot" use this function.
void VoteMap_Think()
{
	CheckVoting(PR_TRUE);

#ifdef PR_NOTDEFINED
	bprint(PR_PRINT_HIGH,"Voting\x8D Map voting failed");

	bprintVoteIssue();

	bprint(PR_PRINT_HIGH,"\n");

	BroadcastSound("misc/runekey");

	ResetVoteEnt();
#endif
}

/*
===============
StartPoll - Pulseczar (PZ)

Polls players on any given question, for admin to use poll result accordingly.
Works just like normal votes, except it doesn't do anything after voting is over.

Examples:
    /cmd admin poll "Extend map time by 10 minutes?"
===============
*/
void StartPoll(const string& question)
{
	if (current_voteent != world)
	{
		sprint(self, PR_PRINT_HIGH, "Already running a vote!\n");
		return;
	}

	float n;
	n = strlen(question);
	if (n >= 1024) // permanent strings are limited to 30 characters (want to up that) // PZ: perm strings are now 1024 chars
	{
		sprint(self, PR_PRINT_HIGH, "Sorry. Poll questions are currently limited to 1023 characters.\n");
		return;
	}

	BackFlash(self, 1);

	entity voteent;
	string str;

	voteent = spawnServerSide();    // PZ: make it a server-side only entity
	voteent->netname = makestr(question);
	voteent->think = VoteMap_Think; // PZ: we'll just use that function for right now
	voteent->nextthink = time + PR_VOTE_TIME;
	voteent->classname = "voteent";
	voteent->owner = self;          // PZ: was = world

	last_vote_starter = self->netname; // for AGR -PZ

	CleanClientVotes();

	voteent->all_active = 0; // in this case, vote starter doesn't count as affirmative vote
                            // vote starter can also vote in poll
	voteent->has_holo = 0;

	voteent->has_sentry = PR_VOTE_POLL; // has_sentry is the generic vote type field

	current_voteent = voteent;

	current_voteent->dont_do_triggerwork = PR_FALSE; // player to be banned has left (false)

	BroadcastSound("misc/medkey");

	str = colstr(question, PR_COLSTR_RED);

	bprint(PR_PRINT_HIGH,"Voting\x8D Admin, ", self->netname);
	bprint(PR_PRINT_HIGH,", begins poll: ");
	bprint(PR_PRINT_HIGH, str);
	bprint(PR_PRINT_HIGH,S_(", type ^b\"^bVoteYes^b\"^b or ^b\"^bVoteNo^b\"^b to vote"));
	bprintVotesNeeded(0, 0);
	bprint(PR_PRINT_HIGH, "\n");

	ResetClientsMenu(); // reset menus on players/specs which are on MENU_PUNISH or MENU_VOTEMAP

	CheckVoting(PR_FALSE); // If we are the only player on server must check
}

//=================================================================

void StartUseObjectVote()
{
	string objectName, objectAction;

	makevectors( self->v_angle );
	traceline( self->origin + V({0, 0, 16}), ( self->origin + V({0, 0, 16}) ) + v_forward * 4096, PR_TL_ANY_SOLID, self );

	if ( !IsValidUseObject( trace_ent ) ) {
		sprint( self, PR_PRINT_HIGH, "Not a valid object to use!\n" );
		return;
	}

	BroadcastSound( "misc/medkey" );

	objectName = GetUseObjectName( trace_ent );
	objectAction = GetUseObjectAction( trace_ent );
	bprint( PR_PRINT_HIGH, "Voting\x8D ", self->netname, " wants to force a ", objectName, " to ", objectAction );
	bprint( PR_PRINT_HIGH, S_(", type ^b\"^bVoteYes^b\"^b or ^b\"^bVoteNo^b\"^b to vote") );
	// Gizmo
	bprintVotesNeeded( 1, 0 );
	bprint( PR_PRINT_HIGH, "\n" );

	BackFlash( self, 1 );
	CleanClientVotes ();

	self->goal_no = time + PR_VOTE_DELAY;
	self->owned_by = PR_VOTED_YES;		// we already voted yes

	entity voteent;

	voteent = spawnServerSide(); // PZ: make it a server-side only entity
	voteent->think = PunishVote_Think;
	voteent->nextthink = time + PR_VOTE_TIME;
	voteent->classname = "voteent";
	voteent->owner = self;       // PZ: was = world
	voteent->all_active = 1;
	voteent->has_holo = 0;
	voteent->enemy = trace_ent;
	voteent->has_sentry = PR_VOTE_USEOBJECT;

	current_voteent = voteent;

	ResetClientsMenu ();			// reset menus on players/specs which r on MENU_PUNISH or MENU_VOTEMAP
	CheckVoting( PR_FALSE );			// if we are the only player on server checks
}


//==================================================================
// resets all clients to ready-to-vote state

void CleanClientVotes()
{
	entity te;

	te = find(world, "classname", "player");
	while (te != world)
	{
		if (te->is_connected)
			te->owned_by = PR_VOTED_READY;

		te = find(te, "classname", "player");
	}

	te = find(world, "classname", "spec");
	while (te != world)
	{
		if (te->is_connected)
			te->owned_by = PR_VOTED_READY;

		te = find(te, "classname", "spec");
	}
}

//=============================================================================
// clear menus of clients that have vote menus on - WATCHOUT - Using self

void ResetClientsMenu()
{
	entity te;

	te = find(world, "classname", "player");
	while (te != world)
	{
		if (te->is_connected)
		if (te != self)                                                               // PZ: Added votebot.
		if (te->current_menu == PR_MENU_PUNISH || te->current_menu == PR_MENU_VOTEMAP /*|| te->current_menu == PR_MENU_VOTEBOT*/)
		{
			sprint(te,PR_PRINT_HIGH,"Already running a vote!\n");
			CenterPrint(te,"\n");
			te->current_menu = PR_MENU_DEFAULT;
		}
		te = find(te, "classname", "player");
	}

	te = find(world, "classname", "spec");
	while (te != world)
	{
		if (te->is_connected)
		if (te != self)
		if (te->current_menu == PR_SPEC_MENU_PUNISH || te->current_menu == PR_SPEC_MENU_VOTEMAP /*||
		    te->current_menu == PR_SPEC_MENU_VOTEBOT*/) // PZ: Added votebot.
		{
			sprint(te,PR_PRINT_HIGH,"Already running a vote!\n");
			CenterPrint(te,"\n");
			te->current_menu = PR_SPEC_MENU_NONE;
		}
		te = find(te, "classname", "spec");
	}
}

//==========================================================================
// called after each vote (yes or no) to see if its a succesfull vote


/*
=============
VotePassed

=============
*/
float VotePassed( float numFor, float numAgainst, float fraction)
{
//	bprint (#PRINT_HIGH, ftos(numAgainst));
//	bprint (#PRINT_HIGH, "<against> + ((", ftos(num_players - (numFor + numAgainst)), ") / 2) == ");

	// get the number of players that didn't vote, and count them as half a voteno
	// don't count spectators if they didn't vote
	if ( num_players - bot_count - ( numFor + numAgainst ) > 0 )                                        // PZ: subtracted bot count
		numAgainst = numAgainst + ( ( num_players - bot_count - ( numFor + numAgainst ) ) * fraction ); // PZ: subtracted bot count

//	bprint (#PRINT_HIGH, ftos(numAgainst), "\n");

	if ( numFor > numAgainst )
		return PR_TRUE;

	return PR_FALSE;
}

// PZ NOTE: One thing this does is count up the votes.
//          Returns true, when voting is finished? Return value never seems to be used, anyway.
float CheckVoting(float final)
{
	// this shouldn't be needed
	if (current_voteent == world || current_voteent->classname != "voteent")
	{
		bprint(PR_PRINT_HIGH,"Voting\x8D Error in voting system!\n");
		ResetVoteEnt();
		return PR_TRUE;
	}

	if (current_voteent->admin_kick->admin_flag) // PZ
	{
		bprint(PR_PRINT_HIGH, "Voting\x8D Ending vote. Punishment target logged in as admin.\n");
		BroadcastSound("misc/runekey");
		ResetVoteEnt();
		return PR_TRUE;
	}

	float num_for, num_against, num_total;
	entity te;

	num_for = 0;
	num_against = 0;
	num_total = 0;

	// Count the number of votes for, against, and total clients.
	te = nextent(world);
	while (te != world)
	{
		if ((te->classname == "player" || te->classname == "spec") && te->ishuman) // PZ: don't count bots
		{
			num_total = num_total + 1;
			if (te->owned_by == PR_VOTED_YES) {
				num_for = num_for + 1;
			} else if (te->owned_by == PR_VOTED_NO) {
				num_against = num_against + 1;
			}
		}
		te = nextent(te);
	}

// GIZMO BEGIN
/*
	// If this is the final check then only count the people who actually voted.
	if (final)
		num_total = num_for + num_against;
*/
	// if this isn't the final check then only do anything if over 50% of the population has voted
//	if (!final && num_for + num_against <= num_total / 2)
	if (!final && num_for <= num_total / 2 && num_against <= num_total / 2)
		return PR_FALSE;
// GIZMO END

	// If an update is taking place, dont allow map voting
	if (IsMapVote())
	if (ServerUpdating())
	{
		bprint(PR_PRINT_HIGH,"Voting\x8D Server update is taking place, no map changes are allowed\n");
		ResetVoteEnt();
		return PR_TRUE;
	}

	//float numclients;
	string st;
	float uid;

	//numclients = GetNoClients();

	if (current_voteent->has_sentry == PR_VOTE_PUNISH) // Punishment voting
	{
		// Check for a valid client as target
		if (!current_voteent->dont_do_triggerwork) // only if not a special ban (user left)
		if ((current_voteent->admin_kick->classname != "player" && current_voteent->admin_kick->classname != "spec") || !current_voteent->admin_kick->is_connected)
		{
			bprint(PR_PRINT_HIGH,"Voting\x8D Invalid client as target for vote!\n");
			ResetVoteEnt();
			return PR_TRUE;
		}

		if (current_voteent->g_a == PR_PUNISH_CURSE) // Curse
		{
			//if (num_for > num_total * #VOTESNEEDED_CURSE) // success?
			if ( VotePassed( num_for, num_against, PR_VOTESNEEDED_CURSE ) )
			{
				float tf;

				tf = 0;
				st = infokey(world, "curse");

				if (st == string_null)
					tf = stof(st);

				bprint(PR_PRINT_HIGH,"Voting\x8D Democracy says ");
				bprint(PR_PRINT_HIGH,current_voteent->admin_kick->netname);
				bprint(PR_PRINT_HIGH," must be cursed\n");

				BroadcastSound("doors/runeuse");

				createBastard(current_voteent->admin_kick,tf);

				ResetVoteEnt(); // removes current vote entity

				return PR_TRUE;
			}
			//else if (num_against >= num_total * (1 - #VOTESNEEDED_CURSE))
			else
			{
				bprint(PR_PRINT_HIGH,"Voting\x8D Punishment vote has failed");
				bprintVoteIssue();
				bprint(PR_PRINT_HIGH,"\n");

				BroadcastSound("misc/runekey");

				ResetVoteEnt();
			}
		}
		else if (current_voteent->g_a == PR_PUNISH_KICK) // Kick
		{
			//if (num_for > num_total * #VOTESNEEDED_KICK) // success?
			if ( VotePassed( num_for, num_against, PR_VOTESNEEDED_KICK ) )
			{
				bprint(PR_PRINT_HIGH,"Voting\x8D Democracy says ");
				bprint(PR_PRINT_HIGH,current_voteent->admin_kick->netname);
				bprint(PR_PRINT_HIGH," must be kicked\n");

				bprint(PR_PRINT_HIGH, current_voteent->admin_kick->netname);
				bprint(PR_PRINT_HIGH, S_(" is ^bKICKED^b from the server\n"));

				sprint(current_voteent->admin_kick,PR_PRINT_HIGH,S_("\nYou have been ^bKICKED^b from the server!\n"));

				stuffcmd(current_voteent->admin_kick, "disconnect\n"); //Kick them!

				BroadcastSound("player/teledth1");

				ResetVoteEnt(); // removes current vote entity

				return PR_TRUE;
			}
			//else if (num_against >= num_total * (1 - #VOTESNEEDED_KICK))
			else
			{
				bprint(PR_PRINT_HIGH,"Voting\x8D Punishment vote has failed");
				bprintVoteIssue();
				bprint(PR_PRINT_HIGH,"\n");

				BroadcastSound("misc/runekey");

				ResetVoteEnt();
			}
		}
		else if (current_voteent->g_a == PR_PUNISH_BAN) // Ban!
		{
			//if (num_for > num_total * #VOTESNEEDED_BAN) // success?
			if ( VotePassed( num_for, num_against, PR_VOTESNEEDED_BAN ) )
			{
				bprint(PR_PRINT_HIGH,"Voting\x8D Democracy says ");

				if (current_voteent->dont_do_triggerwork) // Special ban (user has left)
				{
					bprint(PR_PRINT_HIGH,current_voteent->t_s_m);
					bprint(PR_PRINT_HIGH," must be banned\n");

					bprint(PR_PRINT_HIGH, current_voteent->t_s_m);
					bprint(PR_PRINT_HIGH, S_(" is ^bBANNED^b from the server\n"));

					localcmd("addip ");
					localcmd(current_voteent->t_s_h);
					localcmd(" " TO_STR(PR_BAN_VOTED_TIME) "");
					localcmd("\n");

					BroadcastSound("doors/runeuse");
				}
				else
				{
					bprint(PR_PRINT_HIGH,current_voteent->admin_kick->netname);
					bprint(PR_PRINT_HIGH," must be banned\n");

					bprint(PR_PRINT_HIGH, current_voteent->admin_kick->netname);
					bprint(PR_PRINT_HIGH, S_(" is ^bBANNED^b from the server\n"));

					sprint(current_voteent->admin_kick,PR_PRINT_HIGH,S_("\nYou have been ^bBANNED^b from the server!\n"));

					stuffcmd(current_voteent->admin_kick, "disconnect\n"); //Kick them!

					st = infokey(current_voteent->admin_kick,"ip");
					localcmd("addip ");
					localcmd(st);
					localcmd(" " TO_STR(PR_BAN_VOTED_TIME) "");
					localcmd("\n");

					BroadcastSound("player/teledth1");
				}

				ResetVoteEnt(); // removes current vote entity

				return PR_TRUE;
			}
			//else if (num_against >= num_total * (1 - #VOTESNEEDED_BAN))
			else
			{
				bprint(PR_PRINT_HIGH,"Voting\x8D Punishment vote has failed");
				bprintVoteIssue();
				bprint(PR_PRINT_HIGH,"\n");

				BroadcastSound("misc/runekey");

				ResetVoteEnt();
			}
		}
		else if (current_voteent->g_a == PR_PUNISH_MUTE) // Mute
		{
			//if (num_for > num_total * #VOTESNEEDED_MUTE) // success?
			if ( VotePassed( num_for, num_against, PR_VOTESNEEDED_MUTE ) )
			{
				bprint(PR_PRINT_HIGH,"Voting\x8D Democracy says ");
				bprint(PR_PRINT_HIGH,current_voteent->admin_kick->netname);
				bprint(PR_PRINT_HIGH," must be muted =)\n");

				bprint(PR_PRINT_HIGH, current_voteent->admin_kick->netname);
				bprint(PR_PRINT_HIGH, " is muted.\n");

				BroadcastSound("doors/runeuse");

				sprint(current_voteent->admin_kick,PR_PRINT_HIGH,"\nYou have been muted!\n");

				uid = getuid(current_voteent->admin_kick);
				st = ftos(uid);

				localcmd("mute 00");
				localcmd(st);
				localcmd(" " TO_STR(PR_MUTE_VOTED_TIME) "");
				localcmd("\n");

				ResetVoteEnt(); // removes current vote entity

				return PR_TRUE;
			}
			//else if (num_against >= num_total * (1 - #VOTESNEEDED_MUTE))
			else
			{
				bprint(PR_PRINT_HIGH,"Voting\x8D Punishment vote has failed");
				bprintVoteIssue();
				bprint(PR_PRINT_HIGH,"\n");

				BroadcastSound("misc/runekey");

				ResetVoteEnt();
			}
		}
		else if (current_voteent->g_a == PR_PUNISH_CUFF) // Cuff
		{
			//if (num_for > num_total * #VOTESNEEDED_CUFF) // success?
			if ( VotePassed( num_for, num_against, PR_VOTESNEEDED_CUFF ) )
			{
				bprint(PR_PRINT_HIGH,"Voting\x8D Democracy says ");
				bprint(PR_PRINT_HIGH,current_voteent->admin_kick->netname);
				bprint(PR_PRINT_HIGH," is annoying\n");

				bprint(PR_PRINT_HIGH, current_voteent->admin_kick->netname);
				bprint(PR_PRINT_HIGH, " is cuffed.\n");

				BroadcastSound("doors/runeuse");

				sprint(current_voteent->admin_kick,PR_PRINT_HIGH,"\nYou have been cuffed!\n");

				uid = getuid(current_voteent->admin_kick);
				st = ftos(uid);

				localcmd("cuff 00");
				localcmd(st);
				localcmd(" " TO_STR(PR_CUFF_VOTED_TIME) "");
				localcmd("\n");

				ResetVoteEnt(); // removes current vote entity

				return PR_TRUE;
			}
			//else if (num_against >= num_total * (1 - #VOTESNEEDED_CUFF))
			else
			{
				bprint(PR_PRINT_HIGH,"Voting\x8D Punishment vote has failed");
				bprintVoteIssue();
				bprint(PR_PRINT_HIGH,"\n");

				BroadcastSound("misc/runekey");

				ResetVoteEnt();
			}
		}
	}
	else if (current_voteent->has_sentry == PR_VOTE_NEXTMAP)
	{
		//if (num_for > num_total * #VOTESNEEDED_NEXTMAP) // success?
		if ( VotePassed( num_for, num_against, PR_VOTESNEEDED_NEXTMAP ) )
		{
			st = colstr(current_voteent->netname,PR_COLSTR_RED);

			bprint(PR_PRINT_HIGH,S_("Voting\x8D Success^b!^b Setting "), st, " as next map");
			// Gizmo - print the map vote type
			PrintVoteMapType( ", in ", current_voteent->group_no, " mode" );
			bprint( PR_PRINT_HIGH, "\n" );

			// Gizmo
			if      (current_voteent->group_no == PR_VOTEMAP_AGR)
			{
				localcmd("localinfo agr 1\n");
				localcmd("localinfo invade \"\"\n");
				if (coop)
					localcmd("localinfo _voted_coop 0\n");
				if (!deathmatch)
					localcmd("localinfo _voted_deathmatch 3\n");
			}
			else if (current_voteent->group_no == PR_VOTEMAP_INVADE)
			{
				localcmd("localinfo invade 1\n");
				localcmd("localinfo agr \"\"\n");
				if (coop)
					localcmd("localinfo _voted_coop 0\n");
				if (!deathmatch)
					localcmd("localinfo _voted_deathmatch 3\n");
			}
			else if (current_voteent->group_no == PR_VOTEMAP_NEO)
			{
				localcmd("localinfo neo 1\n");
				if (coop)
					localcmd("localinfo _voted_coop 0\n");
				if (!deathmatch)
					localcmd("localinfo _voted_deathmatch 3\n");
			}

#ifdef PR_COOP_MODE_ENHANCED
			else if ( current_voteent->group_no == PR_VOTEMAP_TF ) {
				localcmd("localinfo agr \"\"\n");    // PZ - make sure AGR is going to be off
				localcmd("localinfo invade \"\"\n"); // PZ - make sure Invade is going to be off
				if ( coop )
					localcmd( "localinfo _voted_coop 0\n" );
				if ( !deathmatch )
					localcmd( "localinfo _voted_deathmatch 3\n" );
			} else if ( current_voteent->group_no == PR_VOTEMAP_COOP ) {
				localcmd("localinfo agr \"\"\n");    // PZ - make sure AGR is going to be off
				localcmd("localinfo invade \"\"\n"); // PZ - make sure Invade is going to be off
				if ( !coop || coop == 3 )
					localcmd( "localinfo _voted_coop 1\n" );
				if ( deathmatch )
					localcmd( "localinfo _voted_deathmatch 0\n" );
			} else if ( current_voteent->group_no == PR_VOTEMAP_INSANECOOP ) {
				localcmd("localinfo agr \"\"\n");    // PZ - make sure AGR is going to be off
				localcmd("localinfo invade \"\"\n"); // PZ - make sure Invade is going to be off
				if ( coop != 3 )
					localcmd( "localinfo _voted_coop 3\n" );
				if ( deathmatch )
					localcmd( "localinfo _voted_deathmatch 0\n" );
			}
#endif
			else {
				localcmd("localinfo agr \"\"\n");    // PZ - make sure AGR is going to be off
				localcmd("localinfo invade \"\"\n"); // PZ - make sure Invade is going to be off
				localcmd("localinfo neo \"\"\n");    // PZ - make sure Neo is going to be off
				localcmd( "localinfo _voted_coop \"\"\n" );
				localcmd( "localinfo _voted_deathmatch \"\"\n" );
			}

			localcmd("localinfo nmap \"");
			localcmd(current_voteent->netname);
			localcmd("\"\n");

			BroadcastSound("misc/runekey");

			ResetVoteEnt(); // removes current vote entity

			return PR_TRUE;
		}
		//else if (num_against >= num_total * (1 - #VOTESNEEDED_NEXTMAP))
		else
		{
			bprint(PR_PRINT_HIGH,"Voting\x8D Map voting failed");
			bprintVoteIssue();
			bprint(PR_PRINT_HIGH,"\n");

			BroadcastSound("misc/runekey");

			ResetVoteEnt();
		}
	}
	else if (current_voteent->has_sentry == PR_VOTE_SWITCHMAP)
	{
		//if (num_for > num_total * #VOTESNEEDED_MAP) // success?
		if ( VotePassed( num_for, num_against, PR_VOTESNEEDED_MAP ) )
		{
			st = colstr(current_voteent->netname,PR_COLSTR_RED);

			bprint(PR_PRINT_HIGH,S_("Voting\x8D Success^b!^b Switching map to "),st);
			// Gizmo - print the map vote type
			PrintVoteMapType( ", with ", current_voteent->group_no, " mode" );
			bprint( PR_PRINT_HIGH, "\n" );

			// Gizmo
			if      (current_voteent->group_no == PR_VOTEMAP_AGR)
			{
				localcmd("localinfo agr 1\n");
				localcmd("localinfo invade \"\"\n");
				if (coop)
					localcmd("localinfo _voted_coop 0\n");
				if (!deathmatch)
					localcmd("localinfo _voted_deathmatch 3\n");
			}
			else if (current_voteent->group_no == PR_VOTEMAP_INVADE)
			{
				localcmd("localinfo invade 1\n");
				localcmd("localinfo agr \"\"\n");
				if (coop)
					localcmd("localinfo _voted_coop 0\n");
				if (!deathmatch)
					localcmd("localinfo _voted_deathmatch 3\n");
			}
			else if (current_voteent->group_no == PR_VOTEMAP_NEO)
			{
				localcmd("localinfo neo 1\n");
				if (coop)
					localcmd("localinfo _voted_coop 0\n");
				if (!deathmatch)
					localcmd("localinfo _voted_deathmatch 3\n");
			}
#ifdef PR_COOP_MODE_ENHANCED
			else if ( current_voteent->group_no == PR_VOTEMAP_TF ) {
				localcmd("localinfo agr \"\"\n");    // PZ - make sure AGR is going to be off
				localcmd("localinfo invade \"\"\n"); // PZ - make sure Invade is going to be off
				if ( coop )
					localcmd( "localinfo _voted_coop 0\n" );
				if ( !deathmatch )
					localcmd( "localinfo _voted_deathmatch 3\n" );
			} else if ( current_voteent->group_no == PR_VOTEMAP_COOP ) {
				localcmd("localinfo agr \"\"\n");    // PZ - make sure AGR is going to be off
				localcmd("localinfo invade \"\"\n"); // PZ - make sure Invade is going to be off
				if ( !coop || coop == 3 )
					localcmd( "localinfo _voted_coop 1\n" );
				if ( deathmatch )
					localcmd( "localinfo _voted_deathmatch 0\n" );
			} else if ( current_voteent->group_no == PR_VOTEMAP_INSANECOOP ) {
				localcmd("localinfo agr \"\"\n");    // PZ - make sure AGR is going to be off
				localcmd("localinfo invade \"\"\n"); // PZ - make sure Invade is going to be off
				if ( coop != 3 )
					localcmd( "localinfo _voted_coop 3\n" );
				if ( deathmatch )
					localcmd( "localinfo _voted_deathmatch 0\n" );
			}
#endif
			else {
				localcmd("localinfo agr \"\"\n");    // PZ - make sure AGR is going to be off
				localcmd("localinfo invade \"\"\n"); // PZ - make sure Invade is going to be off
				localcmd("localinfo neo \"\"\n");    // PZ - make sure Neo is going to be off
				localcmd( "localinfo _voted_coop \"\"\n" );
				localcmd( "localinfo _voted_deathmatch \"\"\n" );
			}

			BroadcastSound("misc/runekey");

			LaunchMap(current_voteent->netname);

			ResetVoteEnt(); // removes current vote entity

			return PR_TRUE;
		}
		//else if (num_against >= num_total * (1 - #VOTESNEEDED_MAP))
		else
		{
			bprint(PR_PRINT_HIGH,"Voting\x8D Map voting failed");
			bprintVoteIssue();
			bprint(PR_PRINT_HIGH,"\n");

			BroadcastSound("misc/runekey");

			ResetVoteEnt();
		}
	}
	// Gizmo - use object voting
	else if (current_voteent->has_sentry == PR_VOTE_USEOBJECT)
	{
		string objectAction;

		if ( VotePassed( num_for, num_against, PR_VOTESNEEDED_USE ) ) {
			st = GetUseObjectName( current_voteent->enemy );
			objectAction = GetUseObjectAction( current_voteent->enemy );
			bprint( PR_PRINT_HIGH, S_("Voting\x8D Success^b!^b "), st, " will now ", objectAction, "\n" );

			UseObject( current_voteent->enemy );

			BroadcastSound( "misc/runekey" );
			ResetVoteEnt(); // removes current vote entity

			return PR_TRUE;
		} else {
			bprint( PR_PRINT_HIGH, "Voting\x8D Use voting failed" );
			bprintVoteIssue ();
			bprint( PR_PRINT_HIGH, "\n" );

			BroadcastSound( "misc/runekey" );

			ResetVoteEnt ();
		}
	}
	// PZ: generic player polling
	else if (current_voteent->has_sentry == PR_VOTE_POLL)
	{
		st = colstr(current_voteent->netname, PR_COLSTR_RED);

		if (num_for > num_against)
		{
			bprint(PR_PRINT_HIGH, "Voting\x8D Poll: ", st, "\nVoting\x8D Players agreed\n");
			BroadcastSound("misc/runekey");
			ResetVoteEnt(); // removes current vote entity
			return PR_TRUE;
		}
		else if (num_for < num_against)
		{
			bprint(PR_PRINT_HIGH, "Voting\x8D Poll: ", st, "\nVoting\x8D Players disagreed");
			bprintVoteIssue();
			bprint(PR_PRINT_HIGH, "\n");
			BroadcastSound("misc/runekey");
			ResetVoteEnt();
		}
		else
		{
			bprint(PR_PRINT_HIGH, "Voting\x8D Poll: ", st, "\nVoting\x8D Players were undecided");
			bprintVoteIssue();
			bprint(PR_PRINT_HIGH, "\n");
			BroadcastSound("misc/runekey");
			ResetVoteEnt();
		}
	}
	// PZ: For the votebot menu.
	else if (current_voteent->has_sentry == PR_VOTE_BOT)
	{
		if (VotePassed(num_for, num_against, PR_VOTESNEEDED_BOT))
		{
			bprint(PR_PRINT_HIGH, S_("Voting\x8D Success^b!^b Changing bot settings.\n"));
			BroadcastSound("misc/runekey");
			// Copy the votebot settings from the committed settings, of the player who started the vote, into
			// the currently active bot settings.
			copyVoteBotData(ENT_TO_NUM(current_voteent->owner)+MAX_CLIENTS, CURRENT, PR_VOTEBOT_MENU_MAIN);
			ResetVoteEnt(); // removes current vote entity
			return PR_TRUE;
		}
		else
		{
			bprint(PR_PRINT_HIGH, "Voting\x8D Vote to change bot settings failed.\n");
			//bprintVoteIssue();
			//bprint(PR_PRINT_HIGH, "\n");
			BroadcastSound("misc/runekey");
			ResetVoteEnt();
		}
	}

	return PR_FALSE;
}

//==========================================================================
// Checks if valid vote entity

float ValidVoteEnt()
{
	if (current_voteent == world)
		return PR_FALSE;

	if (current_voteent->classname != "voteent")
		return PR_FALSE;

	return PR_TRUE;
}


//============================================================
// gets the map localinfo string for the number passed in
// PZ NOTE: This is for the 'votemap' list.
string GetMapString(float themap)
{
	// PZ: We now store the votemap list in fortress/votemaplist.txt.
	if (themap < 1 || themap > mapVoteMenuData.mapList.size())
		return "";

	return mapVoteMenuData.mapList[(int)themap - 1];

	// PZ: The old code (used localinfo m# <mapname> config entries):
	/*string st, mapnum, temp;

	mapnum = ftos(themap);

	temp = strcat("m", mapnum);
	st = infokey(world, temp);

	return st;*/
}

//===============================================================
// Prints remaining votes for success

/*
=============
bprintVotesNeeded

Prints the number of votes needed to
make the vote not fail when the timer's up.
Note that this may not print anything at all.
=============
*/
void bprintVotesNeeded(float num_for, float num_against)
{
	string	st;
	float		forTemp, againstTemp;
	float		fraction;

/*local string blah;
blah = ftos(num_players);
bprint(2, "bprintVotesNeeded(): num_players = ", blah, "\n");
blah = ftos(bot_count);
bprint(2, "bprintVotesNeeded(): bot_count = ", blah, "\n");
blah = ftos(num_for);
bprint(2, "bprintVotesNeeded(): num_for = ", blah, "\n");
blah = ftos(num_against);
bprint(2, "bprintVotesNeeded(): num_against = ", blah, "\n");*/

	// only bother if people didn't vote yet
	if ( num_players - bot_count - ( num_for + num_against ) > 0 ) { // PZ: subtracted bot count
		fraction = 0.5;

		if ( current_voteent->has_sentry == PR_VOTE_PUNISH ) {
			if (current_voteent->g_a == PR_PUNISH_CURSE)
				fraction = PR_VOTESNEEDED_CURSE;
			else if ( current_voteent->g_a == PR_PUNISH_KICK )
				fraction = PR_VOTESNEEDED_KICK;
			else if ( current_voteent->g_a == PR_PUNISH_BAN )
				fraction = PR_VOTESNEEDED_BAN;
			else if ( current_voteent->g_a == PR_PUNISH_MUTE )
				fraction = PR_VOTESNEEDED_MUTE;
			else if ( current_voteent->g_a == PR_PUNISH_CUFF )
				fraction = PR_VOTESNEEDED_CUFF;
		} else if ( current_voteent->has_sentry == PR_VOTE_NEXTMAP )
			fraction = PR_VOTESNEEDED_NEXTMAP;
		else if ( current_voteent->has_sentry == PR_VOTE_SWITCHMAP )
			fraction = PR_VOTESNEEDED_MAP;
		// Gizmo - use object voting
		else if ( current_voteent->has_sentry == PR_VOTE_USEOBJECT )
			fraction = PR_VOTESNEEDED_USE;
		else if (current_voteent->has_sentry == PR_VOTE_POLL)
			fraction = PR_VOTESNEEDED_POLL;

		num_against = num_against + ( num_players - bot_count - ( num_for + num_against ) ) * fraction; // PZ: subtracted bot count

		// if the vote won't pass at the end, then print the number of votes needed to make it pass
		if ( num_for <= num_against ) {
			forTemp = num_for;
			againstTemp = num_against;
			do {
				forTemp = forTemp + 1;
				againstTemp = againstTemp - fraction;
			} while ( forTemp <= againstTemp );

			st = colstr( ftos( forTemp - num_for ), PR_COLSTR_NUMBER );
			bprint( PR_PRINT_HIGH, " (", st, " needed to not fail)" );
		}
	}
}

void bprintVotesLeft()
{
	string st;
	float num_for, num_against;
	entity te;

	num_for = 0;
	num_against = 0;

	// Count the number of votes for and against.
	te = nextent(world);
	while (te != world) {
		if (te->classname == "player" || te->classname == "spec") {
			if (te->owned_by == PR_VOTED_YES) {
				num_for = num_for + 1;
			} else if (te->owned_by == PR_VOTED_NO) {
				num_against = num_against + 1;
			}
		}
		te = nextent(te);
	}

	st = colstr(ftos(num_for), PR_COLSTR_NUMBER);
	bprint(PR_PRINT_HIGH, S_(" ^b[^b"), st, S_(" for ^b/^b "));

	st = colstr(ftos(num_against), PR_COLSTR_NUMBER);
	bprint(PR_PRINT_HIGH, st, " against");

	// Gizmo - print the number of votes needed for the vote to pass
	bprintVotesNeeded( num_for, num_against );

	bprint( PR_PRINT_HIGH, S_("^b]^b\n") );
	//Ý\n
}

//======================================================================================
// The current punishment vote goes to sleep (player to be muted/cuffed hides away)

// Warning: here we assume self is the player leaving server to be punished
void VoteToSleep()
{
	string st;

	// First clean up any sleeping vote
	CleanUpSleepVote();

	current_voteent->think = VoteSleep_think;
	current_voteent->nextthink = time + PR_VOTESLEEP_RATECHECK;
	current_voteent->classname = "sleepvote";

	st = infokey(self,"ip");

	current_voteent->message = makestr(st);
	current_voteent->admin_kick = world;
	current_voteent->ltime = time;

	current_voteent = world;

	votesleeping = PR_TRUE;
}

void VoteSleep_think()
{
	self->nextthink = time + PR_VOTESLEEP_RATECHECK;

	if (!ValidVoteEnt())
	{
		if (self->admin_kick != world)
		{
			if (!self->admin_kick->is_connected) // He tries to hide again? lol
				self->admin_kick = world; // reset
			else
				SleepVoteResume(self, self->admin_kick);
		}
	}

	// Check if we should discard this sleeping vote
	if (floor((time - self->ltime) / 60) > PR_VOTESLEEP_DURATION)
	{
		delstr(self->message);
		dremove(self);
		votesleeping = PR_FALSE;
	}
}

void CleanUpSleepVote()
{
	entity te;

	te = find(world,"classname","sleepvote");

	while (te != world)
	{
		delstr(te->message);
		dremove(te);

		te = find(te,"classname","sleepvote");
	}

	votesleeping = PR_FALSE;
}

entity GetSleepVote()
{
	entity te;

	te = find(world,"classname","sleepvote");

	return te;
}

void SleepVoteResume(entity sleepent, entity targetclient)
{
	delstr(sleepent->message);

	current_voteent = sleepent;
	current_voteent->think = PunishVote_Think;
	current_voteent->nextthink = time + PR_VOTE_TIME;
	current_voteent->classname = "voteent";
	current_voteent->admin_kick = targetclient;

	votesleeping = PR_FALSE;

	BroadcastSound("misc/medkey");

	bprint(PR_PRINT_HIGH,"Voting\x8D Resuming vote to ");
	if (current_voteent->g_a == PR_PUNISH_MUTE)
		bprint(PR_PRINT_HIGH,"mute ");
	else if (current_voteent->g_a == PR_PUNISH_CUFF)
		bprint(PR_PRINT_HIGH,"cuff ");
	bprint(PR_PRINT_HIGH,current_voteent->admin_kick->netname);
	bprintVotesLeft();
}

} // END namespace Progs
