/*======================================================
	PLAYER.QC			Custom TeamFortress v3.1

	(c) TeamFortress Software Pty Ltd 	29/2/97
	(c) William Kerney			5/21/00
	(c) Craig Hauser				19/3/00
========================================================
Handles all player animations, death and etc. Most weapon
firings get shunted through here.
======================================================*/
/*
	Heavily Modified for TeamFortress V1.21
	TeamFortress Software
	Robin Walker, John Cook, Ian Caughley
*/

#include "progs.h"
#include "player.h"
#include "ofndefs.h"
#include "custom.h"
#include "spy.h"
#include "cpstuff.h"
#include "tfort.h"
#include "qw.h"
#include "weapons.h"
#include "debug.h"
#include "items.h"
#include "neo.h"

namespace Progs {

// Prototypes
void bubble_bob();
void W_FireAssaultCannon();
void W_FireLightAssault();

void Throw_Grapple();

// TeamFortress Prototypes
void BioInfection_Decay();
void TeamFortress_RemoveTimers();
void T_Dispenser();
//void() Headless_Think;

// OfN
void MakeCorpse(entity deadone, float corpse_tolerance);
void SpawnBloodEx(const vector& where, float bloodtype, float bloodammount);

/*
==============================================================================

PLAYER

==============================================================================
*/

//$cd /raid/quake/id1/models/player_4
//$origin 0 -6 24
//$base base
//$skin skin

// PZ: Moved PlayerFrames to header file.

/*
==============================================================================
PLAYER
==============================================================================
*/
void player_touch()
{
	//local entity te;

	// Spies and Scouts can uncloak enemy spies
	// WK Or those with Spy kits & Scanners
	if (invis_only == PR_FALSE && (self->cutf_items & PR_CUTF_SPY_KIT || other->cutf_items & PR_CUTF_SPY_KIT))
	{
		if (other->classname == "player")
		{
			if (self->undercover_team != 0 || self->undercover_skin != 0)
			{
				if ((other->cutf_items & PR_CUTF_SPY_KIT || other->tf_items & PR_NIT_SCANNER) && !Teammate(self, other))
				{
					sprint(other, PR_PRINT_HIGH, "Wait a minute... he's a Spy!\n");
					sprint(self, PR_PRINT_HIGH, "The enemy sees through your disguise!\n");
					Spy_RemoveDisguise(self);
				}
			}
			if (other->undercover_team != 0 || other->undercover_skin != 0)
			{
				if ((self->cutf_items & PR_CUTF_SPY_KIT || self->tf_items & PR_NIT_SCANNER) && !Teammate(other, self))
				{
					sprint(self, PR_PRINT_HIGH, "Wait a minute... he's a Spy!\n");
					sprint(other, PR_PRINT_HIGH, "The enemy sees through your disguise!\n");
					Spy_RemoveDisguise(other);
				}
			}
		}
	}

	// Pass on infections
	if (self->tfstate & PR_TFSTATE_INFECTED)
	{
		entity Bio;

        //WK Melee armor stops infections from spreading
		if (other->classname == "player" && !(other->invincible_finished) && !(other->armorclass & PR_AT_SAVEMELEE))
		{
			// may sure other player is not already infected
			if (!(other->tfstate & PR_TFSTATE_INFECTED))
			{
				// can't infect the medic
				if (!(other->weapons_carried & PR_WEAP_MEDIKIT)) //WK
				{
					// infect 'em
					Bio = spawnServerSide(); // PZ: make it a server-side only entity
					Bio->nextthink = 2;
					Bio->think = BioInfection_Decay;
/* WK Why doesn't this work??? Causes occasional program crashes...
					//WK The owner is the original medic if spreading to friend
					if (self.team_no == other.team_no) {
						te = find(world, netname, "biotimer");
						while ((te.owner != self) && (te != world))
							te = find(te, netname, "biotimer");
						if (te != world)
							Bio.enemy = te.enemy; //Person infecting
						else
							RPrint("CustomTF: Odd Behavior in infection spreading");
					}
					else */
						Bio->enemy = self; //Person causing infection
					Bio->owner = other; //Person getting hurt
					Bio->classname = "timer";
					Bio->netname = "biotimer";

					other->tfstate = other->tfstate | PR_TFSTATE_INFECTED;
					other->infection_team_no = self->infection_team_no;

					sprint(other, PR_PRINT_MEDIUM, "You have been infected by ");
					sprint(other, PR_PRINT_MEDIUM, self->netname);
					sprint(other, PR_PRINT_MEDIUM, "!\n");
					sprint(self, PR_PRINT_MEDIUM, "You have infected ");
					sprint(self, PR_PRINT_MEDIUM, other->netname);
					sprint(self, PR_PRINT_MEDIUM, "!\n");
                    //-OfN
					teamprefixsprintbi(other->team_no,other,self);
					teamsprintbi(other,self, other->netname," has been infected!\n","","","","");
				}
			}
		}
        else if (IsOwnedMonster(other))
        {
            if (!(other->tfstate & PR_TFSTATE_INFECTED))
            {
                // Report to owner
                string tmps;
                tmps = GetMonsterName(other);
                sprint(other->real_owner,PR_PRINT_HIGH,"Your ",tmps," ",other->netname," has been infected!\n");

                // Report infecter
                sprint(self,PR_PRINT_HIGH,"You have infected the ",tmps," ",other->netname,"!\n");

                // Report to teammates of owner
                tmps = GetMonsterName(other);
                teamprefixsprint(other->real_owner->team_no,other->real_owner);
                teamsprint6(other->real_owner,"The ",tmps," ", other->netname," has been infected!\n","");

                // Update infection flag
                other->tfstate = other->tfstate | PR_TFSTATE_INFECTED;

                // Infect it!
                Bio = spawnServerSide(); // PZ: make it a server-side only entity
                Bio->classname = "timer";
                Bio->classname = "biotimer";
                Bio->nextthink = time + 2;
                Bio->think = BioInfection_MonsterDecay;
                Bio->owner = self;
                Bio->enemy = other;

                other->infection_team_no = self->infection_team_no;
            }
        }
#ifdef PR_COOP_MODE_ENHANCED
		else if ( COOP_IsCoopMonster( other ) ) {
			// Gizmo - allow non demons and non army soldiers to be affected (coop monsters)
			if (!(other->tfstate & PR_TFSTATE_INFECTED))
			{
				// Report infecter
				sprint(self, PR_PRINT_HIGH, "You have infected ", other->netname, "!\n");

				// Update infection flag
				other->tfstate = other->tfstate | PR_TFSTATE_INFECTED;

				// Infect it!
				Bio = spawnServerSide(); // PZ: make it a server-side only entity
				Bio->classname = "timer";
				Bio->classname = "biotimer";
				Bio->nextthink = time + 2;
				Bio->think = BioInfection_MonsterDecay;
				Bio->owner = self;
				Bio->enemy = other;

				other->infection_team_no = self->infection_team_no;
			}
		}
#endif
	}

#ifdef PR_PLAYER_PUSHING

    float v1, v2;

    if (other->classname == "player")
	{
		if (Teammate(self, other))
		{
			v1 = vlen(other->velocity);
			if (v1 < 50)
			{
				v2 = vlen(self->velocity);
				if (v2 > 55 || (self->items & (PR_IT_KEY1 | PR_IT_KEY2)))
				{
					makevectors(self->angles);
					traceline (self->origin, self->origin + v_forward*32, PR_TL_ANY_SOLID, self);
					if (trace_ent == other)
					{
						if (self->items & (PR_IT_KEY1 | PR_IT_KEY2))
							other->velocity = other->velocity + (normalize(self->velocity) * 120);
						else if (!(other->items & (PR_IT_KEY1 | PR_IT_KEY2)))
							other->velocity = other->velocity + (normalize(self->velocity) * 30);
					}
				}
			}
		}
	}

#endif

}

void player_run();

void player_stand1()
{
	FRAME_STATE(PlayerFrames::FR_AXSTND1, player_stand1);
	self->weaponframe=0;
	if (self->velocity[X] || self->velocity[Y])
	{
		self->walkframe=0;
		player_run();
		return;
	}

	if (self->current_weapon <= PR_WEAP_AXE)
	{
		if (self->walkframe >= 12)
			self->walkframe = 0;
		self->frame = PlayerFrames::FR_AXSTND1 + self->walkframe;
	}
	else
	{
		if (self->walkframe >= 5)
			self->walkframe = 0;
		self->frame = PlayerFrames::FR_STAND1 + self->walkframe;
	}
	self->walkframe = self->walkframe + 1;
}

void player_run()
{
	FRAME_STATE(PlayerFrames::FR_ROCKRUN1, player_run);
	self->weaponframe=0;
	if (!self->velocity[X] && !self->velocity[Y])
	{
		self->walkframe=0;
		player_stand1();
		return;
	}

	if (self->current_weapon <= PR_WEAP_AXE)
	{
		if (self->walkframe >= 6)
			self->walkframe = 0;
		self->frame = PlayerFrames::FR_AXRUN1 + self->walkframe;
	}
	else
	{
		if (self->walkframe >= 6)
			self->walkframe = 0;
		self->frame = self->frame + self->walkframe;
	}
	self->walkframe = self->walkframe + 1;
}


void player_shot1()	 {FRAME_STATE(PlayerFrames::FR_SHOTATT1, player_shot2); self->weaponframe=1;muzzleflash();}
void player_shot2()	 {FRAME_STATE(PlayerFrames::FR_SHOTATT2, player_shot3); self->weaponframe=2;}
void player_shot3()	 {FRAME_STATE(PlayerFrames::FR_SHOTATT3, player_shot4); self->weaponframe=3;}
void player_shot4()	 {FRAME_STATE(PlayerFrames::FR_SHOTATT4, player_shot5); self->weaponframe=4;}
void player_shot5()	 {FRAME_STATE(PlayerFrames::FR_SHOTATT5, player_shot6); self->weaponframe=5;}
void player_shot6()	 {FRAME_STATE(PlayerFrames::FR_SHOTATT6, player_run); self->weaponframe=6;}

void player_autorifle1()	 {FRAME_STATE(PlayerFrames::FR_SHOTATT1, player_autorifle2); self->weaponframe=1; muzzleflash(); }
void player_autorifle2()	 {FRAME_STATE(PlayerFrames::FR_SHOTATT2, player_autorifle3); self->weaponframe=2;}
void player_autorifle3()	 {FRAME_STATE(PlayerFrames::FR_SHOTATT6, player_run); self->weaponframe=6;}

void player_axe1()	 {FRAME_STATE(PlayerFrames::FR_AXATT1, player_axe2); self->weaponframe=1;}
void player_axe2()	 {FRAME_STATE(PlayerFrames::FR_AXATT2, player_axe3); self->weaponframe=2;}
void player_axe3()
{
	FRAME_STATE(PlayerFrames::FR_AXATT3, player_axe4);
	self->weaponframe=3;
	if (self->current_weapon == PR_WEAP_AXE)
		W_FireAxe();
	else // if (self.current_weapon == #WEAP_SPANNER)
		W_FireSpanner();
}
void player_axe4()	 {FRAME_STATE(PlayerFrames::FR_AXATT4, player_run); self->weaponframe=4;}

void player_axeb1()	 {FRAME_STATE(PlayerFrames::FR_AXATTB1, player_axeb2); self->weaponframe=5;}
void player_axeb2()	 {FRAME_STATE(PlayerFrames::FR_AXATTB2, player_axeb3); self->weaponframe=6;}
void player_axeb3()
{
	FRAME_STATE(PlayerFrames::FR_AXATTB3, player_axeb4);
	self->weaponframe=7;
	if (self->current_weapon == PR_WEAP_AXE)
		W_FireAxe();
	else // if (self.current_weapon == #WEAP_SPANNER)
		W_FireSpanner();
}
void player_axeb4()	 {FRAME_STATE(PlayerFrames::FR_AXATTB4, player_run); self->weaponframe=8;}

void player_axec1()	 {FRAME_STATE(PlayerFrames::FR_AXATTC1, player_axec2); self->weaponframe=1;}
void player_axec2()	 {FRAME_STATE(PlayerFrames::FR_AXATTC2, player_axec3); self->weaponframe=2;}
void player_axec3()
{
	FRAME_STATE(PlayerFrames::FR_AXATTC3, player_axec4);
	self->weaponframe=3;
	if (self->current_weapon == PR_WEAP_AXE)
		W_FireAxe();
	else // if (self.current_weapon == #WEAP_SPANNER)
		W_FireSpanner();
}
void player_axec4()	 {FRAME_STATE(PlayerFrames::FR_AXATTC4, player_run); self->weaponframe=4;}

void player_axed1()	 {FRAME_STATE(PlayerFrames::FR_AXATTD1, player_axed2); self->weaponframe=5;}
void player_axed2()	 {FRAME_STATE(PlayerFrames::FR_AXATTD2, player_axed3); self->weaponframe=6;}
void player_axed3()
{
	FRAME_STATE(PlayerFrames::FR_AXATTD3, player_axed4);
	self->weaponframe=7;
	if (self->current_weapon == PR_WEAP_AXE)
		W_FireAxe();
	else // if (self.current_weapon == #WEAP_SPANNER)
		W_FireSpanner();
}
void player_axed4()	 {FRAME_STATE(PlayerFrames::FR_AXATTD4, player_run); self->weaponframe=8;}

//=========================
// QW Grappling hook frames
void player_chain1()   {FRAME_STATE(PlayerFrames::FR_AXATTD1, player_chain1a); self->weaponframe=1;Throw_Grapple();}
void player_chain1a()  {FRAME_STATE(PlayerFrames::FR_AXATTD1, player_chain2); self->weaponframe=2;}
void player_chain2()   {FRAME_STATE(PlayerFrames::FR_AXATTD2, player_chain2a); self->weaponframe=3;}
void player_chain2a()  {FRAME_STATE(PlayerFrames::FR_AXATTD2, player_chain3); self->weaponframe=4;}
void player_chain3()
{
        FRAME_STATE(PlayerFrames::FR_AXATTD3, player_chain3);
        self->weaponframe=6;
        if (!self->hook_out)
        {
                player_chain5();
                return;
        }
        if (vlen(self->velocity) >= 750)
        {
                player_chain4();
                return;
        }
}

void player_chain4()
{
        FRAME_STATE(PlayerFrames::FR_DEATHC4, player_chain4);
        self->weaponframe=6;
        if (!self->hook_out)
        {
                player_chain5();
                return;
        }
        if (vlen(self->velocity) < 750)
        {
                player_chain3();
                return;
        }
}

void player_chain5()   {FRAME_STATE(PlayerFrames::FR_AXATTD4, player_run); self->weaponframe=0;}

// newstuff

void player_medikit1()	 {FRAME_STATE(PlayerFrames::FR_AXATT1, player_medikit2); self->weaponframe=1;}
void player_medikit2()	 {FRAME_STATE(PlayerFrames::FR_AXATT2, player_medikit3); self->weaponframe=2;}
void player_medikit3()	 {FRAME_STATE(PlayerFrames::FR_AXATT3, player_medikit4); self->weaponframe=3;W_FireMedikit(PR_FALSE);}
void player_medikit4()	 {FRAME_STATE(PlayerFrames::FR_AXATT4, player_run); self->weaponframe=4;}

void player_medikitb1()	 {FRAME_STATE(PlayerFrames::FR_AXATTB1, player_medikitb2); self->weaponframe=5;}
void player_medikitb2()	 {FRAME_STATE(PlayerFrames::FR_AXATTB2, player_medikitb3); self->weaponframe=6;}
void player_medikitb3()	 {FRAME_STATE(PlayerFrames::FR_AXATTB3, player_medikitb4); self->weaponframe=7;W_FireMedikit(PR_FALSE);}
void player_medikitb4()	 {FRAME_STATE(PlayerFrames::FR_AXATTB4, player_run); self->weaponframe=8;}

void player_medikitc1()	 {FRAME_STATE(PlayerFrames::FR_AXATTC1, player_medikitc2); self->weaponframe=1;}
void player_medikitc2()	 {FRAME_STATE(PlayerFrames::FR_AXATTC2, player_medikitc3); self->weaponframe=2;}
void player_medikitc3()	 {FRAME_STATE(PlayerFrames::FR_AXATTC3, player_medikitc4); self->weaponframe=3;W_FireMedikit(PR_FALSE);}
void player_medikitc4()	 {FRAME_STATE(PlayerFrames::FR_AXATTC4, player_run); self->weaponframe=4;}

void player_medikitd1()	 {FRAME_STATE(PlayerFrames::FR_AXATTD1, player_medikitd2); self->weaponframe=5;}
void player_medikitd2()	 {FRAME_STATE(PlayerFrames::FR_AXATTD2, player_medikitd3); self->weaponframe=6;}
void player_medikitd3()	 {FRAME_STATE(PlayerFrames::FR_AXATTD3, player_medikitd4); self->weaponframe=7;W_FireMedikit(PR_FALSE);}
void player_medikitd4()	 {FRAME_STATE(PlayerFrames::FR_AXATTD4, player_run); self->weaponframe=8;}

void player_bioweapon1()	 {FRAME_STATE(PlayerFrames::FR_AXATT1, player_bioweapon2); self->weaponframe=1;}
void player_bioweapon2()	 {FRAME_STATE(PlayerFrames::FR_AXATT2, player_bioweapon3); self->weaponframe=2;}
void player_bioweapon3()	 {FRAME_STATE(PlayerFrames::FR_AXATT3, player_bioweapon4); self->weaponframe=3;W_FireBioweapon();}
void player_bioweapon4()	 {FRAME_STATE(PlayerFrames::FR_AXATT4, player_run); self->weaponframe=4;}

void player_bioweaponb1()	 {FRAME_STATE(PlayerFrames::FR_AXATTB1, player_bioweaponb2); self->weaponframe=5;}
void player_bioweaponb2()	 {FRAME_STATE(PlayerFrames::FR_AXATTB2, player_bioweaponb3); self->weaponframe=6;}
void player_bioweaponb3()	 {FRAME_STATE(PlayerFrames::FR_AXATTB3, player_bioweaponb4); self->weaponframe=7;W_FireBioweapon();}
void player_bioweaponb4()	 {FRAME_STATE(PlayerFrames::FR_AXATTB4, player_run); self->weaponframe=8;}

void player_bioweaponc1()	 {FRAME_STATE(PlayerFrames::FR_AXATTC1, player_bioweaponc2); self->weaponframe=1;}
void player_bioweaponc2()	 {FRAME_STATE(PlayerFrames::FR_AXATTC2, player_bioweaponc3); self->weaponframe=2;}
void player_bioweaponc3()	 {FRAME_STATE(PlayerFrames::FR_AXATTC3, player_bioweaponc4); self->weaponframe=3;W_FireBioweapon();}
void player_bioweaponc4()	 {FRAME_STATE(PlayerFrames::FR_AXATTC4, player_run); self->weaponframe=4;}

void player_bioweapond1()	 {FRAME_STATE(PlayerFrames::FR_AXATTD1, player_bioweapond2); self->weaponframe=5;}
void player_bioweapond2()	 {FRAME_STATE(PlayerFrames::FR_AXATTD2, player_bioweapond3); self->weaponframe=6;}
void player_bioweapond3()	 {FRAME_STATE(PlayerFrames::FR_AXATTD3, player_bioweapond4); self->weaponframe=7;W_FireBioweapon();}
void player_bioweapond4()	 {FRAME_STATE(PlayerFrames::FR_AXATTD4, player_run); self->weaponframe=8;}

//============================================================================

void player_snail1()
{

	FRAME_STATE(PlayerFrames::FR_NAILATT1, player_snail2);

	if (!self->PR_BUTTON_FIRE || intermission_running)
		{player_run ();return;}
	//WK
	self->weaponframe = self->weaponframe + 1;
	if (self->weaponframe == 9)
		self->weaponframe = 1;

    if (self->attack_finished > time)
		return;
	muzzleflash();

	/*self.weaponframe = self.weaponframe + 1;
	if (self.weaponframe == 9)
		self.weaponframe = 1;*/

	SuperDamageSound();
	W_FireSpikes (4);
	//Attack_Finished(0.1);
  	Attack_Finished(0.2);
}
//- OfN -

void player_snail2()
{
	/*if (!self.#BUTTON_FIRE || intermission_running)
		{player_run ();return;}*/
	//WK
	FRAME_STATE(PlayerFrames::FR_NAILATT2, player_snail1);
	self->weaponframe = self->weaponframe + 1;
	if (self->weaponframe == 9)
		self->weaponframe = 1;

    /*if (self.attack_finished > time)
		return;*/

	/*self.weaponframe = self.weaponframe + 1;
	if (self.weaponframe == 9)
		self.weaponframe = 1;*/

	//SuperDamageSound();
//	if (random() < 0.5) W_FireSpikes (4);
	//Attack_Finished(0.2);
    //Attack_Finished(0.1);
}

void player_nail1()
{

	FRAME_STATE(PlayerFrames::FR_NAILATT1, player_nail2);

	if (!self->PR_BUTTON_FIRE || intermission_running)
		{player_run ();return;}
	//WK
	self->weaponframe = self->weaponframe + 1;
	if (self->weaponframe == 9)
		self->weaponframe = 1;

    if (self->attack_finished > time)
		return;
	muzzleflash();

	/*self.weaponframe = self.weaponframe + 1;
	if (self.weaponframe == 9)
		self.weaponframe = 1;*/

	SuperDamageSound();
	W_FireSpikes (4);
	//Attack_Finished(0.1);
  	Attack_Finished(0.2);
}
//- OfN -

void player_nail2()
{
	/*if (!self.#BUTTON_FIRE || intermission_running)
		{player_run ();return;}*/
	//WK
	FRAME_STATE(PlayerFrames::FR_NAILATT2, player_nail3);
	self->weaponframe = self->weaponframe + 1;
	if (self->weaponframe == 9)
		self->weaponframe = 1;

    /*if (self.attack_finished > time)
		return;*/

	/*self.weaponframe = self.weaponframe + 1;
	if (self.weaponframe == 9)
		self.weaponframe = 1;*/

	//SuperDamageSound();
//	if (random() < 0.5) W_FireSpikes (4);
	//Attack_Finished(0.2);
    //Attack_Finished(0.1);
}


void player_nail3()
{
	FRAME_STATE(PlayerFrames::FR_NAILATT1, player_nail4);
	if (!self->PR_BUTTON_FIRE || intermission_running)
		{player_run ();return;}
	//WK
	self->weaponframe = self->weaponframe + 1;
	if (self->weaponframe == 9)
		self->weaponframe = 1;

    if (self->attack_finished > time)
		return;

	/*self.weaponframe = self.weaponframe + 1;
	if (self.weaponframe == 9)
		self.weaponframe = 1;*/

   	muzzleflash();
	SuperDamageSound();
	W_FireSpikes (-4);
	//Attack_Finished(0.2);
    Attack_Finished(0.2);
}


void player_nail4()
{
	/*if (!self.#BUTTON_FIRE || intermission_running)
		{player_run ();return;}*/
	//WK
	FRAME_STATE(PlayerFrames::FR_NAILATT2, player_nail1);
	self->weaponframe = self->weaponframe + 1;
	if (self->weaponframe == 9)
		self->weaponframe = 1;

    /*if (self.attack_finished > time)
		return;*/

	/*self.weaponframe = self.weaponframe + 1;
	if (self.weaponframe == 9)
		self.weaponframe = 1;*/

	//SuperDamageSound();
//	if (random() < 0.5) W_FireSpikes (-4);
	//Attack_Finished(0.2);
    //Attack_Finished(0.1);
}


//============================================================================
// ASSAULT CANNON FRAMES
// Cannon powering up
void player_assaultcannonup1()
{
	FRAME_STATE(PlayerFrames::FR_NAILATT1, player_assaultcannonup2);
	if ((!self->PR_BUTTON_FIRE) || (self->ammo_shells < 1) || intermission_running)
	{
		// Let him/her walk again
		//WK 2/8/7 Ass cannons can walk now
		//self.tfstate = self.tfstate - (self.tfstate & #TFSTATE_CANT_MOVE);
		TeamFortress_SetSpeed(self);

		self->count = 1;
		self->heat = 0;
		player_assaultcannondown1();
		return;
	}

    self->fire_held_down = PR_TRUE;

	if (self->heat == 1)
		sound (self, PR_CHAN_WEAPON, "weapons/asscan1.wav", 1, PR_ATTN_NORM);

	SuperDamageSound();
	Attack_Finished(0.1);

	if (self->heat != 2 && self->heat != 4)
	{
		if (self->weaponframe >= 3)
			self->weaponframe = 0;
		else
			self->weaponframe = self->weaponframe + 1;
	}

	self->heat = self->heat + 1;
	if (self->heat >= 7)
	{
		self->heat = 0;
		player_assaultcannon1();
	}
}

// Cannon powering up 2
void player_assaultcannonup2()
{
	FRAME_STATE(PlayerFrames::FR_NAILATT1, player_assaultcannonup1);
	if ((!self->PR_BUTTON_FIRE) || (self->ammo_shells < 1) || intermission_running)
	{
		//WK 2/8/7 Ass cannons can walk now
            //self.tfstate = self.tfstate - (self.tfstate & #TFSTATE_CANT_MOVE);
		    TeamFortress_SetSpeed(self);
		self->count = 1;
		self->heat = 0;
		player_assaultcannondown1();
		return;
	}

	SuperDamageSound();
	Attack_Finished(0.1);

	if (self->heat != 2 && self->heat != 4 && self->heat != 7)
	{
		if (self->weaponframe == 2 && self->heat >= 9)
			self->weaponframe = 0;
		else if (self->weaponframe >= 3)
			self->weaponframe = 0;
		else
			self->weaponframe = self->weaponframe + 1;
	}

	self->heat = self->heat + 1;
	if (self->heat >= 13)
	{
		self->heat = 0;
		player_assaultcannon1();
	}
}

// Cannon Firing
void player_assaultcannon1()
{
	FRAME_STATE(PlayerFrames::FR_NAILATT1, player_assaultcannon2);
	string st;

	muzzleflash();

	sound (self, PR_CHAN_WEAPON, "weapons/asscan2.wav", 1, PR_ATTN_NORM);

	if ((!self->PR_BUTTON_FIRE) || (self->ammo_shells < 1) || intermission_running)
	{
		stuffcmd(self, "v_idlescale 0\n");
		// Let him/her walk again
        //WK 2/8/7 Ass cannnons can walk now
	  //self.tfstate = self.tfstate - (self.tfstate & #TFSTATE_CANT_MOVE);
        TeamFortress_SetSpeed(self);

		self->weaponframe = 0;
		self->count = 1;
		player_assaultcannondown1();
		return;
	}

	if (self->weaponframe == 2)
		self->weaponframe = 4;
	else
		self->weaponframe = 2;
	SuperDamageSound();
	W_FireAssaultCannon();

	// Shake the screen
	stuffcmd(self, "v_idlescale ");
	if (self->heat < 5)
		st = ftos(self->heat * 4);
	else
		st = "20";
	stuffcmd(self, st);
	stuffcmd(self, "\n");
	Attack_Finished(0.1);
}

// Cannon Firing 2
void player_assaultcannon2()
{
	//local string st;

	FRAME_STATE(PlayerFrames::FR_NAILATT2, player_assaultcannon1);

	if ((!self->PR_BUTTON_FIRE) || (self->ammo_shells < 1) || intermission_running)
	{
		stuffcmd(self, "v_idlescale 0\n");
		// Let him/her walk again
	    //WK 2/8/7 Ass cannons can walk again
	    //self.tfstate = self.tfstate - (self.tfstate & #TFSTATE_CANT_MOVE);
	    TeamFortress_SetSpeed(self);

		self->weaponframe = 0;
		self->count = 1;
		player_assaultcannondown1();
		return;
	}

	if (self->weaponframe == 2)
		self->weaponframe = 4;
	else
		self->weaponframe = 2;
	SuperDamageSound();
	W_FireAssaultCannon();

	// increase the heat of the cannon
	self->heat = self->heat + 0.1;
	// Shake the screen
	stuffcmd(self, "v_idlescale 0\n");
	stuffcmd(self, "bf\n");

	Attack_Finished(0.1);
}

// Cannon powering down
void player_assaultcannondown1()
{
	//local entity tcool;

	FRAME_STATE(PlayerFrames::FR_NAILATT1, player_assaultcannondown1);

	if (self->count == 1)
		sound (self, PR_CHAN_WEAPON, "weapons/asscan3.wav", 1, PR_ATTN_NORM);

	if (self->count >= 15)
	{
		self->heat = 0;
	    self->fire_held_down = PR_FALSE;

		// Let him/her walk again
	    //WK self.tfstate = self.tfstate - (self.tfstate & #TFSTATE_CANT_MOVE);
	    TeamFortress_SetSpeed(self);

		if (self->ammo_shells < 1 || self->ammo_cells < 6)
		{
			self->current_weapon = W_BestWeapon();
			W_SetCurrentAmmo();
			W_PrintWeaponMessage();
			return;
		}

		player_run ();
		return;
	}

	if (self->count != 8 && self->count != 10 && self->count != 12 && self->count != 14)
	{
		if (self->weaponframe == 3)
			self->weaponframe = 0;
		else
			self->weaponframe = self->weaponframe + 1;
	}

	self->count = self->count + 1;
	Attack_Finished(0.1);
}

//============================================================================

void player_light1()
{
	FRAME_STATE(PlayerFrames::FR_LIGHT1, player_light2);
	muzzleflash();

	if (!self->PR_BUTTON_FIRE || intermission_running)
		{player_run ();return;}
	self->weaponframe = self->weaponframe + 1;
	if (self->weaponframe == 5)
		self->weaponframe = 1;
	SuperDamageSound();
	W_FireLightning();
	Attack_Finished(0.2);
}
void player_light2()
{
	FRAME_STATE(PlayerFrames::FR_LIGHT2, player_light1);
	if (!self->PR_BUTTON_FIRE || intermission_running)
		{player_run ();return;}
	self->weaponframe = self->weaponframe + 1;
	if (self->weaponframe == 5)
		self->weaponframe = 1;
	SuperDamageSound();
	W_FireLightning();
	Attack_Finished(0.2);
}

//============================================================================
//WK Frames for the new light assault gun
//(Using the frames for the lightning gun)

void player_light_assault1()
{
	//muzzleflash();

    FRAME_STATE(PlayerFrames::FR_LIGHT1, player_light_assault2);

    if (!self->PR_BUTTON_FIRE || intermission_running || self->tfstate & PR_TFSTATE_RELOADING)
		{player_run ();return;}

    muzzleflash(); //- oFn moved

    self->weaponframe = self->weaponframe + 1;
	if (self->weaponframe == 5)
		self->weaponframe = 1;
	SuperDamageSound();
	W_FireLightAssault();
	Attack_Finished(0.2);
}
void player_light_assault2()
{
	FRAME_STATE(PlayerFrames::FR_LIGHT2, player_light_assault1);
	if (!self->PR_BUTTON_FIRE || intermission_running || self->tfstate & PR_TFSTATE_RELOADING)
		{player_run ();return;}
	self->weaponframe = self->weaponframe + 1;
	if (self->weaponframe == 5)
		self->weaponframe = 1;
	SuperDamageSound();
	W_FireLightAssault();
	Attack_Finished(0.2);
}
//End WK


//============================================================================


void player_rocket1() {FRAME_STATE(PlayerFrames::FR_ROCKATT1, player_rocket2); self->weaponframe=1;	muzzleflash();}
void player_rocket2() {FRAME_STATE(PlayerFrames::FR_ROCKATT2, player_rocket3); self->weaponframe=2;}
void player_rocket3() {FRAME_STATE(PlayerFrames::FR_ROCKATT3, player_rocket4); self->weaponframe=3;}
void player_rocket4() {FRAME_STATE(PlayerFrames::FR_ROCKATT4, player_rocket5); self->weaponframe=4;}
void player_rocket5() {FRAME_STATE(PlayerFrames::FR_ROCKATT5, player_rocket6); self->weaponframe=5;}
void player_rocket6() {FRAME_STATE(PlayerFrames::FR_ROCKATT6, player_run); self->weaponframe=6;}
void DeathBubbles(float num_bubbles);

void PainSound()
{
float		rs;

	if (self->health < 0)
		return;

	if (damage_attacker->classname == "teledeath")
	{
		sound (self, PR_CHAN_VOICE, "player/teledth1.wav", 1, PR_ATTN_NONE);
		return;
	}

	// water pain sounds
	if (self->watertype == PR_CONTENT_WATER && self->waterlevel == 3)
	{
		DeathBubbles(1);
		if (random() > 0.5)
			sound (self, PR_CHAN_VOICE, "player/drown1.wav", 1, PR_ATTN_NORM);
		else
			sound (self, PR_CHAN_VOICE, "player/drown2.wav", 1, PR_ATTN_NORM);
		return;
	}

	// slime pain sounds
	if (self->watertype == PR_CONTENT_SLIME)
	{
		// FIX ME	put in some steam here
		if (random() > 0.5)
			sound (self, PR_CHAN_VOICE, "player/lburn1.wav", 1, PR_ATTN_NORM);
		else
			sound (self, PR_CHAN_VOICE, "player/lburn2.wav", 1, PR_ATTN_NORM);
		return;
	}

	if (self->watertype == PR_CONTENT_LAVA)
	{
		if (random() > 0.5)
			sound (self, PR_CHAN_VOICE, "player/lburn1.wav", 1, PR_ATTN_NORM);
		else
			sound (self, PR_CHAN_VOICE, "player/lburn2.wav", 1, PR_ATTN_NORM);
		return;
	}

	if (self->pain_finished > time)
	{
		self->axhitme = 0;
		return;
	}
	self->pain_finished = time + 0.5;

// don't make multiple pain sounds right after each other

// ax pain sound
	if (self->axhitme == 1)
	{
		self->axhitme = 0;
		sound (self, PR_CHAN_VOICE, "player/axhit1.wav", 1, PR_ATTN_NORM);
		return;
	}

	rs = rint((random() * 5) + 1);

	self->noise = "";
	if (rs == 1)
		self->noise = "player/pain1.wav";
	else if (rs == 2)
		self->noise = "player/pain2.wav";
	else if (rs == 3)
		self->noise = "player/pain3.wav";
	else if (rs == 4)
		self->noise = "player/pain4.wav";
	else if (rs == 5)
		self->noise = "player/pain5.wav";
	else
		self->noise = "player/pain6.wav";

	sound (self, PR_CHAN_VOICE, self->noise, 1, PR_ATTN_NORM);
	return;
}

void player_pain1()	 {FRAME_STATE(PlayerFrames::FR_PAIN1, player_pain2); PainSound();self->weaponframe=0;}
void player_pain2()	 {FRAME_STATE(PlayerFrames::FR_PAIN2, player_pain3);}
void player_pain3()	 {FRAME_STATE(PlayerFrames::FR_PAIN3, player_pain4);}
void player_pain4()	 {FRAME_STATE(PlayerFrames::FR_PAIN4, player_pain5);}
void player_pain5()	 {FRAME_STATE(PlayerFrames::FR_PAIN5, player_pain6);}
void player_pain6()	 {FRAME_STATE(PlayerFrames::FR_PAIN6, player_run);}

void player_axpain1()	 {FRAME_STATE(PlayerFrames::FR_AXPAIN1, player_axpain2); PainSound();self->weaponframe=0;}
void player_axpain2()	 {FRAME_STATE(PlayerFrames::FR_AXPAIN2, player_axpain3);}
void player_axpain3()	 {FRAME_STATE(PlayerFrames::FR_AXPAIN3, player_axpain4);}
void player_axpain4()	 {FRAME_STATE(PlayerFrames::FR_AXPAIN4, player_axpain5);}
void player_axpain5()	 {FRAME_STATE(PlayerFrames::FR_AXPAIN5, player_axpain6);}
void player_axpain6()	 {FRAME_STATE(PlayerFrames::FR_AXPAIN6, player_run);}

void player_pain(entity attacker, float damage)
{
	if (self->weaponframe)
		return;

	// eyes don't have pain frames
	if (self->invisible_finished > time)
		return;

	// feigning players don't have pain frames
	if (self->is_feigning)
	{
		PainSound();
		return;
	}

	// HvyWeap Guys firing assault cannon don't have pain frames
	if (self->PR_BUTTON_FIRE && self->current_weapon == PR_WEAP_ASSAULT_CANNON)
		return;

	if (self->current_weapon <= PR_WEAP_AXE)
		player_axpain1 ();
	else
		player_pain1 ();
}

void player_diea1();
void player_dieb1();
void player_diec1();
void player_died1();
void player_diee1();
void player_die_ax1();

// PZ: On water maps, like the water arena on arenasix, bubbles get out of hand to where there are so many bubbles that the
//     game can't make other really important entities (only an issue with visible entities). So, let's limit the bubbles.
//     I modified DeathBubblesSpawn() and NormalBubblesSpawn().
void DeathBubblesSpawn()
{
	if (self->owner->waterlevel != 3 && self->has_holo == 0)
	{
		dremove(self);
		return;
	}

	// PZ: If there are currently a lot of visible entities, don't allow spawning a bubble.
	float numberOfVisibleEnts;
	numberOfVisibleEnts = getClientEntityCount();
	if (numberOfVisibleEnts < (512 - 62))
	{
		newmis = spawn();
		setmodel(newmis, "progs/s_bubble.spr");
		setorigin(newmis, self->owner->origin + V({0, 0, 24}));
		newmis->movetype = PR_MOVETYPE_NOCLIP;
		newmis->solid = PR_SOLID_NOT;
		newmis->velocity = V({0, 0, 15});
		newmis->nextthink = time + 0.5;
		newmis->think = bubble_bob;
		newmis->classname = "bubble";
		newmis->frame = 0;
		newmis->cnt = 0;
		setsize(newmis, V({-8, -8, -8}), V({8, 8, 8}));
	}
	self->nextthink = time + 0.1;
	self->think = DeathBubblesSpawn;
	self->air_finished = self->air_finished + 1;
	if (self->air_finished >= self->bubble_count)
		dremove(self);
}

void DeathBubbles(float num_bubbles)
{
	entity	bubble_spawner;

	bubble_spawner = spawnServerSide(); // PZ: make it a server-side only entity
	setorigin(bubble_spawner, self->origin);

	bubble_spawner->movetype = PR_MOVETYPE_NONE;
	bubble_spawner->solid = PR_SOLID_NOT;
	bubble_spawner->nextthink = time + 0.1;
	bubble_spawner->think = DeathBubblesSpawn;
	bubble_spawner->air_finished = 0;
	bubble_spawner->owner = self;
	bubble_spawner->bubble_count = num_bubbles;

	// Gizmo - allow non demons and non army soldiers to be affected (coop monsters)
    if (IsMonster(self))
        bubble_spawner->has_holo = 1; // forced mode
    else
        bubble_spawner->has_holo = 0; // normal mode

	return;
}

void NormalBubblesSpawn()
{
	// PZ: If there are currently a lot of visible entities, don't allow spawning a bubble.
	float numberOfVisibleEnts;
	numberOfVisibleEnts = getClientEntityCount();
	if (numberOfVisibleEnts < (512 - 62))
	{
		newmis = spawn();
		setmodel(newmis, "progs/s_bubble.spr");
		setorigin(newmis, self->origin + V({0, 0, 24}));
		newmis->movetype = PR_MOVETYPE_NOCLIP;
		newmis->solid = PR_SOLID_NOT;
		newmis->velocity = V({0, 0, 15});
		newmis->nextthink = time + 0.5;
		newmis->think = bubble_bob;
		newmis->classname = "bubble";
		newmis->frame = 0;
		newmis->cnt = 0;
		setsize(newmis, V({-8, -8, -8}), V({8, 8, 8}));
	}
	self->nextthink = time + 0.1;
	self->think = NormalBubblesSpawn;
	self->air_finished = self->air_finished + 1;
	if (self->air_finished >= self->bubble_count)
		dremove(self);
}

void SpawnBubbles(float num_bubbles, const vector& where)
{
	entity	bubble_spawner;

	bubble_spawner = spawnServerSide(); // PZ: make it a server-side only entity
	setorigin(bubble_spawner, where);

	bubble_spawner->movetype = PR_MOVETYPE_NONE;
	bubble_spawner->solid = PR_SOLID_NOT;
	bubble_spawner->nextthink = time + 0.1;
	bubble_spawner->think = NormalBubblesSpawn;
	bubble_spawner->air_finished = 0;
	bubble_spawner->owner = world;
	bubble_spawner->bubble_count = num_bubbles;

	return;
}


void DeathSound()
{
	float		rs;

	// water death sounds
	if (self->waterlevel == 3)
	{
		if (self->is_feigning)
			DeathBubbles(2);
		else
			DeathBubbles(10);
		sound (self, PR_CHAN_VOICE, "player/h2odeath.wav", 1, PR_ATTN_NONE);
		return;
	}

	rs = rint ((random() * 4) + 1);
	if (rs == 1)
		self->noise = "player/death1.wav";
	if (rs == 2)
		self->noise = "player/death2.wav";
	if (rs == 3)
		self->noise = "player/death3.wav";
	if (rs == 4)
		self->noise = "player/death4.wav";
	if (rs == 5)
		self->noise = "player/death5.wav";

	sound (self, PR_CHAN_VOICE, self->noise, 1, PR_ATTN_NONE);
	return;
}


void PlayerCorpse()
{
	self->nextthink = -1;
	// allow respawn after a certain time
	self->deadflag = PR_DEAD_DEAD;

    #ifdef PR_GIBABLE_CORPSES
    MakeCorpse(self,PR_CORPSE_HP_PLAYER);
    #endif
}

// PZ: If you use this function for anything other than gibs, please take into account how it will work in Neo mode.
//     I don't want rocket jumps and such being changed; just gib velocities. Thanks.
vector VelocityForDamage(float dm)
{
	vector v;

	v[X] = 100 * crandom();
	v[Y] = 100 * crandom();
	v[Z] = 150 + 100 * random();//v_z = 200 + 100 * random(); // <-- ORiginal code

	// TODO: Make the calculation acceptable and more realistic/accurate
	if (!neo.isModeActive()) // for Neo mode
	{
		if (dm > -50)
		{
			//RPrint("level 1\n");
			v = v * 0.7;
		}
		else if (dm > -200)
		{
			//RPrint("level 3\n");
			v = v * 2;
		}
		else
			v = v * 10;
	}
	else
		v = v * (dm * -1.0 / 30.0); // end of Neo mod

	return v;
}

/*

Entity fields used on gibs:
---------------------------

.has_fieldgen - Kgs value for the gib
.message      - When "cool" is assigned to it, it will be pushed by explosions and trace attacks (if enabled)

*/

void GibPickSound(entity warlock);

float GetMeat(entity player);
void SetMeat(entity player, float numgibs);

void gib_touch()
{
    if (self->velocity == V({0, 0, 0}))
        self->avelocity = V({0, 0, 0});

    if (other->classname == "player")
    #ifndef PR_WARLOCK_TEST
    if (!Teammate(other, self)) // Only enemy gibs can be picked up // PZ FIXME? Why didn't I put a isFamilyMember() check here (for deathmatch)?
    #endif
    if (other->job & PR_JOB_WARLOCK)
    if (other->playerclass != PR_PC_UNDEFINED) // Skip Observers
    if (!(other->done_custom & PR_CUSTOM_BUILDING)) // ppl customizing
    //if (self.has_fieldgen) NOT NEEDED, only gibs with meat have their touch assigned
    if (other->health > 0)
    {
        string kgs;
        kgs = ftos(self->has_fieldgen);

        float meat;
        meat = GetMeat(other);
        SetMeat(other,meat+self->has_fieldgen);

        sprint(other,PR_PRINT_HIGH,"You get ");
        sprint(other,PR_PRINT_HIGH,kgs);

        if (self->has_fieldgen > 1)
            sprint(other,PR_PRINT_HIGH," kgs of fresh meat\n");
        else
            sprint(other,PR_PRINT_HIGH," kg of fresh meat\n");

        GibPickSound(other);
        stuffcmd(other,"bf\n");
        dremove(self);
    }
}

void ThrowGib(const string& gibname, float dm, float makecool, float gibskin, float gibkgs, float randorg)
{
	// PZ: If there are currently a lot of visible entities, don't allow spawning a gib.
	float numberOfVisibleEnts;
	numberOfVisibleEnts = getClientEntityCount();
	if (numberOfVisibleEnts >= (512 - 52))
		return;

	newmis = spawn();
	if (!randorg)
		newmis->origin = self->origin;
	else
	{
		newmis->origin[X] = self->origin[X] + (random()* (self->maxs[X] + self->mins[X])) * randorg;
		newmis->origin[Y] = self->origin[Y] + (random()* (self->maxs[Y] + self->mins[Y])) * randorg;
		newmis->origin[Z] = self->origin[Z] + (random()* (self->maxs[Z] + self->mins[Z])) * randorg;
	}
	setmodel (newmis, gibname);
	setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));
	newmis->velocity = VelocityForDamage (dm);
	newmis->movetype = PR_MOVETYPE_BOUNCE;
	newmis->solid = PR_SOLID_NOT;
	newmis->avelocity[X] = crandom()*600;
	newmis->avelocity[Y] = crandom()*600;
	newmis->avelocity[Z] = crandom()*600;
	newmis->think = SUB_Remove;
	newmis->ltime = time;
#ifdef PR_COOP_MODE_ENHANCED
	float r;

	if ( self->endtime > 0 && COOP_IsCoopMonster( self ) ) {
		r = random();
		if ( r < 0.5 )
			r = 0.5 + random()*0.5;

		newmis->nextthink = time + self->endtime*r;
	} else
#endif
	newmis->nextthink = time + PR_MIN_GIB_TIME + random()*(PR_MAX_GIB_TIME - PR_MIN_GIB_TIME);
	newmis->frame = 0;
	newmis->flags = 0;

	#ifdef PR_COOL_GIBS
	if (makecool)
	{
		newmis->PR_coolflag = PR_STRFLAG_COOL;
		newmis->classname = "gib";
	}
	#endif

	if (gibskin)
		newmis->skin = gibskin;

	if (gibkgs)
	{
		newmis->touch = gib_touch;
		newmis->solid = PR_SOLID_TRIGGER;
		newmis->has_fieldgen = gibkgs;
		newmis->team_no = self->team_no;
	}
	#ifdef PR_COOL_GIBS
	#ifdef PR_COOL_METAL_GIBS
	else
		newmis->solid = PR_SOLID_TRIGGER;
	#endif
	#endif
}

/*

Entity fields used for heads:
-----------------------------

.oldenemy   - Last person that kicked this head
.t_length   - Last time this head was kicked
.t_width    - First stop done?
.message    - When "cool" is assigned to it, it will be pushed by explosions and trace attacks (if enabled)

*/

#ifdef PR_KICKABLE_HEADS
float kickable_heads;
#endif

float GetHeads(entity player);
void SetHeads(entity player, float numheads);

void head_touch()
{
    if (self->velocity == V({0, 0, 0})) // stoped?
    {
        self->avelocity = V({0, 0, 0});

        #ifdef PR_KICKABLE_HEADS
        self->t_width = PR_TRUE;
        #endif
    }

    if (other->classname == "player")
    {
        if (other->health <= 0)
            return;

        if (other->playerclass == PR_PC_UNDEFINED) // Skip Observers
            return;

        if (other->done_custom & PR_CUSTOM_BUILDING) // ppl customizing
            return;

        if (other->job & PR_JOB_WARLOCK) // Warlock collects this head
        #ifndef PR_WARLOCK_TEST
        if (!Teammate(other, self)) // Only enemy heads can be picked up  // PZ FIXME?: Why didn't I check isFamilyMember() here (for deathmatch)?
        #endif
        {
            self->modelindex = modelindex_null;
            self->touch = SUB_Null;
            self->solid = PR_SOLID_NOT;

            if (self->classname == "player")
            {
                sprint(other,PR_PRINT_HIGH,"You pick up ");
                sprint(other,PR_PRINT_HIGH,self->netname);
                sprint(other,PR_PRINT_HIGH,"'s head\n");

                sprint(self,PR_PRINT_HIGH,other->netname);
                sprint(self,PR_PRINT_HIGH," picks up your head!\n");
            }
            else
                sprint(other,PR_PRINT_HIGH,"You pick up a head\n");

            GibPickSound(other);
            stuffcmd(other,"bf\n");

            float numheads;
            numheads = GetHeads(other);
            SetHeads(other,numheads+1);

            // Dont remove self because we r a player entity or a bodyque entry

            // Unless its an special head (see below)
            if (self->classname == "headout")
                dremove(self);

            return;
        }

        #ifdef PR_KICKABLE_HEADS
        if (!kickable_heads)
            return;

        if (!self->t_width)
            return;

        if (self->oldenemy == other) // we are the last person that kicked this head
            if (self->t_length + PR_KICKHEAD_RATE_PERUSER > time) // too early for him?
                return;

        if (self->t_length + PR_KICKHEAD_RATE > time) // too early?
            return; // ok, do nothing, dont bounce or anything

        if (other->velocity == V({0, 0, 0})) // Dont kick if the player is not moving
            return;

        self->t_length = time;

        // Do pain sound if a "living" head :)
        if (self->classname == "player")
        if (self->is_connected)
        {
            // TODO: Make the following work!
            // Put it in a pushable origin
            /*local vector oldorig;
            local float oldcont;
            oldorig = self.origin;
            oldcont = pointcontents(self.origin);

            if (fabs(self.origin_x - other.origin_x) > fabs(self.origin_y - other.origin_y))
            {
                if (self.origin_x < other.origin_x)
                    self.origin_x = self.origin_x -( (  (other.absmax_x - other.absmin_x) - (self.absmax_x - other.absmin_x)  )+1);
                else
                    self.origin_x = self.origin_x +( (  (other.absmax_x - other.absmin_x) - (self.absmax_x - other.absmin_x)  )+1);
            }
            else
            {
                if (self.origin_x < other.origin_x)
                    self.origin_y = self.origin_y -( (  (other.absmax_y - other.absmin_y) - (self.absmax_y - other.absmin_y)  )+1);
                else
                    self.origin_y = self.origin_y +( (  (other.absmax_y - other.absmin_y) - (self.absmax_y - other.absmin_y)  )+1);
            }

            // valid origin?
            if ((pointcontents(self.origin) != oldcont)
              ||(pointcontents(self.absmin + '1 1 1') != oldcont)
              ||(pointcontents(self.absmax - '1 1 1') != oldcont))
            {
                self.origin = oldorig;
                setorigin(self,self.origin);
                return;
            }*/

            // water pain sounds
        	if (self->watertype == PR_CONTENT_WATER && self->waterlevel == 3)
        	{
        		DeathBubbles(1);
        		if (random() > 0.5)
        			sound (self, PR_CHAN_VOICE, "player/drown1.wav", 1, PR_ATTN_IDLE);
        		else
        			sound (self, PR_CHAN_VOICE, "player/drown2.wav", 1, PR_ATTN_IDLE);
            }
            else
            {
                float		rs;
                rs = rint((random() * 5) + 1);

                self->noise = "";
                if (rs == 1)
                    self->noise = "player/pain1.wav";
                else if (rs == 2)
                    self->noise = "player/pain2.wav";
                else if (rs == 3)
                    self->noise = "player/pain3.wav";
                else if (rs == 4)
                    self->noise = "player/pain4.wav";
                else if (rs == 5)
                    self->noise = "player/pain5.wav";
                else
                    self->noise = "player/pain6.wav";

                sound (self, PR_CHAN_VOICE, self->noise, 1, PR_ATTN_IDLE);
            }

            sprint(self,PR_PRINT_MEDIUM,other->netname);
            sprint(self,PR_PRINT_MEDIUM," kicks your head!\n");

            sprint(other,PR_PRINT_MEDIUM,self->netname);
            sprint(other,PR_PRINT_MEDIUM,"'s head is still alive!\n");

            self->t_length = time + PR_KICKHEAD_RATE_ALIVE;
        }

        //ok, kick it out
        self->oldenemy = other;
        makevectors (other->v_angle);
        self->velocity = other->velocity * 1.5 + v_forward * 128 + V({0, 0, 260}) + V({0, 0, 300})*random();
        self->flags = self->flags - (self->flags & PR_FL_ONGROUND);

        self->avelocity = crandom() * V({0, 600, 0});

        sound(self,PR_CHAN_BODY,"zombie/z_miss.wav",0.5, PR_ATTN_IDLE);
        #endif
    }
}

// PZ NOTE: This creates a 'living head'. It just converts a player entity into a head until the player respawns or disconnects.
void ThrowHead(const string& gibname, float dm)
{
	setmodel (self, gibname);
	self->skin = 0;
	self->frame = 0;
	self->nextthink = -1;
	self->movetype = PR_MOVETYPE_BOUNCE;
	self->takedamage = PR_DAMAGE_NO;
	//self.solid = #SOLID_NOT;
	self->solid = PR_SOLID_TRIGGER; // OfN
	self->view_ofs = V({0, 0, 8});
	//setsize (self, '-16 -16 0', '16 16 56');
	setsize (self, V({-16, -16, 0}), V({16, 16, 8}));
	self->velocity = VelocityForDamage (dm);
	//self.origin_z = self.origin_z - 24;
	self->flags = self->flags - (self->flags & PR_FL_ONGROUND);
	self->avelocity = crandom() * V({0, 600, 0});

	//-OfN
	self->touch = head_touch;

	#ifdef PR_COOL_GIBS
	self->PR_coolflag = PR_STRFLAG_COOL;
	#endif

	#ifdef PR_KICKABLE_HEADS
	self->t_width = PR_FALSE;
	self->oldenemy = world;
	self->t_length = time;
	#endif
}

//==========================================================
// Special client head throw =) (when disconnect)

void ThrowHeadOut(const string& gibname, float dm)
{
	// PZ: If there are currently a lot of visible entities, don't allow spawning a head.
	float numberOfVisibleEnts;
	numberOfVisibleEnts = getClientEntityCount();
	if (numberOfVisibleEnts >= (512 - 52))
		return;

	entity thehead;
	thehead = spawn();

	setmodel (thehead, gibname);
	setorigin(thehead,self->origin);
	thehead->skin = 0;
	thehead->frame = 0;
	thehead->movetype = PR_MOVETYPE_BOUNCE;
	thehead->takedamage = PR_DAMAGE_NO;
	thehead->solid = PR_SOLID_TRIGGER; // OfN
	thehead->view_ofs = V({0, 0, 8});
	//setsize (thehead, '-16 -16 0', '16 16 56');
	setsize (thehead, V({-16, -16, 0}), V({16, 16, 8}));
	thehead->velocity = VelocityForDamage (dm);
	//thehead.origin_z = thehead.origin_z - 24;
	thehead->flags = thehead->flags - (thehead->flags & PR_FL_ONGROUND);
	thehead->avelocity = crandom() * V({0, 600, 0});

	//-OfN
	thehead->classname = "headout";
	thehead->touch = head_touch;
	thehead->think = SUB_Remove;
	thehead->nextthink = time + PR_MIN_HEAD_TIME + (PR_MAX_HEAD_TIME - PR_MIN_HEAD_TIME) * random();
	thehead->team_no = self->team_no;

	#ifdef PR_COOL_GIBS
	thehead->PR_coolflag = PR_STRFLAG_COOL;
	#endif

	#ifdef PR_KICKABLE_HEADS
	thehead->t_width = PR_FALSE;
	thehead->oldenemy = world;
	thehead->t_length = time;
	#endif
}

//=====================================================
// Called on client disconnect if player is "in-game"

void GibPlayerOut()
{
	if (random() < 0.5)
		sound (self, PR_CHAN_VOICE, "player/gib.wav", 1, PR_ATTN_NONE);
	else
		sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_NONE);

	ThrowHeadOut("progs/h_player.mdl",-40);
	ThrowGib("progs/gib1.mdl",-40, PR_TRUE, 0,PR_GIB1_KGS, PR_FALSE);
	ThrowGib("progs/gib2.mdl",-40, PR_TRUE, 0,PR_GIB2_KGS, PR_FALSE);
	ThrowGib("progs/gib3.mdl",-40, PR_TRUE, 0,PR_GIB3_KGS, PR_FALSE);
	SpawnBloodEx(self->origin,0.75,9);
}

/*void(string gibname) HeadShotThrowHead =
{
	setmodel (self, gibname);
	self.frame = 0;
	self.nextthink = -1;
	self.movetype = #MOVETYPE_BOUNCE;
	self.takedamage = #DAMAGE_NO;
	self.solid = #SOLID_NOT;
	self.view_ofs = '0 0 8';
	setsize (self, '-16 -16 0', '16 16 56');
	self.velocity = normalize(self.head_shot_vector) * 600;
	self.origin_z = self.origin_z + 24;
	self.flags = self.flags - (self.flags & #FL_ONGROUND);
	self.avelocity = '0 0 0';
};*/

void KillPlayer()
{
	self->owner->deadflag = PR_DEAD_DEAD;
	dremove(self);
}

void GibPlayer()
{
  //- OfN now this can be turned on/off with the headless localinfo -//
  // Decapitation ------------------------------------------------------//
  /*if (headless == 1)
  {
	if (deathmsg == #DMSG_SNIPERHEADSHOT) //- Ofn - I don't like this:  || deathmsg == #DMSG_BACKSTAB)
	{
		//if (deathmsg == #DMSG_SNIPERHEADSHOT)
			HeadShotThrowHead("progs/h_player.mdl");
		//else
		//	ThrowHead("progs/h_player.mdl", -60);

		newmis = spawn();
		newmis.solid = #SOLID_NOT;
        newmis.movetype = #MOVETYPE_STEP;

        newmis.angles = self.angles;

        setsize(newmis, '0 0 0', '0 0 0');
		setorigin(newmis, self.origin);

        newmis.colormap = self.colormap;
//        newmis.color=self.color
		//newmis.skin = self.skin;

        setmodel(newmis, "progs/headless.mdl");

		newmis.think = Headless_Think;
		newmis.nextthink = time + 0.1;
		if (self.current_weapon <= #WEAP_AXE)
			newmis.frame = 0;
		else
			newmis.frame = 8;

        ThrowGib ("progs/zom_gib.mdl", self.health); //- lol, looks cool
	}
	else
	{
    	ThrowHead ("progs/h_player.mdl", self.health);
		ThrowGib ("progs/gib1.mdl", self.health);
		ThrowGib ("progs/gib2.mdl", self.health);
		ThrowGib ("progs/gib3.mdl", self.health);
        ThrowGib ("progs/gib3.mdl", self.health); //-added
        //ThrowGib ("progs/zom_gib.mdl", self.health); //-added

	}
  }
  else
  {*/
   	ThrowHead ("progs/h_player.mdl", self->health);
	ThrowGib ("progs/gib1.mdl", self->health, PR_TRUE, 0,PR_GIB1_KGS, PR_FALSE);
	ThrowGib ("progs/gib2.mdl", self->health, PR_TRUE, 0,PR_GIB2_KGS, PR_FALSE);
	ThrowGib ("progs/gib3.mdl", self->health, PR_TRUE, 0,PR_GIB3_KGS, PR_FALSE);
    //ThrowGib ("progs/gib3.mdl", self.health); //-added
  //}

	// Why do we need this? What are we doing that's clobbering _something_
	// within a second after a trigger_hurt kills the player?
	if (deathmsg == PR_DMSG_TRIGGER)
	{
		newmis = spawnServerSide(); // PZ: make it a server-side only entity
		newmis->owner = self;
		newmis->think = KillPlayer;
		newmis->nextthink = time + 1;
	}
	else
	{
		self->deadflag = PR_DEAD_DEAD;
	}

	TeamFortress_SetupRespawn(PR_FALSE);

	if (damage_attacker->classname == "teledeath")
	{
		sound (self, PR_CHAN_VOICE, "player/teledth1.wav", 1, PR_ATTN_NONE);

		// if telefragged, extend respawn delay to prevent chaining telefrags
		// Gizmo - added a random respawn amount to minimize telefrags that keep happening
		self->respawn_time = self->respawn_time + (1 + (random() * 2));

		return;
	}

	if (damage_attacker->classname == "teledeath2")
	{
		sound (self, PR_CHAN_VOICE, "player/teledth1.wav", 1, PR_ATTN_NONE);

		// if telefragged, extend respawn delay to prevent chaining telefrags
		// Gizmo - added a random respawn amount to minimize telefrags that keep happening
		self->respawn_time = self->respawn_time + (1 + (random() * 2));

		return;
	}

	if (damage_attacker->classname == "teledeath3") {
		sound (self, PR_CHAN_VOICE, "player/teledth1.wav", 1, PR_ATTN_NONE);

		// Gizmo - added a random respawn amount to minimize telefrags that keep happening
		self->respawn_time = self->respawn_time + (1 + (random() * 2));

		return;
	}

	if (random() < 0.5)
		sound (self, PR_CHAN_VOICE, "player/gib.wav", 1, PR_ATTN_NONE);
	else
		sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_NONE);
}

void PlayerDead()
{
    self->weaponmodel="";
	self->view_ofs = V({0, 0, -8});
	self->deadflag = PR_DEAD_DYING;
	self->solid = PR_SOLID_NOT;
	self->flags = self->flags - (self->flags & PR_FL_ONGROUND);
	self->movetype = PR_MOVETYPE_TOSS;
    SpawnBloodEx(self->origin,0.75,9);

    // Reset Auras and runes
    self->aura = 0;
    //self.#runes = 0; // Not needed
}

void PlayerDie()
{
	float i,finished;
	entity te;
	//WK Stop orff
	//sound (self, #CHAN_MUSIC, "items/r_item1.wav", 0.1, #ATTN_NORM);
	sound (self, PR_CHAN_MUSIC, "misc/null.wav", 0.1, PR_ATTN_NORM);
	//WK Stop glows!
      self->effects = 0;
    // make dead guy release hook (wedge)
    if (self->hook_out)
    {
	    Reset_Grapple (self->hook);
    	Attack_Finished(0.75);
	    self->hook_out = PR_TRUE;	// PutClientInServer will reset this
    }

    self->items = self->items - (self->items &
    (PR_IT_INVISIBILITY | PR_IT_INVULNERABILITY | PR_IT_SUIT | PR_IT_QUAD) );
    self->invisible_finished = 0;    // don't die as eyes
    self->invincible_finished = 0;
    self->super_damage_finished = 0;
    self->radsuit_finished = 0;
    self->modelindex = modelindex_player;    // don't use eyes

    // If infected, give the medic a frag
	finished = PR_TRUE;
	if (self->tfstate & PR_TFSTATE_INFECTED && self == self->enemy)
	{
		te = find(world, "classname", "timer");
		while (te != world)
		{
			if (te->owner == self && te->think == BioInfection_Decay)
			{
			#ifdef PR_QUAKE_WORLD
				logfrag(te->enemy, self);
			#endif
				te->enemy->real_frags = te->enemy->real_frags + 1;
			 	if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
					te->enemy->frags = te->enemy->real_frags;
				finished = PR_TRUE;
			}

			te = find(te, "classname", "timer");
			if (finished) te = world;
		}
	}

	// Remove all the timer entities for this player
	TeamFortress_RemoveTimers();

    // Drop any runes he would carry
    PlayerDropRunes(self);

	// Restore player's speed
#ifdef PR_RESTORE_DEAD_SPEEDS

    stuffcmd(self,"cl_backspeed 1000\n");
	stuffcmd(self,"cl_forwardspeed 1000\n");
	stuffcmd(self,"cl_sidespeed 1000\n");
	stuffcmd(self,"cl_movespeedkey 2\n");

#endif

	if (deathmatch || coop)
		DropBackpack();

	PlayerDead();

	if (self->velocity[Z] < 10)
		self->velocity[Z] = self->velocity[Z] + random()*300;
	if (self->health < -40)
	{
		GibPlayer ();
		return;
	}

	DeathSound();

	self->angles[X] = 0;
	self->angles[Z] = 0;

	if (self->current_weapon <= PR_WEAP_AXE)
	{
		player_die_ax1 ();
		TeamFortress_SetupRespawn(PR_FALSE);
		return;
	}

	i = 1 + floor(random()*6);

	if (i == 1)
		player_diea1();
	else if (i == 2)
		player_dieb1();
	else if (i == 3)
		player_diec1();
	else if (i == 4)
		player_died1();
	else
		player_diee1();

	// get ready to spawn!
	TeamFortress_SetupRespawn(PR_FALSE);
}

void set_suicide_frame()
{	// used by kill command and diconnect command (OfN suicide)
	if (self->model != "progs/player.mdl")
		return;	// allready gibbed

// Was commented
	//self.frame = $deatha11;
	self->solid = PR_SOLID_NOT;
	self->movetype = PR_MOVETYPE_TOSS;
	self->deadflag = PR_DEAD_DEAD;
	self->nextthink = -1;
// was comented

	/* OfN <- commented by
    setmodel(self, string_null);
	setsize (self, #VEC_HULL_MIN, #VEC_HULL_MAX);*/
}

void set_suicide_frame_disc()
{	// used by kill command and diconnect command (OfN disconnect)
	if (self->model != "progs/player.mdl")
		return;	// allready gibbed

/*
    self.frame = $deatha11;
	self.solid = #SOLID_NOT;
	self.movetype = #MOVETYPE_TOSS;
	self.deadflag = #DEAD_DEAD;
	self.nextthink = -1;
*/

	setmodel(self, string_null);
	setsize (self, PR_VEC_HULL_MIN, PR_VEC_HULL_MAX);
}

void player_diea1()	 {FRAME_STATE(PlayerFrames::FR_DEATHA1, player_diea2);}
void player_diea2()	 {FRAME_STATE(PlayerFrames::FR_DEATHA2, player_diea3);}
void player_diea3()	 {FRAME_STATE(PlayerFrames::FR_DEATHA3, player_diea4);}
void player_diea4()	 {FRAME_STATE(PlayerFrames::FR_DEATHA4, player_diea5);}
void player_diea5()	 {FRAME_STATE(PlayerFrames::FR_DEATHA5, player_diea6);}
void player_diea6()	 {FRAME_STATE(PlayerFrames::FR_DEATHA6, player_diea7);}
void player_diea7()	 {FRAME_STATE(PlayerFrames::FR_DEATHA7, player_diea8);}
void player_diea8()	 {FRAME_STATE(PlayerFrames::FR_DEATHA8, player_diea9);}
void player_diea9()	 {FRAME_STATE(PlayerFrames::FR_DEATHA9, player_diea10);}
void player_diea10()	 {FRAME_STATE(PlayerFrames::FR_DEATHA10, player_diea11);}
void player_diea11()	 {FRAME_STATE(PlayerFrames::FR_DEATHA11, player_diea11); PlayerCorpse();}

void player_dieb1()	 {FRAME_STATE(PlayerFrames::FR_DEATHB1, player_dieb2);}
void player_dieb2()	 {FRAME_STATE(PlayerFrames::FR_DEATHB2, player_dieb3);}
void player_dieb3()	 {FRAME_STATE(PlayerFrames::FR_DEATHB3, player_dieb4);}
void player_dieb4()	 {FRAME_STATE(PlayerFrames::FR_DEATHB4, player_dieb5);}
void player_dieb5()	 {FRAME_STATE(PlayerFrames::FR_DEATHB5, player_dieb6);}
void player_dieb6()	 {FRAME_STATE(PlayerFrames::FR_DEATHB6, player_dieb7);}
void player_dieb7()	 {FRAME_STATE(PlayerFrames::FR_DEATHB7, player_dieb8);}
void player_dieb8()	 {FRAME_STATE(PlayerFrames::FR_DEATHB8, player_dieb9);}
void player_dieb9()	 {FRAME_STATE(PlayerFrames::FR_DEATHB9, player_dieb9); PlayerCorpse();}

void player_diec1()	 {FRAME_STATE(PlayerFrames::FR_DEATHC1, player_diec2);}
void player_diec2()	 {FRAME_STATE(PlayerFrames::FR_DEATHC2, player_diec3);}
void player_diec3()	 {FRAME_STATE(PlayerFrames::FR_DEATHC3, player_diec4);}
void player_diec4()	 {FRAME_STATE(PlayerFrames::FR_DEATHC4, player_diec5);}
void player_diec5()	 {FRAME_STATE(PlayerFrames::FR_DEATHC5, player_diec6);}
void player_diec6()	 {FRAME_STATE(PlayerFrames::FR_DEATHC6, player_diec7);}
void player_diec7()	 {FRAME_STATE(PlayerFrames::FR_DEATHC7, player_diec8);}
void player_diec8()	 {FRAME_STATE(PlayerFrames::FR_DEATHC8, player_diec9);}
void player_diec9()	 {FRAME_STATE(PlayerFrames::FR_DEATHC9, player_diec10);}
void player_diec10()	 {FRAME_STATE(PlayerFrames::FR_DEATHC10, player_diec11);}
void player_diec11()	 {FRAME_STATE(PlayerFrames::FR_DEATHC11, player_diec12);}
void player_diec12()	 {FRAME_STATE(PlayerFrames::FR_DEATHC12, player_diec13);}
void player_diec13()	 {FRAME_STATE(PlayerFrames::FR_DEATHC13, player_diec14);}
void player_diec14()	 {FRAME_STATE(PlayerFrames::FR_DEATHC14, player_diec15);}
void player_diec15()	 {FRAME_STATE(PlayerFrames::FR_DEATHC15, player_diec15); PlayerCorpse();}

void player_died1()	 {FRAME_STATE(PlayerFrames::FR_DEATHD1, player_died2);}
void player_died2()	 {FRAME_STATE(PlayerFrames::FR_DEATHD2, player_died3);}
void player_died3()	 {FRAME_STATE(PlayerFrames::FR_DEATHD3, player_died4);}
void player_died4()	 {FRAME_STATE(PlayerFrames::FR_DEATHD4, player_died5);}
void player_died5()	 {FRAME_STATE(PlayerFrames::FR_DEATHD5, player_died6);}
void player_died6()	 {FRAME_STATE(PlayerFrames::FR_DEATHD6, player_died7);}
void player_died7()	 {FRAME_STATE(PlayerFrames::FR_DEATHD7, player_died8);}
void player_died8()	 {FRAME_STATE(PlayerFrames::FR_DEATHD8, player_died9);}
void player_died9()	 {FRAME_STATE(PlayerFrames::FR_DEATHD9, player_died9); PlayerCorpse();}

void player_diee1()	 {FRAME_STATE(PlayerFrames::FR_DEATHE1, player_diee2);}
void player_diee2()	 {FRAME_STATE(PlayerFrames::FR_DEATHE2, player_diee3);}
void player_diee3()	 {FRAME_STATE(PlayerFrames::FR_DEATHE3, player_diee4);}
void player_diee4()	 {FRAME_STATE(PlayerFrames::FR_DEATHE4, player_diee5);}
void player_diee5()	 {FRAME_STATE(PlayerFrames::FR_DEATHE5, player_diee6);}
void player_diee6()	 {FRAME_STATE(PlayerFrames::FR_DEATHE6, player_diee7);}
void player_diee7()	 {FRAME_STATE(PlayerFrames::FR_DEATHE7, player_diee8);}
void player_diee8()	 {FRAME_STATE(PlayerFrames::FR_DEATHE8, player_diee9);}
void player_diee9()	 {FRAME_STATE(PlayerFrames::FR_DEATHE9, player_diee9); PlayerCorpse();}

void player_die_ax1()	 {FRAME_STATE(PlayerFrames::FR_AXDETH1, player_die_ax2);}
void player_die_ax2()	 {FRAME_STATE(PlayerFrames::FR_AXDETH2, player_die_ax3);}
void player_die_ax3()	 {FRAME_STATE(PlayerFrames::FR_AXDETH3, player_die_ax4);}
void player_die_ax4()	 {FRAME_STATE(PlayerFrames::FR_AXDETH4, player_die_ax5);}
void player_die_ax5()	 {FRAME_STATE(PlayerFrames::FR_AXDETH5, player_die_ax6);}
void player_die_ax6()	 {FRAME_STATE(PlayerFrames::FR_AXDETH6, player_die_ax7);}
void player_die_ax7()	 {FRAME_STATE(PlayerFrames::FR_AXDETH7, player_die_ax8);}
void player_die_ax8()	 {FRAME_STATE(PlayerFrames::FR_AXDETH8, player_die_ax9);}
void player_die_ax9()	 {FRAME_STATE(PlayerFrames::FR_AXDETH9, player_die_ax9); PlayerCorpse();}

/*void() Headless_Think =
{
	self.frame = self.frame + 1;

	if (self.frame == 7 || self.frame == 18)
	{
		self.nextthink = time + 30 + random()*10;
		self.think = SUB_Remove;
		return;
	}

	self.nextthink = time + 0.1;
};*/

//===========================================================
// Airgun player stuff

void player_airgun1() {FRAME_STATE(PlayerFrames::FR_ROCKATT1, player_airgun2); self->weaponframe=1;}
void player_airgun2() {FRAME_STATE(PlayerFrames::FR_ROCKATT2, player_airgun3); self->weaponframe=2;}
void player_airgun3() {FRAME_STATE(PlayerFrames::FR_ROCKATT3, player_airgun4); self->weaponframe=3;}
void player_airgun4() {FRAME_STATE(PlayerFrames::FR_ROCKATT3, player_airgun5); self->weaponframe=4;}
void player_airgun5() {FRAME_STATE(PlayerFrames::FR_ROCKATT4, player_airgun6); self->weaponframe=5;}
void player_airgun6() {FRAME_STATE(PlayerFrames::FR_ROCKATT4, player_airgun7); self->weaponframe=6;}
void player_airgun7() {FRAME_STATE(PlayerFrames::FR_ROCKATT5, player_airgun8); self->weaponframe=7;}
void player_airgun8() {FRAME_STATE(PlayerFrames::FR_ROCKATT6, player_run); self->weaponframe = 8;}
void player_failedairgun1() {FRAME_STATE(PlayerFrames::FR_NAILATT1, player_failedairgun2); self->weaponframe=6;}
void player_failedairgun2() {FRAME_STATE(PlayerFrames::FR_NAILATT2, player_failedairgun3); self->weaponframe=7;}
void player_failedairgun3() {FRAME_STATE(PlayerFrames::FR_NAILATT2, player_run); self->weaponframe=8;}

//================================================================0
// Laser cannon player stuff

void player_laser1()
{
    FRAME_STATE(PlayerFrames::FR_NAILATT1, player_laser2);
    if (!self->PR_BUTTON_FIRE || intermission_running || self->tfstate & PR_TFSTATE_RELOADING)
		{player_run ();return;}

    self->attack_finished = time + 0.2;
    self->weaponframe=1;
    HIP_FireLaser(0);
}
void player_laser2()
{
    FRAME_STATE(PlayerFrames::FR_NAILATT2, player_laser3);
    if (!self->PR_BUTTON_FIRE || intermission_running || self->tfstate & PR_TFSTATE_RELOADING)
		{player_run ();return;}

    self->weaponframe=2;
}
void player_laser3()
{
    FRAME_STATE(PlayerFrames::FR_NAILATT2, player_laser4);
    if (!self->PR_BUTTON_FIRE || intermission_running || self->tfstate & PR_TFSTATE_RELOADING)
		{player_run ();return;}

    self->attack_finished = time + 0.2;
    self->weaponframe=4;
    HIP_FireLaser(1);
}
void player_laser4()
{
    FRAME_STATE(PlayerFrames::FR_NAILATT1, player_laser1);
    if (!self->PR_BUTTON_FIRE || intermission_running || self->tfstate & PR_TFSTATE_RELOADING)
		{player_run ();return;}

    self->weaponframe=5;
}

} // END namespace Progs
