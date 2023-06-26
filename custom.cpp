/*======================================================
	CUSTOM.QC			Custom TeamFortress v3.2

	(c) William Kerney			5/21/00
========================================================
All the functions pertaining to custom class generation and
the miscellanious new features of Custom TF
======================================================*/

#include "progs.h"
#include "custom.h"
#include "cutfmenu.h"
#include "menu.h"
#include "ofndefs.h"
#include "debug.h"
#include "combat.h"
#include "tforttm.h"
#include "cpstuff.h"
#include "optimize.h"
#include "spectate.h"
#include "neo.h"

namespace Progs {

void DropToCustomClassGen(); //Called when starting class generation
void DropFromCustomClassGen(); //Called when finished class generation
void PrintMoney();
void PrintRefund(float in);
void PrintNotEnoughMoney(float in);
void BuyHealth( float type);
void BuyWeapon(float cost, int item);
void BuyCuTF(float cost, int item);
void BuyItem(float cost, int item);
void BuyJob(float cost, float type);
/*void(float cost, float type) BuyGren1;
void(float cost, float type) BuyGren2;*/
void RevealThief(entity tif, float pain);
void FragGrenadeTouch();
void FragGrenadeExplode();
void KracGrenadeTouch();
void KracGrenadeExplode();
void createBastard(entity bastard,float threshold);
void makeImmune(entity immuner,float timer);
void UseJobSkill(); //Function for handling professions
void DetonateAllGuns();
/*void (string temp) DebugSprint;
void (float temp)  DebugSprintFloat;*/
float IsBuilding(entity tester);
void autoteam_think();
//Extern
void spawn_tdeath(const vector& org, entity death_owner);
//void (string gib, float health) ThrowGib;
void ThrowGib(const string& gibname, float dm, float makecool, float gibskin, float gibkgs, float randorg);
void TeamFortress_SetHealth();
void TeamFortress_SetEquipment();
float W_BestWeapon();
void W_SetCurrentAmmo();
void TeamFortress_SetSpeed(entity p);
void TeamFortress_SetSkin(entity p);
void TeamFortress_PrintJobName(entity Viewer, float pc);
void BecomeExplosion();
void T_RadiusDamage(entity bomb, entity attacker, float rad, entity ignore);
void SUB_regen();
//float modelindex_eyes, modelindex_player, modelindex_null;
void W_FireMedikit(float inAuto);
void TeamFortress_ID(float inAuto);
void TeamFortress_Scan(float range,float inAuto);
void kill_my_demons();
void player_assaultcannondown1();
void Reset_Grapple(entity rhook);
void GuerillaExplode();
void TeamFortress_DetpackStop(float krac);
void TeamFortress_TeamShowScores(float all, float teamscored, float scorepoints);
void execute_changelevel();
void tfgoalitem_RemoveFromPlayer(entity Item, entity AP, float method);

//- OfN
void DetonateMines(entity mine_owner); //external, job.qc
string TeamGetNiceColor(float tno);
void SetArmyTimer(entity player, float start);
void RemoveArmyTimer();
entity SelectSpawnPoint();
void CleanUpEverything();
//void(entity tfield, vector where, entity thing) FieldExplosion;
void FieldEvent(entity tfield, const vector& where, entity thing);
float GiveFreeStuff(entity player);

void SetMeatUsage(entity player, float usage);
void SetHeadsUsage(entity player, float usage);
void SetCurrentSoul(entity player, float soul);

void PlayerDropRunes(entity player);
void ResetStripedAndGiven(entity player);

//KK: are team_no's current teammates
// PZ: DELETE THIS. PREVIOUS VERSION.
/*float Teammate(float targteam, float attackteam)
{
	int teammask;

	if ( !targteam ) return PR_FALSE;
	if ( targteam == attackteam ) return PR_TRUE;
	if ( number_of_teams < 3 ) return PR_FALSE;

	if      ( targteam == 1 ) teammask = 1;
	else if ( targteam == 2 ) teammask = 2;
	else if ( targteam == 3 ) teammask = 4;
	else if ( targteam == 4 ) teammask = 8;
	else                      teammask = 0;   // Gizmo - only happens if there's a bug

	if      ( attackteam == 1 && (friends1_mask & teammask)) return PR_TRUE;
	else if ( attackteam == 2 && (friends2_mask & teammask)) return PR_TRUE;
	else if ( attackteam == 3 && (friends3_mask & teammask)) return PR_TRUE;
	else if ( attackteam == 4 && (friends4_mask & teammask)) return PR_TRUE;

	return PR_FALSE;
}*/

// PZ: Permanent server-side entities for the purpose of comparing entities.
entity a_G = world; // FIXME: `world` isn't set to anything yet when this is instantiated and initialized.
entity b_G = world; //        I guard against this, below, by checking for nullptr, as well.

// KK: Are team_no's current teammates?
// PZ: Made it so this function receives entities rather than team numbers, for greater flexibility.
//     Needed now for Neo mode. This also helps with supporting teamplay 0 for deathmatch levels.
// TODO DONE?: What should happen when a and/or b are fake entities? It seems to only matter when team_no isn't what is being considered.
bool Teammate(entity a, entity b, bool aIsFakeEnt/* = false*/, bool bIsFakeEnt/* = false*/)
{
	if (a == world || b == world) return false;   // PZ: I added this.
	if (a == b)                   return true;    // PZ: I added this.

	// Neo mode
	//if (neo.isModeActive()) return neo.teammate(a, b);
	// When Neo doesn't exist, no one is a teammate, except "family members" (basically objects and their owners).
	if (neo.isModeActive() && neo.getNeo() == world)
		return areFamilyMembers(a, b);

	if (!aIsFakeEnt && !bIsFakeEnt) // NOTE: areFamilyMembers() looks at owners, which 'fake' entities don't have.
	{
		if (areFamilyMembers(a, b)) return true;  // PZ: I added this for teamplay 0 and Neo mode.
	}

	if (a->team_no == 0)          return false;
	if (a->team_no == b->team_no) return true;
	if (number_of_teams < 3)      return false;

	// NOTE: The rest of this is for when there are more than 2 teams.
	int teammask;
	if      (a->team_no == 1) teammask = 1;
	else if (a->team_no == 2) teammask = 2;
	else if (a->team_no == 3) teammask = 4;
	else if (a->team_no == 4) teammask = 8;

	if      (b->team_no == 1 && (friends1_mask & teammask)) return true;
	else if (b->team_no == 2 && (friends2_mask & teammask)) return true;
	else if (b->team_no == 3 && (friends3_mask & teammask)) return true;
	else if (b->team_no == 4 && (friends4_mask & teammask)) return true;

	return false;
}
// PZ: Overloaded Teammate() to allow team number arguments.
bool Teammate(float num, entity b)
{
	if (a_G == world || a_G == nullptr)
	{
		a_G = spawnServerSide();
		a_G->classname = "Teammate() entity 'a'";
		a_G->real_owner = world;
	}
	a_G->team_no = num;
	return Teammate(a_G, b, true, false);
}
// PZ: Overloaded Teammate() to allow team number arguments.
bool Teammate(entity a, float num)
{
	if (b_G == world || b_G == nullptr)
	{
		b_G = spawnServerSide();
		b_G->classname = "Teammate() entity 'b'";
		b_G->real_owner = world;
	}
	b_G->team_no = num;
	return Teammate(a, b_G, false, true);
}
// PZ: Overloaded Teammate() to allow team number arguments.
bool Teammate(float numA, float numB)
{
	if (a_G == world || a_G == nullptr)
	{
		a_G = spawnServerSide();
		a_G->classname = "Teammate() entity 'a'";
		a_G->real_owner = world;
	}
	if (b_G == world || b_G == nullptr)
	{
		b_G = spawnServerSide();
		b_G->classname = "Teammate() entity 'b'";
		b_G->real_owner = world;
	}
	a_G->team_no = numA;
	b_G->team_no = numB;
	return Teammate(a_G, b_G, true, true);
}
// PZ: For deathmatch (teamplay 0), added areFamilyMembers(), so that it could be used to test whether players/things
// are friendly to each other, despite there not being any teams. Things should be friendly with their owners and with
// other things owned by their owners. This will not test for teammates. Use Teammate(), above, for that.
bool areFamilyMembers(entity a, entity b)
{
	if (a == world || b == world) return false;

	if (a == b)                                                   return true; // sure, whatever
	if (a->real_owner == b)                                       return true; // child to parent
	if (a == b->real_owner)                                       return true; // parent to child
	if (a->real_owner == b->real_owner && a->real_owner != world) return true; // siblings

	return false;
}

/*
void (string temp) DebugSprint =
{
	sprint(self,#PRINT_HIGH,temp);
	sprint(self,#PRINT_HIGH,"\n");
};

void (float foo)  DebugSprintFloat =
{
	local string temp;
	temp = ftos(foo);
	sprint(self,#PRINT_HIGH,temp);
	sprint(self,#PRINT_HIGH,"\n");
};
*/

/*
==============
BuyStockClassItems    // PZ NOTE: Looks like Gizmo started working on this, but it's clearly unfinished.

Used to buy all the items of a stock class, if we
have enough money. This way we can start from an
already built class and make a few changes.
==============
*/
void BuyStockClassItems( float stockClass) {
	if ( stockClass == PR_PC_SCOUT ) {
		BuyHealth( 1 );
		if ( self->custom_speed == PR_CUSTOM_SPEED7 && self->money >= PR_COST_SPEED1 ) {
			self->money = self->money - PR_COST_SPEED1;
			self->custom_speed = PR_CUSTOM_SPEED1; // Original 650
			PrintMoney();
		}
		BuyWeapon( PR_COST_SHOTGUN, PR_WEAP_SHOTGUN );
		BuyWeapon( PR_COST_NAILGUN, PR_WEAP_NAILGUN );
		BuyWeapon( PR_COST_AIRF, PR_WEAP_AIRF );
		BuyGren( PR_COST_GR_CALTROP, PR_GR_TYPE_CALTROP );
		BuyGren( PR_COST_GR_CONCUSSION, PR_GR_TYPE_CONCUSSION );
		BuyItem( PR_COST_SECURITYCAMERA, PR_NIT_SECURITY_CAMERA );
		BuyJob( PR_COST_THIEF, PR_JOB_THIEF );
	}
}

// PZ NOTE: This is called when entering the custom menu.
void DropToCustomClassGen()
{
	float previouslyCustom; previouslyCustom = PR_FALSE; // PZ: added
	//local entity te;
	float sell_no;
	//local string st;
	sell_no = 0;

/*local string strtemp;
strtemp = ftos(self.playerclass);
bprint(2, ".playerclass = ", strtemp, "\n");
strtemp = ftos(self.done_custom);
bprint(2, "1) .done_custom = ", strtemp, "\n");*/
	// PZ: Allowed editing custom class. If we already have a custom class, first entry into class building puts us in the class editing mode, which
	//     keeps our previous selections, allowing us to simply edit them. Typing 'custom' again while in the custom menu clears out all selections.
	if (self->done_custom & PR_CUSTOM_FINISHED) previouslyCustom = PR_TRUE;
	if (self->done_custom == 0 || self->done_custom & PR_CUSTOM_BUILDING)
		self->done_custom = self->done_custom - (self->done_custom & PR_CUSTOM_EDITING);
	else
		self->done_custom = self->done_custom | PR_CUSTOM_EDITING;

/*strtemp = ftos(self.done_custom);
bprint(2, "2) .done_custom = ", strtemp, "\n");*/

	self->gravity = 1;
	self->aura = 0;

	//Remove everything he's carrying
	if (!(self->done_custom & PR_CUSTOM_EDITING)) // PZ: When you type 'custom'/'upgrade', it keeps your current class build, allowing you to make edits to it.
		self->job = 0;

	// This is the appropiate place to: (after clearing job due to armytimer stuff weirdness)
	CleanUpEverything();

	ResetStripedAndGiven(self);

	self->is_malfunctioning = 0;
	//self.is_cameraviewing = 0;

#ifdef PR_COOP_MODE_ENHANCED
	if (!(self->done_custom & PR_CUSTOM_EDITING)) // PZ: When you type 'custom'/'upgrade', it keeps your current class build, allowing you to make edits to it.
	{
		if ( !deathmatch )
			self->tf_items = PR_NIT_AMMO_BANDOLIER;
		else
			self->tf_items = 0;
	}
#else
	if (!(self->done_custom & PR_CUSTOM_EDITING)) // PZ: When you type 'custom'/'upgrade', it keeps your current class build, allowing you to make edits to it.
		self->tf_items = 0;
#endif
#ifdef PR_COOP_MODE_ENHANCED
	if (!(self->done_custom & PR_CUSTOM_EDITING)) // PZ: When you type 'custom'/'upgrade', it keeps your current class build, allowing you to make edits to it.
	{
		if (!deathmatch || normalQuakeMap)      // PZ: added `normalQuakeMap`
			self->cutf_items = PR_CUTF_FULLARMOUR;
		else
			self->cutf_items = 0;
	}
#else
	if (!(self->done_custom & PR_CUSTOM_EDITING)) // PZ: When you type 'custom'/'upgrade', it keeps your current class build, allowing you to make edits to it.
		self->cutf_items = 0;
#endif
	// Gizmo - new item flags
	if (!(self->done_custom & PR_CUSTOM_EDITING)) // PZ: When you type 'custom'/'upgrade', it keeps your current class build, allowing you to make edits to it.
	{
		self->cutf_moreitems = 0;
		self->tf_items_flags = 0;
		self->items = 0;
		self->ammo_rockets = 0;
		self->ammo_nails = 0;
		self->ammo_shells = 0;
		self->ammo_cells = 0;
		self->maxammo_rockets = 10; //Give em a minimal carrying capacity
		self->maxammo_nails = 10;
		self->maxammo_shells = 10;
		self->maxammo_cells = 10;
		self->maxammo_medikit = 0;
		self->maxammo_detpack = 0;
		self->ammo_c4det = 0;

		self->demon_one = world; //SB - this tells us who our demon is for easy reference
		self->demon_two = world; //- OfN-  Used for hacker job, target building and for timer (% on sbar)
		self->demon_three = world;

		self->demon_blood = 0;

		self->all_active = 0; // OfN Number of mines set by the player

		self->no_grenades_1 = 0;
		self->no_grenades_2 = 0;
		self->tp_grenades_1 = 0;
		self->tp_grenades_2 = 0;

		self->armor_allowed = 0.3; //Red yellow or green
		self->armorclass = 0; //Red yellow or green
		self->armorvalue = 0; //200 etc
		self->maxarmor = 0;	//200 etc
		self->weapon = 0; // #WEAP_HOOK & #WEAP_SHOTGUN etc

		if ((allow_hook) && (no_grapple != 1)) //If map allows it...
			self->weapons_carried = PR_WEAP_HOOK; // Start with grapple
		else
			self->weapons_carried = 0; // Or remove everything
	}
	else // PZ: we're editing
	{
		//self.armorvalue = self.maxarmor;
	}

	self->current_weapon = 0; // No current weapon
	self->weaponmodel = ""; //Remove the model of a weapon in front

	self->solid = PR_SOLID_NOT;
	self->movetype = PR_MOVETYPE_NONE;
	self->takedamage = PR_DAMAGE_NO;

	// Ready to buy
	if (!(self->done_custom & PR_CUSTOM_EDITING)) // PZ: When you type 'custom'/'upgrade', it keeps your current class build, allowing you to make edits to it.
	{
		self->max_health = 50;
		self->health = 50;

		GiveFreeStuff(self);
	}
	else // PZ: we're editing
	{
		float currentSelection;
		currentSelection = GetCurrentHPItem();
		if (currentSelection > 0) // we have health and armor that matches one of the custom options
		{
			self->health = self->max_health;
			self->armorvalue = self->maxarmor;
			self->armortype = self->armor_allowed;
		}
		else // our previous class didn't match one of the custom options
		{
			// Change the actual values, but hold on to the max values, so that we can remember what they were.
			self->health = 50;
			self->armorvalue = 0;
			self->armortype = 0;
			UpdateArmorItem(self);   // This probably updates the armor graphic displayed.
		}
	}

	// PZ: Remember their previous maxspeed in another variable. .worldtype should be good for that.
	self->worldtype = self->maxspeed;
	self->maxspeed = 0; // Can't move while buying
	// Determine their custom class speed      // PZ: I had to move this section above the money section, below. This section was previously right below the money section.
	if (!(self->done_custom & PR_CUSTOM_EDITING)) // PZ: When you type 'custom'/'upgrade', it keeps your current class build, allowing you to make edits to it.
		self->custom_speed = 250;               // HWGUY Speed
	// PZ: When editing, we just keep the previous custom_speed. But if we were stock class before, we want to restore the stock class's speed,
	//     if the class had a speed equal to a custom speed.
	else if (!previouslyCustom)
	{
		float legs;
		legs = GetPreviousLegs();
		if (legs == 0 || ((legs - floor(legs)) != 0)) // This means our previous maxspeed wasn't one of the custom class speed options.
			self->custom_speed = 250;                  // So, we set the .custom_speed to 250, like above, which is the default/slowest custom speed.
		else
			self->custom_speed = self->worldtype;       // The stock speed was one of the custom speed options. So use that option.
	}

	#define PR_PRICE_PER_FRAG    100
	#define PR_MAX_FRAGS_TO_SELL  50
	// Calculate starting money
	// This means the most someone can get is 50*100 = $5000   // PZ NOTE: 'upgrade' command
	if (self->done_custom & PR_CUSTOM_SELLING)
	{
		if (self->real_frags >= PR_MAX_FRAGS_TO_SELL)
			sell_no = PR_MAX_FRAGS_TO_SELL;
		else if (self->real_frags > 0)
			sell_no = self->real_frags;
		else {
			sell_no = 0;
			self->done_custom = self->done_custom - (self->done_custom & PR_CUSTOM_SELLING);
		}

		self->real_frags = self->real_frags - sell_no;
		self->frags = self->real_frags; //TODO: Make this work with TEAMFRAGS on
		if (!(self->done_custom & PR_CUSTOM_EDITING)) // PZ: When you type 'custom'/'upgrade', it keeps your current class build, allowing you to make edits to it.
			self->money = custom_money + (sell_no * PR_PRICE_PER_FRAG);
		else // PZ: for editing
		{
			if (previouslyCustom)
				self->money = self->money + (sell_no * PR_PRICE_PER_FRAG);
			else  // previously stock class; so we need to calculate how much that class costs
				self->money = custom_money - getTotalSpent() + (sell_no * PR_PRICE_PER_FRAG);
		}
	}
	// Not upgrading/selling. Normal 'custom' menu.
	else  // PZ NOTE: We use the previous .money value if they were previously a custom class. (i.e., we don't change .money)
	{
//bprint(2, "Reached this spot.\n");
		if (!(self->done_custom & PR_CUSTOM_EDITING)) // PZ: When you type 'custom'/'upgrade', it keeps your current class build, allowing you to make edits to it.
		{
//bprint(2, "Executed path 1.\n");
			self->money = custom_money;
		}
		else if (!previouslyCustom)                // PZ: If they weren't a custom class when they went into the custom menu, we will need to total up their items' costs, if editing.
		{
//bprint(2, "Executed path 2.\n");
			self->money = custom_money - getTotalSpent();
//local string tempstr;
//tempstr = ftos(self.money);
//bprint(2, "self.money = ", tempstr, "\n");
		}
	}

	//Overrides the menu handler to always display my menu
	//Hack alert! This preserves the selling flag. :p
	if (self->done_custom & PR_CUSTOM_SELLING)
		self->done_custom = PR_CUSTOM_BUILDING | PR_CUSTOM_SELLING | (self->done_custom & PR_CUSTOM_EDITING); // PZ: keep editing flag, if set (added from "|" on);
	else
		self->done_custom = PR_CUSTOM_BUILDING | (self->done_custom & PR_CUSTOM_EDITING); // PZ: keep editing flag, if set (added from "|" on)

	//Destroy Buildings... again. :p
	//DetonateAllGuns();

	//And then kill all demons I own

	//kill_my_demon();
	//DetonateMines(self);
	//RemoveArmyTimer();

	self->current_menu = PR_MENU_PRIMARY_WEAPON;
	Menu_PrimaryWeapon();
	//return;
}

void spawn_tfog(const vector& org);

// PZ NOTE: This is called when the player finishes customizing.
void DropFromCustomClassGen()
{
	// OfN
	self->is_killed = PR_FALSE;

	self->done_custom = PR_CUSTOM_FINISHED;
	self->maxspeed = self->custom_speed;
	self->gravity = 1;

	if (self->cutf_items & PR_CUTF_TOSSABLEDET)
		self->maxammo_detpack = 1;

	//Make sure they have at least one axe-like weapon
	if (!(self->weapons_carried & (PR_WEAP_MEDIKIT | PR_WEAP_SPANNER)) || (self->cutf_items & PR_CUTF_KNIFE))
		self->weapons_carried = self->weapons_carried | PR_WEAP_AXE;

	//CH if you bought Guerilla, you get 20 rockets :)
	if (self->job & PR_JOB_GUERILLA)
		self->maxammo_rockets = self->maxammo_rockets + 20;

	//OfN if we got the holo we get extra cells ammo space
	if (self->cutf_items & PR_CUTF_HOLO)
		self->maxammo_cells = self->maxammo_cells + 40;

	//If they bought weapons, remove their basic ammo allotment
	if (self->maxammo_rockets > 10)
		self->maxammo_rockets = self->maxammo_rockets - 10;
	  if (self->maxammo_nails > 10)
		self->maxammo_nails = self->maxammo_nails - 10;
	  if (self->maxammo_shells > 10)
		self->maxammo_shells = self->maxammo_shells - 10;
	  if (self->maxammo_cells > 10)
		self->maxammo_cells = self->maxammo_cells - 10;

	//Increase their supply if they bought a backpack or bandolier
	if (self->tf_items & PR_NIT_AMMO_BACKPACK) {
		self->maxammo_rockets = self->maxammo_rockets + 30;
		self->maxammo_nails	 = self->maxammo_nails	+ 100;
		self->maxammo_shells  = self->maxammo_shells	+ 100;
		if (self->weapons_carried & PR_WEAP_SPANNER)
			self->maxammo_cells	 = self->maxammo_cells	+ 170;
		else
			self->maxammo_cells	 = self->maxammo_cells	+ 50;
		if (self->maxammo_medikit > 0) //Only increase their medical supplies when...
			self->maxammo_medikit = self->maxammo_medikit + 70;
		if (self->maxammo_detpack > 0)
			self->maxammo_detpack = self->maxammo_detpack + 0.5;
	}
	if (self->tf_items & PR_NIT_AMMO_BANDOLIER) {
		self->maxammo_rockets = self->maxammo_rockets + 10;
		self->maxammo_nails	 = self->maxammo_nails	+ 50;
		self->maxammo_shells  = self->maxammo_shells	+ 50;
		self->maxammo_cells	 = self->maxammo_cells	+ 30;
		if (self->maxammo_medikit > 0)
			self->maxammo_medikit = self->maxammo_medikit + 50;
		if (self->maxammo_detpack > 1) //Only give extra det if they bought both
			self->maxammo_detpack = 2;
	}

	//Clean up detpack fraction
	if (self->maxammo_detpack > 1 && self->maxammo_detpack < 2)
		self->maxammo_detpack = 1;

	if (self->cutf_items & PR_CUTF_SENSOR) // SB
		self->maxammo_cells = self->maxammo_cells + 70;
	if (self->cutf_items & PR_CUTF_DISPENSER) // SB
		self->maxammo_cells = self->maxammo_cells + 100;
	if (self->cutf_items & PR_CUTF_FIELDGEN) // SB
		self->maxammo_cells = self->maxammo_cells + 140;

	//- OfN
	if (self->weapons_carried & PR_WEAP_LASERCANNON) self->maxammo_cells = self->maxammo_cells + 70;
	if (self->weapons_carried & PR_WEAP_SNG) self->maxammo_nails = self->maxammo_nails + 70;
	if (self->weapons_carried & PR_WEAP_AIRF) self->maxammo_cells = self->maxammo_cells + 60;

	//Now cap the max ammo
	if (self->maxammo_rockets > 100)
		self->maxammo_rockets = 100;
	  if (self->maxammo_nails > 350)
		self->maxammo_nails = 350;
	  if (self->maxammo_shells > 300)
		self->maxammo_shells = 300;
	  if (self->maxammo_cells > PR_MAXAMMO_CELLS)
		self->maxammo_cells = PR_MAXAMMO_CELLS;

/* WK 1/7/7 We don't need all this complication
	  //- ofn
	  if (self.tf_items & #NIT_AMMO_BANDOLIER && self.tf_items & #NIT_AMMO_BACKPACK)
	  {
		if (self.maxammo_cells > #MAXAMMO_CELLS + 100)
			self.maxammo_cells = #MAXAMMO_CELLS + 100;
	  }
	  else if (self.tf_items & #NIT_AMMO_BACKPACK)
	  {
		if (self.maxammo_cells > #MAXAMMO_CELLS + 50) // was 360
			self.maxammo_cells = #MAXAMMO_CELLS + 50;
	  }
	  else
	  {
		if (self.maxammo_cells > #MAXAMMO_CELLS) // was 320
			self.maxammo_cells = #MAXAMMO_CELLS;
	  }
*/

	//- OFN HERE FRONTLIN HELI BUG FIX ---//
	entity spot;
	spot = SelectSpawnPoint ();

	self->observer_list = spot;
	self->origin = spot->origin + V({0, 0, 1});
	self->angles = spot->angles;
	self->fixangle = PR_TRUE;		// turn this way immediately

	makevectors(self->angles);
	spawn_tfog (self->origin + v_forward*20);
	//------------------------------------//

	//Telefrag anyone sitting on top of us
	spawn_tdeath(self->origin,self);

	//Remove observer-like mode
	self->takedamage = PR_DAMAGE_AIM;
	//Prevent bad movetype errors, or something
	if (self->classname != "player") {
		RPrint("BUG BUG BUG BUG BUG BUG BUG BUG BUG\n");
		RPrint("Non-player was in custom class generation!\n");
		return;
	}
	self->movetype = PR_MOVETYPE_WALK;
	self->flags = PR_FL_CLIENT | PR_FL_ONGROUND;
	self->waterlevel = 0;
	self->air_finished = time + 12;
	self->solid = PR_SOLID_SLIDEBOX;
	self->pausetime = 0;

	//Remove invisibility
	//setmodel (self, "progs/eyes.mdl");
	//setmodel (self, "progs/player.mdl");
	self->modelindex = modelindex_player;

	// Stock him up
	// Display the Player's Class
	TeamFortress_PrintJobName(self,self->job);
	// Set the weapons and ammo for the player based on class
	TeamFortress_SetEquipment();
	// Set the health for the player based on class
	TeamFortress_SetHealth();
	// Set the speed for the player based on class
	TeamFortress_SetSpeed(self);
	// Set the skin for the player based on class
	TeamFortress_SetSkin(self);
	stuffcmd(self, "v_idlescale 0\n");
	stuffcmd(self, "v_cshift 0 0 0 0\n");
	stuffcmd(self, "bf\n");

	ResetMenu();
	self->impulse = 0;

	//Make them invincible if they bought respawn protection
	if (self->tf_items & PR_NIT_RESPAWN_GUARD && !normalQuakeMap) { // PZ: respawn guard is annoying as hell on DM maps
		self->items = self->items + PR_IT_INVULNERABILITY;
		self->invincible_time = 1;
#ifdef PR_COOP_MODE_ENHANCED
		if ( !deathmatch )
			self->invincible_finished = time + PR_COOP_RESPAWN_GUARD_TIME;
		else
#endif
		self->invincible_finished = time + PR_RESPAWN_GUARD_TIME;
		if (self->custom_speed > 300)
			self->invincible_finished = self->invincible_finished - 1;
		if (self->custom_speed > 400)
			self->invincible_finished = self->invincible_finished - 1;
	}

	//Come again!
	sprint(self,PR_PRINT_HIGH,S_("Type ^b\"^bcustom^b\"^b at the console to build a new class.\n"));

	// PZ: display the class help on spawn
	if (PR_DISPLAY_CLASS_HELP)
		self->current_menu = PR_MENU_CLASSHELP;
	self->menu_displaytime = 0;
	// PZ: END

	return;
}

void PrintMoney()
{
	CuTFMenuSound(PR_MENUSOUND_BUY);

	if (pay_msgs != 1)
		return;

	string as;
	sprint(self,PR_PRINT_HIGH,"You have ");
	as = ftos(self->money);
	sprint(self,PR_PRINT_HIGH,as);
	sprint(self,PR_PRINT_HIGH," dollars left.\n");
	return;
}


void PrintRefund(float in)
{
	CuTFMenuSound(PR_MENUSOUND_SELL);

	string as;

	if (pay_msgs == 1)
	{
		sprint(self,PR_PRINT_HIGH,"You sell it back for ");
		as = ftos(in);
		sprint(self,PR_PRINT_HIGH,as);
		sprint(self,PR_PRINT_HIGH," dollars.\n");
	}

	self->money = self->money + in; //Give em a refund

	if (pay_msgs == 1)
	{
		sprint(self,PR_PRINT_HIGH,"You now have ");
		as = ftos(self->money);
		sprint(self,PR_PRINT_HIGH,as);
		sprint(self,PR_PRINT_HIGH," dollars available.\n");
	}

	return;
}

void PrintNotEnoughMoney(float cost)
{
	CuTFMenuSound(PR_MENUSOUND_WRONG);

	self->tfstate = self->tfstate | PR_TFSTATE_NOTENOUGHMONEY;

	if (pay_msgs != 1)
	{
		sprint(self,PR_PRINT_HIGH,"Not enough money!\n");
		return;
	}
	// PZ: added this condition, so that I can use this function to stop them from finishing customizing when their money is negative
	if (cost > 0)
	{
		string as;

		sprint(self,PR_PRINT_HIGH,"Sorry, that costs ");
		as = ftos(cost);
		sprint(self,PR_PRINT_HIGH,as);
		sprint(self,PR_PRINT_HIGH," dollars.\n");

		sprint(self,PR_PRINT_HIGH,"You only have ");
		as = ftos(self->money);
		sprint(self,PR_PRINT_HIGH,as);
		sprint(self,PR_PRINT_HIGH," dollars left.\n");
	}
}

//Functions for determining ammo carrying capacity
float countshells(float item)
{
/*	if (chris)
	{
		if (item == #WEAP_SHOTGUN) return 16;
		if (item == #WEAP_SPANNER) return 20;
		if (item == #WEAP_SUPER_SHOTGUN) return 32;
		if (item == #WEAP_ASSAULT_CANNON) return 75;
	}
	else
	{*/
		if (item == PR_WEAP_SHOTGUN) return 25;
		if (item == PR_WEAP_SPANNER) return 40;
		if (item == PR_WEAP_SUPER_SHOTGUN) return 50;
		if (item == PR_WEAP_ASSAULT_CANNON) return 100;
	//}
	return 0;
}
float countnails(float item)
{
	/*if (chris)
	{
		if (item == #WEAP_MAUSER) return 10;
		if (item == #WEAP_TRANQ) return 20;
		if (item == #WEAP_LASER) return 10;
		if (item == #WEAP_NAILGUN) return 75;
		if (item == #WEAP_LIGHT_ASSAULT) return 100;
	}
	else
	{*/
		if (item == PR_WEAP_MAUSER) return 30;
		if (item == PR_WEAP_TRANQ) return 30;
		if (item == PR_WEAP_RAILGUN) return 50;
		if (item == PR_WEAP_NAILGUN) return 100;
		if (item == PR_WEAP_LIGHT_ASSAULT) return 150;
	//}
	return 0;
}
float countrockets(float item)
{
	/*if (chris)
	{
		if (item == #WEAP_SPANNER) return 2;
		if (item == #WEAP_ROCKET_LAUNCHER) return 3;
		if (item == #WEAP_GRENADE_LAUNCHER) return 7;
	}
	else
	{*/
		if (item == PR_WEAP_SPANNER) return 30;
		if (item == PR_WEAP_ROCKET_LAUNCHER) return 30;
		if (item == PR_WEAP_GRENADE_LAUNCHER) return 50;
	//}
	return 0;
}
float countcells(float item)
{
	/*if (chris)
	{
		if (item == #WEAP_ASSAULT_CANNON) return 16;
		if (item == #WEAP_DAEDALUS) return 50;
		if (item == #WEAP_LIGHTNING) return 75;
		if (item == #WEAP_SPANNER) return 150;
	}
	else
	{*/
		if (item == PR_WEAP_ASSAULT_CANNON) return 50;
		if (item == PR_WEAP_DAEDALUS) return 100;
		if (item == PR_WEAP_LIGHTNING) return 100;
		if (item == PR_WEAP_SPANNER) return 150;
	//}
	return 0;
}

#ifdef PR_NEVER_DEFINED
float GetHPCost(float numitem)
{
	if (numitem == 1)
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
#endif

/*
==============
BuyHealth       // PZ NOTE: This is only used for BuyStockClassItems(), which isn't finished. At some point, this
                //          function should use the new defines for health and armor (see top of cutfmenu.qc).
Gizmo - so we don't have to duplicate code when buying
health and armor.
==============
*/
void BuyHealth( float type) {
	float	current;
	float	tosell, tobuy;

	if ( !type )
		return;

	if (self->maxarmor == 50)
	{
		if (self->health == 75)
			current = 1;
		else
			current = 2;
	}
	else if (self->maxarmor == 100)
		current = 3;
	else if (self->maxarmor == 120)
		current = 4;
	else if (self->maxarmor == 150)
		current = 5;
	else if (self->maxarmor == 200)
		current = 6;
	else if (self->maxarmor == 300)
		current = 7;
	else
		current = 0;	// Gizmo - only set to zero if all others fail

	tosell = GetHPCost(current);
	tobuy = GetHPCost(type);
	if (self->money < tobuy - tosell)
	{
		PrintNotEnoughMoney(tobuy - tosell);
		return;
	}

	if ( current == type || current ) { // sell back?
		self->max_health = 50;
		self->health = self->max_health;
		self->maxarmor = 0;
		self->armorvalue = 0;
		self->armortype = 0;
		self->armor_allowed = 0;

		PrintRefund(tosell);
		UpdateArmorItem(self);

		if (current == type)
			return;
	}

	if (type == 1)
	{
		self->max_health = 75;
		self->health = self->max_health;
		self->maxarmor = 50;
		self->armorvalue = self->maxarmor;
		self->armortype = 0.3;	// Green
		self->armor_allowed = 0.3; //Green max
		UpdateArmorItem(self);
	}
	else if (type == 2)
	{
		self->max_health = 90;
		self->health = self->max_health;
		self->maxarmor = 50;
		self->armorvalue = self->maxarmor;
		self->armortype = 0.3;	// Green
		self->armor_allowed = 0.3; //Green max
		UpdateArmorItem(self);
	}
	else if (type == 3)
	{
		self->max_health = 80;
		self->health = self->max_health;
		self->maxarmor = 100;
		self->armorvalue = self->maxarmor;
		self->armortype = 0.6;
		self->armor_allowed = 0.6;
		UpdateArmorItem(self);
	}
	else if (type == 4)
	{
		self->max_health = 90;
		self->health = self->max_health;
		self->maxarmor = 120;
		self->armorvalue = self->maxarmor;
		self->armortype = 0.6;
		self->armor_allowed = 0.6;
		UpdateArmorItem(self);
	}
	else if (type == 5)
	{
		self->max_health = 100;
		self->health = self->max_health;
		self->maxarmor = 150;
		self->armorvalue = self->maxarmor;
		self->armortype = 0.6;
		self->armor_allowed = 0.6;
		UpdateArmorItem(self);
	}
	else if (type == 6)
	{
		self->max_health = 100;
		self->health = self->max_health;
		self->maxarmor = 200;
		self->armorvalue = self->maxarmor;
		self->armortype = 0.8;
		self->armor_allowed = 0.8;
		UpdateArmorItem(self);
	}
	else if (type == 7)
	{
		self->max_health = 100;
		self->health = self->max_health;
		self->maxarmor = 300;
		self->armorvalue = self->maxarmor;
		self->armortype = 0.8;
		self->armor_allowed = 0.8;
		UpdateArmorItem(self);
	}

	self->money = self->money - tobuy;
	PrintMoney();
}

void BuyWeapon(float cost, int item)
{
	if (item == 0) //Make sure we don't buy a null weapon
		return;

	if (self->weapons_carried & item) //We are selling
	{
		// Gizmo - changed this to just one rocket launcher check
		if ( item == PR_WEAP_ROCKET_LAUNCHER ) {
			if ( self->tf_items & PR_NIT_RL_LASER_SIGHT ) {
				self->money = self->money + PR_COST_RLASERGUIDED;
				self->tf_items = self->tf_items - PR_NIT_RL_LASER_SIGHT;
			}
			if ( self->tf_items & PR_NIT_CLUSTER_ROCKETS ) {
				self->money = self->money + PR_COST_CLUSTER;
				self->tf_items = self->tf_items - PR_NIT_CLUSTER_ROCKETS;
			}
			if ( self->cutf_moreitems & PR_CUTF_MI_FASTERROCKETS ) {
				self->money = self->money + PR_COST_FASTERROCKETS;
				self->cutf_moreitems = self->cutf_moreitems - PR_CUTF_MI_FASTERROCKETS;
			}
		}
		self->maxammo_shells = self->maxammo_shells - countshells(item);
		self->maxammo_nails = self->maxammo_nails - countnails(item);
		self->maxammo_rockets = self->maxammo_rockets - countrockets(item);
		self->maxammo_cells = self->maxammo_cells - countcells(item);
		PrintRefund(cost);
		self->weapons_carried = self->weapons_carried - item;
		return;
	}
	if (cost > self->money)
	{
		PrintNotEnoughMoney(cost);
		return;
	}
	//Add to max ammo
	self->maxammo_shells = self->maxammo_shells + countshells(item);
	self->maxammo_nails = self->maxammo_nails + countnails(item);
	self->maxammo_rockets = self->maxammo_rockets + countrockets(item);
	self->maxammo_cells = self->maxammo_cells + countcells(item);

	self->weapons_carried = self->weapons_carried | item;
	self->money = self->money - cost;
	PrintMoney();
}


void BuyCuTF(float cost, int item)
{
	if (item == 0) //Make sure we don't buy a null weapon
		return;

	if (item == PR_CUTF_HOLO && !(self->cutf_items & PR_CUTF_HOLO)) {
		if (!(cost > self->money))
			sprint(self,PR_PRINT_HIGH,"Use the 'holo' command to activate/deactivate the holograph\n");
	}

	if (self->cutf_items & item) //We are selling
	{
		if (item & PR_CUTF_SENTRYGUN) {
			self->maxammo_shells = self->maxammo_shells - 130;
			self->maxammo_cells  = self->maxammo_cells  - 130;
		}
		PrintRefund(cost);
		self->cutf_items = self->cutf_items - item;
		return;
	}
	if (cost > self->money)
	{
		PrintNotEnoughMoney(cost);
		return;
	}
	//Add to max ammo if sentrygun purchase
	if (item & PR_CUTF_SENTRYGUN) {
		self->maxammo_shells = self->maxammo_shells + 130;
		self->maxammo_cells  = self->maxammo_cells  + 130;
	}

	self->cutf_items = self->cutf_items | item;
	self->money = self->money - cost;
	PrintMoney();
}

//CH gives ammo for items
void Apply_Item_Ammo(int item)
{
	float cellsfoo,rocketsfoo;
	cellsfoo = rocketsfoo = 0;

	if (item == PR_NIT_TESLA)
		cellsfoo = 150;
	if (item == PR_NIT_SECURITY_CAMERA)
		cellsfoo = 60;
	if (item == PR_NIT_TELEPORTER)
		cellsfoo = 100;

	if (item == PR_NIT_RL_LASER_SIGHT)
		rocketsfoo = 10;
	if (item == PR_NIT_CLUSTER_ROCKETS)
		rocketsfoo = 20;

	if (self->tf_items & item) { //we selling
		self->maxammo_cells = self->maxammo_cells - cellsfoo;
		self->maxammo_rockets = self->maxammo_rockets - rocketsfoo;
	}
	else {
		self->maxammo_cells = self->maxammo_cells + cellsfoo;
		self->maxammo_rockets = self->maxammo_rockets + rocketsfoo;
	}
}
void BuyItem(float cost, int item)
{
	if (item == 0) //Make sure we don't buy a null item
		return;

	Apply_Item_Ammo(item); //CH

	if (self->tf_items & item) //We are selling
	{
		if (item == PR_NIT_HOVER_BOOTS && self->tf_items & PR_NIT_HOVER_BOOTS_UPGRADE) {
			self->money = self->money + PR_COST_BOOTUPGRADE; //sync with buying of upgrade
			item = item + PR_NIT_HOVER_BOOTS_UPGRADE;
		}

		PrintRefund(cost);
		self->tf_items = self->tf_items - item;
		return;
	}
	//Special rules
	if (item == PR_NIT_AUTOSCANNER && !(self->tf_items & PR_NIT_SCANNER)) {
		sprint(self,PR_PRINT_HIGH,"Sorry, you have to buy the scanner first!\n");
		CuTFMenuSound(PR_MENUSOUND_WRONG);
		return;
	}
	if ((item == PR_NIT_RL_LASER_SIGHT || item == PR_NIT_CLUSTER_ROCKETS) && !(self->weapons_carried & PR_WEAP_ROCKET_LAUNCHER)) {
		sprint(self,PR_PRINT_HIGH,"Sorry, you have to buy the rocket launcher first!\n");
		CuTFMenuSound(PR_MENUSOUND_WRONG);
		return;
	}

	if (item == PR_NIT_HOVER_BOOTS && !(cost > self->money))
		sprint(self,PR_PRINT_HIGH,"Hold down jump to use the boots\n");

	if (item == PR_NIT_HOVER_BOOTS_UPGRADE && !(self->tf_items & PR_NIT_HOVER_BOOTS)) {
		sprint(self,PR_PRINT_HIGH,"Sorry, you have to buy the hover boots first!\n");
		CuTFMenuSound(PR_MENUSOUND_WRONG);
		return;
	}
	if (cost > self->money)
	{
		PrintNotEnoughMoney(cost);
		return;
	}

	self->tf_items = self->tf_items | item;
	self->money = self->money - cost;
	PrintMoney();
}

void BuyJob(float cost, float type)
{
	if (type == 0) //Make sure we don't buy a null grenade
		return;

	if (type == self->job) //We are selling
	{
		/*if (self.job & #JOB_WARLOCK)
		{
			if (!self.cutf_items & #CUTF_DEMONLORE)
				cost = cost - #COST_DEMONLORE; // sync with demonlore cost
			else
				self.cutf_items = self.cutf_items - (self.cutf_items & #CUTF_DEMONLORE);
		}
		if (self.job & #JOB_RUNNER)
		{
			if (!self.cutf_items & #CUTF_HIGHJUMP)
				cost = cost - #COST_HIGHJUMP; // sync with highjump cost
			else
				self.cutf_items = self.cutf_items - (self.cutf_items & #CUTF_HIGHJUMP);
		}
		if (self.job & #JOB_JUDOKA)
		{
			if (!self.cutf_items & #CUTF_STEALTH)
				cost = cost - #COST_STEALTH; // sync with stealth cost
			else
				self.cutf_items = self.cutf_items - (self.cutf_items & #CUTF_STEALTH);
		}*/
		PrintRefund(cost);
		self->job = 0;
		return;
	}
	if (cost > self->money)
	{
		PrintNotEnoughMoney(cost);
		return;
	}
	if (self->job) { //Make sure we haven't bought another kind already
		sprint(self,PR_PRINT_HIGH,"You already have a job. Leave your other one first.\n");
		CuTFMenuSound(PR_MENUSOUND_WRONG);
		return;
	}
	//Special rules for buying jobs
	if (type == PR_JOB_WARLOCK && !(self->cutf_items & PR_CUTF_KNIFE)) {
		sprint(self,PR_PRINT_HIGH,"Every Warlock needs a good bloody knife. Buy one.\n");
		CuTFMenuSound(PR_MENUSOUND_WRONG);
		return;
	}
	/*if (type == #JOB_WARLOCK)
		if (self.cutf_items & #CUTF_DEMONLORE)
			self.money = self.money + #COST_DEMONLORE; // sync this with demonlore cost
		else
			self.cutf_items = self.cutf_items | #CUTF_DEMONLORE; // warlocks see demon health

	if (type == #JOB_RUNNER)
		if (self.cutf_items & #CUTF_HIGHJUMP)
			self.money = self.money + #COST_HIGHJUMP; // sync with highjump cost
		else
			self.cutf_items = self.cutf_items | #CUTF_HIGHJUMP; // runner jumps high

	if (type == #JOB_JUDOKA)
		if (self.cutf_items & #CUTF_STEALTH)
			self.money = self.money + #COST_STEALTH; // sync with stealth price
		else
			self.cutf_items = self.cutf_items | #CUTF_STEALTH; // Judo is a master of stealth by default
	*/

	//Ok we buy it
	self->job = type;
	self->money = self->money - cost;
	PrintMoney();
}

void BuyExtra(float cost1, float cost2)
{
	float allcost;
	allcost = 0;

	if (self->job & PR_JOB_EXTRA1) // we have extra1
	{
		if (self->money < cost2) // not enough money for extra2, sell back job and extra1 then..
		{
			if (self->job & PR_JOB_WARLOCK)
			{
				allcost = PR_COST_WARLOCK + cost1;

				/*if (!self.cutf_items & #CUTF_DEMONLORE)
					allcost = allcost - #COST_DEMONLORE; // sync with demonlore cost
				else
					self.cutf_items = self.cutf_items - (self.cutf_items & #CUTF_DEMONLORE);*/
			}
			else if (self->job & PR_JOB_ARMY)
			{
				allcost = PR_COST_ARMY + cost1;
			}
			else if (self->job & PR_JOB_GUERILLA)
			{
				allcost = PR_COST_GUERILLA + cost1;
			}

			self->job = 0;
		PrintNotEnoughMoney(cost1);
			PrintRefund(allcost);
			return;
		}
		else // ok, buy second extra
		{
			self->job = self->job - (self->job & PR_JOB_EXTRA1);
			self->job = self->job | PR_JOB_EXTRA2;

			// decrease our money
			self->money = self->money - cost2;
		}
	}
	else if (self->job & PR_JOB_EXTRA2) // we have extra2, so sell back everything..
	{
		if (self->job & PR_JOB_WARLOCK)
		{
			allcost = (cost1+cost2)+PR_COST_WARLOCK;

			/*if (!self.cutf_items & #CUTF_DEMONLORE)
				allcost = allcost - #COST_DEMONLORE; // sync with demonlore cost
			else
				self.cutf_items = self.cutf_items - (self.cutf_items & #CUTF_DEMONLORE);*/
		}
		else if (self->job & PR_JOB_ARMY)
		{
			allcost = (cost1+cost2)+PR_COST_ARMY;
		}
		else if (self->job & PR_JOB_GUERILLA)
		{
			allcost = (cost1+cost2)+PR_COST_GUERILLA;
		}

		self->job = 0;
		PrintRefund(allcost);
		return;
	}
	else // we dont have any extra
	{
		if (self->money < cost1) // not enough money for 1st extra, so sell back job
		{
			if (self->job & PR_JOB_WARLOCK)
			{
				allcost = PR_COST_WARLOCK;

				/*if (!self.cutf_items & #CUTF_DEMONLORE)
					allcost = allcost - #COST_DEMONLORE; // sync with demonlore cost
				else
					self.cutf_items = self.cutf_items - (self.cutf_items & #CUTF_DEMONLORE);*/
			}
			else if (self->job & PR_JOB_ARMY)
			{
				allcost = PR_COST_ARMY;
			}
			else if (self->job & PR_JOB_GUERILLA)
			{
				allcost = PR_COST_GUERILLA;
			}

			self->job = 0;
			PrintNotEnoughMoney(cost1);
			PrintRefund(allcost);
			return;
		}
		else // ok, buy 1st extra
		{
			self->job = self->job | PR_JOB_EXTRA1;

			// decrease our money
			self->money = self->money - cost1;
		}
	}

	// Print money
	PrintMoney();
}

void BuyGren(float cost, float type)
{
	//local string talk;
	///local float level; // This is the level of legs we have.

	if (type == 0) // if type is 0 we are buying the NULL GRENADE(tm)
	{
		sprint(self, PR_PRINT_HIGH, "Congratulations. You have just attempted to buy a null grenade.\n");
		RPrint("WARNING: Attempted purchase of NULL GRENADE. BUG BUG BUG BUG BUG BUG BUG\n");
		return;
	}
	// Check whether we're allowing gren purchase.
	/*level = ReturnLegLevel(self.custom_speed);
	if (level == 1) // grens are a no-no
	{
		sprint(self, #PRINT_HIGH, "Sorry, you are not allowed any grenades at that speed. It is very dangerous.\n");
		return;
	}
	else if (level == 2)
	{
		local float acc; // Acceptable.

		acc = #FALSE;

		if (type == #GR_TYPE_CALTROP)
			acc = #TRUE;
		else if (type == #GR_TYPE_CONCUSSION)
			acc = #TRUE;
		else if (type == GR_TYPE_STASIS)
			acc = #TRUE;
		else if (type == #GR_TYPE_ANTIGRAV)
			acc = #TRUE;
		else if (type == #GR_TYPE_FLARE)
			acc = #TRUE;
		if (!acc)
		{
			sprint(self, #PRINT_HIGH, "Sorry, using those grenades at your speed could be dangerous.\n");
			return;
		}
	}
	else if (level == 3)
	{
		if (type == #GR_TYPE_MIRV)
		{
			sprint(self, #PRINT_HIGH, "Sorry, you aren't allowed MIRVs at that speed.\n");
			return;
		}
	}*/
	// Grenade and speed is fine.

	if (!self->tp_grenades_1 && cost <= self->money) // if we don't have gren1s, make it a gren1
	{
/*		if (self.money_misc + cost > self.money_spent - self.money_misc)
		{
			PrintTooHighProportion();
			return;
		} */
		self->tp_grenades_1 = type;
		//sprint(self, #PRINT_HIGH, "You buy the grenades and add them to your first grenade slot for ");
	}
	else if (!self->tp_grenades_2 && cost <= self->money) // We already have gren1s, can we buy gren2s?
	{
		/*if (self.money_misc + cost > self.money_spent - self.money_misc)
		{
			PrintTooHighProportion();
			return;
		}*/
		self->tp_grenades_2 = type;
		//sprint(self, #PRINT_HIGH, "You buy the grenades and add them to your second grenade slot for ");
	}
	else	// We have both gren1s and gren2s so now we need to try to sell some
	{		// If our type matches gren1 AND gren2s we want to sell both
		if (self->tp_grenades_1 == type && self->tp_grenades_2 == type)
		{
			PrintRefund(cost * 2);
			//self.money_misc = self.money_misc - cost * 2;
			//self.money = self.money + cost * 2; //*2
			self->tp_grenades_1 = 0;
			self->tp_grenades_2 = 0;
		}
		else if (self->tp_grenades_1 == type) // otherwise if our gren1s are the type
		{
			PrintRefund(cost);
			//self.money_misc = self.money_misc - cost;
			//self.money = self.money + cost;
			self->tp_grenades_1 = 0;
		}
		else if (self->tp_grenades_2 == type) // otherwise if we have gren2s as the type..
		{
			PrintRefund(cost);
			//self.money_misc = self.money_misc - cost;
			//self.money = self.money + cost;
			self->tp_grenades_2 = 0;
		}
		else if (cost <= self->money)
		{
			CuTFMenuSound(PR_MENUSOUND_WRONG);
			sprint(self,PR_PRINT_HIGH,"You already have a full set of grenades. Sell some back first.\n");
		}
		else
			PrintNotEnoughMoney(cost);

		return;
	}
	self->money = self->money - cost;
	PrintMoney();
	//self.money_misc = self.money_misc + cost;
	//self.money_spent = self.money_spent + cost;
	/*talk = ftos(cost);
	sprint(self, #PRINT_HIGH, talk);
	sprint(self, #PRINT_HIGH, " dollars. You have ");
	talk = ftos(self.money);
	sprint(self, #PRINT_HIGH, talk);
	sprint(self, #PRINT_HIGH, " dollars left.\n");*/
}
/*

void(float cost, float type) BuyGren1 =
{
	if (type == 0) //Make sure we don't buy a null grenade
		return;

	if (type == self.tp_grenades_1) //We are selling
	{
		PrintRefund(cost);
		self.tp_grenades_1 = 0;
		return;
	}
	if (cost > self.money)
	{
		PrintNotEnoughMoney(cost);
		return;
	}
	if (self.tp_grenades_1) { //Make sure we haven't bought another kind already
		sprint(self,#PRINT_HIGH,"You already have grenades. Sell back the ones you bought first.\n");
		return;
	}
	//Ok we buy it
	self.tp_grenades_1 = type;
	self.money = self.money - cost;
	PrintMoney();
};

void(float cost, float type) BuyGren2 =
{
	if (type == 0) //Make sure we don't buy a null grenade
		return;

	if (type == self.tp_grenades_2) //We are selling
	{
		PrintRefund(cost);
		self.tp_grenades_2 = 0;
		return;
	}
	if (cost > self.money)
	{
		PrintNotEnoughMoney(cost);
		return;
	}
	if (self.tp_grenades_2) { //Make sure we haven't bought another kind already
		sprint(self,#PRINT_HIGH,"You already have grenades. Sell back the ones you bought first.\n");
		return;
	}

	//Ok we buy it
	self.tp_grenades_2 = type;
	self.money = self.money - cost;
	PrintMoney();
};
*/

//Functions for our new grenades
void fragspike_touch()
{
	//if (pointcontents(self.origin) == #CONTENT_SKY)
	if (pointcontents(self->origin) != PR_CONTENT_EMPTY)
	{
		dremove(self);
		return;
	}

// hit something that bleeds
	if (other->takedamage)
	{
		deathmsg = PR_DMSG_GREN_FRAG;

		TF_T_Damage (other, self, self->real_owner, 150, PR_TF_TD_NOTTEAM, PR_TF_TD_NAIL);
		dremove(self);
	}
	else
	{
		//We bounce off world since we are fragments
		if (self->heat > 0) {
//			self.heat = self.heat - 1;
//			if (self.velocity = '0 0 0') { //Skip around
			if (1) { //Skip around
				self->velocity[X] = (random() - 0.5) * 200;
				self->velocity[Y] = (random() - 0.5) * 200;
				self->velocity[Z] = random() * 200;
			}
		}
		else {
			if (other->classname == "force_field") //- OfN - Makes field explosion b4 removing it
			   //FieldExplosion(other,self.origin,self);
			   FieldEvent(other,self->origin,self);
			else
			{
				WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
				WriteByte (PR_MSG_BROADCAST, PR_TE_SUPERSPIKE);
				WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
				WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
				WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
			#ifdef PR_QUAKE_WORLD
				multicast (self->origin, PR_MULTICAST_PHS);
			#endif
			}

			dremove(self);
		}
	}
}
void FragSpikeThink()
{
	if (self->heat > 0) {
		self->heat = self->heat - 1;

		self->angles[Y] = random() * 360;
		self->angles[X] = 0;
		self->angles[Z] = 0;
		makevectors(self->angles);
		if (self->velocity == V({0, 0, 0}))
			self->origin[Z] = self->origin[Z] + 50;

		self->velocity = v_forward * 1000;
		self->velocity[Z] = (random() + 0.25) * 1000;

		self->nextthink = time + 0.5 + random();
	}
	else {
		WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_BROADCAST, PR_TE_SUPERSPIKE);
		WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
	#ifdef PR_QUAKE_WORLD
		multicast (self->origin, PR_MULTICAST_PHS);
	#endif
		dremove(self);
	}
}

void FragGrenadeTouch()
{
	if (other == self->owner)
		return; 	// don't explode on owner

	// Thrown grenades don't detonate when hitting an enemy

	sound (self, PR_CHAN_WEAPON, "weapons/bounce.wav", 1, PR_ATTN_NORM);	// bounce sound
	if (self->velocity == V({0, 0, 0}))
		self->avelocity = V({0, 0, 0});
}
void FragGrenadeExplode()
{
	float shraps;
	//local string happy;
	entity newmis; // (???)

//Small explosive radius
	deathmsg = PR_DMSG_GREN_HAND;
	T_RadiusDamage (self, self->owner, 80, world);

	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
	WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
#ifdef PR_QUAKE_WORLD
	multicast (self->origin, PR_MULTICAST_PHS);
#endif

	shraps = rint(6 * random()) + 6; //Pieces of shrapmetal;
	while (shraps > 0) {
		self->angles[Y] = random() * 360;
		self->angles[X] = 0;
		self->angles[Z] = 0;
		makevectors(self->angles);

		deathmsg = PR_DMSG_GREN_NAIL;
		newmis = spawn ();
		//WK Make shraps hit owner newmis.owner = self.owner;
		newmis->real_owner = self->owner;
		newmis->movetype = PR_MOVETYPE_BOUNCE;
		newmis->solid = PR_SOLID_BBOX;

		newmis->angles = V({0, 0, 0});
		newmis->classname = "spike";
		newmis->think = FragSpikeThink;
//		newmis.think = SUB_Remove;
		newmis->nextthink = time + 1;
		setmodel (newmis, "progs/spike.mdl");
		setsize (newmis, PR_VEC_ORIGIN, PR_VEC_ORIGIN);
		setorigin (newmis, self->origin);

		//Start it over the ground
		newmis->origin[Z] = newmis->origin[Z] + 10;

		newmis->velocity = v_forward * 1000;
		if (random() < 0.5)
			newmis->velocity[Z] = 20;
		else
			newmis->velocity[Z] = random() * 1000;
		newmis->avelocity = V({100, 100, 100});
		newmis->touch = fragspike_touch;
		newmis->weapon = PR_DMSG_GREN_NAIL;

		//Make it bounce six times.
		newmis->heat = 5;
		shraps = shraps - 1;
	}
#ifdef PR_QUAKE_WORLD
	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
	WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
	multicast (self->origin, PR_MULTICAST_PHS);
	dremove(self);
#else
	BecomeExplosion();
#endif

}

void KracGrenadeTouch()
{
	//WK Have it detonate on impact
	if (other == self->owner)
		return; 	// don't explode on owner

	sound (self, PR_CHAN_WEAPON, "weapons/bounce.wav", 1, PR_ATTN_NORM);	// bounce sound

	KracGrenadeExplode();
}


#ifdef PR_COOP_MODE_ENHANCED
float COOP_IsCoopMonster(entity test);
#endif

void KracGrenadeExplode()
{
	entity te;
	entity oldself;
	float damage;

	te = findradius(self->origin, 150);
	while (te != world)
	{
		// Player?
		if (te->classname == "player" && te->health > 0)
		{
			//WW: Kracs disable detpack
			oldself = self;
			self = te;
			TeamFortress_DetpackStop(PR_TRUE);
			self = oldself;

			//WK 1/7/7 Krac nerf
			//Deals 200, tapers off linearly with radius (min 75)
			damage = 200 - vlen(te->origin - self->origin);
			if (damage > 200 || damage < 0) {
				bprint(PR_PRINT_MEDIUM,"Logic error in krac occured.\n");
				dremove(self);
				return;
			}
			if (damage < 75) damage = 75;
			if (te->armorclass & PR_AT_SAVEMELEE) damage = floor(damage / 2);
			te->armorvalue = te->armorvalue - damage;
			if (te->armorvalue < 10) te->armorvalue = 10;

			//Damage is now dealt to cells instead of killing it all at once.
			//Do this before we adjust for armor damage
			te->ammo_cells = te->ammo_cells - damage;
			if (te->ammo_cells < 0) te->ammo_cells = 0;

			//WK 1/7/7 Fix the "dead thunderbolt" bug when you get kraced.
			//This should force a weapon switch on the next frame.
			if (te->current_weapon == PR_WEAP_LIGHTNING || te->current_weapon == PR_WEAP_LASERCANNON ||
				te->current_weapon == PR_WEAP_DAEDALUS || te->current_weapon == PR_WEAP_AIRF ||
				te->current_weapon == PR_WEAP_FLAMETHROWER)
				te->currentammo = te->ammo_cells;

			/* WK 1/7/7 Old damage code:
			//Gel armor makes kracs do less damage
			if (te.armorclass & #AT_SAVEMELEE) {
				if (te.armorvalue > 75)
					te.armorvalue = 75;
			} else {
				if (te.armorvalue > 10)
					te.armorvalue = 10;
			}
			te.ammo_cells = 0; //Kill their metal supply too
			*/
		}
		else if (te->classname == "pipebomb")
			te->nextthink = time + 0.1 + random() / 4; //WK 1/7/7 Speed up kracs breaking pipebombs, since a 1.1 second delay is too long
		else if (te->classname == "grenade" && te->netname == "land_mine")
		{
			te->think = GuerillaExplode;
			te->nextthink = time + 0.1;
		}
		else if (te->classname == "building_sentrygun") {
			// Gizmo - this was left out here...
			damage = 200 - vlen(te->origin - self->origin);
			if (damage > 200 || damage < 0) {
				bprint(PR_PRINT_MEDIUM,"Logic error in krac occured.\n");
				dremove(self);
				return;
			}
			if (damage < 75) damage = 75;

			TF_T_Damage(te, self, self->owner, damage, 0, 0); //WK 1/7/7 Teslas and sentries take less now
		} else if (te->classname == "building_tesla") {
			// Gizmo - this was left out here...
			damage = 200 - vlen(te->origin - self->origin);
			if (damage > 200 || damage < 0) {
				bprint(PR_PRINT_MEDIUM,"Logic error in krac occured.\n");
				dremove(self);
				return;
			}
			if (damage < 75) damage = 75;

			TF_T_Damage(te, self, self->owner, damage, 0, 0);
		} else if (te->classname == "building_dispenser")
			TF_T_Damage(te, self, self->owner, 500, 0, 0);
		else if (te->classname == "building_camera")
			TF_T_Damage(te, self, self->owner, 400, 0, 0);
		else if (te->classname == "building_teleporter")
			TF_T_Damage(te, self, self->owner, 200, 0, 0);
		else if (te->classname == "building_sensor") //- Often - needed?
			TF_T_Damage(te, self, self->owner, 200, 0, 0);
		else if (te->classname == "building_fieldgen") //- Often - needed?
			TF_T_Damage(te, self, self->owner, 200, 0, 0);
		else if (te->classname == "item_armor1" || te->classname == "item_armor2" || te->classname == "item_armorInv") {
			te->solid = PR_SOLID_NOT;
			te->model = string_null;
			te->nextthink = time + 45; //WK 20
			te->think = SUB_regen;
			//SUB_UseTargets();
		}
		else if (te->classname == "detpack") {
			if (te->owner->netname != self->owner->netname) {
				bprint (PR_PRINT_MEDIUM, te->owner->netname);
				bprint (PR_PRINT_MEDIUM, "'s detpack was Kraced by ");
				bprint (PR_PRINT_MEDIUM, self->owner->netname);
				bprint (PR_PRINT_MEDIUM, "\n");
			} else {
				sprint (te->owner, PR_PRINT_MEDIUM, "You Kraced your own detpack!\n");
			}
			// This code handles disarming a detpack
			if (te->weaponmode == 1) // Detpack was being disarmed
			{
				te->enemy->tfstate = te->enemy->tfstate - (te->enemy->tfstate & PR_TFSTATE_CANT_MOVE);
		#ifdef PR_QUAKE_WORLD
				TeamFortress_SetSpeed(te->enemy);
		#else
				te->enemy->pausetime = time;
		#endif

				dremove(te->oldenemy);	 // CountDown
				dremove(te->observer_list); //	Disarm timer
			}
			else { //Not being disarmed
				dremove(te->oldenemy);	 // CountDown
			}
			te->think = BecomeExplosion;
			te->nextthink = time + 0.1;
		}
#ifdef PR_COOP_MODE_ENHANCED
		// Gizmo - enable kracs to work on monsters
		else if (COOP_IsCoopMonster(te)) {
			// code is just copied from above
			damage = 200 - vlen(te->origin - self->origin);
			if (damage > 200 || damage < 0) {
				bprint(PR_PRINT_MEDIUM,"Logic error in krac occured.\n");
				dremove(self);
				return;
			}
			if (damage < 75) damage = 75;
			te->armorvalue = te->armorvalue - damage;
			if (te->armorvalue < 10) te->armorvalue = 10;
		}
#endif

		te = te->chain;
	}

#ifdef PR_QUAKE_WORLD
	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
	WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
	multicast (self->origin, PR_MULTICAST_PHS);
	dremove(self);
#else
	BecomeExplosion();
#endif
}

//Stops us from lowering the time immunity
void makeImmune(entity immuner,float timeimmune)
{
	if (immuner == world) return;
	if (immuner->immune_to_chec > timeimmune) return;
	immuner->immune_to_chec = timeimmune;
}

void BastardTimer()
{
	entity te;
	string st;
	te = self->owner;

	//makeImmune(te,time+5);

	// Bastard Memory
	// If the timer's bastard is no longer connected to the server, the timer
	// will sit dormant and remove itself, once penance time has passed. -Pulseczar (PZ)
	if (te  == world|| (te != world && !te->is_connected))
	{
		if (self->penance_time <= time)
		{
			delstr(self->t_s_h);
			dremove(self);
			return;
		}
		self->nextthink = time + 2;
		return;
	}

	if (te->penance_time < time)
	{
		stuffcmd(te, "cl_yawspeed 140;-right;-left\n");
		//stuffcmd(te, "name \"");
		//stuffcmd(te, te.old_netname);
		//stuffcmd(te, "\"\n");

		//Reset their color
		// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
		//     themselves, to keep from having to kick them when they do.
		//stuffcmd(te, "color ");
		st = ftos(TeamFortress_TeamGetColor(te->team_no) - 1);
		//- OfN - Nice colors
		if (nicecolors==1) st =TeamGetNiceColor(te->team_no);
		setinfo(te, "topcolor", st);    // PZ
		setinfo(te, "bottomcolor", st); // PZ
		//stuffcmd(te, st);
		//stuffcmd(te, "\n");

		bprint (PR_PRINT_MEDIUM, te->netname);
		bprint (PR_PRINT_MEDIUM, " has his teamkill curse removed\n");

		//Don't let up on them. Two more teamkills and its back into the tarpit for them
		te->ff_count = self->frags - 1.9;
		if (te->ff_count < 0) te->ff_count = 0;

		TF_T_Damage(te, world, world, te->health + 300, PR_TF_TD_IGNOREARMOUR, PR_TF_TD_OTHER);

		delstr(self->t_s_h);
		remove(self);
		return;
	}

	// PZ NOTE: *sigh* It took me forever to figure out where a teamkiller's color is set to pink.
	if (random() > 0.5)
		stuffcmd(te, "cl_yawspeed 500;-left;+right;color 8 8\n");
	else
		stuffcmd(te, "cl_yawspeed 500;-right;+left;color 8 8\n"); // <-- was 9 9
	// PZ: Players can no longer change their color, team, or skin manually, via console/script/stuffcmd(). Created setinfo()
	//     for this purpose. This keeps players from accidentally changing these things and getting kicked from the server, probably
	//     usually due to autoexecution of .cfg files by their client.
	setinfo(te, "topcolor", "8");
	setinfo(te, "bottomcolor", "8");
	//te.immune_to_check = time + 5;
	te->ammo_rockets = 0;
	te->ammo_cells = 0;
	te->ammo_nails = 0;
	te->ammo_shells = 0;
	te->ammo_detpack = 0;
	te->ammo_c4det = 0;
	  te->no_grenades_1 = 0;
	  te->no_grenades_2 = 0;
	if (te->health > 50) te->health = 50;
	if (te->armorvalue > 10) te->armorvalue = 10;

	self->nextthink = time + 2;
}

void createBastard(entity bastard,float threshold)
{
	//if (mapname == "border1" || mapname == "border1r") // dont curse ppl on these maps, thay use a bug
	// done - TODO: Make triggers ignore cursed persons instead..
	//	  return;

	entity te;

	if (bastard == world) return;

	// Neo Mode
	if (neo.isModeActive()) if (neo.getNeo() == bastard)
	{
		neo.removeNeo(bastard);
		bprint(PR_PRINT_HIGH, "\n^<^-^-^-^-^-^-^-^-^-^> ^bNeo Mode^b ^<^-^-^-^-^-^-^-^-^-^>\n");
		bprint(PR_PRINT_HIGH, "Kill someone to become Neo!\n");
		bprint(PR_PRINT_HIGH, "^<^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^>\n");
		BroadcastSound("boss1/sight1.wav");
		neo.centerPrint();
	}
	if (bastard->penance_time > time) {
		bastard->ff_count = bastard->ff_count - 1; //Dont count it
		bastard->penance_time = time + 60; //Just reset the clock
		makeImmune(bastard,time+65);
		return;
	}

	//---------------------------------------------------------------------------//
	// Why do I not like MegaTF? ------------------------------------------------//
	// Look at http://www.planetfortress.com/history/people/shaka.html ----------//
	if (bastard->ff_count == threshold + 0.1 || bastard->ff_count == threshold + 0.6) {
		//- OfN - lol
		//bprint(#PRINT_HIGH,"���� �� ���� ��M\n");
		bprint(PR_PRINT_HIGH,"ok, lets send ");
		bprint(PR_PRINT_HIGH,bastard->netname);
		bprint(PR_PRINT_HIGH," to a nice megaTF server...\n");
		stuffcmd(bastard, "name \"CustomTF reject-wanna play Mega\"\n");
		//stuffcmd(bastard, "name \"im gay, wanna make new friends\"\n");

		string st;
#ifdef PR_QUAKE_WORLD
		st = infokey(world,"curseserver");
#endif
		if (st == string_null) st = "amnesia.matrux.net:27502"; //WK 1/7/7 Updated ip
			stuffcmd(bastard, "connect ");
		stuffcmd(bastard, st);
		stuffcmd(bastard, "\n");

		return; //- OfN - (missing) OLD BUG?
	} //---------------------------------------------------------------------------//


	newmis = spawnServerSide(); // PZ: make it a server-side only entity

	newmis->classname = "timer";
	newmis->netname = "bastardtimer";
	newmis->owner = bastard;
	newmis->think = BastardTimer;
	newmis->nextthink = time + 1;
	newmis->frags = threshold; //Let the timer know what the thresh is

	// Bastard Memory
	// Bastard timer will now continue to live after the bastard leaves the game.
	// If bastard returns before their curse time is complete, they will continue
	// to be punished until penance served. -Pulseczar (Jan 2010)
	newmis->t_s_h = infokey(bastard, "ip");      // Bastard timer will hold bastard's IP and penance time.
	newmis->t_s_h = makestr(newmis->t_s_h);       // Penance time is stored in the timer's .penance_time.

	makeImmune(bastard,time+65);
	//bastard.immune_to_check = time + 65; //Make him immune while purple
	//bastard.old_netname = bastard.netname;

	bprint (PR_PRINT_MEDIUM, bastard->netname);
	bprint (PR_PRINT_MEDIUM, " has been struck down by the wrath of the admin.\n");
	bprint (PR_PRINT_MEDIUM, "Feel free to frag him at will.\n");

	bastard->penance_time = time + 60;
	stuffcmd(bastard, S_("name \"^bTeamKiller^b ("));
	stuffcmd(bastard, bastard->netname);
	stuffcmd(bastard, ")\"\n");

	// Drop any GoalItems
	te = find (world, "classname", "item_tfgoal");
	while (te != world)
	{
		if (te->owner == bastard)
		{
			// Remove it from the player, if it is supposed to be
			if (!(te->goal_activation & PR_TFGI_KEEP))
			{
				tfgoalitem_RemoveFromPlayer(te, bastard, 0);
			}

			// CTF support
			if (CTF_Map == PR_TRUE && te->goal_no == PR_CTF_FLAG1)
			{
				bprint(PR_PRINT_HIGH, bastard->netname);
				bprint(PR_PRINT_HIGH, S_(" ^bLOST^b the ^bBLUE^b flag!\n"));
			}
			else if (CTF_Map == PR_TRUE && te->goal_no == PR_CTF_FLAG2)
			{
				bprint(PR_PRINT_HIGH, bastard->netname);
				bprint(PR_PRINT_HIGH, S_(" ^bLOST^b the ^bRED^b flag!\n"));
			}
		}
		te = find(te, "classname", "item_tfgoal");
	}

	// Drop any runes too!
	PlayerDropRunes(bastard);

	//return;
}

/*
void() Autoitem_think =
{
	local entity oself;
	local float happy;
	if (self.heat == #TRUE)
		self.heat = #FALSE;
	else
		self.heat = #TRUE;
	happy = self.heat;

	oself = self;
	self = self.owner;
	if (self.tf_items & #NIT_AUTOID) {
		//Do an id
		TeamFortress_ID(#TRUE); //TRUE means we are in autoitemthink
	}
	if (self.weapons_carried & #WEAP_MEDIKIT) {
		//Do a heal, passing it #TRUE tells it not to "tink"
		//Don't automedic whack while fully invisible
		if (!(self.job & #JOB_THIEF && (self.job & #JOB_ACTIVE || self.job & #JOB_FULL_HIDE)))
			W_FireMedikit(#TRUE);
	}
	if (happy) { //Only do this every two seconds
	if (self.tf_items & #NIT_AUTOSCANNER) {
		//Do a scan15
		TeamFortress_Scan(15,#TRUE);
	}
	}

	self = oself;
	self.nextthink = time + 0.5;
};
*/
void Autoitem_think()
{
	entity	oself;
	vector	src;
	float 	tmp;

	oself = self;
	self = self->owner;

	// Gizmo - made extensive mods to this function, it's now a lot better
	makevectors( self->v_angle );
	src = self->origin + self->view_ofs;
	traceline( src, src + v_forward*2048, PR_TL_ANY_SOLID, self );

	// if trace_ent is world, everything should still work out fine
	if ( trace_ent != oself->oldenemy ) {
		if (self->tf_items & PR_NIT_AUTOID) {
			if ( trace_ent != world && trace_ent->solid != PR_SOLID_BSP )
				//Do an id
				TeamFortress_ID(PR_TRUE); //TRUE means we are in autoitemthink
			else
				centerprint( self, "" );
		}
		// TODO: set initial crosshair color when spawning
		if ( self->cutf_moreitems & PR_CUTF_MI_FRIENDORFOE ) {
			if ( trace_ent != world && trace_ent->solid != PR_SOLID_BSP && trace_ent->takedamage ) {
				// if teamplay is disabled, then always report as an enemy
				if ( teamplay >= 1 ) {
					tmp = GetTeam( trace_ent );

					// team zero when teamplay is on should be neutral
					if ( !tmp )
						stuffcmd( self, "crosshaircolor 254\n" );
					else if ( !Teammate(tmp, self) ) {
						// check for spies
						if ( trace_ent->classname == "player" && trace_ent->cutf_items & PR_CUTF_SPY_KIT && trace_ent->undercover_team != 0 ) {
							if ( Teammate(trace_ent->undercover_team, self) )
								stuffcmd( self, "crosshaircolor 210\n" );
							else
								stuffcmd( self, "crosshaircolor 79\n" );
						} else
							stuffcmd( self, "crosshaircolor 79\n" );
					} else
						stuffcmd( self, "crosshaircolor 210\n" );
				} else
					stuffcmd( self, "crosshaircolor 79\n" );
			} else
				stuffcmd( self, "crosshaircolor 254\n" );
		}

		oself->oldenemy = trace_ent;
	}

	if ( oself->attack_finished <= time ) {
		if (self->weapons_carried & PR_WEAP_MEDIKIT) {
			//Do a heal, passing it #TRUE tells it not to "tink"
			//Don't automedic whack while fully invisible
			if (!(self->job & PR_JOB_THIEF && (self->job & PR_JOB_ACTIVE || self->job & PR_JOB_FULL_HIDE)))
				W_FireMedikit(PR_TRUE);
		}
		oself->attack_finished = time + 0.5;
	}

	// only do this every second
	if ( oself->pain_finished <= time ) {
		if (self->tf_items & PR_NIT_AUTOSCANNER) {
			//Do a scan15
			TeamFortress_Scan(15,PR_TRUE);
		}
		oself->pain_finished = time + 1;
	}

	self = oself;
	self->nextthink = time + 0.1;
}

// PZ: Why did the original authors punish MegaTF players, when they were really mad at the author of MegaTF for releasing his server to the
// public so that anyone could have a server, ignoring Team Fortress Software's source code agreement? This definitely has no place in 2014
// when there are so few players. If someone comes from a Mega server with their config set up for it, and they accidentally hit their 'lay'
// key, they shouldn't be punished for it. The community is way too small now for the in-fighting. Removed the call to this function.
void custom_lay()
{
	if (intermission_running)
		return;

	//local string st;
	if (self->last_saveme_sound < time) {
		bprint(PR_PRINT_HIGH,self->netname);
		bprint(PR_PRINT_HIGH,"'s legs were broken when he tried to 'lay'\n");
		sound(self,PR_CHAN_VOICE,"player/pain1.wav",1,PR_ATTN_NONE);
		self->last_saveme_sound = time + 60;
	}
	self->suicide_time = time + 60;
	self->leg_damage = 8; // Original was 9
	TeamFortress_SetSpeed(self);
	if (self->health > 50) self->health = 50;
	if (self->armorvalue > 10) self->armorvalue = 10;
}

//Blow up all sentries, dispensers and teslas if we can no longer build them
void DetonateAllGuns()
{
	if (!(self->cutf_items & PR_CUTF_DISPENSER))
		Find_And_Dmg("building_dispenser", self, 1,0);
	if (!(self->cutf_items & PR_CUTF_SENTRYGUN))
		Find_And_Dmg("building_sentrygun", self, 1,0);
	if (!(self->tf_items & PR_NIT_TESLA)) //WK
		Find_And_Dmg("building_tesla", self, 1,0);
	if (!(self->tf_items & PR_NIT_SECURITY_CAMERA)) //CH
		Find_And_Dmg("building_camera", self, 1,0);
	if (!(self->tf_items & PR_NIT_TELEPORTER)) //CH
		Find_And_Dmg("building_teleporter", self, 1,0);
	if (!(self->cutf_items & PR_CUTF_SENSOR)) //SB
		Find_And_Dmg("building_sensor", self, 1,0);
	if (!(self->cutf_items & PR_CUTF_FIELDGEN)) //OfN
		Find_And_Dmg("building_fieldgen", self, 1,0);
}

//Blow up all sentries, dispensers and teslas if we can no longer build them
void DetonateAllGunsForced()
{
	Find_And_Dmg("building_dispenser", self, 1,0);
	Find_And_Dmg("building_sentrygun", self, 1,0);
	Find_And_Dmg("building_tesla", self, 1,0);
	Find_And_Dmg("building_camera", self, 1,0);
	Find_And_Dmg("building_teleporter", self, 1,0);
	Find_And_Dmg("building_sensor", self, 1,0);
	Find_And_Dmg("building_fieldgen", self, 1,0);
}

//Returns true if the entity is a building -- UPDATE THIS WHEN A NEW ONE IS ADDED
float IsBuilding(entity tester)
{
	if (
		tester->classname == "building_sentrygun" ||
		tester->classname == "building_sentrygun_base" ||
		tester->classname == "building_tesla" ||
		tester->classname == "building_dispenser" ||
		tester->classname == "building_camera" ||
		tester->classname == "building_arawana" ||            // PZ NOTE: a fish? or a ladybug?
		tester->classname == "building_teleporter"||
		tester->classname == "building_fieldgen"||
		tester->classname == "building_sensor"
	   )
		return PR_TRUE;
	return PR_FALSE;
}

void UpdateWeaponItems()
{
	//#ifdef CUSTOM_PC_USES_ITEMS
	if (self->weapons_carried & PR_WEAP_SHOTGUN)
		self->items = self->items | PR_IT_SHOTGUN;
	else
		self->items = self->items - (self->items & PR_IT_SHOTGUN);

	if (self->weapons_carried & PR_WEAP_SUPER_SHOTGUN)
		self->items = self->items | PR_IT_SUPER_SHOTGUN;
	else
		self->items = self->items - (self->items & PR_IT_SUPER_SHOTGUN);

	if (self->weapons_carried & PR_WEAP_NAILGUN)
		self->items = self->items | PR_IT_NAILGUN;
	else
		self->items = self->items - (self->items & PR_IT_NAILGUN);

	if (self->weapons_carried & (PR_WEAP_LIGHT_ASSAULT | PR_WEAP_SNG))
		self->items = self->items | PR_IT_LIGHT_ASSAULT;// #IT_SUPER_NAILGUN;
	else
		self->items = self->items - (self->items & PR_IT_LIGHT_ASSAULT);

	if (self->weapons_carried & (PR_WEAP_GRENADE_LAUNCHER | PR_WEAP_FLAMETHROWER | PR_WEAP_DAEDALUS))
		self->items = self->items | PR_IT_GRENADE_LAUNCHER;
	else
		self->items = self->items - (self->items & PR_IT_GRENADE_LAUNCHER);

	if (self->weapons_carried & (PR_WEAP_ROCKET_LAUNCHER | PR_WEAP_INCENDIARY))
		self->items = self->items | PR_IT_ROCKET_LAUNCHER;
	else
		self->items = self->items - (self->items & PR_IT_ROCKET_LAUNCHER);

	if (self->weapons_carried & PR_WEAP_LIGHTNING)
		self->items = self->items | PR_IT_LIGHTNING;
	else
		self->items = self->items - (self->items & PR_IT_LIGHTNING);
	//#endif
}

float CanBuyItem(float inp)
{
	float mnu_offset;
	float mnu_num;
	float itm_num;

	mnu_offset = self->current_menu - PR_MENU_PRIMARY_WEAPON;
	mnu_num = mnu_offset - (floor(mnu_offset/3))*3;
	itm_num = inp + (mnu_num * 7);

	if (mnu_offset < 3)
	{
		if (disabledstuff1 & itob(itm_num))
			return PR_FALSE;
	}
	else if (mnu_offset < 6)
	{
		if (disabledstuff2 & itob(itm_num))
			return PR_FALSE;
	}
	else if (mnu_offset < 9)
	{
		if (disabledstuff3 & itob(itm_num))
			return PR_FALSE;
	}
	else if (mnu_offset < 12)
	{
		if (disabledstuff4 & itob(itm_num))
			return PR_FALSE;
	}
	else if (mnu_offset < 15)
	{
		if (disabledstuff5 & itob(itm_num))
			return PR_FALSE;
	}
	else
		return PR_TRUE; // bug, anyway

	return PR_TRUE;
}

float ItemIsFree(float inp)
{
	float mnu_offset;
	float mnu_num;
	float itm_num;

	mnu_offset = self->current_menu - PR_MENU_PRIMARY_WEAPON;
	mnu_num = mnu_offset - (floor(mnu_offset/3))*3;
	itm_num = inp + (mnu_num * 7);

	if (mnu_offset < 3)
	{
		if (givenstuff1 & itob(itm_num))
			return PR_TRUE;
	}
	else if (mnu_offset < 6)
	{
		if (givenstuff2 & itob(itm_num))
			return PR_TRUE;
	}
	else if (mnu_offset < 9)
	{
		if (givenstuff3 & itob(itm_num))
			return PR_TRUE;
	}
	else if (mnu_offset < 12)
	{
		if (givenstuff4 & itob(itm_num))
			return PR_TRUE;
	}
	else if (mnu_offset < 15)
	{
		if (givenstuff5 & itob(itm_num))
			return PR_TRUE;
	}
	else
		return PR_FALSE; // bug, anyway

	return PR_FALSE;
}

} // END namespace Progs
