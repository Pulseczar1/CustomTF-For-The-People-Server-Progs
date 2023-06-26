/*
	dog.qc
*/

#include "progs.h"

#ifndef PR_COOP_MODE_ENHANCED
//#pragma DONT_COMPILE_THIS_FILE
#else

#include "idmonsters/dog.h"
#include "idmonsters/coop_monsters.h"
#include "ofndefs.h"
#include "fight.h"
#include "combat.h"
#include "common.h"
#include "weapons.h"
#include "ai.h"
#include "monsters.h"
#include "warlock.h"
#include "coop_defs.h"

namespace Progs {

#define PR_IDMON_DOG_HEALTH		120
#define PR_IDMON_DOG_BITEDMG		60
#define PR_IDMON_DOG_JUMPDMG		80
#define PR_IDMON_DOG_JUMPVEL		700

/*
==============================================================================

DOG

==============================================================================
*/
//$cd id1/models/dog
//$origin 0 0 24
//$base base
//$skin skin

namespace DogFrames
{
	enum {FR_ATTACK1, FR_ATTACK2, FR_ATTACK3, FR_ATTACK4, FR_ATTACK5, FR_ATTACK6, FR_ATTACK7, FR_ATTACK8};

	enum {FR_DEATH1 = 8, FR_DEATH2, FR_DEATH3, FR_DEATH4, FR_DEATH5, FR_DEATH6, FR_DEATH7, FR_DEATH8, FR_DEATH9};

	enum {FR_DEATHB1 = 17, FR_DEATHB2, FR_DEATHB3, FR_DEATHB4, FR_DEATHB5, FR_DEATHB6, FR_DEATHB7, FR_DEATHB8};
	enum {FR_DEATHB9 = 25};

	enum {FR_PAIN1 = 26, FR_PAIN2, FR_PAIN3, FR_PAIN4, FR_PAIN5, FR_PAIN6};

	enum {FR_PAINB1 = 32, FR_PAINB2, FR_PAINB3, FR_PAINB4, FR_PAINB5, FR_PAINB6, FR_PAINB7, FR_PAINB8, FR_PAINB9, FR_PAINB10};
	enum {FR_PAINB11 = 42, FR_PAINB12, FR_PAINB13, FR_PAINB14, FR_PAINB15, FR_PAINB16};

	enum {FR_RUN1 = 48, FR_RUN2, FR_RUN3, FR_RUN4, FR_RUN5, FR_RUN6, FR_RUN7, FR_RUN8, FR_RUN9, FR_RUN10, FR_RUN11, FR_RUN12};

	enum {FR_LEAP1 = 60, FR_LEAP2, FR_LEAP3, FR_LEAP4, FR_LEAP5, FR_LEAP6, FR_LEAP7, FR_LEAP8, FR_LEAP9};

	enum {FR_STAND1 = 69, FR_STAND2, FR_STAND3, FR_STAND4, FR_STAND5, FR_STAND6, FR_STAND7, FR_STAND8, FR_STAND9};

	enum {FR_WALK1 = 78, FR_WALK2, FR_WALK3, FR_WALK4, FR_WALK5, FR_WALK6, FR_WALK7, FR_WALK8};
}

void dog_leap1();
void dog_run1();

/*
================
dog_bite

================
*/
void dog_bite()
{
vector	delta;
float 	ldmg;

	if (self->enemy == world)
		return;

	ai_charge(10);

	if (!CanDamage (self->enemy, self))
		return;

	delta = self->enemy->origin - self->origin;

	if (vlen(delta) > 100)
		return;

	ldmg = (random() + random() + random()) * PR_IDMON_DOG_BITEDMG;
	TF_T_Damage (self->enemy, self, self, ldmg, PR_TF_TD_NOTTEAM, PR_TF_TD_MELEE);
}

void Dog_JumpTouch()
{
	float	ldmg;

	if (self->health <= 0)
		return;

	if (other->takedamage)
	{
		if ( vlen(self->velocity) > 300 )
		{
			ldmg = PR_IDMON_DOG_JUMPDMG + 10*random();
			TF_T_Damage (other, self, self, ldmg, PR_TF_TD_NOTTEAM, PR_TF_TD_MELEE);
		}
	}

	if (!checkbottom(self))
	{
		if (self->flags & PR_FL_ONGROUND)
		{	// jump randomly to not get hung up
//dprint ("popjump\n");
	self->touch = SUB_Null;
	self->think = dog_leap1;
	self->nextthink = time + 0.1;

//			self.velocity_x = (random() - 0.5) * 600;
//			self.velocity_y = (random() - 0.5) * 600;
//			self.velocity_z = 200;
//			self.flags = self.flags - #FL_ONGROUND;
		}
		return;	// not on ground yet
	}

	self->touch = SUB_Null;
	self->think = dog_run1;
	self->nextthink = time + 0.1;
}


void dog_stand1() {FRAME_STATE(DogFrames::FR_STAND1, dog_stand2); COOP_Idle();}
void dog_stand2() {FRAME_STATE(DogFrames::FR_STAND2, dog_stand3);}
void dog_stand3() {FRAME_STATE(DogFrames::FR_STAND3, dog_stand4);}
void dog_stand4() {FRAME_STATE(DogFrames::FR_STAND4, dog_stand5);}
void dog_stand5() {FRAME_STATE(DogFrames::FR_STAND5, dog_stand6); COOP_Idle();}
void dog_stand6() {FRAME_STATE(DogFrames::FR_STAND6, dog_stand7);}
void dog_stand7() {FRAME_STATE(DogFrames::FR_STAND7, dog_stand8);}
void dog_stand8() {FRAME_STATE(DogFrames::FR_STAND8, dog_stand9);}
void dog_stand9() {FRAME_STATE(DogFrames::FR_STAND9, dog_stand1);}

void dog_walk1() {
FRAME_STATE(DogFrames::FR_WALK1, dog_walk2);
if (random() < 0.2)
	sound (self, PR_CHAN_VOICE, "dog/idle.wav", 1, PR_ATTN_IDLE);
COOP_Walk(8);}
void dog_walk2() {FRAME_STATE(DogFrames::FR_WALK2, dog_walk3); COOP_Walk(8);}
void dog_walk3() {FRAME_STATE(DogFrames::FR_WALK3, dog_walk4); COOP_Walk(8);}
void dog_walk4() {FRAME_STATE(DogFrames::FR_WALK4, dog_walk5); COOP_Walk(8);}
void dog_walk5() {FRAME_STATE(DogFrames::FR_WALK5, dog_walk6); COOP_Walk(8);}
void dog_walk6() {FRAME_STATE(DogFrames::FR_WALK6, dog_walk7); COOP_Walk(8);}
void dog_walk7() {FRAME_STATE(DogFrames::FR_WALK7, dog_walk8); COOP_Walk(8);}
void dog_walk8() {FRAME_STATE(DogFrames::FR_WALK8, dog_walk1); COOP_Walk(8);}

void dog_run1() {
FRAME_STATE(DogFrames::FR_RUN1, dog_run2);
if (random() < 0.2)
	sound (self, PR_CHAN_VOICE, "dog/idle.wav", 1, PR_ATTN_IDLE);
COOP_Run(16);}
void dog_run2() {FRAME_STATE(DogFrames::FR_RUN2, dog_run3); COOP_Run(32);}
void dog_run3() {FRAME_STATE(DogFrames::FR_RUN3, dog_run4); COOP_Run(32);}
void dog_run4() {FRAME_STATE(DogFrames::FR_RUN4, dog_run5); COOP_Run(20);}
void dog_run5() {FRAME_STATE(DogFrames::FR_RUN5, dog_run6); COOP_Run(64);}
void dog_run6() {FRAME_STATE(DogFrames::FR_RUN6, dog_run7); COOP_Run(32);}
void dog_run7() {FRAME_STATE(DogFrames::FR_RUN7, dog_run8); COOP_Run(16);}
void dog_run8() {FRAME_STATE(DogFrames::FR_RUN8, dog_run9); COOP_Run(32);}
void dog_run9() {FRAME_STATE(DogFrames::FR_RUN9, dog_run10); COOP_Run(32);}
void dog_run10() {FRAME_STATE(DogFrames::FR_RUN10, dog_run11); COOP_Run(20);}
void dog_run11() {FRAME_STATE(DogFrames::FR_RUN11, dog_run12); COOP_Run(64);}
void dog_run12() {FRAME_STATE(DogFrames::FR_RUN12, dog_run1); COOP_Run(32);}

void dog_atta1() {FRAME_STATE(DogFrames::FR_ATTACK1, dog_atta2); ai_charge(10);}
void dog_atta2() {FRAME_STATE(DogFrames::FR_ATTACK2, dog_atta3); ai_charge(10);}
void dog_atta3() {FRAME_STATE(DogFrames::FR_ATTACK3, dog_atta4); ai_charge(10);}
void dog_atta4() {
FRAME_STATE(DogFrames::FR_ATTACK4, dog_atta5);
sound (self, PR_CHAN_VOICE, "dog/dattack1.wav", 1, PR_ATTN_NORM);
dog_bite();}
void dog_atta5() {FRAME_STATE(DogFrames::FR_ATTACK5, dog_atta6); ai_charge(10);}
void dog_atta6() {FRAME_STATE(DogFrames::FR_ATTACK6, dog_atta7); ai_charge(10);}
void dog_atta7() {FRAME_STATE(DogFrames::FR_ATTACK7, dog_atta8); ai_charge(10);}
void dog_atta8() {FRAME_STATE(DogFrames::FR_ATTACK8, dog_run1); ai_charge(10);}

void dog_leap1() {FRAME_STATE(DogFrames::FR_LEAP1, dog_leap2); COOP_Turn(); Attack_Finished( 2 );}
void dog_leap2()
{
	FRAME_STATE(DogFrames::FR_LEAP2, dog_leap3);
	COOP_Turn();

	self->touch = Dog_JumpTouch;
	makevectors (self->angles);
	self->origin[Z] = self->origin[Z] + 1;
	self->velocity = v_forward * PR_IDMON_DOG_JUMPVEL + V({0, 0, 200});
	if (self->flags & PR_FL_ONGROUND)
		self->flags = self->flags - PR_FL_ONGROUND;
}

void dog_leap3() {FRAME_STATE(DogFrames::FR_LEAP3, dog_leap4);}
void dog_leap4() {FRAME_STATE(DogFrames::FR_LEAP4, dog_leap5);}
void dog_leap5() {FRAME_STATE(DogFrames::FR_LEAP5, dog_leap6);}
void dog_leap6() {FRAME_STATE(DogFrames::FR_LEAP6, dog_leap7);}
void dog_leap7() {FRAME_STATE(DogFrames::FR_LEAP7, dog_leap8);}
void dog_leap8() {FRAME_STATE(DogFrames::FR_LEAP8, dog_leap9);}
void dog_leap9() {FRAME_STATE(DogFrames::FR_LEAP9, dog_leap9);}

void dog_pain1() {FRAME_STATE(DogFrames::FR_PAIN1, dog_pain2);}
void dog_pain2() {FRAME_STATE(DogFrames::FR_PAIN2, dog_pain3);}
void dog_pain3() {FRAME_STATE(DogFrames::FR_PAIN3, dog_pain4);}
void dog_pain4() {FRAME_STATE(DogFrames::FR_PAIN4, dog_pain5);}
void dog_pain5() {FRAME_STATE(DogFrames::FR_PAIN5, dog_pain6);}
void dog_pain6() {FRAME_STATE(DogFrames::FR_PAIN6, dog_run1);}

void dog_painb1() {FRAME_STATE(DogFrames::FR_PAINB1, dog_painb2);}
void dog_painb2() {FRAME_STATE(DogFrames::FR_PAINB2, dog_painb3);}
void dog_painb3() {FRAME_STATE(DogFrames::FR_PAINB3, dog_painb4); ai_pain(4);}
void dog_painb4() {FRAME_STATE(DogFrames::FR_PAINB4, dog_painb5); ai_pain(12);}
void dog_painb5() {FRAME_STATE(DogFrames::FR_PAINB5, dog_painb6); ai_pain(12);}
void dog_painb6() {FRAME_STATE(DogFrames::FR_PAINB6, dog_painb7); ai_pain(2);}
void dog_painb7() {FRAME_STATE(DogFrames::FR_PAINB7, dog_painb8);}
void dog_painb8() {FRAME_STATE(DogFrames::FR_PAINB8, dog_painb9); ai_pain(4);}
void dog_painb9() {FRAME_STATE(DogFrames::FR_PAINB9, dog_painb10);}
void dog_painb10() {FRAME_STATE(DogFrames::FR_PAINB10, dog_painb11); ai_pain(10);}
void dog_painb11() {FRAME_STATE(DogFrames::FR_PAINB11, dog_painb12);}
void dog_painb12() {FRAME_STATE(DogFrames::FR_PAINB12, dog_painb13);}
void dog_painb13() {FRAME_STATE(DogFrames::FR_PAINB13, dog_painb14);}
void dog_painb14() {FRAME_STATE(DogFrames::FR_PAINB14, dog_painb15);}
void dog_painb15() {FRAME_STATE(DogFrames::FR_PAINB15, dog_painb16);}
void dog_painb16() {FRAME_STATE(DogFrames::FR_PAINB16, dog_run1);}

void dog_pain(entity attacker, float damage)
{
	// if it's in hard mode, don't do pain as often
	if (skill >= 2 && random() > 0.3)
		return;

	sound (self, PR_CHAN_VOICE, "dog/dpain1.wav", 1, PR_ATTN_NORM);

	if (random() > 0.5)
		dog_pain1 ();
	else
		dog_painb1 ();
}

void dog_die1() {FRAME_STATE(DogFrames::FR_DEATH1, dog_die2);}
void dog_die2() {FRAME_STATE(DogFrames::FR_DEATH2, dog_die3);}
void dog_die3() {FRAME_STATE(DogFrames::FR_DEATH3, dog_die4);}
void dog_die4() {FRAME_STATE(DogFrames::FR_DEATH4, dog_die5);}
void dog_die5() {FRAME_STATE(DogFrames::FR_DEATH5, dog_die6);}
void dog_die6() {FRAME_STATE(DogFrames::FR_DEATH6, dog_die7);}
void dog_die7() {FRAME_STATE(DogFrames::FR_DEATH7, dog_die8);}
void dog_die8() {FRAME_STATE(DogFrames::FR_DEATH8, dog_die9);}
void dog_die9() {FRAME_STATE(DogFrames::FR_DEATH9, dog_die9); MonsterCorpse();}

void dog_dieb1() {FRAME_STATE(DogFrames::FR_DEATHB1, dog_dieb2);}
void dog_dieb2() {FRAME_STATE(DogFrames::FR_DEATHB2, dog_dieb3);}
void dog_dieb3() {FRAME_STATE(DogFrames::FR_DEATHB3, dog_dieb4);}
void dog_dieb4() {FRAME_STATE(DogFrames::FR_DEATHB4, dog_dieb5);}
void dog_dieb5() {FRAME_STATE(DogFrames::FR_DEATHB5, dog_dieb6);}
void dog_dieb6() {FRAME_STATE(DogFrames::FR_DEATHB6, dog_dieb7);}
void dog_dieb7() {FRAME_STATE(DogFrames::FR_DEATHB7, dog_dieb8);}
void dog_dieb8() {FRAME_STATE(DogFrames::FR_DEATHB8, dog_dieb9);}
void dog_dieb9() {FRAME_STATE(DogFrames::FR_DEATHB9, dog_dieb9); MonsterCorpse();}


void dog_die()
{
// check for gib
	if (self->health < -35)
	{
		sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_NORM);
		ThrowMonsterHead ("progs/h_dog.mdl", self->health);
		ThrowGib("progs/gib3.mdl", -40, PR_TRUE, 0, PR_GIB1_KGS, PR_FALSE);
		ThrowGib("progs/gib3.mdl", -40, PR_TRUE, 0, PR_GIB1_KGS, PR_FALSE);
		ThrowGib("progs/gib3.mdl", -40, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
		remove (self);
		return;
	}

// regular death
	sound (self, PR_CHAN_VOICE, "dog/ddeath.wav", 1, PR_ATTN_NORM);
	self->solid = PR_SOLID_NOT;

	if (random() > 0.5)
		dog_die1 ();
	else
		dog_dieb1 ();
}

//============================================================================

/*
==============
CheckDogMelee

Returns TRUE if a melee attack would hit right now
==============
*/
float CheckDogMelee()
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
CheckDogJump

==============
*/
float CheckDogJump()
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

	if (d < 80)
		return PR_FALSE;

	if (d > 150)
		return PR_FALSE;

	if (!FacingIdeal() || !(self->flags & PR_FL_ONGROUND))
		return PR_FALSE;

	return PR_TRUE;
}

float DogCheckAttack()
{
	enemy_range = range (self->enemy);

// if close enough for slashing, go for it
	if (CheckDogMelee ())
	{
		self->attack_state = PR_AS_MELEE;
		return PR_TRUE;
	}

	if (CheckDogJump ())
	{
		self->attack_state = PR_AS_MISSILE;
		return PR_TRUE;
	}

	return PR_FALSE;
}


//===========================================================================

float DogOnSight()
{
	sound (self, PR_CHAN_VOICE, "dog/dsight.wav", 1, PR_ATTN_NORM);
	return PR_TRUE;
}


/*QUAKED monster_dog (1 0 0) (-32 -32 -24) (32 32 40) Ambush

*/
void monster_dog()
{
	if ( deathmatch ) {
		remove( self );
		return;
	}

	self->classname = "idmonster_dog";

	if (!server_spawned) {
		precache_model ("progs/h_dog.mdl");
		precache_model ("progs/dog.mdl");

		precache_sound ("dog/dattack1.wav");
		precache_sound ("dog/ddeath.wav");
		precache_sound ("dog/dpain1.wav");
		precache_sound ("dog/dsight.wav");
		precache_sound ("dog/idle.wav");
	}

	self->solid = PR_SOLID_SLIDEBOX;
	self->movetype = PR_MOVETYPE_STEP;

	self->mdl = "progs/h_dog.mdl";
	setmodel (self, "progs/dog.mdl");

	setsize (self, V({-32, -32, -24}), V({32, 32, 40}));
	self->max_health = self->health = PR_IDMON_DOG_HEALTH;

	self->th_stand = dog_stand1;
	self->th_walk = dog_walk1;
	self->th_run = dog_run1;
	self->th_pain = dog_pain;
	self->th_die = dog_die;
	self->th_melee = dog_atta1;
	self->th_missile = dog_leap1;


	self->netname = "fido";
//	self.netname = COOP_GenerateMonsterName (self);

	self->monsterweight = PR_MWEIGHT_SMALL;
	self->monsterflags = PR_MFLAG_BITER;

	// monsters have a team of their own
	if ( !self->team_no )
		self->team_no = PR_TEAMNO_MONSTERS;
	self->PR_monsterflag = PR_STRFLAG_MONSTER;

	self->is_connected = PR_TRUE;

	// custom fields not required
	self->PR_npc_speedfactor = 2.8;
	self->OnPreAttack = DogCheckAttack;
	self->OnNoticeEnemy = DogOnSight;
	self->PR_npc_spawnfunc = monster_dog;
	COOP_SetLives ();

	self->PR_npc_firemult = 1.25;

	coop_walkmonster_start();
}

} // END namespace Progs

#endif
