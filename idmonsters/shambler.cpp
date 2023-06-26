/*
	shambler.qc
*/

#include "progs.h"

#ifndef PR_COOP_MODE_ENHANCED
//#pragma DONT_COMPILE_THIS_FILE
#else

#include "idmonsters/shambler.h"
#include "idmonsters/common.h"
#include "idmonsters/coop_monsters.h"
#include "coop_defs.h"
#include "ofndefs.h"
#include "combat.h"
#include "qw.h"
#include "weapons.h"
#include "fight.h"
#include "ai.h"
#include "monsters.h"
#include "tsoldier.h"
#include "warlock.h"

namespace Progs {

#define PR_IDMON_SHAMBLER_HEALTH		4000
#define PR_IDMON_SHAMBLER_LIGHTNINGDMG	30		// was 25
#define PR_IDMON_SHAMBLER_CLAWDMG	125
#define PR_IDMON_SHAMBLER_SMASHDMG	220

#define PR__sham_healtime			lip

/*
==============================================================================

SHAMBLER

==============================================================================
*/

//$cd id1/models/shams
//$origin 0 0 24
//$base base
//$skin base

namespace ShamblerFrames
{
	enum {FR_STAND1, FR_STAND2, FR_STAND3, FR_STAND4, FR_STAND5, FR_STAND6, FR_STAND7, FR_STAND8, FR_STAND9};
	enum {FR_STAND10 = 9, FR_STAND11, FR_STAND12, FR_STAND13, FR_STAND14, FR_STAND15, FR_STAND16, FR_STAND17};

	enum {FR_WALK1 = 17, FR_WALK2, FR_WALK3, FR_WALK4, FR_WALK5, FR_WALK6, FR_WALK7};
	enum {FR_WALK8 = 24, FR_WALK9, FR_WALK10, FR_WALK11, FR_WALK12};

	enum {FR_RUN1 = 29, FR_RUN2, FR_RUN3, FR_RUN4, FR_RUN5, FR_RUN6};

	enum {FR_SMASH1 = 35, FR_SMASH2, FR_SMASH3, FR_SMASH4, FR_SMASH5, FR_SMASH6, FR_SMASH7};
	enum {FR_SMASH8 = 42, FR_SMASH9, FR_SMASH10, FR_SMASH11, FR_SMASH12};

	enum {FR_SWINGR1 = 47, FR_SWINGR2, FR_SWINGR3, FR_SWINGR4, FR_SWINGR5};
	enum {FR_SWINGR6 = 52, FR_SWINGR7, FR_SWINGR8, FR_SWINGR9};

	enum {FR_SWINGL1 = 56, FR_SWINGL2, FR_SWINGL3, FR_SWINGL4, FR_SWINGL5};
	enum {FR_SWINGL6 = 61, FR_SWINGL7, FR_SWINGL8, FR_SWINGL9};

	enum {FR_MAGIC1 = 65, FR_MAGIC2, FR_MAGIC3, FR_MAGIC4, FR_MAGIC5};
	enum {FR_MAGIC6 = 70, FR_MAGIC7, FR_MAGIC8, FR_MAGIC9, FR_MAGIC10, FR_MAGIC11, FR_MAGIC12};

	enum {FR_PAIN1 = 77, FR_PAIN2, FR_PAIN3, FR_PAIN4, FR_PAIN5, FR_PAIN6};

	enum {FR_DEATH1 = 83, FR_DEATH2, FR_DEATH3, FR_DEATH4, FR_DEATH5, FR_DEATH6};
	enum {FR_DEATH7 = 89, FR_DEATH8, FR_DEATH9, FR_DEATH10, FR_DEATH11};
}

void _ShamHeal()
{
	float hp;

	if (self->health >= self->max_health || self->PR__sham_healtime > time)
		return;

	self->PR__sham_healtime = time + 5;

	hp = self->max_health - self->health;

	if (hp > 10)
		hp = hp / (8 + random()*10);
	else if (hp < 1)
		hp = 1;

	// round the heal amount up
	hp = ceil (hp);
	self->health = self->health + hp;

	// heal leg wounds
	self->PR_npc_speedfactor = 2.8;

	sound(self, PR_CHAN_ITEM, "items/r_item1.wav", 1, PR_ATTN_NORM);
}

void _sham_stand1() {FRAME_STATE(ShamblerFrames::FR_STAND1, _sham_stand2); COOP_Idle();}
void _sham_stand2() {FRAME_STATE(ShamblerFrames::FR_STAND2, _sham_stand3);}
void _sham_stand3() {FRAME_STATE(ShamblerFrames::FR_STAND3, _sham_stand4);}
void _sham_stand4() {FRAME_STATE(ShamblerFrames::FR_STAND4, _sham_stand5);}
void _sham_stand5() {FRAME_STATE(ShamblerFrames::FR_STAND5, _sham_stand6);}
void _sham_stand6() {FRAME_STATE(ShamblerFrames::FR_STAND6, _sham_stand7); COOP_Idle();}
void _sham_stand7() {FRAME_STATE(ShamblerFrames::FR_STAND7, _sham_stand8);}
void _sham_stand8() {FRAME_STATE(ShamblerFrames::FR_STAND8, _sham_stand9);}
void _sham_stand9() {FRAME_STATE(ShamblerFrames::FR_STAND9, _sham_stand10);}
void _sham_stand10() {FRAME_STATE(ShamblerFrames::FR_STAND10, _sham_stand11);}
void _sham_stand11() {FRAME_STATE(ShamblerFrames::FR_STAND11, _sham_stand12); COOP_Idle();}
void _sham_stand12() {FRAME_STATE(ShamblerFrames::FR_STAND12, _sham_stand13);}
void _sham_stand13() {FRAME_STATE(ShamblerFrames::FR_STAND13, _sham_stand14);}
void _sham_stand14() {FRAME_STATE(ShamblerFrames::FR_STAND14, _sham_stand15);}
void _sham_stand15() {FRAME_STATE(ShamblerFrames::FR_STAND15, _sham_stand16); COOP_Idle();}
void _sham_stand16() {FRAME_STATE(ShamblerFrames::FR_STAND16, _sham_stand17);}
void _sham_stand17() {
	FRAME_STATE(ShamblerFrames::FR_STAND17, _sham_stand1);
	_ShamHeal ();
}

void _sham_walk1() {FRAME_STATE(ShamblerFrames::FR_WALK1, _sham_walk2); COOP_Walk(10);}
void _sham_walk2() {FRAME_STATE(ShamblerFrames::FR_WALK2, _sham_walk3); COOP_Walk(9);}
void _sham_walk3() {FRAME_STATE(ShamblerFrames::FR_WALK3, _sham_walk4); COOP_Walk(9);}
void _sham_walk4() {FRAME_STATE(ShamblerFrames::FR_WALK4, _sham_walk5); COOP_Walk(5);}
void _sham_walk5() {FRAME_STATE(ShamblerFrames::FR_WALK5, _sham_walk6); COOP_Walk(6);}
void _sham_walk6() {FRAME_STATE(ShamblerFrames::FR_WALK6, _sham_walk7); COOP_Walk(12);}
void _sham_walk7() {FRAME_STATE(ShamblerFrames::FR_WALK7, _sham_walk8); COOP_Walk(8);}
void _sham_walk8() {FRAME_STATE(ShamblerFrames::FR_WALK8, _sham_walk9); COOP_Walk(3);}
void _sham_walk9() {FRAME_STATE(ShamblerFrames::FR_WALK9, _sham_walk10); COOP_Walk(13);}
void _sham_walk10() {FRAME_STATE(ShamblerFrames::FR_WALK10, _sham_walk11); COOP_Walk(9);}
void _sham_walk11() {FRAME_STATE(ShamblerFrames::FR_WALK11, _sham_walk12); COOP_Walk(7);}
void _sham_walk12() {FRAME_STATE(ShamblerFrames::FR_WALK12, _sham_walk1); COOP_Walk(7);
	if (random() > 0.8)
		sound (self, PR_CHAN_VOICE, "shambler/sidle.wav", 1, PR_ATTN_IDLE);
	_ShamHeal ();
}

void _sham_throw();
void _sham_run1() {FRAME_STATE(ShamblerFrames::FR_RUN1, _sham_run2); COOP_Run(20);}
void _sham_run2() {FRAME_STATE(ShamblerFrames::FR_RUN2, _sham_run3); COOP_Run(24);}
void _sham_run3() {FRAME_STATE(ShamblerFrames::FR_RUN3, _sham_run4); COOP_Run(20);}
void _sham_run4() {FRAME_STATE(ShamblerFrames::FR_RUN4, _sham_run5); COOP_Run(20);}
void _sham_run5() {FRAME_STATE(ShamblerFrames::FR_RUN5, _sham_run6); COOP_Run(24);}
void _sham_run6() {
FRAME_STATE(ShamblerFrames::FR_RUN6, _sham_run1);
if (random() > 0.8)
	sound (self, PR_CHAN_VOICE, "shambler/sidle.wav", 1, PR_ATTN_IDLE);

	// check if things are in the way
	if ( random() < 0.25 ) {
		entity head;

		head = findradius( self->origin, 128 );
		while ( head  != world) {
			if ( !( head->flags & PR_FL_ITEM ) )
			if ( !head->takedamage )
			if ( visible( head ) )
			if ( head->solid != PR_SOLID_BSP && head->movetype != PR_MOVETYPE_NONE && infront( head ) ) {
				_sham_throw();
				return;
			}

			if ( head->touch == NailGrenadeTouch || head->think == NailGrenadeExplode ) {
				_sham_throw();
				return;
			}
			head = head->chain;
		}
	}

	// only run if we're not wacking something out of the way
	COOP_Run(20);
}

void _sham_smash1() {
FRAME_STATE(ShamblerFrames::FR_SMASH1, _sham_smash2);
sound (self, PR_CHAN_VOICE, "shambler/melee1.wav", 1, PR_ATTN_NORM);
ai_charge(2);}
void _sham_smash2() {FRAME_STATE(ShamblerFrames::FR_SMASH2, _sham_smash3); ai_charge(6);}
void _sham_smash3() {FRAME_STATE(ShamblerFrames::FR_SMASH3, _sham_smash4); ai_charge(6);}
void _sham_smash4() {FRAME_STATE(ShamblerFrames::FR_SMASH4, _sham_smash5); ai_charge(5);}
void _sham_smash5() {FRAME_STATE(ShamblerFrames::FR_SMASH5, _sham_smash6); ai_charge(4);}
void _sham_smash6() {FRAME_STATE(ShamblerFrames::FR_SMASH6, _sham_smash7); ai_charge(1);}
void _sham_smash7() {FRAME_STATE(ShamblerFrames::FR_SMASH7, _sham_smash8); ai_charge(0);}
void _sham_smash8() {FRAME_STATE(ShamblerFrames::FR_SMASH8, _sham_smash9); ai_charge(0);}
void _sham_smash9() {FRAME_STATE(ShamblerFrames::FR_SMASH9, _sham_smash10); ai_charge(0);}
void _sham_smash10() {
FRAME_STATE(ShamblerFrames::FR_SMASH10, _sham_smash11);
vector	delta;
float 	ldmg;

	if (self->enemy == world)
		return;
	ai_charge(0);

	delta = self->enemy->origin - self->origin;

	if (vlen(delta) > 175 || !visible(self->enemy))
		return;
	if (!CanDamage (self->enemy, self))
		return;

	ldmg = (random() + random() + random()) * PR_IDMON_SHAMBLER_SMASHDMG;
	TF_T_Damage (self->enemy, self, self, ldmg, PR_TF_TD_NOTTEAM, PR_TF_TD_MELEE);
	sound (self, PR_CHAN_VOICE, "shambler/smack.wav", 1, PR_ATTN_NORM);

	SpawnMeatSpray (self->origin + v_forward*16, crandom() * 100 * v_right);
	SpawnMeatSpray (self->origin + v_forward*16, crandom() * 100 * v_right);
}
void _sham_smash11() {FRAME_STATE(ShamblerFrames::FR_SMASH11, _sham_smash12); ai_charge(5);}
void _sham_smash12() {FRAME_STATE(ShamblerFrames::FR_SMASH12, _sham_run1); ai_charge(4);}

void _sham_swingr1();

void _ShamClaw(float side)
{
vector	delta;
float 	ldmg;

	if (self->enemy == world)
		return;
	ai_charge(10);

	delta = self->enemy->origin - self->origin;

	if (vlen(delta) > 175 || !visible(self->enemy))
		return;

	ldmg = (random() + random() + random()) * PR_IDMON_SHAMBLER_CLAWDMG;
	TF_T_Damage (self->enemy, self, self, ldmg, PR_TF_TD_NOTTEAM, PR_TF_TD_MELEE);
	sound (self, PR_CHAN_VOICE, "shambler/smack.wav", 1, PR_ATTN_NORM);

	if (side)
	{
		makevectors (self->angles);
		SpawnMeatSpray (self->origin + v_forward*16, side * v_right);
	}
}

void _sham_swingl1() {
FRAME_STATE(ShamblerFrames::FR_SWINGL1, _sham_swingl2);
sound (self, PR_CHAN_VOICE, "shambler/melee2.wav", 1, PR_ATTN_NORM);
ai_charge(5);}
void _sham_swingl2() {FRAME_STATE(ShamblerFrames::FR_SWINGL2, _sham_swingl3); ai_charge(3);}
void _sham_swingl3() {FRAME_STATE(ShamblerFrames::FR_SWINGL3, _sham_swingl4); ai_charge(7);}
void _sham_swingl4() {FRAME_STATE(ShamblerFrames::FR_SWINGL4, _sham_swingl5); ai_charge(3);}
void _sham_swingl5() {FRAME_STATE(ShamblerFrames::FR_SWINGL5, _sham_swingl6); ai_charge(7);}
void _sham_swingl6() {FRAME_STATE(ShamblerFrames::FR_SWINGL6, _sham_swingl7); ai_charge(9);}
void _sham_swingl7() {FRAME_STATE(ShamblerFrames::FR_SWINGL7, _sham_swingl8); ai_charge(5); _ShamClaw(250);}
void _sham_swingl8() {FRAME_STATE(ShamblerFrames::FR_SWINGL8, _sham_swingl9); ai_charge(4);}
void _sham_swingl9() {
FRAME_STATE(ShamblerFrames::FR_SWINGL9, _sham_run1);
ai_charge(8);
if (random()<0.5)
	self->think = _sham_swingr1;
}

void _sham_swingr1() {
FRAME_STATE(ShamblerFrames::FR_SWINGR1, _sham_swingr2);
sound (self, PR_CHAN_VOICE, "shambler/melee1.wav", 1, PR_ATTN_NORM);
ai_charge(1);}
void _sham_swingr2() {FRAME_STATE(ShamblerFrames::FR_SWINGR2, _sham_swingr3); ai_charge(8);}
void _sham_swingr3() {FRAME_STATE(ShamblerFrames::FR_SWINGR3, _sham_swingr4); ai_charge(14);}
void _sham_swingr4() {FRAME_STATE(ShamblerFrames::FR_SWINGR4, _sham_swingr5); ai_charge(7);}
void _sham_swingr5() {FRAME_STATE(ShamblerFrames::FR_SWINGR5, _sham_swingr6); ai_charge(3);}
void _sham_swingr6() {FRAME_STATE(ShamblerFrames::FR_SWINGR6, _sham_swingr7); ai_charge(6);}
void _sham_swingr7() {FRAME_STATE(ShamblerFrames::FR_SWINGR7, _sham_swingr8); ai_charge(6); _ShamClaw(-250);}
void _sham_swingr8() {FRAME_STATE(ShamblerFrames::FR_SWINGR8, _sham_swingr9); ai_charge(3);}
void _sham_swingr9() {FRAME_STATE(ShamblerFrames::FR_SWINGR9, _sham_run1); ai_charge(1);
ai_charge(10);
if (random()<0.5)
	self->think = _sham_swingl1;
}

void _sham_melee()
{
	float chance;

	chance = random();
	if (chance > 0.6 || self->health == PR_IDMON_SHAMBLER_HEALTH)
		_sham_smash1 ();
	else if (chance > 0.3)
		_sham_swingr1 ();
	else
		_sham_swingl1 ();
}


//============================================================================


/*
=============
_ShamThrowObjects

Knocks objects out of the way
=============
*/
void _ShamThrowObjects() {
	entity	head;
	vector	dir;

	head = findradius( self->origin, 128 );
	while ( head  != world) {
		if ( !( head->flags & PR_FL_ITEM ) )
		if ( visible( head ) )
		if ( head->solid != PR_SOLID_BSP && head->movetype != PR_MOVETYPE_NONE && infront( head ) ) {
			if ( head->flags & PR_FL_ONGROUND ) {
				head->flags = head->flags - PR_FL_ONGROUND;
				head->velocity[Z] = head->velocity[Z] + ( ( random() + 1 ) * 300 );
			}

			dir = normalize( head->origin - self->origin );
			head->velocity = head->velocity + dir * ( ( random() + 1 ) * 500 );
		}

		head = head->chain;
	}
}

void _sham_throwl1() {
FRAME_STATE(ShamblerFrames::FR_SWINGL1, _sham_throwl2);
sound (self, PR_CHAN_VOICE, "shambler/melee2.wav", 1, PR_ATTN_NORM);}
void _sham_throwl2() {FRAME_STATE(ShamblerFrames::FR_SWINGL2, _sham_throwl3);}
void _sham_throwl3() {FRAME_STATE(ShamblerFrames::FR_SWINGL3, _sham_throwl4);}
void _sham_throwl4() {FRAME_STATE(ShamblerFrames::FR_SWINGL4, _sham_throwl5);}
void _sham_throwl5() {FRAME_STATE(ShamblerFrames::FR_SWINGL5, _sham_throwl6);}
void _sham_throwl6() {FRAME_STATE(ShamblerFrames::FR_SWINGL6, _sham_throwl7);}
void _sham_throwl7() {FRAME_STATE(ShamblerFrames::FR_SWINGL7, _sham_throwl8); _ShamThrowObjects();}
void _sham_throwl8() {FRAME_STATE(ShamblerFrames::FR_SWINGL8, _sham_throwl9);}
void _sham_throwl9() {FRAME_STATE(ShamblerFrames::FR_SWINGL9, _sham_run1);}

void _sham_throwr1() {
FRAME_STATE(ShamblerFrames::FR_SWINGR1, _sham_throwr2);
sound (self, PR_CHAN_VOICE, "shambler/melee1.wav", 1, PR_ATTN_NORM);}
void _sham_throwr2() {FRAME_STATE(ShamblerFrames::FR_SWINGR2, _sham_throwr3);}
void _sham_throwr3() {FRAME_STATE(ShamblerFrames::FR_SWINGR3, _sham_throwr4);}
void _sham_throwr4() {FRAME_STATE(ShamblerFrames::FR_SWINGR4, _sham_throwr5);}
void _sham_throwr5() {FRAME_STATE(ShamblerFrames::FR_SWINGR5, _sham_throwr6);}
void _sham_throwr6() {FRAME_STATE(ShamblerFrames::FR_SWINGR6, _sham_throwr7);}
void _sham_throwr7() {FRAME_STATE(ShamblerFrames::FR_SWINGR7, _sham_throwr8); _ShamThrowObjects();}
void _sham_throwr8() {FRAME_STATE(ShamblerFrames::FR_SWINGR8, _sham_throwr9);}
void _sham_throwr9() {FRAME_STATE(ShamblerFrames::FR_SWINGR9, _sham_run1);}

void _sham_throw()
{
	if (random() < 0.5)
		_sham_throwr1 ();
	else
		_sham_throwl1 ();
}


//============================================================================

void _CastLightning()
{
	vector	org, dir;

	muzzleflash ();

	COOP_Turn ();

	org = self->origin + V({0, 0, 40});

//	dir = self.enemy.origin + '0 0 16' - org;
	dir = (self->enemy->origin + self->PR_npc_enemyoffset) - org;
	dir = normalize (dir);

	// self.origin or org?
	traceline (org, org + dir*4096, PR_TRUE, self);

	WriteByte (PR_MSG_MULTICAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_MULTICAST, PR_TE_LIGHTNING1);
	WriteEntity (PR_MSG_MULTICAST, self);
	WriteCoord (PR_MSG_MULTICAST, org[X]);
	WriteCoord (PR_MSG_MULTICAST, org[Y]);
	WriteCoord (PR_MSG_MULTICAST, org[Z]);
	WriteCoord (PR_MSG_MULTICAST, trace_endpos[X]);
	WriteCoord (PR_MSG_MULTICAST, trace_endpos[Y]);
	WriteCoord (PR_MSG_MULTICAST, trace_endpos[Z]);
	multicast (org, PR_MULTICAST_PHS);

	LightningDamage (org, trace_endpos, self, PR_IDMON_SHAMBLER_LIGHTNINGDMG);
}

void _sham_magic1() {FRAME_STATE(ShamblerFrames::FR_MAGIC1, _sham_magic2); COOP_Turn();
	sound (self, PR_CHAN_WEAPON, "shambler/sattck1.wav", 1, PR_ATTN_NORM);
}
void _sham_magic2() {FRAME_STATE(ShamblerFrames::FR_MAGIC2, _sham_magic3); COOP_Turn();}
void _sham_magic3() {FRAME_STATE(ShamblerFrames::FR_MAGIC3, _sham_magic4); COOP_Turn();self->nextthink = self->nextthink + 0.2;
entity o;

muzzleflash ();
COOP_Turn();
self->owner = spawn();
o = self->owner;
setmodel (o, "progs/s_light.mdl");
setorigin (o, self->origin);
o->angles = self->angles;
o->nextthink = time + 0.7;
o->think = SUB_Remove;
}
void _sham_magic4()
{
FRAME_STATE(ShamblerFrames::FR_MAGIC4, _sham_magic5);
muzzleflash ();
self->owner->frame = 1;
}
void _sham_magic5()
{
FRAME_STATE(ShamblerFrames::FR_MAGIC5, _sham_magic6);
muzzleflash ();
self->owner->frame = 2;
}
void _sham_magic6()
{
FRAME_STATE(ShamblerFrames::FR_MAGIC6, _sham_magic9);
remove (self->owner);
_CastLightning();
sound (self, PR_CHAN_WEAPON, "shambler/sboom.wav", 1, PR_ATTN_NORM);
}
void _sham_magic9()
{FRAME_STATE(ShamblerFrames::FR_MAGIC9, _sham_magic10); _CastLightning();}
void _sham_magic10()
{FRAME_STATE(ShamblerFrames::FR_MAGIC10, _sham_magic11); _CastLightning();}
void _sham_magic11()
{
	FRAME_STATE(ShamblerFrames::FR_MAGIC11, _sham_magic12);
	_CastLightning();
}
void _sham_magic12() {FRAME_STATE(ShamblerFrames::FR_MAGIC12, _sham_run1);}



void _sham_pain1() {FRAME_STATE(ShamblerFrames::FR_PAIN1, _sham_pain2);}
void _sham_pain2() {FRAME_STATE(ShamblerFrames::FR_PAIN2, _sham_pain3);}
void _sham_pain3() {FRAME_STATE(ShamblerFrames::FR_PAIN3, _sham_pain4);}
void _sham_pain4() {FRAME_STATE(ShamblerFrames::FR_PAIN4, _sham_pain5);}
void _sham_pain5() {FRAME_STATE(ShamblerFrames::FR_PAIN5, _sham_pain6);}
void _sham_pain6() {FRAME_STATE(ShamblerFrames::FR_PAIN6, _sham_run1);}

void _sham_pain(entity attacker, float damage)
{
	sound (self, PR_CHAN_VOICE, "shambler/shurt2.wav", 1, PR_ATTN_NORM);

	if (self->health <= 0)
		return;		// allready dying, don't go into pain frame

	if (random()*400 > damage)
		return;		// didn't flinch

	if (self->pain_finished > time)
		return;
	self->pain_finished = time + 2;

	_sham_pain1 ();
}


//============================================================================

void _sham_death1() {FRAME_STATE(ShamblerFrames::FR_DEATH1, _sham_death2);}
void _sham_death2() {FRAME_STATE(ShamblerFrames::FR_DEATH2, _sham_death3);}
void _sham_death3() {FRAME_STATE(ShamblerFrames::FR_DEATH3, _sham_death4); self->solid = PR_SOLID_NOT;}
void _sham_death4() {FRAME_STATE(ShamblerFrames::FR_DEATH4, _sham_death5);}
void _sham_death5() {FRAME_STATE(ShamblerFrames::FR_DEATH5, _sham_death6);}
void _sham_death6() {FRAME_STATE(ShamblerFrames::FR_DEATH6, _sham_death7);}
void _sham_death7() {FRAME_STATE(ShamblerFrames::FR_DEATH7, _sham_death8);}
void _sham_death8() {FRAME_STATE(ShamblerFrames::FR_DEATH8, _sham_death9);}
void _sham_death9() {FRAME_STATE(ShamblerFrames::FR_DEATH9, _sham_death10);}
void _sham_death10() {FRAME_STATE(ShamblerFrames::FR_DEATH10, _sham_death11);}
void _sham_death11() {FRAME_STATE(ShamblerFrames::FR_DEATH11, _sham_death11); MonsterCorpse();}

void _sham_die()
{
// check for gib
	if (self->health < -60)
	{
		sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_NORM);
		ThrowMonsterHead ("progs/h_shams.mdl", self->health);
		ThrowGib("progs/gib1.mdl", -40, PR_TRUE, 0, PR_GIB1_KGS, PR_FALSE);
		ThrowGib("progs/gib2.mdl", -40, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
		ThrowGib("progs/gib3.mdl", -40, PR_TRUE, 0, PR_GIB3_KGS, PR_FALSE);
		remove (self);
		return;
	}

// regular death
	//sound (self, #CHAN_VOICE, "shambler/sdeath.wav", 1, #ATTN_NORM);
	sound (self, PR_CHAN_VOICE, "shambler/sdeath.wav", 1, PR_ATTN_NONE);
	_sham_death1 ();
}

//============================================================================

float _ShamCheckAttack()
{
	vector	spot1, spot2;
	entity	targ;

	enemy_range = range (self->enemy);
//	enemy_vis = visible (self.enemy);
	enemy_vis = PR_TRUE;

	if (enemy_range == PR_RANGE_MELEE)
	{
		self->attack_state = PR_AS_MELEE;
		return PR_TRUE;
	}

	if (time < self->attack_finished)
		return PR_FALSE;

//	if (!enemy_vis)
//		return #FALSE;

	targ = self->enemy;

	spot1 = self->origin + self->view_ofs;
	spot2 = targ->origin + targ->view_ofs;

	if (vlen(spot1 - spot2) > 4096)
		return PR_FALSE;

// missile attack

	self->attack_state = PR_AS_MISSILE;
	// make shamblers more aggressive against hwguys
	if ( !( self->enemy->flags & PR_FL_CLIENT ) || self->enemy->current_weapon != PR_WEAP_ASSAULT_CANNON )
		Attack_Finished( 1 + 2*random() );
	return PR_TRUE;
}

//============================================================================


float _ShamOnSight()
{
	sound (self, PR_CHAN_VOICE, "shambler/ssight.wav", 1, PR_ATTN_NORM);
	return PR_TRUE;
}


/*QUAKED monster_shambler (1 0 0) (-32 -32 -24) (32 32 64) Ambush
*/
void monster_shambler()
{
	if ( deathmatch ) {
		remove( self );
		return;
	}

	self->classname = "idmonster_shambler";

	if (!server_spawned) {
		precache_model ("progs/shambler.mdl");
		precache_model ("progs/s_light.mdl");
		precache_model ("progs/h_shams.mdl");
		precache_model ("progs/bolt.mdl");

		precache_sound ("shambler/sattck1.wav");
		precache_sound ("shambler/sboom.wav");
		precache_sound ("shambler/sdeath.wav");
		precache_sound ("shambler/shurt2.wav");
		precache_sound ("shambler/sidle.wav");
		precache_sound ("shambler/ssight.wav");
		precache_sound ("shambler/melee1.wav");
		precache_sound ("shambler/melee2.wav");
		precache_sound ("shambler/smack.wav");
	}

	self->PR_npc_attackoffset = V({0, 0, 40});

	self->solid = PR_SOLID_SLIDEBOX;
	self->movetype = PR_MOVETYPE_STEP;

	self->mdl = "progs/h_shams.mdl";
	setmodel (self, "progs/shambler.mdl");

	setsize (self, PR_VEC_HULL2_MIN, PR_VEC_HULL2_MAX);
	self->max_health = self->health = PR_IDMON_SHAMBLER_HEALTH;

	self->th_stand = _sham_stand1;
	self->th_walk = _sham_walk1;
	self->th_run = _sham_run1;
	self->th_die = _sham_die;
	self->th_melee = _sham_melee;
	self->th_missile = _sham_magic1;
	self->th_pain = _sham_pain;


	self->netname = "shambler";

	self->monsterweight = PR_MWEIGHT_LARGE;
	self->monsterflags = PR_MFLAG_NOBREATHING;

	// monsters have a team of their own
	if ( !self->team_no )
		self->team_no = PR_TEAMNO_MONSTERS;
	self->PR_monsterflag = PR_STRFLAG_MONSTER;

	self->is_connected = PR_TRUE;

	self->OnPreAttack = _ShamCheckAttack;
	self->OnNoticeEnemy = _ShamOnSight;
	self->PR_npc_speedfactor = 2.8;
	self->PR_npc_spawnfunc = monster_shambler;
	COOP_SetLives ();

	self->PR_npc_bulletmult		= 0.80;
	self->PR_npc_electricitymult	= 0.25;
	self->PR_npc_spikemult		= 1.60;
	self->PR_npc_meleemult		= 0.50;

	coop_walkmonster_start();
}

} // END namespace Progs

#endif
