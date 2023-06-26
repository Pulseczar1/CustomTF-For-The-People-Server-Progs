/*
	enforcer.qc
*/

#include "progs.h"

#ifndef PR_COOP_MODE_ENHANCED
//#pragma DONT_COMPILE_THIS_FILE
#else

#include "idmonsters/enforcer.h"
#include "idmonsters/coop_monsters.h"
#include "coop_defs.h"
#include "ofndefs.h"
#include "weapons.h"
#include "qw.h"
#include "common.h"
#include "subs.h"
#include "ai.h"
#include "items.h"
#include "monsters.h"
#include "warlock.h"

namespace Progs {

#define PR_IDMON_ENFORCER_HEALTH		300
#define PR_IDMON_ENFORCER_ZAPDMG		60

/*
==============================================================================

SOLDIER / PLAYER

==============================================================================
*/

//$cd id1/models/enforcer
//$origin 0 -6 24
//$base base
//$skin skin

namespace EnforcerFrames
{
	enum {FR_STAND1, FR_STAND2, FR_STAND3, FR_STAND4, FR_STAND5, FR_STAND6, FR_STAND7};

	enum {FR_WALK1 = 7, FR_WALK2, FR_WALK3, FR_WALK4, FR_WALK5, FR_WALK6, FR_WALK7, FR_WALK8, FR_WALK9, FR_WALK10};
	enum {FR_WALK11 = 17, FR_WALK12, FR_WALK13, FR_WALK14, FR_WALK15, FR_WALK16};

	enum {FR_RUN1 = 23, FR_RUN2, FR_RUN3, FR_RUN4, FR_RUN5, FR_RUN6, FR_RUN7, FR_RUN8};

	enum {FR_ATTACK1 = 31, FR_ATTACK2, FR_ATTACK3, FR_ATTACK4, FR_ATTACK5, FR_ATTACK6};
	enum {FR_ATTACK7 = 37, FR_ATTACK8, FR_ATTACK9, FR_ATTACK10};

	enum {FR_DEATH1 = 41, FR_DEATH2, FR_DEATH3, FR_DEATH4, FR_DEATH5, FR_DEATH6, FR_DEATH7, FR_DEATH8};
	enum {FR_DEATH9 = 49, FR_DEATH10, FR_DEATH11, FR_DEATH12, FR_DEATH13, FR_DEATH14};

	enum {FR_FDEATH1 = 55, FR_FDEATH2, FR_FDEATH3, FR_FDEATH4, FR_FDEATH5, FR_FDEATH6, FR_FDEATH7, FR_FDEATH8};
	enum {FR_FDEATH9 = 63, FR_FDEATH10, FR_FDEATH11};

	enum {FR_PAINA1 = 66, FR_PAINA2, FR_PAINA3, FR_PAINA4};

	enum {FR_PAINB1 = 70, FR_PAINB2, FR_PAINB3, FR_PAINB4, FR_PAINB5};

	enum {FR_PAINC1 = 75, FR_PAINC2, FR_PAINC3, FR_PAINC4, FR_PAINC5, FR_PAINC6, FR_PAINC7, FR_PAINC8};

	enum {FR_PAIND1 = 83, FR_PAIND2, FR_PAIND3, FR_PAIND4, FR_PAIND5, FR_PAIND6, FR_PAIND7, FR_PAIND8};
	enum {FR_PAIND9 = 91, FR_PAIND10, FR_PAIND11, FR_PAIND12, FR_PAIND13, FR_PAIND14, FR_PAIND15, FR_PAIND16};
	enum {FR_PAIND17 = 99, FR_PAIND18, FR_PAIND19};
}

void _Laser_Touch()
{
	vector org;

	if (other == self->owner)
		return;		// don't explode on owner

	if (pointcontents(self->origin) == PR_CONTENT_SKY)
	{
		remove(self);
		return;
	}

	sound (self, PR_CHAN_WEAPON, "enforcer/enfstop.wav", 1, PR_ATTN_STATIC);
	org = self->origin - 8*normalize(self->velocity);

	if (other->health)
	{
		SpawnBlood (org, 15);
		TF_T_Damage (other, self, self->owner, PR_IDMON_ENFORCER_ZAPDMG + random()*10, PR_TF_TD_NOTTEAM, PR_TF_TD_ELECTRICITY);
	}
	else
	{
		WriteByte (PR_MSG_MULTICAST, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_MULTICAST, PR_TE_GUNSHOT);
		WriteByte (PR_MSG_MULTICAST, 3);
		WriteCoord (PR_MSG_MULTICAST, org[X]);
		WriteCoord (PR_MSG_MULTICAST, org[Y]);
		WriteCoord (PR_MSG_MULTICAST, org[Z]);
		multicast (org, PR_MULTICAST_PHS);
		if (other->takedamage) {
			other->show_hostile = time + 1;	// wake monsters up
		}
	}

	remove(self);
}

void _LaunchLaser(const vector& org, vector vec)
{
	if (self->classname == "idmonster_enforcer")
		sound (self, PR_CHAN_WEAPON, "enforcer/enfire.wav", 1, PR_ATTN_NORM);

	vec = normalize(vec);

	newmis = spawn();
	newmis->owner = self;
	newmis->movetype = PR_MOVETYPE_FLY;
	newmis->solid = PR_SOLID_BBOX;
	newmis->effects = PR_EF_DIMLIGHT;

	setmodel (newmis, "progs/laser.mdl");
	setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));

	setorigin (newmis, org);

	newmis->velocity = vec * 1200;
	newmis->angles = vectoangles(newmis->velocity);

	newmis->nextthink = time + 5;
	newmis->think = SUB_Remove;
	newmis->touch = _Laser_Touch;
}



void enforcer_fire()
{
	vector org;

	muzzleflash ();
	makevectors (self->angles);

	org = self->origin + v_forward * 30 + v_right * 8.5 + V({0, 0, 16});

	_LaunchLaser(org, (self->enemy->origin + self->PR_npc_enemyoffset) - org);
}

//============================================================================

void enf_stand1() {FRAME_STATE(EnforcerFrames::FR_STAND1, enf_stand2); COOP_Idle();}
void enf_stand2() {FRAME_STATE(EnforcerFrames::FR_STAND2, enf_stand3);}
void enf_stand3() {FRAME_STATE(EnforcerFrames::FR_STAND3, enf_stand4);}
void enf_stand4() {FRAME_STATE(EnforcerFrames::FR_STAND4, enf_stand5); COOP_Idle();}
void enf_stand5() {FRAME_STATE(EnforcerFrames::FR_STAND5, enf_stand6);}
void enf_stand6() {FRAME_STATE(EnforcerFrames::FR_STAND6, enf_stand7);}
void enf_stand7() {FRAME_STATE(EnforcerFrames::FR_STAND7, enf_stand1);}

void enf_walk1() {
FRAME_STATE(EnforcerFrames::FR_WALK1, enf_walk2);
if (random() < 0.2)
	sound (self, PR_CHAN_VOICE, "enforcer/idle1.wav", 1, PR_ATTN_IDLE);
COOP_Walk(2);}
void enf_walk2() {FRAME_STATE(EnforcerFrames::FR_WALK2, enf_walk3); COOP_Walk(4);}
void enf_walk3() {FRAME_STATE(EnforcerFrames::FR_WALK3, enf_walk4); COOP_Walk(4);}
void enf_walk4() {FRAME_STATE(EnforcerFrames::FR_WALK4, enf_walk5); COOP_Walk(3);}
void enf_walk5() {FRAME_STATE(EnforcerFrames::FR_WALK5, enf_walk6); COOP_Walk(1);}
void enf_walk6() {FRAME_STATE(EnforcerFrames::FR_WALK6, enf_walk7); COOP_Walk(2);}
void enf_walk7() {FRAME_STATE(EnforcerFrames::FR_WALK7, enf_walk8); COOP_Walk(2);}
void enf_walk8() {FRAME_STATE(EnforcerFrames::FR_WALK8, enf_walk9); COOP_Walk(1);}
void enf_walk9() {FRAME_STATE(EnforcerFrames::FR_WALK9, enf_walk10); COOP_Walk(2);}
void enf_walk10() {FRAME_STATE(EnforcerFrames::FR_WALK10, enf_walk11); COOP_Walk(4);}
void enf_walk11() {FRAME_STATE(EnforcerFrames::FR_WALK11, enf_walk12); COOP_Walk(4);}
void enf_walk12() {FRAME_STATE(EnforcerFrames::FR_WALK12, enf_walk13); COOP_Walk(1);}
void enf_walk13() {FRAME_STATE(EnforcerFrames::FR_WALK13, enf_walk14); COOP_Walk(2);}
void enf_walk14() {FRAME_STATE(EnforcerFrames::FR_WALK14, enf_walk15); COOP_Walk(3);}
void enf_walk15() {FRAME_STATE(EnforcerFrames::FR_WALK15, enf_walk16); COOP_Walk(4);}
void enf_walk16() {FRAME_STATE(EnforcerFrames::FR_WALK16, enf_walk1); COOP_Walk(2);}

void enf_run1() {
FRAME_STATE(EnforcerFrames::FR_RUN1, enf_run2);
if (random() < 0.2)
	sound (self, PR_CHAN_VOICE, "enforcer/idle1.wav", 1, PR_ATTN_IDLE);
COOP_Run(18);}
void enf_run2() {FRAME_STATE(EnforcerFrames::FR_RUN2, enf_run3); COOP_Run(14);}
void enf_run3() {FRAME_STATE(EnforcerFrames::FR_RUN3, enf_run4); COOP_Run(7);}
void enf_run4() {FRAME_STATE(EnforcerFrames::FR_RUN4, enf_run5); COOP_Run(12);}
void enf_run5() {FRAME_STATE(EnforcerFrames::FR_RUN5, enf_run6); COOP_Run(14);}
void enf_run6() {FRAME_STATE(EnforcerFrames::FR_RUN6, enf_run7); COOP_Run(14);}
void enf_run7() {FRAME_STATE(EnforcerFrames::FR_RUN7, enf_run8); COOP_Run(7);}
void enf_run8() {FRAME_STATE(EnforcerFrames::FR_RUN8, enf_run1); COOP_Run(11);}

void enf_atk1() {FRAME_STATE(EnforcerFrames::FR_ATTACK1, enf_atk2); COOP_Turn();}
void enf_atk2() {FRAME_STATE(EnforcerFrames::FR_ATTACK2, enf_atk3); COOP_Turn();}
void enf_atk3() {FRAME_STATE(EnforcerFrames::FR_ATTACK3, enf_atk4); COOP_Turn();}
void enf_atk4() {FRAME_STATE(EnforcerFrames::FR_ATTACK4, enf_atk5); COOP_Turn();}
void enf_atk5() {FRAME_STATE(EnforcerFrames::FR_ATTACK5, enf_atk6); COOP_Turn();}
void enf_atk6() {FRAME_STATE(EnforcerFrames::FR_ATTACK6, enf_atk7); enforcer_fire();}
void enf_atk7() {FRAME_STATE(EnforcerFrames::FR_ATTACK7, enf_atk8); COOP_Turn();}
void enf_atk8() {FRAME_STATE(EnforcerFrames::FR_ATTACK8, enf_atk9); COOP_Turn();}
void enf_atk9() {FRAME_STATE(EnforcerFrames::FR_ATTACK5, enf_atk10); COOP_Turn();}
void enf_atk10() {FRAME_STATE(EnforcerFrames::FR_ATTACK6, enf_atk11); enforcer_fire();}
void enf_atk11() {FRAME_STATE(EnforcerFrames::FR_ATTACK7, enf_atk12); COOP_Turn();}
void enf_atk12() {FRAME_STATE(EnforcerFrames::FR_ATTACK8, enf_atk13); COOP_Turn();}
void enf_atk13() {FRAME_STATE(EnforcerFrames::FR_ATTACK9, enf_atk14); COOP_Turn();}
void enf_atk14() {FRAME_STATE(EnforcerFrames::FR_ATTACK10, enf_run1); COOP_Turn();
SUB_CheckRefire (enf_atk1);
	Attack_Finished( random()*2 );
}

void enf_paina1() {FRAME_STATE(EnforcerFrames::FR_PAINA1, enf_paina2);}
void enf_paina2() {FRAME_STATE(EnforcerFrames::FR_PAINA2, enf_paina3);}
void enf_paina3() {FRAME_STATE(EnforcerFrames::FR_PAINA3, enf_paina4);}
void enf_paina4() {FRAME_STATE(EnforcerFrames::FR_PAINA4, enf_run1);}

void enf_painb1() {FRAME_STATE(EnforcerFrames::FR_PAINB1, enf_painb2);}
void enf_painb2() {FRAME_STATE(EnforcerFrames::FR_PAINB2, enf_painb3);}
void enf_painb3() {FRAME_STATE(EnforcerFrames::FR_PAINB3, enf_painb4);}
void enf_painb4() {FRAME_STATE(EnforcerFrames::FR_PAINB4, enf_painb5);}
void enf_painb5() {FRAME_STATE(EnforcerFrames::FR_PAINB5, enf_run1);}

void enf_painc1() {FRAME_STATE(EnforcerFrames::FR_PAINC1, enf_painc2);}
void enf_painc2() {FRAME_STATE(EnforcerFrames::FR_PAINC2, enf_painc3);}
void enf_painc3() {FRAME_STATE(EnforcerFrames::FR_PAINC3, enf_painc4);}
void enf_painc4() {FRAME_STATE(EnforcerFrames::FR_PAINC4, enf_painc5);}
void enf_painc5() {FRAME_STATE(EnforcerFrames::FR_PAINC5, enf_painc6);}
void enf_painc6() {FRAME_STATE(EnforcerFrames::FR_PAINC6, enf_painc7);}
void enf_painc7() {FRAME_STATE(EnforcerFrames::FR_PAINC7, enf_painc8);}
void enf_painc8() {FRAME_STATE(EnforcerFrames::FR_PAINC8, enf_run1);}

void enf_paind1() {FRAME_STATE(EnforcerFrames::FR_PAIND1, enf_paind2);}
void enf_paind2() {FRAME_STATE(EnforcerFrames::FR_PAIND2, enf_paind3);}
void enf_paind3() {FRAME_STATE(EnforcerFrames::FR_PAIND3, enf_paind4);}
void enf_paind4() {FRAME_STATE(EnforcerFrames::FR_PAIND4, enf_paind5); ai_painforward(2);}
void enf_paind5() {FRAME_STATE(EnforcerFrames::FR_PAIND5, enf_paind6); ai_painforward(1);}
void enf_paind6() {FRAME_STATE(EnforcerFrames::FR_PAIND6, enf_paind7);}
void enf_paind7() {FRAME_STATE(EnforcerFrames::FR_PAIND7, enf_paind8);}
void enf_paind8() {FRAME_STATE(EnforcerFrames::FR_PAIND8, enf_paind9);}
void enf_paind9() {FRAME_STATE(EnforcerFrames::FR_PAIND9, enf_paind10);}
void enf_paind10() {FRAME_STATE(EnforcerFrames::FR_PAIND10, enf_paind11);}
void enf_paind11() {FRAME_STATE(EnforcerFrames::FR_PAIND11, enf_paind12); ai_painforward(1);}
void enf_paind12() {FRAME_STATE(EnforcerFrames::FR_PAIND12, enf_paind13); ai_painforward(1);}
void enf_paind13() {FRAME_STATE(EnforcerFrames::FR_PAIND13, enf_paind14); ai_painforward(1);}
void enf_paind14() {FRAME_STATE(EnforcerFrames::FR_PAIND14, enf_paind15);}
void enf_paind15() {FRAME_STATE(EnforcerFrames::FR_PAIND15, enf_paind16);}
void enf_paind16() {FRAME_STATE(EnforcerFrames::FR_PAIND16, enf_paind17); ai_pain(1);}
void enf_paind17() {FRAME_STATE(EnforcerFrames::FR_PAIND17, enf_paind18); ai_pain(1);}
void enf_paind18() {FRAME_STATE(EnforcerFrames::FR_PAIND18, enf_paind19);}
void enf_paind19() {FRAME_STATE(EnforcerFrames::FR_PAIND19, enf_run1);}

void enf_pain(entity attacker, float damage)
{
	float r;

	r = random ();
	if (self->pain_finished > time)
		return;


	if (r < 0.5)
		sound (self, PR_CHAN_VOICE, "enforcer/pain1.wav", 1, PR_ATTN_NORM);
	else
		sound (self, PR_CHAN_VOICE, "enforcer/pain2.wav", 1, PR_ATTN_NORM);

	if (r < 0.2)
	{
		self->pain_finished = time + 1;
		enf_paina1 ();
	}
	else if (r < 0.4)
	{
		self->pain_finished = time + 1;
		enf_painb1 ();
	}
	else if (r < 0.7)
	{
		self->pain_finished = time + 1;
		enf_painc1 ();
	}
	else
	{
		self->pain_finished = time + 2;
		enf_paind1 ();
	}
}

//============================================================================




void enf_die1() {FRAME_STATE(EnforcerFrames::FR_DEATH1, enf_die2);}
void enf_die2() {FRAME_STATE(EnforcerFrames::FR_DEATH2, enf_die3);}
void enf_die3()
{FRAME_STATE(EnforcerFrames::FR_DEATH3, enf_die4); self->solid = PR_SOLID_NOT;self->ammo_cells = 30;DropBackpack();}
void enf_die4() {FRAME_STATE(EnforcerFrames::FR_DEATH4, enf_die5); ai_forward(14);}
void enf_die5() {FRAME_STATE(EnforcerFrames::FR_DEATH5, enf_die6); ai_forward(2);}
void enf_die6() {FRAME_STATE(EnforcerFrames::FR_DEATH6, enf_die7);}
void enf_die7() {FRAME_STATE(EnforcerFrames::FR_DEATH7, enf_die8);}
void enf_die8() {FRAME_STATE(EnforcerFrames::FR_DEATH8, enf_die9);}
void enf_die9() {FRAME_STATE(EnforcerFrames::FR_DEATH9, enf_die10); ai_forward(3);}
void enf_die10() {FRAME_STATE(EnforcerFrames::FR_DEATH10, enf_die11); ai_forward(5);}
void enf_die11() {FRAME_STATE(EnforcerFrames::FR_DEATH11, enf_die12); ai_forward(5);}
void enf_die12() {FRAME_STATE(EnforcerFrames::FR_DEATH12, enf_die13); ai_forward(5);}
void enf_die13() {FRAME_STATE(EnforcerFrames::FR_DEATH13, enf_die14);}
void enf_die14() {FRAME_STATE(EnforcerFrames::FR_DEATH14, enf_die14); MonsterCorpse();}

void enf_fdie1() {

FRAME_STATE(EnforcerFrames::FR_FDEATH1, enf_fdie2);

}
void enf_fdie2() {FRAME_STATE(EnforcerFrames::FR_FDEATH2, enf_fdie3);}
void enf_fdie3()
{FRAME_STATE(EnforcerFrames::FR_FDEATH3, enf_fdie4); self->solid = PR_SOLID_NOT;self->ammo_cells = 30;DropBackpack();}
void enf_fdie4() {FRAME_STATE(EnforcerFrames::FR_FDEATH4, enf_fdie5);}
void enf_fdie5() {FRAME_STATE(EnforcerFrames::FR_FDEATH5, enf_fdie6);}
void enf_fdie6() {FRAME_STATE(EnforcerFrames::FR_FDEATH6, enf_fdie7);}
void enf_fdie7() {FRAME_STATE(EnforcerFrames::FR_FDEATH7, enf_fdie8);}
void enf_fdie8() {FRAME_STATE(EnforcerFrames::FR_FDEATH8, enf_fdie9);}
void enf_fdie9() {FRAME_STATE(EnforcerFrames::FR_FDEATH9, enf_fdie10);}
void enf_fdie10() {FRAME_STATE(EnforcerFrames::FR_FDEATH10, enf_fdie11);}
void enf_fdie11() {FRAME_STATE(EnforcerFrames::FR_FDEATH11, enf_fdie11); MonsterCorpse();}


void enf_die()
{
// check for gib
	if (self->health < -35)
	{
		sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_NORM);
		ThrowMonsterHead ("progs/h_mega.mdl", self->health);
		ThrowGib("progs/gib1.mdl", -40, PR_TRUE, 0, PR_GIB1_KGS, PR_FALSE);
		ThrowGib("progs/gib2.mdl", -40, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
		ThrowGib("progs/gib3.mdl", -40, PR_TRUE, 0, PR_GIB3_KGS, PR_FALSE);
		remove (self);
		return;
	}

// regular death
	sound (self, PR_CHAN_VOICE, "enforcer/death1.wav", 1, PR_ATTN_NORM);
	if (random() > 0.5)
		enf_die1 ();
	else
		enf_fdie1 ();
}



float EnforcerOnSight()
{
	float rsnd;

	rsnd = rint(random() * 3);
	if (rsnd == 1)
		sound (self, PR_CHAN_VOICE, "enforcer/sight1.wav", 1, PR_ATTN_NORM);
	else if (rsnd == 2)
		sound (self, PR_CHAN_VOICE, "enforcer/sight2.wav", 1, PR_ATTN_NORM);
	else if (rsnd == 0)
		sound (self, PR_CHAN_VOICE, "enforcer/sight3.wav", 1, PR_ATTN_NORM);
	else
		sound (self, PR_CHAN_VOICE, "enforcer/sight4.wav", 1, PR_ATTN_NORM);

	return PR_TRUE;
}


/*QUAKED monster_enforcer (1 0 0) (-16 -16 -24) (16 16 40) Ambush

*/
void monster_enforcer()
{
	if ( deathmatch ) {
		remove( self );
		return;
	}

	self->classname = "idmonster_enforcer";

	if (!server_spawned) {
		precache_model2 ("progs/enforcer.mdl");
		precache_model2 ("progs/h_mega.mdl");
		precache_model2 ("progs/laser.mdl");

		precache_sound2 ("enforcer/death1.wav");
		precache_sound2 ("enforcer/enfire.wav");
		precache_sound2 ("enforcer/enfstop.wav");
		precache_sound2 ("enforcer/idle1.wav");
		precache_sound2 ("enforcer/pain1.wav");
		precache_sound2 ("enforcer/pain2.wav");
		precache_sound2 ("enforcer/sight1.wav");
		precache_sound2 ("enforcer/sight2.wav");
		precache_sound2 ("enforcer/sight3.wav");
		precache_sound2 ("enforcer/sight4.wav");
	}

	self->solid = PR_SOLID_SLIDEBOX;
	self->movetype = PR_MOVETYPE_STEP;

	self->mdl = "progs/h_mega.mdl";
	setmodel (self, "progs/enforcer.mdl");

	setsize (self, V({-16, -16, -24}), V({16, 16, 40}));
	self->max_health = self->health = PR_IDMON_ENFORCER_HEALTH;

	self->th_stand = enf_stand1;
	self->th_walk = enf_walk1;
	self->th_run = enf_run1;
	self->th_pain = enf_pain;
	self->th_die = enf_die;
	self->th_missile = enf_atk1;


	self->netname = "enforcer";

	self->monsterweight = PR_MWEIGHT_NORMAL;
	self->monsterflags = PR_MFLAG_HASWEAPON | PR_MFLAG_HUMANOID | PR_MFLAG_NOBREATHING;

	// monsters have a team of their own
	if ( !self->team_no )
		self->team_no = PR_TEAMNO_MONSTERS;
	self->PR_monsterflag = PR_STRFLAG_MONSTER;

	self->is_connected = PR_TRUE;

	self->OnNoticeEnemy = EnforcerOnSight;
	self->PR_npc_spawnfunc = monster_enforcer;
	COOP_SetLives ();

	self->PR_npc_electricitymult = 0.75;
	self->PR_npc_firemult = 0.75;

	// set ammo for emp
	self->ammo_cells = 125;

	coop_walkmonster_start ();
}

} // END namespace Progs

#endif
