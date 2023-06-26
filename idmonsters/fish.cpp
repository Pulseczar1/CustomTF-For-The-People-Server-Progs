/*
	fish.qc
*/

#include "progs.h"

#ifndef PR_COOP_MODE_ENHANCED
//#pragma DONT_COMPILE_THIS_FILE
#else

#include "idmonsters/fish.h"
#include "ofndefs.h"
#include "coop_defs.h"
#include "common.h"
#include "combat.h"
#include "fight.h"
#include "monsters.h"
#include "ai.h"

namespace Progs {

#define PR_IDMON_FISH_HEALTH		240
#define PR_IDMON_FISH_BITEDMG		100

//$cd id1/models/fish
//$origin 0 0 24
//$base base
//$skin skin

namespace FishFrames
{
	enum {FR_ATTACK1, FR_ATTACK2, FR_ATTACK3, FR_ATTACK4, FR_ATTACK5, FR_ATTACK6};
	enum {FR_ATTACK7 = 6, FR_ATTACK8, FR_ATTACK9, FR_ATTACK10, FR_ATTACK11, FR_ATTACK12, FR_ATTACK13};
	enum {FR_ATTACK14 = 13, FR_ATTACK15, FR_ATTACK16, FR_ATTACK17, FR_ATTACK18};

	enum {FR_DEATH1 = 18, FR_DEATH2, FR_DEATH3, FR_DEATH4, FR_DEATH5, FR_DEATH6, FR_DEATH7};
	enum {FR_DEATH8 = 25, FR_DEATH9, FR_DEATH10, FR_DEATH11, FR_DEATH12, FR_DEATH13, FR_DEATH14, FR_DEATH15};
	enum {FR_DEATH16 = 33, FR_DEATH17, FR_DEATH18, FR_DEATH19, FR_DEATH20, FR_DEATH21};

	enum {FR_SWIM1 = 39, FR_SWIM2, FR_SWIM3, FR_SWIM4, FR_SWIM5, FR_SWIM6, FR_SWIM7, FR_SWIM8};
	enum {FR_SWIM9 = 47, FR_SWIM10, FR_SWIM11, FR_SWIM12, FR_SWIM13, FR_SWIM14, FR_SWIM15, FR_SWIM16, FR_SWIM17};
	enum {FR_SWIM18 = 56};

	enum {FR_PAIN1 = 57, FR_PAIN2, FR_PAIN3, FR_PAIN4, FR_PAIN5, FR_PAIN6, FR_PAIN7, FR_PAIN8};
	enum {FR_PAIN9 = 65};
}

void coop_swimmonster_start();

void f_stand1() {FRAME_STATE(FishFrames::FR_SWIM1, f_stand2); COOP_Idle();}
void f_stand2() {FRAME_STATE(FishFrames::FR_SWIM2, f_stand3);}
void f_stand3() {FRAME_STATE(FishFrames::FR_SWIM3, f_stand4);}
void f_stand4() {FRAME_STATE(FishFrames::FR_SWIM4, f_stand5);}
void f_stand5() {FRAME_STATE(FishFrames::FR_SWIM5, f_stand6);}
void f_stand6() {FRAME_STATE(FishFrames::FR_SWIM6, f_stand7);}
void f_stand7() {FRAME_STATE(FishFrames::FR_SWIM7, f_stand8);}
void f_stand8() {FRAME_STATE(FishFrames::FR_SWIM8, f_stand9); COOP_Idle();}
void f_stand9() {FRAME_STATE(FishFrames::FR_SWIM9, f_stand10);}
void f_stand10() {FRAME_STATE(FishFrames::FR_SWIM10, f_stand11);}
void f_stand11() {FRAME_STATE(FishFrames::FR_SWIM11, f_stand12);}
void f_stand12() {FRAME_STATE(FishFrames::FR_SWIM12, f_stand13);}
void f_stand13() {FRAME_STATE(FishFrames::FR_SWIM13, f_stand14);}
void f_stand14() {FRAME_STATE(FishFrames::FR_SWIM14, f_stand15); COOP_Idle();}
void f_stand15() {FRAME_STATE(FishFrames::FR_SWIM15, f_stand16);}
void f_stand16() {FRAME_STATE(FishFrames::FR_SWIM16, f_stand17);}
void f_stand17() {FRAME_STATE(FishFrames::FR_SWIM17, f_stand18);}
void f_stand18() {FRAME_STATE(FishFrames::FR_SWIM18, f_stand1);}

void f_walk1() {FRAME_STATE(FishFrames::FR_SWIM1, f_walk2); COOP_Walk(8);}
void f_walk2() {FRAME_STATE(FishFrames::FR_SWIM2, f_walk3); COOP_Walk(8);}
void f_walk3() {FRAME_STATE(FishFrames::FR_SWIM3, f_walk4); COOP_Walk(8);}
void f_walk4() {FRAME_STATE(FishFrames::FR_SWIM4, f_walk5); COOP_Walk(8);}
void f_walk5() {FRAME_STATE(FishFrames::FR_SWIM5, f_walk6); COOP_Walk(8);}
void f_walk6() {FRAME_STATE(FishFrames::FR_SWIM6, f_walk7); COOP_Walk(8);}
void f_walk7() {FRAME_STATE(FishFrames::FR_SWIM7, f_walk8); COOP_Walk(8);}
void f_walk8() {FRAME_STATE(FishFrames::FR_SWIM8, f_walk9); COOP_Walk(8);}
void f_walk9() {FRAME_STATE(FishFrames::FR_SWIM9, f_walk10); COOP_Walk(8);}
void f_walk10() {FRAME_STATE(FishFrames::FR_SWIM10, f_walk11); COOP_Walk(8);}
void f_walk11() {FRAME_STATE(FishFrames::FR_SWIM11, f_walk12); COOP_Walk(8);}
void f_walk12() {FRAME_STATE(FishFrames::FR_SWIM12, f_walk13); COOP_Walk(8);}
void f_walk13() {FRAME_STATE(FishFrames::FR_SWIM13, f_walk14); COOP_Walk(8);}
void f_walk14() {FRAME_STATE(FishFrames::FR_SWIM14, f_walk15); COOP_Walk(8);}
void f_walk15() {FRAME_STATE(FishFrames::FR_SWIM15, f_walk16); COOP_Walk(8);}
void f_walk16() {FRAME_STATE(FishFrames::FR_SWIM16, f_walk17); COOP_Walk(8);}
void f_walk17() {FRAME_STATE(FishFrames::FR_SWIM17, f_walk18); COOP_Walk(8);}
void f_walk18() {FRAME_STATE(FishFrames::FR_SWIM18, f_walk1); COOP_Walk(8);}

void f_run1() {FRAME_STATE(FishFrames::FR_SWIM1, f_run2); COOP_Run(12);
	if (random() < 0.5)
		sound (self, PR_CHAN_VOICE, "fish/idle.wav", 1, PR_ATTN_NORM);
}
void f_run2() {FRAME_STATE(FishFrames::FR_SWIM3, f_run3); COOP_Run(12);}
void f_run3() {FRAME_STATE(FishFrames::FR_SWIM5, f_run4); COOP_Run(12);}
void f_run4() {FRAME_STATE(FishFrames::FR_SWIM7, f_run5); COOP_Run(12);}
void f_run5() {FRAME_STATE(FishFrames::FR_SWIM9, f_run6); COOP_Run(12);}
void f_run6() {FRAME_STATE(FishFrames::FR_SWIM11, f_run7); COOP_Run(12);}
void f_run7() {FRAME_STATE(FishFrames::FR_SWIM13, f_run8); COOP_Run(12);}
void f_run8() {FRAME_STATE(FishFrames::FR_SWIM15, f_run9); COOP_Run(12);}
void f_run9() {FRAME_STATE(FishFrames::FR_SWIM17, f_run1); COOP_Run(12);
	// if he's no longer in the water, forget about him
	if (self->enemy->waterlevel < 2)
		self->enemy = world;
}

void _fish_melee()
{
	vector	delta;
	float 	ldmg;

	if (self->enemy == world)
		return;		// removed before stroke

	delta = self->enemy->origin - self->origin;

	if (vlen(delta) > 150)
		return;

	sound (self, PR_CHAN_VOICE, "fish/bite.wav", 1, PR_ATTN_NORM);
	ldmg = PR_IDMON_FISH_BITEDMG + random()*10;
	TF_T_Damage (self->enemy, self, self, ldmg, PR_TF_TD_NOTTEAM, PR_TF_TD_MELEE);
}

void f_attack1() {FRAME_STATE(FishFrames::FR_ATTACK1, f_attack2); ai_charge(10);}
void f_attack2() {FRAME_STATE(FishFrames::FR_ATTACK2, f_attack3); ai_charge(10);}
void f_attack3() {FRAME_STATE(FishFrames::FR_ATTACK3, f_attack4); _fish_melee();}
void f_attack4() {FRAME_STATE(FishFrames::FR_ATTACK4, f_attack5); ai_charge(10);}
void f_attack5() {FRAME_STATE(FishFrames::FR_ATTACK5, f_attack6); ai_charge(10);}
void f_attack6() {FRAME_STATE(FishFrames::FR_ATTACK6, f_attack7); ai_charge(10);}
void f_attack7() {FRAME_STATE(FishFrames::FR_ATTACK7, f_attack8); ai_charge(10);}
void f_attack8() {FRAME_STATE(FishFrames::FR_ATTACK8, f_attack9); ai_charge(10);}
void f_attack9() {FRAME_STATE(FishFrames::FR_ATTACK9, f_attack10); _fish_melee();}
void f_attack10() {FRAME_STATE(FishFrames::FR_ATTACK10, f_attack11); ai_charge(10);}
void f_attack11() {FRAME_STATE(FishFrames::FR_ATTACK11, f_attack12); ai_charge(10);}
void f_attack12() {FRAME_STATE(FishFrames::FR_ATTACK12, f_attack13); ai_charge(10);}
void f_attack13() {FRAME_STATE(FishFrames::FR_ATTACK13, f_attack14); ai_charge(10);}
void f_attack14() {FRAME_STATE(FishFrames::FR_ATTACK14, f_attack15); ai_charge(10);}
void f_attack15() {FRAME_STATE(FishFrames::FR_ATTACK15, f_attack16); _fish_melee();}
void f_attack16() {FRAME_STATE(FishFrames::FR_ATTACK16, f_attack17); ai_charge(10);}
void f_attack17() {FRAME_STATE(FishFrames::FR_ATTACK17, f_attack18); ai_charge(10);}
void f_attack18() {FRAME_STATE(FishFrames::FR_ATTACK18, f_run1); ai_charge(10);}

void f_death1() {
FRAME_STATE(FishFrames::FR_DEATH1, f_death2);
sound (self, PR_CHAN_VOICE, "fish/death.wav", 1, PR_ATTN_NORM);
}
void f_death2() {FRAME_STATE(FishFrames::FR_DEATH2, f_death3);}
void f_death3() {FRAME_STATE(FishFrames::FR_DEATH3, f_death4);}
void f_death4() {FRAME_STATE(FishFrames::FR_DEATH4, f_death5);}
void f_death5() {FRAME_STATE(FishFrames::FR_DEATH5, f_death6);}
void f_death6() {FRAME_STATE(FishFrames::FR_DEATH6, f_death7);}
void f_death7() {FRAME_STATE(FishFrames::FR_DEATH7, f_death8);}
void f_death8() {FRAME_STATE(FishFrames::FR_DEATH8, f_death9);}
void f_death9() {FRAME_STATE(FishFrames::FR_DEATH9, f_death10);}
void f_death10() {FRAME_STATE(FishFrames::FR_DEATH10, f_death11);}
void f_death11() {FRAME_STATE(FishFrames::FR_DEATH11, f_death12);}
void f_death12() {FRAME_STATE(FishFrames::FR_DEATH12, f_death13);}
void f_death13() {FRAME_STATE(FishFrames::FR_DEATH13, f_death14);}
void f_death14() {FRAME_STATE(FishFrames::FR_DEATH14, f_death15);}
void f_death15() {FRAME_STATE(FishFrames::FR_DEATH15, f_death16);}
void f_death16() {FRAME_STATE(FishFrames::FR_DEATH16, f_death17);}
void f_death17() {FRAME_STATE(FishFrames::FR_DEATH17, f_death18);}
void f_death18() {FRAME_STATE(FishFrames::FR_DEATH18, f_death19);}
void f_death19() {FRAME_STATE(FishFrames::FR_DEATH19, f_death20);}
void f_death20() {FRAME_STATE(FishFrames::FR_DEATH20, f_death21);}
void f_death21() {FRAME_STATE(FishFrames::FR_DEATH21, f_death21); self->solid = PR_SOLID_NOT;MonsterCorpse();}

void f_pain1() {FRAME_STATE(FishFrames::FR_PAIN1, f_pain2);}
void f_pain2() {FRAME_STATE(FishFrames::FR_PAIN2, f_pain3); ai_pain(6);}
void f_pain3() {FRAME_STATE(FishFrames::FR_PAIN3, f_pain4); ai_pain(6);}
void f_pain4() {FRAME_STATE(FishFrames::FR_PAIN4, f_pain5); ai_pain(6);}
void f_pain5() {FRAME_STATE(FishFrames::FR_PAIN5, f_pain6); ai_pain(6);}
void f_pain6() {FRAME_STATE(FishFrames::FR_PAIN6, f_pain7); ai_pain(6);}
void f_pain7() {FRAME_STATE(FishFrames::FR_PAIN7, f_pain8); ai_pain(6);}
void f_pain8() {FRAME_STATE(FishFrames::FR_PAIN8, f_pain9); ai_pain(6);}
void f_pain9() {FRAME_STATE(FishFrames::FR_PAIN9, f_run1); ai_pain(6);}

void _fish_pain(entity attacker, float damage)
{
// fish allways do pain frames
	f_pain1 ();
}




float _fish_noticeenemy()
{
	// if the enemy isn't in the water, then he's not a valid target
	if (self->enemy->waterlevel < 2)
		return PR_FALSE;

	return PR_TRUE;
}

/*QUAKED monster_fish (1 0 0) (-16 -16 -24) (16 16 24) Ambush
*/
void monster_fish()
{
	if ( deathmatch ) {
		remove( self );
		return;
	}

	self->classname = "idmonster_fish";

	if (!server_spawned) {
		precache_model2 ("progs/fish.mdl");

		precache_sound2 ("fish/death.wav");
		precache_sound2 ("fish/bite.wav");
		precache_sound2 ("fish/idle.wav");
	}

	self->solid = PR_SOLID_SLIDEBOX;
	self->movetype = PR_MOVETYPE_STEP;

	// no head model for fish gibs
	setmodel (self, "progs/fish.mdl");

	setsize (self, V({-16, -16, -24}), V({16, 16, 24}));
	self->max_health = self->health = PR_IDMON_FISH_HEALTH;

	self->th_stand = f_stand1;
	self->th_walk = f_walk1;
	self->th_run = f_run1;
	self->th_die = f_death1;
	self->th_pain = _fish_pain;
	self->th_melee = f_attack1;


	self->netname = "fish";

	self->monsterweight = PR_MWEIGHT_SMALL;
	self->monsterflags = PR_MFLAG_BITER | PR_MFLAG_NOLEGS;

	// monsters have a team of their own
	if ( !self->team_no )
		self->team_no = PR_TEAMNO_MONSTERS;
	self->PR_monsterflag = PR_STRFLAG_MONSTER;

	self->is_connected = PR_TRUE;

	self->OnNoticeEnemy = _fish_noticeenemy;
	self->PR_npc_speedfactor = 1.9;
	self->PR_npc_spawnfunc = monster_fish;
	COOP_SetLives ();

	self->PR_npc_electricitymult = 2.6;
	self->PR_npc_meleemult = 2;

	coop_swimmonster_start ();
}

} // END namespace Progs

#endif
