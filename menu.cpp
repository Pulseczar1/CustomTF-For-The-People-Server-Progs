/*======================================================
	MENU.QC 		Custom TeamFortress v2.3

	(c) TeamFortress Software Pty Ltd	29/2/97
	(c) William Kerney					 4/4/00
	(c) Craig Hauser					19/3/00
========================================================
This file handles all menu functions and displays.
OfN - Custom menu is on cutfmenu.qc and other menus
are on their respective files now.
======================================================*/

#include "progs.h"
#include "ofndefs.h"
#include "menu.h"
#include "vote.h"
#include "vote2.h"
#include "prozac.h"
#include "debug.h"
#include "optimize.h"
#include "invade.h"
#include "neo.h"
#include "custom.h"

namespace Progs {

void spawnFOG(const vector& where);
void CenterPrint4(entity pl, const string& s1, const string& s2, const string& s3, const string& s4);
void CenterPrint7(entity pl, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5, const string& s6, const string& s7);
void bprintVoteIssue();
void BroadcastSound(const string& thesound);
void ResetVoteEnt();
float GetNoPlayers(float checkIP, float noIdlers);

// PZ: Used to have #defines here.

void TF_T_Damage(entity targ, entity inflictor, entity attacker, float damage, int T_flags, float T_AttackType);
void W_SetCurrentAmmo();
void bound_other_ammo(entity p);
float TeamFortress_TeamSet(float tno, float performChecks);
void TeamFortress_SetSkin(entity p);
float TeamFortress_TeamGetColor(float tno);
void TeamFortress_ChangeClass();
void TeamFortress_DropAmmo(float type);
float TeamFortress_TeamPutPlayerInTeam(float performChecks);
float TeamFortress_TeamIsCivilian(float tno);
void TeamFortress_SpyChangeSkin(float playerClass);
void TeamFortress_SpyChangeColor(float teamno);
void TeamFortress_Build(float building, int devicenum);
void TeamFortress_SpyFeignDeath(float type);
void Spy_RemoveDisguise(entity spy);
void lvl1_sentry_stand();
void lvl2_sentry_stand();
void lvl3_sentry_stand();
float TeamFortress_TeamGetNoPlayers(float tno);
float HasFlag(float myteam);

/* WK void(float menu_no) DisplayMenu; */
// Assorted menus
void Menu_Team();
void Menu_Class();
void Menu_Drop();
void Menu_Intro();
void PlayerObserverMode();
// Classhelp
void Menu_ClassHelp();
void Menu_ClassHelp2();
void Menu_StuffClassHelp();
void Menu_ClassHelp_Input(float inp);
// Bindings
void Menu_DoBindings();
void Menu_ShowBindings1();
void Menu_ShowBindings2();
void Menu_RepeatHelp();
// Spy
void Menu_Spy();
void Menu_Spy_Skin();
void Menu_Spy_Color();
void Menu_Spy_Input(float inp);
void Menu_Spy_Skin_Input(float inp);
void Menu_Spy_Color_Input(float inp);
// Engineer
void Menu_EngineerConfirmDismantle();  // PZ
void Menu_EngineerDismantle();         // PZ
void Menu_EngineerDestroy();           // PZ
void Menu_Engineer();
void Menu_EngineerFix_Dispenser();
void Menu_EngineerFix_SentryGun();
void Menu_EngineerFix_Tesla();
void Menu_EngineerFix_Tesla2();
void Menu_EngineerFix_Camera();
void Menu_EngineerFix_Sensor();
void Menu_EngineerFix_Teleporter();
void Menu_EngineerFix_FieldGen();
void Menu_EngineerConfirmDismantle_Input(float inp);   // PZ
void Menu_Engineer_Input(float inp);
void Menu_EngineerFix_Dispenser_Input(float inp);
void Menu_EngineerFix_SentryGun_Input(float inp);
void Menu_EngineerFix_Tesla_Input(float inp);
void Menu_EngineerFix_Tesla_Input2(float inp);
void Menu_EngineerFix_Camera_Input(float inp);
void Menu_EngineerFix_Sensor_Input(float inp);
void Menu_EngineerFix_Teleporter_Input(float inp);
void Menu_EngineerFix_FieldGen_Input(float inp);

// Demons
void Menu_Demon();
void Menu_Demon_Input(float inp);
void kill_my_demons();

// Buildings
void Menu_Dispenser();
void Menu_Dispenser_Input(float inp);

float CheckArea(entity obj, entity builder);

// WK - Custom Class
void DropFromCustomClassGen(); //Extern
void PrintMoney();
void BuyWeapon(float cost, int item);
void BuyCuTF(float cost, int item);
void BuyItem(float cost, int item);
void BuyJob(float cost, float type);
/*void(float cost,float type) BuyGren1;
void(float cost,float type) BuyGren2;*/
void PrintRefund(float in);
void PrintNotEnoughMoney(float in);
void Menu_PrimaryWeapon();
void Menu_PrimaryWeapon_Input(float inp);
void Menu_SecondaryWeapon();
void Menu_SecondaryWeapon_Input(float inp);
//- Ofn -
void Menu_MiscWeapon();
void Menu_MiscWeapon_Input(float inp);
void Menu_Crusader();
void Menu_Crusader_Input(float inp);
void Menu_Punish();
void Menu_Punish_Input(float inp);
void Menu_VoteMap();
void Menu_VoteMap_Input(float inp);
void Menu_Guerilla();
void Menu_Guerilla_Input(float inp);

void Menu_Legs();
void Menu_Legs_Input(float inp);
void Menu_Health();
void Menu_Health_Input(float inp);
void Menu_Armor();
void Menu_Armor_Input(float inp);
void Menu_Special();
void Menu_Special_Input(float inp);
void Menu_Special2();
void Menu_Special2_Input(float inp);
void Menu_Gren1();
void Menu_Gren1_Input(float inp);
void Menu_Gren2();
void Menu_Gren2_Input(float inp);
void Menu_Engineering();
void Menu_Engineering_Input(float inp);
void Menu_Profession();
void Menu_Profession_Input(float inp);
void Menu_Profession2();
void Menu_Profession2_Input(float inp);
void Menu_Proficiency();
void Menu_Proficiency_Input(float inp);
void Menu_Option();
void Menu_Option_Input(float inp);

void Menu_ToolWeapon();
void Menu_ToolWeapon_Input(float inp);
void Menu_MoreOption();
void Menu_MoreOption_Input(float inp);

//WK Needed for rotate
float anglemod(float v);
//WK Needed to det sentries et al.
void T_RadiusDamage(entity inflictor, entity attacker, float damage, entity ignore);
//WK Needed to make a bastard
void createBastard(entity bastard,float threshold);
//CY Tinker
void DoTinker();
//WW Needed for the Dismantling
void ClientObituary(entity targ, entity attacker);

// WK --------------

//- OfN - Warlock menu (now in warlock.qc) --//
void Menu_Demon_Input(float inp);			 //
void Menu_Demon();							 //
//-------------------------------------------//
//- OfN - Army menu (now in army.qc) --------//
void Menu_Army_Input(float inp);			 //
void Menu_Army();							 //
//-------------------------------------------//
//- OfN - Hack menu (now in haxxx.qc) -------//
void Menu_EnemyHax_Inp(float inp);			 //
void Menu_Enemy_Hax();						 //
void Menu_FriendHax_Inp(float inp); 		 //
void Menu_Friend_Hax(); 					 //
//-------------------------------------------//
void BuyExtra(float cost1, float cost2);
void UpdateWeaponItems();
float CheckEnemyDismantle();
void SBBuildSensor();
void Tesla_Lose_Glow();
//entity(entity OldTesla) TeslaClone;
float GetMaxGrens(entity theplayer, float grenslot);
void BuyGren(float cost, float type);
string GetGrenadeText(float typ);
//void(entity thing) RemoveMyTimers;
void GuerillaInit(entity player);
float CanBuyItem(float inp);
float ItemIsFree(float inp);
void MenuResetCount();

#ifdef PR_FIELD_FORCEMODE
void SetFieldForcedStatus(float value); // player function (self = player) cuts disabled time also
float GetFieldForcedStatus(); // player
#endif

//------------------------------------------//
#ifdef PR_MENU_SOUNDS
float menu_sounds;
#endif

void MenuPrecache()
{
	#ifdef PR_MENU_SOUNDS
	precache_sound("cutfmenu/menu1.wav");
	precache_sound("cutfmenu/menu2.wav");
	precache_sound("cutfmenu/menu3.wav");
	precache_sound("cutfmenu/menu4.wav");
	#endif
}

void CuTFMenuSound(float sample)
{
	#ifdef PR_MENU_SOUNDS
	if (!menu_sounds)
		return;

	if (sample == PR_MENUSOUND_BROWSE)
		stuffcmd(self,"playvol cutfmenu/menu1.wav 0.30\n"); // 0.4
	else if (sample == PR_MENUSOUND_BUY)
		stuffcmd(self,"playvol cutfmenu/menu2.wav 0.30\n"); // 0.4
	else if (sample == PR_MENUSOUND_SELL)
		stuffcmd(self,"playvol cutfmenu/menu3.wav 0.5\n"); // 0.4
	else if (sample == PR_MENUSOUND_WRONG)
		stuffcmd(self,"playvol cutfmenu/menu4.wav 0.7\n"); // 0.6
	#endif
}


void ResetMenu()
{
	if (self->StatusBarSize == 0)
		CenterPrint(self, "\n");
	else
		self->StatusRefreshTime = time + 0.1;
	self->menu_count = PR_MENU_REFRESH_RATE;
	self->current_menu = PR_MENU_DEFAULT;
	self->impulse = 0; // PZ
}

void Player_Menu()
{
	// loop function
	if (self->menu_count > PR_MENU_REFRESH_RATE)
	{
		self->menu_count = 0;
	}
	else
	{
		self->menu_count = self->menu_count + 1;
		return;
	}

	// determine which menu to display
	// WK - Check to see if the custom class menu is overriding us
	if ((self->done_custom & PR_CUSTOM_BUILDING) && !(self->done_custom & PR_CUSTOM_OVERRIDE) && self->playerclass == PR_PC_CUSTOM) {
		self->current_menu = PR_MENU_PRIMARY_WEAPON;
		self->done_custom = self->done_custom | PR_CUSTOM_OVERRIDE;
	}

	//Don't flash status bars
	//if (self.current_menu >= #MENU_PRIMARY_WEAPON && self.current_menu <= #MENU_OPTION)
	if (self->current_menu >= PR_MENU_CUSTOM_MIN && self->current_menu <= PR_MENU_CUSTOM_MAX)
		self->StatusRefreshTime = time + 1.5;

	//- OfN - Checks for destroyed buildings - NEEDED?
	/*if (self.current_menu >= #MENU_ENGINEER_FIX_DISPENSER && self.current_menu <= #MENU_ENGINEER_FIX_FIELDGEN && self.building == world)
	{
		ResetMenu();
		return;
	}*/

	// OfN - increase string frame animation counter
	/*self.ex_skill_min = self.ex_skill_min +1;
	if (self.ex_skill_min > 3)
		self.ex_skill_min = 0;*/

	if (self->current_menu == PR_MENU_INTRO)
	{
		Menu_Intro();
		self->current_menu = PR_MENU_DEFAULT;
	}
	else if (self->current_menu == PR_MENU_CLASSHELP)       // PZ NOTE: actual class help
	{
		Menu_ClassHelp();
		if (self->menu_displaytime >= 30)             // PZ: was > 8
		{
			Menu_StuffClassHelp();
			self->current_menu = PR_MENU_REPEATHELP;    // PZ: was #MENU_DEFAULT
		}
		else
			self->menu_displaytime = self->menu_displaytime + 1;
	}
	else if (self->current_menu == PR_MENU_CLASSHELP2)      // PZ NOTE: command list
	{
		Menu_ClassHelp2();
		if (self->menu_displaytime >= 30)             // PZ: was > 5
		{
			Menu_StuffClassHelp();
			self->current_menu = PR_MENU_REPEATHELP;    // PZ: was #MENU_DEFAULT
		}
		else
			self->menu_displaytime = self->menu_displaytime + 1;
	}
	else if (self->current_menu == PR_MENU_REPEATHELP)
	{
		Menu_RepeatHelp();
		self->current_menu = PR_MENU_DEFAULT;
	}
	else if (self->current_menu == PR_MENU_SHOWBINDS1)
	{
		Menu_ShowBindings1();
		if (self->menu_displaytime > 8)
		{
			self->current_menu = PR_MENU_DEFAULT;
		}
		else
			self->menu_displaytime = self->menu_displaytime + 1;
	}
	else if (self->current_menu == PR_MENU_SHOWBINDS2)
	{
		Menu_ShowBindings2();
		if (self->menu_displaytime > 8)
		{
			self->current_menu = PR_MENU_DEFAULT;
		}
		else
			self->menu_displaytime = self->menu_displaytime + 1;
	}
	else if (self->current_menu == PR_MENU_DROP)
	{
		Menu_Drop();
	}
	else if (self->current_menu == PR_MENU_SPY)
	{
		Menu_Spy();
	}
	else if (self->current_menu == PR_MENU_SPY_SKIN)
	{
		Menu_Spy_Skin();
	}
	else if (self->current_menu == PR_MENU_SPY_COLOR)
	{
		Menu_Spy_Color();
	}
	else if (self->current_menu == PR_MENU_ENGINEER)
	{
		Menu_Engineer();
	}
	else if (self->current_menu == PR_MENU_ENGINEER_CONFIRM_DISMANTLE)  // PZ
	{
		Menu_EngineerConfirmDismantle();
	}
	else if (self->current_menu == PR_MENU_ENGINEER_FIX_DISPENSER)
	{
		Menu_EngineerFix_Dispenser();
	}
	else if (self->current_menu == PR_MENU_ENGINEER_FIX_SENTRYGUN)
	{
		Menu_EngineerFix_SentryGun();
	}
	else if (self->current_menu == PR_MENU_ENGINEER_FIX_TESLA)
	{
		Menu_EngineerFix_Tesla();
	}
	else if (self->current_menu == PR_MENU_ENGINEER_FIX_TESLA2)
	{
		Menu_EngineerFix_Tesla2();
	}
	else if (self->current_menu == PR_MENU_ENGINEER_FIX_CAMERA)
	{
		Menu_EngineerFix_Camera();
	}
	else if (self->current_menu == PR_MENU_ENGINEER_FIX_SENSOR)
	{
		Menu_EngineerFix_Sensor();
	}
	else if (self->current_menu == PR_MENU_ENGINEER_FIX_TELEPORTER)
	{
		Menu_EngineerFix_Teleporter();
	}
	else if (self->current_menu == PR_MENU_ENGINEER_FIX_FIELDGEN)
	{
		Menu_EngineerFix_FieldGen();
	}
	else if (self->current_menu == PR_MENU_DISPENSER)
	{
		Menu_Dispenser();
	}
	else if (self->current_menu == PR_MENU_PICKBINDS)
	{
		Menu_DoBindings();
	}
	else if (self->current_menu == PR_MENU_DEMON)
	{
		Menu_Demon();
	}
	else if (self->current_menu == PR_MENU_ARMY) //- OfN
	{
		Menu_Army();
	}
	else if (self->current_menu == PR_MENU_E_HACK) //- OfN
	{
		Menu_Enemy_Hax();
	}
	else if (self->current_menu == PR_MENU_F_HACK) //- OfN
	{
		Menu_Friend_Hax();
	}
	else if (self->current_menu == PR_MENU_CRUSADER)
	{
		Menu_Crusader();
	}
	else if (self->current_menu == PR_MENU_GUERILLA)
	{
		Menu_Guerilla();
	}
	//- OfN - Democracy
	else if (self->current_menu == PR_MENU_PUNISH)
	{
		Menu_Punish();
	}
	else if (self->current_menu == PR_MENU_VOTEMAP)
	{
		Menu_VoteMap();
	}
	else if (self->current_menu == PR_MENU_VOTEBOT)  // PZ: for "votebot"
	{
		Menu_VoteBot();
	}
	//WK ------------------
	else if (self->current_menu == PR_MENU_PRIMARY_WEAPON)
	{
		Menu_PrimaryWeapon();
	}
	else if (self->current_menu == PR_MENU_SECONDARY_WEAPON)
	{
		Menu_SecondaryWeapon();
	}
	else if (self->current_menu == PR_MENU_MISC_WEAPON)
	{
		Menu_MiscWeapon();
	}
	else if (self->current_menu == PR_MENU_LEGS)
	{
		Menu_Legs();
	}
	else if (self->current_menu == PR_MENU_HEALTH)
	{
		Menu_Health();
	}
	else if (self->current_menu == PR_MENU_ARMOR)
	{
		Menu_Armor();
	}
	else if (self->current_menu == PR_MENU_SPECIAL)
	{
		Menu_Special();
	}
	else if (self->current_menu == PR_MENU_SPECIAL2)
	{
		Menu_Special2();
	}
	else if (self->current_menu == PR_MENU_GREN1)
	{
		Menu_Gren1();
	}
	else if (self->current_menu == PR_MENU_GREN2)
	{
		Menu_Gren2();
	}
	else if (self->current_menu == PR_MENU_ENGINEERING)
	{
		Menu_Engineering();
	}
	else if (self->current_menu == PR_MENU_PROFESSION)
	{
		Menu_Profession();
	}
	else if (self->current_menu == PR_MENU_PROFESSION2)
	{
		Menu_Profession2();
	}
	else if (self->current_menu == PR_MENU_PROFICIENCY)
	{
		Menu_Proficiency();
	}
	else if (self->current_menu == PR_MENU_OPTION)
	{
		Menu_Option();
	}
	else if (self->current_menu == PR_MENU_TOOLS)
		Menu_ToolWeapon ();
	else if (self->current_menu == PR_MENU_MOREOPTION)
		Menu_MoreOption ();

	//WK ----------------
	// PZ: allow us to enter the team selection menu at any time (added "|| self.current_menu == #MENU_TEAM")
	else if ((self->team_no == 0 && teamplay && (self->lives != 0)) || self->current_menu == PR_MENU_TEAM)
	{
		if (self->motd >= PR_MOTD_FINISHED) {
			self->current_menu = PR_MENU_TEAM;
			Menu_Team();
		}
	}
	// PZ: allow us to enter the class selection menu at any time (added "|| self.current_menu == #MENU_CLASS")
	//WK Default to custom class gen - OfN - nope!
	else if ((self->playerclass == PR_PC_UNDEFINED && (self->lives != 0)) || self->current_menu == PR_MENU_CLASS)
	{
		//- OfN - stock_mode applies
		if (stock_mode < 2) {
			self->current_menu = PR_MENU_CLASS;//-was commented
			Menu_Class();//-was commented
		}
		else
		{
			self->impulse = PR_PC_CUSTOM + PR_TF_CHANGEPC; //-wasnt commented
			TeamFortress_ChangeClass();//-wasnt commented
		}
	}
	else
		self->current_menu = 0;
}


//WK What the hell is this for?
/*
void(float menu_no) DisplayMenu =
{
	self.current_menu = menu_no;

	if (menu_no == #MENU_TEAM)
	{
		Menu_Team();
	}
	else if (menu_no == #MENU_CLASS)
	{
		Menu_Class();
	}
};
*/

void Menu_Team_Input(float inp);

void Menu_Team()
{
	string teamMenu; // PZ: added

	if ((toggleflags & PR_TFLAG_AUTOTEAM) && teamplay && !neo.isModeActive()) // not in Neo mode
	{
		// assign player a team
		if (TeamFortress_TeamPutPlayerInTeam(1))
			return;
	}

	// PZ - Attackers Go Red: begin code
	float x;

	// The Detection entity may have specified a Team Menu String (PZ NOTE: maps can override the default team selection menu)
	if (team_menu_string != string_null)
	{
		/*if (agr)
		{
			x = floor(time);
			if (x - (2 * floor(x / 2)) == 0) // x mod 2 == 0
				CenterPrint2(self, team_menu_string, "\n\n\{17} \bAttackers Go Red\b \{16}\n");
			else
				CenterPrint2(self, team_menu_string, "\n\n\{17} Attackers Go Red \{16}\n");
		}
		else
			CenterPrint(self, team_menu_string);*/
		// PZ: redid the above, as one concatenated string, to add message about canceling team selection menu
		teamMenu = team_menu_string;
		// determine how many newlines they ended with, so that we can make it end with exactly 1 newline
		float numberOfNewlines, i;
		numberOfNewlines = 0;
		i = strlen(teamMenu) - 1;
		while (i >= 0)
		{
			if (substring(teamMenu, i, 1) != "\n") i = -1; // to 'break' (I don't trust QCCX 'break' statement)
			else
			{
				numberOfNewlines = numberOfNewlines + 1;
				i = i - 1;
			}
		}
		// remove all ending newlines from `teamMenu`
		i = strlen(teamMenu) - numberOfNewlines;
		teamMenu = substring(teamMenu, 0, i);
		// make sure it ends in exactly one newline
		teamMenu = strcat(teamMenu, "\n");
		// add messages for AGR and/or the 'changeteam' command
		if (agr)
		{
			x = floor(time);
			if (x - (2 * floor(x / 2)) == 0) // x mod 2 == 0
				teamMenu = strcat(teamMenu, S_("\n\n\x11 ^bAttackers Go Red^b \x10\n"));
			else
				teamMenu = strcat(teamMenu, "\n\n\x11 Attackers Go Red \x10\n");
		}
		else if (invade) // PZ: for Invade mode
		{
			string message;
			if (invade_teamOnDef == 1)
				message = "Red (Team 2) on Offense\n";
			else
				message = "Blue (Team 1) on Offense\n";
			x = floor(time);
			if ((int)x % 2 == 0) message = colstr(message, PR_COLSTR_RED);
			teamMenu += "\n\n\x11 Invade Mode \x10\n\n" + message;
		}
		if (self->team_no) // PZ: if they already have a team, let them know they can cancel this menu (in case they used "changeteam")
			teamMenu = strcat(teamMenu, S_("\nUse ^bchangeteam^b again to\n"
		                                "cancel team selection. \n"));
		CenterPrint(self, teamMenu);
		// PZ: END
		return;
	}

	/*if (CTF_Map == #TRUE)
	{
		if (agr)
			CenterPrint2(self, "\{157}\{158}\{159} \bChoose Your Team\b \{157}\{158}\{159}\n\n\1.. Blue Team           \n\2.. Red Team            \n\n\n\n\7.. Bind my keys for me!\n\nFor full details on this patch:\nhttp://www.telefragged.com/teamfortress/\n", "\n\n\{17} \bAttackers Go Red\b \{16}\n");
		else
			CenterPrint(self, "\{157}\{158}\{159} \bChoose Your Team\b \{157}\{158}\{159}\n\n\1.. Blue Team           \n\2.. Red Team            \n\n\n\n\7.. Bind my keys for me!\n\nFor full details on this patch:\nhttp://www.telefragged.com/teamfortress/\n");
	}
	else if (number_of_teams == 1)
	{
		if (agr)
			CenterPrint2(self, "\{157}\{158}\{159} \bChoose Your Team\b \{157}\{158}\{159}\n\n\1.. Team One  \n", "\n\n\{17} \bAttackers Go Red\b \{16}\n");
		else
			CenterPrint(self, "\{157}\{158}\{159} \bChoose Your Team\b \{157}\{158}\{159}\n\n\1.. Team One  \n");
	}
	else if (number_of_teams == 2)
	{
		if (agr)
			CenterPrint2(self, "\{157}\{158}\{159} \bChoose Your Team\b \{157}\{158}\{159}\n\n\1.. Team One  \n\2.. Team Two  \n              \n              \n\n\5.. \bAuto Team\b \n", "\n\n\{17} \bAttackers Go Red\b \{16}\n");
		else
			CenterPrint(self, "\{157}\{158}\{159} \bChoose Your Team\b \{157}\{158}\{159}\n\n\1.. Team One  \n\2.. Team Two  \n              \n              \n\n\5.. \bAuto Team\b \n");
	}
	else if (number_of_teams == 3)
	{
		if (agr)
			CenterPrint2(self, "\{157}\{158}\{159} \bChoose Your Team\b \{157}\{158}\{159}\n\n\1.. Team One  \n\2.. Team Two  \n\3.. Team Three\n              \n\n\5.. \bAuto Team\b \n", "\n\n\{17} \bAttackers Go Red\b \{16}\n");
		else
			CenterPrint(self, "\{157}\{158}\{159} \bChoose Your Team\b \{157}\{158}\{159}\n\n\1.. Team One  \n\2.. Team Two  \n\3.. Team Three\n              \n\n\5.. \bAuto Team\b \n");
	}
	else // if (number_of_teams == 4)
	{
		if (agr)
			CenterPrint2(self, "\{157}\{158}\{159} \bChoose Your Team\b \{157}\{158}\{159}\n\n\1.. Team One  \n\2.. Team Two  \n\3.. Team Three\n\4.. Team Four \n\n\5.. \bAuto Team\b \n", "\n\n\{17} \bAttackers Go Red\b \{16}\n");
		else
			CenterPrint(self, "\{157}\{158}\{159} \bChoose Your Team\b \{157}\{158}\{159}\n\n\1.. Team One  \n\2.. Team Two  \n\3.. Team Three\n\4.. Team Four \n\n\5.. \bAuto Team\b \n");
	}*/
	// PZ - AGR end code
	// PZ: redid the above, as one concatenated string
	// PZ NOTE: This is the default team menu. The map can replace this menu with its own. (The code for the menu override is right above this.)
	if (CTF_Map == PR_TRUE)
		teamMenu = S_("\x9D\x9E\x9F ^bChoose Your Team^b \x9D\x9E\x9F\n\n^1.. Blue Team           \n^2.. Red Team            \n\n\n\n^7.. Bind my keys for me!\n\nFor full details on this patch:\nhttp://www.telefragged.com/teamfortress/\n");
	else
	{
		teamMenu = S_("\x9D\x9E\x9F ^bChoose Your Team^b \x9D\x9E\x9F\n\n^1.. Team One  \n");
		if      (number_of_teams == 2)
			teamMenu = strcat(teamMenu, S_("^2.. Team Two  \n              \n              \n\n^5.. ^bAuto Team^b \n"));
		else if (number_of_teams == 3)
			teamMenu = strcat(teamMenu, S_("^2.. Team Two  \n^3.. Team Three\n              \n\n^5.. ^bAuto Team^b \n"));
		else if (number_of_teams == 4)
			teamMenu = strcat(teamMenu, S_("^2.. Team Two  \n^3.. Team Three\n^4.. Team Four \n\n^5.. ^bAuto Team^b \n"));
	}
	if (agr)
	{
		x = floor(time);
		if (x - (2 * floor(x / 2)) == 0) // x mod 2 == 0
			teamMenu = strcat(teamMenu, S_("\n\n\x11 ^bAttackers Go Red^b \x10\n"));
		else
			teamMenu = strcat(teamMenu, "\n\n\x11 Attackers Go Red \x10\n");
	}
	else if (invade) // PZ: for Invade mode
	{
		string message;
		if (invade_teamOnDef == 1)
			message = "Red (Team 2) on Offense\n";
		else
			message = "Blue (Team 1) on Offense\n";
		x = floor(time);
		if ((int)x % 2 == 0) message = colstr(message, PR_COLSTR_RED);
		teamMenu += "\n\n\x11 Invade Mode \x10\n\n" + message;
	}
	if (self->team_no) // PZ: if they already have a team, let them know they can cancel this menu (in case they used "changeteam")
		teamMenu = strcat(teamMenu, S_("\nUse ^bchangeteam^b again to\n"
		                            "cancel team selection. \n"));
	CenterPrint(self, teamMenu);
	// PZ: END
}

void Menu_Team_Input(float inp)
{
	bool teamSet = false; // bool

	//WK Remove starting observer gravity. It might come right back.
	self->gravity = 1;

	//WK 1-19-09 Disable autoteam in Neo mode
	if (neo.isModeActive() && inp == 5) inp = 0;
	if (inp == 5) // auto team
	{
		if (!self->team_no) teamSet = TeamFortress_TeamPutPlayerInTeam(1);
		else                TeamFortress_TeamPutPlayerInTeam(2);          // PZ: they used "changeteam"; only see if we can add him
	}
	else if (inp <= number_of_teams && inp > 0)
	{
		if (!self->team_no) teamSet = TeamFortress_TeamSet(inp, 1);
		else                TeamFortress_TeamSet(inp, 2);                 // PZ: they used "changeteam"; only see if we can add him
	}
	else if (number_of_teams == 0 && inp <= PR_TM_MAX_NO)
	{
		if (!self->team_no) teamSet = TeamFortress_TeamSet(inp, 1);
		else                TeamFortress_TeamSet(inp, 2);                 // PZ: they used "changeteam"; only see if we can add him
	}
	// PZ NOTE: I don't think that this section is running. I think W_WeaponFrame() is catching these 8 through 10, and doing what we are trying to do here (this is duplicated code).
	else if (inp == 8  && self->motd < PR_MOTD_FINISHED - 2) // PZ: press 8 to view previous page of intro/MOTD
	{
		self->worldtype = self->worldtype - 1;   // .worldtype is used only by the player entity here for controlling MOTD page selection
		if (self->worldtype < 1) self->worldtype = PR_NUMBER_OF_MOTD_PAGES; // loop around to last page
		self->show_hostile = 0; // make it perform a new centerprint immediately, so that page is fast to update
		self->impulse = 0;
		return;
	}
	else if (inp == 9  && self->motd < PR_MOTD_FINISHED - 2) // PZ: press 9 to view next page of intro/MOTD
	{
		self->worldtype = self->worldtype + 1;
		if (self->worldtype > PR_NUMBER_OF_MOTD_PAGES) self->worldtype = 1; // loop back to first page
		self->show_hostile = 0; // make it perform a new centerprint immediately, so that page is fast to update
		self->impulse = 0;
		return;
	}
	else if (inp == 10 && self->motd < PR_MOTD_FINISHED - 2) // PZ: press 0 to skip intro/MOTD
	{
		self->motd = PR_MOTD_FINISHED - 2; // -2 because there are things that get done at -2 and -1
		self->impulse = 0;
		return;
	}
	else //- OfN
	{
		//ResetMenu();
		self->impulse = 0;
		return;
	}

	// added teamSet for AGR code below -PZ
	entity ent;
	string temp;
	if (teamSet) // observers do not count against AGR limits
	{
		ent = find(world, "classname", "voteent");
		if (ent != world)
		{
			if (ent->group_no == 2 || ent->group_no == 3) // is there a vote running for an AGR map?
			{
				// check that newly-added player to the game doesn't go over agr_vote_max
				float x;
				x = GetNoPlayers(PR_TRUE, PR_TRUE);
				if (agr_vote_max != 0 && x > agr_vote_max)
				{
					bprint(PR_PRINT_HIGH, S_("Voting\x8D Max number of players allowed for an ^bAGR^b vote is "));
					temp = ftos(agr_vote_max); temp = colstr(temp, PR_COLSTR_NUMBER);
					bprint(PR_PRINT_HIGH, temp); bprint(PR_PRINT_HIGH, " players.\n");
					bprint(PR_PRINT_HIGH, last_vote_starter); bprint(PR_PRINT_HIGH, " thanks ");
					bprint(PR_PRINT_HIGH, self->netname);
					bprint(PR_PRINT_HIGH, " for putting the player-count above ");
					bprint(PR_PRINT_HIGH, temp); bprint(PR_PRINT_HIGH, ". :P\n");

					bprint(PR_PRINT_HIGH,"Voting\x8D Map voting failed");
					bprintVoteIssue();
					bprint(PR_PRINT_HIGH,"\n");

					BroadcastSound("misc/runekey");

					ResetVoteEnt();
				}
			}
		}
		if (self->motd < PR_MOTD_FINISHED - 2) self->motd = PR_MOTD_FINISHED - 2; // PZ: kill MOTD once player has selected a team
		if (custom_mode!=2) sprint(self, PR_PRINT_MEDIUM, S_("Press \xA2^0\xA2 for a custom player class\n"));
	}
	// end AGR code

	/*else if (inp == 7)
		self.current_menu = #MENU_PICKBINDS;
	else if (inp == 8)
		PlayerObserverMode();*/

	ResetMenu();
	self->impulse = 0;
}

void Menu_Class()
{
	entity AD;
	string classMenu;
	float usedMenuOverride; // PZ: added

	// PZ NOTE: This section allows the map to override the default class menu for each of the 4 possible teams.
	AD = find(world, "classname", "info_tfdetect");
	if (AD != world)
	{
		usedMenuOverride = 0;
		if      (self->team_no == 1)
		{
			if (AD->noise1 != string_null)
			{
				classMenu = AD->noise1; //CenterPrint(self, AD.noise1);  // PZ
				usedMenuOverride = 1;  //return;                        // PZ
			}
		}
		else if (self->team_no == 2)
		{
			if (AD->noise2 != string_null)
			{
				classMenu = AD->noise2; //CenterPrint(self, AD.noise2);  // PZ
				usedMenuOverride = 1;  //return;                        // PZ
			}
		}
		else if (self->team_no == 3)
		{
			if (AD->noise3 != string_null)
			{
				classMenu = AD->noise3; //CenterPrint(self, AD.noise3);  // PZ
				usedMenuOverride = 1;  //return;                        // PZ
			}
		}
		else if (self->team_no == 4)
		{
			if (AD->noise4 != string_null)
			{
				classMenu = AD->noise4; //CenterPrint(self, AD.noise4);  // PZ
				usedMenuOverride = 1;  //return;                        // PZ
			}
		}

		// PZ: Add on a message about the 'changeclass' command, if the command was used by the player.
		if (usedMenuOverride)
		{
			// determine how many newlines they ended with, so that we can make it end with exactly 1 newline
			float numberOfNewlines, i;
			numberOfNewlines = 0;
			i = strlen(classMenu) - 1;
			while (i >= 0)
			{
				if (substring(classMenu, i, 1) != "\n") i = -1; // to 'break' (I don't trust QCCX 'break' statement)
				else
				{
					numberOfNewlines = numberOfNewlines + 1;
					i = i - 1;
				}
			}
			// remove all ending newlines from `classMenu`
			i = strlen(classMenu) - numberOfNewlines;
			classMenu = substring(classMenu, 0, i);
			// make sure it ends in exactly one newline
			classMenu = strcat(classMenu, "\n");

			if (self->playerclass) // PZ: If they currently already have a class (i.e., they used 'changeclass'), let them know they can cancel the menu.
				classMenu = strcat(classMenu, S_("\nUse ^bchangeclass^b again to\n"
				                                 "cancel class selection. \n"));
			CenterPrint(self, classMenu);
			return; // PZ: put a single return here; removed others
		}
		// PZ: END
	}

	// PZ NOTE: This section displays the default class menu, if the map does not override the class menu.
	/*if (custom_mode!=2)
	{
		if (TeamFortress_TeamIsCivilian(self.team_no))
			CenterPrint(self, "Your team can only be Civilians.\n");
		else if (spy_off == #TRUE)
			CenterPrint(self, "\{157}\{158}\{159} \bChoose Your Class\b \{157}\{158}\{159}\n\n\1.. Scout   \n\2.. Sniper  \n\3.. Soldier \n\4.. Demoman \n\5.. Medic   \n\6.. Hvwep   \n\7.. Pyro    \n\9.. Engineer\n\n\0.. \bCustom\b! \n");
		else
			CenterPrint(self, "\{157}\{158}\{159} \bChoose Your Class\b \{157}\{158}\{159}\n\n\1.. Scout   \n\2.. Sniper  \n\3.. Soldier \n\4.. Demoman \n\5.. Medic   \n\6.. Hvwep   \n\7.. Pyro    \n\8.. Spy     \n\9.. Engineer\n\n\0.. \bCustom\b! \n");
	}
	else
	{
		if (TeamFortress_TeamIsCivilian(self.team_no))
			CenterPrint(self, "Your team can only be Civilians.\n");
		else if (spy_off == #TRUE)
			CenterPrint(self, "\{157}\{158}\{159} \bChoose Your Class\b \{157}\{158}\{159}\n\n\1.. Scout   \n\2.. Sniper  \n\3.. Soldier \n\4.. Demoman \n\5.. Medic   \n\6.. Hvwep   \n\7.. Pyro    \n\9.. Engineer\n");
		else
			CenterPrint(self, "\{157}\{158}\{159} \bChoose Your Class\b \{157}\{158}\{159}\n\n\1.. Scout   \n\2.. Sniper  \n\3.. Soldier \n\4.. Demoman \n\5.. Medic   \n\6.. Hvwep   \n\7.. Pyro    \n\8.. Spy     \n\9.. Engineer\n");
	}*/
	// PZ: redid the above as one concatenated string
	if (TeamFortress_TeamIsCivilian(self->team_no))
		classMenu = "Your team can only be Civilians.\n";
	else
	{
		if (spy_off == PR_TRUE)
			classMenu = S_("\x9D\x9E\x9F ^bChoose Your Class^b \x9D\x9E\x9F\n\n  ^1.. Scout     \n  ^2.. Sniper    \n  ^3.. Soldier   \n  ^4.. Demoman   \n  ^5.. Medic     \n  ^6.. Hvwep     \n  ^7.. Pyro      \n  ^9.. Engineer  \n");
		else
			classMenu = S_("\x9D\x9E\x9F ^bChoose Your Class^b \x9D\x9E\x9F\n\n  ^1.. Scout     \n  ^2.. Sniper    \n  ^3.. Soldier   \n  ^4.. Demoman   \n  ^5.. Medic     \n  ^6.. Hvwep     \n  ^7.. Pyro      \n  ^8.. Spy       \n  ^9.. Engineer  \n");
		if (custom_mode != 2)
		{
			if (((int)floor(time) & 1) == 0) // PZ: Making it flash. A lot of the newb, no-namers don't seem to ever customize.
				classMenu = strcat(classMenu, S_("\n^[ ^0.. ^bCustom^b!! ^]\n"));
			else
				classMenu = strcat(classMenu, S_("\n^[ ^0.. Custom^b!!^b ^]\n"));
		}
		if (neo.isModeActive()) // Neo mode
		{
			if (((int)floor(time) & 1) == 0) // PZ: Making it flash.
				classMenu = strcat(classMenu, S_("\n\n\n\n^] Neo Mode - BETA ^[\n"));
			else
				classMenu = strcat(classMenu, S_("\n\n\n\n^] Neo Mode - ^bBETA^b ^[\n"));
		}
		if (self->playerclass) // PZ: If they currently already have a class (i.e., they used 'changeclass'), let them know they can cancel the menu.
			classMenu = strcat(classMenu, S_("\nUse ^bchangeclass^b again to\n"
			                              "cancel class selection. \n"));
	}
	CenterPrint(self, classMenu);
	// PZ: END
}

void Menu_Class_Input(float inp)
{
	float displayClassHelp; // PZ

	if (inp > 10 || inp < 1)
		return;

	if (inp == 10 && custom_mode == 2)
	{
		self->impulse = 0;
		return;
	}

	// PZ: if we used "changeclass", don't immediately give us the class help (wait until respawn)
	if (!self->playerclass) displayClassHelp = PR_TRUE;
	else                    displayClassHelp = PR_FALSE;

	self->impulse = inp + PR_TF_CHANGEPC;
	if (inp == 10) //We picked custom, change it from random
		self->impulse = PR_PC_CUSTOM + PR_TF_CHANGEPC;

	//WK Default to Custom class. TODO: Make this a serverside option - OfN - done!
	//self.impulse = #PC_CUSTOM + #TF_CHANGEPC;

	TeamFortress_ChangeClass();
	ResetMenu();

	if (PR_DISPLAY_CLASS_HELP && displayClassHelp) // PZ: added "&& displayClassHelp"
		self->current_menu = PR_MENU_CLASSHELP;
	else
		self->current_menu = PR_MENU_DEFAULT;
	self->menu_displaytime = 0;

	self->impulse = 0;
}

void Menu_Drop()
{
	if (self->weapons_carried & PR_WEAP_SPANNER)
		CenterPrint(self, S_("^bDrop or Make^b:                   \n\n^1.. Shells                      \n^2.. Nails                       \n^3.. Rockets                     \n^4.. Cells                       \n\n^0.. ^bNothing^b                     \n\n"));
	else
		CenterPrint(self, S_("^bDrop^b:                           \n\n^1.. Shells                      \n^2.. Nails                       \n^3.. Rockets                     \n^4.. Cells                       \n\n^0.. ^bNothing^b                     \n\n"));
}

void Menu_RepeatHelp()
{
	CenterPrint(self, S_("Press ^9 to see this help again\n"));
}

void Menu_Drop_Input(float inp)
{
	if ((inp > 0) && (inp < 5))
	{
		TeamFortress_DropAmmo(inp);
	}

	//if ((inp > 0) && (inp < 6))
	if (inp == 10)
		ResetMenu();

	self->impulse = 0;
}

void Menu_DoBindings_Input(float inp)
{/*- unusefull
	local string st;

	if (inp < 4)
	{
		self.impulse = 0;
		if (inp == 1)
		{
			// Flag info
			stuffcmd(self, "bind q \"impulse 23\"\n");
			// Hook
			stuffcmd(self, "bind e \"impulse 22\"\n");
			// Grenade 1
			stuffcmd(self, "bind r \"+gren1\"\n");
			// Grenade 2
			stuffcmd(self, "bind f \"+gren2\"\n");
			// Detonate Pipebombs
			stuffcmd(self, "bind v \"detpipe\"\n");
			// Drop ammo
			stuffcmd(self, "bind c \"dropammo\"\n");
			// Scan
			stuffcmd(self, "bind x \"scan50\"\n");
			// Showclasses
			stuffcmd(self, "bind g \"showclasses\"\n");
			// Inventory
			stuffcmd(self, "bind z \"inv\"\n");

			self.menu_count = #MENU_REFRESH_RATE;
			self.current_menu = #MENU_SHOWBINDS1;
			self.menu_displaytime = 0;
			return;
		}
		else if (inp == 2)
		{
			// Flag info
			stuffcmd(self, "bind s \"impulse 23\"\n");
			// Hook
			stuffcmd(self, "bind x \"impulse 22\"\n");
			// Grenade 1
			stuffcmd(self, "bind d \"+gren1\"\n");
			// Grenade 2
			stuffcmd(self, "bind c \"+gren2\"\n");
			// Detonate Pipebombs
			stuffcmd(self, "bind f \"detpipe\"\n");
			// Drop ammo
			stuffcmd(self, "bind v \"dropammo\"\n");
			// Scan
			stuffcmd(self, "bind b \"scan50\"\n");
			// Showclasses
			stuffcmd(self, "bind g \"showclasses\"\n");
			// Inventory
			stuffcmd(self, "bind n \"inv\"\n");

			self.menu_count = #MENU_REFRESH_RATE;
			self.current_menu = #MENU_SHOWBINDS2;
			self.menu_displaytime = 0;
			return;
		}
		ResetMenu();
	}*///- save space
}

void Menu_Input(float inp)
{
	//if (self.current_menu >= #MENU_PRIMARY_WEAPON && self.current_menu <= #MENU_OPTION)
	if (self->current_menu >= PR_MENU_CUSTOM_MIN && self->current_menu <= PR_MENU_CUSTOM_MAX)
	if (inp >= 1 && inp <= 7)
	{
		if (!CanBuyItem(inp))
		{
			CuTFMenuSound(PR_MENUSOUND_WRONG);
			sprint(self,PR_PRINT_HIGH,"This specific item is disabled, sorry!\n");
			self->impulse = 0;
			return;
		}

		if (ItemIsFree(inp))
		{
			CuTFMenuSound(PR_MENUSOUND_WRONG);
			sprint(self,PR_PRINT_HIGH,"You can not sell this item, sorry!\n");
			self->impulse = 0;
			return;
		}
	}

	if (self->current_menu == PR_MENU_TEAM)
		Menu_Team_Input(inp);
	else if (self->current_menu == PR_MENU_CLASS)
		Menu_Class_Input(inp);
	else if (self->current_menu == PR_MENU_DROP)
		Menu_Drop_Input(inp);
	else if (self->current_menu == PR_MENU_SPY)
		Menu_Spy_Input(inp);
	else if (self->current_menu == PR_MENU_SPY_SKIN)
		Menu_Spy_Skin_Input(inp);
	else if (self->current_menu == PR_MENU_SPY_COLOR)
		Menu_Spy_Color_Input(inp);
	else if (self->current_menu == PR_MENU_ENGINEER)
		Menu_Engineer_Input(inp);
	else if (self->current_menu == PR_MENU_ENGINEER_CONFIRM_DISMANTLE)  // PZ
		Menu_EngineerConfirmDismantle_Input(inp);
	else if (self->current_menu == PR_MENU_ENGINEER_FIX_DISPENSER)
		Menu_EngineerFix_Dispenser_Input(inp);
	else if (self->current_menu == PR_MENU_ENGINEER_FIX_TESLA)
		Menu_EngineerFix_Tesla_Input(inp);
	else if (self->current_menu == PR_MENU_ENGINEER_FIX_TESLA2)
		Menu_EngineerFix_Tesla_Input2(inp);
	else if (self->current_menu == PR_MENU_ENGINEER_FIX_SENTRYGUN)
		Menu_EngineerFix_SentryGun_Input(inp);
	else if (self->current_menu == PR_MENU_ENGINEER_FIX_CAMERA)
		Menu_EngineerFix_Camera_Input(inp);
	else if (self->current_menu == PR_MENU_ENGINEER_FIX_SENSOR)
		Menu_EngineerFix_Sensor_Input(inp);
	else if (self->current_menu == PR_MENU_ENGINEER_FIX_TELEPORTER)
		Menu_EngineerFix_Teleporter_Input(inp);
	else if (self->current_menu == PR_MENU_ENGINEER_FIX_FIELDGEN)
		Menu_EngineerFix_FieldGen_Input(inp);
	else if (self->current_menu == PR_MENU_DISPENSER)
		Menu_Dispenser_Input(inp);
	else if (self->current_menu == PR_MENU_CLASSHELP)
		Menu_ClassHelp_Input(inp);
	else if (self->current_menu == PR_MENU_PICKBINDS)
		Menu_DoBindings_Input(inp);
	else if (self->current_menu == PR_MENU_DEMON)
		Menu_Demon_Input(inp);
	else if (self->current_menu == PR_MENU_ARMY)
		Menu_Army_Input(inp);
	else if (self->current_menu == PR_MENU_E_HACK)
		Menu_EnemyHax_Inp(inp);
	else if (self->current_menu == PR_MENU_F_HACK)
		Menu_FriendHax_Inp(inp);
	else if (self->current_menu == PR_MENU_CRUSADER)
		Menu_Crusader_Input(inp);
	else if (self->current_menu == PR_MENU_GUERILLA)
		Menu_Guerilla_Input(inp);
	else if (self->current_menu == PR_MENU_PUNISH)
		Menu_Punish_Input(inp);
	else if (self->current_menu == PR_MENU_VOTEMAP)
		Menu_VoteMap_Input(inp);
	else if (self->current_menu == PR_MENU_VOTEBOT) // PZ: for "votebot"
		Menu_VoteBot_Input(inp);
	else if (self->current_menu == PR_MENU_PRIMARY_WEAPON) //WK --
		Menu_PrimaryWeapon_Input(inp);
	else if (self->current_menu == PR_MENU_SECONDARY_WEAPON)
		Menu_SecondaryWeapon_Input(inp);
	else if (self->current_menu == PR_MENU_MISC_WEAPON)
		Menu_MiscWeapon_Input(inp);
	else if (self->current_menu == PR_MENU_LEGS)
		Menu_Legs_Input(inp);
	else if (self->current_menu == PR_MENU_HEALTH)
		Menu_Health_Input(inp);
	else if (self->current_menu == PR_MENU_ARMOR)
		Menu_Armor_Input(inp);
	else if (self->current_menu == PR_MENU_SPECIAL)
		Menu_Special_Input(inp);
	else if (self->current_menu == PR_MENU_SPECIAL2)
		Menu_Special2_Input(inp);
	else if (self->current_menu == PR_MENU_GREN1)
		Menu_Gren1_Input(inp);
	else if (self->current_menu == PR_MENU_GREN2)
		Menu_Gren2_Input(inp);
	else if (self->current_menu == PR_MENU_ENGINEERING)
		Menu_Engineering_Input(inp);
	else if (self->current_menu == PR_MENU_PROFESSION)
		Menu_Profession_Input(inp);
	  else if (self->current_menu == PR_MENU_PROFESSION2)
		Menu_Profession2_Input(inp);
	else if (self->current_menu == PR_MENU_PROFICIENCY)
		Menu_Proficiency_Input(inp);
	else if (self->current_menu == PR_MENU_OPTION)
		Menu_Option_Input(inp); //WK --
	else if (self->current_menu == PR_MENU_TOOLS)
		Menu_ToolWeapon_Input (inp);
	else if (self->current_menu == PR_MENU_MOREOPTION)
		Menu_MoreOption_Input (inp);
}

void Menu_Intro()
{
	//CenterPrint3(self, #MSG_INTRO, #MSG_INTRO2, #MSG_INTRO3);
	PrintProzacMOTD(); // PZ NOTE: I'm not sure when this is called. The other call to this function appeared to be the only one actually used in the game.
}

void Menu_ClassHelp()
{
	string classHelp; // PZ

	// PZ NOTE: swap every 10  (only works if Class Help lasts for 30 menu frames)
	//          Look at the bottom of this function. On frame 30, it prints "" to clear out the class help.
	// 0 1 2 3 4 5 6 7 8 9   10 11 12 13 14 15 16 17 18 19   20 21 22 23 24 25 26 27 28 29

	// first page
	if ((self->menu_displaytime >=  0 && self->menu_displaytime <=  9) ||            // PZ: was < 5
		(self->menu_displaytime >= 20 && self->menu_displaytime <= 29))
		//(self.menu_displaytime >= 20 && self.menu_displaytime <= 24))
	{
		/*if (self.playerclass == #PC_SCOUT)
			CenterPrint(self, "\bSCOUT\b Details:               \n\n\bWEAPONS\b:                     \n2.. Shotgun                  \n4.. Nailgun                  \n\nPress \9 for alias help\n");
		else if (self.playerclass == #PC_SNIPER)
			CenterPrint(self, "\bSNIPER\b Details:              \n\n\bWEAPONS\b:                     \n2.. Sniper Rifle/Auto Rifle  \n4.. Nailgun                  \n\nPress \9 for alias help\n");
		else if (self.playerclass == #PC_SOLDIER)
			CenterPrint(self, "\bSOLDIER\b Details:             \n\n\bWEAPONS\b:                     \n2.. Shotgun                  \n3.. Super Shotgun            \n7.. Rocket Launcher          \n\nPress \9 for alias help\n");
		else if (self.playerclass == #PC_DEMOMAN)
			CenterPrint(self, "\bDEMOLITIONS MAN\b Details:     \n\n\bWEAPONS\b:                     \n2.. Shotgun                  \n6.. Grenade/Pipebomb Launcher\n                             \n\nPress \9 for alias help\n");
		else if (self.playerclass == #PC_MEDIC)
			CenterPrint(self, "\bMEDIC\b Details:               \n\n\bWEAPONS\b:                     \n1.. Medikit/BioWeapon        \n2.. Shotgun                  \n3.. Super Shotgun            \n5.. Super Nailgun            \n\nPress \9 for alias help\n");
		else if (self.playerclass == #PC_HVYWEAP)
			CenterPrint(self, "\bHEAVY WEAPONS GUY\b Details:   \n\n\bWEAPONS\b:                     \n2.. Shotgun                  \n3.. Super Shotgun            \n7.. Assault Cannon           \n\nPress \9 for alias help\n");
		else if (self.playerclass == #PC_PYRO)
			CenterPrint(self, "\bPYRO\b Details:                \n\n\bWEAPONS\b:                     \n2.. Shotgun                  \n6.. Flamethrower             \n7.. Incendiary Cannon        \n\nPress \9 for alias help\n");
		else if (self.playerclass == #PC_SPY)
			CenterPrint(self, "\bSPY\b Details:                 \n\n\bWEAPONS\b:                     \n2.. Tranquiliser Gun         \n3.. Super Shotgun            \n4.. Nailgun                  \n\nPress \9 for alias help\n");
		else if (self.playerclass == #PC_ENGINEER)
			CenterPrint(self, "\bENGINEER\b Details:            \n\n\bWEAPONS\b:                     \n1.. Spanner                  \n2.. RailGun                  \n3.. Super Shotgun            \n\nPress \9 for alias help\n");*/
		// PZ: allowed for concatenation
		if (self->playerclass == PR_PC_SCOUT)
			classHelp = S_("^bSCOUT^b Details:               \n\n^bWEAPONS^b:                     \n2.. Shotgun                  \n4.. Nailgun                  \n\n");
		else if (self->playerclass == PR_PC_SNIPER)
			classHelp = S_("^bSNIPER^b Details:              \n\n^bWEAPONS^b:                     \n2.. Sniper Rifle/Auto Rifle  \n4.. Nailgun                  \n\n");
		else if (self->playerclass == PR_PC_SOLDIER)
			classHelp = S_("^bSOLDIER^b Details:             \n\n^bWEAPONS^b:                     \n2.. Shotgun                  \n3.. Super Shotgun            \n7.. Rocket Launcher          \n\n");
		else if (self->playerclass == PR_PC_DEMOMAN)
			classHelp = S_("^bDEMOLITIONS MAN^b Details:     \n\n^bWEAPONS^b:                     \n2.. Shotgun                  \n6.. Grenade/Pipebomb Launcher\n                             \n\n");
		else if (self->playerclass == PR_PC_MEDIC)
			classHelp = S_("^bMEDIC^b Details:               \n\n^bWEAPONS^b:                     \n1.. Medikit/BioWeapon        \n2.. Shotgun                  \n3.. Super Shotgun            \n5.. Super Nailgun            \n\n");
		else if (self->playerclass == PR_PC_HVYWEAP)
			classHelp = S_("^bHEAVY WEAPONS GUY^b Details:   \n\n^bWEAPONS^b:                     \n2.. Shotgun                  \n3.. Super Shotgun            \n7.. Assault Cannon           \n\n");
		else if (self->playerclass == PR_PC_PYRO)
			classHelp = S_("^bPYRO^b Details:                \n\n^bWEAPONS^b:                     \n2.. Shotgun                  \n6.. Flamethrower             \n7.. Incendiary Cannon        \n\n");
		else if (self->playerclass == PR_PC_SPY)
			classHelp = S_("^bSPY^b Details:                 \n\n^bWEAPONS^b:                     \n2.. Tranquiliser Gun         \n3.. Super Shotgun            \n4.. Nailgun                  \n\n");
		else if (self->playerclass == PR_PC_ENGINEER)
			classHelp = S_("^bENGINEER^b Details:            \n\n^bWEAPONS^b:                     \n1.. Spanner                  \n2.. RailGun                  \n3.. Super Shotgun            \n\n");
		else if (self->playerclass == PR_PC_CUSTOM)
			classHelp = S_("^bCUSTOM CLASS^b Details:            \n\n^bWEAPONS^b:                     \n1..                          \n2..                          \n3..                          \n\n");
	}
	// second page
	else
	{
		/*if (self.playerclass == #PC_SCOUT)
			CenterPrint(self, "\bGRENADES\b:            \nFlash Grenade       \nConcussion Grenade  \n\n\bSPECIAL COMMANDS\b:   \nScanner: scan10,scan50,scan250\nHolograph:  holo        \n\n\nPress \9 for alias help\n");
		else if (self.playerclass == #PC_SNIPER)
			CenterPrint(self, "\bGRENADES\b:            \nHand Grenade        \nFlare               \n\n\bSPECIAL COMMANDS\b:   \nautozoom : Toggle Rifle Autozooming\n\n\nPress \9 for alias help\n");
		else if (self.playerclass == #PC_SOLDIER)
			CenterPrint(self, "\bGRENADES\b:            \nHand Grenade        \nNail Grenade        \n\n\bSPECIAL COMMANDS\b:   \nNone\n\n\nPress \9 for alias help\n");
		else if (self.playerclass == #PC_DEMOMAN)
			CenterPrint(self, "\bGRENADES\b:            \nHand Grenade        \nMirv Grenade        \n\n\bSPECIAL COMMANDS\b:   \nThe Detpack : det5,det20,det50\nDetonate Pipebombs : detpipe  \n\n\nPress \9 for alias help\n");
		else if (self.playerclass == #PC_MEDIC)
			CenterPrint(self, "\bGRENADES\b:            \nHand Grenade        \nConcussion Grenade  \n\n\bSPECIAL COMMANDS\b:   \nNone\n\n\nPress \9 for alias help\n");
		else if (self.playerclass == #PC_HVYWEAP)
			CenterPrint(self, "\bGRENADES\b:            \nHand Grenade        \nMirv Grenade        \n\n\bSPECIAL COMMANDS\b:   \nNone\n\n\nPress \9 for alias help\n");
		else if (self.playerclass == #PC_PYRO)
			CenterPrint(self, "\bGRENADES\b:            \nHand Grenade        \nNapalm Grenade      \n\n\bSPECIAL COMMANDS\b:   \nNone\n\n\nPress \9 for alias help\n");
		else if (self.playerclass == #PC_SPY)
			CenterPrint(self, "\bGRENADES\b:            \nHand Grenade        \nHallucinogen Grenade\n\n\bSPECIAL COMMANDS\b:   \nGo Undercover : disguise\nStart feigning: feign\n\n\nPress \9 for alias help\n");
		else if (self.playerclass == #PC_ENGINEER)
			CenterPrint(self, "\bGRENADES\b:            \nHand Grenade        \nEMP Grenade         \n\n\bSPECIAL COMMANDS\b:   \nStart Building : build  \n\n\nPress \9 for alias help\n");*/
		/* WK Stop GRENADES - Build your own class from appearing
			else if (self.playerclass == #PC_CUSTOM)
				CenterPrint(self, "\bGRENADES\b:            \nBuild your own class!\n\n");*/
		// PZ: allowed for concatenation
		if (self->playerclass == PR_PC_SCOUT)
			classHelp = S_("^bGRENADES^b:            \nFlash Grenade       \nConcussion Grenade  \n\n^bSPECIAL COMMANDS^b:   \nScanner: scan10,scan50,scan250\nHolograph:  holo        \n\n");
		else if (self->playerclass == PR_PC_SNIPER)
			classHelp = S_("^bGRENADES^b:            \nHand Grenade        \nFlare               \n\n^bSPECIAL COMMANDS^b:   \nautozoom : Toggle Rifle Autozooming\n\n");
		else if (self->playerclass == PR_PC_SOLDIER)
			classHelp = S_("^bGRENADES^b:            \nHand Grenade        \nNail Grenade        \n\n^bSPECIAL COMMANDS^b:   \nNone\n\n");
		else if (self->playerclass == PR_PC_DEMOMAN)
			classHelp = S_("^bGRENADES^b:            \nHand Grenade        \nMirv Grenade        \n\n^bSPECIAL COMMANDS^b:   \nThe Detpack : det5,det20,det50\nDetonate Pipebombs : detpipe  \n\n");
		else if (self->playerclass == PR_PC_MEDIC)
			classHelp = S_("^bGRENADES^b:            \nHand Grenade        \nConcussion Grenade  \n\n^bSPECIAL COMMANDS^b:   \nNone\n\n");
		else if (self->playerclass == PR_PC_HVYWEAP)
			classHelp = S_("^bGRENADES^b:            \nHand Grenade        \nMirv Grenade        \n\n^bSPECIAL COMMANDS^b:   \nNone\n\n");
		else if (self->playerclass == PR_PC_PYRO)
			classHelp = S_("^bGRENADES^b:            \nHand Grenade        \nNapalm Grenade      \n\n^bSPECIAL COMMANDS^b:   \nNone\n\n");
		else if (self->playerclass == PR_PC_SPY)
			classHelp = S_("^bGRENADES^b:            \nHand Grenade        \nHallucinogen Grenade\n\n^bSPECIAL COMMANDS^b:   \nGo Undercover : disguise\nStart feigning: feign\n\n");
		else if (self->playerclass == PR_PC_ENGINEER)
			classHelp = S_("^bGRENADES^b:            \nHand Grenade        \nEMP Grenade         \n\n^bSPECIAL COMMANDS^b:   \nStart Building : build  \n\n");
		else if (self->playerclass == PR_PC_CUSTOM)
			classHelp = S_("^bGRENADES^b:            \n                    \n                    \n\n^bSPECIAL COMMANDS^b:   \n                        \n\n");
	}
	classHelp = strcat(classHelp, "\n");
	/*local string animation;
	// generate eye-catching animation   ������
	local float x;
	x = self.menu_displaytime;
	x = x - (8 * floor(x / 8)); // x = x mod 8    (change 8's to change the modulus)
	if      (x == 0) animation = "\b|\b����\b|\b\n";
	else if (x == 1) animation = "\b/\b����\b/\b\n";
	else if (x == 2) animation = "\b-\b����\b-\b\n";
	else if (x == 3) animation = "\b\\\b����\b\\\b\n";
	else if (x == 4) animation = "\b|\b����\b|\b\n";
	else if (x == 5) animation = "\b/\b����\b/\b\n";
	else if (x == 6) animation = "\b-\b����\b-\b\n";
	else if (x == 7) animation = "\b\\\b����\b\\\b\n";
	classHelp = strcat(classHelp, animation);*/

	if ((int)self->menu_displaytime & 1)  // if the frame is an odd number
		classHelp = strcat(classHelp, S_("Press ^9 for a list of commands\n"));
	else
		classHelp = strcat(classHelp, S_("^bPress ^9 for a list of commands ^b\n"));

	//classHelp = strcat(classHelp, animation);

	if (self->menu_displaytime != 30) CenterPrint(self, classHelp); // PZ: added
	else                             CenterPrint(self, "");        // PZ: make help actually disappear, not hang around with frozen animation
}

void Menu_StuffClassHelp()
{
	/*
	if (self.playerclass == #PC_SCOUT)
	{
		sprint(self, #PRINT_HIGH, "����� Details:\n�������:\n   2.. Shotgun\n   4.. Nailgun\n");
		sprint(self, #PRINT_HIGH, "��������:\n   Concussion Grenade\n   Flare\n������� ��������:\n");
		sprint(self, #PRINT_HIGH, "Scanner: scan10,scan30,scan100\nHolograph: holo\n\n\n");
	}
	else if (self.playerclass == #PC_SNIPER)
	{
		sprint (self, #PRINT_HIGH, "������ Details:\n\n�������:\n   2.. Sniper Rifle/Auto Rifle\n");
		sprint (self, #PRINT_HIGH, "4.. Nailgun\n��������:\n   Hand Grenade\n\n\n������� ��������:\n");
		sprint (self, #PRINT_HIGH, "  autozoom : Toggle Rifle Autozooming\n\n\n");
	}
	else if (self.playerclass == #PC_SOLDIER)
	{
		sprint (self, #PRINT_HIGH, "������� Details:\n\n�������:\n   2.. Shotgun\n   3.. Super Shotgun\n");
		sprint (self, #PRINT_HIGH, "   7.. Rocket Launcher\n��������:\n   Hand Grenade\n   Nail Grenade\n");
		sprint (self, #PRINT_HIGH, "������� ��������:\n   None\n\n\n");
	}
	else if (self.playerclass == #PC_DEMOMAN)
	{
		sprint (self, #PRINT_HIGH, "����������� ��� Details:\n\n�������:\n   2.. Shotgun\n   6.. Grenade/Pipebomb Launcher\n");
		sprint (self, #PRINT_HIGH, "��������:\n   Hand Grenade\n   Mirv Grenade\n");
		sprint (self, #PRINT_HIGH, "������� ��������:\n   The Detpack : det5,det20,det50\n   Detonate Pipebombs : detpipe\n\n\n");
	}
	else if (self.playerclass == #PC_MEDIC)
	{
		sprint (self, #PRINT_HIGH, "����� Details:\n\n�������:\n   1.. Medikit/BioWeapon\n   2.. Shotgun\n   3.. Super Shotgun\n");
		sprint (self, #PRINT_HIGH, "   5.. Super Nailgun\n��������:\n   Hand Grenade\n   Concussion Grenade\n");
		sprint (self, #PRINT_HIGH, "������� ��������:\n   None\n\n\n");
	}
	else if (self.playerclass == #PC_HVYWEAP)
	{
		sprint (self, #PRINT_HIGH, "����� ������� ��� Details:\n\n�������:\n   2.. Shotgun\n   3.. Super Shotgun\n");
		sprint (self, #PRINT_HIGH, "   7.. Assault Cannon\n��������:\n   Hand Grenade\n");
		sprint (self, #PRINT_HIGH, "   Mirv Grenade\n������� ��������:\n   None\n\n\n");
	}
	else if (self.playerclass == #PC_PYRO)
	{
		sprint (self, #PRINT_HIGH, "���� Details:\n\n�������:\n   2.. Shotgun\n   6.. Flamethrower\n   7.. Incendiary Cannon\n");
		sprint (self, #PRINT_HIGH, "��������:\n   Hand Grenade\n   Napalm Grenade\n");
		sprint (self, #PRINT_HIGH, "������� ��������:\n   None\n\n\n");
	}
	else if (self.playerclass == #PC_SPY)
	{
		sprint (self, #PRINT_HIGH, "��� Details:\n\n�������:\n   2.. Tranquiliser Gun\n   3.. Super Shotgun\n   4.. Nailgun\n");
		sprint (self, #PRINT_HIGH, "��������:\n   Hand Grenade\n   Hallucinogenic Grenade\n");
		sprint (self, #PRINT_HIGH, "������� ��������:\n   Go Undercover : disguise\n   Start feigning: feign\n\n\n");
	}
	else if (self.playerclass == #PC_ENGINEER)
	{
		sprint (self, #PRINT_HIGH, "�������� Details:\n\n�������:\n   1.. Spanner\n   2.. RailGun\n   3.. Super Shotgun\n");
		sprint (self, #PRINT_HIGH, "��������:\n   Hand Grenade\n   EMP Grenade\n");
		sprint (self, #PRINT_HIGH, "������� ��������:\n   Start Building : build\n\n\n");
	}
	*/
}

void Menu_ClassHelp2()
{
	//CenterPrint(self, "\bCOMMANDS\b:     \n\ninv       : Show inventory \n+gren1    : Throw grenade 1\n+gren2    : Throw Grenade 2\nreload    : Force a reload \ndropammo  : drop some ammo \nskill     : use job ability\n\btaunt1\b    : taunts (also 2,3,4)\n");
	// PZ NOTE: the standard client has a max width of 40 characters for CenterPrint()
	CenterPrint(self, S_("^bGENERAL CONSOLE COMMANDS^b:     \n"
	                     "(press ^b~^b to open the console)     \n\n"
	                     "^bchangeclass^b   Change your class         \n"
	                     "^bchangeteam^b    Change your team          \n"
	                     "^bskill^b         Use job ability           \n"
	                     "^b+gren1^b        Throw grenade 1           \n"
	                     "^b+gren2^b        Throw Grenade 2           \n"
	                     "^breload^b        Reload your weapon        \n"
	                     "^binv^b           Show inventory            \n"
	                     "^bvotemap^b       Vote to change the map    \n"
	                     "^bvotebot^b       Vote to adjust the bots   \n"
	                     "^bpunish^b        Vote to punish a player   \n"
	                     "^bdropammo^b      Drop some ammo            \n"
	                     "^bid^b            ID player in your xhairs  \n"
	                     "^bmaphelp^b       Get map help, if provided \n"
	                     "^bflaginfo^b      Get location of the flag  \n"
	                     "^bsaveme^b        Call for a medic          \n"
	                     "^btaunt1^b        Taunts (also 2 through 5) \n\n"
	                     "Bind a key to a command.                    \n"
	                     "Examples: ^bbind e skill^b                  \n"
	                     "          ^bbind q \"inv; wait; flaginfo\"^b  \n\n"
	                     "Type ^bcmd help^b to see additional           \n"
	                     "available commands.                       \n"));
}

void Menu_ClassHelp_Input(float inp)
{
	if (self->playerclass == PR_PC_UNDEFINED) return;

//CH changed inp from 8 to 9,  lgun uses 8
	if (inp == 9)
	{
		Menu_ClassHelp2();
		self->menu_count = PR_MENU_REFRESH_RATE;
		self->current_menu = PR_MENU_CLASSHELP2;
		self->menu_displaytime = 0;
		self->impulse = 0;
	}

	// Otherwise, the impulse is passed on
}

void Menu_DoBindings()
{
	//CenterPrint(self, "\1.. Use keys: E R F V C Q X G Z\n\2.. Use keys: S X D C F V B G N\n\3.. Don't make bindings        \n");
}

void Menu_ShowBindings1()
{
	//CenterPrint(self, "\bQ\b.. Flaginfo      \n\bE\b.. Hook          \n\bR\b.. Throw Gren 1  \n\bF\b.. Throw Gren 2  \n\bV\b.. Det. Pipebombs\n\bC\b.. Drop ammo     \n\bX\b.. Use Scanner   \n\bG\b.. Showclasses   \n\bZ\b.. inventory     \n");
}

void Menu_ShowBindings2()
{
	//CenterPrint(self, "\bS\b.. Flaginfo      \n\bX\b.. Hook          \n\bD\b.. Throw Gren 1  \n\bC\b.. Throw Gren 2  \n\bF\b.. Det. Pipebombs\n\bV\b.. Drop ammo     \n\bB\b.. Use Scanner   \n\bG\b.. Showclasses   \n\bN\b.. inventory     \n");
}

//============================================================================
// SPY menus for the Skin and Color changing ability
void Menu_Spy()
{
		if (self->is_feigning)
	{
		if (self->undercover_team != 0 && self->undercover_skin != 0)
			CenterPrint(self, S_("^bAction^b:                           \n\n^1.. Change Skin                 \n^2.. Change Color                \n^3.. Stop Feigning               \n^4.. Reset Skin and Color        \n\n^5.. ^bNothing^b                     \n\n"));
		else if (self->undercover_team != 0)
			CenterPrint(self, S_("^bAction^b:                           \n\n^1.. Change Skin                 \n^2.. Change Color                \n^3.. Stop Feigning               \n^4.. Reset Color                 \n\n^5.. ^bNothing^b                     \n\n"));
		else if (self->undercover_skin != 0)
			CenterPrint(self, S_("^bAction^b:                           \n\n^1.. Change Skin                 \n^2.. Change Color                \n^3.. Stop Feigning               \n^4.. Reset Skin                  \n\n^5.. ^bNothing^b                     \n\n"));
		else
			CenterPrint(self, S_("^bAction^b:                           \n\n^1.. Change Skin                 \n^2.. Change Color                \n^3.. Stop Feigning               \n\n^4.. ^bNothing^b                     \n\n"));
	}
	else
	{
		if (self->undercover_team != 0 && self->undercover_skin != 0)
			CenterPrint(self, S_("^bAction^b:                           \n\n^1.. Change Skin                 \n^2.. Change Color                \n^3.. Start Feigning              \n^4.. Reset Skin and Color        \n\n^5.. ^bNothing^b                     \n\n"));
		else if (self->undercover_team != 0)
			CenterPrint(self, S_("^bAction^b:                           \n\n^1.. Change Skin                 \n^2.. Change Color                \n^3.. Start Feigning              \n^4.. Reset Color                 \n\n^5.. ^bNothing^b                     \n\n"));
		else if (self->undercover_skin != 0)
			CenterPrint(self, S_("^bAction^b:                           \n\n^1.. Change Skin                 \n^2.. Change Color                \n^3.. Start Feigning              \n^4.. Reset Skin                  \n\n^5.. ^bNothing^b                     \n\n"));
		else
			CenterPrint(self, S_("^bAction^b:                           \n\n^1.. Change Skin                 \n^2.. Change Color                \n^3.. Start Feigning              \n\n^4.. ^bNothing^b                     \n\n"));
	}
}

void Menu_Spy_Input(float inp)
{
	//local float tc;
	//local string st;

	if (inp == 1 || inp == 2)
	{
		if (self->effects & (PR_EF_DIMLIGHT | PR_EF_BRIGHTLIGHT))
		{
			sprint(self, PR_PRINT_HIGH, "You can't go undercover while glowing.\n");
			ResetMenu();
			self->impulse = 0;
			return;
		}
		if (self->is_unabletospy == 1)
		{
			sprint(self, PR_PRINT_HIGH, "You can't go undercover right now.\n");
			ResetMenu();
			self->impulse = 0;
			return;
		}
	}

	if (inp == 1)
	{
		Menu_Spy_Skin();
		self->menu_count = PR_MENU_REFRESH_RATE;
		self->current_menu = PR_MENU_SPY_SKIN;
		self->menu_displaytime = 0;
		self->impulse = 0;
	}
	else if (inp == 2)
	{
		Menu_Spy_Color();
		self->menu_count = PR_MENU_REFRESH_RATE;
		self->current_menu = PR_MENU_SPY_COLOR;
		self->menu_displaytime = 0;
		self->impulse = 0;
	}
	else if (inp == 3)
	{
		TeamFortress_SpyFeignDeath(1); //CH Normal feign
		ResetMenu();
		self->impulse = 0;
	}
	else if (inp == 4)
	{
		Spy_RemoveDisguise(self);

		ResetMenu();
		self->impulse = 0;
	}
	else if (inp == 5 && (self->undercover_team != 0 || self->undercover_skin != 0))
	{
		ResetMenu();
		self->impulse = 0;
	}

	// Otherwise, the impulse is passed on
}

void Menu_Spy_Skin()
{
	CenterPrint(self, S_("^bChange Skin to^b:\n\n^1.. Scout   \n^2.. Sniper  \n^3.. Soldier \n^4.. Demoman \n^5.. Medic   \n^6.. Hvwep   \n^7.. Pyro    \n^8.. Spy     \n^9.. Engineer\n\n"));
}

void Menu_Spy_Skin_Input(float inp)
{
	if (inp < 10 && inp > 0)
	{
		TeamFortress_SpyChangeSkin(inp);
		ResetMenu();
		self->impulse = 0;
	}

	// Otherwise, the impulse is passed on
}

void Menu_Spy_Color()
{
	if (number_of_teams == 0)
	{
		sprint(self, PR_PRINT_HIGH, "No color changing allowed in deathmatch.\n");
		ResetMenu();
		self->impulse = 0;
		return;
	}

	// PZ: Spies can't change color in Neo mode.
	if (neo.isModeActive())
	{
		sprint(self, PR_PRINT_HIGH, "No color changing allowed in Neo Mode.\n");
		ResetMenu();
		self->impulse = 0;
		return;
	}

	if (number_of_teams == 1)
		CenterPrint(self, S_("^bChange Color to^b:\n\n^1.. Team One  \n\n"));
	else if (number_of_teams == 2)
		CenterPrint(self, S_("^bChange Color to^b:\n\n^1.. Team One  \n^2.. Team Two  \n\n"));
	else if (number_of_teams == 3)
		CenterPrint(self, S_("^bChange Color to^b:\n\n^1.. Team One  \n^2.. Team Two  \n^3.. Team Three\n\n"));
	else // if (number_of_teams == 4)
		CenterPrint(self, S_("^bChange Color to^b:\n\n^1.. Team One  \n^2.. Team Two  \n^3.. Team Three\n^4.. Team Four \n\n"));
}

void Menu_Spy_Color_Input(float inp)
{
	if (inp >= 1 && inp <= number_of_teams)
	{
		TeamFortress_SpyChangeColor(inp);
		ResetMenu();
		self->impulse = 0;
	}

	// Otherwise, the impulse is passed on
}

//============================================================================
// ENGINEER menus for the building ability
//WK Rewritten for better extensibility and less lines of code

// PZ - added dismantle/destroy confirmation menu (enough of those damn accidental dismantles!)
void Menu_EngineerConfirmDismantle()
{
	CenterPrint(self, S_("^bDismantle^b/^bDestroy Confirmation^b:\n\n^1.. Cancel                 \n\n^7.. Confirm                \n"));
}
void Menu_EngineerConfirmDismantle_Input(float inp)
{
	// PZ - need this check?
	if (self->classname != "player")
		return;

	if (inp == 7)
	{
		if (self->group_no == 1)
		{
			if (self->building == world)
				return;
			Menu_EngineerDismantle(); // player confirmed dismantle
		}
		else if (self->group_no == 2)
		{
			Menu_EngineerDestroy(); // player confirmed destroy
		}
	}
	if (inp == 1 || inp == 7)
	{
		ResetMenu();
		self->current_menu = PR_MENU_DEFAULT;
		self->impulse = 0;
		self->building = world;

		bound_other_ammo(self);

		if (self->armorvalue == 0)
		{
			self->armortype = 0;  // lost all armor
			self->armorclass = 0; // lost special armor
			self->items = self->items - (self->items & (PR_IT_ARMOR1 | PR_IT_ARMOR2 | PR_IT_ARMOR3));
		}

		W_SetCurrentAmmo();
	}
}

// after player confirms dismantle, this function executes it
void Menu_EngineerDismantle()
{
	if (self->building->classname == "building_dispenser")
	{
		sprint (self, PR_PRINT_HIGH, "You dismantle the Dispenser.\n");
		self->ammo_cells = self->ammo_cells + (PR_BUILD_COST_DISPENSER / 2);
		self->building->health=0; // - OfN aborts hacks on it
		self->building->real_owner->has_dispenser = PR_FALSE;
		dremove(self->building);
	}
	else if (self->building->classname == "building_sentrygun")
	{
		sprint(self, PR_PRINT_HIGH, "You dismantle the Sentry Gun.\n");
		//CH give .5 of build cost====give 25*level
		self->ammo_cells = self->ammo_cells + (PR_BUILD_COST_SENTRYGUN / 2) + (self->building->weapon * 25);
		//self.building.real_owner.has_sentry = #FALSE;
		self->building->real_owner->has_sentry = self->building->real_owner->has_sentry - (self->building->real_owner->has_sentry & self->building->dont_do_triggerwork);
		dremove(self->building->trigger_field);
		self->building->health=0; // - OfN aborts hacks on it
		dremove(self->building);
	}
	else if (self->building->classname == "building_tesla")
	{
		sprint(self, PR_PRINT_HIGH, "You dismantle the Tesla Gun.\n");
		//self.building.real_owner.has_turretized_tesla = 0;
		//CH give .5 of ammount of build====give 25*health level (0-3)====give .5 of cells
		self->ammo_cells = self->ammo_cells + (PR_BUILD_COST_TESLA / 2) + (self->building->ammo_rockets * 25) + (self->building->ammo_cells / 2);
		//self.building.real_owner.has_tesla = #FALSE;
		self->building->real_owner->has_tesla = self->building->real_owner->has_tesla - (self->building->real_owner->has_tesla & self->building->dont_do_triggerwork);
		dremove(self->building);
	}
	else if (self->building->classname == "building_camera")
	{
		sprint(self, PR_PRINT_HIGH, "You dismantle the Security Camera.\n");
		//CH will give you half of cost + max of 20
		self->ammo_cells = self->ammo_cells + (PR_BUILD_COST_CAMERA * 0.5) + floor(self->building->health / 40);
		self->building->real_owner->has_camera = PR_FALSE;
		self->building->health=0; // - OfN aborts hacks on it
		dremove(self->building);
	}
	else if (self->building->classname == "building_teleporter")
	{
		sprint(self, PR_PRINT_HIGH, "You dismantle the Teleporter Pad.\n");
		//CH will give you half of cost + max of 20
		self->ammo_cells = self->ammo_cells + (PR_BUILD_COST_TELEPORTER * 0.5) + floor(self->building->health / 15);
		self->building->real_owner->has_teleporter = (self->building->real_owner->has_teleporter - 1);
		self->building->health=0; // - OfN aborts hacks on it
		dremove(self->building);
	}
	else if (self->building->classname == "building_fieldgen")
	{
		sprint(self, PR_PRINT_HIGH, "You dismantle the Field Generator.\n");
		//CH will give you half of cost + max of 20
		self->ammo_cells = self->ammo_cells + (PR_BUILD_COST_FIELDGEN * 0.5) + floor(self->building->health / 15);
		self->building->real_owner->has_fieldgen = (self->building->real_owner->has_fieldgen - 1);
		self->building->health = 0; // - OfN aborts hacks on it
		dremove(self->building);
	}
	else if (self->building->classname == "building_sensor")
	{
		sprint(self, PR_PRINT_HIGH, "You dismantle the Motion Sensor.\n");
		//CH will give you half of cost + max of 20
		self->ammo_cells = self->ammo_cells + (PR_BUILD_COST_SENSOR * 0.5) + floor(self->building->health / 40);
		self->building->real_owner->has_sensor = PR_FALSE;
		self->building->health=0; // - OfN aborts hacks on it
		dremove(self->building);
	}
}

// after player confirms destroy, this function executes it
// .owned_by is used to indicate input in these menus
void Menu_EngineerDestroy()
{
	entity te, te2;

////////////////////////////////
// Double Tes or Sent
	if (self->owned_by >= 2 && self->owned_by <=3 && (self->cutf_items & PR_CUTF_DOUBLESENTRY || self->tf_items & PR_NIT_DOUBLETESLA))
	{
		if (self->cutf_items & PR_CUTF_DOUBLESENTRY)
		{
			if (self->owned_by == 2 && self->has_sentry & 1)
			{
				te = find(world, "classname", "building_sentrygun");
				while (te != world)
				{
					if (te->dont_do_triggerwork == 1)
					{
						if (te->real_owner == self)
						{
							if ((te->max_health - te->health) > (te->max_health * PR_SENTRIES_MINIMUMHP_TODETONATE_FACTOR))
							{
								sprint(self,PR_PRINT_HIGH,"Your sentry gun holds too much damage to be able to detonate it remotely!\n");
								self->impulse = 0;
								return;
							}
						}
						Find_And_Dmg("building_sentrygun",self,1,1);
					}
					te = find(te, "classname", "building_sentrygun");
				}
			}
			else if (self->owned_by == 3 && self->has_sentry & 2)
			{
				te = find(world, "classname", "building_sentrygun");
				while (te != world)
				{
					if (te->dont_do_triggerwork == 2)
					{
						if (te->real_owner == self)
						{
							if ((te->max_health - te->health) > (te->max_health * PR_SENTRIES_MINIMUMHP_TODETONATE_FACTOR))
							{
								sprint(self,PR_PRINT_HIGH,"Your sentry gun holds too much damage to be able to detonate it remotely!\n");
								self->impulse = 0;
								return;
							}
						}
						Find_And_Dmg("building_sentrygun",self,1,2);
					}
					te = find(te, "classname", "building_sentrygun");
				}
			}
		}
		else if (self->tf_items & PR_NIT_DOUBLETESLA)
		{
			if (self->owned_by == 2 && self->has_tesla & 1)
			{
				te = find(world, "classname", "building_tesla");
				while (te != world)
				{
					if (te->dont_do_triggerwork == 1)
					{
						if (te->real_owner == self)
						{
							if (te->tf_items & PR_NIT_SECURITY_CAMERA) {
								sprint(self, PR_PRINT_HIGH, "Can't detonate a rogue tesla. Sorry, it's not that easy.\n");
								ResetMenu();
								self->impulse = 0;
								return;
							}

							if ((te->max_health - te->health) > (te->max_health * PR_SENTRIES_MINIMUMHP_TODETONATE_FACTOR))
							{
								sprint(self,PR_PRINT_HIGH,"Your tesla holds too much damage to be able to detonate it remotely!\n");
								self->impulse = 0;
								return;
							}
						}

						Find_And_Dmg("building_tesla",self,1,1);
					}
					te = find(te, "classname", "building_tesla");
				}
			}
			else if (self->owned_by == 3 && self->has_tesla & 2)
			{
				te = find(world, "classname", "building_tesla");
				while (te != world)
				{
					if (te->dont_do_triggerwork == 2)
					{
						if (te->real_owner == self)
						{
							if (te->tf_items & PR_NIT_SECURITY_CAMERA) {
								sprint(self, PR_PRINT_HIGH, "Can't detonate a rogue tesla. Sorry, it's not that easy.\n");
								ResetMenu();
								self->impulse = 0;
								return;
							}

							if ((te->max_health - te->health) > (te->max_health * PR_SENTRIES_MINIMUMHP_TODETONATE_FACTOR))
							{
								sprint(self,PR_PRINT_HIGH,"Your tesla holds too much damage to be able to detonate it remotely!\n");
								self->impulse = 0;
								return;
							}
						}

						Find_And_Dmg("building_tesla",self,1,2);
					}
					te = find(te, "classname", "building_tesla");
				}
			}
		}
		ResetMenu();
		self->impulse = 0;
		return;
	}
////////////////////////////////
// Dispenser
	else if (self->owned_by == 1 && self->has_dispenser == PR_TRUE)
	{
		Find_And_Dmg("building_dispenser", self, 1,0);
		ResetMenu();
		self->impulse = 0;
	}
////////////////////////////////
// Sentry Gun
	else if (self->owned_by == 2 && self->has_sentry == PR_TRUE)
	{
		te2 = world;
		te = find(world,"classname","building_sentrygun");
		while(te != world && te2 == world)
		{
			if (te->real_owner == self)
				te2 = te;

			te = find(te, "classname","building_sentrygun");
		}

		if (te2 == world)
			return;

		if ((te2->max_health - te2->health) > (te2->max_health * PR_SENTRIES_MINIMUMHP_TODETONATE_FACTOR))
		{
			sprint(self,PR_PRINT_HIGH,"Your sentry holds too much damage to be able to detonate it remotely!\n");
			self->impulse = 0;
			return;
		}

		Find_And_Dmg("building_sentrygun", self, 1,0);
		ResetMenu();
		self->impulse = 0;
	}
////////////////////////////////
// Tesla Coil
	else if (self->owned_by == 3 && self->has_tesla == PR_TRUE)
	{
		te = find(world, "classname", "building_tesla");
		while (te != world)
		{
			if (te->real_owner == self)
			{
				if (te->tf_items & PR_NIT_SECURITY_CAMERA) {
					sprint(self, PR_PRINT_HIGH, "Can't detonate a rogue tesla. Sorry, it's not that easy.\n");
					ResetMenu();
					self->impulse = 0;
					return;
				}

				if ((te->max_health - te->health) > (te->max_health * PR_SENTRIES_MINIMUMHP_TODETONATE_FACTOR))
				{
					sprint(self,PR_PRINT_HIGH,"Your tesla holds too much damage to be able to detonate it remotely!\n");
					self->impulse = 0;
					return;
				}
			}
			te = find(te, "classname", "building_tesla");
		}

		Find_And_Dmg("building_tesla", self, 1,0);
		ResetMenu();
		self->impulse = 0;
	}
////////////////////////////////
// Camera
	else if (self->owned_by == 4 && self->has_camera == 1)
	{
		if (self->option < time)
			Find_And_Dmg("building_camera", self, 1,0);
		else
		{
			self->impulse = 0;
			return;
		}
	}
////////////////////////////////
// Teleporter
	else if (self->owned_by == 6 && (self->has_teleporter == 1 || self->has_teleporter == 2))
	{
		Find_And_Dmg("building_teleporter", self, 1,0);
		ResetMenu();
		self->impulse = 0;
	}
////////////////////////////////
// Field Generator
	else if (self->owned_by == 9 && (self->has_fieldgen >= 1))
	{
		Find_And_Dmg("building_fieldgen", self, 1,0);
		self->has_fieldgen = 0; // FIXME: shouldnt be needed
		ResetMenu();
		self->impulse = 0;
	}
////////////////////////////////
// Sensor
	else if (self->owned_by == 7 && self->has_sensor == 1)
	{
		Find_And_Dmg("building_sensor", self, 1,0);
		sprint(self, PR_PRINT_HIGH, "You detonate your motion sensor.\n");
		self->has_sensor = 0;
		ResetMenu();
		self->impulse = 0;
	}
}

void Menu_Engineer()
{
	//Set up empty menu
	string line1;
	string line2;
	string line3;
	string line4;
	string line5;
	string line6;
	string line7;

	//Check line for Dispenser
	if (self->has_dispenser == PR_TRUE)
		line1 = S_("^bAction^b:                           \n\n^1^b..^b Destroy Dispenser             \n");
	else if ((Invade_buffPlayer(self) || self->ammo_cells >= PR_BUILD_COST_DISPENSER) && (self->cutf_items & PR_CUTF_DISPENSER))//SB
		line1 = S_("^bAction^b:                           \n\n^1.. Build Dispenser               \n");
	else
		line1 = S_("^bAction^b:                           \n\n                                  \n");

	if (self->cutf_items & PR_CUTF_SENTRYGUN && self->cutf_items & PR_CUTF_DOUBLESENTRY) // 2 sentries
	{
		if (self->has_sentry & 1) // 1st sentry up
			line2 = S_("^2^b..^b Destroy Sentry Gun #1         \n");
		else if (Invade_buffPlayer(self) || (self->ammo_cells >= PR_BUILD_COST_SENTRYGUN))
			line2 = S_("^2.. Build Sentry Gun #1           \n");
		else line2 = "                                  \n";

		if (self->has_sentry & 2) // 2nd sentry up
			line3 = S_("^3^b..^b Destroy Sentry Gun #2         \n");
		else if (Invade_buffPlayer(self) || (self->ammo_cells >= PR_BUILD_COST_SENTRYGUN))
			line3 = S_("^3.. Build Sentry Gun #2           \n");
		else line3 = "                                  \n";
	}
	else if (self->tf_items & PR_NIT_TESLA && self->tf_items & PR_NIT_DOUBLETESLA) // 2 teslas
	{
		if (self->has_tesla & 1) // 1st sentry up
			line2 = S_("^2^b..^b Destroy Tesla Coil #1         \n");
		else if (Invade_buffPlayer(self) || (self->ammo_cells >= PR_BUILD_COST_TESLA))
			line2 = S_("^2.. Build Tesla Coil #1           \n");
		else line2 = "                                  \n";

		if (self->has_tesla & 2) // 2nd sentry up
			line3 = S_("^3^b..^b Destroy Tesla Coil #2         \n");
		else if (Invade_buffPlayer(self) || (self->ammo_cells >= PR_BUILD_COST_TESLA))
			line3 = S_("^3.. Build Tesla Coil #2           \n");
		else line3 = "                                  \n";
	}
	else
	{
		if (self->has_sentry)
			line2 = S_("^2^b..^b Destroy Sentry Gun            \n");
		else if ((Invade_buffPlayer(self) || self->ammo_cells >= PR_BUILD_COST_SENTRYGUN) && self->cutf_items & PR_CUTF_SENTRYGUN)
			line2 = S_("^2.. Build Sentry Gun              \n");
		else
			line2 = "                                  \n";

		if (self->has_tesla)
			line3 = S_("^3^b..^b Destroy Tesla Coil            \n");
		else if ((Invade_buffPlayer(self) || self->ammo_cells >= PR_BUILD_COST_TESLA) && self->tf_items & PR_NIT_TESLA)
			line3 = S_("^3.. Build Tesla Coil              \n");
		else
			line3 = "                                  \n";
	}

	if (self->has_camera == PR_TRUE)
		line4 = S_("^4^b..^b Destroy Security Camera       \n"); //CH impulse 4 for both
	else if ((Invade_buffPlayer(self) || self->ammo_cells >= PR_BUILD_COST_CAMERA) && self->tf_items & PR_NIT_SECURITY_CAMERA)
		line4 = S_("^4.. Launch Security Camera        \n");
	else
		line4 = "                                  \n";

	if (self->has_teleporter != 0) //CH messy, yes
	{
		if (self->has_teleporter >= 2)
			line5 = S_("^6^b..^b Destroy Both Teleporter Pads  \n"); // 6
		else if (Invade_buffPlayer(self) || (self->ammo_cells >= PR_BUILD_COST_TELEPORTER))
			line5 = S_("^5.. Build a Teleporter Pad        \n^6^b..^b Destroy a Teleporter Pad      \n"); //ofn the number is 5
		else
			line5 = S_("^6^b..^b Destroy a Teleporter Pad      \n"); // 6
	}
	else if ((Invade_buffPlayer(self) || self->ammo_cells >= PR_BUILD_COST_TELEPORTER) && self->tf_items & PR_NIT_TELEPORTER)
		line5 = S_("^5.. Build a Teleporter Pad        \n");
	else
		line5 = "                                  \n";

	if (self->has_sensor==PR_TRUE)
	{
		line6 = S_("^7^b..^b Destroy Motion Sensor         \n");
	}
	else if ((Invade_buffPlayer(self) || self->ammo_cells >= PR_BUILD_COST_SENSOR) && self->cutf_items & PR_CUTF_SENSOR)
	{
		line6 = S_("^7.. Deploy Motion Sensor          \n");
	}
	else
	{
		line6 = "                                  \n";
	}

	if (self->has_fieldgen != 0) //CH messy, yes
	{
#ifdef PR_FIELD_FORCEMODE

		if (self->has_fieldgen >= 2)
		{
			if (GetFieldForcedStatus())
				line7 = S_("^8.. Force field: ^bClosed Mode^b      \n^9^b..^b Destroy Both Field Generators \n\n^0.. ^bNothing^b                       \n");
			else
				line7 = S_("^8.. Force field: ^bIntelligent Mode^b \n^9^b..^b Destroy Both Field Generators \n\n^0.. ^bNothing^b                       \n");
		}

#else

		if (self->has_fieldgen >= 2)
			line7 = S_("^9^b..^b Destroy Both Field Generators \n\n^0.. ^bNothing^b                       \n"); // 6

#endif
		else if (Invade_buffPlayer(self) || (self->ammo_cells >= PR_BUILD_COST_FIELDGEN))
			line7 = S_("^8.. Build a Field Generator       \n^9^b..^b Destroy a Field Generator     \n\n^0.. ^bNothing^b                       \n"); //ofn the number is 5
		else
			line7 = S_("^9^b..^b Destroy a Field Generator     \n\n^0.. ^bNothing^b                       \n"); // 6

	}
	else if ((Invade_buffPlayer(self) || self->ammo_cells >= PR_BUILD_COST_FIELDGEN) && self->cutf_items & PR_CUTF_FIELDGEN)
		line7 = S_("^8.. Build a Field Generator       \n\n^0.. ^bNothing^b                       \n");
	else
		line7 = S_("                                  \n^0.. ^bNothing^b                       \n");


	//Finally, we output the menu...
	CenterPrint7(self,line1,line2,line3,line4,line5,line6,line7);

}

void Menu_Engineer_Input(float inp)
{
	entity te/*, te2*/;

	// OfN - Handle double stuff first
	if (inp >= 2 && inp <=3)
	if (self->cutf_items & PR_CUTF_DOUBLESENTRY || self->tf_items & PR_NIT_DOUBLETESLA)
	{
		if (self->cutf_items & PR_CUTF_DOUBLESENTRY)
		{
			if (inp == 2)
			{
				if (self->has_sentry & 1)
				{
					//PZ - confirm check
					self->impulse = 0;
					self->group_no = 2;
					self->owned_by = 2;
					Menu_EngineerConfirmDismantle();
					self->current_menu = PR_MENU_ENGINEER_CONFIRM_DISMANTLE;
					CuTFMenuSound(PR_MENUSOUND_BROWSE);
				}
				else
				{
					TeamFortress_Build(PR_BUILD_SENTRYGUN,1);
					ResetMenu();
					self->impulse = 0;
				}
			}
			else if (inp == 3)
			{
				if (self->has_sentry & 2)
				{
					//PZ - confirm check
					self->impulse = 0;
					self->group_no = 2;
					self->owned_by = 3;
					Menu_EngineerConfirmDismantle();
					self->current_menu = PR_MENU_ENGINEER_CONFIRM_DISMANTLE;
					CuTFMenuSound(PR_MENUSOUND_BROWSE);
				}
				else
				{
					TeamFortress_Build(PR_BUILD_SENTRYGUN,2);
					ResetMenu();
					self->impulse = 0;
				}
			}
		}
		else if (self->tf_items & PR_NIT_DOUBLETESLA)
		{
			if (inp == 2)
			{
				if (self->has_tesla & 1)
				{
					//PZ - confirm check
					self->impulse = 0;
					self->group_no = 2;
					self->owned_by = 2;
					Menu_EngineerConfirmDismantle();
					self->current_menu = PR_MENU_ENGINEER_CONFIRM_DISMANTLE;
					CuTFMenuSound(PR_MENUSOUND_BROWSE);
				}
				else
				{
					TeamFortress_Build(PR_BUILD_TESLA,1);
					ResetMenu();
					self->impulse = 0;
				}
			}
			else if (inp == 3)
			{
				if (self->has_tesla & 2)
				{
					//PZ - confirm check
					self->impulse = 0;
					self->group_no = 2;
					self->owned_by = 3;
					Menu_EngineerConfirmDismantle();
					self->current_menu = PR_MENU_ENGINEER_CONFIRM_DISMANTLE;
					CuTFMenuSound(PR_MENUSOUND_BROWSE);
				}
				else
				{
					TeamFortress_Build(PR_BUILD_TESLA,2);
					ResetMenu();
					self->impulse = 0;
				}
			}
		}

		//ResetMenu();
		//self.impulse = 0;
		return;
	}

	if (inp == 1 && (Invade_buffPlayer(self) || self->ammo_cells >= PR_BUILD_COST_DISPENSER) && self->has_dispenser == PR_FALSE)
	{
		//if (!self.has_dispenser)
		TeamFortress_Build(PR_BUILD_DISPENSER,0);
		ResetMenu();
		self->impulse = 0;
	}
	else if (inp == 2 && (Invade_buffPlayer(self) || self->ammo_cells >= PR_BUILD_COST_SENTRYGUN) && self->has_sentry == PR_FALSE)
	{
		if (HasFlag(self->team_no) == PR_FALSE) {
			CenterPrint(self, "No building until your team has the flag!\n");
			return;
		}

		TeamFortress_Build(PR_BUILD_SENTRYGUN,1);
		ResetMenu();
		self->impulse = 0;
	}
	else if (inp == 3 && (Invade_buffPlayer(self) || self->ammo_cells >= PR_BUILD_COST_TESLA) && self->has_tesla == PR_FALSE)
	{
		if (HasFlag(self->team_no) == PR_FALSE) {
			CenterPrint(self, "No building until your team has the flag!\n");
			return;
		}

		TeamFortress_Build(PR_BUILD_TESLA,1);
		ResetMenu();
		self->impulse = 0;
	}
	else if (inp == 4)
	{
		if (!self->has_camera)
			TeamFortress_Build(PR_BUILD_SECURITY_CAMERA,0);
		else
		{
			//PZ - confirm check
			self->impulse = 0;
			self->group_no = 2;
			self->owned_by = 4;
			Menu_EngineerConfirmDismantle();
			self->current_menu = PR_MENU_ENGINEER_CONFIRM_DISMANTLE;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
		//ResetMenu();
		//self.impulse = 0;
	}
	//CH uses 5 and 6
	else if (inp == 5 && (self->has_teleporter == 0 || self->has_teleporter == 1) &&
	         (Invade_buffPlayer(self) || self->ammo_cells >= PR_BUILD_COST_TELEPORTER)) //ch if have one out, can build
	{
		if (HasFlag(self->team_no) == PR_FALSE) {
			CenterPrint(self, "No building until your team has the flag!\n");
			return;
		}

		if (self->has_teleporter == 1)
		{
			float r;
			te = find(world, "classname", "building_teleporter");
			while (te != world)
			{
				if (te->real_owner == self)
				{
					r = vlen(te->origin - self->origin);
					if (r > PR_TELEPORTER_RANGE && !(te->all_active & PR_IMPROVED_FOUR))
					{
						sprint(self, PR_PRINT_HIGH, "Other Teleporter is too far away\n");
					}
					else
					{
						TeamFortress_Build(PR_BUILD_TELEPORTER,0);
						ResetMenu();
						self->impulse = 0;
					}
				}
				te = find(te, "classname", "building_teleporter");
			}
		}
		else
		{
			TeamFortress_Build(PR_BUILD_TELEPORTER,0);
			ResetMenu();
			self->impulse = 0;
		}

	}
	else if (inp == 6 && (self->has_teleporter == 1 || self->has_teleporter == 2)) //ch if have one out. can destroy
	{
		//PZ - confirm check
		self->impulse = 0;
		self->group_no = 2;
		self->owned_by = 6;
		Menu_EngineerConfirmDismantle();
		self->current_menu = PR_MENU_ENGINEER_CONFIRM_DISMANTLE;
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
	}
	else if (inp == 7 && ((Invade_buffPlayer(self) || self->ammo_cells >= PR_BUILD_COST_SENSOR) || self->has_sensor) &&
	         self->cutf_items & PR_CUTF_SENSOR)
	{
		SBBuildSensor();
		//ResetMenu();
		//self.impulse = 0;
	}
	//CH uses 5 and 6
	else if (inp == 8) //ch if have one out, can build
	{
		if ((self->has_fieldgen == 0 || self->has_fieldgen == 1) && (Invade_buffPlayer(self) || self->ammo_cells >= PR_BUILD_COST_FIELDGEN))
		{
			if (HasFlag(self->team_no) == PR_FALSE) {
				CenterPrint(self, "No building until your team has the flag!\n");
				return;
			}

			#ifndef PR_NO_FIELDGEN_INTERFERENCE

			// find if there are other generators too close, if this is our first generator only - nope

			float r, mindist;
			mindist = 3000;

			te = find(world, "classname", "building_fieldgen");
			while (te != world)
			{
				if (!te->real_owner == self)
				{
					r = vlen(te->origin - self->origin);

					if (r < mindist)
						mindist = r;
				}
				te = find(te, "classname", "building_fieldgen");
			}

			if (mindist < PR_FIELDGEN_HACKEDRANGE) // * 2
			{
				sprint(self, PR_PRINT_HIGH, "There are interferences with other field generators here!\n");
			}
			else
			{
				TeamFortress_Build(PR_BUILD_FIELDGEN,0);
				ResetMenu();
				self->impulse = 0;
			}

			#else

			TeamFortress_Build(PR_BUILD_FIELDGEN,0);
			ResetMenu();
			self->impulse = 0;

			#endif
		}

#ifdef PR_FIELD_FORCEMODE

		else if (self->has_fieldgen > 1)
		{
			if (GetFieldForcedStatus())
			{
				SetFieldForcedStatus(PR_FALSE);
				sprint(self,PR_PRINT_HIGH,"You set the force field to Intelligent Mode\n");
			}
			else
			{
				SetFieldForcedStatus(PR_TRUE);
				sprint(self,PR_PRINT_HIGH,"You set the force field to Closed Mode\n");
			}

			ResetMenu();
			self->impulse = 0;
		}

#endif

	}
	else if (inp == 9 && (self->has_fieldgen >= 1)) //ch if have one out. can destroy
	{
		//PZ - confirm check
		self->impulse = 0;
		self->group_no = 2;
		self->owned_by = 9;
		Menu_EngineerConfirmDismantle();
		self->current_menu = PR_MENU_ENGINEER_CONFIRM_DISMANTLE;
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
	}
	else if (inp == 1 && self->has_dispenser == PR_TRUE)
	{
		//PZ - confirm check
		self->impulse = 0;
		self->group_no = 2;
		self->owned_by = 1;
		Menu_EngineerConfirmDismantle();
		self->current_menu = PR_MENU_ENGINEER_CONFIRM_DISMANTLE;
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
	}
	else if (inp == 2 && self->has_sentry == PR_TRUE)
	{
		//PZ - confirm check
		self->impulse = 0;
		self->group_no = 2;
		self->owned_by = 2;
		Menu_EngineerConfirmDismantle();
		self->current_menu = PR_MENU_ENGINEER_CONFIRM_DISMANTLE;
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
	}
	else if (inp == 3 && self->has_tesla == PR_TRUE)
	{
		//PZ - confirm check
		self->impulse = 0;
		self->group_no = 2;
		self->owned_by = 3;
		Menu_EngineerConfirmDismantle();
		self->current_menu = PR_MENU_ENGINEER_CONFIRM_DISMANTLE;
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
	}
	else if (inp == 10) //CH was 4
	{
		ResetMenu();
		self->impulse = 0;
	}

	// Otherwise, the impulse is passed on
}
//CH to reduce code.
/* PZ - no more friendly dismantles - don't need function
void(entity person, string build) Add_Building_Teamkill =
{
	local string st;
	local float threshold;
	threshold = 0;
	bprint (#PRINT_HIGH, person.netname);
	bprint (#PRINT_HIGH, " has dismantled a friendly ");
	bprint (#PRINT_HIGH, build);
	bprint (#PRINT_HIGH, "!\n");

	st = infokey(world, "curse");
	if (st != string_null)
	threshold = stof(st);
	person.ff_count = person.ff_count + 2; //Increase their bastard rating
	//Increase the engineers bastard rating so they
	//can't block everyone.
	person.building.real_owner.ff_count = person.building.real_owner.ff_count + 0.5;
	if (threshold >= 1)
	{
		if (person.ff_count >= threshold) createBastard(person,threshold);
		if (person.building.real_owner.ff_count >= threshold) createBastard(person.building.real_owner,threshold);
		if ((person.ff_count == threshold - 2) || (person.ff_count == threshold - 1.5) || (person.ff_count == threshold - 1) || (person.ff_count == threshold - 0.5))
		{
			sprint (person, #PRINT_MEDIUM, "One more dismantling and you will be cursed.\n");
		}
	}
}; */

//============================================================================
// ENGINEER menus for fixing buildings

void Menu_EngineerFix_Dispenser()
{
	CenterPrint(self, S_("^bAction^b:                            \n\n^1.. Put Ammo into Dispenser     \n^2.. Put Armor into Dispenser    \n^3.. Repair Dispenser            \n^4.. Dismantle Dispenser         \n\n^8.. ^bNothing^b                     \n\n"));
}

void Menu_EngineerFix_Dispenser_Input(float inp)
{
	float metalcost;
	float am, waitToDismantle; waitToDismantle = PR_FALSE; // PZ: added `waitToDismantle`

	if (self->classname != "player" || self->building == world)
		return;

	float iI; // is Improved?
	iI=1;
	if (self->building->all_active & PR_IMPROVED_ONE)
		iI=2;

	if (inp == 1)
	{
		// shells
		am = (PR_DROP_SHELLS * 2);
		if (am > self->ammo_shells)
			am = self->ammo_shells;
		if (am > (PR_BUILD_DISPENSER_MAX_SHELLS*iI - self->building->ammo_shells))
			am = PR_BUILD_DISPENSER_MAX_SHELLS*iI - self->building->ammo_shells;
		self->ammo_shells = self->ammo_shells - am;
		self->building->ammo_shells = self->building->ammo_shells + am;

		// nails
		am = (PR_DROP_NAILS * 2);
		if (am > self->ammo_nails)
			am = self->ammo_nails;
		if (am > (PR_BUILD_DISPENSER_MAX_NAILS*iI - self->building->ammo_nails))
			am = PR_BUILD_DISPENSER_MAX_NAILS*iI - self->building->ammo_nails;
		self->ammo_nails = self->ammo_nails - am;
		self->building->ammo_nails = self->building->ammo_nails + am;

		// rockets
		am = (PR_DROP_ROCKETS * 2);
		if (am > self->ammo_rockets)
			am = self->ammo_rockets;
		if (am > (PR_BUILD_DISPENSER_MAX_ROCKETS*iI - self->building->ammo_rockets))
			am = PR_BUILD_DISPENSER_MAX_ROCKETS*iI - self->building->ammo_rockets;
		self->ammo_rockets = self->ammo_rockets - am;
		self->building->ammo_rockets = self->building->ammo_rockets + am;

		// cells
		am = (PR_DROP_CELLS * 2);
		if (am > self->ammo_cells)
			am = self->ammo_cells;
		if (am > (PR_BUILD_DISPENSER_MAX_CELLS*iI - self->building->ammo_cells))
			am = PR_BUILD_DISPENSER_MAX_CELLS*iI - self->building->ammo_cells;
		self->ammo_cells = self->ammo_cells - am;
		self->building->ammo_cells = self->building->ammo_cells + am;
	}
	else if (inp == 2)
	{
		// armor
		am = (PR_DROP_ARMOR * 2);
		if (am > self->armorvalue)
			am = self->armorvalue;
		if (am > (PR_BUILD_DISPENSER_MAX_ARMOR*iI - self->building->armorvalue))
			am = PR_BUILD_DISPENSER_MAX_ARMOR*iI - self->building->armorvalue;
		self->armorvalue = self->armorvalue - am;
		self->building->armorvalue = self->building->armorvalue + am;
	}
	else if (inp == 3)
	{
		//local string f1;

		metalcost = (self->building->max_health - self->building->health) / 5;

		if (metalcost > self->ammo_cells)
			metalcost = self->ammo_cells;

		self->ammo_cells = self->ammo_cells - metalcost;
		self->building->health = self->building->health + (metalcost * 5);
	}
	else if (inp == 4)
	{
		if (CheckEnemyDismantle()==PR_FALSE)
		{
			ResetMenu();
			self->impulse=0;
			return;
		}

#ifdef PR_QUAKE_WORLD
		if (Teammate(self->building->real_owner, self) && teamplay != 0 && self->building->real_owner != self) { //Boot em if they dismantle too many
			//Add_Building_Teamkill (self, "dispenser");
			// PZ - no more friendly dismantles (use vote punish for tkers)
			sprint(self, PR_PRINT_HIGH, "You can not dismantle friendly devices. Use 'punish' to kick teamkillers.\n");
		}
#endif
		else if (!Teammate(self->building->real_owner, self))
		{
			sprint (self, PR_PRINT_HIGH, "You dismantle the Dispenser.\n");
			self->ammo_cells = self->ammo_cells + (PR_BUILD_COST_DISPENSER / 2);
			self->building->health=0; // - OfN aborts hacks on it
			self->building->real_owner->has_dispenser = PR_FALSE;
			dremove(self->building);
		}
		else if (self->building->real_owner == self)
		{
			// PZ - confirm check
			waitToDismantle = PR_TRUE;
			self->impulse = 0;
			self->group_no = 1;
			Menu_EngineerConfirmDismantle();
			self->current_menu = PR_MENU_ENGINEER_CONFIRM_DISMANTLE;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
	}

	//if ((inp >= 1 && inp <= 3) || inp == 5)
	if (inp >= 1 && inp <= 5 && !waitToDismantle) // PZ: brought back old code; added `waitToDismantle`
	{
		ResetMenu();
		self->impulse = 0;
		self->building = world;

		bound_other_ammo(self);

		if (self->armorvalue == 0)
		{
			self->armortype = 0; // lost all armor
			self->armorclass = 0; // lost special armor
			self->items = self->items - (self->items & (PR_IT_ARMOR1 | PR_IT_ARMOR2 | PR_IT_ARMOR3));
		}

		W_SetCurrentAmmo();
	}

	// Otherwise, the impulse is passed on
}

void Menu_EngineerFix_SentryGun()
{
	if (self->building->weapon < 3 && self->ammo_cells >= PR_BUILD_COST_SENTRYGUN)
	{
		if (!(self->tf_items & PR_NIT_TURRET)) // TODO: Action --> OfteN's sentry gun
			CenterPrint(self, S_("^bAction^b:                            \n\n^1.. Put Ammo into Sentry Gun    \n^2.. Upgrade Sentry Gun          \n^3.. Repair Sentry Gun           \n\n^4.. ^bNothing^b                     \n\n^5.. Dismantle Sentry Gun        \n^6.. Rotate Clockwise            \n^7.. Rotate Counter-Clockwise    \n"));
		else
			CenterPrint(self, S_("^bAction^b:                            \n\n^1.. Put Ammo into Sentry Gun    \n^2.. Upgrade Sentry Gun          \n^3.. Repair Sentry Gun           \n\n^4.. ^bNothing^b                     \n\n^5.. Dismantle Sentry Gun        \n^6.. Rotate Clockwise            \n^7.. Rotate Counter-Clockwise    \n^8.. Create Turret               \n"));
	}
	else
	{
		if (!(self->tf_items & PR_NIT_TURRET))
			CenterPrint(self, S_("^bAction^b:                            \n\n^1.. Put Ammo into Sentry Gun    \n^3.. Repair Sentry Gun           \n\n^4.. ^bNothing^b                     \n\n^5.. Dismantle Sentry Gun        \n^6.. Rotate Clockwise            \n^7.. Rotate Counter-Clockwise    \n"));
		else
			CenterPrint(self, S_("^bAction^b:                            \n\n^1.. Put Ammo into Sentry Gun    \n^3.. Repair Sentry Gun           \n\n^4.. ^bNothing^b                     \n\n^5.. Dismantle Sentry Gun        \n^6.. Rotate Clockwise            \n^7.. Rotate Counter-Clockwise    \n^8.. Create Turret               \n"));
	}
}

void Menu_EngineerFix_SentryGun_Input(float inp)
{
	float am, metalcost, waitToDismantle; waitToDismantle = PR_FALSE; // PZ: added `waitToDismantle`
	string st;

	if (self->classname != "player" || self->building == world)
		return;

	if (inp == 1)
	{
		// shells
		am = (PR_DROP_SHELLS * 2);
		if (am > self->ammo_shells)
			am = self->ammo_shells;
		if (am > (self->building->maxammo_shells - self->building->ammo_shells))
			am = (self->building->maxammo_shells - self->building->ammo_shells);
		self->ammo_shells = self->ammo_shells - am;
		self->building->ammo_shells = self->building->ammo_shells + am;

		// If it's level 3, put some rockets in too
		if (self->building->weapon == 3)
		{
			am = (PR_DROP_ROCKETS * 2);
			if (am > self->ammo_rockets)
				am = self->ammo_rockets;
			if (am > (self->building->maxammo_rockets - self->building->ammo_rockets))
				am = (self->building->maxammo_rockets - self->building->ammo_rockets);
			self->ammo_rockets = self->ammo_rockets - am;
			self->building->ammo_rockets = self->building->ammo_rockets + am;
		}
	}
	else if (self->building->weapon < 3 && inp == 2 && (Invade_buffPlayer(self) || self->ammo_cells >= PR_BUILD_COST_SENTRYGUN))
	{
		if (!Invade_buffPlayer(self))
			self->ammo_cells = self->ammo_cells - PR_BUILD_COST_SENTRYGUN;
		self->building->weapon = self->building->weapon + 1;

		float HPfactor,AMMOfactor;
		if (self->building->all_active & PR_IMPROVED_THREE)
			HPfactor=PR_SENTRY_ARMORHACKFACTOR;
		else
			HPfactor=1;

		if (self->building->all_active & PR_IMPROVED_ONE)
			AMMOfactor=PR_SENTRY_AMMOHACKFACTOR;
		else
			AMMOfactor=1;

		// more health
		//self.building.max_health = self.building.max_health * 1.2;
		//self.building.health = self.building.max_health;

		// more ammo capability
		//self.building.maxammo_shells = self.building.maxammo_shells * 1.2;
		// Change the skin and frames
		if (self->building->weapon == 2)
		{
			sound (self->building, PR_CHAN_ITEM, "weapons/turrset.wav", 1, PR_ATTN_NORM);
			self->building->think = lvl2_sentry_stand;
			//self.building.skin = 1;
			self->building->max_health = floor(PR_SENTRY_HEALTH_L2 * HPfactor);
			self->building->maxammo_shells = PR_SENTRY_AMMO_L2 * AMMOfactor;
		}
		else // if (self.building.weapon == 3)
		{
			sound (self->building, PR_CHAN_ITEM, "weapons/turrset.wav", 1, PR_ATTN_NORM);
			self->building->think = lvl3_sentry_stand;
			//self.building.skin = 2;
			self->building->max_health = floor(PR_SENTRY_HEALTH_L3 * HPfactor);
			self->building->maxammo_shells = PR_SENTRY_AMMO_L3 * AMMOfactor;
		}

		self->building->health = self->building->max_health;

		sprint(self, PR_PRINT_HIGH, "You upgrade the Sentry Gun to level ");
		st = ftos(self->building->weapon);
		sprint(self, PR_PRINT_HIGH, st);
		sprint(self, PR_PRINT_HIGH, "\n");
	}
	else if (inp == 3)
	{
		metalcost = (self->building->max_health - self->building->health) / 5;

		if (metalcost > self->ammo_cells)
			metalcost = self->ammo_cells;

		self->ammo_cells = self->ammo_cells - metalcost;
		self->building->health = self->building->health + (metalcost * 5);
	}
	else if (inp == 5)
	{
		if (CheckEnemyDismantle()==PR_FALSE) return;

		//RemoveMyTimers(self.building); // psionic clean-up
#ifdef PR_QUAKE_WORLD
		if (Teammate(self->building->real_owner, self) && teamplay != 0 && self->building->real_owner != self) { //Boot em if they dismantle too many
			//Add_Building_Teamkill (self, "sentry gun");
			// PZ - no more friendly dismantles (use vote punish for tkers)
			sprint(self, PR_PRINT_HIGH, "You can not dismantle friendly devices. Use 'punish' to kick teamkillers.\n");
		}
#endif
		else if (!Teammate(self->building->real_owner, self))
		{
			sprint(self, PR_PRINT_HIGH, "You dismantle the Sentry Gun.\n");
			//CH give .5 of build cost====give 25*level
			self->ammo_cells = self->ammo_cells + (PR_BUILD_COST_SENTRYGUN / 2) + (self->building->weapon * 25);
			//self.building.real_owner.has_sentry = #FALSE;
			self->building->real_owner->has_sentry = self->building->real_owner->has_sentry - (self->building->real_owner->has_sentry & self->building->dont_do_triggerwork);
			dremove(self->building->trigger_field);
			self->building->health=0; // - OfN aborts hacks on it
			dremove(self->building);
		}
		else if (self->building->real_owner == self)
		{
			// PZ - confirm check
			waitToDismantle = PR_TRUE;
			self->impulse = 0;
			self->group_no = 1;
			Menu_EngineerConfirmDismantle();
			self->current_menu = PR_MENU_ENGINEER_CONFIRM_DISMANTLE;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
	}
	else if (inp == 6) //WK
	{
		sprint(self, PR_PRINT_HIGH, "You rotate it clockwise 45 degrees\n");

		#ifdef PR_SENTRY_SMOOTHROTATION
		self->building->angles[Y] = anglemod(self->building->angles[Y] - 45);
		#else
		self->building->angles[Y] = anglemod((self->building->waitmax - PR_SENTRY_ROTATIONWIDTH) - 45);
		#endif

		self->building->angles[Y] = ((ceil(self->building->angles[Y] / 10)) * 10); //CH set last int to 0

		#ifdef PR_SENTRY_SMOOTHROTATION
		self->building->waitmin = self->building->angles[Y] - PR_SENTRY_ROTATIONWIDTH;
		self->building->waitmax = self->building->angles[Y] + PR_SENTRY_ROTATIONWIDTH;
		self->building->heat = rint(PR_SENTRY_ROTATIONSTEPS/2);
		#else
		self->building->waitmin = anglemod(self->building->angles[Y] - PR_SENTRY_ROTATIONWIDTH);
		self->building->waitmax = anglemod(self->building->angles[Y] + PR_SENTRY_ROTATIONWIDTH);
		self->building->heat = 0;
		#endif
	}
	else if (inp == 7) //WK
	{
		sprint(self, PR_PRINT_HIGH, "You rotate it counter-clockwise 45 degrees\n");

		#ifdef PR_SENTRY_SMOOTHROTATION
		self->building->angles[Y] = anglemod((self->building->waitmin + PR_SENTRY_ROTATIONWIDTH) + 45);
		#else
		self->building->angles[Y] = anglemod(self->building->angles[Y] + 45);
		#endif

			self->building->angles[Y] = ((ceil(self->building->angles[Y] / 10)) * 10); //CH set last int to 0

		#ifdef PR_SENTRY_SMOOTHROTATION
		self->building->waitmin = self->building->angles[Y] - PR_SENTRY_ROTATIONWIDTH;
		self->building->waitmax = self->building->angles[Y] + PR_SENTRY_ROTATIONWIDTH;
		self->building->heat = 0;
		#else
		self->building->waitmin = anglemod(self->building->angles[Y] - PR_SENTRY_ROTATIONWIDTH);
		self->building->waitmax = anglemod(self->building->angles[Y] + PR_SENTRY_ROTATIONWIDTH);
		self->building->heat = 1;
		#endif
	}
	//WK Turret Sentries!
	else if (inp == 8)
	{
		if (self->building->tf_items & PR_NIT_TURRET) {
			sprint(self, PR_PRINT_HIGH, "Gun is already deployed\n");
		}
		else if (!(self->tf_items & PR_NIT_TURRET)) {
			sprint(self, PR_PRINT_HIGH, "You need to purchase the turret upgrade\n");
		}
		else if (!Invade_buffPlayer(self) && self->ammo_cells < PR_BUILD_COST_TURRET)
		{
			sprint(self, PR_PRINT_HIGH, "Creating a turret costs " TO_STR(PR_BUILD_COST_TURRET) " metal\n");
		}
		else {
			self->building->origin[Z] = self->building->origin[Z] + 15; //Elevate for the check
			//Make sure the launch area is clear
			if (CheckArea(self->building, self) == PR_FALSE) {
				sprint(self, PR_PRINT_HIGH, "You need a clear area to launch\n");
				self->building->origin[Z] = self->building->origin[Z] - 15; //Set back down
			}
			else {
				self->building->origin[Z] = self->building->origin[Z] + 25; //Finish liftoff
				sprint(self, PR_PRINT_HIGH, "You launch the sentrygun\n");
				if (!Invade_buffPlayer(self))
					self->ammo_cells = self->ammo_cells - PR_BUILD_COST_TURRET;
				self->building->angles[Z] = 180;
				setmodel(self->building->trigger_field, string_null); // PZ NOTE: make its stand invisible
				self->building->trigger_field->solid = PR_SOLID_NOT;
				self->building->flags = self->building->flags - (self->building->flags & PR_FL_ONGROUND);
				self->building->movetype = PR_MOVETYPE_FLY;
				self->building->velocity[Z] = 200;
				setsize(self->building, V({-16, -16, -40}), V({16, 16, -10})); //WK -40, -10
				self->building->tf_items = self->building->tf_items | PR_NIT_TURRET;
			}
		}
	}
	//if ((inp >= 1 && inp <= 4) || (inp >= 6 && inp <= 8))
	if (inp >= 1 && inp <= 8 && !waitToDismantle) // PZ: brought back old code; added `waitToDismantle`
	{
		ResetMenu();
		self->impulse = 0;

		bound_other_ammo(self);

		if (self->armorvalue == 0)
		{
			self->armortype = 0; // lost all armor
			self->armorclass = 0; // lost special armor
			self->items = self->items - (self->items & (PR_IT_ARMOR1 | PR_IT_ARMOR2 | PR_IT_ARMOR3));
		}

		W_SetCurrentAmmo();
	}
}

void Menu_EngineerFix_Sensor()
{
	CenterPrint(self, S_("^bAction^b:              \n\n^1.. Repair Sensor      \n\n^2.. ^bNothing^b            \n\n^3.. Dismantle          \n"));
}
void Menu_EngineerFix_Sensor_Input(float inp)
{
//*ch
	float metalcost; //am,
	//local string st;
	float waitToDismantle; waitToDismantle = PR_FALSE; // PZ: added `waitToDismantle`

	if (self->classname != "player" || self->building == world)
		return;

	if (inp == 1)
	{
		metalcost = (self->building->max_health - self->building->health) / 10;

		if (metalcost > self->ammo_cells)
			metalcost = self->ammo_cells;

		self->ammo_cells = self->ammo_cells - metalcost;
		self->building->health = self->building->health + (metalcost * 10);
	}
	else if (inp == 3)
	{
		if (CheckEnemyDismantle() == PR_FALSE) return; //- OfN - ICE avoids enemy dismantle (see often.qc)

#ifdef PR_QUAKE_WORLD
		if (Teammate(self->building->real_owner, self) && teamplay != 0 && self->building->real_owner != self) { //Boot em if they dismantle too many
			//Add_Building_Teamkill (self, "motion sensor");
			// PZ - no more friendly dismantles (use vote punish for tkers)
			sprint(self, PR_PRINT_HIGH, "You can not dismantle friendly devices. Use 'punish' to kick teamkillers.\n");
		}
#endif
		else if (!Teammate(self->building->real_owner, self))
		{
			sprint(self, PR_PRINT_HIGH, "You dismantle the Motion Sensor.\n");
			//CH will give you half of cost + max of 20
			self->ammo_cells = self->ammo_cells + (PR_BUILD_COST_SENSOR * 0.5) + floor(self->building->health / 40);
			self->building->real_owner->has_sensor = PR_FALSE;
			self->building->health=0; // - OfN aborts hacks on it
			dremove(self->building);
		}
		else if (self->building->real_owner == self)
		{
			//PZ - confirm check
			waitToDismantle = PR_TRUE;
			self->impulse = 0;
			self->group_no = 1;
			Menu_EngineerConfirmDismantle();
			self->current_menu = PR_MENU_ENGINEER_CONFIRM_DISMANTLE;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
	}
	//if (inp >= 1 && inp <= 2)
	if (inp >= 1 && inp <= 3 && !waitToDismantle) // PZ: brought back old code; added `waitToDismantle`
	{
		ResetMenu();
		self->impulse = 0;

		bound_other_ammo(self);

		if (self->armorvalue == 0)
		{
			self->armortype = 0; // lost all armor
			self->armorclass = 0; // lost special armor
			self->items = self->items - (self->items & (PR_IT_ARMOR1 | PR_IT_ARMOR2 | PR_IT_ARMOR3));
		}

		W_SetCurrentAmmo();
	}
}

void Menu_EngineerFix_Camera()
{
	CenterPrint(self, S_("^bAction^b:                \n\n^1.. Repair Camera      \n\n^2.. ^bNothing^b            \n\n^3.. Dismantle          \n"));
}
void Menu_EngineerFix_Camera_Input(float inp)
{
//*ch
	float metalcost; //am,
	float waitToDismantle; waitToDismantle = PR_FALSE; // PZ: added `waitToDismantle`
	//local string st;

	if (self->classname != "player" || self->building == world)
		return;

	if (inp == 1)
	{
		metalcost = (self->building->max_health - self->building->health) / 10;

		if (metalcost > self->ammo_cells)
			metalcost = self->ammo_cells;

		self->ammo_cells = self->ammo_cells - metalcost;
		self->building->health = self->building->health + (metalcost * 10);
	}
	else if (inp == 3)
	{
		if (CheckEnemyDismantle() == PR_FALSE) return; //- OfN - ICE avoids enemy dismantle (see often.qc)

#ifdef PR_QUAKE_WORLD
		if (Teammate(self->building->real_owner, self) && teamplay != 0 && self->building->real_owner != self) { //Boot em if they dismantle too many
			//Add_Building_Teamkill (self, "security camera");
			// PZ - no more friendly dismantles (use vote punish for tkers)
			sprint(self, PR_PRINT_HIGH, "You can not dismantle friendly devices. Use 'punish' to kick teamkillers.\n");
		}
#endif
		else if (!Teammate(self->building->real_owner, self))
		{

			sprint(self, PR_PRINT_HIGH, "You dismantle the Security Camera.\n");
			//CH will give you half of cost + max of 20
			self->ammo_cells = self->ammo_cells + (PR_BUILD_COST_CAMERA * 0.5) + floor(self->building->health / 40);
			self->building->real_owner->has_camera = PR_FALSE;
			self->building->health=0; // - OfN aborts hacks on it
			dremove(self->building);
		}
		else if (self->building->real_owner == self)
		{
			// PZ - confirm check
			waitToDismantle = PR_TRUE;
			self->impulse = 0;
			self->group_no = 1;
			Menu_EngineerConfirmDismantle();
			self->current_menu = PR_MENU_ENGINEER_CONFIRM_DISMANTLE;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
	}
	//if (inp >= 1 && inp <= 2)
	if (inp >= 1 && inp <= 3 && !waitToDismantle) // PZ: brought back old code; added `waitToDismantle`
	{
		ResetMenu();
		self->impulse = 0;

		bound_other_ammo(self);

		if (self->armorvalue == 0)
		{
			self->armortype = 0; // lost all armor
			self->armorclass = 0; // lost special armor
			self->items = self->items - (self->items & (PR_IT_ARMOR1 | PR_IT_ARMOR2 | PR_IT_ARMOR3));
		}

		W_SetCurrentAmmo();
	}
}

void Menu_EngineerFix_Teleporter()
{
	if (self->building->classname == "building_teleporter" && (self->building->tf_items & PR_NIT_TURRET))
	{
		CenterPrint(self, S_("^bAction^b:                \n\n^1.. Repair Teleporter     \n^2.. Recharge Teleporter   \n                          \n\n^8.. Dismantle             \n\n^0.. ^bNothing^b               \n"));
	}
	else
	{
		CenterPrint(self, S_("^bAction^b:                \n\n^1.. Repair Teleporter     \n^2.. Recharge Teleporter   \n^3.. Turretize Teleporter  \n\n^8.. Dismantle             \n\n^0.. ^bNothing^b               \n"));
	}
}
void Menu_EngineerFix_Teleporter_Input(float inp)
{
//*ch
	float metalcost; //am,
	float waitToDismantle; waitToDismantle = PR_FALSE; // PZ: added `waitToDismantle`
	//local string st;

	if (self->classname != "player" || self->building == world)
		return;

	if (inp == 1)
	{
		metalcost = (self->building->max_health - self->building->health) / 5;

		if (metalcost > self->ammo_cells)
			metalcost = ceil(self->ammo_cells);

		self->ammo_cells = self->ammo_cells - metalcost;
		self->building->health = self->building->health + (metalcost * 5);
	}
	else if (inp == 2)
	{
		metalcost = (self->building->maxammo_cells - self->building->ammo_cells);

		if (metalcost > self->ammo_cells)
			metalcost = ceil(self->ammo_cells);
		self->ammo_cells = self->ammo_cells - metalcost;
		self->building->ammo_cells = self->building->ammo_cells + metalcost;
		if (self->building->ammo_cells > self->building->maxammo_cells)
			self->building->ammo_cells = self->building->maxammo_cells;
	}
	else if (inp == 8)
	{
		if (CheckEnemyDismantle() == PR_FALSE) return; //- OfN - ICE avoids enemy dismantle (see often.qc)

#ifdef PR_QUAKE_WORLD
		if (Teammate(self->building->real_owner, self) && teamplay != 0 && self->building->real_owner != self) { //Boot em if they dismantle too many
			//Add_Building_Teamkill (self, "teleporter");
			// PZ - no more friendly dismantles (use vote punish for tkers)
			sprint(self, PR_PRINT_HIGH, "You can not dismantle friendly devices. Use 'punish' to kick teamkillers.\n");
		}
#endif
		else if (!Teammate(self->building->real_owner, self))
		{

			sprint(self, PR_PRINT_HIGH, "You dismantle the Teleporter Pad.\n");
			//CH will give you half of cost + max of 20
			self->ammo_cells = self->ammo_cells + (PR_BUILD_COST_TELEPORTER * 0.5) + floor(self->building->health / 15);
			self->building->real_owner->has_teleporter = (self->building->real_owner->has_teleporter - 1);
			self->building->health=0; // - OfN aborts hacks on it
			dremove(self->building);
		}
		else if (self->building->real_owner == self)
		{
			// PZ - confirm check
			waitToDismantle = PR_TRUE;
			self->impulse = 0;
			self->group_no = 1;
			Menu_EngineerConfirmDismantle();
			self->current_menu = PR_MENU_ENGINEER_CONFIRM_DISMANTLE;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
	}
	else if (inp == 3 && !(self->building->tf_items & PR_NIT_TURRET))
	{
		if (!Invade_buffPlayer(self) && self->ammo_cells < PR_TELEPORTER_TURRETIZE_CELLS_COST)
		{
			sprint(self,PR_PRINT_HIGH,"You don't have enough cells to turretize the teleporter\n");
			ResetMenu();
			self->impulse = 0;
			return;
		}

		sprint(self,PR_PRINT_HIGH,"You launch the teleporter\n");

		if (!Invade_buffPlayer(self))
			self->ammo_cells = self->ammo_cells - PR_TELEPORTER_TURRETIZE_CELLS_COST;

		self->building->origin[Z] = self->building->origin[Z] + 32;
		self->building->angles[Z] = 180;

		self->building->flags = self->building->flags - (self->building->flags & PR_FL_ONGROUND);
		self->building->movetype = PR_MOVETYPE_FLY;
		self->building->velocity[Z] = 200;
		setsize (self->building, V({-16, -16, -8}), V({16, 16, -4}));
		//setsize (self.building, '-16 -16 -48', '16 16 -52');

		self->building->tf_items = self->building->tf_items | PR_NIT_TURRET;
		//self.building.is_haxxxoring=1;

		ResetMenu();
		self->impulse = 0;

	}

	else if (inp == 10)
	{
		ResetMenu();
		self->impulse = 0;
		return;
	}

	//if (inp >= 1 && inp <= 4)
	if (((inp >= 1 && inp <= 4) || inp == 8) && !waitToDismantle) // PZ: brought back old code; added `waitToDismantle`)
	{
		ResetMenu();
		self->impulse = 0;

		bound_other_ammo(self);

		if (self->armorvalue == 0)
		{
			self->armortype = 0; // lost all armor
			self->armorclass = 0; // lost special armor
			self->items = self->items - (self->items & (PR_IT_ARMOR1 | PR_IT_ARMOR2 | PR_IT_ARMOR3));
		}

		W_SetCurrentAmmo();
	}
}

//=========================================================================
// field generator

void Menu_EngineerFix_FieldGen()
{
	CenterPrint(self, S_("^bAction^b:                \n\n^1.. Repair Field Generator  \n^2.. Recharge Field Generator\n\n^3.. ^bNothing^b                 \n\n^4.. Dismantle               \n"));
}
void Menu_EngineerFix_FieldGen_Input(float inp)
{
	float metalcost; //am,
	float waitToDismantle; waitToDismantle = PR_FALSE; // PZ: added `waitToDismantle`
	//local string st;

	if (self->classname != "player" || self->building == world)
		return;

	if (inp == 1)
	{
		metalcost = (self->building->max_health - self->building->health) / 5;

		if (metalcost > self->ammo_cells)
			metalcost = ceil(self->ammo_cells);

		self->ammo_cells = self->ammo_cells - metalcost;
		self->building->health = self->building->health + (metalcost * 5);
	}
	else if (inp == 2)
	{
		metalcost = (self->building->maxammo_cells - self->building->ammo_cells);

		if (metalcost > self->ammo_cells)
			metalcost = ceil(self->ammo_cells);
		self->ammo_cells = self->ammo_cells - metalcost;
		self->building->ammo_cells = self->building->ammo_cells + metalcost;
		if (self->building->ammo_cells > self->building->maxammo_cells)
			self->building->ammo_cells = self->building->maxammo_cells;
	}
	else if (inp == 4)
	{
		if (CheckEnemyDismantle() == PR_FALSE) return; //- OfN - ICE avoids enemy dismantle (see often.qc)

#ifdef PR_QUAKE_WORLD
		if (Teammate(self->building->real_owner, self) && teamplay != 0 && self->building->real_owner != self) { //Boot em if they dismantle too many
			//Add_Building_Teamkill (self, "field generator");
			// PZ - no more friendly dismantles (use vote punish for tkers)
			sprint(self, PR_PRINT_HIGH, "You can not dismantle friendly devices. Use 'punish' to kick teamkillers.\n");
		}
#endif
		else if (!Teammate(self->building->real_owner, self))
		{
			sprint(self, PR_PRINT_HIGH, "You dismantle the Field Generator.\n");
			//CH will give you half of cost + max of 20
			self->ammo_cells = self->ammo_cells + (PR_BUILD_COST_FIELDGEN * 0.5) + floor(self->building->health / 15);
			self->building->real_owner->has_fieldgen = (self->building->real_owner->has_fieldgen - 1);
			self->building->health=0; // - OfN aborts hacks on it
			dremove(self->building);
		}
		else if (self->building->real_owner == self)
		{
			//PZ - confirm check
			waitToDismantle = PR_TRUE;
			self->impulse = 0;
			self->group_no = 1;
			Menu_EngineerConfirmDismantle();
			self->current_menu = PR_MENU_ENGINEER_CONFIRM_DISMANTLE;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}
	}
	//if (inp >= 1 && inp <= 3)
	if (inp >= 1 && inp <= 4 && !waitToDismantle) // PZ: brought back old code; added `waitToDismantle`
	{
		ResetMenu();
		self->impulse = 0;

		bound_other_ammo(self);

		if (self->armorvalue == 0)
		{
			self->armortype = 0; // lost all armor
			self->armorclass = 0; // lost special armor
			self->items = self->items - (self->items & (PR_IT_ARMOR1 | PR_IT_ARMOR2 | PR_IT_ARMOR3));
		}

		W_SetCurrentAmmo();
	}
}


//CH
string Return_Colored_Num(float num)
{
	if (num == 0)
		return S_("^0");
	else if (num == 1)
		return S_("^1");
	else if (num == 2)
		return S_("^2");
	else if (num == 3)
		return S_("^3");
	else if (num == 4)
		return S_("^4");
	else if (num == 5)
		return S_("^5");
	else if (num == 6)
		return S_("^6");
	else if (num == 7)
		return S_("^7");
	else if (num == 8)
		return S_("^8");
	else if (num == 9)
		return S_("^9");
	else
		return "\xA3"; //Should never happen // OfN - It does when a tesla is in upgrade for frags
}
//Upgrades tesla, type 1=volt 2=amps 3=health 4=spy 5=kevlar 6=blast 7=turret 8=improve 9=cloak
//menu 0=normal 1=misc
float Check_Tesla_Ups(float ups, entity person, entity sent, float type, float menu)
{
	float upgrades;

	if (menu == 1)
		upgrades = sent->has_tesla; //misc
	else
		upgrades = sent->has_sentry; //normal
	if (ups > upgrades) {//not enough upgrades
		sprint(person,PR_PRINT_HIGH,"You do not have enough upgrades to improve the tesla\n");
		return PR_FALSE;
	}
	if (type == 4 || type == 5) { //Spy detector and frag to upgrade
		if (!Invade_buffPlayer(self) && (person->ammo_cells < (ups / 2) * PR_UPGRADE)) {//not enough cells
			sprint(person,PR_PRINT_HIGH,"You need more cells to upgrade the tesla\n");
			return PR_FALSE;
		}
	}
	else
	{
		if (!Invade_buffPlayer(self) && (person->ammo_cells < ups * PR_UPGRADE)) {//not enough cells
			sprint(person,PR_PRINT_HIGH,"You need more cells to upgrade the tesla\n");
			return PR_FALSE;
		}
	}
	if (type ==1)
	{
		if (sent->ammo_shells >= 3) {
			sprint(self,PR_PRINT_HIGH,"You can only upgrade voltage 3 times\n");
			return PR_FALSE;
		}
		else {
			if (!Invade_buffPlayer(self))
				self->ammo_cells = self->ammo_cells - (ups * PR_UPGRADE);
			sent->has_sentry = sent->has_sentry - ups;
			return PR_TRUE;
		}
	}
	if (type ==2)
	{
		if (sent->ammo_nails >= 3) {
			sprint(person,PR_PRINT_HIGH,"You can only upgrade amperage 3 times\n");
			return PR_FALSE;
		}
		else {
			if (!Invade_buffPlayer(self))
				self->ammo_cells = self->ammo_cells - (ups * PR_UPGRADE);
			sent->has_sentry = sent->has_sentry - ups;
			return PR_TRUE;
		}
	}
	if (type ==3)
	{
		if (sent->ammo_rockets >= 3) {
			sprint(person,PR_PRINT_HIGH,"You can only upgrade the capacitor 3 times\n");
			return PR_FALSE;
		}
		else {
			if (!Invade_buffPlayer(self))
				self->ammo_cells = self->ammo_cells - (ups * PR_UPGRADE);
			sent->has_sentry = sent->has_sentry - ups;
			return PR_TRUE;
		}
	}
	if (type ==4)
	{
		if (sent->tf_items & PR_NIT_AUTOID) {
			sprint(person,PR_PRINT_HIGH,"You already built a spy detector\n");
			return PR_FALSE;
		}
		else {
			if (!Invade_buffPlayer(self))
				self->ammo_cells = self->ammo_cells - (ups * PR_UPGRADE);
			sent->has_sentry = sent->has_sentry - ups;
			return PR_TRUE;
		}
	}
	if (type ==5)
	{
		if (sent->tf_items & PR_NIT_TELEPORTER) {
			sprint(person,PR_PRINT_HIGH,"You already have the upgrade\n");
			return PR_FALSE;
		}
		else {
			if (!Invade_buffPlayer(self))
				self->ammo_cells = self->ammo_cells - (ups * PR_UPGRADE);
			sent->has_sentry = sent->has_sentry - ups;
			return PR_TRUE;
		}
	}
	if (type ==6)
	{
		if (sent->tf_items & PR_NIT_KEVLAR) {
			sprint(person,PR_PRINT_HIGH,"The Tesla already has Kevlar Armor\n");
			return PR_FALSE;
		}
		else {
			if (!Invade_buffPlayer(self))
				self->ammo_cells = self->ammo_cells - (ups * PR_UPGRADE);
			sent->has_tesla = sent->has_tesla - ups;
			return PR_TRUE;
		}
	}
	if (type ==7)
	{
		if (sent->tf_items & PR_NIT_BLAST) {
			sprint(person,PR_PRINT_HIGH,"The Tesla already has Blast Armor\n");
			return PR_FALSE;
		}
		else {
			if (!Invade_buffPlayer(self))
				self->ammo_cells = self->ammo_cells - (ups * PR_UPGRADE);
			sent->has_tesla = sent->has_tesla - ups;
			return PR_TRUE;
		}
	}
	if (type ==8)
	{
		if (sent->tf_items & PR_NIT_ASBESTOS) {
			sprint(person,PR_PRINT_HIGH,"The Tesla already has Asbestos Armor\n");
			return PR_FALSE;
		}
		else {
			if (!Invade_buffPlayer(self))
				self->ammo_cells = self->ammo_cells - (ups * PR_UPGRADE);
			sent->has_tesla = sent->has_tesla - ups;
			return PR_TRUE;
		}
	}
	if (type ==9)
	{
		if (sent->tf_items & PR_NIT_TURRET) {
			sprint(person,PR_PRINT_HIGH,"The Tesla is already a turret!\n");
			return PR_FALSE;
		}
		else {
			if (!Invade_buffPlayer(self))
				self->ammo_cells = self->ammo_cells - (ups * PR_UPGRADE);
			sent->has_tesla = sent->has_tesla - ups;
			return PR_TRUE;
		}
	}
	if (type ==10)
	{
		if (sent->tf_items & PR_NIT_SCANNER) {
			sprint(person,PR_PRINT_HIGH,"The Tesla already has an improved targeter\n");
			return PR_FALSE;
		}
		else {
			if (!Invade_buffPlayer(self))
				self->ammo_cells = self->ammo_cells - (ups * PR_UPGRADE);
			sent->has_tesla = sent->has_tesla - ups;
			return PR_TRUE;
		}
	}
	if (type ==11)
	{

		//sprint(person,#PRINT_HIGH,"Not implemented yet!\n");
		//	return #FALSE;
		if (sent->tf_items & PR_NIT_TESLA_CLOAKING) {
			sprint(person,PR_PRINT_HIGH,"The Tesla already has a cloaking device\n");
			return PR_FALSE;
		}
		else {
			if (!Invade_buffPlayer(self))
				self->ammo_cells = self->ammo_cells - (ups * PR_UPGRADE);
			sent->has_tesla = sent->has_tesla - ups;
			return PR_TRUE;
		}
	}
	return PR_FALSE; //Should not happen

}
//Cause its used 2x
void Menu_EngineerRepair_Tesla()
{
	float cost;
	float maxcells;
	if (self->building->health < self->building->max_health) //CH allow repair with limited cells
	{
		cost = (self->building->max_health - self->building->health) / 2;
		if (cost > self->ammo_cells)
			cost = self->ammo_cells;

			self->ammo_cells = self->ammo_cells - cost;
		self->building->health = self->building->health + (cost * 2);
		if (self->building->health >= self->building->max_health)
			self->building->health = self->building->max_health;
	}

	maxcells = self->building->maxammo_cells;

	cost = maxcells - self->building->ammo_cells;
	if (cost > self->ammo_cells) cost = self->ammo_cells;
	self->ammo_cells = self->ammo_cells - cost;
	self->building->ammo_cells = self->building->ammo_cells + cost;
	if (self->building->ammo_cells >= maxcells)
		self->building->ammo_cells = maxcells;
}
// PZ: It now returns whether or not we need to wait for a confirmation before dismantling. It previously returned `void`.
float Menu_EngineerDismantle_Tesla()
{
	float waitForConfirmation; waitForConfirmation = PR_FALSE; // PZ
	if (self->building == world)
		return waitForConfirmation; // PZ

	if (self->building->tf_items & PR_NIT_SECURITY_CAMERA) {
		sprint(self, PR_PRINT_HIGH, "You can't dismantle it. It's not that easy!\n");
		return waitForConfirmation; // PZ
	}

	if (CheckEnemyDismantle() == PR_FALSE) return waitForConfirmation; // PZ

#ifdef PR_QUAKE_WORLD
	if (Teammate(self->building->real_owner, self) && teamplay != 0 && self->building->real_owner != self)
	{
		// PZ - no more friendly dismantles (use vote punish for tkers)
		sprint(self, PR_PRINT_HIGH, "You can not dismantle friendly devices. Use 'punish' to kick teamkillers.\n");
	}
#endif
	//RemoveMyTimers(self.building); // psionic clean-up

	else if (!Teammate(self->building->real_owner, self))
	{
		sprint(self, PR_PRINT_HIGH, "You dismantle the Tesla Gun.\n");
		//CH give .5 of ammount of build====give 25*health level (0-3)====give .5 of cells
		self->ammo_cells = self->ammo_cells + (PR_BUILD_COST_TESLA / 2) + (self->building->ammo_rockets * 25) + (self->building->ammo_cells / 2);
		//self.building.real_owner.has_tesla = #FALSE;
		self->building->real_owner->has_tesla = self->building->real_owner->has_tesla - (self->building->real_owner->has_tesla & self->building->dont_do_triggerwork);
		dremove(self->building);
	}
	else if (self->building->real_owner == self)
	{
		// PZ - confirm check
		waitForConfirmation = PR_TRUE;
		self->impulse = 0;
		self->group_no = 1;
		Menu_EngineerConfirmDismantle();
		self->current_menu = PR_MENU_ENGINEER_CONFIRM_DISMANTLE;
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
	}
	return waitForConfirmation; // PZ
}
//CH does random and picks what upgrade to give
void Tesla_Add_Rand_Upgrade(entity sent, entity who)
{
	float num;
	num = random();
	if (num <= 0.8) //give them a normal upgrade
	{
		sprint(who,PR_PRINT_HIGH,"The tesla got a normal upgrade!\n");
		sent->has_sentry = sent->has_sentry + 1;
	}
	else
	{
		sprint(who,PR_PRINT_HIGH,"The tesla got a misc upgrade!\n");
		sent->has_tesla = sent->has_tesla + 1;
	}
}
void Menu_EngineerFix_Tesla()
{
	string l1,l2;

	l1 = Return_Colored_Num(self->building->has_sentry);
	l2 = S_("^1.. Upgrade Voltage           ^b-^b 1u\n^2.. Upgrade Amperage          ^b-^b 1u\n^3.. Upgrade Power Supply      ^b-^b 1u\n^4.. Add Spy Detector          ^b-^b 4u\n^5.. Upgrades from frags       ^b-^b 3u\n^6.. Tinker                        \n^7.. Repair and Recharge           \n^8.. Dismantle                     \n^9.. ^bNothing^b                       \n\n^0.. Misc Upgrades  \n");

	CenterPrint4(self, S_("Tesla ^bModification Lab^b: "),l1, " Upgrades Left\n\n\x9D\x9E\x9E\x9E\x9E\x9E\x9E\x9F Normal Tesla Upgrades \x9D\x9E\x9E\x9E\x9E\x9E\x9E\x9F\n\n" ,l2);

}
void Menu_EngineerFix_Tesla_Input(float inp)
{
	/*local string temp;
	local float cost = 0;
	local float maxcells;*/
	float waitToDismantle; waitToDismantle = PR_FALSE; // PZ: added `waitToDismantle`
	if (self->classname != "player" || self->building == world)
		return;

	if (inp <= 10 && inp >= 1)
	{
		if (self->building->real_owner != self && (inp < 7 || inp == 10) )
		{
			sprint(self,PR_PRINT_HIGH,"Sorry, only the owner can do that\n");
			return;
		}

		if (inp == 1) { //Upgrade volt
			if (Check_Tesla_Ups(1, self, self->building, 1, 0))
			{
				self->building->ammo_shells = self->building->ammo_shells + 1;
				self->building->waitmin = (self->building->ammo_shells + 2) * (self->building->ammo_nails + 2); //Ammo consumption
			}
		}
		if (inp == 2) { //Upgrade amperage
			if (Check_Tesla_Ups(1, self, self->building, 2, 0))
			{
				self->building->ammo_nails = self->building->ammo_nails + 1;
				self->building->waitmin = (self->building->ammo_shells + 2) * (self->building->ammo_nails + 2); //Ammo consumption
			}
		}
		if (inp == 3) { //Upgrade power supply
			if (Check_Tesla_Ups(1, self, self->building, 3, 0))
			{
				float HPfactor,num,AMMOfactor;
				if (self->building->all_active & PR_IMPROVED_THREE)
					HPfactor=2; 	// TODO: add macros for this and armor levels in defs.qc
				else
					HPfactor=1;

				if (self->building->all_active & PR_IMPROVED_ONE)
					AMMOfactor=2;	// TODO: and this too!
				else
					AMMOfactor=1;

				num=self->building->health/self->building->max_health;

				self->building->ammo_rockets = self->building->ammo_rockets + 1;
				//Upgrade them //- OfN - teslas start with 150 hp
				// Gizmo - fixed tinkering for health and then tinkering again for power and losing your health bonuses
				if (self->building->ammo_rockets == 1) {
					//self.building.max_health = self.building.max_health + 75; // 225
					self->building->max_health = floor( self->building->max_health + 75 * HPfactor );
					//self.building.health = self.building.health + 75;
					self->building->maxammo_cells = self->building->maxammo_cells + PR_ADDCELLS1 * AMMOfactor;
					self->building->ammo_cells = self->building->maxammo_cells;
				}
				else if (self->building->ammo_rockets == 2) {
					//self.building.max_health = self.building.max_health + 100;// 325
					self->building->max_health = floor( self->building->max_health + 100 * HPfactor );// 325
					//self.building.health = self.building.health + 100;
					//self.building.maxammo_cells = self.building.maxammo_cells + #ADDCELLS2; //Add
					self->building->maxammo_cells = self->building->maxammo_cells + PR_ADDCELLS2 * AMMOfactor;
					self->building->ammo_cells = self->building->maxammo_cells;
				}
				else if (self->building->ammo_rockets == 3) {
					//self.building.max_health = self.building.max_health + 75; // 400
					self->building->max_health = floor( self->building->max_health + 75 * HPfactor ); // 400
					//self.building.health = self.building.health + 75;
					self->building->maxammo_cells = self->building->maxammo_cells + PR_ADDCELLS3 * AMMOfactor; //Add
					self->building->ammo_cells = self->building->maxammo_cells;
				}

				self->building->health=self->building->max_health*num;
			}
		}
		if (inp == 4) { //Spy Detector
			if (Check_Tesla_Ups(4, self, self->building, 4, 0))
			{
				self->building->tf_items = self->building->tf_items | PR_NIT_AUTOID;
			}
		}
		if (inp == 5) { //CH tesla frags generate extra upgrades
			if (Check_Tesla_Ups(3, self, self->building, 5, 0))
			{
				self->building->tf_items = self->building->tf_items | PR_NIT_TELEPORTER;
			}
		}
		if (inp == 6){ // Tinker by Cyt0  Remade by CH to fit with new code
			if (self->building->tf_items & PR_NIT_TELEPORTER) //- ofn - workaround to solve the HP/cells increase by tinker combined with the hack
			{
				sprint(self,PR_PRINT_HIGH,"You can't tinker a tesla in upgrade from frags mode!\n");
			}
			else
			{
				DoTinker();
			}
		}
		if (inp == 7) { //Repair and Rearm
			Menu_EngineerRepair_Tesla();
		}
		if (inp == 8) {
			waitToDismantle = Menu_EngineerDismantle_Tesla();
		}
		//if (inp == 9) {
		//}
		if (inp == 10) {
			self->impulse = 0;
			Menu_EngineerFix_Tesla2();
			self->current_menu = PR_MENU_ENGINEER_FIX_TESLA2;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}

		//if (inp >= 1 && inp <= 7)
		if (inp >= 1 && inp <= 9 && !waitToDismantle) // PZ: brought back old code; added `waitToDismantle`
		{
			ResetMenu();
			self->impulse = 0;

			if (self->ammo_cells < 0) self->ammo_cells = 0;

			bound_other_ammo(self);

			if (self->armorvalue == 0)
			{
				self->armortype = 0; // lost all armor
				self->armorclass = 0; // lost special armor
				self->items = self->items - (self->items & (PR_IT_ARMOR1 | PR_IT_ARMOR2 | PR_IT_ARMOR3));
			}

			W_SetCurrentAmmo();
		}
	}
}
void Menu_EngineerFix_Tesla2()
{
	string l1,l2;

	l1 = Return_Colored_Num(self->building->has_tesla);
	l2 = S_("^1.. Kevlar Armor              ^b-^b 1u\n^2.. Blast Armor               ^b-^b 1u\n^3.. Asbestos Armor            ^b-^b 1u\n^4.. Make TeslaTurret(tm)      ^b-^b 1u\n^5.. Improved Targeting System ^b-^b 1u\n^6.. Cloaking Device           ^b-^b 1u\n^7.. Repair and Recharge           \n^8.. Dismantle                     \n^9.. ^bNothing^b                       \n\n^0.. Normal Upgrades\n");

	CenterPrint4(self, S_("Tesla ^bModification Lab^b: "),l1, " Upgrades Left\n\n\x80\x81\x81\x81\x81\x81\x81\x81\x82 Misc Tesla Upgrades \x80\x81\x81\x81\x81\x81\x81\x81\x82\n\n" ,l2);
}
void Menu_EngineerFix_Tesla_Input2(float inp)
{
	//local string temp;
	//local float cost = 0;
	//local float maxcells;
	float waitToDismantle; waitToDismantle = PR_FALSE; // PZ: added `waitToDismantle`
	if (self->classname != "player" || self->building == world)
		return;

	if (inp <= 10 && inp >= 1)
	{
		if (inp == 1) { //Kevlar
			if (Check_Tesla_Ups(1, self, self->building, 6, 1))
			{
				self->building->tf_items = self->building->tf_items | PR_NIT_KEVLAR;
				self->building->armorclass = self->building->armorclass | PR_AT_SAVESHOT;
			}
		}
		if (inp == 2) { //Blast
			if (Check_Tesla_Ups(1, self, self->building, 7, 1))
			{
				self->building->tf_items = self->building->tf_items | PR_NIT_BLAST;
				self->building->armorclass = self->building->armorclass | PR_AT_SAVEEXPLOSION;
			}
		}
		if (inp == 3) { //Blast
			if (Check_Tesla_Ups(1, self, self->building, 8, 1))
			{
				self->building->tf_items = self->building->tf_items | PR_NIT_ASBESTOS;
				self->building->armorclass = self->building->armorclass | PR_AT_SAVEFIRE;
			}
		}
		if (inp == 4) { //Tesla Turret(tm)

			if (Check_Tesla_Ups(1, self, self->building, 9, 1))
			{
				if (self->building->tf_items & PR_NIT_TURRET) {
					sprint(self, PR_PRINT_HIGH, "Gun is already deployed\n");
				}
				else {
					self->building->origin[Z] = self->building->origin[Z] + 15; // +15//Elevate for the check
					//Make sure the launch area is clear
					if (PR_FALSE) {
					//if (CheckArea(self.building,self) == #FALSE) {
						sprint(self, PR_PRINT_HIGH, "You need a clear area to launch\n");
						self->building->origin[Z] = self->building->origin[Z] - 15; //-15//Set back down
					}
					else {
						//self.building.origin_z = self.building.origin_z + 25; //+ 25 //Finish liftoff
						self->building->origin[Z] = self->building->origin[Z] - 35;//- 25;//40;//- 25; //often fixes tesla not touchin ceiling//+ 25 //Finish liftoff
						sprint(self, PR_PRINT_HIGH, "You turretize the tesla\n");

						if (self->building->job == 1 && self->building->tf_items & PR_NIT_TESLA_CLOAKING)
						{
							/*
							local entity TSelf, OSelfB;
							OSelfB=self.building;
							TSelf=TeslaClone(OSelfB);
							dremove(OSelfB);
							self.building=TSelf; // NEEDED? teslaclone should have updated this pointer anyway..
							*/
							setmodel (self->building, "progs/coil.mdl");

							spawnFOG(self->building->origin);
							sound (self->building, PR_CHAN_MISC, "misc/r_tele4.wav", 1, PR_ATTN_NORM);
						}

						self->building->angles[Z] = 180;
						self->building->flags = self->building->flags - (self->building->flags & PR_FL_ONGROUND);
						self->building->movetype = PR_MOVETYPE_FLY;
						self->building->velocity[Z] = 200;
						setsize (self->building, V({-16, -16, 10}), V({16, 16, 40}));//35');//'-16 -16 -40', '16 16 -10'); //WK -40, -10
						self->building->tf_items = self->building->tf_items | PR_NIT_TURRET;
						self->building->is_haxxxoring=1; //flag determines if flying and it's used for cloaking teslas, set to 0 when landed

					}
				}
			}
		}
		if (inp == 5) { //Improved Targeter
			if (Check_Tesla_Ups(1, self, self->building, 10, 1))
			{
				self->building->tf_items = self->building->tf_items | PR_NIT_SCANNER;
			}
		}
		//CH its commented out here and in the centerprint - OfN nomore!
		if (inp == 6) { //Cloaking tesla
			if (Check_Tesla_Ups(1, self, self->building, 11, 1))
			{
				self->building->tf_items = self->building->tf_items | PR_NIT_TESLA_CLOAKING;
				if (self->building->job == 1)
				{
					entity oself;
					oself=self;
					self=self->building;
					self->job=3; //- OfN - this flag indicates it must skip condition on lose_glow
					Tesla_Lose_Glow();
					self=oself;
				}
			}
		}

		if (inp == 7) { //Repair and Rearm
			Menu_EngineerRepair_Tesla();
		}
		if (inp == 8) {
			waitToDismantle = Menu_EngineerDismantle_Tesla();
		}
		//if (inp == 9) {
		//}
		if (inp == 10) {
			self->impulse = 0;
			Menu_EngineerFix_Tesla();
			self->current_menu = PR_MENU_ENGINEER_FIX_TESLA;
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}

		//if (inp >= 1 && inp <= 7)
		if (inp >= 1 && inp <= 9 && !waitToDismantle) // PZ: brought back old code; added `waitToDismantle`
		{
			ResetMenu();
			self->impulse = 0;

			if (self->ammo_cells < 0) self->ammo_cells = 0;

			bound_other_ammo(self);

			if (self->armorvalue == 0)
			{
				self->armortype = 0; // lost all armor
				self->armorclass = 0; // lost special armor
				self->items = self->items - (self->items & (PR_IT_ARMOR1 | PR_IT_ARMOR2 | PR_IT_ARMOR3));
			}

			W_SetCurrentAmmo();
		}
	}
}

//============================================================================
// Menu for using buildings
void Menu_Dispenser()
{
	if (self->classname != "player" || self->building == world || self->building->classname != "building_dispenser")
		return;

	if (self->building->all_active & PR_IMPROVED_FOUR)
	{
		string st, st2, st3, st4;

		st = S_("^bUse Dispenser^b:              \n\n^1.. Withdraw some ammo          \n^2.. Withdraw some Armor         \n\n");

		if (self->maxammo_detpack > 0)
			st2 = S_("^3.. Get detpack                 \n");
		else st2 = "\n";

		if (self->cutf_items & PR_CUTF_TOSSABLEDET )
			st3 = S_("^4.. Get C4 detpack              \n");
		else st3 = "\n";

		st4 = S_("^5.. Get medikit                 \n\n^0.. ^bNothing^b                     \n\n");

		CenterPrint4(self, st, st2, st3, st4);
	}
	else
		CenterPrint(self, S_("^bUse Dispenser^b:             \n\n^1.. Withdraw some ammo          \n^2.. Withdraw some armor         \n\n^0.. ^bNothing^b                     \n\n"));
}

void CureAdverseEffects(entity doc, entity patient, const vector& org);

void Menu_Dispenser_Input(float inp)
{
	float am, empty;
	string desc;

	if (self->classname != "player" || self->building == world)
		return;

	empty = PR_FALSE;
	if (inp == 1)
	{
		if (self->building->ammo_shells == 0 && self->building->ammo_nails == 0 &&
			self->building->ammo_rockets == 0 && self->building->ammo_cells == 0)
		{
			empty = PR_TRUE;
		}
		else
		{
			float anydone; // OfN - To determine if sound should be played
			string tmpstr;
			anydone = PR_FALSE;

			// shells
			am = self->maxammo_shells - self->ammo_shells;
			if (am > self->building->ammo_shells)
				am = self->building->ammo_shells;
			self->building->ammo_shells = self->building->ammo_shells - am;
			self->ammo_shells = self->ammo_shells + am;

			if (am > 0)
			{
				anydone = PR_TRUE;
				tmpstr = ftos(floor(am));
				sprint(self,PR_PRINT_LOW,"You get ",tmpstr," shells.\n");
			}

			// nails
			am = self->maxammo_nails - self->ammo_nails;
			if (am > self->building->ammo_nails)
				am = self->building->ammo_nails;
			self->building->ammo_nails = self->building->ammo_nails - am;
			self->ammo_nails = self->ammo_nails + am;

			if (am > 0)
			{
				anydone = PR_TRUE;
				tmpstr = ftos(floor(am));
				sprint(self,PR_PRINT_LOW,"You get ",tmpstr," nails.\n");
			}

			// rockets
			am = self->maxammo_rockets - self->ammo_rockets;
			if (am > self->building->ammo_rockets)
				am = self->building->ammo_rockets;
			self->building->ammo_rockets = self->building->ammo_rockets - am;
			self->ammo_rockets = self->ammo_rockets + am;

			if (am > 0)
			{
				anydone = PR_TRUE;
				tmpstr = ftos(floor(am));
				sprint(self,PR_PRINT_LOW,"You get ",tmpstr," rockets.\n");
			}

			// cells
			am = self->maxammo_cells - self->ammo_cells;
			if (am > self->building->ammo_cells)
				am = self->building->ammo_cells;
			self->building->ammo_cells = self->building->ammo_cells - am;
			self->ammo_cells = self->ammo_cells + am;

			if (am > 0)
			{
				anydone = PR_TRUE;
				tmpstr = ftos(floor(am));
				sprint(self,PR_PRINT_LOW,"You get ",tmpstr," cells.\n");
			}

			// SB gren1s
			float grenammo;
			grenammo = GetMaxGrens(self,1);

			am =  grenammo - self->no_grenades_1;
			if (am*15 > self->building->ammo_rockets)
				am = floor(self->building->ammo_rockets / 15);
			self->building->ammo_rockets = self->building->ammo_rockets - am * 15;
			self->no_grenades_1 = self->no_grenades_1 + am;

			if (am > 0)
			{
				anydone = PR_TRUE;
				tmpstr = ftos(floor(am));
				desc = GetGrenadeText(self->tp_grenades_1);
				sprint(self,PR_PRINT_LOW,"You get ",tmpstr," ",desc," grenades.\n");
			}

			// SB gren2s
			grenammo = GetMaxGrens(self,2);

			am =  grenammo - self->no_grenades_2;
			if (am*15 > self->ammo_rockets)
				am = floor(self->ammo_rockets / 15);
			self->building->ammo_rockets = self->building->ammo_rockets - am * 15;
			self->no_grenades_2 = self->no_grenades_2 + am;

			if (am > 0)
			{
				anydone = PR_TRUE;
				tmpstr = ftos(floor(am));
				desc = GetGrenadeText(self->tp_grenades_2);
				sprint(self,PR_PRINT_LOW,"You get ",tmpstr," ",desc," grenades.\n");
			}

			if (anydone)
				sound(self->building,PR_CHAN_MISC,"weapons/lock4.wav",1,PR_ATTN_IDLE);
		}
	}
	else if (inp == 2)
	{
		if (self->building->is_malfunctioning & PR_SCREWUP_FOUR)
		{
			sprint(self,PR_PRINT_HIGH,"Trapped dispenser, have a nice day!\n");
			TF_T_Damage(self->building, self->building, self->building, 500, 0, PR_TF_TD_OTHER);
			return;
		}

		if (self->building->armorvalue == 0)
		{
			empty = PR_TRUE;
		}
		else
		{
			// armor
			am = self->maxarmor - self->armorvalue;
			if (am > self->building->armorvalue)
				am = self->building->armorvalue;

			//if (self->armortype == 0 || self->armortype != self->armor_allowed)
			if (isApproxEqual(self->armortype, 0, 0.05) || !isApproxEqual(self->armortype, self->armor_allowed, 0.05)) // PZ: For C++ floating point math.
			{
				self->armortype = self->armor_allowed;
				//self.items = self.items | #IT_ARMOR1;
				W_SetCurrentAmmo();
			}

			self->building->armorvalue = self->building->armorvalue - am;
			self->armorvalue = self->armorvalue + am;

			if (am > 0)
			{
				sound(self->building,PR_CHAN_MISC,"items/armor1.wav",1,PR_ATTN_IDLE);
				sprint(self,PR_PRINT_LOW,"You get some armor.\n");
			}
		}
	}
	else if (inp == 3 && self->maxammo_detpack > 0)
	{
		if (self->building->ammo_detpack < 1)
		{
			empty = PR_TRUE;
		}
		else
		{
			// detpacks
			am = floor(self->maxammo_detpack - self->ammo_detpack);
			if (am > floor(self->building->ammo_detpack))
				am = floor(self->building->ammo_detpack);
			self->building->ammo_detpack = self->building->ammo_detpack - am;
			self->ammo_detpack = self->ammo_detpack + am;

			if (am < 1)
				sprint(self,PR_PRINT_HIGH,"You can not carry more detpacks!\n");
		}
	}
	else if (inp == 4 && self->cutf_items & PR_CUTF_TOSSABLEDET)
	{
		if (self->building->ammo_c4det < 1)
		{
			empty = PR_TRUE;
		}
		else
		{
			// detpacks
			am = floor(self->maxammo_detpack - self->ammo_c4det);
			if (am > floor(self->building->ammo_c4det))
				am = floor(self->building->ammo_c4det);
			self->building->ammo_c4det = self->building->ammo_c4det - am;
			self->ammo_c4det = self->ammo_c4det + am;

			if (am < 1)
				sprint(self,PR_PRINT_HIGH,S_("You can not carry more ^bC^b^4 detpacks!\n"));
		}
	}
	else if (inp == 5 && self->building->all_active & PR_IMPROVED_FOUR)
	{
		if (self->building->ammo_medikit < PR_DISPENSER_MEDIKIT_MINIMUM) // OfN minimum
		{
			empty = PR_TRUE;
		}
		else
		{
		// Gizmo - added am2 (move up top if needed else where)
			float am2;

			CureAdverseEffects(self,self, self->origin);

		// heal player (ceil fixes 1 less than max health bug)
			am = ceil(self->max_health - self->health);
			am2 = ceil(self->maxammo_medikit - self->ammo_medikit);

			if (am > floor(self->building->ammo_medikit))
				am = floor(self->building->ammo_medikit);

			if (am >= 1)
				T_Heal(self, am, PR_FALSE);

			self->building->ammo_medikit = self->building->ammo_medikit - am;

		// Gizmo - give medikit ammo too
			if (am2 > floor(self->building->ammo_medikit))
				am2 = floor(self->building->ammo_medikit);

			if (am2 >= 1)
				self->ammo_medikit = self->ammo_medikit + am2;

			self->building->ammo_medikit = self->building->ammo_medikit - am2;

		// let the player know what happened
			if (am + am2) {
				sound(self, PR_CHAN_WEAPON, "items/r_item1.wav", 1, PR_ATTN_NORM);
				sprint(self,PR_PRINT_LOW,"You get some medikit.\n");
			}
		}
	}

	if (inp >= 1 && inp <= 5)
	{
		if (empty)
		{
			if (inp == 5)
				sprint(self, PR_PRINT_HIGH, "Not enough medikit in dispenser.\n");
			else
				sprint(self, PR_PRINT_HIGH, "The dispenser is empty.\n");
		}
		ResetMenu();
		self->impulse = 0;
		self->building = world;
		self->building_wait = time + 0.5;

		bound_other_ammo(self);

		if (self->armorvalue == 0)
		{
			self->armortype = 0; // lost all armor
			self->armorclass = 0; // lost special armor
			self->items = self->items - (self->items & (PR_IT_ARMOR1 | PR_IT_ARMOR2 | PR_IT_ARMOR3));
		}

		W_SetCurrentAmmo();
	}

	if (inp == 10)
	{
		ResetMenu();
		self->impulse = 0;
	}

	// Otherwise, the impulse is passed on
}

void MenuResetCount()
{
	self->menu_count = 0;
	self->tfstate = self->tfstate - (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY);
}

} // END namespace Progs
