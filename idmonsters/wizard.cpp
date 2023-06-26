/*
	wizard.qc
*/

#include "progs.h"

#ifndef PR_COOP_MODE_ENHANCED
//#pragma DONT_COMPILE_THIS_FILE
#else

#include "idmonsters/wizard.h"
#include "idmonsters/common.h"
#include "idmonsters/coop_monsters.h"
#include "coop_defs.h"
#include "ofndefs.h"
#include "qw.h"
#include "weapons.h"
#include "ai.h"
#include "monsters.h"
#include "warlock.h"

namespace Progs {

#define PR_IDMON_WIZARD_HEALTH		220
#define PR_IDMON_WIZARD_SPIKEDMG		60		// 150

/*
==============================================================================

WIZARD

==============================================================================
*/

//$cd id1/models/a_wizard
//$origin 0 0 24
//$base wizbase
//$skin wizbase

namespace WizardFrames
{
	enum {FR_HOVER1, FR_HOVER2, FR_HOVER3, FR_HOVER4, FR_HOVER5, FR_HOVER6, FR_HOVER7, FR_HOVER8};
	enum {FR_HOVER9 = 8, FR_HOVER10, FR_HOVER11, FR_HOVER12, FR_HOVER13, FR_HOVER14, FR_HOVER15};

	enum {FR_FLY1 = 15, FR_FLY2, FR_FLY3, FR_FLY4, FR_FLY5, FR_FLY6, FR_FLY7, FR_FLY8, FR_FLY9, FR_FLY10};
	enum {FR_FLY11 = 25, FR_FLY12, FR_FLY13, FR_FLY14};

	enum {FR_MAGATT1 = 29, FR_MAGATT2, FR_MAGATT3, FR_MAGATT4, FR_MAGATT5, FR_MAGATT6, FR_MAGATT7};
	enum {FR_MAGATT8 = 36, FR_MAGATT9, FR_MAGATT10, FR_MAGATT11, FR_MAGATT12, FR_MAGATT13};

	enum {FR_PAIN1 = 42, FR_PAIN2, FR_PAIN3, FR_PAIN4};

	enum {FR_DEATH1 = 46, FR_DEATH2, FR_DEATH3, FR_DEATH4, FR_DEATH5, FR_DEATH6, FR_DEATH7, FR_DEATH8};
}

/*
==============================================================================

WIZARD

If the player moves behind cover before the missile is launched, launch it
at the last visible spot with no velocity leading, in hopes that the player
will duck back out and catch it.
==============================================================================
*/

/*
=============
LaunchMissile

Sets the given entities velocity and angles so that it will hit self.enemy
if self.enemy maintains it's current velocity
0.1 is moderately accurate, 0.0 is totally accurate
=============
*/
void LaunchMissile(entity missile, float mspeed, float accuracy)
{
	vector	vec, move;
	float	fly;

	makevectors (self->angles);

// set missile speed
	vec = self->enemy->origin + self->enemy->mins + self->enemy->size * 0.7 - missile->origin;

// calc aproximate time for missile to reach vec
	fly = vlen (vec) / mspeed;

// get the entities xy velocity
	move = self->enemy->velocity;
	move[Z] = 0;

// project the target forward in time
	vec = vec + move * fly;

	vec = normalize(vec);
	vec = vec + accuracy*v_up*(random()- 0.5) + accuracy*v_right*(random()- 0.5);

	missile->velocity = vec * mspeed;

//	missile.angles = '0 0 0';
//	missile.angles_y = vectoyaw(missile.velocity);
	missile->angles = vectoangles (missile->velocity);

// set missile duration
	missile->nextthink = time + 5;
	missile->think = SUB_Remove;
}


void _wiz_run1();
void _wiz_side1();

/*
=================
WizardCheckAttack
=================
*/
float _WizardCheckAttack()
{
	//entity  targ;
	float     chance;

	if (!infront(self->enemy))
		return PR_FALSE;

	enemy_range = range (self->enemy);
	enemy_vis = visible (self->enemy);

	if (time < self->attack_finished)
		return PR_FALSE;
	if (!enemy_vis)
		return PR_FALSE;

	if (enemy_range == PR_RANGE_FAR) {
		if (self->attack_state != PR_AS_STRAIGHT) {
			self->attack_state = PR_AS_STRAIGHT;
		}
		return PR_FALSE;
	}

	//targ = self->enemy;

	if (enemy_range == PR_RANGE_MELEE)
		chance = 0.9;
	else if (enemy_range == PR_RANGE_NEAR)
		chance = 0.6;
	else if (enemy_range == PR_RANGE_MID)
		chance = 0.2;
	else
		chance = 0;

	if (random () < chance) {
		self->attack_state = PR_AS_MISSILE;
		return PR_TRUE;
	}

	if (enemy_range == PR_RANGE_MID) {
		if (self->attack_state != PR_AS_STRAIGHT) {
			self->attack_state = PR_AS_STRAIGHT;
		}
	} else {
		if (self->attack_state != PR_AS_SLIDING) {
			self->attack_state = PR_AS_SLIDING;
		}
	}

	return PR_FALSE;
}

/*
=================
WizardAttackFinished
=================
*/
void _WizardAttackFinished()  // PZ: This did have a return value of `float`, but it wasn't returning anything.
{
	if (enemy_range >= PR_RANGE_MID || !visible(self->enemy))
	{
		self->attack_state = PR_AS_STRAIGHT;
		self->think = _wiz_run1;
	}
	else
	{
		self->attack_state = PR_AS_SLIDING;
		self->think = _wiz_side1;
	}
}

/*
==============================================================================

FAST ATTACKS

==============================================================================
*/

void wizspike_touch()
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
		spawn_touchblood (PR_IDMON_WIZARD_SPIKEDMG);
		other->deathtype = "nail";
		TF_T_Damage (other, self, self->owner, PR_IDMON_WIZARD_SPIKEDMG + random()*50, PR_TF_TD_NOTTEAM, PR_TF_TD_NAIL);
	} else {
		WriteByte (PR_MSG_MULTICAST, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_MULTICAST, PR_TE_WIZSPIKE);
		WriteCoord (PR_MSG_MULTICAST, self->origin[X]);
		WriteCoord (PR_MSG_MULTICAST, self->origin[Y]);
		WriteCoord (PR_MSG_MULTICAST, self->origin[Z]);
		multicast (self->origin, PR_MULTICAST_PHS);
	}

	remove(self);
}


void _Wiz_FastFire()
{
	vector		vec;
	vector		dst;

	if (self->owner->health > 0)
	{
		muzzleflash ();

		makevectors (self->enemy->angles);
		dst = self->enemy->origin - 13*self->movedir;

		vec = normalize(dst - self->origin);
		sound (self, PR_CHAN_WEAPON, "wizard/wattack.wav", 1, PR_ATTN_NORM);


		newmis = spawn ();
		newmis->movetype = PR_MOVETYPE_FLYMISSILE;
		newmis->solid = PR_SOLID_BBOX;

		newmis->angles = vectoangles(vec);

		newmis->touch = wizspike_touch;
		newmis->think = SUB_Remove;
		newmis->nextthink = time + 6;
		setorigin (newmis, self->origin);

//		newmis.velocity = vec * 2000;
//		newmis.velocity = vec*600;

		// TODO: Gizmo - someday I want monsters to look at where they want to shoot, and just have weapons shoot via v_forward
		LaunchMissile (newmis, 600, 0.1);
		newmis->owner = self->owner;
		newmis->classname = "wizspike";
		setmodel (newmis, "progs/w_spike.mdl");
		setsize (newmis, PR_VEC_ORIGIN, PR_VEC_ORIGIN);
	}

	remove (self);
}


void _Wiz_StartFast()
{
	entity	missile;

	sound (self, PR_CHAN_WEAPON, "wizard/wattack.wav", 1, PR_ATTN_NORM);
	self->v_angle = self->angles;
	makevectors (self->angles);

	missile = spawnServerSide(); // PZ: make it a server-side-only entity
	missile->owner = self;
	missile->nextthink = time + 0.6;
	setsize (missile, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (missile, self->origin + V({0, 0, 30}) + v_forward*14 + v_right*14);
	missile->enemy = self->enemy;
	missile->nextthink = time + 0.8;
	missile->think = _Wiz_FastFire;
	missile->movedir = v_right;

	missile = spawnServerSide(); // PZ: make it a server-side-only entity
	missile->owner = self;
	missile->nextthink = time + 1;
	setsize (missile, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (missile, self->origin + V({0, 0, 30}) + v_forward*14 + v_right* -14);
	missile->enemy = self->enemy;
	missile->nextthink = time + 0.3;
	missile->think = _Wiz_FastFire;
	missile->movedir = PR_VEC_ORIGIN - v_right;
}



void _Wiz_idlesound()
{
float wr;
	wr = random() * 5;

	if (self->waitmin < time)
	{
	 	self->waitmin = time + 2;
	 	if (wr > 4.5)
	 		sound (self, PR_CHAN_VOICE, "wizard/widle1.wav", 1,  PR_ATTN_IDLE);
	 	if (wr < 1.5)
	 		sound (self, PR_CHAN_VOICE, "wizard/widle2.wav", 1, PR_ATTN_IDLE);
	}
	return;
}

void _wiz_stand1() {FRAME_STATE(WizardFrames::FR_HOVER1, _wiz_stand2); COOP_Idle();}
void _wiz_stand2() {FRAME_STATE(WizardFrames::FR_HOVER2, _wiz_stand3);}
void _wiz_stand3() {FRAME_STATE(WizardFrames::FR_HOVER3, _wiz_stand4);}
void _wiz_stand4() {FRAME_STATE(WizardFrames::FR_HOVER4, _wiz_stand5);}
void _wiz_stand5() {FRAME_STATE(WizardFrames::FR_HOVER5, _wiz_stand6); COOP_Idle();}
void _wiz_stand6() {FRAME_STATE(WizardFrames::FR_HOVER6, _wiz_stand7);}
void _wiz_stand7() {FRAME_STATE(WizardFrames::FR_HOVER7, _wiz_stand8);}
void _wiz_stand8() {FRAME_STATE(WizardFrames::FR_HOVER8, _wiz_stand1);}

void _wiz_walk1() {FRAME_STATE(WizardFrames::FR_HOVER1, _wiz_walk2); COOP_Walk(8);
_Wiz_idlesound();}
void _wiz_walk2() {FRAME_STATE(WizardFrames::FR_HOVER2, _wiz_walk3); COOP_Walk(8);}
void _wiz_walk3() {FRAME_STATE(WizardFrames::FR_HOVER3, _wiz_walk4); COOP_Walk(8);}
void _wiz_walk4() {FRAME_STATE(WizardFrames::FR_HOVER4, _wiz_walk5); COOP_Walk(8);}
void _wiz_walk5() {FRAME_STATE(WizardFrames::FR_HOVER5, _wiz_walk6); COOP_Walk(8);}
void _wiz_walk6() {FRAME_STATE(WizardFrames::FR_HOVER6, _wiz_walk7); COOP_Walk(8);}
void _wiz_walk7() {FRAME_STATE(WizardFrames::FR_HOVER7, _wiz_walk8); COOP_Walk(8);}
void _wiz_walk8() {FRAME_STATE(WizardFrames::FR_HOVER8, _wiz_walk1); COOP_Walk(8);}

void _wiz_side1() {FRAME_STATE(WizardFrames::FR_HOVER1, _wiz_side2); COOP_Run(8);
_Wiz_idlesound();}
void _wiz_side2() {FRAME_STATE(WizardFrames::FR_HOVER2, _wiz_side3); COOP_Run(8);}
void _wiz_side3() {FRAME_STATE(WizardFrames::FR_HOVER3, _wiz_side4); COOP_Run(8);}
void _wiz_side4() {FRAME_STATE(WizardFrames::FR_HOVER4, _wiz_side5); COOP_Run(8);}
void _wiz_side5() {FRAME_STATE(WizardFrames::FR_HOVER5, _wiz_side6); COOP_Run(8);}
void _wiz_side6() {FRAME_STATE(WizardFrames::FR_HOVER6, _wiz_side7); COOP_Run(8);}
void _wiz_side7() {FRAME_STATE(WizardFrames::FR_HOVER7, _wiz_side8); COOP_Run(8);}
void _wiz_side8() {FRAME_STATE(WizardFrames::FR_HOVER8, _wiz_side1); COOP_Run(8);}

void _wiz_run1() {FRAME_STATE(WizardFrames::FR_FLY1, _wiz_run2); COOP_Run(16);
_Wiz_idlesound();
}
void _wiz_run2() {FRAME_STATE(WizardFrames::FR_FLY2, _wiz_run3); COOP_Run(16);}
void _wiz_run3() {FRAME_STATE(WizardFrames::FR_FLY3, _wiz_run4); COOP_Run(16);}
void _wiz_run4() {FRAME_STATE(WizardFrames::FR_FLY4, _wiz_run5); COOP_Run(16);}
void _wiz_run5() {FRAME_STATE(WizardFrames::FR_FLY5, _wiz_run6); COOP_Run(16);}
void _wiz_run6() {FRAME_STATE(WizardFrames::FR_FLY6, _wiz_run7); COOP_Run(16);}
void _wiz_run7() {FRAME_STATE(WizardFrames::FR_FLY7, _wiz_run8); COOP_Run(16);}
void _wiz_run8() {FRAME_STATE(WizardFrames::FR_FLY8, _wiz_run9); COOP_Run(16);}
void _wiz_run9() {FRAME_STATE(WizardFrames::FR_FLY9, _wiz_run10); COOP_Run(16);}
void _wiz_run10() {FRAME_STATE(WizardFrames::FR_FLY10, _wiz_run11); COOP_Run(16);}
void _wiz_run11() {FRAME_STATE(WizardFrames::FR_FLY11, _wiz_run12); COOP_Run(16);}
void _wiz_run12() {FRAME_STATE(WizardFrames::FR_FLY12, _wiz_run13); COOP_Run(16);}
void _wiz_run13() {FRAME_STATE(WizardFrames::FR_FLY13, _wiz_run14); COOP_Run(16);}
void _wiz_run14() {FRAME_STATE(WizardFrames::FR_FLY14, _wiz_run1); COOP_Run(16);}

void _wiz_fast1() {FRAME_STATE(WizardFrames::FR_MAGATT1, _wiz_fast2); COOP_Turn();_Wiz_StartFast();}
void _wiz_fast2() {FRAME_STATE(WizardFrames::FR_MAGATT2, _wiz_fast3); COOP_Turn();}
void _wiz_fast3() {FRAME_STATE(WizardFrames::FR_MAGATT3, _wiz_fast4); COOP_Turn();}
void _wiz_fast4() {FRAME_STATE(WizardFrames::FR_MAGATT4, _wiz_fast5); COOP_Turn();}
void _wiz_fast5() {FRAME_STATE(WizardFrames::FR_MAGATT5, _wiz_fast6); COOP_Turn();}
void _wiz_fast6() {FRAME_STATE(WizardFrames::FR_MAGATT6, _wiz_fast7); COOP_Turn();}
void _wiz_fast7() {FRAME_STATE(WizardFrames::FR_MAGATT5, _wiz_fast8); COOP_Turn();}
void _wiz_fast8() {FRAME_STATE(WizardFrames::FR_MAGATT4, _wiz_fast9); COOP_Turn();}
void _wiz_fast9() {FRAME_STATE(WizardFrames::FR_MAGATT3, _wiz_fast10); COOP_Turn();}
void _wiz_fast10() {FRAME_STATE(WizardFrames::FR_MAGATT2, _wiz_run1); COOP_Turn();
	float r;

	r = range (self->enemy);

	if (r <= PR_RANGE_MELEE)
		Attack_Finished( 0.1 );
	else if (r > PR_RANGE_NEAR)
		Attack_Finished( 2 );
	else
		Attack_Finished( 0.5 );
	_WizardAttackFinished ();
}

void _wiz_pain1() {FRAME_STATE(WizardFrames::FR_PAIN1, _wiz_pain2);}
void _wiz_pain2() {FRAME_STATE(WizardFrames::FR_PAIN2, _wiz_pain3);}
void _wiz_pain3() {FRAME_STATE(WizardFrames::FR_PAIN3, _wiz_pain4);}
void _wiz_pain4() {FRAME_STATE(WizardFrames::FR_PAIN4, _wiz_run1);}

void _wiz_death1() {

FRAME_STATE(WizardFrames::FR_DEATH1, _wiz_death2);

self->velocity[X] = -200 + 400*random();
self->velocity[Y] = -200 + 400*random();
self->velocity[Z] = 100 + 100*random();
self->flags = self->flags - (self->flags & PR_FL_ONGROUND);
sound (self, PR_CHAN_VOICE, "wizard/wdeath.wav", 1, PR_ATTN_NORM);
}
void _wiz_death2() {FRAME_STATE(WizardFrames::FR_DEATH2, _wiz_death3);}
void _wiz_death3() {FRAME_STATE(WizardFrames::FR_DEATH3, _wiz_death4); self->solid = PR_SOLID_NOT;}
void _wiz_death4() {FRAME_STATE(WizardFrames::FR_DEATH4, _wiz_death5);}
void _wiz_death5() {FRAME_STATE(WizardFrames::FR_DEATH5, _wiz_death6);}
void _wiz_death6() {FRAME_STATE(WizardFrames::FR_DEATH6, _wiz_death7);}
void _wiz_death7() {FRAME_STATE(WizardFrames::FR_DEATH7, _wiz_death8);}
void _wiz_death8() {FRAME_STATE(WizardFrames::FR_DEATH8, _wiz_death8); MonsterCorpse();}

void _wiz_die()
{
// check for gib
	if (self->health < -40)
	{
		sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_NORM);
		ThrowMonsterHead ("progs/h_wizard.mdl", self->health);
		ThrowGib("progs/gib2.mdl", -40, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
		ThrowGib("progs/gib2.mdl", -40, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
		ThrowGib("progs/gib2.mdl", -40, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
		remove (self);
		return;
	}

	_wiz_death1 ();
}


void _Wiz_Pain(entity attacker, float damage)
{
	sound (self, PR_CHAN_VOICE, "wizard/wpain.wav", 1, PR_ATTN_NORM);
	if (random()*70 > damage)
		return;		// didn't flinch

	_wiz_pain1 ();
}


void _Wiz_Missile()
{
	_wiz_fast1();
}


float _WizardOnSight()
{
	sound (self, PR_CHAN_VOICE, "wizard/wsight.wav", 1, PR_ATTN_NORM);
	return PR_TRUE;
}

/*QUAKED monster_wizard (1 0 0) (-16 -16 -24) (16 16 40) Ambush
*/
void monster_wizard()
{
	if ( deathmatch ) {
		remove( self );
		return;
	}

	self->classname = "idmonster_wizard";

	if (!server_spawned) {
		precache_model ("progs/wizard.mdl");
		precache_model ("progs/h_wizard.mdl");
		precache_model ("progs/w_spike.mdl");

		precache_sound ("wizard/hit.wav");		// used by c code
		precache_sound ("wizard/wattack.wav");
		precache_sound ("wizard/wdeath.wav");
		precache_sound ("wizard/widle1.wav");
		precache_sound ("wizard/widle2.wav");
		precache_sound ("wizard/wpain.wav");
		precache_sound ("wizard/wsight.wav");
	}

	self->solid = PR_SOLID_SLIDEBOX;
	self->movetype = PR_MOVETYPE_STEP;

	self->mdl = "progs/h_wizard.mdl";
	setmodel (self, "progs/wizard.mdl");

	setsize (self, V({-16, -16, -24}), V({16, 16, 40}));
	self->max_health = self->health = PR_IDMON_WIZARD_HEALTH;

	self->th_stand = _wiz_stand1;
	self->th_walk = _wiz_walk1;
	self->th_run = _wiz_run1;
	self->th_missile = _Wiz_Missile;
	self->th_pain = _Wiz_Pain;
	self->th_die = _wiz_die;


	self->netname = "scrag";

	self->monsterweight = PR_MWEIGHT_SMALL;
	self->monsterflags = PR_MFLAG_NOLEGS;

	// monsters have a team of their own
	if ( !self->team_no )
		self->team_no = PR_TEAMNO_MONSTERS;
	self->PR_monsterflag = PR_STRFLAG_MONSTER;

	self->is_connected = PR_TRUE;

	self->OnPreAttack = _WizardCheckAttack;
	self->OnNoticeEnemy = _WizardOnSight;
	self->PR_npc_speedfactor = 3;
	self->PR_npc_spawnfunc = monster_wizard;
	COOP_SetLives ();

	self->PR_npc_spikemult = 1.2;
	self->PR_npc_explosionmult = 1.2;

	coop_flymonster_start ();
}

} // END namespace Progs

#endif
