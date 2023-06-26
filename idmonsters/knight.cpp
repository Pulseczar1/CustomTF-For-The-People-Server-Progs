/*
	knight.qc

	TODO: ai_melee_side, ai_charge, ai_charge_side, ai_melee
*/

#include "progs.h"
#include "idmonsters/knight.h"
#include "idmonsters/common.h"
#include "idmonsters/coop_monsters.h"
#include "ofndefs.h"
#include "coop_defs.h"
#include "fight.h"
#include "ai.h"
#include "monsters.h"
#include "warlock.h"
#include "player.h"

#ifndef PR_COOP_MODE_ENHANCED
//#pragma DONT_COMPILE_THIS_FILE
#else

namespace Progs {

#define PR_IDMON_KNIGHT_HEALTH		100
#define PR_IDMON_KNIGHT_ARMORVALUE	200
#define PR_IDMON_KNIGHT_ARMORTYPE	0.8
#define PR_IDMON_KNIGHT_SLASHDMG		14

/*
==============================================================================

KNIGHT

==============================================================================
*/

//$cd id1/models/knight
//$origin 0 0 24
//$base base
//$skin badass3

namespace KnightFrames
{
	enum {FR_STAND1, FR_STAND2, FR_STAND3, FR_STAND4, FR_STAND5, FR_STAND6, FR_STAND7, FR_STAND8, FR_STAND9};

	enum {FR_RUNB1 = 9, FR_RUNB2, FR_RUNB3, FR_RUNB4, FR_RUNB5, FR_RUNB6, FR_RUNB7, FR_RUNB8};

	//frame runc1 runc2 runc3 runc4 runc5 runc6

	enum {FR_RUNATTACK1 = 17, FR_RUNATTACK2, FR_RUNATTACK3, FR_RUNATTACK4, FR_RUNATTACK5};
	enum {FR_RUNATTACK6 = 22, FR_RUNATTACK7, FR_RUNATTACK8, FR_RUNATTACK9, FR_RUNATTACK10};
	enum {FR_RUNATTACK11 = 27};

	enum {FR_PAIN1 = 28, FR_PAIN2, FR_PAIN3};

	enum {FR_PAINB1 = 31, FR_PAINB2, FR_PAINB3, FR_PAINB4, FR_PAINB5, FR_PAINB6, FR_PAINB7, FR_PAINB8, FR_PAINB9};
	enum {FR_PAINB10 = 40, FR_PAINB11};

	//frame attack1 attack2 attack3 attack4 attack5 attack6 attack7
	//frame attack8 attack9 attack10 attack11

	// PZ: I commented out the .DUP one. Not sure why that's there.
	enum {FR_ATTACKB1 = 42, /*FR_ATTACKB1.DUP,*/ FR_ATTACKB2, FR_ATTACKB3, FR_ATTACKB4, FR_ATTACKB5};
	enum {FR_ATTACKB6 = 47, FR_ATTACKB7, FR_ATTACKB8, FR_ATTACKB9, FR_ATTACKB10};

	enum {FR_WALK1 = 52, FR_WALK2, FR_WALK3, FR_WALK4, FR_WALK5, FR_WALK6, FR_WALK7, FR_WALK8, FR_WALK9};
	enum {FR_WALK10 = 61, FR_WALK11, FR_WALK12, FR_WALK13, FR_WALK14};

	enum {FR_KNEEL1 = 66, FR_KNEEL2, FR_KNEEL3, FR_KNEEL4, FR_KNEEL5};

	enum {FR_STANDING2 = 71, FR_STANDING3, FR_STANDING4, FR_STANDING5};

	enum {FR_DEATH1 = 75, FR_DEATH2, FR_DEATH3, FR_DEATH4, FR_DEATH5, FR_DEATH6, FR_DEATH7, FR_DEATH8};
	enum {FR_DEATH9 = 83, FR_DEATH10};

	enum {FR_DEATHB1 = 85, FR_DEATHB2, FR_DEATHB3, FR_DEATHB4, FR_DEATHB5, FR_DEATHB6, FR_DEATHB7, FR_DEATHB8};
	enum {FR_DEATHB9 = 93, FR_DEATHB10, FR_DEATHB11};
}

void knight_stand1() {FRAME_STATE(KnightFrames::FR_STAND1, knight_stand2); COOP_Idle();}
void knight_stand2() {FRAME_STATE(KnightFrames::FR_STAND2, knight_stand3);}
void knight_stand3() {FRAME_STATE(KnightFrames::FR_STAND3, knight_stand4);}
void knight_stand4() {FRAME_STATE(KnightFrames::FR_STAND4, knight_stand5);}
void knight_stand5() {FRAME_STATE(KnightFrames::FR_STAND5, knight_stand6); COOP_Idle();}
void knight_stand6() {FRAME_STATE(KnightFrames::FR_STAND6, knight_stand7);}
void knight_stand7() {FRAME_STATE(KnightFrames::FR_STAND7, knight_stand8);}
void knight_stand8() {FRAME_STATE(KnightFrames::FR_STAND8, knight_stand9);}
void knight_stand9() {FRAME_STATE(KnightFrames::FR_STAND9, knight_stand1);}

void knight_walk1() {
FRAME_STATE(KnightFrames::FR_WALK1, knight_walk2);
if (random() < 0.2)
	sound (self, PR_CHAN_VOICE, "knight/idle.wav", 1,  PR_ATTN_IDLE);
COOP_Walk(3);}
void knight_walk2() {FRAME_STATE(KnightFrames::FR_WALK2, knight_walk3); COOP_Walk(2);}
void knight_walk3() {FRAME_STATE(KnightFrames::FR_WALK3, knight_walk4); COOP_Walk(3);}
void knight_walk4() {FRAME_STATE(KnightFrames::FR_WALK4, knight_walk5); COOP_Walk(4);}
void knight_walk5() {FRAME_STATE(KnightFrames::FR_WALK5, knight_walk6); COOP_Walk(3);}
void knight_walk6() {FRAME_STATE(KnightFrames::FR_WALK6, knight_walk7); COOP_Walk(3);}
void knight_walk7() {FRAME_STATE(KnightFrames::FR_WALK7, knight_walk8); COOP_Walk(3);}
void knight_walk8() {FRAME_STATE(KnightFrames::FR_WALK8, knight_walk9); COOP_Walk(4);}
void knight_walk9() {FRAME_STATE(KnightFrames::FR_WALK9, knight_walk10); COOP_Walk(3);}
void knight_walk10() {FRAME_STATE(KnightFrames::FR_WALK10, knight_walk11); COOP_Walk(3);}
void knight_walk11() {FRAME_STATE(KnightFrames::FR_WALK11, knight_walk12); COOP_Walk(2);}
void knight_walk12() {FRAME_STATE(KnightFrames::FR_WALK12, knight_walk13); COOP_Walk(3);}
void knight_walk13() {FRAME_STATE(KnightFrames::FR_WALK13, knight_walk14); COOP_Walk(4);}
void knight_walk14() {FRAME_STATE(KnightFrames::FR_WALK14, knight_walk1); COOP_Walk(3);}


void knight_run1() {
FRAME_STATE(KnightFrames::FR_RUNB1, knight_run2);
if (random() < 0.2)
	sound (self, PR_CHAN_VOICE, "knight/idle.wav", 1,  PR_ATTN_IDLE);
COOP_Run(16);}
void knight_run2() {FRAME_STATE(KnightFrames::FR_RUNB2, knight_run3); COOP_Run(20);}
void knight_run3() {FRAME_STATE(KnightFrames::FR_RUNB3, knight_run4); COOP_Run(13);}
void knight_run4() {FRAME_STATE(KnightFrames::FR_RUNB4, knight_run5); COOP_Run(7);}
void knight_run5() {FRAME_STATE(KnightFrames::FR_RUNB5, knight_run6); COOP_Run(16);}
void knight_run6() {FRAME_STATE(KnightFrames::FR_RUNB6, knight_run7); COOP_Run(20);}
void knight_run7() {FRAME_STATE(KnightFrames::FR_RUNB7, knight_run8); COOP_Run(14);}
void knight_run8() {FRAME_STATE(KnightFrames::FR_RUNB8, knight_run1); COOP_Run(6);}


void knight_runatk1()
{
FRAME_STATE(KnightFrames::FR_RUNATTACK1, knight_runatk2);
if (random() > 0.5)
	sound (self, PR_CHAN_WEAPON, "knight/sword2.wav", 1, PR_ATTN_NORM);
else
	sound (self, PR_CHAN_WEAPON, "knight/sword1.wav", 1, PR_ATTN_NORM);
ai_charge(20);
}
void knight_runatk2() {FRAME_STATE(KnightFrames::FR_RUNATTACK2, knight_runatk3); ai_charge_side();}
void knight_runatk3() {FRAME_STATE(KnightFrames::FR_RUNATTACK3, knight_runatk4); ai_charge_side();}
void knight_runatk4() {FRAME_STATE(KnightFrames::FR_RUNATTACK4, knight_runatk5); ai_charge_side();}
void knight_runatk5() {FRAME_STATE(KnightFrames::FR_RUNATTACK5, knight_runatk6); ai_melee_side();}
void knight_runatk6() {FRAME_STATE(KnightFrames::FR_RUNATTACK6, knight_runatk7); ai_melee_side();}
void knight_runatk7() {FRAME_STATE(KnightFrames::FR_RUNATTACK7, knight_runatk8); ai_melee_side();}
void knight_runatk8() {FRAME_STATE(KnightFrames::FR_RUNATTACK8, knight_runatk9); ai_melee_side();}
void knight_runatk9() {FRAME_STATE(KnightFrames::FR_RUNATTACK9, knight_runatk10); ai_melee_side();}
void knight_runatk10() {FRAME_STATE(KnightFrames::FR_RUNATTACK10, knight_runatk11); ai_charge_side();}
void knight_runatk11() {FRAME_STATE(KnightFrames::FR_RUNATTACK11, knight_run1); ai_charge(10);}

void knight_atk1()
{
FRAME_STATE(KnightFrames::FR_ATTACKB1, knight_atk2);
sound (self, PR_CHAN_WEAPON, "knight/sword1.wav", 1, PR_ATTN_NORM);
ai_charge(0);}
void knight_atk2() {FRAME_STATE(KnightFrames::FR_ATTACKB2, knight_atk3); ai_charge(7);}
void knight_atk3() {FRAME_STATE(KnightFrames::FR_ATTACKB3, knight_atk4); ai_charge(4);}
void knight_atk4() {FRAME_STATE(KnightFrames::FR_ATTACKB4, knight_atk5); ai_charge(0);}
void knight_atk5() {FRAME_STATE(KnightFrames::FR_ATTACKB5, knight_atk6); ai_charge(3);}
void knight_atk6() {FRAME_STATE(KnightFrames::FR_ATTACKB6, knight_atk7); ai_charge(4); ai_melee();}
void knight_atk7() {FRAME_STATE(KnightFrames::FR_ATTACKB7, knight_atk8); ai_charge(1); ai_melee();}
void knight_atk8() {FRAME_STATE(KnightFrames::FR_ATTACKB8, knight_atk9); ai_charge(3);
ai_melee();}
void knight_atk9() {FRAME_STATE(KnightFrames::FR_ATTACKB9, knight_atk10); ai_charge(1);}
void knight_atk10() {FRAME_STATE(KnightFrames::FR_ATTACKB10, knight_run1); ai_charge(5);}

//void()	knight_atk9	=[	$attack9,		knight_atk10	] {};
//void()	knight_atk10	=[	$attack10,		knight_atk11	] {};
//void()	knight_atk11	=[	$attack11,		knight_run1	] {};

//===========================================================================

void knight_pain1() {FRAME_STATE(KnightFrames::FR_PAIN1, knight_pain2);}
void knight_pain2() {FRAME_STATE(KnightFrames::FR_PAIN2, knight_pain3);}
void knight_pain3() {FRAME_STATE(KnightFrames::FR_PAIN3, knight_run1);}

void knight_painb1() {FRAME_STATE(KnightFrames::FR_PAINB1, knight_painb2); ai_painforward(0);}
void knight_painb2() {FRAME_STATE(KnightFrames::FR_PAINB2, knight_painb3); ai_painforward(3);}
void knight_painb3() {FRAME_STATE(KnightFrames::FR_PAINB3, knight_painb4);}
void knight_painb4() {FRAME_STATE(KnightFrames::FR_PAINB4, knight_painb5);}
void knight_painb5() {FRAME_STATE(KnightFrames::FR_PAINB5, knight_painb6); ai_painforward(2);}
void knight_painb6() {FRAME_STATE(KnightFrames::FR_PAINB6, knight_painb7); ai_painforward(4);}
void knight_painb7() {FRAME_STATE(KnightFrames::FR_PAINB7, knight_painb8); ai_painforward(2);}
void knight_painb8() {FRAME_STATE(KnightFrames::FR_PAINB8, knight_painb9); ai_painforward(5);}
void knight_painb9() {FRAME_STATE(KnightFrames::FR_PAINB9, knight_painb10); ai_painforward(5);}
void knight_painb10() {FRAME_STATE(KnightFrames::FR_PAINB10, knight_painb11); ai_painforward(0);}
void knight_painb11() {FRAME_STATE(KnightFrames::FR_PAINB11, knight_run1);}

void knight_pain(entity attacker, float damage)
{
	float r;

	// don't bother doing pain if on hard mode
	if (skill >= 2)
		return;

	if (self->pain_finished > time)
		return;

	r = random();

	sound (self, PR_CHAN_VOICE, "knight/khurt.wav", 1, PR_ATTN_NORM);
	if (r < 0.85)
	{
		knight_pain1 ();
		self->pain_finished = time + 1;
	}
	else
	{
		knight_painb1 ();
		self->pain_finished = time + 1;
	}

}

//===========================================================================

void knight_bow1() {FRAME_STATE(KnightFrames::FR_KNEEL1, knight_bow2); COOP_Turn();}
void knight_bow2() {FRAME_STATE(KnightFrames::FR_KNEEL2, knight_bow3); COOP_Turn();}
void knight_bow3() {FRAME_STATE(KnightFrames::FR_KNEEL3, knight_bow4); COOP_Turn();}
void knight_bow4() {FRAME_STATE(KnightFrames::FR_KNEEL4, knight_bow5); COOP_Turn();}

void knight_bow5() {FRAME_STATE(KnightFrames::FR_KNEEL5, knight_bow5); COOP_Turn();}

void knight_bow6() {FRAME_STATE(KnightFrames::FR_KNEEL4, knight_bow7); COOP_Turn();}
void knight_bow7() {FRAME_STATE(KnightFrames::FR_KNEEL3, knight_bow8); COOP_Turn();}
void knight_bow8() {FRAME_STATE(KnightFrames::FR_KNEEL2, knight_bow9); COOP_Turn();}
void knight_bow9() {FRAME_STATE(KnightFrames::FR_KNEEL1, knight_bow10); COOP_Turn();}
void knight_bow10() {FRAME_STATE(KnightFrames::FR_WALK1, knight_walk1); COOP_Turn();}



void knight_die1() {FRAME_STATE(KnightFrames::FR_DEATH1, knight_die2);}
void knight_die2() {FRAME_STATE(KnightFrames::FR_DEATH2, knight_die3);}
void knight_die3()
{FRAME_STATE(KnightFrames::FR_DEATH3, knight_die4); self->solid = PR_SOLID_NOT;}
void knight_die4() {FRAME_STATE(KnightFrames::FR_DEATH4, knight_die5);}
void knight_die5() {FRAME_STATE(KnightFrames::FR_DEATH5, knight_die6);}
void knight_die6() {FRAME_STATE(KnightFrames::FR_DEATH6, knight_die7);}
void knight_die7() {FRAME_STATE(KnightFrames::FR_DEATH7, knight_die8);}
void knight_die8() {FRAME_STATE(KnightFrames::FR_DEATH8, knight_die9);}
void knight_die9() {FRAME_STATE(KnightFrames::FR_DEATH9, knight_die10);}
void knight_die10() {FRAME_STATE(KnightFrames::FR_DEATH10, knight_die10); MonsterCorpse();}


void knight_dieb1() {FRAME_STATE(KnightFrames::FR_DEATHB1, knight_dieb2);}
void knight_dieb2() {FRAME_STATE(KnightFrames::FR_DEATHB2, knight_dieb3);}
void knight_dieb3()
{FRAME_STATE(KnightFrames::FR_DEATHB3, knight_dieb4); self->solid = PR_SOLID_NOT;}
void knight_dieb4() {FRAME_STATE(KnightFrames::FR_DEATHB4, knight_dieb5);}
void knight_dieb5() {FRAME_STATE(KnightFrames::FR_DEATHB5, knight_dieb6);}
void knight_dieb6() {FRAME_STATE(KnightFrames::FR_DEATHB6, knight_dieb7);}
void knight_dieb7() {FRAME_STATE(KnightFrames::FR_DEATHB7, knight_dieb8);}
void knight_dieb8() {FRAME_STATE(KnightFrames::FR_DEATHB8, knight_dieb9);}
void knight_dieb9() {FRAME_STATE(KnightFrames::FR_DEATHB9, knight_dieb10);}
void knight_dieb10()  {FRAME_STATE(KnightFrames::FR_DEATHB10, knight_dieb11);}
void knight_dieb11()  {FRAME_STATE(KnightFrames::FR_DEATHB11, knight_dieb11); MonsterCorpse();}


void knight_die()
{
// check for gib
	if (self->health < -40)
	{
		sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_NORM);
		ThrowMonsterHead ("progs/h_knight.mdl", self->health);
		ThrowGib("progs/gib1.mdl", -40, PR_TRUE, 0, PR_GIB1_KGS, PR_FALSE);
		ThrowGib("progs/gib2.mdl", -40, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
		ThrowGib("progs/gib3.mdl", -40, PR_TRUE, 0, PR_GIB3_KGS, PR_FALSE);
		remove (self);
		return;
	}

// regular death
	sound (self, PR_CHAN_VOICE, "knight/kdeath.wav", 1, PR_ATTN_NORM);
	if (random() < 0.5)
		knight_die1 ();
	else
		knight_dieb1 ();
}


float KnightPreAttack()
{
	float		len;

// decide if now is a good swing time
	len = vlen(self->enemy->origin+self->enemy->view_ofs - (self->origin+self->view_ofs));

/*
	// TODO: make ai_charge_side and ai_melee_side for coop
	if (len < 80)
		knight_atk1 ();
	else
		knight_runatk1 ();
*/
	if (len < 80)
		return PR_TRUE;

	return PR_FALSE;
}

float KnightOnSight()
{
	sound (self, PR_CHAN_VOICE, "knight/ksight.wav", 1, PR_ATTN_NORM);
	return PR_TRUE;
}


/*QUAKED monster_knight (1 0 0) (-16 -16 -24) (16 16 40) Ambush
*/
void monster_knight()
{
	if ( deathmatch ) {
		remove( self );
		return;
	}

	self->classname = "idmonster_knight";

	if (!server_spawned) {
		precache_model ("progs/knight.mdl");
		precache_model ("progs/h_knight.mdl");

		precache_sound ("knight/kdeath.wav");
		precache_sound ("knight/khurt.wav");
		precache_sound ("knight/ksight.wav");
		precache_sound ("knight/sword1.wav");
		precache_sound ("knight/sword2.wav");
		precache_sound ("knight/idle.wav");
	}

	self->solid = PR_SOLID_SLIDEBOX;
	self->movetype = PR_MOVETYPE_STEP;

	self->mdl = "progs/h_knight.mdl";
	setmodel (self, "progs/knight.mdl");

	setsize (self, V({-16, -16, -24}), V({16, 16, 40}));
	self->max_health = self->health = PR_IDMON_KNIGHT_HEALTH;
	self->armorvalue = PR_IDMON_KNIGHT_ARMORVALUE;
	self->armortype = PR_IDMON_KNIGHT_ARMORTYPE;

	self->th_stand = knight_stand1;
	self->th_walk = knight_walk1;
	self->th_run = knight_run1;
	self->th_melee = knight_atk1;
	self->th_pain = knight_pain;
	self->th_die = knight_die;


	self->netname = "knight";

	self->monsterweight = PR_MWEIGHT_NORMAL;
	self->monsterflags = PR_MFLAG_HUMANOID;

	// monsters have a team of their own
	if ( !self->team_no )
		self->team_no = PR_TEAMNO_MONSTERS;
	self->PR_monsterflag = PR_STRFLAG_MONSTER;

	self->is_connected = PR_TRUE;


	self->OnPreAttack = KnightPreAttack;
	self->OnNoticeEnemy = KnightOnSight;
	self->PR_npc_speedfactor = 2;
	self->PR_npc_spawnfunc = monster_knight;
	COOP_SetLives ();

	self->PR_npc_bulletmult = 0.80;
	self->PR_npc_spikemult = 0.80;
	self->PR_npc_electricitymult = 3;
	self->PR_npc_meleemult = 0.50;

	coop_walkmonster_start ();
}

} // END namespace Progs

#endif
