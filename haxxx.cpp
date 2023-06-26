/*=======================================================//
// haxxx.QC - CustomTF 3.2.OfN			   - 30/1/2001 - //
// Idea and base code by SB-1 tech, rewritten by often	 //
//=======================================================//
// Hacker related stuff 								 //
//=======================================================*/

#include "progs.h"
#include "haxxx.h"
#include "ofndefs.h"
#include "menu.h"
#include "monsters.h"
#include "custom.h"
#include "optimize.h"
#include "debug.h"
#include "combat.h"
#include "cpstuff.h"
#include "invade.h"

namespace Progs {

// Gizmo - added these to keep myself sane while changing hack times for coop
#ifdef PR_COOP_MODE_ENHANCED

// dispenser
#define PR_COOP_HAXTIME_DISP_CAPACITY			10
#define PR_COOP_HAXTIME_DISP_ARMOR			10
#define PR_COOP_HAXTIME_DISP_UPGRADE			20

// sentry gun
#define PR_COOP_HAXTIME_SGUN_CAPACITY			10
#define PR_COOP_HAXTIME_SGUN_ARMOR			15
#define PR_COOP_HAXTIME_SGUN_CIRCUITS			20

// tesla gun
#define PR_COOP_HAXTIME_TGUN_CAPACITY			10
#define PR_COOP_HAXTIME_TGUN_ARMOR			15
#define PR_COOP_HAXTIME_TGUN_CIRCUITS			20

// teleporter
#define PR_COOP_HAXTIME_TELE_CAPACITY			5
#define PR_COOP_HAXTIME_TELE_ARMOR			10
#define PR_COOP_HAXTIME_TELE_RANGE			20

// field generator
#define PR_COOP_HAXTIME_FG_CAPACITY			10
#define PR_COOP_HAXTIME_FG_ARMOR			10
#define PR_COOP_HAXTIME_FG_RANGE			10

#endif

// dispenser
#define PR_HAXTIME_DISP_CAPACITY			15
#define PR_HAXTIME_DISP_ARMOR			10
#define PR_HAXTIME_DISP_UPGRADE			30

// sentry gun
#define PR_HAXTIME_SGUN_CAPACITY			10
#define PR_HAXTIME_SGUN_ARMOR			25
#define PR_HAXTIME_SGUN_CIRCUITS			30

// tesla gun
#define PR_HAXTIME_TGUN_CAPACITY			10
#define PR_HAXTIME_TGUN_ARMOR			25
#define PR_HAXTIME_TGUN_CIRCUITS			30

// teleporter
#define PR_HAXTIME_TELE_CAPACITY			5
#define PR_HAXTIME_TELE_ARMOR			20
#define PR_HAXTIME_TELE_RANGE			30

// field generator
#define PR_HAXTIME_FG_CAPACITY			10
#define PR_HAXTIME_FG_ARMOR			15
#define PR_HAXTIME_FG_RANGE			20

// Invade mode prematch
#define PR_HAXTIME_INVADE_PM           0 // PZ: using 0 because hack timer entity only checks every 1 second

void InitiateInterface(float input);
string GetBuildingName(entity thebuilding);
//float(float input) itob;
float GetICELevel(entity hacked);
float CheckHaxxx();
void teamsprint6(entity ignore, const string& st, const string& st2, const string& st3, const string& st4, const string& st5, const string& st6);
void MuzzleFlash(entity ent);

void Menu_Friend_Hax()
{
	if (self->demon_two == world) // safety
	{
		sprint(self,PR_PRINT_HIGH,"You have lost the connection!\n");
		ResetMenu();
		return;
	}

	string st,st2,st3,st4,st5,st6,st7;

	st6=S_("\n\nCom^b-^bLink ^bestablished^b");
	st7=S_(" ^bAction^b:                    \n\n");
	st5=S_("\n\n^8.. Hack-Repair       ^b[^b8 sec^b]^b\n\n^0.. ^bNothing^b                  ");
	st4="";
	st2=S_("\n^2.. Add ^blevel ^1^b ICE  ^b[^b20 sec^b]^b");

	if (!visible2x(self,self->demon_two))
		st6="\n\nLost connection with target!\n";
/*	  else
	{
		local float dist;
		dist = vlen(self.origin - self.demon_two.origin);
		if (dist > #HAX_UNSTABLEDIST)
		{
			st6="\n\nUnstable connection with target!\n";
		}
	}*/

	if (Teammate(self->demon_two->real_owner, self))
	{
		if (self->demon_two->classname == "building_dispenser")
		{
#ifdef PR_COOP_MODE_ENHANCED
			if ( !deathmatch ) {
				st7=S_("Target^b:^b Friendly Dispenser\n\n");
				st=   S_("^1.. Improve capacity ^b[^b" TO_STR(PR_COOP_HAXTIME_DISP_CAPACITY) " sec^b]^b"); // DONE
				st3=S_("\n^3.. Enhance armour   ^b[^b" TO_STR(PR_COOP_HAXTIME_DISP_ARMOR) " sec^b]^b"); // DONE
				st4=S_("\n^4.. Upgrade          ^b[^b" TO_STR(PR_COOP_HAXTIME_DISP_UPGRADE) " sec^b]^b");
			} else {
#endif
				st7=S_("Target^b:^b Friendly Dispenser\n\n");
				st=   S_("^1.. Improve capacity ^b[^b" TO_STR(PR_HAXTIME_DISP_CAPACITY) " sec^b]^b"); // DONE
				st3=S_("\n^3.. Enhance armour   ^b[^b" TO_STR(PR_HAXTIME_DISP_ARMOR) " sec^b]^b"); // DONE
				st4=S_("\n^4.. Upgrade          ^b[^b" TO_STR(PR_HAXTIME_DISP_UPGRADE) " sec^b]^b");
#ifdef PR_COOP_MODE_ENHANCED
			}
#endif
		}
		else if (self->demon_two->classname == "building_sentrygun")
		{
#ifdef PR_COOP_MODE_ENHANCED
			if ( !deathmatch ) {
				st7=S_("Target^b:^b Friendly Sentry Gun\n\n");
				st=   S_("^1.. Improve capacity ^b[^b" TO_STR(PR_COOP_HAXTIME_SGUN_CAPACITY) " sec^b]^b"); // DONE - increase shells only
				st3=S_("\n^3.. Enhance armour   ^b[^b" TO_STR(PR_COOP_HAXTIME_SGUN_ARMOR) " sec^b]^b");
				st4=S_("\n^4.. Enhance circuits ^b[^b" TO_STR(PR_COOP_HAXTIME_SGUN_CIRCUITS) " sec^b]^b"); // DONE
			} else {
#endif
				st7=S_("Target^b:^b Friendly Sentry Gun\n\n");
				st=   S_("^1.. Improve capacity ^b[^b" TO_STR(PR_HAXTIME_SGUN_CAPACITY) " sec^b]^b"); // DONE - increase shells only
				st3=S_("\n^3.. Enhance armour   ^b[^b" TO_STR(PR_HAXTIME_SGUN_ARMOR) " sec^b]^b");
				st4=S_("\n^4.. Enhance circuits ^b[^b" TO_STR(PR_HAXTIME_SGUN_CIRCUITS) " sec^b]^b"); // DONE
#ifdef PR_COOP_MODE_ENHANCED
			}
#endif
		}
		else if (self->demon_two->classname == "building_tesla")
		{
#ifdef PR_COOP_MODE_ENHANCED
			if ( !deathmatch ) {
				st7=S_("Target^b:^b Friendly Tesla Sentry\n\n");
				st=   S_("^1.. Improve capacity ^b[^b" TO_STR(PR_COOP_HAXTIME_TGUN_CAPACITY) " sec^b]^b"); // DONE
				st3=S_("\n^3.. Enhance armour   ^b[^b" TO_STR(PR_COOP_HAXTIME_TGUN_ARMOR) " sec^b]^b"); // DONE
				st4=S_("\n^4.. Enhance circuits ^b[^b" TO_STR(PR_COOP_HAXTIME_TGUN_CIRCUITS) " sec^b]^b"); //\n.. Add TractorBeam �20 sec�");
			} else {
#endif
				st7=S_("Target^b:^b Friendly Tesla Sentry\n\n");
				st=   S_("^1.. Improve capacity ^b[^b" TO_STR(PR_HAXTIME_TGUN_CAPACITY) " sec^b]^b"); // DONE
				st3=S_("\n^3.. Enhance armour   ^b[^b" TO_STR(PR_HAXTIME_TGUN_ARMOR) " sec^b]^b"); // DONE
				st4=S_("\n^4.. Enhance circuits ^b[^b" TO_STR(PR_HAXTIME_TGUN_CIRCUITS) " sec^b]^b"); //\n.. Add TractorBeam �20 sec�");
#ifdef PR_COOP_MODE_ENHANCED
			}
#endif
		}
		else if (self->demon_two->classname == "building_camera")
		{
			st7=S_("Target^b:^b Friendly Camera\n\n");
			st=   S_("^1.. Improve sensors  ^b[^b20 sec^b]^b"); // DONE
			st3=S_("\n^3.. Enhance armour   ^b[^b20 sec^b]^b"); // DONE
		}
		else if (self->demon_two->classname == "building_sensor")
		{
			st7=S_("Target^b:^b Friendly Motion Sensor\n\n");
			st=   S_("^1.. Improve range    ^b[^b20 sec^b]^b"); // DONE
			st3=S_("\n^3.. Enhance armour   ^b[^b10 sec^b]^b"); // DONE
			st4=S_("\n^4.. Add spy revealer ^b[^b25 sec^b]^b"); // DONE
		}
		else if (self->demon_two->classname == "building_teleporter")
		{
#ifdef PR_COOP_MODE_ENHANCED
			if ( !deathmatch ) {
				st7=S_("Target^b:^b Friendly Teleporter\n\n");
				st=   S_("^1.. Improve capacity  ^b[^b" TO_STR(PR_COOP_HAXTIME_TELE_CAPACITY) " sec^b]^b"); // DONE
				st3=S_("\n^3.. Enhance armour   ^b[^b" TO_STR(PR_COOP_HAXTIME_TELE_ARMOR) " sec^b]^b"); // DONE
				st4=S_("\n^4.. Increase range   ^b[^b" TO_STR(PR_COOP_HAXTIME_TELE_RANGE) " sec^b]^b"); // DONE
			} else {
#endif
				st7=S_("Target^b:^b Friendly Teleporter\n\n");
				st=   S_("^1.. Improve capacity  ^b[^b" TO_STR(PR_HAXTIME_TELE_CAPACITY) " sec^b]^b"); // DONE
				st3=S_("\n^3.. Enhance armour   ^b[^b" TO_STR(PR_HAXTIME_TELE_ARMOR) " sec^b]^b"); // DONE
				st4=S_("\n^4.. Increase range   ^b[^b" TO_STR(PR_HAXTIME_TELE_RANGE) " sec^b]^b"); // DONE
#ifdef PR_COOP_MODE_ENHANCED
			}
#endif
		}
		else if (self->demon_two->classname == "building_fieldgen")
		{
#ifdef PR_COOP_MODE_ENHANCED
			if ( !deathmatch ) {
				st7=S_("Target^b:^b Friendly Field Generator\n\n");
				st=   S_("^1.. Improve capacity ^b[^b" TO_STR(PR_COOP_HAXTIME_FG_CAPACITY) " sec^b]^b"); // DONE
				st3=S_("\n^3.. Enhance armour   ^b[^b" TO_STR(PR_COOP_HAXTIME_FG_ARMOR) " sec^b]^b"); // DONE
				st4=S_("\n^4.. Increase range   ^b[^b" TO_STR(PR_COOP_HAXTIME_FG_RANGE) " sec^b]^b"); // DONE
			} else {
#endif
				st7=S_("Target^b:^b Friendly Field Generator\n\n");
				st=   S_("^1.. Improve capacity ^b[^b" TO_STR(PR_HAXTIME_FG_CAPACITY) " sec^b]^b"); // DONE
				st3=S_("\n^3.. Enhance armour   ^b[^b" TO_STR(PR_HAXTIME_FG_ARMOR) " sec^b]^b"); // DONE
				st4=S_("\n^4.. Increase range   ^b[^b" TO_STR(PR_HAXTIME_FG_RANGE) " sec^b]^b"); // DONE
#ifdef PR_COOP_MODE_ENHANCED
			}
#endif
		} else
			// Gizmo - this can probably be avoided but too lazy for now ~~~
			st = st3 = "";
	}

	if (self->demon_two->all_active & PR_IMPROVED_ONE)
	{
		st="";
	}
	if (self->demon_two->all_active & PR_IMPROVED_TWO)
	{
		st2=S_("\n^2.. Add ^blevel ^2^b ICE  ^b[^b25 sec^b]^b");
		if (self->demon_two->all_active & PR_IMPROVED_FIVE)
		{
			st2=S_("\n^2.. Add ^blevel ^3^b ICE  ^b[^b30 sec^b]^b");

			if (self->demon_two->all_active & PR_IMPROVED_SIX)
			   st2="\n";
		}
	}
	if (self->demon_two->all_active & PR_IMPROVED_THREE)
	{
		st3="\n";
	}
	if (self->demon_two->classname != "building_sensor")
	{
		if (self->demon_two->all_active & PR_IMPROVED_FOUR)
		{
			st4="\n";
		}
	}
	else
	{
		if (self->demon_two->all_active & PR_IMPROVED_FOUR)
		{
			if (self->demon_two->all_active & PR_IMPROVED_SEVEN)
				st4="\n";
			else
				st4=S_("\n^4.. Thief revealer   ^b[^b25 sec^b]^b");
		}
	}

	CenterPrint7(self,st7,st,st2,st3,st4,st5,st6);
}

void Menu_FriendHax_Inp(float inp)
{
	if (self->classname != "player" || self->demon_two == world)
		return;

	if (inp == 10)
	{
		self->impulse=0;
		self->demon_two=world;
		BackFlash(self,1);
		sprint(self,PR_PRINT_LOW,"Leaving connection with device..\n");
		//CuTFMenuSound(#MENUSOUND_SELL);
		ResetMenu();
		return;
	}
	if (inp == 8)
	{
		self->impulse=0;
		InitiateInterface(inp);
		ResetMenu();
		return;
	}
	if (inp < 5 && inp > 0)
	{

		if (inp == 4)
		{
			if (self->demon_two->classname == "building_sensor")
			{
				if (self->demon_two->all_active & PR_IMPROVED_FOUR)
					inp = 7; // already has spy revealer so go for thief, check if it already has thief revealer below..
			}
			else if (self->demon_two->classname == "building_camera")
				return; // CAMERAS doenst have a 4th frienly hack
		}
		if (inp == 2 && self->demon_two->all_active & PR_IMPROVED_SIX)
			return; // device completelly iced, so do nothing

		if (self->demon_two->all_active & itob(inp) && inp != 2) // exclude ICE's
			return; // device already has this hack done, so do nothing

		InitiateInterface(inp);
		ResetMenu();
		self->impulse = 0;
	}

}

void Menu_EnemyHax_Inp(float inp)
{
	if (self->classname != "player" || self->demon_two == world)
		return;

	if (inp == 10)
	{
		self->impulse=0;
		self->demon_two=world;
		BackFlash(self,1);
		sprint(self,PR_PRINT_LOW,"Leaving connection with device..\n");
		//CuTFMenuSound(#MENUSOUND_SELL);
		ResetMenu();
		return;
	}
	if (inp < 5 && inp > 0)
	{
		if (self->demon_two->is_malfunctioning & itob(inp))
			return;

		InitiateInterface(inp);
		ResetMenu();
		self->impulse = 0;
	}

}

void Menu_Enemy_Hax()
{
	if (self->demon_two == world) // safety
	{
		sprint(self,PR_PRINT_HIGH,"You have lost the connection!\n");
		ResetMenu();
		return;
	}

	string st,st2,st3,st4,st5,st6,st7;

	st6=S_("\n\nCom^b-^bLink ^bestablished^b");
	st5=S_("\n\n^0.. ^bNothing^b                  ");
	st7=S_(" ^bAction^b:                    \n\n");

	if (!visible2x(self,self->demon_two))
		st6 = "\n\nLost connection with target!\n";
	else
	{
		float dist;
		dist = vlen(self->origin - self->demon_two->origin);
		if (dist > PR_HAX_UNSTABLEDIST)
		{
			st6 = "\n\nUnstable connection with target!\n";
		}
	}

	if (!Teammate(self->demon_two->real_owner, self))
	{
		if (self->demon_two->classname == "building_dispenser")
		{
			st7=S_("^bTarget^b: ^bEnemy^b Dispenser\n\n");
			st=S_("^1.. Disable           ^b[^b3 sec^b]^b");
			st2=S_("\n^2.. Booby trap        ^b[^b6 sec^b]^b");
			st3=S_("\n^3.. No production     ^b[^b3 sec^b]^b");
			st4=S_("\n^4.. Subtle bomb       ^b[^b4 sec^b]^b");
		}
		else if (self->demon_two->classname == "building_sentrygun")
		{
			st7=S_("^bTarget^b: ^bEnemy^b Sentry Gun\n\n");
			st=S_("^1.. Incapacitate      ^b[^b3 sec^b]^b");
			st2=S_("\n^2.. Reduce accuracy   ^b[^b5 sec^b]^b");
			st3=S_("\n^3.. RL malfunction    ^b[^b4 sec^b]^b");
			st4=S_("\n^4.. Hostile takeover ^b[^b12 sec^b]^b");
		}
		else if (self->demon_two->classname == "building_tesla")
		{
			st7=S_("^bTarget^b: ^bEnemy^b Tesla Sentry\n\n");
			st=S_("^1.. Incapacitate      ^b[^b8 sec^b]^b");
			st2=S_("\n^2.. Reduce output     ^b[^b6 sec^b]^b");
			st3=S_("\n^3.. Overload          ^b[^b7 sec^b]^b");
			st4=S_("\n^4.. Hostile takeover ^b[^b15 sec^b]^b");
		}
		else if (self->demon_two->classname == "building_camera")
		{
			st7=S_("^bTarget^b: ^bEnemy^b Camera\n\n");
			st=S_("^1.. Disable comms     ^b[^b5 sec^b]^b");
			st2=S_("\n^2.. Disable speaker   ^b[^b2 sec^b]^b");
			st3=S_("\n^3.. Sensor ghosts     ^b[^b6 sec^b]^b");
			st4=S_("\n^4.. Hostile takeover  ^b[^b5 sec^b]^b");
		}
		else if (self->demon_two->classname == "building_sensor")
		{
			st7=S_("^bTarget^b: ^bEnemy^b Motion Sensor\n\n");
			st=S_("^1.. Disable sound     ^b[^b5 sec^b]^b");
			st2=S_("\n^2.. Disable light     ^b[^b5 sec^b]^b");
			st3=S_("\n^3.. Deactivate        ^b[^b5 sec^b]^b");
			st4=S_("\n^4.. Hostile takeover  ^b[^b5 sec^b]^b");
		}
		else if (self->demon_two->classname == "building_teleporter")
		{
			st7=S_("^bTarget^b: ^bEnemy^b Teleporter\n\n");
			st=S_("^1.. Item stripper     ^b[^b3 sec^b]^b");
			st2=S_("\n^2.. Void location     ^b[^b7 sec^b]^b");
			st3=S_("\n^3.. Target builder    ^b[^b5 sec^b]^b");
			st4=S_("\n^4.. Disable           ^b[^b2 sec^b]^b");
		}
		else if (self->demon_two->classname == "building_fieldgen")
		{
			st7=S_("^bTarget^b: ^bEnemy^b Field Generator\n\n");
			st=S_("^1.. Corrupt Generator ^b[^b3 sec^b]^b");
			st2=S_("\n^2.. Engineer Trap     ^b[^b7 sec^b]^b");
			st3=S_("\n^3.. Reduce Output     ^b[^b5 sec^b]^b");
			st4=S_("\n^4.. Disable           ^b[^b2 sec^b]^b");
		} else
			// Gizmo - this can probably be avoided ~~~
			st = st2 = st3 = st4 = "";
	}

	if (self->demon_two->is_malfunctioning & PR_SCREWUP_ONE)
	{
		st="";
	}
	if (self->demon_two->is_malfunctioning & PR_SCREWUP_TWO)
	{
		st2="\n";
	}
	if (self->demon_two->is_malfunctioning & PR_SCREWUP_THREE)
	{
		st3="\n";
	}
	if (self->demon_two->is_malfunctioning & PR_SCREWUP_FOUR)
	{
		st4="\n";
	}

	CenterPrint7(self,st7,st,st2,st3,st4,st5,st6);
}

/*
===============
SBFireInterface

Uses the Cyber Interface on whatever you're looking at.

OfN - Rewritten
===============
*/

void SBFireInterface()
{
	if (CheckHaxxx() == PR_FALSE) return;

	entity oself;
	entity targ;
	int hackThis;
	string st;

	targ = self->martyr_enemy;
	hackThis = self->has_camera;

	oself = self;

	self = self->owner;

	oself->think = SUB_Remove;
	oself->nextthink = 0.1;
	dremove(oself);

	self->is_haxxxoring = 0;

	self->tfstate = self->tfstate - (self->tfstate & PR_TFSTATE_CANT_MOVE);
	TeamFortress_SetSpeed(self);

	if (hackThis == -2)
	{
		sprint(self, PR_PRINT_HIGH, "Hack aborted.\n");
		return;
	}

	if (targ->classname == "player" && (targ->cutf_items & PR_CUTF_CYBERAUG))
	{
		if (!Teammate(targ, self))
		{
			targ->is_malfunctioning = 1;
			sprint(targ, PR_PRINT_HIGH, "Oh no! Your cybernetic augmentations are malfunctioning!\n");
			sprint(self, PR_PRINT_HIGH, "You tamper with ", targ->netname, "'s cybernetic augmentations.\n");
		}
		else if (Teammate(targ, self) && targ->is_malfunctioning)
		{
			targ->is_malfunctioning = 0;
			sprint(targ, PR_PRINT_HIGH, self->netname, " adjusts your cybernetics to work properly again.\n");
			sprint(self, PR_PRINT_HIGH, "You readjust ", targ->netname, "'s CyberAugs to work properly again.\n");
		}
	}
	else if (IsBuilding(targ))
	{
		if (!Teammate(targ->real_owner, self) && GetICELevel(targ)==1 && random() < 0.2)
		{
			sprint(self, PR_PRINT_HIGH, "This device has a level 1 ICE, it's not easy to hack!\n");
			//sprint(targ.real_owner, #PRINT_HIGH,"Somebody tried to hack your stuff...\n");
			return;
		}
		if (!Teammate(targ->real_owner, self) && GetICELevel(targ)==2)
		{
			sprint(self, PR_PRINT_HIGH, "This device is hack-protected.\n");
			sprint(targ->real_owner, PR_PRINT_HIGH,"Somebody just tried to hack your stuff...\n");
			return;
		}
		if (!Teammate(targ->real_owner, self) && GetICELevel(targ)==3)
		{
			sprint(self, PR_PRINT_HIGH, "This device is hack-protected with a level 3 ICE!\n");
			deathmsg=PR_DMSG_ANTIHACK;
			TF_T_Damage(self, self, targ, self->health + 50, PR_TF_TD_IGNOREARMOUR, PR_TF_TD_OTHER);

			sound (self, PR_CHAN_MISC, "effects/crunch.wav", 1, PR_ATTN_NONE);

			return;
		}

		MuzzleFlash(targ);
		sound(targ,PR_CHAN_MISC,"weapons/cambeep.wav",1, PR_ATTN_NORM);

		BackFlash(self,1);

		if (!Teammate(targ->real_owner, self))
		{
			sprint(self, PR_PRINT_HIGH, "You make a few adjustments...\n");
			if (hackThis == PR_SCREWUP_ONE)
			{
				targ->is_malfunctioning = targ->is_malfunctioning | PR_SCREWUP_ONE;
				if (targ->classname == "building_sentrygun") //
					sprint(self, PR_PRINT_HIGH, "The sentrygun is now incapacitated.\n");
				else if (targ->classname == "building_tesla")
					sprint(self, PR_PRINT_HIGH, "The tesla won't shoot any more.\n");
				else if (targ->classname == "building_sensor")
					sprint(self, PR_PRINT_HIGH, "Better when it doesn't make so much noise.\n");
				else if (targ->classname == "building_dispenser")
					sprint(self, PR_PRINT_HIGH, "This is one dispenser that won't dispense any more.\n");
				else if (targ->classname == "building_camera")
					sprint(self, PR_PRINT_HIGH, "The camera won't be reporting to its owner any longer, but to all outward appearances remains the same.\n");
				else if (targ->classname == "building_teleporter")
					sprint(self, PR_PRINT_HIGH, "The teleporter will now strip the items off all who enter...\n");
				else if (targ->classname == "building_fieldgen")
					sprint(self, PR_PRINT_HIGH, "The field generator is now rigged to hurt teammates...\n");
			}
			else if (hackThis == PR_SCREWUP_TWO)
			{
				targ->is_malfunctioning = targ->is_malfunctioning | PR_SCREWUP_TWO;
				if (targ->classname == "building_sentrygun")
					sprint(self, PR_PRINT_HIGH, "The sentrygun's accuracy is now reduced.\n");
				else if (targ->classname == "building_tesla")
					sprint(self, PR_PRINT_HIGH, "This tesla won't put out enough amperage to hurt a fly.\n");
				else if (targ->classname == "building_camera")
					sprint(self, PR_PRINT_HIGH, "The camera is silenced.\n");
				else if (targ->classname == "building_sensor")
					sprint(self, PR_PRINT_HIGH, "That glow was always pretty annoying anyway.\n");
				else if (targ->classname == "building_dispenser")
				{
					sprint(self, PR_PRINT_HIGH, "The dispenser is now rigged to explode!\n");
					targ->martyr_enemy = self;
				}
				else if (targ->classname == "building_teleporter")
				{
					sprint(self, PR_PRINT_HIGH, "Entering this teleporter could be unhealthy...\n");
					targ->martyr_enemy = self;
				}
				else if (targ->classname == "building_fieldgen")
				{
					sprint(self, PR_PRINT_HIGH, "Better not use a spanner on this one...\n");
					//targ.martyr_enemy = self; // FIX: used on fieldgens to point to field
				}
			}
			else if (hackThis == PR_SCREWUP_THREE)
			{
				targ->is_malfunctioning = targ->is_malfunctioning | PR_SCREWUP_THREE;
				if (targ->classname == "building_sentrygun")
					sprint(self, PR_PRINT_HIGH, "The sentrygun rocket launcher will now malfunction...\n");
				else if (targ->classname == "building_tesla")
					sprint(self, PR_PRINT_HIGH, "The tesla may experience some technical difficulties the next time it fires...\n");
				else if (targ->classname == "building_camera")
					sprint(self, PR_PRINT_HIGH, "This camera will be reading ghosts now...\n");
				else if (targ->classname == "building_dispenser")
					sprint(self, PR_PRINT_HIGH, "The dispenser will no longer regenerate ammo.\n");
				else if (targ->classname == "building_teleporter")
					sprint(self, PR_PRINT_HIGH, "The teleporter will now teleport directly to its builder!\n");
				else if (targ->classname == "building_sensor")
					sprint(self, PR_PRINT_HIGH, "The sensor is now off.\n");
				else if (targ->classname == "building_fieldgen")
					sprint(self, PR_PRINT_HIGH, "The field generator won't hurt too much now.\n");
			}
			else if (hackThis == PR_SCREWUP_FOUR)
			{
				if (targ->classname != "building_dispenser" && targ->classname != "building_teleporter" && targ->classname != "building_fieldgen")
				{
					st=GetBuildingName(targ);

					sprint(targ->real_owner, PR_PRINT_HIGH,"You've lost the control of your ");
					sprint(targ->real_owner, PR_PRINT_HIGH, st);
					sprint(targ->real_owner, PR_PRINT_HIGH, "!!\n");

					sprint (self, PR_PRINT_HIGH, "The ");
					sprint (self, PR_PRINT_HIGH, st);
					sprint (self,PR_PRINT_HIGH," is now yours!\n");

					teamprefixsprint(self->team_no,self);
					teamsprint6(self,self->netname," hacked ",targ->real_owner->netname,"'s ",st,", now it's ours!\n");

					teamprefixsprint(targ->real_owner->team_no,targ->real_owner);
					teamsprint6(targ->real_owner,targ->real_owner->netname," has lost the control of his ",st,"!\n","","");

					if (targ->classname == "building_sentrygun")
					{
						targ->real_owner->has_sentry = targ->real_owner->has_sentry - (targ->real_owner->has_sentry & targ->dont_do_triggerwork);
						targ->dont_do_triggerwork = 0;
						//self.has_sentry = #TRUE;
					}
					else if (targ->classname == "building_tesla")
					{
						targ->real_owner->has_tesla = targ->real_owner->has_tesla - (targ->real_owner->has_tesla & targ->dont_do_triggerwork);
						targ->dont_do_triggerwork = 0;

						//targ.real_owner.has_tesla = #FALSE;
						//self.has_tesla = #TRUE;
					}
					else if (targ->classname == "building_camera")
					{
						targ->real_owner->has_camera = PR_FALSE;
						//self.has_camera = #TRUE;
					}
					else if (targ->classname == "building_sensor")
					{
						targ->real_owner->has_sensor = PR_FALSE;
						//self.has_sensor = #TRUE;
					}

					targ->real_owner = self;
					targ->team_no = self->team_no;
				}
				else if (targ->classname == "building_fieldgen")
				{
					sprint(self, PR_PRINT_HIGH, "The field generator is now disabled.\n");
					targ->is_malfunctioning = PR_SCREWUP_FOUR;
				}
				else if (targ->classname == "building_teleporter")
				{
					sprint(self, PR_PRINT_HIGH, "The teleporter is now broken.\n");
					targ->is_malfunctioning = PR_SCREWUP_FOUR;
				}
				else if (targ->classname == "building_dispenser")
				{
					sprint(self, PR_PRINT_HIGH, "The dispenser will explode when someone attempts to withdraw armour!\n");
					targ->martyr_enemy = self;
					targ->is_malfunctioning = PR_SCREWUP_FOUR;
				}
			}
			else
			{
				sprint(self, PR_PRINT_HIGH, "Your attempt at hacking fails.\n");
			}
		}
		else if (Teammate(targ, self) && hackThis == -1)
		{
			if (targ->is_malfunctioning)
			{
				targ->is_malfunctioning = 0;

				sprint(self, PR_PRINT_HIGH, "You correct the machine's malfunction.\n");

				st=GetBuildingName(targ);

				if (self!=targ->real_owner)
				{
					sprint(targ->real_owner, PR_PRINT_HIGH, "your ");
					sprint(targ->real_owner, PR_PRINT_HIGH, st);
					sprint(targ->real_owner, PR_PRINT_HIGH," has been fixed by ");
					sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
					sprint(targ->real_owner, PR_PRINT_HIGH, "\n");

					teamprefixsprintbi(self->team_no,self,targ->real_owner);
					teamsprintbi(self,targ->real_owner,targ->real_owner->netname,"'s malfunctioning ",st," has been fixed by ",self->netname,"\n");
				}
				else
				{
					teamprefixsprint(self->team_no,self);
					teamsprint6(self,self->netname, " fixes his malfunctioning ",st,"\n","","");
				}
			}
			else
			{
				sprint(self, PR_PRINT_HIGH, "There is nothing wrong with this machine.\n");
			}
		}
		else if (Teammate(targ, self))
		{
			//FRIENDLY IMPROVEMENTS
			targ->all_active = targ->all_active | hackThis;

			st=GetBuildingName(targ);

			sprint(self, PR_PRINT_HIGH, "You make a few adjustments...\n");

			if (hackThis == PR_IMPROVED_ONE)
			{
				if (targ->classname=="building_sensor")
				{
					sprint(self, PR_PRINT_HIGH, "The sensor has greater detection range now.\n");

					if (self != targ->real_owner)
					{
						sprint(targ->real_owner, PR_PRINT_HIGH,"your motion sensor has greater detection range thanks to ");
						sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
						sprint(targ->real_owner, PR_PRINT_HIGH, ".\n");
					}
				}
				else if (targ->classname=="building_camera")
				{
					sprint(self, PR_PRINT_HIGH, "The camera has greater range of view now.\n");

					if (self != targ->real_owner)
					{
						sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
						sprint(targ->real_owner, PR_PRINT_HIGH," hacked your camera, it has better sensors now.\n");
					}

				}
				else if (targ->classname=="building_tesla")
				{
					targ->maxammo_cells=targ->maxammo_cells*2;
					sprint(self, PR_PRINT_HIGH, "The tesla has extra cells capacity now.\n");

					if (self != targ->real_owner)
					{
						sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
						sprint(targ->real_owner, PR_PRINT_HIGH," hacked your tesla, it has extra capacity for cells now.\n");
					}


				}
				else if (targ->classname=="building_sentrygun")
				{
					targ->maxammo_shells=targ->maxammo_shells*2;
					sprint(self, PR_PRINT_HIGH, "The sentry gun has greater ammo capacity for shells now.\n");

					if (self != targ->real_owner)
					{
						sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
						sprint(targ->real_owner, PR_PRINT_HIGH," hacked your sentry gun, it has extra capacity for shells now.\n");
					}

				}
				else if (targ->classname=="building_dispenser")
				{
					sprint(self, PR_PRINT_HIGH, "The dispenser capacity was increased.\n");

					if (self != targ->real_owner)
					{
						sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
						sprint(targ->real_owner, PR_PRINT_HIGH," hacked your dispenser to increase its capacity.\n");
					}

				}
				else if (targ->classname=="building_teleporter")
				{
					targ->maxammo_cells=targ->maxammo_cells*2;

					sprint(self, PR_PRINT_HIGH, "The teleporter cells capacity was increased.\n");

					if (self != targ->real_owner)
					{
						sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
						sprint(targ->real_owner, PR_PRINT_HIGH," hacked your teleporter to increase its cells capacity.\n");
					}

				}
				else if (targ->classname=="building_fieldgen")
				{
					targ->maxammo_cells=targ->maxammo_cells*2;

					sprint(self, PR_PRINT_HIGH, "The field generator cells capacity was increased.\n");

					if (self != targ->real_owner)
					{
						sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
						sprint(targ->real_owner, PR_PRINT_HIGH," hacked your field generator to increase its cells capacity.\n");
					}

				}


			}
			else if (hackThis == PR_IMPROVED_TWO) //- ICE level 1
			{
				sprint(self, PR_PRINT_HIGH, S_("You add a ^blevel ^1^b ICE to the "));
				sprint(self, PR_PRINT_HIGH, st);
				sprint(self, PR_PRINT_HIGH,".\n");

				if (self!=targ->real_owner)
				{
					sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
					sprint(targ->real_owner, PR_PRINT_HIGH,S_(" adds a ^blevel ^1^b ICE to your "));
					sprint(targ->real_owner, PR_PRINT_HIGH, st);
					sprint(targ->real_owner, PR_PRINT_HIGH, ".\n");
				}

			}
			else if (hackThis == PR_IMPROVED_FIVE) //- ICE level 2
			{
				sprint(self, PR_PRINT_HIGH, S_("You add a ^blevel ^2^b ICE to the "));
				sprint(self, PR_PRINT_HIGH, st);
				sprint(self, PR_PRINT_HIGH,".\n");

				if (self!=targ->real_owner)
				{
					sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
					sprint(targ->real_owner, PR_PRINT_HIGH,S_(" adds a ^blevel ^2^b ICE to your "));
					sprint(targ->real_owner, PR_PRINT_HIGH, st);
					sprint(targ->real_owner, PR_PRINT_HIGH, ".\n");
				}

			}
			else if (hackThis == PR_IMPROVED_SIX) //- ICE level 3
			{
				sprint(self, PR_PRINT_HIGH, S_("You add a ^blevel ^3^b ICE to the "));
				sprint(self, PR_PRINT_HIGH, st);
				sprint(self, PR_PRINT_HIGH,".\n");

				if (self!=targ->real_owner)
				{
					sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
					sprint(targ->real_owner, PR_PRINT_HIGH,S_(" adds a ^blevel ^3^b ICE to your "));
					sprint(targ->real_owner, PR_PRINT_HIGH, st);
					sprint(targ->real_owner, PR_PRINT_HIGH, ".\n");
				}

			}
			else if (hackThis == PR_IMPROVED_THREE)
			{
				sprint(self, PR_PRINT_HIGH, "You enhance the armour of the ");
				sprint(self, PR_PRINT_HIGH, st);
				sprint(self, PR_PRINT_HIGH,".\n");

				if (self!=targ->real_owner)
				{
					sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
					sprint(targ->real_owner, PR_PRINT_HIGH," enhances the armour of your ");
					sprint(targ->real_owner, PR_PRINT_HIGH, st);
					sprint(targ->real_owner, PR_PRINT_HIGH, ".\n");
				}

				float num;

				num=targ->health/targ->max_health;
				targ->max_health=floor(targ->max_health*2); // Gizmo - changed hacking armor back to 2
				targ->health=num*targ->max_health;

			}
			else if (hackThis == PR_IMPROVED_SEVEN)
			{
				if (targ->classname=="building_sensor")
				{
					sprint(self, PR_PRINT_HIGH, "The sensor will be able to uncover thieves now.\n");

					if (self!=targ->real_owner)
					{
						sprint(targ->real_owner, PR_PRINT_HIGH, "your motion sensor can now uncover thieves after ");
						sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
						sprint(targ->real_owner, PR_PRINT_HIGH, "'s hack.\n");
					}
				}
			}
			else if (hackThis == PR_IMPROVED_FOUR)
			{
				if (targ->classname=="building_dispenser")
				{
					sprint(self, PR_PRINT_HIGH, "The dispenser has been upgraded.\n");

					if (self!=targ->real_owner)
					{
						sprint(targ->real_owner, PR_PRINT_HIGH, "your dispenser has been upgraded by ");
						sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
						sprint(targ->real_owner, PR_PRINT_HIGH, "\n");
					}
				}
				else if (targ->classname=="building_sensor")
				{
					sprint(self, PR_PRINT_HIGH, "The sensor will be able to uncover spies now.\n");

					if (self!=targ->real_owner)
					{
						sprint(targ->real_owner, PR_PRINT_HIGH, "your motion sensor can now detect spies after ");
						sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
						sprint(targ->real_owner, PR_PRINT_HIGH, "'s hack.\n");
					}
				}
				else if (targ->classname=="building_teleporter")
				{
					sprint(self, PR_PRINT_HIGH, "The teleporter had its range increased.\n");

					if (self!=targ->real_owner)
					{
						sprint(targ->real_owner, PR_PRINT_HIGH, "your teleporter had its range increased by ");
						sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
						sprint(targ->real_owner, PR_PRINT_HIGH, "'s hack.\n");
					}
				}
				else if (targ->classname=="building_fieldgen")
				{
					sprint(self, PR_PRINT_HIGH, "The field generator had its range increased.\n");

					if (self!=targ->real_owner)
					{
						sprint(targ->real_owner, PR_PRINT_HIGH, "your field generator had its range increased by ");
						sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
						sprint(targ->real_owner, PR_PRINT_HIGH, "'s hack.\n");
					}
				}
				else if (targ->classname=="building_tesla")
				{
					sprint(self, PR_PRINT_HIGH, "The tesla has a better chip-set now.\n");

					if (self!=targ->real_owner)
					{
						sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
						sprint(targ->real_owner, PR_PRINT_HIGH, " enhances your tesla main circuits.\n");
					}
				}
				else if (targ->classname=="building_sentrygun")
				{
					sprint(self, PR_PRINT_HIGH, "The sentry gun has a better chip-set now.\n");

					if (self!=targ->real_owner)
					{
						sprint(targ->real_owner, PR_PRINT_HIGH, self->netname);
						sprint(targ->real_owner, PR_PRINT_HIGH, " enhances your sentry gun main circuits.\n");
					}
				}
			}
			else
			{
				sprint(self, PR_PRINT_HIGH, "BUG!: unknown hack value BUG! (wtf u hacked)\n");
				RPrint(self->netname);
				RPrint(" just made an unknown hack. (bug)\n");
			}
		}
	}

	//else
	//	sprint(self, #PRINT_HIGH, "There is nothing there to hack!\n");
}

float ReturnFHackDelay(entity hacker, entity hacked, float r);

// SB hax0r tool - Cyber Interface
// OfN - Rewritten
void InitiateInterface(float input)
{
	entity te;

	if (!visible2x(self,self->demon_two))
	{
		sprint(self, PR_PRINT_HIGH, "You are not connected to the target.\n");
		self->demon_two=world;
		return;
	}

	// DOUBLE CHECK, THIS ONE AFTER MENU, THE OTHER ON JOB_HACKER //
	if (self->is_feigning)
	{
		sprint(self, PR_PRINT_HIGH, "You cannot hack while feigning.\n");
		return;
	}
	if (self->is_detpacking || self->is_toffingadet)
	{
		sprint(self, PR_PRINT_HIGH, "You can't hack something while setting a detpack.\n");
		return;
	}
	if (self->is_building)
	{
		sprint(self, PR_PRINT_HIGH, "You can't hack while building!\n");
		return;
	}
	// DOUBLE CHECK, THIS ONE AFTER MENU, THE OTHER ON JOB_HACKER //

	self->is_haxxxoring = 1;

	te = spawnServerSide(); // PZ: make it a server-side only entity
	te->owner = self;
	te->classname = "timer";
	te->origin = self->demon_two->origin;
	te->nextthink = time + 1;

	te->has_holo = self->demon_two->team_no; // checked on think to break hacking if changes of team

	te->has_camera = itob(input);

	if (!Teammate(self->demon_two, self))
	{
		te->heat = ReturnHackDelay(self->demon_two, input);

		if (GetICELevel(self->demon_two)==1) // if it has a level1 ice add more delay to the hack
			te->heat=te->heat*2.5;

		string st;
		st=GetBuildingName(self->demon_two);

		teamprefixsprint(self->team_no,self);
		teamsprint6(self,self->netname," started to hack ",self->demon_two->real_owner->netname,"'s ",st,"\n");

		float dist;
		dist = vlen(self->origin - self->demon_two->origin);
		if (dist > PR_HAX_UNSTABLEDIST)
		{
			sprint(self,PR_PRINT_HIGH,"Connection unstable, it will take longer...");
			te->heat=te->heat*2;
		}
	}
	else
	{
		te->heat = ReturnFHackDelay(self, self->demon_two, input);
		if (input == 2 && self->demon_two->all_active & PR_IMPROVED_TWO)
		{
			te->has_camera = PR_IMPROVED_FIVE;
			if (self->demon_two->all_active & PR_IMPROVED_FIVE)
				te->has_camera = PR_IMPROVED_SIX;
		}
		// PZ: The above takes care of changing `has_camera` to higher security levels as needed, but doesn't take care of the sensor's thief revealer
		// secondary function on input 4. Bots can't seem to hack thief. I don't know why this isn't a problem for humans. It has something to do with
		// how hacks are set up for real players versus how they are set up for bots. But I'm adding the sensor consideration here to fix it for bots.
		else if (input == 4 && self->demon_two->classname == "building_sensor" && self->demon_two->all_active & PR_IMPROVED_FOUR)
			te->has_camera = PR_IMPROVED_SEVEN;

		else if (input == 8) te->has_camera = -1;
	}

	te->martyr_enemy = self->demon_two;
	te->think = SBHackDotTimerThink;

	self->tfstate = self->tfstate | PR_TFSTATE_CANT_MOVE;
	TeamFortress_SetSpeed(self);

	if (te->has_camera==-1)
		sprint (self, PR_PRINT_HIGH, "Scanning device for enemy modifications...\n");
	else
		sprint (self, PR_PRINT_HIGH, "You immerse yourself in the cybernet...\n");

	sound(self->demon_two,PR_CHAN_MISC,"weapons/cambeep.wav",1, PR_ATTN_NORM);

	makeImmune(self,time + te->heat); //- OfN - WORKS? yup

	te->ltime = te->heat;
	self->demon_two=te;
}

float GetICELevel(entity hacked)
{
	float retval;
	retval=0;

	if (hacked->all_active & PR_IMPROVED_TWO)
		retval=1;
	if (hacked->all_active & PR_IMPROVED_FIVE)
		retval=2;
	if (hacked->all_active & PR_IMPROVED_SIX)
		retval=3;

	return retval;

}

float ReturnHackDelay(entity hacked, float r)
{
	if (hacked->classname == "player")
		return 5; // cyberaug hack
	else if (hacked->classname == "building_dispenser")
	{
		if (r == 1)
			return 3;
		else if (r == 2)
			return 6;
		else if (r == 3)
			return 3;
		else
			return 4;
	}
	else if (hacked->classname == "building_sentrygun")
	{
		if (r == 1)
			return 3;
		else if (r == 2)
			return 5;
		else if (r == 3)
			return 4;
		else
			return 12;
	}
	else if (hacked->classname == "building_tesla")
	{
		if (r == 1)
			return 8;
		else if (r == 2)
			return 6;
		else if (r == 3)
			return 7;
		else
			return 15;
	}
	else if (hacked->classname == "building_sensor")
		return 5;
	else if (hacked->classname == "building_camera")
	{
		if (r == 1)
			return 5;
		else if (r == 2)
			return 2;
		else if (r == 3)
			return 6;
		else
			return 5;
	}
	else if (hacked->classname == "building_teleporter")
	{
		if (r == 1)
			return 3;
		else if (r == 2)
			return 7;
		else if (r == 3)
			return 5;
		else
			return 2;
	}
	else if (hacked->classname == "building_fieldgen")
	{
		if (r == 1)
			return 3;
		else if (r == 2)
			return 7;
		else if (r == 3)
			return 5;
		else
			return 2;
	}
	return 0;  // PZ: added this line
}

float ReturnFHackDelay(entity hacker, entity hacked, float r)
{
	if (Invade_buffPlayer(hacker))
		return PR_HAXTIME_INVADE_PM;

	if (hacked->classname == "player")
		return 5; // cyberaug hack
	else if (r == 8) return 8; // repairing..
	else if (r == 2 && IsBuilding(hacked))
	{
		if (!(hacked->all_active & PR_IMPROVED_TWO))
			return 20; // level 1 ICE lasts for 20 seconds always
		if (!(hacked->all_active & PR_IMPROVED_FIVE))
			return 25; // level 2 ICE lasts for 25 seconds always
		if (!(hacked->all_active & PR_IMPROVED_SIX))
			return 30;
	}
	else if (hacked->classname == "building_dispenser")
	{
#ifdef PR_COOP_MODE_ENHANCED
		if ( !deathmatch ) {
			if (r == 1)
				return PR_COOP_HAXTIME_DISP_CAPACITY;
			else if (r == 3)
				return PR_COOP_HAXTIME_DISP_ARMOR;
			else
				return PR_COOP_HAXTIME_DISP_UPGRADE;
		} else {
#endif
			if (r == 1)
				return PR_HAXTIME_DISP_CAPACITY;
//			else if (r == 2)
//				return 30;
			else if (r == 3)
				return PR_HAXTIME_DISP_ARMOR;
			else
				return PR_HAXTIME_DISP_UPGRADE;
#ifdef PR_COOP_MODE_ENHANCED
		}
#endif
	}
	else if (hacked->classname == "building_sentrygun")
	{
#ifdef PR_COOP_MODE_ENHANCED
		if ( !deathmatch ) {
			if (r == 1)
				return PR_COOP_HAXTIME_SGUN_CAPACITY;
			else if (r == 3)
				return PR_COOP_HAXTIME_SGUN_ARMOR;
			else
				return PR_COOP_HAXTIME_SGUN_CIRCUITS;
		} else {
#endif
			if (r == 1)
				return PR_HAXTIME_SGUN_CAPACITY;
//			else if (r == 2)
//				return 30;
			else if (r == 3)
				return PR_HAXTIME_SGUN_ARMOR;
			else
				return PR_HAXTIME_SGUN_CIRCUITS;
#ifdef PR_COOP_MODE_ENHANCED
		}
#endif
	}
	else if (hacked->classname == "building_tesla")
	{
#ifdef PR_COOP_MODE_ENHANCED
		if ( !deathmatch ) {
			if (r == 1)
				return PR_COOP_HAXTIME_TGUN_CAPACITY;
			else if (r == 3)
				return PR_COOP_HAXTIME_TGUN_ARMOR;
			else
				return PR_COOP_HAXTIME_TGUN_CIRCUITS;
		} else {
#endif
			if (r == 1)
				return PR_HAXTIME_TGUN_CAPACITY;
//			else if (r == 2)
//				return 30;
			else if (r == 3)
				return PR_HAXTIME_TGUN_ARMOR;
			else
				return PR_HAXTIME_TGUN_CIRCUITS;
#ifdef PR_COOP_MODE_ENHANCED
		}
#endif
	}
	else if (hacked->classname == "building_sensor")
	{
		if (r == 1)
			return 20;
//		else if (r == 2)
//			return 30;
		else if (r == 3)
			return 10;
		else //if (r == 4)
			return 25;
		//else
		//	  return 8;
	}
	else if (hacked->classname == "building_camera")
	{
		if (r == 1)
			return 20;
//		else if (r == 2)
//			return 30;
		else // if (r == 3)
			return 20;
		//else
		//	return 8;
	}
	else if (hacked->classname == "building_teleporter")
	{
#ifdef PR_COOP_MODE_ENHANCED
		if ( !deathmatch ) {
			if (r == 1)
				return PR_COOP_HAXTIME_TELE_CAPACITY;
			else if (r == 3)
				return PR_COOP_HAXTIME_TELE_ARMOR;
			else
				return PR_COOP_HAXTIME_TELE_RANGE;
		} else {
#endif
			if (r == 1)
				return PR_HAXTIME_TELE_CAPACITY;
			else if (r == 3)
				return PR_HAXTIME_TELE_ARMOR;
			else
				return PR_HAXTIME_TELE_RANGE;
#ifdef PR_COOP_MODE_ENHANCED
		}
#endif
	}
	else if (hacked->classname == "building_fieldgen")
	{
#ifdef PR_COOP_MODE_ENHANCED
		if ( !deathmatch ) {
			if (r == 1)
				return PR_COOP_HAXTIME_FG_CAPACITY;
			else if (r == 3)
				return PR_COOP_HAXTIME_FG_ARMOR;
			else
				return PR_COOP_HAXTIME_FG_RANGE;
		} else {
#endif
			if (r == 1)
				return PR_HAXTIME_FG_CAPACITY;
			else if (r == 3)
				return PR_HAXTIME_FG_ARMOR;
			else
				return PR_HAXTIME_FG_RANGE;
#ifdef PR_COOP_MODE_ENHANCED
		}
#endif
	}

	return 999; // hehe, punishment to the bug generator :)
}

// PZ NOTE: `self` here is likely a timer entity; and `self.owner` is the player associated with the timer
float CheckHaxxx()
{
	float retval;
	retval = PR_TRUE;

	if (self->martyr_enemy == world) // BUG!
		retval = PR_FALSE;
	else if (!(IsBuilding(self->martyr_enemy))) // BUG! 2
		retval = PR_FALSE;
	else if (self->martyr_enemy->health <= 0)
	{
		sprint(self->owner, PR_PRINT_HIGH, "Your target has been destroyed!\n");

		if (self->has_camera != -2)
		{
			deathmsg=PR_DMSG_CYBERNET;
			TF_T_Damage(self->owner, self->owner, self->martyr_enemy, self->owner->health + 10, PR_TF_TD_IGNOREARMOUR, PR_TF_TD_OTHER);
			sound(self->owner, PR_CHAN_MISC, "effects/crunch.wav", 1, PR_ATTN_NONE);
		}

		retval = PR_FALSE;
	}
	else if (self->martyr_enemy->team_no != self->has_holo) // target switched team? if so.. abort
	{
		sprint(self->owner, PR_PRINT_HIGH, "The communication has been lost!\n");
		retval = PR_FALSE;
	}
	else if (self->has_camera == -2)
	{
		retval = PR_TRUE;
	}
	else if (Teammate(self->martyr_enemy, self->owner) &&
	         self->martyr_enemy->all_active & self->has_camera && self->has_camera != -1)
	{
		sprint(self->owner, PR_PRINT_HIGH, "Somebody improved that first.\n");
		retval = PR_FALSE;
	}
	else if (!Teammate(self->martyr_enemy, self->owner) &&
	         self->martyr_enemy->is_malfunctioning & self->has_camera)
	{
		sprint(self->owner, PR_PRINT_HIGH, "Somebody screwed that up first! heh\n");
		retval = PR_FALSE;
	}

	if (!retval)
	{
		self->owner->is_haxxxoring = 0;
		self->owner->tfstate = self->tfstate - (self->tfstate & PR_TFSTATE_CANT_MOVE);
		TeamFortress_SetSpeed(self->owner);
		self->think = SUB_Remove;
		self->nextthink = time + 0.1;
		dremove(self);
	}

	return retval;
}

void SBHackDotTimerThink()
{
	if (CheckHaxxx() == PR_FALSE) return;

	vector org;

	if (random() < 0.4) //-ofn was 0.3
	{
		org = self->martyr_enemy->origin;
		if (self->martyr_enemy->classname == "building_tesla" && self->martyr_enemy->tf_items & PR_NIT_TURRET)
			org[Z] = org[Z] - 7; // - 32;
		else if (self->martyr_enemy->classname == "building_tesla")
			org[Z] = org[Z] + 7; // + 32;
		else if (self->martyr_enemy->classname == "building_sentrygun" && self->martyr_enemy->tf_items & PR_NIT_TURRET)
			org[Z] = org[Z] - 16;
		else if (self->martyr_enemy->classname == "building_sentrygun")
			org[Z] = org[Z] + 16;
		else if (self->martyr_enemy->classname == "building_dispenser")
			org[Z] = org[Z] + 16;
		else if (self->martyr_enemy->classname == "building_fieldgen")
			org[Z] = org[Z] + 32;

		if (random() < 0.20)
		{
			WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
			WriteByte (PR_MSG_BROADCAST, PR_TE_GUNSHOT);
			WriteByte (PR_MSG_MULTICAST, 4); // Original 3

			WriteCoord (PR_MSG_BROADCAST, org[X]);
			WriteCoord (PR_MSG_BROADCAST, org[Y]);
			WriteCoord (PR_MSG_BROADCAST, org[Z]);
			multicast (org, PR_MULTICAST_PVS);
			MuzzleFlash(self->martyr_enemy);
		}
		else
		{
			WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
			WriteByte (PR_MSG_BROADCAST, PR_TE_GUNSHOT);
			WriteByte (PR_MSG_MULTICAST, 3); // Original 3

			WriteCoord (PR_MSG_BROADCAST, org[X]);
			WriteCoord (PR_MSG_BROADCAST, org[Y]);
			WriteCoord (PR_MSG_BROADCAST, org[Z]);
			multicast (org, PR_MULTICAST_PVS);
		}

	}

	self->heat = self->heat - 1;

	if (self->heat <= 0)
	{
		SBFireInterface();
		return;
	}

	self->nextthink = time + 1;
}

void JobHacker()
{
	vector source;

	self->job_finished = time + PR_HACKER_ATTEMPTRATE;

	if (self->is_haxxxoring)
	{
		if (self->demon_two->has_camera == -2) return; // if we ralready aborting a hack...
		sprint(self, PR_PRINT_HIGH, "Aborting hack...\n");
		self->demon_two->heat = 1 + 2 * random();
		self->demon_two->has_camera = -2;
		return;
	}
	if (self->is_feigning)
	{
		sprint(self, PR_PRINT_HIGH, "You cannot hack while feigning.\n");
		return;
	}
	if (self->is_detpacking || self->is_toffingadet)
	{
		sprint(self, PR_PRINT_HIGH, "You can't hack something while setting a detpack.\n");
		return;
	}
	if (self->is_building)
	{
		sprint(self, PR_PRINT_HIGH, "You can't hack while building!\n");
		return;
	}

	source = self->origin + V({0, 0, 16});

	traceline (source, source + v_forward*500, PR_TL_ANY_SOLID, self);

	if ( (IsBuilding(trace_ent) && trace_ent->classname != "building_sentrygun_base") || trace_ent->classname == "player")
	{
		if (!Teammate(trace_ent, self))
		{
			if (trace_ent->is_malfunctioning == PR_SCREWUP_ONE + PR_SCREWUP_TWO + PR_SCREWUP_THREE + PR_SCREWUP_FOUR)
			{
				sprint(self, PR_PRINT_HIGH, "You couldn't possibly screw that up more than it already is.\n");
				return;
			}
			else if (IsBuilding(trace_ent) && trace_ent->classname != "building_sentrygun_base")
			{
				self->current_menu = PR_MENU_E_HACK;
				self->menu_count = PR_MENU_REFRESH_RATE;
			}
		}
		else if (IsBuilding(trace_ent) && trace_ent->classname != "building_sentrygun_base")
		{
			self->current_menu = PR_MENU_F_HACK;
			self->menu_count = PR_MENU_REFRESH_RATE;
		}

		self->demon_two = trace_ent;

		if (trace_ent->classname=="player")
		{
			if (!(trace_ent->cutf_items & PR_CUTF_CYBERAUG))
			{
				sprint(self, PR_PRINT_HIGH, "You can't hack him.\n");
				ResetMenu();
				self->demon_two=world;
				return;
			}

			sprint (self, PR_PRINT_HIGH, "\nScanning for cyberaug equipment...\n");
			InitiateInterface(0);
			ResetMenu();
		}
		else
		{
			sprint (self, PR_PRINT_HIGH, "\nEstablishing connection...\n");

			string st;

			st=GetBuildingName(self->demon_two);

			sprint (self, PR_PRINT_HIGH, S_("Target ^btype^b: "));
			sprint (self, PR_PRINT_HIGH, st);
			sprint (self, PR_PRINT_HIGH, S_("\nTarget ^bowner^b: "));
			sprint (self, PR_PRINT_HIGH, self->demon_two->real_owner->netname);

			if (Teammate(self, self->demon_two))
				sprint (self, PR_PRINT_HIGH, "\nFriendly device.\n");
			else
				sprint (self, PR_PRINT_HIGH, S_("\n^bEnemy^b device!\n"));

			// Create Lightning Signal
			msg_entity = self;
			WriteByte (PR_MSG_ONE, PR_SVC_TEMPENTITY);
			WriteByte (PR_MSG_ONE, PR_TE_LIGHTNING2);
			WriteEntity (PR_MSG_ONE, self);
			WriteCoord (PR_MSG_ONE, self->origin[X]);
			WriteCoord (PR_MSG_ONE, self->origin[Y]);
			WriteCoord (PR_MSG_ONE, self->origin[Z]);
			WriteCoord (PR_MSG_ONE, self->demon_two->origin[X]);
			WriteCoord (PR_MSG_ONE, self->demon_two->origin[Y]);
			WriteCoord (PR_MSG_ONE, self->demon_two->origin[Z]);

			// flash machine - Nah..
			/*WriteByte (#MSG_MULTICAST, #SVC_MUZZLEFLASH);
			WriteEntity (#MSG_MULTICAST, self.demon_two);
			multicast (self.demon_two.origin, #MULTICAST_PVS);*/

			stuffcmd(self,"playvol misc/ffact.wav 0.4\n");

			BackFlash(self,1);
		}
	}
	else
	{
		sprint (self, PR_PRINT_HIGH, "There is nothing there to hack!\n");
		CuTFMenuSound(PR_MENUSOUND_WRONG);
		return;
	}
}

} // END namespace Progs
