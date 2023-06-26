/*=======================================================//
// CPStuff.QC - CustomTF 3.2.OfN		   - 30/1/2001 - //
// by Sergio Fuma�a Grunwaldt  - OfteN [cp] 			 //
=========================================================//
 Additional stuff, basicaly things i dunno where to put :)
=========================================================*/

#include "progs.h"
#include "ofndefs.h"
#include "agr.h"
#include "invade.h"
#include "debug.h"
#include "environ.h"
#include "custom.h"
#include "cpstuff.h"
#include "warlock.h"
#include "grunty.h"
#include "combat.h"
#include "weapons.h"
#include "haxxx.h"
#include "tforttm.h"
#include "tfdefs.h"
#include "coop_defs.h"
#include "player.h"
#include "spectate.h"
#include "client.h"
#include "optimize.h"
#include "doors.h"
#include "plats.h"
#include "neo.h"

namespace Progs {

void HoloThink();
void RemoveHolo(entity player);
float StripSpecificItem(entity player, float itemid, float storechange, float resetstored);
//float(float itemid, float stuff1, float stuff2, float stuff3, float stuff4, float stuff5) ItemInside;
float StripDisallowed(entity player);
float GiveSpecificItem(entity player, float itemid, float storechange, float resetstored);
float GiveFreeStuff(entity player);
float SetItemInside(entity ent, float type, float itemid, float value);
float RestoreOriginalItems(entity player);
string ver_ftos(float ver);
void InitMegaTF();

//====================================================================//
// Initialize stuff, localinfos etc...								  //
// This is called on worldspawn and with the Admin infos command	  //
//====================================================================//

// If startup is TRUE, even values that imply precachees are updated
// so it should only be set on wordspawn

void UpdateInfos(float startup)
{
	string st;

	st = infokey(world, "allow_debug"); //
	if (st == "1" || st =="on")
		allow_debug = 1;
	else
	{
		debug_target=world;
		allow_debug = 0;
	}

	st = infokey(world, "bodyque_size"); //
	bodyque_size = stof(st);
	if (bodyque_size < 4)
		bodyque_size = PR_DEFAULT_BODYQUE_SIZE;
	if (bodyque_size > 40)
		bodyque_size = 40;

	#ifdef PR_COOL_GIBS
	st = infokey(world, "cool_gibs"); //
	if (st == "0" || st =="off")
		cool_gibs = 0;
	else
		cool_gibs = 1;
	#endif

	#ifdef PR_KICKABLE_HEADS
	st = infokey(world,"kickable_heads");
	if (st == "0" || st =="off")
		kickable_heads = 0;
	else
		kickable_heads = 1;
	#endif

	#ifdef PR_MENU_SOUNDS
	//if (startup) // Only update on map restart, avoids precache error
	//{

	st = infokey(world,"menu_sounds");
	if (st == "0" || st =="off")
		menu_sounds = 0;
	else
		menu_sounds = 1;

	//}
	#endif

	st = infokey(world, "drop_items"); //
	if (st == "1" || st == "on")
		drop_items = 1;
	else
		drop_items = 0;

	st = infokey(world, "pay_msgs"); //
	if (st == "1" || st =="on")
		pay_msgs = 1;
	else
		pay_msgs = 0;

	st = infokey(world, "team_prefix"); //
	if (st == "0" || st =="off")
		team_prefix = 0;
	else
		team_prefix = 1;

	/*st = infokey(world, "headless"); //
	if (st == "1" || st =="on" )
		headless = 1;
	else
		headless = 0;*/

	st = infokey(world, "no_grapple"); //
	if (st == "1" || st =="on" )
		no_grapple = 1;
	else
		no_grapple = 0;

	/*st = infokey(world, "smooth_sentry");
	if (st == "1" || st == "on")
		smooth_sentry = 1;
	else
		smooth_sentry = 0;*/

	st = infokey(world, "nicecolors"); //
	if (st == "1" || st =="on" )
		nicecolors = 1;
	else
		nicecolors = 0;

	st = infokey(world, "relax_cheatcheck"); //
	if (st == "1" || st =="on" )
		relax_cheatcheck = 1;
	else
		relax_cheatcheck = 0;

	st = infokey(world, "no_punish"); //
	if (st == "1" || st =="on" )
		no_punish = 1;
	else
		no_punish = 0;

	st = infokey(world, "no_votemap"); //
	if (st == "1" || st =="on" )
		no_votemap = 1;
	else
		no_votemap = 0;

	st = infokey(world, "no_detpush"); //
	if (st == "1" || st =="on" )
		no_detpush = 1;
	else
		no_detpush = 0;

	/*st = infokey(world, "no_monstercolors"); //
	if (st == "1" || st =="on" )
		no_monstercolors = 1;
	else
		no_monstercolors = 0;*/

	st = infokey(world, "custom_mode"); //
	if (st==string_null) st="0"; //sets default
	custom_mode = stof(st);
	if (custom_mode < 0 ) custom_mode = 0; // the allowed minimum
	else if (custom_mode > 2) custom_mode = 2; //the allowed maximum

	st = infokey(world, "stock_mode"); //
	if (st==string_null) st="0"; //sets default
	stock_mode = stof(st);
	if (stock_mode < 0 ) stock_mode = 0; // the allowed minimum
	else if (stock_mode > 2) stock_mode = 2; //the allowed maximum

	st = infokey(world, "extras_mode"); //
	if (st==string_null) st="0"; //sets default
	extras_mode = stof(st);
	if (extras_mode < 0 ) extras_mode = 0; // the allowed minimum
	else if (extras_mode > 2) extras_mode = 2; //the allowed maximum

	st = infokey(world, "sniper_factor"); //ARE YOU HAPPY XSETZER?? :P
	if (st==string_null) st="0.85"; //sets default
	sniper_factor = stof(st);
	if (sniper_factor < 0.1 ) sniper_factor = 0.1; // the allowed minimum
	else if (sniper_factor > 2) sniper_factor = 2; //the allowed maximum

	if (stock_mode == 2 && custom_mode == 2) custom_mode = 0;

	AGR_initialize();    // Attackers Go Red mode -PZ
	Invade_initialize(); // Invade mode -PZ
	neo.initialize();    // Neo mode -PZ

	if (agr && invade)
	{
		localcmd("say \"Silly admin enabled AGR and Invade modes at the same time. Disabling both.\"\n");
		agr = invade = 0;
		localcmd("localinfo agr \"\"\n"); localcmd("localinfo invade \"\"\n");
		localcmd("serverinfo agr \"\"\n"); localcmd("serverinfo invade \"\"\n");
	}

	st = infokey(world, "army_delay"); //
#ifdef PR_COOP_MODE_ENHANCED
	if ( st == string_null ) {
		if ( !deathmatch )
			st = "2";
		else
			st = "5";
	}
#else
	if (st==string_null) st="5"; //was 5 ,sets default
#endif
	army_delay = stof(st);
#ifdef PR_COOP_MODE_ENHANCED
	if ( !deathmatch ) {
		if ( army_delay < 1 )
			army_delay = 1; // the allowed minimum
		else if ( army_delay > 60 )
			army_delay = 60; //the allowed maximum
	} else {
		if ( army_delay < 2 )
			army_delay = 2; // the allowed minimum
		else if ( army_delay > 60 )
			army_delay = 60; //the allowed maximum
	}
#else
	if (army_delay < 2 ) army_delay = 2; // the allowed minimum
	else if (army_delay > 60) army_delay = 60; //the allowed maximum
#endif

	st = infokey(world, "allow_watermonsters"); //
	if (st == "1" || st =="on" || mapname == "frontlin") //
		allow_watermonsters = 1;
	else
		allow_watermonsters = 0;

	st = infokey(world, "spec_menu"); //
	if (st == "0" || st =="off")
		spec_menu = 0;
	else
		spec_menu = 1;

	// Gets amount of money
	st = infokey(world, "m");
	if (st == string_null)
		st = infokey(world, "money");
	float numba;
	numba = stof(st);
	if (numba)
		custom_money = numba;
	else
		custom_money = PR_DEFAULT_MONEY;

	if (startup) // Map startup?
	{
		// Retrieve and do precaches if needed for megatf stuff..
		InitMegaTF();

		// Report initialization has been done
		RPrint("INFO: Prozac-specific localinfos retrieved and initialized.\n");
	}
	else
	{
		// Retrieve environment settings and apply them
		float oldstorm, oldeq, olddt, oldrunes;

		oldstorm = storm;
		oldeq = earthquake;
		olddt = daytime;
		oldrunes = runes;

		GetEnvironmentGlobals();

		UpdateEnvironment(oldstorm,oldeq,olddt,oldrunes);

		if (!ceasefire && !specialstate)
			World_FadeIn(0,0,0);

		RPrint("INFO: Localinfos updated successfully.\n");
	}
}

//====================================================================//
// Sprints to all the members on one team except one the Team> Prefix //
// It should be used prior any teamsprint (look at tforttm.qc)		  //
//====================================================================//

void teamprefixsprint(float tno, entity ignore)
{
	if ( team_prefix != 1 )
	  return;

	entity te;

	// Don't do teamprints in DM
	if (tno == 0)
		return;

	te = find(world, "classname", "player");
	while (te != world)
	{
		if (Teammate(te, tno) && te != ignore)
			sprint(te, PR_PRINT_HIGH, "Team\x8D ");

		te = find(te, "classname", "player");
	}
}

//==========================================================
// same shit but ignores 2 players

void teamprefixsprintbi(float tno, entity ignore, entity ignore2)
{
	if ( team_prefix != 1 )
	  return;

	entity te;

	// Don't do teamprints in DM
	if (tno == 0)
		return;

	te = find(world, "classname", "player");
	while (te != world)
	{
		if (Teammate(te, tno) && te != ignore && te != ignore2)
		{
			sprint(te, PR_PRINT_HIGH, "Team\x8D ");
		}

		te = find(te, "classname", "player");
	}
}

//- For better performance i added this: --//
void teamsprint6(entity ignore, const string& st, const string& st2, const string& st3, const string& st4, const string& st5, const string& st6)
{
	entity te;

	te = find(world, "classname", "player");
	while (te != world)
	{
		if (Teammate(te, ignore) && te != ignore)
		{
			sprint(te, PR_PRINT_HIGH, st);
			sprint(te, PR_PRINT_HIGH, st2);
			sprint(te, PR_PRINT_HIGH, st3);
			sprint(te, PR_PRINT_HIGH, st4);
			sprint(te, PR_PRINT_HIGH, st5);
			sprint(te, PR_PRINT_HIGH, st6);
		}

		te = find(te, "classname", "player");
	}
}

//==========================================================
// same shit but ignores 2 players

void teamsprintbi(entity ignore, entity ignore2, const string& st, const string& st2, const string& st3, const string& st4, const string& st5, const string& st6)
{
	entity te;

	te = find(world, "classname", "player");
	while (te != world)
	{
		if (Teammate(te, ignore) && te != ignore && te != ignore2)
		{
			sprint(te, PR_PRINT_HIGH, st);
			sprint(te, PR_PRINT_HIGH, st2);
			sprint(te, PR_PRINT_HIGH, st3);
			sprint(te, PR_PRINT_HIGH, st4);
			sprint(te, PR_PRINT_HIGH, st5);
			sprint(te, PR_PRINT_HIGH, st6);
		}

		te = find(te, "classname", "player");
	}
}

//================================================//
// Kicks any player with no TF skin on tforttm.qc //
//================================================//

float Is_TF_Skin(const string& skin_str)
{
   // if relax_cheatcheck is "on" return its a valid skin
   if (relax_cheatcheck==1) return PR_TRUE;

   if ( skin_str != "tf_scout"
	&& skin_str != "tf_snipe"
	&& skin_str != "tf_sold"
	&& skin_str != "tf_demo"
	&& skin_str != "tf_medic"
	&& skin_str != "tf_hwguy"
	&& skin_str != "tf_pyro"
	&& skin_str != "tf_spy"
	&& skin_str != "tf_eng")
		return PR_FALSE;
   else
		return PR_TRUE;
}

//============================================================================//
// Return the new "NICE" color for the team corresponding to the no passed in //
//============================================================================//

string TeamGetNiceColor(float tno)
{
	// Neo mode
	if (neo.isModeActive()) return "";
	if (tno == 1)
		return "2 13";
	if (tno == 2)
		return "9 4";
	if (tno == 3)
		return "5 12";
	if (tno == 4)
		return "3 11";

	return "";
}

float IsValidTopColor(float tno, float theColor)
{
	// Neo mode
	if (neo.isModeActive()) return true;
	theColor = floor(theColor);

	// if relax_cheatcheck is "on" return its a valid color value
	if (relax_cheatcheck==1)
	{
		if (theColor > 13 || theColor < 0)
			return PR_FALSE;

		if (tno == 1)
		{
			if (theColor == 4)
				return PR_FALSE;

			if (number_of_teams > 2 && theColor == 12)
				return PR_FALSE;
			if (number_of_teams > 3 && theColor == 11)
				return PR_FALSE;
		}

		if (tno == 2)
		{
			if (theColor == 13)
				return PR_FALSE;

			if (number_of_teams > 2 && theColor == 12)
				return PR_FALSE;
			if (number_of_teams > 3 && theColor == 11)
				return PR_FALSE;
		}

		if (tno == 3)
		{
			if (theColor==4||theColor==13)
				return PR_FALSE;

			if (number_of_teams > 3 && theColor == 11)
				return PR_FALSE;
		}

		if (tno == 4 && (theColor==4||theColor==13||theColor==12))
			return PR_FALSE;

		return PR_TRUE;
	}

	if (tno == 1 && (theColor==2||theColor==13))
		return PR_TRUE;
	if (tno == 2 && (theColor==9||theColor==4))
		return PR_TRUE;
	if (tno == 3 && (theColor==12||theColor==5))
		return PR_TRUE;
	if (tno == 4 && (theColor==11||theColor==3))
		return PR_TRUE;

	return PR_FALSE;
}


//==========================//
// The new holograph device //
//==========================//
void UpdateCells(entity player)
{
	/*if (player.current_weapon == #WEAP_SPANNER
	|| player.current_weapon == #WEAP_LIGHTNING
	|| player.current_weapon == #WEAP_FLAMETHROWER
	|| player.current_weapon == #WEAP_DAEDALUS
	|| player.current_weapon == #WEAP_LASERCANNON
	|| player.current_weapon == #WEAP_AIRF)
	{
		player.currentammo = player.ammo_cells;
		player.items = player.items | #IT_CELLS;
	}*/

	if (player->items & PR_IT_CELLS)
		player->currentammo = player->ammo_cells;
	else if (player->current_weapon == PR_WEAP_SPANNER)
		player->currentammo = player->ammo_cells;
}

void ActivateHolo(entity player)
{
	if (!(player->cutf_items & PR_CUTF_HOLO))
	{
		//sprint(player, #PRINT_MEDIUM, "You do not have the holograph!\n");
		return;
	}

	if (player->has_holo == 2) return; // avoids overflow, turns to 1 after first think

	if (player->ammo_cells < 10 && player->has_holo == 0)
	{
		string st;
		sprint(player, PR_PRINT_HIGH, "Your holograph needs ");
		st=ftos(PR_HOLO_POWER_COST);
		sprint(player, PR_PRINT_HIGH, st);
		sprint(player, PR_PRINT_HIGH," cells at least!\n");
		return;
	}

	if (player->has_holo == 1)
	{
		sprint(player, PR_PRINT_HIGH, "You turn off the holograph device\n");
		RemoveHolo(player);
		return;
	}

	newmis = spawn();
	newmis->solid = PR_SOLID_NOT;
	newmis->movetype = PR_MOVETYPE_NOCLIP;
	newmis->origin = player->origin;
	newmis->angles = player->angles;
	newmis->colormap = player->colormap;
	newmis->skin = player->skin;
	setmodel(newmis, "progs/player.mdl");
	newmis->classname = "holo";
	newmis->owner=player;
	newmis->frame=player->frame;
	newmis->nextthink = time + PR_HOLO_CYCLE_TIME;
	newmis->think = HoloThink;
	newmis->effects = PR_EF_DIMLIGHT;
	player->ammo_cells = player->ammo_cells - PR_HOLO_POWER_COST;

	newmis->has_holo = 1; // needed for targeting - has_holo goes to 0 b4 removed
	newmis->team_no = player->team_no;
	newmis->takedamage = PR_DAMAGE_NO;                 // PZ: changed from DAMAGE_NO; need bot to try to attack it (UNDID.. caused crash)
	newmis->real_owner = player;
	newmis->netname = S_("^b???^b");
	newmis->health = 1; // needed for tesla?         // PZ: I uncommented this to get bots to attack holographs.
	setsize(newmis, PR_VEC_HULL_MIN, PR_VEC_HULL_MAX);  // PZ: We also need it to have a size, so that the bots can see it. (Needs size for frik_getRealOriginOfEntity().)
	newmis->flags = newmis->flags | PR_FL_FINDABLE_NONSOLID;  // PZ: To allow bots to see it. (traceline() can't see non-solids, currently, without this.)

	UpdateCells(player);

	stuffcmd (player, "bf\nbf\n");
	sprint(player,PR_PRINT_HIGH,"you turn on your holograph...\n");
	//sound (newmis, #CHAN_MISC, "effects/bodyhit2.wav", 0.5, #ATTN_NORM);
	player->has_holo = 2; // 2 makes it wait until first think for turning off the holo
}

void HoloThink()
{
	//local entity oldself;

	if (self->owner->ammo_cells < PR_HOLO_CYCLE_COST)
	{
		sprint(self->owner, PR_PRINT_MEDIUM, "your hologram runs out of energy\n");
		//sound (self, #CHAN_MISC, "effects/bodyhit1.wav", 0.6, #ATTN_NORM);
		spawnFOG(self->origin);
		self->owner->has_holo = 0;
		self->has_holo = 0; // notifies to "targeters" as a dead holo
		//self.health = 0;
		dremove(self);
		return;
	}

	self->owner->ammo_cells = self->owner->ammo_cells - PR_HOLO_CYCLE_COST;

	UpdateCells(self->owner);

	self->nextthink = time + PR_HOLO_CYCLE_TIME;
	self->owner->has_holo = 1; // ready to be turned off
}

void RemoveHolo(entity player)
{
	entity te;
	te = find(world, "classname", "holo");
	while (te != world)
	{
		if (te->owner == player) {
		  te->owner->has_holo = 0;
		  //sound (te, #CHAN_MISC, "effects/bodyhit1.wav", 0.6, #ATTN_NORM);
		  spawnFOG(te->origin);
		  te->has_holo = 0;
		  //te.health = 0;
		  dremove(te);
		  return;
		}

		te = find(te, "classname", "holo");
	}
}

//===========================//
// THE FLARES ARE BACK! hehe //
//===========================//

void FlareBounce();

//---------------------------//
// Flare touch function.	 //

void FlareGrenadeTouch()
{
	if (other == self->owner)
		return; 	// don't bounce on owner

	if (pointcontents(self->origin) == PR_CONTENT_SKY || pointcontents(self->origin) == PR_CONTENT_SOLID) // if in wall or sky
	{
	   dremove(self);
	   return;
	}

	if (self->has_holo==0)
	{
		self->skin=1;
		//setmodel (self, "progs/flare.mdl");
	}

	if (other == world && self->movetype != PR_MOVETYPE_BOUNCE)
	{
		self->velocity = V({0, 0, 0});

		/*
		if (random()<0.6)
		{
		   sound (self, #CHAN_MISC, "effects/bodyhit2.wav", 0.7, #ATTN_NORM);	// bounce sound
		}
		else
		{
		   sound (self, #CHAN_MISC, "effects/bodyhit1.wav", 0.6, #ATTN_NORM);	// bounce sound
		}*/
	}
	else
	{
		self->movetype = PR_MOVETYPE_BOUNCE;
		self->avelocity = V({1000, 200, 850});
		self->touch = FlareBounce;

	}

	if (self->velocity == V({0, 0, 0}))
		self->avelocity = V({0, 0, 0});

}

void FlareBounce()
{
	if (other == self->owner)
		return; 	// don't bounce on owner


	if (self->velocity == V({0, 0, 0}))
		self->avelocity = V({0, 0, 0});
}

//---------------------------//
// Flare grenade explosion.  //

void FlareGrenadeExplode()
{
	sound (self, PR_CHAN_MISC, "items/flare1.wav", 1, PR_ATTN_NORM);

	//setmodel (self, "progs/flare.mdl");
	self->skin=0;
	self->has_holo=1;

	vector org;
	org=self->origin;

		WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_BROADCAST, PR_TE_GUNSHOT);
		#ifdef PR_QUAKE_WORLD
			WriteByte (PR_MSG_MULTICAST, 3);
		#endif
		WriteCoord (PR_MSG_BROADCAST, org[X]);
		WriteCoord (PR_MSG_BROADCAST, org[Y]);
		WriteCoord (PR_MSG_BROADCAST, org[Z]);
		#ifdef PR_QUAKE_WORLD
			multicast (org, PR_MULTICAST_PVS);
		#endif

	self->effects = self->effects | PR_EF_DIMLIGHT;

	self->think = SUB_Remove;
	self->nextthink = time + PR_FLARE_DURATION;
}

//========================================================//
// My stupid contributions to optimize something!		  //
//========================================================//

float IsMonster( entity thing) {
	if ( thing->flags & PR_FL_MONSTER )
		return PR_TRUE;

	return PR_FALSE;
}

float IsOwnedMonster(entity thing)
{
/*
	if (thing.classname=="monster_shambler")
		return #TRUE;
	if (thing.classname=="monster_demon1")
		return #TRUE;
	if (thing.classname=="monster_wizard")
		return #TRUE;
	if (thing.classname=="monster_army")
		return #TRUE;
	if (thing.classname=="monster_fish")
		return #TRUE;
	if (thing.classname=="monster_gremlin")
		return #TRUE;
*/

	if ( thing->flags & PR_FL_MONSTER )
	if ( thing->real_owner != world )	// Gizmo - added this because not all monsters are pets
		return PR_TRUE;

	return PR_FALSE;
}

float IsSummon(entity thing)
{
/*
	if (thing.classname=="monster_shambler")
		return #TRUE;
	if (thing.classname=="monster_demon1")
		return #TRUE;
	if (thing.classname=="monster_wizard")
		return #TRUE;
	if (thing.classname=="monster_fish")
		return #TRUE;
	if (thing.classname=="monster_gremlin")
		return #TRUE;
*/

	if ( thing->flags & PR_FL_MONSTER )
	if ( thing->real_owner != world )	// Gizmo - added this because not all monsters are pets
	if ( thing->classname != "monster_army" )
		return PR_TRUE;

	return PR_FALSE;
}

float HasMonster(entity player)
{
	float res;
	res = 0;

	if ( player->job & PR_JOB_WARLOCK
		|| player->job & PR_JOB_ARMY )
	{
		if (player->demon_one != world)
			res = 1;
		if (player->demon_two != world)
			res = res + 1;
		if (player->demon_three != world)
			res = res + 1;
	}

	return res;
}

string GetMonsterName(entity themonster)
{
	if (themonster->classname == "monster_wizard")
		return "scrag";
	if (themonster->classname == "monster_shambler")
		return "shambler";
	if (themonster->classname == "monster_demon1")
		return "fiend";
	if (themonster->classname == "monster_army")
		return "army soldier";
	if (themonster->classname == "monster_fish")
		return "piranha";
	if (themonster->classname == "monster_gremlin")
		return "gremlin";

#ifdef PR_COOP_MODE_ENHANCED
	if ( COOP_IsCoopMonster( themonster ) )
		return themonster->netname;
#endif

	return "unknown monster";
}

void MonsterDie(entity themonster)
{
	//Needed for "die" functions
	entity oself;
	oself=self;

	self=themonster;

	/*if (themonster.classname == "monster_wizard")
		wiz_die();
	else if (themonster.classname == "monster_shambler")
		custom_shambler_die();
	else if (themonster.classname == "monster_demon1")
		custom_demon_die();
	else if (themonster.classname == "monster_army")
		custom_grunt_die();
	else if (themonster.classname == "monster_fish")
		fish_death();
	else if (themonster.classname == "monster_gremlin")
		gremlin_die();*/

	if (self->th_die != SUB_Null)
		self->th_die();

	self=oself;
}

string GetBuildingName(entity thebuilding)
{
	if (thebuilding->classname == "building_dispenser")
		return "dispenser";
	if (thebuilding->classname == "building_sentrygun")
		return "sentry gun";
	if (thebuilding->classname == "building_tesla")
		return "tesla sentry";
	if (thebuilding->classname == "building_sensor")
		return "motion sensor";
	if (thebuilding->classname == "building_camera")
		return "security camera";
	if (thebuilding->classname == "building_teleporter")
		return "teleporter";
	if (thebuilding->classname == "building_fieldgen")
		return "field generator";

	return "unknown machine";
}

// soldiers don't target unoffensive buildings, or cloaked teslas!
float IsOffenseBuilding(entity thebuilding)
{
	if (!IsBuilding(thebuilding))
		return PR_FALSE;

	if (thebuilding->classname == "building_sentrygun")
		return PR_TRUE;

	if (thebuilding->classname == "building_tesla")
	{
		if (thebuilding->tf_items & PR_NIT_TESLA_CLOAKING)
		{
			if (thebuilding->job == 2)
				return PR_TRUE; // tesla is uncloaked, so soldier can see it
		}
		else
		{
			return PR_TRUE;
		}
	}

	return PR_FALSE;
}

string GetEnemyName(entity thething)
{
	if (thething->classname == "player")
		return thething->netname;

	if (IsMonster(thething))
		return GetMonsterName(thething);

	if (IsBuilding(thething))
		return GetBuildingName(thething);

	if (thething->classname == "grenade" && thething->netname == "land_mine")
		return "land mine";

	if (thething->netname != "")
		return thething->netname;

	if (thething->classname != "")
		return thething->classname;

	return "unknown stuff";
}

/*
===============
PrintOwnedMonsterInfo

Prints basic information about the specified owned monster.
===============
*/
void PrintOwnedMonsterInfo( entity printDest, entity thething) {
	if ( IsSummon( thething ) )
		Summon_PrintDetails( printDest, thething );
	else
		Grunty_PrintDetails( printDest, thething, 0 );
}

/*
===============
PrintOwnedMonsterDetails

Prints detailed information about the specified owned monster.
===============
*/
void PrintOwnedMonsterDetails( entity printDest, entity thething) {
	if ( IsSummon( thething ) )
		Summon_PrintDetails( printDest, thething );
	else
		Grunty_PrintDetails( printDest, thething, 2 );
}

//======================================================//
void ExpBodyThink();

void ExpBody(entity body)
{
	newmis=spawn();
	newmis->owner=body;
	newmis->think=ExpBodyThink;
	newmis->nextthink=time;
	newmis->origin=body->origin;
	newmis->origin[Z] = newmis->origin[Z] - 46;
	newmis->origin[X] = newmis->origin[X] - crandom()*6;
	newmis->origin[Y] = newmis->origin[Y] - crandom()*6;
}

void ExpBodyThink()
{
	if (self->owner->classname != "player" || !self->owner->is_connected)
	{
		dremove(self);
		return;
	}

	TF_T_Damage(self->owner, self->owner, self->owner->martyr_enemy, self->owner->health + 60, PR_TF_TD_IGNOREARMOUR, PR_TF_TD_OTHER);// #TF_TD_OTHER);

	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
	WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
  #ifdef PR_QUAKE_WORLD
	multicast (self->origin, PR_MULTICAST_PHS);
  #endif

	deathmsg = PR_DMSG_EXPBODY;
	T_RadiusDamage (self->owner, self->owner, PR_EXPBODY_DMG, self->owner);

	dremove(self);
}

//===========================================//

void spawnFOG(const vector& where)
{
	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_TELEPORT);
	WriteCoord (PR_MSG_BROADCAST, where[X]);
	WriteCoord (PR_MSG_BROADCAST, where[Y]);
	WriteCoord (PR_MSG_BROADCAST, where[Z]);
#ifdef PR_QUAKE_WORLD
	multicast (where, PR_MULTICAST_PHS);
#endif
}

//=========================================================//
// called on any try to dismantle something from menu.qc
// returns true if the thing should be dismantled
float CheckEnemyDismantle()
{
	// PZ NOTE: The next line returns #TRUE so that players can dismantle their own devices.
	if (Teammate(self->building->real_owner, self)) return PR_TRUE;

	string st;
	st=GetBuildingName(self->building);

	if (GetICELevel(self->building) == 1 && random() > 0.25)
	{
		sprint(self, PR_PRINT_HIGH, "This ");
		sprint(self, PR_PRINT_HIGH, st);
		sprint(self, PR_PRINT_HIGH, " isn't easy to dismantle, it has a level 1 ICE!\n");

		sprint(self->building->real_owner, PR_PRINT_HIGH, "Somebody is trying to dismantle your ");
		sprint(self->building->real_owner, PR_PRINT_HIGH,st);
		sprint(self->building->real_owner, PR_PRINT_HIGH, "!\n");

		return PR_FALSE;
	}
	else if (GetICELevel(self->building) == 2)
	{
		sprint(self, PR_PRINT_HIGH, "You can't dismantle this ");
		sprint(self, PR_PRINT_HIGH, st);
		sprint(self, PR_PRINT_HIGH, ", it's protected with a level 2 ICE!\n");

		sprint(self->building->real_owner, PR_PRINT_HIGH, "Somebody tried to dismantle your ");
		sprint(self->building->real_owner, PR_PRINT_HIGH,st);
		sprint(self->building->real_owner, PR_PRINT_HIGH, "!\n");

		return PR_FALSE;
	}
	else if (GetICELevel(self->building) == 3)
	{
		sprint(self, PR_PRINT_HIGH, "This ");
		sprint(self, PR_PRINT_HIGH, st);
		sprint(self, PR_PRINT_HIGH, " is protected with a level 3 ICE!\n");
		deathmsg=PR_DMSG_ANTIDISM;
		TF_T_Damage(self, self, self->building, self->health + 50, PR_TF_TD_IGNOREARMOUR, PR_TF_TD_OTHER);
		sound (self, PR_CHAN_MISC, "effects/crunch.wav", 1, PR_ATTN_NONE);

		return PR_FALSE;
	}

	bprint(PR_PRINT_MEDIUM, self->building->real_owner->netname);
	bprint(PR_PRINT_MEDIUM, "'s ");
	bprint(PR_PRINT_MEDIUM, st);
	bprint(PR_PRINT_MEDIUM, " has been dismantled by ");
	bprint(PR_PRINT_MEDIUM, self->netname);
	bprint(PR_PRINT_MEDIUM, "\n");

	sprint(self->building->real_owner, PR_PRINT_HIGH, "The enemy has dismantled your ");
	sprint(self->building->real_owner, PR_PRINT_HIGH, st);
	sprint(self->building->real_owner, PR_PRINT_HIGH, "!\n");

#ifdef PR_QUAKE_WORLD
	logfrag(self, self->building->real_owner);
#endif

	self->real_frags = self->real_frags + 1;
	if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
		self->frags = self->real_frags;

	return PR_TRUE;
}

//- OfN - Checks spots are visible between them
float vis2orig(const vector& spot1, const vector& spot2)
{
	traceline (spot1, spot2, PR_TL_BSP_ONLY, world);	// see through other monsters

	if (trace_inopen && trace_inwater)
		return PR_FALSE;			// sight line crossed contents

	if (trace_fraction == 1 && trace_endpos == spot2) //CH just extra check
		return PR_TRUE;

	return PR_FALSE;
}

//=========================================================================
// Return the number of players in game
// PZ - use checkIP option to differentiate IP's of users
//		(counts unique IP's instead of counting players)
// PZ - use noIdlers option to not count players not on a team
//		(has no effect if checkIP is off)
float GetNoPlayers(float checkIP, float noIdlers)
{
	if (!checkIP)
		return num_players;

	// PZ - one part stolen from Often's modified bodyque
	// linked list of ip addresses
	entity ipList; ipList = world; // always the first entity in the list
	entity element; element = world; // varies with traversal of the list
	entity player; player = world;
	string ip;
	float ipCount; ipCount = 0;
	float counter;
	entity pt; pt = world; // first empty slot in list
	float listed; // bool

	/*
	construct the list
	*/

	counter = 1;
	element = spawnServerSide(); // PZ: make it a server-side only entity
	element->ltime = 0;
	element->classname = "playerIP";
	element->modelindex = modelindex_null;
	ipList = element;

	while (counter < num_players)
	{
		element->owner = spawnServerSide(); // PZ: make it a server-side only entity
		element->owner->ltime = 0;
		element->owner->classname = "playerIP";
		element->owner->modelindex = modelindex_null;

		element = element->owner;

		counter = counter + 1;
	}
	element->owner = world;

	/*
	enter IP's into the list - only if they're not already listed
	*/

	player = find(world, "classname", "player");
	while (player != world)
	{
		if (!noIdlers || player->team_no)
		{
			pt = world; // first empty slot in list
			ip = infokey(player, "ip");
			ip = strcat(ip, ""); // this is simply to get around Quake string buffer issue..
			                     // a problem which I don't completely understand at the moment

			counter = 1;
			element = ipList;
			listed = PR_FALSE;
			while (counter <= num_players)
			{
				// is this an empty slot?
				if (element->netname == string_null)
				{
					if (pt == world) // only set pt to this element if pt isn't already set
						pt = element;
				}
				// if this element of the list holds this player's IP ..
				else if (!strcasecmp(element->netname, ip))
				{
					listed = PR_TRUE;
					counter = 9999;  //break;  // PZ: break statements don't seem to work right; so, replaced it
				}
				// else continue looking through list
				element = element->owner;
				counter = counter + 1;
			}
			if (!listed || !player->ishuman) // if the player's IP is not listed, list in first empty slot
			{
				pt->netname = ip;
				ipCount = ipCount + 1;
			}
		}

		player = find(player, "classname", "player");
	}

	/*
	clean-up.. remove list
	*/

	element = find(world, "classname", "playerIP");
	while (element != world)
	{
		dremove(element);
		element = find(element, "classname", "playerIP");
	}

	/*
	return the number of individual IP addresses
	*/

	return ipCount;
	// PZ - end
}

//==================================================================
// prints the overall resutls, best player etc.. on intermission

entity GetBestPlayer()
{
	float bestscore;//, tempscore;
	entity theplayer, search;

	theplayer = world;
	bestscore = 0;

	search = find (world, "classname", "player");
	while (search != world)
	{
		if (search->frags > bestscore)
		{
			bestscore = search->frags;
			theplayer = search;
		}

		search = find (search, "classname", "player");
	}

	return theplayer;
}

entity GetBestKiller()
{
	float bestscore;//, tempscore;
	entity theplayer, search;

	theplayer = world;
	bestscore = 0;

	search = find (world, "classname", "player");
	while (search != world)
	{
		if (search->frags - TeamFortress_TeamGetScore(search->team_no) > bestscore && search->team_no > 0)
		{
			bestscore = search->frags - TeamFortress_TeamGetScore(search->team_no);
			theplayer = search;
		}

		search = find (search, "classname", "player");
	}

	return theplayer;
}

// Gizmo
entity GetBestTeamKiller() {
	entity	bestplayer, head;
	float		bestValue;

	bestValue = 0;
	bestplayer = world;

	head = find( world, "classname", "player" );
	while ( head  != world) {
		if ( head->PR_team_damage_dealt > bestValue ) {
			bestValue = head->PR_team_damage_dealt;
			bestplayer = head;
		}

		head = find( head, "classname", "player" );
	}

	return bestplayer;
}
entity GetBestDamager() {
	entity	bestplayer, head;
	float		bestvalue;

	bestplayer	= world;
	bestvalue	= 0;

	head = find( world, "classname", "player" );
	while ( head  != world) {
		if ( head->PR_enemy_damage_dealt > bestvalue ) {
			bestplayer = head;
			bestvalue = head->PR_enemy_damage_dealt;
		}

		head = find( head, "classname", "player" );
	}

	return bestplayer;
}
entity GetBiggestTarget() {
	entity	bestplayer, head;
	float		bestvalue;

	bestplayer	= world;
	bestvalue	= 0;

	head = find( world, "classname", "player" );
	while ( head  != world) {
		if ( head->PR_damage_taken > bestvalue ) {
			bestplayer = head;
			bestvalue = head->PR_damage_taken;
		}

		head = find( head, "classname", "player" );
	}

	return bestplayer;
}

void bprintline()
{
	bprint(PR_PRINT_HIGH,"\n\x9D\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9F\n\n");
}

void PrintResults()
{
	string winteam;
	entity theplayer;

	string tmps;
	float tmpf,tmpf2;

	//bprintline();

	bprint(PR_PRINT_HIGH,S_("\n\x9D\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9F ^bEnd Of Game^b! \x9D\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9F\n\n"));

	// Gizmo - fixed printing this with a zero timelimit
	if (time >= timelimit && timelimit > 0)
	{
		bprint(PR_PRINT_HIGH,"Time limit reached!\n");
	}

	tmpf = time/60;
	tmpf2 = floor(tmpf);
	tmps = ftos(tmpf2);

	bprint(PR_PRINT_HIGH,"Map ");
	bprint(PR_PRINT_HIGH,mapname);
	bprint(PR_PRINT_HIGH," was played for ");
	bprint(PR_PRINT_HIGH,tmps);
	bprint(PR_PRINT_HIGH," minutes\n");

	bprint(PR_PRINT_HIGH,S_("\n\x9D\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9F ^bResults^b: \x9D\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9F\n\n"));

	// PZ: support teamplay 0 (basically, deathmatch) properly
	if (teamplay)
	{
		winteam = GetTrueTeamName(TeamFortress_TeamGetWinner());

		if (winteam != "ERROR")
		{
			bprint(PR_PRINT_HIGH,winteam);
			bprint(PR_PRINT_HIGH,S_(" team ^bWINS^b the game!!\n\n"));
		}

		TeamFortress_TeamShowScores(3,0,0);
		bprint(PR_PRINT_HIGH,"\n"); // PZ
	}

	// best player
	bprint(PR_PRINT_HIGH,S_("^bBest Player^b: "));
	theplayer = GetBestPlayer();

	if (theplayer != world)
	{
		tmpf = floor(theplayer->frags);
		tmps = ftos(tmpf);

		bprint(PR_PRINT_HIGH,theplayer->netname);
		// PZ: support teamplay 0 (basically, deathmatch) properly
		if (teamplay)
		{
			bprint(PR_PRINT_HIGH," (");
			bprint(PR_PRINT_HIGH,GetTrueTeamName(theplayer->team_no));
			bprint(PR_PRINT_HIGH,")");
		}
		bprint(PR_PRINT_HIGH, ", with ");
		bprint(PR_PRINT_HIGH,tmps);
		bprint(PR_PRINT_HIGH," frags");
	}
	else
		bprint(PR_PRINT_HIGH,"None");

	// best killer
	// PZ: this is redundant, with the above, in deathmatch
	if (teamplay)
	{
		bprint(PR_PRINT_HIGH,S_("\n^bKilling Machine^b: "));
		theplayer = GetBestKiller();

		if (theplayer != world)
		{
			tmpf = floor(theplayer->frags - TeamFortress_TeamGetScore(theplayer->team_no));
			tmps = ftos(tmpf);

			bprint(PR_PRINT_HIGH,theplayer->netname);
			// PZ: support teamplay 0 (basically, deathmatch) properly
			if (teamplay)
			{
				bprint(PR_PRINT_HIGH," (");
				bprint(PR_PRINT_HIGH,GetTrueTeamName(theplayer->team_no));
				bprint(PR_PRINT_HIGH,")");
			}
			bprint(PR_PRINT_HIGH,", with ");
			bprint(PR_PRINT_HIGH,tmps);
			bprint(PR_PRINT_HIGH," kills");
		}
		else
			bprint(PR_PRINT_HIGH,"None");
	}

	// Gizmo - player that dealt the most damage
	bprint(PR_PRINT_HIGH,S_("\n^bMost Damage^b: "));
	theplayer = GetBestDamager();

	if (theplayer != world)
	{
		tmpf = floor(theplayer->PR_enemy_damage_dealt);
		tmps = ftos(tmpf);

		bprint(PR_PRINT_HIGH,theplayer->netname);
		// PZ: support teamplay 0 (basically, deathmatch) properly
		if (teamplay)
		{
			bprint(PR_PRINT_HIGH," (");
			bprint(PR_PRINT_HIGH,GetTrueTeamName(theplayer->team_no));
			bprint(PR_PRINT_HIGH,")");
		}
		bprint(PR_PRINT_HIGH,", with ");
		bprint(PR_PRINT_HIGH,tmps);
		bprint(PR_PRINT_HIGH," damage dealt to enemies");
	}
	else
		bprint(PR_PRINT_HIGH,"None");

	// Gizmo - best team killer
	// PZ: support teamplay 0 (basically, deathmatch) properly
	if (teamplay)
	{
		bprint(PR_PRINT_HIGH,S_("\n\n^bTeam Killing Machine^b: "));
		theplayer = GetBestTeamKiller();

		if (theplayer != world)
		{
			tmpf = floor(theplayer->PR_team_damage_dealt);
			tmps = ftos(tmpf);

			bprint(PR_PRINT_HIGH,theplayer->netname);
			bprint(PR_PRINT_HIGH," (");
			bprint(PR_PRINT_HIGH,GetTrueTeamName(theplayer->team_no));
			bprint(PR_PRINT_HIGH,"), with ");
			bprint(PR_PRINT_HIGH,tmps);
			bprint(PR_PRINT_HIGH," team-damage dealt");
		}
		else
			bprint(PR_PRINT_HIGH,"None");
	}

	// Gizmo - player that took the most damage
	bprint(PR_PRINT_HIGH,S_("\n^bBiggest Target^b: "));
	theplayer = GetBiggestTarget();

	if (theplayer != world)
	{
		tmpf = floor(theplayer->PR_damage_taken);
		tmps = ftos(tmpf);

		bprint(PR_PRINT_HIGH,theplayer->netname);
		// PZ: support teamplay 0 (basically, deathmatch) properly
		if (teamplay)
		{
			bprint(PR_PRINT_HIGH," (");
			bprint(PR_PRINT_HIGH,GetTrueTeamName(theplayer->team_no));
			bprint(PR_PRINT_HIGH,")");
		}
		bprint(PR_PRINT_HIGH,", with ");
		bprint(PR_PRINT_HIGH,tmps);
		bprint(PR_PRINT_HIGH," damage taken");
	}
	else
		bprint(PR_PRINT_HIGH,"None");

	bprint(PR_PRINT_HIGH,"\n");
	bprintline();
}

//======================================================================================
// called on clientobituary to check if warlock knife kills should be increased

void MonsterKill(entity attacker)
{
	if (attacker->job & PR_JOB_WARLOCK)
	if (deathmsg == PR_DMSG_AXE || deathmsg == PR_DMSG_BACKSTAB)
		WarlockKnifeKill(attacker);
}

//===============================================================================
// this function returns the max amount of grens a player can carry

float GetMaxGrens(entity theplayer, float grenslot)
{
	#ifdef PR_GRENADES_TEST
	return 999;
	#endif

	if (theplayer->tf_items & PR_NIT_AMMO_BANDOLIER) // player can carry more grens cause he got bandolier
	{
		if (grenslot == 2) //2nd slot
		{
			if (theplayer->tp_grenades_2 == 0)
				return 0;

			#ifndef PR_OLD_FLASH
			if (theplayer->tp_grenades_2 == PR_GR_TYPE_PSIONIC)
				return 4;
			#endif

			if (theplayer->tp_grenades_2 == PR_GR_TYPE_NAIL)
				return 3;// was 1
			if (theplayer->tp_grenades_2 == PR_GR_TYPE_FRAG)
				return 4;// was 1
			if (theplayer->tp_grenades_2 == PR_GR_TYPE_CALTROP)
				return 4;// was 1
			if (theplayer->tp_grenades_2 == PR_GR_TYPE_MIRV)
				return 3;// was 1
			if (theplayer->tp_grenades_2 == PR_GR_TYPE_FLARE)
				return 6;
			if (theplayer->tp_grenades_2 == PR_GR_TYPE_NAPALM)
				return 3;

			return 5;
		}
		else // first slot, or bug :)
		{
			if (theplayer->tp_grenades_1 == 0)
				return 0;

			#ifndef PR_OLD_FLASH
			if (theplayer->tp_grenades_1 == PR_GR_TYPE_PSIONIC)
				return 4;
			#endif

			if (theplayer->tp_grenades_1 == PR_GR_TYPE_NAIL)
				return 3;// was 1
			if (theplayer->tp_grenades_1 == PR_GR_TYPE_FRAG)
				return 4;// was 1
			if (theplayer->tp_grenades_1 == PR_GR_TYPE_CALTROP)
				return 4;// was 1
			if (theplayer->tp_grenades_1 == PR_GR_TYPE_MIRV)
				return 3;// was 1
			if (theplayer->tp_grenades_1 == PR_GR_TYPE_FLARE)
				return 6;
			if (theplayer->tp_grenades_1 == PR_GR_TYPE_NAPALM)
				return 3;

			return 5;
		}
	}

	if (grenslot == 2) //2nd slot
	{
		if (theplayer->tp_grenades_2 == 0)
			return 0;

		#ifndef PR_OLD_FLASH
		if (theplayer->tp_grenades_2 == PR_GR_TYPE_PSIONIC)
			return 3;
		#endif

		if (theplayer->tp_grenades_2 == PR_GR_TYPE_NAIL)
			return 2;// was 1
		if (theplayer->tp_grenades_2 == PR_GR_TYPE_FRAG)
			return 3;// was 1
		if (theplayer->tp_grenades_2 == PR_GR_TYPE_CALTROP)
			return 3;// was 1
		if (theplayer->tp_grenades_2 == PR_GR_TYPE_MIRV)
			return 2;// was 1
		if (theplayer->tp_grenades_2 == PR_GR_TYPE_FLARE)
			return 5;
		if (theplayer->tp_grenades_2 == PR_GR_TYPE_NAPALM)
			return 2;

		return 4;
	}
	else // first slot, or bug :)
	{
		if (theplayer->tp_grenades_1 == 0)
			return 0;

		#ifndef PR_OLD_FLASH
		if (theplayer->tp_grenades_1 == PR_GR_TYPE_PSIONIC)
			return 3;
		#endif

		if (theplayer->tp_grenades_1 == PR_GR_TYPE_NAIL)
			return 2;// was 1
		if (theplayer->tp_grenades_1 == PR_GR_TYPE_FRAG)
			return 3;// was 1
		if (theplayer->tp_grenades_2 == PR_GR_TYPE_CALTROP)
			return 3;// was 1
		if (theplayer->tp_grenades_1 == PR_GR_TYPE_MIRV)
			return 2;// was 1
		if (theplayer->tp_grenades_1 == PR_GR_TYPE_FLARE)
			return 5;
		if (theplayer->tp_grenades_1 == PR_GR_TYPE_NAPALM)
			return 2;

		return 4;
	}

	return 0; // shouldnt happen ever.
}

//=====================================================//

/*void(entity attacker) Check_PainInflictor =
{
	if (self.health <= 0)
		return;

	local entity real_attacker;
	real_attacker = attacker;

	if (!IsMonster(attacker) && attacker.classname!="player")
	{
		if (!IsMonster(atta
	}


	if (self.enemy!=self && self.enemy!=world && self.enemy!=attacker && self.enemy!=attacker.owner)
	{
		if (!visible(self.enemy))
		{


		}
	}
};*/
/*

void(entity player) SwitchToCamera =
{
	if (player.classname!="player")
	{
		RPrint("OFTEN BUG REPORT: Object '");
		RPrint(player.classname);
		RPrint("' in SwitchToCamera()\n");
		return;
	}

	local entity camera;
	local float done;

	if (!player.has_camera)
		return;
	if (player.is_cameraviewing)
		return;

	 // FIXME: no inair, no water, no moving, no haxxxoring, no building, no detpacking, no throwing a det,
	 // no feinginG? (special)

	camera = find(world, classname, "building_camera");
	if (camera.real_owner == player)
		done = #TRUE;
	while (!done)
	{
		camera = find(camera, classname, "building_camera");
		if (camera.real_owner == player)
			done = #TRUE;
		if (camera == world)
			done = #TRUE;
	}

	if (camera == world)
		return;

	/COMMENT*

	msg_entity = player;
	WriteByte(#MSG_ONE, #SVC_SETVIEWPORT);
	WriteEntity(#MSG_ONE, camera);
	WriteByte(#MSG_ONE, #SVC_SETANGLES);
	WriteAngle(#MSG_ONE, camera.angles_x);
	WriteAngle(#MSG_ONE, camera.angles_y);
	WriteAngle(#MSG_ONE, camera.angles_z);

	*COMMENT/

	/COMMENT*

	//- OfN create the "fake" player image
	newmis=spawn();
	newmis.solid = #SOLID_BBOX; //#SOLID_BSP
	newmis.movetype = #MOVETYPE_NONE;
	newmis.takedamage = #DAMAGE_AIM;
	newmis.origin = player.origin;
	newmis.angles = player.angles;
	newmis.colormap = player.colormap;
	newmis.skin = player.skin;
	setmodel (newmis, "progs/player.mdl");
	//setsize (newmis,????); // NEEDED?
	//setorigin (newmis, newmis.origin); // NEEDED?
	newmis.classname = "fake_player";
	newmis.owner=player;
	newmis.frame=player.frame;

	newmis.th_pain = FakePain;
	newmis.th_die = FakeDie;

	newmis.max_health = 9999;
	newmis.health = 9999;

	//newmis.nextthink = time + #HOLO_CYCLE_TIME;
	//newmis.think = HoloThink;
	//newmis.effects = #EF_DIMLIGHT;
	//player.ammo_cells = player.ammo_cells - #HOLO_POWER_COST;

	player.fixangle = #TRUE;
	player.is_cameraviewing = #TRUE;
	player.t_s_h = player.weaponmodel; // FIXME: reloading�?
	player.weaponmodel= ""; // FIXME: reloading�?
	player.view_ofs = '0 0 0';

	setorigin (player, camera.origin);

	sprint(player, #PRINT_HIGH, "Camera view activated.\n");
};

void(entity player) SwitchFromCamera =
{
	if (player.classname!="player")
	{
		RPrint("OFTEN BUG REPORT: Object '");
		RPrint(player.classname);
		RPrint("' in SwitchFromCamera()\n");
		return;
	}

	if (!player.has_camera)
		return;
	if (!player.is_cameraviewing)
		return;

	/COMMENT*

	msg_entity = player;
	WriteByte(#MSG_ONE, #SVC_SETVIEWPORT);
	WriteEntity(#MSG_ONE, player);
	WriteByte(#MSG_ONE, #SVC_SETANGLES);
	WriteAngle(#MSG_ONE, player.angles_x);
	WriteAngle(#MSG_ONE, player.angles_y);
	WriteAngle(#MSG_ONE, player.angles_z);

	*COMMENT/

	/COMMENT*

	player.fixangle = #FALSE;

	player.weaponmodel = player.t_s_h; // FIXME: reloading�?
	player.view_ofs = '0 0 22';

	player.is_cameraviewing = #FALSE;
};

*/

//==========================================================================================================
// used on players after teleporting to a turretized teleport to see if its stuck in wall or outside world

float IsOutWorld(entity player)
{
	if (pointcontents(player->origin) == PR_CONTENT_SOLID || pointcontents(player->origin) == PR_CONTENT_SKY)
		return PR_TRUE;

	if (pointcontents(player->absmax) == PR_CONTENT_SOLID || pointcontents(player->absmax) == PR_CONTENT_SKY)
		return PR_TRUE;

	if (pointcontents(player->absmin) == PR_CONTENT_SOLID || pointcontents(player->absmin) == PR_CONTENT_SKY)
		return PR_TRUE;

	return PR_FALSE;
}

//===================================================================================================
// Removes timers set on anything, used for psionic clean-up on sentries and teslas

/*void(entity thing) RemoveMyTimers =
{
	local entity te;

	te = find(world,classname,"timer");

	while(te)
	{
		if (te.owner == thing)
		{
			te.think = SUB_Remove;
			te.nextthink = time + 0.1;
		}

		te = find(te,classname,"timer");
	}
};*/

//=============================================================
// returns FALSE if tesla/sentry is in invalid location

/*
float(entity gun) IsInValidLocation =
{
	local vector tmpvec;

	if (gun.tf_items & #NIT_TURRET)
	{
		if (pointcontents(gun.origin) == #CONTENT_SOLID || pointcontents(gun.origin) == #CONTENT_SKY)
			return #FALSE;

		tmpvec = gun.absmax - '1 1 41';

		if (pointcontents(tmpvec) == #CONTENT_SOLID || pointcontents(tmpvec) == #CONTENT_SKY)
			return #FALSE;

		tmpvec = gun.absmin + '1 1 -11';

		if (pointcontents(tmpvec) == #CONTENT_SOLID || pointcontents(tmpvec) == #CONTENT_SKY)
			return #FALSE;
	}
	else
	{
		if (pointcontents(gun.origin) == #CONTENT_SOLID || pointcontents(gun.origin) == #CONTENT_SKY)
			return #FALSE;

		tmpvec = gun.absmax - '1 1 1';

		if (pointcontents(tmpvec) == #CONTENT_SOLID || pointcontents(tmpvec) == #CONTENT_SKY)
			return #FALSE;

		tmpvec = gun.absmin + '1 1 1';

		if (pointcontents(tmpvec) == #CONTENT_SOLID || pointcontents(tmpvec) == #CONTENT_SKY)
			return #FALSE;
	}

	return #TRUE;
};*/


float GoodIntermissionImpulse(float imp)
{
	if (imp == PR_TF_STATUS_QUERY ||
	imp == PR_TF_DISPLAYLOCATION	 ||
	imp == PR_TF_STATUS_QUERY ||
	imp == PR_TF_HELP_MAP ||
	imp == PR_TF_SHOWTF ||
	imp == PR_I_CHEAT_ONE ||
	imp == PR_I_CHEAT_TWO ||
	imp == PR_I_CHEAT_THREE ||
	//imp == #IMPULSE_PUNISH ||
	//imp == #IMPULSE_VOTEMAP ||
	imp == PR_IMPULSE_VOTEYES ||
	imp == PR_IMPULSE_VOTENO)
	//inp == #IMPULSE_TESTRANGE)
	return PR_TRUE;

	return PR_FALSE;
}

//==================================================================
// Returns TRUE if a server update is taking place

float ServerUpdating()
{
	if (cvar("sv_updating")==0)
		return PR_FALSE;

	return PR_TRUE;
}

//============================================================
// Reports current server main settings

void PrintGameSettings(float broadcastmsg)
{
	string st, temp;
	float fl, fl2;

	st = strcat(S_("\n\x9D\x9E\x9E\x9E\x9E\x9E\x9E\x9F ^bMain Game Settings^b \x9D\x9E\x9E\x9E\x9E\x9E\x9E\x9F\n^bMap^b: ^b\"^b"),mapname);

	st = strcat(st, S_("^b\"^b \x9C ^bTime Left^b: "));

	if (timelimit <= 0)
		st = strcat(st,"No limit");
	else
	{
		if (time >= timelimit)
			return;

		fl = floor(((timelimit - time)/60)/60);
		temp = ftos(fl);
		st = strcat(st,temp);
		st = strcat(st,S_("^b:^b"));

		fl2 = floor((timelimit - time)/60);
		fl = fl2 - (fl*60);
		temp = ftos(fl);
		if (strlen(temp)< 2)
			st = strcat(st,"0");
		st = strcat(st,temp);
		st = strcat(st,S_("^b:^b"));

		fl = floor(timelimit - time) - (fl2*60);
		temp = ftos(fl);
		if (strlen(temp)< 2)
			st = strcat(st,"0");
		st = strcat(st,temp);
	}

	st = strcat(st,S_("\n^bCustom^b"));

	if (custom_mode == 0)
	{
		temp = ftos(custom_money);
		temp = colstr(temp,PR_COLSTR_NUMBER);

		st = strcat(st,S_(": Enabled \x9C ^bMoney^b: "));
		st = strcat(st,temp);
	}
	else
		st = strcat(st,S_(" ^bClasses^b: Disabled"));

	// first step printing purge (due to strcat limit)
	if (broadcastmsg)
		bprint(PR_PRINT_HIGH,st);
	else
		sprint(self,PR_PRINT_HIGH,st);

	st = "";

	st = strcat(st,S_("\n^bEnvironment^b: "));

	if (!(storm | earthquake | daytime | runes))
		st = strcat(st,"Standard World");
	else
	{
		float first;

		first = PR_TRUE;

		if (storm != 0)
		{
			st = strcat(st,S_("Lightning Storm ^<"));
			temp = ftos(floor(storm*100));
			temp = colstr(temp,PR_COLSTR_NUMBER);
			st = strcat(st,temp);
			st = strcat(st,S_("^b%^b^)"));

			first = PR_FALSE;
		}

		if (earthquake != 0)
		{
			if (first)
				st = strcat(st,S_("EarthQuakes ^<"));
			else
				st = strcat(st,S_("\n             EarthQuakes ^<"));

			temp = ftos(floor(earthquake*100));
			temp = colstr(temp,PR_COLSTR_NUMBER);
			st = strcat(st,temp);
			st = strcat(st,S_("^b%^b^)"));

			first = PR_FALSE;
		}

		// second step printing (due to strcat limit)
		if (broadcastmsg)
			bprint(PR_PRINT_HIGH,st);
		else
			sprint(self,PR_PRINT_HIGH,st);

		st = "";

		if (daytime != 0)
		{
			if (first)
				st = strcat(st,S_("Day Light ^<"));
			else
				st = strcat(st,S_("\n             Day Light ^<"));

			temp = ftos(floor(daytime));
			temp = colstr(temp,PR_COLSTR_NUMBER);
			st = strcat(st,temp);
			st = strcat(st,S_(" ^bdays^b^)"));

			first = PR_FALSE;
		}

		if (runes != 0)
		{
			if (first)
				st = strcat(st,S_("Ancient Runes ^<"));
			else
				st = strcat(st,S_("\n             Ancient Runes ^<"));

			temp = ftos(floor(runes));
			temp = colstr(temp,PR_COLSTR_NUMBER);
			st = strcat(st,temp);
			st = strcat(st,S_("^bpp^b^)"));
		}
	}

	// third step printing (due to strcat limit)
	if (broadcastmsg)
		bprint(PR_PRINT_HIGH,st);
	else
		sprint(self,PR_PRINT_HIGH,st);

	//st = "\n\{157}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{158}\{159}\n\n";
	st = "\n\n";

	// 4th step printing (due to strcat limit)
	if (broadcastmsg)
		bprint(PR_PRINT_HIGH,st);
	else
		sprint(self,PR_PRINT_HIGH,st);
}

//==========================================================================
// Used on TF_T_Damage, returns the outgoing damage multiplier for entity

float GetDamageOutFactor(entity thing)
{
	float factor;
	factor = 1;

	if (thing->super_damage_finished > time) {
		if (thing->tfstate & PR_TFSTATE_INSPIRED)
			factor = factor * PR_CHAPLAN_DMGFACTOR; // was 1.5
		if (thing->job & PR_JOB_BERSERKER && thing->job & PR_JOB_ACTIVE)
			factor = factor * PR_BERSERKER_DMGFACTOR;
		else
			factor = factor * 4; // quad
	}

	if (thing->aura == PR_AURA_POWER)
		factor = factor * PR_POWERAURA_DMGFACTOR; // was 1.5
	// modded for Neo mode
	if (!neo.isModeActive())
	{
		if (thing->PR_runes & PR_RUNE_TRIAD)
			factor = factor * 3;

		if (factor > PR_MAX_OUT_DMGFACTOR)
			factor = PR_MAX_OUT_DMGFACTOR;
	}
	else if (neo.getNeo() == thing)
		factor = factor * neo.getNeoDamageFactor();
	// end Neo mod

	return factor;
}

//==========================================================================
// Used on TF_T_Damage, returns the incoming damage multiplier for entity

float GetDamageInFactor(entity thing)
{
	float factor;
	factor = 1;

	if (thing->aura == PR_AURA_RESIS)
		factor = factor * PR_RESISAURA_DMGFACTOR; // was 0.66

	if (thing->tfstate & PR_TFSTATE_INSPIRED) //Chaplan defense
		factor = factor * PR_CHAPLAN_DEFENSE_DMGFACTOR;

	// modded for Neo mode
	if (!neo.isModeActive())
	{
		if (thing->PR_runes & PR_RUNE_RESIS)
			factor = factor * PR_RESISRUNE_DMGFACTOR;
		
		if (factor < PR_MIN_IN_DMGFACTOR)
			factor = PR_MIN_IN_DMGFACTOR;
	}
	else if (neo.getNeo() == thing)
	{
		factor = factor * neo.getNeoResistFactor();

		if (factor < 0.05)
			factor = 0.05; // make sure he never ends up impossible to kill because of Resist
	}

	if (factor > 1) bprint(PR_PRINT_HIGH, "Error: GetDamageInFactor() result greater than 1\n");
	// end Neo mod

	return factor;
}

//=========================================================================
// Used to muzzle flash any entity

void MuzzleFlash(entity ent)
{
	#ifdef PR_QUAKE_WORLD
	WriteByte (PR_MSG_MULTICAST, PR_SVC_MUZZLEFLASH);
	WriteEntity (PR_MSG_MULTICAST, ent);
	multicast (ent->origin, PR_MULTICAST_PVS);
	#else
	ent->effects = ent->effects | PR_EF_MUZZLEFLASH;
	#endif
}

//===========================================================================
// This is used for any leg damage performed, returns the damage done if any

float HurtLegs(entity player, float damage)
{
#ifdef PR_COOP_MODE_ENHANCED
	// Gizmo - enabled leg damage for coop monsters
	if ( COOP_IsCoopMonster( player ) ) {
		if ( player->monsterflags & PR_MFLAG_NOLEGS || player->monsterflags & PR_MFLAG_NOLEGDAMAGE )
			return 0;

		// decrease monster speed by 10% each wound
		if ( !player->PR_npc_speedfactor )
			player->PR_npc_speedfactor = 0.9;
		else {
			player->PR_npc_speedfactor = player->PR_npc_speedfactor - 0.1;
			if ( player->PR_npc_speedfactor < 0.1 )
				player->PR_npc_speedfactor = 0.1;
		}

		return damage;
	}
#endif
	if (player->classname != "player") return 0;

	if (player->PR_runes & PR_RUNE_RESIS)
		return 0;

	player->leg_damage = player->leg_damage + damage;

	TeamFortress_SetSpeed(player);

	return damage;
}

//==========================================================================
// This restores leg damage on player

void HealLegs(entity player)
{
	if (player->classname != "player") return;

	player->leg_damage = 0;
	TeamFortress_SetSpeed(player);
}

//==========================================================================
// Displays a lesson when a player really needs it :P (ban/kick etc..)

#define PR_FINAL_PREDEF_MSG_TK    "You are supposed to not kill your\nteammates in this game, if you are\nunable to understand that you better\ngo to play on another server.\n"
#define PR_FINAL_PREDEF_MSG_CHEAT "Don't cheat on this server please,\nwe all want to play and have fun here.\nPeople that cheat spoil the game, and\nwill not be tolerated.\n"

void PlayerFinal(entity player, const string& st, float action, float predefined)
{
	if (player->classname != "player")
		return;

	entity oself;
	oself = self;

	self = player;

	self->message = "";

	// Throw gib/head stuff if appropiate only
	if (!(self->done_custom & PR_CUSTOM_BUILDING)) // customizing, so nope
	if (!(self->playerclass == PR_PC_UNDEFINED))	// observing, so nope
	if (self->modelindex == modelindex_player)	// are we visible and on a player model? (not a head)
		GibPlayerOut(); // ok, gib him

	CleanUpEverything();

	// Drop any runes he would carry
	PlayerDropRunes(self);

	player->effects = 0;
	player->job = 0;
	player->current_menu = 0;
	player->items = 0;
	player->weapon = 0;
	player->weapons_carried = 0;
	player->cutf_items = 0;
	player->tf_items = 0;
	player->aura = 0;
	player->takedamage = 0;
	player->tfstate = 0;

	player->flags = player->flags | PR_FL_FINALIZED; // Our server code ignores centerprints for players with this set, any centerprint clears the FINALE text

	// QF guys told me the following does nothing on player entity but..
	self->solid = PR_SOLID_NOT;
	self->movetype = PR_MOVETYPE_NONE;
	//-

	self->modelindex = modelindex_null;
	setsize(self, V({0, 0, 0}), V({0, 0, 0}));
	self->touch = SUB_Null;
	self->velocity = V({0, 0, 0});
	self->gravity = 0;
	self->playerclass = PR_PC_UNDEFINED;

	msg_entity = self;
	WriteByte (PR_MSG_ONE, PR_SVC_FINALE);

	if (predefined == 1)
		WriteString(PR_MSG_ONE,PR_FINAL_PREDEF_MSG_TK);
	else if (predefined == 2)
		WriteString(PR_MSG_ONE,PR_FINAL_PREDEF_MSG_CHEAT);
	else
		WriteString (PR_MSG_ONE, st);

	self->health = 0;
	self->deadflag = PR_DEAD_DEAD;

	// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
	//     themselves, to keep from having to kick them when they do.
	//stuffcmd(self,"color 1 1\n");
	setinfo(self, "topcolor", "1");    // PZ
	setinfo(self, "bottomcolor", "1"); // PZ

	if (action == 1)
		//stuffcmd(self,"team kick\n");
		setinfo(self, "team", "kick"); // PZ
	else if (action == 2)
		//stuffcmd(self,"team ban\n");
		setinfo(self, "team", "ban");  // PZ
	else
		//stuffcmd(self,"team serm\n");
		setinfo(self, "team", "serm"); // PZ

	if (action == 2)
		sprint(self,PR_PRINT_HIGH,"You are going to be banned after reading the sermon, if you disconnect you will be banned anyway, so better read it!\n");

	self->dont_do_triggerwork = action;

	self->ex_skill_max = time;
	self->ex_skill_min = time + PR_FINALIZED_RATE_TEAMFLASH;
	self->option2 = PR_FALSE;

	self = oself;
}

void FinalizedPerformAction()
{
	string foo;

	if (self->dont_do_triggerwork == 2) // Ban
	{
		bprint(PR_PRINT_HIGH, self->netname);
		bprint(PR_PRINT_HIGH, S_(" is ^bBANNED^b after the sermon\n"));

		sprint(self,PR_PRINT_HIGH,S_("\nYou have been ^bBANNED^b from the server!\n"));

		foo = infokey(self,"ip");
		localcmd("addip ");
		localcmd(foo);
		localcmd("\n");

		self->dont_do_triggerwork = 4;

		stuffcmd(self, "disconnect\n"); //Kick them!

		BroadcastSound("player/teledth1");
	}
	else if (self->dont_do_triggerwork == 1) // Kick
	{
		bprint(PR_PRINT_HIGH, self->netname);
		bprint(PR_PRINT_HIGH, S_(" is ^bKICKED^b after the sermon\n"));

		sprint(self,PR_PRINT_HIGH,S_("\nYou have been ^bKICKED^b from the server!\n"));

		stuffcmd(self, "disconnect\n"); //Kick them!

		BroadcastSound("player/teledth1");
	}
	else
	{
		self->dont_do_triggerwork = 4; // Stop team flashes
	}
}

//=========================================================================
// Improved/extended SpawnBlood()

void SpawnBloodEx(const vector& where, float bloodtype, float bloodammount)
{
	if (random() < bloodtype)
	{
		WriteByte (PR_MSG_MULTICAST, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_MULTICAST, PR_TE_LIGHTNINGBLOOD);
		WriteCoord (PR_MSG_MULTICAST, where[X]);
		WriteCoord (PR_MSG_MULTICAST, where[Y]);
		WriteCoord (PR_MSG_MULTICAST, where[Z]);
		multicast (where, PR_MULTICAST_PVS);
	}
	else
	{
		WriteByte (PR_MSG_MULTICAST, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_MULTICAST, PR_TE_BLOOD);
		WriteByte (PR_MSG_MULTICAST, bloodammount);
		WriteCoord (PR_MSG_MULTICAST, where[X]);
		WriteCoord (PR_MSG_MULTICAST, where[Y]);
		WriteCoord (PR_MSG_MULTICAST, where[Z]);
		multicast (where, PR_MULTICAST_PVS);
	}
}

//==========================================================================
// Same thing but for spike particles instead

void SpawnSpikeFX(const vector& where, float spiketype)
{
	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	if (random() < spiketype)
		WriteByte (PR_MSG_BROADCAST, PR_TE_SUPERSPIKE);
	else
		WriteByte (PR_MSG_BROADCAST, PR_TE_SPIKE);
	WriteCoord (PR_MSG_BROADCAST, where[X]);
	WriteCoord (PR_MSG_BROADCAST, where[Y]);
	WriteCoord (PR_MSG_BROADCAST, where[Z]);
	multicast (where, PR_MULTICAST_PHS);
}

//=========================================================================================
// Does effect stuff on the entity ala SpawnBlood but with the specified type of blood
// Also takes into account the type of effect, if player is invincible, or target is
// a machine or bsp solid, no blood and does spike effect stuff instead

void SpawnDmgFX(entity targetent, const vector& where, float spiketype, float bloodtype, float bloodammount)
{
	// If the specified spot is in sky, do nothing
	if (pointcontents(where) == PR_CONTENT_SKY)
		return;

	// handle effect on BSP
	if (targetent == world || targetent->solid == PR_SOLID_BSP)
	{
		SpawnSpikeFX(where,spiketype);
		return;
	}

	// Solid models that doesnt take damage
	if (targetent->takedamage == PR_DAMAGE_NO)
	{
		SpawnSpikeFX(where,spiketype);
		return;
	}

	// handle effect on machines
	if (IsBuilding(targetent))
	{
		SpawnSpikeFX(where,spiketype);
		return;
	}

	// handle effect on monsters
	if (IsMonster(targetent))
	{
		SpawnBloodEx(where,bloodtype,bloodammount);
		return;
	}

	// handle effect on players
	if (targetent->classname == "player")
	{
		if (!targetent->is_connected) return;
		if (targetent->health <= 0) return;
		if (targetent->done_custom & PR_CUSTOM_BUILDING) // skip ppl customizing
			return;
		if (targetent->playerclass == PR_PC_UNDEFINED) // skip observers
			return;

		// If invincible spawn spike particle
		if ((targetent->invincible_finished > time) || (targetent->items & PR_IT_INVULNERABILITY))
			SpawnSpikeFX(where,spiketype);
		else // Otherwise just bleed..
			SpawnBloodEx(where,bloodtype,bloodammount);
	}
}

//========================================================================
// Returns TRUE if a player has enough ammo to build something

float EnoughToBuild(entity player, float buildtype)
{
	if (Invade_buffPlayer(player)) return PR_TRUE;

	if (buildtype == PR_BUILD_SECURITY_CAMERA)
	{
		if (player->ammo_cells >= PR_BUILD_COST_CAMERA)
			return PR_TRUE;
	}
	else if (buildtype == PR_BUILD_SENSOR)
	{
		if (player->ammo_cells >= PR_BUILD_COST_SENSOR)
			return PR_TRUE;
	}
	else if (buildtype == PR_BUILD_DISPENSER)
	{
		if (player->ammo_cells >= PR_BUILD_COST_DISPENSER)
			return PR_TRUE;
	}
	else if (buildtype == PR_BUILD_SENTRYGUN)
	{
		if (player->ammo_cells >= PR_BUILD_COST_SENTRYGUN)
			return PR_TRUE;
	}
	else if (buildtype == PR_BUILD_TESLA)
	{
		if (player->ammo_cells >= PR_BUILD_COST_TESLA)
			return PR_TRUE;
	}
	else if (buildtype == PR_BUILD_FIELDGEN)
	{
		if (player->ammo_cells >= PR_BUILD_COST_FIELDGEN)
			return PR_TRUE;
	}
	else if (buildtype == PR_BUILD_TELEPORTER)
	{
		if (player->ammo_cells >= PR_BUILD_COST_TELEPORTER)
			return PR_TRUE;
	}

	return PR_FALSE;
}

//=============================================================================
// Subroutine to engage a map change, takes stuff into account

void ExecCycleStandard();

void LaunchMap(const string& nfmap)
{
	string temp;
	string nmap;

	bprint(PR_PRINT_HIGH,"\nLoading ");
	bprint(PR_PRINT_HIGH,nfmap);
	bprint(PR_PRINT_HIGH," map file...\n");

	localcmd("localinfo nmap \"\"\n");

	ExecCycleStandard();

	// Set a local indicating map to be executed - used only only for timeout/error timers report
	localcmd("localinfo tmap \"");
	localcmd(nfmap);
	localcmd("\"\n");

	// execute map cfg
	temp = infokey(world, "cycledir");
	if ( temp == string_null )
		localcmd("exec " TO_STR(PR_DEFAULT_CYCLEDIR) "/");
	else
	{
		localcmd("exec ");
		localcmd(temp);
		localcmd("/");
	}

	localcmd(nfmap);
	localcmd(".cfg\n");

	// PZ: execute any <mapname>.cfg file in the cycledir
	// localcmd() doesn't immediately execute, so I made flushcmd().
	flushcmd();                               // make it execute the previous localcmds now
	nmap = infokey(world, "nmap");            // nmap doesn't get set until the above cfg is ran
	if (temp == string_null)
		localcmd("exec " TO_STR(PR_DEFAULT_CYCLEDIR) "/");
	else
	{
		localcmd("exec ");
		localcmd(temp);
		localcmd("/");
	}
	localcmd(nmap);
	localcmd(".cfg\n");

	// Set timeout timer just in case couldn't launch map yet
	SetCycleTimeoutTimer();
}

void ExecCycleStandard()
{
	if (infokey(world,"specialmap")!="") return;

	string temp;

	// Exec "standard.cfg" also to reset stuff if needed if changed by any custom map
	temp = infokey(world, "cycledir");
	if ( temp == string_null )
		localcmd("exec " TO_STR(PR_DEFAULT_CYCLEDIR) "/");
	else
	{
		localcmd("exec ");
		localcmd(temp);
		localcmd("/");
	}

	localcmd("standard.cfg\n");
}

//=============================================================================
// Removes the stuff that is currently disallowed on a player

float StripDisallowed(entity player)
{
	if (!(disabledstuff1 | disabledstuff2 | disabledstuff3 | disabledstuff4 | disabledstuff5))
		return 0;

	float itemid, counter;

	counter = 0;

	if (disabledstuff1)
	{
		itemid = 1;

		while (itemid < 22)
		{
			if (disabledstuff1 & itob(itemid))
			if (StripSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
				counter = counter + 1;

			itemid = itemid + 1;
		}
	}

	if (disabledstuff2)
	{
		itemid = 25;

		while (itemid < 22 + 24)
		{
			if (disabledstuff2 & itob(itemid - 24))
			if (StripSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
				counter = counter + 1;

			itemid = itemid + 1;
		}
	}

	if (disabledstuff3)
	{
		itemid = 25+24;

		while (itemid < 22 + 24 + 24)
		{
			if (disabledstuff3 & itob(itemid - (24+24)))
			if (StripSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
				counter = counter + 1;

			itemid = itemid + 1;
		}
	}

	if (disabledstuff4)
	{
		itemid = 25+24+24;

		while (itemid < 22 + 24 + 24 + 24)
		{
			if (disabledstuff4 & itob(itemid - (24+24+24)))
			if (StripSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
				counter = counter + 1;

			itemid = itemid + 1;
		}
	}

	if (disabledstuff5)
	{
		itemid = 25+24+24+24;

		while (itemid < 22 + 24 + 24 + 24 + 24)
		{
			if (disabledstuff5 & itob(itemid - (24+24+24+24)))
			if (StripSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
				counter = counter + 1;

			itemid = itemid + 1;
		}
	}

	if (counter)
	{
		entity oself;
		oself = self;
		self = player;
		self->current_weapon = W_BestWeapon ();
		W_SetCurrentAmmo ();

		UpdateWeaponItems();
		self = oself;
	}

	return counter;
}

//====================================================================================================
// Removes an specific item from a player, storing change for restore, or cleaning up stored flag

float StripSpecificItem(entity player, float itemid, float storechange, float resetstored)
{
	float done;
	done = PR_FALSE;

	// PAGE 1 - Primary Weapons
	if (itemid == 1)
	{
		if (player->weapons_carried & PR_WEAP_SNIPER_RIFLE)
		{
			player->weapons_carried = player->weapons_carried - (player->weapons_carried & (PR_WEAP_SNIPER_RIFLE | PR_WEAP_AUTO_RIFLE));
			done = PR_TRUE;
		}
	}
	else if (itemid == 2)
	{
		if (player->weapons_carried & PR_WEAP_ASSAULT_CANNON)
		{
			player->weapons_carried = player->weapons_carried - PR_WEAP_ASSAULT_CANNON;
			done = PR_TRUE;
		}
	}
	else if (itemid == 3)
	{
		if (player->weapons_carried & PR_WEAP_ROCKET_LAUNCHER)
		{
			player->weapons_carried = player->weapons_carried - PR_WEAP_ROCKET_LAUNCHER;
			done = PR_TRUE;
		}
	}
	else if (itemid == 4)
	{
		if (player->weapons_carried & PR_WEAP_INCENDIARY)
		{
			player->weapons_carried = player->weapons_carried - (player->weapons_carried & (PR_WEAP_INCENDIARY | PR_WEAP_FLAMETHROWER));
			done = PR_TRUE;
		}
	}
	else if (itemid == 5)
	{
		if (player->weapons_carried & PR_WEAP_GRENADE_LAUNCHER)
		{
			player->weapons_carried = player->weapons_carried - PR_WEAP_GRENADE_LAUNCHER;
			done = PR_TRUE;
		}
	}
	else if (itemid == 6)
	{
		if (player->weapons_carried & PR_WEAP_LIGHTNING)
		{
			player->weapons_carried = player->weapons_carried - PR_WEAP_LIGHTNING;
			done = PR_TRUE;
		}
	}
	else if (itemid == 7)
	{
		if (player->weapons_carried & PR_WEAP_LIGHT_ASSAULT)
		{
			player->weapons_carried = player->weapons_carried - PR_WEAP_LIGHT_ASSAULT;
			done = PR_TRUE;
		}
	}

	// PAGE 2 - Secondary Weapons
	else if (itemid == 8)
	{
		if (player->weapons_carried & PR_WEAP_SNG)
		{
			player->weapons_carried = player->weapons_carried - PR_WEAP_SNG;
			done = PR_TRUE;
		}
	}
	else if (itemid == 9)
	{
		if (player->cutf_items & PR_CUTF_KNIFE)
		{
			player->cutf_items = player->cutf_items - PR_CUTF_KNIFE;
			done = PR_TRUE;
		}
	}
	else if (itemid == 10)
	{
		if (player->weapons_carried & PR_WEAP_SUPER_SHOTGUN)
		{
			player->weapons_carried = player->weapons_carried - PR_WEAP_SUPER_SHOTGUN;
			done = PR_TRUE;
		}
	}
	else if (itemid == 11)
	{
		if (player->weapons_carried & PR_WEAP_RAILGUN)
		{
			player->weapons_carried = player->weapons_carried - PR_WEAP_RAILGUN;
			done = PR_TRUE;
		}
	}
	else if (itemid == 12)
	{
		if (player->weapons_carried & PR_WEAP_NAILGUN)
		{
			player->weapons_carried = player->weapons_carried - PR_WEAP_NAILGUN;
			done = PR_TRUE;
		}
	}
	else if (itemid == 13)
	{
		if (player->weapons_carried & PR_WEAP_TRANQ)
		{
			player->weapons_carried = player->weapons_carried - PR_WEAP_TRANQ;
			done = PR_TRUE;
		}
	}
	else if (itemid == 14)
	{
		if (player->weapons_carried & PR_WEAP_SHOTGUN)
		{
			player->weapons_carried = player->weapons_carried - PR_WEAP_SHOTGUN;
			done = PR_TRUE;
		}
	}

	// PAGE 3 - Misc/Extra Weapons
	else if (itemid == 15)
	{
		if (player->weapons_carried & PR_WEAP_MAUSER)
		{
			player->weapons_carried = player->weapons_carried - PR_WEAP_MAUSER;
			done = PR_TRUE;
		}
	}
	else if (itemid == 16)
	{
		if (player->weapons_carried & PR_WEAP_AIRF)
		{
			player->weapons_carried = player->weapons_carried - PR_WEAP_AIRF;
			done = PR_TRUE;
		}
	}
	else if (itemid == 17)
	{
		if (player->cutf_items & PR_CUTF_AIRFISTUPGRADE)
		{
			player->cutf_items = player->cutf_items - PR_CUTF_AIRFISTUPGRADE;
			done = PR_TRUE;
		}
	}
	else if (itemid == 18)
	{
		if (player->weapons_carried & PR_WEAP_DAEDALUS)
		{
			player->weapons_carried = player->weapons_carried - PR_WEAP_DAEDALUS;
			done = PR_TRUE;
		}
	}
	else if (itemid == 19)
	{
		if (player->weapons_carried & PR_WEAP_LASERCANNON)
		{
			player->weapons_carried = player->weapons_carried - PR_WEAP_LASERCANNON;
			done = PR_TRUE;
		}
	}
	else if (itemid == 20)
	{
		if (player->cutf_items & PR_CUTF_HOLO)
		{
			player->cutf_items = player->cutf_items - PR_CUTF_HOLO;
			done = PR_TRUE;
		}
	}
	else if (itemid == 21)
	{
		if (player->cutf_items & PR_CUTF_CLIPEXTEND)
		{
			player->cutf_items = player->cutf_items - PR_CUTF_CLIPEXTEND;
			done = PR_TRUE;
		}
	}

	// (skipped: Legs and Health)...

	// PAGE 6 - Special Armors & Ammo
	else if (itemid == 39)
	{
		if (player->tf_items & PR_NIT_CERAMIC)
		{
			player->tf_items = player->tf_items - PR_NIT_CERAMIC;
			done = PR_TRUE;
		}
	}
	else if (itemid == 40)
	{
		if (player->tf_items & PR_NIT_GEL)
		{
			player->tf_items = player->tf_items - PR_NIT_GEL;
			done = PR_TRUE;
		}
	}
	else if (itemid == 41)
	{
		if (player->tf_items & PR_NIT_ASBESTOS)
		{
			player->tf_items = player->tf_items - PR_NIT_ASBESTOS;
			done = PR_TRUE;
		}
	}
	else if (itemid == 42)
	{
		if (player->tf_items & PR_NIT_KEVLAR)
		{
			player->tf_items = player->tf_items - PR_NIT_KEVLAR;
			done = PR_TRUE;
		}
	}
	else if (itemid == 43)
	{
		if (player->tf_items & PR_NIT_BLAST)
		{
			player->tf_items = player->tf_items - PR_NIT_BLAST;
			done = PR_TRUE;
		}
	}
	else if (itemid == 44)
	{
		if (player->tf_items & PR_NIT_AMMO_BACKPACK)
		{
			player->tf_items = player->tf_items - PR_NIT_AMMO_BACKPACK;
			done = PR_TRUE;
		}
	}
	else if (itemid == 45)
	{
		if (player->tf_items & PR_NIT_AMMO_BANDOLIER)
		{
			player->tf_items = player->tf_items - PR_NIT_AMMO_BANDOLIER;
			done = PR_TRUE;
		}
	}

	if (done)
	{
		if (storechange)
			SetItemInside(player,0,itemid,PR_TRUE);
	}

	if (resetstored)
		SetItemInside(player,1,itemid,PR_FALSE);

	return done;
}

//====================================================================================================
// Gives an specific item to a player, storing change for restore, or cleaning up stored flag

float GiveSpecificItem(entity player, float itemid, float storechange, float resetstored)
{
	float done;
	done = PR_FALSE;

	// PAGE 1 - Primary Weapons
	if (itemid == 1)
	{
		if (!(player->weapons_carried & PR_WEAP_SNIPER_RIFLE))
		{
			player->weapons_carried = player->weapons_carried | PR_WEAP_SNIPER_RIFLE | PR_WEAP_AUTO_RIFLE;
			done = PR_TRUE;
		}
	}
	else if (itemid == 2)
	{
		if (!(player->weapons_carried & PR_WEAP_ASSAULT_CANNON))
		{
			player->weapons_carried = player->weapons_carried | PR_WEAP_ASSAULT_CANNON;
			done = PR_TRUE;
		}
	}
	else if (itemid == 3)
	{
		if (!(player->weapons_carried & PR_WEAP_ROCKET_LAUNCHER))
		{
			player->weapons_carried = player->weapons_carried | PR_WEAP_ROCKET_LAUNCHER;
			done = PR_TRUE;
		}
	}
	else if (itemid == 4)
	{
		if (!(player->weapons_carried & PR_WEAP_INCENDIARY))
		{
			player->weapons_carried = player->weapons_carried | PR_WEAP_INCENDIARY | PR_WEAP_FLAMETHROWER;
			done = PR_TRUE;
		}
	}
	else if (itemid == 5)
	{
		if (!(player->weapons_carried & PR_WEAP_GRENADE_LAUNCHER))
		{
			player->weapons_carried = player->weapons_carried | PR_WEAP_GRENADE_LAUNCHER;
			done = PR_TRUE;
		}
	}
	else if (itemid == 6)
	{
		if (!(player->weapons_carried & PR_WEAP_LIGHTNING))
		{
			player->weapons_carried = player->weapons_carried | PR_WEAP_LIGHTNING;
			done = PR_TRUE;
		}
	}
	else if (itemid == 7)
	{
		if (!(player->weapons_carried & PR_WEAP_LIGHT_ASSAULT))
		{
			player->weapons_carried = player->weapons_carried | PR_WEAP_LIGHT_ASSAULT;
			done = PR_TRUE;
		}
	}

	// PAGE 2 - Secondary Weapons
	else if (itemid == 8)
	{
		if (!(player->weapons_carried & PR_WEAP_SNG))
		{
			player->weapons_carried = player->weapons_carried | PR_WEAP_SNG;
			done = PR_TRUE;
		}
	}
	else if (itemid == 9)
	{
		if (!(player->cutf_items & PR_CUTF_KNIFE))
		{
			player->cutf_items = player->cutf_items | PR_CUTF_KNIFE;
			done = PR_TRUE;
		}
	}
	else if (itemid == 10)
	{
		if (!(player->weapons_carried & PR_WEAP_SUPER_SHOTGUN))
		{
			player->weapons_carried = player->weapons_carried | PR_WEAP_SUPER_SHOTGUN;
			done = PR_TRUE;
		}
	}
	else if (itemid == 11)
	{
		if (!(player->weapons_carried & PR_WEAP_RAILGUN))
		{
			player->weapons_carried = player->weapons_carried | PR_WEAP_RAILGUN;
			done = PR_TRUE;
		}
	}
	else if (itemid == 12)
	{
		if (!(player->weapons_carried & PR_WEAP_NAILGUN))
		{
			player->weapons_carried = player->weapons_carried | PR_WEAP_NAILGUN;
			done = PR_TRUE;
		}
	}
	else if (itemid == 13)
	{
		if (!(player->weapons_carried & PR_WEAP_TRANQ))
		{
			player->weapons_carried = player->weapons_carried | PR_WEAP_TRANQ;
			done = PR_TRUE;
		}
	}
	else if (itemid == 14)
	{
		if (!(player->weapons_carried & PR_WEAP_SHOTGUN))
		{
			player->weapons_carried = player->weapons_carried | PR_WEAP_SHOTGUN;
			done = PR_TRUE;
		}
	}

	// PAGE 3 - Misc/Extra Weapons
	else if (itemid == 15)
	{
		if (!(player->weapons_carried & PR_WEAP_MAUSER))
		{
			player->weapons_carried = player->weapons_carried | PR_WEAP_MAUSER;
			done = PR_TRUE;
		}
	}
	else if (itemid == 16)
	{
		if (!(player->weapons_carried & PR_WEAP_AIRF))
		{
			player->weapons_carried = player->weapons_carried | PR_WEAP_AIRF;
			done = PR_TRUE;
		}
	}
	else if (itemid == 17)
	{
		if (!(player->cutf_items & PR_CUTF_AIRFISTUPGRADE))
		{
			player->cutf_items = player->cutf_items | PR_CUTF_AIRFISTUPGRADE;
			done = PR_TRUE;
		}
	}
	else if (itemid == 18)
	{
		if (!(player->weapons_carried & PR_WEAP_DAEDALUS))
		{
			player->weapons_carried = player->weapons_carried | PR_WEAP_DAEDALUS;
			done = PR_TRUE;
		}
	}
	else if (itemid == 19)
	{
		if (!(player->weapons_carried & PR_WEAP_LASERCANNON))
		{
			player->weapons_carried = player->weapons_carried | PR_WEAP_LASERCANNON;
			done = PR_TRUE;
		}
	}
	else if (itemid == 20)
	{
		if (!(player->cutf_items & PR_CUTF_HOLO))
		{
			player->cutf_items = player->cutf_items | PR_CUTF_HOLO;
			done = PR_TRUE;
		}
	}
	else if (itemid == 21)
	{
		if (!(player->cutf_items & PR_CUTF_CLIPEXTEND))
		{
			player->cutf_items = player->cutf_items | PR_CUTF_CLIPEXTEND;
			done = PR_TRUE;
		}
	}

	// (skipped: Legs and Health)...

	// PAGE 6 - Special Armors & Ammo
	else if (itemid == 39)
	{
		if (!(player->tf_items & PR_NIT_CERAMIC))
		{
			player->tf_items = player->tf_items | PR_NIT_CERAMIC;
			done = PR_TRUE;
		}
	}
	else if (itemid == 40)
	{
		if (!(player->tf_items & PR_NIT_GEL))
		{
			player->tf_items = player->tf_items | PR_NIT_GEL;
			done = PR_TRUE;
		}
	}
	else if (itemid == 41)
	{
		if (!(player->cutf_items & PR_NIT_ASBESTOS))
		{
			player->cutf_items = player->cutf_items | PR_NIT_ASBESTOS;
			done = PR_TRUE;
		}
	}
	else if (itemid == 42)
	{
		if (!(player->tf_items & PR_NIT_KEVLAR))
		{
			player->tf_items = player->tf_items | PR_NIT_KEVLAR;
			done = PR_TRUE;
		}
	}
	else if (itemid == 43)
	{
		if (!(player->tf_items & PR_NIT_BLAST))
		{
			player->tf_items = player->tf_items | PR_NIT_BLAST;
			done = PR_TRUE;
		}
	}
	else if (itemid == 44)
	{
		if (!(player->cutf_items & PR_NIT_AMMO_BACKPACK))
		{
			player->cutf_items = player->cutf_items | PR_NIT_AMMO_BACKPACK;
			done = PR_TRUE;
		}
	}
	else if (itemid == 45)
	{
		if (!(player->cutf_items & PR_NIT_AMMO_BANDOLIER))
		{
			player->cutf_items = player->cutf_items | PR_NIT_AMMO_BANDOLIER;
			done = PR_TRUE;
		}
	}

	if (done)
	{
		if (storechange)
			SetItemInside(player,1,itemid,PR_TRUE);
	}

	if (resetstored)
		SetItemInside(player,0,itemid,PR_FALSE);

	return done;
}

//==================================================================================
// Gives the free stuff to the player (no cost and every1 gets it)

float GiveFreeStuff(entity player)
{
	if (!(givenstuff1 | givenstuff2 | givenstuff3 | givenstuff4 | givenstuff5))
		return 0;

	float itemid, counter;
	entity oself;

	counter = 0;

	if (givenstuff1)
	{
		itemid = 1;

		while (itemid < 22)
		{
			if (givenstuff1 & itob(itemid))
			if (GiveSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
				counter = counter + 1;

			itemid = itemid + 1;
		}
	}

	if (givenstuff2)
	{
		itemid = 25;

		while (itemid < 22 + 24)
		{
			if (givenstuff2 & itob(itemid - 24))
			if (GiveSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
				counter = counter + 1;

			itemid = itemid + 1;
		}
	}

	if (givenstuff3)
	{
		itemid = 25+24;

		while (itemid < 22 + 24 + 24)
		{
			if (givenstuff3 & itob(itemid - (24+24)))
			if (GiveSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
				counter = counter + 1;

			itemid = itemid + 1;
		}
	}

	if (givenstuff4)
	{
		itemid = 25+24+24;

		while (itemid < 22 + 24 + 24 + 24)
		{
			if (givenstuff4 & itob(itemid - (24+24+24)))
			if (GiveSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
				counter = counter + 1;

			itemid = itemid + 1;
		}
	}

	if (givenstuff5)
	{
		itemid = 25+24+24+24;

		while (itemid < 22 + 24 + 24 + 24 + 24)
		{
			if (givenstuff5 & itob(itemid - (24+24+24+24)))
			if (GiveSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
				counter = counter + 1;

			itemid = itemid + 1;
		}
	}

	if (counter)
	{
		oself = self;
		self = player;
		UpdateWeaponItems();
		self = oself;
	}

	return counter;
}

//=====================================================================================
// Checks if the specified item flag is in the supplied fields or not

/*float(float itemid, float stuff1, float stuff2, float stuff3, float stuff4, float stuff5) ItemInside =
{
	local float bit;

	bit = itemid - (floor(itemid/24)*24);

	if (itemid < 24)
	{
		if (stuff1 & itob(bit))
			return #TRUE;
	}
	else if (itemid < 48)
	{
		if (stuff2 & itob(bit))
			return #TRUE;
	}
	else if (itemid < 72)
	{
		if (stuff3 & itob(bit))
			return #TRUE;
	}
	else if (itemid < 96)
	{
		if (stuff4 & itob(bit))
			return #TRUE;
	}
	else if (itemid < 120)
	{
		if (stuff5 & itob(bit))
			return #TRUE;
	}
	else
		return #FALSE; // bug, anyway

	return #FALSE;
};*/

//=====================================================================
// Modifies the striped/given flags on an entity

float SetItemInside(entity ent, float type, float itemid, float value)
{
	int bit;

	bit = itemid - (floor(itemid/24)*24);
	bit = itob(bit);

	if (type == 0) // Striped/Disabled fields
	{
		if (value) // Set bit
		{
			if (itemid < 24)
			{
				if (!(ent->PR_disabledstuff1 & bit))
				{
					ent->PR_disabledstuff1 = ent->PR_disabledstuff1 | bit;
					return PR_TRUE;
				}
			}
			else if (itemid < 48)
			{
				if (!(ent->PR_disabledstuff2 & bit))
				{
					ent->PR_disabledstuff2 = ent->PR_disabledstuff2 | bit;
					return PR_TRUE;
				}
			}
			else if (itemid < 72)
			{
				if (!(ent->PR_disabledstuff3 & bit))
				{
					ent->PR_disabledstuff3 = ent->PR_disabledstuff3 | bit;
					return PR_TRUE;
				}
			}
			else if (itemid < 96)
			{
				if (!(ent->PR_disabledstuff4 & bit))
				{
					ent->PR_disabledstuff4 = ent->PR_disabledstuff4 | bit;
					return PR_TRUE;
				}
			}
			else if (itemid < 120)
			{
				if (!(ent->PR_disabledstuff5 & bit))
				{
					ent->PR_disabledstuff5 = ent->PR_disabledstuff5 | bit;
					return PR_TRUE;
				}
			}
			else
				return PR_FALSE; // bug, anyway
		}
		else // un-set bit
		{
			if (itemid < 24)
			{
				if (ent->PR_disabledstuff1 & bit)
				{
					ent->PR_disabledstuff1 = ent->PR_disabledstuff1 - bit;
					return PR_TRUE;
				}
			}
			else if (itemid < 48)
			{
				if (ent->PR_disabledstuff2 & bit)
				{
					ent->PR_disabledstuff2 = ent->PR_disabledstuff2 - bit;
					return PR_TRUE;
				}
			}
			else if (itemid < 72)
			{
				if (ent->PR_disabledstuff3 & bit)
				{
					ent->PR_disabledstuff3 = ent->PR_disabledstuff3 - bit;
					return PR_TRUE;
				}
			}
			else if (itemid < 96)
			{
				if (ent->PR_disabledstuff4 & bit)
				{
					ent->PR_disabledstuff4 = ent->PR_disabledstuff4 - bit;
					return PR_TRUE;
				}
			}
			else if (itemid < 120)
			{
				if (ent->PR_disabledstuff5 & bit)
				{
					ent->PR_disabledstuff5 = ent->PR_disabledstuff5 - bit;
					return PR_TRUE;
				}
			}
			else
				return PR_FALSE; // bug, anyway
		}
	}
	else // Given fields
	{
		if (value) // Set bit
		{
			if (itemid < 24)
			{
				if (!(ent->PR_givenstuff1 & bit))
				{
					ent->PR_givenstuff1 = ent->PR_givenstuff1 | bit;
					return PR_TRUE;
				}
			}
			else if (itemid < 48)
			{
				if (!(ent->PR_givenstuff2 & bit))
				{
					ent->PR_givenstuff2 = ent->PR_givenstuff2 | bit;
					return PR_TRUE;
				}
			}
			else if (itemid < 72)
			{
				if (!(ent->PR_givenstuff3 & bit))
				{
					ent->PR_givenstuff3 = ent->PR_givenstuff3 | bit;
					return PR_TRUE;
				}
			}
			else if (itemid < 96)
			{
				if (!(ent->PR_givenstuff4 & bit))
				{
					ent->PR_givenstuff4 = ent->PR_givenstuff4 | bit;
					return PR_TRUE;
				}
			}
			else if (itemid < 120)
			{
				if (!(ent->PR_givenstuff5 & bit))
				{
					ent->PR_givenstuff5 = ent->PR_givenstuff5 | bit;
					return PR_TRUE;
				}
			}
			else
				return PR_FALSE; // bug, anyway
		}
		else // un-set bit
		{
			if (itemid < 24)
			{
				if (ent->PR_givenstuff1 & bit)
				{
					ent->PR_givenstuff1 = ent->PR_givenstuff1 - bit;
					return PR_TRUE;
				}
			}
			else if (itemid < 48)
			{
				if (ent->PR_givenstuff2 & bit)
				{
					ent->PR_givenstuff2 = ent->PR_givenstuff2 - bit;
					return PR_TRUE;
				}
			}
			else if (itemid < 72)
			{
				if (ent->PR_givenstuff3 & bit)
				{
					ent->PR_givenstuff3 = ent->PR_givenstuff3 - bit;
					return PR_TRUE;
				}
			}
			else if (itemid < 96)
			{
				if (ent->PR_givenstuff4 & bit)
				{
					ent->PR_givenstuff4 = ent->PR_givenstuff4 - bit;
					return PR_TRUE;
				}
			}
			else if (itemid < 120)
			{
				if (ent->PR_givenstuff5 & bit)
				{
					ent->PR_givenstuff5 = ent->PR_givenstuff5 - bit;
					return PR_TRUE;
				}
			}
			else
				return PR_FALSE; // bug, anyway
		}
	}

	return PR_FALSE;
}

//=================================================================================
// Returns/strips on a player the stuff that has been striped/given to him or her

float RestoreOriginalItems(entity player)
{
	float itemid, counter, all;
	entity oself;

	all = 0;
	counter = 0;

	if (player->PR_givenstuff1 | player->PR_givenstuff2 | player->PR_givenstuff3 | player->PR_givenstuff4 | player->PR_givenstuff5)
	{
		if (player->PR_givenstuff1)
		{
			itemid = 1;

			while (itemid < 22)
			{
				if (player->PR_givenstuff1 & itob(itemid))
				if (StripSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
					counter = counter + 1;

				itemid = itemid + 1;
			}
		}

		if (player->PR_givenstuff2)
		{
			itemid = 25;

			while (itemid < 22 + 24)
			{
				if (player->PR_givenstuff2 & itob(itemid - 24))
				if (StripSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
					counter = counter + 1;

				itemid = itemid + 1;
			}
		}

		if (player->PR_givenstuff3)
		{
			itemid = 25+24;

			while (itemid < 22 + 24 + 24)
			{
				if (player->PR_givenstuff3 & itob(itemid - (24+24)))
				if (StripSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
					counter = counter + 1;

				itemid = itemid + 1;
			}
		}

		if (player->PR_givenstuff4)
		{
			itemid = 25+24+24;

			while (itemid < 22 + 24 + 24 + 24)
			{
				if (player->PR_givenstuff4 & itob(itemid - (24+24+24)))
				if (StripSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
					counter = counter + 1;

				itemid = itemid + 1;
			}
		}

		if (player->PR_givenstuff5)
		{
			itemid = 25+24+24+24;

			while (itemid < 22 + 24 + 24 + 24 + 24)
			{
				if (player->PR_givenstuff5 & itob(itemid - (24+24+24+24)))
				if (StripSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
					counter = counter + 1;

				itemid = itemid + 1;
			}
		}

		if (counter)
		{
			oself = self;
			self = player;
			self->current_weapon = W_BestWeapon ();
			W_SetCurrentAmmo ();
			UpdateWeaponItems();
			self = oself;
		}
	}

	if (player->PR_disabledstuff1 | player->PR_disabledstuff2 | player->PR_disabledstuff3 | player->PR_disabledstuff4 | player->PR_disabledstuff5)
	{
		all = counter;
		counter = 0;

		if (player->PR_disabledstuff1)
		{
			itemid = 1;

			while (itemid < 22)
			{
				if (player->PR_disabledstuff1 & itob(itemid))
				if (GiveSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
					counter = counter + 1;

				itemid = itemid + 1;
			}
		}

		if (player->PR_disabledstuff2)
		{
			itemid = 25;

			while (itemid < 22 + 24)
			{
				if (player->PR_disabledstuff2 & itob(itemid - 24))
				if (GiveSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
					counter = counter + 1;

				itemid = itemid + 1;
			}
		}

		if (player->PR_disabledstuff3)
		{
			itemid = 25+24;

			while (itemid < 22 + 24 + 24)
			{
				if (player->PR_disabledstuff3 & itob(itemid - (24+24)))
				if (GiveSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
					counter = counter + 1;

				itemid = itemid + 1;
			}
		}

		if (player->PR_disabledstuff4)
		{
			itemid = 25+24+24;

			while (itemid < 22 + 24 + 24 + 24)
			{
				if (player->PR_disabledstuff4 & itob(itemid - (24+24+24)))
				if (GiveSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
					counter = counter + 1;

				itemid = itemid + 1;
			}
		}

		if (player->PR_disabledstuff5)
		{
			itemid = 25+24+24+24;

			while (itemid < 22 + 24 + 24 + 24 + 24)
			{
				if (player->PR_disabledstuff5 & itob(itemid - (24+24+24+24)))
				if (GiveSpecificItem(player,itemid,PR_FALSE,PR_FALSE))
					counter = counter + 1;

				itemid = itemid + 1;
			}
		}

		if (counter)
		{
			oself = self;
			self = player;
			UpdateWeaponItems();
			self = oself;
		}
	}

	// Clear bits
	ResetStripedAndGiven(player);

	return all + counter;
}

//============================================================================
// Subroutine for easy checking the disabled global fields

float DisabledStuff(float fieldnum)
{
	if (fieldnum == 1)
		return disabledstuff1;
	else if (fieldnum == 2)
		return disabledstuff2;
	else if (fieldnum == 3)
		return disabledstuff3;
	else if (fieldnum == 4)
		return disabledstuff4;
	else if (fieldnum == 5)
		return disabledstuff5;

	return 0;
}

//============================================================================
// Subroutine for easy changing the disabled global fields

void SetDisabledStuff(float fieldnum, float value)
{
	if (fieldnum == 1)
		disabledstuff1 = value;
	else if (fieldnum == 2)
		disabledstuff2 = value;
	else if (fieldnum == 3)
		disabledstuff3 = value;
	else if (fieldnum == 4)
		disabledstuff4 = value;
	else if (fieldnum == 5)
		disabledstuff5 = value;
}

//============================================================================
// Subroutine for easy checking the free items global fields

float GivenStuff(float fieldnum)
{
	if (fieldnum == 1)
		return givenstuff1;
	else if (fieldnum == 2)
		return givenstuff2;
	else if (fieldnum == 3)
		return givenstuff3;
	else if (fieldnum == 4)
		return givenstuff4;
	else if (fieldnum == 5)
		return givenstuff5;

	return 0;
}

//============================================================================
// Subroutine for easy changing the free items global fields

void SetGivenStuff(float fieldnum, float value)
{
	if (fieldnum == 1)
		givenstuff1 = value;
	else if (fieldnum == 2)
		givenstuff2 = value;
	else if (fieldnum == 3)
		givenstuff3 = value;
	else if (fieldnum == 4)
		givenstuff4 = value;
	else if (fieldnum == 5)
		givenstuff5 = value;
}

//=======================================================================
// Resets directly all the striped/given flags on a player

void ResetStripedAndGiven(entity player)
{
	player->PR_disabledstuff1 = 0;
	player->PR_disabledstuff2 = 0;
	player->PR_disabledstuff3 = 0;
	player->PR_disabledstuff4 = 0;
	player->PR_disabledstuff5 = 0;

	player->PR_givenstuff1 = 0;
	player->PR_givenstuff2 = 0;
	player->PR_givenstuff3 = 0;
	player->PR_givenstuff4 = 0;
	player->PR_givenstuff5 = 0;
}

//==========================================================================
// Returns a string with client's description and version number

string GetClientDescription(entity client)
{
	string result, tmp;

	if (client->cltype == PR_CLTYPE_DEFAULT)
	{
		result = "Original\xAFUnknown client";

		if (client->clversion != 0)
		{
			tmp = ver_ftos(client->clversion);
			result = strcat(result,S_(" ^bv^b"));
			result = strcat(result,tmp);
		}

		return result;
	}

	if (client->cltype == PR_CLTYPE_PROZACQW)
	{
		result = S_("^bP^brozac^bQW^b");

		tmp = ver_ftos(client->clversion);
		result = strcat(result,S_(" ^bv^b"));
		result = strcat(result,tmp);

		return result;
	}

	if (client->cltype == PR_CLTYPE_FUHQUAKE)
	{
		result = S_("^bF^buh^bQ^buake");

		tmp = ver_ftos(client->clversion);
		result = strcat(result,S_(" ^bv^b"));
		result = strcat(result,tmp);

		return result;
	}

	if (client->cltype == PR_CLTYPE_AMFQUAKE)
	{
		result = S_("^bAMFQ^buake");

		tmp = ver_ftos(client->clversion);
		result = strcat(result,S_(" ^bv^b"));
		result = strcat(result,tmp);

		return result;
	}

	if (client->cltype == PR_CLTYPE_ZQUAKE)
	{
		result = S_("^bZQ^buake");

		tmp = ver_ftos(client->clversion);
		result = strcat(result,S_(" ^bv^b"));
		result = strcat(result,tmp);

		return result;
	}

	if (client->cltype == PR_CLTYPE_QUAKEFORGE)
	{
		result = S_("^bQ^buake^bF^borge");

		tmp = ver_ftos(client->clversion);
		result = strcat(result,S_(" ^bv^b"));
		result = strcat(result,tmp);

		return result;
	}

	if (client->cltype == PR_CLTYPE_FTEQUAKE)
	{
		result = "FTEquake";
		return result;
	}

	if (client->cltype == PR_CLTYPE_EZQUAKE)
	{
		result = "EZQuake";
		return result;
	}

	// PZ: added DarkPlaces
	if (client->cltype == PR_CLTYPE_DARKPLACES)
	{
		result = "DarkPlaces";
		return result;
	}

	return "unrecognized"; // wont happen
}

//===========================================================
// Same thing but puts description inside "[]" chars

string GetBracedClDesc(entity client)
{
	string result;

	if (client->cltype <= PR_CLTYPE_DEFAULT)
	{
		result = GetClientDescription(client);
		result = strcat(S_("^["),result);
		result = strcat(result,S_("^]"));
	}
	else
	{
		result = GetClientDescription(client);
		result = strcat("[",result);
		result = strcat(result,"]");
	}

	return result;
}

//==============================================================
// Transforms a version float into a version string

string ver_ftos(float ver)
{
	float tmpf;
	string result, tmpst;

	tmpf = floor(ver);
	result = ftos(tmpf);
	result = strcat(result,S_("^b.^b"));
	tmpf = floor((ver - tmpf) * 100);
	tmpst = ftos(tmpf);
	result = strcat(result,tmpst);
	result = colstr(result,PR_COLSTR_NUMBER);

	return result;
}

//=============================================================================
// Extended version of SpawnMeatSpray()

entity SpawnMeatSprayEx(const vector& org, const vector& dir, float meatduration)
{
	entity missile;

	missile = spawn ();
	missile->owner = self;
	missile->movetype = PR_MOVETYPE_BOUNCE;
	missile->solid = PR_SOLID_NOT;

	makevectors (self->angles);

	missile->velocity = dir;

	missile->avelocity = V({3000, 1000, 2000});

// set missile duration
	missile->nextthink = time + meatduration;
	missile->think = SUB_Remove;

	setmodel (missile, "progs/zom_gib.mdl");
	setsize (missile, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (missile, org);

	return missile;
}

//===========================================================================
// Corpse related functions

void GibCorpse(entity corpse)
{
	#ifdef PR_GIBABLE_CORPSES

	entity oself;

	oself = self;
	self = corpse;

	if (self->modelindex == modelindex_player)
	{
		ThrowHeadOut("progs/h_player.mdl", 0);
		ThrowGib ("progs/gib1.mdl", 0,PR_TRUE,0,PR_GIB1_KGS,PR_TRUE);
		ThrowGib ("progs/gib2.mdl", 0,PR_TRUE,0,PR_GIB2_KGS,PR_TRUE);
		ThrowGib ("progs/gib3.mdl", 0,PR_TRUE,0,PR_GIB3_KGS,PR_TRUE);
	}
	else if (self->PR_monster_type == PR_MONSTER_SCRAG)
	{
		ThrowMonsterHead("progs/h_wizard.mdl",0);
		ThrowGib ("progs/gib2.mdl", 0, PR_TRUE, 0, PR_GIB2_KGS,PR_TRUE);
		ThrowGib ("progs/gib3.mdl", 0, PR_TRUE, 0, PR_GIB3_KGS,PR_TRUE);
		ThrowGib ("progs/gib3.mdl", 0, PR_TRUE, 0, PR_GIB3_KGS,PR_TRUE);
	}
	else if (self->PR_monster_type == PR_MONSTER_FISH)
	{
		ThrowGib ("progs/gib2.mdl", 0, PR_TRUE, 0, PR_GIB2_KGS,PR_TRUE);
		ThrowGib ("progs/gib3.mdl", 0, PR_TRUE, 0, PR_GIB3_KGS,PR_TRUE);
	}
	else if (self->PR_monster_type == PR_MONSTER_DEMON)
	{
		ThrowMonsterHead("progs/h_demon.mdl",0);
		ThrowGib ("progs/gib3.mdl", 0, PR_TRUE, 0, PR_GIB3_KGS,PR_TRUE);
		ThrowGib ("progs/gib2.mdl", 0, PR_TRUE, 0, PR_GIB2_KGS,PR_TRUE);
		ThrowGib ("progs/gib3.mdl", 0, PR_TRUE, 0, PR_GIB3_KGS,PR_TRUE);
	}
	else if (self->PR_monster_type == PR_MONSTER_GREMLIN)
	{
		ThrowMonsterHead("progs/h_grem.mdl",0);
		ThrowGib ("progs/gib1.mdl", 0,PR_TRUE,0,PR_GIB1_KGS,PR_TRUE);
		ThrowGib ("progs/gib2.mdl", 0,PR_TRUE,0,PR_GIB2_KGS,PR_TRUE);
		ThrowGib ("progs/gib3.mdl", 0,PR_TRUE,0,PR_GIB3_KGS,PR_TRUE);
	}
	else if (self->PR_monster_type == PR_MONSTER_SHAMBLER)
	{
		ThrowMonsterHead("progs/h_shams.mdl",0);
		ThrowGib ("progs/gib1.mdl", 0, PR_TRUE, 0,PR_GIB1_KGS,PR_TRUE);
		ThrowGib ("progs/gib2.mdl", 0, PR_TRUE, 0,PR_GIB2_KGS,PR_TRUE);
		ThrowGib ("progs/gib3.mdl", 0, PR_TRUE, 0,PR_GIB3_KGS,PR_TRUE);
		ThrowGib ("progs/gib3.mdl", 0, PR_TRUE, 0,PR_GIB3_KGS,PR_TRUE);
	}
	else if (self->PR_monster_type == PR_MONSTER_ARMY)
	{
		ThrowMonsterHead("progs/h_player.mdl",0);
		ThrowGib ("progs/gib1.mdl", 0,PR_TRUE,0,PR_GIB1_KGS,PR_TRUE);
		ThrowGib ("progs/gib2.mdl", 0,PR_TRUE,0,PR_GIB2_KGS,PR_TRUE);
		ThrowGib ("progs/gib3.mdl", 0,PR_TRUE,0,PR_GIB3_KGS,PR_TRUE);
	}
#ifdef PR_COOP_MODE_ENHANCED
	else if ( COOP_IsCoopMonster( self ) ) {
		if ( self->mdl != string_null )
			ThrowMonsterHead( self->mdl, 0 );

		ThrowGib( "progs/gib1.mdl", -40, PR_TRUE, 0, PR_GIB1_KGS, PR_FALSE );
		ThrowGib( "progs/gib2.mdl", -40, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE );
		ThrowGib( "progs/gib3.mdl", -40, PR_TRUE, 0, PR_GIB3_KGS, PR_FALSE );
	}
#endif

	SpawnBloodEx(self->origin,0.66,9);

	self = oself;

	#endif
}

void DamageCorpse(entity corpse, float damage, vector where, entity attacker, float directdmg)
{
	#ifdef PR_GIBABLE_CORPSES

	float points;

	// Safety checks, first
	if (corpse->PR_corpse_HP <= 0)
		return;
	if (corpse->modelindex == modelindex_null)
		return;
	if (damage <= 0)
		return;

	// Calculate damage if needed
	if (directdmg) // Direct damage, normal
		points = damage;
	else // Radius/Blast damage, we should adjust damage based on distance to explosion
	{
		points = 0.5*vlen (where - corpse->origin);

		if (points < 0)
			points = 0;

		points = damage - points;

		if (points <= 0)
			return;
	}

	// Apply damage to the corpse
	corpse->PR_corpse_HP = corpse->PR_corpse_HP - points;

	if (corpse->PR_corpse_HP <= 0) // got destroyed?
	{
		// Corpse explodes.. throw gibs!
		// Throw an appropiate head too! :)
		GibCorpse(corpse);

		// If this is a monster corpse, just remove entity
		if (corpse->classname == "monster_corpse")
		{
			dremove(corpse);
			return;
		}
		// If a player and still on the corpse, report his corpse is destroyed and who is gibbing it
		else if (corpse->classname == "player" && corpse->is_connected)
		{
			if (attacker != world)
			if (attacker->classname == "player" && attacker->is_connected)
			{
				if (corpse != attacker)
				{
					sprint(corpse,PR_PRINT_MEDIUM,attacker->netname," destroys your corpse!\n");
					sprint(attacker,PR_PRINT_LOW,"You destroy ",corpse->netname,"'s corspe!\n");
				}
				else
					sprint(corpse,PR_PRINT_MEDIUM,"You destroy your own corpse!\n");
			}
		}

		// Make corpse disappear, and "unmark" entity as corpse, doing cleanup
		corpse->modelindex = modelindex_null;
		corpse->solid = PR_SOLID_NOT;
		corpse->PR_corpseflag = "";
		corpse->flags = corpse->flags - (corpse->flags & PR_FL_FINDABLE_NONSOLID);
		corpse->takedamage = PR_DAMAGE_NO;
	}
	else // still not destroyed, it should bleed
	{
		// Spawn corresponding ammount of blood
		SpawnBloodEx(corpse->origin, 0.5, PR_MAX_CORPSE_BLOOD);

		// If not enough damage, dont spawn meat spray, sometimes dont also even if enough dmg
		if (random() > 0.6 || points < 20)
			return;

		// Calculate random direction for meat spray
		where[X] = crandom()*120;
		where[Y] = crandom()*120;
		where[Z] = (200 + crandom()*60)*(points/40);
		if (where[Z] > 300)
			where[Z] = 300;

		// Launch meat spray
		SpawnMeatSprayEx(corpse->origin,where,1);
	}

	#endif
}

//===========================================================================
// Prepares an entity for corpse damage

void MakeCorpse(entity deadone, float corpse_dmg_tolerance)
{
	#ifdef PR_USE_CORPSE_CODE

	deadone->PR_corpseflag = PR_STRFLAG_CORPSE;
	deadone->PR_corpse_HP = corpse_dmg_tolerance + crandom()*40 - random()*60;
	if (deadone->PR_corpse_HP < 1)
		deadone->PR_corpse_HP = 1;
	deadone->touch = SUB_Null;
	deadone->flags = deadone->flags | PR_FL_FINDABLE_NONSOLID;
	deadone->maxs[Z] = deadone->mins[Z] + 15;
	setsize (deadone, deadone->mins, deadone->maxs);
	deadone->takedamage = PR_DAMAGE_YES;

	#endif
}

//=======================================================================
// Reverts last increase of CPrint FX's counter/indicator

void NoCprintFX()
{
	self->PR_cprint_fx = self->PR_cprint_fx - 1;
}

//=======================================================================
// Flashes background of a client, using "bf" command the specified times

void BackFlash(entity client, float ammount)
{
	string tstr;
	float counter;

	counter = 1;

	tstr = "bf";

	while (counter < ammount)
	{
		tstr = strcat(tstr,";bf");

		counter = counter + 1;
	}

	tstr = strcat(tstr,"\n");

	stuffcmd(client,tstr);
}

//=======================================================================
// Gizmo - use object stuff

float IsValidUseObject( entity object) {
	if ( object->classname == "door" && object->use != fd_secret_use )
		return PR_TRUE;
	else if ( object->classname == "plat" )
		return PR_TRUE;
	else if ( object->classname == "train" )
		return PR_TRUE;
	else if ( object->classname == "func_button" )
		return PR_TRUE;

	return PR_FALSE;
}

string GetUseObjectAction( entity object) {
	if ( object->classname == "door" && object->use != fd_secret_use )
		return "open/close";
	else if ( object->classname == "plat" )
		return "move up/down";
	else if ( object->classname == "train" )
		return "goto the next waypoint";
	else if ( object->classname == "func_button" )
		return "be pushed";

	return "do nothing";
}

string GetUseObjectName( entity object) {
	// for now, everything uses .classname as the name
	return object->classname;
}

void door_go_down();
void door_go_up();
void door_killed();
void fd_secret_use();
void plat_go_up();
void plat_go_down();
void UseObject( entity object) {
	entity	oself;

	if ( object == world ) {
		makevectors( self->v_angle );
		traceline( self->origin + V({0, 0, 16}), ( self->origin + V({0, 0, 16}) ) + v_forward * 4096, PR_TL_ANY_SOLID, self );
	} else
		trace_ent = object;

	if ( trace_ent->classname == "door" && trace_ent->use != fd_secret_use ) {
		oself = self;
		self = trace_ent;

		if ( self->state == PR_STATE_DOWN || self->state == PR_STATE_BOTTOM )
			door_go_up ();
		else
			door_go_down ();

		self = oself;
	} else if ( trace_ent->classname == "plat" ) {
		oself = self;
		self = trace_ent;

		if ( self->state == PR_STATE_DOWN || self->state == PR_STATE_BOTTOM )
			plat_go_up ();
		else
			plat_go_down ();

		self = oself;
	} else if ( trace_ent->classname == "train" ) {
		oself = self;
		self = trace_ent;

		train_next ();

		self = oself;
	} else if ( trace_ent->classname == "func_button" ) {
		trace_ent->enemy = self;

		oself = self;
		self = trace_ent;

		button_fire();

		self = oself;
	}
}

//=======================================================================
// Used to get the team number of any entity

float GetTeam( entity e) {
	if ( e->flags & PR_FL_MONSTER ) {
		if ( e->real_owner != world )
			return e->real_owner->team_no;
		return e->team_no;
	}
	if ( e->classname == "player" )
		return e->team_no;
	if ( e->classname == "grenade" ) {
		if ( e->netname == "land_mine" )
			return e->owner->team_no;
	}
	if ( IsBuilding( e ) )
		return e->real_owner->team_no;

	return e->team_no;
}

/*
=============
infront2

returns 1 if the entity is in front (in sight) of check
=============
*/
float infront2( entity targ, entity check) {
	vector	vec;
	float		dot;

	makevectors( check->angles );
	vec = normalize( targ->origin - check->origin );
	dot = vec * v_forward;

	if ( dot > 0.3 )
		return PR_TRUE;
	return PR_FALSE;
}

// PZ: Used for prolonging how long CenterPrints are displayed and so forth.
// [tgt]: if world, print to all, else print to [tgt]
// [flash]: if true, message flashes
void ExtendedCPrint(entity tgt, const string& str, float iterations, float delayBtwPrints, float flash)
{
	if (tgt != world && tgt->classname != "player")
		return;

	if (iterations < 1)
		return;

	entity timer;

	// if there's another ExtendedCPrint already running for tgt, kill the one running
	entity pt; // generic pointer used for delayed removal
	timer = find(world, "classname", "cprint_timer");
	while (timer != world)
	{
		if (tgt == world || (tgt != world && timer->owner == tgt))
			pt = timer; // remove [timer]
		else
			pt = world; // don't remove [timer]
		timer = find(timer, "classname", "cprint_timer");
		if (pt != world)
			dremove(pt);
	}

	iterations = floor(iterations);

	// setup timer ent for centerprint iterations
	timer = spawnServerSide(); // PZ: make it a server-side only entity
	timer->owner = tgt;
	timer->netname = str;
	timer->classname = "cprint_timer";
	if (flash)
	{
		timer->aflag = 1;
		timer->health = iterations * 2;
		timer->heat = delayBtwPrints / 2;
	}
	else
	{
		timer->health = iterations;
		timer->heat = delayBtwPrints;
	}
	timer->nextthink = time + 0.0001;
	timer->think = ExtendedCPrintThink;
}
// could do interesting effects like changing color of string
// and maybe other animations besides flashing
void ExtendedCPrintThink()
{
	string msg;
	if (self->aflag == 1) // non-blank condition
	{
		msg = self->netname;
		self->aflag = -1;
	}
	else if (self->aflag == -1) // blank condition
	{
		msg = "\n";
		self->aflag = 1;
	}
	else
		msg = self->netname; // no flashing

	// print to all
	if (self->owner == world)
	{
		entity player;
		player = find(world, "classname", "player");
		while (player != world)
		{
			CenterPrint(player, msg);
			player = find(player, "classname", "player");
		}
	}
	else if (self->owner->classname != "player")
		return;
	// print to [tgt]
	else
		CenterPrint(self->owner, msg);

	// handle centerprint iterations
	self->health = self->health - 1;
	if (self->health <= 0)
	{
		dremove(self); // remove timer ent
		return;
	}
	self->nextthink = time + self->heat;
}

} // END namespace Progs
