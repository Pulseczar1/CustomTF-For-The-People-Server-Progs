/*=======================================================//
// guerilla.QC - CustomTF 3.2.OfN		   - 18/4/2004 - //
=========================================================//
 Guerilla job stuff - Revamped by OfteN [cp]
---------------------------------------------------------//
 Moved to its own file due to the huge changes
 The Guerilla Menu is also handled here
=========================================================*/

#include "progs.h"
#include "ofndefs.h"
#include "jobs.h"
#include "menu.h"
#include "sprites.h"
#include "combat.h"
#include "custom.h"
#include "debug.h"
#include "optimize.h"

namespace Progs {

/*
** Guerilla Profession -
** Can set self-detonating land mines
**
** OfN - Modified and improved using JOB_EXTRA's
** No job extras = Everything as before
** 1 extra = Menu, can place level2 mines and special mines
** 2 extras = Menu, can place level3 and level2 mines, special mines
**			  and can convert grenades into mines.
** Appart from converted mines when u have "Specialist":
** When having any job extras a player can place NAPALM or BIO mines
** if he has Pyrotoys or napalm grens and medikit or bio grens respectively
*/

// Guerilla maximums and job setttings
#define PR_GUERILLA_NOEXTRAS_MINEMAX 3
#define PR_GUERILLA_EXTRA1_POINTSMIN 4
#define PR_GUERILLA_EXTRA1_POINTSMAX 8
#define PR_GUERILLA_EXTRA2_POINTSMIN 5
#define PR_GUERILLA_EXTRA2_POINTSMAX 10
#define PR_GUERILLA_FIXED_MINEMAX 5 // Mines hard max

// Bonuses
#define PR_GUERILLA_BONUS1  20 // Number of mine kills to get 1st bonus
#define PR_GUERILLA_BONUS2  36 // Number of mine kills to get 2nd bonus
#define PR_GUERILLA_FRAGSFORPOINT 7 // Number of mine kills to get an extra point

//- Mine types -//

// Regular mines
#define PR_MINE_REGULAR	  1
#define PR_MINE_LEVEL1 	  1
#define PR_MINE_LEVEL2 	  2
#define PR_MINE_LEVEL3 	  3

// Special mines
#define PR_MINE_CONCUSSION   4
#define PR_MINE_PSIONIC	  5
#define PR_MINE_BIO		  6 // specially available
#define PR_MINE_NAPALM 	  7 // specially available
#define PR_MINE_MIRV		  8

// Converted-only mines
#define PR_MINE_NAIL		  9
#define PR_MINE_GAS		  10
#define PR_MINE_EMP		  11
#define PR_MINE_FRAG		  12
#define PR_MINE_KRAC		  13
#define PR_MINE_CALTROP	  14
#define PR_MINE_ANTIGRAV	  15

// Damage settings
#define PR_MINE_DMG			200
#define PR_MINE_UNARMED_DMG	60 // 80
#define PR_MINE_DMG_LEVEL1 	200//200 // was 240 /215
#define PR_MINE_DMG_LEVEL2 	250
#define PR_MINE_DMG_LEVEL3 	300
#define PR_MINE_LVL2_LEGDMG	1 // 2
#define PR_MINE_LVL3_LEGDMG	2 // 4

// Flags for converted mines on player entity (2nd byte on .increase_team4 field)
#define PR_GUERILLA_CONV1_SET 1
#define PR_GUERILLA_CONV2_SET 2

// Various settings
#define PR_GUERILLA_ACTIVATE_TIME 8 //Time until it turns on //- it was 3? prolly 10
#define PR_GUERILLA_BEEP_RATE 4 //Delay between beeps //- it was 3
#define PR_GUERILLA_MINE_DURATION 360 //Time it lasts after being activated //- it was 60 ofn
#define PR_GUERILLA_JOB_DELAY 4 //Time between mine placements //- it was 10
#define PR_GUERILLA_RADIUS 135 //-it was 150 without define
#define PR_GUERILLA_MINE_COST 4 // OfN number of rockets a mine needs

/*===============================================================================================

EXPLANATION OF HOW THE ENTITY FIELDS ARE USED (thnx? np.. :P)
---------------------------------------------

For player entity:
------------------

.all_active 	- Used for the following stuff
				(byte1) Points spent on currently existing mines
				(byte2) Mine points still available for mines
				(byte3) Mine points earned

.increase_team4 - Used for the following stuff
	(WARNING: This field is used also for warlock/army status bar)
				(byte1) Number of mines up
				(byte2) Bitfield for converted grenade flags
					|1 Converted type1 grenade mine up
					|2 Converted type2 grenade mine up
				(byte3) UNUSED STILL


.dont_do_triggerwork -
	(WARNING: This field is used also for warlock points/army rating)
				Kills achieved by mines

For mine entity:
----------------

.health 			- Remaining life time for the mine
.last_attacked_time - Controls delay between beeps on mine
.has_tesla			- Determines what to sprint to owner on mine explosion
.heat				- Time variable, determines if mine is charged
.has_sentry 		- Type of the mine
.has_sensor 		- Determines if this mine is a standard lvl1/lvl2/lvl3 mine or not
.has_fieldgen		- Flag used to flash mines on activation
.has_holo			- If 1 or 2, mine is a converted grenade, indicates grenade slot also

================================================================================================*/

float GetMineSpentPoints(entity player);
float GetMinePoints(entity player);
float GetMineEarnedPoints(entity player);
void SetMineSpentPoints(entity player,float spentpoints);
void SetMinePoints(entity player,float points);
void SetMineEarnedPoints(entity player,float points);

float GetMineConvBitfield(entity player);
void SetMineConvBitfield(entity player, float bitfield);
float GetMinesUp(entity player);
void SetMinesUp(entity player, float minesup);


void GuerillaSetMine(float minetype, float converted);
string GetGrenadeText(float typ);
float IsMineConvertible(float typ);
string GetMineStr(float minetype);
float ConvertGrenIDToMine(float grentype);
void MineUp(entity player, entity mine);
void MineDown(entity player, entity mine);
float AttemptToSetMine(entity player, float minetype, int converted);
float MineNeededPoints(float minetype);
float GuerillaGetMaxPointsForJob(entity player);
void GuerillaUpgradeJob(entity player);
//float(entity player) GuerillaGetTotalPoints;

// External
void CenterPrint7(entity pl, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5, const string& s6, const string& s7);
void ConcussionGrenadeExplode();
void PsionicGrenadeExplode();
void BioGrenadeExplode();
void NapalmGrenadeExplode();
void MirvGrenadeExplode();
void NailGrenadeExplode();
void GasGrenadeExplode();
void EMPGrenadeExplode();
void FragGrenadeExplode();
void KracGrenadeExplode();
void CaltropGrenadeExplode();
void AntiGravGrenadeExplode();

float HurtLegs(entity player, float damage);

float IsOwnedMonster( entity thing);
float IsMonster( entity thing);

//===============================================================
// Guerilla menu (only used if we have job extras)

void Menu_Guerilla()
{
	string header, st1, st2, st3, st4, st5, foot, tmp;
	float extras, points, minesup;
	int bitfield;

	extras = GetJobExtras(self);
	points = GetMinePoints(self);
	minesup = GetMinesUp(self);

	if (extras == 1) // Technician
	{
		if (!minesup)
		{
			header = S_("^bTechnician^b:     ^b(^bno mines up^b)^b\n\n");
		}
		else if (minesup == 1)
		{
			header = S_("^bTechnician^b:      ^b(^b1 mine up^b)^b\n\n");
		}
		else
		{
			header = S_("^bTechnician^b:      ^b(^b");
			tmp = ftos(minesup);
			header = strcat(header,tmp);
			header = strcat(header,S_(" mines up^b)^b\n\n"));
		}

		if (points <= 0)
		{
			st1 = S_("^1.. Level 1 Mine        (1 point)\n^2.. Level 2 Mine       (2 points)\n\n^4.. Concussion Mine     (1 point)\n^5.. Psionic Mine       (2 points)\n");

			if (self->tp_grenades_1 == PR_GR_TYPE_NAPALM || self->tp_grenades_2 == PR_GR_TYPE_NAPALM || self->weapons_carried & PR_WEAP_FLAMETHROWER)
				st2 = S_("^6.. Napalm Mine        (3 points)\n");
			else if (self->tp_grenades_1 == PR_GR_TYPE_BIO || self->tp_grenades_2 == PR_GR_TYPE_BIO || self->weapons_carried & PR_WEAP_MEDIKIT)
				st2 = S_("^6.. Biological Mine    (2 points)\n");
			else
				st2 = "\n";

			st3 = S_("^7.. MIRV Mine          (3 points)\n\n");
		}
		else if (points == 1)
		{
			st1 = S_("^1.. Level 1 Mine        (^1 ^bpoint^b)\n^2.. Level 2 Mine       (2 points)\n\n^4.. Concussion Mine     (^1 ^bpoint^b)\n^5.. Psionic Mine       (2 points)\n");

			if (self->tp_grenades_1 == PR_GR_TYPE_NAPALM || self->tp_grenades_2 == PR_GR_TYPE_NAPALM || self->weapons_carried & PR_WEAP_FLAMETHROWER)
				st2 = S_("^6.. Napalm Mine        (3 points)\n");
			else if (self->tp_grenades_1 == PR_GR_TYPE_BIO || self->tp_grenades_2 == PR_GR_TYPE_BIO || self->weapons_carried & PR_WEAP_MEDIKIT)
				st2 = S_("^6.. Biological Mine    (2 points)\n");
			else
				st2 = "\n";

			st3 = S_("^7.. MIRV Mine          (3 points)\n\n");
		}
		else if (points == 2)
		{
			st1 = S_("^1.. Level 1 Mine        (^1 ^bpoint^b)\n^2.. Level 2 Mine       (^2 ^bpoints^b)\n\n^4.. Concussion Mine     (^1 ^bpoint^b)\n^5.. Psionic Mine       (^2 ^bpoints^b)\n");

			if (self->tp_grenades_1 == PR_GR_TYPE_NAPALM || self->tp_grenades_2 == PR_GR_TYPE_NAPALM || self->weapons_carried & PR_WEAP_FLAMETHROWER)
				st2 = S_("^6.. Napalm Mine        (3 points)\n");
			else if (self->tp_grenades_1 == PR_GR_TYPE_BIO || self->tp_grenades_2 == PR_GR_TYPE_BIO || self->weapons_carried & PR_WEAP_MEDIKIT)
				st2 = S_("^6.. Biological Mine    (^2 ^bpoints^b)\n");
			else
				st2 = "\n";

			st3 = S_("^7.. MIRV Mine          (3 points)\n\n");
		}
		else
		{
			st1 = S_("^1.. Level 1 Mine        (^1 ^bpoint^b)\n^2.. Level 2 Mine       (^2 ^bpoints^b)\n\n^4.. Concussion Mine     (^1 ^bpoint^b)\n^5.. Psionic Mine       (^2 ^bpoints^b)\n");

			if (self->tp_grenades_1 == PR_GR_TYPE_NAPALM || self->tp_grenades_2 == PR_GR_TYPE_NAPALM || self->weapons_carried & PR_WEAP_FLAMETHROWER)
				st2 = S_("^6.. Napalm Mine        (^3 ^bpoints^b)\n");
			else if (self->tp_grenades_1 == PR_GR_TYPE_BIO || self->tp_grenades_2 == PR_GR_TYPE_BIO || self->weapons_carried & PR_WEAP_MEDIKIT)
				st2 = S_("^6.. Biological Mine    (^2 ^bpoints^b)\n");
			else
				st2 = "\n";

			st3 = S_("^7.. MIRV Mine          (^3 ^bpoints^b)\n\n");
		}

		st4 = "";
		st5 = "";
	}
	else // Specialist
	{
		if (!minesup)
		{
			header = S_("^bSpecialist^b:     ^b(^bno mines up^b)^b\n\n");
		}
		else if (minesup == 1)
		{
			header = S_("^bSpecialist^b:      ^b(^b1 mine up^b)^b\n\n");
		}
		else
		{
			header = S_("^bSpecialist^b:      ^b(^b");
			tmp = ftos(minesup);
			header = strcat(header,tmp);
			header = strcat(header,S_(" mines up^b)^b\n\n"));
		}

		if (points <= 0)
		{
			st1 = S_("^1.. Level 1 Mine        (1 point)\n^2.. Level 2 Mine       (2 points)\n^3.. Level 3 Mine       (3 points)\n\n^4.. Concussion Mine     (1 point)\n^5.. Psionic Mine       (2 points)\n");

			if (self->tp_grenades_1 == PR_GR_TYPE_NAPALM || self->tp_grenades_2 == PR_GR_TYPE_NAPALM || self->weapons_carried & PR_WEAP_FLAMETHROWER)
				st2 = S_("^6.. Napalm Mine        (3 points)\n");
			else if (self->tp_grenades_1 == PR_GR_TYPE_BIO || self->tp_grenades_2 == PR_GR_TYPE_BIO || self->weapons_carried & PR_WEAP_MEDIKIT)
				st2 = S_("^6.. Biological Mine    (2 points)\n");
			else
				st2 = "\n";

			st3 = S_("^7.. MIRV Mine          (3 points)\n\n");
		}
		else if (points == 1)
		{
			st1 = S_("^1.. Level 1 Mine        (^1 ^bpoint^b)\n^2.. Level 2 Mine       (2 points)\n^3.. Level 3 Mine       (3 points)\n\n^4.. Concussion Mine     (^1 ^bpoint^b)\n^5.. Psionic Mine       (2 points)\n");

			if (self->tp_grenades_1 == PR_GR_TYPE_NAPALM || self->tp_grenades_2 == PR_GR_TYPE_NAPALM || self->weapons_carried & PR_WEAP_FLAMETHROWER)
				st2 = S_("^6.. Napalm Mine        (3 points)\n");
			else if (self->tp_grenades_1 == PR_GR_TYPE_BIO || self->tp_grenades_2 == PR_GR_TYPE_BIO || self->weapons_carried & PR_WEAP_MEDIKIT)
				st2 = S_("^6.. Biological Mine    (2 points)\n");
			else
				st2 = "\n";

			st3 = S_("^7.. MIRV Mine          (3 points)\n\n");
		}
		else if (points == 2)
		{
			st1 = S_("^1.. Level 1 Mine        (^1 ^bpoint^b)\n^2.. Level 2 Mine       (^2 ^bpoints^b)\n^3.. Level 3 Mine       (3 points)\n\n^4.. Concussion Mine     (^1 ^bpoint^b)\n^5.. Psionic Mine       (^2 ^bpoints^b)\n");

			if (self->tp_grenades_1 == PR_GR_TYPE_NAPALM || self->tp_grenades_2 == PR_GR_TYPE_NAPALM || self->weapons_carried & PR_WEAP_FLAMETHROWER)
				st2 = S_("^6.. Napalm Mine        (3 points)\n");
			else if (self->tp_grenades_1 == PR_GR_TYPE_BIO || self->tp_grenades_2 == PR_GR_TYPE_BIO || self->weapons_carried & PR_WEAP_MEDIKIT)
				st2 = S_("^6.. Biological Mine    (^2 ^bpoints^b)\n");
			else
				st2 = "\n";

			st3 = S_("^7.. MIRV Mine          (3 points)\n\n");
		}
		else
		{
			st1 = S_("^1.. Level 1 Mine        (^1 ^bpoint^b)\n^2.. Level 2 Mine       (^2 ^bpoints^b)\n^3.. Level 3 Mine       (^3 ^bpoints^b)\n\n^4.. Concussion Mine     (^1 ^bpoint^b)\n^5.. Psionic Mine       (^2 ^bpoints^b)\n");

			if (self->tp_grenades_1 == PR_GR_TYPE_NAPALM || self->tp_grenades_2 == PR_GR_TYPE_NAPALM || self->weapons_carried & PR_WEAP_FLAMETHROWER)
				st2 = S_("^6.. Napalm Mine        (^3 ^bpoints^b)\n");
			else if (self->tp_grenades_1 == PR_GR_TYPE_BIO || self->tp_grenades_2 == PR_GR_TYPE_BIO || self->weapons_carried & PR_WEAP_MEDIKIT)
				st2 = S_("^6.. Biological Mine    (^2 ^bpoints^b)\n");
			else
				st2 = "\n";

			st3 = S_("^7.. MIRV Mine          (^3 ^bpoints^b)\n\n");
		}

		st4="";
		st5="";

		bitfield = GetMineConvBitfield(self);

		if (!(bitfield & PR_GUERILLA_CONV1_SET))
		if (self->no_grenades_1 >= 1)
		if (IsMineConvertible(self->tp_grenades_1))
		{
			st4 = S_("^8.. Convert ");
			tmp = GetGrenadeText(self->tp_grenades_1);
			st4 = strcat(st4,tmp);
			st4 = strcat(st4," Grenade");
			st4 = padstr(st4,33);
			st4 = strcat(st4,"\n");
		}

		if (!(bitfield & PR_GUERILLA_CONV2_SET))
		if (self->no_grenades_2 >= 1)
		if (IsMineConvertible(self->tp_grenades_2))
		{
			st5 = S_("^9.. Convert ");
			tmp = GetGrenadeText(self->tp_grenades_2);
			st5 = strcat(st5,tmp);
			st5 = strcat(st5," Grenade");
			st5 = padstr(st5,33);
			st5 = strcat(st5,"\n");
		}
	}

	foot =	  S_("\n^0.. ^bNothing^b                      \n\n^bGuerilla Points^b: ");

	tmp = ftos(points);
	foot = strcat(foot,tmp);
	foot = strcat(foot,S_("^b/^b"));

	bitfield = GetMineSpentPoints(self);
	tmp = ftos(points + bitfield);
	foot = strcat(foot,tmp);

	CenterPrint7(self,header,st1,st2,st3,st4,st5,foot);
}

//======================================================================
// Guerilla menu handler

void Menu_Guerilla_Input(float inp)
{
	if (inp == 10)
	{
		ResetMenu();
		self->impulse = 0;
	}

	// are we a specialist? if so.. we can convert grenades
	if (self->job & PR_JOB_EXTRA2)
	{
		float convID;
		int bitfield;

		bitfield = GetMineConvBitfield(self);

		if (inp == 8) // conversion of grenade 1
		{
			if (!(bitfield & PR_GUERILLA_CONV1_SET))
			if (self->no_grenades_1 >= 1)
			if (IsMineConvertible(self->tp_grenades_1))
			{
				convID = ConvertGrenIDToMine(self->tp_grenades_1);
				GuerillaSetMine(convID,1);
			}
		}

		if (inp == 9) // conversion of grenade 2
		{
			if (!(bitfield & PR_GUERILLA_CONV2_SET))
			if (self->no_grenades_2 >= 1)
			if (IsMineConvertible(self->tp_grenades_2))
			{
				convID = ConvertGrenIDToMine(self->tp_grenades_2);
				GuerillaSetMine(convID,2);
			}
		}
	}

	if (inp == 1)
		GuerillaSetMine(PR_MINE_LEVEL1,0);
	else if (inp == 2)
		GuerillaSetMine(PR_MINE_LEVEL2,0);
	else if (inp == 3 && self->job & PR_JOB_EXTRA2)
		GuerillaSetMine(PR_MINE_LEVEL3,0);
	else if (inp == 4)
		GuerillaSetMine(PR_MINE_CONCUSSION,0);
	else if (inp == 5)
		GuerillaSetMine(PR_MINE_PSIONIC,0);
	else if (inp == 6)
	{
		if (self->tp_grenades_1 == PR_GR_TYPE_NAPALM || self->tp_grenades_2 == PR_GR_TYPE_NAPALM || self->weapons_carried & PR_WEAP_FLAMETHROWER)
			GuerillaSetMine(PR_MINE_NAPALM,0);
		else if (self->tp_grenades_1 == PR_GR_TYPE_BIO || self->tp_grenades_2 == PR_GR_TYPE_BIO || self->weapons_carried & PR_WEAP_MEDIKIT)
			GuerillaSetMine(PR_MINE_BIO,0);
	}
	else if (inp == 7)
		GuerillaSetMine(PR_MINE_MIRV,0);

	self->impulse = 0;
}

//======================================================================
// A mine blows up

void GuerillaExplode()
{
	// Only call minedown() if we r still guerilla, this avoids garbage on clean up
	if (self->owner->job & PR_JOB_GUERILLA)
		MineDown(self->owner,self);

	string st,st2;
	float charged;

	if (time < self->heat + PR_GUERILLA_ACTIVATE_TIME)
		charged = PR_FALSE;
	else
		charged = PR_TRUE;

	self->health = 0; //CH would cause tesla to act weird without

	if (self->has_sensor) // Standard mines
	{
		float hitdamage, legdmg;
		float joblevel;

		joblevel = GetJobExtras(self->owner);

		legdmg = 0;

		if (self->has_sentry == PR_MINE_LEVEL1)
			hitdamage = PR_MINE_DMG_LEVEL1;
		else if (self->has_sentry == PR_MINE_LEVEL2)
		{
			hitdamage = PR_MINE_DMG_LEVEL2;

			legdmg = PR_MINE_LVL2_LEGDMG;

			if (charged)
				SpawnSprite(1,PR_SPRITE_AIRBURST,self->origin,V({0, 0, 0}),PR_SPRITEMOVE_UP,0.1);
		}
		else
		{
			hitdamage = PR_MINE_DMG_LEVEL3;

			legdmg = PR_MINE_LVL3_LEGDMG;

			if (charged)
				SpawnSprite(3,PR_SPRITE_AIRBURST,self->origin,V({0, 0, 0}),PR_SPRITEMOVE_UP,0.15);
		}

		deathmsg = PR_DMSG_LAND_MINE;

		if (joblevel == 0)
		{
		  if (self->has_tesla == 2) // set by GuerillaSwep for trace weapons
			sprint(self->owner,PR_PRINT_HIGH,"your mine is destroyed\n");

		  if (self->has_tesla == 0) // default
			sprint(self->owner,PR_PRINT_HIGH,"your mine explodes\n");

		  // if has_tesla is 1 print nothing, as this is set by DetonateMines() and GuerillaThink
		}
		else // specialist or technician
		{
			st = GetMineStr(self->has_sentry);
			st2 = "";

			if (self->has_holo)
				st2 = " converted";

			if (self->has_tesla == 2) // set by GuerillaSwep for trace weapons
				sprint(self->owner,PR_PRINT_HIGH,"Your ",st,st2," mine is destroyed\n");

			if (self->has_tesla == 0) // default
				sprint(self->owner,PR_PRINT_HIGH,"Your ",st,st2," mine explodes\n");
		}

		if (!charged) //If not charged, do less damage when blowing up
		   T_RadiusDamage (self, self->owner, PR_MINE_UNARMED_DMG, world); //- damage was 80
		else
		{
		   T_RadiusDamage (self, self->owner, hitdamage, world); //- damage was 160

		   // legdamage for lvl2 and lvl3 mines if was hit from below (on ground usually)
		   if (legdmg)
		   {
			   entity te;

			   te = findradius(self->origin, hitdamage + 15);

			   while (te != world)
			   {
					if (te->classname == "player")
					if (CanDamage (te, self))
					if (te->health > 0)
					if (te->playerclass != PR_PC_UNDEFINED)
					if (!Teammate(te, self->owner) || te == self->owner)
					if (!(te->invincible_finished > time))
					if (te->is_connected)
					if (!(te->armorclass & PR_AT_SAVEEXPLOSION) || random() < 0.25) // blast armor avoids our legs to be hurt most of the time
					if (te->origin[Z] > self->origin[Z]) // only hurt legs if hit from below..
					{
						if (HurtLegs(te,legdmg))
						{
							if (legdmg == PR_MINE_LVL3_LEGDMG)
								sprint(te,PR_PRINT_HIGH,"Woah! That mine really hurts your legs!\n");
							else
								sprint(te,PR_PRINT_HIGH,"That mine hurts your legs!\n");
						}
					}

					te = te->chain;
			   }
		   }
		}

		WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
		WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
	#ifdef PR_QUAKE_WORLD
		multicast (self->origin, PR_MULTICAST_PHS);
		dremove(self);
	#else
		BecomeExplosion ();
	#endif
	 }
	 else // Non-standard mines
	 {
		self->netname = "special_mine"; // avoids recursive calls and lets us identify mines vs grens
		self->touch = SUB_Null;
		self->think = SUB_Null;
		self->nextthink = -1;
		self->takedamage = PR_DAMAGE_NO; // NEEDED or crash

		st = GetMineStr(self->has_sentry);
		st2 = "";

		if (self->has_holo)
			st2 = " converted";

		if (self->has_tesla == 2) // set by GuerillaSwep for trace weapons
			sprint(self->owner,PR_PRINT_HIGH,"Your ",st,st2," mine is destroyed\n");

		if (self->has_tesla == 0) // default
			sprint(self->owner,PR_PRINT_HIGH,"Your ",st,st2," mine explodes\n");

		if (!charged) // not armed?
		{
			WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
			WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
			WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
			WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
			WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
		#ifdef PR_QUAKE_WORLD
			multicast (self->origin, PR_MULTICAST_PHS);
			dremove(self);
		#else
			BecomeExplosion ();
		#endif
		}
		else // special mine is charged..
		{
			if (self->has_sentry == PR_MINE_CONCUSSION)
				ConcussionGrenadeExplode();
			else if (self->has_sentry == PR_MINE_PSIONIC)
				PsionicGrenadeExplode();
			else if (self->has_sentry == PR_MINE_BIO)
				BioGrenadeExplode();
			else if (self->has_sentry == PR_MINE_NAPALM)
				NapalmGrenadeExplode();
			else if (self->has_sentry == PR_MINE_MIRV)
				MirvGrenadeExplode();
			else if (self->has_sentry == PR_MINE_NAIL)
				NailGrenadeExplode();
			else if (self->has_sentry == PR_MINE_GAS)
				GasGrenadeExplode();
			else if (self->has_sentry == PR_MINE_EMP)
				EMPGrenadeExplode();
			else if (self->has_sentry == PR_MINE_FRAG)
				FragGrenadeExplode();
			else if (self->has_sentry == PR_MINE_KRAC)
				KracGrenadeExplode();
			else if (self->has_sentry == PR_MINE_CALTROP)
				CaltropGrenadeExplode();
			else if (self->has_sentry == PR_MINE_ANTIGRAV)
				AntiGravGrenadeExplode();
			else
			{
				RPrint("BUG: Unknown mine type on GuerillaExplode()!\n");
				dremove(self);
				return;
			}
		}
	 }
}

//=========================================================================
// Detonate all mines the mine_owner player entity has

void DetonateMines(entity mine_owner)
{
	entity e;
	entity oself;

	oself = self;

	// Find any mine
	e = find(world, "netname", "land_mine");

	while (e != world)
	{
	   if(e->classname == "grenade")
	   if(e->owner == mine_owner) {
		 /*e.heat = time;
		 e.has_tesla = 1; //- display no message on GuerillaExplode
		 e.think = GuerillaExplode;
		 e.nextthink = time;*/
		 self = e;
		 e->heat = time;
		 e->has_tesla = 1;
		 GuerillaExplode();
	   }
	   e = find(e, "netname", "land_mine");
	}

	self = oself;
}

//====================================================
//Code to detonate the guerilla mines on traceattack weapons.
//Has small radius, just large enough to hit what was aimed at

void GuerillaMineSweep(const vector& startpos)
{
	entity head;
	head = findradius(startpos, 30);
	while (head != world)
	{
		if (head->classname == "grenade" && head->netname == "land_mine") {

		  head->has_tesla = 2; // "mine is destroyed" message on guerillaexplode()
		  head->think = GuerillaExplode;
		  head->nextthink = time + 0.1;

		}
		head = head->chain;
	}
}

//============================================================
// Think of mines, makes sound, scans and explodes if needed

void GuerillaThink() //Every second see if we have enemy nearby
{
	if (!self->owner->is_connected)
	{
		dremove(self);
		return;
	}

	entity head;
	float finished; //Only blow up once

	if (self->has_fieldgen == 1)
	{
		self->effects = 0;
		self->has_fieldgen = 2;
		self->nextthink = time + 0.1;
		return;
	}
	if (self->has_fieldgen == 2)
	{
		self->effects = PR_EF_DIMLIGHT;
		self->has_fieldgen = 3;
		self->nextthink = time + 0.1;
		return;
	}
	if (self->has_fieldgen == 3)
	{
		self->effects = 0;
		self->has_fieldgen = 4;
	}

	finished = 0;

	self->nextthink = time + PR_MINE_SCANRATE;

	if (time < self->heat + PR_GUERILLA_ACTIVATE_TIME)
		return;

	if (time > self->last_attacked_time) {
		sound (self, PR_CHAN_WEAPON, "weapons/guerblip.wav", 1, PR_ATTN_IDLE);
		self->last_attacked_time = time + PR_GUERILLA_BEEP_RATE;

		if (!self->has_fieldgen)
		{
			self->has_fieldgen = 1;
			self->effects = PR_EF_DIMLIGHT;
			self->nextthink = time + 0.1;
			return;
		}
	}

	string desc, conv;
	float hasextras;

	if (self->has_holo)
		conv = " converted";
	else
		conv = "";

	if (self->owner->job & PR_JOB_EXTRA1 || self->owner->job & PR_JOB_EXTRA2)
		hasextras = PR_TRUE;
	else
		hasextras = PR_FALSE;

	desc = GetMineStr(self->has_sentry);

	self->health = self->health - 1;

	if (self->health <= 0) { //Detonate mine cause we ran out of time

		float minesup;

		minesup = GetMinesUp(self->owner);

		self->heat = time; //Make it a smaller explosion

		if (hasextras)
			sprint(self->owner,PR_PRINT_HIGH,"Your ",desc,conv," mine runs out of energy, ");
		else
			sprint(self->owner,PR_PRINT_HIGH,"Your mine runs out of energy, ");

		if (minesup > 1) {
		   string st;
		   st = ftos (minesup - 1);

		   if (self->owner->job & PR_JOB_EXTRA1 || self->owner->job & PR_JOB_EXTRA2)
		   {
			   sprint(self->owner,PR_PRINT_HIGH,"you still have ");
			   sprint(self->owner,PR_PRINT_HIGH,st);
			   sprint(self->owner,PR_PRINT_HIGH," mines up\n");
		   }
		   else
		   {
			   sprint(self->owner,PR_PRINT_HIGH,"you still have ");
			   sprint(self->owner,PR_PRINT_HIGH,st);
			   sprint(self->owner,PR_PRINT_HIGH,"/" TO_STR(PR_GUERILLA_NOEXTRAS_MINEMAX) " mines up\n");
		   }
		}
		else
		   sprint(self->owner,PR_PRINT_HIGH,"you currently have no active mines\n");
		self->has_tesla = 1; //- display no message on GuerillaExplode
		GuerillaExplode();
		finished = 1;
	}
	head = findradius(self->origin,PR_GUERILLA_RADIUS);//OfN it was 150
	while (head != world && !finished) {
#ifdef PR_COOP_MODE_ENHANCED
		if (((head->classname == "player" && head->playerclass != PR_PC_UNDEFINED) || COOP_IsCoopMonster( head )) && head->health > 0) {
#else
		if (head->classname == "player" && head->health > 0)
		{
			if (head->playerclass != PR_PC_UNDEFINED) // skip Observers
			{
#endif
				//Mines detonate on either enemies or yourself
				if (CanDamage(head,self)) {
					if (!Teammate(head, self->owner) && head->undercover_team != self->owner->team_no)
					{						//Uncouvered ENEMY
						if (hasextras)
							sprint(self->owner,PR_PRINT_HIGH,"your ",desc,conv," mine explodes on ");
						else
							sprint(self->owner,PR_PRINT_HIGH,"your mine explodes on ");

						sprint(self->owner,PR_PRINT_HIGH,head->netname);

						if (head->origin[Z] > self->origin[Z])
							sprint(self->owner,PR_PRINT_HIGH,"'s ass!\n");
						else
							sprint(self->owner,PR_PRINT_HIGH,"'s face!\n");

						self->has_tesla = 1; //- display no message on GuerillaExplode
						GuerillaExplode();
						finished = 1;
					} else if (head == self->owner) { // ourselves

					   self->has_tesla = 1; //- display no message on GuerillaExplode
					   GuerillaExplode();
					   finished = 1;
					}
				}
#ifndef PR_COOP_MODE_ENHANCED
			}
#endif
		}
		else if (IsOwnedMonster(head) && !Teammate(head->real_owner, self->owner)) { //- OfN - monsters and grunt are affected
			T_Damage(head,self,self->owner,PR_MINE_DMG); //Demons are vulnerable
			self->has_tesla = 1; //- display no message on GuerillaExplode
			GuerillaExplode();
			finished = 1;
		}
		head = head->chain;
	}
	//if (finished && self.has_sensor)
	//	dremove(self);
}

//================================================================
// What happens when someone runs it over

void GuerillaTouch()
{
	if (time < self->heat + PR_GUERILLA_ACTIVATE_TIME)
		return;

	if (other->playerclass == PR_PC_UNDEFINED) // skip Observers
		return;

	#ifdef PR_MINES_IGNORE_TEAMMATETOUCH
	if (other != self->owner)
	if (other->classname == "player" || IsMonster(other))	// Gizmo - allow non demons and non army soldiers to be affected (coop monsters)
	if (Teammate(self->owner, other))
	if (other->health > 0)
		return;
	#endif

	if (other->classname == "player")
	if (other->health > 0)
	if (other->is_connected)
	{
		string desc, conv;
		conv = "";
		desc = GetMineStr(self->has_sentry);

		if (self->has_holo)
			conv = " converted";

		if (self->owner->job & PR_JOB_EXTRA1 || self->owner->job & PR_JOB_EXTRA2)
			sprint(self->owner,PR_PRINT_HIGH,other->netname," touches your ",desc,conv," mine!\n");
		else
			sprint(self->owner,PR_PRINT_HIGH,other->netname," touches your mine!\n");
	}

	GuerillaExplode();
}

void GuerillaTossTouch()
{
	if (other != world) {
		GuerillaTouch();
		return;
	}

	if (pointcontents(self->origin) == PR_CONTENT_SKY || pointcontents(self->origin) == PR_CONTENT_SOLID) //CH if in wall or sky
	{
		self->health = 0; //CH needed for tesla
		// OfN CAUSES ANY PROBLEM? Is this needed?

		MineDown(self->owner,self);

		dremove(self);
		return;
	}

	self->angles = self->velocity = self->avelocity = V({0, 0, 0});
	self->movetype = PR_MOVETYPE_NONE;
	sound (self, PR_CHAN_WEAPON, "weapons/guerset.wav", 1, PR_ATTN_NORM);
	self->touch = GuerillaTouch;
	self->think = GuerillaThink;
	self->nextthink = time + 1;
}

//==================================================================
// Called when a guerilla player uses 'skill'

void JobGuerilla()
{
	if (self->job & PR_JOB_EXTRA1 || self->job & PR_JOB_EXTRA2)
	{
		if (self->current_menu == PR_MENU_GUERILLA)
		{
			GuerillaSetMine(PR_MINE_LEVEL1,0);
			return;
		}

		self->current_menu = PR_MENU_GUERILLA;
		self->menu_count = PR_MENU_REFRESH_RATE;
		return;
	}

	GuerillaSetMine(PR_MINE_REGULAR,0);
}

//============================================================================
// Sets a mine of the specified kind

void GuerillaSetMine(float minetype, float converted)
{
	if (!minetype)
	{
		// if this gets executed its a bug
		RPrint("BUG: Invalid mine type on GuerillaSetMine()\n");
		return;
	}

	if (!AttemptToSetMine(self,minetype,converted)) return;

	self->ammo_rockets = self->ammo_rockets - PR_GUERILLA_MINE_COST;
	self->job_finished = time + PR_GUERILLA_JOB_DELAY;

	if (self->job & PR_JOB_EXTRA1 || self->job & PR_JOB_EXTRA2)
	{
		string st;
		st = GetMineStr(minetype);

		if (converted)
			sprint(self,PR_PRINT_HIGH,"you convert a grenade into a ",st," mine...\n");
		else
			sprint(self,PR_PRINT_HIGH,"you place a ",st," mine...\n");
	}
	else
		sprint(self,PR_PRINT_HIGH,"you place a mine...\n");

	if (self->current_menu == PR_MENU_GUERILLA)
		ResetMenu();

	#ifdef PR_TEAM_MINE_REPORTS
	teamprefixsprint(self->team_no,self);
	teamsprint(self->team_no,self,self->netname);
	teamsprint(self->team_no,self," places a mine\n");
	#endif

	//Set all the critical crap on the mine
	newmis = spawn();
	newmis->movetype = PR_MOVETYPE_BOUNCE;
	newmis->solid = PR_SOLID_BBOX;
	newmis->takedamage = PR_DAMAGE_AIM;
	newmis->classname = "grenade";
	newmis->netname	 = "land_mine";
	newmis->team_no = self->team_no;          // PZ: keep bots from shooting friendly mines
	//newmis.flags = #FL_FINDABLE_NONSOLID; // For mine ID
	//setsize (newmis, '-1 -1 0', '1 1 0.5');
	//setsize (newmis, '0 0 0','0 0 0');          // PZ: using the below size (which was commented) because bots shoot too high at mines
	setsize (newmis, V({-0.5, -0.5, -0.5}), V({1, 1, 1}));  // PZ: don't know why they were using 0 for size, unfortunately
	setorigin (newmis, self->origin);
	newmis->owner = self;
	makevectors (self->v_angle);
	newmis->avelocity = V({300, 300, 300});
	newmis->velocity = v_forward*600 + v_up * 200 + v_right*10 + v_up*10;
	newmis->angles = vectoangles(newmis->velocity);

	if (minetype == PR_MINE_NAPALM)
		newmis->skin = 2;
	else if (minetype == PR_MINE_MIRV)
		newmis->skin = 0;
	else // default look
		newmis->skin = 1;

	//setmodel (newmis, "progs/lndmine.mdl");
	setmodel (newmis, "progs/biggren.mdl");

	newmis->heat = time; //Controls when mine can first go off
	newmis->has_tesla = 0; // OfN flag to Control what to sprint to owner of the mine when it exlodes
	newmis->last_attacked_time = time; // Controls beeps
	newmis->health = PR_GUERILLA_MINE_DURATION; //Max time for mine to live
	newmis->touch = GuerillaTossTouch;
	newmis->think = GuerillaThink;
	newmis->nextthink = time + PR_GUERILLA_ACTIVATE_TIME;

	newmis->has_sentry = minetype;

	newmis->has_fieldgen = 0;

	if (minetype == PR_MINE_LEVEL1 || minetype == PR_MINE_LEVEL2 || minetype == PR_MINE_LEVEL3)
		newmis->has_sensor = 1; // Standard mine
	else
		newmis->has_sensor = 0; // Non-standard mine

	newmis->has_holo = converted;

	MineUp(self,newmis);
}

//==============================================================================
// We have just killed some1 with our mines, award point/bonuses if appropiate

void MineKill(entity player)
{
	if (!(player->job & PR_JOB_GUERILLA))
	{
		//RPrint("BUG: Non-guerilla was in MineKill()!\n");
			// ^_^ not a bug, may happen on cleanup

		return;
	}

	// increase our mine kills counter
	player->dont_do_triggerwork = player->dont_do_triggerwork + 1;

	// award extra guerilla points if appropiate
	if (player->job & PR_JOB_EXTRA1 || player->job & PR_JOB_EXTRA2)
	{
		float tfloat, tint, maxpoints, spent;

		tfloat = player->dont_do_triggerwork / PR_GUERILLA_FRAGSFORPOINT;
		tint = floor(player->dont_do_triggerwork / PR_GUERILLA_FRAGSFORPOINT);

		if (tfloat == tint)
		{
			tfloat = GetMineEarnedPoints(player);
			tint = GetMinePoints(player);
			spent = GetMineSpentPoints(player);
			maxpoints = GuerillaGetMaxPointsForJob(player);

			if ((tint + spent + 1) > maxpoints)
			{
				// Do nothing
			}
			else
			{
				sprint(player,PR_PRINT_HIGH,"You have earned an extra guerilla point!\n");

				SetMinePoints(player, tint + 1);
				SetMineEarnedPoints(player, tfloat + 1);
			}

		}
	}

	if (extras_mode == 2)
		return;

	// reward with job extras
	if (player->dont_do_triggerwork == PR_GUERILLA_BONUS1)
		GuerillaUpgradeJob(player);
	else if (player->dont_do_triggerwork == PR_GUERILLA_BONUS2)
		GuerillaUpgradeJob(player);
}

//====================================================================
// We use the 3 different bytes on .all_active field for the stuff

float GetMineSpentPoints(entity player)
{
	return GetByte1(player->all_active);
}

float GetMinePoints(entity player)
{
	return GetByte2(player->all_active);
}

float GetMineEarnedPoints(entity player)
{
	return GetByte3(player->all_active);
}

void SetMineSpentPoints(entity player,float spentpoints)
{
	player->all_active = AssignByte1(player->all_active,spentpoints);
}

void SetMinePoints(entity player,float points)
{
	player->all_active = AssignByte2(player->all_active,points);
}

void SetMineEarnedPoints(entity player,float points)
{
	player->all_active = AssignByte3(player->all_active,points);
}

//============================================================
// Returns an appropiate text for the given kind of gren

string GetGrenadeText(float typ)
{
	string st;

	if (typ == PR_GR_TYPE_NORMAL)
		st = "Normal";
	else if (typ == PR_GR_TYPE_CONCUSSION)
		st = "Concussion";
	else if (typ == PR_GR_TYPE_NAIL)
		st = "Nail";
	else if (typ == PR_GR_TYPE_MIRV)
		st = "Mirv";
	else if (typ == PR_GR_TYPE_NAPALM)
		st = "Napalm";
	else if (typ == PR_GR_TYPE_FLARE)
		st = "Flare";
	else if (typ == PR_GR_TYPE_GAS)
		st = "Gas";
	else if (typ == PR_GR_TYPE_EMP)
		st = "EMP";
	#ifdef PR_OLD_FLASH
	else if (typ == PR_GR_TYPE_FLASH)
		st = "Flash";
	#else
	else if (typ == PR_GR_TYPE_PSIONIC)
		st = "Psionic";
	#endif
	else if (typ == PR_GR_TYPE_FRAG)
		st = "Fragmentation";
	else if (typ == PR_GR_TYPE_KRAC)
		st = "KRAC";
	else if (typ == PR_GR_TYPE_CALTROP)
		st = "Caltrop";
	else if (typ == PR_GR_TYPE_BIO)
		st = "Biological";
	else if (typ == PR_GR_TYPE_ANTIGRAV)
		st = "Antigrav";
	else st = "ERROR!";

	return st;
}

//====================================================================
// Returns TRUE if the specified gren type is "convertible" to a mine

float IsMineConvertible(float typ)
{
	if (typ == PR_GR_TYPE_NORMAL)
		return PR_TRUE;
	if (typ == PR_GR_TYPE_CONCUSSION)
		return PR_TRUE;
	if (typ == PR_GR_TYPE_NAIL)
		return PR_TRUE;
	if (typ == PR_GR_TYPE_MIRV)
		return PR_TRUE;
	if (typ == PR_GR_TYPE_NAPALM)
		return PR_TRUE;
	if (typ == PR_GR_TYPE_FLARE)
		return PR_FALSE;
	if (typ == PR_GR_TYPE_GAS)
		return PR_TRUE;
	if (typ == PR_GR_TYPE_EMP)
		return PR_TRUE;
	if (typ == PR_GR_TYPE_PSIONIC)
		return PR_TRUE;
	if (typ == PR_GR_TYPE_FRAG)
		return PR_TRUE;
	if (typ == PR_GR_TYPE_KRAC)
		return PR_TRUE;
	if (typ == PR_GR_TYPE_CALTROP)
		return PR_TRUE;
	if (typ == PR_GR_TYPE_BIO)
		return PR_TRUE;
	if (typ == PR_GR_TYPE_ANTIGRAV)
		return PR_TRUE;

	return PR_FALSE;
}

//===========================================================================
// Gets the name of the given mine kind

string GetMineStr(float minetype)
{
	if (minetype == PR_MINE_LEVEL1)
		return "level 1";
	if (minetype == PR_MINE_LEVEL2)
		return "level 2";
	if (minetype == PR_MINE_LEVEL3)
		return "level 3";
	if (minetype == PR_MINE_CONCUSSION)
		return "concussion";
	if (minetype == PR_MINE_PSIONIC)
		return "psionic";
	if (minetype == PR_MINE_BIO)
		return "biological";
	if (minetype == PR_MINE_NAPALM)
		return "napalm";
	if (minetype == PR_MINE_MIRV)
		return "MIRV";
	if (minetype == PR_MINE_NAIL)
		return "nail";
	if (minetype == PR_MINE_GAS)
		return "gas";
	if (minetype == PR_MINE_EMP)
		return "EMP";
	if (minetype == PR_MINE_FRAG)
		return "fragmentation";
	if (minetype == PR_MINE_KRAC)
		return "krac";
	if (minetype == PR_MINE_CALTROP)
		return "caltrop";
	if (minetype == PR_MINE_ANTIGRAV)
		return "antigrav";

	return "ERROR!";
}

//==========================================================
// Converts gren ID number into its corresponding mine ID

float ConvertGrenIDToMine(float grentype)
{
	if (grentype == PR_GR_TYPE_NORMAL)
		return PR_MINE_REGULAR;
	if (grentype == PR_GR_TYPE_CONCUSSION)
		return PR_MINE_CONCUSSION;
	if (grentype == PR_GR_TYPE_NAIL)
		return PR_MINE_NAIL;
	if (grentype == PR_GR_TYPE_MIRV)
		return PR_MINE_MIRV;
	if (grentype == PR_GR_TYPE_NAPALM)
		return PR_MINE_NAPALM;
	if (grentype == PR_GR_TYPE_GAS)
		return PR_MINE_GAS;
	if (grentype == PR_GR_TYPE_EMP)
		return PR_MINE_EMP;
	if (grentype == PR_GR_TYPE_PSIONIC)
		return PR_MINE_PSIONIC;
	if (grentype == PR_GR_TYPE_FRAG)
		return PR_MINE_FRAG;
	if (grentype == PR_GR_TYPE_KRAC)
		return PR_MINE_KRAC;
	if (grentype == PR_GR_TYPE_CALTROP)
		return PR_MINE_CALTROP;
	if (grentype == PR_GR_TYPE_BIO)
		return PR_MINE_BIO;
	if (grentype == PR_GR_TYPE_ANTIGRAV)
		return PR_MINE_ANTIGRAV;

	return 0; // Bug if happens
}

//========================================================================
// The following funcs are used to update stuff when a mine goes down/up

void MineUp(entity player, entity mine)
{
	#ifdef PR_MINE_DEBUG
	if (debug_target == player)
	if (debug_target->is_connected)
	{
		sprint(debug_target,PR_PRINT_HIGH,"MineUp()\n");
	}
	#endif

	float minesup;

	minesup = GetMinesUp(player);

	if (mine->has_holo) // Converted grenade mine
	{
		int bitfield;

		bitfield = GetMineConvBitfield(player);

		if (bitfield & (int)mine->has_holo)
			RPrint("BUG: Double conversion of mine!\n"); // shouldnt happen

		bitfield = bitfield | (int)mine->has_holo;

		SetMineConvBitfield(player,bitfield);

		if (mine->has_holo == 1)
		{
			if (!player->no_grenades_1)
				RPrint("BUG: MineUp() called with no grenades to be able to convert!\n");
			else
				player->no_grenades_1 = player->no_grenades_1 - 1;
		}
		else
		{
			if (!player->no_grenades_2)
				RPrint("BUG: MineUp() called with no grenades to be able to convert!\n");
			else
				player->no_grenades_2 = player->no_grenades_2 - 1;
		}
	}
	else // non-converted mine
	{
		float points;
		float neededpoints;
		float spent;

		points = GetMinePoints(player);
		neededpoints = MineNeededPoints(mine->has_sentry);

		if (neededpoints > points) // shouldnt happen
			RPrint("BUG: Points error in MineUp()!\n");

		// decrease our guerilla available points
		SetMinePoints(player, points - neededpoints);

		// increase our spent points counter
		spent = GetMineSpentPoints(player);
		SetMineSpentPoints(player, spent + neededpoints);

		#ifdef PR_MINE_DEBUG
		if (debug_target == player)
		if (debug_target->is_connected)
		{
			string st, st2;

			st = ftos(points);
			st2 = ftos(neededpoints);

			sprint(player,PR_PRINT_HIGH,"points: ",st," needed: ",st2,"\n");

			st = ftos(spent);
			st2 = ftos(points - neededpoints);

			sprint(player,PR_PRINT_HIGH,"spent: ",st," result: ",st2, "\n");

			st = ftos (minesup + 1);
			sprint (player,PR_PRINT_HIGH,"Res mines: ",st,"\n");
		}
		#endif
	}

	// increase our mine counter
	SetMinesUp(player, minesup + 1);
}

void MineDown(entity player, entity mine)
{
	#ifdef PR_MINE_DEBUG
	if (debug_target == player)
	if (debug_target->is_connected)
	{
		sprint(debug_target,PR_PRINT_HIGH,"MineDown()\n");
	}
	#endif

	float minesup;

	minesup = GetMinesUp(player);

	if (mine->has_holo) // converted grenade mine
	{
		int bitfield;

		bitfield = GetMineConvBitfield(player);

		/*if (!(bitfield & mine.has_holo))
			RPrint("BUG: Wrong assignment of converted mine detected!\n"); // shouldnt happen*/

			// ^_^ this happens when CleanUpEverything() is called

		bitfield = bitfield - (bitfield & (int)mine->has_holo);

		SetMineConvBitfield(player,bitfield);
	}
	else // non-converted mine
	{
		float points;
		float neededpoints;
		float spent;

		points = GetMinePoints(player);
		neededpoints = MineNeededPoints(mine->has_sentry);

		// increase our guerilla available points
		SetMinePoints(player,points + neededpoints);

		// decrease our spent points counter
		spent = GetMineSpentPoints(player);
		SetMineSpentPoints(player,spent - neededpoints);

		#ifdef PR_MINE_DEBUG
		if (debug_target == player)
		if (debug_target->is_connected)
		{
			string st, st2;

			st = ftos(points);
			st2 = ftos(neededpoints);

			sprint(player,PR_PRINT_HIGH,"points: ",st," needed: ",st2,"\n");

			st = ftos(spent);
			st2 = ftos(spent - neededpoints);

			sprint(player,PR_PRINT_HIGH,"spent: ",st," result: ",st2, "\n");

			st = ftos (minesup - 1);
			sprint (player,PR_PRINT_HIGH,"Res mines: ",st,"\n");
		}
		#endif
	}

	// decrease our mine counter
	SetMinesUp(player, minesup - 1);
}

//========================================================================
// Returns TRUE if able to set the specified mine, and reports if not

float AttemptToSetMine(entity player, float minetype, int converted)
{
	float minesup;

	minesup = GetMinesUp(player);

	// have we set too many mines already? (fixed max)
	if (minesup >= PR_GUERILLA_FIXED_MINEMAX)
	{
		sprint(player,PR_PRINT_HIGH,"you can't set more than " TO_STR(PR_GUERILLA_FIXED_MINEMAX) " mines!\n");

		player->job_finished = time + 1;
		return PR_FALSE;
	}

	// not enough ammo for mine? (rockets)
	if (player->ammo_rockets < PR_GUERILLA_MINE_COST)
	{
		sprint(player,PR_PRINT_HIGH,"every mine needs " TO_STR(PR_GUERILLA_MINE_COST) " rockets to work!\n");

		player->job_finished = time + 1;
		return PR_FALSE;
	}

	// converted grenade?
	if (converted)
	{
		// we already have this mine up?
		int bitfield;
		bitfield = GetMineConvBitfield(player);

		if (bitfield & converted)
			return PR_FALSE;
		else
			return PR_TRUE; //..ok, set mine
	}
	else // non-converted mines
	{
		if (player->job & PR_JOB_EXTRA1 || player->job & PR_JOB_EXTRA2)
		{
			float points, neededpoints;

			points = GetMinePoints(player);
			neededpoints = MineNeededPoints(minetype);

			if (points < neededpoints)
			{
				sprint(player,PR_PRINT_HIGH,"not enough guerilla points to set that mine!\n");
				return PR_FALSE;
			}
			else
				return PR_TRUE; //..ok, set mine
		}
		else // standard guerilla (no job extras)
		{
			if (minesup >= PR_GUERILLA_NOEXTRAS_MINEMAX)
			{
				sprint(player,PR_PRINT_HIGH,"you can set upto " TO_STR(PR_GUERILLA_NOEXTRAS_MINEMAX) " mines!\n");
				return PR_FALSE;
			}
			else
				return PR_TRUE; //..ok, set mine
		}
	}

	return PR_FALSE; // wont happen anyway..
}

//===========================================================
// Returns needed points for the given mine type

float MineNeededPoints(float minetype)
{
	if (minetype == PR_MINE_LEVEL1)
		return 1;
	if (minetype == PR_MINE_LEVEL2)
		return 2;
	if (minetype == PR_MINE_LEVEL3)
		return 3;
	if (minetype == PR_MINE_CONCUSSION)
		return 1;
	if (minetype == PR_MINE_PSIONIC)
		return 2;
	if (minetype == PR_MINE_BIO)
		return 2;
	if (minetype == PR_MINE_NAPALM)
		return 3;
	if (minetype == PR_MINE_MIRV)
		return 3;

	/*if (minetype == #MINE_NAIL)
		return ;
	if (minetype == #MINE_GAS)
		return ;
	if (minetype == #MINE_EMP)
		return ;
	if (minetype == #MINE_FRAG)
		return ;
	if (minetype == #MINE_KRAC)
		return ;
	if (minetype == #MINE_CALTROP)
		return ;
	if (minetype == #MINE_ANTIGRAV)
		return ;*/

	return 99; // bug if this value is taken into account
}

//==============================================================================
// Initializes guerilla points (MUST BE CALLED WITH NO MINES UP and only once)

void GuerillaInit(entity player)
{
	player->all_active = 0; // first reset everything

	// then add the corresponding points
	if (player->job & PR_JOB_EXTRA1)
		SetMinePoints(player, PR_GUERILLA_EXTRA1_POINTSMIN);
	else if (player->job & PR_JOB_EXTRA2)
		SetMinePoints(player, PR_GUERILLA_EXTRA2_POINTSMIN);
	else SetMinePoints(player, PR_GUERILLA_NOEXTRAS_MINEMAX);

	// also reset mines up flags and counter
	player->increase_team4 = 0;
}

//===========================================================================
// Functions to handle flags and variables on .increase_team4 field

float GetMinesUp(entity player)
{
	return GetByte1(player->increase_team4);
}

void SetMinesUp(entity player, float minesup)
{
	player->increase_team4 = AssignByte1(player->increase_team4,minesup);
}

float GetMineConvBitfield(entity player)
{
	return GetByte2(player->increase_team4);
}

void SetMineConvBitfield(entity player, float bitfield)
{
	player->increase_team4 = AssignByte2(player->increase_team4,bitfield);
}

//=================================================================
// Returns the max number of points the player may have by job

float GuerillaGetMaxPointsForJob(entity player)
{
	if (player->job & PR_JOB_GUERILLA)
	{
		if (player->job & PR_JOB_EXTRA2)
			return PR_GUERILLA_EXTRA2_POINTSMAX;
		else if (player->job & PR_JOB_EXTRA1)
			return PR_GUERILLA_EXTRA1_POINTSMAX;
		else
			return PR_GUERILLA_NOEXTRAS_MINEMAX;
	}
	else
		return 0;
}

//==================================================================
// Upgrades job, called from MineKill() if needed

void GuerillaUpgradeJob(entity player)
{
	float currentpoints, spent;

	if (player->job & PR_JOB_EXTRA2)
		return; // cant upgrade anymore

	if (player->job & PR_JOB_EXTRA1)
	{
		sprint(player,PR_PRINT_HIGH,"You are now a guerilla specialist!\n");

		player->job = (player->job | PR_JOB_EXTRA2) - PR_JOB_EXTRA1;

		// adjust points if needed and for sanity
		spent = GetMineSpentPoints(player);
		currentpoints = GetMinePoints(player) + spent;

		if (currentpoints < PR_GUERILLA_EXTRA2_POINTSMIN)
		{
			SetMinePoints(player, PR_GUERILLA_EXTRA2_POINTSMIN - spent);
			currentpoints = GetMinePoints(player);
		}

		if (currentpoints > PR_GUERILLA_EXTRA2_POINTSMAX)
			SetMinePoints(player, PR_GUERILLA_EXTRA2_POINTSMAX - spent);
	}
	else // no extras
	{
		sprint(player,PR_PRINT_HIGH,"You have become a guerilla technician!\n");

		player->job = player->job | PR_JOB_EXTRA1;

		// adjust points if needed and for sanity
		spent = GetMineSpentPoints(player);
		currentpoints = GetMinePoints(player) + spent;

		if (currentpoints < PR_GUERILLA_EXTRA1_POINTSMIN)
		{
			SetMinePoints(player, PR_GUERILLA_EXTRA1_POINTSMIN - spent);
			currentpoints = GetMinePoints(player);
		}

		if (currentpoints > PR_GUERILLA_EXTRA1_POINTSMAX)
			SetMinePoints(player, PR_GUERILLA_EXTRA1_POINTSMAX - spent);
	}
}

//=================================================================
// returns the total points a player has (available+spent)

/*float(entity player) GuerillaGetTotalPoints =
{
	local float spent, avail;
	avail = GetMinePoints(player);
	spent = GetMineSpentPoints(player);

	return spent + avail;
};*/

} // END namespace Progs
