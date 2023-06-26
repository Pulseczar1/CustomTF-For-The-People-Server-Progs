/*=======================================================//
// CuTFMenu.QC - CustomTF 3.2.OfN         - 30/10/2004 - //
// by Sergio Fuma�a Grunwaldt - OfteN [cp]               //
=========================================================//
 All the stuff related to the customizing menu is here
 Moved to its own file for organization purposes
 Here now single price definitions also.. finally! =)
=========================================================*/

#include "progs.h"
#include "cutfmenu.h"
#include "menu.h"
#include "custom.h"

namespace Progs {

string ConstructMenuItemStr(float itemnum, string itemdesc, float cost, int field, int bit);
string ConstructMenuTeslaStr(float itemnum, string itemdesc, float cost);
string ConstructMenuSentryStr(float itemnum, string itemdesc, float cost);
string ConstructMenuJobStr(float itemnum, string itemdesc, int jobid, float cost, float extra1cost, float extra2cost, const string& extra1st, const string& extra2st);
string ConstructMenuGrenStr(float itemnum, string itemdesc, float cost, float grentype);
string ConstructMenuSpeedStr(float itemnum, string itemdesc, float cost);
string ConstructMenuHPStr(float itemnum, string itemdesc, float cost);
float SpentOnJob();
float SpentOnGrens();
float SpentOnLegs();
float SpentOnHP();
void UpdateArmorItem(entity player);

// PZ: Moved #defines to header file.

//==============================================================================
// The following "ancient" functions have been there doing the work for years
// I feel sad removing them, so they r still here :P

//----------------------------------------------------------------------
// Rod May	kk@rod.net
// mstr and mstr2 and eqstr only exist to cut down on a crap load of code.
//
/*string(float v,float m1,float m2,string s1,string s2,string s3,string s4) mstr =
{
	// return string for matching masks (m1 and m2) on value v.
	// neither match: s1, first only: s2, second only: s3, both: s4

	if ((v & m1) && (v & m2)) return s4;
	if (v & m1) return s2;
	if (v & m2) return s3;
	return s1;
};
string(float v1,float v2,float m1,float m2,string s1,string s2,string s3,string s4) mstr2 =
{
	// same func as above but with a value for each mask.
	// return string for matching mask m1 on value v1, and m2 on v2.

	if ((v1 & m1) && (v2 & m2)) return s4;
	if (v1 & m1) return s2;
	if (v2 & m2) return s3;
	return s1;
};*/
/*string(float v,float v1,float v2,string s1,string s2,string s3) eqstr =
{
	// return string based on v equals values v1 OR v2
	// neither equal: s1, first only: s2, second only: s3

	if (v == v1) return s2;
	if (v == v2) return s3;
	return s1;
};*/
//----------------------------------------------------------------------
//----------------------------------------------------------------------

//WK =====================================================================
//	Prints Menu of Primary Weapons Available
//	 =====================================================================
void Menu_PrimaryWeapon()
{
	string temp;
	temp = ftos(self->money);

	if (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY)
		temp = colstr(temp,PR_COLSTR_RED);

	MenuResetCount();

	string msg_custom;
	string l1,l2,l3,l4, str;

	l1 = ConstructMenuItemStr(1,PR_STR_SNIPERRIFLE,PR_COST_SNIPERRIFLE,self->weapons_carried,PR_WEAP_SNIPER_RIFLE);
	str = ConstructMenuItemStr(2,PR_STR_ASSAULTCANNON,PR_COST_ASSAULTCANNON,self->weapons_carried,PR_WEAP_ASSAULT_CANNON);
	l1 = strcat(l1,str);

	l2 = ConstructMenuItemStr(3,PR_STR_ROCKETLAUNCHER,PR_COST_ROCKETLAUNCHER,self->weapons_carried,PR_WEAP_ROCKET_LAUNCHER);
	str = ConstructMenuItemStr(4,PR_STR_PYROTOYS,PR_COST_PYROTOYS,self->weapons_carried,PR_WEAP_INCENDIARY);
	l2 = strcat(l2,str);

	l3 = ConstructMenuItemStr(5,PR_STR_GRENADELAUNCHER,PR_COST_GRENADELAUNCHER,self->weapons_carried,PR_WEAP_GRENADE_LAUNCHER);
	str = ConstructMenuItemStr(6,PR_STR_LIGHTNING,PR_COST_LIGHTNING,self->weapons_carried,PR_WEAP_LIGHTNING);
	l3 = strcat(l3,str);

	l4 = ConstructMenuItemStr(7,PR_STR_LIGHTASSAULT,PR_COST_LIGHTASSAULT,self->weapons_carried,PR_WEAP_LIGHT_ASSAULT);
	l4 = strcat(l4,S_("^8.. ^bLast^b    \n^9.. ^bNext^b    \n^0.. ^bDone^b!   \n"));  // PZ: Made menus loop. Added "Last".

	if (self->done_custom & PR_CUSTOM_SELLING)
		msg_custom = S_(" dollars left\n\n*CURRENTLY SELLING FRAGS*\nTyping ^bCUSTOM^b again will waste your cash\n");
	else           // PZ: used to say "Use the \bCUSTOM\b command to start over"
		msg_custom = S_(" dollars left\n\nUse the ^bCUSTOM^b command to clear entries\nUse the ^bUPGRADE^b command to sell frags\n");

	centerprint(self,S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 Primary Weapons    \x11\x9C\x8B\n\n"),l1,l2,l3,l4,temp,msg_custom);
}

void Menu_PrimaryWeapon_Input(float inp)
{
	if (inp <= 10 && inp >= 1)
	{
//Snipe
		if (inp == 1)
		{
			if (self->weapons_carried & PR_WEAP_SNIPER_RIFLE) //Already have it
			{
				PrintRefund(PR_COST_SNIPERRIFLE);
				self->weapons_carried = self->weapons_carried - PR_WEAP_SNIPER_RIFLE;
				self->weapons_carried = self->weapons_carried - PR_WEAP_AUTO_RIFLE;
				self->maxammo_shells = self->maxammo_shells - 75;
				Menu_PrimaryWeapon();
			}
			else if (self->money >= PR_COST_SNIPERRIFLE)
			{
				self->money = self->money - PR_COST_SNIPERRIFLE;
				self->weapons_carried = self->weapons_carried | PR_WEAP_SNIPER_RIFLE | PR_WEAP_AUTO_RIFLE;
				self->maxammo_shells = self->maxammo_shells + 75;
				PrintMoney();
				Menu_PrimaryWeapon();
			}
			else
				PrintNotEnoughMoney(PR_COST_SNIPERRIFLE);
		}
		if (inp == 2)
		{
			BuyWeapon(PR_COST_ASSAULTCANNON,PR_WEAP_ASSAULT_CANNON);
			Menu_PrimaryWeapon();
		}
		if (inp == 3)
		{
			BuyWeapon(PR_COST_ROCKETLAUNCHER,PR_WEAP_ROCKET_LAUNCHER);
			Menu_PrimaryWeapon();
			UpdateWeaponItems();
		}
//IC
		if (inp == 4)
		{
			if (self->weapons_carried & PR_WEAP_INCENDIARY) //Already have it
			{
				//Clean up asbestos armor
				if (!(self->tf_items & PR_NIT_ASBESTOS))
					self->money = self->money - PR_COST_ASBESTOS; //Sync this with asbestos
				self->tf_items = self->tf_items - (self->tf_items & PR_NIT_ASBESTOS);

				PrintRefund(PR_COST_PYROTOYS);
				self->weapons_carried = self->weapons_carried - PR_WEAP_INCENDIARY;
				self->weapons_carried = self->weapons_carried - PR_WEAP_FLAMETHROWER;
				self->maxammo_cells = self->maxammo_cells - 200;
				self->maxammo_rockets = self->maxammo_rockets - 60;
				Menu_PrimaryWeapon();
			}
			else if (self->money >= PR_COST_PYROTOYS)
			{
				self->money = self->money - PR_COST_PYROTOYS;
				self->weapons_carried = self->weapons_carried | PR_WEAP_INCENDIARY | PR_WEAP_FLAMETHROWER;
				//Give em asbestos armor, and a refund if they have it
				if (self->tf_items & PR_NIT_ASBESTOS)
					self->money = self->money + PR_COST_ASBESTOS; //Sync this with asbestos price
				self->tf_items = self->tf_items | PR_NIT_ASBESTOS;


				self->maxammo_cells = self->maxammo_cells + 200;
				self->maxammo_rockets = self->maxammo_rockets + 60;
				PrintMoney();
				Menu_PrimaryWeapon();
			}
			else
				PrintNotEnoughMoney(PR_COST_PYROTOYS);

			UpdateWeaponItems();
		}
		if (inp == 5)
		{
			BuyWeapon(PR_COST_GRENADELAUNCHER,PR_WEAP_GRENADE_LAUNCHER);
			Menu_PrimaryWeapon();
			UpdateWeaponItems();
		}
		if (inp == 6)
		{
			BuyWeapon(PR_COST_LIGHTNING,PR_WEAP_LIGHTNING);
			Menu_PrimaryWeapon();
			UpdateWeaponItems();
		}
		if (inp == 7)
		{
			BuyWeapon(PR_COST_LIGHTASSAULT,PR_WEAP_LIGHT_ASSAULT);
			Menu_PrimaryWeapon();
			UpdateWeaponItems();
		}
		if (inp == 8)   // PZ: Made it so that the menus loop from end to beginning and vice versa.
		{
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
			self->current_menu = PR_MENU_MOREOPTION;
			Menu_MoreOption();
		}
		if (inp == 9)
		{
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
			self->current_menu = PR_MENU_SECONDARY_WEAPON;
			Menu_SecondaryWeapon();
		}
		if (inp == 10)
		{
			if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
				DropFromCustomClassGen();
			else
				PrintNotEnoughMoney(0);
		}

		self->impulse = 0;
	}
	// Otherwise, the impulse is passed on
	// SB to where, nobody knows
}

// ====================================================================
//	Prints Menu of Secondary Weapons Available
// ====================================================================
void Menu_SecondaryWeapon()
{
	string temp;
	temp = ftos(self->money);

	if (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY)
		temp = colstr(temp,PR_COLSTR_RED);

	string l1,l2,l3,l4, str;

	MenuResetCount();

	l1 = ConstructMenuItemStr(1,PR_STR_SNG,PR_COST_SNG,self->weapons_carried,PR_WEAP_SNG);
	str = ConstructMenuItemStr(2,PR_STR_KNIFE,PR_COST_KNIFE,self->cutf_items,PR_CUTF_KNIFE);
	l1 = strcat(l1,str);

	l2 = ConstructMenuItemStr(3,PR_STR_SUPERSHOTGUN,PR_COST_SUPERSHOTGUN,self->weapons_carried,PR_WEAP_SUPER_SHOTGUN);
	str = ConstructMenuItemStr(4,PR_STR_RAILGUN,PR_COST_RAILGUN,self->weapons_carried,PR_WEAP_RAILGUN);
	l2 = strcat(l2,str);

	l3 = ConstructMenuItemStr(5,PR_STR_NAILGUN,PR_COST_NAILGUN,self->weapons_carried,PR_WEAP_NAILGUN);
	str = ConstructMenuItemStr(6,PR_STR_TRANQ,PR_COST_TRANQ,self->weapons_carried,PR_WEAP_TRANQ);
	l3 = strcat(l3,str);

	l4 = ConstructMenuItemStr(7,PR_STR_SHOTGUN,PR_COST_SHOTGUN,self->weapons_carried,PR_WEAP_SHOTGUN);
	l4 = strcat(l4,S_("^8.. ^bPrev^b    \n^9.. ^bNext^b    \n^0.. ^bDone^b!   \n"));

	centerprint(self, S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 Secondary Weapons  \x11\x9C\x8B\n\n"),l1,l2,l3,l4,temp," dollars left\n");
}

void Menu_SecondaryWeapon_Input(float inp)
{
	if (inp <= 10 && inp >= 1)
	{
		if (inp == 1) {
			BuyWeapon(PR_COST_SNG,PR_WEAP_SNG);
			Menu_SecondaryWeapon();
			UpdateWeaponItems();
		}

	//Knife
		if (inp == 2) {
			if (self->cutf_items & PR_CUTF_KNIFE) //Already have it
			{
				//Clean up knife-less warlocks (sync this with Custom.qc)
				if (self->job & PR_JOB_WARLOCK) { // OfN Fix getting extra money with demon lore
					sprint(self, PR_PRINT_HIGH, "You will need this knife being a warlock!\n");
					CuTFMenuSound(PR_MENUSOUND_WRONG);
					self->impulse=0;
					return;
				}

				PrintRefund(PR_COST_KNIFE);
				self->cutf_items = self->cutf_items - PR_CUTF_KNIFE;
				Menu_SecondaryWeapon();
			}
			else if (self->money >= PR_COST_KNIFE) {
				self->money = self->money - PR_COST_KNIFE;
				self->cutf_items = self->cutf_items | PR_CUTF_KNIFE;
				PrintMoney();
				Menu_SecondaryWeapon();
			}
			else
				PrintNotEnoughMoney(PR_COST_KNIFE);
		}
		if (inp == 3) {
			BuyWeapon(PR_COST_SUPERSHOTGUN,PR_WEAP_SUPER_SHOTGUN);
			Menu_SecondaryWeapon();
			UpdateWeaponItems();
		}
		if (inp == 4) {
			BuyWeapon(PR_COST_RAILGUN,PR_WEAP_RAILGUN);
			Menu_SecondaryWeapon();
		}
		if (inp == 5) {
			BuyWeapon(PR_COST_NAILGUN,PR_WEAP_NAILGUN);
			Menu_SecondaryWeapon();
			UpdateWeaponItems();
		}
		if (inp == 6) {
			BuyWeapon(PR_COST_TRANQ,PR_WEAP_TRANQ);
			Menu_SecondaryWeapon();
		}
		if (inp == 7) {
			BuyWeapon(PR_COST_SHOTGUN,PR_WEAP_SHOTGUN);
			Menu_SecondaryWeapon();
			UpdateWeaponItems();
		}
		if (inp == 8) {
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
			self->current_menu = PR_MENU_PRIMARY_WEAPON;
			Menu_PrimaryWeapon();
		}
		if (inp == 9)
		{
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
			self->current_menu = PR_MENU_MISC_WEAPON;
			Menu_MiscWeapon();
		}
		if (inp == 10)
		{
			if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
				DropFromCustomClassGen();
			else
				PrintNotEnoughMoney(0);
		}

		self->impulse = 0;
	}
}


// ====================================================================
//	Prints Menu of Secondary Weapons Available
// ====================================================================
void Menu_MiscWeapon()
{
	string temp;
	temp = ftos(self->money);

	if (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY)
		temp = colstr(temp,PR_COLSTR_RED);

	string l1,l2,l3,l4, str;

	MenuResetCount();

	l1 = ConstructMenuItemStr(1,PR_STR_MAUSER,PR_COST_MAUSER,self->weapons_carried,PR_WEAP_MAUSER);
	str = ConstructMenuItemStr(2,PR_STR_AIRF,PR_COST_AIRF,self->weapons_carried,PR_WEAP_AIRF);
	l1 = strcat(l1,str);

	l2 = ConstructMenuItemStr(3,PR_STR_AIRFISTUPGRADE,PR_COST_AIRFISTUPGRADE,self->cutf_items,PR_CUTF_AIRFISTUPGRADE);
	str = ConstructMenuItemStr(4,PR_STR_DAEDALUS,PR_COST_DAEDALUS,self->weapons_carried,PR_WEAP_DAEDALUS);
	l2 = strcat(l2,str);

	l3 = ConstructMenuItemStr(5,PR_STR_LASERCANNON,PR_COST_LASERCANNON,self->weapons_carried,PR_WEAP_LASERCANNON);
	str = ConstructMenuItemStr(6,PR_STR_HOLO,PR_COST_HOLO,self->cutf_items,PR_CUTF_HOLO);
	l3 = strcat(l3,str);

	l4 = ConstructMenuItemStr(7,PR_STR_CLIPEXTEND,PR_COST_CLIPEXTEND,self->cutf_items,PR_CUTF_CLIPEXTEND);
	l4 = strcat(l4,S_("^8.. ^bPrev^b    \n^9.. ^bNext^b    \n^0.. ^bDone^b!   \n"));

	centerprint(self, S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 Misc^b/^bExtra Weapons \x11\x9C\x8B\n\n"),l1,l2,l3,l4,temp," dollars left\n");
}

void Menu_MiscWeapon_Input(float inp)
{
	if (inp <= 10 && inp >= 1)
	{

		if (inp == 1) {
			BuyWeapon(PR_COST_MAUSER, PR_WEAP_MAUSER);
			//sprint(self, #PRINT_HIGH, "Unfortunately all stocks of this item have been requisitioned to increase the glory of the Queen. God save the Queen!\n");
			Menu_MiscWeapon();
		}
		if (inp == 2) {

			BuyWeapon(PR_COST_AIRF,PR_WEAP_AIRF);
			//sprint(self, #PRINT_HIGH, "Unfortunately all stocks of this item have been requisitioned to increase the glory of the Queen. God save the Queen!\n");
			Menu_MiscWeapon();
		}

		if (inp == 3) {
			if (!(self->weapons_carried & PR_WEAP_AIRF) && !(self->cutf_items & PR_CUTF_AIRFISTUPGRADE))
			{
				sprint(self,PR_PRINT_HIGH,"You need to buy an airfist to use the upgrade!\n");
				CuTFMenuSound(PR_MENUSOUND_WRONG);
			}
			else
			{
				BuyCuTF(PR_COST_AIRFISTUPGRADE,PR_CUTF_AIRFISTUPGRADE);
				Menu_MiscWeapon();
			}
		}

		if (inp == 4) {

			BuyWeapon(PR_COST_DAEDALUS,PR_WEAP_DAEDALUS);
			Menu_MiscWeapon();
		}
		if (inp == 5) {

			BuyWeapon(PR_COST_LASERCANNON,PR_WEAP_LASERCANNON);
			Menu_MiscWeapon();

		}
		if (inp == 6) {
			BuyCuTF(PR_COST_HOLO,PR_CUTF_HOLO);
			Menu_MiscWeapon();
		}

		if (inp == 7) {
			BuyCuTF(PR_COST_CLIPEXTEND,PR_CUTF_CLIPEXTEND);
			Menu_MiscWeapon();
		}

		if (inp == 8) {
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
			self->current_menu = PR_MENU_SECONDARY_WEAPON;
			Menu_SecondaryWeapon();

		}
		if (inp == 9)
		{
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
			self->current_menu = PR_MENU_TOOLS;
			Menu_ToolWeapon();
		}
		if (inp == 10)
		{
			if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
				DropFromCustomClassGen();
			else
				PrintNotEnoughMoney(0);
		}

		self->impulse = 0;
	}
}


// ====================================================================
//	Tools - Gizmo
// ====================================================================
void Menu_ToolWeapon()
{
	string temp;
	temp = ftos(self->money);

	if (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY)
		temp = colstr(temp,PR_COLSTR_RED);

	string l1,l2,l3,l4, str;

	MenuResetCount();

	l1 = ConstructMenuItemStr(1,PR_STR_ZEROGRAVITY,PR_COST_ZEROGRAVITY,self->weapons_carried,PR_WEAP_ZEROGRAVITY);
	str = "\n";
	l1 = strcat(l1,str);

	l2 = l3 = "\n\n";

	l4 = "\n";
	l4 = strcat(l4,S_("^8.. ^bPrev^b    \n^9.. ^bNext^b    \n^0.. ^bDone^b!   \n"));

	centerprint(self, S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 Tools              \x11\x9C\x8B\n\n"), l1, l2, l3, l4, temp," dollars left\n");
}

void Menu_ToolWeapon_Input(float inp)
{
	if (inp <= 10 && inp >= 1)
	{

		if (inp == 1) {
#ifndef PR_NO_ZEROGRAVGUN
			BuyWeapon(PR_COST_ZEROGRAVITY, PR_WEAP_ZEROGRAVITY);
			Menu_ToolWeapon ();
#else
			sprint( self, PR_PRINT_HIGH, "The Zero-Gravity Gun has been disabled in this compilation\n" );
			CuTFMenuSound( PR_MENUSOUND_WRONG );
#endif
		}

		if (inp == 8) {
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
			self->current_menu = PR_MENU_MISC_WEAPON;
			Menu_MiscWeapon();

		}
		if (inp == 9)
		{
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
			self->current_menu = PR_MENU_LEGS;
			Menu_Legs();
		}
		if (inp == 10)
		{
			if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
				DropFromCustomClassGen();
			else
				PrintNotEnoughMoney(0);
		}

		self->impulse = 0;
	}
}

/*
650  9000
420  6000
330  3250
310  2500
290  1250
270  500
250  0
*/

// =============================
//	Prints Menu of Legs for Sale
// =============================
void Menu_Legs()
{
	string temp;
	temp = ftos(self->money);

	if (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY)
		temp = colstr(temp,PR_COLSTR_RED);

	string msg_custom;         // PZ: added
	string l1,l2,l3,l4,str;

	MenuResetCount();

	l1 = ConstructMenuSpeedStr(1,PR_STR_SPEED1,PR_COST_SPEED1);
	str = ConstructMenuSpeedStr(2,PR_STR_SPEED2,PR_COST_SPEED2);
	l1 = strcat(l1,str);

	l2 = ConstructMenuSpeedStr(3,PR_STR_SPEED3,PR_COST_SPEED3);
	str = ConstructMenuSpeedStr(4,PR_STR_SPEED4,PR_COST_SPEED4);
	l2 = strcat(l2,str);

	l3 = ConstructMenuSpeedStr(5,PR_STR_SPEED5,PR_COST_SPEED5);
	str = ConstructMenuSpeedStr(6,PR_STR_SPEED6,PR_COST_SPEED6);
	l3 = strcat(l3,str);

	l4 = ConstructMenuSpeedStr(7,PR_STR_SPEED7,0);
	l4 = strcat(l4,S_("^8.. ^bPrev^b    \n^9.. ^bNext^b    \n^0.. ^bDone^b!   \n"));

	msg_custom = " dollars left\n\n"; // PZ: added

	// PZ: If we are editing, and we couldn't match the player's previous leg speed to one of the custom options, we removed the added leg speed,
	//     and, here, we tell the player. `.worldtype` was the player's speed when entering the custom menu.
	float legs;
	legs = GetPreviousLegs();
	if (self->done_custom & PR_CUSTOM_EDITING /*&& self.worldtype > 0 && self.worldtype != self.custom_speed*/ && (legs == 0 || ((legs - floor(legs)) != 0)))
	{
		msg_custom = strcat(msg_custom, "Previous leg speed did not match a\ncustom speed option. Removed.\n");
		if      (legs == 0.5)
			msg_custom = strcat(msg_custom, S_("Legs were faster than option ^1.\n"));
		else if (legs == 1.5)
			msg_custom = strcat(msg_custom, S_("Legs were between options ^1 and ^2.\n"));
		else if (legs == 2.5)
			msg_custom = strcat(msg_custom, S_("Legs were between options ^2 and ^3.\n"));
		else if (legs == 3.5)
			msg_custom = strcat(msg_custom, S_("Legs were between options ^3 and ^4.\n"));
		else if (legs == 4.5)
			msg_custom = strcat(msg_custom, S_("Legs were between options ^4 and ^5.\n"));
		else if (legs == 5.5)
			msg_custom = strcat(msg_custom, S_("Legs were between options ^5 and ^6.\n"));
		else if (legs == 6.5)
			msg_custom = strcat(msg_custom, S_("Legs were between options ^6 and ^7.\n"));
		else
			msg_custom = strcat(msg_custom, S_("Legs were slower than option ^7.\n"));
	}

	//centerprint(self, "\{156}\{139}\{144} C\bu\bs\bt\bo\bm\bi\bz\be \bYourself\b \{17}\{139}\{156}\n\{139}\{156}\{16} Leg Enhancements   \{17}\{156}\{139}\n\n", l1,l2,l3,l4, temp," dollars left\n");
	centerprint(self, S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 Leg Enhancements   \x11\x9C\x8B\n\n"), l1,l2,l3,l4, temp, msg_custom); // PZ: replaced above
}

void Menu_Legs_Input(float inp)
{

	//First, prevent them from getting high speed while upgraded
	if (inp < 3 && self->done_custom & PR_CUSTOM_SELLING)
	{
		sprint(self,PR_PRINT_HIGH,"The upgraded CustomTF soldier is a fighting machine. He doesn't have access to fast legs\n");
		CuTFMenuSound(PR_MENUSOUND_WRONG);
		self->impulse = 0;
		return;
	}

	//Second, sell off our current legs
	if (inp < 8)
	{
		if (self->custom_speed == PR_CUSTOM_SPEED1) { //400 Original 650
			PrintRefund(PR_COST_SPEED1);
			self->custom_speed = PR_CUSTOM_SPEED7;
			if (self->impulse == 1) {self->impulse = 0; Menu_Legs(); return;}
		}
		else if (self->custom_speed == PR_CUSTOM_SPEED2) { //360 Original 420
			PrintRefund(PR_COST_SPEED2);
			self->custom_speed = PR_CUSTOM_SPEED7;
			if (self->impulse == 2) {self->impulse = 0; Menu_Legs(); return;}
		}
		else if (self->custom_speed == PR_CUSTOM_SPEED3) { // Original 330
			PrintRefund(PR_COST_SPEED3);
			self->custom_speed = PR_CUSTOM_SPEED7;
			if (self->impulse == 3) {self->impulse = 0; Menu_Legs(); return;}
		}
		else if (self->custom_speed == PR_CUSTOM_SPEED4) { // Original 310
			PrintRefund(PR_COST_SPEED4);
			self->custom_speed = PR_CUSTOM_SPEED7;
			if (self->impulse == 4) {self->impulse = 0; Menu_Legs(); return;}
		}
		else if (self->custom_speed == PR_CUSTOM_SPEED5) {
			PrintRefund(PR_COST_SPEED5);
			self->custom_speed = PR_CUSTOM_SPEED7;
			if (self->impulse == 5) {self->impulse = 0; Menu_Legs(); return;}
		}
		else if (self->custom_speed == PR_CUSTOM_SPEED6) {
			PrintRefund(PR_COST_SPEED6);
			self->custom_speed = PR_CUSTOM_SPEED7;
			if (self->impulse == 6) {self->impulse = 0; Menu_Legs(); return;}
		}
	}

	// PZ NOTE: buy legs
	if (inp == 1) {
		if (self->money >= PR_COST_SPEED1) {
			self->money = self->money - PR_COST_SPEED1;
			self->custom_speed = PR_CUSTOM_SPEED1; // Original 650
			PrintMoney();
			Menu_Legs();
		}
		else
			PrintNotEnoughMoney(PR_COST_SPEED1);
	}
	if (inp == 2) {
		if (self->money >= PR_COST_SPEED2) {
			self->money = self->money - PR_COST_SPEED2;
			self->custom_speed = PR_CUSTOM_SPEED2; // Original 420
			PrintMoney();
			Menu_Legs();
		}
		else
			PrintNotEnoughMoney(PR_COST_SPEED2);
	}
	if (inp == 3) {
		if (self->money >= PR_COST_SPEED3) {
			self->money = self->money - PR_COST_SPEED3;
			self->custom_speed = PR_CUSTOM_SPEED3;
			PrintMoney();
			Menu_Legs();
		}
		else
			PrintNotEnoughMoney(PR_COST_SPEED3);
	}
	if (inp == 4) {
		if (self->money >= PR_COST_SPEED4) {
			self->money = self->money - PR_COST_SPEED4;
			self->custom_speed = PR_CUSTOM_SPEED4;
			PrintMoney();
			Menu_Legs();
		}
		else
			PrintNotEnoughMoney(PR_COST_SPEED4);
	}
	if (inp == 5) {
		if (self->money >= PR_COST_SPEED5) {
			self->money = self->money - PR_COST_SPEED5;
			self->custom_speed = PR_CUSTOM_SPEED5;
			PrintMoney();
			Menu_Legs();
		}
		else
			PrintNotEnoughMoney(PR_COST_SPEED5);
	}
	if (inp == 6) {
		if (self->money >= PR_COST_SPEED6) {
			self->money = self->money - PR_COST_SPEED6;
			self->custom_speed = PR_CUSTOM_SPEED6;
			PrintMoney();
			Menu_Legs();
		}
		else
			PrintNotEnoughMoney(PR_COST_SPEED6);
	}

	if (inp == 7) {
		/*if (self.money >= #PRICE) {
			self.money = self.money - #PRICE;*/
			self->custom_speed = PR_CUSTOM_SPEED7;
			PrintMoney();
			Menu_Legs();
		/*}
		else
			PrintNotEnoughMoney(#PRICE);*/
	}
	if (inp == 8) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_TOOLS;
		Menu_ToolWeapon();
	}
	if (inp == 9)
	{
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_HEALTH;
		Menu_Health();
	}
	if (inp == 10) {
		if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
			DropFromCustomClassGen();
		else
			PrintNotEnoughMoney(0);
	}

	self->impulse = 0;
}

// ==========================================
//	Prints Menu of Armor and Health Available
// ==========================================

#ifdef PR_OLD_HP_SYSTEM     // PZ: Oops. I sat here and cleaned up these functions a little, and they aren't even being used.
void Menu_Health()
{
	string temp;
	temp = ftos(self->money);
	#ifdef PR_MONEY_RED_ONCUSTOMIZE
	temp = colstr(temp,PR_COLSTR_RED);
	#endif
	string l1,l2,l3,l4;

	MenuResetCount();

	l1 = eqstr(self->maxarmor, 75, 100,
	           S_("  ^1.. 75 Green armor  ^b-^b  200\n  ^2.. 100 Yellow      ^b-^b  700\n"),
	           S_("* ^1.. 75 Green armor  ^b-^b  200\n  ^2.. 100 Yellow      ^b-^b  700\n"),
	           S_("  ^1.. 75 Green armor  ^b-^b  200\n* ^2.. 100 Yellow      ^b-^b  700\n"));

	l2 = eqstr(self->maxarmor, 150, 200,
	           S_("  ^3.. 150 Red         ^b-^b 1400\n  ^4.. 200 Red         ^b-^b 2400\n"), // original 2200
	           S_("* ^3.. 150 Red         ^b-^b 1400\n  ^4.. 200 Red         ^b-^b 2400\n"),
	           S_("  ^3.. 150 Red         ^b-^b 1400\n* ^4.. 200 Red         ^b-^b 2400\n"));

	l3 = S_("  ^5.. 300 Red         ^b-^b 4600\n"); // 4200 Original 3600

	if (self->maxarmor == 300 )
		l3 = S_("* ^5.. 300 Red         ^b-^b 4600\n");

	l4 = S_("  ^6.. +50 Health      ^b-^b 1000\n  ^7.. +5 Health       ^b-^b  100\n^8.. ^bPrev^b    \n^9.. ^bNext^b    \n^0.. ^bDone^b!   \n");

	centerprint(self, S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 Armor and Health   \x11\x9C\x8B\n\n"), l1,l2,l3,l4, temp," dollars left\n");
}

void Menu_Health_Input(float inp)
{
	//Fix multiple armor buy bug
	//WW: Modded to allow to sell armor back
	if (inp >= 1 && inp <= 5 && self->maxarmor > 0)
	{
		if (self->maxarmor == 75)
		{
			if (inp == 1) {
				inp = 0;
				PrintRefund(200);
				Menu_Health();
			}
			else
				self->money = self->money + 200;
		}
		else if (self->maxarmor == 100)
		{
			if (inp == 2) {
				inp = 0;
				PrintRefund(700);
				Menu_Health();
			}
			else
				self->money = self->money + 700;
		}
		else if (self->maxarmor == 150)
		{
			if (inp == 3) {
				inp = 0;
				PrintRefund(1400);
				Menu_Health();
			}
			else
				self->money = self->money + 1400;
		}
		else if (self->maxarmor == 200)
		{
			if (inp == 4) {
				inp = 0;
				PrintRefund(2400); // Original 2200
				Menu_Health();
			}
			else
				self->money = self->money + 2400;
		}
		else if (self->maxarmor == 300)
		{
			if (inp == 5) {
				inp = 0;
				PrintRefund(4600); //4200 Original 3600
				Menu_Health();
			}
			else
				self->money = self->money + 4600;

		}
		self->armor_allowed = self->armorvalue = self->maxarmor = 0;
		Menu_Health(); // FIXME: optimize (its actually sent twice)
	}
#define PR_PRICE 200
	if (inp == 1)
	{
		if (self->money >= PR_PRICE)
		{
			self->money = self->money - PR_PRICE;
			self->maxarmor = 75;
			self->armorvalue = self->maxarmor;
			self->armortype = 0.3;	// Green
			self->armor_allowed = 0.3; //Green max
			PrintMoney();
			Menu_Health();
		}
		else
			PrintNotEnoughMoney(PR_PRICE);
	}
#define PR_PRICE 700
	if (inp == 2)
	{
		if (self->money >= PR_PRICE)
		{
			self->money = self->money - PR_PRICE;
			self->maxarmor = 100;
			self->armorvalue = self->maxarmor;
			self->armortype = 0.6;	// Yellow
			self->armor_allowed = 0.6; //Yellow max
			PrintMoney();
			Menu_Health();
		}
		else
			PrintNotEnoughMoney(PR_PRICE);
	}
#define PR_PRICE 1400
	if (inp == 3)
	{
		if (self->money >= PR_PRICE)
		{
			self->money = self->money - PR_PRICE;
			self->maxarmor = 150;
			self->armorvalue = self->maxarmor;
			self->armortype = 0.8; //Red armor
			self->armor_allowed = 0.8; //Max
			PrintMoney();
			Menu_Health();
		}
		else
			PrintNotEnoughMoney(PR_PRICE);
	}
#define PR_PRICE 2400 // Original 2200
	if (inp == 4)
	{
		if (self->money >= PR_PRICE)
		{
			self->money = self->money - PR_PRICE;
			self->maxarmor = 200;
			self->armorvalue = self->maxarmor;
			self->armortype = 0.8; //Red armor
			self->armor_allowed = 0.8; //Red max
			PrintMoney();
			Menu_Health();
		}
		else
			PrintNotEnoughMoney(PR_PRICE);
	}
#define PR_PRICE 4600 //4200 Original 3600
	if (inp == 5)
	{
		if (self->money >= PR_PRICE)
		{
			self->money = self->money - PR_PRICE;
			self->maxarmor = 300;
			self->armorvalue = self->maxarmor;
			self->armortype = 0.8; //Red armor (used to be 0.9)
			self->armor_allowed = 0.8; //Red max
			PrintMoney();
			W_SetCurrentAmmo ();
			Menu_Health();
		}
		else
			PrintNotEnoughMoney(PR_PRICE);
	}
#define PR_PRICE 1000
	if (inp == 6)
	{
		if (self->money >= PR_PRICE)
		{
			if (self->max_health + 50 > PR_MAX_PLAYER_HP)
			{
				sprint(self,PR_PRINT_HIGH,"You reached maximum health allowed!\n");
				CuTFMenuSound(PR_MENUSOUND_WRONG);

				self->money = self->money + ((self->max_health - 50) / 5) * 100;
				self->max_health = 50;
				self->health = self->max_health;

				self->impulse = 0;
				return;
			}

			self->money = self->money - PR_PRICE;
			self->max_health = self->max_health + 50;
			self->health = self->max_health;
			PrintMoney();
			//Menu_Health();
		}
		else
		{
			// WW: Modded to allow to sell health back
			//	   Buying #6 with <#7 money resets hp
			if (self->money < 100)
			{
				self->money = self->money + ((self->max_health - 50) / 5) * 100;
				self->max_health = 50;
				self->health = self->max_health;
				PrintMoney();
				//Menu_Health();
			}
			else
				PrintNotEnoughMoney(PR_PRICE);
		}
	}
#define PR_PRICE 100
	if (inp == 7)
	{
		if (self->money >= PR_PRICE)
		{

			if (self->max_health + 5 > PR_MAX_PLAYER_HP)
			{
				sprint(self,PR_PRINT_HIGH,"You reached maximum health allowed!\n");
				CuTFMenuSound(PR_MENUSOUND_WRONG);
				self->impulse = 0;
				return;
			}

			self->money = self->money - PR_PRICE;
			self->max_health = self->max_health + 5;
			self->health = self->max_health;
			PrintMoney();
			//Menu_Health();
		}
		else
			PrintNotEnoughMoney(PR_PRICE);
	}
	if (inp == 8)
	{
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_LEGS;
		Menu_Legs();
	}
	if (inp == 9) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_ARMOR;
		Menu_Armor();
	}
	if (inp == 10) {
		if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
			DropFromCustomClassGen();
		else
			PrintNotEnoughMoney(0);
	}

	self->impulse = 0;
}

#else

// PZ NOTE: This one is being used.
void Menu_Health()
{
	string temp;
	temp = ftos(self->money);

	if (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY)
		temp = colstr(temp,PR_COLSTR_RED);

	string l1,l2,l3,l4,str;
	string msg_custom;         // PZ: added

	MenuResetCount();

	l1 = ConstructMenuHPStr(1,PR_STR_HP1,PR_COST_HP1);
	str = ConstructMenuHPStr(2,PR_STR_HP2,PR_COST_HP2);
	l1 = strcat(l1,str);

	l2 = ConstructMenuHPStr(3,PR_STR_HP3,PR_COST_HP3);
	str = ConstructMenuHPStr(4,PR_STR_HP4,PR_COST_HP4);
	l2 = strcat(l2,str);

	l3 = ConstructMenuHPStr(5,PR_STR_HP5,PR_COST_HP5);
	str = ConstructMenuHPStr(6,PR_STR_HP6,PR_COST_HP6);
	l3 = strcat(l3,str);

	l4 = ConstructMenuHPStr(7,PR_STR_HP7,PR_COST_HP7);
	l4 = strcat(l4,S_("^8.. ^bPrev^b    \n^9.. ^bNext^b    \n^0.. ^bDone^b!   \n"));

	msg_custom = " dollars left\n\n"; // PZ: added

	// PZ: If we are editing, and we couldn't match the player's previous health and armor to one of the custom options, the health and armor selections
	//     are removed, and, here, we tell the player about it.
	float selection;
	selection = GetCurrentHPItem();
	if (self->done_custom & PR_CUSTOM_EDITING && selection == 0 && (self->maxarmor > 0 || self->max_health > 50))
	{
		string str;
		msg_custom = strcat(msg_custom, "Previous armor and health did not\nmatch a custom option. Removed.\nPrevious was: ");
		//if (self.maxarmor == #CUSTOM_ARMOR_AMOUNT_1 && self.max_health == #CUSTOM_HEALTH_1 && self.armor_allowed = #CUSTOM_ARMOR_TYPE_1)
		str = ftos(self->maxarmor);    msg_custom = strcat(msg_custom, str);  msg_custom = strcat(msg_custom, S_("^b/^b"));
		str = ftos(self->max_health);  msg_custom = strcat(msg_custom, str);  msg_custom = strcat(msg_custom, " ");
		if      (isApproxEqual(self->armor_allowed, PR_GREEN_ARMOR, 0.05))  msg_custom = strcat(msg_custom, "Green");
		else if (isApproxEqual(self->armor_allowed, PR_YELLOW_ARMOR, 0.05)) msg_custom = strcat(msg_custom, "Yellow");
		else if (isApproxEqual(self->armor_allowed, PR_RED_ARMOR, 0.05))    msg_custom = strcat(msg_custom, "Red");
		msg_custom = strcat(msg_custom, "\n");
	}

	//centerprint(self, "\{156}\{139}\{144} C\bu\bs\bt\bo\bm\bi\bz\be \bYourself\b \{17}\{139}\{156}\n\{139}\{156}\{16} Armor and Health   \{17}\{156}\{139}\n\n", l1,l2,l3,l4, temp," dollars left\n");
	centerprint(self, S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 Armor and Health   \x11\x9C\x8B\n\n"), l1,l2,l3,l4, temp, msg_custom); // PZ: replaced above
}

float GetHPCost(float numitem)
{
	if      (numitem == 1)
		return PR_COST_HP1;
	else if (numitem == 2)
		return PR_COST_HP2;
	else if (numitem == 3)
		return PR_COST_HP3;
	else if (numitem == 4)
		return PR_COST_HP4;
	else if (numitem == 5)
		return PR_COST_HP5;
	else if (numitem == 6)
		return PR_COST_HP6;
	else if (numitem == 7)
		return PR_COST_HP7;

	return 0;
}

// PZ NOTE: This one is being used.
void Menu_Health_Input(float inp)
{
	if (inp == 8)
	{
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_LEGS;
		Menu_Legs();
		self->impulse = 0;
		return;
	}
	else if (inp == 9) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_ARMOR;
		Menu_Armor();
		self->impulse = 0;
		return;
	}
	else if (inp == 10) {
		if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
			DropFromCustomClassGen();
		else
			PrintNotEnoughMoney(0);
		self->impulse = 0;
		return;
	}

	float current;
	float tosell, tobuy;

	// PZ: We have a function that does the below (now, commented out) code. So, let's use it.
	//     GetCurrentHPItem() also now checks for everything (armor, health, and armor type).
	current = GetCurrentHPItem();
	// PZ NOTE: previous code
	/*if (self.maxarmor == 50)
	{
		if (self.health == 75)
			current = 1;
		else
			current = 2;
	}
	else if (self.maxarmor == 100)
		current = 3;
	else if (self.maxarmor == 120)
		current = 4;
	else if (self.maxarmor == 150)
		current = 5;
	else if (self.maxarmor == 200)
		current = 6;
	else if (self.maxarmor == 300)
		current = 7;
	else
		current = 0;                  // Gizmo - only set to zero if all others fail*/

	tosell = GetHPCost(current);
	tobuy = GetHPCost(inp);
	if (self->money < tobuy - tosell)
	{
		PrintNotEnoughMoney(tobuy - tosell);
		self->impulse = 0;
		return;
	}

	if (current == inp || current != 0) // sell back?
	{
		self->max_health = 50;
		self->health = self->max_health;
		self->maxarmor = 0;
		self->armorvalue = 0;
		self->armortype = 0;
		self->armor_allowed = 0;

		PrintRefund(tosell);
		UpdateArmorItem(self);

		if (current == inp)
		{
			Menu_Health();
			self->impulse = 0;
			return;
		}
	}

	// PZ: Replaced hard-coded values with defines to make editing the code (and these values) easier.
	if (inp == 1)
	{
		self->max_health = PR_CUSTOM_HEALTH_1;
		self->health = self->max_health;
		self->maxarmor = PR_CUSTOM_ARMOR_AMOUNT_1;
		self->armorvalue = self->maxarmor;
		self->armortype = PR_CUSTOM_ARMOR_TYPE_1;     // Green
		self->armor_allowed = PR_CUSTOM_ARMOR_TYPE_1; // Green max
		UpdateArmorItem(self);
	}
	else if (inp == 2)
	{
		self->max_health = PR_CUSTOM_HEALTH_2;
		self->health = self->max_health;
		self->maxarmor = PR_CUSTOM_ARMOR_AMOUNT_2;
		self->armorvalue = self->maxarmor;
		self->armortype = PR_CUSTOM_ARMOR_TYPE_2;     // Green
		self->armor_allowed = PR_CUSTOM_ARMOR_TYPE_2; // Green max
		UpdateArmorItem(self);
	}
	else if (inp == 3)
	{
		self->max_health = PR_CUSTOM_HEALTH_3;
		self->health = self->max_health;
		self->maxarmor = PR_CUSTOM_ARMOR_AMOUNT_3;
		self->armorvalue = self->maxarmor;
		self->armortype = PR_CUSTOM_ARMOR_TYPE_3;
		self->armor_allowed = PR_CUSTOM_ARMOR_TYPE_3;
		UpdateArmorItem(self);
	}
	else if (inp == 4)
	{
		self->max_health = PR_CUSTOM_HEALTH_4;
		self->health = self->max_health;
		self->maxarmor = PR_CUSTOM_ARMOR_AMOUNT_4;
		self->armorvalue = self->maxarmor;
		self->armortype = PR_CUSTOM_ARMOR_TYPE_4;
		self->armor_allowed = PR_CUSTOM_ARMOR_TYPE_4;
		UpdateArmorItem(self);
	}
	else if (inp == 5)
	{
		self->max_health = PR_CUSTOM_HEALTH_5;
		self->health = self->max_health;
		self->maxarmor = PR_CUSTOM_ARMOR_AMOUNT_5;
		self->armorvalue = self->maxarmor;
		self->armortype = PR_CUSTOM_ARMOR_TYPE_5;
		self->armor_allowed = PR_CUSTOM_ARMOR_TYPE_5;
		UpdateArmorItem(self);
	}
	else if (inp == 6)
	{
		self->max_health = PR_CUSTOM_HEALTH_6;
		self->health = self->max_health;
		self->maxarmor = PR_CUSTOM_ARMOR_AMOUNT_6;
		self->armorvalue = self->maxarmor;
		self->armortype = PR_CUSTOM_ARMOR_TYPE_6;
		self->armor_allowed = PR_CUSTOM_ARMOR_TYPE_6;
		UpdateArmorItem(self);
	}
	else if (inp == 7)
	{
		self->max_health = PR_CUSTOM_HEALTH_7;
		self->health = self->max_health;
		self->maxarmor = PR_CUSTOM_ARMOR_AMOUNT_7;
		self->armorvalue = self->maxarmor;
		self->armortype = PR_CUSTOM_ARMOR_TYPE_7;
		self->armor_allowed = PR_CUSTOM_ARMOR_TYPE_7;
		UpdateArmorItem(self);
	}

	self->money = self->money - tobuy;

	PrintMoney();
	Menu_Health();

	self->impulse = 0;
}
#endif

// ==========================================
//	Prints Menu of Special Armor and Ammo
// ==========================================
void Menu_Armor()
{
	string temp;
	temp = ftos(self->money);
	if (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY)
		temp = colstr(temp,PR_COLSTR_RED);
	string l1,l2,l3,l4,str;

	MenuResetCount();

	l1 = ConstructMenuItemStr(1,PR_STR_CERAMIC,PR_COST_CERAMIC,self->tf_items,PR_NIT_CERAMIC);
	str = ConstructMenuItemStr(2,PR_STR_GET,PR_COST_GEL,self->tf_items,PR_NIT_GEL);
	l1 = strcat(l1,str);

	l2 = ConstructMenuItemStr(3,PR_STR_ASBESTOS,PR_COST_ASBESTOS,self->tf_items,PR_NIT_ASBESTOS);
	str = ConstructMenuItemStr(4,PR_STR_KEVLAR,PR_COST_KEVLAR,self->tf_items,PR_NIT_KEVLAR);
	l2 = strcat(l2,str);

	l3 = ConstructMenuItemStr(5,PR_STR_BLAST,PR_COST_BLAST,self->tf_items,PR_NIT_BLAST);
	str = ConstructMenuItemStr(6,PR_STR_AMMOBACKPACK,PR_COST_AMMOBACKPACK,self->tf_items,PR_NIT_AMMO_BACKPACK);
	l3 = strcat(l3,str);

	l4 = ConstructMenuItemStr(7,PR_STR_AMMOBANDOLIER,PR_COST_AMMOBANDOLIER,self->tf_items,PR_NIT_AMMO_BANDOLIER);
	l4 = strcat(l4,S_("^8.. ^bPrev^b    \n^9.. ^bNext^b    \n^0.. ^bDone^b!   \n"));

	centerprint(self, S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 Special Armor^b/^bAmmo \x11\x9C\x8B\n\n"), l1,l2,l3,l4, temp," dollars left\n");
}

void Menu_Armor_Input(float inp)
{
	if (inp == 1) {
		BuyItem(PR_COST_CERAMIC,PR_NIT_CERAMIC);
		Menu_Armor();
	}
	if (inp == 2) {
		BuyItem(PR_COST_GEL, PR_NIT_GEL);
		Menu_Armor();
	}
	if (inp == 3) {
		BuyItem(PR_COST_ASBESTOS,PR_NIT_ASBESTOS);
		Menu_Armor();
	}
	if (inp == 4) {
		BuyItem(PR_COST_KEVLAR,PR_NIT_KEVLAR);
		Menu_Armor();
	}
	if (inp == 5) {
		BuyItem(PR_COST_BLAST,PR_NIT_BLAST);
		Menu_Armor();
	}
	if (inp == 6) {
		BuyItem(PR_COST_AMMOBACKPACK,PR_NIT_AMMO_BACKPACK);
		Menu_Armor();
	}
	if (inp == 7) {
#ifdef PR_COOP_MODE_ENHANCED
		if ( !deathmatch ) {
			sprint( self, PR_PRINT_HIGH, "Ammo Bandolier is free in coop.\n" );
			CuTFMenuSound( PR_MENUSOUND_WRONG );
		} else {
			BuyItem( PR_COST_AMMOBANDOLIER, PR_NIT_AMMO_BANDOLIER );
			Menu_Armor();
		}
#else
		BuyItem(PR_COST_AMMOBANDOLIER,PR_NIT_AMMO_BANDOLIER);
		Menu_Armor();
#endif
	}
	if (inp == 8)
	{
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_HEALTH;
		Menu_Health();
	}
	if (inp == 9) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_SPECIAL;
		Menu_Special();
	}
	if (inp == 10) {
		if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
			DropFromCustomClassGen();
		else
			PrintNotEnoughMoney(0);
	}

	self->impulse = 0;
}

// ==================================
//	Prints Menu of Specials Available
// ==================================
void Menu_Special()
{
	string temp;
	temp = ftos(self->money);
	if (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY)
		temp = colstr(temp,PR_COLSTR_RED);
	string l1,l2,l3,l4,str;

	MenuResetCount();

	l1 = ConstructMenuItemStr(1,PR_STR_SCANNER,PR_COST_SCANNER,self->tf_items,PR_NIT_SCANNER);
	str = ConstructMenuItemStr(2,PR_STR_AUTOSCANNER,PR_COST_AUTOSCANNER,self->tf_items,PR_NIT_AUTOSCANNER);
	l1 = strcat(l1,str);

	l2 = ConstructMenuItemStr(3,PR_STR_DETPACK,PR_COST_DETPACK,self->cutf_items,PR_CUTF_DETPACK);
	str = ConstructMenuItemStr(4,PR_STR_MEDIKIT,PR_COST_MEDIKIT,self->weapons_carried,PR_WEAP_MEDIKIT);
	l2 = strcat(l2,str);

	l3 = ConstructMenuItemStr(5,PR_STR_SPYKIT,PR_COST_SPYKIT,self->cutf_items,PR_CUTF_SPY_KIT);
	str = ConstructMenuItemStr(6,PR_STR_SCUBA,PR_COST_SCUBA,self->tf_items,PR_NIT_SCUBA);
	l3 = strcat(l3,str);

	l4 = ConstructMenuItemStr(7,PR_STR_HOOK,PR_COST_HOOK,self->weapons_carried,PR_WEAP_HOOK);
	l4 = strcat(l4,S_("^8.. ^bPrev^b    \n^9.. ^bNext^b    \n^0.. ^bDone^b!   \n"));

	centerprint(self, S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 Special Items      \x11\x9C\x8B\n\n"),l1,l2,l3,l4, temp," dollars left\n");
}

void Menu_Special_Input(float inp)
{
	string st2;

//Scanner

	//st = infokey(world, "no_grapple");
	st2 = infokey(world, "no_spam");
	if (inp == 1) {
		if (self->tf_items & PR_NIT_SCANNER)
		{
			//Clean up auto-medic (sync this with Custom.qc)
			if (self->tf_items & PR_NIT_AUTOSCANNER)
				self->money = self->money + PR_COST_AUTOSCANNER; //Sync this with price below
			self->tf_items = self->tf_items - (self->tf_items & PR_NIT_AUTOSCANNER);

			PrintRefund(PR_COST_SCANNER);
			self->tf_items = self->tf_items - PR_NIT_SCANNER;
			self->tf_items_flags = self->tf_items_flags - (self->tf_items & PR_NIT_SCANNER_ENEMY);
			Menu_Special();
		}
		else if (self->money >= PR_COST_SCANNER) {
			self->money = self->money - PR_COST_SCANNER;
			self->tf_items = self->tf_items | PR_NIT_SCANNER;
			self->tf_items_flags = self->tf_items_flags | PR_NIT_SCANNER_ENEMY;
			PrintMoney();
			Menu_Special();
		}
		else
			PrintNotEnoughMoney(PR_COST_SCANNER);
	}
	if (inp == 2) { //Sync this with scanner above
		BuyItem(PR_COST_AUTOSCANNER,PR_NIT_AUTOSCANNER);
		Menu_Special();
	}
//Detpack
	if (inp == 3) {
		if (st2 == "on") {//If admin disable it
			sprint(self,PR_PRINT_HIGH,"The admin has disabled spam devices on this map.\n");
//				  self.impulse=0;
			return;
		}
		if (self->cutf_items & PR_CUTF_DETPACK) //Already have it
		{
			PrintRefund(PR_COST_DETPACK);
			self->cutf_items = self->cutf_items - PR_CUTF_DETPACK;
			self->maxammo_detpack = self->maxammo_detpack - 1;
			Menu_Special();
		}
		else if (self->money >= PR_COST_DETPACK) {
			self->money = self->money - PR_COST_DETPACK;
			self->cutf_items = self->cutf_items | PR_CUTF_DETPACK;
			self->maxammo_detpack = self->maxammo_detpack + 1;
			PrintMoney();
			Menu_Special();
		}
		else
			PrintNotEnoughMoney(PR_COST_DETPACK);
	}
	if (inp == 4) {
		if (self->weapons_carried & PR_WEAP_MEDIKIT)
		{
			PrintRefund(PR_COST_MEDIKIT);
			self->weapons_carried = self->weapons_carried - PR_WEAP_MEDIKIT;
			self->maxammo_medikit = self->maxammo_medikit - 80;
			Menu_Special();
		}
		else if (self->money >= PR_COST_MEDIKIT) {
			self->money = self->money - PR_COST_MEDIKIT;
			self->weapons_carried = self->weapons_carried | PR_WEAP_MEDIKIT;
			self->maxammo_medikit = self->maxammo_medikit + 80;
			PrintMoney();
			Menu_Special();
		}
		else
			PrintNotEnoughMoney(PR_COST_MEDIKIT);
	}
	if (inp == 5) {
		BuyCuTF(PR_COST_SPYKIT,PR_CUTF_SPY_KIT);
		Menu_Special();
	}
	//Self-Contained Underwater Breathing Apparatus
	if (inp == 6) {
		BuyItem(PR_COST_SCUBA,PR_NIT_SCUBA);
		Menu_Special();
	}
	if (inp == 7) {
		if (no_grapple == 1) //If admin disable it
		{
			sprint(self,PR_PRINT_HIGH,"The admin has disabled the grapple on this map.\n");
			self->impulse = 0;
			return;
		}
		else if (allow_hook) //If map allows it...
		{
		   sprint(self,PR_PRINT_HIGH,"You can't sell your grapple on this map!\n");
		   CuTFMenuSound(PR_MENUSOUND_WRONG);
		}
		else
		{
			BuyWeapon(PR_COST_HOOK,PR_WEAP_HOOK);
			Menu_Special();
		}
	}
	if (inp == 8) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_ARMOR;
		Menu_Armor();
	}
	if (inp == 9) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_SPECIAL2;
		Menu_Special2();
	}
	if (inp == 10) {
		if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
			DropFromCustomClassGen();
		else
			PrintNotEnoughMoney(0);
	}

	self->impulse = 0;
}

// ============================================//
// Prints Menu of Specials Available		   //
// ============================================//
void Menu_Special2()
{
	string temp;
	temp = ftos(self->money);
	if (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY)
		temp = colstr(temp,PR_COLSTR_RED);
	string l1,l2,l3,l4,str;

	MenuResetCount();

	l1 = ConstructMenuItemStr(1,PR_STR_JAMMER,PR_COST_JAMMER,self->cutf_items,PR_CUTF_JAMMER);
	str = ConstructMenuItemStr(2,PR_STR_CYBERAUG,PR_COST_CYBERAUG,self->cutf_items,PR_CUTF_CYBERAUG);
	l1 = strcat(l1,str);

	l2 = ConstructMenuItemStr(3,PR_STR_SENSOR,PR_COST_SENSOR,self->cutf_items,PR_CUTF_SENSOR);
	str = ConstructMenuItemStr(4,PR_STR_FIELDGEN,PR_COST_FIELDGEN,self->cutf_items,PR_CUTF_FIELDGEN);
	l2 = strcat(l2,str);

	l3 = ConstructMenuItemStr(5,PR_STR_FULLARMOUR,PR_COST_FULLARMOUR,self->cutf_items,PR_CUTF_FULLARMOUR);
	str = ConstructMenuItemStr(6,PR_STR_TOSSABLEDET,PR_COST_TOSSABLEDET,self->cutf_items,PR_CUTF_TOSSABLEDET);
	l3 = strcat(l3,str);

	l4 = ConstructMenuItemStr(7,PR_STR_DISPENSER,PR_COST_DISPENSER,self->cutf_items,PR_CUTF_DISPENSER);
	l4 = strcat(l4,S_("^8.. ^bPrev^b    \n^9.. ^bNext^b    \n^0.. ^bDone^b!   \n"));

	centerprint(self, S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 More Special Items \x11\x9C\x8B\n\n"),l1,l2,l3,l4, temp," dollars left\n");
}

void Menu_Special2_Input(float inp)
{
	if (inp == 1)//SB Jammer
	{
		BuyCuTF(PR_COST_JAMMER, PR_CUTF_JAMMER);
		Menu_Special2();
	}
	else if (inp == 2)//SB cybernetic augmentation
	{
		if (self->cutf_items & PR_CUTF_CYBERAUG)
		{
			self->maxammo_medikit = self->maxammo_medikit - 80;
			self->maxammo_cells = self->maxammo_cells - 80;
		}
		else
		{
			self->maxammo_medikit = self->maxammo_medikit + 80;
			self->maxammo_cells = self->maxammo_cells + 80;
		}
		BuyCuTF(PR_COST_CYBERAUG,PR_CUTF_CYBERAUG);
		Menu_Special2();
	}
	else if (inp == 3)//SB motion sensor
	{
		BuyCuTF(PR_COST_SENSOR,PR_CUTF_SENSOR);
		Menu_Special2();
	}
	else if (inp == 4)//OfN Field Generator
	{
		BuyCuTF(PR_COST_FIELDGEN,PR_CUTF_FIELDGEN);
		Menu_Special2();
	}
	else if (inp == 5)//SB full armour
	{
#ifdef PR_COOP_MODE_ENHANCED
		if ( !deathmatch ) {
			sprint( self, PR_PRINT_HIGH, "Full armor is free in coop.\n" );
			CuTFMenuSound( PR_MENUSOUND_WRONG );
		} else {
			BuyCuTF( PR_COST_FULLARMOUR, PR_CUTF_FULLARMOUR );
			Menu_Special2();
		}
#else
		BuyCuTF(PR_COST_FULLARMOUR,PR_CUTF_FULLARMOUR);
		Menu_Special2();
#endif
	}
	else if (inp == 6)//SB C4 tossable detpack
	{
		BuyCuTF(PR_COST_TOSSABLEDET,PR_CUTF_TOSSABLEDET);
		sprint(self, PR_PRINT_HIGH, "Type 'throwdet' to throw the detpack.\n");
	}
	else if (inp == 7)
	{
		BuyCuTF(PR_COST_DISPENSER,PR_CUTF_DISPENSER);
		Menu_Special2();
	}
	else if (inp == 8) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_SPECIAL;
		Menu_Special();
	}
	else if (inp == 9) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_GREN1;
		Menu_Gren1();
	}
	else if (inp == 10) {
		if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
			DropFromCustomClassGen();
		else
			PrintNotEnoughMoney(0);
	}

	self->impulse = 0;
}

// ====================================
//	Prints Menu of Grenade-1s Available
// ====================================

void Menu_Gren1()
{
	string temp;
	temp = ftos(self->money);
	if (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY)
		temp = colstr(temp,PR_COLSTR_RED);
	string l1,l2,l3,l4,str;

	MenuResetCount();

	l1 = ConstructMenuGrenStr(1,PR_STR_GR_NORMAL,PR_COST_GR_NORMAL,PR_GR_TYPE_NORMAL);
	str = ConstructMenuGrenStr(2,PR_STR_GR_NAPALM,PR_COST_GR_NAPALM,PR_GR_TYPE_NAPALM);
	l1 = strcat(l1,str);

	l2 = ConstructMenuGrenStr(3,PR_STR_GR_GAS,PR_COST_GR_GAS,PR_GR_TYPE_GAS);
	str = ConstructMenuGrenStr(4,PR_STR_GR_EMP,PR_COST_GR_EMP,PR_GR_TYPE_EMP);
	l2 = strcat(l2,str);

	l3 = ConstructMenuGrenStr(5,PR_STR_GR_FRAG,PR_COST_GR_FRAG,PR_GR_TYPE_FRAG);
	str = ConstructMenuGrenStr(6,PR_STR_GR_NAIL,PR_COST_GR_NAIL,PR_GR_TYPE_NAIL);
	l3 = strcat(l3,str);

	l4 = ConstructMenuGrenStr(7,PR_STR_GR_MIRV,PR_COST_GR_MIRV,PR_GR_TYPE_MIRV);
	l4 = strcat(l4,S_("^8.. ^bPrev^b    \n^9.. ^bNext^b    \n^0.. ^bDone^b!   \n"));

	centerprint(self, S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 Grenades ^b-^b Combat  \x11\x9C\x8B\n\n"),l1,l2,l3,l4, temp," dollars left\n");
}

void Menu_Gren1_Input(float inp)
{
	string st, st2;
	st = infokey(world, "no_gasgren");
	st2 = infokey (world, "no_spam");

	if (inp == 1)
	{
		BuyGren(PR_COST_GR_NORMAL,PR_GR_TYPE_NORMAL);
		Menu_Gren1();
	}
	else if (inp == 2)
	{
		BuyGren(PR_COST_GR_NAPALM,PR_GR_TYPE_FLAME); // Flame was 750
		Menu_Gren1();
	}
	else if (inp == 3)
	{
		if (st == "on")
		{
			sprint(self, PR_PRINT_HIGH, "The admin has disabled gas grens on this map.\n");
			return;
		}
		if (st2 == "on") //If admin disable it
		{
			sprint(self,PR_PRINT_HIGH,"The admin has disabled spam devices on this map.\n");
			return;
		}
		BuyGren(PR_COST_GR_GAS,PR_GR_TYPE_GAS);
		Menu_Gren1();
	}
	else if (inp == 4)
	{
		if (st2 == "on") //If admin disable it
		{
			sprint(self,PR_PRINT_HIGH,"The admin has disabled spam devices on this map.\n");
			return;
		}
		BuyGren(PR_COST_GR_EMP,PR_GR_TYPE_EMP);
		Menu_Gren1();
	}
	else if (inp == 5)
	{
		if (st2 == "on") //If admin disable it
		{
			sprint(self,PR_PRINT_HIGH,"The admin has disabled spam devices on this map.\n");
			return;
		}
		BuyGren(PR_COST_GR_FRAG,PR_GR_TYPE_FRAG);
		Menu_Gren1();
	}
	else if (inp == 6)
	{
		BuyGren(PR_COST_GR_NAIL,PR_GR_TYPE_NAIL);
		Menu_Gren1();
	}
	if (inp == 7)
	{
		if (st2 == "on") //If admin disable it
		{
			sprint(self,PR_PRINT_HIGH,"The admin has disabled spam devices on this map.\n");
				return;
		}
		BuyGren(PR_COST_GR_MIRV,PR_GR_TYPE_MIRV);
		Menu_Gren1();
	}

	if (inp == 8)
	{
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_SPECIAL2;
		Menu_Special2();
	}
	if (inp == 9)
	{
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_GREN2;
		Menu_Gren2();
	}
	else if (inp == 10)
	{
		if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
			DropFromCustomClassGen();
		else
			PrintNotEnoughMoney(0);
	}

	self->impulse = 0;
}

// ====================================
//	Prints Menu of Grenade-2s Available
// ====================================
void Menu_Gren2()
{
	string temp;
	temp = ftos(self->money);
	if (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY)
		temp = colstr(temp,PR_COLSTR_RED);
	string l1,l2,l3,l4,str;

	MenuResetCount();

	l1 = ConstructMenuGrenStr(1,PR_STR_GR_FLARE,PR_COST_GR_FLARE,PR_GR_TYPE_FLARE);
	str = ConstructMenuGrenStr(2,PR_STR_GR_CALTROP,PR_COST_GR_CALTROP,PR_GR_TYPE_CALTROP);
	l1 = strcat(l1,str);

	l2 = ConstructMenuGrenStr(3,PR_STR_GR_CONCUSSION,PR_COST_GR_CONCUSSION,PR_GR_TYPE_CONCUSSION);
	str = ConstructMenuGrenStr(4,PR_STR_GR_PSIONIC,PR_COST_GR_PSIONIC,PR_GR_TYPE_PSIONIC);
	l2 = strcat(l2,str);

	l3 = ConstructMenuGrenStr(5,PR_STR_GR_ANTIGRAV,PR_COST_GR_ANTIGRAV,PR_GR_TYPE_ANTIGRAV);
	str = ConstructMenuGrenStr(6,PR_STR_GR_BIOLOGICAL,PR_COST_GR_BIOLOGICAL,PR_GR_TYPE_BIO);
	l3 = strcat(l3,str);

	l4 = ConstructMenuGrenStr(7,PR_STR_GR_KRAC,PR_COST_GR_KRAC,PR_GR_TYPE_KRAC);
	l4 = strcat(l4,S_("^8.. ^bPrev^b    \n^9.. ^bNext^b    \n^0.. ^bDone^b!   \n"));

	centerprint(self, S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 Grenades ^b-^b Utility \x11\x9C\x8B\n\n"),l1,l2,l3,l4, temp," dollars left\n");
}

void Menu_Gren2_Input(float inp)
{
	string st, st2;
	st = infokey(world, "no_antigrav");
	st2 = infokey(world, "no_spam");

	//WK I should have done all the buy menus like this
	//SB I reckon - this one actually looks nice and makes sense
	if (inp == 1)
	{
		BuyGren(PR_COST_GR_FLARE, PR_GR_TYPE_FLARE);
		Menu_Gren2();
	}
	if (inp == 2)
	{
		if (st2 == "on")
			sprint(self, PR_PRINT_HIGH, "The admin has disabled spam devices on this map.\n");
		else
		{
			BuyGren(PR_COST_GR_CALTROP, PR_GR_TYPE_CALTROP);
			Menu_Gren2();
		}
	}
	if (inp == 3)
	{
		BuyGren(PR_COST_GR_CONCUSSION, PR_GR_TYPE_CONCUSSION);
		Menu_Gren2();
	}
	if (inp == 4)
	{
		#ifdef PR_OLD_FLASH
		BuyGren(1000,PR_GR_TYPE_FLASH);
		#else
		BuyGren(PR_COST_GR_PSIONIC,PR_GR_TYPE_PSIONIC);
		#endif
		Menu_Gren2();
	}
	if (inp == 5)
	{
		if (st == "on")
			sprint(self, PR_PRINT_HIGH, "The admin has disabled anti-grav grens on this map.\n");
		else
		{
			BuyGren(PR_COST_GR_ANTIGRAV, PR_GR_TYPE_ANTIGRAV);
			Menu_Gren2();
		}
	}
	if (inp == 6)
	{
		BuyGren(PR_COST_GR_BIOLOGICAL, PR_GR_TYPE_BIO);
		Menu_Gren2();
	}
	if (inp == 7)
	{
		BuyGren(PR_COST_GR_KRAC, PR_GR_TYPE_KRAC);
		Menu_Gren2();
	}
	else if (inp == 8)
	{
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_GREN1;
		Menu_Gren1();
	}
	else if (inp == 9)
	{
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_ENGINEERING;
		Menu_Engineering();
	}
	if (inp == 10)
	{
		if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
			DropFromCustomClassGen();
		else
			PrintNotEnoughMoney(0);
	}

	self->impulse = 0;
}

// ====================================
//	Prints Menu of Optional Items Here
// ====================================
void Menu_Engineering()
{
	string temp;
	temp = ftos(self->money);
	if (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY)
		temp = colstr(temp,PR_COLSTR_RED);
	string l1,l2,l3,l4,str;

	MenuResetCount();

	l1 = ConstructMenuItemStr(1,PR_STR_SPANNER,PR_COST_SPANNER,self->weapons_carried,PR_WEAP_SPANNER);
	str = ConstructMenuSentryStr(2,PR_STR_SENTRYGUN,PR_COST_SENTRYGUN);
	l1 = strcat(l1,str);

	l2 = ConstructMenuItemStr(3,PR_STR_SENTRYUPGRADE,PR_COST_SENTRYUPGRADE,self->tf_items,PR_NIT_TURRET);
	str = ConstructMenuTeslaStr(4,PR_STR_TESLACOIL,PR_COST_TESLACOIL);
	l2 = strcat(l2,str);

	l3 = ConstructMenuItemStr(5,PR_STR_TESLAUPGRADE,PR_COST_TESLAUPGRADE,self->tf_items,PR_NIT_TESLA_UPGRADE);
	str = ConstructMenuItemStr(6,PR_STR_SECURITYCAMERA,PR_COST_SECURITYCAMERA,self->tf_items,PR_NIT_SECURITY_CAMERA);
	l3 = strcat(l3,str);

	l4 = ConstructMenuItemStr(7,PR_STR_TELEPORTER,PR_COST_TELEPORTER,self->tf_items,PR_NIT_TELEPORTER);
	l4 = strcat(l4,S_("^8.. ^bPrev^b    \n^9.. ^bNext^b    \n^0.. ^bDone^b!   \n"));

	centerprint(self, S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 Engineering Items  \x11\x9C\x8B\n\n"), l1,l2,l3,l4, temp," dollars left\n");
}
void Menu_Engineering_Input(float inp)
{
	if (inp == 1) {
		BuyWeapon(PR_COST_SPANNER,PR_WEAP_SPANNER);
		Menu_Engineering();
	}
	if (inp == 2) {
		if (self->cutf_items & PR_CUTF_SENTRYGUN)
		{
			if (self->cutf_items & PR_CUTF_DOUBLESENTRY)
			{
				BuyCuTF(PR_COST_SENTRYGUN*2,PR_CUTF_SENTRYGUN | PR_CUTF_DOUBLESENTRY); // will sell both
			}
			else
			{
				if (self->money < PR_COST_SENTRYGUN)
					BuyCuTF(PR_COST_SENTRYGUN,PR_CUTF_SENTRYGUN); // will sell it
				else
				{
					if (self->tf_items & PR_NIT_TESLA)
						BuyCuTF(PR_COST_SENTRYGUN,PR_CUTF_SENTRYGUN); // will sell it
					else
						BuyCuTF(PR_COST_SENTRYGUN,PR_CUTF_DOUBLESENTRY);
				}
			}
		}
		else
		{
			if (self->tf_items & PR_NIT_DOUBLETESLA)
			{
				sprint(self,PR_PRINT_HIGH,"You can only carry equipment for 2 offensive machines!\n");
				CuTFMenuSound(PR_MENUSOUND_WRONG);
				self->impulse = 0;
				return;
			}

			BuyCuTF(PR_COST_SENTRYGUN,PR_CUTF_SENTRYGUN);
		}

		Menu_Engineering();
	}
	if (inp == 3) {
		BuyItem(PR_COST_SENTRYUPGRADE,PR_NIT_TURRET);
		Menu_Engineering();
	}
	if (inp == 4) {
		if (self->tf_items & PR_NIT_TESLA)
		{
			if (self->tf_items & PR_NIT_DOUBLETESLA)
			{
				BuyItem(PR_COST_TESLACOIL*2,PR_NIT_TESLA | PR_NIT_DOUBLETESLA); // will sell both
			}
			else
			{
				if (self->money < PR_COST_TESLACOIL)
					BuyItem(PR_COST_TESLACOIL,PR_NIT_TESLA); // will sell it
				else
				{
					if (self->cutf_items & PR_CUTF_SENTRYGUN)
						BuyItem(PR_COST_TESLACOIL,PR_NIT_TESLA); // will sell it
					else
						BuyItem(PR_COST_TESLACOIL,PR_NIT_DOUBLETESLA);
				}
			}
		}
		else
		{
			if (self->cutf_items & PR_CUTF_DOUBLESENTRY)
			{
				sprint(self,PR_PRINT_HIGH,"You can only carry equipment for 2 offensive machines!\n");
				CuTFMenuSound(PR_MENUSOUND_WRONG);
				self->impulse = 0;
				return;
			}

			BuyItem(PR_COST_TESLACOIL,PR_NIT_TESLA);
		}

		Menu_Engineering();
	}
	if (inp == 5) {
		BuyItem(PR_COST_TESLAUPGRADE,PR_NIT_TESLA_UPGRADE);
		Menu_Engineering();
	}
	if (inp == 6) {
		BuyItem(PR_COST_SECURITYCAMERA,PR_NIT_SECURITY_CAMERA);
		Menu_Engineering();
	}
	if (inp == 7) {
		BuyItem(PR_COST_TELEPORTER,PR_NIT_TELEPORTER);
		Menu_Engineering();
	}
	if (inp == 8) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_GREN2;
		Menu_Gren2();
	}
	if (inp == 9) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		if (custom_mode!=1)
		{
			self->current_menu = PR_MENU_PROFESSION;
			Menu_Profession();
		}
		else
		{
			self->current_menu = PR_MENU_PROFICIENCY;
			Menu_Proficiency();
		}
	}
	if (inp == 10) {
		if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
			DropFromCustomClassGen();
		else
			PrintNotEnoughMoney(0);
	}

	self->impulse = 0;
}

// =====================================
//	Prints Menu of Professions Available
// =====================================
void Menu_Profession()
{
	string temp;
	temp = ftos(self->money);
	if (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY)
		temp = colstr(temp,PR_COLSTR_RED);
	string l1,l2,l3,l4,str;

	MenuResetCount();

	l1 = ConstructMenuJobStr(1,PR_STR_THIEF,PR_JOB_THIEF,PR_COST_THIEF,0,0,"","");
	str = ConstructMenuJobStr(2,PR_STR_RUNNER,PR_JOB_RUNNER,PR_COST_RUNNER,0,0,"","");
	l1 = strcat(l1,str);

	l2 = ConstructMenuJobStr(3,PR_STR_WARLOCK,PR_JOB_WARLOCK,PR_COST_WARLOCK,PR_COST_WARLOCK_EXTRA1,PR_COST_WARLOCK_EXTRA2,PR_STR_WARLOCK_EXTRA1,PR_STR_WARLOCK_EXTRA2);
	str = ConstructMenuJobStr(4,PR_STR_CHAPLAN,PR_JOB_CHAPLAN,PR_COST_CHAPLAN,0,0,"","");
	l2 = strcat(l2,str);

	l3 = ConstructMenuJobStr(5,PR_STR_BERSERKER,PR_JOB_BERSERKER,PR_COST_BERSERKER,0,0,"","");
	str = ConstructMenuJobStr(6,PR_STR_GUERILLA,PR_JOB_GUERILLA,PR_COST_GUERILLA,PR_COST_GUERILLA_EXTRA1,PR_COST_GUERILLA_EXTRA2,PR_STR_GUERILLA_EXTRA1,PR_STR_GUERILLA_EXTRA2);
	l3 = strcat(l3,str);

	l4 = ConstructMenuJobStr(7,PR_STR_JUDOKA,PR_JOB_JUDOKA,PR_COST_JUDOKA,0,0,"","");
	l4 = strcat(l4,S_("^8.. ^bPrev^b    \n^9.. ^bNext^b    \n^0.. ^bDone^b!   \n"));

	centerprint(self, S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 Skilled Profession \x11\x9C\x8B\n\n"), l1,l2,l3,l4, temp,S_(" dollars left\n\nUse ^bSKILL^b to trigger ability\n"));
}
void Menu_Profession_Input(float inp)
{
	if (inp == 1) {
		BuyJob( PR_COST_THIEF,PR_JOB_THIEF);
		Menu_Profession();
	}
	if (inp == 2) {
		BuyJob(PR_COST_RUNNER,PR_JOB_RUNNER);
		Menu_Profession();
	}
	if (inp == 3) { //Sync this with price in knife
		if (extras_mode == 0)
		{
			if (self->job & PR_JOB_WARLOCK)
				BuyExtra(PR_COST_WARLOCK_EXTRA1,PR_COST_WARLOCK_EXTRA2);
			else
				BuyJob(PR_COST_WARLOCK,PR_JOB_WARLOCK);
		}
		else
			BuyJob(PR_COST_WARLOCK,PR_JOB_WARLOCK);

		Menu_Profession();
	}
	if (inp == 4) {
		BuyJob(PR_COST_CHAPLAN,PR_JOB_CHAPLAN);
		Menu_Profession();
	}
	if (inp == 5) {
		BuyJob(PR_COST_BERSERKER,PR_JOB_BERSERKER);
		Menu_Profession();
	}
	if (inp == 6) {
		if (extras_mode == 0)
		{
			if (self->job & PR_JOB_GUERILLA)
				BuyExtra(PR_COST_GUERILLA_EXTRA1,PR_COST_GUERILLA_EXTRA2);
			else
				BuyJob(PR_COST_GUERILLA,PR_JOB_GUERILLA);
		}
		else
			BuyJob(PR_COST_GUERILLA,PR_JOB_GUERILLA);

		Menu_Profession();
	}
	if (inp == 7) {
		BuyJob(PR_COST_JUDOKA,PR_JOB_JUDOKA); // Original = 700
		Menu_Profession();
	}
	if (inp == 8) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_ENGINEERING;
		Menu_Engineering();
	}
	if (inp == 9) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_PROFESSION2;
		Menu_Profession2();
	}
	if (inp == 10) {
		if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
			DropFromCustomClassGen();
		else
			PrintNotEnoughMoney(0);
	}

	self->impulse = 0;
}

// ==================================
//	Prints Second page of Professions
// ==================================
void Menu_Profession2()
{
	string temp;
	temp = ftos(self->money);
	if (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY)
		temp = colstr(temp,PR_COLSTR_RED);
	string l1,l2,l3,l4, str;

	MenuResetCount();

	l1 = ConstructMenuJobStr(1,PR_STR_ARMY,PR_JOB_ARMY,PR_COST_ARMY,PR_COST_ARMY_EXTRA1,PR_COST_ARMY_EXTRA2,PR_STR_ARMY_EXTRA1,PR_STR_ARMY_EXTRA2);
	str = ConstructMenuJobStr(2,PR_STR_HACKER,PR_JOB_HACKER,PR_COST_HACKER,0,0,"","");
	l1 = strcat(l1,str);


	l2 = ConstructMenuJobStr(3,PR_STR_MARTYR,PR_JOB_MARTYR,PR_COST_MARTYR,0,0,"","");
	str = ConstructMenuJobStr(4,PR_STR_CRUSADER,PR_JOB_CRUSADER,PR_COST_CRUSADER,0,0,"","");
	l2 = strcat(l2,str);

	l4 = S_("\n^8.. ^bPrev^b    \n^9.. ^bNext^b    \n^0.. ^bDone^b!   \n");
	l3 = "\n\n";

	centerprint(self, S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 More Professions   \x11\x9C\x8B\n\n"), l1,l2,l3,l4, temp,S_(" dollars left\n\nUse ^bSKILL^b to trigger ability\n"));
}

void Menu_Profession2_Input(float inp)
{
	if (inp == 1) {
		if (extras_mode == 0)
		{
			if (self->job & PR_JOB_ARMY)
			   BuyExtra(PR_COST_ARMY_EXTRA1,PR_COST_ARMY_EXTRA2);
			else
			   BuyJob(PR_COST_ARMY,PR_JOB_ARMY);
		}
		else
			BuyJob(PR_COST_ARMY,PR_JOB_ARMY);

		Menu_Profession2();
	}
	if (inp == 2) {
		BuyJob(PR_COST_HACKER,PR_JOB_HACKER);
		Menu_Profession2();
	}
	if (inp == 3) {
		BuyJob(PR_COST_MARTYR,PR_JOB_MARTYR);
		Menu_Profession2();
	}
	if (inp == 4) {
		BuyJob(PR_COST_CRUSADER,PR_JOB_CRUSADER);
		Menu_Profession2();
	}
	if (inp == 8) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_PROFESSION;
		Menu_Profession();
	}
	if (inp == 9) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_PROFICIENCY;
		Menu_Proficiency();
	}
	if (inp == 10) {
		if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
			DropFromCustomClassGen();
		else
			PrintNotEnoughMoney(0);
	}

	self->impulse = 0;
}

// =====================================
//	Prints Menu of Proficiencies Available
// =====================================
void Menu_Proficiency()
{
	string temp;
	temp = ftos(self->money);
	if (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY)
		temp = colstr(temp,PR_COLSTR_RED);
	string l1,l2,l3,l4,str;

	MenuResetCount();

	l1 = ConstructMenuItemStr(1,PR_STR_STEALTH,PR_COST_STEALTH,self->cutf_items,PR_CUTF_STEALTH);
	str = ConstructMenuItemStr(2,PR_STR_HIGHJUMP,PR_COST_HIGHJUMP,self->cutf_items,PR_CUTF_HIGHJUMP);
	l1 = strcat(l1,str);

	l2 = ConstructMenuItemStr(3,PR_STR_HWGUY,PR_COST_HWGUY,self->cutf_items,PR_CUTF_ASPECT_OF_HWGUY);
	str = ConstructMenuItemStr(4,PR_STR_EXPBODY,PR_COST_EXPBODY,self->cutf_items,PR_CUTF_EXPBODY);
	l2 = strcat(l2,str);

	l3 = ConstructMenuItemStr(5,PR_STR_GYMNAST,PR_COST_GYMNAST,self->cutf_items,PR_CUTF_GYMNAST);
	str = ConstructMenuItemStr(6,PR_STR_DEMONLORE,PR_COST_DEMONLORE,self->cutf_items,PR_CUTF_DEMONLORE);
	l3 = strcat(l3,str);

	l4 = ConstructMenuItemStr(7,PR_STR_CLOSECOMBAT,PR_COST_CLOSECOMBAT,self->cutf_items,PR_CUTF_CLOSECOMBAT);
	l4 = strcat(l4,S_("^8.. ^bPrev^b    \n^9.. ^bNext^b    \n^0.. ^bDone^b!   \n"));

	centerprint(self, S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 Proficiency        \x11\x9C\x8B\n\n"), l1,l2,l3,l4, temp," dollars left\n");
}

void Menu_Proficiency_Input(float inp)
{
	if (inp == 1) {
		BuyCuTF(PR_COST_STEALTH,PR_CUTF_STEALTH);
		Menu_Proficiency();
	}
	if (inp == 2) {
		BuyCuTF(PR_COST_HIGHJUMP,PR_CUTF_HIGHJUMP);
		Menu_Proficiency();
	}
	if (inp == 3) {
		if (self->cutf_items & PR_CUTF_GYMNAST)
		{
			sprint(self, PR_PRINT_HIGH, "Having Gymnast and Aspect of HWGuy proficiencies is illogical.\n");
			CuTFMenuSound(PR_MENUSOUND_WRONG);
		}
		else
		{
			BuyCuTF(PR_COST_HWGUY,PR_CUTF_ASPECT_OF_HWGUY);
			Menu_Proficiency();
		}
	}
	if (inp == 4) {
		BuyCuTF(PR_COST_EXPBODY,PR_CUTF_EXPBODY);
		Menu_Proficiency();
	}
	if (inp == 5) {
		if (self->cutf_items & PR_CUTF_ASPECT_OF_HWGUY)
		{
			sprint(self, PR_PRINT_HIGH, "Trust me, you don't want Aspect of HWGuy AND Gymnast.\n");
			CuTFMenuSound(PR_MENUSOUND_WRONG);
		}
		else
		{
			BuyCuTF(PR_COST_GYMNAST,PR_CUTF_GYMNAST);
			Menu_Proficiency();
		}
	}
	if (inp == 6) {
		BuyCuTF(PR_COST_DEMONLORE,PR_CUTF_DEMONLORE);
		Menu_Proficiency();
	}
	if (inp == 7) {
		BuyCuTF(PR_COST_CLOSECOMBAT,PR_CUTF_CLOSECOMBAT);
		Menu_Proficiency();
	}
	if (inp == 8) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		if (custom_mode!=1)
		{
			self->current_menu = PR_MENU_PROFESSION2;
			Menu_Profession2();
		}
		else
		{
			self->current_menu = PR_MENU_ENGINEERING;
			Menu_Engineering();
		}
	}
	if (inp == 9) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_OPTION;
		Menu_Option();
	}
	if (inp == 10) {
		if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
			DropFromCustomClassGen();
		else
			PrintNotEnoughMoney(0);
	}

	self->impulse = 0;
}

// ====================================
//	Prints Menu of Optional Items Here
// ====================================
void Menu_Option()
{
	string temp;
	temp = ftos(self->money);
	if (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY)
		temp = colstr(temp,PR_COLSTR_RED);
	string l1,l2,l3,l4,str;

	MenuResetCount();

	l1 = ConstructMenuItemStr(1,PR_STR_AUTOID,PR_COST_AUTOID,self->tf_items,PR_NIT_AUTOID);
	str = ConstructMenuItemStr(2,PR_STR_RESPAWNGUARD,PR_COST_RESPAWNGUARD,self->tf_items,PR_NIT_RESPAWN_GUARD);
	l1 = strcat(l1,str);

	l2 = ConstructMenuItemStr(3,PR_STR_HOVERBOOTS,PR_COST_HOVERBOOTS,self->tf_items,PR_NIT_HOVER_BOOTS);
	str = ConstructMenuItemStr(4,PR_STR_BOOTUPGRADE,PR_COST_BOOTUPGRADE,self->tf_items,PR_NIT_HOVER_BOOTS_UPGRADE);
	l2 = strcat(l2,str);

	l3 = ConstructMenuItemStr(5,PR_STR_RLASERGUIDED,PR_COST_RLASERGUIDED,self->tf_items,PR_NIT_RL_LASER_SIGHT);
	str = ConstructMenuItemStr(6,PR_STR_OTR,PR_COST_OTR,self->cutf_items,PR_CUTF_OTR);
	l3 = strcat(l3,str);

	l4 = ConstructMenuItemStr(7,PR_STR_CLUSTER,PR_COST_CLUSTER,self->tf_items,PR_NIT_CLUSTER_ROCKETS);
	l4 = strcat(l4,S_("^8.. ^bPrev^b    \n^9.. ^bNext^b    \n^0.. ^bDone^b!   \n"));

	centerprint(self, S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 Optional Equipment \x11\x9C\x8B\n\n"), l1,l2,l3,l4, temp," dollars left\n");
}
void Menu_Option_Input(float inp)
{
	if (inp == 1) {
		BuyItem(PR_COST_AUTOID,PR_NIT_AUTOID);
		Menu_Option();
	}
	if (inp == 2) {
		BuyItem(PR_COST_RESPAWNGUARD,PR_NIT_RESPAWN_GUARD);
		Menu_Option();
	}
	if (inp == 3) {
		BuyItem(PR_COST_HOVERBOOTS,PR_NIT_HOVER_BOOTS);
		Menu_Option();
	}
	if (inp == 4) {
		BuyItem(PR_COST_BOOTUPGRADE, PR_NIT_HOVER_BOOTS_UPGRADE); //sync price with buyitem
		Menu_Option();
	}
	if (inp == 5) {
		BuyItem(PR_COST_RLASERGUIDED, PR_NIT_RL_LASER_SIGHT); //sync with RL
		Menu_Option();
	}
	if (inp == 6) {  // PZ: this section was after "inp == 2", for some reason...
		if (!(self->weapons_carried & PR_WEAP_SNIPER_RIFLE) && !(self->cutf_items & PR_CUTF_OTR))
		{
			CuTFMenuSound(PR_MENUSOUND_WRONG);
			sprint (self, PR_PRINT_HIGH, "Are you going to throw these bullets at them or something?\n");
			self->impulse = 0;
			return;
		}

		BuyCuTF(PR_COST_OTR,PR_CUTF_OTR);
		Menu_Option();
	}
	if (inp == 7) {
		  BuyItem(PR_COST_CLUSTER, PR_NIT_CLUSTER_ROCKETS); //sync with RL
		  Menu_Option();
	}
	if (inp == 8) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_PROFICIENCY;
		Menu_Proficiency();
	}
	if (inp == 9) {
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->current_menu = PR_MENU_MOREOPTION;
		Menu_MoreOption ();
	}
	if (inp == 10) {
		if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
			DropFromCustomClassGen();
		else
			PrintNotEnoughMoney(0);
	}

	self->impulse = 0;
}


// ====================================================================
//	Menu of more optional equipment - Gizmo
// ====================================================================
void Menu_MoreOption()
{
	string temp;
	temp = ftos(self->money);

	if (self->tfstate & PR_TFSTATE_NOTENOUGHMONEY)
		temp = colstr(temp,PR_COLSTR_RED);

	string l1,l2,l3,l4;

	MenuResetCount();

	l1 = ConstructMenuItemStr(1, PR_STR_FASTERROCKETS, PR_COST_FASTERROCKETS, self->cutf_moreitems, PR_CUTF_MI_FASTERROCKETS);
	l2 = ConstructMenuItemStr( 2, PR_STR_FRIENDORFOE, PR_COST_FRIENDORFOE, self->cutf_moreitems, PR_CUTF_MI_FRIENDORFOE );

	l3 = "\n\n\n";

	l4 = "\n\n"; // PZ: Added one \n to make it same length as other menus.
	l4 = strcat(l4, S_("^8.. ^bPrev^b    \n^9.. ^bFirst^b   \n^0.. ^bDone^b!   \n"));  // PZ: Made menus loop. Added "First".

	centerprint(self, S_("\x9C\x8B\x90 C^bu^bs^bt^bo^bm^bi^bz^be ^bYourself^b \x11\x8B\x9C\n\x8B\x9C\x10 More Equipment     \x11\x9C\x8B\n\n"), l1, l2, l3, l4, temp," dollars left\n");
}

void Menu_MoreOption_Input(float inp)
{
	if (inp >= 1 && inp <= 10)
	{
		// Gizmo - modified version of avirox's mods
		if (inp == 1)
		{
			if (self->weapons_carried & PR_WEAP_ROCKET_LAUNCHER) {
				if (self->cutf_moreitems & PR_CUTF_MI_FASTERROCKETS) {
					PrintRefund (PR_COST_FASTERROCKETS);
					self->cutf_moreitems = self->cutf_moreitems - PR_CUTF_MI_FASTERROCKETS;
				} else if (self->money >= PR_COST_FASTERROCKETS) {
					self->money = self->money - PR_COST_FASTERROCKETS;
					self->cutf_moreitems = self->cutf_moreitems | PR_CUTF_MI_FASTERROCKETS;
					PrintMoney ();
				} else
					PrintNotEnoughMoney (PR_COST_FASTERROCKETS);
			} else {
				// TODO: add these into custom.qc; infact all this should be in a new moreitems function in custom.qc
				sprint( self, PR_PRINT_HIGH, "Sorry, you have to buy the rocket launcher first!\n" );
				CuTFMenuSound( PR_MENUSOUND_WRONG );
			}

			Menu_MoreOption ();
		}
		else if ( inp == 2 )
		{
			if ( self->cutf_moreitems & PR_CUTF_MI_FRIENDORFOE ) {
				PrintRefund( PR_COST_FRIENDORFOE );
				self->cutf_moreitems = self->cutf_moreitems - PR_CUTF_MI_FRIENDORFOE;
			} else if ( self->money >= PR_COST_FRIENDORFOE ) {
				self->money = self->money - PR_COST_FRIENDORFOE;
				self->cutf_moreitems = self->cutf_moreitems | PR_CUTF_MI_FRIENDORFOE;
				PrintMoney();
			} else
				PrintNotEnoughMoney( PR_COST_FRIENDORFOE );

			Menu_MoreOption();
		}
		if (inp == 8)
		{
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
			self->current_menu = PR_MENU_OPTION;
			Menu_Option();
		}
		if (inp == 9)   // PZ: Made it so that the menus loop from end to beginning and vice versa.
		{
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
			self->current_menu = PR_MENU_PRIMARY_WEAPON;
			Menu_PrimaryWeapon();
		}
		if (inp == 10)
		{
			if (self->money >= 0)          // PZ: make sure not in negative; added all but DropFromCustomClassGen()
				DropFromCustomClassGen();
			else
				PrintNotEnoughMoney(0);
		}

		self->impulse = 0;
	}
}

//___________________________________________________________________________________
//===================================================================================
// Item string constructors for menu displayers

string ConstructMenuItemStr(float itemnum, string itemdesc, float cost, int field, int bit)
{
	string result;

	result = ftos(itemnum);
	result = colstr(result,PR_COLSTR_NUMBER);

	if (field & bit)
		result = strcat("" TO_STR(PR_CHAR_BOUGHT) " ",result);
	else
		result = strcat("  ",result);

	if (!CanBuyItem(itemnum))
	{
		result = strcat(result,S_("^b..^b "));
		itemdesc = colstr(itemdesc,PR_COLSTR_RED);
	}
	else if (ItemIsFree(itemnum))
	{
		result = strcat(result,S_("^b..^b "));
	}
	else
	{
		result = strcat(result,".. ");
		if (cost > self->money && !(field & bit))
			itemdesc = colstr(itemdesc,PR_COLSTR_RED);
	}

	result = strcat(result,itemdesc);

	if (cost < 100)
		result = strcat(result,S_("^b-^b   "));
	else if (cost < 1000)
		result = strcat(result,S_("^b-^b  "));
	else if (cost < 10000)
		result = strcat(result,S_("^b-^b "));
	else
		result = strcat(result,S_("^b-^b"));

	itemdesc = ftos(cost);

	if (field & bit)
		itemdesc = colstr(itemdesc,PR_COLSTR_NUMBER);
	/*else
	{
		if (cost > self.money)
			itemdesc = colstr(itemdesc,#COLSTR_RED);
	}*/

	result = strcat(result,itemdesc);

	result = strcat(result,"\n");

	return result;
}

string ConstructMenuSentryStr(float itemnum, string itemdesc, float cost)
{
	string result;
	float numbought;

	result = ftos(itemnum);
	result = colstr(result,PR_COLSTR_NUMBER);

	if (!(self->cutf_items & PR_CUTF_SENTRYGUN))
	{
		result = strcat("  ",result);
		numbought = 0;
	}
	else
	{
		if (self->cutf_items & PR_CUTF_DOUBLESENTRY)
		{
			result = strcat("2 ",result);
			numbought = 2;
		}
		else
		{
			result = strcat("1 ",result);
			numbought = 1;
		}
	}

	if (!CanBuyItem(itemnum))
	{
		result = strcat(result,S_("^b..^b "));
		itemdesc = colstr(itemdesc,PR_COLSTR_RED);
	}
	else if (ItemIsFree(itemnum))
	{
		result = strcat(result,S_("^b..^b "));
	}
	else
	{
		result = strcat(result,".. ");
		if (cost > self->money && !numbought)
			itemdesc = colstr(itemdesc,PR_COLSTR_RED);
	}

	result = strcat(result,itemdesc);

	if (cost < 100)
		result = strcat(result,S_("^b-^b   "));
	else if (cost < 1000)
		result = strcat(result,S_("^b-^b  "));
	else if (cost < 10000)
		result = strcat(result,S_("^b-^b "));
	else
		result = strcat(result,S_("^b-^b"));

	itemdesc = ftos(cost);

	if (numbought)
	{
		if (numbought == 2)
			itemdesc = ftos(cost*2);

		itemdesc = colstr(itemdesc,PR_COLSTR_NUMBER);
	}
	/*else
	{
		if (cost > self.money)
			itemdesc = colstr(itemdesc,#COLSTR_RED);
	}*/

	result = strcat(result,itemdesc);

	result = strcat(result,"\n");

	return result;
}

string ConstructMenuTeslaStr(float itemnum, string itemdesc, float cost)
{
	string result;
	float numbought;

	result = ftos(itemnum);
	result = colstr(result,PR_COLSTR_NUMBER);

	if (!(self->tf_items & PR_NIT_TESLA))
	{
		result = strcat("  ",result);
		numbought = 0;
	}
	else
	{
		if (self->tf_items & PR_NIT_DOUBLETESLA)
		{
			result = strcat("2 ",result);
			numbought = 2;
		}
		else
		{
			result = strcat("1 ",result);
			numbought = 1;
		}
	}

	if (!CanBuyItem(itemnum))
	{
		result = strcat(result,S_("^b..^b "));
		itemdesc = colstr(itemdesc,PR_COLSTR_RED);
	}
	else if (ItemIsFree(itemnum))
	{
		result = strcat(result,S_("^b..^b "));
	}
	else
	{
		result = strcat(result,".. ");
		if (cost > self->money && !numbought)
			itemdesc = colstr(itemdesc,PR_COLSTR_RED);
	}

	result = strcat(result,itemdesc);

	if (cost < 100)
		result = strcat(result,S_("^b-^b   "));
	else if (cost < 1000)
		result = strcat(result,S_("^b-^b  "));
	else if (cost < 10000)
		result = strcat(result,S_("^b-^b "));
	else
		result = strcat(result,S_("^b-^b"));

	itemdesc = ftos(cost);

	if (numbought)
	{
		if (numbought == 2)
			itemdesc = ftos(cost*2);

		itemdesc = colstr(itemdesc,PR_COLSTR_NUMBER);
	}
	/*else
	{
		if (cost > self.money)
			itemdesc = colstr(itemdesc,#COLSTR_RED);
	}*/

	result = strcat(result,itemdesc);

	result = strcat(result,"\n");

	return result;
}

string ConstructMenuJobStr(float itemnum, string itemdesc, int jobid, float cost, float extra1cost, float extra2cost, const string& extra1st, const string& extra2st)
{
	string result;
	float numbought;

	result = ftos(itemnum);
	result = colstr(result,PR_COLSTR_NUMBER);

	if (!(self->job & jobid))
	{
		result = strcat("  ",result);
		numbought = 0;
	}
	else
	{
		if (self->job & PR_JOB_EXTRA2)
		{
			result = strcat("3 ",result);
			numbought = 3;
		}
		else if (self->job & PR_JOB_EXTRA1)
		{
			result = strcat("2 ",result);
			numbought = 2;
		}
		else
		{
			if (!extra1cost || extras_mode)
				result = strcat("" TO_STR(PR_CHAR_BOUGHT) " ",result);
			else
				result = strcat("1 ",result);

			numbought = 1;
		}
	}

	if (numbought)
	if (!extras_mode)
	if (extra1cost)
	{
		if (numbought == 1)
			itemdesc = extra1st;
		else if (numbought == 2)
			itemdesc = extra2st;
		else if (numbought == 3)
			itemdesc = PR_STR_SELLEXTRAS;
	}

	if (!CanBuyItem(itemnum))
	{
		result = strcat(result,S_("^b..^b "));
		itemdesc = colstr(itemdesc,PR_COLSTR_RED);
	}
	else if (ItemIsFree(itemnum))
	{
		result = strcat(result,S_("^b..^b "));
	}
	else
	{
		result = strcat(result,".. ");

		if (!numbought)
		if (cost > self->money + SpentOnJob())
			itemdesc = colstr(itemdesc,PR_COLSTR_RED);
	}

	result = strcat(result,itemdesc);

	if (cost < 100)
		result = strcat(result,S_("^b-^b  "));
	else if (cost < 1000)
		result = strcat(result,S_("^b-^b "));
	else if (cost < 10000)
		result = strcat(result,S_("^b-^b"));
	else
		result = strcat(result,"");

	if (numbought && extra1cost && numbought != 3 && !extras_mode)
		result = strcat(result," +");
	else
		result = strcat(result," ");

	itemdesc = ftos(cost);

	if (numbought)
	{
		if (numbought == 3)
			itemdesc = ftos(cost+extra1cost+extra2cost);
		else if (numbought == 2)
			itemdesc = ftos(extra2cost);
		else if (extra1cost && !extras_mode)
			itemdesc = ftos(extra1cost);

		itemdesc = colstr(itemdesc,PR_COLSTR_NUMBER);
	}
	/*else
	{
		if (cost > self.money)
			itemdesc = colstr(itemdesc,#COLSTR_RED);
	}*/

	result = strcat(result,itemdesc);

	result = strcat(result,"\n");

	return result;
}

float SpentOnJob()
{
	if (self->job & PR_JOB_THIEF)
		return PR_COST_THIEF;
	if (self->job & PR_JOB_RUNNER)
		return PR_COST_RUNNER;
	if (self->job & PR_JOB_WARLOCK)
	{
		if (self->job & PR_JOB_EXTRA2)
			return PR_COST_WARLOCK + PR_COST_WARLOCK_EXTRA1 + PR_COST_WARLOCK_EXTRA2;
		if (self->job & PR_JOB_EXTRA1)
			return PR_COST_WARLOCK + PR_COST_WARLOCK_EXTRA1;

		return PR_COST_WARLOCK;
	}
	if (self->job & PR_JOB_CHAPLAN)
		return PR_COST_CHAPLAN;
	if (self->job & PR_JOB_BERSERKER)
		return PR_COST_BERSERKER;
	if (self->job & PR_JOB_GUERILLA)
	{
		if (self->job & PR_JOB_EXTRA2)
			return PR_COST_GUERILLA + PR_COST_GUERILLA_EXTRA1 + PR_COST_GUERILLA_EXTRA2;
		if (self->job & PR_JOB_EXTRA1)
			return PR_COST_GUERILLA + PR_COST_GUERILLA_EXTRA1;

		return PR_COST_GUERILLA;
	}
	if (self->job & PR_JOB_JUDOKA)
		return PR_COST_JUDOKA;
	if (self->job & PR_JOB_ARMY)
	{
		if (self->job & PR_JOB_EXTRA2)
			return PR_COST_ARMY + PR_COST_ARMY_EXTRA1 + PR_COST_ARMY_EXTRA2;
		if (self->job & PR_JOB_EXTRA1)
			return PR_COST_ARMY + PR_COST_ARMY_EXTRA1;

		return PR_COST_ARMY;
	}
	if (self->job & PR_JOB_HACKER)
		return PR_COST_HACKER;
	if (self->job & PR_JOB_MARTYR)
		return PR_COST_MARTYR;
	if (self->job & PR_JOB_CRUSADER)
		return PR_COST_CRUSADER;

	return 0;
}

string ConstructMenuGrenStr(float itemnum, string itemdesc, float cost, float grentype)
{
	string result;
	float numbought;

	result = ftos(itemnum);
	result = colstr(result,PR_COLSTR_NUMBER);

	if ((self->tp_grenades_1 != grentype) && (self->tp_grenades_2 != grentype))
	{
		result = strcat("  ",result);
		numbought = 0;
	}
	else
	{
		if ((self->tp_grenades_1 == grentype) && (self->tp_grenades_2 == grentype))
		{
			result = strcat("2 ",result);
			numbought = 2;
		}
		else
		{
			result = strcat("1 ",result);
			numbought = 1;
		}
	}

	if (!CanBuyItem(itemnum))
	{
		result = strcat(result,S_("^b..^b "));
		itemdesc = colstr(itemdesc,PR_COLSTR_RED);
	}
	else if (ItemIsFree(itemnum))
	{
		result = strcat(result,S_("^b..^b "));
	}
	else
	{
		result = strcat(result,".. ");

		if (!numbought)
		if (cost > self->money + SpentOnGrens())
			itemdesc = colstr(itemdesc,PR_COLSTR_RED);
	}

	result = strcat(result,itemdesc);

	if (numbought == 2)
		cost = cost * 2;

	if (cost < 100)
		result = strcat(result,S_("^b-^b   "));
	else if (cost < 1000)
		result = strcat(result,S_("^b-^b  "));
	else if (cost < 10000)
		result = strcat(result,S_("^b-^b "));
	else
		result = strcat(result,S_("^b-^b"));

	itemdesc = ftos(cost);

	if (numbought)
		itemdesc = colstr(itemdesc,PR_COLSTR_NUMBER);
	/*else
	{
		if (cost > self.money)
			itemdesc = colstr(itemdesc,#COLSTR_RED);
	}*/

	result = strcat(result,itemdesc);

	result = strcat(result,"\n");

	return result;
}

float GetGrenCost(float grentype)
{
	if (grentype == 0)
		return 0;
	if (grentype == PR_GR_TYPE_NORMAL)
		return PR_COST_GR_NORMAL;
	if (grentype == PR_GR_TYPE_CONCUSSION)
		return PR_COST_GR_CONCUSSION;
	if (grentype == PR_GR_TYPE_NAIL)
		return PR_COST_GR_NAIL;
	if (grentype == PR_GR_TYPE_MIRV)
		return PR_COST_GR_MIRV;
	if (grentype == PR_GR_TYPE_NAPALM)
		return PR_COST_GR_NAPALM;
	if (grentype == PR_GR_TYPE_FLARE)
		return PR_COST_GR_FLARE;
	if (grentype == PR_GR_TYPE_GAS)
		return PR_COST_GR_GAS;
	if (grentype == PR_GR_TYPE_EMP)
		return PR_COST_GR_EMP;
	if (grentype == PR_GR_TYPE_PSIONIC)
		return PR_COST_GR_PSIONIC;
	if (grentype == PR_GR_TYPE_FRAG)
		return PR_COST_GR_FRAG;
	if (grentype == PR_GR_TYPE_KRAC)
		return PR_COST_GR_KRAC;
	if (grentype == PR_GR_TYPE_ANTIGRAV)
		return PR_COST_GR_ANTIGRAV;
	if (grentype == PR_GR_TYPE_BIO)
		return PR_COST_GR_BIOLOGICAL;
	if (grentype == PR_GR_TYPE_CALTROP)
		return PR_COST_GR_CALTROP;

	return 0;
}

float SpentOnGrens()
{
	float result;

	result = GetGrenCost(self->tp_grenades_1);
	result = result + GetGrenCost(self->tp_grenades_2);

	return result;
}

// PZ NOTE: Speed 1 is the fastest (cheetah).
float SpentOnLegs()
{
	// PZ: Check to see whether their stock class speed matches any of the custom speeds.
	//     Added everything from "||" to the right, in conditions.
/*local string strtemp;
strtemp = ftos(self.custom_speed);
bprint(2, "self.custom_speed = ", strtemp, " \\ ");
strtemp = ftos(self.worldtype);
bprint(2, "self.worldtype = ", strtemp, "\n");*/
	if (self->custom_speed == PR_CUSTOM_SPEED1 || (self->custom_speed == 0 && self->worldtype == PR_CUSTOM_SPEED1))
		return PR_COST_SPEED1;
	if (self->custom_speed == PR_CUSTOM_SPEED2 || (self->custom_speed == 0 && self->worldtype == PR_CUSTOM_SPEED2))
		return PR_COST_SPEED2;
	if (self->custom_speed == PR_CUSTOM_SPEED3 || (self->custom_speed == 0 && self->worldtype == PR_CUSTOM_SPEED3))
		return PR_COST_SPEED3;
	if (self->custom_speed == PR_CUSTOM_SPEED4 || (self->custom_speed == 0 && self->worldtype == PR_CUSTOM_SPEED4))
		return PR_COST_SPEED4;
	if (self->custom_speed == PR_CUSTOM_SPEED5 || (self->custom_speed == 0 && self->worldtype == PR_CUSTOM_SPEED5))
		return PR_COST_SPEED5;
	if (self->custom_speed == PR_CUSTOM_SPEED6 || (self->custom_speed == 0 && self->worldtype == PR_CUSTOM_SPEED6))
		return PR_COST_SPEED6;

	return 0;
}

// PZ NOTE: Speed 1 is the fastest (cheetah).
float GetCurrentLegs()
{
	if (self->custom_speed == PR_CUSTOM_SPEED1)
		return 1;
	if (self->custom_speed == PR_CUSTOM_SPEED2)
		return 2;
	if (self->custom_speed == PR_CUSTOM_SPEED3)
		return 3;
	if (self->custom_speed == PR_CUSTOM_SPEED4)
		return 4;
	if (self->custom_speed == PR_CUSTOM_SPEED5)
		return 5;
	if (self->custom_speed == PR_CUSTOM_SPEED6)
		return 6;

	return 7;
}

// PZ: Added. This gets the previous legs that were saved in .worldtype, before .maxspeed was cleared, when entering the custom menu.
//     This will return a number half way between each of the integer return values (e.g., 1.5), if the previous legs don't match one
//     of the custom leg options. It will indicate which options it falls between. For example, faster than speed 2 and slower than
//     speed 1 will result in 1.5 returned.
// PZ NOTE: Speed 1 is the fastest (cheetah).
float GetPreviousLegs()
{
	// Check to see whether their stock class speed matches any of the custom speeds.
	if (self->worldtype == PR_CUSTOM_SPEED1)
		return 1;
	if (self->worldtype == PR_CUSTOM_SPEED2)
		return 2;
	if (self->worldtype == PR_CUSTOM_SPEED3)
		return 3;
	if (self->worldtype == PR_CUSTOM_SPEED4)
		return 4;
	if (self->worldtype == PR_CUSTOM_SPEED5)
		return 5;
	if (self->worldtype == PR_CUSTOM_SPEED6)
		return 6;
	if (self->worldtype == PR_CUSTOM_SPEED7)
		return 7;

	// PZ: See if we can return an in-between value.
	if (self->worldtype > 0)
	{
		if (self->worldtype > PR_CUSTOM_SPEED1) return 0.5; // better than cheetah legs
		if (self->worldtype > PR_CUSTOM_SPEED2) return 1.5;
		if (self->worldtype > PR_CUSTOM_SPEED3) return 2.5;
		if (self->worldtype > PR_CUSTOM_SPEED4) return 3.5;
		if (self->worldtype > PR_CUSTOM_SPEED5) return 4.5;
		if (self->worldtype > PR_CUSTOM_SPEED6) return 5.5;
		if (self->worldtype > PR_CUSTOM_SPEED7) return 6.5;
		else                                 return 7.5; // worse than penguin legs
	}

	return 0;  // .worldtype should be 0, if we got here
}

string ConstructMenuSpeedStr(float itemnum, string itemdesc, float cost)
{
	string result;
	float bought;

	result = ftos(itemnum);
	result = colstr(result,PR_COLSTR_NUMBER);

	bought = PR_FALSE;

	if (GetCurrentLegs() == itemnum)
	{
		result = strcat("" TO_STR(PR_CHAR_BOUGHT) " ",result);
		bought = PR_TRUE;
	}
	else
		result = strcat("  ",result);

	if (!CanBuyItem(itemnum))
	{
		result = strcat(result,S_("^b..^b "));
		itemdesc = colstr(itemdesc,PR_COLSTR_RED);
	}
	else if (ItemIsFree(itemnum))
	{
		result = strcat(result,S_("^b..^b "));
	}
	else
	{
		result = strcat(result,".. ");

		if (!bought)
		if (cost > self->money + SpentOnLegs())
			itemdesc = colstr(itemdesc,PR_COLSTR_RED);
	}

	result = strcat(result,itemdesc);

	if (cost < 10)
		result = strcat(result,S_("^b-^b    "));
	else if (cost < 100)
		result = strcat(result,S_("^b-^b   "));
	else if (cost < 1000)
		result = strcat(result,S_("^b-^b  "));
	else if (cost < 10000)
		result = strcat(result,S_("^b-^b "));
	else
		result = strcat(result,S_("^b-^b"));

	itemdesc = ftos(cost);

	if (bought && cost)
		itemdesc = colstr(itemdesc,PR_COLSTR_NUMBER);
	/*else
	{
		if (cost > self.money)
			itemdesc = colstr(itemdesc,#COLSTR_RED);
	}*/

	result = strcat(result,itemdesc);

	result = strcat(result,"\n");

	return result;
}

float SpentOnHP()
{
	// PZ: I edited this section to make it check for everything (armor, health, and armor type), and use the defines that I created.
	if (self->maxarmor == PR_CUSTOM_ARMOR_AMOUNT_1 && self->max_health == PR_CUSTOM_HEALTH_1 && isApproxEqual(self->armor_allowed, PR_CUSTOM_ARMOR_TYPE_1, 0.05))
		return PR_COST_HP1;
	if (self->maxarmor == PR_CUSTOM_ARMOR_AMOUNT_2 && self->max_health == PR_CUSTOM_HEALTH_2 && isApproxEqual(self->armor_allowed, PR_CUSTOM_ARMOR_TYPE_2, 0.05))
		return PR_COST_HP2;
	if (self->maxarmor == PR_CUSTOM_ARMOR_AMOUNT_3 && self->max_health == PR_CUSTOM_HEALTH_3 && isApproxEqual(self->armor_allowed, PR_CUSTOM_ARMOR_TYPE_3, 0.05))
		return PR_COST_HP3;
	if (self->maxarmor == PR_CUSTOM_ARMOR_AMOUNT_4 && self->max_health == PR_CUSTOM_HEALTH_4 && isApproxEqual(self->armor_allowed, PR_CUSTOM_ARMOR_TYPE_4, 0.05))
		return PR_COST_HP4;
	if (self->maxarmor == PR_CUSTOM_ARMOR_AMOUNT_5 && self->max_health == PR_CUSTOM_HEALTH_5 && isApproxEqual(self->armor_allowed, PR_CUSTOM_ARMOR_TYPE_5, 0.05))
		return PR_COST_HP5;
	if (self->maxarmor == PR_CUSTOM_ARMOR_AMOUNT_6 && self->max_health == PR_CUSTOM_HEALTH_6 && isApproxEqual(self->armor_allowed, PR_CUSTOM_ARMOR_TYPE_6, 0.05))
		return PR_COST_HP6;
	if (self->maxarmor == PR_CUSTOM_ARMOR_AMOUNT_7 && self->max_health == PR_CUSTOM_HEALTH_7 && isApproxEqual(self->armor_allowed, PR_CUSTOM_ARMOR_TYPE_7, 0.05))
		return PR_COST_HP7;

	// PZ NOTE: previous code
	//WK 1/7/7 This needs to be aligned with the constants
	/*if (self.maxarmor == 300)
		return #COST_HP7;
	if (self.maxarmor == 200)
		return #COST_HP6;
	if (self.maxarmor == 150)
		return #COST_HP5;
	if (self.maxarmor == 120)
		return #COST_HP4;
	if (self.maxarmor == 100)
		return #COST_HP3;
	if (self.health == 90)
		return #COST_HP2;
	if (self.health == 75)
		return #COST_HP1;*/

	return 0;
}

float GetCurrentHPItem()
{
	// PZ: I edited this section to make it check for everything (armor, health, and armor type), and use the defines that I created.
	if (self->maxarmor == PR_CUSTOM_ARMOR_AMOUNT_1 && self->max_health == PR_CUSTOM_HEALTH_1 && isApproxEqual(self->armor_allowed, PR_CUSTOM_ARMOR_TYPE_1, 0.05))
		return 1;
	if (self->maxarmor == PR_CUSTOM_ARMOR_AMOUNT_2 && self->max_health == PR_CUSTOM_HEALTH_2 && isApproxEqual(self->armor_allowed, PR_CUSTOM_ARMOR_TYPE_2, 0.05))
		return 2;
	if (self->maxarmor == PR_CUSTOM_ARMOR_AMOUNT_3 && self->max_health == PR_CUSTOM_HEALTH_3 && isApproxEqual(self->armor_allowed, PR_CUSTOM_ARMOR_TYPE_3, 0.05))
		return 3;
	if (self->maxarmor == PR_CUSTOM_ARMOR_AMOUNT_4 && self->max_health == PR_CUSTOM_HEALTH_4 && isApproxEqual(self->armor_allowed, PR_CUSTOM_ARMOR_TYPE_4, 0.05))
		return 4;
	if (self->maxarmor == PR_CUSTOM_ARMOR_AMOUNT_5 && self->max_health == PR_CUSTOM_HEALTH_5 && isApproxEqual(self->armor_allowed, PR_CUSTOM_ARMOR_TYPE_5, 0.05))
		return 5;
	if (self->maxarmor == PR_CUSTOM_ARMOR_AMOUNT_6 && self->max_health == PR_CUSTOM_HEALTH_6 && isApproxEqual(self->armor_allowed, PR_CUSTOM_ARMOR_TYPE_6, 0.05))
		return 6;
	if (self->maxarmor == PR_CUSTOM_ARMOR_AMOUNT_7 && self->max_health == PR_CUSTOM_HEALTH_7 && isApproxEqual(self->armor_allowed, PR_CUSTOM_ARMOR_TYPE_7, 0.05))
		return 7;

	// PZ NOTE: previous code
	//WK 1/7/7 This needs to be aligned with the constants
	/*if (self.maxarmor == 300)
		return 7;
	if (self.maxarmor == 200)
		return 6;
	if (self.maxarmor == 150)
		return 5;
	if (self.maxarmor == 120)
		return 4;
	if (self.maxarmor == 100)
		return 3;
	if (self.health == 90)
		return 2;
	if (self.health == 75)
		return 1;*/

	return 0;
}

string ConstructMenuHPStr(float itemnum, string itemdesc, float cost)
{
	string result;
	float bought;

	result = ftos(itemnum);
	result = colstr(result,PR_COLSTR_NUMBER);

	bought = PR_FALSE;

	if (GetCurrentHPItem() == itemnum)
	{
		result = strcat("" TO_STR(PR_CHAR_BOUGHT) " ",result);
		bought = PR_TRUE;
	}
	else
		result = strcat("  ",result);

	if (!CanBuyItem(itemnum))
	{
		result = strcat(result,S_("^b..^b "));
		itemdesc = colstr(itemdesc,PR_COLSTR_RED);
	}
	else if (ItemIsFree(itemnum))
	{
		result = strcat(result,S_("^b..^b "));
	}
	else
	{
		result = strcat(result,".. ");

		if (!bought)
		if (cost > self->money + SpentOnHP())
			itemdesc = colstr(itemdesc,PR_COLSTR_RED);
	}

	result = strcat(result,itemdesc);

	if (cost < 10)
		result = strcat(result,S_("^b-^b    "));
	else if (cost < 100)
		result = strcat(result,S_("^b-^b   "));
	else if (cost < 1000)
		result = strcat(result,S_("^b-^b  "));
	else if (cost < 10000)
		result = strcat(result,S_("^b-^b "));
	else
		result = strcat(result,S_("^b-^b"));

	itemdesc = ftos(cost);

	if (bought && cost)
		itemdesc = colstr(itemdesc,PR_COLSTR_NUMBER);
	/*else
	{
		if (cost > self.money)
			itemdesc = colstr(itemdesc,#COLSTR_RED);
	}*/

	result = strcat(result,itemdesc);
	result = strcat(result,"\n");

	return result;
}

void UpdateArmorItem(entity player)
{
	player->items = player->items - (player->items & (PR_IT_ARMOR1 | PR_IT_ARMOR2 | PR_IT_ARMOR3));
	if      (player->armortype >= PR_RED_ARMOR)        // PZ: replaced armor 'percentage' values with the defines that I created (#RED_ARMOR, etc.)
		player->items = player->items | PR_IT_ARMOR3;
	else if (player->armortype >= PR_YELLOW_ARMOR)
		player->items = player->items | PR_IT_ARMOR2;
	else if (player->armortype >= PR_GREEN_ARMOR)
		player->items = player->items | PR_IT_ARMOR1;
}

// PZ: This adds up all the money that the player has spent on everything. This was created for the custom class editing mode.
//     When the player goes from a stock class to custom, we need to determine the cost of the stock class's items.
//     WARNING: This may not handle all free items correctly, if items weren't made free in givenstuff[1-5] (or gstuff[1-5]).
float getTotalSpent()
{
string tempstr;
//float tempnum;
	float totalCost; totalCost = 0;
	float prevCurrentMenu;
	prevCurrentMenu = self->current_menu; // we need to change `.current_menu` for ItemIsFree()

	self->current_menu = 1; // Primary Weapons
	/*1*/if (!ItemIsFree(1) && self->weapons_carried & PR_WEAP_SNIPER_RIFLE) totalCost = totalCost + PR_COST_SNIPERRIFLE;
	/*2*/if (!ItemIsFree(2) && self->weapons_carried & PR_WEAP_ASSAULT_CANNON) totalCost = totalCost + PR_COST_ASSAULTCANNON;
	/*3*/if (!ItemIsFree(3) && self->weapons_carried & PR_WEAP_ROCKET_LAUNCHER) totalCost = totalCost + PR_COST_ROCKETLAUNCHER;
	/*4*/if (!ItemIsFree(4) && self->weapons_carried & PR_WEAP_FLAMETHROWER) totalCost = totalCost + PR_COST_PYROTOYS;
	/*5*/if (!ItemIsFree(5) && self->weapons_carried & PR_WEAP_GRENADE_LAUNCHER) totalCost = totalCost + PR_COST_GRENADELAUNCHER;
	/*6*/if (!ItemIsFree(6) && self->weapons_carried & PR_WEAP_LIGHTNING) totalCost = totalCost + PR_COST_LIGHTNING;
	/*7*/if (!ItemIsFree(7) && self->weapons_carried & PR_WEAP_LIGHT_ASSAULT) totalCost = totalCost + PR_COST_LIGHTASSAULT;
	self->current_menu = 2; // Secondary Weapons
	/*1*/if (!ItemIsFree(1) && self->weapons_carried & PR_WEAP_SNG) totalCost = totalCost + PR_COST_SNG;
	/*2*/if (!ItemIsFree(2) && self->cutf_items & PR_CUTF_KNIFE) totalCost = totalCost + PR_COST_KNIFE;
	/*3*/if (!ItemIsFree(3) && self->weapons_carried & PR_WEAP_SUPER_SHOTGUN) totalCost = totalCost + PR_COST_SUPERSHOTGUN;
	/*4*/if (!ItemIsFree(4) && self->weapons_carried & PR_WEAP_RAILGUN) totalCost = totalCost + PR_COST_RAILGUN;
	/*5*/if (!ItemIsFree(5) && self->weapons_carried & PR_WEAP_NAILGUN) totalCost = totalCost + PR_COST_NAILGUN;
	/*6*/if (!ItemIsFree(6) && self->weapons_carried & PR_WEAP_TRANQ) totalCost = totalCost + PR_COST_TRANQ;
	/*7*/if (!ItemIsFree(7) && self->weapons_carried & PR_WEAP_SHOTGUN) totalCost = totalCost + PR_COST_SHOTGUN;
	self->current_menu = 3; // Misc/Extra Weapons
	/*1*/if (!ItemIsFree(1) && self->weapons_carried & PR_WEAP_MAUSER) totalCost = totalCost + PR_COST_MAUSER;
	/*2*/if (!ItemIsFree(2) && self->weapons_carried & PR_WEAP_AIRF) totalCost = totalCost + PR_COST_AIRF;
	/*3*/if (!ItemIsFree(3) && self->cutf_items & PR_CUTF_AIRFISTUPGRADE) totalCost = totalCost + PR_COST_AIRFISTUPGRADE;
	/*4*/if (!ItemIsFree(4) && self->weapons_carried & PR_WEAP_DAEDALUS) totalCost = totalCost + PR_COST_DAEDALUS;
	/*5*/if (!ItemIsFree(5) && self->weapons_carried & PR_WEAP_LASERCANNON) totalCost = totalCost + PR_COST_LASERCANNON;
	/*6*/if (!ItemIsFree(6) && self->cutf_items & PR_CUTF_HOLO) totalCost = totalCost + PR_COST_HOLO;
	/*7*/if (!ItemIsFree(7) && self->cutf_items & PR_CUTF_CLIPEXTEND) totalCost = totalCost + PR_COST_CLIPEXTEND;
	self->current_menu = 4; // Tools
	/*1*/if (!ItemIsFree(1) && self->weapons_carried & PR_WEAP_ZEROGRAVITY) totalCost = totalCost + PR_COST_ZEROGRAVITY;
	self->current_menu = 5; // Legs
	totalCost = totalCost + SpentOnLegs();
/*tempnum = SpentOnLegs();
tempstr = ftos(tempnum);
bprint(2, "SpentOnLegs() = ", tempstr, "\n");*/
	self->current_menu = 6; // Health and Armor
	totalCost = totalCost + SpentOnHP();
/*tempnum = SpentOnHP();
tempstr = ftos(tempnum);
bprint(2, "SpentOnHP() = ", tempstr, "\n");*/
	self->current_menu = 7; // Special Armor / Ammo
	/*1*/if (!ItemIsFree(1) && self->tf_items & PR_NIT_CERAMIC) totalCost = totalCost + PR_COST_CERAMIC;
	/*2*/if (!ItemIsFree(2) && self->tf_items & PR_NIT_GEL) totalCost = totalCost + PR_COST_GEL;
	/*3*/if (!ItemIsFree(3) && self->tf_items & PR_NIT_ASBESTOS) totalCost = totalCost + PR_COST_ASBESTOS;
	/*4*/if (!ItemIsFree(4) && self->tf_items & PR_NIT_KEVLAR) totalCost = totalCost + PR_COST_KEVLAR;
	/*5*/if (!ItemIsFree(5) && self->tf_items & PR_NIT_BLAST) totalCost = totalCost + PR_COST_BLAST;
	/*6*/if (!ItemIsFree(6) && self->tf_items & PR_NIT_AMMO_BACKPACK) totalCost = totalCost + PR_COST_AMMOBACKPACK;
	/*7*/if (!ItemIsFree(7) && self->tf_items & PR_NIT_AMMO_BANDOLIER) totalCost = totalCost + PR_COST_AMMOBANDOLIER;
	self->current_menu = 8; // Special Items
	/*1*/if (!ItemIsFree(1) && self->tf_items & PR_NIT_SCANNER) totalCost = totalCost + PR_COST_SCANNER;
	/*2*/if (!ItemIsFree(2) && self->tf_items & PR_NIT_AUTOSCANNER) totalCost = totalCost + PR_COST_AUTOSCANNER;
	/*3*/if (!ItemIsFree(3) && self->cutf_items & PR_CUTF_DETPACK) totalCost = totalCost + PR_COST_DETPACK;
	/*4*/if (!ItemIsFree(4) && self->weapons_carried & PR_WEAP_MEDIKIT) totalCost = totalCost + PR_COST_MEDIKIT;
	/*5*/if (!ItemIsFree(5) && self->cutf_items & PR_CUTF_SPY_KIT) totalCost = totalCost + PR_COST_SPYKIT;
	/*6*/if (!ItemIsFree(6) && self->tf_items & PR_NIT_SCUBA) totalCost = totalCost + PR_COST_SCUBA;
	/*7*/if (!ItemIsFree(7) && self->weapons_carried & PR_WEAP_HOOK) totalCost = totalCost + PR_COST_HOOK;
	self->current_menu = 9; // More Special Items
	/*1*/if (!ItemIsFree(1) && self->cutf_items & PR_CUTF_JAMMER) totalCost = totalCost + PR_COST_JAMMER;
	/*2*/if (!ItemIsFree(2) && self->cutf_items & PR_CUTF_CYBERAUG) totalCost = totalCost + PR_COST_CYBERAUG;
	/*3*/if (!ItemIsFree(3) && self->cutf_items & PR_CUTF_SENSOR) totalCost = totalCost + PR_COST_SENSOR;
	/*4*/if (!ItemIsFree(4) && self->cutf_items & PR_CUTF_FIELDGEN) totalCost = totalCost + PR_COST_FIELDGEN;
	/*5*/if (!ItemIsFree(5) && self->cutf_items & PR_CUTF_FULLARMOUR) totalCost = totalCost + PR_COST_FULLARMOUR;
	/*6*/if (!ItemIsFree(6) && self->cutf_items & PR_CUTF_TOSSABLEDET) totalCost = totalCost + PR_COST_TOSSABLEDET;
	/*7*/if (!ItemIsFree(7) && self->cutf_items & PR_CUTF_DISPENSER) totalCost = totalCost + PR_COST_DISPENSER;
	self->current_menu = 10; // Grenades - Combat
	self->current_menu = 11; // Grenades - Utility
	totalCost = totalCost + SpentOnGrens();
/*tempnum = SpentOnGrens();
tempstr = ftos(tempnum);
bprint(2, "SpentOnGrens() = ", tempstr, "\n");*/
	self->current_menu = 12; // Engineering Items
	/*1*/if (!ItemIsFree(1) && self->weapons_carried & PR_WEAP_SPANNER) totalCost = totalCost + PR_COST_SPANNER;
	/*2*/if (!ItemIsFree(2) && self->cutf_items & PR_CUTF_SENTRYGUN) totalCost = totalCost + PR_COST_SENTRYGUN;
	/*2*/if (!ItemIsFree(2) && self->cutf_items & PR_CUTF_DOUBLESENTRY) totalCost = totalCost + PR_COST_SENTRYGUN;
	/*3*/if (!ItemIsFree(3) && self->tf_items & PR_NIT_TURRET) totalCost = totalCost + PR_COST_SENTRYUPGRADE;
	/*4*/if (!ItemIsFree(4) && self->tf_items & PR_NIT_TESLA) totalCost = totalCost + PR_COST_TESLACOIL;
	/*4*/if (!ItemIsFree(4) && self->tf_items & PR_NIT_DOUBLETESLA) totalCost = totalCost + PR_COST_TESLACOIL;
	/*5*/if (!ItemIsFree(5) && self->tf_items & PR_NIT_TESLA_UPGRADE) totalCost = totalCost + PR_COST_TESLAUPGRADE;
	/*6*/if (!ItemIsFree(6) && self->tf_items & PR_NIT_SECURITY_CAMERA) totalCost = totalCost + PR_COST_SECURITYCAMERA;
	/*7*/if (!ItemIsFree(7) && self->tf_items & PR_NIT_TELEPORTER) totalCost = totalCost + PR_COST_TELEPORTER;
	self->current_menu = 13; // Skilled Profession
	self->current_menu = 14; // More Professions
	totalCost = totalCost + SpentOnJob();
/*tempnum = SpentOnJob();
tempstr = ftos(tempnum);
bprint(2, "SpentOnJob() = ", tempstr, "\n");*/
	self->current_menu = 15; // Proficiency
	/*1*/if (!ItemIsFree(1) && self->cutf_items & PR_CUTF_STEALTH) totalCost = totalCost + PR_COST_STEALTH;
	/*2*/if (!ItemIsFree(2) && self->cutf_items & PR_CUTF_HIGHJUMP) totalCost = totalCost + PR_COST_HIGHJUMP;
	/*3*/if (!ItemIsFree(3) && self->cutf_items & PR_CUTF_ASPECT_OF_HWGUY) totalCost = totalCost + PR_COST_HWGUY;
	/*4*/if (!ItemIsFree(4) && self->cutf_items & PR_CUTF_EXPBODY) totalCost = totalCost + PR_COST_EXPBODY;
	/*5*/if (!ItemIsFree(5) && self->cutf_items & PR_CUTF_GYMNAST) totalCost = totalCost + PR_COST_GYMNAST;
	/*6*/if (!ItemIsFree(6) && self->cutf_items & PR_CUTF_DEMONLORE) totalCost = totalCost + PR_COST_DEMONLORE;
	/*7*/if (!ItemIsFree(7) && self->cutf_items & PR_CUTF_CLOSECOMBAT) totalCost = totalCost + PR_COST_CLOSECOMBAT;
	self->current_menu = 16; // Optional Equipment
	/*1*/if (!ItemIsFree(1) && self->tf_items & PR_NIT_AUTOID) totalCost = totalCost + PR_COST_AUTOID;
	/*2*/if (!ItemIsFree(2) && self->tf_items & PR_NIT_RESPAWN_GUARD) totalCost = totalCost + PR_COST_RESPAWNGUARD;
	/*3*/if (!ItemIsFree(3) && self->tf_items & PR_NIT_HOVER_BOOTS) totalCost = totalCost + PR_COST_HOVERBOOTS;
	/*4*/if (!ItemIsFree(4) && self->tf_items & PR_NIT_HOVER_BOOTS_UPGRADE) totalCost = totalCost + PR_COST_BOOTUPGRADE;
	/*5*/if (!ItemIsFree(5) && self->tf_items & PR_NIT_RL_LASER_SIGHT) totalCost = totalCost + PR_COST_RLASERGUIDED;
	/*6*/if (!ItemIsFree(6) && self->cutf_items & PR_CUTF_OTR) totalCost = totalCost + PR_COST_OTR;
	/*7*/if (!ItemIsFree(7) && self->tf_items & PR_NIT_CLUSTER_ROCKETS) totalCost = totalCost + PR_COST_CLUSTER;
	self->current_menu = 17; // More Equipment
	/*1*/if (!ItemIsFree(1) && self->cutf_moreitems & PR_CUTF_MI_FASTERROCKETS) totalCost = totalCost + PR_COST_FASTERROCKETS;
	/*2*/if (!ItemIsFree(2) && self->cutf_moreitems & PR_CUTF_MI_FRIENDORFOE) totalCost = totalCost + PR_COST_FRIENDORFOE;

	self->current_menu = prevCurrentMenu;
	return totalCost;
}

} // END namespace Progs
