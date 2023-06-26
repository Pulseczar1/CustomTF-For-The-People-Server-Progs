/*======================================================
	TESLA.QC			Custom TeamFortress v3.1

	(c) William Kerney		5/21/00
	(c) Craig Hauser			19/3/00
========================================================
Weapons and functions for the Tesla Sentries
======================================================*/

#include "progs.h"
#include "tesla.h"
#include "menu.h"
#include "cpstuff.h"
#include "sprites.h"
#include "player.h"
#include "debug.h"
#include "combat.h"
#include "monsters.h"
#include "field.h"
#include "custom.h"
#include "optimize.h"

namespace Progs {

//$cd fortress\models\coil
//$origin 0 0 0
//$scale 1
//$base base

//$skin yellow
//$skin red
//$skin blue
//$skin green

namespace TeslaFrames
{
	enum {FR_ON1, FR_ON2, FR_ON3, FR_ON4, FR_ON5, FR_ON6, FR_ON7};
	enum {FR_FIRE1 = 7, FR_FIRE2, FR_FIRE3, FR_FIRE4, FR_FIRE5, FR_FIRE6};
}

//--------- NEW TESLA MODEL (COIL.MDL) FRAMES ----------//
//------------------------------------------------------//
//- I took this model from labyrinth server ------------//



#ifdef PR_foobar
.ammo_shells  = voltage upgrades (0-3) 0 = 200 1 = 600 2 = 1200 3 = unlimitied
.ammo_nails   = amp upgrades (0-3) 0 = 30/1 secs 1 = 60/1 sec 2 = 120/s cont. 3 = 500/s
.ammo_rockets = power upgrades(0-3) 0 = 100 health/50 battery 1 = 200h/120b 2 = 350h/200b 3 = 500h/300b
.health = current health
.ammo_cells = Cells currently in battery, max cells always dynamically calculated
.maxhealth = max health
.tf_items PR_NIT_AUTOID = spydetector upgrade (0-1) 0 = no spy 1 = tesla will attack disguised spies
.tf_items PR_NIT_FLYING_SENTRY = turret upgrade (0-1) 0 = normal 1 = tesla launches into the air and attaches to ceiling
.tf_items PR_NIT_SCANNER = Improved Targeter (0-1) 0 = normal checkclient 1 = radiuscan attacks enemy mines & demons, etc
.currentammo = total upgrades (0-5)
.waitmin = battery drain
Battery Drain = Power = I*V = (i+2)*(v+2) which will range between 4 to 25 cells per shot
Initial Attack Delay -- 0 = 1.5secs 1 = 1.5sec 2 = 1secs 3 = 2secs

//CH
.ammo_shells  = voltage upgrades (0-3) 0 = 200 1 = 600 2 = 1200 3 = 3000
.ammo_nails   = amp upgrades (0-3) 0 = 30/1 secs 1 = 60/1 sec 2 = 120/s cont. 3 = 500/s
.ammo_rockets = power upgrades(0-3) 0 = 100 health/50 battery 1 = 200h/120b 2 = 350h/200b 3 = 500h/300b
.health = health
.ammo_cells = Cells currently in battery
.maxammo_cells = Max cells
.maxhealth = max health
.tf_items PR_NIT_AUTOID = spydetector upgrade (0-1) 0 = no spy 1 = tesla will attack disguised spies
.tf_items PR_NIT_FLYING_SENTRY = turret upgrade (0-1) 0 = normal 1 = tesla launches into the air and attaches to ceiling
.tf_items PR_NIT_SCANNER = Improved Targeter (0-1) 0 = normal checkclient 1 = radiuscan attacks enemy mines & demons, etc
.tf_items PR_NIT_TESLA_CLOAKING = Cloaking device (0-1) 0=nothing 1=Cloaked unless firing does not glow
.tf_items PR_NIT_KEVLAR = Kevlar Armor (0-1) 0=nothing 1=has Kevlar Armor
.tf_items PR_NIT_BLAST = Blast Armor (0-1) 0=nothing 1=has Blast Armor
.has_sentry = Normal upgrades left
.has_tesla = Misc upgrades left
.waitmin = battery drain
Battery Drain = Power = I*V = (i+2)*(v+2) which will range between 4 to 25 cells per shot
Initial Attack Delay -- based on range/1000 (spy detector is range/500) amps and other stuff
#endif

// Gizmo - to make things easier, make a macro for each field
#define PR_tesla_volts			ammo_shells
#define PR_tesla_amps			ammo_nails
#define PR_tesla_power			ammo_rockets
#define PR_tesla_batterydrain		waitmin

float modelindex_tesla; //CH
// Tesla AI Functions
float Tesla_FindTarget();
void Tesla_FoundTarget();
void Tesla_Pain(entity attacker, float damage);
void Tesla_Die();
float Tesla_Fire();
void Tesla_Idle();
void Tesla_Touch();

float ReturnTeslaDelay();
entity Tesla_RadiusScan(entity scanner, float scanrange);

// OfN
//entity(entity OldTesla) TeslaClone;
//float(entity gun) IsInValidLocation;

void Tesla_Check_Frags()
{
	if (self->has_teleporter == 0) { //no extra frags
		if (self->frags >= 2) {
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			self->has_teleporter = 1;
		}
	}
	else if (self->has_teleporter == 1) {
		if (self->frags >= 5) {
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			self->has_teleporter = 2;
		}
	}
	else if (self->has_teleporter == 2) {
		if (self->frags >= 8) {
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			self->has_teleporter = 3;
		}
	}
	else if (self->has_teleporter == 3) {
		if (self->frags >= 11) {
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			self->has_teleporter = 4;
		}
	}
	else if (self->has_teleporter == 4) {
		if (self->frags >= 15) {
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			self->has_teleporter = 5;
		}
	}
	else if (self->has_teleporter == 5) {
		if (self->frags >= 20) {
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			self->has_teleporter = 6;
		}
	}
	else if (self->has_teleporter == 6) {
		if (self->frags >= 26) {
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			self->has_teleporter = 7;
		}
	}
	else if (self->has_teleporter == 7) {
		if (self->frags >= 33) {
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			self->has_teleporter = 8;
		}
	}
	else if (self->has_teleporter == 8) {
		if (self->frags >= 40) {
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			self->has_teleporter = 9;
		}
	}

	else if (self->has_teleporter == 9) {
		if (self->frags >= 50) {
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			bprint(PR_PRINT_HIGH, self->real_owner->netname);
			bprint(PR_PRINT_HIGH, " is a master of tesla placement, his tesla has over 50 kills!!\n");
			self->has_teleporter = 10;
		}
	}
	else if (self->has_teleporter == 10) {
		if (self->frags >= 60) {
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			self->has_teleporter = 11;
		}
	}
	else if (self->has_teleporter == 11) {
		if (self->frags >= 70) {
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			self->has_teleporter = 12;
		}
	}
	else if (self->has_teleporter == 12) {
		if (self->frags >= 80) {
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			self->has_teleporter = 13;
		}
	}
	else if (self->has_teleporter == 13) {
		if (self->frags >= 90) {
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			self->has_teleporter = 14;
		}
	}
	else if (self->has_teleporter == 14) {
		if (self->frags >= 100) {
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			Tesla_Add_Rand_Upgrade(self, self->real_owner);
			bprint(PR_PRINT_HIGH, self->real_owner->netname);
			bprint(PR_PRINT_HIGH, " is a true legend at tesla placement, his tesla has over 100 kills!!\n");
			self->has_teleporter = 15;
		}
	}
}
void Tesla_Lose_Glow()
{
	if ((self->tf_items & PR_NIT_TESLA_CLOAKING && (self->effects & PR_EF_DIMLIGHT)) || self->job == 3)
	{
		if (self->is_haxxxoring == 0)
		{
			self->modelindex = modelindex_null;
			spawnFOG(self->origin);
			sound (self, PR_CHAN_MISC, "misc/r_tele3.wav", 1, PR_ATTN_NORM);
			if (self->job == 3) self->job = 1;
		}
	}
	self->effects = self->effects - (self->effects & PR_EF_DIMLIGHT);
}
void Tesla_Give_Glow()
{
	if (self->tf_items & PR_NIT_TESLA_CLOAKING && !(self->effects & PR_EF_DIMLIGHT))
	{
		if (self->is_haxxxoring == 0)
		{
			spawnFOG(self->origin);
			sound (self, PR_CHAN_MISC, "misc/r_tele4.wav", 1, PR_ATTN_NORM);

			self->pain_finished=1;
		}
	}
	self->effects = self->effects | PR_EF_DIMLIGHT;
}

void TeslaThink();

//-------------------------------------------------------------------------//
//- OfN - A new main think sub is needed for the new tesla model animation //
//-------------------------------------------------------------------------//
void tsla_on1() {FRAME_STATE(TeslaFrames::FR_ON1, tsla_on2); self->job=1; TeslaThink();}
void tsla_on2() {FRAME_STATE(TeslaFrames::FR_ON1, tsla_on3); TeslaThink();
/*if (self.velocity == '0 0 0')
if (!IsInValidLocation(self)) {
		sprint(self.real_owner, #PRINT_HIGH, "Your tesla malfunctioned and blew up!\n");
		Tesla_Die();
	}*/
}
void tsla_on3() {FRAME_STATE(TeslaFrames::FR_ON2, tsla_on4); TeslaThink();}
void tsla_on4() {FRAME_STATE(TeslaFrames::FR_ON2, tsla_on5); TeslaThink();}
void tsla_on5() {FRAME_STATE(TeslaFrames::FR_ON3, tsla_on6); TeslaThink();}
void tsla_on6() {FRAME_STATE(TeslaFrames::FR_ON3, tsla_on7); TeslaThink();}
void tsla_on7() {FRAME_STATE(TeslaFrames::FR_ON4, tsla_on8); TeslaThink();}
void tsla_on8() {FRAME_STATE(TeslaFrames::FR_ON4, tsla_on9); TeslaThink();}
void tsla_on9() {FRAME_STATE(TeslaFrames::FR_ON5, tsla_on10); TeslaThink();}
void tsla_on10() {FRAME_STATE(TeslaFrames::FR_ON5, tsla_on11); TeslaThink();}
void tsla_on11() {FRAME_STATE(TeslaFrames::FR_ON6, tsla_on12); TeslaThink();}
void tsla_on12() {FRAME_STATE(TeslaFrames::FR_ON6, tsla_on13); TeslaThink();}
void tsla_on13() {FRAME_STATE(TeslaFrames::FR_ON7, tsla_on14); TeslaThink();}
void tsla_on14() {FRAME_STATE(TeslaFrames::FR_ON7, tsla_on1); TeslaThink();}
//--------------------------------------------------------------//
void tsla_fire1() {FRAME_STATE(TeslaFrames::FR_FIRE1, tsla_fire2); self->job=2; TeslaThink();}
void tsla_fire2() {FRAME_STATE(TeslaFrames::FR_FIRE1, tsla_fire3); TeslaThink();}
void tsla_fire3() {FRAME_STATE(TeslaFrames::FR_FIRE2, tsla_fire4); TeslaThink();}
void tsla_fire4() {FRAME_STATE(TeslaFrames::FR_FIRE2, tsla_fire5); TeslaThink();}
void tsla_fire5() {FRAME_STATE(TeslaFrames::FR_FIRE3, tsla_fire6); TeslaThink();}
void tsla_fire6() {FRAME_STATE(TeslaFrames::FR_FIRE3, tsla_fire7); TeslaThink();}
void tsla_fire7() {FRAME_STATE(TeslaFrames::FR_FIRE4, tsla_fire8); TeslaThink();}
void tsla_fire8() {FRAME_STATE(TeslaFrames::FR_FIRE4, tsla_fire9); TeslaThink();}
void tsla_fire9() {FRAME_STATE(TeslaFrames::FR_FIRE5, tsla_fire10); TeslaThink();}
void tsla_fire10() {FRAME_STATE(TeslaFrames::FR_FIRE5, tsla_fire11); TeslaThink();}
void tsla_fire11() {FRAME_STATE(TeslaFrames::FR_FIRE6, tsla_fire12); TeslaThink();}
void tsla_fire12() {FRAME_STATE(TeslaFrames::FR_FIRE6, tsla_fire1); TeslaThink();}


void TeslaThink()
{
	if (self->pain_finished == 1) // replace tesla entity if it was cloaked
	{
/*
		local entity TSelf;
		TSelf=TeslaClone(self);
		dremove(self);
		self=TSelf;
		self.pain_finished=0;
		self.nextthink=time;
*/
		setmodel (self, "progs/coil.mdl");
		setorigin (self, self->origin);
		self->pain_finished = 0;
		self->nextthink = time;
		return;
	}

	#ifdef PR_FIXES_FOR_FLOATINGBUILDS
	/*if (!(self.tf_items & #NIT_TURRET))
	//if (!(self.flags & #FL_ONGROUND))
	{
		self.flags = self.flags - (self.flags & #FL_ONGROUND);
		self.velocity_z = -80;
	}*/
	#endif

	self->nextthink = time + 0.05;

	// Restore velocity if flying and stoped
	if (self->is_haxxxoring)
		self->velocity[Z] = 200;

	if (self->has_holo <= time && self->no_grenades_1 == PR_FALSE)
	{
		self->no_grenades_1 = PR_TRUE;
		Tesla_Idle();
	}

	if (self->job == 1 && self->effects & PR_EF_DIMLIGHT)
	{
		tsla_fire1();
		return;
	}
	else if (self->job == 2 && !(self->effects & PR_EF_DIMLIGHT))
	{
		tsla_on1();
		return;
	}
}

//Main loop for tesla - OfN - was
void Tesla_Idle()
{
	if (self->is_malfunctioning & PR_SCREWUP_ONE)
	{
		self->has_holo = time + 0.25; //FIXED - unhacked teslas work again
		self->no_grenades_1 = PR_FALSE;
		return;
	}

	if (self->tf_items & PR_NIT_TELEPORTER) //CH
		Tesla_Check_Frags();

	//self.waitmax holds if we have a target
	if (self->waitmax) { //If we have target, shoot it
		self->waitmax = Tesla_Fire();
//		if (!self.waitmax)
//			bprint (#PRINT_HIGH, "LOST TARGET!\n");
	}

	if (!self->waitmax)
	{
		// Grievre fix for multiple targets kill bug
		self->attack_finished = 1;

		//Try to reacquire target
		if (Tesla_FindTarget())
			self->waitmax = PR_TRUE;
		else
		{
			//Lost a lock
			Tesla_Lose_Glow();
			//self.nextthink = time + 0.25; //4Hz check rate
			self->has_holo = time + 0.25;
			self->no_grenades_1 = PR_FALSE; // reset main think
		}
	}

	if (self->attack_finished < 1)
		self->attack_finished = self->attack_finished + 0.1;

	//self.think = Tesla_Idle; //WK Unecessary but keeps us in the loop
}

float Tesla_FindTarget()
{
	entity client;

	//WK Hack to get floating tesla working
	if (self->tf_items & PR_NIT_TURRET)
		self->origin[Z] = self->origin[Z] - 40;//40;
	//else
	//	self.origin_z = self.origin_z + 24;

	self->oldenemy = world; //CH for sbar

	if (self->PR_tesla_volts == 0)
		client = Tesla_RadiusScan (self, 400);
	else if (self->PR_tesla_volts == 1)
		client = Tesla_RadiusScan (self, 800);
	else if (self->PR_tesla_volts == 2)
		client = Tesla_RadiusScan (self, 1200);
	else
		client = Tesla_RadiusScan (self, 3500); //I dont think that anyone would be this far away

	//WK Unhack our hack
	if (self->tf_items & PR_NIT_TURRET)
		self->origin[Z] = self->origin[Z] + 40;//+ 40;
	//else
	//	self.origin_z = self.origin_z - 24;

	if (client != self)
	{
		// Found a Target
		/*if (self.enemy == client) //Recovering lock
			return Tesla_Fire();*/ // SB not any more you're not

		self->enemy = client;
		self->oldenemy = self->enemy; //CH for sbar

		Tesla_FoundTarget ();

		return PR_TRUE;
	}
	return PR_FALSE;
}

void Tesla_FoundTarget()
{
	// Cannon Powerup Sound?
	if (self->ammo_cells > self->PR_tesla_batterydrain)
	// PZ: teslas being carried, or flying through the air after being thrown, by ZGG, don't shoot, except in Coop
	if (!deathmatch || (self->pickedupby == world && self->velocity == V({0, 0, 0})))
		sound(self, PR_CHAN_VOICE, "weapons/guerset.wav", 1, PR_ATTN_NORM);

	//Glow
	Tesla_Give_Glow();

	self->goalentity = self->enemy;

	//self.nextthink = time + ReturnTeslaDelay();
	self->has_holo = time + ReturnTeslaDelay(); //- OfN -
	self->no_grenades_1 = PR_FALSE;
}

void Tesla_Pain(entity attacker, float damage)
{
	// Update the owner's status bar
	self->real_owner->StatusRefreshTime = time + 0.2;
//CH special sbar  for eng.
	self->real_owner->StatusBarScreen = 4;
}

void Tesla_Die()
{
	//RemoveMyTimers(self); // psionic clean-up

	if (self->real_owner->tf_items & PR_NIT_DOUBLETESLA && self->dont_do_triggerwork)
	{
		if (self->dont_do_triggerwork == 1)
			sprint(self->real_owner, PR_PRINT_HIGH, "Your tesla coil #1 was destroyed.\n");
		else if (self->dont_do_triggerwork == 2)
			sprint(self->real_owner, PR_PRINT_HIGH, "Your tesla coil #2 was destroyed.\n");
		else
			sprint(self->real_owner, PR_PRINT_HIGH, "Your tesla coil was destroyed.\n");
	}
	else
		sprint(self->real_owner, PR_PRINT_HIGH, "Your tesla coil was destroyed.\n");

	self->real_owner->has_tesla = self->real_owner->has_tesla - (self->real_owner->has_tesla & self->dont_do_triggerwork);
	if (self->real_owner->has_tesla < 0)
		self->real_owner->has_tesla = 0;

	entity sprite;

	sprite = SpawnSprite(1,PR_SPRITE_AIRBURST,self->origin,V({0, 0, 0}),PR_SPRITEMOVE_UPSLOW,0.1);

	if (sprite != world)
	{
		sprite->effects = PR_EF_DIMLIGHT;

		if (self->real_owner->team_no == 1)
			sprite->effects = PR_EF_DIMLIGHT | PR_EF_BLUE;
		else if (self->real_owner->team_no == 2)
			sprite->effects = PR_EF_DIMLIGHT | PR_EF_RED;
	}

//CH REAL tesla gibs
	ThrowGib("progs/tesgib1.mdl", -70, PR_TRUE,0,0, PR_FALSE);
	ThrowGib("progs/tesgib2.mdl", -70, PR_TRUE,0,0, PR_FALSE);
	//ThrowGib("progs/tesgib3.mdl", -70);
	ThrowGib("progs/tesgib4.mdl", -70, PR_TRUE,self->skin,0, PR_FALSE);
	ThrowGib("progs/tesgib4.mdl", -70, PR_TRUE,self->skin,0, PR_FALSE);

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

float DoorsAt(const vector& where)
{
	traceline (where, where, PR_TL_ANY_SOLID, self);
	if (trace_ent->classname == "door" || trace_ent->classname == "plat")
		return PR_TRUE;
	else
		return PR_FALSE;
}

float Tesla_Fire()
{
	//local vector dir;
	//local float hit;
	float damage;
	float cheater;
	vector below;

	// OfN - Psionized teslas don't shot
	/*if (self.tfstate & #TFSTATE_PSIONIZED)
		return #FALSE;*/

#ifdef PR_0
// remove these prints when it's apparent that this is working fine -PZ
bprint(PR_PRINT_HIGH, "pickedupby: ");
if (self->pickedupby != world)
    bprint(PR_PRINT_HIGH, self->pickedupby->netname);
else
    bprint(PR_PRINT_HIGH, "NO ONE");
bprint(PR_PRINT_HIGH, "; cells: ");
string tmp;
tmp = ftos(self->ammo_cells);
bprint(PR_PRINT_HIGH, tmp);
bprint(PR_PRINT_HIGH, "\n");
#endif
	// PZ: teslas being carried, or flying through the air after being thrown, by ZGG, don't shoot, except in Coop
	if (deathmatch && (self->pickedupby != world || (self->velocity != V({0, 0, 0}) && !(self->flags & PR_FL_ONGROUND)))) return PR_FALSE;

	// PZ NOTE: I'm guessing this is in case tesla is turretted in the area of an open door. When door closes, tesla is inside the door, where it can
	//          likely still shoot enemies, but be protected by the door.
	if (self->tf_items & PR_NIT_TURRET)
	{
		below = V({0, 0, 1});	// So as not to hit tesla.
		below[Z] = below[Z] + self->size[Z]; // Below should be 1 unit below the tesla.

		// Check a variety of locations for a door. 5 and -5 should be the width of tesla
		if (DoorsAt(self->origin - below + V({0, 0, 0})))
			cheater = PR_TRUE;
		else if (DoorsAt(self->origin - below + V({8, 0, 0})))
			cheater = PR_TRUE;
		else if (DoorsAt(self->origin - below + V({-8, 0, 0})))
			cheater = PR_TRUE;
		else if (DoorsAt(self->origin - below + V({0, 8, 0})))
			cheater = PR_TRUE;
		else if (DoorsAt(self->origin - below + V({0, -8, 0})))
			cheater = PR_TRUE;
		else
			cheater = PR_FALSE;

		if (cheater){
			sprint(self->real_owner,PR_PRINT_HIGH,"The door's wiring conflicts with your tesla's!\n");
			TF_T_Damage(self,world,world,self->health+100,0,0);
			return PR_FALSE;
		}
	}

	//WK Stop gun from shooting at dead spies
	if (self->enemy == world)
		return PR_FALSE;
	if (self->enemy == world)        // PZ NOTE: One check for this wasn't enough?
		return PR_FALSE;
	if (self->enemy == self)
		return PR_FALSE;
	if ((self->enemy->health <= 0 && self->enemy->classname!="holo") || (self->enemy->classname=="holo" && self->enemy->has_holo == 0))
		return PR_FALSE;
#ifdef PR_QUAKE_WORLD // culled by KK. was QUAKE_WORLD
	if (ceasefire)
		return PR_FALSE;
	/*if (infokey(world,"ceasefire")=="on") //CH
		return #FALSE;*/
#endif
	if (self->enemy->classname == "player")
	{
		if (!self->enemy->is_connected)
		{
			self->enemy = world;
			return PR_FALSE;
		}
		if (!(self->tf_items & PR_NIT_AUTOID))
		{
			if (self->enemy->is_feigning)
				return PR_FALSE;
			if (self->enemy->is_undercover == 1) // PZ: added "== 1" (2 is disguisING status; we want teslas to shoot at spies trying to disguise but not yet disguised)
				return PR_FALSE;
			if (self->enemy->job & PR_JOB_THIEF && self->enemy->job & PR_JOB_ACTIVE)
				return PR_FALSE;
		}
	}
	//CH rechecks if target is out of range,  has a little extra room added.
	// SB +100 for each one, values were incorrect before
	float maxrange;
	maxrange = 500;

	if (self->PR_tesla_volts == 0)
		maxrange = 500;
	else if (self->PR_tesla_volts == 1) //+300 no
		maxrange = 900;
	else if (self->PR_tesla_volts == 2) //+500 no
		maxrange = 1300;
	else if (self->PR_tesla_volts == 3) //+800 no
		maxrange = 4100;

	/*if (self.#tesla_volts == 0 && vlen(self.origin - self.enemy.origin) >= 500)
		return #FALSE;
	else if (self.#tesla_volts == 1 && vlen(self.origin - self.enemy.origin) >= 900) //+300 no
		return #FALSE;
	else if (self.#tesla_volts == 2 && vlen(self.origin - self.enemy.origin) >= 1300) //+500 no
		return #FALSE;
	else if (self.#tesla_volts == 3 && vlen(self.origin - self.enemy.origin) >= 4100) //+800 no
		return #FALSE;*/

	if (vlen(self->origin - self->enemy->origin) >= maxrange) //+800 no
		return PR_FALSE;

	self->oldenemy = self->enemy; //CH for sbar

	//WK Hack to get floating sentry working - reset before all the returns
	if (self->tf_items & PR_NIT_TURRET) self->origin[Z] = self->origin[Z] - 40;// 40
	//else self.origin_z = self.origin_z + 24;

	if (!visible2(self->enemy, self))
	{
		if (self->tf_items & PR_NIT_TURRET) self->origin[Z] = self->origin[Z] + 40; // 40
		//else self.origin_z = self.origin_z - 24;
		return PR_FALSE;
	}

	// ADDED
	if (!visible2(self, self->enemy))
	{
		if (self->tf_items & PR_NIT_TURRET) self->origin[Z] = self->origin[Z] + 40; // 40
		//else self.origin_z = self.origin_z - 24;
		return PR_FALSE;
	}


	self->ammo_cells = self->ammo_cells - self->PR_tesla_batterydrain; //Waitmin is precalculated cost
	if (self->ammo_cells < 0)
	{
		self->ammo_cells = 0;
		if (self->tf_items & PR_NIT_TURRET) self->origin[Z] = self->origin[Z] + 40; // 40
		//else self.origin_z = self.origin_z - 24;
		self->enemy = world;
		return PR_FALSE;
	}

	Tesla_Give_Glow(); //FIXED

	if (self->is_malfunctioning & PR_SCREWUP_THREE)
	{
		float damg;
		damg = random() * 100 + 200 * self->PR_tesla_amps; // the bigger they come, the harder they fall
		deathmsg = PR_DMSG_TESLA;
		WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
		WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
		T_RadiusDamage(self, self, damg, world);
		if (self->tf_items & PR_NIT_TURRET) self->origin[Z] = self->origin[Z] + 40; // 40
		//else self.origin_z = self.origin_z - 24;
		return PR_FALSE;
	}

	deathmsg = PR_DMSG_TESLA;

	MuzzleFlash(self);

 // OfN - Check for force field
	float shoulddmg;
	shoulddmg = PR_TRUE;

	traceline (self->origin, self->enemy->origin, PR_TL_ANY_SOLID, self);

	if (trace_ent->classname == "force_field")
	{
		WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_BROADCAST, PR_TE_LIGHTNING2);
		WriteEntity (PR_MSG_BROADCAST, self);
		WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
		if (self->tf_items & PR_NIT_TURRET)
			WriteCoord (PR_MSG_BROADCAST, self->origin[Z] + 10);
		else
			WriteCoord (PR_MSG_BROADCAST, self->origin[Z] + 30);
		WriteCoord (PR_MSG_BROADCAST, trace_endpos[X]);
		WriteCoord (PR_MSG_BROADCAST, trace_endpos[Y]);
		WriteCoord (PR_MSG_BROADCAST, trace_endpos[Z]);
	#ifdef PR_QUAKE_WORLD
		multicast (self->origin, PR_MULTICAST_PHS);
	#endif

		shoulddmg = PR_FALSE;

		FieldEvent(trace_ent,trace_endpos,trace_ent);
		//FieldExplosion(trace_ent,trace_endpos,trace_ent);
		//PutFieldWork(trace_ent);
	}
	else
	{
		WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_BROADCAST, PR_TE_LIGHTNING2);
		WriteEntity (PR_MSG_BROADCAST, self);
		WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
		if (self->tf_items & PR_NIT_TURRET)
			WriteCoord (PR_MSG_BROADCAST, self->origin[Z] + 10);
		else
			WriteCoord (PR_MSG_BROADCAST, self->origin[Z] + 30);
		WriteCoord (PR_MSG_BROADCAST, self->enemy->origin[X]);
		WriteCoord (PR_MSG_BROADCAST, self->enemy->origin[Y]);
		WriteCoord (PR_MSG_BROADCAST, self->enemy->origin[Z]);
	#ifdef PR_QUAKE_WORLD
		multicast (self->origin, PR_MULTICAST_PHS);
	#endif
	}

	sound (self, PR_CHAN_WEAPON, "weapons/lhit.wav", 1, PR_ATTN_NORM);
	if (vlen(self->enemy->origin - self->origin) >= 800 && shoulddmg) //Only play end sound if far away
		sound (self->enemy, PR_CHAN_WEAPON, "weapons/lhit.wav", 1, PR_ATTN_NORM); //CH at start and end of arc

	/*if (self.#tesla_amps == 0) damage = 40;
	if (self.#tesla_amps == 1) damage = 80;
	if (self.#tesla_amps == 2) damage = 160;
	if (self.#tesla_amps == 3) damage = 320;*/

	if (self->PR_tesla_amps <= 0)
		damage = 40;
	else if (self->PR_tesla_amps == 1)
		damage = 80;
	else if (self->PR_tesla_amps == 2)
		damage = 130;
	else
		damage = 240;

	if (self->all_active & PR_IMPROVED_FOUR) damage = damage * 1.15; //- OfN - Improved circuit hack - was 1.2 - TOO LOW NOW?

	if (self->is_malfunctioning & PR_SCREWUP_TWO) damage = 1;
	//TF_T_Damage (self.enemy, self, self.real_owner, damage, #TF_TD_NOTTEAM, #TF_TD_ELECTRICITY);

	if (shoulddmg)
		TF_T_Damage (self->enemy, self, self, damage, PR_TF_TD_NOTTEAM, PR_TF_TD_ELECTRICITY);

	//self.nextthink = time + ReturnTeslaDelay();
	//self.has_holo = time + ReturnTeslaDelay(); //?? wTF

	self->attack_finished = 1;

	self->has_holo = time + ReturnTeslaDelay(); //- OfN -
	self->no_grenades_1 = PR_FALSE;

	// Warn owner that it's low on ammo
	if (self->ammo_cells == 0 && (random() < 0.1))
		sprint(self->real_owner, PR_PRINT_HIGH, "Tesla is out of cells.\n");
	else if (self->ammo_cells <= self->PR_tesla_batterydrain)
		sprint(self->real_owner, PR_PRINT_HIGH, "Tesla is low on cells.\n");

	if (self->tf_items & PR_NIT_TURRET) self->origin[Z] = self->origin[Z] + 40; //40
	//else self.origin_z = self.origin_z - 24;

	//self.attack_finished = 1;

							   //ofn Holos
	if ((self->enemy->health <= 0 && self->enemy->classname!="holo") || (self->enemy->classname=="holo" && self->enemy->has_holo == 0) || (self->enemy->classname == "holo" && self->has_fieldgen > time))
	{
		self->enemy = world;
		return PR_FALSE;
	}
	return PR_TRUE;

}

//WK Sentry Touch function
//Will kill bad guy
void Tesla_Touch()
{
	vector below;
	float cheater;
	cheater = PR_FALSE;

	//WK Check for blockage
	if (pointcontents(self->origin) == PR_CONTENT_SKY)
	{
		sprint(self->real_owner, PR_PRINT_HIGH, "Your sentry gun flew away.\n");
		Tesla_Die();
		return;
	}

	if (other->classname=="player" && !(self->tf_items & PR_NIT_TURRET) && (self->tf_items & PR_NIT_TESLA_CLOAKING) &&
	    self->no_grenades_2 < time && self->job == 1 && Teammate(self, other))
	{
		if (self->real_owner == other)
			sprint(other,PR_PRINT_HIGH,"Your cloaked tesla here!\n");
		else
			sprint(other,PR_PRINT_HIGH,"There is a cloaked friendly tesla here!\n");

		self->no_grenades_2 = time + 2;
	}

	// Gizmo - because of gravgun don't shock while we're moving
	if (other->takedamage && !(self->tf_items & PR_NIT_TURRET) && self->velocity == V({0, 0, 0}))  // OfN - fixme: doesn't check for enemy disguised spies
	{
		deathmsg = PR_DMSG_BUG_ZAPPER;

		// Gizmo - allow non demons and non army soldiers to be affected (coop monsters)
		if (IsMonster(other))
		{
			//float temp;
			//temp = GetTeam(other);
			if (!Teammate(other, self->real_owner))
			{
				TF_T_Damage (other, self, self, 400, PR_TF_TD_NOTTEAM, PR_TF_TD_ELECTRICITY);
				return;
			}

			return;
		}
		if (!Teammate(other, self->real_owner) && other->is_undercover != 1) // <== FIXME
			//TF_T_Damage (other, self, self.real_owner, 400, #TF_TD_NOTTEAM, #TF_TD_ELECTRICITY);
			TF_T_Damage (other, self, self, 400, PR_TF_TD_NOTTEAM, PR_TF_TD_ELECTRICITY);
		return;
	}
	//I'm in midflight and hit something
	if (self->tf_items & PR_NIT_TURRET && self->movetype == PR_MOVETYPE_FLY)
	{
		//WK Check to see if we are blocked
		if (pointcontents(self->origin) == PR_CONTENT_SKY ||
			//pointcontents(self.origin - '0 0 10') == #CONTENT_SKY ||
			//pointcontents(self.origin + '0 0 5') == #CONTENT_SKY ||
			//pointcontents(self.origin + '0 0 10') == #CONTENT_SKY ||
			//pointcontents(self.origin + '0 0 15') == #CONTENT_SKY ||
			//pointcontents(self.origin + '0 0 20') == #CONTENT_SKY ||
			//pointcontents(self.origin + '0 0 30') == #CONTENT_SKY ||
			//pointcontents(self.origin + '0 0 40') == #CONTENT_SKY ||
			pointcontents(self->origin + V({0, 0, 70})) == PR_CONTENT_SKY) //- OfN
		{
			sprint(self->real_owner, PR_PRINT_HIGH, "Your tesla flew away.\n");
			Tesla_Die();
			return;
		}
		below = V({0, 0, 1});	// So as not to hit tesla.
		below[Z] = below[Z] + self->size[Z];

		// Check a varity of locations for a door. 5 and -5 should be the with of tesla
		if (DoorsAt(self->origin - below + V({0, 0, 0})))
			cheater = PR_TRUE;
		if (DoorsAt(self->origin - below + V({8, 0, 0})))
			cheater = PR_TRUE;
		if (DoorsAt(self->origin - below + V({-8, 0, 0})))
			cheater = PR_TRUE;
		if (DoorsAt(self->origin - below + V({0, 8, 0})))
			cheater = PR_TRUE;
		if (DoorsAt(self->origin - below + V({0, -8, 0})))
			cheater = PR_TRUE;

		if (cheater){
			sprint(self->real_owner,PR_PRINT_HIGH,"The door's wiring conflicts with your tesla's!\n");
			TF_T_Damage(self,world,world,self->health+100,0,0);
			return;
		}

		if (other == world)
		{ //The eagle has landed!
		//	sprint(self.real_owner, #PRINT_HIGH, "The eagle has landed!\n");
			self->solid = PR_SOLID_BBOX;
			self->flags = self->flags | PR_FL_ONGROUND;
			self->movetype = PR_MOVETYPE_STEP;
			self->origin[Z] = self->origin[Z] + 40;
			setorigin(self,self->origin); // OfN - Added 2004

			self->is_haxxxoring=0; // this flag is for cloaked teslas be able to cloak again

			if (self->job == 1)
			{
				self->effects = self->effects | PR_EF_DIMLIGHT; // hack to make lose_glow to run properly to make disappear again the tesla
				Tesla_Lose_Glow();
			}

			return;
		}
		else if (other->classname == "player")
		{
			deathmsg = PR_DMSG_BUG_ZAPPER;
			//TF_T_Damage (other, self, self.real_owner, 400, #TF_TD_NOTTEAM, #TF_TD_ELECTRICITY);
			TF_T_Damage (other, self, self, 400, PR_TF_TD_NOTTEAM, PR_TF_TD_ELECTRICITY);

			self->velocity[Z] = 200; //- OfN reset velocity, not stop
		}
		else
		{
			deathmsg = PR_DMSG_BUG_ZAPPER;
			//TF_T_Damage (other, self, self.real_owner, 400, #TF_TD_NOTTEAM, #TF_TD_ELECTRICITY);
			if (!Teammate(other->real_owner, self->real_owner))
				TF_T_Damage (other, self, self, 400, PR_TF_TD_NOTTEAM, PR_TF_TD_ELECTRICITY);
			//sprint(self.real_owner, #PRINT_HIGH, "The eagle has died.\n");
			Tesla_Die();
		}
	}
}
//========
//CH this sets the 'charge time' needed in order to fire at the target.
//Using the idea that a certain charge has to be generated in order for the current to arc to the enemy
//The time it takes to make this charge would be based on distance and amp plus some extras
//Thus fast firing close and slower as you get away and or the more amps to generate.
//This code works nicely to do that :)
float ReturnTeslaDelay()
{
	float r;
	float tesladelay;

	if (self->PR_tesla_amps == 0) //Initial delay based on AMPS
		tesladelay = 0.5;
	else if (self->PR_tesla_amps == 1) //So its not as fast,  but still fast
		tesladelay = 0.75;
	else if (self->PR_tesla_amps == 2)
		tesladelay = 1;
	else
		tesladelay = 1.5;

	if (self->enemy->cutf_items & PR_CUTF_JAMMER) // if we have jammer - Share and Enjoy!
		tesladelay = tesladelay + 0.5;

	r = vlen(self->enemy->origin - self->origin);
	if (self->tf_items & PR_NIT_AUTOID) //spy detect takes twice as long, as it does more in depth search
		tesladelay = tesladelay + (r / 500);
	else
		tesladelay = tesladelay + (r / 1000);

	// Invisible people take longer for sentries to lock onto, MAY HAPPEN
	if (self->enemy->modelindex == modelindex_null)
		tesladelay = tesladelay + 2;	// Must acquire a heat signal
	else if (self->enemy->modelindex == modelindex_eyes)
		tesladelay = tesladelay + 1;	// Some visual, so its a little easier

	//Improved targetter decreases lock time to 5/8
	//WK Changed to 7/8ths
	// Changed back again, what a lame item
	if (self->tf_items & PR_NIT_SCANNER)
		tesladelay = (tesladelay * 5) / 8;

	if (self->all_active & PR_IMPROVED_FOUR) tesladelay = tesladelay * 0.85; //was 0.8

	tesladelay = tesladelay * self->attack_finished; // SB so we don't fire instantly at old targ...
	// SB this also means we don't have to fully charge for new target
	self->attack_finished = 0;

	return tesladelay;
}
//=========================================================================
// Returns a list of players within a radius around the origin, like findradius,
// except that some parsing of the list can be done based on the parameters passed in.
// Make sure you check that the return value is not NULL before using it.
entity Tesla_RadiusScan(entity scanner, float scanrange)
{
	entity head;
	/*local entity list_head;
	local entity list;*/
	float gotatarget;

	head = findradius(scanner->origin, scanrange);

	while (head != world) {
		gotatarget = 0;

		if (/*vlen(head.origin - scanner.origin) <= scanrange && */head != scanner && visible2(head,scanner)) {
		//RPrint("TeslaScan()\n");
			//RPrint(head.classname); RPrint("\n");

			if (head->classname == "player" && head->health > 0)
			#ifdef PR_MAD_TESLA
				gotatarget = 1;
			#else
				gotatarget = Pharse_Client(head, scanner, 1, 0, 1, 1);
			#endif

#ifdef PR_COOP_MODE_ENHANCED
			// teslas always target coop monsters in coop mode, without improved targeter
			else if (COOP_IsCoopMonster( head ) && head->health > 0 && head->takedamage) {
				if (!Teammate(head, scanner))
					gotatarget = 1;
			}
#endif
			else if (scanner->tf_items & PR_NIT_SCANNER && scanner->team_no > 0) //Improved targeter targets monsters and soldiers
			{
				if (IsOwnedMonster(head)) {
					if (head->health > 0)		// Gizmo
					if (!Teammate(head->real_owner, scanner))
						gotatarget = 1;
				}
				else if (head->classname == "grenade" && head->netname == "land_mine")
				{
					if (!Teammate(head->owner, scanner))
						gotatarget = 1;
				}
				else if (!Teammate(head, scanner))
				{
					if (IsBuilding(head) && head->classname != "building_sentrygun_base")
						gotatarget = 1;
				}
			}
		}				// ADDED
		if (gotatarget && visible2(scanner,head))
			return head;

		head = head->chain;
	}

	//- OfN - Targets an holograph? // has improved circuits?, then nothing
	if (!(scanner->all_active & PR_IMPROVED_FOUR))
	{
		entity te;
		float gotone;

		gotone = PR_FALSE;

		te = find(world, "classname","holo");

		while (te != world && gotone == PR_FALSE)
		{
			//local vector	spot1, spot2;
			float 	r;

			r = vlen (scanner->origin - te->origin);

			gotone = PR_TRUE;

			#ifndef PR_HOLO_TEST
			if (Teammate(scanner->real_owner, te->real_owner))
				gotone = PR_FALSE;
			else
			#endif

			if (scanner->has_fieldgen > time) // engineers add 10 seconds of ignore-holo status to sentries when spannered
				gotone = PR_FALSE;
			else if (te->has_holo == 0)
				gotone = PR_FALSE;
			else if ( !visible2(scanner,te) )
				gotone = PR_FALSE;
			else if (r > scanrange)
				gotone = PR_FALSE;
			else if ( !visible2(te,scanner) ) // ADDED
				gotone = PR_FALSE;

			if (gotone)
				return te;

			te = find(te, "classname", "holo");
		} // while
	}

	return scanner;
}

//==========================================================//
//- Ofn - ugly hack i know... ------------------------------//
//- This is needed for the cloaking tesla to work ----------//

/*
entity(entity OldTesla) TeslaClone =
{
	newmis = spawn();

	//here the updates of any entity pointers (demon_two and building for players, plus goalentity and enemy for grunts)
	OldTesla.solid=#SOLID_NOT;

	local entity te;
	local vector tmp1, tmp2;

	//now the hacks inside hack! =)
	te = find(world, classname, "player");
	while (te)
	{
		if (te.is_haxxxoring) // if we r currently hacking...
		{
			if (te.demon_two.martyr_enemy==OldTesla) // ...this tesla
				te.demon_two.martyr_enemy=newmis; //UPDATE IT

		}
		else if (te.demon_two == OldTesla) // if we r targetting this tesla for a hack
		{
			te.demon_two = newmis; // UPDATE POINTER
		}


		if (te.building == OldTesla) // if we r fixing (menu) this tesla..
			te.building = newmis; // update it

		te = find(te, classname, "player");
	}

	// Grunts targetting this tesla
	te = find(world, classname, "monster_army");
	while (te)
	{
		if (te.goalentity == OldTesla)
			te.goalentity = newmis;
		if (te.enemy == OldTesla)
			te.enemy = newmis;

		te = find(te, classname, "monster_army");
	}

	// Check teslas for targeting our cloned one
	te = find(world, classname, "building_tesla");
	while (te)
	{
		if (te.enemy == OldTesla)
			te.enemy = newmis;

		te = find(te, classname, "building_tesla");
	}

	//solid_not also

	//newmis.origin = self.origin + v_forward;
	newmis.origin = OldTesla.origin;

	tmp1 = '-16 -16 -25';
	//tmp2 = '16 16 48'; //WK 62 is better, but crashes?
	tmp2 = '16 16 23';

	//newmis.mdl = "progs/newtesla.mdl";
	newmis.mdl = "progs/coil.mdl";
	newmis.netname = "tesla";
	//newmis.origin = newmis.origin + '0 0 25';
	newmis.origin = OldTesla.origin;

	newmis.owner = OldTesla.owner;
	newmis.real_owner = OldTesla.real_owner;//self;

	newmis.think = OldTesla.think;
	newmis.nextthink = time + 0.05;

	newmis.colormap = OldTesla.colormap;
	newmis.weapon = #BUILD_TESLA;
	//newmis.angles_y = anglemod(self.angles_y + 180);
	newmis.angles = OldTesla.angles;

	//newmis.velocity = '0 0 8';
	newmis.velocity = OldTesla.velocity; // AVOIDS TURRET LAUNCHING BUG?
	newmis.movetype = OldTesla.movetype;// #MOVETYPE_TOSS;

	newmis.solid = #SOLID_BBOX; // ;
	setmodel (newmis, newmis.mdl);
	setsize (newmis, tmp1, tmp2);
	setorigin (newmis, newmis.origin);

	//if (objtobuild==#BUILD_TESLA) newmis.skin = self.team_no;
	//if (self.team_no==3) newmis.skin=0;
	//else if (self.team_no==4) newmis.skin=3;

	newmis.skin = OldTesla.skin; //

	//newmis.flags = newmis.flags - (newmis.flags & #FL_ONGROUND);
	newmis.flags = OldTesla.flags;

	///////////////////////////

	newmis.classname = "building_tesla";
	newmis.netname = "tesla";
	newmis.takedamage = OldTesla.takedamage; //#DAMAGE_AIM;
	//newmis.solid = #SOLID_BBOX;
	newmis.th_die = OldTesla.th_die; //Tesla_Die;		// Death function
	newmis.th_pain = OldTesla.th_pain; //Tesla_Pain; // BUG WAS DIE!!
	//self.ammo_cells = self.ammo_cells - #BUILD_COST_TESLA;

	newmis.health = OldTesla.health; //#BUILD_HEALTH_TESLA;
	newmis.movetype = OldTesla.movetype; //#MOVETYPE_TOSS;
	newmis.colormap = OldTesla.colormap; //self.colormap;	// Set the Base Color
	//newmis.velocity = '0 0 -8';
	newmis.avelocity = '0 0 0';
	newmis.flags = OldTesla.flags; // newmis.flags - (newmis.flags & #FL_ONGROUND);
	newmis.team_no = OldTesla.team_no;

	//- OfN -
	//newmis.think = OldTesla.think;

	//newmis.nextthink = time + 0.1;
	newmis.has_holo = OldTesla.has_holo; // next Tesla_Idle run
	newmis.job = OldTesla.job; // this flag will determine which frame animation is currently on

	//newmis.job_finished = time; // change for frame animation purposes, instead of increasing its nextthing during charging
	newmis.job_finished = OldTesla.job_finished;

	newmis.no_grenades_1 = OldTesla.no_grenades_1; //#FALSE; // first think reset
	newmis.no_grenades_2 = OldTesla.no_grenades_2; //0; // cloak touch delay reset

	newmis.touch = Tesla_Touch;
	newmis.enemy = OldTesla.enemy; //world;
	newmis.oldenemy = OldTesla.oldenemy; //world; //CH for sbar

	//Set all initial tesla values here
	newmis.maxammo_shells = OldTesla.maxammo_shells; //Voltage == 0
	newmis.maxammo_nails = OldTesla.maxammo_nails; //Amps == 0
	newmis.maxammo_rockets = OldTesla.maxammo_rockets; //Battery == 0
	newmis.max_health = OldTesla.max_health;//150;
	newmis.ammo_cells = OldTesla.ammo_cells; //#MAXCELLS0; //Initial ammo allocation
	newmis.maxammo_cells = OldTesla.maxammo_cells; //#MAXCELLS0; //Initial maxammo
	newmis.tf_items = OldTesla.tf_items; //#NIT_CERAMIC; //Start with shock armor
	newmis.armorclass = OldTesla.armorclass; //#AT_SAVEELECTRICITY; //Shock armor

	newmis.has_sentry = OldTesla.has_sentry;
	newmis.has_tesla = OldTesla.has_tesla;
	newmis.has_teleporter = OldTesla.has_teleporter; //0; //CH for frag related upgrades


	newmis.#tesla_volts = OldTesla.#tesla_volts;
	newmis.#tesla_amps = OldTesla.#tesla_amps;
	newmis.#tesla_power = OldTesla.#tesla_power;

	newmis.health = OldTesla.health;
	//newmis.health = newmis.max_health;
	newmis.#tesla_batterydrain = OldTesla.#tesla_batterydrain; //(newmis.#tesla_volts + 2) * (newmis.#tesla_amps + 2);
	newmis.waitmax = OldTesla.waitmax; //#FALSE; //No target yet

	newmis.frags = OldTesla.frags; //0; //CH how many people has your sent killed?
	newmis.lip	 = OldTesla.lip; //0; //WK How many tinkers have been done
	//modelindex_tesla = newmis.modelindex; //CH

	newmis.all_active=OldTesla.all_active; //0; // OfN - reset HACKER improvements
	newmis.is_malfunctioning = OldTesla.is_malfunctioning;

	//newmis.waitmax=#FALSE;

	newmis.pain_finished=0;

	newmis.effects = OldTesla.effects;
	newmis.is_haxxxoring = OldTesla.is_haxxxoring;
	newmis.has_fieldgen = OldTesla.has_fieldgen;

	newmis.attack_finished = OldTesla.attack_finished;
	newmis.dont_do_triggerwork = OldTesla.dont_do_triggerwork;

	newmis.is_connected = OldTesla.is_connected;

	return newmis;
};
*/

/*
==============
Tesla_IsCloaked

Gizmo: Added this external function so there can
be a singular place where tesla cloaking is determined,
useful if tesla cloaking ever changes in the future to
make that easier to implement.
==============
*/
float Tesla_IsCloaked( entity tes) {
	if ( tes->classname != "building_tesla" )
		return PR_FALSE;

	if ( tes->tf_items & PR_NIT_TESLA_CLOAKING ) {
		if ( tes->job != 2 )
			return PR_TRUE;
	}

	return PR_FALSE;
}

} // END namespace Progs
