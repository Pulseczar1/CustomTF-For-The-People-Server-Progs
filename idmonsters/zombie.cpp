/*
	zombie.qc
*/

#include "progs.h"

#ifndef PR_COOP_MODE_ENHANCED
//#pragma DONT_COMPILE_THIS_FILE
#else

#include "idmonsters/zombie.h"
#include "idmonsters/common.h"
#include "idmonsters/coop_monsters.h"
#include "coop_defs.h"
#include "gweapons.h"
#include "ofndefs.h"
#include "player.h"
#include "ai.h"
#include "warlock.h"

namespace Progs {

#define PR_IDMON_ZOMBIE_HEALTH		145
#define PR_IDMON_ZOMBIE_DMGIGNORE	60
#define PR_IDMON_ZOMBIE_DMGFALL		100
#define PR_IDMON_ZOMBIE_ATTACKDMG	40

/*
==============================================================================

ZOMBIE

==============================================================================
*/
//$cd id1/models/zombie
//$origin	0 0 24
//$base base
//$skin skin

namespace ZombieFrames
{
	enum {FR_STAND1, FR_STAND2, FR_STAND3, FR_STAND4, FR_STAND5, FR_STAND6, FR_STAND7, FR_STAND8};
	enum {FR_STAND9 = 8, FR_STAND10, FR_STAND11, FR_STAND12, FR_STAND13, FR_STAND14, FR_STAND15};

	enum {FR_WALK1 = 15, FR_WALK2, FR_WALK3, FR_WALK4, FR_WALK5, FR_WALK6, FR_WALK7, FR_WALK8, FR_WALK9, FR_WALK10, FR_WALK11};
	enum {FR_WALK12 = 26, FR_WALK13, FR_WALK14, FR_WALK15, FR_WALK16, FR_WALK17, FR_WALK18, FR_WALK19};

	enum {FR_RUN1 = 34, FR_RUN2, FR_RUN3, FR_RUN4, FR_RUN5, FR_RUN6, FR_RUN7, FR_RUN8, FR_RUN9, FR_RUN10, FR_RUN11, FR_RUN12};
	enum {FR_RUN13 = 46, FR_RUN14, FR_RUN15, FR_RUN16, FR_RUN17, FR_RUN18};

	enum {FR_ATTA1 = 52, FR_ATTA2, FR_ATTA3, FR_ATTA4, FR_ATTA5, FR_ATTA6, FR_ATTA7, FR_ATTA8, FR_ATTA9, FR_ATTA10, FR_ATTA11};
	enum {FR_ATTA12 = 63, FR_ATTA13};

	enum {FR_ATTB1 = 65, FR_ATTB2, FR_ATTB3, FR_ATTB4, FR_ATTB5, FR_ATTB6, FR_ATTB7, FR_ATTB8, FR_ATTB9, FR_ATTB10, FR_ATTB11};
	enum {FR_ATTB12 = 76, FR_ATTB13, FR_ATTB14};

	enum {FR_ATTC1 = 79, FR_ATTC2, FR_ATTC3, FR_ATTC4, FR_ATTC5, FR_ATTC6, FR_ATTC7, FR_ATTC8, FR_ATTC9, FR_ATTC10, FR_ATTC11};
	enum {FR_ATTC12 = 90};

	enum {FR_PAINA1 = 91, FR_PAINA2, FR_PAINA3, FR_PAINA4, FR_PAINA5, FR_PAINA6, FR_PAINA7, FR_PAINA8, FR_PAINA9, FR_PAINA10};
	enum {FR_PAINA11 = 101, FR_PAINA12};

	enum {FR_PAINB1 = 103, FR_PAINB2, FR_PAINB3, FR_PAINB4, FR_PAINB5, FR_PAINB6, FR_PAINB7, FR_PAINB8, FR_PAINB9, FR_PAINB10};
	enum {FR_PAINB11 = 113, FR_PAINB12, FR_PAINB13, FR_PAINB14, FR_PAINB15, FR_PAINB16, FR_PAINB17, FR_PAINB18, FR_PAINB19};
	enum {FR_PAINB20 = 122, FR_PAINB21, FR_PAINB22, FR_PAINB23, FR_PAINB24, FR_PAINB25, FR_PAINB26, FR_PAINB27, FR_PAINB28};

	enum {FR_PAINC1 = 131, FR_PAINC2, FR_PAINC3, FR_PAINC4, FR_PAINC5, FR_PAINC6, FR_PAINC7, FR_PAINC8, FR_PAINC9, FR_PAINC10};
	enum {FR_PAINC11 = 141, FR_PAINC12, FR_PAINC13, FR_PAINC14, FR_PAINC15, FR_PAINC16, FR_PAINC17, FR_PAINC18};

	enum {FR_PAIND1 = 149, FR_PAIND2, FR_PAIND3, FR_PAIND4, FR_PAIND5, FR_PAIND6, FR_PAIND7, FR_PAIND8, FR_PAIND9, FR_PAIND10};
	enum {FR_PAIND11 = 159, FR_PAIND12, FR_PAIND13};

	enum {FR_PAINE1 = 162, FR_PAINE2, FR_PAINE3, FR_PAINE4, FR_PAINE5, FR_PAINE6, FR_PAINE7, FR_PAINE8, FR_PAINE9, FR_PAINE10};
	enum {FR_PAINE11 = 172, FR_PAINE12, FR_PAINE13, FR_PAINE14, FR_PAINE15, FR_PAINE16, FR_PAINE17, FR_PAINE18, FR_PAINE19};
	enum {FR_PAINE20 = 181, FR_PAINE21, FR_PAINE22, FR_PAINE23, FR_PAINE24, FR_PAINE25, FR_PAINE26, FR_PAINE27, FR_PAINE28};
	enum {FR_PAINE29 = 190, FR_PAINE30};

	enum {FR_CRUC_1 = 192, FR_CRUC_2, FR_CRUC_3, FR_CRUC_4, FR_CRUC_5, FR_CRUC_6};
}

#define PR_SPAWN_CRUCIFIED			1

//=============================================================================

FIELD(".float inpain;")

void zombie_stand1() {FRAME_STATE(ZombieFrames::FR_STAND1, zombie_stand2); COOP_Idle();}
void zombie_stand2() {FRAME_STATE(ZombieFrames::FR_STAND2, zombie_stand3);}
void zombie_stand3() {FRAME_STATE(ZombieFrames::FR_STAND3, zombie_stand4);}
void zombie_stand4() {FRAME_STATE(ZombieFrames::FR_STAND4, zombie_stand5);}
void zombie_stand5() {FRAME_STATE(ZombieFrames::FR_STAND5, zombie_stand6); COOP_Idle();}
void zombie_stand6() {FRAME_STATE(ZombieFrames::FR_STAND6, zombie_stand7);}
void zombie_stand7() {FRAME_STATE(ZombieFrames::FR_STAND7, zombie_stand8);}
void zombie_stand8() {FRAME_STATE(ZombieFrames::FR_STAND8, zombie_stand9);}
void zombie_stand9() {FRAME_STATE(ZombieFrames::FR_STAND9, zombie_stand10);}
void zombie_stand10() {FRAME_STATE(ZombieFrames::FR_STAND10, zombie_stand11);}
void zombie_stand11() {FRAME_STATE(ZombieFrames::FR_STAND11, zombie_stand12);}
void zombie_stand12() {FRAME_STATE(ZombieFrames::FR_STAND12, zombie_stand13); COOP_Idle();}
void zombie_stand13() {FRAME_STATE(ZombieFrames::FR_STAND13, zombie_stand14);}
void zombie_stand14() {FRAME_STATE(ZombieFrames::FR_STAND14, zombie_stand15);}
void zombie_stand15() {FRAME_STATE(ZombieFrames::FR_STAND15, zombie_stand1);}

void zombie_cruc1()	 {
FRAME_STATE(ZombieFrames::FR_CRUC_1, zombie_cruc2);
if (random() < 0.1)
	sound (self, PR_CHAN_VOICE, "zombie/idle_w2.wav", 1, PR_ATTN_STATIC);}
void zombie_cruc2()	 {FRAME_STATE(ZombieFrames::FR_CRUC_2, zombie_cruc3); self->nextthink = time + 0.1 + random()*0.1;}
void zombie_cruc3()	 {FRAME_STATE(ZombieFrames::FR_CRUC_3, zombie_cruc4); self->nextthink = time + 0.1 + random()*0.1;}
void zombie_cruc4()	 {FRAME_STATE(ZombieFrames::FR_CRUC_4, zombie_cruc5); self->nextthink = time + 0.1 + random()*0.1;}
void zombie_cruc5()	 {FRAME_STATE(ZombieFrames::FR_CRUC_5, zombie_cruc6); self->nextthink = time + 0.1 + random()*0.1;}
void zombie_cruc6()	 {FRAME_STATE(ZombieFrames::FR_CRUC_6, zombie_cruc1); self->nextthink = time + 0.1 + random()*0.1;}

void zombie_walk1() {FRAME_STATE(ZombieFrames::FR_WALK1, zombie_walk2); COOP_Walk(0);}
void zombie_walk2() {FRAME_STATE(ZombieFrames::FR_WALK2, zombie_walk3); COOP_Walk(2);}
void zombie_walk3() {FRAME_STATE(ZombieFrames::FR_WALK3, zombie_walk4); COOP_Walk(3);}
void zombie_walk4() {FRAME_STATE(ZombieFrames::FR_WALK4, zombie_walk5); COOP_Walk(2);}
void zombie_walk5() {FRAME_STATE(ZombieFrames::FR_WALK5, zombie_walk6); COOP_Walk(1);}
void zombie_walk6() {FRAME_STATE(ZombieFrames::FR_WALK6, zombie_walk7); COOP_Walk(0);}
void zombie_walk7() {FRAME_STATE(ZombieFrames::FR_WALK7, zombie_walk8); COOP_Walk(0);}
void zombie_walk8() {FRAME_STATE(ZombieFrames::FR_WALK8, zombie_walk9); COOP_Walk(0);}
void zombie_walk9() {FRAME_STATE(ZombieFrames::FR_WALK9, zombie_walk10); COOP_Walk(0);}
void zombie_walk10() {FRAME_STATE(ZombieFrames::FR_WALK10, zombie_walk11); COOP_Walk(0);}
void zombie_walk11() {FRAME_STATE(ZombieFrames::FR_WALK11, zombie_walk12); COOP_Walk(2);}
void zombie_walk12() {FRAME_STATE(ZombieFrames::FR_WALK12, zombie_walk13); COOP_Walk(2);}
void zombie_walk13() {FRAME_STATE(ZombieFrames::FR_WALK13, zombie_walk14); COOP_Walk(1);}
void zombie_walk14() {FRAME_STATE(ZombieFrames::FR_WALK14, zombie_walk15); COOP_Walk(0);}
void zombie_walk15() {FRAME_STATE(ZombieFrames::FR_WALK15, zombie_walk16); COOP_Walk(0);}
void zombie_walk16() {FRAME_STATE(ZombieFrames::FR_WALK16, zombie_walk17); COOP_Walk(0);}
void zombie_walk17() {FRAME_STATE(ZombieFrames::FR_WALK17, zombie_walk18); COOP_Walk(0);}
void zombie_walk18() {FRAME_STATE(ZombieFrames::FR_WALK18, zombie_walk19); COOP_Walk(0);}
void zombie_walk19() {
FRAME_STATE(ZombieFrames::FR_WALK19, zombie_walk1);
COOP_Walk(0);
if (random() < 0.2)
	sound (self, PR_CHAN_VOICE, "zombie/z_idle.wav", 1, PR_ATTN_IDLE);}

void zombie_run1() {FRAME_STATE(ZombieFrames::FR_RUN1, zombie_run2); COOP_Run(1);self->inpain = 0;}
void zombie_run2() {FRAME_STATE(ZombieFrames::FR_RUN2, zombie_run3); COOP_Run(1);}
void zombie_run3() {FRAME_STATE(ZombieFrames::FR_RUN3, zombie_run4); COOP_Run(0);}
void zombie_run4() {FRAME_STATE(ZombieFrames::FR_RUN4, zombie_run5); COOP_Run(1);}
void zombie_run5() {FRAME_STATE(ZombieFrames::FR_RUN5, zombie_run6); COOP_Run(2);}
void zombie_run6() {FRAME_STATE(ZombieFrames::FR_RUN6, zombie_run7); COOP_Run(3);}
void zombie_run7() {FRAME_STATE(ZombieFrames::FR_RUN7, zombie_run8); COOP_Run(4);}
void zombie_run8() {FRAME_STATE(ZombieFrames::FR_RUN8, zombie_run9); COOP_Run(4);}
void zombie_run9() {FRAME_STATE(ZombieFrames::FR_RUN9, zombie_run10); COOP_Run(2);}
void zombie_run10() {FRAME_STATE(ZombieFrames::FR_RUN10, zombie_run11); COOP_Run(0);}
void zombie_run11() {FRAME_STATE(ZombieFrames::FR_RUN11, zombie_run12); COOP_Run(0);}
void zombie_run12() {FRAME_STATE(ZombieFrames::FR_RUN12, zombie_run13); COOP_Run(0);}
void zombie_run13() {FRAME_STATE(ZombieFrames::FR_RUN13, zombie_run14); COOP_Run(2);}
void zombie_run14() {FRAME_STATE(ZombieFrames::FR_RUN14, zombie_run15); COOP_Run(4);}
void zombie_run15() {FRAME_STATE(ZombieFrames::FR_RUN15, zombie_run16); COOP_Run(6);}
void zombie_run16() {FRAME_STATE(ZombieFrames::FR_RUN16, zombie_run17); COOP_Run(7);}
void zombie_run17() {FRAME_STATE(ZombieFrames::FR_RUN17, zombie_run18); COOP_Run(3);}
void zombie_run18() {
FRAME_STATE(ZombieFrames::FR_RUN18, zombie_run1);
COOP_Run(8);
if (random() < 0.2)
	sound (self, PR_CHAN_VOICE, "zombie/z_idle.wav", 1, PR_ATTN_IDLE);
if (random() > 0.8)
	sound (self, PR_CHAN_VOICE, "zombie/z_idle1.wav", 1, PR_ATTN_IDLE);
}

/*
=============================================================================

ATTACKS

=============================================================================
*/

void ZombieGrenadeTouch()
{
	if (other == self->owner)
		return;		// don't explode on owner
	if (other->takedamage)
	{
		T_Damage (other, self, self->owner, PR_IDMON_ZOMBIE_ATTACKDMG );
		sound (self, PR_CHAN_WEAPON, "zombie/z_hit.wav", 1, PR_ATTN_NORM);
		remove (self);
		return;
	}
	sound (self, PR_CHAN_WEAPON, "zombie/z_miss.wav", 1, PR_ATTN_NORM);	// bounce sound
	self->velocity = V({0, 0, 0});
	self->avelocity = V({0, 0, 0});
	self->touch = SUB_Remove;
}

/*
================
ZombieFireGrenade
================
*/
void ZombieFireGrenade(const vector& st)
{
	entity missile;
	vector	org;

	sound (self, PR_CHAN_WEAPON, "zombie/z_shot1.wav", 1, PR_ATTN_NORM);

	missile = spawn ();
	missile->owner = self;
	missile->movetype = PR_MOVETYPE_BOUNCE;
	missile->solid = PR_SOLID_BBOX;

// calc org
	org = self->origin + st[X] * v_forward + st[Y] * v_right + (st[Z] - 24) * v_up;

// set missile speed

	makevectors (self->angles);

	missile->velocity = normalize((self->enemy->origin + self->PR_npc_enemyoffset) - org);
	missile->velocity = missile->velocity * 600;
	missile->velocity[Z] = 200;

	missile->avelocity = V({3000, 1000, 2000});

	missile->touch = ZombieGrenadeTouch;

// set missile duration
	missile->nextthink = time + 2.5;
	missile->think = SUB_Remove;

	setmodel (missile, "progs/zom_gib.mdl");
	setsize (missile, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (missile, org);
}


void zombie_atta1() {FRAME_STATE(ZombieFrames::FR_ATTA1, zombie_atta2); COOP_Turn();}
void zombie_atta2() {FRAME_STATE(ZombieFrames::FR_ATTA2, zombie_atta3); COOP_Turn();}
void zombie_atta3() {FRAME_STATE(ZombieFrames::FR_ATTA3, zombie_atta4); COOP_Turn();}
void zombie_atta4() {FRAME_STATE(ZombieFrames::FR_ATTA4, zombie_atta5); COOP_Turn();}
void zombie_atta5() {FRAME_STATE(ZombieFrames::FR_ATTA5, zombie_atta6); COOP_Turn();}
void zombie_atta6() {FRAME_STATE(ZombieFrames::FR_ATTA6, zombie_atta7); COOP_Turn();}
void zombie_atta7() {FRAME_STATE(ZombieFrames::FR_ATTA7, zombie_atta8); COOP_Turn();}
void zombie_atta8() {FRAME_STATE(ZombieFrames::FR_ATTA8, zombie_atta9); COOP_Turn();}
void zombie_atta9() {FRAME_STATE(ZombieFrames::FR_ATTA9, zombie_atta10); COOP_Turn();}
void zombie_atta10() {FRAME_STATE(ZombieFrames::FR_ATTA10, zombie_atta11); COOP_Turn();}
void zombie_atta11() {FRAME_STATE(ZombieFrames::FR_ATTA11, zombie_atta12); COOP_Turn();}
void zombie_atta12() {FRAME_STATE(ZombieFrames::FR_ATTA12, zombie_atta13); COOP_Turn();}
void zombie_atta13() {FRAME_STATE(ZombieFrames::FR_ATTA13, zombie_run1); COOP_Turn();ZombieFireGrenade(V({-10, -22, 30}));}

void zombie_attb1() {FRAME_STATE(ZombieFrames::FR_ATTB1, zombie_attb2); COOP_Turn();}
void zombie_attb2() {FRAME_STATE(ZombieFrames::FR_ATTB2, zombie_attb3); COOP_Turn();}
void zombie_attb3() {FRAME_STATE(ZombieFrames::FR_ATTB3, zombie_attb4); COOP_Turn();}
void zombie_attb4() {FRAME_STATE(ZombieFrames::FR_ATTB4, zombie_attb5); COOP_Turn();}
void zombie_attb5() {FRAME_STATE(ZombieFrames::FR_ATTB5, zombie_attb6); COOP_Turn();}
void zombie_attb6() {FRAME_STATE(ZombieFrames::FR_ATTB6, zombie_attb7); COOP_Turn();}
void zombie_attb7() {FRAME_STATE(ZombieFrames::FR_ATTB7, zombie_attb8); COOP_Turn();}
void zombie_attb8() {FRAME_STATE(ZombieFrames::FR_ATTB8, zombie_attb9); COOP_Turn();}
void zombie_attb9() {FRAME_STATE(ZombieFrames::FR_ATTB9, zombie_attb10); COOP_Turn();}
void zombie_attb10() {FRAME_STATE(ZombieFrames::FR_ATTB10, zombie_attb11); COOP_Turn();}
void zombie_attb11() {FRAME_STATE(ZombieFrames::FR_ATTB11, zombie_attb12); COOP_Turn();}
void zombie_attb12() {FRAME_STATE(ZombieFrames::FR_ATTB12, zombie_attb13); COOP_Turn();}
void zombie_attb13() {FRAME_STATE(ZombieFrames::FR_ATTB13, zombie_attb14); COOP_Turn();}
void zombie_attb14() {FRAME_STATE(ZombieFrames::FR_ATTB13, zombie_run1); COOP_Turn();ZombieFireGrenade(V({-10, -24, 29}));}

void zombie_attc1() {FRAME_STATE(ZombieFrames::FR_ATTC1, zombie_attc2); COOP_Turn();}
void zombie_attc2() {FRAME_STATE(ZombieFrames::FR_ATTC2, zombie_attc3); COOP_Turn();}
void zombie_attc3() {FRAME_STATE(ZombieFrames::FR_ATTC3, zombie_attc4); COOP_Turn();}
void zombie_attc4() {FRAME_STATE(ZombieFrames::FR_ATTC4, zombie_attc5); COOP_Turn();}
void zombie_attc5() {FRAME_STATE(ZombieFrames::FR_ATTC5, zombie_attc6); COOP_Turn();}
void zombie_attc6() {FRAME_STATE(ZombieFrames::FR_ATTC6, zombie_attc7); COOP_Turn();}
void zombie_attc7() {FRAME_STATE(ZombieFrames::FR_ATTC7, zombie_attc8); COOP_Turn();}
void zombie_attc8() {FRAME_STATE(ZombieFrames::FR_ATTC8, zombie_attc9); COOP_Turn();}
void zombie_attc9() {FRAME_STATE(ZombieFrames::FR_ATTC9, zombie_attc10); COOP_Turn();}
void zombie_attc10() {FRAME_STATE(ZombieFrames::FR_ATTC10, zombie_attc11); COOP_Turn();}
void zombie_attc11() {FRAME_STATE(ZombieFrames::FR_ATTC11, zombie_attc12); COOP_Turn();}
void zombie_attc12() {FRAME_STATE(ZombieFrames::FR_ATTC12, zombie_run1); COOP_Turn();ZombieFireGrenade(V({-12, -19, 29}));}

void zombie_missile()
{
	float	r;

	r = random();

	if (r < 0.3)
		zombie_atta1 ();
	else if (r < 0.6)
		zombie_attb1 ();
	else
		zombie_attc1 ();
}

float ZombieCheckAttack()
{
	vector delta;

	delta = self->enemy->origin - self->origin;

	if (vlen(delta) > 350)
		return PR_FALSE;
	return PR_TRUE;
}


/*
=============================================================================

PAIN

=============================================================================
*/

void zombie_paina1() {FRAME_STATE(ZombieFrames::FR_PAINA1, zombie_paina2); sound (self, PR_CHAN_VOICE, "zombie/z_pain.wav", 1, PR_ATTN_NORM);}
void zombie_paina2() {FRAME_STATE(ZombieFrames::FR_PAINA2, zombie_paina3); ai_painforward(3);}
void zombie_paina3() {FRAME_STATE(ZombieFrames::FR_PAINA3, zombie_paina4); ai_painforward(1);}
void zombie_paina4() {FRAME_STATE(ZombieFrames::FR_PAINA4, zombie_paina5); ai_pain(1);}
void zombie_paina5() {FRAME_STATE(ZombieFrames::FR_PAINA5, zombie_paina6); ai_pain(3);}
void zombie_paina6() {FRAME_STATE(ZombieFrames::FR_PAINA6, zombie_paina7); ai_pain(1);}
void zombie_paina7() {FRAME_STATE(ZombieFrames::FR_PAINA7, zombie_paina8);}
void zombie_paina8() {FRAME_STATE(ZombieFrames::FR_PAINA8, zombie_paina9);}
void zombie_paina9() {FRAME_STATE(ZombieFrames::FR_PAINA9, zombie_paina10);}
void zombie_paina10() {FRAME_STATE(ZombieFrames::FR_PAINA10, zombie_paina11);}
void zombie_paina11() {FRAME_STATE(ZombieFrames::FR_PAINA11, zombie_paina12);}
void zombie_paina12() {FRAME_STATE(ZombieFrames::FR_PAINA12, zombie_run1);}

void zombie_painb1() {FRAME_STATE(ZombieFrames::FR_PAINB1, zombie_painb2); sound (self, PR_CHAN_VOICE, "zombie/z_pain1.wav", 1, PR_ATTN_NORM);}
void zombie_painb2() {FRAME_STATE(ZombieFrames::FR_PAINB2, zombie_painb3); ai_pain(2);}
void zombie_painb3() {FRAME_STATE(ZombieFrames::FR_PAINB3, zombie_painb4); ai_pain(8);}
void zombie_painb4() {FRAME_STATE(ZombieFrames::FR_PAINB4, zombie_painb5); ai_pain(6);}
void zombie_painb5() {FRAME_STATE(ZombieFrames::FR_PAINB5, zombie_painb6); ai_pain(2);}
void zombie_painb6() {FRAME_STATE(ZombieFrames::FR_PAINB6, zombie_painb7);}
void zombie_painb7() {FRAME_STATE(ZombieFrames::FR_PAINB7, zombie_painb8);}
void zombie_painb8() {FRAME_STATE(ZombieFrames::FR_PAINB8, zombie_painb9);}
void zombie_painb9() {FRAME_STATE(ZombieFrames::FR_PAINB9, zombie_painb10); sound (self, PR_CHAN_BODY, "zombie/z_fall.wav", 1, PR_ATTN_NORM);}
void zombie_painb10() {FRAME_STATE(ZombieFrames::FR_PAINB10, zombie_painb11);}
void zombie_painb11() {FRAME_STATE(ZombieFrames::FR_PAINB11, zombie_painb12);}
void zombie_painb12() {FRAME_STATE(ZombieFrames::FR_PAINB12, zombie_painb13);}
void zombie_painb13() {FRAME_STATE(ZombieFrames::FR_PAINB13, zombie_painb14);}
void zombie_painb14() {FRAME_STATE(ZombieFrames::FR_PAINB14, zombie_painb15);}
void zombie_painb15() {FRAME_STATE(ZombieFrames::FR_PAINB15, zombie_painb16);}
void zombie_painb16() {FRAME_STATE(ZombieFrames::FR_PAINB16, zombie_painb17);}
void zombie_painb17() {FRAME_STATE(ZombieFrames::FR_PAINB17, zombie_painb18);}
void zombie_painb18() {FRAME_STATE(ZombieFrames::FR_PAINB18, zombie_painb19);}
void zombie_painb19() {FRAME_STATE(ZombieFrames::FR_PAINB19, zombie_painb20);}
void zombie_painb20() {FRAME_STATE(ZombieFrames::FR_PAINB20, zombie_painb21);}
void zombie_painb21() {FRAME_STATE(ZombieFrames::FR_PAINB21, zombie_painb22);}
void zombie_painb22() {FRAME_STATE(ZombieFrames::FR_PAINB22, zombie_painb23);}
void zombie_painb23() {FRAME_STATE(ZombieFrames::FR_PAINB23, zombie_painb24);}
void zombie_painb24() {FRAME_STATE(ZombieFrames::FR_PAINB24, zombie_painb25);}
void zombie_painb25() {FRAME_STATE(ZombieFrames::FR_PAINB25, zombie_painb26); ai_painforward(1);}
void zombie_painb26() {FRAME_STATE(ZombieFrames::FR_PAINB26, zombie_painb27);}
void zombie_painb27() {FRAME_STATE(ZombieFrames::FR_PAINB27, zombie_painb28);}
void zombie_painb28() {FRAME_STATE(ZombieFrames::FR_PAINB28, zombie_run1);}

void zombie_painc1() {FRAME_STATE(ZombieFrames::FR_PAINC1, zombie_painc2); sound (self, PR_CHAN_VOICE, "zombie/z_pain1.wav", 1, PR_ATTN_NORM);}
void zombie_painc2() {FRAME_STATE(ZombieFrames::FR_PAINC2, zombie_painc3);}
void zombie_painc3() {FRAME_STATE(ZombieFrames::FR_PAINC3, zombie_painc4); ai_pain(3);}
void zombie_painc4() {FRAME_STATE(ZombieFrames::FR_PAINC4, zombie_painc5); ai_pain(1);}
void zombie_painc5() {FRAME_STATE(ZombieFrames::FR_PAINC5, zombie_painc6);}
void zombie_painc6() {FRAME_STATE(ZombieFrames::FR_PAINC6, zombie_painc7);}
void zombie_painc7() {FRAME_STATE(ZombieFrames::FR_PAINC7, zombie_painc8);}
void zombie_painc8() {FRAME_STATE(ZombieFrames::FR_PAINC8, zombie_painc9);}
void zombie_painc9() {FRAME_STATE(ZombieFrames::FR_PAINC9, zombie_painc10);}
void zombie_painc10() {FRAME_STATE(ZombieFrames::FR_PAINC10, zombie_painc11);}
void zombie_painc11() {FRAME_STATE(ZombieFrames::FR_PAINC11, zombie_painc12); ai_painforward(1);}
void zombie_painc12() {FRAME_STATE(ZombieFrames::FR_PAINC12, zombie_painc13); ai_painforward(1);}
void zombie_painc13() {FRAME_STATE(ZombieFrames::FR_PAINC13, zombie_painc14);}
void zombie_painc14() {FRAME_STATE(ZombieFrames::FR_PAINC14, zombie_painc15);}
void zombie_painc15() {FRAME_STATE(ZombieFrames::FR_PAINC15, zombie_painc16);}
void zombie_painc16() {FRAME_STATE(ZombieFrames::FR_PAINC16, zombie_painc17);}
void zombie_painc17() {FRAME_STATE(ZombieFrames::FR_PAINC17, zombie_painc18);}
void zombie_painc18() {FRAME_STATE(ZombieFrames::FR_PAINC18, zombie_run1);}

void zombie_paind1() {FRAME_STATE(ZombieFrames::FR_PAIND1, zombie_paind2); sound (self, PR_CHAN_VOICE, "zombie/z_pain.wav", 1, PR_ATTN_NORM);}
void zombie_paind2() {FRAME_STATE(ZombieFrames::FR_PAIND2, zombie_paind3);}
void zombie_paind3() {FRAME_STATE(ZombieFrames::FR_PAIND3, zombie_paind4);}
void zombie_paind4() {FRAME_STATE(ZombieFrames::FR_PAIND4, zombie_paind5);}
void zombie_paind5() {FRAME_STATE(ZombieFrames::FR_PAIND5, zombie_paind6);}
void zombie_paind6() {FRAME_STATE(ZombieFrames::FR_PAIND6, zombie_paind7);}
void zombie_paind7() {FRAME_STATE(ZombieFrames::FR_PAIND7, zombie_paind8);}
void zombie_paind8() {FRAME_STATE(ZombieFrames::FR_PAIND8, zombie_paind9);}
void zombie_paind9() {FRAME_STATE(ZombieFrames::FR_PAIND9, zombie_paind10); ai_pain(1);}
void zombie_paind10() {FRAME_STATE(ZombieFrames::FR_PAIND10, zombie_paind11);}
void zombie_paind11() {FRAME_STATE(ZombieFrames::FR_PAIND11, zombie_paind12);}
void zombie_paind12() {FRAME_STATE(ZombieFrames::FR_PAIND12, zombie_paind13);}
void zombie_paind13() {FRAME_STATE(ZombieFrames::FR_PAIND13, zombie_run1);}

void zombie_paine1() {
FRAME_STATE(ZombieFrames::FR_PAINE1, zombie_paine2);
sound (self, PR_CHAN_VOICE, "zombie/z_pain.wav", 1, PR_ATTN_NORM);
self->health = PR_IDMON_ZOMBIE_HEALTH;
}
void zombie_paine2() {FRAME_STATE(ZombieFrames::FR_PAINE2, zombie_paine3); ai_pain(8);}
void zombie_paine3() {FRAME_STATE(ZombieFrames::FR_PAINE3, zombie_paine4); ai_pain(5);}
void zombie_paine4() {FRAME_STATE(ZombieFrames::FR_PAINE4, zombie_paine5); ai_pain(3);}
void zombie_paine5() {FRAME_STATE(ZombieFrames::FR_PAINE5, zombie_paine6); ai_pain(1);}
void zombie_paine6() {FRAME_STATE(ZombieFrames::FR_PAINE6, zombie_paine7); ai_pain(2);}
void zombie_paine7() {FRAME_STATE(ZombieFrames::FR_PAINE7, zombie_paine8); ai_pain(1);}
void zombie_paine8() {FRAME_STATE(ZombieFrames::FR_PAINE8, zombie_paine9); ai_pain(1);}
void zombie_paine9() {FRAME_STATE(ZombieFrames::FR_PAINE9, zombie_paine10); ai_pain(2);}
void zombie_paine10() {
FRAME_STATE(ZombieFrames::FR_PAINE10, zombie_paine11);
sound (self, PR_CHAN_BODY, "zombie/z_fall.wav", 1, PR_ATTN_NORM);
self->solid = PR_SOLID_NOT;
}
void zombie_paine11() {FRAME_STATE(ZombieFrames::FR_PAINE11, zombie_paine12); self->nextthink = self->nextthink + 5;self->health = PR_IDMON_ZOMBIE_HEALTH;}
void zombie_paine12() {
// see if ok to stand up
FRAME_STATE(ZombieFrames::FR_PAINE12, zombie_paine13);
self->health = PR_IDMON_ZOMBIE_HEALTH;
sound (self, PR_CHAN_VOICE, "zombie/z_idle.wav", 1, PR_ATTN_IDLE);
self->solid = PR_SOLID_SLIDEBOX;
if ( InSolid( self ) )
{
	self->think = zombie_paine11;
	self->solid = PR_SOLID_NOT;
	return;
}
}
void zombie_paine13() {FRAME_STATE(ZombieFrames::FR_PAINE13, zombie_paine14);}
void zombie_paine14() {FRAME_STATE(ZombieFrames::FR_PAINE14, zombie_paine15);}
void zombie_paine15() {FRAME_STATE(ZombieFrames::FR_PAINE15, zombie_paine16);}
void zombie_paine16() {FRAME_STATE(ZombieFrames::FR_PAINE16, zombie_paine17);}
void zombie_paine17() {FRAME_STATE(ZombieFrames::FR_PAINE17, zombie_paine18);}
void zombie_paine18() {FRAME_STATE(ZombieFrames::FR_PAINE18, zombie_paine19);}
void zombie_paine19() {FRAME_STATE(ZombieFrames::FR_PAINE19, zombie_paine20);}
void zombie_paine20() {FRAME_STATE(ZombieFrames::FR_PAINE20, zombie_paine21);}
void zombie_paine21() {FRAME_STATE(ZombieFrames::FR_PAINE21, zombie_paine22);}
void zombie_paine22() {FRAME_STATE(ZombieFrames::FR_PAINE22, zombie_paine23);}
void zombie_paine23() {FRAME_STATE(ZombieFrames::FR_PAINE23, zombie_paine24);}
void zombie_paine24() {FRAME_STATE(ZombieFrames::FR_PAINE24, zombie_paine25);}
void zombie_paine25() {FRAME_STATE(ZombieFrames::FR_PAINE25, zombie_paine26); ai_painforward(5);}
void zombie_paine26() {FRAME_STATE(ZombieFrames::FR_PAINE26, zombie_paine27); ai_painforward(3);}
void zombie_paine27() {FRAME_STATE(ZombieFrames::FR_PAINE27, zombie_paine28); ai_painforward(1);}
void zombie_paine28() {FRAME_STATE(ZombieFrames::FR_PAINE28, zombie_paine29); ai_pain(1);}
void zombie_paine29() {FRAME_STATE(ZombieFrames::FR_PAINE29, zombie_paine30);}
void zombie_paine30() {FRAME_STATE(ZombieFrames::FR_PAINE30, zombie_run1);}

void zombie_die()
{
	sound (self, PR_CHAN_VOICE, "zombie/z_gib.wav", 1, PR_ATTN_NORM);
	ThrowMonsterHead ("progs/h_zombie.mdl", self->health);
	ThrowGib("progs/gib1.mdl", -40, PR_TRUE, 0, PR_GIB1_KGS, PR_FALSE);
	ThrowGib("progs/gib2.mdl", -40, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
	ThrowGib("progs/gib3.mdl", -40, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
	remove (self);
}

/*
=================
zombie_pain

Zombies can only be killed (gibbed) by doing 60 hit points of damage
in a single frame (rockets, grenades, quad shotgun, quad nailgun).

A hit of 25 points or more (super shotgun, quad nailgun) will allways put it
down to the ground.

A hit of from 10 to 40 points in one frame will cause it to go down if it
has been twice in two seconds, otherwise it goes into one of the four
fast pain frames.

A hit of less than 10 points of damage (winged by a shotgun) will be ignored.

FIXME: don't use pain_finished because of nightmare hack
=================
*/
void zombie_pain(entity attacker, float take)
{
	float r;

	self->health = PR_IDMON_ZOMBIE_HEALTH;		// always reset health

	if (take < PR_IDMON_ZOMBIE_DMGIGNORE)
		return;				// totally ignore

	if (self->inpain == 2)
		return;			// down on ground, so don't reset any counters

// go down immediately if a big enough hit
	if (take >= PR_IDMON_ZOMBIE_DMGFALL)
	{
		self->inpain = 2;
		zombie_paine1 ();
		return;
	}

	if (self->inpain)
	{
// if hit again in next gre seconds while not in pain frames, definately drop
		self->pain_finished = time + 3;
		return;			// currently going through an animation, don't change
	}

	if (self->pain_finished > time)
	{
// hit again, so drop down
		self->inpain = 2;
		zombie_paine1 ();
		return;
	}

// gp into one of the fast pain animations
	self->inpain = 1;

	r = random();
	if (r < 0.25)
		zombie_paina1 ();
	else if (r <  0.5)
		zombie_painb1 ();
	else if (r <  0.75)
		zombie_painc1 ();
	else
		zombie_paind1 ();
}

//============================================================================

float ZombieOnSight()
{
	sound (self, PR_CHAN_VOICE, "zombie/z_idle.wav", 1, PR_ATTN_NORM);
	return PR_TRUE;
}


/*QUAKED monster_zombie (1 0 0) (-16 -16 -24) (16 16 32) Crucified ambush

If crucified, stick the bounding box 12 pixels back into a wall to look right.
*/
void monster_zombie()
{
	if ( deathmatch ) {
		remove( self );
		return;
	}

	self->classname = "idmonster_zombie";

	if (!server_spawned) {
		precache_model ("progs/zombie.mdl");
		precache_model ("progs/h_zombie.mdl");
		precache_model ("progs/zom_gib.mdl");

		precache_sound ("zombie/z_idle.wav");
		precache_sound ("zombie/z_idle1.wav");
		precache_sound ("zombie/z_shot1.wav");
		precache_sound ("zombie/z_gib.wav");
		precache_sound ("zombie/z_pain.wav");
		precache_sound ("zombie/z_pain1.wav");
		precache_sound ("zombie/z_fall.wav");
		precache_sound ("zombie/z_miss.wav");
		precache_sound ("zombie/z_hit.wav");
		precache_sound ("zombie/idle_w2.wav");
	}

	self->solid = PR_SOLID_SLIDEBOX;
	self->movetype = PR_MOVETYPE_STEP;

	// there are no such things as zombie corpses, so no self.mdl head here
	setmodel (self, "progs/zombie.mdl");

	setsize (self, V({-16, -16, -24}), V({16, 16, 40}));
	self->max_health = self->health = PR_IDMON_ZOMBIE_HEALTH;

	self->th_stand = zombie_stand1;
	self->th_walk = zombie_walk1;
	self->th_run = zombie_run1;
	self->th_pain = zombie_pain;
	self->th_die = zombie_die;
	self->th_missile = zombie_missile;


	self->netname = "zombie";

	self->monsterweight = PR_MWEIGHT_SMALL;
	self->monsterflags = PR_MFLAG_HUMANOID | PR_MFLAG_NOHEART | PR_MFLAG_NOBREATHING | PR_MFLAG_NOLEGDAMAGE;

	// monsters have a team of their own
	if ( !self->team_no )
		self->team_no = PR_TEAMNO_MONSTERS;
	self->PR_monsterflag = PR_STRFLAG_MONSTER;

	self->is_connected = PR_TRUE;


	self->OnPreAttack = ZombieCheckAttack;
	self->OnNoticeEnemy = ZombieOnSight;
	self->PR_npc_spawnfunc = monster_zombie;
	COOP_SetLives ();

	self->PR_npc_bulletmult = 0.50;
	self->PR_npc_spikemult = 0.50;
	self->PR_npc_explosionmult = 2.15;
	self->PR_npc_electricitymult = 0.50;
	self->PR_npc_firemult = 3.96;

	if (self->spawnflags & PR_SPAWN_CRUCIFIED)
	{
		self->movetype = PR_MOVETYPE_NONE;
		zombie_cruc1 ();
	} else
		coop_walkmonster_start();
}

} // END namespace Progs

#endif
