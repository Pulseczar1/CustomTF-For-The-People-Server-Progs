/*==============================================
	SPY.QC

TeamFortress v2.5	 29/2/97
========================================================
Functions for the SPY class and associated weaponry
========================================================*/

#include "progs.h"
#include "player.h"
#include "spy.h"
#include "coop_defs.h"
#include "ofndefs.h"
#include "gweapons.h"
#include "debug.h"
#include "weapons.h"
#include "menu.h"
#include "tfort.h"
#include "cpstuff.h"
#include "idmonsters/common.h"
#include "qw.h"
#include "custom.h"

namespace Progs {

void TeamFortress_SpyGoUndercover();
void TeamFortress_SpyFeignDeath(float type);
void TeamFortress_SpyCalcName(entity spy);
void TeamFortress_SpyUndercoverThink();
void TeamFortress_SpyChangeSkin(float playerClass);
void TeamFortress_SpyChangeColor(float teamno);
void GasGrenadeMakeGas();
void HallucinationTimer();
void W_FireTranq();
void T_TranqDartTouch();
void TranquiliserTimer();
void TranquiliserTimer2(); //CH for demons
void Spy_RemoveDisguise(entity spy);

// Spy Feign Death Handling
//$cd /raid/quake/id1/models/player_4
//$origin 0 -6 24
//$base base
//$skin skin
/* PZ: I have commented these because they are already declared in player.cpp. C++ says "redeclaration".
       I could give them their own namespace here, but I don't see why the spy needs his own frames.
	   I think they gave the spy his own frames because he needs to "die in reverse", to get up after feigning.
enum {FR_AXRUN1, FR_AXRUN2, FR_AXRUN3, FR_AXRUN4, FR_AXRUN5, FR_AXRUN6};
enum {FR_ROCKRUN1 = 6, FR_ROCKRUN2, FR_ROCKRUN3, FR_ROCKRUN4, FR_ROCKRUN5, FR_ROCKRUN6};
enum {FR_STAND1 = 12, FR_STAND2, FR_STAND3, FR_STAND4, FR_STAND5};
enum {FR_AXSTND1 = 17, FR_AXSTND2, FR_AXSTND3, FR_AXSTND4, FR_AXSTND5, FR_AXSTND6};
enum {FR_AXSTND7 = 23, FR_AXSTND8, FR_AXSTND9, FR_AXSTND10, FR_AXSTND11, FR_AXSTND12};
enum {FR_AXPAIN1 = 29, FR_AXPAIN2, FR_AXPAIN3, FR_AXPAIN4, FR_AXPAIN5, FR_AXPAIN6};
enum {FR_PAIN1 = 35, FR_PAIN2, FR_PAIN3, FR_PAIN4, FR_PAIN5, FR_PAIN6};
enum {FR_AXDETH1 = 41, FR_AXDETH2, FR_AXDETH3, FR_AXDETH4, FR_AXDETH5, FR_AXDETH6};
enum {FR_AXDETH7 = 47, FR_AXDETH8, FR_AXDETH9};
enum {FR_DEATHA1 = 50, FR_DEATHA2, FR_DEATHA3, FR_DEATHA4, FR_DEATHA5, FR_DEATHA6, FR_DEATHA7, FR_DEATHA8};
enum {FR_DEATHA9 = 58, FR_DEATHA10, FR_DEATHA11};
enum {FR_DEATHB1 = 61, FR_DEATHB2, FR_DEATHB3, FR_DEATHB4, FR_DEATHB5, FR_DEATHB6, FR_DEATHB7, FR_DEATHB8};
enum {FR_DEATHB9 = 69};
enum {FR_DEATHC1 = 70, FR_DEATHC2, FR_DEATHC3, FR_DEATHC4, FR_DEATHC5, FR_DEATHC6, FR_DEATHC7, FR_DEATHC8};
enum {FR_DEATHC9 = 78, FR_DEATHC10, FR_DEATHC11, FR_DEATHC12, FR_DEATHC13, FR_DEATHC14, FR_DEATHC15};
enum {FR_DEATHD1 = 85, FR_DEATHD2, FR_DEATHD3, FR_DEATHD4, FR_DEATHD5, FR_DEATHD6, FR_DEATHD7};
enum {FR_DEATHD8 = 92, FR_DEATHD9};
enum {FR_DEATHE1 = 94, FR_DEATHE2, FR_DEATHE3, FR_DEATHE4, FR_DEATHE5, FR_DEATHE6, FR_DEATHE7};
enum {FR_DEATHE8 = 101, FR_DEATHE9};
*/

/*
$frame nailatt1 nailatt2
$frame light1 light2
$frame rockatt1 rockatt2 rockatt3 rockatt4 rockatt5 rockatt6
$frame shotatt1 shotatt2 shotatt3 shotatt4 shotatt5 shotatt6
$frame axatt1 axatt2 axatt3 axatt4 axatt5 axatt6
$frame axattb1 axattb2 axattb3 axattb4 axattb5 axattb6
$frame axattc1 axattc2 axattc3 axattc4 axattc5 axattc6
$frame axattd1 axattd2 axattd3 axattd4 axattd5 axattd6
*/

// Spy Feign Death Frames
void spy_diea1()	 {FRAME_STATE(PlayerFrames::FR_DEATHA1, spy_diea2);}
void spy_diea2()	 {FRAME_STATE(PlayerFrames::FR_DEATHA2, spy_diea3);}
void spy_diea3()	 {FRAME_STATE(PlayerFrames::FR_DEATHA3, spy_diea4);}
void spy_diea4()	 {FRAME_STATE(PlayerFrames::FR_DEATHA4, spy_diea5);}
void spy_diea5()	 {FRAME_STATE(PlayerFrames::FR_DEATHA5, spy_diea6);}
void spy_diea6()	 {FRAME_STATE(PlayerFrames::FR_DEATHA6, spy_diea7);}
void spy_diea7()	 {FRAME_STATE(PlayerFrames::FR_DEATHA7, spy_diea8);}
void spy_diea8()	 {FRAME_STATE(PlayerFrames::FR_DEATHA8, spy_diea9);}
void spy_diea9()	 {FRAME_STATE(PlayerFrames::FR_DEATHA9, spy_diea10);}
void spy_diea10()	 {FRAME_STATE(PlayerFrames::FR_DEATHA10, spy_diea11);}
void spy_diea11()	 {FRAME_STATE(PlayerFrames::FR_DEATHA11, spy_diea11);}

void spy_dieb1()	 {FRAME_STATE(PlayerFrames::FR_DEATHB1, spy_dieb2);}
void spy_dieb2()	 {FRAME_STATE(PlayerFrames::FR_DEATHB2, spy_dieb3);}
void spy_dieb3()	 {FRAME_STATE(PlayerFrames::FR_DEATHB3, spy_dieb4);}
void spy_dieb4()	 {FRAME_STATE(PlayerFrames::FR_DEATHB4, spy_dieb5);}
void spy_dieb5()	 {FRAME_STATE(PlayerFrames::FR_DEATHB5, spy_dieb6);}
void spy_dieb6()	 {FRAME_STATE(PlayerFrames::FR_DEATHB6, spy_dieb7);}
void spy_dieb7()	 {FRAME_STATE(PlayerFrames::FR_DEATHB7, spy_dieb8);}
void spy_dieb8()	 {FRAME_STATE(PlayerFrames::FR_DEATHB8, spy_dieb9);}
void spy_dieb9()	 {FRAME_STATE(PlayerFrames::FR_DEATHB9, spy_dieb9);}

void spy_diec1()	 {FRAME_STATE(PlayerFrames::FR_DEATHC1, spy_diec2);}
void spy_diec2()	 {FRAME_STATE(PlayerFrames::FR_DEATHC2, spy_diec3);}
void spy_diec3()	 {FRAME_STATE(PlayerFrames::FR_DEATHC3, spy_diec4);}
void spy_diec4()	 {FRAME_STATE(PlayerFrames::FR_DEATHC4, spy_diec5);}
void spy_diec5()	 {FRAME_STATE(PlayerFrames::FR_DEATHC5, spy_diec6);}
void spy_diec6()	 {FRAME_STATE(PlayerFrames::FR_DEATHC6, spy_diec7);}
void spy_diec7()	 {FRAME_STATE(PlayerFrames::FR_DEATHC7, spy_diec8);}
void spy_diec8()	 {FRAME_STATE(PlayerFrames::FR_DEATHC8, spy_diec9);}
void spy_diec9()	 {FRAME_STATE(PlayerFrames::FR_DEATHC9, spy_diec10);}
void spy_diec10()	 {FRAME_STATE(PlayerFrames::FR_DEATHC10, spy_diec11);}
void spy_diec11()	 {FRAME_STATE(PlayerFrames::FR_DEATHC11, spy_diec12);}
void spy_diec12()	 {FRAME_STATE(PlayerFrames::FR_DEATHC12, spy_diec13);}
void spy_diec13()	 {FRAME_STATE(PlayerFrames::FR_DEATHC13, spy_diec14);}
void spy_diec14()	 {FRAME_STATE(PlayerFrames::FR_DEATHC14, spy_diec15);}
void spy_diec15()	 {FRAME_STATE(PlayerFrames::FR_DEATHC15, spy_diec15);}

void spy_died1()	 {FRAME_STATE(PlayerFrames::FR_DEATHD1, spy_died2);}
void spy_died2()	 {FRAME_STATE(PlayerFrames::FR_DEATHD2, spy_died3);}
void spy_died3()	 {FRAME_STATE(PlayerFrames::FR_DEATHD3, spy_died4);}
void spy_died4()	 {FRAME_STATE(PlayerFrames::FR_DEATHD4, spy_died5);}
void spy_died5()	 {FRAME_STATE(PlayerFrames::FR_DEATHD5, spy_died6);}
void spy_died6()	 {FRAME_STATE(PlayerFrames::FR_DEATHD6, spy_died7);}
void spy_died7()	 {FRAME_STATE(PlayerFrames::FR_DEATHD7, spy_died8);}
void spy_died8()	 {FRAME_STATE(PlayerFrames::FR_DEATHD8, spy_died9);}
void spy_died9()	 {FRAME_STATE(PlayerFrames::FR_DEATHD9, spy_died9);}

void spy_diee1()	 {FRAME_STATE(PlayerFrames::FR_DEATHE1, spy_diee2);}
void spy_diee2()	 {FRAME_STATE(PlayerFrames::FR_DEATHE2, spy_diee3);}
void spy_diee3()	 {FRAME_STATE(PlayerFrames::FR_DEATHE3, spy_diee4);}
void spy_diee4()	 {FRAME_STATE(PlayerFrames::FR_DEATHE4, spy_diee5);}
void spy_diee5()	 {FRAME_STATE(PlayerFrames::FR_DEATHE5, spy_diee6);}
void spy_diee6()	 {FRAME_STATE(PlayerFrames::FR_DEATHE6, spy_diee7);}
void spy_diee7()	 {FRAME_STATE(PlayerFrames::FR_DEATHE7, spy_diee8);}
void spy_diee8()	 {FRAME_STATE(PlayerFrames::FR_DEATHE8, spy_diee9);}
void spy_diee9()	 {FRAME_STATE(PlayerFrames::FR_DEATHE9, spy_diee9);}

void spy_die_ax1()	 {FRAME_STATE(PlayerFrames::FR_AXDETH1, spy_die_ax2);}
void spy_die_ax2()	 {FRAME_STATE(PlayerFrames::FR_AXDETH2, spy_die_ax3);}
void spy_die_ax3()	 {FRAME_STATE(PlayerFrames::FR_AXDETH3, spy_die_ax4);}
void spy_die_ax4()	 {FRAME_STATE(PlayerFrames::FR_AXDETH4, spy_die_ax5);}
void spy_die_ax5()	 {FRAME_STATE(PlayerFrames::FR_AXDETH5, spy_die_ax6);}
void spy_die_ax6()	 {FRAME_STATE(PlayerFrames::FR_AXDETH6, spy_die_ax7);}
void spy_die_ax7()	 {FRAME_STATE(PlayerFrames::FR_AXDETH7, spy_die_ax8);}
void spy_die_ax8()	 {FRAME_STATE(PlayerFrames::FR_AXDETH8, spy_die_ax9);}
void spy_die_ax9()	 {FRAME_STATE(PlayerFrames::FR_AXDETH9, spy_die_ax9);}

// Spy Feign Death... getting up Frames
void spy_upb1()	 {FRAME_STATE(PlayerFrames::FR_DEATHB9, spy_upb2);}
void spy_upb2()	 {FRAME_STATE(PlayerFrames::FR_DEATHB8, spy_upb3);}
void spy_upb3()	 {FRAME_STATE(PlayerFrames::FR_DEATHB7, spy_upb4);}
void spy_upb4()	 {FRAME_STATE(PlayerFrames::FR_DEATHB6, spy_upb5);}
void spy_upb5()	 {FRAME_STATE(PlayerFrames::FR_DEATHB5, spy_upb6);}
void spy_upb6()	 {FRAME_STATE(PlayerFrames::FR_DEATHB4, spy_upb7);}
void spy_upb7()	 {FRAME_STATE(PlayerFrames::FR_DEATHB3, spy_upb8);}
void spy_upb8()	 {FRAME_STATE(PlayerFrames::FR_DEATHB2, spy_upb9);}
void spy_upb9()
{
	FRAME_STATE(PlayerFrames::FR_DEATHB1, spy_upb9);
	player_stand1();
}

void spy_upc1()	 {FRAME_STATE(PlayerFrames::FR_DEATHC15, spy_upc2);}
void spy_upc2()	 {FRAME_STATE(PlayerFrames::FR_DEATHC14, spy_upc3);}
void spy_upc3()	 {FRAME_STATE(PlayerFrames::FR_DEATHC13, spy_upc4);}
void spy_upc4()	 {FRAME_STATE(PlayerFrames::FR_DEATHC12, spy_upc5);}
void spy_upc5()	 {FRAME_STATE(PlayerFrames::FR_DEATHC11, spy_upc6);}
void spy_upc6()	 {FRAME_STATE(PlayerFrames::FR_DEATHC10, spy_upc7);}
void spy_upc7()	 {FRAME_STATE(PlayerFrames::FR_DEATHC9, spy_upc8);}
void spy_upc8()	 {FRAME_STATE(PlayerFrames::FR_DEATHC8, spy_upc9);}
void spy_upc9()	 {FRAME_STATE(PlayerFrames::FR_DEATHC7, spy_upc10);}
void spy_upc10()	 {FRAME_STATE(PlayerFrames::FR_DEATHC6, spy_upc11);}
void spy_upc11()	 {FRAME_STATE(PlayerFrames::FR_DEATHC5, spy_upc12);}
void spy_upc12()	 {FRAME_STATE(PlayerFrames::FR_DEATHC4, spy_upc13);}
void spy_upc13()	 {FRAME_STATE(PlayerFrames::FR_DEATHC3, spy_upc14);}
void spy_upc14()	 {FRAME_STATE(PlayerFrames::FR_DEATHC2, spy_upc15);}
void spy_upc15()
{
	FRAME_STATE(PlayerFrames::FR_DEATHC1, spy_upc15);
	player_stand1();
}

void spy_upd1()	 {FRAME_STATE(PlayerFrames::FR_DEATHD9, spy_upd2);}
void spy_upd2()	 {FRAME_STATE(PlayerFrames::FR_DEATHD8, spy_upd3);}
void spy_upd3()	 {FRAME_STATE(PlayerFrames::FR_DEATHD7, spy_upd4);}
void spy_upd4()	 {FRAME_STATE(PlayerFrames::FR_DEATHD6, spy_upd5);}
void spy_upd5()	 {FRAME_STATE(PlayerFrames::FR_DEATHD5, spy_upd6);}
void spy_upd6()	 {FRAME_STATE(PlayerFrames::FR_DEATHD4, spy_upd7);}
void spy_upd7()	 {FRAME_STATE(PlayerFrames::FR_DEATHD3, spy_upd8);}
void spy_upd8()	 {FRAME_STATE(PlayerFrames::FR_DEATHD2, spy_upd9);}
void spy_upd9()
{
	FRAME_STATE(PlayerFrames::FR_DEATHD1, spy_upd9);
	player_stand1();
}

void spy_upe1()	 {FRAME_STATE(PlayerFrames::FR_DEATHE1, spy_upe9);}
void spy_upe2()	 {FRAME_STATE(PlayerFrames::FR_DEATHE2, spy_upe8);}
void spy_upe3()	 {FRAME_STATE(PlayerFrames::FR_DEATHE3, spy_upe7);}
void spy_upe4()	 {FRAME_STATE(PlayerFrames::FR_DEATHE4, spy_upe6);}
void spy_upe5()	 {FRAME_STATE(PlayerFrames::FR_DEATHE5, spy_upe5);}
void spy_upe6()	 {FRAME_STATE(PlayerFrames::FR_DEATHE6, spy_upe4);}
void spy_upe7()	 {FRAME_STATE(PlayerFrames::FR_DEATHE7, spy_upe3);}
void spy_upe8()	 {FRAME_STATE(PlayerFrames::FR_DEATHE8, spy_upe2);}
void spy_upe9()
{
	FRAME_STATE(PlayerFrames::FR_DEATHE9, spy_upe1);
	player_stand1();
}

void spy_upaxe1()  {FRAME_STATE(PlayerFrames::FR_AXDETH9, spy_upaxe2);}
void spy_upaxe2()  {FRAME_STATE(PlayerFrames::FR_AXDETH8, spy_upaxe3);}
void spy_upaxe3()  {FRAME_STATE(PlayerFrames::FR_AXDETH7, spy_upaxe4);}
void spy_upaxe4()  {FRAME_STATE(PlayerFrames::FR_AXDETH6, spy_upaxe5);}
void spy_upaxe5()  {FRAME_STATE(PlayerFrames::FR_AXDETH5, spy_upaxe6);}
void spy_upaxe6()  {FRAME_STATE(PlayerFrames::FR_AXDETH4, spy_upaxe7);}
void spy_upaxe7()  {FRAME_STATE(PlayerFrames::FR_AXDETH3, spy_upaxe8);}
void spy_upaxe8()  {FRAME_STATE(PlayerFrames::FR_AXDETH2, spy_upaxe9);}
void spy_upaxe9()
{
	FRAME_STATE(PlayerFrames::FR_AXDETH1, spy_upaxe9);
	player_stand1();
}

//=========================================================================
// Function handling the Spy's feign death ability

#define PR_VEC_FEIGNHULL_MIN	V({-16, -16, -24})
#define PR_VEC_FEIGNHULL_MAX	V({16, 16, 4})
void TeamFortress_SpyFeignDeath(float type)
{
	float i;
	entity at_spot;

	if (self->is_feigning)
	{
		// Check to make sure there isn't anyone on top of us
		at_spot = findradius(self->origin, 64);
		i = PR_TRUE;

		while (at_spot != world)
		{
			if (at_spot->classname == "player" && at_spot->health > 0 && at_spot->deadflag == PR_DEAD_NO && self != at_spot)
				i = PR_FALSE;

			at_spot = at_spot->chain;
		}

		// Gizmo - this prevents the spy getting up when an object is on him, like a tesla or sentry
		if ( i == PR_TRUE ) {
			setsize( self, PR_VEC_HULL_MIN, PR_VEC_HULL_MAX );

			if ( InSolid( self ) ) {
				setsize( self, PR_VEC_FEIGNHULL_MIN, PR_VEC_FEIGNHULL_MAX );
				sprint( self, PR_PRINT_HIGH, "You can't get up while something is on top of you.\n" );
				return;
			}
		}

		if (i == PR_TRUE)
		{
			i = 1 + floor(random()*5);

			self->velocity = V({0, 0, 0});
			//WK Jump em up since they fall down now when feigning
			//setorigin (self, self.origin + '0 0 30');
			setsize (self, PR_VEC_HULL_MIN, PR_VEC_HULL_MAX);
			self->view_ofs = V({0, 0, 22});
			//Prevent bad movetype errors, or something
			if (self->classname != "player") {
				RPrint("BUG BUG BUG BUG BUG BUG BUG BUG BUG\n");
				RPrint("Non-player tried to un-feign!\n");
				return;
			}
			self->movetype = PR_MOVETYPE_WALK;
			self->is_feigning = PR_FALSE;
			self->current_weapon = self->weapon;
			W_SetCurrentAmmo();

		#ifdef PR_QUAKE_WORLD
			self->tfstate = self->tfstate - (self->tfstate & PR_TFSTATE_CANT_MOVE);
			TeamFortress_SetSpeed(self);
		#endif

			// Check .weapon, because .current_weapon has been reset
			if (self->weapon <= PR_WEAP_AXE)
			{
				spy_upaxe1 ();
				return;
			}

			if (i == 1)
				spy_upb1();
			else if (i == 2)
				spy_upc1();
			else if (i == 3)
				spy_upd1();
			else
				spy_upe1();
		}
		else
		{
			sprint(self, PR_PRINT_HIGH, "You can't get up while someone\nis standing on you.\n");
		}
	}
	else
	{
		if (!(self->flags & PR_FL_ONGROUND) && (self->waterlevel == 0))
		{
			sprint(self, PR_PRINT_HIGH, "You can't feign in the air.\n");
			return;
		}

		//WK - Can't feign while carrying flag. King cheater!
		if (self->effects & (PR_EF_DIMLIGHT | PR_EF_BRIGHTLIGHT)) {
			sprint(self, PR_PRINT_HIGH, "Can't feign while glowing, you sloppy bitch.\n");
			return;
		}
		//WK - Can't feign while shooting grapple. Crash fix?
		if (self->hook_out) {
			sprint(self, PR_PRINT_HIGH, "Can't feign and grapple, silly\n");
			return;
		}
		//WK - No feign detpackers
		if (self->is_detpacking) {
			sprint(self, PR_PRINT_HIGH, "Can't feign while setting a detpack, fool\n");
			return;
		}
		if (self->is_toffingadet) //SB same for C4 det
		{
			sprint(self, PR_PRINT_HIGH, "How are you going to throw a detpack while lying down?\n");
			return;
		}
		if (self->is_haxxxoring) // SB No sleep-hackers
		{
			sprint(self, PR_PRINT_HIGH, "You can't fall asleep AND hack, do one or the other.\n");
			return;
		}
		//WK Stop detpackers
		if (self->tfstate & PR_TFSTATE_CANT_MOVE) {
			sprint(self,PR_PRINT_HIGH, "Can't feign whilst stationary\n");
			return;
		}
		//WK Prevent sliders
		if (self->is_building == PR_TRUE) {
			sprint(self, PR_PRINT_HIGH, "Can't feign while building, beeatch.\n");
			return;
		}
		//WK Prevent worker/feigners
		if (self->job & PR_JOB_ACTIVE) {
			sprint(self, PR_PRINT_HIGH, "Can't rest while working, you lazy bum!\n");
			return;
		}
		//CH Prevents the most evil bug of all, so evil that I can not speak of it.... j/k
		if (self->done_custom & PR_CUSTOM_BUILDING) {
			sprint (self, PR_PRINT_HIGH, "You can't pretend to die when you're not alive!\n");
			return;
		}

		// Check to make sure there isn't anyone on top of us
		at_spot = findradius(self->origin, 64);
		while (at_spot != world)
		{
			if (at_spot->classname == "player" && at_spot->health > 0 && self != at_spot && at_spot->is_feigning == PR_TRUE)
			{
				sprint(self, PR_PRINT_HIGH, "You can't feign on top of another spy!\n");
			}

			at_spot = at_spot->chain;
		}

	#ifdef PR_QUAKE_WORLD
		makeImmune(self,time+2);
		//self.immune_to_check = time + 2;
		self->tfstate = self->tfstate | PR_TFSTATE_CANT_MOVE;
		TeamFortress_SetSpeed(self);
	#endif

		self->is_feigning = PR_TRUE;
		Attack_Finished(0.8);

	    // make dead guy release hook (wedge)
	    if (self->hook_out)
	    {
		    Reset_Grapple (self->hook);
	    	Attack_Finished(0.75);
		    self->hook_out = PR_TRUE;	// PutClientInServer will reset this
	    }

		if (self->undercover_team == 0 && self->undercover_skin == 0)
			self->items = self->items - (self->items & PR_IT_INVISIBILITY);
		self->invisible_finished = 0;
		self->modelindex = modelindex_player;	// don't use eyes

		// Save the current weapon and remove it
		self->weapon = self->current_weapon;
		self->current_weapon = 0;
		self->weaponmodel = "";
		self->weaponframe = 0;

		//WK setsize (self, '-16 -16 -24', '16 16 4');
		//WK Make people fall to the ground when they feign
		// Ambush, the guy that made MegaTF swears that there is no way
		// to lower the camera angle, and even has a letter from Carmack
		// testifying to that fact...
		// Here's how you do it, moron:
 		// setsize (self, '0 0 -10', '0 0 0');
		setsize (self, PR_VEC_FEIGNHULL_MIN, PR_VEC_FEIGNHULL_MAX);
		self->view_ofs = V({0, 0, 4});
		self->movetype = PR_MOVETYPE_TOSS;

		if (type == 1) //CH normal feign
			DeathSound();

		self->angles[X] = 0;
		self->angles[Z] = 0;

		// Check .weapon, because .current_weapon has been reset
		if (self->weapon <= PR_WEAP_AXE)
		{
			spy_die_ax1 ();
			return;
		}

		i = 1 + floor(random()*6);

		if (i == 1)
			spy_diea1();
		else if (i == 2)
			spy_dieb1();
		else if (i == 3)
			spy_diec1();
		else if (i == 4)
			spy_died1();
		else
			spy_diee1();
	}
}

//=========================================================================
// If its a net game, the SPY goes invisible.
// If its a LAN game, the class/skin changing menu pops up.
void TeamFortress_SpyGoUndercover()
{
	entity te;

	if (self->effects & (PR_EF_DIMLIGHT | PR_EF_BRIGHTLIGHT))
	{
		sprint(self, PR_PRINT_MEDIUM, "You cannot disguise while glowing.\n");
		return;
	}

	if (invis_only == PR_TRUE)
	{
		// If the spy is already invisible, become visible
		if (self->is_undercover == 1)
		{
			self->is_undercover = 0;
			self->modelindex = modelindex_player;	// return to normal
//WK			self.items = self.items - (self.items & #IT_INVISIBILITY);
		}
		else if (self->is_undercover == 2)
		{
			sprint(self, PR_PRINT_HIGH, "You stop going undercover.\n");
			self->is_undercover = 0;
		}
		else // Become invisible
		{
			if (self->ammo_cells > 1)
			{
				sprint(self, PR_PRINT_HIGH, "Going undercover...\n");

				self->is_undercover = 2;

				// Start a timer, which turns the player invisible when it ticks
				te = spawnServerSide(); // PZ: make it a server-side only entity
				te->classname = "timer";
				te->owner = self;
				te->think = TeamFortress_SpyUndercoverThink;
				te->nextthink = time + PR_PC_SPY_GO_UNDERCOVER_TIME;
			}
		}
		self->StatusRefreshTime = time + 0.1;
	}
	else
	{
		// Pop up the menu
		if (self->is_undercover != 2)
		{
			self->current_menu = PR_MENU_SPY;
			self->menu_count = PR_MENU_REFRESH_RATE;
		}
		else // if (self.is_undercover == 2)
		{
			//WK Fix TF2.5 bug
			sprint(self, PR_PRINT_HIGH, "You stop going undercover.\n");
			if (self->undercover_team != 0 || self->undercover_skin != 0)
				self->is_undercover = 1;
			else
				self->is_undercover = 0;
		}
	}
}

//=========================================================================
// Try and find the player's name who's skin and team closest fit the
// current disguise of the spy
void TeamFortress_SpyCalcName(entity spy)
{
	entity te;

	spy->undercover_name = string_null;
	// Find a player on the team the spy is disguised as to pretend to be
	if (spy->undercover_team != 0)
	{
		te = find(world, "classname", "player");
		while (te != world)
		{
			// First, try to find a player with same color and skins
			if (te->team_no == spy->undercover_team && te->skin == spy->undercover_skin)
			{
				spy->undercover_name = te->netname;
				te = world;
			}
			else
				te = find(te, "classname", "player");
		}

		// If we couldn't, just find one of that team
		if (spy->undercover_name == string_null)
		{
			te = find(world, "classname", "player");
			while (te != world)
			{
				if (te->team_no == spy->undercover_team)
				{
					spy->undercover_name = te->netname;
					te = world;
				}
				else
					te = find(te, "classname", "player");
			}
		}
	}
}

//=========================================================================
// Make the spy who owns this timer undercover, and then remove itself
void TeamFortress_SpyUndercoverThink()
{
	float tc;
	string st;

	if (!(self->owner->cutf_items & PR_CUTF_SPY_KIT)) //WK
		return;

	if (self->owner->is_undercover == 2)
	{
//WK		self.owner.items = self.owner.items | #IT_INVISIBILITY;
		if (invis_only == PR_TRUE)
		{
			self->owner->frame = 0;
			self->owner->modelindex = modelindex_eyes;
			self->owner->is_undercover = 1;
		}
		else
		{
			makeImmune(self->owner,time + 4);
			//self.owner.immune_to_check = time + 4;

			// Skin change?
			if (self->skin != 0)
			{
				sprint(self->owner, PR_PRINT_HIGH, "Skin set to ");
				TeamFortress_PrintClassName(self->owner,self->skin,0);

				self->owner->undercover_skin = self->skin;
				TeamFortress_SetSkin(self->owner);
			}

			// Color change
			if (self->team != 0)
			{
				sprint(self->owner, PR_PRINT_HIGH, "Colors set to Team ");
				st = ftos(self->team);
				sprint(self->owner, PR_PRINT_HIGH, st);
				sprint(self->owner, PR_PRINT_HIGH, "\n");

				self->owner->undercover_team = self->team;
				// Set their color
				// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
				//     themselves, to keep from having to kick them when they do.
				//stuffcmd(self.owner, "color ");
				tc = TeamFortress_TeamGetColor(self->team) - 1;
				st = ftos(tc);
                 //- OfN - Nice colors                       //? team_no
                if (nicecolors == 1) st = TeamGetNiceColor(self->team);
				setinfo(self->owner, "topcolor", st);    // PZ
				setinfo(self->owner, "bottomcolor", st); // PZ
				//stuffcmd(self.owner, st);
				//stuffcmd(self.owner, "\n");
			}

			TeamFortress_SpyCalcName(self->owner);

			if (self->owner->StatusBarSize == 0)
				CenterPrint(self->owner, "You are now disguised.\n");

			self->owner->is_undercover = 1;
		}
	}

	self->owner->StatusRefreshTime = time + 0.1;
//CH go to special bar :)
	self->owner->StatusBarScreen = 2;

	dremove(self);
}

//=========================================================================
// Change the Spy's skin to the class they chose
void TeamFortress_SpyChangeSkin(float playerClass)  // PZ: was `class`; changed for C++
{
	entity te;

    // OfN - Psionized guys can't do it
    if (self->tfstate & PR_TFSTATE_PSIONIZED)
    {
        sprint(self,PR_PRINT_HIGH,"You hardly can do anything right now!\n");
        return;
    }

	// If they're returning their skin to their Spy, just reset it
	if (playerClass == PR_PC_SPY && self->playerclass == PR_PC_SPY) //WK
	{
		sprint (self, PR_PRINT_HIGH, "Skin reset.");
		self->undercover_skin = 0;
		TeamFortress_SetSkin(self);

		if (self->undercover_team == 0)
		{
//WK			self.items = self.items - (self.items & #IT_INVISIBILITY);
			self->is_undercover = 0;
		}
		return;
	}

	sprint(self, PR_PRINT_HIGH, "Going undercover...\n");

	self->is_undercover = 2;

	// Start a timer, which changes the spy's skin to the chosen one
	te = spawnServerSide(); // PZ: make it a server-side only entity
	te->classname = "timer";
	te->owner = self;
	te->think = TeamFortress_SpyUndercoverThink;
	te->nextthink = time + PR_PC_SPY_GO_UNDERCOVER_TIME;
	te->skin = playerClass;
	TeamFortress_SetSkin(self);
}

//=========================================================================
// Change the Spy's color to that of the Team they chose
void TeamFortress_SpyChangeColor(float teamno)
{
	entity te;
	string st;
	float tc;

    // OfN - Psionized guys can't do it
    if (self->tfstate & PR_TFSTATE_PSIONIZED)
    {
        sprint(self,PR_PRINT_HIGH,"You hardly can do anything right now!\n");
        return;
    }

	// If they're returning their color to their own team, just reset it
	if (teamno == self->team_no)
	{
		sprint (self, PR_PRINT_HIGH, "Colors reset.\n");

		makeImmune(self,time + 4);
		//self.immune_to_check = time + 4;
		self->undercover_team = 0;
		// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
		//     themselves, to keep from having to kick them when they do.
		//stuffcmd(self, "color ");
		tc = TeamFortress_TeamGetColor(self->team_no) - 1;
		st = ftos(tc);
		//- OfN - Nice colors
		if (nicecolors==1) st =TeamGetNiceColor(self->team_no);
		setinfo(self, "topcolor", st);    // PZ
		setinfo(self, "bottomcolor", st); // PZ
		//stuffcmd(self, st);
		//stuffcmd(self, "\n");

		if (self->undercover_skin == 0)
		{
//WK			self.items = self.items - (self.items & #IT_INVISIBILITY);
			self->is_undercover = 0;
		}
		return;
	}

	sprint(self, PR_PRINT_HIGH, "Going undercover...\n");

	self->is_undercover = 2;

	// Start a timer, which changes the spy's skin to the chosen one
	te = spawnServerSide(); // PZ: make it a server-side only entity
	te->classname = "timer";
	te->owner = self;
	te->think = TeamFortress_SpyUndercoverThink;
	te->nextthink = time + PR_PC_SPY_GO_UNDERCOVER_TIME;
	te->team = teamno;
}

//=========================================================================
// Gas Grenade touch function.
void GasGrenadeTouch()
{
	// Thrown grenades don't detonate when hitting an enemy

	sound (self, PR_CHAN_WEAPON, "weapons/bounce.wav", 1, PR_ATTN_NORM);	// bounce sound
	if (self->velocity == V({0, 0, 0}))
		self->avelocity = V({0, 0, 0});
}

//=========================================================================
// Gas grenade explosion. Throws up the particle cloud.
void GasGrenadeExplode()
{
	entity te;
	float pos;

    sound(self,PR_CHAN_WEAPON,"misc/vapeur2.wav",1,PR_ATTN_NORM);

	// Check the pointcontents to prevent detpack outside the world
	pos = pointcontents(self->origin);
	if (pos == PR_CONTENT_EMPTY)
	{
		te = spawnServerSide(); // PZ: make it a server-side only entity
		te->think = GasGrenadeMakeGas;
		te->nextthink = time + 0.1;
		te->heat = 0;
		te->origin = self->origin;
		te->owner = self->owner;
		te->team_no = self->owner->team_no;
		te->weapon = 0;
	}
	else
	{
		// Make some bubbles :)
		pos = 0;
		while (pos < 10)
		{
			newmis = spawn();
			setmodel (newmis, "progs/s_bubble.spr");
			setorigin (newmis, self->origin);
			newmis->movetype = PR_MOVETYPE_NOCLIP;
			newmis->solid = PR_SOLID_NOT;
			newmis->velocity = V({0, 0, 15});
			newmis->velocity[Z] = 10 + (random() * 20);
			newmis->nextthink = time + 0.5;
			newmis->think = bubble_bob;
			newmis->classname = "bubble";
			newmis->frame = 0;
			newmis->cnt = 0;
			setsize (newmis, V({-8, -8, -8}), V({8, 8, 8}));

			pos = pos + 1;
		}
	}

#ifdef PR_DEMO_STUFF
	// Remove any camera's locks on this missile
	if (self->enemy != world)
		CamProjectileLockOff();
#endif

#ifndef PR_QUAKE_WORLD
	BecomeExplosion ();
#else
	dremove(self);
#endif
}

//=========================================================================
// Gas Grenade Gas function
void GasGrenadeMakeGas()
{
	entity te, timer;

	self->nextthink = time + 0.75;

	// Do the Effects
	te = findradius(self->origin, 200);
	while (te != world)
	{
#ifdef PR_COOP_MODE_ENHANCED
		if (COOP_IsCoopMonster( te ) && te->takedamage && te->health > 0) {
			// only effect monsters that breathe
			if (!(te->monsterflags & PR_MFLAG_NOBREATHING)) {
				deathmsg = PR_DMSG_GREN_GAS;
				TF_T_Damage (te, world, self->owner, 40, PR_TF_TD_IGNOREARMOUR, PR_TF_TD_OTHER);
			}
		} else
#endif

		if (te->classname == "player" && te->deadflag == PR_DEAD_NO && te->health > 0)
		{
			//Dont affect those with gas masks or invincible or biosuits
			if (!(te->tf_items & PR_NIT_SCUBA || te->invincible_finished > time || te->radsuit_finished > time)) //WK
			{
			// Damage
			deathmsg = PR_DMSG_GREN_GAS;
			if (self->heat == 0) // Grievre fix
				TF_T_Damage (te, world, self->owner, 20, PR_TF_TD_IGNOREARMOUR, PR_TF_TD_OTHER);
			else
				TF_T_Damage (te, world, self->owner, 40, PR_TF_TD_IGNOREARMOUR, PR_TF_TD_OTHER);

			// Make them hallucinate
			if (te->tfstate & PR_TFSTATE_HALLUCINATING)
			{
				// Already Hallucination, so just increase the time
				timer = find(world, "classname", "timer");
				while (((timer->owner != te) || (timer->think != HallucinationTimer)) && (timer != world))
				{
					timer = find(timer, "classname", "timer");
				}
				if (timer != world)
				{
					timer->health = timer->health + 50; //SB used to be 25
					if (timer->health > 100)
						timer->health = 100;
					timer->nextthink = time + PR_GR_HALLU_TIME;
				}
			}
			else
			{
				sprint(te, PR_PRINT_HIGH, "You stop breathing as the poison attacks your nerves...\n");
				te->tfstate = te->tfstate | PR_TFSTATE_HALLUCINATING;
				// Create a timer entity
				timer = spawnServerSide(); // PZ: make it a server-side only entity
				timer->nextthink = time + PR_GR_HALLU_TIME;
				timer->think = HallucinationTimer;
				timer->classname = "timer";
				timer->owner = te;
				timer->health = 75; //WK 100 - this makes it more balanced
									//SB 25 - this makes it more funner
				timer->team_no = self->team_no;
			}
			} //WK --^
		}
		te = te->chain;
	}

	self->heat = self->heat + 1;

	// Do the cloud
	if (self->heat == 1)
	{
		WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_BROADCAST, PR_TE_TAREXPLOSION);
		WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
	#ifdef PR_QUAKE_WORLD
		multicast (self->origin, PR_MULTICAST_PVS);
	#endif
		return;
	}
	if (self->heat <= PR_GAS_GREN_CYCLES)
	{
		self->weapon = self->weapon + 1;
		if (self->weapon == 1)
		{
			WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
			WriteByte (PR_MSG_BROADCAST, PR_TE_LAVASPLASH);
			WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
			WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
			WriteCoord (PR_MSG_BROADCAST, self->origin[Z] - 24);
		#ifdef PR_QUAKE_WORLD
			multicast (self->origin, PR_MULTICAST_PVS);
		#endif
		}
		else if (self->weapon == 2)
		{
			self->weapon = 0;
		}

		return;
	}

	dremove(self);
}

//=========================================================================
// Timer function for Hallucinations.
void HallucinationTimer()
{
	entity te;
	float tmpx, tmpy, halltype;

	// Setup for the next Think
	self->health = self->health - PR_GR_HALLU_DEC;
	// medic recovers twice as fast
	if (self->owner->weapons_carried & PR_WEAP_MEDIKIT) //WK
		self->health = self->health - PR_GR_HALLU_DEC;
	if (self->health <= 0)
		self->owner->tfstate = self->owner->tfstate - (self->owner->tfstate & PR_TFSTATE_HALLUCINATING);
	if (!(self->owner->tfstate & PR_TFSTATE_HALLUCINATING))
	{
		sprint(self->owner, PR_PRINT_HIGH, "You feel a little better now.\n");
		dremove(self);
		return;
	}

	self->nextthink = time + PR_GR_HALLU_TIME;

	// Kick
	if (random() < 0.5)
	{
		KickPlayer(-10, self->owner);
	}

	// Do the Hallucinations
	tmpx = (random() * 800) - 400;
	tmpy = (random() * 800) - 400;
	halltype = random();
	msg_entity = self->owner;
	// Explosions
	if (halltype < 0.4) //WK .4 We want all hallucination to be explosions. :)
	{
		WriteByte (PR_MSG_ONE, PR_SVC_TEMPENTITY);
		if (halltype < 0.4)
			WriteByte (PR_MSG_ONE, PR_TE_EXPLOSION);
		else if (halltype < 0.6)
			WriteByte (PR_MSG_ONE, PR_TE_TAREXPLOSION);
		else // if (halltype < 0.4)
			WriteByte (PR_MSG_ONE, PR_TE_LAVASPLASH);
		WriteCoord (PR_MSG_ONE, msg_entity->origin[X] + tmpx);
		WriteCoord (PR_MSG_ONE, msg_entity->origin[Y] + tmpy);
		WriteCoord (PR_MSG_ONE, msg_entity->origin[Z]);
	}
	// Teleport
	else if (halltype < 0.7)
	{
		WriteByte (PR_MSG_ONE, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_ONE, PR_TE_TELEPORT);
		WriteCoord (PR_MSG_ONE, msg_entity->origin[X] + tmpx);
		WriteCoord (PR_MSG_ONE, msg_entity->origin[Y] + tmpy);
		WriteCoord (PR_MSG_ONE, msg_entity->origin[Z]);
	}
	// Lightning
	else // if (halltype < 1)
	{
		te = spawn();
		te->origin[X] = msg_entity->origin[X] + tmpx;
		te->origin[Y] = msg_entity->origin[Y] + tmpy;
		te->origin[Z] = msg_entity->origin[Z];
		WriteByte (PR_MSG_ONE, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_ONE, PR_TE_LIGHTNING2);
		WriteEntity (PR_MSG_ONE, te);
		WriteCoord (PR_MSG_ONE, te->origin[X]);
		WriteCoord (PR_MSG_ONE, te->origin[Y]);
		WriteCoord (PR_MSG_ONE, te->origin[Z]);
		tmpx = (random() * 800) - 400;
		tmpy = (random() * 800) - 400;
		WriteCoord (PR_MSG_ONE, msg_entity->origin[X] + tmpx);
		WriteCoord (PR_MSG_ONE, msg_entity->origin[Y] + tmpy);
		WriteCoord (PR_MSG_ONE, msg_entity->origin[Z]);
		dremove(te);
	}
}

//=========================================================================
// Firing Function for the Tranquiliser Gun
void W_FireTranq()
{
	self->currentammo = self->ammo_nails = self->ammo_nails - 1;

	KickPlayer(-2, self);

	newmis = spawn ();
	newmis->owner = self;
	newmis->movetype = PR_MOVETYPE_FLYMISSILE;
	newmis->solid = PR_SOLID_BBOX;

	makevectors (self->v_angle);
	newmis->velocity = v_forward;
	newmis->velocity = newmis->velocity * 1500;		// Faster than a normal nail WK 1500
	newmis->angles = vectoangles(newmis->velocity);

	newmis->touch = T_TranqDartTouch;

	newmis->think = SUB_Remove;
	newmis->nextthink = time + 6;

	setmodel (newmis, "progs/spike.mdl");
	setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (newmis, self->origin + v_forward*8 + V({0, 0, 16}));
}

//=========================================================================
// Touch Function for the Tranquiliser Darts
void T_TranqDartTouch()
{
	entity timer;

	if (other->solid == PR_SOLID_TRIGGER)
		return;	// trigger field, do nothing

	if (pointcontents(self->origin) == PR_CONTENT_SKY)
	{
		dremove(self);
		return;
	}

	//WK Sweep for mines at point of contact
	GuerillaMineSweep(self->origin);

	// hit something that bleeds
	if (other->takedamage)
	{
		//WK Make sure they aren't immune
		if (!(other->tf_items & PR_NIT_SCUBA || other->invincible_finished > time || other->radsuit_finished > time))
		{
			spawn_touchblood (9);
			deathmsg = PR_DMSG_TRANQ;

			// Do the Damage
			TF_T_Damage (other, self, self->owner, 10, PR_TF_TD_NOTTEAM, PR_TF_TD_NAIL);

			if (other->classname == "player")
			if (other->PR_runes & PR_RUNE_RESIS)
				return;

			// If its a player or demon, tranquilise them
			// Gizmo - allow non demons and non army soldiers to be tranquilised (coop monsters)
			if (IsMonster(other))
			{
				// If they're already tranquilised, just make it last longer
				if (other->tfstate & PR_TFSTATE_TRANQUILISED)
				{
					timer = find(world, "classname", "timer");
					while ((timer->owner != other || timer->think != TranquiliserTimer2) && timer != world)
					{
						timer = find(timer, "classname", "timer");
					}
					if (timer != world)
					{
						timer->nextthink = time + PR_TRANQ_TIME;
					}
				}
				else
				{
					other->tfstate = other->tfstate | PR_TFSTATE_TRANQUILISED;
					// Create a timer entity
					timer = spawnServerSide(); // PZ: make it a server-side only entity
					timer->nextthink = time + PR_TRANQ_TIME;
					timer->think = TranquiliserTimer2;
					timer->classname = "timer";
					timer->owner = other;
					timer->team_no = self->owner->team_no;
				}
			}
			else if (other->classname == "player" && !(Teammate(other, self->owner) && (teamplay & (PR_TEAMPLAY_HALFDIRECT | PR_TEAMPLAY_NODIRECT))))
			{
				// If they're already tranquilised, just make it last longer
				if (other->tfstate & PR_TFSTATE_TRANQUILISED)
				{
					timer = find(world, "classname", "timer");
					while (((timer->owner != other) || (timer->think != TranquiliserTimer)) && (timer != world))
					{
						timer = find(timer, "classname", "timer");
					}
					if (timer != world)
					{
						timer->nextthink = time + PR_TRANQ_TIME;
					}
				}
				else
				{
					sprint(other, PR_PRINT_HIGH, "You stop breathing...\n");
					other->tfstate = other->tfstate | PR_TFSTATE_TRANQUILISED;
					// Create a timer entity
					timer = spawnServerSide(); // PZ: make it a server-side only entity
					timer->nextthink = time + PR_TRANQ_TIME;
					timer->think = TranquiliserTimer;
					timer->classname = "timer";
					timer->owner = other;
					timer->team_no = self->owner->team_no;

					// now slow them down
					TeamFortress_SetSpeed(other);
				}
			}
		}
	}
	else
	{
       if (other->classname == "force_field") //- OfN - Makes field explosion b4 removing it
            FieldEvent(other,self->origin,self);//FieldExplosion(other,self.origin,self);
        else
        {
            WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
            WriteByte (PR_MSG_BROADCAST, PR_TE_SPIKE);
            WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
            WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
            WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
        #ifdef PR_QUAKE_WORLD
            multicast (self->origin, PR_MULTICAST_PVS);
        #endif
        }
	}

	dremove(self);
}

//=========================================================================
// Think function for Tranquilisation.
// Just remove the player's tranquilisation.
void TranquiliserTimer()
{
	self->owner->tfstate = self->owner->tfstate - (self->owner->tfstate & PR_TFSTATE_TRANQUILISED);

	TeamFortress_SetSpeed(self->owner);

	sprint(self->owner, PR_PRINT_HIGH, "You feel more alert now\n");

	dremove(self);
}
//CH used for demons
void TranquiliserTimer2()
{
	self->owner->tfstate = self->owner->tfstate - (self->owner->tfstate & PR_TFSTATE_TRANQUILISED);
	dremove(self);
}

// Reset spy skin and color or remove invisibility
void Spy_RemoveDisguise(entity spy)
{
	string st;
	float tc;
	//local float reset;

	if (invis_only != PR_TRUE)
	{
		if (spy->cutf_items & PR_CUTF_SPY_KIT)
		{
			// Reset the Skin
			if (spy->undercover_skin != 0)
			{
//WK				spy.items = spy.items - (spy.items & #IT_INVISIBILITY);
				makeImmune(spy,time + 4);
				//spy.immune_to_check = time + 4;
				spy->undercover_skin = 0;
				spy->skin = 0;
				TeamFortress_SetSkin(spy);
			}

			// Set their color
			if (spy->undercover_team != 0)
			{
//WK				spy.items = spy.items - (spy.items & #IT_INVISIBILITY);
				makeImmune(spy,time + 4);
				//spy.immune_to_check = time + 4;
				spy->undercover_team = 0;
				// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
				//     themselves, to keep from having to kick them when they do.
				//stuffcmd(spy, "color ");
				tc = TeamFortress_TeamGetColor(spy->team_no) - 1;
				st = ftos(tc);
				//- OfN - Nice colors
				if (nicecolors==1) st =TeamGetNiceColor(spy->team_no);
				setinfo(spy, "topcolor", st);    // PZ
				setinfo(spy, "bottomcolor", st); // PZ
				//stuffcmd(spy, st);
				//stuffcmd(spy, "\n");
			}

			spy->is_undercover = 0;
			self->StatusRefreshTime = time + 0.1;

			TeamFortress_SpyCalcName(spy);
		}
	}
	else
	{
		if (spy->is_undercover)
		{
			// remove the invisibility
			spy->is_undercover = 0;
			spy->modelindex = modelindex_player;	// return to normal

//WK			if (spy.items & #IT_INVISIBILITY)
//WK				spy.items = spy.items - #IT_INVISIBILITY;

			self->StatusRefreshTime = time + 0.1;
		}
	}
}

} // END namespace Progs
