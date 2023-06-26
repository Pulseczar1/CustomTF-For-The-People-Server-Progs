/*
	oldone.qc
*/

#include "progs.h"

#ifndef PR_COOP_MODE_ENHANCED
//#pragma DONT_COMPILE_THIS_FILE
#else

#include "idmonsters/oldone.h"
#include "coop_defs.h"
#include "ofndefs.h"
#include "player.h"

namespace Progs {

#define PR_IDMON_OLDONE_HEALTH		40000

/*
==============================================================================

OLD ONE

==============================================================================
*/
//$cd id1/models/old_one
//$origin 0 0 24
//$base base
//$skin skin
//$scale 1

void finale_1();
void finale_2();
void finale_3();
void finale_4();

entity	shub;

namespace OldOneFrames
{
	enum {FR_OLD1, FR_OLD2, FR_OLD3, FR_OLD4, FR_OLD5, FR_OLD6, FR_OLD7, FR_OLD8, FR_OLD9};
	enum {FR_OLD10 = 9, FR_OLD11, FR_OLD12, FR_OLD13, FR_OLD14, FR_OLD15, FR_OLD16, FR_OLD17, FR_OLD18, FR_OLD19};
	enum {FR_OLD20 = 19, FR_OLD21, FR_OLD22, FR_OLD23, FR_OLD24, FR_OLD25, FR_OLD26, FR_OLD27, FR_OLD28, FR_OLD29};
	enum {FR_OLD30 = 29, FR_OLD31, FR_OLD32, FR_OLD33, FR_OLD34, FR_OLD35, FR_OLD36, FR_OLD37, FR_OLD38, FR_OLD39};
	enum {FR_OLD40 = 39, FR_OLD41, FR_OLD42, FR_OLD43, FR_OLD44, FR_OLD45, FR_OLD46};

	enum {FR_SHAKE1 = 46, FR_SHAKE2, FR_SHAKE3, FR_SHAKE4, FR_SHAKE5, FR_SHAKE6, FR_SHAKE7, FR_SHAKE8};
	// PZ: Commented out the .DUP frame. I have no idea why that is there.
	enum {FR_SHAKE9 = 54, FR_SHAKE10, FR_SHAKE11, FR_SHAKE12, /*FR_SHAKE12.DUP,*/ FR_SHAKE13, FR_SHAKE14};
	enum {FR_SHAKE15 = 60, FR_SHAKE16, FR_SHAKE17, FR_SHAKE18, FR_SHAKE19, FR_SHAKE20};
}

//void() old_stand     =[      $old1,       old_stand    ] {};

void old_idle1() {FRAME_STATE(OldOneFrames::FR_OLD1, old_idle2);}
void old_idle2() {FRAME_STATE(OldOneFrames::FR_OLD2, old_idle3);}
void old_idle3() {FRAME_STATE(OldOneFrames::FR_OLD3, old_idle4);}
void old_idle4() {FRAME_STATE(OldOneFrames::FR_OLD4, old_idle5);}
void old_idle5() {FRAME_STATE(OldOneFrames::FR_OLD5, old_idle6);}
void old_idle6() {FRAME_STATE(OldOneFrames::FR_OLD6, old_idle7);}
void old_idle7() {FRAME_STATE(OldOneFrames::FR_OLD7, old_idle8);}
void old_idle8() {FRAME_STATE(OldOneFrames::FR_OLD8, old_idle9);}
void old_idle9() {FRAME_STATE(OldOneFrames::FR_OLD9, old_idle10);}
void old_idle10() {FRAME_STATE(OldOneFrames::FR_OLD10, old_idle11);}
void old_idle11() {FRAME_STATE(OldOneFrames::FR_OLD11, old_idle12);}
void old_idle12() {FRAME_STATE(OldOneFrames::FR_OLD12, old_idle13);}
void old_idle13() {FRAME_STATE(OldOneFrames::FR_OLD13, old_idle14);}
void old_idle14() {FRAME_STATE(OldOneFrames::FR_OLD14, old_idle15);}
void old_idle15() {FRAME_STATE(OldOneFrames::FR_OLD15, old_idle16);}
void old_idle16() {FRAME_STATE(OldOneFrames::FR_OLD16, old_idle17);}
void old_idle17() {FRAME_STATE(OldOneFrames::FR_OLD17, old_idle18);}
void old_idle18() {FRAME_STATE(OldOneFrames::FR_OLD18, old_idle19);}
void old_idle19() {FRAME_STATE(OldOneFrames::FR_OLD19, old_idle20);}
void old_idle20() {FRAME_STATE(OldOneFrames::FR_OLD20, old_idle21);}
void old_idle21() {FRAME_STATE(OldOneFrames::FR_OLD21, old_idle22);}
void old_idle22() {FRAME_STATE(OldOneFrames::FR_OLD22, old_idle23);}
void old_idle23() {FRAME_STATE(OldOneFrames::FR_OLD23, old_idle24);}
void old_idle24() {FRAME_STATE(OldOneFrames::FR_OLD24, old_idle25);}
void old_idle25() {FRAME_STATE(OldOneFrames::FR_OLD25, old_idle26);}
void old_idle26() {FRAME_STATE(OldOneFrames::FR_OLD26, old_idle27);}
void old_idle27() {FRAME_STATE(OldOneFrames::FR_OLD27, old_idle28);}
void old_idle28() {FRAME_STATE(OldOneFrames::FR_OLD28, old_idle29);}
void old_idle29() {FRAME_STATE(OldOneFrames::FR_OLD29, old_idle30);}
void old_idle30() {FRAME_STATE(OldOneFrames::FR_OLD30, old_idle31);}
void old_idle31() {FRAME_STATE(OldOneFrames::FR_OLD31, old_idle32);}
void old_idle32() {FRAME_STATE(OldOneFrames::FR_OLD32, old_idle33);}
void old_idle33() {FRAME_STATE(OldOneFrames::FR_OLD33, old_idle34);}
void old_idle34() {FRAME_STATE(OldOneFrames::FR_OLD34, old_idle35);}
void old_idle35() {FRAME_STATE(OldOneFrames::FR_OLD35, old_idle36);}
void old_idle36() {FRAME_STATE(OldOneFrames::FR_OLD36, old_idle37);}
void old_idle37() {FRAME_STATE(OldOneFrames::FR_OLD37, old_idle38);}
void old_idle38() {FRAME_STATE(OldOneFrames::FR_OLD38, old_idle39);}
void old_idle39() {FRAME_STATE(OldOneFrames::FR_OLD39, old_idle40);}
void old_idle40() {FRAME_STATE(OldOneFrames::FR_OLD40, old_idle41);}
void old_idle41() {FRAME_STATE(OldOneFrames::FR_OLD41, old_idle42);}
void old_idle42() {FRAME_STATE(OldOneFrames::FR_OLD42, old_idle43);}
void old_idle43() {FRAME_STATE(OldOneFrames::FR_OLD43, old_idle44);}
void old_idle44() {FRAME_STATE(OldOneFrames::FR_OLD44, old_idle45);}
void old_idle45() {FRAME_STATE(OldOneFrames::FR_OLD45, old_idle46);}
void old_idle46() {FRAME_STATE(OldOneFrames::FR_OLD46, old_idle1);}


void old_thrash1() {FRAME_STATE(OldOneFrames::FR_SHAKE1, old_thrash2); lightstyle(0, "m");}
void old_thrash2() {FRAME_STATE(OldOneFrames::FR_SHAKE2, old_thrash3); lightstyle(0, "k");}
void old_thrash3() {FRAME_STATE(OldOneFrames::FR_SHAKE3, old_thrash4); lightstyle(0, "k");}
void old_thrash4() {FRAME_STATE(OldOneFrames::FR_SHAKE4, old_thrash5); lightstyle(0, "i");}
void old_thrash5() {FRAME_STATE(OldOneFrames::FR_SHAKE5, old_thrash6); lightstyle(0, "g");}
void old_thrash6() {FRAME_STATE(OldOneFrames::FR_SHAKE6, old_thrash7); lightstyle(0, "e");}
void old_thrash7() {FRAME_STATE(OldOneFrames::FR_SHAKE7, old_thrash8); lightstyle(0, "c");}
void old_thrash8() {FRAME_STATE(OldOneFrames::FR_SHAKE8, old_thrash9); lightstyle(0, "a");}
void old_thrash9() {FRAME_STATE(OldOneFrames::FR_SHAKE9, old_thrash10); lightstyle(0, "c");}
void old_thrash10() {FRAME_STATE(OldOneFrames::FR_SHAKE10, old_thrash11); lightstyle(0, "e");}
void old_thrash11() {FRAME_STATE(OldOneFrames::FR_SHAKE11, old_thrash12); lightstyle(0, "g");}
void old_thrash12() {FRAME_STATE(OldOneFrames::FR_SHAKE12, old_thrash13); lightstyle(0, "i");}
void old_thrash13() {FRAME_STATE(OldOneFrames::FR_SHAKE13, old_thrash14); lightstyle(0, "k");}
void old_thrash14() {FRAME_STATE(OldOneFrames::FR_SHAKE14, old_thrash15); lightstyle(0, "m");}
void old_thrash15() {FRAME_STATE(OldOneFrames::FR_SHAKE15, old_thrash16); lightstyle(0, "m");
self->cnt = self->cnt + 1;
if (self->cnt != 3)
	self->think = old_thrash1;
}
void old_thrash16() {FRAME_STATE(OldOneFrames::FR_SHAKE16, old_thrash17); lightstyle(0, "g");}
void old_thrash17() {FRAME_STATE(OldOneFrames::FR_SHAKE17, old_thrash18); lightstyle(0, "c");}
void old_thrash18() {FRAME_STATE(OldOneFrames::FR_SHAKE18, old_thrash19); lightstyle(0, "b");}
void old_thrash19() {FRAME_STATE(OldOneFrames::FR_SHAKE19, old_thrash20); lightstyle(0, "a");}
void old_thrash20() {FRAME_STATE(OldOneFrames::FR_SHAKE20, old_thrash20); finale_4();}

//============================================================================

void finale_1()
{
	entity	pos, pl;
	entity	timer;

	intermission_exittime = time + 30;	// never allow exit
	intermission_running = 1;

	// find the intermission spot
	pos = find (world, "classname", "info_intermission");
	if (pos == world)
		error ("no info_intermission");

	pl = find (world, "classname", "misc_teleporttrain");
	if (pl != world)
		remove (pl);
/*
	// FIXME: misc_teleporttrain: why doesn't this work?
	if (!pl)
		error ("no teleporttrain");
	remove (pl);
*/

	WriteByte (PR_MSG_ALL, PR_SVC_FINALE);
	WriteString (PR_MSG_ALL, "");

	pl = find (world, "classname", "player");
	while (pl != world)
	{
		pl->view_ofs = V({0, 0, 0});
		pl->angles = other->v_angle = pos->mangle;
		pl->fixangle = PR_TRUE;		// turn this way immediately
		pl->map = self->map;
		pl->nextthink = time + 0.5;
		pl->takedamage = PR_DAMAGE_NO;
		pl->solid = PR_SOLID_NOT;
		pl->movetype = PR_MOVETYPE_NONE;
		pl->modelindex = 0;
		setorigin (pl, pos->origin);
		pl = find (pl, "classname", "player");
	}

	// make fake versions of all players as standins, and move the real
	// players to the intermission spot

	// wait for 1 second
	timer = spawn();
	timer->nextthink = time + 1;
	timer->think = finale_2;
}

void finale_2()
{
	vector	o;

	// start a teleport splash inside shub

	o = shub->origin - V({0, 100, 0});
	WriteByte (PR_MSG_MULTICAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_MULTICAST, PR_TE_TELEPORT);
	WriteCoord (PR_MSG_MULTICAST, o[X]);
	WriteCoord (PR_MSG_MULTICAST, o[Y]);
	WriteCoord (PR_MSG_MULTICAST, o[Z]);
	multicast (o, PR_MULTICAST_PHS);

	sound (shub, PR_CHAN_VOICE, "misc/r_tele1.wav", 1, PR_ATTN_NORM);

	self->nextthink = time + 2;
	self->think = finale_3;
}

void finale_3()
{
	// start shub thrashing wildly
	shub->think = old_thrash1;
	sound (shub, PR_CHAN_VOICE, "boss2/death.wav", 1, PR_ATTN_NORM);
	lightstyle(0, "abcdefghijklmlkjihgfedcb");
}

void finale_4()
{
	// throw tons of meat chunks
	vector	oldo;
	float	x, y, z;
	float	r;
	entity	n;

	sound (self, PR_CHAN_VOICE, "boss2/pop2.wav", 1, PR_ATTN_NORM);

	oldo = self->origin;

	z = 16;
	while (z <= 144)
	{
		x = -64;
		while (x <= 64)
		{
			y = -64;
			while (y <= 64)
			{
				self->origin[X] = oldo[X] + x;
				self->origin[Y] = oldo[Y] + y;
				self->origin[Z] = oldo[Z] + z;

				r = random();
				if (r < 0.3)
					ThrowGib("progs/gib1.mdl", -999, PR_TRUE, 0, PR_GIB1_KGS, PR_FALSE);
				else if (r < 0.6)
					ThrowGib("progs/gib2.mdl", -999, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
				else
					ThrowGib("progs/gib3.mdl", -999, PR_TRUE, 0, PR_GIB3_KGS, PR_FALSE);
				y = y + 32;
			}
			x = x + 32;
		}
		z = z + 96;
	}
	// start the end text
	WriteByte (PR_MSG_ALL, PR_SVC_FINALE);
	WriteString (PR_MSG_ALL, "Congratulations and well done! You have\nbeaten the hideous Shub-Niggurath, and\nher hundreds of ugly changelings and\nmonsters. You have proven that your\nskill and your cunning are greater than\nall the powers of Quake. You are the\nmaster now. Id Software salutes you.");

// put a player model down
	n = spawn();
	setmodel (n, "progs/player.mdl");
	oldo = oldo - V({32, 264, 0});
	setorigin (n, oldo);
	n->angles = V({0, 290, 0});
	n->frame = 1;

	remove (self);

// switch cd track
/*
	WriteByte (#MSG_ALL, #SVC_CDTRACK);
	WriteByte (#MSG_ALL, 3);
	WriteByte (#MSG_ALL, 3);
*/
	lightstyle(0, "m");
}

//============================================================================

void nopain(entity attacker, float damage)
{
	self->health = PR_IDMON_OLDONE_HEALTH;

	// if the telefrag didn't do enough damage, then force it to be telefragged
	if (damage_attacker->classname == "teledeath" || damage_attacker->classname == "teledeath2")
		finale_1 ();
}

//============================================================================


/*QUAKED monster_oldone (1 0 0) (-16 -16 -24) (16 16 32)
*/
void monster_oldone()
{
	if ( deathmatch ) {
		remove( self );
		return;
	}

	self->classname = "idmonster_oldone";

	precache_model2 ("progs/oldone.mdl");

	precache_sound2 ("boss2/death.wav");
	precache_sound2 ("boss2/idle.wav");
	precache_sound2 ("boss2/sight.wav");
	precache_sound2 ("boss2/pop2.wav");

	self->solid = PR_SOLID_SLIDEBOX;
	self->movetype = PR_MOVETYPE_STEP;

	setmodel (self, "progs/oldone.mdl");
	setsize (self, V({-160, -128, -24}), V({160, 128, 256}));

	self->max_health = self->health = PR_IDMON_OLDONE_HEALTH;		// kill by telefrag

	self->think = old_idle1;
	self->nextthink = time + 0.1;
	self->takedamage = PR_DAMAGE_YES;
	self->th_pain = nopain;
	self->th_die = finale_1;
	shub = self;


	self->netname = "oldone";

	self->monsterweight = PR_MWEIGHT_HUGE;
	self->monsterflags = PR_MFLAG_NOHEART;

	// monsters have a team of their own
	if ( !self->team_no )
		self->team_no = PR_TEAMNO_MONSTERS;
	self->PR_monsterflag = PR_STRFLAG_MONSTER;

	self->is_connected = PR_TRUE;

	total_monsters = total_monsters + 1;
}

} // END namespace Progs

#endif
