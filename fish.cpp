#include "progs.h"
#include "ofndefs.h"
#include "fish.h"
#include "ai.h"
#include "monsters.h"
#include "combat.h"
#include "fight.h"
#include "warlock.h"
#include "player.h"
#include "debug.h"
#include "cpstuff.h"

namespace Progs {

//$cd id1/models/fish
//$origin 0 0 24
//$base base
//$skin skin

namespace CuTF_FishFrames
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

void swimmonster_start();

void fish_stand1() {FRAME_STATE(CuTF_FishFrames::FR_SWIM1, fish_stand2); ai_stand();}
void fish_stand2() {FRAME_STATE(CuTF_FishFrames::FR_SWIM2, fish_stand3); ai_stand();}
void fish_stand3() {FRAME_STATE(CuTF_FishFrames::FR_SWIM3, fish_stand4); ai_stand();}
void fish_stand4() {FRAME_STATE(CuTF_FishFrames::FR_SWIM4, fish_stand5); ai_stand();}
void fish_stand5() {FRAME_STATE(CuTF_FishFrames::FR_SWIM5, fish_stand6); ai_stand();}
void fish_stand6() {FRAME_STATE(CuTF_FishFrames::FR_SWIM6, fish_stand7); ai_stand();}
void fish_stand7() {FRAME_STATE(CuTF_FishFrames::FR_SWIM7, fish_stand8); ai_stand();}
void fish_stand8() {FRAME_STATE(CuTF_FishFrames::FR_SWIM8, fish_stand9); ai_stand();}
void fish_stand9() {FRAME_STATE(CuTF_FishFrames::FR_SWIM9, fish_stand10); ai_stand();}
void fish_stand10() {FRAME_STATE(CuTF_FishFrames::FR_SWIM10, fish_stand11); ai_stand();}
void fish_stand11() {FRAME_STATE(CuTF_FishFrames::FR_SWIM11, fish_stand12); ai_stand();}
void fish_stand12() {FRAME_STATE(CuTF_FishFrames::FR_SWIM12, fish_stand13); ai_stand();}
void fish_stand13() {FRAME_STATE(CuTF_FishFrames::FR_SWIM13, fish_stand14); ai_stand();}
void fish_stand14() {FRAME_STATE(CuTF_FishFrames::FR_SWIM14, fish_stand15); ai_stand();}
void fish_stand15() {FRAME_STATE(CuTF_FishFrames::FR_SWIM15, fish_stand16); ai_stand();}
void fish_stand16() {FRAME_STATE(CuTF_FishFrames::FR_SWIM16, fish_stand17); ai_stand();}
void fish_stand17() {FRAME_STATE(CuTF_FishFrames::FR_SWIM17, fish_stand18); ai_stand();}
void fish_stand18() {FRAME_STATE(CuTF_FishFrames::FR_SWIM18, fish_stand1); ai_stand();}

void fish_walk1() {FRAME_STATE(CuTF_FishFrames::FR_SWIM1, fish_walk2); ai_walk(18);} // original is 8
void fish_walk2() {FRAME_STATE(CuTF_FishFrames::FR_SWIM2, fish_walk3); ai_walk(18);}
void fish_walk3() {FRAME_STATE(CuTF_FishFrames::FR_SWIM3, fish_walk4); ai_walk(18);}
void fish_walk4() {FRAME_STATE(CuTF_FishFrames::FR_SWIM4, fish_walk5); ai_walk(18);}
void fish_walk5() {FRAME_STATE(CuTF_FishFrames::FR_SWIM5, fish_walk6); ai_walk(18);}
void fish_walk6() {FRAME_STATE(CuTF_FishFrames::FR_SWIM6, fish_walk7); ai_walk(18);}
void fish_walk7() {FRAME_STATE(CuTF_FishFrames::FR_SWIM7, fish_walk8); ai_walk(18);}
void fish_walk8() {FRAME_STATE(CuTF_FishFrames::FR_SWIM8, fish_walk9); ai_walk(18);}
void fish_walk9() {FRAME_STATE(CuTF_FishFrames::FR_SWIM9, fish_walk10); ai_walk(18);}
void fish_walk10() {FRAME_STATE(CuTF_FishFrames::FR_SWIM10, fish_walk11); ai_walk(18);}
void fish_walk11() {FRAME_STATE(CuTF_FishFrames::FR_SWIM11, fish_walk12); ai_walk(18);}
void fish_walk12() {FRAME_STATE(CuTF_FishFrames::FR_SWIM12, fish_walk13); ai_walk(18);}
void fish_walk13() {FRAME_STATE(CuTF_FishFrames::FR_SWIM13, fish_walk14); ai_walk(18);}
void fish_walk14() {FRAME_STATE(CuTF_FishFrames::FR_SWIM14, fish_walk15); ai_walk(18);}
void fish_walk15() {FRAME_STATE(CuTF_FishFrames::FR_SWIM15, fish_walk16); ai_walk(18);}
void fish_walk16() {FRAME_STATE(CuTF_FishFrames::FR_SWIM16, fish_walk17); ai_walk(18);}
void fish_walk17() {FRAME_STATE(CuTF_FishFrames::FR_SWIM17, fish_walk18); ai_walk(18);}
void fish_walk18() {FRAME_STATE(CuTF_FishFrames::FR_SWIM18, fish_walk1); ai_walk(18);}

void fish_run1() {FRAME_STATE(CuTF_FishFrames::FR_SWIM1, fish_run2); ai_run(28); // original is 12
	if (random() < 0.2)
		sound (self, PR_CHAN_VOICE, "fish/idle.wav", 1, PR_ATTN_NORM);
}
void fish_run2() {FRAME_STATE(CuTF_FishFrames::FR_SWIM3, fish_run3); ai_run(28);}
void fish_run3() {FRAME_STATE(CuTF_FishFrames::FR_SWIM5, fish_run4); ai_run(28);}
void fish_run4() {FRAME_STATE(CuTF_FishFrames::FR_SWIM7, fish_run5); ai_run(28);}
void fish_run5() {FRAME_STATE(CuTF_FishFrames::FR_SWIM9, fish_run6); ai_run(28);}
void fish_run6() {FRAME_STATE(CuTF_FishFrames::FR_SWIM11, fish_run7); ai_run(28);}
void fish_run7() {FRAME_STATE(CuTF_FishFrames::FR_SWIM13, fish_run8); ai_run(28);}
void fish_run8() {FRAME_STATE(CuTF_FishFrames::FR_SWIM15, fish_run9); ai_run(28);}
void fish_run9() {FRAME_STATE(CuTF_FishFrames::FR_SWIM17, fish_run1); ai_run(28);}

void fish_melee()
{
	vector	delta;
	float 	ldmg;
                        // dont bite if enemy dead
	//if (!self.enemy || self.enemy.health <= 0 || self.enemy.has_disconnected)
    if (!StillValidEnemy(self->enemy))
	{
        self->enemy = world;

        fish_run2(); // not f_run1 cause we dont want idle sounds
        return;		// removed before stroke
    }

	delta = self->enemy->origin - self->origin;

	MonsterAuraPower();

    if (vlen(delta) > 85) // Original was 60 THEN 80
		return;

	sound (self, PR_CHAN_VOICE, "fish/bite.wav", 1, PR_ATTN_NORM);

    ldmg = random()* 100 + 25;

    TF_T_Damage(self->enemy, self, self, ldmg, PR_TF_TD_DONTGIB, PR_TF_TD_OTHER);
	//T_Damage (self.enemy, self, self, ldmg);
}

void fish_attack1() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK1, fish_attack2); ai_charge(10);} // original is 10
void fish_attack2() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK2, fish_attack3); ai_charge(10);}
void fish_attack3() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK3, fish_attack4); fish_melee();}
void fish_attack4() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK4, fish_attack5); ai_charge(10);}
void fish_attack5() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK5, fish_attack6); ai_charge(10);}
void fish_attack6() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK6, fish_attack7); ai_charge(10);}
void fish_attack7() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK7, fish_attack8); ai_charge(10);}
void fish_attack8() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK8, fish_attack9); ai_charge(10);}
void fish_attack9() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK9, fish_attack10); fish_melee();}
void fish_attack10() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK10, fish_attack11); ai_charge(10);}
void fish_attack11() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK11, fish_attack12); ai_charge(10);}
void fish_attack12() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK12, fish_attack13); ai_charge(10);}
void fish_attack13() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK13, fish_attack14); ai_charge(10);}
void fish_attack14() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK14, fish_attack15); ai_charge(10);}
void fish_attack15() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK15, fish_attack16); fish_melee();}
void fish_attack16() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK16, fish_attack17); ai_charge(10);}
void fish_attack17() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK17, fish_attack18); ai_charge(10);}
void fish_attack18() {FRAME_STATE(CuTF_FishFrames::FR_ATTACK18, fish_run1); ai_charge(10);}

void fish_death1() {
    FRAME_STATE(CuTF_FishFrames::FR_DEATH1, fish_death2);
    sound (self, PR_CHAN_VOICE, "fish/death.wav", 1, PR_ATTN_MONSTERDIE);

    // Hackish fix for visuals if outside water
    if (pointcontents(self->origin) ==  PR_CONTENT_EMPTY)
        self->movetype = PR_MOVETYPE_TOSS;
}
void fish_death2() {FRAME_STATE(CuTF_FishFrames::FR_DEATH2, fish_death3);}
void fish_death3() {FRAME_STATE(CuTF_FishFrames::FR_DEATH3, fish_death4); self->solid = PR_SOLID_NOT;}
void fish_death4() {FRAME_STATE(CuTF_FishFrames::FR_DEATH4, fish_death5);}
void fish_death5() {FRAME_STATE(CuTF_FishFrames::FR_DEATH5, fish_death6);}
void fish_death6() {FRAME_STATE(CuTF_FishFrames::FR_DEATH6, fish_death7);}
void fish_death7() {FRAME_STATE(CuTF_FishFrames::FR_DEATH7, fish_death8);}
void fish_death8() {FRAME_STATE(CuTF_FishFrames::FR_DEATH8, fish_death9);}
void fish_death9() {FRAME_STATE(CuTF_FishFrames::FR_DEATH9, fish_death10);}
void fish_death10() {FRAME_STATE(CuTF_FishFrames::FR_DEATH10, fish_death11);}
void fish_death11() {FRAME_STATE(CuTF_FishFrames::FR_DEATH11, fish_death12);}
void fish_death12() {FRAME_STATE(CuTF_FishFrames::FR_DEATH12, fish_death13);}
void fish_death13() {FRAME_STATE(CuTF_FishFrames::FR_DEATH13, fish_death14);}
void fish_death14() {FRAME_STATE(CuTF_FishFrames::FR_DEATH14, fish_death15);}
void fish_death15() {FRAME_STATE(CuTF_FishFrames::FR_DEATH15, fish_death16);}
void fish_death16() {FRAME_STATE(CuTF_FishFrames::FR_DEATH16, fish_death17);}
void fish_death17() {FRAME_STATE(CuTF_FishFrames::FR_DEATH17, fish_death18);}
void fish_death18() {FRAME_STATE(CuTF_FishFrames::FR_DEATH18, fish_death19);}
void fish_death19() {FRAME_STATE(CuTF_FishFrames::FR_DEATH19, fish_death20);}
void fish_death20() {FRAME_STATE(CuTF_FishFrames::FR_DEATH20, fish_death21);}
void fish_death21() {
    //self.nextthink = time + 40 + 40*random();
    //self.think = SUB_Remove;

    //self.solid = #SOLID_NOT;
    FRAME_STATE(CuTF_FishFrames::FR_DEATH21, fish_death21);
    MonsterCorpse();
}

void fish_pain1() {FRAME_STATE(CuTF_FishFrames::FR_PAIN1, fish_pain2);}
void fish_pain2() {FRAME_STATE(CuTF_FishFrames::FR_PAIN2, fish_pain3); ai_pain(6);} // original is 6
void fish_pain3() {FRAME_STATE(CuTF_FishFrames::FR_PAIN3, fish_pain4); ai_pain(6);}
void fish_pain4() {FRAME_STATE(CuTF_FishFrames::FR_PAIN4, fish_pain5); ai_pain(6);}
void fish_pain5() {FRAME_STATE(CuTF_FishFrames::FR_PAIN5, fish_pain6); ai_pain(6);}
void fish_pain6() {FRAME_STATE(CuTF_FishFrames::FR_PAIN6, fish_pain7); ai_pain(6);}
void fish_pain7() {FRAME_STATE(CuTF_FishFrames::FR_PAIN7, fish_pain8); ai_pain(6);}
void fish_pain8() {FRAME_STATE(CuTF_FishFrames::FR_PAIN8, fish_pain9); ai_pain(6);}
void fish_pain9() {FRAME_STATE(CuTF_FishFrames::FR_PAIN9, fish_run1); ai_pain(6);}

void fish_pain(entity attacker, float damage)
{
    if (self->health <= 0)
		return;

    if (self->pain_finished > time)
        return;

// fish allways do pain frames - OfN - nope
    if (random() > 0.85 || (damage > random()*50))
        fish_pain1 ();

    AI_CheckAttacker(attacker);

    self->pain_finished = time +1 +random();
}

void fish_death()
{
    if (self->real_owner->classname == "player")
    {
	    sprint(self->real_owner,PR_PRINT_HIGH,"Your piranha ");
        sprint(self->real_owner,PR_PRINT_HIGH,self->netname);
        sprint(self->real_owner,PR_PRINT_HIGH," is dead.\n");
	    self->real_owner->job_finished = time + 5; // delay between summons
        ResetSlotPointer(self->real_owner, self->increase_team1);
    }

// check for gib
    if (self->health < -35) // was 30
	{
		//sound (self, #CHAN_VOICE, "player/udeath.wav", 1, #ATTN_MONSTERDIE);

        ThrowGib ("progs/gib2.mdl", self->health,PR_TRUE,0,PR_GIB2_KGS, PR_FALSE);
		ThrowGib ("progs/gib3.mdl", self->health,PR_TRUE,0,PR_GIB3_KGS, PR_FALSE);

        dremove(self);
        return;
	}

    // show bubbles if fish is in liquid
    if (pointcontents(self->origin) != PR_CONTENT_EMPTY)
        DeathBubbles(4);

    //self.classname = "monster_corpse";
    MonsterDead();
    self->think=SUB_Null;
    self->nextthink = time + 0.1;

    fish_death1();
}

/*QUAKED monster_fish (1 0 0) (-16 -16 -24) (16 16 24) Ambush
*/
/*
void() monster_fish =
{
	if (deathmatch)
	{
		dremove(self);
		return;
	}
	precache_model2 ("progs/fish.mdl");

	precache_sound2 ("fish/death.wav");
	precache_sound2 ("fish/bite.wav");
	precache_sound2 ("fish/idle.wav");

	self.solid = #SOLID_SLIDEBOX;
	self.movetype = #MOVETYPE_STEP;

	setmodel (self, "progs/fish.mdl");

	setsize (self, '-16 -16 -24', '16 16 24');
	self.health = 25;

	self.th_stand = f_stand1;
	self.th_walk = f_walk1;
	self.th_run = f_run1;
	self.th_die = fish_death;
	self.th_pain = fish_pain;
	self.th_melee = f_attack1;

	swimmonster_start ();
};*/

void Fish_Precache()
{
    precache_model ("progs/fish.mdl");

	precache_sound ("fish/death.wav");
	precache_sound ("fish/bite.wav");
	precache_sound ("fish/idle.wav");
}

void fish_toss()
{
    float contenttype;
    contenttype = pointcontents(self->origin);

    if (contenttype != PR_CONTENT_EMPTY) // we have been tossed and now we got into water/slime or lava
    {
        if (contenttype == PR_CONTENT_SKY || contenttype == PR_CONTENT_SOLID) // on a invalid place
        {
            sprint(self->real_owner,PR_PRINT_HIGH,"Your piranha gets into the twilight zone!\n");
            self->health = -50;
            MonsterDie(self);
            return;
        }

        // make splash sound (we got into liquid)
        sound(self,PR_CHAN_BODY,"player/inh2o.wav",1,PR_ATTN_NORM);

        DeathBubbles(3); // Not dying tho

        // ok, fish is ready to go..
        self->movetype = PR_MOVETYPE_STEP;
        self->velocity = V({0, 0, 0});
        self->think = swimmonster_start_go;
        self->nextthink = time + 0.1;
        return;
    }

    // are we on ground? if so, die (also die if 15 seconds passed since toss)
    if (self->flags & PR_FL_ONGROUND || time > self->ltime + 15)
    {
        self->dmgtime = time; // check now
        self->nextthink = time + 0.1;
        self->think = swimmonster_start_go; // will call AI_check_contents
    }
}

void fish_toss1() {FRAME_STATE(CuTF_FishFrames::FR_SWIM1, fish_toss2); fish_toss();}
void fish_toss2() {FRAME_STATE(CuTF_FishFrames::FR_SWIM2, fish_toss3); fish_toss();}
void fish_toss3() {FRAME_STATE(CuTF_FishFrames::FR_SWIM3, fish_toss4); fish_toss();}
void fish_toss4() {FRAME_STATE(CuTF_FishFrames::FR_SWIM4, fish_toss5); fish_toss();}
void fish_toss5() {FRAME_STATE(CuTF_FishFrames::FR_SWIM5, fish_toss6); fish_toss();}
void fish_toss6() {FRAME_STATE(CuTF_FishFrames::FR_SWIM6, fish_toss7); fish_toss();}
void fish_toss7() {FRAME_STATE(CuTF_FishFrames::FR_SWIM7, fish_toss8); fish_toss();}
void fish_toss8() {FRAME_STATE(CuTF_FishFrames::FR_SWIM8, fish_toss9); fish_toss();}
void fish_toss9() {FRAME_STATE(CuTF_FishFrames::FR_SWIM9, fish_toss10); fish_toss();}
void fish_toss10() {FRAME_STATE(CuTF_FishFrames::FR_SWIM10, fish_toss11); fish_toss();}
void fish_toss11() {FRAME_STATE(CuTF_FishFrames::FR_SWIM11, fish_toss12); fish_toss();}
void fish_toss12() {FRAME_STATE(CuTF_FishFrames::FR_SWIM12, fish_toss13); fish_toss();}
void fish_toss13() {FRAME_STATE(CuTF_FishFrames::FR_SWIM13, fish_toss14); fish_toss();}
void fish_toss14() {FRAME_STATE(CuTF_FishFrames::FR_SWIM14, fish_toss15); fish_toss();}
void fish_toss15() {FRAME_STATE(CuTF_FishFrames::FR_SWIM15, fish_toss16); fish_toss();}
void fish_toss16() {FRAME_STATE(CuTF_FishFrames::FR_SWIM16, fish_toss17); fish_toss();}
void fish_toss17() {FRAME_STATE(CuTF_FishFrames::FR_SWIM17, fish_toss18); fish_toss();}
void fish_toss18() {FRAME_STATE(CuTF_FishFrames::FR_SWIM18, fish_toss1); fish_toss();}

//TODO: void PiranhaElectric() with checks for start of electricity charge, sets lits on fish and gets called EVERYWHERE

} // END namespace Progs
