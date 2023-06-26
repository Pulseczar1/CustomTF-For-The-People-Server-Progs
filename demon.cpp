#include "progs.h"
#include "ofndefs.h"
#include "demon.h"
#include "ai.h"
#include "fight.h"
#include "monsters.h"
#include "combat.h"
#include "weapons.h"
#include "debug.h"
#include "cpstuff.h"
#include "player.h"

namespace Progs {

void custom_demon_create(float points, float type);
void custom_demon_precache();
void custom_grunt_die();
void custom_shambler_die();
//Extern

/*void () army_stand1;
void () army_walk1;
void () army_run1;
void () army_atk1;*/

//void (entity attacker, float damage) army_pain;

void sham_stand1();
void sham_walk1();
void sham_run1();
void sham_melee();
void sham_magic1();

//void () sham_fireball;

void sham_pain(entity attacker, float damage);

float CheckArea(entity obj, entity builder); //For demon summons

// -- OfN --
void teamprefixsprint(float tno, entity ignore);
void MonsterTouch();
void ThrowMonsterHead(const string& gibname, float dm);
void ResetSlotPointer(entity player, float slot);

/*
==============================================================================

DEMON

==============================================================================
*/

//$cd /raid/quake/id1/models/demon3
//$scale	0.8
//$origin 0 0 24
//$base base
//$skin base

namespace CuTF_DemonFrames
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
void Demon_Water_Jump();
void Demon_JumpTouch();

void demon1_stand1() {FRAME_STATE(CuTF_DemonFrames::FR_STAND1, demon1_stand2); ai_stand();}//Demon_Water_Jump();};
void demon1_stand2() {FRAME_STATE(CuTF_DemonFrames::FR_STAND2, demon1_stand3); ai_stand();}
void demon1_stand3() {FRAME_STATE(CuTF_DemonFrames::FR_STAND3, demon1_stand4); ai_stand();}
void demon1_stand4() {FRAME_STATE(CuTF_DemonFrames::FR_STAND4, demon1_stand5); ai_stand();}
void demon1_stand5() {FRAME_STATE(CuTF_DemonFrames::FR_STAND5, demon1_stand6); ai_stand();}
void demon1_stand6() {FRAME_STATE(CuTF_DemonFrames::FR_STAND6, demon1_stand7); ai_stand();}
void demon1_stand7() {FRAME_STATE(CuTF_DemonFrames::FR_STAND7, demon1_stand8); ai_stand();}
void demon1_stand8() {FRAME_STATE(CuTF_DemonFrames::FR_STAND8, demon1_stand9); ai_stand();}
void demon1_stand9() {FRAME_STATE(CuTF_DemonFrames::FR_STAND9, demon1_stand10); ai_stand();}
void demon1_stand10() {FRAME_STATE(CuTF_DemonFrames::FR_STAND10, demon1_stand11); ai_stand();}
void demon1_stand11() {FRAME_STATE(CuTF_DemonFrames::FR_STAND11, demon1_stand12); ai_stand();}
void demon1_stand12() {FRAME_STATE(CuTF_DemonFrames::FR_STAND12, demon1_stand13); ai_stand();}
void demon1_stand13() {FRAME_STATE(CuTF_DemonFrames::FR_STAND13, demon1_stand1); ai_stand();}

//WK Speeds doubled
#define PR_DEMON_WALKFACTOR 1.25

void demon1_walk1() {
FRAME_STATE(CuTF_DemonFrames::FR_WALK1, demon1_walk2);
Demon_Water_Jump();
if (random() < 0.2)
    sound (self, PR_CHAN_VOICE, "demon/idle1.wav", 1, PR_ATTN_IDLE);
ai_walk(8*PR_DEMON_WALKFACTOR); //8
}
void demon1_walk2() {FRAME_STATE(CuTF_DemonFrames::FR_WALK2, demon1_walk3); ai_walk(6*PR_DEMON_WALKFACTOR);} //6
void demon1_walk3() {FRAME_STATE(CuTF_DemonFrames::FR_WALK3, demon1_walk4); ai_walk(6*PR_DEMON_WALKFACTOR);} //6
void demon1_walk4() {FRAME_STATE(CuTF_DemonFrames::FR_WALK4, demon1_walk5); ai_walk(7*PR_DEMON_WALKFACTOR);} //7
void demon1_walk5() {FRAME_STATE(CuTF_DemonFrames::FR_WALK5, demon1_walk6); ai_walk(4*PR_DEMON_WALKFACTOR);} //4
void demon1_walk6() {FRAME_STATE(CuTF_DemonFrames::FR_WALK6, demon1_walk7); ai_walk(6*PR_DEMON_WALKFACTOR);} //6
void demon1_walk7() {FRAME_STATE(CuTF_DemonFrames::FR_WALK7, demon1_walk8); ai_walk(10*PR_DEMON_WALKFACTOR);} //10
void demon1_walk8() {FRAME_STATE(CuTF_DemonFrames::FR_WALK8, demon1_walk1); ai_walk(10*PR_DEMON_WALKFACTOR);} //10

//WK Speeds doubled
void demon1_run1() {
//Demon_Water_Jump();
FRAME_STATE(CuTF_DemonFrames::FR_RUN1, demon1_run2);
if (random() < 0.2)
    sound (self, PR_CHAN_VOICE, "demon/idle1.wav", 1, PR_ATTN_IDLE);
ai_run(20);}
void demon1_run2() {FRAME_STATE(CuTF_DemonFrames::FR_RUN2, demon1_run3); ai_run(15);}
void demon1_run3() {FRAME_STATE(CuTF_DemonFrames::FR_RUN3, demon1_run4); ai_run(36);}
void demon1_run4() {FRAME_STATE(CuTF_DemonFrames::FR_RUN4, demon1_run5); ai_run(20);}
void demon1_run5() {FRAME_STATE(CuTF_DemonFrames::FR_RUN5, demon1_run6); ai_run(15);}
void demon1_run6() {FRAME_STATE(CuTF_DemonFrames::FR_RUN6, demon1_run1); ai_run(36);}

void demon1_jump1() {FRAME_STATE(CuTF_DemonFrames::FR_LEAP1, demon1_jump2); ai_face();}
void demon1_jump2() {FRAME_STATE(CuTF_DemonFrames::FR_LEAP2, demon1_jump3); ai_face();}
void demon1_jump3() {FRAME_STATE(CuTF_DemonFrames::FR_LEAP3, demon1_jump4); ai_face();}
void demon1_jump4()
{
	FRAME_STATE(CuTF_DemonFrames::FR_LEAP4, demon1_jump5);
	ai_face();

    MonsterAuraPower();

	self->touch = Demon_JumpTouch;
	makevectors (self->angles);
	self->origin[Z] = self->origin[Z] + 1;
	if (self->tfstate & PR_TFSTATE_TRANQUILISED) //Cant jump as far
		self->velocity = v_forward * (600 * (PR_AI_TRANQ_FACTOR_UP / PR_AI_TRANQ_FACTOR_DN)) + V({0, 0, 250});
	else
		self->velocity = v_forward * 600 + V({0, 0, 250});
	if (self->has_sentry == PR_TRUE) //CH if stuck jump, jump higher
	{
		self->velocity[Z] = self->velocity[Z] + 200;
		self->has_sentry = 0;
	}

	if (self->has_tesla == PR_TRUE) //CH why tesla??  I like teslas and its a float
	{
//		sprint(self.real_owner,#PRINT_HIGH,"Demon Jello Jump!\n");
//		sound (self, #CHAN_VOICE, "misc/vapeur2.wav", 1, #ATTN_NONE);
		if (jello == PR_TRUE) //Binary on/off
			self->velocity[Z] = self->velocity[Z] + 1000;
		else
			self->velocity[Z] = self->velocity[Z] + jello;
		self->has_tesla = 0;
	}
	if (self->flags & PR_FL_ONGROUND)
		self->flags = self->flags - PR_FL_ONGROUND;
}
void demon1_jump5() {FRAME_STATE(CuTF_DemonFrames::FR_LEAP5, demon1_jump6);}
void demon1_jump6() {FRAME_STATE(CuTF_DemonFrames::FR_LEAP6, demon1_jump7);}
void demon1_jump7() {FRAME_STATE(CuTF_DemonFrames::FR_LEAP7, demon1_jump8);}
void demon1_jump8() {FRAME_STATE(CuTF_DemonFrames::FR_LEAP8, demon1_jump9);}
void demon1_jump9() {FRAME_STATE(CuTF_DemonFrames::FR_LEAP9, demon1_jump10); self->has_teleporter = 0;}
//CH rather then a 3 sec wait.  bust up into 5  better jello effect
void demon1_jump10() {
	FRAME_STATE(CuTF_DemonFrames::FR_LEAP10, demon1_jump13);
	Demon_Water_Jump();
//CH just in case have a loop catcher
	self->has_teleporter = self->has_teleporter + 1;
	if (self->has_teleporter > 40 || self->velocity == V({0, 0, 0})) {
		self->has_sentry = PR_TRUE; //CH Stuck Jump
		self->think = demon1_jump1;
	}
	self->nextthink = time + 0.1;
}
void demon1_jump13() {
FRAME_STATE(CuTF_DemonFrames::FR_LEAP10, demon1_jump10);
Demon_Water_Jump();
self->nextthink = time + 0.02;
}


void demon1_jump11() {FRAME_STATE(CuTF_DemonFrames::FR_LEAP11, demon1_jump12);}
void demon1_jump12() {FRAME_STATE(CuTF_DemonFrames::FR_LEAP12, demon1_run1);}

//WK Speeds doubled
void demon1_atta1() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA1, demon1_atta2); ai_charge(8);}
void demon1_atta2() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA2, demon1_atta3); ai_charge(0);}
void demon1_atta3() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA3, demon1_atta4); ai_charge(0);
    MonsterAuraPower();
}
void demon1_atta4() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA4, demon1_atta5); ai_charge(2);}
void demon1_atta5() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA5, demon1_atta6); ai_charge(4); Demon_Melee(200);}
void demon1_atta6() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA6, demon1_atta7); ai_charge(2);}
void demon1_atta7() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA7, demon1_atta8); ai_charge(12);}
void demon1_atta8() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA8, demon1_atta9); ai_charge(16);}
void demon1_atta9() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA9, demon1_atta10); ai_charge(8);}
void demon1_atta10() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA10, demon1_atta11); ai_charge(4);
    MonsterAuraPower();
}
void demon1_atta11() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA11, demon1_atta12); Demon_Melee(-200);}
void demon1_atta12() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA12, demon1_atta13); ai_charge(10);}
void demon1_atta13() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA13, demon1_atta14); ai_charge(16);}
void demon1_atta14() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA14, demon1_atta15); ai_charge(8);}
void demon1_atta15() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA15, demon1_run1); ai_charge(8);}

void demon1_pain1() {FRAME_STATE(CuTF_DemonFrames::FR_PAIN1, demon1_pain2);}
void demon1_pain2() {FRAME_STATE(CuTF_DemonFrames::FR_PAIN2, demon1_pain3);}
void demon1_pain3() {FRAME_STATE(CuTF_DemonFrames::FR_PAIN3, demon1_pain4);}
void demon1_pain4() {FRAME_STATE(CuTF_DemonFrames::FR_PAIN4, demon1_pain5);}
void demon1_pain5() {FRAME_STATE(CuTF_DemonFrames::FR_PAIN5, demon1_pain6);}
void demon1_pain6() {FRAME_STATE(CuTF_DemonFrames::FR_PAIN6, demon1_run1);} // was 1


//CH animation frame for demon throwing fire balls
void demon1_fire1();
void Demon_Shoot_Fire(float side);
void demon1_fire1() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA1, demon1_fire2); ai_charge(16);}
void demon1_fire2() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA2, demon1_fire3); ai_charge(8);}
void demon1_fire3() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA3, demon1_fire4); ai_charge(2);}
void demon1_fire4() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA4, demon1_fire5); ai_charge(4);
    MonsterAuraPower();
}
void demon1_fire5() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA5, demon1_fire6); ai_charge(8);}
void demon1_fire6() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA6, demon1_fire7); ai_charge(4); Demon_Shoot_Fire(-1);}
void demon1_fire7() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA7, demon1_fire8); ai_charge(24);}
void demon1_fire8() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA8, demon1_fire9); ai_charge(32);}
void demon1_fire9() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA9, demon1_fire10); ai_charge(16);}
void demon1_fire10() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA10, demon1_fire11); ai_charge(8);
    MonsterAuraPower();
}
void demon1_fire11() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA11, demon1_fire12); ai_charge(4); Demon_Shoot_Fire(1);}
void demon1_fire12() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA12, demon1_fire13); ai_charge(20);}
void demon1_fire13() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA13, demon1_fire14); ai_charge(32);}
void demon1_fire14() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA14, demon1_fire15); ai_charge(16);}
void demon1_fire15() {FRAME_STATE(CuTF_DemonFrames::FR_ATTACKA15, demon1_run1); ai_charge(16);}

void demon1_pain(entity attacker, float damage)
{

	//self.real_owner.StatusRefreshTime = time + 0.2;
	//self.real_owner.StatusBarScreen = 3;

    if (self->touch == Demon_JumpTouch)
		return;

	if (self->pain_finished > time)
		return;

    if (self->health <= 0)
		return;

	//CH because its on the sbar :)

	if (random()*80 > damage)
		return;		// didn't flinch

    AI_CheckAttacker(attacker);

    self->pain_finished = time + 0.8 + random() * 0.8;
    sound (self, PR_CHAN_VOICE, "demon/dpain1.wav", 1, PR_ATTN_NORM);

	demon1_pain1 ();
}

void demon1_die1() {
FRAME_STATE(CuTF_DemonFrames::FR_DEATH1, demon1_die2);
sound (self, PR_CHAN_VOICE, "demon/ddeath.wav", 1, PR_ATTN_MONSTERDIE);}
void demon1_die2() {FRAME_STATE(CuTF_DemonFrames::FR_DEATH2, demon1_die3);}
void demon1_die3() {FRAME_STATE(CuTF_DemonFrames::FR_DEATH3, demon1_die4);}
void demon1_die4() {FRAME_STATE(CuTF_DemonFrames::FR_DEATH4, demon1_die5);}
void demon1_die5() {FRAME_STATE(CuTF_DemonFrames::FR_DEATH5, demon1_die6);}
void demon1_die6()
{FRAME_STATE(CuTF_DemonFrames::FR_DEATH6, demon1_die7); self->solid = PR_SOLID_NOT;}
void demon1_die7() {FRAME_STATE(CuTF_DemonFrames::FR_DEATH7, demon1_die8);}
void demon1_die8() {FRAME_STATE(CuTF_DemonFrames::FR_DEATH8, demon1_die9);}
void demon1_die9()
{
    //self.nextthink = time + 40 + 40*random();
	//self.think = SUB_Remove;
    FRAME_STATE(CuTF_DemonFrames::FR_DEATH9, demon1_die9);
    MonsterCorpse();
}

//void() monster_demon_respawn;

/*void() demon_die =
{
	self.oldorigin = self.origin;
// check for gib
	if (self.health < -80)
	{
		sound (self, #CHAN_VOICE, "player/udeath.wav", 1, #ATTN_NORM);
		ThrowGib ("progs/h_demon.mdl", self.health);
		ThrowGib ("progs/gib1.mdl", self.health);
		ThrowGib ("progs/gib2.mdl", self.health);
		ThrowGib ("progs/gib3.mdl", self.health);

		self.lives = self.lives - 1;
		monster_demon_respawn();
		return;
	}

// regular death
	demon1_die1 ();
	//monster_demon_respawn();
};*/


void Demon_MeleeAttack()
{
	demon1_atta1 ();
}


/*QUAKED monster_demon1 (1 0 0) (-32 -32 -24) (32 32 64) Ambush

*/
/* UNUSED - OfN -
void() monster_demon1 =
{
	if (CheckExistence() == #FALSE)
	{
		dremove(self);
		return;
	}

	if (deathmatch)
	{
		dremove(self);
		return;
	}
	precache_model ("progs/demon.mdl");
	precache_model ("progs/h_demon.mdl");

	precache_sound ("demon/ddeath.wav");
	precache_sound ("demon/dhit2.wav");
	precache_sound ("demon/djump.wav");
	precache_sound ("demon/dpain1.wav");
	precache_sound ("demon/idle1.wav");
	precache_sound ("demon/sight2.wav");

	self.solid = #SOLID_SLIDEBOX;
	self.movetype = #MOVETYPE_STEP;

	setmodel (self, "progs/demon.mdl");

	setsize (self, #VEC_HULL2_MIN, #VEC_HULL2_MAX);
	self.health = 300;
	self.has_tesla = 0; //CH Jello jump
	self.has_sentry = 0; //CH stuck jump
	self.has_camera = 0; //CH set name as 'demon'

	self.th_stand = demon1_stand1;
	self.th_walk = demon1_walk1;
	self.th_run = demon1_run1;
	self.th_die = demon_die;
	self.th_melee = Demon_MeleeAttack;		// one of two attacks
	self.th_missile = demon1_jump1;			// jump attack
	self.th_pain = demon1_pain;
	self.th_fireball = demon1_fire1; //CH

	walkmonster_start();
};*/


/*
==============================================================================

DEMON

==============================================================================
*/

/*
==============
CheckDemonMelee

Returns #TRUE if a melee attack would hit right now
==============
*/
float CheckDemonMelee()
{
	vector dist;
	float d;

	if (enemy_range == PR_RANGE_MELEE)
	{
		dist = self->enemy->origin - self->origin;
		dist[X] = dist[Y] = 0; //CH only need z
		d = vlen(dist);
		if (d < 100) //CH Check reach
			{
				self->attack_state = PR_AS_MELEE;
				return PR_TRUE;
			}
	}
	return PR_FALSE;
}

/*
==============
CheckDemonFireBall
==============
inp = 0 normal check
inp = 1 check for long range
*/
float CheckDemonFireBall(float inp)
{
	vector	dist;
	float	d;
	float d2;

	if (!visible2(self->enemy, self)) //If can see
		return PR_FALSE;
	dist = self->enemy->origin - self->origin;
	dist[Z] = 0; //CH only need x,y
	d2 = vlen(dist);
	dist = self->enemy->origin - self->origin;
	dist[X] = dist[Y] = 0; //CH only need z
	d = vlen(dist);
	if (inp == 1) //Range attack
	{
		if (random() < 0.9) //CH as not to fire all the time
			return PR_FALSE;
		if (d2 < 600) //Min X,Y distance away before throw fireball
			return PR_FALSE;
		return PR_TRUE;
	}
	else //Normal attack
	{
		if (random() > 0.9) //CH as not to fire all the time
			return PR_FALSE;
		if (d2 < 200) //Min X,Y distance away before throw fireball
			return PR_FALSE;
		if (d < 100) //Min Z distance away before throw fireball
			return PR_FALSE;
		return PR_TRUE;
	}
}

/*
==============
CheckDemonJump
==============
*/
float CheckDemonJump()
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
		if (random() < 0.95) //WK 0.9 Make it jump less often
			return PR_FALSE;
	}

	return PR_TRUE;
}

float DemonCheckAttack()
{
//	local	vector	vec;

// if close enough for slashing, go for it
	if (CheckDemonMelee ())
	{
		self->attack_state = PR_AS_MELEE;
		return PR_TRUE;
	}

	if (CheckDemonJump ())
	{
		self->attack_state = PR_AS_MISSILE;
        sound (self, PR_CHAN_VOICE, "demon/djump.wav", 1, PR_ATTN_NORM);
		return PR_TRUE;
	}

	if (CheckDemonFireBall(0)) //Normal
	{
		self->attack_state = PR_AS_FIREBALL;
		return PR_TRUE;
	}
	if (CheckDemonFireBall(1)) //Ranged
	{
		self->attack_state = PR_AS_FIREBALL;
		return PR_TRUE;
	}

	return PR_FALSE;
}


//===========================================================================

void Demon_Melee(float side)
{
	float	ldmg;
	vector	delta;

	ai_face ();
	// WK walkmove (self.ideal_yaw, 12);	// allow a little closing
	walkmove (self->ideal_yaw, 24);	// allow a lot closing

	delta = self->enemy->origin - self->origin;

	if (vlen(delta) > 120)
		return;
	if (!CanDamage (self->enemy, self))
		return;

	deathmsg = 0; //CH
	sound (self, PR_CHAN_WEAPON, "demon/dhit2.wav", 1, PR_ATTN_NORM);
	//WK ldmg = 10 + 5*random();
	ldmg = 80 + 40*random();
	if (self->enemy->tf_items & PR_NIT_GEL)
		ldmg = ldmg / 2;
	if (self->enemy->cutf_items & PR_CUTF_DEMONLORE)
		ldmg = ldmg * 0.8;

	//T_Damage (self.enemy, self, self, ldmg);
    TF_T_Damage(self->enemy, self, self, ldmg,0,PR_TF_TD_OTHER);

	makevectors (self->angles);
	SpawnMeatSpray (self->origin + v_forward*16, side * v_right);
}

//===========================================================================
//CH demon shoots fire...
void demon_fire_touch();
void Demon_Shoot_Fire(float side)
{
//	local	float	ldmg;
	vector	delta;
	vector	offang;
	vector	org, dir;

	ai_face ();
	walkmove (self->ideal_yaw, 12);	// allow a little closing

	delta = self->enemy->origin - self->origin;
	offang = vectoangles (delta);
	makevectors (offang);

	sound (self, PR_CHAN_WEAPON, "hknight/attack1.wav", 1, PR_ATTN_NORM); //Odd, it was already precached

	if (side > 0) //CH cause to spawn on side of demon.
		org = self->origin + (v_forward * 10) + (v_right * 20);
	else
		org = self->origin + (v_forward * 10) - (v_right * 20);

// set missile speed
	dir = normalize (v_forward);

//CH demons are not good at throwing
	dir[Z] = 0 - dir[Z] + (random() - 0.5)*0.1; //Random Z addage
	dir[X] = dir[X] + (random() - 0.5)*0.05; //Random X addage
	dir[Y] = dir[Y] + (random() - 0.5)*0.05; //Random Y addage

	newmis = spawn ();
	newmis->owner = self;
	newmis->movetype = PR_MOVETYPE_FLYMISSILE;
	newmis->solid = PR_SOLID_BBOX;

	newmis->angles = vectoangles(dir);

	newmis->touch = demon_fire_touch;
	newmis->weapon = PR_DMSG_DEMON_FIRE;
	newmis->classname = "demon_fire";
	newmis->think = SUB_Remove;
	newmis->nextthink = time + 6;
	setmodel (newmis, "progs/lavaball.mdl");
//	setsize (newmis, '-4 -8 -4', '10 8 14'); //CH actual mdl bounds
	setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (newmis, org);
	newmis->velocity = dir * 1000;
}
void demon_fire_touch()
{
	float fire_dmg;
	fire_dmg = 60 + random()*40;
	if (pointcontents(self->origin) == PR_CONTENT_SKY)
	{
		dremove(self);
		return;
	}

	deathmsg = self->weapon;
	T_RadiusDamage (self, self->owner, fire_dmg , self->owner);

	self->origin = self->origin - 8*normalize(self->velocity); //???

#ifdef PR_DEMO_STUFF
	// Remove any camera's locks on this missile
	if (self->enemy != world)
		CamProjectileLockOff();
#endif

	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
	WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
#ifdef PR_QUAKE_WORLD
	multicast (self->origin, PR_MULTICAST_PHS);
	dremove(self);
#else
	BecomeExplosion ();
#endif
}

void Demon_Water_Jump()
{
	traceline(self->origin,self->origin - V({0, 0, 2}),PR_TL_BSP_ONLY,self);
	if (trace_inwater == PR_TRUE && jello)
	{
		ai_face();
		//self.touch = SUB_Null;
        self->touch = MonsterTouch; // OfN
		self->think = demon1_jump4;
		self->has_tesla = PR_TRUE; //Landed in water
		self->nextthink = time + 0.1;
	}
}
void Demon_JumpTouch()
{
	float	ldmg;

	if (self->health <= 0)
		return;
	deathmsg = 0; //CH

	//WK Jello water support
	//CH a working version.
	traceline(self->origin,self->origin - V({0, 0, 2}),PR_TL_BSP_ONLY,self);
	if (trace_inwater == PR_TL_BSP_ONLY && jello)
	{
		//self.touch = SUB_Null;
        self->touch = MonsterTouch; // OfN
		self->think = demon1_jump1;
		self->has_tesla = PR_TRUE; //Landed in water
		self->nextthink = time + 0.1;
		return;
	}

	if (other->takedamage)
	{
		if ( vlen(self->velocity) > 400 )
		{
			//WK ldmg = 40 + 10*random();
			ldmg = 120 + 30*random();
			if (other->cutf_items & PR_CUTF_DEMONLORE)
				ldmg = ldmg * 0.8;

            TF_T_Damage(other, self, self, ldmg,0,PR_TF_TD_OTHER);
			//T_Damage (other, self, self, ldmg);
			//WK Comment out this next line for demon-head hopping action!
			if (self->think == SUB_Remove) return; //We can die when killing owner
		}
	}

	if (!checkbottom(self))
	{
		if (self->flags & PR_FL_ONGROUND)
		{	// jump randomly to not get hung up
			//RPrint ("popjump\n");
			//sprint(self.real_owner,#PRINT_HIGH,"Stuck Jump flag set!\n");
			self->has_sentry = PR_TRUE; //CH Stuck Jump
			//self.touch = SUB_Null;
			self->touch = MonsterTouch; // OfN
            self->think = demon1_jump1;
			self->nextthink = time + 0.1;

			//self.velocity_x = (random() - 0.5) * 600;
			//self.velocity_y = (random() - 0.5) * 600;
			//self.velocity_z = 200;
			//self.flags = self.flags - #FL_ONGROUND;
		}
		return;	// not on ground yet
	}

	self->has_sentry = 0; //Cancel if this far
	//self.touch = SUB_Null;
    self->touch = MonsterTouch; // OfN
	self->think = demon1_jump11;
	self->nextthink = time + 0.1;
}

/*UNUSED - OfN -
void() respawn_demon =
{
	self = self.owner;

	self.origin = self.oldorigin;
	spawn_tfog(self.origin);
	spawn_tdeath(self.origin, self);
	setorigin(self, self.origin);

	self.solid = #SOLID_SLIDEBOX;
	self.movetype = #MOVETYPE_STEP;
	self.takedamage = #DAMAGE_AIM;

	setmodel (self, "progs/demon.mdl");

	setsize (self, #VEC_HULL2_MIN, #VEC_HULL2_MAX);
	self.health = 300;
	self.oldorigin = self.origin;

	self.target = string_null;

	HuntTarget();
	dremove(self.owner);
};*/

/*UNUSED - OfN -
void() monster_demon_respawn =
{
	local entity resp;

	if (self.lives <= 0.5)
		return;

	self.lives = self.lives - 1;

	resp = spawn();

	self.owner = resp;
	resp.owner = self;

	resp.think = respawn_demon;
	resp.nextthink = time + 10;
};*/

/*
 * WK - Pet demon functions
*/

void custom_demon_die()
{
	if (self->real_owner->classname == "player")
	{
		sprint(self->real_owner,PR_PRINT_HIGH,"Your fiend ");
		sprint(self->real_owner,PR_PRINT_HIGH,self->netname);
		sprint(self->real_owner,PR_PRINT_HIGH," is dead.\n");
		self->real_owner->job_finished = time + 5; //Can't summon streams of demons SB can so
        ResetSlotPointer(self->real_owner, self->increase_team1);
	}

    if (self->health < -40)
    {
        sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_MONSTERDIE);
	    ThrowMonsterHead ("progs/h_demon.mdl", self->health);
	    ThrowGib ("progs/gib3.mdl", self->health, PR_TRUE, 0, PR_GIB3_KGS, PR_FALSE);
	    ThrowGib ("progs/gib2.mdl", self->health, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
	    ThrowGib ("progs/gib3.mdl", self->health, PR_TRUE, 0, PR_GIB3_KGS, 0.5);
        SpawnBloodEx(self->origin,0.66,9);

	    dremove(self);
        return;
    }

    // Spawn bubbles if under water/slime
    float contres;
    contres = pointcontents(self->origin);
    if (contres == PR_CONTENT_WATER || contres == PR_CONTENT_SLIME)
        DeathBubbles(5);

    //self.classname = "monster_corpse";
    MonsterDead();
    self->think=SUB_Null;
    demon1_die1 ();
}

} // END namespace Progs
