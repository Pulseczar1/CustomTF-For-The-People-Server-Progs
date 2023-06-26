/*
	demon.qc
*/

#include "progs.h"

#ifndef PR_COOP_MODE_ENHANCED
//#pragma DONT_COMPILE_THIS_FILE
#else

#include "idmonsters/demon.h"
#include "idmonsters/coop_monsters.h"
#include "ofndefs.h"
#include "coop_defs.h"
#include "common.h"
#include "weapons.h"
#include "fight.h"
#include "monsters.h"
#include "warlock.h"
#include "ai.h"

namespace Progs {

#define PR_IDMON_DEMON_HEALTH		700
#define PR_IDMON_DEMON_DMGIGNORE		200
#define PR_IDMON_DEMON_SLASHDMG		80
#define PR_IDMON_DEMON_JUMPDMG		110
#define PR_IDMON_DEMON_JUMPVEL		800

/*
==============================================================================

DEMON

==============================================================================
*/

//$cd id1/models/demon3
//$scale	0.8
//$origin 0 0 24
//$base base
//$skin base

namespace DemonFrames
{
	enum {FR_STAND1, FR_STAND2, FR_STAND3, FR_STAND4, FR_STAND5, FR_STAND6, FR_STAND7, FR_STAND8, FR_STAND9};
	enum {FR_STAND10 = 9, FR_STAND11, FR_STAND12, FR_STAND13};

	enum {FR_WALK1 = 13, FR_WALK2, FR_WALK3, FR_WALK4, FR_WALK5, FR_WALK6, FR_WALK7, FR_WALK8};

	enum {FR_RUN1 = 21, FR_RUN2, FR_RUN3, FR_RUN4, FR_RUN5, FR_RUN6};

	enum {FR_LEAP1 = 27, FR_LEAP2, FR_LEAP3, FR_LEAP4, FR_LEAP5, FR_LEAP6, FR_LEAP7, FR_LEAP8, FR_LEAP9, FR_LEAP10};
	enum {FR_LEAP11 = 37, FR_LEAP12};

	enum {FR_PAIN1 = 39, FR_PAIN2, FR_PAIN3, FR_PAIN4, FR_PAIN5, FR_PAIN6};

	enum {FR_DEATH1 = 45, FR_DEATH2, FR_DEATH3, FR_DEATH4, FR_DEATH5, FR_DEATH6, FR_DEATH7, FR_DEATH8, FR_DEATH9};

	enum {FR_ATTACKA1 = 54, FR_ATTACKA2, FR_ATTACKA3, FR_ATTACKA4, FR_ATTACKA5, FR_ATTACKA6, FR_ATTACKA7, FR_ATTACKA8};
	enum {FR_ATTACKA9 = 62, FR_ATTACKA10, FR_ATTACKA11, FR_ATTACKA12, FR_ATTACKA13, FR_ATTACKA14, FR_ATTACKA15};
}

//============================================================================

void _Demon_JumpTouch();
void _Demon_Melee(float side);
float _DemonCheckAttack();

void _demon1_stand1() {FRAME_STATE(DemonFrames::FR_STAND1, _demon1_stand2); COOP_Idle();}
void _demon1_stand2() {FRAME_STATE(DemonFrames::FR_STAND2, _demon1_stand3);}
void _demon1_stand3() {FRAME_STATE(DemonFrames::FR_STAND3, _demon1_stand4);}
void _demon1_stand4() {FRAME_STATE(DemonFrames::FR_STAND4, _demon1_stand5);}
void _demon1_stand5() {FRAME_STATE(DemonFrames::FR_STAND5, _demon1_stand6); COOP_Idle();}
void _demon1_stand6() {FRAME_STATE(DemonFrames::FR_STAND6, _demon1_stand7);}
void _demon1_stand7() {FRAME_STATE(DemonFrames::FR_STAND7, _demon1_stand8);}
void _demon1_stand8() {FRAME_STATE(DemonFrames::FR_STAND8, _demon1_stand9);}
void _demon1_stand9() {FRAME_STATE(DemonFrames::FR_STAND9, _demon1_stand10); COOP_Idle();}
void _demon1_stand10() {FRAME_STATE(DemonFrames::FR_STAND10, _demon1_stand11);}
void _demon1_stand11() {FRAME_STATE(DemonFrames::FR_STAND11, _demon1_stand12);}
void _demon1_stand12() {FRAME_STATE(DemonFrames::FR_STAND12, _demon1_stand13);}
void _demon1_stand13() {FRAME_STATE(DemonFrames::FR_STAND13, _demon1_stand1);}

void _demon1_walk1() {
FRAME_STATE(DemonFrames::FR_WALK1, _demon1_walk2);
if (random() < 0.2)
    sound (self, PR_CHAN_VOICE, "demon/idle1.wav", 1, PR_ATTN_IDLE);
COOP_Walk(8);
}
void _demon1_walk2() {FRAME_STATE(DemonFrames::FR_WALK2, _demon1_walk3); COOP_Walk(6);}
void _demon1_walk3() {FRAME_STATE(DemonFrames::FR_WALK3, _demon1_walk4); COOP_Walk(6);}
void _demon1_walk4() {FRAME_STATE(DemonFrames::FR_WALK4, _demon1_walk5); COOP_Walk(7);}
void _demon1_walk5() {FRAME_STATE(DemonFrames::FR_WALK5, _demon1_walk6); COOP_Walk(4);}
void _demon1_walk6() {FRAME_STATE(DemonFrames::FR_WALK6, _demon1_walk7); COOP_Walk(6);}
void _demon1_walk7() {FRAME_STATE(DemonFrames::FR_WALK7, _demon1_walk8); COOP_Walk(10);}
void _demon1_walk8() {FRAME_STATE(DemonFrames::FR_WALK8, _demon1_walk1); COOP_Walk(10);}

void _demon1_run1() {
FRAME_STATE(DemonFrames::FR_RUN1, _demon1_run2);
if (random() < 0.2)
    sound (self, PR_CHAN_VOICE, "demon/idle1.wav", 1, PR_ATTN_IDLE);
COOP_Run(20);}
void _demon1_run2() {FRAME_STATE(DemonFrames::FR_RUN2, _demon1_run3); COOP_Run(15);}
void _demon1_run3() {FRAME_STATE(DemonFrames::FR_RUN3, _demon1_run4); COOP_Run(36);}
void _demon1_run4() {FRAME_STATE(DemonFrames::FR_RUN4, _demon1_run5); COOP_Run(20);}
void _demon1_run5() {FRAME_STATE(DemonFrames::FR_RUN5, _demon1_run6); COOP_Run(15);}
void _demon1_run6() {FRAME_STATE(DemonFrames::FR_RUN6, _demon1_run1); COOP_Run(36);}

void _demon1_jump1() {FRAME_STATE(DemonFrames::FR_LEAP1, _demon1_jump2); COOP_Turn();}
void _demon1_jump2() {FRAME_STATE(DemonFrames::FR_LEAP2, _demon1_jump3); COOP_Turn();}
void _demon1_jump3() {FRAME_STATE(DemonFrames::FR_LEAP3, _demon1_jump4); COOP_Turn();}
void _demon1_jump4()
{
	FRAME_STATE(DemonFrames::FR_LEAP4, _demon1_jump5);
	COOP_Turn();

	self->touch = _Demon_JumpTouch;
	makevectors (self->angles);
	self->origin[Z] = self->origin[Z] + 1;
	self->velocity = v_forward * PR_IDMON_DEMON_JUMPVEL + V({0, 0, 250});
	if (self->flags & PR_FL_ONGROUND)
		self->flags = self->flags - PR_FL_ONGROUND;
}
void _demon1_jump5() {FRAME_STATE(DemonFrames::FR_LEAP5, _demon1_jump6);}
void _demon1_jump6() {FRAME_STATE(DemonFrames::FR_LEAP6, _demon1_jump7);}
void _demon1_jump7() {FRAME_STATE(DemonFrames::FR_LEAP7, _demon1_jump8);}
void _demon1_jump8() {FRAME_STATE(DemonFrames::FR_LEAP8, _demon1_jump9);}
void _demon1_jump9() {FRAME_STATE(DemonFrames::FR_LEAP9, _demon1_jump10);}
void _demon1_jump10() {
FRAME_STATE(DemonFrames::FR_LEAP10, _demon1_jump1);
self->nextthink = time + 3;
// if three seconds pass, assume demon is stuck and jump again
}

void _demon1_jump11() {FRAME_STATE(DemonFrames::FR_LEAP11, _demon1_jump12);}
void _demon1_jump12() {
FRAME_STATE(DemonFrames::FR_LEAP12, _demon1_run1);
Attack_Finished( random()*2 );
}


void _demon1_atta1() {FRAME_STATE(DemonFrames::FR_ATTACKA1, _demon1_atta2); ai_charge(4);}
void _demon1_atta2() {FRAME_STATE(DemonFrames::FR_ATTACKA2, _demon1_atta3); ai_charge(0);}
void _demon1_atta3() {FRAME_STATE(DemonFrames::FR_ATTACKA3, _demon1_atta4); ai_charge(0);}
void _demon1_atta4() {FRAME_STATE(DemonFrames::FR_ATTACKA4, _demon1_atta5); ai_charge(1);}
void _demon1_atta5() {FRAME_STATE(DemonFrames::FR_ATTACKA5, _demon1_atta6); ai_charge(2); _Demon_Melee(200);}
void _demon1_atta6() {FRAME_STATE(DemonFrames::FR_ATTACKA6, _demon1_atta7); ai_charge(1);}
void _demon1_atta7() {FRAME_STATE(DemonFrames::FR_ATTACKA7, _demon1_atta8); ai_charge(6);}
void _demon1_atta8() {FRAME_STATE(DemonFrames::FR_ATTACKA8, _demon1_atta9); ai_charge(8);}
void _demon1_atta9() {FRAME_STATE(DemonFrames::FR_ATTACKA9, _demon1_atta10); ai_charge(4);}
void _demon1_atta10() {FRAME_STATE(DemonFrames::FR_ATTACKA10, _demon1_atta11); ai_charge(2);}
void _demon1_atta11() {FRAME_STATE(DemonFrames::FR_ATTACKA11, _demon1_atta12); _Demon_Melee(-200);}
void _demon1_atta12() {FRAME_STATE(DemonFrames::FR_ATTACKA12, _demon1_atta13); ai_charge(5);}
void _demon1_atta13() {FRAME_STATE(DemonFrames::FR_ATTACKA13, _demon1_atta14); ai_charge(8);}
void _demon1_atta14() {FRAME_STATE(DemonFrames::FR_ATTACKA14, _demon1_atta15); ai_charge(4);}
void _demon1_atta15() {FRAME_STATE(DemonFrames::FR_ATTACKA15, _demon1_run1); ai_charge(4);}

void _demon1_pain1() {FRAME_STATE(DemonFrames::FR_PAIN1, _demon1_pain2);}
void _demon1_pain2() {FRAME_STATE(DemonFrames::FR_PAIN2, _demon1_pain3);}
void _demon1_pain3() {FRAME_STATE(DemonFrames::FR_PAIN3, _demon1_pain4);}
void _demon1_pain4() {FRAME_STATE(DemonFrames::FR_PAIN4, _demon1_pain5);}
void _demon1_pain5() {FRAME_STATE(DemonFrames::FR_PAIN5, _demon1_pain6);}
void _demon1_pain6() {FRAME_STATE(DemonFrames::FR_PAIN6, _demon1_run1);}

void _demon1_pain(entity attacker, float damage)
{
	if (self->touch == _Demon_JumpTouch)
		return;

	if (self->pain_finished > time)
		return;

	self->pain_finished = time + 1;
    sound (self, PR_CHAN_VOICE, "demon/dpain1.wav", 1, PR_ATTN_NORM);

	if (random()*PR_IDMON_DEMON_DMGIGNORE > damage)
		return;		// didn't flinch

	_demon1_pain1 ();
}

void _demon1_die1() {
FRAME_STATE(DemonFrames::FR_DEATH1, _demon1_die2);
sound (self, PR_CHAN_VOICE, "demon/ddeath.wav", 1, PR_ATTN_NORM);}
void _demon1_die2() {FRAME_STATE(DemonFrames::FR_DEATH2, _demon1_die3);}
void _demon1_die3() {FRAME_STATE(DemonFrames::FR_DEATH3, _demon1_die4);}
void _demon1_die4() {FRAME_STATE(DemonFrames::FR_DEATH4, _demon1_die5);}
void _demon1_die5() {FRAME_STATE(DemonFrames::FR_DEATH5, _demon1_die6);}
void _demon1_die6()
{FRAME_STATE(DemonFrames::FR_DEATH6, _demon1_die7); self->solid = PR_SOLID_NOT;}
void _demon1_die7() {FRAME_STATE(DemonFrames::FR_DEATH7, _demon1_die8);}
void _demon1_die8() {FRAME_STATE(DemonFrames::FR_DEATH8, _demon1_die9);}
void _demon1_die9() {FRAME_STATE(DemonFrames::FR_DEATH9, _demon1_die9); MonsterCorpse();}

void _demon_die()
{
// check for gib
	if (self->health < -80)
	{
		sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_NORM);
		ThrowMonsterHead ("progs/h_demon.mdl", self->health);
		ThrowGib("progs/gib1.mdl", -40, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
		ThrowGib("progs/gib1.mdl", -40, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
		ThrowGib("progs/gib1.mdl", -40, PR_TRUE, 0, PR_GIB3_KGS, PR_FALSE);
		remove (self);
		return;
	}

// regular death
	_demon1_die1 ();
}


void _Demon_MeleeAttack()
{
	_demon1_atta1 ();
}


float _Demon_OnSight()
{
	sound (self, PR_CHAN_VOICE, "demon/sight2.wav", 1, PR_ATTN_NORM);
	return PR_TRUE;
}


/*QUAKED monster_demon1 (1 0 0) (-32 -32 -24) (32 32 64) Ambush

*/
void monster_demon1()
{
	if ( deathmatch ) {
		remove( self );
		return;
	}

	self->classname = "idmonster_demon1";

	if (!server_spawned) {
		precache_model ("progs/demon.mdl");
		precache_model ("progs/h_demon.mdl");

		precache_sound ("demon/ddeath.wav");
		precache_sound ("demon/dhit2.wav");
		precache_sound ("demon/djump.wav");
		precache_sound ("demon/dpain1.wav");
		precache_sound ("demon/idle1.wav");
		precache_sound ("demon/sight2.wav");
	}

	self->solid = PR_SOLID_SLIDEBOX;
	self->movetype = PR_MOVETYPE_STEP;

	self->mdl = "progs/h_demon.mdl";
	setmodel (self, "progs/demon.mdl");

	setsize (self, PR_VEC_HULL2_MIN, PR_VEC_HULL2_MAX);
	self->max_health = self->health = PR_IDMON_DEMON_HEALTH;

	self->th_stand = _demon1_stand1;
	self->th_walk = _demon1_walk1;
	self->th_run = _demon1_run1;
	self->th_die = _demon_die;
	self->th_melee = _Demon_MeleeAttack;		// one of two attacks
	self->th_missile = _demon1_jump1;			// jump attack
	self->th_pain = _demon1_pain;


	self->netname = "fiend";
	//self.netname = COOP_GenerateMonsterName (self);

	self->monsterweight = PR_MWEIGHT_MEDIUM;

	// monsters have a team of their own
	if ( !self->team_no )
		self->team_no = PR_TEAMNO_MONSTERS;
	self->PR_monsterflag = PR_STRFLAG_MONSTER;

	self->is_connected = PR_TRUE;

	self->OnPreAttack = _DemonCheckAttack;
	self->OnNoticeEnemy = _Demon_OnSight;
	self->PR_npc_spawnfunc = monster_demon1;
	COOP_SetLives ();

	self->PR_npc_firemult = 1.25;

	coop_walkmonster_start();
}


/*
==============================================================================

DEMON

==============================================================================
*/

/*
==============
CheckDemonMelee

Returns TRUE if a melee attack would hit right now
==============
*/
float _CheckDemonMelee()
{
	if (enemy_range == PR_RANGE_MELEE)
	{	// FIXME: check canreach
		self->attack_state = PR_AS_MELEE;
		return PR_TRUE;
	}
	return PR_FALSE;
}

/*
==============
CheckDemonJump

==============
*/
float _CheckDemonJump()
{
	vector	dist;
	float	d;

	if (self->origin[Z] + self->mins[Z] > self->enemy->origin[Z] + self->enemy->mins[Z]
	+ 0.75 * self->enemy->size[Z])
		return PR_FALSE;

	if (self->origin[Z] + self->maxs[Z] < self->enemy->origin[Z] + self->enemy->mins[Z]
	+ 0.25 * self->enemy->size[Z])
		return PR_FALSE;

	dist = self->enemy->origin - self->origin;
	dist[Z] = 0;

	d = vlen(dist);

	if (d < 100)
		return PR_FALSE;

	if (d > 200)
	{
		if (random() < 0.9)
			return PR_FALSE;
	}

	return PR_TRUE;
}

float _DemonCheckAttack()
{
	enemy_range = range (self->enemy);

// if close enough for slashing, go for it
	if (_CheckDemonMelee ())
	{
		self->attack_state = PR_AS_MELEE;
		return PR_TRUE;
	}

	if (_CheckDemonJump ())
	{
		self->attack_state = PR_AS_MISSILE;
		sound (self, PR_CHAN_VOICE, "demon/djump.wav", 1, PR_ATTN_NORM);
		return PR_TRUE;
	}

	return PR_FALSE;
}


//===========================================================================

void _Demon_Melee(float side)
{
	float	ldmg;
	vector	delta;

	COOP_Turn ();
	walkmove (self->ideal_yaw, 12);	// allow a little closing

	delta = self->enemy->origin - self->origin;

	if (vlen(delta) > 100)
		return;
	if (!CanDamage (self->enemy, self))
		return;

	sound (self, PR_CHAN_WEAPON, "demon/dhit2.wav", 1, PR_ATTN_NORM);
	ldmg = PR_IDMON_DEMON_SLASHDMG + 5*random();
	TF_T_Damage (self->enemy, self, self, ldmg, PR_TF_TD_NOTTEAM, PR_TF_TD_MELEE);

	makevectors (self->angles);
	SpawnMeatSpray (self->origin + v_forward*16, side * v_right);
}


void _Demon_JumpTouch()
{
	float	ldmg;

	if (self->health <= 0)
		return;

	if (other->takedamage)
	{
		if ( vlen(self->velocity) > 400 )
		{
			ldmg = PR_IDMON_DEMON_JUMPDMG + 10*random();
			TF_T_Damage (other, self, self, ldmg, PR_TF_TD_NOTTEAM, PR_TF_TD_MELEE);
		}
	}

	if (!checkbottom(self))
	{
		if (self->flags & PR_FL_ONGROUND)
		{	// jump randomly to not get hung up
//dprint ("popjump\n");
	self->touch = SUB_Null;
	self->think = _demon1_jump1;
	self->nextthink = time + 0.1;

//			self.velocity_x = (random() - 0.5) * 600;
//			self.velocity_y = (random() - 0.5) * 600;
//			self.velocity_z = 200;
//			self.flags = self.flags - #FL_ONGROUND;
		}
		return;	// not on ground yet
	}

	self->touch = SUB_Null;
	self->think = _demon1_jump11;
	self->nextthink = time + 0.1;
}

} // END namespace Progs

#endif
