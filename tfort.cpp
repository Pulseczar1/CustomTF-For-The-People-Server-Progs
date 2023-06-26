/*======================================================
	TFORT.QC			Custom TeamFortress v3.2

	(c) TeamFortress Software Pty Ltd	29/2/97
	(c) William Kerney			9/16/00
	(c) Craig Hauser				19/3/00
========================================================
Class handling and other utility functions
======================================================*/

#include "progs.h"
#include "tfort.h"
#include "ofndefs.h"
#include "custom.h"
#include "tforttm.h"
#include "debug.h"
#include "player.h"
#include "tfortmap.h"
#include "menu.h"
#include "agr.h"
#include "invade.h"
#include "qw.h"
#include "weapons.h"
#include "scout.h"
#include "tsoldier.h"
#include "demoman.h"
#include "pyro.h"
#include "spy.h"
#include "engineer.h"
#include "speed.h"
#include "jobs.h"
#include "cpstuff.h"
#include "army.h"
#include "neo.h"

namespace Progs {

// Function Prototypes
//void() RemoveFlare;
void GrenadeExplode();
void spike_touch();
void bound_other_ammo(entity p);
void DisplayItemStatus(entity Goal, entity Player, entity Item);
void TF_T_Damage(entity targ, entity inflictor, entity attacker, float damage, int T_flags, float T_AttackType);

// Map Function Prototypes
void tfgoalitem_RemoveFromPlayer(entity Item, entity AP, float method);

// Help Functions
void TeamFortress_MOTD();

// Team Functions
float TeamFortress_TeamGetColor(float tno);
void TeamFortress_TeamSetColor(float tno);
float TeamFortress_TeamPutPlayerInTeam(float performChecks);
float TeamFortress_TeamGetScore(float tno);
int TeamFortress_TeamGetIllegalClasses(float tno);

//WK
void DropToCustomClassGen(); //In custom.qc
float W_BestWeapon(); //In weapons.qc
void FragGrenadeTouch(); //In custom.qc
void FragGrenadeExplode();
void KracGrenadeTouch();
void KracGrenadeExplode();
void AntiGravGrenadeExplode();
void Autoitem_think();
void DetonateAllGuns();

// Impulse Functions
void TeamFortress_ChangeClass();
void TeamFortress_DisplayLegalClasses();
void TeamFortress_Inventory();
void Grunty_PrintDetails( entity dest, entity g, float printLevel);
void TeamFortress_ShowTF();
void TeamFortress_PrimeGrenade();
void TeamFortress_ThrowGrenade();
void TeamFortress_DisplayDetectionItems();

// Player Class Handling Functions
float IsLegalClass(float pc);
//void() TeamFortress_SetAlias; //WK
void TeamFortress_SetHealth();
void TeamFortress_SetEquipment();
void TeamFortress_SetSpeed(entity p);
void TeamFortress_SetSkin(entity p);
void TeamFortress_PrintClassName(entity Viewer, float pc, float rpc);
void TeamFortress_RemoveTimers();
void TeamFortress_SetupRespawn(float Suicided);
void TeamFortress_CheckClassStats();
float TeamFortress_GetMaxAmmo(entity Retriever, float AmmoType);
float TeamFortress_CanGetWeapon(entity Retriever, int WeaponType);
float TeamFortress_AddBackpackItems(entity Retriever, entity Items);
float TeamFortress_DescribeArmor(entity Player, int Armorclass);
#ifdef PR_SPEECH
void TeamFortress_SayClassName(entity player);
#endif

// AmmoBox Handling Functions
void TeamFortress_AmmoboxTouch();
float num_team_ammoboxes(float tno);
void RemoveOldAmmobox(float tno);
void increment_team_ammoboxes(float tno);
void decrement_team_ammoboxes(float tno);

// Weapon Handling Functions
void TeamFortress_ExplodePerson();
void NormalGrenadeTouch();
void NormalGrenadeExplode();
void PlayerObserverMode();

// Utility Functions
void TeamFortress_Alias(const string& halias, float himpulse1, float himpulse2);

// Cyclic Event Functions
void TeamFortress_Regenerate();
void TeamFortress_RegenerateCyber();
void TeamFortress_RegenerateCells();

//CH
float Return_Custom_Skins(entity p);

//- OfN
void FlareGrenadeTouch();
void FlareGrenadeExplode();
void FlareBounce();
void grunty_boundammo(entity sld);
void UpdateCells(entity player);
void BioGrenadeTouch();
void BioGrenadeExplode();
float GetMaxGrens(entity theplayer, float grenslot);
string GetRuneString(float runetype);
float StripDisallowed(entity player);
float GiveFreeStuff(entity player);
float RestoreOriginalItems(entity player);
void ResetStripedAndGiven(entity player);

//=========================================================================
// IMPULSE FUNCTIONS
//=========================================================================
// Uses a class dependant special skill
void UseSpecialSkill()
{
	self->impulse = 0;

	if (self->cutf_items & PR_CUTF_SENTRYGUN || self->tf_items & PR_NIT_TESLA || self->tf_items & PR_NIT_SECURITY_CAMERA || self->tf_items & PR_NIT_TELEPORTER || self->cutf_items & PR_CUTF_DISPENSER || self->cutf_items & PR_CUTF_FIELDGEN)
		self->impulse = PR_TF_ENGINEER_BUILD;
	else if (self->cutf_items & PR_CUTF_SPY_KIT)
		self->impulse = PR_TF_SPY_SPY;
	else if (self->weapons_carried & PR_WEAP_GRENADE_LAUNCHER)
		self->impulse = PR_TF_PB_DETONATE;
	else if (self->tf_items & PR_NIT_SCANNER)
		self->impulse = PR_TF_SCAN_30;
	else if (self->weapons_carried & PR_WEAP_SNIPER_RIFLE)
		self->impulse = PR_TF_AUTOZOOM;
	else
		self->impulse = PR_TF_RELOAD;

	//WK Since most people don't know "disguise" or "build"
	// we'll allow them to toggle between the two with special.
	// This wipes out .money, so watch out for side effects.
	// PZ: I want to preserve .money, so that players can 'edit' their class when they re-custom. So, using .worldtype instead.
	if (!(self->done_custom & PR_CUSTOM_BUILDING) && (self->impulse == PR_TF_ENGINEER_BUILD) && (self->cutf_items & PR_CUTF_SPY_KIT))
	{
		if (self->worldtype) self->worldtype = 0;
		else self->worldtype = 1;

		if (self->worldtype)
			self->impulse = PR_TF_ENGINEER_BUILD;
		else
		{
			if (self->effects & (PR_EF_DIMLIGHT | PR_EF_BRIGHTLIGHT)) //CH why not have the check here too, saves a keypress
				self->impulse = PR_TF_ENGINEER_BUILD;
			else
				self->impulse = PR_TF_SPY_SPY;
		}
	}
}


//=========================================================================
// Player change class function
// If self.impulse == 1, change to Civilian class
void TeamFortress_ChangeClass()
{
	entity spot, te;
	//local float tc;
	string st;
	//local string scratch;
	float destClass;

	//WK - Handle Custom Class Restarting
	if (self->playerclass == PR_PC_CUSTOM && self->done_custom & PR_CUSTOM_BUILDING) {
		destClass = self->impulse - PR_TF_CHANGEPC;
		DropToCustomClassGen(); //Comatose state

		// Gizmo - if we typed a stock class then buy the items for that class
		if ( destClass != PR_PC_CUSTOM )
			BuyStockClassItems( destClass );
		return;
	}

	// PZ NOTE: This section is for when players already have a class. It won't (re)spawn them. It will just set things up
	//          for them to change class when they die.
	if (self->playerclass != PR_PC_UNDEFINED && !(self->playerclass == PR_PC_CUSTOM && (self->done_custom & PR_CUSTOM_BUILDING)))
	{
#ifdef PR_COOP_MODE_ENHANCED
		// In Deathmatch 3, you can change class after you die
		if (deathmatch != 3 && !coop)
			return;
#else
		// In Deathmatch 3, you can change class after you die
		if (deathmatch != 3)
			return;
#endif

		// Civilian Teams can never change class
		if (TeamFortress_TeamIsCivilian(self->team_no))
		{
			sprint(self, PR_PRINT_HIGH, "You're a civilian. Learn to live with it.\n");
			return;
		}

		if (!IsLegalClass(self->impulse - PR_TF_CHANGEPC))
		{
			sprint(self, PR_PRINT_HIGH, "Your team cannot play that class.\n");

			TeamFortress_DisplayLegalClasses();
			return;
		}

		self->nextpc = self->impulse - PR_TF_CHANGEPC;
		//WK - Handle Custom Class
		if (self->nextpc == PR_PC_CUSTOM)
		{
			self->done_custom = self->done_custom | PR_CUSTOM_ON_SPAWN; //Generate a new custom class when we respawn
		}

		sprint(self, PR_PRINT_HIGH, "After dying, you will return as a ");
		st = TeamFortress_GetClassName(self->nextpc);
		st = colstr(st,PR_COLSTR_RED);
		sprint(self, PR_PRINT_HIGH, st, "\n");
		return;
	}

	// players must join a team, if teamplay is on
	if (teamplay && self->team_no == 0)
	{
		sprint(self, PR_PRINT_HIGH, "You must join a team first. \n");
		return;
	}

	// Only change if you've got any lives left
	if ( self->lives == 0 )
	{
		sprint(self, PR_PRINT_HIGH, "You have no lives left.\n");

		return;
	}

	if (!IsLegalClass(self->impulse - PR_TF_CHANGEPC) && (self->impulse != 1))
	{
		sprint(self, PR_PRINT_HIGH, "You cannot play that playerclass on this map. \n");

		TeamFortress_DisplayLegalClasses();
		return;
	}

	// Spy may be disabled
	if ((spy_off == PR_TRUE) && ((self->impulse - PR_TF_CHANGEPC) == PR_PC_SPY))
	{
		sprint(self, PR_PRINT_HIGH, "The spy class has been disabled on the server by the administrator.\n");
		return;
	}

	if (self->impulse != 1)
		self->playerclass = self->impulse - PR_TF_CHANGEPC;
	else
		self->playerclass = PR_PC_CIVILIAN;

	self->nextpc = 0;

	// Turn off #PC_UNDEFINED's nomove and invincibility
	self->takedamage = PR_DAMAGE_AIM;
	//WK Prevent bad movetype errors, or something
	if (self->classname != "player") {
		RPrint("BUG BUG BUG BUG BUG BUG BUG BUG BUG\n");
		RPrint("Non-player was in tfort.qc::changeclass()!\n");
		return;
	}

	self->movetype = PR_MOVETYPE_WALK;
	self->flags = PR_FL_CLIENT | PR_FL_ONGROUND;
	self->waterlevel = 0;
	self->air_finished = time + 12;

	// give them a model, and a new spawn point
	self->solid = PR_SOLID_SLIDEBOX;
	// pausetime is set by teleporters to keep the player from moving a while
	self->pausetime = 0;

	spot = SelectSpawnPoint ();

#ifdef PR_MAP_DEBUG
	RPrint(self->netname);
	RPrint(" spawned at a ");
	RPrint(spot->netname);
	if (spot->team_no != 0)
	{
		RPrint(", team_no of ");
		st = ftos(spot->team_no);
		RPrint(st);
	}
	RPrint("\n");
#endif

	self->origin = spot->origin + V({0, 0, 1});
	self->angles = spot->angles;
	self->fixangle = PR_TRUE;		// turn this way immediately

	setmodel (self, string_null);
	modelindex_null = self->modelindex;

	setmodel (self, "progs/eyes.mdl");
	modelindex_eyes = self->modelindex;

	setmodel (self, "progs/player.mdl");
	modelindex_player = self->modelindex;

	setsize (self, PR_VEC_HULL_MIN, PR_VEC_HULL_MAX);

	self->view_ofs = V({0, 0, 22});
	player_stand1 ();
	if (deathmatch || coop)
	{
		makevectors(self->angles);
		spawn_tfog (self->origin + v_forward*20);
	}

	// Display chosen class
	if ( self->playerclass == PR_PC_RANDOM )
	{
		sprint(self, PR_PRINT_HIGH, "Random Playerclass.\n");

		self->tfstate = (self->tfstate | PR_TFSTATE_RANDOMPC);
		self->playerclass = 1 + floor(random() * (PR_PC_RANDOM - 1));
	}

	// If this is a TeamSpawnpoint, check to see if it
	// gives out a GoalItem, or displays a message
	if (spot->classname == "info_player_teamspawn")
	{
		if (spot->items != 0)
		{
			te = Finditem(spot->items);
			if (te != world)
				tfgoalitem_GiveToPlayer(te, self, self);

			if (!(spot->goal_activation & PR_TFSP_MULTIPLEITEMS))
				spot->items = 0;
		}

		if (spot->message != "")
		{
			CenterPrint(self, spot->message);

			if (!(spot->goal_activation & PR_TFSP_MULTIPLEMSGS))
				spot->message = string_null;
		}

		// TeamSpawn points can remove themselves after being spawned on
		if (spot->goal_effects == PR_TFSP_REMOVESELF)
		{
			spot->classname = "deadpoint";
			spot->team_str_home = string_null;
			spot->nextthink = time + 1;
			spot->think = SUB_Remove;
		}
	}

	//WK - Handle Custom Class
	if (self->playerclass == PR_PC_CUSTOM) {
		DropToCustomClassGen(); //Comatose state
	}

	ResetMenu();
	TeamFortress_PrintClassName(self,self->playerclass, (self->tfstate & PR_TFSTATE_RANDOMPC));
#ifdef PR_SPEECH
	TeamFortress_SayClassName(self);
#endif
	TeamFortress_SetEquipment();
	TeamFortress_SetHealth();
	TeamFortress_SetSpeed(self);
	TeamFortress_SetSkin(self);
	//WK Give them invincibility if they are a normal class or bought it
	if (self->playerclass >= PR_PC_SCOUT && self->playerclass <= PR_PC_RANDOM && !normalQuakeMap) { // PZ: respawn guard is annoying as hell on DM maps
		self->items = self->items + PR_IT_INVULNERABILITY;
		self->invincible_time = 1;
#ifdef PR_COOP_MODE_ENHANCED
		if ( !deathmatch )
			self->invincible_finished = time + PR_COOP_RESPAWN_GUARD_TIME;
		else
#endif
		self->invincible_finished = time + PR_RESPAWN_GUARD_TIME;
	}

	// Gizmo - enable telefragging when choosing a stock class
	spawn_tdeath (self->origin, self);
}

//=========================================================================
// Display a list of all the legal classes for this map
void TeamFortress_DisplayLegalClasses()
{
	float gotone;
	int ill;

	sprint(self, PR_PRINT_HIGH, "Legal Classes for your team are:\n");
	gotone = PR_FALSE;

	ill = TeamFortress_TeamGetIllegalClasses(self->team_no);

	if (!(illegalclasses & PR_TF_ILL_SCOUT) && !(ill & PR_TF_ILL_SCOUT))
	{
		if (gotone)
			sprint(self, PR_PRINT_HIGH, ", ");
		gotone = PR_TRUE;
		sprint(self, PR_PRINT_HIGH, "Scout");
	}
	if (!(illegalclasses & PR_TF_ILL_SNIPER) && !(ill & PR_TF_ILL_SNIPER))
	{
		if (gotone)
			sprint(self, PR_PRINT_HIGH, ", ");
		gotone = PR_TRUE;
		sprint(self, PR_PRINT_HIGH, "Sniper");
	}
	if (!(illegalclasses & PR_TF_ILL_SOLDIER) && !(ill & PR_TF_ILL_SOLDIER))
	{
		if (gotone)
			sprint(self, PR_PRINT_HIGH, ", ");
		gotone = PR_TRUE;
		sprint(self, PR_PRINT_HIGH, "Soldier");
	}
	if (!(illegalclasses & PR_TF_ILL_DEMOMAN) && !(ill & PR_TF_ILL_DEMOMAN))
	{
		if (gotone)
			sprint(self, PR_PRINT_HIGH, ", ");
		gotone = PR_TRUE;
		sprint(self, PR_PRINT_HIGH, "Demolitions Man");
	}
	if (!(illegalclasses & PR_TF_ILL_MEDIC) && !(ill & PR_TF_ILL_MEDIC))
	{
		if (gotone)
			sprint(self, PR_PRINT_HIGH, ", ");
		gotone = PR_TRUE;
		sprint(self, PR_PRINT_HIGH, "Combat Medic");
	}
	if (!(illegalclasses & PR_TF_ILL_HVYWEP) && !(ill & PR_TF_ILL_HVYWEP))
	{
		if (gotone)
			sprint(self, PR_PRINT_HIGH, ", ");
		gotone = PR_TRUE;
		sprint(self, PR_PRINT_HIGH, "Heavy Weapons Guy");
	}
	if (!(illegalclasses & PR_TF_ILL_PYRO) && !(ill & PR_TF_ILL_PYRO))
	{
		if (gotone)
			sprint(self, PR_PRINT_HIGH, ", ");
		gotone = PR_TRUE;
		sprint(self, PR_PRINT_HIGH, "Pyro");
	}
	if (!(illegalclasses & PR_TF_ILL_SPY) && !(ill & PR_TF_ILL_SPY))
	{
		if (gotone)
			sprint(self, PR_PRINT_HIGH, ", ");
		gotone = PR_TRUE;
		sprint(self, PR_PRINT_HIGH, "Spy");
	}
	if (!(illegalclasses & PR_TF_ILL_ENGINEER) && !(ill & PR_TF_ILL_ENGINEER))
	{
		if (gotone)
			sprint(self, PR_PRINT_HIGH, ", ");
		gotone = PR_TRUE;
		sprint(self, PR_PRINT_HIGH, "Engineer");
	}
	if (!(illegalclasses & PR_TF_ILL_RANDOMPC) && !(ill & PR_TF_ILL_RANDOMPC))
	{
		if (gotone)
			sprint(self, PR_PRINT_HIGH, ", ");
		gotone = PR_TRUE;
		sprint(self, PR_PRINT_HIGH, "RandomPC");
	}

	sprint(self, PR_PRINT_HIGH, "\n");
}

//=========================================================================
// Displays the player's inventory

void PrintOwnedMonsterInfo( entity printDest, entity thething);

void TeamFortress_Inventory()
{
	entity tg;
	string ac;
	float col;
	float special; //CH used to determine if a \n is needed for a line
	special = 0;

	// Display Team
	col = TeamFortress_TeamGetColor(self->team_no);

	sprint(self, PR_PRINT_HIGH, "You're in team ");
	ac = ftos(self->team_no);
	sprint(self, PR_PRINT_HIGH, ac);
	sprint(self, PR_PRINT_HIGH, ", color ");
	ac = ftos(col);
	sprint(self, PR_PRINT_HIGH, ac);
	sprint(self, PR_PRINT_HIGH, ".\n");

	// Display Lives, if applicable
	if (self->lives != -1)
	{
		ac = ftos(self->lives);

		sprint(self, PR_PRINT_HIGH, "You've got ");
		sprint(self, PR_PRINT_HIGH, ac);
		if (self->lives == 1)
			sprint(self, PR_PRINT_HIGH, " life.\n");
		else
			sprint(self, PR_PRINT_HIGH, " lives.\n");

	}

	// Display Number of Grenades of each type
	//WK Fix BUG grenades
	if (self->tp_grenades_1 == 0)
		self->no_grenades_1 = 0;
	if (self->tp_grenades_2 == 0)
		self->no_grenades_2 = 0;

	if (self->no_grenades_1 > 0)
	{
		sprint(self, PR_PRINT_HIGH, "Gren.Type 1: ");
		if (self->tp_grenades_1 == PR_GR_TYPE_NORMAL)
			sprint(self, PR_PRINT_HIGH, "Normal(");
		else if (self->tp_grenades_1 == PR_GR_TYPE_CONCUSSION)
			sprint(self, PR_PRINT_HIGH, "Concussion(");
		else if (self->tp_grenades_1 == PR_GR_TYPE_ANTIGRAV)
			sprint(self, PR_PRINT_HIGH, "AntiGrav(");
		else if (self->tp_grenades_1 == PR_GR_TYPE_NAIL)
			sprint(self, PR_PRINT_HIGH, "Nail(");
		else if (self->tp_grenades_1 == PR_GR_TYPE_MIRV)
			sprint(self, PR_PRINT_HIGH, "Mirv(");
		else if (self->tp_grenades_1 == PR_GR_TYPE_NAPALM)
			sprint(self, PR_PRINT_HIGH, "Napalm(");
		else if (self->tp_grenades_1 == PR_GR_TYPE_FLARE)
			sprint(self, PR_PRINT_HIGH, "Flare(");
		else if (self->tp_grenades_1 == PR_GR_TYPE_GAS)
			sprint(self, PR_PRINT_HIGH, "Hallucinogenic(");
		else if (self->tp_grenades_1 == PR_GR_TYPE_EMP)
			sprint(self, PR_PRINT_HIGH, "EMP(");
		#ifdef PR_OLD_FLASH
		else if (self->tp_grenades_1 == PR_GR_TYPE_FLASH)
			sprint(self, PR_PRINT_HIGH, "Flash(");
		#else
		else if (self->tp_grenades_1 == PR_GR_TYPE_PSIONIC)
			sprint(self, PR_PRINT_HIGH, "Psionic(");
		#endif
		else if (self->tp_grenades_1 == PR_GR_TYPE_FRAG)
			sprint(self, PR_PRINT_HIGH, "Frag(");
		else if (self->tp_grenades_1 == PR_GR_TYPE_KRAC)
			sprint(self, PR_PRINT_HIGH, "Krac(");
		else if (self->tp_grenades_1 == PR_GR_TYPE_CALTROP)
			sprint(self, PR_PRINT_HIGH, "Caltrop(");
		else if (self->tp_grenades_1 == PR_GR_TYPE_BIO)
			sprint(self, PR_PRINT_HIGH, "Biological(");
		else
			sprint(self, PR_PRINT_HIGH, "BUG(");

		ac = ftos(self->no_grenades_1);
		sprint(self, PR_PRINT_HIGH, ac);
		sprint(self, PR_PRINT_HIGH, ")\n");
	}
	if (self->no_grenades_2 > 0)
	{
		sprint(self, PR_PRINT_HIGH, "Gren.Type 2: ");
		if (self->tp_grenades_2 == PR_GR_TYPE_NORMAL)
			sprint(self, PR_PRINT_HIGH, "Normal(");
		else if (self->tp_grenades_2 == PR_GR_TYPE_CONCUSSION)
			sprint(self, PR_PRINT_HIGH, "Concussion(");
		else if (self->tp_grenades_2 == PR_GR_TYPE_NAIL)
			sprint(self, PR_PRINT_HIGH, "Nail(");
		else if (self->tp_grenades_2 == PR_GR_TYPE_ANTIGRAV)
			sprint(self, PR_PRINT_HIGH, "AntiGrav(");
		else if (self->tp_grenades_2 == PR_GR_TYPE_CALTROP)
			sprint(self, PR_PRINT_HIGH, "Caltrop(");
		else if (self->tp_grenades_2 == PR_GR_TYPE_MIRV)
			sprint(self, PR_PRINT_HIGH, "Mirv(");
		else if (self->tp_grenades_2 == PR_GR_TYPE_BIO)
			sprint(self, PR_PRINT_HIGH, "Biological(");
		else if (self->tp_grenades_2 == PR_GR_TYPE_NAPALM)
			sprint(self, PR_PRINT_HIGH, "Napalm(");
		else if (self->tp_grenades_2 == PR_GR_TYPE_FLARE)
			sprint(self, PR_PRINT_HIGH, "Flare(");
		else if (self->tp_grenades_2 == PR_GR_TYPE_GAS)
			sprint(self, PR_PRINT_HIGH, "Hallucinogenic(");
		else if (self->tp_grenades_2 == PR_GR_TYPE_EMP)
			sprint(self, PR_PRINT_HIGH, "EMP(");
		#ifdef PR_OLD_FLASH
		else if (self->tp_grenades_2 == PR_GR_TYPE_FLASH)
			sprint(self, PR_PRINT_HIGH, "Flash(");
		#else
		else if (self->tp_grenades_2 == PR_GR_TYPE_PSIONIC)
			sprint(self, PR_PRINT_HIGH, "Psionic(");
		#endif
		else if (self->tp_grenades_2 == PR_GR_TYPE_FRAG)
			sprint(self, PR_PRINT_HIGH, "Frag(");
		else if (self->tp_grenades_2 == PR_GR_TYPE_KRAC)
			sprint(self, PR_PRINT_HIGH, "Krac(");
		else
			sprint(self, PR_PRINT_HIGH, "BUG(");

		ac = ftos(self->no_grenades_2);
		sprint(self, PR_PRINT_HIGH, ac);
		sprint(self, PR_PRINT_HIGH, ")\n");
	}


	// Scanner
	if (self->tf_items & PR_NIT_SCANNER)
	{
		sprint(self, PR_PRINT_HIGH, "Scanner. ");
		special = special + 1; //CH determines \n
	}
	if (self->cutf_items & PR_CUTF_JAMMER)
	{
		sprint(self, PR_PRINT_HIGH, "Jammer. ");
		special = special + 1; //CH determines \n
	}

	if (self->cutf_items & PR_CUTF_CYBERAUG && !(self->weapons_carried & PR_WEAP_MEDIKIT))
	{
		sprint(self, PR_PRINT_HIGH, "CyberAug (");
		ac = ftos(self->ammo_medikit);
		sprint(self, PR_PRINT_HIGH, ac);
		sprint(self, PR_PRINT_HIGH, "). ");
		special = special + 1; //CH determines \n
	}
	else if (self->cutf_items & PR_CUTF_CYBERAUG && self->weapons_carried & PR_WEAP_MEDIKIT)
		sprint(self, PR_PRINT_HIGH, "CyberAug/");
	// Medikit and ammo
	if (self->weapons_carried & PR_WEAP_MEDIKIT)
	{
		sprint(self, PR_PRINT_HIGH, "Medikit (");
		ac = ftos(self->ammo_medikit);
		sprint(self, PR_PRINT_HIGH, ac);
		sprint(self, PR_PRINT_HIGH, "). ");
		special = special + 1; //CH determines \n
	}

	//if (self.cutf_items & #CUTF_INTERFACE)  //- OfN -
	//	sprint(self, #PRINT_HIGH, "Cyber Interface. ");


	// Detpack
	if (self->cutf_items & PR_CUTF_DETPACK)
	{
		if (self->ammo_detpack > 0)
		{
			ac = ftos(self->ammo_detpack);

			sprint(self, PR_PRINT_HIGH, ac);
			sprint(self, PR_PRINT_HIGH, " Detpack");
			if (self->ammo_detpack > 1)
				sprint(self, PR_PRINT_HIGH, "s");
			sprint(self, PR_PRINT_HIGH, ". ");
			special = special + 1; //CH determines \n
		}
	}
	if (self->cutf_items & PR_CUTF_TOSSABLEDET)
	{
		if (self->ammo_c4det > 0)
		{
			ac = ftos(self->ammo_c4det);

			sprint(self, PR_PRINT_HIGH, ac);
			sprint(self, PR_PRINT_HIGH, " Tossable Detpack");
			if (self->ammo_c4det > 1)
				sprint(self, PR_PRINT_HIGH, "s");
			sprint(self, PR_PRINT_HIGH, ". ");
			special = special + 1; //CH determines \n
		}
	}

	// GoalItems
	tg = find (world, "classname", "item_tfgoal");
	while (tg != world)
	{
		if (tg->owner == self)
		{
			sprint(self, PR_PRINT_HIGH, tg->netname);
			sprint(self, PR_PRINT_HIGH, ". ");
			special = special + 1; //CH determines \n
		}

		tg = find(tg, "classname", "item_tfgoal");
	}

	// Armor
	if (self->armorvalue > 0)
	{
		special = special + TeamFortress_DescribeArmor(self, self->armorclass);
	}
	if (special > 0)
		sprint(self, PR_PRINT_HIGH, "\n");

	if (!invis_only)
	{
		// Spy gets told what skin and color he/she is wearing
		if ((self->cutf_items & PR_CUTF_SPY_KIT) && invis_only == PR_FALSE)
		{
			sprint (self, PR_PRINT_HIGH, "Skin : ");
			if (self->undercover_skin != 0)
				TeamFortress_PrintClassName(self,self->undercover_skin,0);
			else
				TeamFortress_PrintClassName(self,self->skin,0);

			sprint (self, PR_PRINT_HIGH, "Colors : Team ");
			if (self->undercover_team != 0)
				ac = ftos(self->undercover_team);
			else
				ac = ftos(self->team_no);
			sprint (self, PR_PRINT_HIGH, ac);
			sprint(self, PR_PRINT_HIGH, "\n");
		}
	}

	// OfN - Display the health of any summons/army guys
	if (self->job & PR_JOB_WARLOCK || self->job & PR_JOB_ARMY)
	{
		if ( self->demon_one  != world)
			PrintOwnedMonsterInfo( self, self->demon_one );

		if ( self->demon_two  != world)
			PrintOwnedMonsterInfo( self, self->demon_two );

		if ( self->demon_three  != world)
			PrintOwnedMonsterInfo( self, self->demon_three );
	}

	if (self->has_sentry)
	{
		tg = find(world, "classname", "building_sentrygun");
		while (tg != world)
		{
			if (tg->real_owner == self)
			{
				ac = ftos(ceil(tg->health));

				if (!tg->dont_do_triggerwork || !(self->cutf_items & PR_CUTF_DOUBLESENTRY))
					sprint (self, PR_PRINT_HIGH, S_("^bsentry^b: "));
				else
				{
					if (tg->dont_do_triggerwork == 1)
						sprint (self, PR_PRINT_HIGH, S_("^bsentry #^1^b: "));
					else if (tg->dont_do_triggerwork == 2)
						sprint (self, PR_PRINT_HIGH, S_("^bsentry #^2^b: "));
					else
						sprint (self, PR_PRINT_HIGH, S_("^bsentry^b: "));
				}
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_("^b/^b"));
				ac = ftos(ceil(tg->max_health));
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_(" ^bhp^b\n"));

			}
			tg = find(tg, "classname", "building_sentrygun");
		}
	}
	if (self->has_tesla)
	{
		tg = find(world, "classname", "building_tesla");
		while (tg != world)
		{
			if (tg->real_owner == self)
			{
				ac = ftos(ceil(tg->health));


				if (!tg->dont_do_triggerwork || !(self->tf_items & PR_NIT_DOUBLETESLA))
					sprint (self, PR_PRINT_HIGH, S_("^btesla^b: "));
				else
				{
					if (tg->dont_do_triggerwork == 1)
						sprint (self, PR_PRINT_HIGH, S_("^btesla #^1^b: "));
					else if (tg->dont_do_triggerwork == 2)
						sprint (self, PR_PRINT_HIGH, S_("^btesla #^2^b: "));
					else
						sprint (self, PR_PRINT_HIGH, S_("^btesla^b: "));
				}

				//sprint (self, #PRINT_HIGH, "^btesla^b: ");
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_("^b/^b"));
				ac = ftos(ceil(tg->max_health));
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_(" ^bhp^b\n"));

			}
			tg = find(tg, "classname", "building_tesla");
		}
	}
	if (self->has_dispenser)
	{
		tg = find(world, "classname", "building_dispenser");
		while (tg != world)
		{
			if (tg->real_owner == self)
			{
				ac = ftos(ceil(tg->health));
				sprint (self, PR_PRINT_HIGH, S_("^bdispenser^b: "));
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_("^b/^b"));
				ac = ftos(ceil(tg->max_health));
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_(" ^bhp^b\n"));

			}
			tg = find(tg, "classname", "building_dispenser");
		}
	}
	if (self->has_sensor)
	{
		tg = find(world, "classname", "building_sensor");
		while (tg != world)
		{
			if (tg->real_owner == self)
			{
				ac = ftos(ceil(tg->health));
				sprint (self, PR_PRINT_HIGH, S_("^bsensor^b: "));
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_("^b/^b"));
				ac = ftos(ceil(tg->max_health));
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_(" ^bhp^b\n"));

			}
			tg = find(tg, "classname", "building_sensor");
		}
	}
	if (self->has_camera)
	{
		tg = find(world, "classname", "building_camera");
		while (tg != world)
		{
			if (tg->real_owner == self)
			{
				ac = ftos(ceil(tg->health));
				sprint (self, PR_PRINT_HIGH, S_("^bcamera^b: "));
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_("^b/^b"));
				ac = ftos(ceil(tg->max_health));
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_(" ^bhp^b\n"));

			}
			tg = find(tg, "classname", "building_camera");
		}
	}
	if (self->has_teleporter > 0)
	{
		tg = find(world, "classname", "building_teleporter");
		while (tg != world)
		{
			if (tg->real_owner == self)
			{
				ac = ftos(ceil(tg->health));
				sprint (self, PR_PRINT_HIGH, S_("^bteleporter^b: "));
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_("^b/^b"));
				ac = ftos(ceil(tg->max_health));
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_("^bhp^b "));

				ac = ftos(ceil(tg->ammo_cells));
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_("^b/^b"));
				ac = ftos(ceil(tg->maxammo_cells));
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_("^bcells^b"));

				if (!(tg->tf_items & PR_NIT_TURRET))
				{
					sprint (self, PR_PRINT_HIGH, "\n");
				}
				else
				{
					sprint (self, PR_PRINT_HIGH, S_("^b(^bT^b)^b\n"));
				}

			}
			tg = find(tg, "classname", "building_teleporter");
		}
	}
	if (self->has_fieldgen > 0)
	{
		tg = find(world, "classname", "building_fieldgen");
		while (tg != world)
		{
			if (tg->real_owner == self)
			{
				ac = ftos(ceil(tg->health));
				sprint (self, PR_PRINT_HIGH, S_("^bField Generator^b: "));
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_("^b/^b"));
				ac = ftos(ceil(tg->max_health));
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_("^bhp^b "));

				ac = ftos(ceil(tg->ammo_cells));
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_("^b/^b"));
				ac = ftos(ceil(tg->maxammo_cells));
				sprint (self, PR_PRINT_HIGH, ac);
				sprint (self, PR_PRINT_HIGH, S_("^bcells^b\n"));

			}
			tg = find(tg, "classname", "building_fieldgen");
		}
	}

	/*if (self.job & #JOB_WARLOCK && self.job & #JOB_DEMON_OUT)
	{
		sprint (self, #PRINT_HIGH, "You have a ");
		ac=GetMonsterName(self.demon_two);
		sprint (self, #PRINT_HIGH, ac);
		sprint (self, #PRINT_HIGH, " summoned: ");

		tg = find(world, classname, "monster_demon1");
		while (tg)
		{
			if (tg.real_owner == self)
			{
				ac = ftos(ceil(tg.health));
				//sprint (self, #PRINT_HIGH, " (");
				sprint (self, #PRINT_HIGH, ac);
				sprint (self, #PRINT_HIGH, " HP ");
			}
			tg = find(tg, classname, "monster_demon1");
		}

		tg = find(world, classname, "monster_army");
		while (tg)
		{
			if (tg.real_owner == self)
			{
				ac = ftos(ceil(tg.health));
				sprint (self, #PRINT_HIGH, " (");
				sprint (self, #PRINT_HIGH, ac);
				sprint (self, #PRINT_HIGH, ") ");
			}
			tg = find(tg, classname, "monster_army");
		}

		tg = find(world, classname, "monster_shambler");
		while (tg)
		{
			if (tg.real_owner == self)
			{
				ac = ftos(ceil(tg.health));
				//sprint (self, #PRINT_HIGH, " (");
				sprint (self, #PRINT_HIGH, ac);
				sprint (self, #PRINT_HIGH, " HP ");
			}
			tg = find(tg, classname, "monster_shambler");
		}

		tg = find(world, classname, "monster_wizard");
		while (tg)
		{
			if (tg.real_owner == self)
			{
				ac = ftos(ceil(tg.health));
				//sprint (self, #PRINT_HIGH, " (");
				sprint (self, #PRINT_HIGH, ac);
				ac = ftos(ceil(tg.max_health));
				sprint (self, #PRINT_HIGH, "\xAF");
				sprint (self, #PRINT_HIGH, ac);
				sprint (self, #PRINT_HIGH, " HP ");
			}
			tg = find(tg, classname, "monster_wizard");
		}

		//sprint (self, #PRINT_HIGH, ac);
		sprint (self, #PRINT_HIGH, "\n");
	}*/

	/*if (self.job & #JOB_WARLOCK) // they can see that on warlock menu
	{
		ac = ftos (self.demon_blood);
		sprint (self, #PRINT_HIGH, "Knife kills: ", ac, "\n");	// knife kills
	}*/

	//CH if you have boots, state percent of fuel
	if (self->tf_items & PR_NIT_HOVER_BOOTS)
	{
		if (self->hover_time <= 0)
			ac = "0";
		else
			ac = ftos(floor(100 * (self->hover_time / PR_MAX_HOVER_FUEL)));
		sprint (self, PR_PRINT_HIGH, "Boot Charge: ");
		sprint (self, PR_PRINT_HIGH, ac);
		sprint (self, PR_PRINT_HIGH, "%  ");
	}
	//CH if you have a job.  tell when can work
	if (self->job & PR_JOB_ALL)
	{
		if ((self->job_finished - time) <= 0)
			sprint (self,PR_PRINT_HIGH, S_("Skill Time: ^0"));
		else
		{
			ac = ftos(ceil(self->job_finished - time));
		sprint (self,PR_PRINT_HIGH, "Skill Time:");
		sprint (self, PR_PRINT_HIGH, ac);
		if (ceil(self->job_finished - time) == 1)
			sprint (self, PR_PRINT_HIGH, " second");
		else
			sprint (self, PR_PRINT_HIGH, " seconds");
		}
	}

	if (self->aura) //- OfN - If we have an aura it should tell
	{
		if (self->crusader_inspirator != world && self->crusader_inspirator->is_connected && self->crusader_inspirator->classname == "player")
		{
			sprint(self,PR_PRINT_HIGH,"\n");
			sprint(self,PR_PRINT_HIGH,self->crusader_inspirator->netname);
			sprint(self,PR_PRINT_HIGH," grants you the AURA of ");
		}
		else
			sprint(self,PR_PRINT_HIGH,"\nYou enjoy the AURA of ");

		if (self->aura == PR_AURA_POWER)
			sprint(self,PR_PRINT_HIGH,S_("^bpower^b."));
		else if (self->aura == PR_AURA_RESIS)
			sprint(self,PR_PRINT_HIGH,S_("^bresistance^b."));
		else if (self->aura == PR_AURA_HASTE)
			sprint(self,PR_PRINT_HIGH,S_("^bhaste^b."));
		else if (self->aura == PR_AURA_INVIS)
			sprint(self,PR_PRINT_HIGH,S_("^binvisibility^b."));
		else if (self->aura == PR_AURA_REGEN)
			sprint(self,PR_PRINT_HIGH,S_("^bregeneration^b."));
		else if (self->aura == PR_AURA_SPEED)
			sprint(self,PR_PRINT_HIGH,S_("^bspeed^b."));
	}

	if (!neo.isModeActive() && self->PR_runes) // OfN - List all the runes we own // Neo mod
	{
		if (self->PR_runes & PR_RUNE_TRIAD)
		{
			ac = GetRuneString(PR_RUNE_TRIAD);
			sprint(self,PR_PRINT_HIGH,"\nYou have the ",ac,".");
		}
		if (self->PR_runes & PR_RUNE_RESIS)
		{
			ac = GetRuneString(PR_RUNE_RESIS);
			sprint(self,PR_PRINT_HIGH,"\nYou have the ",ac,".");
		}
		if (self->PR_runes & PR_RUNE_SPEED)
		{
			ac = GetRuneString(PR_RUNE_SPEED);
			sprint(self,PR_PRINT_HIGH,"\nYou have the ",ac,".");
		}
		if (self->PR_runes & PR_RUNE_REGEN)
		{
			ac = GetRuneString(PR_RUNE_REGEN);
			sprint(self,PR_PRINT_HIGH,"\nYou have the ",ac,".");
		}
	}
	// PZ: Attackers Go Red mode
	if (agr) AGR_invPrint();

	// PZ: Invade mode
	else if (invade) Invade_invPrint();
	
	// PZ: Neo mode
	if (neo.isModeActive()) neo.invPrint();

	sprint (self, PR_PRINT_HIGH, "\n");
}

//=========================================================================
// Displays the state of the ToggleFlags
void TeamFortress_ShowTF()
{
	string st;

	// Class Persistence between lvls On/Off
	if (toggleflags & PR_TFLAG_CLASS_PERSIST)
		sprint (self, PR_PRINT_HIGH, "Class Persistence On.\n");
	else
		sprint (self, PR_PRINT_HIGH, "Class Persistence Off.\n");

	// Cheat Checking On/Off
	if (toggleflags & PR_TFLAG_CHEATCHECK)
		sprint (self, PR_PRINT_HIGH, "Cheat Checking On.\n");
	else
		sprint (self, PR_PRINT_HIGH, "Cheat Checking Off.\n");

	// AutoTeam On/Off
	if (toggleflags & PR_TFLAG_AUTOTEAM)
		sprint (self, PR_PRINT_HIGH, "AutoTeam On.\n");
	else
		sprint (self, PR_PRINT_HIGH, "AutoTeam Off.\n");

	// RespawnDelay
	if (toggleflags & PR_TFLAG_RESPAWNDELAY)
		st = ftos(respawn_delay_time);
	else
		st = "No";
	sprint (self, PR_PRINT_HIGH, st);
	if (st != "No")
		sprint (self, PR_PRINT_HIGH, " second");
	sprint (self, PR_PRINT_HIGH, " Respawn Delay.\n");

	// TeamFrags On/Off
	if (toggleflags & PR_TFLAG_TEAMFRAGS)
		sprint (self, PR_PRINT_HIGH, "TeamFrags On.\n");
	else
		sprint (self, PR_PRINT_HIGH, "TeamFrags Off.\n");

	// Grapple hook
	if (allow_hook)
		sprint (self, PR_PRINT_HIGH, "Grapple On.\n");
	else
		sprint (self, PR_PRINT_HIGH, "Grapple Off.\n");

}

void TeamFortress_GrenadePrimed();

//=========================================================================
// Primes a grenade of the type corresponding to the player's impulse
void TeamFortress_PrimeGrenade()
{
	float gtype;
	string gs, ptime;
	entity tGrenade;

	/*if (self.job & #JOB_THIEF && self.job & #JOB_FULL_HIDE) {
		sprint(self,#PRINT_HIGH,"You can't throw grens while invisible\n");
		return;
	}*/

	// If you've already primed a grenade, return
	if ((self->tfstate & PR_TFSTATE_GRENPRIMED) || (self->tfstate & PR_TFSTATE_GRENTHROWING))
	{
		return;
	}

	// WK 2/8/7 You can only prime one grenade per 3 seconds
	if (time < self->last_grenade_primed + 1)
	{
		//sprint(self,#PRINT_HIGH,"You can only prime one grenade every 3 seconds\n");
		return;
	}

	// OfN - Psionized guys can't prime grenades
	if (self->tfstate & PR_TFSTATE_PSIONIZED)
	{
		sprint(self,PR_PRINT_HIGH,"You hardly have control over your hands!\n");
		return;
	}

	if (self->impulse == PR_TF_GRENADE_1)
	{
#define PR_SPAM_TIME 7
		//WK Stop frag spammming // enabled again
		if (self->tp_grenades_1 == PR_GR_TYPE_FRAG && self->no_grenades_1 > 0) {
			if ((self->team_no == 1 && team1nextspam >= time) ||
				  (self->team_no == 2 && team2nextspam >= time) ||
					  (self->team_no == 3 && team3nextspam >= time) ||
				(self->team_no == 4 && team4nextspam >= time)) {
				sprint(self,PR_PRINT_HIGH,"Your team already has a spam grenade in the world.\n");
				return;
			}
			if (self->team_no == 1) team1nextspam = time + PR_SPAM_TIME;
			if (self->team_no == 2) team2nextspam = time + PR_SPAM_TIME;
			if (self->team_no == 3) team3nextspam = time + PR_SPAM_TIME;
			if (self->team_no == 4) team4nextspam = time + PR_SPAM_TIME;

		} // OfN Removed spam control // enabled again
		gtype = self->tp_grenades_1;
		if (gtype == PR_GR_TYPE_CONCUSSION)
			gs = "Concussion grenade";
		else if (gtype == PR_GR_TYPE_ANTIGRAV)
			gs = "AntiGrav grenade";
		else if (gtype == PR_GR_TYPE_BIO)
			gs = "Biological grenade";
		else if (gtype == PR_GR_TYPE_CALTROP)
			gs = "Caltrop grenade";
		else if (gtype == PR_GR_TYPE_NAIL)
			gs = "Nail grenade";
		else if (gtype == PR_GR_TYPE_MIRV)
			gs = "Mirv grenade";
		else if (gtype == PR_GR_TYPE_NAPALM)
			gs = "Napalm grenade";
		else if (gtype == PR_GR_TYPE_FLARE)
			gs = "Flare";
		else if (gtype == PR_GR_TYPE_GAS)
			gs = "Gas grenade";
		else if (gtype == PR_GR_TYPE_EMP)
			gs = "EMP grenade";
		#ifdef PR_OLD_FLASH
		else if (gtype == PR_GR_TYPE_FLASH)
			gs = "Flash grenade";
		#else
		else if (gtype == PR_GR_TYPE_PSIONIC)
			gs = "Psionic grenade";
		#endif
		else if (gtype == PR_GR_TYPE_FRAG)
			gs = "Fragmentation grenade";
		else if (gtype == PR_GR_TYPE_KRAC)
			gs = "Krac grenade";
		else
			gs = "Grenade";

		if (self->no_grenades_1 >= 1)
		{
			ptime = ftos( PR_GR_PRIMETIME );

			sprint(self, PR_PRINT_HIGH, gs);
			sprint(self, PR_PRINT_HIGH, " primed, ");
			sprint(self, PR_PRINT_HIGH, ptime);
			sprint(self, PR_PRINT_HIGH, " seconds...\n");

			/*if ((self.tp_grenades_1 == #GR_TYPE_NAIL || self.tp_grenades_1 == #GR_TYPE_FRAG) && self.no_grenades_1 > 2)
				self.no_grenades_1 = 2;*/

			if (self->no_grenades_1 > GetMaxGrens(self,1))
				self->no_grenades_1 = GetMaxGrens(self,1);


			self->no_grenades_1 = self->no_grenades_1 - 1;
			self->last_grenade_primed = time; //WK 2/26/7 Gren prime bugfix

			// OfN
			//sound(self, #CHAN_MISC, "weapons/pinpull.wav", 1, #ATTN_IDLE);
		}
		else
		{
			sprint(self, PR_PRINT_HIGH, "No ");
			sprint(self, PR_PRINT_HIGH, gs);
			sprint(self, PR_PRINT_HIGH, "s left.\n");
			return;
		}
	}
	//if (self.impulse == #TF_GRENADE_2)
	else
	{
		if (self->tp_grenades_2 == PR_GR_TYPE_FRAG  && self->no_grenades_2 > 0) {
			if ((self->team_no == 1 && team1nextspam >= time) ||
				(self->team_no == 2 && team2nextspam >= time) ||
					  (self->team_no == 3 && team3nextspam >= time) ||
				(self->team_no == 4 && team4nextspam >= time)) {
				sprint(self,PR_PRINT_HIGH,"Your team already has a spam grenade in the world.\n");
				return;
			}
			if (self->team_no == 1) team1nextspam = time + PR_SPAM_TIME + 1;
			if (self->team_no == 2) team2nextspam = time + PR_SPAM_TIME + 1;
			if (self->team_no == 3) team3nextspam = time + PR_SPAM_TIME + 1;
			if (self->team_no == 4) team4nextspam = time + PR_SPAM_TIME + 1;

		} // OfN Removed spam control
		gtype = self->tp_grenades_2;
		if (gtype == PR_GR_TYPE_CONCUSSION)
			gs = "Concussion grenade";
		else if (gtype == PR_GR_TYPE_ANTIGRAV)
			gs = "AntiGrav grenade";
		else if (gtype == PR_GR_TYPE_BIO)
			gs = "Biological grenade";
		else if (gtype == PR_GR_TYPE_CALTROP)
			gs = "Caltrop grenade";
		else if (gtype == PR_GR_TYPE_NAIL)
			gs = "Nail grenade";
		else if (gtype == PR_GR_TYPE_MIRV)
			gs = "Mirv grenade";
		else if (gtype == PR_GR_TYPE_NAPALM)
			gs = "Napalm grenade";
		else if (gtype == PR_GR_TYPE_FLARE)
			gs = "Flare";
		else if (gtype == PR_GR_TYPE_GAS)
			gs = "Gas grenade";
		else if (gtype == PR_GR_TYPE_EMP)
			gs = "EMP grenade";
		#ifdef PR_OLD_FLASH
		else if (gtype == PR_GR_TYPE_FLASH)
			gs = "Flash grenade";
		#else
		else if (gtype == PR_GR_TYPE_PSIONIC)
			gs = "Psionic grenade";
		#endif
		else if (gtype == PR_GR_TYPE_FRAG)
			gs = "Fragmentation grenade";
		else if (gtype == PR_GR_TYPE_KRAC)
			gs = "Krac grenade";
		else
			gs = "Grenade";


/*		  if (gtype == #GR_TYPE_CONCUSSION)
			gs = "Concussion grenade";
		else if (gtype == #GR_TYPE_NAIL)
			gs = "Nail grenade";
		else if (gtype == #GR_TYPE_MIRV)
			gs = "Mirv grenade";
		else if (gtype == #GR_TYPE_NAPALM)
			gs = "Napalm grenade";
		else if (gtype == #GR_TYPE_FLARE)
			gs = "Flare";
		else if (gtype == #GR_TYPE_GAS)
			gs = "Gas grenade";
		else if (gtype == #GR_TYPE_EMP)
			gs = "EMP grenade";
		else if (gtype == #GR_TYPE_FLASH)
			gs = "Flash grenade";
		else if (gtype == #GR_TYPE_FRAG)
			gs = "Fragmentation grenade";
		else if (gtype == #GR_TYPE_BIO)
			gs = "BioInfection grenade";
		else if (gtype == #GR_TYPE_CALTROP)
			gs = "Caltrop grenade";

		else if (gtype == #GR_TYPE_KRAC)
			gs = "Krac grenade";
		else
			gs = "Grenade";*/

		if (self->no_grenades_2 >= 1)
		{
			ptime = ftos( PR_GR_PRIMETIME );

			sprint(self, PR_PRINT_HIGH, gs);
			sprint(self, PR_PRINT_HIGH, " primed, ");
			sprint(self, PR_PRINT_HIGH, ptime);
			sprint(self, PR_PRINT_HIGH, " seconds...\n");

			//WK Limit Nail grenades to 2 - ofn and frag

			if (self->no_grenades_2 > GetMaxGrens(self,2))
				self->no_grenades_2 = GetMaxGrens(self,2);

			/*if ((self.tp_grenades_2 == #GR_TYPE_NAIL || self.tp_grenades_2 == #GR_TYPE_FRAG) && self.no_grenades_2 > 2)
				self.no_grenades_2 = 2;*/

			self->no_grenades_2 = self->no_grenades_2 - 1;
			self->last_grenade_primed = time; //WK 2/26/7 Gren Prime Bugfix

			//sound(self, #CHAN_MISC, "weapons/pinpull.wav", 1, #ATTN_IDLE);
		}
		else
		{
			sprint(self, PR_PRINT_HIGH, "No ");
			sprint(self, PR_PRINT_HIGH, gs);
			sprint(self, PR_PRINT_HIGH, "s left.\n");
			return;
		}
	}

	self->tfstate = (self->tfstate | PR_TFSTATE_GRENPRIMED);
	tGrenade = spawnServerSide(); // PZ: make it a server-side only entity
	tGrenade->owner = self;
	tGrenade->weapon = gtype;
	tGrenade->classname = "grenade_timer"; // OfN - New classname to fix gren in spawn
	tGrenade->nextthink = time + 0.8;
	tGrenade->heat = time + PR_GR_PRIMETIME + 0.8;
	tGrenade->think = TeamFortress_GrenadePrimed;
	// PZ NOTE: they are doing this because they want the grenade to explode as soon as it is 'thrown'
	if (gtype == PR_GR_TYPE_CALTROP)
		tGrenade->heat = tGrenade->heat - PR_GR_PRIMETIME;
}

void TeamFortress_GrenadePrimed()
{
	entity user;
	entity oldself;

	user = self->owner;

	if (!(user->tfstate & PR_TFSTATE_GRENTHROWING) && !user->deadflag)
	{
		self->nextthink = time + 0.1;

		if (self->think == SUB_Null)
			dremove(self);

		if (time > self->heat)
			TeamFortress_ExplodePerson();

		return;
	}

	if (!(user->tfstate & PR_TFSTATE_GRENPRIMED))
	{
		RPrint("GrenadePrimed logic error\n"); // OFN: FIXME: This happens!
		dremove(self); // OfN <-- fixed ancient bug?
		return; // OfN <-- fixed ancient bug?
	}

	user->tfstate = user->tfstate - (user->tfstate & PR_TFSTATE_GRENPRIMED);
	user->tfstate = user->tfstate - (user->tfstate & PR_TFSTATE_GRENTHROWING);

	// ofn
	if (self->weapon == PR_GR_TYPE_FLARE)
		sound (user, PR_CHAN_WEAPON, "weapons/flmfire2.wav", 1, PR_ATTN_NORM);
	else
		sound (user, PR_CHAN_WEAPON, "weapons/grenade.wav", 1, PR_ATTN_NORM);
		//sound (user, #CHAN_WEAPON, "weapons/throw.wav", 1, #ATTN_NORM);

	KickPlayer(-1, user);
	newmis = spawn ();
	newmis->owner = user;
	newmis->movetype = PR_MOVETYPE_BOUNCE;
	newmis->solid = PR_SOLID_BBOX;
	newmis->classname = "grenade";

	// set grenade speed
	makevectors (user->v_angle);

	if (user->deadflag)
	{
		// if user is dead, throw grenade directly up
		newmis->velocity = V({0, 0, 200});
	}
	else
	{
		if (user->v_angle[X])
		{
			newmis->velocity = v_forward*600 + v_up * 200 + crandom()*v_right*10 + crandom()*v_up*10;

			if (user->tfstate & PR_TFSTATE_PSIONIZED) // Weak toss when psionized
				newmis->velocity = newmis->velocity * 0.3;
		}
		else
		{
			newmis->velocity = aim(user, 10000);
			newmis->velocity = newmis->velocity * 600;
			newmis->velocity[Z] = 200;

			if (user->tfstate & PR_TFSTATE_PSIONIZED) // Weak toss when psionized
				newmis->velocity = newmis->velocity * 0.3;
		}
	}

	newmis->angles = vectoangles(newmis->velocity);

	// set the grenade's thinktime to when the PRIMETIME runs out
	newmis->think = SUB_Null;
	newmis->nextthink = self->heat;

	// set the think and touches to the appropriate grenade type
	if (self->weapon == PR_GR_TYPE_NORMAL)
	{
		newmis->touch = NormalGrenadeTouch;
		newmis->think = NormalGrenadeExplode;
		newmis->skin = 0;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/hgren2_orig.mdl");

	}
	else if (self->weapon == PR_GR_TYPE_CONCUSSION)
	{
		newmis->touch = ConcussionGrenadeTouch;
		newmis->think = ConcussionGrenadeExplode;
		newmis->skin = 1;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/hgren2_orig.mdl");
	}
	else if (self->weapon == PR_GR_TYPE_BIO)
	{
		newmis->touch = BioGrenadeTouch;
		newmis->think = BioGrenadeExplode;
		newmis->skin = 0;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/hgren2_orig.mdl");
	}
	else if (self->weapon == PR_GR_TYPE_CALTROP)
	{
		newmis->touch = SUB_Null;
		newmis->think = CaltropGrenadeExplode;
		newmis->skin = 0;
		newmis->avelocity = V({0, 0, 0});
		newmis->velocity = V({0, 0, 0});
		setmodel (newmis, string_null);
	}
	else if (self->weapon == PR_GR_TYPE_ANTIGRAV)
	{
		newmis->touch = ConcussionGrenadeTouch;
		newmis->think = AntiGravGrenadeExplode;
		newmis->skin = 1;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/hgren2_orig.mdl");
	}
	else if (self->weapon == PR_GR_TYPE_NAIL)
	{
		newmis->touch = NailGrenadeTouch;
		newmis->think = NailGrenadeExplode;
		newmis->skin = 1;
		newmis->avelocity = V({0, 300, 0});
		setmodel (newmis, "progs/biggren.mdl");
	}
	else if (self->weapon == PR_GR_TYPE_MIRV)
	{
		newmis->touch = MirvGrenadeTouch;
		newmis->think = MirvGrenadeExplode;
		newmis->skin = 0;
		newmis->avelocity = V({0, 300, 0});
		setmodel (newmis, "progs/biggren.mdl");
	}
	else if (self->weapon == PR_GR_TYPE_NAPALM)
	{
		newmis->touch = NapalmGrenadeTouch;
		newmis->think = NapalmGrenadeExplode;
		newmis->skin = 2;
		newmis->avelocity = V({100, 300, 100});
		setmodel (newmis, "progs/biggren.mdl");
	}
//CH flares no longer used
	else if (self->weapon == PR_GR_TYPE_FLARE)
	{
		newmis->movetype = PR_MOVETYPE_FLY;
		newmis->touch = FlareGrenadeTouch;
		newmis->think = FlareGrenadeExplode;
		//newmis.skin  = 0;//TF_FLARE_OFF;
		newmis->avelocity = V({0, 0, 0});


		makevectors (user->v_angle);
		newmis->velocity = v_forward;
		//newmis.velocity = aim(user, 10000);
		newmis->velocity = newmis->velocity * 2000;

//		  newmis.velocity = newmis.velocity * 4;
		//newmis.velocity_z = 0;
		setmodel (newmis, "progs/flarefly.mdl");
		newmis->skin=0;
		newmis->has_holo=0; //flag turned on when explodes
	}
	else if (self->weapon == PR_GR_TYPE_GAS)
	{
		newmis->touch = GasGrenadeTouch;
		newmis->think = GasGrenadeExplode;
		newmis->skin = 3;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/grenade2.mdl");
	}
	else if (self->weapon == PR_GR_TYPE_EMP)
	{
		newmis->touch = EMPGrenadeTouch;
		newmis->think = EMPGrenadeExplode;
		newmis->skin = 4;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/grenade2.mdl");
	}
	#ifdef PR_OLD_FLASH
	else if (self->weapon == PR_GR_TYPE_FLASH)
	{
		newmis->touch = FlashGrenadeTouch;
		newmis->think = FlashGrenadeExplode;
		newmis->skin = 2;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/hgren2_orig.mdl");
	}
	#else
	else if (self->weapon == PR_GR_TYPE_PSIONIC)
	{
		newmis->touch = PsionicGrenadeTouch;
		newmis->think = PsionicGrenadeExplode;
		newmis->skin = 2;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/hgren2_orig.mdl");
	}
	#endif
//WK New Grenades
	else if (self->weapon == PR_GR_TYPE_FRAG)
	{
		newmis->touch = FragGrenadeTouch;
		newmis->think = FragGrenadeExplode;
		newmis->skin = 0; //WK Green Skin
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/hgren2_orig.mdl");
	}
	else if (self->weapon == PR_GR_TYPE_KRAC)
	{
		newmis->touch = KracGrenadeTouch;
		newmis->think = KracGrenadeExplode;
		newmis->skin = 1; //WK Conc skin
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/hgren2_orig.mdl");
	}

	setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (newmis, user->origin);

	// OfN
	if (self->weapon == PR_GR_TYPE_FLARE)
	{
	  setorigin (newmis, user->origin + V({0, 0, 16}));
	}

	oldself = self;
	self = self->owner;

#ifdef PR_DEMO_STUFF
	if (live_camera)
		CamProjectileLockOn();
#endif

	self = oldself;

	// Remove primed grenade object
	dremove(self);
}

//=========================================================================
// Throws a currently primed grenade
void TeamFortress_ThrowGrenade()
{
	// If no grenade is primed, return
	if (!(self->tfstate & PR_TFSTATE_GRENPRIMED))
		return;

	//WK Tossing a gren reveals yourself (so people don't spam)
	//if (self.job & #JOB_THIEF && (self.job & #JOB_ACTIVE || self.job & #JOB_FULL_HIDE))
	if (self->job & PR_JOB_THIEF && self->job & PR_JOB_FULL_HIDE)
		RevealThief(self,PR_FALSE);

	self->tfstate = self->tfstate | PR_TFSTATE_GRENTHROWING;
}

//=========================================================================
// PLAYER CLASS HANDLING FUNCTIONS
//=========================================================================
// Return #TRUE is this class is allowed on this map
float IsLegalClass(float pc)
{
	int bit;

	// Spy may be turned off
	if (spy_off == PR_TRUE && pc == PR_PC_SPY)
		return PR_FALSE;

	if (pc == PR_PC_SCOUT)
		bit = PR_TF_ILL_SCOUT;
	else if (pc == PR_PC_SNIPER)
		bit = PR_TF_ILL_SNIPER;
	else if (pc == PR_PC_SOLDIER)
		bit = PR_TF_ILL_SOLDIER;
	else if (pc == PR_PC_DEMOMAN)
		bit = PR_TF_ILL_DEMOMAN;
	else if (pc == PR_PC_MEDIC)
		bit = PR_TF_ILL_MEDIC;
	else if (pc == PR_PC_HVYWEAP)
		bit = PR_TF_ILL_HVYWEP;
	else if (pc == PR_PC_PYRO)
		bit = PR_TF_ILL_PYRO;
	else if (pc == PR_PC_SPY)
		bit = PR_TF_ILL_SPY;
	else if (pc == PR_PC_ENGINEER)
		bit = PR_TF_ILL_ENGINEER;
//	else if (pc == #PC_RANDOM)
	else
//WK	else if (pc == #PC_RANDOM || pc == #PC_CUSTOM) //WK Disabling randompc disables //WK customclass
		bit = PR_TF_ILL_RANDOMPC;

	if ((illegalclasses & bit) || (TeamFortress_TeamGetIllegalClasses(self->team_no) & bit))
		return PR_FALSE;

	return PR_TRUE;
}

//=========================================================================
// Alter the player's Movement based on class
void TeamFortress_SetSpeed(entity p)
{
	//local string sp;
	float tf;
	entity te;

	//stuffcmd(p,"cl_movespeedkey 1\n");

	makeImmune(self,time + 1.1); //WK Don't boot someone decelerating

	// - OfN -//
	//-------------------------------------------------------------------------//
	// STOP THAT SILLY DAMN BUGS, SPY SLIDING ETC.. !!!!!!!!!!!!!!!!!!!!!!!!! -//
	if (p->is_feigning || p->is_haxxxoring || p->is_detpacking || p->is_building)
		p->tfstate = p->tfstate | PR_TFSTATE_CANT_MOVE;
	//-------------------------------------------------------------------------//

	// Check for any reason why they can't move at all
	if (p->tfstate & PR_TFSTATE_CANT_MOVE)
	{
		if (PR_STOP_MOUSE_MOVEMENT == PR_ON)
		{
			stuffcmd(p,"m_forward 0\n");
			stuffcmd(p,"m_side 0\n");
		}

		p->velocity = V({0, 0, 0});
		stuffcmd(p,"cl_backspeed 0\n");
		stuffcmd(p,"cl_forwardspeed 0\n");
		stuffcmd(p,"cl_sidespeed 0\n");
		p->maxspeed = 0;
		return;
	}
	else
	{
		if (PR_STOP_MOUSE_MOVEMENT == PR_ON)
		{
			stuffcmd(p,"m_forward 1\n");
			stuffcmd(p,"m_side 0.8\n");
		}
	}

	// First, get their max class speed
	// WK We don't use this block much any more
	if ( p->playerclass == PR_PC_SCOUT )
	{p->maxspeed = PR_PC_SCOUT_MAXSPEED;}
	else if ( p->playerclass == PR_PC_SNIPER )
	{p->maxspeed = PR_PC_SNIPER_MAXSPEED;}
	else if ( p->playerclass == PR_PC_SOLDIER )
	{p->maxspeed = PR_PC_SOLDIER_MAXSPEED;}
	else if ( p->playerclass == PR_PC_DEMOMAN )
	{p->maxspeed = PR_PC_DEMOMAN_MAXSPEED;}
	else if ( p->playerclass == PR_PC_MEDIC )
	{p->maxspeed = PR_PC_MEDIC_MAXSPEED;}
	else if ( p->playerclass == PR_PC_HVYWEAP )
	{p->maxspeed = PR_PC_HVYWEAP_MAXSPEED;}
	else if ( p->playerclass == PR_PC_PYRO )
	{p->maxspeed = PR_PC_PYRO_MAXSPEED;}
	else if ( p->playerclass == PR_PC_CIVILIAN )
	{p->maxspeed = PR_PC_CIVILIAN_MAXSPEED;}
	else if ( p->playerclass == PR_PC_SPY )
	{p->maxspeed = PR_PC_SPY_MAXSPEED;}
	else if ( p->playerclass == PR_PC_ENGINEER )
	{p->maxspeed = PR_PC_ENGINEER_MAXSPEED;}
	else if ( p->playerclass == PR_PC_UNDEFINED )
	{p->maxspeed = 320;return;}

	else if ( p->playerclass == PR_PC_CUSTOM )
	{
		// PZ: Added the condition ONLY. (8-22-15) We need to hold on to the previous .maxspeed. Don't clobber it before it gets saved into .worldtype.
		if (p->done_custom & PR_CUSTOM_FINISHED)
			p->maxspeed = p->custom_speed;
		if (p->done_custom & PR_CUSTOM_BUILDING) //We are building a class
			p->maxspeed = 0;
	}

	//1st if we have scuba gear and are underwater, increase speed
	if ((p->tf_items & PR_NIT_SCUBA) && (p->flags & PR_FL_INWATER))
		p->maxspeed = p->maxspeed + 200;
	//Fins slow us down out of water
	if ((p->tf_items & PR_NIT_SCUBA) && !(p->flags & PR_FL_INWATER))
		p->maxspeed = p->maxspeed - 10; //WK 1-7-7 20

	// 2nd, see if any GoalItems are slowing them down
	tf = 0;
	te = find (world, "classname", "item_tfgoal");
	while ((te != world) && (tf == 0))
	{
		if (te->owner == p)
		{
			if (te->goal_activation & PR_TFGI_SLOW)
			{
				tf = 1;
				p->maxspeed = p->maxspeed / 2;
			}
		}
		te = find(te, "classname", "item_tfgoal");
	}

	// 3rd, See if they're tranquilised
	if (p->tfstate & PR_TFSTATE_TRANQUILISED)
	{
		//WK p.maxspeed = (p.maxspeed / 3) * 2;
		if (p->maxspeed > 170)
			p->maxspeed = 170;
	}

	if (p->tfstate & PR_TFSTATE_C4THROW)
		p->maxspeed = p->maxspeed / 3;

	// 4th, check for leg wounds
	if (p->leg_damage)
	{
		if (p->leg_damage > 9)
			p->leg_damage = 9;

		// reduce speed by 10% per leg wound
		p->maxspeed = (p->maxspeed * ((10 - p->leg_damage) / 10));
	}

	// 5th, if they're a sniper, and they're aiming, their speed must be 60 or less
	//WK Changed to allow aiming at half base leg speed, it's more fun that way.
	if (p->tfstate & PR_TFSTATE_AIMING)
	{
		// Gizmo - changed this slower to balance out higher sniper damage
		if (p->maxspeed > 60)
			p->maxspeed = p->maxspeed / 3;
	}

	// WK 6th, See if they're partially hidden
	if (p->job & PR_JOB_THIEF)
	{
		if (p->job & PR_JOB_ACTIVE)
			p->maxspeed = (p->maxspeed * 3) / 4;
		else if (p->job & PR_JOB_FULL_HIDE)
			if (p->maxspeed > 130)
				p->maxspeed = 130;
	}

	//WK 7th, Check to see if they are sprinting or recovering
	//Note that tired is both ACTIVE and TIRED
	if (p->job & PR_JOB_RUNNER) {
		if (p->job & PR_JOB_TIRED)
			p->maxspeed = 2 * p->maxspeed / 3;
		else if (p->job & PR_JOB_ACTIVE)
			p->maxspeed = p->maxspeed + 200;
	}

	// OfN - Speed aura miltiplies our final speed
	if (p->aura == PR_AURA_SPEED)
		p->maxspeed = p->maxspeed * PR_SPEEDAURA_FACTOR;

	// OfN - Speed rune muliplies our speed too
	if (p->PR_runes & PR_RUNE_SPEED)
		p->maxspeed = p->maxspeed * PR_SPEEDRUNE_FACTOR;

	// OfN - Finally, the psionic stuff
	if (p->tfstate & PR_TFSTATE_PSIONIZED)
		p->maxspeed = p->maxspeed * PR_PSIONIC_MOVEFACTOR;

	// WK See if they are ass cannoning WK 2/26/7 -- Slowed down
	// WK 2/8/7 We allow ass cannons to move now, slowly
	if (p->PR_BUTTON_FIRE && p->current_weapon == PR_WEAP_ASSAULT_CANNON)
	{
		p->maxspeed = p->maxspeed / 3;
	}



	#ifdef PR_QUAKE_WORLD
		stuffcmd(p,"cl_backspeed 1000\n");
		stuffcmd(p,"cl_forwardspeed 1000\n");
		stuffcmd(p,"cl_sidespeed 1000\n");
	#else
		sp = ftos(p->maxspeed);
		stuffcmd(p,"cl_backspeed ");
		stuffcmd(p,sp);
		stuffcmd(p,"\n");
		stuffcmd(p,"cl_forwardspeed ");
		stuffcmd(p,sp);
		stuffcmd(p,"\n");
		sp = ftos(p->maxspeed);
		stuffcmd(p,"cl_sidespeed ");
		stuffcmd(p,sp);
		stuffcmd(p,"\n");
	#endif
}

//=========================================================================
// Set the max_health of a player based on his/her class
void TeamFortress_SetHealth()
{
	if ( self->playerclass == PR_PC_SCOUT )
		self->max_health = PR_PC_SCOUT_MAXHEALTH;
	else if ( self->playerclass == PR_PC_SNIPER )
		self->max_health = PR_PC_SNIPER_MAXHEALTH;
	else if ( self->playerclass == PR_PC_SOLDIER )
		self->max_health = PR_PC_SOLDIER_MAXHEALTH;
	else if ( self->playerclass == PR_PC_DEMOMAN )
		self->max_health = PR_PC_DEMOMAN_MAXHEALTH;
	else if ( self->playerclass == PR_PC_MEDIC )
		self->max_health = PR_PC_MEDIC_MAXHEALTH;
	else if ( self->playerclass == PR_PC_HVYWEAP )
		self->max_health = PR_PC_HVYWEAP_MAXHEALTH;
	else if ( self->playerclass == PR_PC_PYRO )
		self->max_health = PR_PC_PYRO_MAXHEALTH;
	else if ( self->playerclass == PR_PC_CIVILIAN )
		self->max_health = PR_PC_CIVILIAN_MAXHEALTH;
	else if ( self->playerclass == PR_PC_SPY )
		self->max_health = PR_PC_SPY_MAXHEALTH;
	else if ( self->playerclass == PR_PC_ENGINEER )
		self->max_health = PR_PC_ENGINEER_MAXHEALTH;
	else if ( self->playerclass == PR_PC_UNDEFINED )
	{
		self->max_health = 1;
		self->takedamage = 0; // Prevent damage to #PC_UNDEFINED players
	}

	self->health = self->max_health;
}
//==================================================
//CH returns value for skin.  used here and tfortmap.qc
float Return_Custom_Skins(entity p)
{
	if (p->weapons_carried & PR_WEAP_SNIPER_RIFLE)
		return PR_PC_SNIPER;
	else if (p->weapons_carried & PR_WEAP_ASSAULT_CANNON)
		return PR_PC_HVYWEAP;
	else if (p->weapons_carried & PR_WEAP_ROCKET_LAUNCHER)
		return PR_PC_SOLDIER;
	else if (p->weapons_carried & PR_WEAP_FLAMETHROWER)
		return PR_PC_PYRO;
	else if (p->weapons_carried & PR_WEAP_GRENADE_LAUNCHER)
		return PR_PC_DEMOMAN;
	else if (p->weapons_carried & PR_WEAP_MEDIKIT)
		return PR_PC_MEDIC;
	else if (p->weapons_carried & PR_WEAP_SPANNER)
		return PR_PC_ENGINEER;
	else if (p->weapons_carried & PR_WEAP_NAILGUN)
		return PR_PC_SCOUT;
	else if (p->job & PR_JOB_GUERILLA)
		return PR_PC_DEMOMAN;
	else if (p->custom_speed > 320 || p->job & PR_JOB_RUNNER)
		return PR_PC_SCOUT;
	else if (p->cutf_items & PR_CUTF_SPY_KIT)
		return PR_PC_SPY;
	else if (p->cutf_items & PR_CUTF_SENTRYGUN || p->tf_items & PR_NIT_TESLA)
		return PR_PC_ENGINEER;
	else
		return PR_PC_SOLDIER;
}
//=========================================================================
// Set the skin of a player based on his/her class, if Classkin is on
void TeamFortress_SetSkin(entity p)
{
	//local string st;

	makeImmune(self,time + 4);
	//self.immune_to_check = time + 4;


	if ((p->cutf_items & PR_CUTF_SPY_KIT) && p->undercover_skin != 0)
		p->skin = p->undercover_skin;
	else
		p->skin = p->playerclass;

#ifdef PR_QUAKE_WORLD
	if (p->skin != PR_PC_UNDEFINED)
	{
		// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
		//     themselves, to keep from having to kick them when they do.
		//stuffcmd(p, "skin ");

		//WK Figure out what Mr.Custom Should look like
		if (p->playerclass == PR_PC_CUSTOM && p->undercover_skin == 0)
		{
			p->skin = Return_Custom_Skins(p); //CH
		}

		if ( p->team_no == 4)
		{
			if ( p->skin == PR_PC_SCOUT )
				//stuffcmd(p, #TEAM4_SCOUT_SKIN);
				setinfo(p, "skin", PR_TEAM4_SCOUT_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_SNIPER )
				//stuffcmd(p, #TEAM4_SNIPER_SKIN);
				setinfo(p, "skin", PR_TEAM4_SNIPER_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_SOLDIER )
				//stuffcmd(p, #TEAM4_SOLDIER_SKIN);
				setinfo(p, "skin", PR_TEAM4_SOLDIER_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_DEMOMAN )
				//stuffcmd(p, #TEAM4_DEMOMAN_SKIN);
				setinfo(p, "skin", PR_TEAM4_DEMOMAN_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_MEDIC )
				//stuffcmd(p, #TEAM4_MEDIC_SKIN);
				setinfo(p, "skin", PR_TEAM4_MEDIC_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_HVYWEAP )
				//stuffcmd(p, #TEAM4_HVYWEAP_SKIN);
				setinfo(p, "skin", PR_TEAM4_HVYWEAP_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_PYRO )
				//stuffcmd(p, #TEAM4_PYRO_SKIN);
				setinfo(p, "skin", PR_TEAM4_PYRO_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_SPY )
				//stuffcmd(p, #TEAM4_SPY_SKIN);
				setinfo(p, "skin", PR_TEAM4_SPY_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_ENGINEER )
				//stuffcmd(p, #TEAM4_ENGINEER_SKIN);
				setinfo(p, "skin", PR_TEAM4_ENGINEER_SKIN); // PZ: see above
		}
		else if ( p->team_no == 3)
		{
			if ( p->skin == PR_PC_SCOUT )
				//stuffcmd(p, #TEAM3_SCOUT_SKIN);
				setinfo(p, "skin", PR_TEAM3_SCOUT_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_SNIPER )
				//stuffcmd(p, #TEAM3_SNIPER_SKIN);
				setinfo(p, "skin", PR_TEAM3_SNIPER_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_SOLDIER )
				//stuffcmd(p, #TEAM3_SOLDIER_SKIN);
				setinfo(p, "skin", PR_TEAM3_SOLDIER_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_DEMOMAN )
				//stuffcmd(p, #TEAM3_DEMOMAN_SKIN);
				setinfo(p, "skin", PR_TEAM3_DEMOMAN_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_MEDIC )
				//stuffcmd(p, #TEAM3_MEDIC_SKIN);
				setinfo(p, "skin", PR_TEAM3_MEDIC_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_HVYWEAP )
				//stuffcmd(p, #TEAM3_HVYWEAP_SKIN);
				setinfo(p, "skin", PR_TEAM3_HVYWEAP_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_PYRO )
				//stuffcmd(p, #TEAM3_PYRO_SKIN);
				setinfo(p, "skin", PR_TEAM3_PYRO_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_SPY )
				//stuffcmd(p, #TEAM3_SPY_SKIN);
				setinfo(p, "skin", PR_TEAM3_SPY_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_ENGINEER )
				//stuffcmd(p, #TEAM3_ENGINEER_SKIN);
				setinfo(p, "skin", PR_TEAM3_ENGINEER_SKIN); // PZ: see above
		}
		else if ( p->team_no == 2)
		{
			if ( p->skin == PR_PC_SCOUT )
				//stuffcmd(p, #TEAM2_SCOUT_SKIN);
				setinfo(p, "skin", PR_TEAM2_SCOUT_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_SNIPER )
				//stuffcmd(p, #TEAM2_SNIPER_SKIN);
				setinfo(p, "skin", PR_TEAM2_SNIPER_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_SOLDIER )
				//stuffcmd(p, #TEAM2_SOLDIER_SKIN);
				setinfo(p, "skin", PR_TEAM2_SOLDIER_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_DEMOMAN )
				//stuffcmd(p, #TEAM2_DEMOMAN_SKIN);
				setinfo(p, "skin", PR_TEAM2_DEMOMAN_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_MEDIC )
				//stuffcmd(p, #TEAM2_MEDIC_SKIN);
				setinfo(p, "skin", PR_TEAM2_MEDIC_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_HVYWEAP )
				//stuffcmd(p, #TEAM2_HVYWEAP_SKIN);
				setinfo(p, "skin", PR_TEAM2_HVYWEAP_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_PYRO )
				//stuffcmd(p, #TEAM2_PYRO_SKIN);
				setinfo(p, "skin", PR_TEAM2_PYRO_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_SPY )
				//stuffcmd(p, #TEAM2_SPY_SKIN);
				setinfo(p, "skin", PR_TEAM2_SPY_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_ENGINEER )
				//stuffcmd(p, #TEAM2_ENGINEER_SKIN);
				setinfo(p, "skin", PR_TEAM2_ENGINEER_SKIN); // PZ: see above
		}
		else // if ( p.team_no == 1)
		{
			if ( p->skin == PR_PC_SCOUT )
				//stuffcmd(p, #TEAM1_SCOUT_SKIN);
				setinfo(p, "skin", PR_TEAM1_SCOUT_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_SNIPER )
				//stuffcmd(p, #TEAM1_SNIPER_SKIN);
				setinfo(p, "skin", PR_TEAM1_SNIPER_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_SOLDIER )
				//stuffcmd(p, #TEAM1_SOLDIER_SKIN);
				setinfo(p, "skin", PR_TEAM1_SOLDIER_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_DEMOMAN )
				//stuffcmd(p, #TEAM1_DEMOMAN_SKIN);
				setinfo(p, "skin", PR_TEAM1_DEMOMAN_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_MEDIC )
				//stuffcmd(p, #TEAM1_MEDIC_SKIN);
				setinfo(p, "skin", PR_TEAM1_MEDIC_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_HVYWEAP )
				//stuffcmd(p, #TEAM1_HVYWEAP_SKIN);
				setinfo(p, "skin", PR_TEAM1_HVYWEAP_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_PYRO )
				//stuffcmd(p, #TEAM1_PYRO_SKIN);
				setinfo(p, "skin", PR_TEAM1_PYRO_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_SPY )
				//stuffcmd(p, #TEAM1_SPY_SKIN);
				setinfo(p, "skin", PR_TEAM1_SPY_SKIN); // PZ: see above
			else if ( p->skin == PR_PC_ENGINEER )
				//stuffcmd(p, #TEAM1_ENGINEER_SKIN);
				setinfo(p, "skin", PR_TEAM1_ENGINEER_SKIN); // PZ: see above
		}

		if ( p->skin == PR_PC_CIVILIAN )
			//stuffcmd(p, "base\n"); // Need a civilian skin
			setinfo(p, "skin", "base"); // PZ: see above

	}
	else
	{
		//stuffcmd(p, "skin base\n"); //WK Need a custom skin, or fake it
		setinfo(p, "skin", "base"); // PZ: see above
	}
#endif
}

//=========================================================================
// Set the details of a player based on his/her class
void TeamFortress_SetEquipment()
{
	entity te;
	entity automan;
	string st;
	int kept_items;

	if (self->classname != "player")
		return;

#ifdef PR_COOP_MODE_ENHANCED
	if ( !deathmatch )
		kept_items = 0;
	else
		kept_items = self->tf_items & (PR_IT_KEY1 | PR_IT_KEY2);
#else
	kept_items = self->tf_items & (PR_IT_KEY1 | PR_IT_KEY2);
#endif

	if (self->playerclass != PR_PC_CUSTOM) { //WK -----v
		self->items = 0;
		self->current_weapon = 0;
		self->weapons_carried = 0;

		self->armorclass = 0;

		self->ammo_medikit = 0;
		self->maxammo_medikit = 0;
		self->ammo_detpack = 0;
		self->ammo_c4det = 0;
		self->maxammo_detpack = 0;
		self->items_allowed = 0;
		self->armor_allowed = 0;
		self->gravity = 1;
		self->maxarmor = 0;
		self->weaponmode = 0;
		self->tf_items = 0;
		self->tf_items_flags = 0;
		self->cutf_items = 0;
		self->done_custom = 0;

		//Clear job without losing track of demons, etc.
		self->job = self->job - (self->job & (PR_JOB_ALL|PR_JOB_EXTRA1|PR_JOB_EXTRA2));
	}
 //WK ------^

	self->hover_time = 0;
	if (self->tf_items & PR_NIT_HOVER_BOOTS)
		self->hover_time = PR_MAX_HOVER_FUEL; //10 secs of hovering or so
	self->respawn_time = 0;
	self->heat = 0;
	self->tfstate = self->tfstate - (self->tfstate & PR_TFSTATE_RELOADING);
	if (self->tf_items & PR_NIT_SCUBA) //WK We're a scuba commando!
	{
		self->items = self->items | PR_IT_SUIT;
		self->rad_time = 1;
		self->tfstate = self->tfstate | PR_TFSTATE_RADSUIT;
		self->radsuit_finished = time + 666;
	}
	else
	{
		self->tfstate = self->tfstate - (self->tfstate & PR_TFSTATE_RADSUIT);
		self->items = 0;
		self->rad_time = 0;
		self->radsuit_finished = 0;
	}
	self->undercover_skin = 0;
	if (self->undercover_team != 0)
	{
		makeImmune(self,time + 4);
		//self.immune_to_check = time + 4;

		self->undercover_team = 0;
		// Set their color
		// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
		//     themselves, to keep from having to kick them when they do.
		//stuffcmd(self, "color ");
		st = ftos(TeamFortress_TeamGetColor(self->team_no) - 1);
		 //- OfN - Nice colors
		if (nicecolors==1) st =TeamGetNiceColor(self->team_no);
		setinfo(self, "topcolor", st);    // PZ
		setinfo(self, "bottomcolor", st); // PZ
		//stuffcmd(self, st);
		//stuffcmd(self, "\n");
	}

	self->is_building = 0;
	self->is_toffingadet = 0;
	self->is_haxxxoring = 0;
	self->is_detpacking = 0;
	self->is_undercover = 0;
	self->is_feigning = 0;
	self->is_unabletospy = 0;
	self->is_malfunctioning = 0;
	self->is_abouttodie = 0; // reset preventing automartyr flag

	self->martyr_enemy = self; // reset martyr enemy
	self->stored_deathmsg = 0; // reset stored death message - OfN UNUSED?

	self->impulse = 0;

	if (self->team_no == 0)
		self->lives = -1;

	self->items = self->items | kept_items;

	// Start the Cheat-Checking Cyclic Event if CheatChecking Toggleflag is on
	if (!stof(infokey(world, "nospeed")) && (toggleflags & PR_TFLAG_CHEATCHECK))
	{
		te = spawnServerSide(); // PZ: make it a server-side only entity    //Cyto
		te->nextthink = time + 2;
		te->think = TeamFortress_CheckForSpeed;
		te->owner = self;
		te->classname = "timer";
	}

	//WK -- Remove flags for job state
	self->job = self->job - (self->job & PR_JOB_ACTIVE);
	self->job = self->job - (self->job & PR_JOB_FULL_HIDE);
	self->job = self->job - (self->job & PR_JOB_TIRED);
	//self.job = self.job - (self.job & #JOB_MARTYR_ENEMY);
	self->job_finished = time;

	if ( self->playerclass == PR_PC_CUSTOM ) //WK - Reset the custom class
	{
		/*self.no_grenades_1 = GetMaxGrens(self,1);
		self.no_grenades_2 = GetMaxGrens(self,2);*/ //done at the end of this func

#ifdef PR_COOP_MODE_ENHANCED
		if ( self->tf_items & PR_NIT_AMMO_BANDOLIER || !deathmatch ) {
			self->ammo_rockets = self->maxammo_rockets / 2;
			self->ammo_nails = (3 * self->maxammo_nails) / 4;
			self->ammo_shells = (3 * self->maxammo_shells) / 4;
			self->ammo_cells = (2 * self->maxammo_cells) / 3;
		} else {
			self->ammo_rockets = self->maxammo_rockets / 4;
			self->ammo_nails = self->maxammo_nails / 2;
			self->ammo_shells = self->maxammo_shells / 2;
			self->ammo_cells = self->maxammo_cells / 2;
		}
#else
		if ( self->tf_items & PR_NIT_AMMO_BANDOLIER ) {
			self->ammo_rockets = self->maxammo_rockets / 2;
			self->ammo_nails = (3 * self->maxammo_nails) / 4;
			self->ammo_shells = (3 * self->maxammo_shells) / 4;
			self->ammo_cells = (2 * self->maxammo_cells) / 3;
		} else {
			self->ammo_rockets = self->maxammo_rockets / 4;
			self->ammo_nails = self->maxammo_nails / 2;
			self->ammo_shells = self->maxammo_shells / 2;
			self->ammo_cells = self->maxammo_cells / 2;
		}
#endif

		self->ammo_detpack = self->maxammo_detpack;
		self->ammo_c4det = self->maxammo_detpack;
		self->ammo_medikit = self->maxammo_medikit;
		if (self->ammo_medikit > 0 && self->weapons_carried & PR_WEAP_MEDIKIT) {
			te = spawnServerSide(); // PZ: make it a server-side only entity
			te->nextthink = time + PR_PC_MEDIC_REGEN_TIME;
			te->think = TeamFortress_Regenerate;
			te->owner = self;
			te->classname = "timer";
		}
		if (self->cutf_items & PR_CUTF_CYBERAUG)
		{
			te = spawnServerSide(); // PZ: make it a server-side only entity
			te->nextthink = time + PR_PC_MEDIC_REGEN_TIME;
			te->think = TeamFortress_RegenerateCyber;
			te->owner = self;
			te->classname = "timer";
		}

		// SB only need to activate this with skill
		/*if (self.job & #JOB_THIEF)
		{
			te = spawn();
			te.nextthink = time + #PC_SPY_CELL_REGEN_TIME;
			te.think = TeamFortress_RegenerateCells;
			te.owner = self;
			te.classname = "timer";
		} */

#ifdef PR_COOP_MODE_ENHANCED
		// Gizmo - changed this for coop
		// TODO: make free instead of this hack
		if ( deathmatch ) {
			if (self->cutf_items & PR_CUTF_FULLARMOUR)
				self->armorvalue = self->maxarmor;
			else
				self->armorvalue = self->maxarmor / 2;
		} else
			self->armorvalue = self->maxarmor;
#else
		if (self->cutf_items & PR_CUTF_FULLARMOUR)
			self->armorvalue = self->maxarmor;
		else
			self->armorvalue = self->maxarmor / 2;
#endif
		self->armortype = self->armor_allowed; //Start with red if red's allowed
		self->armorclass = 0; //Default to no special armor type
		if (self->tf_items & PR_NIT_KEVLAR)
			self->armorclass = self->armorclass | PR_AT_SAVESHOT;
		if (self->tf_items & PR_NIT_GEL)
			self->armorclass = self->armorclass | PR_AT_SAVEMELEE;
		if (self->tf_items & PR_NIT_BLAST)
			self->armorclass = self->armorclass | PR_AT_SAVEEXPLOSION;
		if (self->tf_items & PR_NIT_CERAMIC)
			self->armorclass = self->armorclass | PR_AT_SAVEELECTRICITY;
		if (self->tf_items & PR_NIT_ASBESTOS) //Buying flamer gives this
			self->armorclass = self->armorclass | PR_AT_SAVEFIRE;

		//Support for the "Auto" class itemry
		if (self->tf_items & PR_NIT_AUTOSCANNER || self->weapons_carried & PR_WEAP_MEDIKIT || self->tf_items & PR_NIT_AUTOID
			|| self->cutf_moreitems & PR_CUTF_MI_FRIENDORFOE) {
			automan = spawnServerSide(); // PZ: make it a server-side only entity
			automan->classname = "timer";
			automan->nextthink = time + 2;
			automan->think = Autoitem_think;
			automan->owner = self;
			automan->enemy = self;
			automan->oldenemy = self;	// Gizmo - this is so the crosshair will change color initially
		}

		self->current_weapon = W_BestWeapon ();
		//#ifdef CUSTOM_PC_ITEMS_PLAYING
		UpdateWeaponItems();
		//#endif
	}
	else if ( self->playerclass == PR_PC_SCOUT )
	{
		self->weapons_carried = self->weapons_carried | PR_PC_SCOUT_WEAPONS;
		self->ammo_rockets = PR_PC_SCOUT_INITAMMO_ROCKET;
		self->ammo_nails = PR_PC_SCOUT_INITAMMO_NAIL;
		self->ammo_shells = PR_PC_SCOUT_INITAMMO_SHOT;
		self->ammo_cells = PR_PC_SCOUT_INITAMMO_CELL;
		self->maxammo_rockets = PR_PC_SCOUT_MAXAMMO_ROCKET;
		self->maxammo_nails = PR_PC_SCOUT_MAXAMMO_NAIL;
		self->maxammo_shells = PR_PC_SCOUT_MAXAMMO_SHOT;
		self->maxammo_cells = PR_PC_SCOUT_MAXAMMO_CELL;

		self->no_grenades_1 = PR_PC_SCOUT_GRENADE_INIT_1;
		self->no_grenades_2 = PR_PC_SCOUT_GRENADE_INIT_2;
		self->tp_grenades_1 = PR_PC_SCOUT_GRENADE_TYPE_1;
		self->tp_grenades_2 = PR_PC_SCOUT_GRENADE_TYPE_2;
		self->tf_items = PR_PC_SCOUT_TF_ITEMS;
		self->cutf_items = PR_PC_SCOUT_CUTF_ITEMS;
		self->job = self->job | PR_PC_SCOUT_JOB;

		// the scanner defaults to enemy scanning ON, friendly scanning OFF
		// and movement scanner only OFF
		self->tf_items_flags = self->tf_items_flags | PR_NIT_SCANNER_ENEMY;

		self->armorclass = self->armorclass | PR_PC_SCOUT_INITARMORCLASS;
		self->armortype = PR_PC_SCOUT_INITARMORTYPE;
		self->armorvalue = PR_PC_SCOUT_INITARMOR;
		self->armor_allowed = PR_PC_SCOUT_MAXARMORTYPE;
		self->maxarmor = PR_PC_SCOUT_MAXARMOR;
		self->current_weapon = PR_WEAP_NAILGUN;

		self->items_allowed = PR_PC_SCOUT_WEAPONS;
		//kill_my_demons();

		// set the weapon icons on the status bar
		self->items = self->items | PR_IT_SHOTGUN | PR_IT_NAILGUN;
	}
	else if ( self->playerclass == PR_PC_SNIPER )
	{
		self->weapons_carried = self->weapons_carried | PR_PC_SNIPER_WEAPONS;
		self->ammo_rockets = PR_PC_SNIPER_INITAMMO_ROCKET;
		self->ammo_nails = PR_PC_SNIPER_INITAMMO_NAIL;
		self->ammo_shells = PR_PC_SNIPER_INITAMMO_SHOT;
		self->ammo_cells = PR_PC_SNIPER_INITAMMO_CELL;
		self->maxammo_rockets = PR_PC_SNIPER_MAXAMMO_ROCKET;
		self->maxammo_nails = PR_PC_SNIPER_MAXAMMO_NAIL;
		self->maxammo_shells = PR_PC_SNIPER_MAXAMMO_SHOT;
		self->maxammo_cells = PR_PC_SNIPER_MAXAMMO_CELL;

		self->no_grenades_1 = PR_PC_SNIPER_GRENADE_INIT_1;
		self->no_grenades_2 = PR_PC_SNIPER_GRENADE_INIT_2;
		self->tp_grenades_1 = PR_PC_SNIPER_GRENADE_TYPE_1;
		self->tp_grenades_2 = PR_PC_SNIPER_GRENADE_TYPE_2;
		self->tf_items = PR_PC_SNIPER_TF_ITEMS;
		self->job = self->job | PR_PC_SNIPER_JOB;

		self->armorclass = self->armorclass | PR_PC_SNIPER_INITARMORCLASS;
		self->armortype = PR_PC_SNIPER_INITARMORTYPE;
		self->armorvalue = PR_PC_SNIPER_INITARMOR;
		self->armor_allowed = PR_PC_SNIPER_MAXARMORTYPE;
		self->maxarmor = PR_PC_SNIPER_MAXARMOR;
		self->current_weapon = PR_WEAP_SNIPER_RIFLE;

		self->items_allowed = PR_PC_SNIPER_WEAPONS;
		self->cutf_items = PR_PC_SNIPER_CUTF_ITEMS; // OFN -OTR

		// set the weapon icons on the status bar
		self->items = self->items | PR_IT_SHOTGUN | PR_IT_SUPER_SHOTGUN | PR_IT_NAILGUN;
		//kill_my_demons();
	}
	else if ( self->playerclass == PR_PC_SOLDIER )
	{
		self->weapons_carried = self->weapons_carried | PR_PC_SOLDIER_WEAPONS;
		self->ammo_rockets = PR_PC_SOLDIER_INITAMMO_ROCKET;
		self->ammo_nails = PR_PC_SOLDIER_INITAMMO_NAIL;
		self->ammo_shells = PR_PC_SOLDIER_INITAMMO_SHOT;
		self->ammo_cells = PR_PC_SOLDIER_INITAMMO_CELL;
		self->maxammo_rockets = PR_PC_SOLDIER_MAXAMMO_ROCKET;
		self->maxammo_nails = PR_PC_SOLDIER_MAXAMMO_NAIL;
		self->maxammo_shells = PR_PC_SOLDIER_MAXAMMO_SHOT;
		self->maxammo_cells = PR_PC_SOLDIER_MAXAMMO_CELL;

		self->no_grenades_1 = PR_PC_SOLDIER_GRENADE_INIT_1;
		self->no_grenades_2 = PR_PC_SOLDIER_GRENADE_INIT_2;
		self->tp_grenades_1 = PR_PC_SOLDIER_GRENADE_TYPE_1;
		self->tp_grenades_2 = PR_PC_SOLDIER_GRENADE_TYPE_2;
		self->tf_items = PR_PC_SOLDIER_TF_ITEMS;
		self->cutf_items = PR_PC_SOLDIER_CUTF_ITEMS;          // PZ: added this, in case we ever want to add CuTF items to stock soldier
		self->job = self->job | PR_PC_SOLDIER_JOB;

		self->armorclass = self->armorclass | PR_PC_SOLDIER_INITARMORCLASS;
		self->armortype = PR_PC_SOLDIER_INITARMORTYPE;
		self->armorvalue = PR_PC_SOLDIER_INITARMOR;
		self->armor_allowed = PR_PC_SOLDIER_MAXARMORTYPE;
		self->maxarmor = PR_PC_SOLDIER_MAXARMOR;
		self->current_weapon = PR_WEAP_ROCKET_LAUNCHER;

		self->items_allowed = PR_PC_SOLDIER_WEAPONS;

		// set the weapon icons on the status bar
		self->items = self->items | PR_IT_SHOTGUN | PR_IT_SUPER_SHOTGUN | PR_IT_ROCKET_LAUNCHER;
		//kill_my_demons();
	}
	else if ( self->playerclass == PR_PC_DEMOMAN )
	{
		self->weapons_carried = self->weapons_carried | PR_PC_DEMOMAN_WEAPONS;
		self->ammo_rockets = PR_PC_DEMOMAN_INITAMMO_ROCKET;
		self->ammo_nails = PR_PC_DEMOMAN_INITAMMO_NAIL;
		self->ammo_shells = PR_PC_DEMOMAN_INITAMMO_SHOT;
		self->ammo_cells = PR_PC_DEMOMAN_INITAMMO_CELL;
		self->maxammo_rockets = PR_PC_DEMOMAN_MAXAMMO_ROCKET;
		self->maxammo_nails = PR_PC_DEMOMAN_MAXAMMO_NAIL;
		self->maxammo_shells = PR_PC_DEMOMAN_MAXAMMO_SHOT;
		self->maxammo_cells = PR_PC_DEMOMAN_MAXAMMO_CELL;

		self->no_grenades_1 = PR_PC_DEMOMAN_GRENADE_INIT_1;
		self->no_grenades_2 = PR_PC_DEMOMAN_GRENADE_INIT_2;
		self->tp_grenades_1 = PR_PC_DEMOMAN_GRENADE_TYPE_1;
		self->tp_grenades_2 = PR_PC_DEMOMAN_GRENADE_TYPE_2;
		self->tf_items = PR_PC_DEMOMAN_TF_ITEMS;
		self->job = self->job | PR_PC_DEMOMAN_JOB;
		self->cutf_items = PR_PC_DEMOMAN_CUTF_ITEMS;

		// Detpacks
		self->ammo_detpack = PR_PC_DEMOMAN_INITAMMO_DETPACK;
		self->maxammo_detpack = PR_PC_DEMOMAN_MAXAMMO_DETPACK;

		self->armorclass = self->armorclass | PR_PC_DEMOMAN_INITARMORCLASS;
		self->armortype = PR_PC_DEMOMAN_INITARMORTYPE;
		self->armorvalue = PR_PC_DEMOMAN_INITARMOR;
		self->armor_allowed = PR_PC_DEMOMAN_MAXARMORTYPE;
		self->maxarmor = PR_PC_DEMOMAN_MAXARMOR;
		self->current_weapon = PR_WEAP_GRENADE_LAUNCHER;
		//kill_my_demons();

		self->ammo_c4det = 1; // - OfN

		self->items_allowed = PR_PC_DEMOMAN_WEAPONS;

		// set the weapon icons on the status bar
		self->items = self->items | PR_IT_SHOTGUN | PR_IT_GRENADE_LAUNCHER;
	}
	else if ( self->playerclass == PR_PC_MEDIC )
	{
		self->weapons_carried = self->weapons_carried | PR_PC_MEDIC_WEAPONS;
		self->ammo_rockets = PR_PC_MEDIC_INITAMMO_ROCKET;
		self->ammo_nails = PR_PC_MEDIC_INITAMMO_NAIL;
		self->ammo_shells = PR_PC_MEDIC_INITAMMO_SHOT;
		self->ammo_cells = PR_PC_MEDIC_INITAMMO_CELL;
		self->maxammo_rockets = PR_PC_MEDIC_MAXAMMO_ROCKET;
		self->maxammo_nails = PR_PC_MEDIC_MAXAMMO_NAIL;
		self->maxammo_shells = PR_PC_MEDIC_MAXAMMO_SHOT;
		self->maxammo_cells = PR_PC_MEDIC_MAXAMMO_CELL;

		self->no_grenades_1 = PR_PC_MEDIC_GRENADE_INIT_1;
		self->no_grenades_2 = PR_PC_MEDIC_GRENADE_INIT_2;
		self->tp_grenades_1 = PR_PC_MEDIC_GRENADE_TYPE_1;
		self->tp_grenades_2 = PR_PC_MEDIC_GRENADE_TYPE_2;
		self->tf_items = PR_PC_MEDIC_TF_ITEMS;
		self->cutf_items = PR_PC_MEDIC_CUTF_ITEMS;          // PZ: added this, in case we ever want to add CuTF items to stock medic
		self->job = self->job | PR_PC_MEDIC_JOB;

		self->armorclass = self->armorclass | PR_PC_MEDIC_INITARMORCLASS;
		self->armortype = PR_PC_MEDIC_INITARMORTYPE;
		self->armorvalue = PR_PC_MEDIC_INITARMOR;
		self->armor_allowed = PR_PC_MEDIC_MAXARMORTYPE;
		self->maxarmor = PR_PC_MEDIC_MAXARMOR;
		self->current_weapon = PR_WEAP_LIGHT_ASSAULT;

		self->ammo_medikit = PR_PC_MEDIC_INITAMMO_MEDIKIT;
		self->maxammo_medikit = PR_PC_MEDIC_MAXAMMO_MEDIKIT;

		// Start the Regeneration Cyclic Event
		te = spawnServerSide(); // PZ: make it a server-side only entity
		te->nextthink = time + PR_PC_MEDIC_REGEN_TIME;
		te->think = TeamFortress_Regenerate;
		te->owner = self;
		te->classname = "timer";

		self->items_allowed = PR_PC_MEDIC_WEAPONS;

		// set the weapon icons on the status bar
		self->items = self->items | PR_IT_SHOTGUN | PR_IT_SUPER_SHOTGUN | PR_IT_LIGHT_ASSAULT;
		//kill_my_demons();
	}
	else if ( self->playerclass == PR_PC_HVYWEAP )
	{
		self->weapons_carried = self->weapons_carried | PR_PC_HVYWEAP_WEAPONS;
		self->ammo_rockets = PR_PC_HVYWEAP_INITAMMO_ROCKET;
		self->ammo_nails = PR_PC_HVYWEAP_INITAMMO_NAIL;
		self->ammo_shells = PR_PC_HVYWEAP_INITAMMO_SHOT;
		self->ammo_cells = PR_PC_HVYWEAP_INITAMMO_CELL;
		self->maxammo_rockets = PR_PC_HVYWEAP_MAXAMMO_ROCKET;
		self->maxammo_nails = PR_PC_HVYWEAP_MAXAMMO_NAIL;
		self->maxammo_shells = PR_PC_HVYWEAP_MAXAMMO_SHOT;
		self->maxammo_cells = PR_PC_HVYWEAP_MAXAMMO_CELL;

		self->no_grenades_1 = PR_PC_HVYWEAP_GRENADE_INIT_1;
		self->no_grenades_2 = PR_PC_HVYWEAP_GRENADE_INIT_2;
		self->tp_grenades_1 = PR_PC_HVYWEAP_GRENADE_TYPE_1;
		self->tp_grenades_2 = PR_PC_HVYWEAP_GRENADE_TYPE_2;
		self->tf_items = PR_PC_HVYWEAP_TF_ITEMS;
		self->job = self->job | PR_PC_HVYWEAP_JOB;

		self->armorclass = self->armorclass | PR_PC_HVYWEAP_INITARMORCLASS;
		self->armortype = PR_PC_HVYWEAP_INITARMORTYPE;
		self->armorvalue = PR_PC_HVYWEAP_INITARMOR;
		self->armor_allowed = PR_PC_HVYWEAP_MAXARMORTYPE;
		self->maxarmor = PR_PC_HVYWEAP_MAXARMOR;
		self->current_weapon = PR_WEAP_SUPER_SHOTGUN;
		self->cutf_items = PR_PC_HVYWEAP_CUTF_ITEMS;     // PZ: Changed this, in case we ever want to add more CuTF items to stock HWguy. It was just being set to Aspect here.

		self->items_allowed = PR_PC_HVYWEAP_WEAPONS;

		// set the weapon icons on the status bar
		self->items = self->items | PR_IT_SHOTGUN | PR_IT_SUPER_SHOTGUN | PR_IT_ROCKET_LAUNCHER;
		//kill_my_demons();
	}
	else if ( self->playerclass == PR_PC_PYRO )
	{
		self->weapons_carried = self->weapons_carried | PR_PC_PYRO_WEAPONS;
		self->ammo_rockets = PR_PC_PYRO_INITAMMO_ROCKET;
		self->ammo_nails = PR_PC_PYRO_INITAMMO_NAIL;
		self->ammo_shells = PR_PC_PYRO_INITAMMO_SHOT;
		self->ammo_cells = PR_PC_PYRO_INITAMMO_CELL;
		self->maxammo_rockets = PR_PC_PYRO_MAXAMMO_ROCKET;
		self->maxammo_nails = PR_PC_PYRO_MAXAMMO_NAIL;
		self->maxammo_shells = PR_PC_PYRO_MAXAMMO_SHOT;
		self->maxammo_cells = PR_PC_PYRO_MAXAMMO_CELL;

		self->no_grenades_1 = PR_PC_PYRO_GRENADE_INIT_1;
		self->no_grenades_2 = PR_PC_PYRO_GRENADE_INIT_2;
		self->tp_grenades_1 = PR_PC_PYRO_GRENADE_TYPE_1;
		self->tp_grenades_2 = PR_PC_PYRO_GRENADE_TYPE_2;
		self->tf_items = PR_PC_PYRO_TF_ITEMS;
		self->cutf_items = PR_PC_PYRO_CUTF_ITEMS;          // PZ: added this, in case we ever want to add CuTF items to stock pyro
		self->job = self->job | PR_PC_PYRO_JOB;

		self->armorclass = self->armorclass | PR_PC_PYRO_INITARMORCLASS;
		self->armortype = PR_PC_PYRO_INITARMORTYPE;
		self->armorvalue = PR_PC_PYRO_INITARMOR;
		self->armor_allowed = PR_PC_PYRO_MAXARMORTYPE;
		self->maxarmor = PR_PC_PYRO_MAXARMOR;
		self->current_weapon = PR_WEAP_FLAMETHROWER;
		self->items_allowed = PR_PC_PYRO_WEAPONS;
		//kill_my_demons();

		// set the weapon icons on the status bar
		self->items = self->items | PR_IT_SHOTGUN | PR_IT_GRENADE_LAUNCHER | PR_IT_ROCKET_LAUNCHER;
	}
	else if ( self->playerclass == PR_PC_CIVILIAN )
	{
		self->weapons_carried = self->weapons_carried | PR_PC_CIVILIAN_WEAPONS;
		self->ammo_rockets = PR_PC_CIVILIAN_INITAMMO_ROCKET;
		self->ammo_nails = PR_PC_CIVILIAN_INITAMMO_NAIL;
		self->ammo_shells = PR_PC_CIVILIAN_INITAMMO_SHOT;
		self->ammo_cells = PR_PC_CIVILIAN_INITAMMO_CELL;
		self->maxammo_rockets = PR_PC_CIVILIAN_MAXAMMO_ROCKET;
		self->maxammo_nails = PR_PC_CIVILIAN_MAXAMMO_NAIL;
		self->maxammo_shells = PR_PC_CIVILIAN_MAXAMMO_SHOT;
		self->maxammo_cells = PR_PC_CIVILIAN_MAXAMMO_CELL;

		self->no_grenades_1 = PR_PC_CIVILIAN_GRENADE_INIT_1;
		self->no_grenades_2 = PR_PC_CIVILIAN_GRENADE_INIT_2;
		self->tp_grenades_1 = PR_PC_CIVILIAN_GRENADE_TYPE_1;
		self->tp_grenades_2 = PR_PC_CIVILIAN_GRENADE_TYPE_2;
		self->tf_items = PR_PC_CIVILIAN_TF_ITEMS;
		self->job = self->job | PR_PC_CIVILIAN_JOB;

		self->armorclass = self->armorclass | PR_PC_CIVILIAN_INITARMORCLASS;
		self->armortype = PR_PC_CIVILIAN_INITARMORTYPE;
		self->armorvalue = PR_PC_CIVILIAN_INITARMOR;
		self->armor_allowed = PR_PC_CIVILIAN_MAXARMORTYPE;
		self->maxarmor = PR_PC_CIVILIAN_MAXARMOR;
		self->current_weapon = PR_WEAP_AXE;
		self->items_allowed = PR_PC_CIVILIAN_WEAPONS;
		self->cutf_items = PR_PC_CIVILIAN_CUTF_ITEMS;

		// set the weapon icons on the status bar
		self->items = 0;
		//kill_my_demons();
	}
	else if ( self->playerclass == PR_PC_SPY )
	{
		self->weapons_carried = self->weapons_carried | PR_PC_SPY_WEAPONS;
		self->ammo_rockets = PR_PC_SPY_INITAMMO_ROCKET;
		self->ammo_nails = PR_PC_SPY_INITAMMO_NAIL;
		self->ammo_shells = PR_PC_SPY_INITAMMO_SHOT;
		self->ammo_cells = PR_PC_SPY_INITAMMO_CELL;
		self->maxammo_rockets = PR_PC_SPY_MAXAMMO_ROCKET;
		self->maxammo_nails = PR_PC_SPY_MAXAMMO_NAIL;
		self->maxammo_shells = PR_PC_SPY_MAXAMMO_SHOT;
		self->maxammo_cells = PR_PC_SPY_MAXAMMO_CELL;

		self->no_grenades_1 = PR_PC_SPY_GRENADE_INIT_1;
		self->no_grenades_2 = PR_PC_SPY_GRENADE_INIT_2;
		self->tp_grenades_1 = PR_PC_SPY_GRENADE_TYPE_1;
		self->tp_grenades_2 = PR_PC_SPY_GRENADE_TYPE_2;
		self->tf_items = PR_PC_SPY_TF_ITEMS;
		self->job = self->job | PR_PC_SPY_JOB;

		self->armorclass = self->armorclass | PR_PC_SPY_INITARMORCLASS;
		self->armortype = PR_PC_SPY_INITARMORTYPE;
		self->armorvalue = PR_PC_SPY_INITARMOR;
		self->armor_allowed = PR_PC_SPY_MAXARMORTYPE;
		self->maxarmor = PR_PC_SPY_MAXARMOR;
		self->current_weapon = PR_WEAP_TRANQ;
		self->items_allowed = PR_PC_SPY_WEAPONS;
		self->cutf_items = PR_PC_SPY_CUTF_ITEMS;

		// set the weapon icons on the status bar
		self->items = self->items | PR_IT_SHOTGUN | PR_IT_SUPER_SHOTGUN | PR_IT_NAILGUN;


		// If the Spy only has invis, start the Spy's cell regen timer
		if (invis_only == PR_TRUE)
		{
			te = spawnServerSide(); // PZ: make it a server-side only entity
			te->nextthink = time + PR_PC_SPY_CELL_REGEN_TIME;
			te->think = TeamFortress_RegenerateCells;
			te->owner = self;
			te->classname = "timer";
		}
	}
	else if ( self->playerclass == PR_PC_ENGINEER )
	{
		self->weapons_carried = self->weapons_carried | PR_PC_ENGINEER_WEAPONS;
		self->ammo_rockets = PR_PC_ENGINEER_INITAMMO_ROCKET;
		self->ammo_nails = PR_PC_ENGINEER_INITAMMO_NAIL;
		self->ammo_shells = PR_PC_ENGINEER_INITAMMO_SHOT;
		self->ammo_cells = PR_PC_ENGINEER_INITAMMO_CELL;
		self->maxammo_rockets = PR_PC_ENGINEER_MAXAMMO_ROCKET;
		self->maxammo_nails = PR_PC_ENGINEER_MAXAMMO_NAIL;
		self->maxammo_shells = PR_PC_ENGINEER_MAXAMMO_SHOT;
		self->maxammo_cells = PR_PC_ENGINEER_MAXAMMO_CELL;

		self->no_grenades_1 = PR_PC_ENGINEER_GRENADE_INIT_1;
		self->no_grenades_2 = PR_PC_ENGINEER_GRENADE_INIT_2;
		self->tp_grenades_1 = PR_PC_ENGINEER_GRENADE_TYPE_1;
		self->tp_grenades_2 = PR_PC_ENGINEER_GRENADE_TYPE_2;
		self->tf_items = PR_PC_ENGINEER_TF_ITEMS;
		self->job = self->job | PR_PC_ENGINEER_JOB;

		self->armorclass = self->armorclass | PR_PC_ENGINEER_INITARMORCLASS;
		self->armortype = PR_PC_ENGINEER_INITARMORTYPE;
		self->armorvalue = PR_PC_ENGINEER_INITARMOR;
		self->armor_allowed = PR_PC_ENGINEER_MAXARMORTYPE;
		self->maxarmor = PR_PC_ENGINEER_MAXARMOR;
		self->current_weapon = PR_WEAP_RAILGUN;
		self->items_allowed = PR_PC_ENGINEER_WEAPONS;
		self->cutf_items = PR_PC_ENGINEER_CUTF_ITEMS;

		// set the weapon icons on the status bar
		self->items = self->items | PR_IT_SHOTGUN | PR_IT_SUPER_SHOTGUN;
		//kill_my_demons();
	}
	else if ( self->playerclass == PR_PC_UNDEFINED )
	{
		self->items = 0;
		self->ammo_rockets = 0;
		self->ammo_nails = 0;
		self->ammo_shells = 0;
		self->ammo_cells = 0;

		self->no_grenades_1 = 0;
		self->no_grenades_2 = 0;
		self->tp_grenades_1 = 0;
		self->tp_grenades_2 = 0;

		self->armorclass = 0;
		self->armortype = 0;
		self->armorvalue = 0;
		self->weapon = 0;
		self->current_weapon = 0;
		self->weapons_carried = 0;

		self->flags = PR_FL_CLIENT | PR_FL_NOTARGET; // | #FL_ONGROUND;
		self->waterlevel = 3;
		self->takedamage = PR_DAMAGE_NO;
		self->solid = PR_SOLID_NOT;
		self->movetype = PR_MOVETYPE_NOCLIP;

		self->model = string_null;
		self->mdl = string_null;
		self->modelindex = 0;
		self->weaponmodel = string_null;
		modelindex_player = 0;
		self->tfstate = self->tfstate | PR_TFSTATE_RELOADING;

		setmodel(self, string_null);
	}

// if in coop mode, give them the max
#ifdef PR_COOP_MODE_ENHANCED
	if ( !deathmatch ) {
		self->armorvalue = self->maxarmor;
		self->ammo_shells = self->maxammo_shells;
		self->ammo_nails = self->maxammo_nails;
		self->ammo_rockets = self->maxammo_rockets;
		self->ammo_cells = self->maxammo_cells;
	}
#endif

	/*if (stock_mode==1) //- OfN - Stock classes don't get a job when stock_mode is 1
		self.job=0;*/

	//- OfN - Set initial grens number
	self->no_grenades_1 = GetMaxGrens(self,1);
	self->no_grenades_2 = GetMaxGrens(self,2);

	if (!(self->job & PR_JOB_ARMY) && !(self->job & PR_JOB_WARLOCK))
		kill_my_demons();

	// Initialize any job stuff if needed
	InitJobStuff();

	// Give normal classes the hook
	// Custom classes get it in 'drop into custom class gen'
	if (allow_hook && self->playerclass != PR_PC_UNDEFINED && self->playerclass != PR_PC_CUSTOM)
		self->weapons_carried = self->weapons_carried | PR_WEAP_HOOK;

	// Restore our original items if needed
	if (self->playerclass == PR_PC_CUSTOM)
		RestoreOriginalItems(self);
	else
		ResetStripedAndGiven(self);

	// Remove disallowed items on player, if any
	StripDisallowed(self);
	// Give free items, if any
	GiveFreeStuff(self);

	W_SetCurrentAmmo ();
}

//=========================================================================
// Return the max amount of ammo the Retriever can carry, based on his class
float TeamFortress_GetMaxAmmo(entity Retriever, float AmmoType)
{
	if (AmmoType == PR_IT_SHELLS)
		return Retriever->maxammo_shells;
	else if (AmmoType == PR_IT_NAILS)
		return Retriever->maxammo_nails;
	else if (AmmoType == PR_IT_CELLS)
		return Retriever->maxammo_cells;
	else if (AmmoType == PR_IT_ROCKETS)
		return Retriever->maxammo_rockets;
	else if (AmmoType == PR_WEAP_MEDIKIT)
		return Retriever->maxammo_medikit;
	else if (AmmoType == 131072) //WK Used to be #WEAP_DETPACK
		return Retriever->maxammo_detpack;

	RPrint("Error in TeamFortress_GetMaxAmmo()\n");
	RPrint("Invalid ammo type passed.\n");
	return 0;
}

//=========================================================================
// Return 1 if the Retriever is allowed to pick up the Weapon
float TeamFortress_CanGetWeapon(entity Retriever, int WeaponType)
{
	if ( Retriever->items_allowed & WeaponType )
		return PR_TRUE;

	return PR_FALSE;
}


//=========================================================================
// Print a string to Player, describing this type of armor
//CH returns number effected
float TeamFortress_DescribeArmor(entity Player, int Armorclass)
{
	//local string st;
	float num;
	num = 0;
	if (Armorclass == 0)
		return num;

	if (Armorclass & PR_AT_SAVEFIRE) {
		sprint (Player, PR_PRINT_HIGH, "Asbestos ");
		num = num + 1;
	}
	if (Armorclass & PR_AT_SAVEEXPLOSION) {
		sprint (Player, PR_PRINT_HIGH, "Blast ");
		num = num + 1;
	}
	if (Armorclass & PR_AT_SAVEELECTRICITY) {
		sprint (Player, PR_PRINT_HIGH, "Ceramic ");
		num = num + 1;
	}
	if (Armorclass & PR_AT_SAVEMELEE) {
		sprint (Player, PR_PRINT_HIGH, "Gel ");
		num = num + 1;
	}
	if (Armorclass & PR_AT_SAVESHOT) {
		sprint (Player, PR_PRINT_HIGH, "Kevlar ");
		num = num + 1;
	}

	sprint (Player, PR_PRINT_HIGH, "armor");
	return num;
}

//=========================================================================
// Controls the equipment a class receives from backpacks
float TeamFortress_AddBackpackItems(entity Retriever, entity Items)
{
	// If you want the classes to _not_ start off with all their legal
	// weapons, then you may want them to be able to pick up weapons
	// from backpacks. If so, this is where to do it.
	// For now, return.
	return PR_FALSE;	// Gizmo - what is this suppose to be returning?
}

//=========================================================================
// Return a string containing the class name for pc
string TeamFortress_GetClassName(float pc)
{
	if ( pc == PR_PC_SCOUT )
		return "Scout";
	else if ( pc == PR_PC_SNIPER )
		return "Sniper";
	else if ( pc == PR_PC_SOLDIER )
		return "Soldier";
	else if ( pc == PR_PC_DEMOMAN )
		return "Demolitions Man";
	else if ( pc == PR_PC_MEDIC )
		return "Combat Medic";
	else if ( pc == PR_PC_HVYWEAP )
		return "Heavy Weapons Guy";
	else if ( pc == PR_PC_PYRO )
		return "Pyro";
	else if ( pc == PR_PC_SPY )
		return "Spy";
	else if ( pc == PR_PC_ENGINEER )
		return "Engineer";
	else if ( pc == PR_PC_CIVILIAN )
		return "Civilian";
	else if ( pc == PR_PC_UNDEFINED )
		return "Observer";
	else if ( pc == PR_PC_RANDOM)
		return "Random Playerclass";
	else if ( pc == PR_PC_CUSTOM)
		return "Custom Playerclass";
	return "ERROR"; // PZ: added for C++ compiler
}

//=========================================================================
// Return a string containing the class name for pc
string TeamFortress_GetJobName(int pc)
{
	if ( pc & PR_JOB_THIEF )
		return "Thief";
	else if ( pc & PR_JOB_RUNNER )
		return "Runner";
	else if ( pc & PR_JOB_WARLOCK )
		return "Warlock";
	else if ( pc & PR_JOB_CHAPLAN )
		return "Combat Chaplan";
	else if ( pc & PR_JOB_BERSERKER )
		return "Nordish Berserker";
	else if ( pc & PR_JOB_GUERILLA )
		return "Communist Rebel";
	else if ( pc & PR_JOB_JUDOKA )
		return "Judo Black Belt";
	else if ( pc & PR_JOB_ARMY )
		return "Army Officer";
	else if ( pc & PR_JOB_HACKER )
		return "Hacker";
	else if ( pc & PR_JOB_MARTYR )
		return "Martyr";
	else if ( pc & PR_JOB_CRUSADER )
		return "Crusader";
	else
		return "Custom Playerclass";
	return "ERROR"; // PZ: added for C++ compiler
}

//=========================================================================
// Display the class of Player to Viewer
void TeamFortress_PrintClassName(entity Viewer, float pc, float rpc)
{
	string st;
	st = TeamFortress_GetClassName(pc);
	sprint (Viewer, PR_PRINT_HIGH, st);

	if (rpc != 0)
		sprint (Viewer, PR_PRINT_HIGH, " (Random)");
	sprint (Viewer, PR_PRINT_HIGH, "\n");
}

//=========================================================================
// Display the job of a custom class to Viewer
void TeamFortress_PrintJobName(entity Viewer, float pc)
{
	string st;
	st = TeamFortress_GetJobName(pc);
	sprint (Viewer, PR_PRINT_HIGH, st);
	sprint (Viewer, PR_PRINT_HIGH, "\n");
}

//=========================================================================
// Say the class of Player to him/her
#ifdef PR_SPEECH
void TeamFortress_SayClassName(entity player)
{
	if ( player->playerclass == PR_PC_SCOUT )
		stuffcmd(player, "play speech/scout.wav\n");
	else if ( player->playerclass == PR_PC_SNIPER )
		stuffcmd(player, "play speech/sniper.wav\n");
	else if ( player->playerclass == PR_PC_SOLDIER )
		stuffcmd(player, "play speech/soldier.wav\n");
	else if ( player->playerclass == PR_PC_DEMOMAN )
		stuffcmd(player, "play speech/demoman.wav\n");
	else if ( player->playerclass == PR_PC_MEDIC )
		stuffcmd(player, "play speech/medic.wav\n");
	else if ( player->playerclass == PR_PC_HVYWEAP )
		stuffcmd(player, "play speech/hvyweap.wav\n");
	else if ( player->playerclass == PR_PC_PYRO )
		stuffcmd(player, "play speech/pyro.wav\n");
	else if ( player->playerclass == PR_PC_SPY )
		stuffcmd(player, "play speech/spy.wav\n");
	else if ( player->playerclass == PR_PC_ENGINEER )
		stuffcmd(player, "play speech/engineer.wav\n");
	else if ( player->playerclass == PR_PC_CIVILIAN )
		stuffcmd(player, "play speech/civilian.wav\n");
}
#endif

//=========================================================================
// Remove all the timers for this player
// This function is _always_ called when a player dies.
void TeamFortress_RemoveTimers()
{
	entity te;
	entity oself;

	self->leg_damage = 0;
	self->is_undercover = 0;
	self->is_building = 0;
	self->building = world;

	//CH - TOFIX: IS THIS WHAT CAUSES GRENS TO BE TRHOWN AFTER RESPAWNING??
	//stuffcmd (self, "throwgren\n"); //Throw any grenades
	TeamFortress_ThrowGrenade();

	if (self->tfstate & PR_TFSTATE_GRENTHROWING)
	{
		oself = self;

		te = find(world,"classname","grenade");

		while (te != world)
		{
			if (te->owner == oself)
			if (te->think == TeamFortress_GrenadePrimed)
			{
				self = te;
				TeamFortress_GrenadePrimed();
			}

			te = find(te,"classname","grenade");
		}

		self = oself;
	}
	//stuffcmd (self, "impulse 169\n"); //Stop laying dets

	TeamFortress_DetpackStop(PR_FALSE);

	// Clear the sniper's aiming
	if (self->tfstate & PR_TFSTATE_AIMING)
	{
		self->tfstate = self->tfstate - PR_TFSTATE_AIMING;
		TeamFortress_SetSpeed(self);
		self->heat = 0;
	}

	// Remove all the timer entities for this player
	te = find(world, "classname", "timer");
	while (te != world)
	{
		//WK Judokatimer is a little weird
		if (te->netname == "judokatimer" && te->think == JudokaRearm)
		{
			if (te->owner == self || te->enemy == self)
			{
				oself = self;
				self = te;
				self->think();
				self = oself;
				te->think = SUB_Remove;
				te->nextthink = time + 0.1;
			}
			te = find(te, "classname", "timer");
		}
		else if (te->owner == self)
		{
			//WK Fix our new little flash timer
			if (te->netname == "flashtimer")
			{
				self->FlashTime = 0;
				stuffcmd(self, "v_cshift 0\n");
				stuffcmd(self, "r_norefresh 0;wait;echo;wait;echo;wait;echo;wait;echo\n");
				dremove(te);
				te = find(te, "classname", "timer");
			}
			//WK Support for the curse timer
			else if (te->netname == "bastardtimer")
			{
				if (self->is_connected == PR_FALSE)
				{
					// Bastard Memory
					// Only remove if the player has completed penance time. -Pulseczar
					if (self->penance_time <= time)
					{
						delstr(te->t_s_h);
						dremove(te);
					}
					// Otherwise, store their penance time in the bastard timer. -PZ
					else
					{
						te->penance_time = self->penance_time;
						te->owner = world;
					}
				}
				te = find(te, "classname", "timer");
			}
			else if (te->netname == "alias") // Ignore alias timer
			{
				// do nothing, just skip it (IT MUST NOT BE REMOVED EVER)
				te = find(te, "classname", "timer");
			}
			else
			{
				dremove(te);
				te = find(te, "classname", "timer");
			}
		}
		else
			te = find(te, "classname", "timer");
	}

	// Drop any GoalItems
	te = find (world, "classname", "item_tfgoal");
	while (te != world)
	{
		if (te->owner == self)
		{
			// Remove it from the player, if it is supposed to be
			if (!(te->goal_activation & PR_TFGI_KEEP))
			{
				tfgoalitem_RemoveFromPlayer(te, self, 0);
			}

			// CTF support
			if (CTF_Map == PR_TRUE && te->goal_no == PR_CTF_FLAG1)
			{
				bprint(PR_PRINT_HIGH, self->netname);
				bprint(PR_PRINT_HIGH, S_(" ^bLOST^b the ^bBLUE^b flag!\n"));
			}
			else if (CTF_Map == PR_TRUE && te->goal_no == PR_CTF_FLAG2)
			{
				bprint(PR_PRINT_HIGH, self->netname);
				bprint(PR_PRINT_HIGH, S_(" ^bLOST^b the ^bRED^b flag!\n"));
			}

		}
		te = find(te, "classname", "item_tfgoal");
	}

	// Reset detpacks being disarmed to not being disarmed
	te = find (world, "classname", "detpack");
	while (te != world)
	{
		if ((te->weaponmode == 1) && (te->enemy == self))
		{
			te->weaponmode = 0;
		}
		te = find(te, "classname", "detpack");
	}

	// detonate all of the players pipebombs
	TeamFortress_DetonatePipebombs();

	// OfN Remove holo if player dies!
	if (self->has_holo > 0) RemoveHolo(self);

	stuffcmd(self, "v_idlescale 0\n");
	self->tfstate = self->tfstate - (self->tfstate & PR_TFSTATE_CONCUSSIONED); // PZ: added this flag for concussing bots
	stuffcmd(self, "v_cshift 0 0 0 0\n");
	self->item_list = 0;
	self->FlashTime = 0;

	// Remove Menu
	CenterPrint(self, "\n");
	self->menu_count = PR_MENU_REFRESH_RATE;
	self->current_menu = PR_MENU_DEFAULT;
	self->impulse = 0;
}

//=========================================================================
// Setup the Respawn delays for this player
void TeamFortress_SetupRespawn(float Suicided)
{
	float restime;//,teamsup;
	string db;

	if (self->respawn_time > time)
		return; 	// already respawning

	// Setup Respawn Delay
	if (toggleflags & PR_TFLAG_RESPAWNDELAY)
		restime = respawn_delay_time;
	else
		restime = 0;

	// PZ - respawn delay for blue players in AGR mode
	if (agr && self->team_no == 1 && !restime)
		restime = PR_AGR_BLUE_RESPAWN_DELAY;

	// PZ - respawn delay for defense players in Invade mode
	if (invade && self->team_no == invade_teamOnDef && invade_gameState != InvadeGameState::IN_PREMATCH && !restime)
		restime = PR_INVADE_DEF_RESPAWN_DELAY;

// Gizmo - remove respawn delay for coop, TODO: enable a toggleable amount in localinfo
#ifdef PR_COOP_MODE_ENHANCED
	if ( deathmatch ) {
		// Suiciders can't respawn immediately
		if (Suicided)
			restime = restime + PR_SUICIDE_RESPAWN_DELAY;
	}
#else
	// Suiciders can't respawn immediately
	if (Suicided)
		restime = restime + PR_SUICIDE_RESPAWN_DELAY;
#endif

	// Remove a life
	if (self->lives > 0 && prematch < time)
	{
		self->lives = self->lives - 1;
/*		if (self.team_no == 1) team1total = team1total - 1;
		else if (self.team_no == 2) team1total = team2total - 1;
		else if (self.team_no == 3) team1total = team3total - 1;
		else if (self.team_no == 4) team1total = team4total - 1;*///- ???


	}

	if (self->lives != -1)
	{
		if (self->lives == 0)
		{
			// Move to Observer mode when you die
			PlayerObserverMode();
			//if (livesperguy > 1)
			//{
				sprint (self, PR_PRINT_HIGH, "NO lives left, returning to Observer mode.\n");
				bprint (PR_PRINT_MEDIUM, self->netname);
				bprint (PR_PRINT_MEDIUM, " ran out of lives!\n");
			//}
			return;
		}

		if (self->lives == 1)
			sprint (self, PR_PRINT_HIGH, "LAST life.\n");
		else
		{
			db = ftos(self->lives);
			sprint(self, PR_PRINT_HIGH, db);
			sprint (self, PR_PRINT_HIGH, " lives left.\n");
		}
	}//*/// ???

	// Do this after life calculation, so people without any
	// lives left can respawn back to Observer mode without a delay.
	self->respawn_time = time + restime;

	if (restime > 3)
	{
		db = ftos(restime);

		sprint(self, PR_PRINT_HIGH, db);
		sprint(self, PR_PRINT_HIGH, " seconds till respawn.\n");
	}
}

//=========================================================================
// Check all stats to make sure they're good for this class
void TeamFortress_CheckClassStats()
{
	// Check armor
	if (self->armortype > self->armor_allowed)
		self->armortype = self->armor_allowed;
	if (self->armorvalue > self->maxarmor)
		self->armorvalue = self->maxarmor;
	if (self->armortype < 0)
		self->armortype = 0;
	if (self->armorvalue < 0)
		self->armorvalue = 0;
	// Check ammo
	if (self->ammo_shells > TeamFortress_GetMaxAmmo(self,PR_IT_SHELLS))
		self->ammo_shells = TeamFortress_GetMaxAmmo(self,PR_IT_SHELLS);
	if (self->ammo_shells < 0)
		self->ammo_shells = 0;
	if (self->ammo_nails > TeamFortress_GetMaxAmmo(self,PR_IT_NAILS))
		self->ammo_nails = TeamFortress_GetMaxAmmo(self,PR_IT_NAILS);
	if (self->ammo_nails < 0)
		self->ammo_nails = 0;
	if (self->ammo_rockets > TeamFortress_GetMaxAmmo(self,PR_IT_ROCKETS))
		self->ammo_rockets = TeamFortress_GetMaxAmmo(self,PR_IT_ROCKETS);
	if (self->ammo_rockets < 0)
		self->ammo_rockets = 0;
	if (self->ammo_cells > TeamFortress_GetMaxAmmo(self,PR_IT_CELLS))
		self->ammo_cells = TeamFortress_GetMaxAmmo(self,PR_IT_CELLS);
	if (self->ammo_cells < 0)
		self->ammo_cells = 0;
	if (self->ammo_medikit > TeamFortress_GetMaxAmmo(self,PR_WEAP_MEDIKIT))
		self->ammo_medikit = TeamFortress_GetMaxAmmo(self,PR_WEAP_MEDIKIT);
	if (self->ammo_medikit < 0)
		self->ammo_medikit = 0;
	if (self->ammo_detpack > TeamFortress_GetMaxAmmo(self,131072))
		self->ammo_detpack = TeamFortress_GetMaxAmmo(self,131072);
	if (self->ammo_detpack < 0)
		self->ammo_detpack = 0;
	// Check Grenades
	if (self->no_grenades_1 < 0)
		self->no_grenades_1 = 0;
	if (self->no_grenades_2 < 0)
		self->no_grenades_2 = 0;
	// Check health
	if (self->health > self->max_health && !(self->items & PR_IT_SUPERHEALTH))
		TF_T_Damage (self, world, world, (self->max_health - self->health), 0, PR_TF_TD_NOSOUND);
	if (self->health < 0)
		T_Heal(self, (self->health - self->health), 0);

	// Update armor picture
	self->items = self->items - (self->items & (PR_IT_ARMOR1 | PR_IT_ARMOR2 | PR_IT_ARMOR3));
	if (self->armortype >= 0.8)
		self->items = self->items | PR_IT_ARMOR3;
	else if (self->armortype >= 0.6)
		self->items = self->items | PR_IT_ARMOR2;
	else if (self->armortype >= 0.3)
		self->items = self->items | PR_IT_ARMOR1;
}

//=========================================================================
// AMMOBOX Handling
//=========================================================================
// Throw an ammo box with 10 shells, 10 nails, 5 cells or 5 rockets
void TeamFortress_DropAmmo(float type)
{
	float ammo;

	if (type == 1)
	{
		ammo = PR_DROP_SHELLS;
		if (self->ammo_shells < ammo)
		{
/*CH who cares, we need space
			// ENGINEER can make ammo
			if (self.playerclass == #PC_ENGINEER)
			{
				if ((self.ammo_cells / #AMMO_COST_SHELLS) > (ammo - self.ammo_shells))
				{
					sprint (self, #PRINT_HIGH, "you make some shells.\n");
					self.ammo_cells = self.ammo_cells - ((ammo - self.ammo_shells) * #AMMO_COST_SHELLS);
					self.ammo_shells = ammo;
				}
			}

			if (self.ammo_shells < ammo)
*/				return;
		}

		self->ammo_shells = self->ammo_shells - ammo;
	}
	else if (type == 2)
	{
		ammo = PR_DROP_NAILS;
		if (self->ammo_nails < ammo)
		{
/*CH who cares, we need space
			// ENGINEER can make ammo
			if (self.playerclass == #PC_ENGINEER)
			{
				if ((self.ammo_cells / #AMMO_COST_NAILS) > (ammo - self.ammo_nails))
				{
					sprint (self, #PRINT_HIGH, "you make some nails.\n");
					self.ammo_cells = self.ammo_cells - ((ammo - self.ammo_nails) * #AMMO_COST_NAILS);
					self.ammo_nails = ammo;
				}
			}

			if (self.ammo_nails < ammo)
*/				return;
		}

		self->ammo_nails = self->ammo_nails - ammo;
	}
	else if (type == 3)
	{
		ammo = PR_DROP_ROCKETS;
		if (self->ammo_rockets < ammo)
		{
/*CH who cares, we need space
			// ENGINEER can make ammo
			if (self.playerclass == #PC_ENGINEER)
			{
				if ((self.ammo_cells / #AMMO_COST_ROCKETS) > (ammo - self.ammo_rockets))
				{
					sprint (self, #PRINT_HIGH, "you make some rockets.\n");
					self.ammo_cells = self.ammo_cells - ((ammo - self.ammo_rockets) * #AMMO_COST_ROCKETS);
					self.ammo_rockets = ammo;
				}
			}

			if (self.ammo_rockets < ammo)
*/				return;
		}

		self->ammo_rockets = self->ammo_rockets - ammo;
	}
//	else if (type == 4)
	else
	{
		ammo = PR_DROP_CELLS;
		if (self->ammo_cells < ammo)
		{
/*CH who cares, we need space
			// ENGINEER can make ammo
			if (self.playerclass == #PC_ENGINEER)
			{
				if ((self.ammo_cells / #AMMO_COST_CELLS) > (ammo - self.ammo_cells))
				{
					sprint (self, #PRINT_HIGH, "you make some cells.\n");
					self.ammo_cells = self.ammo_cells - ((ammo - self.ammo_cells) * #AMMO_COST_CELLS);
					self.ammo_cells = ammo;
				}
			}

			if (self.ammo_cells < ammo)
*/				return;
		}

		self->ammo_cells = self->ammo_cells - ammo;
	}
	W_SetCurrentAmmo();

	if (self->team_no != 0)
	{
		increment_team_ammoboxes(self->team_no);
		if (num_team_ammoboxes(self->team_no) > (PR_MAX_WORLD_AMMOBOXES / number_of_teams))
			RemoveOldAmmobox(self->team_no);
	}
	else
	{
		num_world_ammoboxes = num_world_ammoboxes + 1;
		if (num_world_ammoboxes > PR_MAX_WORLD_AMMOBOXES)
			RemoveOldAmmobox(0);
	}

	newmis = spawn();
	newmis->aflag = ammo;
	newmis->weapon = type;
	if (newmis->weapon == 1)
		newmis->ammo_shells = ammo;
	else if (newmis->weapon == 2)
		newmis->ammo_nails = ammo;
	else if (newmis->weapon == 3)
		newmis->ammo_rockets = ammo;
	else if (newmis->weapon == 4)
		newmis->ammo_cells = ammo;
	newmis->enemy = self;
	newmis->health = time;
	newmis->movetype = PR_MOVETYPE_TOSS;
	newmis->solid = PR_SOLID_TRIGGER;
	newmis->classname = "ammobox";
	newmis->team_no = self->team_no;

	makevectors (self->v_angle);

	if (self->v_angle[X])
		newmis->velocity = v_forward*400 + v_up * 200;
	else
	{
		newmis->velocity = aim(self, 10000);
		newmis->velocity = newmis->velocity * 400;
		newmis->velocity[Z] = 200;
	}

	newmis->avelocity = V({0, 300, 0});
	setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (newmis, self->origin);
	newmis->nextthink = time + 30;	// remove after 30 seconds
	newmis->think = SUB_Remove;
	newmis->touch = TeamFortress_AmmoboxTouch;
	newmis->skin = type - 1;
	setmodel (newmis, "progs/ammobox.mdl");
}

void TeamFortress_AmmoboxTouch()
{
	string quantity;

	// Gizmo - added this to prevent things messing up
	if ( other->PR_BUTTON_FIRE )
		return;

	// Cant touch own ammobox for 2 seconds after throwing
	if ((other == self->enemy) && (time < self->health + 2))
		return;

	if (other->health <= 0)
		return;


	// Return if other is not a player
	if (other->classname != "player") //- OfN - Now grunt can take em
	{
		if (other->classname != "monster_army") return;

		num_world_ammoboxes = num_world_ammoboxes - 1;
		decrement_team_ammoboxes(self->team_no);

		other->ammo_shells = other->ammo_shells + self->ammo_shells;
		other->ammo_nails = other->ammo_nails + self->ammo_nails;
		other->ammo_rockets = other->ammo_rockets + self->ammo_rockets;
		other->ammo_cells = other->ammo_cells + self->ammo_cells;

		grunty_boundammo(other);

		PrintFromSoldier(other,other->real_owner,"i picked up some ammo.\n",PR_PRINT_MEDIUM);

		sound (other, PR_CHAN_ITEM, "weapons/lock4.wav", 1, PR_ATTN_NORM);

		dremove(self);
		self = other;

		return;
	}

	num_world_ammoboxes = num_world_ammoboxes - 1;
	decrement_team_ammoboxes(self->team_no);

	// discard backpack
	if (self->weapon == 0)
	{
		sprint(other, PR_PRINT_LOW, "You got ");
		if (self->ammo_shells > 0)
		{
			other->ammo_shells = other->ammo_shells + self->ammo_shells;
			quantity = ftos(self->ammo_shells);
			sprint(other, PR_PRINT_LOW, quantity, " shells  ");
		}
		if (self->ammo_nails > 0)
		{
			other->ammo_nails = other->ammo_nails + self->ammo_nails;
			quantity = ftos(self->ammo_nails);
			sprint(other, PR_PRINT_LOW, quantity, " nails  ");
		}
		if (self->ammo_rockets > 0)
		{
			other->ammo_rockets = other->ammo_rockets + self->ammo_rockets;
			quantity = ftos(self->ammo_rockets);
			sprint(other, PR_PRINT_LOW, quantity, " rockets  ");
		}
		if (self->ammo_cells > 0)
		{
			other->ammo_cells = other->ammo_cells + self->ammo_cells;
			quantity = ftos(self->ammo_cells);
			sprint(other, PR_PRINT_LOW, quantity, " cells  ");
		}
		sprint(other, PR_PRINT_LOW, "\n");
	}
	// shotgun
	else if (self->weapon == 1)
	{
		if (other->ammo_shells >= TeamFortress_GetMaxAmmo(other,PR_IT_SHELLS))
			return;
		other->ammo_shells = other->ammo_shells + self->aflag;
		self->netname = "shells";
	}
	// spikes
	else if (self->weapon == 2)
	{
		if (other->ammo_nails >= TeamFortress_GetMaxAmmo(other,PR_IT_NAILS))
			return;
		other->ammo_nails = other->ammo_nails + self->aflag;
		self->netname = "nails";
	}
	//	rockets
	else if (self->weapon == 3)
	{
		if (other->ammo_rockets >= TeamFortress_GetMaxAmmo(other,PR_IT_ROCKETS))
			return;
		other->ammo_rockets = other->ammo_rockets + self->aflag;
		self->netname = "rockets";
	}
	//	cells
	else if (self->weapon == 4)
	{
		if (other->ammo_cells >= TeamFortress_GetMaxAmmo(other,PR_IT_CELLS))
			return;
		other->ammo_cells = other->ammo_cells + self->aflag;
		self->netname = "cells";
	}

	bound_other_ammo (other);

	// Discard backpacks do their own printing
	if (self->weapon > 0)
	{
		quantity = ftos(self->aflag);
		sprint(other, PR_PRINT_LOW, "You picked up ", quantity, " ", self->netname, "\n");
	}

	sound (other, PR_CHAN_ITEM, "weapons/lock4.wav", 1, PR_ATTN_NORM);
	stuffcmd (other, "bf\n");

	dremove(self);
	self = other;
	W_SetCurrentAmmo();
}

// Return the number of pipebombs in existence for a particular team
float num_team_ammoboxes(float tno)
{
	if (tno == 1)
		return num_team_ammoboxes_1;
	else if (tno == 2)
		return num_team_ammoboxes_2;
	else if (tno == 3)
		return num_team_ammoboxes_3;
	else if (tno == 4)
		return num_team_ammoboxes_4;

	return 0;
}

// Remove enough old ammoboxes to bring the count down to the max level
void RemoveOldAmmobox(float tno)
{
	entity old;
	float index;

	if (tno != 0)
	{
		index = num_team_ammoboxes(tno);
		index = index - (PR_MAX_WORLD_AMMOBOXES / number_of_teams);
	}
	else
	{
		index = num_world_ammoboxes - PR_MAX_WORLD_AMMOBOXES;
	}

	old = find(world, "classname", "ammobox");
	while (index > 0)
	{
		if (old == world)
		{
//			RPrint("*** ERROR: RemoveOldAmmobox. ***\n");
//			RPrint("*** Shaka thought he fixed this ***\n");
			num_world_ammoboxes = num_world_ammoboxes - 1;
			decrement_team_ammoboxes(old->team_no);
			return;
		}

		if (old->team_no == tno || tno == 0)
		{
			old->think = SUB_Remove;
			old->nextthink = time + 0.1;
			index = index - 1;

			num_world_ammoboxes = num_world_ammoboxes - 1;
			decrement_team_ammoboxes(old->team_no);
		}

		old = find(old, "classname", "ammobox");
	}
}

void increment_team_ammoboxes(float tno)
{
	if (tno == 1)
		num_team_ammoboxes_1 = num_team_ammoboxes_1 + 1;
	else if (tno == 2)
		num_team_ammoboxes_2 = num_team_ammoboxes_2 + 1;
	else if (tno == 3)
		num_team_ammoboxes_3 = num_team_ammoboxes_3 + 1;
	else if (tno == 4)
		num_team_ammoboxes_4 = num_team_ammoboxes_4 + 1;
}

void decrement_team_ammoboxes(float tno)
{
	if (tno == 1)
		num_team_ammoboxes_1 = num_team_ammoboxes_1 - 1;
	else if (tno == 2)
		num_team_ammoboxes_2 = num_team_ammoboxes_2 - 1;
	else if (tno == 3)
		num_team_ammoboxes_3 = num_team_ammoboxes_3 - 1;
	else if (tno == 4)
		num_team_ammoboxes_4 = num_team_ammoboxes_4 - 1;
}

//=========================================================================
// WEAPON HANDLING FUNCTIONS
//=========================================================================
//=========================================================================
// Assault Cannon selection function
void TeamFortress_AssaultWeapon()
{
	//local float	it;

	self->impulse = 0;

	if (self->tfstate & PR_TFSTATE_RELOADING)
		return;

	if (!(self->weapons_carried & PR_WEAP_ASSAULT_CANNON))
		return;

	if (self->heat > 0)
	{
		sprint (self, PR_PRINT_HIGH, "the assault cannon is still overheated.\n");
		return;
	}

	if (self->ammo_shells < 1)
	{	// don't have the ammo
		sprint (self, PR_PRINT_HIGH, "not enough ammo.\n");
		return;
	}
	// The cannon also needs 6 cells to power up
	if (self->ammo_cells < 6)
	{
		sprint (self, PR_PRINT_HIGH, "not enough cells to power the assault cannon.\n");
		return;
	}

	self->current_weapon = PR_WEAP_ASSAULT_CANNON;

	W_SetCurrentAmmo ();
}

//=========================================================================
// If this gets called, the players holding onto an exploding grenade :)
void TeamFortress_ExplodePerson()
{
	//local entity te;

	// Removes the owners grenade
	self->owner->tfstate = self->owner->tfstate - (self->owner->tfstate & PR_TFSTATE_GRENPRIMED);

	KickPlayer(-2, self->owner);

	newmis = spawn ();
	newmis->movetype = PR_MOVETYPE_BOUNCE;
	newmis->solid = PR_SOLID_BBOX;
	newmis->classname = "grenade";
	newmis->team_no = self->owner->team_no;
	newmis->owner = self->owner;

	// Don't bother calculating a velocity
	newmis->velocity = V({0, 0, 0});
	newmis->angles = vectoangles(newmis->velocity);

	// set the grenades thinktime to now
	newmis->think = SUB_Null;
	newmis->nextthink = time + 0.1;

	// set the think and touches to the appropriate grenade type
	if (self->weapon == PR_GR_TYPE_NORMAL)
	{
		newmis->touch = NormalGrenadeTouch;
		newmis->think = NormalGrenadeExplode;
		newmis->skin = 0;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/hgren2_orig.mdl");
	}
	else if (self->weapon == PR_GR_TYPE_CONCUSSION)
	{
		newmis->touch = ConcussionGrenadeTouch;
		newmis->think = ConcussionGrenadeExplode;
		newmis->skin = 1;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/hgren2_orig.mdl");
	}
	else if (self->weapon == PR_GR_TYPE_BIO)
	{
		newmis->touch = BioGrenadeTouch;
		newmis->think = BioGrenadeExplode;
		newmis->skin = 0;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/hgren2_orig.mdl");
	}
	else if (self->weapon == PR_GR_TYPE_ANTIGRAV)
	{
		newmis->touch = ConcussionGrenadeTouch;
		newmis->think = AntiGravGrenadeExplode;
		newmis->skin = 1;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/hgren2_orig.mdl");
	}
	else if (self->weapon == PR_GR_TYPE_NAIL)
	{
		newmis->touch = NailGrenadeTouch;
		newmis->think = NailGrenadeExplode;
		newmis->skin = 1;
		newmis->avelocity = V({0, 300, 0});
		setmodel (newmis, "progs/biggren.mdl");
	}
	else if (self->weapon == PR_GR_TYPE_CALTROP)
	{
		newmis->touch = NormalGrenadeTouch;
		newmis->think = CaltropGrenadeExplode;
		newmis->skin = 0;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/hgren2_orig.mdl");
	}
	else if (self->weapon == PR_GR_TYPE_MIRV)
	{
		newmis->touch = MirvGrenadeTouch;
		newmis->think = MirvGrenadeExplode;
		newmis->skin = 0;
		newmis->avelocity = V({0, 300, 0});
		setmodel (newmis, "progs/biggren.mdl");
	}
	else if (self->weapon == PR_GR_TYPE_NAPALM)
	{
		newmis->touch = NapalmGrenadeTouch;
		newmis->think = NapalmGrenadeExplode;
		newmis->skin = 2;
		newmis->avelocity = V({0, 300, 0});
		setmodel (newmis, "progs/biggren.mdl");
	}
//CH no flare
//OfN why??
	else if (self->weapon == PR_GR_TYPE_FLARE)
	{
		//sprint(self.owner, #PRINT_HIGH, "Flare lit.\n");
		sprint(self->owner, PR_PRINT_HIGH, "The flare burns on your hand!\n");
		deathmsg = 0; // TODO
		T_Damage(self->owner, self->owner, self->owner,12);
		stuffcmd(self->owner, "bf\nbf\n");

		newmis->touch = FlareBounce;
		newmis->think = FlareGrenadeExplode;
		newmis->skin = 0;
		newmis->has_holo=1;
		newmis->avelocity = V({0, 0, 0});
		setmodel (newmis, "progs/flarefly.mdl");

		/*te = spawn();
		te.touch = SUB_Null;
		te.think = SUB_Remove;//FlareGrenadeExplode;//RemoveFlare;
		te.nextthink = time + 25;
		te.owner = self.owner;
		te.solid = #SOLID_NOT;
		self.owner.effects = self.owner.effects | #EF_BRIGHTLIGHT;*/

		setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));
		setorigin (newmis, self->owner->origin);


		dremove(self);
		//dremove(newmis);
		return;
	}
	else if (self->weapon == PR_GR_TYPE_GAS)
	{
		newmis->touch = GasGrenadeTouch;
		newmis->think = GasGrenadeExplode;
		newmis->skin = 2;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/grenade2.mdl");
	}
	else if (self->weapon == PR_GR_TYPE_EMP)
	{
		newmis->touch = EMPGrenadeTouch;
		newmis->think = EMPGrenadeExplode;
		newmis->skin = 4;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/grenade2.mdl");
	}
	#ifdef PR_OLD_FLASH
	else if (self->weapon == PR_GR_TYPE_FLASH)
	{
		newmis->touch = FlashGrenadeTouch;
		newmis->think = FlashGrenadeExplode;
		newmis->skin = 1;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/grenade2.mdl");
	}
	#else
	else if (self->weapon == PR_GR_TYPE_PSIONIC)
	{
		newmis->touch = PsionicGrenadeTouch;
		newmis->think = PsionicGrenadeExplode;
		newmis->skin = 1;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/grenade2.mdl");
	}
	#endif
	else if (self->weapon == PR_GR_TYPE_FRAG)
	{
		newmis->touch = FragGrenadeTouch;
		newmis->think = FragGrenadeExplode;
		newmis->skin = 1;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/grenade2.mdl");
	}
	else if (self->weapon == PR_GR_TYPE_KRAC)
	{
		newmis->touch = KracGrenadeTouch;
		newmis->think = KracGrenadeExplode;
		newmis->skin = 1;
		newmis->avelocity = V({300, 300, 300});
		setmodel (newmis, "progs/grenade2.mdl");
	}

	setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (newmis, self->owner->origin);

	//bprint(#PRINT_MEDIUM, "No ");
	//bprint(#PRINT_MEDIUM, self.owner.netname);
	//bprint(#PRINT_MEDIUM, ", throw the grenade, not the pin!\n");

	bprint(PR_PRINT_MEDIUM, "No ");
	bprint(PR_PRINT_MEDIUM, self->owner->netname);
	bprint(PR_PRINT_MEDIUM, ", you are supposed to THROW the grenade!\n");

	//no xxx, your grenade is your friend for another reason!
	//no

	// Remove primed grenade object
	dremove(self);
}

//=========================================================================
// Thrown Grenade touch function.
void NormalGrenadeTouch()
{
	if (other == self->owner)
		return; 	// don't explode on owner

	// Thrown grenades don't detonate when hitting an enemy

	sound (self, PR_CHAN_WEAPON, "weapons/bounce.wav", 1, PR_ATTN_NORM);	// bounce sound
	if (self->velocity == V({0, 0, 0}))
		self->avelocity = V({0, 0, 0});
}

//=========================================================================
// Thrown grenade explosion. 50% more powerful as grenade launcher grenades.
void NormalGrenadeExplode()
{
	deathmsg = PR_DMSG_GREN_HAND;
	T_RadiusDamage (self, self->owner, 180, world);

#ifdef PR_DEMO_STUFF
	// Remove any camera's locks on this missile
	if (self->enemy != world)
		CamProjectileLockOff();
#endif

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

//=========================================================================
// Displays the state of the items in the Detection Entity
void TeamFortress_DisplayDetectionItems()
{
	entity Goal, te;

	Goal = find(world, "classname", "info_tfdetect");
	if (Goal == world)
		return;

	// Check to make sure they've got some strings
	if (Goal->team_str_home == string_null)
		return;

	if (Goal->display_item_status1 != 0)
	{
		te = Finditem(Goal->display_item_status1);
		if (te != world)
			DisplayItemStatus(Goal, self, te);
		else
			sprint (self, PR_PRINT_HIGH, "Item is missing.\n");
	}
	else
		return;

	if (Goal->display_item_status2 != 0)
	{
		te = Finditem(Goal->display_item_status2);
		if (te != world)
			DisplayItemStatus(Goal, self, te);
		else
			sprint (self, PR_PRINT_HIGH, "Item is missing.\n");
	}
	else
		return;

	if (Goal->display_item_status3 != 0)
	{
		te = Finditem(Goal->display_item_status3);
		if (te != world)
			DisplayItemStatus(Goal, self, te);
		else
			sprint (self, PR_PRINT_HIGH, "Item is missing.\n");
	}
	else
		return;

	if (Goal->display_item_status4 != 0)
	{
		te = Finditem(Goal->display_item_status4);
		if (te != world)
			DisplayItemStatus(Goal, self, te);
		else
			sprint (self, PR_PRINT_HIGH, "Item is missing.\n");
	}
}

//=========================================================================
// ITEM HANDLING FUNCTIONS
//=========================================================================
// Function for handling the BioInfection Decay of players
void BioInfection_Decay()
{
	// Remove infections which can't hurt the owner
	if ((teamplay & PR_TEAMPLAY_NOEXPLOSIVE) && Teammate(self->owner, self->enemy))
	{
		self->owner->tfstate = self->owner->tfstate - (self->owner->tfstate & PR_TFSTATE_INFECTED);
		dremove(self);
		return;
	}

	if (self->invincible_finished > time)
	{
		self->owner->tfstate = self->owner->tfstate - (self->owner->tfstate & PR_TFSTATE_INFECTED);
		dremove(self);
		return;
	}

	if (self->owner->current_weapon == PR_WEAP_MEDIKIT)
	{
		sprint(self->owner,PR_PRINT_HIGH,"You heal yourself of your infection.\n");
		self->owner->tfstate = self->owner->tfstate - (self->owner->tfstate & PR_TFSTATE_INFECTED);
		dremove(self);
		return;
	}

	// remove this entity if the infection is gone
	if ( !(self->owner->tfstate & PR_TFSTATE_INFECTED) )
	{
		dremove(self);
		return;
	}

	self->nextthink = time + 2;

	deathmsg = PR_DMSG_BIOWEAPON;
	TF_T_Damage(self->owner, self, self->enemy, 5, PR_TF_TD_IGNOREARMOUR, PR_TF_TD_OTHER);

	SpawnBlood(self->owner->origin, 30);
}

//========================================================================
// Function for handling the BioInfection Decay of monsters
void BioInfection_MonsterDecay()
{
	float drmg;

	if (self->enemy == world)
	{
		dremove(self);
		return;
	}

	if (!(self->enemy->tfstate & PR_TFSTATE_INFECTED)) {
		dremove(self);
		return;
	}

	if (self->enemy->health <= 0)
	{
		dremove(self);
		return;
	}

	drmg = 125 * random(); // was 200
	drmg = drmg * random();

	self->nextthink = time + 2;

	if (self->enemy->health + 30 < drmg)
		drmg = self->enemy->health + 29;

	//T_Damage(self.enemy, self, self.owner, drmg);
#ifdef PR_COOP_MODE_ENHANCED
	if ( COOP_IsCoopMonster( self->enemy ) )
		deathmsg = PR_DMSG_BIOWEAPON;
#endif
	TF_T_Damage(self->enemy, self, self->owner, drmg, PR_TF_TD_NOPAIN,0);

	SpawnBlood(self->enemy->origin, drmg+10);

	if (self->enemy->health > 1 && self->enemy->classname == "monster_army")
	{
		if (random() < 0.10)
		{
			float rnum;
			rnum = random();

			if (rnum > 0.66)
				PrintFromSoldier(self->enemy,self->enemy->real_owner,"Argh! Medic!!\n",PR_PRINT_HIGH);
			else if (rnum > 0.33)
				PrintFromSoldier(self->enemy,self->enemy->real_owner,"Are you going to heal me?\n",PR_PRINT_HIGH);
			else
				PrintFromSoldier(self->enemy,self->enemy->real_owner,"My medikits won't cure me! Help!!\n",PR_PRINT_HIGH);

			sound(self->enemy, PR_CHAN_VOICE, "speech/saveme1.wav", 1, PR_ATTN_NORM);
		}
	}
}

//=========================================================================
// UTILITY FUNCTIONS
//=========================================================================
// Stuff an alias. This is a little messy since we insisted
// on maintaining the ability to alter the impulse numbers in the defs.qc
// and not have to change any code.
void TeamFortress_Alias(const string& halias, float himpulse1, float himpulse2)
{
	string imp;

	stuffcmd(self, "alias ");
	stuffcmd(self, halias);
	stuffcmd(self, " \"impulse ");
	imp = ftos(himpulse1);
	stuffcmd(self, imp);

	// if himpulse2 is not zero, assume that himpulse1 is a preimpulse
	// and complete the alias
	if (himpulse2 != 0)
	{
		stuffcmd(self, ";wait; impulse ");
		imp = ftos(himpulse2);
		stuffcmd(self, imp);
	}
	stuffcmd(self, "\"\n");
}

//=========================================================================
// CYCLIC EVENT FUNCTIONS
//=========================================================================
//=========================================================================
// Regenerates the entity which owns this cyclictimer
void TeamFortress_Regenerate()
{
//WK	if (self.owner.playerclass == #PC_MEDIC)
	if (self->is_abouttodie)
		return;
	if (self->owner->weapons_carried & PR_WEAP_MEDIKIT) //WK Custom class friendly
	{
		self->nextthink = time + PR_PC_MEDIC_REGEN_TIME;

		if (self->owner->health >= self->owner->max_health)
			return;

		if (self->owner->ammo_medikit == 0)
			return;

		if (self->owner->ammo_medikit < PR_PC_MEDIC_REGEN_AMOUNT)
		{
			self->owner->health = self->owner->health + self->owner->ammo_medikit;
			self->owner->ammo_medikit = 0;
		}
		else
		{
			self->owner->health = self->owner->health + PR_PC_MEDIC_REGEN_AMOUNT;
			self->owner->ammo_medikit = self->owner->ammo_medikit - PR_PC_MEDIC_REGEN_AMOUNT;
		}

		if (self->owner->health > self->owner->max_health)
			self->owner->health = self->owner->max_health;
	}
}

void CyberAugSleep()
{
	if (self->owner->velocity[Z] == 0 && self->owner->velocity[Y] == 0 && self->owner->velocity[X] == 0)
		self->think = TeamFortress_RegenerateCyber;

	self->nextthink = time + 2;
}

void TeamFortress_RegenerateCyber()
{
	entity oself;

	if (self->is_abouttodie)
		return;

//WK	if (self.owner.playerclass == #PC_MEDIC)
	if (self->owner->velocity[Z] > 0 || self->owner->velocity[Y] > 0 || self->owner->velocity[X] > 0)
	{
		self->nextthink = time + 2;
		self->think = CyberAugSleep;
		return;
	}

	if (self->owner->cutf_items & PR_CUTF_CYBERAUG && !(self->owner->is_malfunctioning)) //WK Custom class friendly
	{
		self->owner->armortype = self->owner->armor_allowed;
		// Gizmo - whoever did this forgot that W_SetCurrentAmmo only works on self, fixed :)
		oself = self;
		self = self->owner;
		W_SetCurrentAmmo();
		self = oself;

		if (self->owner->health < self->owner->max_health)
		{
			if (self->owner->ammo_medikit == 0)
				self->owner->health = self->owner->health + 1;
			else if (self->owner->ammo_medikit < PR_PC_MEDIC_REGEN_AMOUNT / 2)
			{
				self->owner->health = self->owner->health + self->owner->ammo_medikit * 2;
				self->owner->ammo_medikit = 0;
			}
			else
			{
				self->owner->health = self->owner->health + PR_PC_MEDIC_REGEN_AMOUNT;
				self->owner->ammo_medikit = self->owner->ammo_medikit - PR_PC_MEDIC_REGEN_AMOUNT / 2;
			}
		}

		if (self->owner->armorvalue < self->owner->maxarmor)
		{
			if (self->owner->ammo_cells == 0)
				self->owner->armorvalue = self->owner->armorvalue + 2;
			else if (self->owner->ammo_cells < PR_PC_MEDIC_REGEN_AMOUNT)
			{
				self->owner->armorvalue = self->owner->armorvalue + self->owner->ammo_cells * 2;
				self->owner->ammo_cells = 0;
			}
			else
			{
				self->owner->armorvalue = self->owner->armorvalue + PR_PC_MEDIC_REGEN_AMOUNT * 2;
				self->owner->ammo_cells = self->owner->ammo_cells - PR_PC_MEDIC_REGEN_AMOUNT;
			}
		}

		UpdateCells(self->owner);

		if (self->owner->armorvalue > self->owner->maxarmor)
			self->owner->armorvalue = self->owner->maxarmor;
		if (self->owner->health > self->owner->max_health)
			self->owner->health = self->owner->max_health;
	}
	else if (self->owner->cutf_items & PR_CUTF_CYBERAUG && self->owner->is_malfunctioning == 1)
	{
		if (self->owner->health > 3)
			TF_T_Damage(self->owner, self->owner, self->owner, 3, PR_TF_TD_IGNOREARMOUR, PR_TF_TD_OTHER);
		else if (self->owner->health < 3 && self->owner->health > 1)
			self->owner->health = 1;
		if (self->owner->armorvalue > 3)
			self->owner->armorvalue = self->owner->armorvalue - 3;
		else if (self->owner->armorvalue < 3)
			self->owner->armorvalue = 0;
	}

	self->nextthink = time + PR_PC_MEDIC_REGEN_TIME;

}
//=========================================================================
// Activates when we go hidden
// If we're moving, make us not invis
void TeamFortress_RegenerateCells()
{
	//WK Borrow this code from TF, since there's no reason to
	// duplicate it.
	if (!(self->owner->job & PR_JOB_THIEF))
		dremove(self);
	if (!(self->owner->job & PR_JOB_ACTIVE))
		dremove(self);
	if (self->owner->health <= 0)
		dremove(self);
	self->nextthink = time + PR_PC_SPY_CELL_REGEN_TIME;
	if (self->owner->job & PR_JOB_ACTIVE)
	{
		/*if (self.owner.ammo_cells == 0)
		{
			RevealThief(self.owner,#FALSE);
		}
		else {	*/
			//if (self.owner.ammo_cells > 100) self.owner.ammo_cells = 100;
		if ((self->owner->velocity[Z] > 0 || self->owner->velocity[Y] > 0 || self->owner->velocity[X] > 0) && self->owner->job & PR_JOB_FULL_HIDE)
		{
				self->owner->frame = 0;
				self->owner->weaponframe = 0;
				self->owner->modelindex = modelindex_eyes;
				self->owner->job = self->owner->job - (self->owner->job & PR_JOB_FULL_HIDE);
				TeamFortress_SetSpeed(self->owner);
			/*self.owner.ammo_cells = self.owner.ammo_cells - #PC_SPY_CELL_USAGE;
			if (self.owner.ammo_cells <= 0) {
				self.owner.ammo_cells = 0;
				RevealThief(self.owner,#FALSE);*/

		}
		else if ((self->owner->velocity[Z] == 0 && self->owner->velocity[Y] == 0 && self->owner->velocity[X] == 0) && !(self->owner->job & PR_JOB_FULL_HIDE))
		{
			self->owner->frame = 0;
			self->owner->weaponframe = 0;
			self->owner->modelindex = modelindex_null;
			self->owner->job = self->owner->job | PR_JOB_FULL_HIDE;
		}
	}
	/*else	// Increase cell ammo
	{
		if (self.owner.ammo_cells >= self.owner.maxammo_cells)
			return;

		self.owner.ammo_cells = self.owner.ammo_cells + #PC_SPY_CELL_REGEN_AMOUNT;

		if (self.owner.ammo_cells > self.owner.maxammo_cells)
			self.owner.ammo_cells = self.owner.maxammo_cells;
	}*/
	//return;

	/*
	if (self.owner.playerclass == #PC_SPY)
	{
		self.nextthink = time + #PC_SPY_CELL_REGEN_TIME;

		// If the spy is undercover, decrease the number of cells
		if (self.owner.is_undercover == 1)
		{
			// If the spy has no cells left, he becomes visible again
			if (self.owner.ammo_cells == 0)
			{
				self.owner.is_undercover = 0;
				self.owner.modelindex = modelindex_player;	// return to normal
				self.owner.items = self.owner.items - (self.owner.items & #IT_INVISIBILITY);
			}
			else // Decrease cells
			{
				self.owner.ammo_cells = self.owner.ammo_cells - #PC_SPY_CELL_USAGE;
				if (self.owner.ammo_cells < 0)
					self.owner.ammo_cells = 0;
			}
		}
		else	// Increase cell ammo
		{
			if (self.owner.ammo_cells >= self.owner.maxammo_cells)
				return;

			self.owner.ammo_cells = self.owner.ammo_cells + #PC_SPY_CELL_REGEN_AMOUNT;

			if (self.owner.ammo_cells > self.owner.maxammo_cells)
				self.owner.ammo_cells = self.owner.maxammo_cells;
		}

		return;
	}
	*/
}

//============================================================================
// sends a player to observer mode
void PlayerObserverMode()
{
	DetonateAllGuns();
	kill_my_demons();
	self->current_menu = PR_MENU_DEFAULT;
	self->impulse = 0;
	self->playerclass = PR_PC_UNDEFINED;
	self->lives = 0;
	self->health = 1;
	self->gravity = 0; //WK Make em float so prediction doesn't get all annoying-like
	//if (self.team_no > 0) self.old_team_no = self.team_no; //WK Save old team for scoreboard   // PZ: .old_team_no isn't being used
	self->team_no = -1;

	self->flags = PR_FL_CLIENT | PR_FL_NOTARGET | PR_FL_ONGROUND;
	self->waterlevel = 3;
	self->takedamage = PR_DAMAGE_NO;
	self->solid = PR_SOLID_NOT;
	//self.movetype = #MOVETYPE_NOCLIP;
	self->movetype = PR_MOVETYPE_FLY;

	self->model = string_null;
	self->mdl = string_null;
	setmodel(self, string_null);

	self->velocity = V({0, 0, 0});
	self->avelocity = V({0, 0, 0});

	sprint(self, PR_PRINT_HIGH, "Observer mode\n");
	CenterPrint(self, "\n");
	stuffcmd(self, "color 0; wait; cl_rollangle 0\n");
	setinfo(self, "topcolor", "0");    // PZ: player can no longer use the 'color', 'skin', and 'team' commands
	setinfo(self, "bottomcolor", "0"); // PZ
}

//============================================================================
// Return the crossproduct of 2 vectors
float crossproduct(const vector& veca, const vector& vecb)
{
	float result;

	result = (veca[X] * vecb[Y]) - (vecb[X] * veca[Y]);
	return result;
}

} // END namespace Progs
