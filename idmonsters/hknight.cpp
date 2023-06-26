/*
	hknight.qc

	FIXME: ai_melee
*/

#include "progs.h"

#ifndef PR_COOP_MODE_ENHANCED
//#pragma DONT_COMPILE_THIS_FILE
#else

#include "idmonsters/hknight.h"
#include "ofndefs.h"
#include "coop_defs.h"
#include "weapons.h"
#include "monsters.h"
#include "common.h"
#include "ai.h"
#include "warlock.h"
#include "fight.h"
#include "coop_monsters.h"

namespace Progs {

#define PR_IDMON_HKNIGHT_HEALTH		100
#define PR_IDMON_HKNIGHT_ARMORVALUE	300
#define PR_IDMON_HKNIGHT_ARMORTYPE	0.8
#define PR_IDMON_HKNIGHT_SPIKEDMG	70	// was 85

/*
==============================================================================

KNIGHT

==============================================================================
*/

//$cd id1/models/knight2
//$origin 0 0 24
//$base base
//$skin skin

namespace HKnightFrames
{
	enum {FR_STAND1, FR_STAND2, FR_STAND3, FR_STAND4, FR_STAND5, FR_STAND6, FR_STAND7, FR_STAND8, FR_STAND9};

	enum {FR_WALK1 = 9, FR_WALK2, FR_WALK3, FR_WALK4, FR_WALK5, FR_WALK6, FR_WALK7, FR_WALK8, FR_WALK9};
	enum {FR_WALK10 = 18, FR_WALK11, FR_WALK12, FR_WALK13, FR_WALK14, FR_WALK15, FR_WALK16, FR_WALK17};
	enum {FR_WALK18 = 26, FR_WALK19, FR_WALK20};

	enum {FR_RUN1 = 29, FR_RUN2, FR_RUN3, FR_RUN4, FR_RUN5, FR_RUN6, FR_RUN7, FR_RUN8};

	enum {FR_PAIN1 = 37, FR_PAIN2, FR_PAIN3, FR_PAIN4, FR_PAIN5};

	enum {FR_DEATH1 = 42, FR_DEATH2, FR_DEATH3, FR_DEATH4, FR_DEATH5, FR_DEATH6, FR_DEATH7, FR_DEATH8};
	enum {FR_DEATH9 = 50, FR_DEATH10, FR_DEATH11, FR_DEATH12};

	enum {FR_DEATHB1 = 54, FR_DEATHB2, FR_DEATHB3, FR_DEATHB4, FR_DEATHB5, FR_DEATHB6, FR_DEATHB7, FR_DEATHB8};
	enum {FR_DEATHB9 = 62};

	enum {FR_CHAR_A1 = 63, FR_CHAR_A2, FR_CHAR_A3, FR_CHAR_A4, FR_CHAR_A5, FR_CHAR_A6, FR_CHAR_A7, FR_CHAR_A8};
	enum {FR_CHAR_A9 = 71, FR_CHAR_A10, FR_CHAR_A11, FR_CHAR_A12, FR_CHAR_A13, FR_CHAR_A14, FR_CHAR_A15, FR_CHAR_A16};

	enum {FR_MAGICA1 = 79, FR_MAGICA2, FR_MAGICA3, FR_MAGICA4, FR_MAGICA5, FR_MAGICA6, FR_MAGICA7, FR_MAGICA8};
	enum {FR_MAGICA9 = 87, FR_MAGICA10, FR_MAGICA11, FR_MAGICA12, FR_MAGICA13, FR_MAGICA14};

	enum {FR_MAGICB1 = 93, FR_MAGICB2, FR_MAGICB3, FR_MAGICB4, FR_MAGICB5, FR_MAGICB6, FR_MAGICB7, FR_MAGICB8};
	enum {FR_MAGICB9 = 101, FR_MAGICB10, FR_MAGICB11, FR_MAGICB12, FR_MAGICB13};

	enum {FR_CHAR_B1 = 106, FR_CHAR_B2, FR_CHAR_B3, FR_CHAR_B4, FR_CHAR_B5, FR_CHAR_B6};

	enum {FR_SLICE1 = 112, FR_SLICE2, FR_SLICE3, FR_SLICE4, FR_SLICE5, FR_SLICE6, FR_SLICE7, FR_SLICE8, FR_SLICE9, FR_SLICE10};

	enum {FR_SMASH1 = 122, FR_SMASH2, FR_SMASH3, FR_SMASH4, FR_SMASH5, FR_SMASH6, FR_SMASH7, FR_SMASH8, FR_SMASH9, FR_SMASH10};
	enum {FR_SMASH11 = 132};

	enum {FR_W_ATTACK1 = 133, FR_W_ATTACK2, FR_W_ATTACK3, FR_W_ATTACK4, FR_W_ATTACK5, FR_W_ATTACK6, FR_W_ATTACK7};
	enum {FR_W_ATTACK8 = 140, FR_W_ATTACK9, FR_W_ATTACK10, FR_W_ATTACK11, FR_W_ATTACK12, FR_W_ATTACK13, FR_W_ATTACK14};
	enum {FR_W_ATTACK15 = 147, FR_W_ATTACK16, FR_W_ATTACK17, FR_W_ATTACK18, FR_W_ATTACK19, FR_W_ATTACK20};
	enum {FR_W_ATTACK21 = 153, FR_W_ATTACK22};

	enum {FR_MAGICC1 = 155, FR_MAGICC2, FR_MAGICC3, FR_MAGICC4, FR_MAGICC5, FR_MAGICC6, FR_MAGICC7, FR_MAGICC8};
	enum {FR_MAGICC9 = 163, FR_MAGICC10, FR_MAGICC11};
}

void hknight_char_a1();
void hknight_run1();
void hk_idle_sound();


void hknightspike_touch()
{
	if (other == self->owner)
		return;

	if (other->solid == PR_SOLID_TRIGGER)
		return; // trigger field, do nothing

	if (pointcontents(self->origin) == PR_CONTENT_SKY) {
		remove(self);
		return;
	}

// hit something that bleeds
	if (other->takedamage) {
		spawn_touchblood (PR_IDMON_HKNIGHT_SPIKEDMG);
		other->deathtype = "nail";
		TF_T_Damage (other, self, self->owner, PR_IDMON_HKNIGHT_SPIKEDMG + random()*10, PR_TF_TD_NOTTEAM, PR_TF_TD_NAIL);
	} else {
		WriteByte (PR_MSG_MULTICAST, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_MULTICAST, PR_TE_KNIGHTSPIKE);
		WriteCoord (PR_MSG_MULTICAST, self->origin[X]);
		WriteCoord (PR_MSG_MULTICAST, self->origin[Y]);
		WriteCoord (PR_MSG_MULTICAST, self->origin[Z]);
		multicast (self->origin, PR_MULTICAST_PHS);
	}

	remove(self);
}

void hknight_shot(float offset)
{
	vector	offang;
	vector	org, vec;

	offang = vectoangles (self->enemy->origin - self->origin);
	offang[Y] = offang[Y] + offset * 6;

	makevectors (offang);

	org = self->origin + self->mins + self->size*0.5 + v_forward * 20;

// set missile speed
	vec = normalize (v_forward);
	vec[Z] = 0 - vec[Z] + (random() - 0.5)*0.1;


	newmis = spawn ();
	newmis->movetype = PR_MOVETYPE_FLYMISSILE;
	newmis->solid = PR_SOLID_BBOX;

	newmis->angles = vectoangles(vec);

	newmis->touch = hknightspike_touch;
	newmis->think = SUB_Remove;
	newmis->nextthink = time + 6;
	setorigin (newmis, org);


	newmis->classname = "knightspike";
	newmis->owner = self;
	setmodel (newmis, "progs/k_spike.mdl");
	setsize (newmis, PR_VEC_ORIGIN, PR_VEC_ORIGIN);
	newmis->velocity = vec*820;		// was 900
	sound (self, PR_CHAN_WEAPON, "hknight/attack1.wav", 1, PR_ATTN_NORM);
}

void CheckForCharge()
{
	enemy_vis = visible (self->enemy);

// check for mad charge
if (!enemy_vis)
	return;
if (time < self->attack_finished)
	return;
if ( fabs(self->origin[Z] - self->enemy->origin[Z]) > 20)
	return;		// too much height change
if ( vlen (self->origin - self->enemy->origin) < 80)
	return;		// use regular attack

// charge
	Attack_Finished( 2 );
	hknight_char_a1 ();

}

void CheckContinueCharge()
{
	if (time > self->attack_finished)
	{
		Attack_Finished( 3 );
		hknight_run1 ();
		return;		// done charging
	}
	if (random() > 0.5)
		sound (self, PR_CHAN_WEAPON, "knight/sword2.wav", 1, PR_ATTN_NORM);
	else
		sound (self, PR_CHAN_WEAPON, "knight/sword1.wav", 1, PR_ATTN_NORM);
}

//===========================================================================

void hknight_stand1() {FRAME_STATE(HKnightFrames::FR_STAND1, hknight_stand2); COOP_Idle();}
void hknight_stand2() {FRAME_STATE(HKnightFrames::FR_STAND2, hknight_stand3);}
void hknight_stand3() {FRAME_STATE(HKnightFrames::FR_STAND3, hknight_stand4);}
void hknight_stand4() {FRAME_STATE(HKnightFrames::FR_STAND4, hknight_stand5);}
void hknight_stand5() {FRAME_STATE(HKnightFrames::FR_STAND5, hknight_stand6); COOP_Idle();}
void hknight_stand6() {FRAME_STATE(HKnightFrames::FR_STAND6, hknight_stand7);}
void hknight_stand7() {FRAME_STATE(HKnightFrames::FR_STAND7, hknight_stand8);}
void hknight_stand8() {FRAME_STATE(HKnightFrames::FR_STAND8, hknight_stand9);}
void hknight_stand9() {FRAME_STATE(HKnightFrames::FR_STAND9, hknight_stand1);}

//===========================================================================

void hknight_walk1() {
FRAME_STATE(HKnightFrames::FR_WALK1, hknight_walk2);
hk_idle_sound();
COOP_Walk(2);}
void hknight_walk2() {FRAME_STATE(HKnightFrames::FR_WALK2, hknight_walk3); COOP_Walk(5);}
void hknight_walk3() {FRAME_STATE(HKnightFrames::FR_WALK3, hknight_walk4); COOP_Walk(5);}
void hknight_walk4() {FRAME_STATE(HKnightFrames::FR_WALK4, hknight_walk5); COOP_Walk(4);}
void hknight_walk5() {FRAME_STATE(HKnightFrames::FR_WALK5, hknight_walk6); COOP_Walk(4);}
void hknight_walk6() {FRAME_STATE(HKnightFrames::FR_WALK6, hknight_walk7); COOP_Walk(2);}
void hknight_walk7() {FRAME_STATE(HKnightFrames::FR_WALK7, hknight_walk8); COOP_Walk(2);}
void hknight_walk8() {FRAME_STATE(HKnightFrames::FR_WALK8, hknight_walk9); COOP_Walk(3);}
void hknight_walk9() {FRAME_STATE(HKnightFrames::FR_WALK9, hknight_walk10); COOP_Walk(3);}
void hknight_walk10() {FRAME_STATE(HKnightFrames::FR_WALK10, hknight_walk11); COOP_Walk(4);}
void hknight_walk11() {FRAME_STATE(HKnightFrames::FR_WALK11, hknight_walk12); COOP_Walk(3);}
void hknight_walk12() {FRAME_STATE(HKnightFrames::FR_WALK12, hknight_walk13); COOP_Walk(4);}
void hknight_walk13() {FRAME_STATE(HKnightFrames::FR_WALK13, hknight_walk14); COOP_Walk(6);}
void hknight_walk14() {FRAME_STATE(HKnightFrames::FR_WALK14, hknight_walk15); COOP_Walk(2);}
void hknight_walk15() {FRAME_STATE(HKnightFrames::FR_WALK15, hknight_walk16); COOP_Walk(2);}
void hknight_walk16() {FRAME_STATE(HKnightFrames::FR_WALK16, hknight_walk17); COOP_Walk(4);}
void hknight_walk17() {FRAME_STATE(HKnightFrames::FR_WALK17, hknight_walk18); COOP_Walk(3);}
void hknight_walk18() {FRAME_STATE(HKnightFrames::FR_WALK18, hknight_walk19); COOP_Walk(3);}
void hknight_walk19() {FRAME_STATE(HKnightFrames::FR_WALK19, hknight_walk20); COOP_Walk(3);}
void hknight_walk20() {FRAME_STATE(HKnightFrames::FR_WALK20, hknight_walk1); COOP_Walk(2);}

//===========================================================================

void hknight_run1() {
FRAME_STATE(HKnightFrames::FR_RUN1, hknight_run2);
hk_idle_sound();
COOP_Run (20); CheckForCharge (); }
void hknight_run2() {FRAME_STATE(HKnightFrames::FR_RUN2, hknight_run3); COOP_Run(25);}
void hknight_run3() {FRAME_STATE(HKnightFrames::FR_RUN3, hknight_run4); COOP_Run(18);}
void hknight_run4() {FRAME_STATE(HKnightFrames::FR_RUN4, hknight_run5); COOP_Run(16);}
void hknight_run5() {FRAME_STATE(HKnightFrames::FR_RUN5, hknight_run6); COOP_Run(14);}
void hknight_run6() {FRAME_STATE(HKnightFrames::FR_RUN6, hknight_run7); COOP_Run(25);}
void hknight_run7() {FRAME_STATE(HKnightFrames::FR_RUN7, hknight_run8); COOP_Run(21);}
void hknight_run8() {FRAME_STATE(HKnightFrames::FR_RUN8, hknight_run1); COOP_Run(13);}

//============================================================================

void hknight_pain1() {FRAME_STATE(HKnightFrames::FR_PAIN1, hknight_pain2); sound (self, PR_CHAN_VOICE, "hknight/pain1.wav", 1, PR_ATTN_NORM);}
void hknight_pain2() {FRAME_STATE(HKnightFrames::FR_PAIN2, hknight_pain3);}
void hknight_pain3() {FRAME_STATE(HKnightFrames::FR_PAIN3, hknight_pain4);}
void hknight_pain4() {FRAME_STATE(HKnightFrames::FR_PAIN4, hknight_pain5);}
void hknight_pain5() {FRAME_STATE(HKnightFrames::FR_PAIN5, hknight_run1);}

//============================================================================

void hknight_die1() {FRAME_STATE(HKnightFrames::FR_DEATH1, hknight_die2); ai_forward(10);}
void hknight_die2() {FRAME_STATE(HKnightFrames::FR_DEATH2, hknight_die3); ai_forward(8);}
void hknight_die3()
{FRAME_STATE(HKnightFrames::FR_DEATH3, hknight_die4); self->solid = PR_SOLID_NOT; ai_forward(7);}
void hknight_die4() {FRAME_STATE(HKnightFrames::FR_DEATH4, hknight_die5);}
void hknight_die5() {FRAME_STATE(HKnightFrames::FR_DEATH5, hknight_die6);}
void hknight_die6() {FRAME_STATE(HKnightFrames::FR_DEATH6, hknight_die7);}
void hknight_die7() {FRAME_STATE(HKnightFrames::FR_DEATH7, hknight_die8);}
void hknight_die8() {FRAME_STATE(HKnightFrames::FR_DEATH8, hknight_die9); ai_forward(10);}
void hknight_die9() {FRAME_STATE(HKnightFrames::FR_DEATH9, hknight_die10); ai_forward(11);}
void hknight_die10() {FRAME_STATE(HKnightFrames::FR_DEATH10, hknight_die11);}
void hknight_die11() {FRAME_STATE(HKnightFrames::FR_DEATH11, hknight_die12);}
void hknight_die12() {FRAME_STATE(HKnightFrames::FR_DEATH12, hknight_die12); MonsterCorpse();}

void hknight_dieb1() {FRAME_STATE(HKnightFrames::FR_DEATHB1, hknight_dieb2);}
void hknight_dieb2() {FRAME_STATE(HKnightFrames::FR_DEATHB2, hknight_dieb3);}
void hknight_dieb3()
{FRAME_STATE(HKnightFrames::FR_DEATHB3, hknight_dieb4); self->solid = PR_SOLID_NOT;}
void hknight_dieb4() {FRAME_STATE(HKnightFrames::FR_DEATHB4, hknight_dieb5);}
void hknight_dieb5() {FRAME_STATE(HKnightFrames::FR_DEATHB5, hknight_dieb6);}
void hknight_dieb6() {FRAME_STATE(HKnightFrames::FR_DEATHB6, hknight_dieb7);}
void hknight_dieb7() {FRAME_STATE(HKnightFrames::FR_DEATHB7, hknight_dieb8);}
void hknight_dieb8() {FRAME_STATE(HKnightFrames::FR_DEATHB8, hknight_dieb9);}
void hknight_dieb9() {FRAME_STATE(HKnightFrames::FR_DEATHB9, hknight_dieb9); MonsterCorpse();}

void hknight_die()
{
// check for gib
	if (self->health < -40)
	{
		sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_NORM);
		ThrowMonsterHead ("progs/h_hellkn.mdl", self->health);
		ThrowGib("progs/gib1.mdl", -40, PR_TRUE, 0, PR_GIB1_KGS, PR_FALSE);
		ThrowGib("progs/gib2.mdl", -40, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
		ThrowGib("progs/gib3.mdl", -40, PR_TRUE, 0, PR_GIB3_KGS, PR_FALSE);
		remove (self);
		return;
	}

// regular death
	sound (self, PR_CHAN_VOICE, "hknight/death1.wav", 1, PR_ATTN_NORM);
	if (random() > 0.5)
		hknight_die1 ();
	else
		hknight_dieb1 ();
}


//============================================================================

void hknight_magica1() {FRAME_STATE(HKnightFrames::FR_MAGICA1, hknight_magica2); COOP_Turn();}
void hknight_magica2() {FRAME_STATE(HKnightFrames::FR_MAGICA2, hknight_magica3); COOP_Turn();}
void hknight_magica3() {FRAME_STATE(HKnightFrames::FR_MAGICA3, hknight_magica4); COOP_Turn();}
void hknight_magica4() {FRAME_STATE(HKnightFrames::FR_MAGICA4, hknight_magica5); COOP_Turn();}
void hknight_magica5() {FRAME_STATE(HKnightFrames::FR_MAGICA5, hknight_magica6); COOP_Turn();}
void hknight_magica6() {FRAME_STATE(HKnightFrames::FR_MAGICA6, hknight_magica7); COOP_Turn();}
void hknight_magica7() {FRAME_STATE(HKnightFrames::FR_MAGICA7, hknight_magica8); hknight_shot(-2);}
void hknight_magica8() {FRAME_STATE(HKnightFrames::FR_MAGICA8, hknight_magica9); hknight_shot(-1);}
void hknight_magica9() {FRAME_STATE(HKnightFrames::FR_MAGICA9, hknight_magica10); hknight_shot(0);}
void hknight_magica10() {FRAME_STATE(HKnightFrames::FR_MAGICA10, hknight_magica11); hknight_shot(1);}
void hknight_magica11() {FRAME_STATE(HKnightFrames::FR_MAGICA11, hknight_magica12); hknight_shot(2);}
void hknight_magica12() {FRAME_STATE(HKnightFrames::FR_MAGICA12, hknight_magica13); hknight_shot(3);}
void hknight_magica13() {FRAME_STATE(HKnightFrames::FR_MAGICA13, hknight_magica14); COOP_Turn();}
void hknight_magica14() {FRAME_STATE(HKnightFrames::FR_MAGICA14, hknight_run1); COOP_Turn();}

//============================================================================

void hknight_magicb1() {FRAME_STATE(HKnightFrames::FR_MAGICB1, hknight_magicb2); COOP_Turn();}
void hknight_magicb2() {FRAME_STATE(HKnightFrames::FR_MAGICB2, hknight_magicb3); COOP_Turn();}
void hknight_magicb3() {FRAME_STATE(HKnightFrames::FR_MAGICB3, hknight_magicb4); COOP_Turn();}
void hknight_magicb4() {FRAME_STATE(HKnightFrames::FR_MAGICB4, hknight_magicb5); COOP_Turn();}
void hknight_magicb5() {FRAME_STATE(HKnightFrames::FR_MAGICB5, hknight_magicb6); COOP_Turn();}
void hknight_magicb6() {FRAME_STATE(HKnightFrames::FR_MAGICB6, hknight_magicb7); COOP_Turn();}
void hknight_magicb7() {FRAME_STATE(HKnightFrames::FR_MAGICB7, hknight_magicb8); hknight_shot(-2);}
void hknight_magicb8() {FRAME_STATE(HKnightFrames::FR_MAGICB8, hknight_magicb9); hknight_shot(-1);}
void hknight_magicb9() {FRAME_STATE(HKnightFrames::FR_MAGICB9, hknight_magicb10); hknight_shot(0);}
void hknight_magicb10() {FRAME_STATE(HKnightFrames::FR_MAGICB10, hknight_magicb11); hknight_shot(1);}
void hknight_magicb11() {FRAME_STATE(HKnightFrames::FR_MAGICB11, hknight_magicb12); hknight_shot(2);}
void hknight_magicb12() {FRAME_STATE(HKnightFrames::FR_MAGICB12, hknight_magicb13); hknight_shot(3);}
void hknight_magicb13() {FRAME_STATE(HKnightFrames::FR_MAGICB13, hknight_run1); COOP_Turn();}

//============================================================================

void hknight_magicc1() {FRAME_STATE(HKnightFrames::FR_MAGICC1, hknight_magicc2); COOP_Turn();}
void hknight_magicc2() {FRAME_STATE(HKnightFrames::FR_MAGICC2, hknight_magicc3); COOP_Turn();}
void hknight_magicc3() {FRAME_STATE(HKnightFrames::FR_MAGICC3, hknight_magicc4); COOP_Turn();}
void hknight_magicc4() {FRAME_STATE(HKnightFrames::FR_MAGICC4, hknight_magicc5); COOP_Turn();}
void hknight_magicc5() {FRAME_STATE(HKnightFrames::FR_MAGICC5, hknight_magicc6); COOP_Turn();}
void hknight_magicc6() {FRAME_STATE(HKnightFrames::FR_MAGICC6, hknight_magicc7); hknight_shot(-2);}
void hknight_magicc7() {FRAME_STATE(HKnightFrames::FR_MAGICC7, hknight_magicc8); hknight_shot(-1);}
void hknight_magicc8() {FRAME_STATE(HKnightFrames::FR_MAGICC8, hknight_magicc9); hknight_shot(0);}
void hknight_magicc9() {FRAME_STATE(HKnightFrames::FR_MAGICC9, hknight_magicc10); hknight_shot(1);}
void hknight_magicc10() {FRAME_STATE(HKnightFrames::FR_MAGICC10, hknight_magicc11); hknight_shot(2);}
void hknight_magicc11() {FRAME_STATE(HKnightFrames::FR_MAGICC11, hknight_run1); hknight_shot(3);}

//===========================================================================

void hknight_char_a1() {FRAME_STATE(HKnightFrames::FR_CHAR_A1, hknight_char_a2); ai_charge(20);}
void hknight_char_a2() {FRAME_STATE(HKnightFrames::FR_CHAR_A2, hknight_char_a3); ai_charge(25);}
void hknight_char_a3() {FRAME_STATE(HKnightFrames::FR_CHAR_A3, hknight_char_a4); ai_charge(18);}
void hknight_char_a4() {FRAME_STATE(HKnightFrames::FR_CHAR_A4, hknight_char_a5); ai_charge(16);}
void hknight_char_a5() {FRAME_STATE(HKnightFrames::FR_CHAR_A5, hknight_char_a6); ai_charge(14);}
void hknight_char_a6() {FRAME_STATE(HKnightFrames::FR_CHAR_A6, hknight_char_a7); ai_charge(20); ai_melee();}
void hknight_char_a7() {FRAME_STATE(HKnightFrames::FR_CHAR_A7, hknight_char_a8); ai_charge(21); ai_melee();}
void hknight_char_a8() {FRAME_STATE(HKnightFrames::FR_CHAR_A8, hknight_char_a9); ai_charge(13); ai_melee();}
void hknight_char_a9() {FRAME_STATE(HKnightFrames::FR_CHAR_A9, hknight_char_a10); ai_charge(20); ai_melee();}
void hknight_char_a10() {FRAME_STATE(HKnightFrames::FR_CHAR_A10, hknight_char_a11); ai_charge(20); ai_melee();}
void hknight_char_a11() {FRAME_STATE(HKnightFrames::FR_CHAR_A11, hknight_char_a12); ai_charge(18); ai_melee();}
void hknight_char_a12() {FRAME_STATE(HKnightFrames::FR_CHAR_A12, hknight_char_a13); ai_charge(16);}
void hknight_char_a13() {FRAME_STATE(HKnightFrames::FR_CHAR_A13, hknight_char_a14); ai_charge(14);}
void hknight_char_a14() {FRAME_STATE(HKnightFrames::FR_CHAR_A14, hknight_char_a15); ai_charge(25);}
void hknight_char_a15() {FRAME_STATE(HKnightFrames::FR_CHAR_A15, hknight_char_a16); ai_charge(21);}
void hknight_char_a16() {FRAME_STATE(HKnightFrames::FR_CHAR_A16, hknight_run1); ai_charge(13);}

//===========================================================================

void hknight_char_b1()
{FRAME_STATE(HKnightFrames::FR_CHAR_B1, hknight_char_b2); CheckContinueCharge (); ai_charge(23); ai_melee();}
void hknight_char_b2() {FRAME_STATE(HKnightFrames::FR_CHAR_B2, hknight_char_b3); ai_charge(17); ai_melee();}
void hknight_char_b3() {FRAME_STATE(HKnightFrames::FR_CHAR_B3, hknight_char_b4); ai_charge(12); ai_melee();}
void hknight_char_b4() {FRAME_STATE(HKnightFrames::FR_CHAR_B4, hknight_char_b5); ai_charge(22); ai_melee();}
void hknight_char_b5() {FRAME_STATE(HKnightFrames::FR_CHAR_B5, hknight_char_b6); ai_charge(18); ai_melee();}
void hknight_char_b6() {FRAME_STATE(HKnightFrames::FR_CHAR_B6, hknight_char_b1); ai_charge(8); ai_melee();}

//===========================================================================

void hknight_slice1() {FRAME_STATE(HKnightFrames::FR_SLICE1, hknight_slice2); ai_charge(9);}
void hknight_slice2() {FRAME_STATE(HKnightFrames::FR_SLICE2, hknight_slice3); ai_charge(6);}
void hknight_slice3() {FRAME_STATE(HKnightFrames::FR_SLICE3, hknight_slice4); ai_charge(13);}
void hknight_slice4() {FRAME_STATE(HKnightFrames::FR_SLICE4, hknight_slice5); ai_charge(4);}
void hknight_slice5() {FRAME_STATE(HKnightFrames::FR_SLICE5, hknight_slice6); ai_charge(7); ai_melee();}
void hknight_slice6() {FRAME_STATE(HKnightFrames::FR_SLICE6, hknight_slice7); ai_charge(15); ai_melee();}
void hknight_slice7() {FRAME_STATE(HKnightFrames::FR_SLICE7, hknight_slice8); ai_charge(8); ai_melee();}
void hknight_slice8() {FRAME_STATE(HKnightFrames::FR_SLICE8, hknight_slice9); ai_charge(2); ai_melee();}
void hknight_slice9() {FRAME_STATE(HKnightFrames::FR_SLICE9, hknight_slice10); ai_melee();}
void hknight_slice10() {FRAME_STATE(HKnightFrames::FR_SLICE10, hknight_run1); ai_charge(3);}

//===========================================================================

void hknight_smash1() {FRAME_STATE(HKnightFrames::FR_SMASH1, hknight_smash2); ai_charge(1);}
void hknight_smash2() {FRAME_STATE(HKnightFrames::FR_SMASH2, hknight_smash3); ai_charge(13);}
void hknight_smash3() {FRAME_STATE(HKnightFrames::FR_SMASH3, hknight_smash4); ai_charge(9);}
void hknight_smash4() {FRAME_STATE(HKnightFrames::FR_SMASH4, hknight_smash5); ai_charge(11);}
void hknight_smash5() {FRAME_STATE(HKnightFrames::FR_SMASH5, hknight_smash6); ai_charge(10); ai_melee();}
void hknight_smash6() {FRAME_STATE(HKnightFrames::FR_SMASH6, hknight_smash7); ai_charge(7); ai_melee();}
void hknight_smash7() {FRAME_STATE(HKnightFrames::FR_SMASH7, hknight_smash8); ai_charge(12); ai_melee();}
void hknight_smash8() {FRAME_STATE(HKnightFrames::FR_SMASH8, hknight_smash9); ai_charge(2); ai_melee();}
void hknight_smash9() {FRAME_STATE(HKnightFrames::FR_SMASH9, hknight_smash10); ai_charge(3); ai_melee();}
void hknight_smash10() {FRAME_STATE(HKnightFrames::FR_SMASH10, hknight_smash11); ai_charge(0);}
void hknight_smash11() {FRAME_STATE(HKnightFrames::FR_SMASH11, hknight_run1); ai_charge(0);}

//============================================================================

void hknight_watk1() {FRAME_STATE(HKnightFrames::FR_W_ATTACK1, hknight_watk2); ai_charge(2);}
void hknight_watk2() {FRAME_STATE(HKnightFrames::FR_W_ATTACK2, hknight_watk3); ai_charge(0);}
void hknight_watk3() {FRAME_STATE(HKnightFrames::FR_W_ATTACK3, hknight_watk4); ai_charge(0);}
void hknight_watk4() {FRAME_STATE(HKnightFrames::FR_W_ATTACK4, hknight_watk5); ai_melee();}
void hknight_watk5() {FRAME_STATE(HKnightFrames::FR_W_ATTACK5, hknight_watk6); ai_melee();}
void hknight_watk6() {FRAME_STATE(HKnightFrames::FR_W_ATTACK6, hknight_watk7); ai_melee();}
void hknight_watk7() {FRAME_STATE(HKnightFrames::FR_W_ATTACK7, hknight_watk8); ai_charge(1);}
void hknight_watk8() {FRAME_STATE(HKnightFrames::FR_W_ATTACK8, hknight_watk9); ai_charge(4);}
void hknight_watk9() {FRAME_STATE(HKnightFrames::FR_W_ATTACK9, hknight_watk10); ai_charge(5);}
void hknight_watk10() {FRAME_STATE(HKnightFrames::FR_W_ATTACK10, hknight_watk11); ai_charge(3); ai_melee();}
void hknight_watk11() {FRAME_STATE(HKnightFrames::FR_W_ATTACK11, hknight_watk12); ai_charge(2); ai_melee();}
void hknight_watk12() {FRAME_STATE(HKnightFrames::FR_W_ATTACK12, hknight_watk13); ai_charge(2); ai_melee();}
void hknight_watk13() {FRAME_STATE(HKnightFrames::FR_W_ATTACK13, hknight_watk14); ai_charge(0);}
void hknight_watk14() {FRAME_STATE(HKnightFrames::FR_W_ATTACK14, hknight_watk15); ai_charge(0);}
void hknight_watk15() {FRAME_STATE(HKnightFrames::FR_W_ATTACK15, hknight_watk16); ai_charge(0);}
void hknight_watk16() {FRAME_STATE(HKnightFrames::FR_W_ATTACK16, hknight_watk17); ai_charge(1);}
void hknight_watk17() {FRAME_STATE(HKnightFrames::FR_W_ATTACK17, hknight_watk18); ai_charge(1); ai_melee();}
void hknight_watk18() {FRAME_STATE(HKnightFrames::FR_W_ATTACK18, hknight_watk19); ai_charge(3); ai_melee();}
void hknight_watk19() {FRAME_STATE(HKnightFrames::FR_W_ATTACK19, hknight_watk20); ai_charge(4); ai_melee();}
void hknight_watk20() {FRAME_STATE(HKnightFrames::FR_W_ATTACK20, hknight_watk21); ai_charge(6);}
void hknight_watk21() {FRAME_STATE(HKnightFrames::FR_W_ATTACK21, hknight_watk22); ai_charge(7);}
void hknight_watk22() {FRAME_STATE(HKnightFrames::FR_W_ATTACK22, hknight_run1); ai_charge(3);}

//============================================================================

void hk_idle_sound()
{
	if (random() < 0.2)
		sound (self, PR_CHAN_VOICE, "hknight/idle.wav", 1, PR_ATTN_NORM);
}

void hknight_pain(entity attacker, float damage)
{
	if (self->pain_finished > time)
		return;

	sound (self, PR_CHAN_VOICE, "hknight/pain1.wav", 1, PR_ATTN_NORM);

	if (time - self->pain_finished > 5)
	{	// allways go into pain frame if it has been a while
		hknight_pain1 ();
		self->pain_finished = time + 1;
		return;
	}

	if ((random()*30 > damage) )
		return;		// didn't flinch

	self->pain_finished = time + 1;
	hknight_pain1 ();
}

float	hknight_type;

void hknight_melee()
{
	hknight_type = hknight_type + 1;

	sound (self, PR_CHAN_WEAPON, "hknight/slash1.wav", 1, PR_ATTN_NORM);
	if (hknight_type == 1)
		hknight_slice1 ();
	else if (hknight_type == 2)
		hknight_smash1 ();
	else if (hknight_type == 3)
	{
		hknight_watk1 ();
		hknight_type = 0;
	}
}



float HKnightOnSight()
{
	sound (self, PR_CHAN_VOICE, "hknight/sight1.wav", 1, PR_ATTN_NORM);
	return PR_TRUE;
}


/*QUAKED monster_hell_knight (1 0 0) (-16 -16 -24) (16 16 40) Ambush
*/
void monster_hell_knight()
{
	if ( deathmatch ) {
		remove( self );
		return;
	}

	self->classname = "idmonster_hell_knight";

	if (!server_spawned) {
		precache_model2 ("progs/hknight.mdl");
		precache_model2 ("progs/k_spike.mdl");
		precache_model2 ("progs/h_hellkn.mdl");


		precache_sound2 ("hknight/attack1.wav");
		precache_sound2 ("hknight/death1.wav");
		precache_sound2 ("hknight/pain1.wav");
		precache_sound2 ("hknight/sight1.wav");
		precache_sound ("hknight/hit.wav");		// used by C code, so don't sound2
		precache_sound2 ("hknight/slash1.wav");
		precache_sound2 ("hknight/idle.wav");
		precache_sound2 ("hknight/grunt.wav");

		precache_sound ("knight/sword1.wav");
		precache_sound ("knight/sword2.wav");
	}

	self->solid = PR_SOLID_SLIDEBOX;
	self->movetype = PR_MOVETYPE_STEP;

	self->mdl = "progs/h_hellkn.mdl";
	setmodel (self, "progs/hknight.mdl");

	setsize (self, V({-16, -16, -24}), V({16, 16, 40}));
	self->max_health = self->health = PR_IDMON_HKNIGHT_HEALTH;
	self->armorvalue = PR_IDMON_HKNIGHT_ARMORVALUE;
	self->armortype = PR_IDMON_HKNIGHT_ARMORTYPE;

	self->th_stand = hknight_stand1;
	self->th_walk = hknight_walk1;
	self->th_run = hknight_run1;
	self->th_melee = hknight_melee;
	self->th_missile = hknight_magicc1;
	self->th_pain = hknight_pain;
	self->th_die = hknight_die;


	self->netname = "death knight";

	self->monsterweight = PR_MWEIGHT_NORMAL;
	self->monsterflags = PR_MFLAG_HUMANOID;

	// monsters have a team of their own
	if ( !self->team_no )
		self->team_no = PR_TEAMNO_MONSTERS;
	self->PR_monsterflag = PR_STRFLAG_MONSTER;

	self->is_connected = PR_TRUE;


	self->OnNoticeEnemy = HKnightOnSight;
	self->PR_npc_speedfactor = 1.3;
	self->PR_npc_spawnfunc = monster_hell_knight;
	COOP_SetLives ();

	self->PR_npc_bulletmult = 0.80;
	self->PR_npc_spikemult = 0.80;
	self->PR_npc_electricitymult = 3;
	self->PR_npc_meleemult = 0.5;

	coop_walkmonster_start ();
}

} // END namespace Progs

#endif
