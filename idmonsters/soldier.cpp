/*
	soldier.qc - the army soldier for coop

	Changes from normal code are:
	1) Spawn function (including head model set with self.mdl, monster's actual name self.netname, and classname)
	2) Die gibbing
	3) Die frames
	4) Pain
	5) Running (at the end) - Don't think this one is needed anymore
	6) Stand
	7) precache (don't precache if server is already spawned) in spawn function
	8) call void () COOP_SetLives; in Spawn function
	9) setup spawndelay and spawnfunc in Spawn function

	TODO: use tf_t_damage: TF_T_Damage (trace_ent, self, self, 30, #TF_TD_NOTTEAM, #TF_TD_OTHER);
*/

#include "progs.h"

#ifndef PR_COOP_MODE_ENHANCED
//#pragma DONT_COMPILE_THIS_FILE
#else

#include "idmonsters/soldier.h"
#include "idmonsters/common.h"
#include "idmonsters/coop_monsters.h"
#include "coop_defs.h"
#include "ofndefs.h"
#include "qw.h"
#include "subs.h"
#include "weapons.h"
#include "ai.h"
#include "items.h"
#include "monsters.h"
#include "warlock.h"

namespace Progs {

#define PR_IDMON_GRUNT_HEALTH		200
#define PR_IDMON_GRUNT_SGUNBULLETS	10

#define PR_IDMON_GRUNT_EXPAINTIME	8

/*
==============================================================================

SOLDIER / PLAYER

==============================================================================
*/

//$cd id1/models/soldier3
//$origin 0 -6 24
//$base base
//$skin skin

namespace SoldierFrames
{
	enum {FR_STAND1, FR_STAND2, FR_STAND3, FR_STAND4, FR_STAND5, FR_STAND6, FR_STAND7, FR_STAND8};

	enum {FR_DEATH1 = 8, FR_DEATH2, FR_DEATH3, FR_DEATH4, FR_DEATH5, FR_DEATH6, FR_DEATH7, FR_DEATH8};
	enum {FR_DEATH9 = 16, FR_DEATH10};

	enum {FR_DEATHC1 = 18, FR_DEATHC2, FR_DEATHC3, FR_DEATHC4, FR_DEATHC5, FR_DEATHC6, FR_DEATHC7, FR_DEATHC8};
	enum {FR_DEATHC9 = 26, FR_DEATHC10, FR_DEATHC11};

	enum {FR_LOAD1 = 29, FR_LOAD2, FR_LOAD3, FR_LOAD4, FR_LOAD5, FR_LOAD6, FR_LOAD7, FR_LOAD8, FR_LOAD9, FR_LOAD10, FR_LOAD11};

	enum {FR_PAIN1 = 40, FR_PAIN2, FR_PAIN3, FR_PAIN4, FR_PAIN5, FR_PAIN6};

	enum {FR_PAINB1 = 46, FR_PAINB2, FR_PAINB3, FR_PAINB4, FR_PAINB5, FR_PAINB6, FR_PAINB7, FR_PAINB8, FR_PAINB9, FR_PAINB10};
	enum {FR_PAINB11 = 56, FR_PAINB12, FR_PAINB13, FR_PAINB14};

	enum {FR_PAINC1 = 60, FR_PAINC2, FR_PAINC3, FR_PAINC4, FR_PAINC5, FR_PAINC6, FR_PAINC7, FR_PAINC8, FR_PAINC9, FR_PAINC10};
	enum {FR_PAINC11 = 70, FR_PAINC12, FR_PAINC13};

	enum {FR_RUN1 = 73, FR_RUN2, FR_RUN3, FR_RUN4, FR_RUN5, FR_RUN6, FR_RUN7, FR_RUN8};

	enum {FR_SHOOT1 = 81, FR_SHOOT2, FR_SHOOT3, FR_SHOOT4, FR_SHOOT5, FR_SHOOT6, FR_SHOOT7, FR_SHOOT8, FR_SHOOT9};

	enum {FR_PROWL_1 = 90, FR_PROWL_2, FR_PROWL_3, FR_PROWL_4, FR_PROWL_5, FR_PROWL_6, FR_PROWL_7, FR_PROWL_8};
	enum {FR_PROWL_9 = 98, FR_PROWL_10, FR_PROWL_11, FR_PROWL_12, FR_PROWL_13, FR_PROWL_14, FR_PROWL_15, FR_PROWL_16};
	enum {FR_PROWL_17 = 106, FR_PROWL_18, FR_PROWL_19, FR_PROWL_20, FR_PROWL_21, FR_PROWL_22, FR_PROWL_23, FR_PROWL_24};
}

/*
==============================================================================
SOLDIER CODE
==============================================================================
*/

void army_fire();

void army_stand1() {FRAME_STATE(SoldierFrames::FR_STAND1, army_stand2); COOP_Idle();}
void army_stand2() {FRAME_STATE(SoldierFrames::FR_STAND2, army_stand3);}
void army_stand3() {FRAME_STATE(SoldierFrames::FR_STAND3, army_stand4);}
void army_stand4() {FRAME_STATE(SoldierFrames::FR_STAND4, army_stand5);}
void army_stand5() {FRAME_STATE(SoldierFrames::FR_STAND5, army_stand6); COOP_Idle();}
void army_stand6() {FRAME_STATE(SoldierFrames::FR_STAND6, army_stand7);}
void army_stand7() {FRAME_STATE(SoldierFrames::FR_STAND7, army_stand8);}
void army_stand8() {FRAME_STATE(SoldierFrames::FR_STAND8, army_stand1);}

void army_walk1() {
FRAME_STATE(SoldierFrames::FR_PROWL_1, army_walk2);
if (random() < 0.2)
	sound (self, PR_CHAN_VOICE, "soldier/idle.wav", 1, PR_ATTN_IDLE);
COOP_Walk(1);}
void army_walk2() {FRAME_STATE(SoldierFrames::FR_PROWL_2, army_walk3); COOP_Walk(1);}
void army_walk3() {FRAME_STATE(SoldierFrames::FR_PROWL_3, army_walk4); COOP_Walk(1);}
void army_walk4() {FRAME_STATE(SoldierFrames::FR_PROWL_4, army_walk5); COOP_Walk(1);}
void army_walk5() {FRAME_STATE(SoldierFrames::FR_PROWL_5, army_walk6); COOP_Walk(2);}
void army_walk6() {FRAME_STATE(SoldierFrames::FR_PROWL_6, army_walk7); COOP_Walk(3);}
void army_walk7() {FRAME_STATE(SoldierFrames::FR_PROWL_7, army_walk8); COOP_Walk(4);}
void army_walk8() {FRAME_STATE(SoldierFrames::FR_PROWL_8, army_walk9); COOP_Walk(4);}
void army_walk9() {FRAME_STATE(SoldierFrames::FR_PROWL_9, army_walk10); COOP_Walk(2);}
void army_walk10() {FRAME_STATE(SoldierFrames::FR_PROWL_10, army_walk11); COOP_Walk(2);}
void army_walk11() {FRAME_STATE(SoldierFrames::FR_PROWL_11, army_walk12); COOP_Walk(2);}
void army_walk12() {FRAME_STATE(SoldierFrames::FR_PROWL_12, army_walk13); COOP_Walk(1);}
void army_walk13() {FRAME_STATE(SoldierFrames::FR_PROWL_13, army_walk14); COOP_Walk(0);}
void army_walk14() {FRAME_STATE(SoldierFrames::FR_PROWL_14, army_walk15); COOP_Walk(1);}
void army_walk15() {FRAME_STATE(SoldierFrames::FR_PROWL_15, army_walk16); COOP_Walk(1);}
void army_walk16() {FRAME_STATE(SoldierFrames::FR_PROWL_16, army_walk17); COOP_Walk(1);}
void army_walk17() {FRAME_STATE(SoldierFrames::FR_PROWL_17, army_walk18); COOP_Walk(3);}
void army_walk18() {FRAME_STATE(SoldierFrames::FR_PROWL_18, army_walk19); COOP_Walk(3);}
void army_walk19() {FRAME_STATE(SoldierFrames::FR_PROWL_19, army_walk20); COOP_Walk(3);}
void army_walk20() {FRAME_STATE(SoldierFrames::FR_PROWL_20, army_walk21); COOP_Walk(3);}
void army_walk21() {FRAME_STATE(SoldierFrames::FR_PROWL_21, army_walk22); COOP_Walk(2);}
void army_walk22() {FRAME_STATE(SoldierFrames::FR_PROWL_22, army_walk23); COOP_Walk(1);}
void army_walk23() {FRAME_STATE(SoldierFrames::FR_PROWL_23, army_walk24); COOP_Walk(1);}
void army_walk24() {FRAME_STATE(SoldierFrames::FR_PROWL_24, army_walk1); COOP_Walk(1);}

void army_run1() {
FRAME_STATE(SoldierFrames::FR_RUN1, army_run2);
if (random() < 0.2)
	sound (self, PR_CHAN_VOICE, "soldier/idle.wav", 1, PR_ATTN_IDLE);
COOP_Run(11);}
void army_run2() {FRAME_STATE(SoldierFrames::FR_RUN2, army_run3); COOP_Run(15);}
void army_run3() {FRAME_STATE(SoldierFrames::FR_RUN3, army_run4); COOP_Run(10);}
void army_run4() {FRAME_STATE(SoldierFrames::FR_RUN4, army_run5); COOP_Run(10);}
void army_run5() {FRAME_STATE(SoldierFrames::FR_RUN5, army_run6); COOP_Run(8);}
void army_run6() {FRAME_STATE(SoldierFrames::FR_RUN6, army_run7); COOP_Run(15);}
void army_run7() {FRAME_STATE(SoldierFrames::FR_RUN7, army_run8); COOP_Run(10);}
void army_run8() {FRAME_STATE(SoldierFrames::FR_RUN8, army_run1); COOP_Run(8);}

void army_atk1() {
//	if (COOP_SightBlocked(self, self.enemy)) {
//		self.nextthink = time + 0.01;
//		self.think = self.th_run;
//	} else
		FRAME_STATE(SoldierFrames::FR_SHOOT1, army_atk2);
		COOP_Turn();
}
void army_atk2() {FRAME_STATE(SoldierFrames::FR_SHOOT2, army_atk3); COOP_Turn();}
void army_atk3() {FRAME_STATE(SoldierFrames::FR_SHOOT3, army_atk4); COOP_Turn();}
void army_atk4() {FRAME_STATE(SoldierFrames::FR_SHOOT4, army_atk5); COOP_Turn();}
void army_atk5() {FRAME_STATE(SoldierFrames::FR_SHOOT5, army_atk6); COOP_Turn();army_fire();
muzzleflash ();}
void army_atk6() {FRAME_STATE(SoldierFrames::FR_SHOOT6, army_atk7); COOP_Turn();}
void army_atk7() {FRAME_STATE(SoldierFrames::FR_SHOOT7, army_atk8); COOP_Turn();SUB_CheckRefire (army_atk1);}
void army_atk8() {FRAME_STATE(SoldierFrames::FR_SHOOT8, army_atk9); COOP_Turn();}
void army_atk9() {FRAME_STATE(SoldierFrames::FR_SHOOT9, army_run1); COOP_Turn();}


void army_pain1() {FRAME_STATE(SoldierFrames::FR_PAIN1, army_pain2);}
void army_pain2() {FRAME_STATE(SoldierFrames::FR_PAIN2, army_pain3);}
void army_pain3() {FRAME_STATE(SoldierFrames::FR_PAIN3, army_pain4);}
void army_pain4() {FRAME_STATE(SoldierFrames::FR_PAIN4, army_pain5);}
void army_pain5() {FRAME_STATE(SoldierFrames::FR_PAIN5, army_pain6);}
void army_pain6() {FRAME_STATE(SoldierFrames::FR_PAIN6, army_run1); ai_pain(1);}

void army_painb1() {FRAME_STATE(SoldierFrames::FR_PAINB1, army_painb2);}
void army_painb2() {FRAME_STATE(SoldierFrames::FR_PAINB2, army_painb3); ai_painforward(13);}
void army_painb3() {FRAME_STATE(SoldierFrames::FR_PAINB3, army_painb4); ai_painforward(9);}
void army_painb4() {FRAME_STATE(SoldierFrames::FR_PAINB4, army_painb5);}
void army_painb5() {FRAME_STATE(SoldierFrames::FR_PAINB5, army_painb6);}
void army_painb6() {FRAME_STATE(SoldierFrames::FR_PAINB6, army_painb7);}
void army_painb7() {FRAME_STATE(SoldierFrames::FR_PAINB7, army_painb8);}
void army_painb8() {FRAME_STATE(SoldierFrames::FR_PAINB8, army_painb9);}
void army_painb9() {FRAME_STATE(SoldierFrames::FR_PAINB9, army_painb10);}
void army_painb10() {FRAME_STATE(SoldierFrames::FR_PAINB10, army_painb11);}
void army_painb11() {FRAME_STATE(SoldierFrames::FR_PAINB11, army_painb12);}
void army_painb12() {FRAME_STATE(SoldierFrames::FR_PAINB12, army_painb13); ai_pain(2);}
void army_painb13() {FRAME_STATE(SoldierFrames::FR_PAINB13, army_painb14);}
void army_painb14() {FRAME_STATE(SoldierFrames::FR_PAINB14, army_run1);}

void army_painc1() {FRAME_STATE(SoldierFrames::FR_PAINC1, army_painc2);}
void army_painc2() {FRAME_STATE(SoldierFrames::FR_PAINC2, army_painc3); ai_pain(1);}
void army_painc3() {FRAME_STATE(SoldierFrames::FR_PAINC3, army_painc4);}
void army_painc4() {FRAME_STATE(SoldierFrames::FR_PAINC4, army_painc5);}
void army_painc5() {FRAME_STATE(SoldierFrames::FR_PAINC5, army_painc6); ai_painforward(1);}
void army_painc6() {FRAME_STATE(SoldierFrames::FR_PAINC6, army_painc7); ai_painforward(1);}
void army_painc7() {FRAME_STATE(SoldierFrames::FR_PAINC7, army_painc8);}
void army_painc8() {FRAME_STATE(SoldierFrames::FR_PAINC8, army_painc9); ai_pain(1);}
void army_painc9() {FRAME_STATE(SoldierFrames::FR_PAINC9, army_painc10); ai_painforward(4);}
void army_painc10() {FRAME_STATE(SoldierFrames::FR_PAINC10, army_painc11); ai_painforward(3);}
void army_painc11() {FRAME_STATE(SoldierFrames::FR_PAINC11, army_painc12); ai_painforward(6);}
void army_painc12() {FRAME_STATE(SoldierFrames::FR_PAINC12, army_painc13); ai_painforward(8);}
void army_painc13() {FRAME_STATE(SoldierFrames::FR_PAINC13, army_run1);}

void army_pain(entity attacker, float damage)
{
	float r;

	if (self->pain_finished > time)
		return;

	r = random();

	if (r < 0.2)
	{
		self->pain_finished = time + 0.6 + PR_IDMON_GRUNT_EXPAINTIME;
		army_pain1 ();
		sound (self, PR_CHAN_VOICE, "soldier/pain1.wav", 1, PR_ATTN_NORM);
	}
	else if (r < 0.6)
	{
		self->pain_finished = time + 1.1 + PR_IDMON_GRUNT_EXPAINTIME;
		army_painb1 ();
		sound (self, PR_CHAN_VOICE, "soldier/pain2.wav", 1, PR_ATTN_NORM);
	}
	else
	{
		self->pain_finished = time + 1.1 + PR_IDMON_GRUNT_EXPAINTIME;
		army_painc1 ();
		sound (self, PR_CHAN_VOICE, "soldier/pain2.wav", 1, PR_ATTN_NORM);
	}
}


void army_fire()
{
	vector	dir;
	entity	en;

	COOP_Turn();

	sound (self, PR_CHAN_WEAPON, "soldier/sattck1.wav", 1, PR_ATTN_NORM);

// fire somewhat behind the player, so a dodging player is harder to hit
	en = self->enemy;

	dir = (en->origin + self->PR_npc_enemyoffset) - en->velocity*0.1;
	dir = normalize (dir - self->origin);

	// FIXME: distance, and possibly make damage multiplied by number of players?
	deathmsg = PR_DMSG_SHOTGUN;
	FireBullets (PR_IDMON_GRUNT_SGUNBULLETS, dir, V({0.11, 0.11, 0}), 9999);

	Attack_Finished( random() + 1 );
}



void army_die1() {FRAME_STATE(SoldierFrames::FR_DEATH1, army_die2);}
void army_die2() {FRAME_STATE(SoldierFrames::FR_DEATH2, army_die3);}
void army_die3()
{FRAME_STATE(SoldierFrames::FR_DEATH3, army_die4); self->solid = PR_SOLID_NOT;self->ammo_shells = 15;DropBackpack();}
void army_die4() {FRAME_STATE(SoldierFrames::FR_DEATH4, army_die5);}
void army_die5() {FRAME_STATE(SoldierFrames::FR_DEATH5, army_die6);}
void army_die6() {FRAME_STATE(SoldierFrames::FR_DEATH6, army_die7);}
void army_die7() {FRAME_STATE(SoldierFrames::FR_DEATH7, army_die8);}
void army_die8() {FRAME_STATE(SoldierFrames::FR_DEATH8, army_die9);}
void army_die9() {FRAME_STATE(SoldierFrames::FR_DEATH9, army_die10);}
void army_die10() {FRAME_STATE(SoldierFrames::FR_DEATH10, army_die10); MonsterCorpse();}

void army_cdie1() {FRAME_STATE(SoldierFrames::FR_DEATHC1, army_cdie2);}
void army_cdie2() {FRAME_STATE(SoldierFrames::FR_DEATHC2, army_cdie3); ai_back(5);}
void army_cdie3()
{FRAME_STATE(SoldierFrames::FR_DEATHC3, army_cdie4); self->solid = PR_SOLID_NOT;self->ammo_shells = 15;DropBackpack();ai_back(4);}
void army_cdie4() {FRAME_STATE(SoldierFrames::FR_DEATHC4, army_cdie5); ai_back(13);}
void army_cdie5() {FRAME_STATE(SoldierFrames::FR_DEATHC5, army_cdie6); ai_back(3);}
void army_cdie6() {FRAME_STATE(SoldierFrames::FR_DEATHC6, army_cdie7); ai_back(4);}
void army_cdie7() {FRAME_STATE(SoldierFrames::FR_DEATHC7, army_cdie8);}
void army_cdie8() {FRAME_STATE(SoldierFrames::FR_DEATHC8, army_cdie9);}
void army_cdie9() {FRAME_STATE(SoldierFrames::FR_DEATHC9, army_cdie10);}
void army_cdie10() {FRAME_STATE(SoldierFrames::FR_DEATHC10, army_cdie11);}
void army_cdie11() {FRAME_STATE(SoldierFrames::FR_DEATHC11, army_cdie11); MonsterCorpse();}


void army_die()
{
// check for gib
	if (self->health < -35)
	{
		sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_NORM);
		ThrowMonsterHead ("progs/h_guard.mdl", self->health);
		ThrowGib("progs/gib1.mdl", -40, PR_TRUE, 0, PR_GIB1_KGS, PR_FALSE);
		ThrowGib("progs/gib2.mdl", -40, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
		ThrowGib("progs/gib3.mdl", -40, PR_TRUE, 0, PR_GIB3_KGS, PR_FALSE);
		remove (self);
		return;
	}

// regular death
	sound (self, PR_CHAN_VOICE, "soldier/death1.wav", 1, PR_ATTN_NORM);
	if (random() < 0.5)
		army_die1 ();
	else
		army_cdie1 ();
}


float ArmyOnSight()
{
	sound (self, PR_CHAN_VOICE, "soldier/sight1.wav", 1, PR_ATTN_NORM);
	return PR_TRUE;
}


/*
============
_SoldierCheckAttack
============
*/
float _SoldierCheckAttack() {
	vector	spot1, spot2;
	entity	targ;
	float		chance;

	targ = self->enemy;

// see if any entities are in the way of the shot
	spot1 = self->origin + self->view_ofs;
	spot2 = targ->origin + targ->view_ofs;

	traceline( spot1, spot2, PR_TL_ANY_SOLID, self );

	if ( trace_ent != targ )
		return PR_FALSE;	// don't have a clear shot

// missile attack
	enemy_range = range( self->enemy );

	if ( enemy_range == PR_RANGE_FAR )
		return PR_FALSE;

	if ( enemy_range == PR_RANGE_MELEE )
		chance = 0.9;
	else if ( enemy_range == PR_RANGE_NEAR )
		chance = 0.4;
	else if ( enemy_range == PR_RANGE_MID )
		chance = 0.05;
	else
		chance = 0;

	if ( random() < chance ) {
//		Attack_Finished( 1 + random() );
		if ( random() < 0.3 )
			self->lefty = !self->lefty;

		return PR_TRUE;
	}

	return PR_FALSE;
}


/*QUAKED monster_army (1 0 0) (-16 -16 -24) (16 16 40) Ambush
*/
void monster_army()
{
	if ( deathmatch ) {
		remove( self );
		return;
	}

	self->classname = "idmonster_army";

	if (!server_spawned) {
		precache_model ("progs/soldier.mdl");
		precache_model ("progs/h_guard.mdl");
		precache_model ("progs/gib1.mdl");
		precache_model ("progs/gib2.mdl");
		precache_model ("progs/gib3.mdl");

		precache_sound ("soldier/death1.wav");
		precache_sound ("soldier/idle.wav");
		precache_sound ("soldier/pain1.wav");
		precache_sound ("soldier/pain2.wav");
		precache_sound ("soldier/sattck1.wav");
		precache_sound ("soldier/sight1.wav");

		precache_sound ("player/udeath.wav");		// gib death
	}


	self->solid = PR_SOLID_SLIDEBOX;
	self->movetype = PR_MOVETYPE_STEP;

	// make sure to set this if you want corpse gibs to include head
	self->mdl = "progs/h_guard.mdl";
	setmodel (self, "progs/soldier.mdl");

	setsize (self, V({-16, -16, -24}), V({16, 16, 40}));

	self->max_health = self->health = PR_IDMON_GRUNT_HEALTH;

	self->th_stand = army_stand1;
	self->th_walk = army_walk1;
	self->th_run = army_run1;
	self->th_missile = army_atk1;
	self->th_pain = army_pain;
	self->th_die = army_die;


	self->netname = "grunt";

	self->monsterweight = PR_MWEIGHT_NORMAL;
	self->monsterflags = PR_MFLAG_HASWEAPON | PR_MFLAG_HUMANOID;

	// monsters have a team of their own
	if ( !self->team_no )
		self->team_no = PR_TEAMNO_MONSTERS;
	self->PR_monsterflag = PR_STRFLAG_MONSTER;

	self->is_connected = PR_TRUE;

	self->OnNoticeEnemy = ArmyOnSight;
	self->OnPreAttack = _SoldierCheckAttack;
	self->PR_npc_spawnfunc = monster_army;
	COOP_SetLives ();

	self->PR_npc_bulletmult = 0.75;

	// set ammo for emp
	self->ammo_shells = 75;

	coop_walkmonster_start ();
}

} // END namespace Progs

#endif
