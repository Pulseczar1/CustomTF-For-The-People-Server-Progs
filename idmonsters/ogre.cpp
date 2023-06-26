/*
	ogre.qc
*/

#include "progs.h"

#ifndef PR_COOP_MODE_ENHANCED
//#pragma DONT_COMPILE_THIS_FILE
#else

#include "idmonsters/ogre.h"
#include "idmonsters/common.h"
#include "idmonsters/coop_monsters.h"
#include "coop_defs.h"
#include "ofndefs.h"
#include "combat.h"
#include "qw.h"
#include "weapons.h"
#include "fight.h"
#include "ai.h"
#include "items.h"
#include "monsters.h"
#include "warlock.h"

namespace Progs {

#define PR_IDMON_OGRE_HEALTH       450
#define PR_IDMON_OGRE_CHAINSAWDMG  12
#define PR_IDMON_OGRE_GRENADEDMG   120

/*
==============================================================================

OGRE

==============================================================================
*/

//$cd id1/models/ogre_c
//$origin 0 0 24
//$base base
//$skin base

namespace OgreFrames
{
	enum {FR_STAND1, FR_STAND2, FR_STAND3, FR_STAND4, FR_STAND5, FR_STAND6, FR_STAND7, FR_STAND8, FR_STAND9};

	enum {FR_WALK1 = 9, FR_WALK2, FR_WALK3, FR_WALK4, FR_WALK5, FR_WALK6, FR_WALK7};
	enum {FR_WALK8 = 16, FR_WALK9, FR_WALK10, FR_WALK11, FR_WALK12, FR_WALK13, FR_WALK14, FR_WALK15, FR_WALK16};

	enum {FR_RUN1 = 25, FR_RUN2, FR_RUN3, FR_RUN4, FR_RUN5, FR_RUN6, FR_RUN7, FR_RUN8};

	enum {FR_SWING1 = 33, FR_SWING2, FR_SWING3, FR_SWING4, FR_SWING5, FR_SWING6, FR_SWING7};
	enum {FR_SWING8 = 40, FR_SWING9, FR_SWING10, FR_SWING11, FR_SWING12, FR_SWING13, FR_SWING14};

	enum {FR_SMASH1 = 47, FR_SMASH2, FR_SMASH3, FR_SMASH4, FR_SMASH5, FR_SMASH6, FR_SMASH7};
	enum {FR_SMASH8 = 54, FR_SMASH9, FR_SMASH10, FR_SMASH11, FR_SMASH12, FR_SMASH13, FR_SMASH14};

	enum {FR_SHOOT1 = 61, FR_SHOOT2, FR_SHOOT3, FR_SHOOT4, FR_SHOOT5, FR_SHOOT6};

	enum {FR_PAIN1 = 67, FR_PAIN2, FR_PAIN3, FR_PAIN4, FR_PAIN5};

	enum {FR_PAINB1 = 72, FR_PAINB2, FR_PAINB3};

	enum {FR_PAINC1 = 75, FR_PAINC2, FR_PAINC3, FR_PAINC4, FR_PAINC5, FR_PAINC6};

	enum {FR_PAIND1 = 81, FR_PAIND2, FR_PAIND3, FR_PAIND4, FR_PAIND5, FR_PAIND6, FR_PAIND7, FR_PAIND8, FR_PAIND9, FR_PAIND10};
	enum {FR_PAIND11 = 91, FR_PAIND12, FR_PAIND13, FR_PAIND14, FR_PAIND15, FR_PAIND16};

	enum {FR_PAINE1 = 97, FR_PAINE2, FR_PAINE3, FR_PAINE4, FR_PAINE5, FR_PAINE6, FR_PAINE7, FR_PAINE8, FR_PAINE9, FR_PAINE10};
	enum {FR_PAINE11 = 107, FR_PAINE12, FR_PAINE13, FR_PAINE14, FR_PAINE15};

	enum {FR_DEATH1 = 112, FR_DEATH2, FR_DEATH3, FR_DEATH4, FR_DEATH5, FR_DEATH6};
	enum {FR_DEATH7 = 118, FR_DEATH8, FR_DEATH9, FR_DEATH10, FR_DEATH11, FR_DEATH12};
	enum {FR_DEATH13 = 124, FR_DEATH14};

	enum {FR_BDEATH1 = 126, FR_BDEATH2, FR_BDEATH3, FR_BDEATH4, FR_BDEATH5, FR_BDEATH6};
	enum {FR_BDEATH7 = 132, FR_BDEATH8, FR_BDEATH9, FR_BDEATH10};

	enum {FR_PULL1 = 136, FR_PULL2, FR_PULL3, FR_PULL4, FR_PULL5, FR_PULL6, FR_PULL7, FR_PULL8, FR_PULL9, FR_PULL10, FR_PULL11};
}

//=============================================================================


void OgreGrenadeExplode()
{
	T_RadiusDamage (self, self->owner, PR_IDMON_OGRE_GRENADEDMG, world/*, "grenade"*/);
//	sound (self, #CHAN_VOICE, "weapons/r_exp3.wav", 1, #ATTN_NORM);

	WriteByte (PR_MSG_MULTICAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_MULTICAST, PR_TE_EXPLOSION);
	WriteCoord (PR_MSG_MULTICAST, self->origin[X]);
	WriteCoord (PR_MSG_MULTICAST, self->origin[Y]);
	WriteCoord (PR_MSG_MULTICAST, self->origin[Z]);
	multicast (self->origin, PR_MULTICAST_PHS);

	remove (self);
}

void OgreGrenadeTouch()
{
	if (other == self->owner)
		return;		// don't explode on owner
	if (other->takedamage == PR_DAMAGE_AIM)
	{
		OgreGrenadeExplode();
		return;
	}
	sound (self, PR_CHAN_VOICE, "weapons/bounce.wav", 1, PR_ATTN_NORM);	// bounce sound
	if (self->velocity == V({0, 0, 0}))
		self->avelocity = V({0, 0, 0});
}

/*
================
OgreFireGrenade
================
*/
void OgreFireGrenade()
{
	entity missile;
	float enlen;

	muzzleflash ();

	sound (self, PR_CHAN_WEAPON, "weapons/grenade.wav", 1, PR_ATTN_NORM);

	missile = spawn ();
	missile->owner = self;
	missile->movetype = PR_MOVETYPE_BOUNCE;
	missile->solid = PR_SOLID_BBOX;

// set missile speed

	enlen = vlen (self->enemy->origin - self->origin);
	if (enlen < 600)
		enlen = 600;
	else
		enlen = enlen * 1.55;

	makevectors (self->angles);

	missile->velocity = normalize((self->enemy->origin + self->PR_npc_enemyoffset) - self->origin);
	missile->velocity = missile->velocity * enlen;
	missile->velocity[Z] = missile->velocity[Z] + 200;

	missile->avelocity = V({300, 300, 300});

	missile->angles = vectoangles(missile->velocity);

	missile->touch = OgreGrenadeTouch;

// set missile duration
	missile->nextthink = time + 2.5;
	missile->think = OgreGrenadeExplode;

	setmodel (missile, "progs/grenade.mdl");
	setsize (missile, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (missile, self->origin);

	Attack_Finished( 1 );
}


//=============================================================================

/*
================
chainsaw

FIXME
================
*/
void chainsaw(float side)
{
vector	delta;
float 	ldmg;

	if (self->enemy == world)
		return;
	if (!CanDamage (self->enemy, self))
		return;

	ai_charge(10);

	delta = self->enemy->origin - self->origin;

	if (vlen(delta) > 100)
		return;

	ldmg = (random() + random() + random()) * PR_IDMON_OGRE_CHAINSAWDMG;
	T_Damage (self->enemy, self, self, ldmg);

	if (side)
	{
		makevectors (self->angles);
		if (side == 1)
			SpawnMeatSpray (self->origin + v_forward*16, crandom() * 100 * v_right);
		else
			SpawnMeatSpray (self->origin + v_forward*16, side * v_right);
	}
}


void ogre_stand1() {FRAME_STATE(OgreFrames::FR_STAND1, ogre_stand2); COOP_Idle();}
void ogre_stand2() {FRAME_STATE(OgreFrames::FR_STAND2, ogre_stand3);}
void ogre_stand3() {FRAME_STATE(OgreFrames::FR_STAND3, ogre_stand4);}
void ogre_stand4() {FRAME_STATE(OgreFrames::FR_STAND4, ogre_stand5); COOP_Idle();}
void ogre_stand5() {
FRAME_STATE(OgreFrames::FR_STAND5, ogre_stand6);
if (random() < 0.2)
	sound (self, PR_CHAN_VOICE, "ogre/ogidle.wav", 1, PR_ATTN_IDLE);
}
void ogre_stand6() {FRAME_STATE(OgreFrames::FR_STAND6, ogre_stand7);}
void ogre_stand7() {FRAME_STATE(OgreFrames::FR_STAND7, ogre_stand8); COOP_Idle();}
void ogre_stand8() {FRAME_STATE(OgreFrames::FR_STAND8, ogre_stand9);}
void ogre_stand9() {FRAME_STATE(OgreFrames::FR_STAND9, ogre_stand1);}

void ogre_walk1() {FRAME_STATE(OgreFrames::FR_WALK1, ogre_walk2); COOP_Walk(3);}
void ogre_walk2() {FRAME_STATE(OgreFrames::FR_WALK2, ogre_walk3); COOP_Walk(2);}
void ogre_walk3() {
FRAME_STATE(OgreFrames::FR_WALK3, ogre_walk4);
COOP_Walk(2);
if (random() < 0.2)
	sound (self, PR_CHAN_VOICE, "ogre/ogidle.wav", 1, PR_ATTN_IDLE);
}
void ogre_walk4() {FRAME_STATE(OgreFrames::FR_WALK4, ogre_walk5); COOP_Walk(2);}
void ogre_walk5() {FRAME_STATE(OgreFrames::FR_WALK5, ogre_walk6); COOP_Walk(2);}
void ogre_walk6() {
FRAME_STATE(OgreFrames::FR_WALK6, ogre_walk7);
COOP_Walk(5);
if (random() < 0.1)
	sound (self, PR_CHAN_VOICE, "ogre/ogdrag.wav", 1, PR_ATTN_IDLE);
}
void ogre_walk7() {FRAME_STATE(OgreFrames::FR_WALK7, ogre_walk8); COOP_Walk(3);}
void ogre_walk8() {FRAME_STATE(OgreFrames::FR_WALK8, ogre_walk9); COOP_Walk(2);}
void ogre_walk9() {FRAME_STATE(OgreFrames::FR_WALK9, ogre_walk10); COOP_Walk(3);}
void ogre_walk10() {FRAME_STATE(OgreFrames::FR_WALK10, ogre_walk11); COOP_Walk(1);}
void ogre_walk11() {FRAME_STATE(OgreFrames::FR_WALK11, ogre_walk12); COOP_Walk(2);}
void ogre_walk12() {FRAME_STATE(OgreFrames::FR_WALK12, ogre_walk13); COOP_Walk(3);}
void ogre_walk13() {FRAME_STATE(OgreFrames::FR_WALK13, ogre_walk14); COOP_Walk(3);}
void ogre_walk14() {FRAME_STATE(OgreFrames::FR_WALK14, ogre_walk15); COOP_Walk(3);}
void ogre_walk15() {FRAME_STATE(OgreFrames::FR_WALK15, ogre_walk16); COOP_Walk(3);}
void ogre_walk16() {FRAME_STATE(OgreFrames::FR_WALK16, ogre_walk1); COOP_Walk(4);}

void ogre_run1() {FRAME_STATE(OgreFrames::FR_RUN1, ogre_run2); COOP_Run(9);
if (random() < 0.2)
	sound (self, PR_CHAN_VOICE, "ogre/ogidle2.wav", 1, PR_ATTN_IDLE);
}
void ogre_run2() {FRAME_STATE(OgreFrames::FR_RUN2, ogre_run3); COOP_Run(12);}
void ogre_run3() {FRAME_STATE(OgreFrames::FR_RUN3, ogre_run4); COOP_Run(8);}
void ogre_run4() {FRAME_STATE(OgreFrames::FR_RUN4, ogre_run5); COOP_Run(22);}
void ogre_run5() {FRAME_STATE(OgreFrames::FR_RUN5, ogre_run6); COOP_Run(16);}
void ogre_run6() {FRAME_STATE(OgreFrames::FR_RUN6, ogre_run7); COOP_Run(4);}
void ogre_run7() {FRAME_STATE(OgreFrames::FR_RUN7, ogre_run8); COOP_Run(13);}
void ogre_run8() {FRAME_STATE(OgreFrames::FR_RUN8, ogre_run1); COOP_Run(24);}

void ogre_swing1() {FRAME_STATE(OgreFrames::FR_SWING1, ogre_swing2); ai_charge(11);
sound (self, PR_CHAN_WEAPON, "ogre/ogsawatk.wav", 1, PR_ATTN_NORM);
}
void ogre_swing2() {FRAME_STATE(OgreFrames::FR_SWING2, ogre_swing3); ai_charge(1);}
void ogre_swing3() {FRAME_STATE(OgreFrames::FR_SWING3, ogre_swing4); ai_charge(4);}
void ogre_swing4() {FRAME_STATE(OgreFrames::FR_SWING4, ogre_swing5); ai_charge(13);}
void ogre_swing5() {FRAME_STATE(OgreFrames::FR_SWING5, ogre_swing6); ai_charge(9); chainsaw(0);self->angles[Y] = self->angles[Y] + random()*25;}
void ogre_swing6() {FRAME_STATE(OgreFrames::FR_SWING6, ogre_swing7); chainsaw(200);self->angles[Y] = self->angles[Y] + random()* 25;}
void ogre_swing7() {FRAME_STATE(OgreFrames::FR_SWING7, ogre_swing8); chainsaw(0);self->angles[Y] = self->angles[Y] + random()* 25;}
void ogre_swing8() {FRAME_STATE(OgreFrames::FR_SWING8, ogre_swing9); chainsaw(0);self->angles[Y] = self->angles[Y] + random()* 25;}
void ogre_swing9() {FRAME_STATE(OgreFrames::FR_SWING9, ogre_swing10); chainsaw(0);self->angles[Y] = self->angles[Y] + random()* 25;}
void ogre_swing10() {FRAME_STATE(OgreFrames::FR_SWING10, ogre_swing11); chainsaw(-200);self->angles[Y] = self->angles[Y] + random()* 25;}
void ogre_swing11() {FRAME_STATE(OgreFrames::FR_SWING11, ogre_swing12); chainsaw(0);self->angles[Y] = self->angles[Y] + random()* 25;}
void ogre_swing12() {FRAME_STATE(OgreFrames::FR_SWING12, ogre_swing13); ai_charge(3);}
void ogre_swing13() {FRAME_STATE(OgreFrames::FR_SWING13, ogre_swing14); ai_charge(8);}
void ogre_swing14() {FRAME_STATE(OgreFrames::FR_SWING14, ogre_run1); ai_charge(9);}

void ogre_smash1() {FRAME_STATE(OgreFrames::FR_SMASH1, ogre_smash2); ai_charge(6);
sound (self, PR_CHAN_WEAPON, "ogre/ogsawatk.wav", 1, PR_ATTN_NORM);
}
void ogre_smash2() {FRAME_STATE(OgreFrames::FR_SMASH2, ogre_smash3); ai_charge(0);}
void ogre_smash3() {FRAME_STATE(OgreFrames::FR_SMASH3, ogre_smash4); ai_charge(0);}
void ogre_smash4() {FRAME_STATE(OgreFrames::FR_SMASH4, ogre_smash5); ai_charge(1);}
void ogre_smash5() {FRAME_STATE(OgreFrames::FR_SMASH5, ogre_smash6); ai_charge(4);}
void ogre_smash6() {FRAME_STATE(OgreFrames::FR_SMASH6, ogre_smash7); ai_charge(4); chainsaw(0);}
void ogre_smash7() {FRAME_STATE(OgreFrames::FR_SMASH7, ogre_smash8); ai_charge(4); chainsaw(0);}
void ogre_smash8() {FRAME_STATE(OgreFrames::FR_SMASH8, ogre_smash9); ai_charge(10); chainsaw(0);}
void ogre_smash9() {FRAME_STATE(OgreFrames::FR_SMASH9, ogre_smash10); ai_charge(13); chainsaw(0);}
void ogre_smash10() {FRAME_STATE(OgreFrames::FR_SMASH10, ogre_smash11); chainsaw(1);}
void ogre_smash11() {FRAME_STATE(OgreFrames::FR_SMASH11, ogre_smash12); ai_charge(2); chainsaw(0);
self->nextthink = self->nextthink + random()*0.2;}	// slight variation
void ogre_smash12() {FRAME_STATE(OgreFrames::FR_SMASH12, ogre_smash13); ai_charge(4);}
void ogre_smash13() {FRAME_STATE(OgreFrames::FR_SMASH13, ogre_smash14); ai_charge(4);}
void ogre_smash14() {FRAME_STATE(OgreFrames::FR_SMASH14, ogre_run1); ai_charge(12);}

void ogre_nail1() {FRAME_STATE(OgreFrames::FR_SHOOT1, ogre_nail2); COOP_Turn();}
void ogre_nail2() {FRAME_STATE(OgreFrames::FR_SHOOT2, ogre_nail3); COOP_Turn();}
void ogre_nail3() {FRAME_STATE(OgreFrames::FR_SHOOT2, ogre_nail4); COOP_Turn();}
void ogre_nail4() {FRAME_STATE(OgreFrames::FR_SHOOT3, ogre_nail5); COOP_Turn();OgreFireGrenade();}
void ogre_nail5() {FRAME_STATE(OgreFrames::FR_SHOOT4, ogre_nail6); COOP_Turn();}
void ogre_nail6() {FRAME_STATE(OgreFrames::FR_SHOOT5, ogre_nail7); COOP_Turn();}
void ogre_nail7() {FRAME_STATE(OgreFrames::FR_SHOOT6, ogre_run1); COOP_Turn();}

void ogre_pain1() {FRAME_STATE(OgreFrames::FR_PAIN1, ogre_pain2);}
void ogre_pain2() {FRAME_STATE(OgreFrames::FR_PAIN2, ogre_pain3);}
void ogre_pain3() {FRAME_STATE(OgreFrames::FR_PAIN3, ogre_pain4);}
void ogre_pain4() {FRAME_STATE(OgreFrames::FR_PAIN4, ogre_pain5);}
void ogre_pain5() {FRAME_STATE(OgreFrames::FR_PAIN5, ogre_run1);}


void ogre_painb1() {FRAME_STATE(OgreFrames::FR_PAINB1, ogre_painb2);}
void ogre_painb2() {FRAME_STATE(OgreFrames::FR_PAINB2, ogre_painb3);}
void ogre_painb3() {FRAME_STATE(OgreFrames::FR_PAINB3, ogre_run1);}


void ogre_painc1() {FRAME_STATE(OgreFrames::FR_PAINC1, ogre_painc2);}
void ogre_painc2() {FRAME_STATE(OgreFrames::FR_PAINC2, ogre_painc3);}
void ogre_painc3() {FRAME_STATE(OgreFrames::FR_PAINC3, ogre_painc4);}
void ogre_painc4() {FRAME_STATE(OgreFrames::FR_PAINC4, ogre_painc5);}
void ogre_painc5() {FRAME_STATE(OgreFrames::FR_PAINC5, ogre_painc6);}
void ogre_painc6() {FRAME_STATE(OgreFrames::FR_PAINC6, ogre_run1);}


void ogre_paind1() {FRAME_STATE(OgreFrames::FR_PAIND1, ogre_paind2);}
void ogre_paind2() {FRAME_STATE(OgreFrames::FR_PAIND2, ogre_paind3); ai_pain(10);}
void ogre_paind3() {FRAME_STATE(OgreFrames::FR_PAIND3, ogre_paind4); ai_pain(9);}
void ogre_paind4() {FRAME_STATE(OgreFrames::FR_PAIND4, ogre_paind5); ai_pain(4);}
void ogre_paind5() {FRAME_STATE(OgreFrames::FR_PAIND5, ogre_paind6);}
void ogre_paind6() {FRAME_STATE(OgreFrames::FR_PAIND6, ogre_paind7);}
void ogre_paind7() {FRAME_STATE(OgreFrames::FR_PAIND7, ogre_paind8);}
void ogre_paind8() {FRAME_STATE(OgreFrames::FR_PAIND8, ogre_paind9);}
void ogre_paind9() {FRAME_STATE(OgreFrames::FR_PAIND9, ogre_paind10);}
void ogre_paind10() {FRAME_STATE(OgreFrames::FR_PAIND10, ogre_paind11);}
void ogre_paind11() {FRAME_STATE(OgreFrames::FR_PAIND11, ogre_paind12);}
void ogre_paind12() {FRAME_STATE(OgreFrames::FR_PAIND12, ogre_paind13);}
void ogre_paind13() {FRAME_STATE(OgreFrames::FR_PAIND13, ogre_paind14);}
void ogre_paind14() {FRAME_STATE(OgreFrames::FR_PAIND14, ogre_paind15);}
void ogre_paind15() {FRAME_STATE(OgreFrames::FR_PAIND15, ogre_paind16);}
void ogre_paind16() {FRAME_STATE(OgreFrames::FR_PAIND16, ogre_run1);}

void ogre_paine1() {FRAME_STATE(OgreFrames::FR_PAINE1, ogre_paine2);}
void ogre_paine2() {FRAME_STATE(OgreFrames::FR_PAINE2, ogre_paine3); ai_pain(10);}
void ogre_paine3() {FRAME_STATE(OgreFrames::FR_PAINE3, ogre_paine4); ai_pain(9);}
void ogre_paine4() {FRAME_STATE(OgreFrames::FR_PAINE4, ogre_paine5); ai_pain(4);}
void ogre_paine5() {FRAME_STATE(OgreFrames::FR_PAINE5, ogre_paine6);}
void ogre_paine6() {FRAME_STATE(OgreFrames::FR_PAINE6, ogre_paine7);}
void ogre_paine7() {FRAME_STATE(OgreFrames::FR_PAINE7, ogre_paine8);}
void ogre_paine8() {FRAME_STATE(OgreFrames::FR_PAINE8, ogre_paine9);}
void ogre_paine9() {FRAME_STATE(OgreFrames::FR_PAINE9, ogre_paine10);}
void ogre_paine10() {FRAME_STATE(OgreFrames::FR_PAINE10, ogre_paine11);}
void ogre_paine11() {FRAME_STATE(OgreFrames::FR_PAINE11, ogre_paine12);}
void ogre_paine12() {FRAME_STATE(OgreFrames::FR_PAINE12, ogre_paine13);}
void ogre_paine13() {FRAME_STATE(OgreFrames::FR_PAINE13, ogre_paine14);}
void ogre_paine14() {FRAME_STATE(OgreFrames::FR_PAINE14, ogre_paine15);}
void ogre_paine15() {FRAME_STATE(OgreFrames::FR_PAINE15, ogre_run1);}


void ogre_pain(entity attacker, float damage)
{
	float	r;

// don't make multiple pain sounds right after each other
	if (self->pain_finished > time)
		return;

	sound (self, PR_CHAN_VOICE, "ogre/ogpain1.wav", 1, PR_ATTN_NORM);

	r = random();

	if (r < 0.25)
	{
		ogre_pain1 ();
		self->pain_finished = time + 1;
	}
	else if (r < 0.5)
	{
		ogre_painb1 ();
		self->pain_finished = time + 1;
	}
	else if (r < 0.75)
	{
		ogre_painc1 ();
		self->pain_finished = time + 1;
	}
	else if (r < 0.88)
	{
		ogre_paind1 ();
		self->pain_finished = time + 2;
	}
	else
	{
		ogre_paine1 ();
		self->pain_finished = time + 2;
	}
}

void ogre_die1() {FRAME_STATE(OgreFrames::FR_DEATH1, ogre_die2);}
void ogre_die2() {FRAME_STATE(OgreFrames::FR_DEATH2, ogre_die3);}
void ogre_die3()
{FRAME_STATE(OgreFrames::FR_DEATH3, ogre_die4); self->solid = PR_SOLID_NOT;
self->ammo_rockets = 8;DropBackpack();}
void ogre_die4() {FRAME_STATE(OgreFrames::FR_DEATH4, ogre_die5);}
void ogre_die5() {FRAME_STATE(OgreFrames::FR_DEATH5, ogre_die6);}
void ogre_die6() {FRAME_STATE(OgreFrames::FR_DEATH6, ogre_die7);}
void ogre_die7() {FRAME_STATE(OgreFrames::FR_DEATH7, ogre_die8);}
void ogre_die8() {FRAME_STATE(OgreFrames::FR_DEATH8, ogre_die9);}
void ogre_die9() {FRAME_STATE(OgreFrames::FR_DEATH9, ogre_die10);}
void ogre_die10() {FRAME_STATE(OgreFrames::FR_DEATH10, ogre_die11);}
void ogre_die11() {FRAME_STATE(OgreFrames::FR_DEATH11, ogre_die12);}
void ogre_die12() {FRAME_STATE(OgreFrames::FR_DEATH12, ogre_die13);}
void ogre_die13() {FRAME_STATE(OgreFrames::FR_DEATH13, ogre_die14);}
void ogre_die14() {FRAME_STATE(OgreFrames::FR_DEATH14, ogre_die14); MonsterCorpse();}

void ogre_bdie1() {FRAME_STATE(OgreFrames::FR_BDEATH1, ogre_bdie2);}
void ogre_bdie2() {FRAME_STATE(OgreFrames::FR_BDEATH2, ogre_bdie3); ai_forward(5);}
void ogre_bdie3()
{FRAME_STATE(OgreFrames::FR_BDEATH3, ogre_bdie4); self->solid = PR_SOLID_NOT;
self->ammo_rockets = 8;DropBackpack();}
void ogre_bdie4() {FRAME_STATE(OgreFrames::FR_BDEATH4, ogre_bdie5); ai_forward(1);}
void ogre_bdie5() {FRAME_STATE(OgreFrames::FR_BDEATH5, ogre_bdie6); ai_forward(3);}
void ogre_bdie6() {FRAME_STATE(OgreFrames::FR_BDEATH6, ogre_bdie7); ai_forward(7);}
void ogre_bdie7() {FRAME_STATE(OgreFrames::FR_BDEATH7, ogre_bdie8); ai_forward(25);}
void ogre_bdie8() {FRAME_STATE(OgreFrames::FR_BDEATH8, ogre_bdie9);}
void ogre_bdie9() {FRAME_STATE(OgreFrames::FR_BDEATH9, ogre_bdie10);}
void ogre_bdie10() {FRAME_STATE(OgreFrames::FR_BDEATH10, ogre_bdie10); MonsterCorpse();}

void ogre_die()
{
// check for gib
	if (self->health < -80)
	{
		sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_NORM);
		ThrowMonsterHead ("progs/h_ogre.mdl", self->health);
		ThrowGib("progs/gib3.mdl", -40, PR_TRUE, 0, PR_GIB1_KGS, PR_FALSE);
		ThrowGib("progs/gib3.mdl", -40, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
		ThrowGib("progs/gib3.mdl", -40, PR_TRUE, 0, PR_GIB3_KGS, PR_FALSE);
		remove (self);
		return;
	}

	sound (self, PR_CHAN_VOICE, "ogre/ogdth.wav", 1, PR_ATTN_NORM);

	if (random() < 0.5)
		ogre_die1 ();
	else
		ogre_bdie1 ();
}

void ogre_melee()
{
	if (random() > 0.5)
		ogre_smash1 ();
	else
		ogre_swing1 ();
}



float OgreOnSight()
{
	sound (self, PR_CHAN_VOICE, "ogre/ogwake.wav", 1, PR_ATTN_NORM);
	return PR_TRUE;
}


/*QUAKED monster_ogre (1 0 0) (-32 -32 -24) (32 32 64) Ambush

*/
void monster_ogre()
{
	if ( deathmatch ) {
		remove( self );
		return;
	}

	self->classname = "idmonster_ogre";

	if (!server_spawned) {
		precache_model ("progs/ogre.mdl");
		precache_model ("progs/h_ogre.mdl");
		precache_model ("progs/grenade.mdl");

		precache_sound ("ogre/ogdrag.wav");
		precache_sound ("ogre/ogdth.wav");
		precache_sound ("ogre/ogidle.wav");
		precache_sound ("ogre/ogidle2.wav");
		precache_sound ("ogre/ogpain1.wav");
		precache_sound ("ogre/ogsawatk.wav");
		precache_sound ("ogre/ogwake.wav");
	}

	self->solid = PR_SOLID_SLIDEBOX;
	self->movetype = PR_MOVETYPE_STEP;

	self->mdl = "progs/h_ogre.mdl";
	setmodel (self, "progs/ogre.mdl");

	setsize (self, PR_VEC_HULL2_MIN, PR_VEC_HULL2_MAX);
	self->max_health = self->health = PR_IDMON_OGRE_HEALTH;

	self->th_stand = ogre_stand1;
	self->th_walk = ogre_walk1;
	self->th_run = ogre_run1;
	self->th_die = ogre_die;
	self->th_melee = ogre_melee;
	self->th_missile = ogre_nail1;
	self->th_pain = ogre_pain;

	// for mirvs
	//self.no_grenades_1 = 2;


	self->netname = "ogre";

	self->monsterweight = PR_MWEIGHT_MEDIUM;
	self->monsterflags = PR_MFLAG_HASWEAPON | PR_MFLAG_ALMOSTHUMANOID;

	// monsters have a team of their own
	if ( !self->team_no )
		self->team_no = PR_TEAMNO_MONSTERS;
	self->PR_monsterflag = PR_STRFLAG_MONSTER;

	self->is_connected = PR_TRUE;

	self->OnNoticeEnemy = OgreOnSight;
	self->PR_npc_spawnfunc = monster_ogre;
	COOP_SetLives ();

	self->PR_npc_spikemult = 1.25;
	self->PR_npc_explosionmult = 0.75;
	self->PR_npc_electricitymult = 1.50;

	// set ammo for emp
	self->ammo_rockets = 500;

	coop_walkmonster_start();
}

void monster_ogre_marksman()
{
	monster_ogre ();
}

} // END namespace Progs

#endif
