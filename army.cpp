/*=======================================================//
// ARMY.QC - CustomTF 3.2.OfN			   - 30/1/2001 - //
// by Sergio Fuma�a Grunwaldt - OfteN [cp]				 //
=========================================================//
 Army job stuff - Revamped on 20/06/2004
=========================================================*/

#include "progs.h"
#include "army.h"
#include "grunty.h"
#include "ofndefs.h"
#include "tfdefs.h"
#include "debug.h"
#include "menu.h"
#include "warlock.h"
#include "jobs.h"
#include "monsters.h"
#include "cpstuff.h"
#include "triggers.h"
#include "tforttm.h"
#include "neo.h"

namespace Progs {

/*================================================================================

ARMY RATING/PRESTIGE IS CLACULATED WITH:
----------------------------------------

AverageTeamScore When started beeing army player


EXPLANATION OF HOW THE ENTITY FIELDS ARE USED (thnx? np.. :P)
---------------------------------------------

Entity fields used for player:
------------------------------

.dont_do_triggerwork - Army rating, its a float: 0 = 0%  1 = 100%
.else_goal			 - Difference between player frags and average of his team when started beeing army guy

=================================================================================*/

float CheckArea(entity obj, entity builder);
void PrintArmyTime(entity player);
void PrintFromSoldier(entity sld, entity player, const string& msg, float priority);
entity GetArmyTimer(entity player);
string GetEnemyName(entity thething);
float TeleSoldier(float slot);

float GetFreeReserve(entity player);
float GetMyAverageTeamScore(entity player);
void ArmyUpdateRating(entity player);
void AdjustArmyRating(entity player, float avg_score);

// OfN moved files..
void Grunty_PrintDetails( entity dest, entity g, float printLevel);
void grunty_stand();
void grunty_run();
void GruntyCheckFire();
void grunty_pain(entity attacker, float damage);
void grunty_touch();

void grunty_pain1();
void grunty_axepain1();

void grunty_setstartframe(entity grunty);

/*
$frame stand1 stand2 stand3 stand4 stand5 stand6 stand7 stand8 stand9
$frame stand10 stand11 stand12 stand13
*/

void JobArmy()
{
	if (self->current_menu == PR_MENU_ARMY)
	{
		// Gizmo - this should just always switch to the tactical menu to make impulse scripts easier
//		if (self.job & #JOB_EXTRA1 || self.job & #JOB_EXTRA2)
//			Menu_Army_Input(7); // yeah hackish.. but does the job
//		else
//		{
//			if (self.demon_one)
				Menu_Army_Input(6); // same comment :P
//		}

		return;
	}

	self->current_menu = PR_MENU_ARMY;
	self->menu_count = PR_MENU_REFRESH_RATE;
}

void Menu_Army1(entity soul, float currentsoul);
void Menu_Army2(entity soul, float currentsoul);
void Menu_Army3(entity soul, float currentsoul);

void Menu_Army()
{
	float currentsoul;
	currentsoul = GetCurrentSoul(self);

	entity soul;
	soul = GetSummon(self,currentsoul);

	if (soul != world)
	{
		if (soul->is_haxxxoring == 1)
			Menu_Army2(soul, currentsoul);		// tactical menu
		else if ( soul->is_haxxxoring == 2 )
			Menu_Army3( soul, currentsoul );	// allowed weapons menu
		else
			Menu_Army1(soul, currentsoul);		// normal soldier menu
	}
	else
		Menu_Army1(soul, currentsoul); // army tele menu
}

void Menu_Army1(entity soul, float currentsoul)
{
	float readytime,temp;
	string st,st2,st3;
	string st4, st5;
	float extras;

	st2 = st3 = "";

	extras = GetJobExtras(self);

	if (soul != world)
	{
		if (soul->martyr_enemy==world && soul->demon_two==world)
			st3 = S_("  ^1.. Set ^bprimary^b waypoint    \n  ^2.. Set ^bsecondary^b waypoint  ");

		else if (soul->martyr_enemy!=world && soul->demon_two==world && visible2(soul,soul->martyr_enemy))
			st3 = S_(" ^1.. Set ^bprimary^b waypoint    \n  ^2.. Set ^bsecondary^b waypoint  ");
		else if (soul->martyr_enemy!=world && soul->demon_two==world)
			st3 = S_("\x8B ^1.. Set ^bprimary^b waypoint    \n  ^2.. Set ^bsecondary^b waypoint  ");

		else if (soul->martyr_enemy==world && soul->demon_two!=world && visible2(soul,soul->demon_two))
			st3 = S_("  ^1.. Set ^bprimary^b waypoint    \n ^2.. Set ^bsecondary^b waypoint  ");
		else if (soul->martyr_enemy==world && soul->demon_two!=world)
			st3 = S_("  ^1.. Set ^bprimary^b waypoint    \n\x8B ^2.. Set ^bsecondary^b waypoint  ");

		else if (soul->martyr_enemy!=world && soul->demon_two!=world && visible2(soul,soul->demon_two) && visible2(soul,soul->martyr_enemy))
			st3 = S_(" ^1.. Set ^bprimary^b waypoint    \n ^2.. Set ^bsecondary^b waypoint  ");
		else if (soul->martyr_enemy!=world && soul->demon_two!=world && visible2(soul,soul->demon_two))
			st3 = S_("\x8B ^1.. Set ^bprimary^b waypoint    \n ^2.. Set ^bsecondary^b waypoint  ");
		else if (soul->martyr_enemy!=world && soul->demon_two!=world && visible2(soul,soul->martyr_enemy))
			st3 = S_(" ^1.. Set ^bprimary^b waypoint    \n\x8B ^2.. Set ^bsecondary^b waypoint  ");
		else if (soul->martyr_enemy!=world && soul->demon_two!=world)
			st3 = S_("\x8B ^1.. Set ^bprimary^b waypoint    \n\x8B ^2.. Set ^bsecondary^b waypoint  ");

		if (!extras)
		{
			if (self->delay_time == -1)
				st2=S_(" ^bAction^b: ^<soldier waiting^>\n\n");
			else if (self->delay_time != PR_FALSE)
				st2=S_(" ^bAction^b: ^<soldier ready^>  \n\n");
			else
				st2=S_(" ^bAction^b:                  \n\n");
		}
		else
		{
			if (soul->increase_team1 == 0)
				st2=S_("^bSoldier #1^b:");
			else if (soul->increase_team1 == 1)
				st2=S_("^bSoldier #2^b:");
			else if (soul->increase_team1 == 2)
				st2=S_("^bSoldier #3^b:");

			if (self->delay_time == -1)
				st2 = strcat(st2,S_(" ^<soldier waiting^>\n\n"));
			else if (self->delay_time != PR_FALSE)
				st2 = strcat(st2,S_(" ^<soldier ready^>  \n\n"));
			else
				st2 = strcat(st2,"                  \n\n");
		}

		st4=S_("\n^3.. Follow me^b!^b            \n^4.. Stay there^b!^b           \n^5.. Use your waypoints^b!^b   ");

		if (soul->penance_time==1)
			st4=S_("\n* ^3.. Follow me^b!^b              \n^4.. Stay there^b!^b           \n^5.. Use your waypoints^b!^b   ");
		else if (soul->penance_time==2)
			st4=S_("\n^3.. Follow me^b!^b            \n* ^4.. Stay there^b!^b             \n^5.. Use your waypoints^b!^b   ");
		else if (soul->penance_time==3)
			st4=S_("\n^3.. Follow me^b!^b            \n^4.. Stay there^b!^b           \n* ^5.. Use your waypoints^b!^b     ");

#ifndef PR_NO_GRUNTY_EMERGENCY_TELE
		if (!extras)
			st5 = S_("\n\n^6.. ^bTactical^b              \n\n^8^b..^b Detonate soldier brain\n\n^9.. Emergency teleport    \n\n^0.. ^bNothing^b               \n");
		else
			st5 = S_("\n\n^6.. ^bTactical^b   ^7.. ^bReserve^b\n\n^8^b..^b Detonate soldier brain\n\n^9.. Emergency teleport    \n\n^0.. ^bNothing^b               \n");
#else
		if (!extras)
			st5 = S_("\n\n^6.. ^bTactical^b              \n\n^8^b..^b Detonate soldier brain\n\n^0.. ^bNothing^b               \n");
		else
			st5 = S_("\n\n^6.. ^bTactical^b   ^7.. ^bReserve^b\n\n^8^b..^b Detonate soldier brain\n\n^0.. ^bNothing^b               \n");
#endif

		CenterPrint7(self, soul->undercover_name, soul->netname,"\n\n", st2, st3,st4,st5);
		return;
	}
	else // so.. soul == world
	{
		// First, get army rating string
		st5 = ftos(rint(self->dont_do_triggerwork*100));
		st5 = colstr(st5,PR_COLSTR_NUMBER);
		st5 = strcat(S_("\x9D\x9E\x9F ^bArmy Rating^b: "),st5);
		st5 = strcat(st5,S_("^b%^b \x9D\x9E\x9F\n"));

		self->PR_cprint_fx = self->PR_cprint_fx + 1;

		// Update army rating at a moderate frequency as it calculates the average team score and it's heavy
		if (self->PR_cprint_fx & 8)
			ArmyUpdateRating(self);

		if (!extras)
		{
			// Gizmo - does this fix the bug where it says in-use/ready for reserve when you don't have reserve?
			// if ( self.delay_time > 0 )
			if ( self->delay_time != -1 && self->delay_time != PR_FALSE )
				self->delay_time = currentsoul + 1;
			st3 = S_("^bAction^b:                  \n\n");
			st4 = S_("^0.. ^bNothing^b               \n\n");
		}
		else
		{
			if (currentsoul == 0)
				st3 = S_("^bReserve^b ^b#1^b:              \n\n");
			else if (currentsoul == 1)
				st3 = S_("^bReserve^b ^b#2^b:              \n\n");
			else if (currentsoul == 2)
				st3 = S_("^bReserve^b ^b#3^b:              \n\n");

			st4 = S_("^0.. ^bNothing^b    ^7.. ^bReserve^b\n\n");
		}

		if (self->delay_time - 1 == currentsoul) // We point to a ready soldier
		{
			//CenterPrint4(self, st3,"\1.. Teleport soldier      \n\n",st4,"\nReady for teleporting!");
			if (self->PR_cprint_fx & 1)
				CenterPrint5(self, st3,S_("^1.. Teleport soldier^b!^b     \n\n"),st4,st5,"\nReady for teleporting!");
			else
				CenterPrint5(self, st3,S_("^1.. Teleport soldier^b!^b     \n\n"),st4,st5,S_("\n^bReady for teleporting^b^b!^b"));
		}
		else
		{
			if (self->delay_time == PR_FALSE) // ArmyTimer running
			{
				entity ArmyTimer;
				ArmyTimer=GetArmyTimer(self);

				if (ArmyTimer == world)
				{
					CenterPrint(self,"ERROR: Unable to find ArmyTimer entity for player");
					return;
				}

				if (ArmyTimer->job - 1 == currentsoul)
				{
					readytime=fabs(floor(ArmyTimer->nextthink - time));
					if (readytime<60)
					{
					  st=ftos(readytime);
					  st2=" seconds";
					}
					else
					{
					  temp=floor(readytime/60);//+1;
					  if (temp>1)
						st2=" minutes";
					  else
						st2=" minute";

					  st=ftos(floor(readytime/60));//+1);
					}

					//CenterPrint6(self, st3,"\n\n",st4,"\n�Preparing teleporting...�\n\n���� ����: ",st,st2);
					if (self->PR_cprint_fx & 1)
						CenterPrint7(self, st3,"\n\n",st4,st5,S_("\n^[Preparing teleporting...^]\n\n^bTime left^b: "),st,st2);
					else
						CenterPrint7(self, st3,"\n\n",st4,st5,S_("\n[Preparing teleporting...]\n\n^bTime left^b: "),st,st2);
				}
				else // in use for another reserve
				{
					st = "\nTeleporter in use for reserve \xA3"; // PZ: this is a brown pound symbol; cppreqcc messes this up by removing pounds
					st2 = ftos(ArmyTimer->job);
					st = strcat(st,st2);

					//CenterPrint4(self, st3,"\n\n",st4,st);
					CenterPrint5(self, st3,"\n\n",st4,st5,st);
				}
			}
			else // Something is ready
			{
				st = "\nSoldier ready in reserve \xA3";
				st2 = ftos(self->delay_time);
				st = strcat(st,st2);

				//CenterPrint4(self, st3,"\n\n",st4,st);
				CenterPrint5(self, st3,"\n\n",st4,st5,st);
			}
		}
	}

  //ideas:

  //((centered soldier status
  // \n attacking)) ((xxxx)) - seeking xxxx - standing - on patrol|walking - following you - following u (forced)//1 prt (2 vars, one for displaying and keep preference and the other for ai)

  //((1.. tactic mode:		  normal - agressive - static // 1 prt))
  // 2.. ignore enemy:	 on - off
  // normal - tries to return to waypoints when enemy not visible (like in the aussie version)
  // agressive - he tries to reach enemy when not visible (like before when no tactic stuff)
  // patrol - forces grunty to use waypoints - and reports when he cant
  // static - soldier dont move when attacking if no waypoints, if there r he moves on the line between them
  // passive - makes soldier to ignore enemy completely

  //NO! AI improvement - 0 none 1 desires to be close to enemy 2 desires to be farther

  //((3.. obstacle: 	sort - stop  // 1 prt
  //4.. talk mode:	   on - off))
  //((5.. reports:	 all - most - some - off // 1 prt

  //st=

  //6.. show soldier's inventory
  //

  // health:)) ((500)) ((hp // 1 prt

	// 0.. NOTHING)) // 1 prt
  //FULL
  // (when in patrol or others) hmmm... forgot my xxxx waypoint, sorry!

  // when forced "follow me", random time until "lemme kill them!" and restore status
  // when forced "follow me" super_time controls time between the "i can see the enemy from here"

  // inv in new menu, so spacing nicer

  // "use ur waypoints!" in first menu

  // follow me, stay there and use ur waypoints defined in a variable to restore last intention when enemy becomes not visible

  //self.demon_one = AssignEasyWaypoint
  // scan for other targets when aggressive and enemy not visible
  // when aggressive augment probability of direct walk to last seen mark
  // * primary

  //in waypoint do nothing, if soldier already have assigned the corresponding waypoint to another entity dremove(self);
}

void Menu_Army2(entity soul, float currentsoul)
{
	string st,st2,st3,st4,st5,st6,st7, tmp;
	float extras;

	extras = GetJobExtras(self);

	st = st2 = st3 = st4 = st6 = "";

	if (soul==world) // our sold was killed (this never gets executed?)
	{
		ResetMenu();
		return;
	}

	if (soul->enemy!=world && visible2(soul,soul->enemy))
	{
		st2="\n\nAttacking ";
		st3=GetEnemyName(soul->enemy);
	}
	else if (soul->enemy!=world)
	{
		st2="\n\nSeeking ";
		st3=GetEnemyName(soul->enemy);
	}
	else if (soul->goalentity==self)
		st2="\n\nFollowing You";
	else if (soul->goalentity==world)
		st2="\n\nStanding";
	else if (soul->goalentity!=world)
		st2="\n\nOn Patrol";

	if (!extras)
		st2 = strcat(S_("\x9D\x9E\x9F ^bSoldier^b ^bStatus^b \x9D\x9E\x9F"),st2);
	else
	{
		if (currentsoul == 0)
			st2 = strcat(S_("\x9D\x9E\x9F ^bSoldier #1^b ^bStatus^b \x9D\x9E\x9F"),st2);
		else if (currentsoul == 1)
			st2 = strcat(S_("\x9D\x9E\x9F ^bSoldier #2^b ^bStatus^b \x9D\x9E\x9F"),st2);
		else if (currentsoul == 2)
			st2 = strcat(S_("\x9D\x9E\x9F ^bSoldier #3^b ^bStatus^b \x9D\x9E\x9F"),st2);
	}

	st5 = ""; // this will be preferred range

	// tactic mode option
	if (soul->is_malfunctioning==0)
		st4 = S_("\n\n^1.. ^bTactic Mode^b:    normal\n");
	else if (soul->is_malfunctioning==1)
		st4 = S_("\n\n^1.. ^bTactic Mode^b:    seek  \n");
	else if (soul->is_malfunctioning==2)
		st4 = S_("\n\n^1.. ^bTactic Mode^b:    static\n");

	// engage enemy option
	if ( soul->is_detpacking )
		st4 = strcat( st4, S_("^2.. ^bEngage Enemy^b:   yes   \n") );
	else
		st4 = strcat( st4, S_("^2.. ^bEngage Enemy^b:   no    \n") );

	// on obstacle option
	if ( !soul->is_toffingadet )
		st4 = strcat( st4, S_("^3.. ^bOn Obstacle^b:    jump  \n") );
	else if ( soul->is_toffingadet == 1 )
		st4 = strcat( st4, S_("^3.. ^bOn Obstacle^b:    stop  \n") );
	else
		st4 = strcat( st4, S_("^3.. ^bOn Obstacle^b:    auto  \n") );

	// attack range option, don't allow on static since he only moves to goto waypoints or to follow his owner on that mode
	if ( soul->is_malfunctioning != 2 ) {
		if (soul->delay_time == 0)
			st5 = S_("^4.. ^bAttack Range^b:   any   \n");
		else if (soul->delay_time == 1)
			st5 = S_("^4.. ^bAttack Range^b:   small \n");
		else if (soul->delay_time == 2)
			st5 = S_("^4.. ^bAttack Range^b:   medium\n");
		else if (soul->delay_time == 3)
			st5 = S_("^4.. ^bAttack Range^b:   large \n");
	}

	if (!extras)
	{
		if (soul->all_active==0)
			st6=S_("^5.. ^bReports^b:        all   \n\n^6.. Back to standard menu \n\n\x9A.. Allowed Weapons       \n\n^9.. Inventory             \n\n^0.. ^bNothing^b               \n\n^bMax HP^b: ");
		else if (soul->all_active==1)
			st6=S_("^5.. ^bReports^b:        most  \n\n^6.. Back to standard menu \n\n\x9A.. Allowed Weapons       \n\n^9.. Inventory             \n\n^0.. ^bNothing^b               \n\n^bMax HP^b: ");
		else if (soul->all_active==2)
			st6=S_("^5.. ^bReports^b:        some  \n\n^6.. Back to standard menu \n\n\x9A.. Allowed Weapons       \n\n^9.. Inventory             \n\n^0.. ^bNothing^b               \n\n^bMax HP^b: ");
		else if (soul->all_active==3)
			st6=S_("^5.. ^bReports^b:        none  \n\n^6.. Back to standard menu \n\n\x9A.. Allowed Weapons       \n\n^9.. Inventory             \n\n^0.. ^bNothing^b               \n\n^bMax HP^b: ");
	}
	else
	{
		if (soul->all_active==0)
			st6=S_("^5.. ^bReports^b:        all   \n\n^6.. Back to standard menu \n\n\x9A.. Allowed Weapons       \n\n^7.. ^bReserve^b  ^9.. Inventory\n\n^0.. ^bNothing^b               \n\n^bMax HP^b: ");
		else if (soul->all_active==1)
			st6=S_("^5.. ^bReports^b:        most  \n\n^6.. Back to standard menu \n\n\x9A.. Allowed Weapons       \n\n^7.. ^bReserve^b  ^9.. Inventory\n\n^0.. ^bNothing^b               \n\n^bMax HP^b: ");
		else if (soul->all_active==2)
			st6=S_("^5.. ^bReports^b:        some  \n\n^6.. Back to standard menu \n\n\x9A.. Allowed Weapons       \n\n^7.. ^bReserve^b  ^9.. Inventory\n\n^0.. ^bNothing^b               \n\n^bMax HP^b: ");
		else if (soul->all_active==3)
			st6=S_("^5.. ^bReports^b:        none  \n\n^6.. Back to standard menu \n\n\x9A.. Allowed Weapons       \n\n^7.. ^bReserve^b  ^9.. Inventory\n\n^0.. ^bNothing^b               \n\n^bMax HP^b: ");
	}

	st7=ftos(floor(soul->max_health));
	st7 = strcat(st7,S_("  ^bKills^b: "));
	tmp = ftos(soul->frags);
	st7 = strcat(st7,tmp);

	CenterPrint7(self,st,st2,st3,st4,st5,st6,st7);
}

void Menu_Army3( entity soul, float currentsoul) {
	string st1, st2, st3, st4, st5;

	if ( soul == world ) {
		ResetMenu ();
		return;
	}

	if ( soul->PR_allowed_weapons & PR_WEAP_AXE )
		st1 = "" TO_STR(PR_CHAR_BOUGHT) " \x93.. Axe                  \n";
	else
		st1 = "  \x93.. Axe                  \n";
	if ( soul->PR_allowed_weapons & PR_WEAP_SHOTGUN )
		st2 = "" TO_STR(PR_CHAR_BOUGHT) " \x94.. Shotgun              \n";
	else
		st2 = "  \x94.. Shotgun              \n";
	if ( soul->PR_allowed_weapons & PR_WEAP_SUPER_SHOTGUN )
		st3 = "" TO_STR(PR_CHAR_BOUGHT) " \x95.. Super Shotgun        \n";
	else
		st3 = "  \x95.. Super Shotgun        \n";
	if ( soul->PR_allowed_weapons & PR_WEAP_NAILGUN )
		st4 = "" TO_STR(PR_CHAR_BOUGHT) " \x96.. Nailgun              \n";
	else
		st4 = "  \x96.. Nailgun              \n";
	if ( soul->PR_allowed_weapons & PR_WEAP_ROCKET_LAUNCHER )
		st5 = "" TO_STR(PR_CHAR_BOUGHT) " \x97.. Rocket Launcher      \n\n";
	else
		st5 = "  \x97.. Rocket Launcher      \n\n";

	CenterPrint7( self, S_("\x9D\x9E\x9F ^bAllowed Weapons^b \x9D\x9E\x9F\n\n"), st1, st2, st3, st4, st5, "  \x92.. Back to tactical menu" );
}

void Menu_Army_Input1(float inp, entity soul, float currentsoul);
void Menu_Army_Input2(float inp, entity soul, float currentsoul);
void Menu_Army_Input3(float inp, entity soul, float currentsoul);

void Menu_Army_Input(float inp)
{
	float currentsoul;
	currentsoul = GetCurrentSoul(self);

	entity soul;
	soul = GetSummon(self,currentsoul);

	if (soul != world)
	{
		if (soul->is_haxxxoring == 1)
			Menu_Army_Input2(inp, soul, currentsoul);
		else if ( soul->is_haxxxoring == 2 )
			Menu_Army_Input3( inp, soul, currentsoul );
		else
			Menu_Army_Input1(inp, soul, currentsoul);
	}
	else
		Menu_Army_Input1(inp, soul, currentsoul);
}

void Menu_Army_Input3( float inp, entity soul, float currentsoul) {
	if ( soul == world ) {
		ResetMenu ();
		self->impulse = 0;
		return;
	}

	if ( inp == 1 ) {
		self->impulse = 0;
		if ( soul->PR_allowed_weapons & PR_WEAP_AXE )
			soul->PR_allowed_weapons = soul->PR_allowed_weapons - PR_WEAP_AXE;
		else
			soul->PR_allowed_weapons = soul->PR_allowed_weapons + PR_WEAP_AXE;
		Menu_Army3( soul, currentsoul );
		CuTFMenuSound( PR_MENUSOUND_BROWSE );
		return;
	}
	if ( inp == 2 ) {
		self->impulse = 0;
		if ( soul->PR_allowed_weapons & PR_WEAP_SHOTGUN )
			soul->PR_allowed_weapons = soul->PR_allowed_weapons - PR_WEAP_SHOTGUN;
		else
			soul->PR_allowed_weapons = soul->PR_allowed_weapons + PR_WEAP_SHOTGUN;
		Menu_Army3( soul, currentsoul );
		CuTFMenuSound( PR_MENUSOUND_BROWSE );
		return;
	}
	if ( inp == 3 ) {
		self->impulse = 0;
		if ( soul->PR_allowed_weapons & PR_WEAP_SUPER_SHOTGUN )
			soul->PR_allowed_weapons = soul->PR_allowed_weapons - PR_WEAP_SUPER_SHOTGUN;
		else
			soul->PR_allowed_weapons = soul->PR_allowed_weapons + PR_WEAP_SUPER_SHOTGUN;
		Menu_Army3( soul, currentsoul );
		CuTFMenuSound( PR_MENUSOUND_BROWSE );
		return;
	}
	if ( inp == 4 ) {
		self->impulse = 0;
		if ( soul->PR_allowed_weapons & PR_WEAP_NAILGUN )
			soul->PR_allowed_weapons = soul->PR_allowed_weapons - PR_WEAP_NAILGUN;
		else
			soul->PR_allowed_weapons = soul->PR_allowed_weapons + PR_WEAP_NAILGUN;
		Menu_Army3( soul, currentsoul );
		CuTFMenuSound( PR_MENUSOUND_BROWSE );
		return;
	}
	if ( inp == 5 ) {
		self->impulse = 0;
		if ( soul->PR_allowed_weapons & PR_WEAP_ROCKET_LAUNCHER )
			soul->PR_allowed_weapons = soul->PR_allowed_weapons - PR_WEAP_ROCKET_LAUNCHER;
		else
			soul->PR_allowed_weapons = soul->PR_allowed_weapons + PR_WEAP_ROCKET_LAUNCHER;
		Menu_Army3( soul, currentsoul );
		CuTFMenuSound( PR_MENUSOUND_BROWSE );
		return;
	}

	if ( inp == 10 ) {
		self->impulse = 0;
		soul->is_haxxxoring = 1;
		Menu_Army2( soul, currentsoul );
		CuTFMenuSound( PR_MENUSOUND_BROWSE );
		return;
	}
}

void Menu_Army_Input2(float inp, entity soul, float currentsoul)
{
	if (inp == 10 || soul==world) // NOTHING
	{
		ResetMenu();
		self->impulse = 0;
		return;
	}

	if (inp == 1)
	{
		self->impulse = 0;
		soul->is_malfunctioning=soul->is_malfunctioning+1;

		if (soul->is_malfunctioning > 2)
			soul->is_malfunctioning = 0;

		if ( soul->enemy != world ) {
			// Gizmo - check for static tactic mode
			if ( soul->is_malfunctioning == 2 ) {
				if ( soul->goalentity == soul->enemy ) {
					if ( soul->penance_time == 1 )
						soul->goalentity = soul->real_owner;
					else if ( soul->penance_time == 3 )
						soul->goalentity = ReturnEasyWaypoint( soul, soul );
					else
						soul->goalentity = world;
				}
			} else {
				soul->goalentity = soul->enemy;
			}
		}

		Menu_Army2(soul, currentsoul);
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		return;
	}

	if (inp == 2) // engage enemy??
	{
		self->impulse = 0;
		if (soul->is_detpacking == 0)
		{
			soul->is_detpacking=1;
			PrintFromSoldier(soul,self,"i'm gonna kill them all!\n",PR_PRINT_HIGH);
		}
		else
		{
			soul->is_detpacking=0;
			soul->has_teleporter = 0;
			soul->effects = soul->effects - (soul->effects & PR_EF_DIMLIGHT);

			if (soul->enemy!=world)//soul.goalentity)
			{
				soul->enemy=world;
				soul->goalentity = ReturnEasyWaypoint(soul,soul);
			}

			PrintFromSoldier(soul,self,"Ignoring enemy!\n",PR_PRINT_HIGH);
		}
		Menu_Army2(soul, currentsoul);
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
	}

	if (inp == 3) // jump or stop??
	{
		self->impulse = 0;
		soul->is_toffingadet = soul->is_toffingadet + 1;
		if ( soul->is_toffingadet > 2 )
			soul->is_toffingadet = 0;

		Menu_Army2(soul, currentsoul);
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
	}

	if (inp == 4)
	{
		if (soul->is_malfunctioning != 2) // preferred range, none small medium or large
		{

			soul->delay_time=soul->delay_time+1;

			if (soul->delay_time > 3)
				soul->delay_time=0;

			Menu_Army2(soul, currentsoul);
			CuTFMenuSound(PR_MENUSOUND_BROWSE);
		}

		self->impulse = 0;
		return;
	}

	if (inp == 5)
	{
		self->impulse = 0;
		soul->all_active=soul->all_active+1;

		if (soul->all_active > 3)
		soul->all_active=0;

		Menu_Army2(soul, currentsoul);
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		return;
	}

	if (inp == 6)
	{
		self->impulse = 0;
		soul->is_haxxxoring = 0;
		Menu_Army1(soul, currentsoul);
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		return;
	}

	if (inp == 7)
	{
		float extras;
		extras = GetJobExtras(self);

		if (!extras)
			return;

		currentsoul = currentsoul +1;
		if (currentsoul > extras)
			currentsoul = 0;
		SetCurrentSoul(self,currentsoul);

		CuTFMenuSound(PR_MENUSOUND_BROWSE);

		Menu_Army();
		self->impulse = 0;
		return;
	}

	// Gizmo - allowed weapons
	if ( inp == 8 ) {
		self->impulse = 0;
		soul->is_haxxxoring = 2;
		Menu_Army3( soul, currentsoul );
		CuTFMenuSound( PR_MENUSOUND_BROWSE );
		return;
	}

	if (inp == 9) // Army info
	{
		ResetMenu();
		Grunty_PrintDetails( self, soul, 1 );
		self->impulse = 0;
		return;
	}
}

void Menu_Army_Input1(float inp, entity soul, float currentsoul)
{
	float extras;
	extras = GetJobExtras(self);

	if (inp == 10) // NOTHING
	{
		ResetMenu();
		self->impulse = 0;

		//if (self.delay_time == #FALSE && !(self.job & #JOB_DEMON_OUT))
		if (self->delay_time == PR_FALSE && soul == world)//!HasMonster(self)) // <-- TOCHANGE
		{
			sprint(self,PR_PRINT_HIGH,"Army HQ\x8D Still ");
			PrintArmyTime(self);
			sprint(self,PR_PRINT_HIGH," time left until next teleporting\n");
			self->job_finished=time+0.5;
		}

		return;
	}
	else if (inp == 1 && soul == world) // Teleport it!
	{
		//if (self.delay_time == #TRUE && !(self.job & #JOB_DEMON_OUT))
		if (self->delay_time - 1 == currentsoul)//!HasMonster(self)) // <-- TOCHANGE
		{
		   ResetMenu();

		   if (TeleSoldier(currentsoul))
		   {
			   self->delay_time=PR_FALSE; // needed for setarmytimer
			   SetArmyTimer(self,PR_FALSE); // prepare reserve soldier
		   }

		   self->impulse = 0;
		}

		return;
	}
	else if (inp == 7 && extras !=0) // Next reserve
	{
		currentsoul = currentsoul +1;
		if (currentsoul > extras)
			currentsoul = 0;
		SetCurrentSoul(self,currentsoul);

		Menu_Army();
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		self->impulse = 0;
		return;
	}
	else if (inp == 8) // Kill it!
	{
		if (soul!=world)
		{
/*
			if (soul.health < #MINHP_TOKILL_MONSTERS)
			{
				ResetMenu();

				sprint(self,#PRINT_HIGH,"The explosive implanted in your soldier brain is damaged...\n");
				PrintFromSoldier(soul,self,"Argh! what's that noise?\n",#PRINT_CHAT);

				// Do pain frames and sound
				local entity oself;
				oself = self;
				self = soul;

				if (isMelee())
					grunty_axepain1();
				else
					grunty_pain1();

				self = oself;

				//ok, done
				self.impulse = 0;
			}
			else
			{
*/
			ResetMenu();
			sprint(self,PR_PRINT_HIGH,"You detonate the micro-explosive implanted in his brain!\n");
			KillSoul(self,soul->increase_team1);
			teamprefixsprint(self->team_no, self);
			teamsprint6(self,self->netname, " detonates his soldier ",soul->netname,"\n","","");
			self->impulse = 0;
		}
		return;
	}

	if (soul == world)
		return;

	if (inp == 1) // primary waypoint
	{
		if (soul->martyr_enemy != world)
			RemoveWaypoint(soul->martyr_enemy,soul);

		if (soul->demon_two != world)
			RemoveWaypoint(soul->demon_two,soul);

		soul->martyr_enemy = CreateWaypoint(self->origin, PR_WAYPOINT_LIFE, PR_WAYPOINT_TYPE_PRIMARY, soul);
		soul->martyr_enemy->goalentity = world;

		soul->penance_time=3;
		if (visible2(soul,soul->martyr_enemy)) soul->goalentity = soul->martyr_enemy;

		ResetMenu(); // reset menu?

		//CuTFMenuSound(#MENUSOUND_BUY);

		self->impulse = 0;

		PrintFromSoldier(soul,self,"yes sir!\n",PR_PRINT_HIGH);

		return;
	}

	else if (inp == 2) // secondary waypoint
	{
		if (soul->demon_two != world)
			RemoveWaypoint(soul->demon_two,soul);

		if (soul->martyr_enemy==world)
		{
			self->impulse=0;
			PrintFromSoldier(soul,self,"you must set my primary waypoint first!\n",PR_PRINT_CHAT);
			ResetMenu(); // reset menu?
			return;
		}

		soul->demon_two = CreateWaypoint(self->origin, PR_WAYPOINT_LIFE, PR_WAYPOINT_TYPE_SECONDARY, soul);
		soul->demon_two->goalentity = soul->martyr_enemy;
		soul->job = 2;
		soul->penance_time=3;
		soul->goalentity = ReturnEasyWaypoint(soul,soul);
		if (soul->martyr_enemy!=world) soul->martyr_enemy->goalentity = soul->demon_two;

		ResetMenu(); // reset menu?

		//CuTFMenuSound(#MENUSOUND_BUY);

		self->impulse = 0;

		PrintFromSoldier(soul,self,"Ok\n",PR_PRINT_HIGH);

		return;
	}

	else if (inp == 3) // FOLLOW ME, GRUNTY!
	{
		if (soul->enemy!=world)
		{
			// Gizmo - we may be seeking out the enemy
			if ( visible2( soul,soul->enemy ) || soul->PR_grunty_tacticMode == PR_GRUNTY_TACTICMODE_SEEK )
			{
				soul->penance_time = 1;

				if ( soul->is_malfunctioning != 2 )
					PrintFromSoldier( soul, self, "i'm fighting the enemy!\n", PR_PRINT_CHAT );
				else {
					if ( soul->goalentity != self ) {
						soul->goalentity = self;
						PrintFromSoldier( soul, self, "Ok!\n", PR_PRINT_HIGH );
					} else
						PrintFromSoldier( soul, self, "Already following you!\n", PR_PRINT_CHAT );
				}
				ResetMenu();
				self->impulse=0;
				return;
			}
		}

		soul->has_teleporter = 0;
		soul->effects = soul->effects - (soul->effects & PR_EF_DIMLIGHT);
		soul->penance_time = 1;

		if (soul->goalentity != self)
		{
			soul->goalentity = self;
			PrintFromSoldier(soul,self,"Ok!\n",PR_PRINT_HIGH);
		}
		else
			PrintFromSoldier(soul,self,"Already following you!\n",PR_PRINT_CHAT);

		ResetMenu();
		//CuTFMenuSound(#MENUSOUND_BUY);
		self->impulse = 0;
		return;
	}

	else if (inp == 4) // Stay there!
	{
		if (soul->enemy!=world)
		{
			if (visible2(soul,soul->enemy))
			{
				soul->penance_time = 2;

				if ( soul->is_malfunctioning != 2 )
					PrintFromSoldier( soul, self, "i'm fighting the enemy!\n", PR_PRINT_CHAT );
				else {
					soul->goalentity = world;
					PrintFromSoldier( soul, self, "Holding position...\n", PR_PRINT_HIGH );
				}
				ResetMenu();
				self->impulse=0;
				return;
			}

			soul->has_teleporter = 0;
			soul->effects = soul->effects - (soul->effects & PR_EF_DIMLIGHT);
		}

		soul->goalentity = world;
		PrintFromSoldier(soul,self,"Holding position...\n",PR_PRINT_HIGH);

		soul->penance_time = 2;

		ResetMenu();
		//CuTFMenuSound(#MENUSOUND_BUY);
		self->impulse = 0;
		return;
	}

	else if (inp == 5) // Use your waypoints!
	{
		if (soul->enemy!=world)
		{
			if (visible2(soul,soul->enemy))
			{
				soul->penance_time = 3;

				if ( soul->is_malfunctioning != 2 )
					PrintFromSoldier( soul, self, "i'm fighting the enemy!\n", PR_PRINT_CHAT );
				else {
					soul->goalentity = ReturnEasyWaypoint( soul, soul );

					if ( soul->goalentity != world )
						PrintFromSoldier( soul, self, "Moving on!\n", PR_PRINT_HIGH );
				}
				ResetMenu();
				self->impulse=0;
				return;
			}
			soul->has_teleporter = 0;
			soul->effects = soul->effects - (soul->effects & PR_EF_DIMLIGHT);
		}

		soul->penance_time = 3;
		soul->goalentity = ReturnEasyWaypoint(soul,soul);

		if (soul->goalentity != world)
			PrintFromSoldier(soul,self,"Moving on!\n",PR_PRINT_HIGH);

		soul->has_teleporter = 0;
		soul->effects = soul->effects - (soul->effects & PR_EF_DIMLIGHT);

		//CuTFMenuSound(#MENUSOUND_BUY);

		ResetMenu();
		self->impulse = 0;
		return;
	}
	else if (inp == 6)
	{
		soul->is_haxxxoring = 1;
		Menu_Army2(soul, currentsoul);
		self->impulse = 0;
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		return;
	}
#ifndef PR_NO_GRUNTY_EMERGENCY_TELE
	// Gizmo
	else if ( inp == 9 ) {
		ResetMenu();
		self->impulse = 0;

		if ( GetArmyTimer( self ) == world ) {
			vector oldorg;

			makevectors( self->v_angle );
			v_forward[Z] = 0;
			v_forward = normalize( v_forward );

			oldorg = soul->origin;
			setorigin( soul, self->origin + v_forward * 64 );

			soul->owner = self;
			if ( !CheckArea( soul, self ) ) {
				sprint( self, PR_PRINT_HIGH, "Not enough room to teleport soldier!\n" );
				setorigin( soul, oldorg );
				soul->owner = soul;
				return;
			}
			soul->owner = soul;

			spawn_tfog( oldorg );
			spawn_tfog( soul->origin );

			soul->velocity = v_forward * 640;
			soul->angles = vectoangles( soul->velocity );
			soul->flags = soul->flags - ( soul->flags & PR_FL_ONGROUND );	// don't let grunty get stuck in the air

			self->delay_time = PR_FALSE; // needed for setarmytimer
			SetArmyTimer( self, PR_FALSE ); // prepare reserve soldier
		} else {
			sprint( self, PR_PRINT_HIGH, "Army HQ\x8D Still " );
			PrintArmyTime( self );
			sprint( self, PR_PRINT_HIGH, " time left until next teleporting\n" );
		}

		return;
	}
#endif
}


float TeleSoldier(float slot)
{
	makevectors (self->v_angle);
	v_forward[Z] = 0;
	v_forward = normalize(v_forward) * 64;

	newmis = spawn();
	newmis->PR_allowed_weapons = PR_WEAP_AXE | PR_WEAP_SHOTGUN | PR_WEAP_SUPER_SHOTGUN | PR_WEAP_NAILGUN | PR_WEAP_ROCKET_LAUNCHER;
	newmis->owner = newmis; //WK Self
	newmis->real_owner = self;

	newmis->classname = "monster_army";
	setmodel(newmis, "progs/grunty.mdl");

	newmis->origin = self->origin + v_forward;
	newmis->has_camera = 4;

	newmis->increase_team1 = slot; //monster slot on player

	newmis->increase_team2 = GetNameType(self,newmis->increase_team1,PR_FALSE);
	MakeMonsterName(newmis);
	newmis->PR_monster_type = PR_MONSTER_ARMY;

	setmodel (newmis, "progs/grunty.mdl");
	setsize (newmis, V({-16, -16, -24}), V({16, 16, 32}));

	// Gizmo - this and spawn_tfog used to be above setmodel and setsize
	newmis->owner = self;
	if (!CheckArea(newmis,self)) { //No room for el diablo
		sprint(self,PR_PRINT_HIGH,"Not enough room to teleport soldier!\n");
		newmis->think = SUB_Remove;
		newmis->nextthink = time + 0.1;

		// Gizmo - don't let the soldier stay visible
		setmodel( newmis, "" );
		return PR_FALSE;
	}
	newmis->owner = newmis;

	spawn_tfog(newmis->origin);

	// for Neo mode
	if (neo.isModeActive())
	{
		if (newmis->real_owner != neo.getNeo())
			newmis->skin = 3;
		else
			newmis->skin = 0;
	}
	else newmis->skin = self->team_no - 1;

	teamprefixsprint(self->team_no,self);
	teamsprint(self->team_no,self,self->netname);
	teamsprint(self->team_no,self," teleported Private Class III "); // <--- TOCHANGE
	teamsprint(self->team_no,self,newmis->netname);
	teamsprint(self->team_no,self,"\n");

	self->job_finished = time + 2;	//Don't let em kill demon for 15 secs.
									// SB 15 is tight; 10
									// 10 is tight; 5
									// OfN 5 is tight; 3 ;)

	newmis->velocity = v_forward * 10;
	newmis->angles = vectoangles(newmis->velocity);
	if (newmis->flags & PR_FL_ONGROUND)
		newmis->flags = newmis->flags - PR_FL_ONGROUND;

	newmis->movetype = PR_MOVETYPE_TOSS;
	newmis->solid = PR_SOLID_SLIDEBOX;
//	newmis.takedamage = #DAMAGE_AIM;	// Gizmo - this is done in walkmonster_start(), bad things could happen if this is set here

	newmis->health = PR_GRUNTY_HP;
	newmis->has_tesla = 1; // SB level multiplier
	newmis->has_sensor = 1; // SB level
	newmis->has_sentry = 2; // SB frags to level

	//newmis.frame = $stand13;
	grunty_setstartframe(newmis);

	newmis->th_stand = grunty_stand;
	newmis->th_run = grunty_run;
	newmis->th_missile = GruntyCheckFire;
	newmis->th_pain = grunty_pain;
	newmis->custom_speed = 10; // 10
	newmis->yaw_speed = 55;

	//#ifdef GRUNTY_EXTRA_WEAPONS
	//newmis.weapons_carried = #WEAP_AXE + #WEAP_SHOTGUN + #WEAP_NAILGUN + #GRUNTY_EXTRA_WEAPONS;
	//#else
	newmis->weapons_carried = PR_WEAP_AXE + PR_WEAP_SHOTGUN + PR_WEAP_NAILGUN;
	//#endif

	newmis->tf_items = 0;
	newmis->view_ofs = V({0, 0, 22});
	newmis->money = 100;
	newmis->respawn_time = time + 20;
	newmis->suicide_time = time + 20;
	newmis->ammo_shells = 25;
	newmis->ammo_nails = 50;
	newmis->ammo_rockets = 8; //was 4
	newmis->ammo_cells = 10;
	newmis->maxammo_shells = 50; // was 75;
	newmis->maxammo_nails = 200;//was 200;
	newmis->maxammo_rockets = 20;
	newmis->maxammo_cells = 100;
	newmis->current_weapon = PR_WEAP_SHOTGUN;
	newmis->th_die = custom_grunt_die;
	newmis->think = walkmonster_start;
	newmis->nextthink = time + 1;
	newmis->armorclass = 0; //- OfN none // #AT_SAVESHOT; // kevlar
	newmis->last_saveme_sound = 0; // ? oh that

	newmis->touch = grunty_touch; //-OfN-
	newmis->has_holo = time+2;		// touch messages delay
	newmis->is_detpacking=1;			// resets engage enemy to "yes"
	newmis->is_toffingadet = 2;		// Gizmo - start with "auto" mode

	if (slot == 0)
		newmis->real_owner->demon_one = newmis;
	else if (slot == 1)
		newmis->real_owner->demon_two = newmis;
	else
		newmis->real_owner->demon_three = newmis;

	newmis->max_health = newmis->health;

	newmis->ltime=time; // trhown or felt on water?

	newmis->martyr_enemy=world;
	newmis->demon_one=world;
	newmis->demon_two=world;

	sprint(self,PR_PRINT_HIGH,"You teleport your soldier.\n");
	//PrintFromSoldier(self.demon_one,self,"Your orders?\n");

	newmis->PR_monsterflag = PR_STRFLAG_MONSTER; // flag to identify monsters/army for sentry targetting

	newmis->team_no = self->team_no;

	newmis->aura = 0;
	newmis->tfstate = 0;
	newmis->all_active = 1; // Default priority of reports is "most"

	GetRank(newmis); // needed for army menu first update

	newmis->is_connected = PR_TRUE;

	newmis->flags = newmis->flags | PR_FL_MONSTER;

	return PR_TRUE;

}

//==============================================================//

void ArmyTimerThink();


//===========================================================================
// Creates an army timer for given player, only if none currently

void SetArmyTimer(entity player, float start)
{
	if (player->delay_time!=PR_FALSE) return;

	if (GetArmyTimer(player)!=world) return;

	newmis = spawnServerSide(); // PZ: make it a server-side only entity
	newmis->classname="army_timer";
	newmis->owner=player;

	newmis->think = ArmyTimerThink;
	newmis->nextthink = time + (60*army_delay)+60-(120*random()); // +/- 1 random minute

	newmis->job = GetFreeReserve(player);

	float minutes;
	string st;
	minutes = rint((newmis->nextthink - time) / 60);
	st = ftos(minutes);

	if (start)
		sprint(player,PR_PRINT_HIGH,"Army HQ\x8D Your first soldier will teleport in ",st," minutes\n");
	else
		sprint(player,PR_PRINT_HIGH,"Army HQ\x8D Reinforcement soldier will be ready in ",st," minutes\n");

#ifdef PR_ARMY_TEST
	newmis->nextthink = time + 1;
#endif
}

//==========================================================================
// Checks consistency and sets the "Army Ready" flag on player

void ArmyTimerThink()
{
	if (!(self->owner->is_connected) || !(self->owner->job & PR_JOB_ARMY) || self->owner->classname != "player")
	{
		dremove(self);
		return;
	}

	if (self->job == -1)
		sprint(self->owner,PR_PRINT_HIGH,"Army HQ\x8D You have a soldier waiting!\n");
	else
		sprint(self->owner,PR_PRINT_HIGH,"Army HQ\x8D Soldier is ready for teleporting!\n");

	self->owner->delay_time = self->job;
	dremove(self);
}

//=====================================================================
// Returns a pointer to the army timer (if any) of given player

entity GetArmyTimer(entity player)
{
	entity te;
	te = find(world, "classname", "army_timer");
	while (te != world)
	{
		if (te->owner == player)
		  return te;

		te = find(te, "classname", "army_timer");
	}

	return world;
}

//================================================================
// Removes our army timer, if any

void RemoveArmyTimer()
{
	entity te;
	te=GetArmyTimer(self);
	if (te!=world)
		dremove(te);
}

string Digitize(float num);

//=============================================================
// Sprints remaining army time in m:ss format

void PrintArmyTime(entity player)
{
	if (player->delay_time!=PR_FALSE || player->classname!="player") return;

	string st;
	float fl,fl2,fl3;
	entity ArmyTimer;

	ArmyTimer=GetArmyTimer(player);

	if (ArmyTimer==world)
	{
		sprint(player,PR_PRINT_HIGH,"!ERROR!");
		return;
	}

	fl=fabs(floor(ArmyTimer->nextthink - time));

	fl3=floor(fl/60);

	st=ftos(fl3);
	st = colstr(st,PR_COLSTR_NUMBER);

	sprint(player,PR_PRINT_HIGH,st);
	sprint(player,PR_PRINT_HIGH,S_("^b:^b"));

	fl2=fabs(fl-fl3*60);
	st=Digitize(fl2);
	sprint(player,PR_PRINT_HIGH,st);
}

//==============================================================
// Transform an integer into a 2 digits string always

string Digitize(float num)
{
	string tmps;

	tmps = ftos(num);
	tmps = colstr(tmps,PR_COLSTR_NUMBER);

	if (num < 10)
		tmps = strcat(S_("^0"),tmps);

	return tmps;
}

//========================================================================
// Prints a text message from a soldier, uses report priorities

void PrintFromSoldier(entity sld, entity player, const string& msg, float priority)
{
	if (sld->classname!="monster_army" || player->classname!="player") return;

	if (sld->all_active <= priority)
		sprint(player,PR_PRINT_HIGH,sld->netname,"\x8D ",msg);
}

//================================================================================
// Picks and returns a string to be used as a soldier touch response from owner

string GetOwnerMessage(entity sld)
{
	float rnum;
	rnum=random();

	if (sld->goalentity!=sld->real_owner)
	{
		if (rnum < 0.2 && sld->health < 200)
		   return "tell my wife I love her!\n";
		else if (rnum < 0.2)
			return "remember I can't swim! heh\n";
		else if (rnum < 0.4)
			return "ready to kill!\n";
		else if (rnum < 0.6)
			return "i'm looking for the enemy\n";
		else if (rnum < 0.8)
			return "i'm prepared!\n";
		else return "awaiting your orders!\n";
	}
	else
	{
		if (rnum < 0.5)
			return "i'm following you!\n";
		else return "where do you want to go?\n";
	}
}

//================================================================================
// Picks and returns a string to be used as a soldier touch response from teammate

string GetFriendlyMessage(entity sld)
{
	float rnum;
	rnum=random();

	if (rnum < 0.5)
	  return "try to not shoot me when fighting the enemy! heh\n";
	else return "lets destroy the enemy!\n";
	//else return "Hope I don't crash the server\n";
}

//========================================================================
// Gets a free "reserve slot" to be assigned to an army timer

float GetFreeReserve(entity player)
{
	float res;
	res = -1;

	if (player->demon_one == world)
		res = 1;
	else if (player->demon_two == world)
		res = 2;
	else if (player->demon_three == world)
		res = 3;

	float extras;
	extras = GetJobExtras(player);

	if (extras + 1 < res) // Check if this will be an "accumulation"
		return -1; // Accumulated reserve (no regular free slot)

	return res;
}

//===============================================================================
// Called to move any "accumulation" type reserve to a free reserve slot

void UpdateReserves(entity player)
{
	if (player->delay_time == -1)
	{
		player->delay_time = GetFreeReserve(player);

		/*if (player.delay_time == -1)
			RPrint("Error: Unable to find a free reserve slot in UpdateReserves()\n");*/
	}
	else
	{
		if (player->delay_time == PR_FALSE)
		{
			entity ArmyTimer;
			ArmyTimer = GetArmyTimer(player);

			if (ArmyTimer != world)
			{
				if (ArmyTimer->job == -1)
					ArmyTimer->job = GetFreeReserve(player);
			}
			/*else
				RPrint("Error: Couldn't find the ArmyTimer in UpdateReserves()\n");*/
		}
	}
}

//==========================================================================
// Updates army rating on a player

void ArmyUpdateRating(entity player)
{
	float offsetrat;

	offsetrat = (player->real_frags - GetMyAverageTeamScore(player)) - player->else_goal;

	// Check if value is beyond max/min
	if (offsetrat > PR_ARMY_MAXFRAGS_PRESTIGE) // beyond maximum - Congrats!
		player->dont_do_triggerwork = 1;
	else if (offsetrat < (0 - PR_ARMY_MAXFRAGS_PRESTIGE)) // beyond minimum - Lame!
		player->dont_do_triggerwork = 0;
	else if (offsetrat == 0)
		player->dont_do_triggerwork = 0.5; // 50%
	else // lets calculate it then:
		player->dont_do_triggerwork = 0.5+(offsetrat/PR_ARMY_MAXFRAGS_PRESTIGE)*0.5; // this will result in a 0-100% value (0-1 float)
}

//==========================================================================
// Gets the average team score of a player

float GetMyAverageTeamScore(entity player)
{
	entity te;
	float sum, numplteam;

	sum = 0;
	numplteam = 0;

	te = find(world, "classname", "player");

	while (te != world)
	{
		if (te->is_connected)
		if (player != te)
		if (player->team_no == te->team_no)
		{
			numplteam = numplteam + 1;
			sum = sum + te->real_frags;
		}

		te = find(te, "classname", "player");
	}

	if (!numplteam)
		return 0;

	return sum / numplteam;
}

//==============================================================================
// Initializes army rating/prestige and any army related stuff (called once)

void ArmyInit(entity player)
{
	// Get starting difference of average team frags and our ones
	player->else_goal = player->real_frags - GetMyAverageTeamScore(player);

	// Start with 50% army prestige
	player->dont_do_triggerwork = 0.5;

	// Set the army timer also
	player->delay_time = PR_FALSE;
	SetArmyTimer(player, PR_TRUE);
}

//=============================================================================
// The following functions are called when a player joins/leaves a team
// to update the army rating offset on players that use army on his same team
// The objective is simple: Army rating shouldn't change at all when someone
// joins or leaves a team. These functions ensure just that and are fun! :P

void ArmyRatingLeave(entity player)
{
	entity te, te2;
	float counter, tfrags;
	float avg_with_player;
	float avg_without_player;

	// Cycle thru all the players on his team that use army
	te = find(world,"classname","player");

	while (te != world)
	{
		if (te->is_connected)
		if (te->job & PR_JOB_ARMY)
		if (te != player)
		if (te->team_no == player->team_no) // Got one?
		{
			counter = 0;
			tfrags = 0;

			// Get the averages on his team
			te2 = find(world, "classname", "player");

			while (te2 != world)
			{
				if (te2->is_connected)
				if (te2->team_no == player->team_no)
				if (te2 != te && te2 != player)
				{
					tfrags = tfrags + te2->real_frags;
					counter = counter + 1;
				}

				te2 = find(te2, "classname","player");
			}

			if (counter == 0)
			{
				avg_without_player = 0;
				avg_with_player = player->real_frags;
			}
			else
			{
				// Get the average excluding the player that leaves
				avg_without_player = tfrags / counter;

				// Get the average including the player that leaves
				avg_with_player = (tfrags+player->real_frags) / (counter + 1);
			}

			// Get the offset to compensate the player leaving

			/*old_offset = (te.real_frags - avg_with_player) - te.else_goal;
			new_offset = (te.real_frags - avg_without_player) - te.else_goal;

			te.else_goal = te.else_goal - (old_offset - new_offset);*/

			// Compacted to save temp allocation space (same as above)
			te->else_goal = te->else_goal - (((te->real_frags - avg_with_player) - te->else_goal) - ((te->real_frags - avg_without_player) - te->else_goal));

			// Adjust army rating for this one
			AdjustArmyRating(te,avg_without_player);
		}

		te = find(te,"classname","player");
	}
}

// Called when a player joins a specific team
void ArmyRatingJoin(entity player)
{
	entity te, te2;
	float counter, tfrags;
	float avg_with_player;
	float avg_without_player;

	// Cycle thru all the players on his team that use army
	te = find(world,"classname","player");

	while (te != world)
	{
		if (te->is_connected)
		if (te->job & PR_JOB_ARMY)
		if (te != player)
		if (te->team_no == player->team_no) // Got one?
		{
			counter = 0;
			tfrags = 0;

			// Get the averages on his team
			te2 = find(world, "classname", "player");

			while (te2 != world)
			{
				if (te2->is_connected)
				if (te2->team_no == player->team_no)
				if (te2 != te && te2 != player)
				{
					tfrags = tfrags + te2->real_frags;
					counter = counter + 1;
				}

				te2 = find(te2, "classname","player");
			}

			if (counter == 0)
			{
				avg_without_player = 0;
				avg_with_player = player->real_frags;
			}
			else
			{
				// Get the average excluding the player that joins
				avg_without_player = tfrags / counter;

				// Get the average including the player that joins
				avg_with_player = (tfrags+player->real_frags) / (counter + 1);
			}

			// Get the offset to compensate the player joining

			/*old_offset = (te.real_frags - avg_without_player) - te.else_goal;
			new_offset = (te.real_frags - avg_with_player) - te.else_goal;

			te.else_goal = te.else_goal - (old_offset - new_offset);*/

			// Compacted to save temp allocation space (same as above)
			te->else_goal = te->else_goal - (((te->real_frags - avg_without_player) - te->else_goal) - ((te->real_frags - avg_with_player) - te->else_goal));

			// Adjust army rating for this one
			AdjustArmyRating(te,avg_with_player);
		}

		te = find(te,"classname","player");
	}
}

//===========================================================================
// Special subroutine to adjust army rating value suplying the team average

void AdjustArmyRating(entity player, float avg_score)
{
	float offsetrat;

	offsetrat = (player->real_frags - avg_score) - player->else_goal;

	// Check if value is beyond max/min
	if (offsetrat > PR_ARMY_MAXFRAGS_PRESTIGE) // beyond maximum - Congrats!
		player->dont_do_triggerwork = 1;
	else if (offsetrat < (0 - PR_ARMY_MAXFRAGS_PRESTIGE)) // beyond minimum - Lame!
		player->dont_do_triggerwork = 0;
	else if (offsetrat == 0)
		player->dont_do_triggerwork = 0.5; // 50%
	else // lets calculate it then:
		player->dont_do_triggerwork = 0.5+(offsetrat/PR_ARMY_MAXFRAGS_PRESTIGE)*0.5; // this will result in a 0-100% value (0-1 float)
}

} // END namespace Progs
