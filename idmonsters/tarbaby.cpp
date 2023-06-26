/*
	tarbaby.qc
*/

#include "progs.h"

#ifndef PR_COOP_MODE_ENHANCED
//#pragma DONT_COMPILE_THIS_FILE
#else

#include "idmonsters/tarbaby.h"
#include "idmonsters/common.h"
#include "idmonsters/coop_monsters.h"
#include "coop_defs.h"
#include "ofndefs.h"
#include "combat.h"

namespace Progs {

#define PR_IDMON_TARBABY_HEALTH	275
#define PR_IDMON_TARBABY_HITDMG	50

/*
==============================================================================

BLOB

==============================================================================
*/

//$cd id1/models/tarbaby
//$origin 0 0 24
//$base base
//$skin skin

namespace TarbabyFrames
{
	enum {FR_WALK1, FR_WALK2, FR_WALK3, FR_WALK4, FR_WALK5, FR_WALK6, FR_WALK7, FR_WALK8, FR_WALK9, FR_WALK10};
	enum {FR_WALK11 = 10, FR_WALK12, FR_WALK13, FR_WALK14, FR_WALK15, FR_WALK16, FR_WALK17, FR_WALK18, FR_WALK19};
	enum {FR_WALK20 = 19, FR_WALK21, FR_WALK22, FR_WALK23, FR_WALK24, FR_WALK25};

	enum {FR_RUN1 = 25, FR_RUN2, FR_RUN3, FR_RUN4, FR_RUN5, FR_RUN6, FR_RUN7, FR_RUN8, FR_RUN9, FR_RUN10, FR_RUN11, FR_RUN12, FR_RUN13};
	enum {FR_RUN14 = 38, FR_RUN15, FR_RUN16, FR_RUN17, FR_RUN18, FR_RUN19, FR_RUN20, FR_RUN21, FR_RUN22, FR_RUN23};
	enum {FR_RUN24 = 48, FR_RUN25};

	enum {FR_JUMP1 = 50, FR_JUMP2, FR_JUMP3, FR_JUMP4, FR_JUMP5, FR_JUMP6};

	enum {FR_FLY1 = 56, FR_FLY2, FR_FLY3, FR_FLY4};

	enum {FR_EXP = 60};
}

void tbaby_stand1() {FRAME_STATE(TarbabyFrames::FR_WALK1, tbaby_stand1); COOP_Idle();}

void tbaby_hang1() {FRAME_STATE(TarbabyFrames::FR_WALK1, tbaby_hang1); COOP_Idle();}

void tbaby_walk1() {FRAME_STATE(TarbabyFrames::FR_WALK1, tbaby_walk2); COOP_Turn();}
void tbaby_walk2() {FRAME_STATE(TarbabyFrames::FR_WALK2, tbaby_walk3); COOP_Turn();}
void tbaby_walk3() {FRAME_STATE(TarbabyFrames::FR_WALK3, tbaby_walk4); COOP_Turn();}
void tbaby_walk4() {FRAME_STATE(TarbabyFrames::FR_WALK4, tbaby_walk5); COOP_Turn();}
void tbaby_walk5() {FRAME_STATE(TarbabyFrames::FR_WALK5, tbaby_walk6); COOP_Turn();}
void tbaby_walk6() {FRAME_STATE(TarbabyFrames::FR_WALK6, tbaby_walk7); COOP_Turn();}
void tbaby_walk7() {FRAME_STATE(TarbabyFrames::FR_WALK7, tbaby_walk8); COOP_Turn();}
void tbaby_walk8() {FRAME_STATE(TarbabyFrames::FR_WALK8, tbaby_walk9); COOP_Turn();}
void tbaby_walk9() {FRAME_STATE(TarbabyFrames::FR_WALK9, tbaby_walk10); COOP_Turn();}
void tbaby_walk10() {FRAME_STATE(TarbabyFrames::FR_WALK10, tbaby_walk11); COOP_Turn();}
void tbaby_walk11() {FRAME_STATE(TarbabyFrames::FR_WALK11, tbaby_walk12); COOP_Walk(2);}
void tbaby_walk12() {FRAME_STATE(TarbabyFrames::FR_WALK12, tbaby_walk13); COOP_Walk(2);}
void tbaby_walk13() {FRAME_STATE(TarbabyFrames::FR_WALK13, tbaby_walk14); COOP_Walk(2);}
void tbaby_walk14() {FRAME_STATE(TarbabyFrames::FR_WALK14, tbaby_walk15); COOP_Walk(2);}
void tbaby_walk15() {FRAME_STATE(TarbabyFrames::FR_WALK15, tbaby_walk16); COOP_Walk(2);}
void tbaby_walk16() {FRAME_STATE(TarbabyFrames::FR_WALK16, tbaby_walk17); COOP_Walk(2);}
void tbaby_walk17() {FRAME_STATE(TarbabyFrames::FR_WALK17, tbaby_walk18); COOP_Walk(2);}
void tbaby_walk18() {FRAME_STATE(TarbabyFrames::FR_WALK18, tbaby_walk19); COOP_Walk(2);}
void tbaby_walk19() {FRAME_STATE(TarbabyFrames::FR_WALK19, tbaby_walk20); COOP_Walk(2);}
void tbaby_walk20() {FRAME_STATE(TarbabyFrames::FR_WALK20, tbaby_walk21); COOP_Walk(2);}
void tbaby_walk21() {FRAME_STATE(TarbabyFrames::FR_WALK21, tbaby_walk22); COOP_Walk(2);}
void tbaby_walk22() {FRAME_STATE(TarbabyFrames::FR_WALK22, tbaby_walk23); COOP_Walk(2);}
void tbaby_walk23() {FRAME_STATE(TarbabyFrames::FR_WALK23, tbaby_walk24); COOP_Walk(2);}
void tbaby_walk24() {FRAME_STATE(TarbabyFrames::FR_WALK24, tbaby_walk25); COOP_Walk(2);}
void tbaby_walk25() {FRAME_STATE(TarbabyFrames::FR_WALK25, tbaby_walk1); COOP_Walk(2);}

void tbaby_run1() {FRAME_STATE(TarbabyFrames::FR_RUN1, tbaby_run2); COOP_Turn();}
void tbaby_run2() {FRAME_STATE(TarbabyFrames::FR_RUN2, tbaby_run3); COOP_Turn();}
void tbaby_run3() {FRAME_STATE(TarbabyFrames::FR_RUN3, tbaby_run4); COOP_Turn();}
void tbaby_run4() {FRAME_STATE(TarbabyFrames::FR_RUN4, tbaby_run5); COOP_Turn();}
void tbaby_run5() {FRAME_STATE(TarbabyFrames::FR_RUN5, tbaby_run6); COOP_Turn();}
void tbaby_run6() {FRAME_STATE(TarbabyFrames::FR_RUN6, tbaby_run7); COOP_Turn();}
void tbaby_run7() {FRAME_STATE(TarbabyFrames::FR_RUN7, tbaby_run8); COOP_Turn();}
void tbaby_run8() {FRAME_STATE(TarbabyFrames::FR_RUN8, tbaby_run9); COOP_Turn();}
void tbaby_run9() {FRAME_STATE(TarbabyFrames::FR_RUN9, tbaby_run10); COOP_Turn();}
void tbaby_run10() {FRAME_STATE(TarbabyFrames::FR_RUN10, tbaby_run11); COOP_Turn();}
void tbaby_run11() {FRAME_STATE(TarbabyFrames::FR_RUN11, tbaby_run12); COOP_Run(2);}
void tbaby_run12() {FRAME_STATE(TarbabyFrames::FR_RUN12, tbaby_run13); COOP_Run(2);}
void tbaby_run13() {FRAME_STATE(TarbabyFrames::FR_RUN13, tbaby_run14); COOP_Run(2);}
void tbaby_run14() {FRAME_STATE(TarbabyFrames::FR_RUN14, tbaby_run15); COOP_Run(2);}
void tbaby_run15() {FRAME_STATE(TarbabyFrames::FR_RUN15, tbaby_run16); COOP_Run(2);}
void tbaby_run16() {FRAME_STATE(TarbabyFrames::FR_RUN16, tbaby_run17); COOP_Run(2);}
void tbaby_run17() {FRAME_STATE(TarbabyFrames::FR_RUN17, tbaby_run18); COOP_Run(2);}
void tbaby_run18() {FRAME_STATE(TarbabyFrames::FR_RUN18, tbaby_run19); COOP_Run(2);}
void tbaby_run19() {FRAME_STATE(TarbabyFrames::FR_RUN19, tbaby_run20); COOP_Run(2);}
void tbaby_run20() {FRAME_STATE(TarbabyFrames::FR_RUN20, tbaby_run21); COOP_Run(2);}
void tbaby_run21() {FRAME_STATE(TarbabyFrames::FR_RUN21, tbaby_run22); COOP_Run(2);}
void tbaby_run22() {FRAME_STATE(TarbabyFrames::FR_RUN22, tbaby_run23); COOP_Run(2);}
void tbaby_run23() {FRAME_STATE(TarbabyFrames::FR_RUN23, tbaby_run24); COOP_Run(2);}
void tbaby_run24() {FRAME_STATE(TarbabyFrames::FR_RUN24, tbaby_run25); COOP_Run(2);}
void tbaby_run25() {FRAME_STATE(TarbabyFrames::FR_RUN25, tbaby_run1); COOP_Run(2);}


//============================================================================


void tbaby_jump1();

void Tar_JumpTouch()
{
	float	ldmg;

	if (other->takedamage && other->classname != self->classname)
	{
		if ( vlen(self->velocity) > 400 )
		{
			ldmg = PR_IDMON_TARBABY_HITDMG + 10*random();
			TF_T_Damage (other, self, self, ldmg, PR_TF_TD_NOTTEAM, PR_TF_TD_MELEE);
			sound (self, PR_CHAN_WEAPON, "blob/hit1.wav", 1, PR_ATTN_NORM);
		}
	}
	else
		sound (self, PR_CHAN_WEAPON, "blob/land1.wav", 1, PR_ATTN_NORM);


	if (!checkbottom(self))
	{
		if (self->flags & PR_FL_ONGROUND)
		{	// jump randomly to not get hung up
//dprint ("popjump\n");
	self->touch = SUB_Null;
	self->think = tbaby_run1;
	self->movetype = PR_MOVETYPE_STEP;
	self->nextthink = time + 0.1;

//			self.velocity_x = (random() - 0.5) * 600;
//			self.velocity_y = (random() - 0.5) * 600;
//			self.velocity_z = 200;
//			self.flags = self.flags - #FL_ONGROUND;
		}
		return;	// not on ground yet
	}

	self->touch = SUB_Null;
	self->think = tbaby_jump1;
	self->nextthink = time + 0.1;
}

void tbaby_jump5();

void tbaby_fly1() {FRAME_STATE(TarbabyFrames::FR_FLY1, tbaby_fly2);}
void tbaby_fly2() {FRAME_STATE(TarbabyFrames::FR_FLY2, tbaby_fly3);}
void tbaby_fly3() {FRAME_STATE(TarbabyFrames::FR_FLY3, tbaby_fly4);}
void tbaby_fly4() {
FRAME_STATE(TarbabyFrames::FR_FLY4, tbaby_fly1);
self->cnt = self->cnt + 1;
if (self->cnt == 4)
{
//dprint ("spawn hop\n");
tbaby_jump5 ();
}
}

void tbaby_jump1() {FRAME_STATE(TarbabyFrames::FR_JUMP1, tbaby_jump2); COOP_Turn();}
void tbaby_jump2() {FRAME_STATE(TarbabyFrames::FR_JUMP2, tbaby_jump3); COOP_Turn();}
void tbaby_jump3() {FRAME_STATE(TarbabyFrames::FR_JUMP3, tbaby_jump4); COOP_Turn();}
void tbaby_jump4() {FRAME_STATE(TarbabyFrames::FR_JUMP4, tbaby_jump5); COOP_Turn();}
void tbaby_jump5()
{
	FRAME_STATE(TarbabyFrames::FR_JUMP5, tbaby_jump6);
	self->movetype = PR_MOVETYPE_BOUNCE;
	self->touch = Tar_JumpTouch;
	makevectors (self->angles);
	self->origin[Z] = self->origin[Z] + 1;
	self->velocity = v_forward * 600 + V({0, 0, 200});
	self->velocity[Z] = self->velocity[Z] + random()*150;
	if (self->flags & PR_FL_ONGROUND)
		self->flags = self->flags - PR_FL_ONGROUND;
	self->cnt = 0;
}
void tbaby_jump6() {FRAME_STATE(TarbabyFrames::FR_JUMP6, tbaby_fly1);}



//=============================================================================

void tbaby_die1() {
FRAME_STATE(TarbabyFrames::FR_EXP, tbaby_die2);
self->takedamage = PR_DAMAGE_NO;
}
void tbaby_die2()
{
	FRAME_STATE(TarbabyFrames::FR_EXP, tbaby_run1);
	T_RadiusDamage (self, self, 120, world);

	sound (self, PR_CHAN_VOICE, "blob/death1.wav", 1, PR_ATTN_NORM);
	self->origin = self->origin - 8*normalize(self->velocity);

	WriteByte (PR_MSG_MULTICAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_MULTICAST, PR_TE_TAREXPLOSION);
	WriteCoord (PR_MSG_MULTICAST, self->origin[X]);
	WriteCoord (PR_MSG_MULTICAST, self->origin[Y]);
	WriteCoord (PR_MSG_MULTICAST, self->origin[Z]);
	multicast (self->origin, PR_MULTICAST_PHS);

	remove (self);
}

//=============================================================================

float TarbabyOnSight()
{
	sound (self, PR_CHAN_VOICE, "blob/sight1.wav", 1, PR_ATTN_NORM);
	return PR_TRUE;
}


/*QUAKED monster_tarbaby (1 0 0) (-16 -16 -24) (16 16 24) Ambush
*/
void monster_tarbaby()
{
	if ( deathmatch ) {
		remove( self );
		return;
	}

	self->classname = "idmonster_tarbaby";

	if (!server_spawned) {
		precache_model2 ("progs/tarbaby.mdl");

		precache_sound2 ("blob/death1.wav");
		precache_sound2 ("blob/hit1.wav");
		precache_sound2 ("blob/land1.wav");
		precache_sound2 ("blob/sight1.wav");
	}

	self->solid = PR_SOLID_SLIDEBOX;
	self->movetype = PR_MOVETYPE_STEP;

	// tarbaby doesn't gib, so no need for a head
	setmodel (self, "progs/tarbaby.mdl");

	setsize (self, V({-16, -16, -24}), V({16, 16, 40}));
	self->max_health = self->health = PR_IDMON_TARBABY_HEALTH;

	self->th_stand = tbaby_stand1;
	self->th_walk = tbaby_walk1;
	self->th_run = tbaby_run1;
	self->th_missile = tbaby_jump1;
	self->th_melee = tbaby_jump1;
	self->th_die = tbaby_die1;


	self->netname = "spawn";

	self->monsterweight = PR_MWEIGHT_SMALL;
	self->monsterflags = PR_MFLAG_NOHEART | PR_MFLAG_NOLEGS;

	// monsters have a team of their own
	if ( !self->team_no )
		self->team_no = PR_TEAMNO_MONSTERS;
	self->PR_monsterflag = PR_STRFLAG_MONSTER;

	self->OnNoticeEnemy = TarbabyOnSight;
	self->PR_npc_spawnfunc = monster_tarbaby;
	COOP_SetLives ();

	self->PR_npc_explosionmult = 1.25;

	coop_walkmonster_start ();
}

} // END namespace Progs

#endif
