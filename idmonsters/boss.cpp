/*
	boss.qc
*/

#include "progs.h"

#ifndef PR_COOP_MODE_ENHANCED
//#pragma DONT_COMPILE_THIS_FILE
#else

#include "idmonsters/boss.h"
#include "idmonsters/common.h"
#include "coop_defs.h"
#include "ofndefs.h"
#include "weapons.h"
#include "doors.h"

namespace Progs {

/*
==============================================================================

BOSS-ONE

==============================================================================
*/
//$cd id1/models/boss1
//$origin 0 0 -15
//$base base
//$skin skin
//$scale 5

namespace BossFrames
{
	enum {FR_RISE1, FR_RISE2, FR_RISE3, FR_RISE4, FR_RISE5, FR_RISE6, FR_RISE7, FR_RISE8, FR_RISE9, FR_RISE10};
	enum {FR_RISE11 = 10, FR_RISE12, FR_RISE13, FR_RISE14, FR_RISE15, FR_RISE16, FR_RISE17};

	enum {FR_WALK1 = 17, FR_WALK2, FR_WALK3, FR_WALK4, FR_WALK5, FR_WALK6, FR_WALK7, FR_WALK8};
	enum {FR_WALK9 = 25, FR_WALK10, FR_WALK11, FR_WALK12, FR_WALK13, FR_WALK14, FR_WALK15};
	enum {FR_WALK16 = 32, FR_WALK17, FR_WALK18, FR_WALK19, FR_WALK20, FR_WALK21, FR_WALK22};
	enum {FR_WALK23 = 39, FR_WALK24, FR_WALK25, FR_WALK26, FR_WALK27, FR_WALK28, FR_WALK29, FR_WALK30, FR_WALK31};

	enum {FR_DEATH1 = 48, FR_DEATH2, FR_DEATH3, FR_DEATH4, FR_DEATH5, FR_DEATH6, FR_DEATH7, FR_DEATH8, FR_DEATH9};

	enum {FR_ATTACK1 = 57, FR_ATTACK2, FR_ATTACK3, FR_ATTACK4, FR_ATTACK5, FR_ATTACK6, FR_ATTACK7, FR_ATTACK8};
	enum {FR_ATTACK9 = 65, FR_ATTACK10, FR_ATTACK11, FR_ATTACK12, FR_ATTACK13, FR_ATTACK14, FR_ATTACK15};
	enum {FR_ATTACK16 = 72, FR_ATTACK17, FR_ATTACK18, FR_ATTACK19, FR_ATTACK20, FR_ATTACK21, FR_ATTACK22};
	enum {FR_ATTACK23 = 79};

	enum {FR_SHOCKA1 = 80, FR_SHOCKA2, FR_SHOCKA3, FR_SHOCKA4, FR_SHOCKA5, FR_SHOCKA6, FR_SHOCKA7, FR_SHOCKA8};
	enum {FR_SHOCKA9 = 88, FR_SHOCKA10};

	enum {FR_SHOCKB1 = 90, FR_SHOCKB2, FR_SHOCKB3, FR_SHOCKB4, FR_SHOCKB5, FR_SHOCKB6};

	enum {FR_SHOCKC1 = 96, FR_SHOCKC2, FR_SHOCKC3, FR_SHOCKC4, FR_SHOCKC5, FR_SHOCKC6, FR_SHOCKC7, FR_SHOCKC8};
	enum {FR_SHOCKC9 = 104, FR_SHOCKC10};
}

void boss_missile(const vector& p);

void boss_face()
{

// go for another player if multi player
	if (self->enemy->health <= 0 || random() < 0.02)
	{
		self->enemy = find(self->enemy, "classname", "player");
		if (self->enemy == world)
			self->enemy = find(self->enemy, "classname", "player");
	}
	COOP_Turn ();
}

void boss_rise1() {
FRAME_STATE(BossFrames::FR_RISE1, boss_rise2);
sound (self, PR_CHAN_WEAPON, "boss1/out1.wav", 1, PR_ATTN_NORM);
}
void boss_rise2() {
FRAME_STATE(BossFrames::FR_RISE2, boss_rise3);
sound (self, PR_CHAN_VOICE, "boss1/sight1.wav", 1, PR_ATTN_NORM);
}
void boss_rise3() {FRAME_STATE(BossFrames::FR_RISE3, boss_rise4);}
void boss_rise4() {FRAME_STATE(BossFrames::FR_RISE4, boss_rise5);}
void boss_rise5() {FRAME_STATE(BossFrames::FR_RISE5, boss_rise6);}
void boss_rise6() {FRAME_STATE(BossFrames::FR_RISE6, boss_rise7);}
void boss_rise7() {FRAME_STATE(BossFrames::FR_RISE7, boss_rise8);}
void boss_rise8() {FRAME_STATE(BossFrames::FR_RISE8, boss_rise9);}
void boss_rise9() {FRAME_STATE(BossFrames::FR_RISE9, boss_rise10);}
void boss_rise10() {FRAME_STATE(BossFrames::FR_RISE10, boss_rise11);}
void boss_rise11() {FRAME_STATE(BossFrames::FR_RISE11, boss_rise12);}
void boss_rise12() {FRAME_STATE(BossFrames::FR_RISE12, boss_rise13);}
void boss_rise13() {FRAME_STATE(BossFrames::FR_RISE13, boss_rise14);}
void boss_rise14() {FRAME_STATE(BossFrames::FR_RISE14, boss_rise15);}
void boss_rise15() {FRAME_STATE(BossFrames::FR_RISE15, boss_rise16);}
void boss_rise16() {FRAME_STATE(BossFrames::FR_RISE16, boss_rise17);}
void boss_rise17() {FRAME_STATE(BossFrames::FR_RISE17, boss_missile1);}

void boss_idle1()
{
// look for other players
FRAME_STATE(BossFrames::FR_WALK1, boss_idle2);
}
void boss_idle2() {FRAME_STATE(BossFrames::FR_WALK2, boss_idle3); boss_face();}
void boss_idle3() {FRAME_STATE(BossFrames::FR_WALK3, boss_idle4); boss_face();}
void boss_idle4() {FRAME_STATE(BossFrames::FR_WALK4, boss_idle5); boss_face();}
void boss_idle5() {FRAME_STATE(BossFrames::FR_WALK5, boss_idle6); boss_face();}
void boss_idle6() {FRAME_STATE(BossFrames::FR_WALK6, boss_idle7); boss_face();}
void boss_idle7() {FRAME_STATE(BossFrames::FR_WALK7, boss_idle8); boss_face();}
void boss_idle8() {FRAME_STATE(BossFrames::FR_WALK8, boss_idle9); boss_face();}
void boss_idle9() {FRAME_STATE(BossFrames::FR_WALK9, boss_idle10); boss_face();}
void boss_idle10() {FRAME_STATE(BossFrames::FR_WALK10, boss_idle11); boss_face();}
void boss_idle11() {FRAME_STATE(BossFrames::FR_WALK11, boss_idle12); boss_face();}
void boss_idle12() {FRAME_STATE(BossFrames::FR_WALK12, boss_idle13); boss_face();}
void boss_idle13() {FRAME_STATE(BossFrames::FR_WALK13, boss_idle14); boss_face();}
void boss_idle14() {FRAME_STATE(BossFrames::FR_WALK14, boss_idle15); boss_face();}
void boss_idle15() {FRAME_STATE(BossFrames::FR_WALK15, boss_idle16); boss_face();}
void boss_idle16() {FRAME_STATE(BossFrames::FR_WALK16, boss_idle17); boss_face();}
void boss_idle17() {FRAME_STATE(BossFrames::FR_WALK17, boss_idle18); boss_face();}
void boss_idle18() {FRAME_STATE(BossFrames::FR_WALK18, boss_idle19); boss_face();}
void boss_idle19() {FRAME_STATE(BossFrames::FR_WALK19, boss_idle20); boss_face();}
void boss_idle20() {FRAME_STATE(BossFrames::FR_WALK20, boss_idle21); boss_face();}
void boss_idle21() {FRAME_STATE(BossFrames::FR_WALK21, boss_idle22); boss_face();}
void boss_idle22() {FRAME_STATE(BossFrames::FR_WALK22, boss_idle23); boss_face();}
void boss_idle23() {FRAME_STATE(BossFrames::FR_WALK23, boss_idle24); boss_face();}
void boss_idle24() {FRAME_STATE(BossFrames::FR_WALK24, boss_idle25); boss_face();}
void boss_idle25() {FRAME_STATE(BossFrames::FR_WALK25, boss_idle26); boss_face();}
void boss_idle26() {FRAME_STATE(BossFrames::FR_WALK26, boss_idle27); boss_face();}
void boss_idle27() {FRAME_STATE(BossFrames::FR_WALK27, boss_idle28); boss_face();}
void boss_idle28() {FRAME_STATE(BossFrames::FR_WALK28, boss_idle29); boss_face();}
void boss_idle29() {FRAME_STATE(BossFrames::FR_WALK29, boss_idle30); boss_face();}
void boss_idle30() {FRAME_STATE(BossFrames::FR_WALK30, boss_idle31); boss_face();}
void boss_idle31() {FRAME_STATE(BossFrames::FR_WALK31, boss_idle1); boss_face();}

void boss_missile1() {FRAME_STATE(BossFrames::FR_ATTACK1, boss_missile2); boss_face();}
void boss_missile2() {FRAME_STATE(BossFrames::FR_ATTACK2, boss_missile3); boss_face();}
void boss_missile3() {FRAME_STATE(BossFrames::FR_ATTACK3, boss_missile4); boss_face();}
void boss_missile4() {FRAME_STATE(BossFrames::FR_ATTACK4, boss_missile5); boss_face();}
void boss_missile5() {FRAME_STATE(BossFrames::FR_ATTACK5, boss_missile6); boss_face();}
void boss_missile6() {FRAME_STATE(BossFrames::FR_ATTACK6, boss_missile7); boss_face();}
void boss_missile7() {FRAME_STATE(BossFrames::FR_ATTACK7, boss_missile8); boss_face();}
void boss_missile8() {FRAME_STATE(BossFrames::FR_ATTACK8, boss_missile9); boss_face();}
void boss_missile9() {FRAME_STATE(BossFrames::FR_ATTACK9, boss_missile10); boss_missile(V({100, 100, 200}));}
void boss_missile10() {FRAME_STATE(BossFrames::FR_ATTACK10, boss_missile11); boss_face();}
void boss_missile11() {FRAME_STATE(BossFrames::FR_ATTACK11, boss_missile12); boss_face();}
void boss_missile12() {FRAME_STATE(BossFrames::FR_ATTACK12, boss_missile13); boss_face();}
void boss_missile13() {FRAME_STATE(BossFrames::FR_ATTACK13, boss_missile14); boss_face();}
void boss_missile14() {FRAME_STATE(BossFrames::FR_ATTACK14, boss_missile15); boss_face();}
void boss_missile15() {FRAME_STATE(BossFrames::FR_ATTACK15, boss_missile16); boss_face();}
void boss_missile16() {FRAME_STATE(BossFrames::FR_ATTACK16, boss_missile17); boss_face();}
void boss_missile17() {FRAME_STATE(BossFrames::FR_ATTACK17, boss_missile18); boss_face();}
void boss_missile18() {FRAME_STATE(BossFrames::FR_ATTACK18, boss_missile19); boss_face();}
void boss_missile19() {FRAME_STATE(BossFrames::FR_ATTACK19, boss_missile20); boss_face();}
void boss_missile20() {FRAME_STATE(BossFrames::FR_ATTACK20, boss_missile21); boss_missile(V({100, -100, 200}));}
void boss_missile21() {FRAME_STATE(BossFrames::FR_ATTACK21, boss_missile22); boss_face();}
void boss_missile22() {FRAME_STATE(BossFrames::FR_ATTACK22, boss_missile23); boss_face();}
void boss_missile23() {FRAME_STATE(BossFrames::FR_ATTACK23, boss_missile1); boss_face();}

void boss_shocka1() {FRAME_STATE(BossFrames::FR_SHOCKA1, boss_shocka2);}
void boss_shocka2() {FRAME_STATE(BossFrames::FR_SHOCKA2, boss_shocka3);}
void boss_shocka3() {FRAME_STATE(BossFrames::FR_SHOCKA3, boss_shocka4);}
void boss_shocka4() {FRAME_STATE(BossFrames::FR_SHOCKA4, boss_shocka5);}
void boss_shocka5() {FRAME_STATE(BossFrames::FR_SHOCKA5, boss_shocka6);}
void boss_shocka6() {FRAME_STATE(BossFrames::FR_SHOCKA6, boss_shocka7);}
void boss_shocka7() {FRAME_STATE(BossFrames::FR_SHOCKA7, boss_shocka8);}
void boss_shocka8() {FRAME_STATE(BossFrames::FR_SHOCKA8, boss_shocka9);}
void boss_shocka9() {FRAME_STATE(BossFrames::FR_SHOCKA9, boss_shocka10);}
void boss_shocka10() {FRAME_STATE(BossFrames::FR_SHOCKA10, boss_missile1);}

void boss_shockb1() {FRAME_STATE(BossFrames::FR_SHOCKB1, boss_shockb2);}
void boss_shockb2() {FRAME_STATE(BossFrames::FR_SHOCKB2, boss_shockb3);}
void boss_shockb3() {FRAME_STATE(BossFrames::FR_SHOCKB3, boss_shockb4);}
void boss_shockb4() {FRAME_STATE(BossFrames::FR_SHOCKB4, boss_shockb5);}
void boss_shockb5() {FRAME_STATE(BossFrames::FR_SHOCKB5, boss_shockb6);}
void boss_shockb6() {FRAME_STATE(BossFrames::FR_SHOCKB6, boss_shockb7);}
void boss_shockb7() {FRAME_STATE(BossFrames::FR_SHOCKB1, boss_shockb8);}
void boss_shockb8() {FRAME_STATE(BossFrames::FR_SHOCKB2, boss_shockb9);}
void boss_shockb9() {FRAME_STATE(BossFrames::FR_SHOCKB3, boss_shockb10);}
void boss_shockb10() {FRAME_STATE(BossFrames::FR_SHOCKB4, boss_missile1);}

void boss_shockc1() {FRAME_STATE(BossFrames::FR_SHOCKC1, boss_shockc2);}
void boss_shockc2() {FRAME_STATE(BossFrames::FR_SHOCKC2, boss_shockc3);}
void boss_shockc3() {FRAME_STATE(BossFrames::FR_SHOCKC3, boss_shockc4);}
void boss_shockc4() {FRAME_STATE(BossFrames::FR_SHOCKC4, boss_shockc5);}
void boss_shockc5() {FRAME_STATE(BossFrames::FR_SHOCKC5, boss_shockc6);}
void boss_shockc6() {FRAME_STATE(BossFrames::FR_SHOCKC6, boss_shockc7);}
void boss_shockc7() {FRAME_STATE(BossFrames::FR_SHOCKC7, boss_shockc8);}
void boss_shockc8() {FRAME_STATE(BossFrames::FR_SHOCKC8, boss_shockc9);}
void boss_shockc9() {FRAME_STATE(BossFrames::FR_SHOCKC9, boss_shockc10);}
void boss_shockc10() {FRAME_STATE(BossFrames::FR_SHOCKC10, boss_death1);}

void boss_death1()  {
FRAME_STATE(BossFrames::FR_DEATH1, boss_death2);
sound (self, PR_CHAN_VOICE, "boss1/death.wav", 1, PR_ATTN_NORM);
}
void boss_death2()  {FRAME_STATE(BossFrames::FR_DEATH2, boss_death3);}
void boss_death3()  {FRAME_STATE(BossFrames::FR_DEATH3, boss_death4);}
void boss_death4()  {FRAME_STATE(BossFrames::FR_DEATH4, boss_death5);}
void boss_death5()  {FRAME_STATE(BossFrames::FR_DEATH5, boss_death6);}
void boss_death6()  {FRAME_STATE(BossFrames::FR_DEATH6, boss_death7);}
void boss_death7()  {FRAME_STATE(BossFrames::FR_DEATH7, boss_death8);}
void boss_death8()  {FRAME_STATE(BossFrames::FR_DEATH8, boss_death9);}
void boss_death9()
{
	FRAME_STATE(BossFrames::FR_DEATH9, boss_death10);
	sound (self, PR_CHAN_BODY, "boss1/out1.wav", 1, PR_ATTN_NORM);
	WriteByte (PR_MSG_MULTICAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_MULTICAST, PR_TE_LAVASPLASH);
	WriteCoord (PR_MSG_MULTICAST, self->origin[X]);
	WriteCoord (PR_MSG_MULTICAST, self->origin[Y]);
	WriteCoord (PR_MSG_MULTICAST, self->origin[Z]);
	multicast (self->origin, PR_MULTICAST_PHS);
}

void boss_death10()
{
	FRAME_STATE(BossFrames::FR_DEATH9, boss_death10);
	killed_monsters = killed_monsters + 1;
	WriteByte (PR_MSG_ALL, PR_SVC_KILLEDMONSTER);	// FIXME: reliable MULTICAST
	SUB_UseTargets ();
	remove (self);
}

void boss_missile(const vector& p)
{
	vector	offang;
	vector	org, vec, d;
	float	t;

	offang = vectoangles (self->enemy->origin - self->origin);
	makevectors (offang);

	org = self->origin + p[X]*v_forward + p[Y]*v_right + p[Z]*V({0, 0, 1});

// lead the player on hard mode

	t = vlen(self->enemy->origin - org) / 300;
	vec = self->enemy->velocity;
	vec[Z] = 0;
	d = self->enemy->origin + t * vec;

	vec = normalize (d - org);

	launch_spike (org, vec);
	setmodel (newmis, "progs/lavaball.mdl");
	newmis->avelocity = V({200, 100, 300});
	setsize (newmis, PR_VEC_ORIGIN, PR_VEC_ORIGIN);
	newmis->velocity = vec*300;
	newmis->touch = T_MissileTouch; // rocket explosion
	sound (self, PR_CHAN_WEAPON, "boss1/throw.wav", 1, PR_ATTN_NORM);

// check for dead enemy
	if (self->enemy->health <= 0)
		boss_idle1 ();
}


void boss_awake()
{
	self->solid = PR_SOLID_SLIDEBOX;
	self->movetype = PR_MOVETYPE_STEP;
	self->takedamage = PR_DAMAGE_NO;

	setmodel (self, "progs/boss.mdl");
	setsize (self, V({-128, -128, -24}), V({128, 128, 256}));

	self->health = 3;

	self->enemy = activator;

	WriteByte (PR_MSG_MULTICAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_MULTICAST, PR_TE_LAVASPLASH);
	WriteCoord (PR_MSG_MULTICAST, self->origin[X]);
	WriteCoord (PR_MSG_MULTICAST, self->origin[Y]);
	WriteCoord (PR_MSG_MULTICAST, self->origin[Z]);
	multicast (self->origin, PR_MULTICAST_PHS);

	self->yaw_speed = 20;
	boss_rise1 ();
}


/*QUAKED monster_boss (1 0 0) (-128 -128 -24) (128 128 256)
*/
void monster_boss()
{
	if ( deathmatch ) {
		remove( self );
		return;
	}

	self->classname = "idmonster_boss";

	precache_model ("progs/boss.mdl");
	precache_model ("progs/lavaball.mdl");

	precache_sound ("weapons/rocket1i.wav");
	precache_sound ("bossout1.wav");
	precache_sound ("boss1/sight11/.wav");
	precache_sound ("misc/power.wav");
	precache_sound ("boss1/throw.wav");
	precache_sound ("boss1/pain.wav");
	precache_sound ("boss1/death.wav");


	// these really aren't needed...
	self->monsterweight = PR_MWEIGHT_HUGE;

	if ( !self->team_no )
		self->team_no = PR_TEAMNO_MONSTERS;
	self->PR_monsterflag = PR_STRFLAG_MONSTER;

	self->is_connected = PR_TRUE;


	total_monsters = total_monsters + 1;

	self->use = boss_awake;
}

//===========================================================================

entity	le1, le2;
float	lightning_end;

void lightning_fire()
{
	vector p1, p2;

	if (time >= lightning_end)
	{	// done here, put the terminals back up
		self = le1;
		door_go_down ();
		self = le2;
		door_go_down ();
		return;
	}

	p1 = (le1->mins + le1->maxs) * 0.5;
	p1[Z] = le1->absmin[Z] - 16;

	p2 = (le2->mins + le2->maxs) * 0.5;
	p2[Z] = le2->absmin[Z] - 16;

	// compensate for length of bolt
	p2 = p2 - normalize(p2-p1)*100;

	self->nextthink = time + 0.1;
	self->think = lightning_fire;

	WriteByte (PR_MSG_ALL, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_ALL, PR_TE_LIGHTNING3);
	WriteEntity (PR_MSG_ALL, world);
	WriteCoord (PR_MSG_ALL, p1[X]);
	WriteCoord (PR_MSG_ALL, p1[Y]);
	WriteCoord (PR_MSG_ALL, p1[Z]);
	WriteCoord (PR_MSG_ALL, p2[X]);
	WriteCoord (PR_MSG_ALL, p2[Y]);
	WriteCoord (PR_MSG_ALL, p2[Z]);
	//multicast (p1, #MULTICAST_PHS);
}

void lightning_use()
{
	if (lightning_end >= time + 1)
		return;

	le1 = find( world, "target", "lightning");
	le2 = find( le1, "target", "lightning");
	if (le1  == world|| le2 == world)
	{
		dprint ("missing lightning targets\n");
		return;
	}

	if (
	 (le1->state != PR_STATE_TOP && le1->state != PR_STATE_BOTTOM)
	|| (le2->state != PR_STATE_TOP && le2->state != PR_STATE_BOTTOM)
	|| (le1->state != le2->state) )
	{
//		dprint ("not aligned\n");
		return;
	}

// don't let the electrodes go back up until the bolt is done
	le1->nextthink = -1;
	le2->nextthink = -1;
	lightning_end = time + 1;

	sound (self, PR_CHAN_VOICE, "misc/power.wav", 1, PR_ATTN_NORM);
	lightning_fire ();

// advance the boss pain if down
	self = find (world, "classname", "idmonster_boss");
	if (self == world)
		return;
	self->enemy = activator;
	if (le1->state == PR_STATE_TOP && self->health > 0)
	{
		sound (self, PR_CHAN_VOICE, "boss1/pain.wav", 1, PR_ATTN_NORM);
		self->health = self->health - 1;
		if (self->health >= 2)
			boss_shocka1();
		else if (self->health == 1)
			boss_shockb1();
		else if (self->health == 0)
			boss_shockc1();
	}
}


/*QUAKED event_lightning (0 1 1) (-16 -16 -16) (16 16 16)
Just for boss level.
*/
void event_lightning()
{
	self->use = lightning_use;
}

} // END namespace Progs

#endif
