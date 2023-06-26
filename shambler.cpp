/*
==============================================================================

SHAMBLER

==============================================================================
*/

#include "progs.h"
#include "shambler.h"
#include "ofndefs.h"
#include "debug.h"
#include "combat.h"
#include "cpstuff.h"
#include "weapons.h"
#include "fight.h"
#include "ai.h"
#include "monsters.h"
#include "player.h"
#include "warlock.h"

namespace Progs {

//void(float side) ShamFireball;
//void() sham_fireballl1;
//void() sham_fireballr1;
//void() shambler_fire_touch;
float CanDamage(entity targ, entity inflictor);
#ifdef PR_XMAS_STUFF
void sham_xmas_removefx();
#endif

//$cd /raid/quake/id1/models/shams
//$scale 1.5
//$origin 0 0 24
//$base base
//$skin base

namespace CuTF_ShamblerFrames
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

void sham_stand1() {FRAME_STATE(CuTF_ShamblerFrames::FR_STAND1, sham_stand2); ai_stand();}
void sham_stand2() {FRAME_STATE(CuTF_ShamblerFrames::FR_STAND2, sham_stand3); ai_stand();}
void sham_stand3() {FRAME_STATE(CuTF_ShamblerFrames::FR_STAND3, sham_stand4); ai_stand();}
void sham_stand4() {FRAME_STATE(CuTF_ShamblerFrames::FR_STAND4, sham_stand5); ai_stand();}
void sham_stand5() {FRAME_STATE(CuTF_ShamblerFrames::FR_STAND5, sham_stand6); ai_stand();}
void sham_stand6() {FRAME_STATE(CuTF_ShamblerFrames::FR_STAND6, sham_stand7); ai_stand();}
void sham_stand7() {FRAME_STATE(CuTF_ShamblerFrames::FR_STAND7, sham_stand8); ai_stand();}
void sham_stand8() {FRAME_STATE(CuTF_ShamblerFrames::FR_STAND8, sham_stand9); ai_stand();}
void sham_stand9() {FRAME_STATE(CuTF_ShamblerFrames::FR_STAND9, sham_stand10); ai_stand();}
void sham_stand10() {FRAME_STATE(CuTF_ShamblerFrames::FR_STAND10, sham_stand11); ai_stand();}
void sham_stand11() {FRAME_STATE(CuTF_ShamblerFrames::FR_STAND11, sham_stand12); ai_stand();}
void sham_stand12() {FRAME_STATE(CuTF_ShamblerFrames::FR_STAND12, sham_stand13); ai_stand();}
void sham_stand13() {FRAME_STATE(CuTF_ShamblerFrames::FR_STAND13, sham_stand14); ai_stand();}
void sham_stand14() {FRAME_STATE(CuTF_ShamblerFrames::FR_STAND14, sham_stand15); ai_stand();}
void sham_stand15() {FRAME_STATE(CuTF_ShamblerFrames::FR_STAND15, sham_stand16); ai_stand();}
void sham_stand16() {FRAME_STATE(CuTF_ShamblerFrames::FR_STAND16, sham_stand17); ai_stand();}
void sham_stand17() {FRAME_STATE(CuTF_ShamblerFrames::FR_STAND17, sham_stand1); ai_stand();}

void sham_walk1() {FRAME_STATE(CuTF_ShamblerFrames::FR_WALK1, sham_walk2); ai_walk(10);}
void sham_walk2() {FRAME_STATE(CuTF_ShamblerFrames::FR_WALK2, sham_walk3); ai_walk(9);}
void sham_walk3() {FRAME_STATE(CuTF_ShamblerFrames::FR_WALK3, sham_walk4); ai_walk(9);}
void sham_walk4() {FRAME_STATE(CuTF_ShamblerFrames::FR_WALK4, sham_walk5); ai_walk(5);}
void sham_walk5() {FRAME_STATE(CuTF_ShamblerFrames::FR_WALK5, sham_walk6); ai_walk(6);}
void sham_walk6() {FRAME_STATE(CuTF_ShamblerFrames::FR_WALK6, sham_walk7); ai_walk(12);}
void sham_walk7() {FRAME_STATE(CuTF_ShamblerFrames::FR_WALK7, sham_walk8); ai_walk(8);}
void sham_walk8() {FRAME_STATE(CuTF_ShamblerFrames::FR_WALK8, sham_walk9); ai_walk(3);}
void sham_walk9() {FRAME_STATE(CuTF_ShamblerFrames::FR_WALK9, sham_walk10); ai_walk(13);}
void sham_walk10() {FRAME_STATE(CuTF_ShamblerFrames::FR_WALK10, sham_walk11); ai_walk(9);}
void sham_walk11() {FRAME_STATE(CuTF_ShamblerFrames::FR_WALK11, sham_walk12); ai_walk(7);}
void sham_walk12() {FRAME_STATE(CuTF_ShamblerFrames::FR_WALK12, sham_walk1); ai_walk(7);
if (random() > 0.8)
	sound (self, PR_CHAN_VOICE, "shambler/sidle.wav", 1, PR_ATTN_IDLE);}

void sham_run1() {FRAME_STATE(CuTF_ShamblerFrames::FR_RUN1, sham_run2); ai_run(20);}
void sham_run2() {FRAME_STATE(CuTF_ShamblerFrames::FR_RUN2, sham_run3); ai_run(24);}
void sham_run3() {FRAME_STATE(CuTF_ShamblerFrames::FR_RUN3, sham_run4); ai_run(20);}
void sham_run4() {FRAME_STATE(CuTF_ShamblerFrames::FR_RUN4, sham_run5); ai_run(20);}
void sham_run5() {FRAME_STATE(CuTF_ShamblerFrames::FR_RUN5, sham_run6); ai_run(24);}
void sham_run6() {FRAME_STATE(CuTF_ShamblerFrames::FR_RUN6, sham_run1); ai_run(20);
if (random() > 0.8)
	sound (self, PR_CHAN_VOICE, "shambler/sidle.wav", 1, PR_ATTN_IDLE);

//if (self.enemy
}

void sham_smash1() {
FRAME_STATE(CuTF_ShamblerFrames::FR_SMASH1, sham_smash2);
sound (self, PR_CHAN_VOICE, "shambler/melee1.wav", 1, PR_ATTN_NORM);
ai_charge(12);}
void sham_smash2() {FRAME_STATE(CuTF_ShamblerFrames::FR_SMASH2, sham_smash3); ai_charge(12);} // spd trple
void sham_smash3() {FRAME_STATE(CuTF_ShamblerFrames::FR_SMASH3, sham_smash4); ai_charge(12);}
void sham_smash4() {FRAME_STATE(CuTF_ShamblerFrames::FR_SMASH4, sham_smash5); ai_charge(10);}
void sham_smash5() {FRAME_STATE(CuTF_ShamblerFrames::FR_SMASH5, sham_smash6); ai_charge(8);}
void sham_smash6() {FRAME_STATE(CuTF_ShamblerFrames::FR_SMASH6, sham_smash7); ai_charge(2);}
void sham_smash7() {FRAME_STATE(CuTF_ShamblerFrames::FR_SMASH7, sham_smash8); ai_charge(0);
    MonsterAuraPower();
}
void sham_smash8() {FRAME_STATE(CuTF_ShamblerFrames::FR_SMASH8, sham_smash9); ai_charge(0);}
void sham_smash9() {FRAME_STATE(CuTF_ShamblerFrames::FR_SMASH9, sham_smash10); ai_charge(0);}
void sham_smash10() {
FRAME_STATE(CuTF_ShamblerFrames::FR_SMASH10, sham_smash11);
vector	delta;//, smack_origin, source;
float 	ldmg;
//local entity	head;

	if (self->enemy == world)
		return;
	ai_charge(0);

	delta = self->enemy->origin - self->origin;

	if (vlen(delta) > 200)
		return;
	if (!CanDamage (self->enemy, self))
		return;

	ldmg = (random() + random() + random()) * 200 + 30*self->has_tesla;
	deathmsg = 0;
	if (self->enemy->tf_items & PR_NIT_GEL) ldmg = ldmg / 2;
	if (self->enemy->cutf_items & PR_CUTF_DEMONLORE) ldmg = ldmg / 2;
	//T_Damage (self.enemy, self, self, ldmg);
    TF_T_Damage(self->enemy, self, self, ldmg,0,PR_TF_TD_OTHER);
	sound (self, PR_CHAN_VOICE, "shambler/smack.wav", 1, PR_ATTN_NORM);

	SpawnMeatSpray (self->origin + v_forward*16, crandom() * 100 * v_right);
	SpawnMeatSpray (self->origin + v_forward*16, crandom() * 100 * v_right);

}
void sham_smash11() {FRAME_STATE(CuTF_ShamblerFrames::FR_SMASH11, sham_smash12); ai_charge(10 * (0.7 + self->has_tesla * 0.2));} // spd dbl
void sham_smash12() {FRAME_STATE(CuTF_ShamblerFrames::FR_SMASH12, sham_run1); ai_charge(8 * (0.7 + self->has_tesla * 0.2));}

void sham_swingr1();

void ShamClaw(float side)
{
vector	delta;
//local vector	smack_origin, source;
float 	ldmg;
//local entity	head;


	if (self->enemy == world)
		return;
	ai_charge(20 * (0.7 + self->has_tesla * 0.2)); //spd dbl

	delta = self->enemy->origin - self->origin;

	if (vlen(delta) > 200)
		return;

	deathmsg = 0;

	ldmg = (random() + random() + random()) * 75 + 15*self->has_tesla;
	if (self->enemy->tf_items & PR_NIT_GEL) ldmg = ldmg / 2;
	if (self->enemy->cutf_items & PR_CUTF_DEMONLORE) ldmg = ldmg * 0.8;
	if (self->has_tesla == 5)
	{
		if (ldmg < self->enemy->health && self->enemy->health > 0)
			self->health = self->health + (ldmg / 20);
		else if (self->enemy->health > 0)
			self->health = self->health + (self->enemy->health / 20);
		if (self->health > self->max_health)
			self->health = self->max_health;
	}
	//T_Damage (self.enemy, self, self, ldmg);
    TF_T_Damage(self->enemy, self, self, ldmg,0,PR_TF_TD_OTHER);
	sound (self, PR_CHAN_VOICE, "shambler/smack.wav", 1, PR_ATTN_NORM);

	if (side)
	{
		makevectors (self->angles);
		SpawnMeatSpray (self->origin + v_forward*16, side * v_right);
	}

}

void sham_swingl1() {
FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGL1, sham_swingl2);
sound (self, PR_CHAN_VOICE, "shambler/melee2.wav", 1, PR_ATTN_NORM);
    MonsterAuraPower();
ai_charge(10);}
void sham_swingl2() {FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGL2, sham_swingl3); ai_charge(6);} //speed doubled
void sham_swingl3() {FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGL3, sham_swingl4); ai_charge(14);}
void sham_swingl4() {FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGL4, sham_swingl5); ai_charge(6);}
void sham_swingl5() {FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGL5, sham_swingl6); ai_charge(14);}
void sham_swingl6() {FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGL6, sham_swingl7); ai_charge(18);}
void sham_swingl7() {FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGL7, sham_swingl8); ai_charge(10); ShamClaw(250);
    //if (self.enemy.health <= 0 || self.enemy.has_disconnected)
    if (!StillValidEnemy(self->enemy))
    {
    self->enemy = world;
    // FIXME: look all around for other targets
        //if (self.oldenemy.health > 0 && !self.oldenemy.has_disconnected)
        if (StillValidEnemy(self->oldenemy))
        {
            self->enemy = self->oldenemy;
            HuntTarget ();
        }
        else
        { //FIXES MAD SHAMBLERs?
            MonsterIdle(self);

            /*if (self.movetarget)
                self.th_walk ();
            else
                self.th_stand ();*/

            return;
        }
    }
}
void sham_swingl8() {FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGL8, sham_swingl9); ai_charge(8);}
void sham_swingl9() {
FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGL9, sham_run1);
ai_charge(16);
if (random()<0.5)
	self->think = sham_swingr1;
}

void sham_swingr1() {
    FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGR1, sham_swingr2);
    sound (self, PR_CHAN_VOICE, "shambler/melee1.wav", 1, PR_ATTN_NORM);
    MonsterAuraPower();

    ai_charge(2);}
void sham_swingr2() {FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGR2, sham_swingr3); ai_charge(16);}
void sham_swingr3() {FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGR3, sham_swingr4); ai_charge(28);}
void sham_swingr4() {FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGR4, sham_swingr5); ai_charge(14);} // speed dbl
void sham_swingr5() {FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGR5, sham_swingr6); ai_charge(6);}
void sham_swingr6() {FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGR6, sham_swingr7); ai_charge(12);}
void sham_swingr7() {FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGR7, sham_swingr8); ai_charge(12); ShamClaw(-250);
    //if (self.enemy.health <= 0 || self.enemy.has_disconnected)
    if (!StillValidEnemy(self->enemy))
    {
        self->enemy = world;
    // FIXME: look all around for other targets
        //if (self.oldenemy.health > 0 && !self.oldenemy.has_disconnected)
        if (StillValidEnemy(self->oldenemy))
        {
            self->enemy = self->oldenemy;
            HuntTarget ();
        }
        else
        { //FIXES MAD SHAMBLERs? - no
            MonsterIdle(self);

            /*if (self.movetarget)
                self.th_walk ();
            else
                self.th_stand ();*/

            return;
        }
    }
}
void sham_swingr8() {FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGR8, sham_swingr9); ai_charge(6);}
void sham_swingr9() {FRAME_STATE(CuTF_ShamblerFrames::FR_SWINGR9, sham_run1); ai_charge(2);
//if ((self.has_sensor && self.enemy.health > 0 && random()<0.5) || (!self.has_sensor && self.enemy.health <= 0 && random()<0.5))
// TOCHECK: WTF is this for??
if ((self->has_sensor && StillValidEnemy(self->enemy) && random()<0.5) || (!self->has_sensor && !StillValidEnemy(self->enemy) && random()<0.5))
	self->think = sham_swingl1;
}

void sham_melee()
{
	float chance;

	chance = random();
	self->has_sensor = 1;
	if (chance > 0.7)
		sham_smash1 ();
	else if (chance > 0.35)
		sham_swingr1 ();
	else
		sham_swingl1 ();
}


//============================================================================

void CastLightning()
{
	vector	org, dir;
	float	ldmg;

	self->effects = self->effects | PR_EF_DIMLIGHT;

	ai_face ();

	org = self->origin + V({0, 0, 40});

	dir = self->enemy->origin + V({0, 0, 16}) - org;
	dir = normalize (dir);

    // OfN - Check for force field
    traceline (org, self->origin + dir*3000, PR_TL_ANY_SOLID, self);

    if (trace_ent->classname == "force_field")
    {
        FieldEvent(trace_ent,trace_endpos,trace_ent);

        /*FieldExplosion(trace_ent,trace_endpos,trace_ent);
        PutFieldWork(trace_ent);*/

        WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
        WriteByte (PR_MSG_BROADCAST, PR_TE_LIGHTNING1);
        WriteEntity (PR_MSG_BROADCAST, self);
        WriteCoord (PR_MSG_BROADCAST, org[X]);
        WriteCoord (PR_MSG_BROADCAST, org[Y]);
        WriteCoord (PR_MSG_BROADCAST, org[Z]);
        WriteCoord (PR_MSG_BROADCAST, trace_endpos[X]);
        WriteCoord (PR_MSG_BROADCAST, trace_endpos[Y]);
        WriteCoord (PR_MSG_BROADCAST, trace_endpos[Z]);
    #ifdef PR_QUAKE_WORLD
        multicast (org, PR_MULTICAST_PHS);
    #endif

        return;
    }
    //_------------------------------------_//

	traceline (org, self->origin + dir*3000, PR_TL_BSP_ONLY, self);

	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_LIGHTNING1);
	WriteEntity (PR_MSG_BROADCAST, self);
	WriteCoord (PR_MSG_BROADCAST, org[X]);
	WriteCoord (PR_MSG_BROADCAST, org[Y]);
	WriteCoord (PR_MSG_BROADCAST, org[Z]);
	WriteCoord (PR_MSG_BROADCAST, trace_endpos[X]);
	WriteCoord (PR_MSG_BROADCAST, trace_endpos[Y]);
	WriteCoord (PR_MSG_BROADCAST, trace_endpos[Z]);

#ifdef PR_QUAKE_WORLD
    multicast (org, PR_MULTICAST_PHS);
#endif

    ldmg = 5 + self->has_tesla * 5;
	if (self->has_tesla == 5)
	{
		if (ldmg < self->enemy->health)
			self->health = self->health + (ldmg / 10);
		else
			self->health = floor(self->health + (self->enemy->health / 10));
		if (self->health > self->max_health)
			self->health = self->max_health;
	}
    #ifdef PR_SHAMBLER_SHOCK_SOUND

    // OfN - hackish, sry, but better than modifying the ancient LightningDamage :)
	float oldhp;
    oldhp = self->enemy->health;

    LightningDamage (org, trace_endpos, self, 5 + self->has_tesla * 5);

    // They got electrified, dont they? so..
    if (self->enemy->health != oldhp)
        sound (self->enemy, PR_CHAN_WEAPON, "weapons/lhit.wav", 0.75, PR_ATTN_NORM);

    #else

    LightningDamage (org, trace_endpos, self, 5 + self->has_tesla * 5);

    #endif

}

void sham_magic1() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC1, sham_magic2); ai_face();
	sound (self, PR_CHAN_WEAPON, "shambler/sattck1.wav", 1, PR_ATTN_NORM);
}
void sham_magic2() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC2, sham_magic3); ai_face();}
void sham_magic3() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC3, sham_magic4); ai_face();self->nextthink = self->nextthink + 0.2;
entity o;

self->effects = self->effects | PR_EF_DIMLIGHT;
ai_face();
self->owner = spawn();
o = self->owner;
setmodel (o, "progs/s_light.mdl");
setorigin (o, self->origin);
o->angles = self->angles;
o->nextthink = time + 0.7;
o->think = SUB_Remove;
}
void sham_magic4()
{
    FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC4, sham_magic5);
    MonsterAuraPower();
self->effects = self->effects | PR_EF_DIMLIGHT;
self->owner->frame = 1;
}
void sham_magic5()
{
FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC5, sham_magic6);
self->effects = self->effects | PR_EF_DIMLIGHT;
self->owner->frame = 2;
}
void sham_magic6()
{
FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC6, sham_magic9);
remove (self->owner);
CastLightning();
sound (self, PR_CHAN_WEAPON, "shambler/sboom.wav", 1, PR_ATTN_NORM);
}
void sham_magic9()
{FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC9, sham_magic10); CastLightning();}
void sham_magic10()
{FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_magic11); CastLightning();}
void sham_magic11()
{
FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC11, sham_magic12);
if (skill == 3)
	CastLightning();
}
void sham_magic12() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC12, sham_run1); self->effects = 0;}



void sham_pain1() {FRAME_STATE(CuTF_ShamblerFrames::FR_PAIN1, sham_pain2);}
void sham_pain2() {FRAME_STATE(CuTF_ShamblerFrames::FR_PAIN2, sham_pain3);}
void sham_pain3() {FRAME_STATE(CuTF_ShamblerFrames::FR_PAIN3, sham_pain4);}
void sham_pain4() {FRAME_STATE(CuTF_ShamblerFrames::FR_PAIN4, sham_pain5);}
void sham_pain5() {FRAME_STATE(CuTF_ShamblerFrames::FR_PAIN5, sham_pain6);}
void sham_pain6() {FRAME_STATE(CuTF_ShamblerFrames::FR_PAIN6, sham_run1);}

void sham_pain(entity attacker, float damage)
{
    //self.real_owner.StatusRefreshTime = time + 0.2;
	//self.real_owner.StatusBarScreen = 3;

    if (self->has_holo) return; // there is no pain during xmas! :)

    if (self->pain_finished > time)
		return;

    if (self->health <= 0)
		return;		// allready dying, don't go into pain frame

    if (random()*100 > damage) // was 70
		return;		// didn't flinch

    AI_CheckAttacker(attacker);

    sound (self, PR_CHAN_VOICE, "shambler/shurt2.wav", 1, PR_ATTN_NORM);

	//self.pain_finished = time + 2; wasnt comented
    self->pain_finished = time + 1.5 + random() * 1.2; //-OfN first was 1, then 1.5
	self->effects=0;

	sham_pain1 ();
}

void sham_death1() {FRAME_STATE(CuTF_ShamblerFrames::FR_DEATH1, sham_death2);}
void sham_death2() {FRAME_STATE(CuTF_ShamblerFrames::FR_DEATH2, sham_death3);}
void sham_death3() {FRAME_STATE(CuTF_ShamblerFrames::FR_DEATH3, sham_death4); self->solid = PR_SOLID_NOT;}
void sham_death4() {FRAME_STATE(CuTF_ShamblerFrames::FR_DEATH4, sham_death5);}
void sham_death5() {FRAME_STATE(CuTF_ShamblerFrames::FR_DEATH5, sham_death6);}
void sham_death6() {FRAME_STATE(CuTF_ShamblerFrames::FR_DEATH6, sham_death7);}
void sham_death7() {FRAME_STATE(CuTF_ShamblerFrames::FR_DEATH7, sham_death8);}
void sham_death8() {FRAME_STATE(CuTF_ShamblerFrames::FR_DEATH8, sham_death9);}
void sham_death9() {FRAME_STATE(CuTF_ShamblerFrames::FR_DEATH9, sham_death10);}
void sham_death10() {FRAME_STATE(CuTF_ShamblerFrames::FR_DEATH10, sham_death11);}
void sham_death11()
{
    //self.nextthink = time + 40 + 40*random();
	//self.think = SUB_Remove;
    FRAME_STATE(CuTF_ShamblerFrames::FR_DEATH11, sham_death11);
    MonsterCorpse();
}

void custom_shambler_die()
{
	self->effects=0;

    if (self->demon_three != world)
        dremove(self->demon_three);

    self->demon_three = world;

    if (self->real_owner->classname == "player")
	{
		sprint(self->real_owner,PR_PRINT_HIGH,"Your shambler ");
		sprint(self->real_owner,PR_PRINT_HIGH,self->netname);
        sprint(self->real_owner,PR_PRINT_HIGH," is dead.\n");
		self->real_owner->job_finished = time + 5; //Can't summon streams of demons SB can so
        ResetSlotPointer(self->real_owner, self->increase_team1);
	}

    // check for gib
	if (self->health < -50)
	{
        sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_MONSTERDIE);
	    ThrowMonsterHead ("progs/h_shams.mdl", self->health);
	    ThrowGib ("progs/gib1.mdl", self->health, PR_TRUE, 0,PR_GIB1_KGS, PR_FALSE);
	    ThrowGib ("progs/gib2.mdl", self->health, PR_TRUE, 0,PR_GIB2_KGS, PR_FALSE);
	    ThrowGib ("progs/gib3.mdl", self->health, PR_TRUE, 0,PR_GIB3_KGS, 0.33);
        ThrowGib ("progs/gib3.mdl", self->health, PR_TRUE, 0,PR_GIB3_KGS, 0.66);
        SpawnBloodEx(self->origin,0.66,9);

        dremove(self);
        return;
    }

    sound (self, PR_CHAN_VOICE, "shambler/sdeath.wav", 1, PR_ATTN_MONSTERDIE);

    // Spawn bubbles if under water/slime
    float contres;
    contres = pointcontents(self->origin);
    if (contres == PR_CONTENT_WATER || contres == PR_CONTENT_SLIME)
        DeathBubbles(5);

    //self.classname = "monster_corpse";
    MonsterDead();
    self->think=SUB_Null;
    sham_death1();
}

float CheckShamMelee()
{
	vector dist;
	float d;

	dist = self->enemy->origin - self->origin;
	d = vlen(dist);
	if (d < 150) //CH Check reach
	{
		self->attack_state = PR_AS_MELEE;
		return PR_TRUE;
	}
	return PR_FALSE;
}

float CheckShamLightning()
{
	vector	dist;
	float	d;
	float	d2;

	if (!visible2(self->enemy, self)) //If can see
		return PR_FALSE;
	dist = self->enemy->origin - self->origin;
	dist[Z] = 0; //CH only need x,y
	d2 = vlen(dist);
	dist = self->enemy->origin - self->origin;
	dist[X] = dist[Y] = 0; //CH only need z
	d = vlen(dist);
	if (self->has_tesla == 1)
		return PR_FALSE;
	if (random() > self->has_tesla * 0.07) //CH as not to fire all the time
		return PR_FALSE;
	if (d2 < 1500 - self->has_tesla * 300 && d < 100)
		return PR_FALSE;
	if (d2 > 2000 + self->has_tesla * 200) //Min X,Y distance away before zap
		return PR_FALSE;
    /*if (self.has_tesla > 3 && random() > 0.5)
		self.attack_state = #AS_FIREBALL;
	//else /// OFTEN*/
		self->attack_state = PR_AS_MISSILE;
	return PR_TRUE;
}

float ShamCheckAttack()
{
	//local	vector	vec;

	// if close enough for slashing, go for it
	if (CheckShamMelee ())
	{
		self->attack_state = PR_AS_MELEE;
		return PR_TRUE;
	}
	if (CheckShamLightning ())
		return PR_TRUE;

	return PR_FALSE;
}

// Shambler shoots fireballs!

/*
void() sham_fireball =
{
	local float		r;

	r = random();

	if (r > 0.5)
		sham_fireballl1();
	else
		sham_fireballr1();

};

void() sham_fireballl1	=[      $swingl1,      sham_fireballl2   ] {
sound (self, #CHAN_VOICE, "shambler/melee2.wav", 1, #ATTN_NORM);
ai_charge(15 * (0.7 + self.has_tesla * 0.2));};
void() sham_fireballl2 =[      $swingl2,      sham_fireballl3   ] {ai_charge(9 * (0.7 + self.has_tesla * 0.2));}; //speed doubled
void() sham_fireballl3 =[      $swingl3,      sham_fireballl4   ] {ai_charge(21 * (0.7 + self.has_tesla * 0.2));};
void() sham_fireballl4 =[      $swingl4,      sham_fireballl5   ] {ai_charge(9 * (0.7 + self.has_tesla * 0.2));};
void() sham_fireballl5 =[      $swingl5,      sham_fireballl6   ] {ai_charge(21 * (0.7 + self.has_tesla * 0.2));};
void() sham_fireballl6 =[      $swingl6,      sham_fireballl7   ] {ai_charge(27 * (0.7 + self.has_tesla * 0.2));};
void() sham_fireballl7 =[      $swingl7,      sham_fireballl8   ] {ai_charge(15 * (0.7 + self.has_tesla * 0.2)); ShamFireball(250);};
void() sham_fireballl8 =[      $swingl8,      sham_fireballl9   ] {ai_charge(12 * (0.7 + self.has_tesla * 0.2));};
void() sham_fireballl9 =[      $swingl9,      sham_run1  ] {
ai_charge(24 * (0.7 + self.has_tesla * 0.2));
if (random() < 0.2)
	self.think = sham_fireballr1;
};

void() sham_fireballr1	=[      $swingr1,      sham_fireballr2   ] {
sound (self, #CHAN_VOICE, "shambler/melee1.wav", 1, #ATTN_NORM);
ai_charge(3 * (0.7 + self.has_tesla * 0.2));};
void() sham_fireballr2	=[      $swingr2,      sham_fireballr3   ] {ai_charge(24 * (0.7 + self.has_tesla * 0.2));};
void() sham_fireballr3 =[      $swingr3,      sham_fireballr4   ] {ai_charge(42 * (0.7 + self.has_tesla * 0.2));};
void() sham_fireballr4 =[      $swingr4,      sham_fireballr5   ] {ai_charge(21 * (0.7 + self.has_tesla * 0.2));}; // speed dbl
void() sham_fireballr5 =[      $swingr5,      sham_fireballr6   ] {ai_charge(9 * (0.7 + self.has_tesla * 0.2));};
void() sham_fireballr6 =[      $swingr6,      sham_fireballr7   ] {ai_charge(18 * (0.7 + self.has_tesla * 0.2));};
void() sham_fireballr7 =[      $swingr7,      sham_fireballr8   ] {ai_charge(18 * (0.7 + self.has_tesla * 0.2)); ShamFireball(-250);};
void() sham_fireballr8 =[      $swingr8,      sham_fireballr9   ] {ai_charge(9 * (0.7 + self.has_tesla * 0.2));};
void() sham_fireballr9 =[      $swingr9,      sham_run1  ] {ai_charge(30 * (0.7 + self.has_tesla * 0.2));
if (random() < 0.2)
	self.think = sham_fireballl1;
};

void(float side) ShamFireball =
{
	local	float	ldmg;
	local	vector	delta;
	local	vector	offang;
	local	vector	org, dir;

	ai_face ();

	delta = self.enemy.origin - self.origin;
	offang = vectoangles (delta);
	makevectors (offang);

	sound (self, #CHAN_WEAPON, "hknight/attack1.wav", 1, #ATTN_NORM); //Odd, it was already precached

	if (side > 0) //CH cause to spawn on side of demon.
		org = self.origin + (v_forward * 10) + (v_right * 20);
	else
		org = self.origin + (v_forward * 10) - (v_right * 20);

// set missile speed
	dir = normalize (v_forward);

//CH demons are not good at throwing
	dir_z = 0 - dir_z; //Random Z addage
	//dir_x = dir_x + (random() - 0.5)*0.05; //Random X addage
	//dir_y = dir_y + (random() - 0.5)*0.05; //Random Y addage

	newmis = spawn ();
	newmis.owner = self;
	newmis.movetype = #MOVETYPE_NOCLIP;
	newmis.solid = #SOLID_BBOX;

	newmis.angles = vectoangles(dir);

	newmis.touch = shambler_fire_touch;
	newmis.weapon = #DMSG_DEMON_FIRE;
	newmis.classname = "demon_fire";
	newmis.think = SUB_Remove;
	newmis.nextthink = time + 10;
	setmodel (newmis, "progs/lavaball.mdl");
	setsize (newmis, '-8 -16 -8', '20 16 28'); //CH actual mdl bounds
//	setsize (newmis, '0 0 0', '0 0 0');
	setorigin (newmis, org);
	newmis.velocity = dir * 1000 * (self.has_tesla - 3);
};
*/

/*void() shambler_fire_touch = -OFN fireballs out!
{
	local float fire_dmg;
	fire_dmg = 10 + random() * 25;
	fire_dmg = fire_dmg * self.owner.has_tesla;
	if (pointcontents(self.origin) == #CONTENT_SKY)
	{
		dremove(self);
		return;
	}

	deathmsg = self.weapon;
	T_RadiusDamage (self, self.owner, fire_dmg , self.owner);

	self.origin = self.origin - 8*normalize(self.velocity); //???

#ifdef DEMO_STUFF
	// Remove any camera's locks on this missile
	if (self.enemy)
		CamProjectileLockOff();
#endif

	WriteByte (#MSG_BROADCAST, #SVC_TEMPENTITY);
	WriteByte (#MSG_BROADCAST, #TE_EXPLOSION);
	WriteCoord (#MSG_BROADCAST, self.origin_x);
	WriteCoord (#MSG_BROADCAST, self.origin_y);
	WriteCoord (#MSG_BROADCAST, self.origin_z);
#ifdef QUAKE_WORLD
	multicast (self.origin, #MULTICAST_PHS);
	dremove(self);
#else
	BecomeExplosion ();
#endif
};
*/

//================================================
// OfN - Xmas stuff
#ifdef PR_XMAS_STUFF

void sham_xmas1() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC12, sham_xmas2); self->has_holo = 1;sound(self,PR_CHAN_VOICE,"misc/shamxmas.wav",1,PR_ATTN_NONE);}
void sham_xmas2() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC11, sham_xmas3); MonsterAuraPower();}
void sham_xmas3() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas4); self->effects = PR_EF_DIMLIGHT;self->nextthink = time + 0.2;

if (self->demon_three != world) return;

entity lightfx;

lightfx = spawn();
lightfx->owner = self;
lightfx->movetype = PR_MOVETYPE_FLY;
lightfx->solid = PR_SOLID_NOT;
lightfx->effects = PR_EF_DIMLIGHT;
setmodel (lightfx, "progs/s_light.mdl");
setorigin (lightfx, self->origin);
lightfx->angles = self->angles;
lightfx->nextthink = time + 3;
lightfx->think = sham_xmas_removefx;
lightfx->avelocity = V({400, 400, 400});
lightfx->velocity = V({0, 0, 5});

self->demon_three = lightfx;
}

void sham_xmas4() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas5); self->effects = PR_EF_BRIGHTLIGHT;self->nextthink = time + 0.2;}
void sham_xmas5() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas6); self->effects = PR_EF_DIMLIGHT;self->nextthink = time + 0.2;}
void sham_xmas6() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas7); self->effects = PR_EF_BRIGHTLIGHT;self->nextthink = time + 0.2;}
void sham_xmas7() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas8); self->effects = PR_EF_DIMLIGHT;self->nextthink = time + 0.2;}
void sham_xmas8() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas9); self->effects = PR_EF_BRIGHTLIGHT;self->nextthink = time + 0.2;}
void sham_xmas9() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas10); self->effects = PR_EF_DIMLIGHT;self->nextthink = time + 0.2;}

void sham_xmas10() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas11); self->effects = PR_EF_BRIGHTLIGHT;self->nextthink = time + 0.2;}
void sham_xmas11() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas12); self->effects = PR_EF_DIMLIGHT;self->nextthink = time + 0.2;}
void sham_xmas12() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas13); self->effects = PR_EF_BRIGHTLIGHT;self->nextthink = time + 0.2;}

void sham_xmas13() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas14); self->effects = PR_EF_DIMLIGHT;self->nextthink = time + 0.1;}
void sham_xmas14() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas15); self->effects = PR_EF_BRIGHTLIGHT;self->nextthink = time + 0.1;}
void sham_xmas15() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas16); self->effects = PR_EF_DIMLIGHT;self->nextthink = time + 0.1;}

void sham_xmas16() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas17); self->effects = PR_EF_BRIGHTLIGHT;self->nextthink = time + 0.1;}
void sham_xmas17() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas18); self->effects = PR_EF_DIMLIGHT;self->nextthink = time + 0.1;}
void sham_xmas18() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas19); self->effects = PR_EF_BRIGHTLIGHT;self->nextthink = time + 0.1;}
void sham_xmas19() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas20); self->effects = PR_EF_DIMLIGHT;self->nextthink = time + 0.1;}
void sham_xmas20() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas21); self->effects = PR_EF_BRIGHTLIGHT;self->nextthink = time + 0.1;}
void sham_xmas21() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC10, sham_xmas22); self->effects = PR_EF_DIMLIGHT;self->nextthink = time + 0.1;}

void sham_xmas22() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC11, sham_xmas23); self->effects = 0;}
void sham_xmas23() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC9, sham_xmas24); self->effects = PR_EF_BRIGHTLIGHT;}
void sham_xmas24() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC8, sham_xmas25);}
void sham_xmas25() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC7, sham_xmas26);}
void sham_xmas26() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC6, sham_xmas27); MonsterAuraPower();}
void sham_xmas27() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC7, sham_xmas28);}
void sham_xmas28() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC8, sham_xmas29);}
void sham_xmas29() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC7, sham_xmas30);}
void sham_xmas30() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC6, sham_xmas31);}
void sham_xmas31() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC7, sham_xmas32);}
void sham_xmas32() {

    // xmas dance! // TODO: make it a function and spread hits among above frames
    FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC8, sham_xmas33);
    entity te;
    float xmashits;

    xmashits = 0;

    te = findradius(self->origin,PR_SHAMBLER_XMAS_RANGE);

    while (te!=world && xmashits <= PR_SHAMBLER_XMAS_MAXHITS)
    {
        if (te->classname == "player")
        if (te->is_connected)
        if (te->health > 0)
        #ifndef PR_MAD_MONSTERS
        if (!Teammate(self, te))
        #endif
        if (!(te->job & PR_JOB_THIEF && (te->job & PR_JOB_ACTIVE || te->job & PR_JOB_FULL_HIDE)))
        if (!(te->items & PR_IT_INVISIBILITY))
        {
            #ifndef PR_XMAS_TEST
            deathmsg = PR_DMSG_XMAS;
            TF_T_Damage(te,self,self,PR_SHAMBLER_XMAS_DMG,0,PR_TF_TD_ELECTRICITY);
            #endif

            // lightning
            WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	        WriteByte (PR_MSG_BROADCAST, PR_TE_LIGHTNING1);
	        WriteEntity (PR_MSG_BROADCAST, self);
	        WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
        	WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
	        WriteCoord (PR_MSG_BROADCAST, self->origin[Z] + 67);
	        WriteCoord (PR_MSG_BROADCAST, te->origin[X]);
	        WriteCoord (PR_MSG_BROADCAST, te->origin[Y]);
	        WriteCoord (PR_MSG_BROADCAST, te->origin[Z]);
            #ifdef PR_QUAKE_WORLD
            multicast (self->origin, PR_MULTICAST_PHS);
            #endif

            SpawnSprite(1,PR_SPRITE_AIRBURST,self->origin + V({0, 0, 65}),V({0, 0, 0}),PR_SPRITEMOVE_UPSLOW,0.15);

            SpawnSprite(1,PR_SPRITE_AIRBURST,te->origin + V({0, 0, 40}),V({0, 0, 0}),PR_SPRITEMOVE_UPSLOW,0.15);

            sound (te, PR_CHAN_WEAPON, "weapons/lhit.wav", 1, PR_ATTN_NONE);

            WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
            WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
            WriteCoord (PR_MSG_BROADCAST, te->origin[X]);
            WriteCoord (PR_MSG_BROADCAST, te->origin[Y]);
            WriteCoord (PR_MSG_BROADCAST, te->origin[Z]);
            #ifdef PR_QUAKE_WORLD
            multicast (te->origin, PR_MULTICAST_PHS);
            #endif

            xmashits = xmashits + 1;
        }

        te = te->chain;
    }

    // Special message if maximum hits is achieved..
    if (xmashits >= PR_SHAMBLER_XMAS_MAXHITS)
        bprint(PR_PRINT_MEDIUM,self->netname," wishes you " TO_STR(PR_SHAMBLER_XMAS_MAXHITS) " a merry xmas!\n");

}
void sham_xmas33() {FRAME_STATE(CuTF_ShamblerFrames::FR_MAGIC9, sham_run1); self->effects = 0;self->has_holo = 0;}

void sham_xmas_removefx()
{
    self->owner->demon_three = world;
    dremove(self);
}

#endif

} // END namespace Progs
