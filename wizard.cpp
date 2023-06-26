/*
==============================================================================

WIZARD

==============================================================================
*/

#include "progs.h"
#include "wizard.h"
#include "ofndefs.h"
#include "debug.h"
#include "cpstuff.h"
#include "weapons.h"
#include "fight.h"
#include "ai.h"
#include "monsters.h"
#include "player.h"
#include "warlock.h"

namespace Progs {

//$cd id1/models/a_wizard
//$origin 0 0 24
//$base wizbase
//$skin wizbase

namespace CuTF_WizardFrames
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

/* OfN UNUSED
void(entity missile, float mspeed, float accuracy) LaunchMissile =
{
	local	vector	vec, move;
	local	float	fly;

	makevectors (self.angles);

// set missile speed
	vec = self.enemy.origin + self.enemy.mins + self.enemy.size * 0.7 - missile.origin;

// calc aproximate time for missile to reach vec
	fly = vlen (vec) / mspeed;

// get the entities xy velocity
	move = self.enemy.velocity;
	move_z = 0;

// project the target forward in time
	vec = vec + move * fly;

	vec = normalize(vec);
	vec = vec + accuracy*v_up*(random()- 0.5) + accuracy*v_right*(random()- 0.5);

	missile.velocity = vec * mspeed;

	missile.angles = '0 0 0';
	missile.angles_y = vectoyaw(missile.velocity);

// set missile duration
	missile.nextthink = time + 10; //- OfN - was 5
	missile.think = SUB_Remove;
};*/


void wiz_run1();
void wiz_side1();

void MuzzleFlash(entity ent);

/*
=================
WizardCheckAttack
=================
*/
float WizardCheckAttack()
{
	vector	spot1, spot2;
	entity	targ;
	float		chance;

	if (time < self->attack_finished)
		return PR_FALSE;
	if (!enemy_vis)
		return PR_FALSE;

	if (enemy_range == PR_RANGE_FAR)
	{
		if (self->attack_state != PR_AS_STRAIGHT)
		{
			self->attack_state = PR_AS_STRAIGHT;
			wiz_run1 ();
		}
		return PR_FALSE;
	}

	targ = self->enemy;

// see if any entities are in the way of the shot
	spot1 = self->origin + self->view_ofs;
	spot2 = targ->origin + targ->view_ofs;

	traceline (spot1, spot2, PR_TL_ANY_SOLID, self);

	if (trace_ent != targ)
	{	// don't have a clear shot, so move to a side
		if (self->attack_state != PR_AS_STRAIGHT)
		{
			self->attack_state = PR_AS_STRAIGHT;
			wiz_run1 ();
		}
		return PR_FALSE;
	}

	if (enemy_range == PR_RANGE_MELEE)
		chance = 0.9;
	else if (enemy_range == PR_RANGE_NEAR)
		chance = 0.6;
	else if (enemy_range == PR_RANGE_MID)
		chance = 0.2;
	else
		chance = 0;

	if (random () < chance)
	{
		self->attack_state = PR_AS_MISSILE;
		return PR_TRUE;
	}

	if (enemy_range == PR_RANGE_MID)
	{
		if (self->attack_state != PR_AS_STRAIGHT)
		{
			self->attack_state = PR_AS_STRAIGHT;
			wiz_run1 ();
		}
	}
	else
	{
		if (self->attack_state != PR_AS_SLIDING)
		{
			self->attack_state = PR_AS_SLIDING;
			wiz_side1 ();
		}
	}

	return PR_FALSE;
}

/*
=================
WizardAttackFinished
=================
*/
void WizardAttackFinished()  // PZ: This did have a return value of `float`, but it wasn't returning anything.
{
	if (enemy_range >= PR_RANGE_MID || !enemy_vis)
	{
		self->attack_state = PR_AS_STRAIGHT;
		self->think = wiz_run1;
	}
	else
	{
		self->attack_state = PR_AS_SLIDING;
		self->think = wiz_side1;
	}
}

/*
==============================================================================

FAST ATTACKS

==============================================================================
*/

void Wiz_FastFire()
{
	vector		vec;
	vector		dst;

	if (self->owner->health > 0)
	{
		//self.owner.effects = self.owner.effects | #EF_MUZZLEFLASH;
        MuzzleFlash(self->owner);

		makevectors (self->enemy->angles);
		dst = self->enemy->origin - 13*self->movedir;

		vec = normalize(dst - self->origin);
		sound (self, PR_CHAN_WEAPON, "wizard/wattack.wav", 1, PR_ATTN_NORM);
		launch_spike (self->origin, vec);
		newmis->velocity = vec*(600 + 600 * random() ); //- Was 600
		newmis->owner = self->owner;
		newmis->classname = "wizspike";
		setmodel (newmis, "progs/w_spike.mdl");
		setsize (newmis, PR_VEC_ORIGIN, PR_VEC_ORIGIN);
	}

	remove (self);
}


void Wiz_StartFast()
{
	entity	missile;

	sound (self, PR_CHAN_WEAPON, "wizard/wattack.wav", 1, PR_ATTN_NORM);
	self->v_angle = self->angles;
	makevectors (self->angles);

	missile = spawn (); // PZ: these might basically be just timers that can be made server-side, but it's hard to tell; leaving alone for now
	missile->owner = self;
	missile->nextthink = time + 0.6;
	setsize (missile, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (missile, self->origin + V({0, 0, 30}) + v_forward*14 + v_right*14);
	missile->enemy = self->enemy;
	missile->nextthink = time + 0.8;
	missile->think = Wiz_FastFire;
	missile->movedir = v_right;

    //- OfN - Now it fires 3 times
    missile = spawn ();
	missile->owner = self;
	missile->nextthink = time + 0.8;
	setsize (missile, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (missile, self->origin + V({0, 0, 30}) + v_forward*14);// + v_right*14);
	missile->enemy = self->enemy;
	missile->nextthink = time + 0.6;
	missile->think = Wiz_FastFire;
	//missile.movedir = v_right;

	missile = spawn ();
	missile->owner = self;
	missile->nextthink = time + 1;
	setsize (missile, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (missile, self->origin + V({0, 0, 30}) + v_forward*14 + v_right* -14);
	missile->enemy = self->enemy;
	missile->nextthink = time + 0.3;
	missile->think = Wiz_FastFire;
	missile->movedir = PR_VEC_ORIGIN - v_right;
}



void Wiz_idlesound()
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

void wiz_stand1() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER1, wiz_stand2); ai_stand();}
void wiz_stand2() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER2, wiz_stand3); ai_stand();}
void wiz_stand3() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER3, wiz_stand4); ai_stand();}
void wiz_stand4() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER4, wiz_stand5); ai_stand();}
void wiz_stand5() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER5, wiz_stand6); ai_stand();}
void wiz_stand6() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER6, wiz_stand7); ai_stand();}
void wiz_stand7() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER7, wiz_stand8); ai_stand();}
void wiz_stand8() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER8, wiz_stand1); ai_stand();}

void wiz_walk1() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER1, wiz_walk2); ai_walk(8);
Wiz_idlesound();}
void wiz_walk2() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER2, wiz_walk3); ai_walk(8);}
void wiz_walk3() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER3, wiz_walk4); ai_walk(8);}
void wiz_walk4() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER4, wiz_walk5); ai_walk(8);}
void wiz_walk5() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER5, wiz_walk6); ai_walk(8);}
void wiz_walk6() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER6, wiz_walk7); ai_walk(8);}
void wiz_walk7() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER7, wiz_walk8); ai_walk(8);}
void wiz_walk8() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER8, wiz_walk1); ai_walk(8);}

void wiz_side1() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER1, wiz_side2); ai_run(8);
Wiz_idlesound();}
void wiz_side2() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER2, wiz_side3); ai_run(8);}
void wiz_side3() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER3, wiz_side4); ai_run(8);}
void wiz_side4() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER4, wiz_side5); ai_run(8);}
void wiz_side5() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER5, wiz_side6); ai_run(8);}
void wiz_side6() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER6, wiz_side7); ai_run(8);}
void wiz_side7() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER7, wiz_side8); ai_run(8);}
void wiz_side8() {FRAME_STATE(CuTF_WizardFrames::FR_HOVER8, wiz_side1); ai_run(8);}

void wiz_run1() {FRAME_STATE(CuTF_WizardFrames::FR_FLY1, wiz_run2); ai_run(16);
Wiz_idlesound();
}
void wiz_run2() {FRAME_STATE(CuTF_WizardFrames::FR_FLY2, wiz_run3); ai_run(16);}
void wiz_run3() {FRAME_STATE(CuTF_WizardFrames::FR_FLY3, wiz_run4); ai_run(16);}
void wiz_run4() {FRAME_STATE(CuTF_WizardFrames::FR_FLY4, wiz_run5); ai_run(16);}
void wiz_run5() {FRAME_STATE(CuTF_WizardFrames::FR_FLY5, wiz_run6); ai_run(16);}
void wiz_run6() {FRAME_STATE(CuTF_WizardFrames::FR_FLY6, wiz_run7); ai_run(16);}
void wiz_run7() {FRAME_STATE(CuTF_WizardFrames::FR_FLY7, wiz_run8); ai_run(16);}
void wiz_run8() {FRAME_STATE(CuTF_WizardFrames::FR_FLY8, wiz_run9); ai_run(16);}
void wiz_run9() {FRAME_STATE(CuTF_WizardFrames::FR_FLY9, wiz_run10); ai_run(16);}
void wiz_run10() {FRAME_STATE(CuTF_WizardFrames::FR_FLY10, wiz_run11); ai_run(16);}
void wiz_run11() {FRAME_STATE(CuTF_WizardFrames::FR_FLY11, wiz_run12); ai_run(16);}
void wiz_run12() {FRAME_STATE(CuTF_WizardFrames::FR_FLY12, wiz_run13); ai_run(16);}
void wiz_run13() {FRAME_STATE(CuTF_WizardFrames::FR_FLY13, wiz_run14); ai_run(16);}
void wiz_run14() {FRAME_STATE(CuTF_WizardFrames::FR_FLY14, wiz_run1); ai_run(16);}

void wiz_fast1() {FRAME_STATE(CuTF_WizardFrames::FR_MAGATT1, wiz_fast2); ai_face();Wiz_StartFast();}
void wiz_fast2() {FRAME_STATE(CuTF_WizardFrames::FR_MAGATT2, wiz_fast3); ai_face();
    MonsterAuraPower();
}
void wiz_fast3() {FRAME_STATE(CuTF_WizardFrames::FR_MAGATT3, wiz_fast4); ai_face();}
void wiz_fast4() {FRAME_STATE(CuTF_WizardFrames::FR_MAGATT4, wiz_fast5); ai_face();}
void wiz_fast5() {FRAME_STATE(CuTF_WizardFrames::FR_MAGATT5, wiz_fast6); ai_face();}
void wiz_fast6() {FRAME_STATE(CuTF_WizardFrames::FR_MAGATT6, wiz_fast7); ai_face();}
void wiz_fast7() {FRAME_STATE(CuTF_WizardFrames::FR_MAGATT5, wiz_fast8); ai_face();}
void wiz_fast8() {FRAME_STATE(CuTF_WizardFrames::FR_MAGATT4, wiz_fast9); ai_face();}
void wiz_fast9() {FRAME_STATE(CuTF_WizardFrames::FR_MAGATT3, wiz_fast10); ai_face();}
void wiz_fast10() {FRAME_STATE(CuTF_WizardFrames::FR_MAGATT2, wiz_run1); ai_face();Attack_Finished(2);WizardAttackFinished ();}

void wiz_pain1() {FRAME_STATE(CuTF_WizardFrames::FR_PAIN1, wiz_pain2);}
void wiz_pain2() {FRAME_STATE(CuTF_WizardFrames::FR_PAIN2, wiz_pain3);}
void wiz_pain3() {FRAME_STATE(CuTF_WizardFrames::FR_PAIN3, wiz_pain4);}
void wiz_pain4() {FRAME_STATE(CuTF_WizardFrames::FR_PAIN4, wiz_run1);}

void wiz_death1() {

FRAME_STATE(CuTF_WizardFrames::FR_DEATH1, wiz_death2);

self->velocity[X] = -200 + 400*random();
self->velocity[Y] = -200 + 400*random();
self->velocity[Z] = 100 + 100*random();
self->flags = self->flags - (self->flags & PR_FL_ONGROUND);
sound (self, PR_CHAN_VOICE, "wizard/wdeath.wav", 1, PR_ATTN_MONSTERDIE);
}
void wiz_death2() {FRAME_STATE(CuTF_WizardFrames::FR_DEATH2, wiz_death3);}
void wiz_death3() {FRAME_STATE(CuTF_WizardFrames::FR_DEATH3, wiz_death4); self->solid = PR_SOLID_NOT;}
void wiz_death4() {FRAME_STATE(CuTF_WizardFrames::FR_DEATH4, wiz_death5);}
void wiz_death5() {FRAME_STATE(CuTF_WizardFrames::FR_DEATH5, wiz_death6);}
void wiz_death6() {FRAME_STATE(CuTF_WizardFrames::FR_DEATH6, wiz_death7);}
void wiz_death7() {FRAME_STATE(CuTF_WizardFrames::FR_DEATH7, wiz_death8);}
void wiz_death8()
{
    //self.nextthink = time + 40 + 40*random();
	//self.think = SUB_Remove;
    FRAME_STATE(CuTF_WizardFrames::FR_DEATH8, wiz_death8);
    MonsterCorpse();
}

void wiz_die()
{
    if (self->real_owner->classname == "player")
    {
	    sprint(self->real_owner,PR_PRINT_HIGH,"Your scrag ");
		sprint(self->real_owner,PR_PRINT_HIGH,self->netname);
		sprint(self->real_owner,PR_PRINT_HIGH," is dead.\n");
	    self->real_owner->job_finished = time + 5; //Can't summon streams of demons SB can so
        ResetSlotPointer(self->real_owner, self->increase_team1);
    }

// check for gib
    if (self->health < -30)
	{
		sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_MONSTERDIE);
		ThrowMonsterHead ("progs/h_wizard.mdl", self->health); // was throwhead
		ThrowGib ("progs/gib2.mdl", self->health,PR_TRUE,0,PR_GIB2_KGS, PR_FALSE);
		ThrowGib ("progs/gib3.mdl", self->health,PR_TRUE,0,PR_GIB3_KGS, PR_FALSE);
		ThrowGib ("progs/gib3.mdl", self->health,PR_TRUE,0,PR_GIB3_KGS, 0.25);
        SpawnBloodEx(self->origin,0.66,9);

        dremove(self);
        return;
	}

    // Spawn bubbles if under water/slime
    float contres;
    contres = pointcontents(self->origin);
    if (contres == PR_CONTENT_WATER || contres == PR_CONTENT_SLIME)
        DeathBubbles(4);

    //self.classname = "monster_corpse";
    MonsterDead();
    self->think=SUB_Null;
    wiz_death1();
}


void Wiz_Pain(entity attacker, float damage)
{
	/*if (!(self.pain_finished > time))
    {
       sound (self, #CHAN_VOICE, "wizard/wpain.wav", 1, #ATTN_NORM);
       self.pain_finished = time + 1.7*random();
    } //only used for sound*/
    if (self->health <= 0)
        return;

    if (random()*80 > damage) // was 60
		return;		// didn't flinch

    AI_CheckAttacker(attacker);

    sound (self, PR_CHAN_VOICE, "wizard/wpain.wav", 1, PR_ATTN_NORM);

	wiz_pain1 ();
}


void Wiz_Missile()
{
	wiz_fast1();
}

//===========================//
// OfteN - Wizard prechaches //
//===========================//

void Wiz_Precache()
{
    precache_model ("progs/wizard.mdl");
    //precache_model ("progs/tf_wiz.mdl");

	precache_model ("progs/h_wizard.mdl");
	precache_model ("progs/w_spike.mdl");

	precache_sound ("wizard/hit.wav");		// used by c code
	precache_sound ("wizard/wattack.wav");
	precache_sound ("wizard/wdeath.wav");
	precache_sound ("wizard/widle1.wav");
	precache_sound ("wizard/widle2.wav");
	precache_sound ("wizard/wpain.wav");
	precache_sound ("wizard/wsight.wav");

    // wizard spike sound, used on weapons.qc spiketouch();
    precache_sound ("effects/crunch.wav"); //OfN - Dunno where i got this sound from, probably mega-tf
}

/*QUAKED monster_wizard (1 0 0) (-16 -16 -24) (16 16 40) Ambush
*/
/*void() monster_wizard =
{
	if (deathmatch)
	{
		remove(self);
		return;
	}
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

	self.solid = #SOLID_SLIDEBOX;
	self.movetype = #MOVETYPE_STEP;

	setmodel (self, "progs/wizard.mdl");

	setsize (self, '-16 -16 -24', '16 16 40');
	self.health = 80;

	self.th_stand = wiz_stand1;
	self.th_walk = wiz_walk1;
	self.th_run = wiz_run1;
	self.th_missile = Wiz_Missile;
	self.th_pain = Wiz_Pain;
	self.th_die = wiz_die;

	flymonster_start ();
};*/

} // END namespace Progs
