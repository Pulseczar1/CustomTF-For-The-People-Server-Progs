/*
	shalrath.qc
*/

#include "progs.h"

#ifndef PR_COOP_MODE_ENHANCED
//#pragma DONT_COMPILE_THIS_FILE
#else

#include "idmonsters/shalrath.h"
#include "idmonsters/common.h"
#include "idmonsters/coop_monsters.h"
#include "coop_defs.h"
#include "ofndefs.h"
#include "player.h"
#include "combat.h"
#include "qw.h"
#include "monsters.h"
#include "warlock.h"

namespace Progs {

#define PR_IDMON_SHALRATH_HEALTH		1000
#define PR_IDMON_SHALRATH_SPIKEDMG	80

/*
==============================================================================

SHAL-RATH

==============================================================================
*/
//$cd id1/models/shalrath
//$origin 0 0 24
//$base base
//$skin skin
//$scale 0.7

namespace ShalrathFrames
{
	enum {FR_ATTACK1, FR_ATTACK2, FR_ATTACK3, FR_ATTACK4, FR_ATTACK5, FR_ATTACK6, FR_ATTACK7, FR_ATTACK8};
	enum {FR_ATTACK9 = 8, FR_ATTACK10, FR_ATTACK11};

	enum {FR_PAIN1 = 11, FR_PAIN2, FR_PAIN3, FR_PAIN4, FR_PAIN5};

	enum {FR_DEATH1 = 16, FR_DEATH2, FR_DEATH3, FR_DEATH4, FR_DEATH5, FR_DEATH6, FR_DEATH7};

	enum {FR_WALK1 = 23, FR_WALK2, FR_WALK3, FR_WALK4, FR_WALK5, FR_WALK6, FR_WALK7, FR_WALK8, FR_WALK9, FR_WALK10};
	enum {FR_WALK11 = 33, FR_WALK12};
}

void shalrath_pain(entity attacker, float damage);
void ShalMissile();
void shal_stand() {FRAME_STATE(ShalrathFrames::FR_WALK1, shal_stand); COOP_Idle();}

void shal_walk1() {
FRAME_STATE(ShalrathFrames::FR_WALK2, shal_walk2);
if (random() < 0.2)
	sound (self, PR_CHAN_VOICE, "shalrath/idle.wav", 1, PR_ATTN_IDLE);
COOP_Walk(6);}
void shal_walk2() {FRAME_STATE(ShalrathFrames::FR_WALK3, shal_walk3); COOP_Walk(4);}
void shal_walk3() {FRAME_STATE(ShalrathFrames::FR_WALK4, shal_walk4); COOP_Walk(0);}
void shal_walk4() {FRAME_STATE(ShalrathFrames::FR_WALK5, shal_walk5); COOP_Walk(0);}
void shal_walk5() {FRAME_STATE(ShalrathFrames::FR_WALK6, shal_walk6); COOP_Walk(0);}
void shal_walk6() {FRAME_STATE(ShalrathFrames::FR_WALK7, shal_walk7); COOP_Walk(0);}
void shal_walk7() {FRAME_STATE(ShalrathFrames::FR_WALK8, shal_walk8); COOP_Walk(5);}
void shal_walk8() {FRAME_STATE(ShalrathFrames::FR_WALK9, shal_walk9); COOP_Walk(6);}
void shal_walk9() {FRAME_STATE(ShalrathFrames::FR_WALK10, shal_walk10); COOP_Walk(5);}
void shal_walk10() {FRAME_STATE(ShalrathFrames::FR_WALK11, shal_walk11); COOP_Walk(0);}
void shal_walk11() {FRAME_STATE(ShalrathFrames::FR_WALK12, shal_walk12); COOP_Walk(4);}
void shal_walk12() {FRAME_STATE(ShalrathFrames::FR_WALK1, shal_walk1); COOP_Walk(5);}

void shal_run1() {
FRAME_STATE(ShalrathFrames::FR_WALK2, shal_run2);
if (random() < 0.2)
	sound (self, PR_CHAN_VOICE, "shalrath/idle.wav", 1, PR_ATTN_IDLE);
COOP_Run(6);}
void shal_run2() {FRAME_STATE(ShalrathFrames::FR_WALK3, shal_run3); COOP_Run(4);}
void shal_run3() {FRAME_STATE(ShalrathFrames::FR_WALK4, shal_run4); COOP_Run(0);}
void shal_run4() {FRAME_STATE(ShalrathFrames::FR_WALK5, shal_run5); COOP_Run(0);}
void shal_run5() {FRAME_STATE(ShalrathFrames::FR_WALK6, shal_run6); COOP_Run(0);}
void shal_run6() {FRAME_STATE(ShalrathFrames::FR_WALK7, shal_run7); COOP_Run(0);}
void shal_run7() {FRAME_STATE(ShalrathFrames::FR_WALK8, shal_run8); COOP_Run(5);}
void shal_run8() {FRAME_STATE(ShalrathFrames::FR_WALK9, shal_run9); COOP_Run(6);}
void shal_run9() {FRAME_STATE(ShalrathFrames::FR_WALK10, shal_run10); COOP_Run(5);}
void shal_run10() {FRAME_STATE(ShalrathFrames::FR_WALK11, shal_run11); COOP_Run(0);}
void shal_run11() {FRAME_STATE(ShalrathFrames::FR_WALK12, shal_run12); COOP_Run(4);}
void shal_run12() {FRAME_STATE(ShalrathFrames::FR_WALK1, shal_run1); COOP_Run(5);}

void shal_attack1() {
FRAME_STATE(ShalrathFrames::FR_ATTACK1, shal_attack2);
sound (self, PR_CHAN_VOICE, "shalrath/attack.wav", 1, PR_ATTN_NORM);
COOP_Turn();
}
void shal_attack2() {FRAME_STATE(ShalrathFrames::FR_ATTACK2, shal_attack3); COOP_Turn();}
void shal_attack3() {FRAME_STATE(ShalrathFrames::FR_ATTACK3, shal_attack4); COOP_Turn();}
void shal_attack4() {FRAME_STATE(ShalrathFrames::FR_ATTACK4, shal_attack5); COOP_Turn();}
void shal_attack5() {FRAME_STATE(ShalrathFrames::FR_ATTACK5, shal_attack6); COOP_Turn();}
void shal_attack6() {FRAME_STATE(ShalrathFrames::FR_ATTACK6, shal_attack7); COOP_Turn();}
void shal_attack7() {FRAME_STATE(ShalrathFrames::FR_ATTACK7, shal_attack8); COOP_Turn();}
void shal_attack8() {FRAME_STATE(ShalrathFrames::FR_ATTACK8, shal_attack9); COOP_Turn();}
void shal_attack9() {FRAME_STATE(ShalrathFrames::FR_ATTACK9, shal_attack10); ShalMissile();}
void shal_attack10() {FRAME_STATE(ShalrathFrames::FR_ATTACK10, shal_attack11); COOP_Turn();}
void shal_attack11() {FRAME_STATE(ShalrathFrames::FR_ATTACK11, shal_run1);}

void shal_pain1() {FRAME_STATE(ShalrathFrames::FR_PAIN1, shal_pain2);}
void shal_pain2() {FRAME_STATE(ShalrathFrames::FR_PAIN2, shal_pain3);}
void shal_pain3() {FRAME_STATE(ShalrathFrames::FR_PAIN3, shal_pain4);}
void shal_pain4() {FRAME_STATE(ShalrathFrames::FR_PAIN4, shal_pain5);}
void shal_pain5() {FRAME_STATE(ShalrathFrames::FR_PAIN5, shal_run1);}

void shal_death1() {FRAME_STATE(ShalrathFrames::FR_DEATH1, shal_death2);}
void shal_death2() {FRAME_STATE(ShalrathFrames::FR_DEATH2, shal_death3);}
void shal_death3() {FRAME_STATE(ShalrathFrames::FR_DEATH3, shal_death4);}
void shal_death4() {FRAME_STATE(ShalrathFrames::FR_DEATH4, shal_death5);}
void shal_death5() {FRAME_STATE(ShalrathFrames::FR_DEATH5, shal_death6);}
void shal_death6() {FRAME_STATE(ShalrathFrames::FR_DEATH6, shal_death7);}
void shal_death7() {FRAME_STATE(ShalrathFrames::FR_DEATH7, shal_death7); MonsterCorpse();}


void shalrath_pain(entity attacker, float damage)
{
	if (self->pain_finished > time)
		return;

	sound (self, PR_CHAN_VOICE, "shalrath/pain.wav", 1, PR_ATTN_NORM);
	shal_pain1();
	self->pain_finished = time + 3;
}

void shalrath_die()
{
// check for gib
	if (self->health < -90)
	{
		sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_NORM);
		ThrowMonsterHead ("progs/h_shal.mdl", self->health);
		ThrowGib("progs/gib1.mdl", -40, PR_TRUE, 0, PR_GIB1_KGS, PR_FALSE);
		ThrowGib("progs/gib2.mdl", -40, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
		ThrowGib("progs/gib3.mdl", -40, PR_TRUE, 0, PR_GIB3_KGS, PR_FALSE);
		remove (self);
		return;
	}

	sound (self, PR_CHAN_VOICE, "shalrath/death.wav", 1, PR_ATTN_NORM);
	shal_death1();
	self->solid = PR_SOLID_NOT;
}

/*
================
ShalMissile
================
*/
void ShalMissileTouch();
void ShalHome();
void ShalMissile()
{
	entity 	missile;
	vector	dir;
	float	dist, flytime;

	dir = normalize((self->enemy->origin + V({0, 0, 10})) - self->origin);
	dist = vlen (self->enemy->origin - self->origin);
	flytime = dist * 0.002;
	if (flytime < 0.1)
		flytime = 0.1;

	muzzleflash ();
	sound (self, PR_CHAN_WEAPON, "shalrath/attack2.wav", 1, PR_ATTN_NORM);

	missile = spawn ();
	missile->owner = self;

	missile->solid = PR_SOLID_BBOX;
	missile->movetype = PR_MOVETYPE_FLYMISSILE;
	setmodel (missile, "progs/v_spike.mdl");

	setsize (missile, V({0, 0, 0}), V({0, 0, 0}));

	missile->origin = self->origin + V({0, 0, 10});
	missile->velocity = dir * 400;
	missile->avelocity = V({300, 300, 300});
	missile->nextthink = flytime + time;
	missile->think = ShalHome;
	missile->enemy = self->enemy;
	missile->touch = ShalMissileTouch;
	missile->PR_npc_enemyoffset = self->PR_npc_enemyoffset;
}

void ShalHome()
{
	vector	dir, vtemp;
	vtemp = self->enemy->origin + self->PR_npc_enemyoffset;
	if (self->enemy->health < 1)
	{
		remove(self);
		return;
	}
	dir = normalize(vtemp - self->origin);
	self->velocity = dir * 350;
	self->nextthink = time + 0.2;
	self->think = ShalHome;
}

void ShalMissileTouch()
{
	if (other == self->owner)
		return;		// don't explode on owner

	// Gizmo - I changed enemy monster classnames to prevent any current or future conflicts with summons
	if (other->classname == "idmonster_zombie")
		T_Damage (other, self, self->owner, 200);
	T_RadiusDamage (self, self->owner, PR_IDMON_SHALRATH_SPIKEDMG, world);
	sound (self, PR_CHAN_WEAPON, "weapons/r_exp3.wav", 1, PR_ATTN_NORM);

	WriteByte (PR_MSG_MULTICAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_MULTICAST, PR_TE_EXPLOSION);
	WriteCoord (PR_MSG_MULTICAST, self->origin[X]);
	WriteCoord (PR_MSG_MULTICAST, self->origin[Y]);
	WriteCoord (PR_MSG_MULTICAST, self->origin[Z]);
	multicast (self->origin, PR_MULTICAST_PHS);

	remove (self);
}

//=================================================================

float ShalOnSight()
{
	sound (self, PR_CHAN_VOICE, "shalrath/sight.wav", 1, PR_ATTN_NORM);
	return PR_TRUE;
}

/*QUAKED monster_shalrath (1 0 0) (-32 -32 -24) (32 32 48) Ambush
*/
void monster_shalrath()
{
	if ( deathmatch ) {
		remove( self );
		return;
	}

	self->classname = "idmonster_shalrath";

	if (!server_spawned) {
		precache_model2 ("progs/shalrath.mdl");
		precache_model2 ("progs/h_shal.mdl");
		precache_model2 ("progs/v_spike.mdl");

		precache_sound2 ("shalrath/attack.wav");
		precache_sound2 ("shalrath/attack2.wav");
		precache_sound2 ("shalrath/death.wav");
		precache_sound2 ("shalrath/idle.wav");
		precache_sound2 ("shalrath/pain.wav");
		precache_sound2 ("shalrath/sight.wav");
	}

	self->solid = PR_SOLID_SLIDEBOX;
	self->movetype = PR_MOVETYPE_STEP;

	self->mdl = "progs/h_shal.mdl";
	setmodel (self, "progs/shalrath.mdl");
	setsize (self, PR_VEC_HULL2_MIN, PR_VEC_HULL2_MAX);
	self->max_health = self->health = PR_IDMON_SHALRATH_HEALTH;

	self->th_stand = shal_stand;
	self->th_walk = shal_walk1;
	self->th_run = shal_run1;
	self->th_die = shalrath_die;
	self->th_pain = shalrath_pain;
	self->th_missile = shal_attack1;


	self->netname = "vore";

	// weight: bigger than MEDIUM, but less than LARGE
	self->monsterweight = 380;
	self->monsterflags = 0;

	// monsters have a team of their own
	if ( !self->team_no )
		self->team_no = PR_TEAMNO_MONSTERS;
	self->PR_monsterflag = PR_STRFLAG_MONSTER;

	self->is_connected = PR_TRUE;

	self->OnNoticeEnemy = ShalOnSight;
	self->PR_npc_spawnfunc = monster_shalrath;
	COOP_SetLives ();

	self->PR_npc_explosionmult = 0.75;
	self->PR_npc_electricitymult = 1.25;

	coop_walkmonster_start();
}

} // END namespace Progs

#endif
