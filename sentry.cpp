/*======================================================
	SENTRY.QC			Custom TeamFortress v2.1

	(c) TeamFortress Software Pty Ltd	29/2/97
	(c) William Kerney			21/10/99
	(c) Craig Hauser				19/3/00
========================================================
Weapons and functions for the Sentry Guns
======================================================*/

#include "progs.h"
#include "sentry.h"
#include "ofndefs.h"
#include "fight.h"
#include "ai.h"
#include "optimize.h"
#include "custom.h"
#include "cpstuff.h"
#include "monsters.h"
#include "sprites.h"
#include "debug.h"
#include "weapons.h"

namespace Progs {

// This is a temporary hack... they'll be replaced with real bots when
// the bot code is in (should that be if? :)
// WK If is right. :)
// SentryGun AI Functions
void Sentry_Rotate();
float Sentry_FindTarget();
void Sentry_FoundTarget();
void Sentry_HuntTarget();
void Sentry_Pain(entity attacker, float damage);
void Sentry_Die();
float Sentry_Fire();
//WK - Will kill enemies on touch, and adds support for flying.
void Sentry_Touch();

// OfN
//float(entity gun) IsInValidLocation;

/* WK==============================================
#ifndef #COOP_MODE
void() ai_face =
{
	local vector temp_vect,temp_ang;
	self.ideal_yaw = vectoyaw(self.enemy.origin - self.origin);
	self.ideal_yaw = anglemod(self.ideal_yaw);
	//WK Turrets swivel instanter
	if (self.tf_items & #NIT_TURRET) { //Auto-swivel
		self.angles_y = self.ideal_yaw;
	}
	else
		ChangeYaw ();
};
#endif
*/
//==============================================
//$cd /quake/fortress/progs/turrgun
//$origin 0 -6 24
//$base base
//$skin skin

namespace SentryFrames
{
	enum {FR_LVL1_STAND1};
	enum {FR_LVL1_SHOOT1 = 1, FR_LVL1_SHOOT2};
	enum {FR_LVL2_STAND1 = 3};
	enum {FR_LVL2_SHOOT1 = 4, FR_LVL2_SHOOT2};
	enum {FR_LVL3_STAND1 = 6};
	enum {FR_LVL3_SHOOT1 = 7, FR_LVL3_SHOOT2};
}

/*	  WK -- This is how you unhack the turret's position :�
	if (self.tf_items & #NIT_TURRET) self.origin_z = self.origin_z + 40;
*/

void RemoveGlow() {
	self->effects = self->effects - (self->effects & PR_EF_DIMLIGHT);
}

//===========================
// Level 1 Sentry Gun Frames
void lvl1_sentry_atk3();
void lvl1_sentry_stand() {FRAME_STATE(SentryFrames::FR_LVL1_STAND1, lvl1_sentry_stand); RemoveGlow(); Sentry_Rotate();}
void lvl1_sentry_atk1()
{
	FRAME_STATE(SentryFrames::FR_LVL1_SHOOT1, lvl1_sentry_atk3);
	ai_face();
	if (self->enemy == world || (self->has_fieldgen > time && self->enemy->classname == "holo") || ((self->enemy->health <= 0 && self->enemy->classname!="holo") || (self->enemy->has_holo == 0 && self->enemy->classname == "holo")) || /* !visible(self.enemy) || */ self->enemy->is_connected == PR_FALSE) //CH
		lvl1_sentry_stand();
	else if (self->ammo_shells <= 0)
		lvl1_sentry_stand();
	else if (Sentry_Fire() == PR_FALSE)
		//WK lvl1_sentry_atk3();
		lvl1_sentry_stand();
}
void lvl1_sentry_atk2()
{
	FRAME_STATE(SentryFrames::FR_LVL1_SHOOT2, lvl1_sentry_atk3);
	ai_face();
	Sentry_Fire();
}
void lvl1_sentry_atk3()	{FRAME_STATE(SentryFrames::FR_LVL1_STAND1, lvl1_sentry_atk1); ai_face();}

//===========================
// Level 2 Sentry Gun Frames
void lvl2_sentry_atk3();
void lvl2_sentry_stand()	 {FRAME_STATE(SentryFrames::FR_LVL2_STAND1, lvl2_sentry_stand); RemoveGlow(); Sentry_Rotate();}
void lvl2_sentry_atk1()
{
	FRAME_STATE(SentryFrames::FR_LVL2_SHOOT1, lvl2_sentry_atk2);
	ai_face();
	if (self->enemy == world || (self->has_fieldgen > time && self->enemy->classname == "holo") || ((self->enemy->health <= 0 && self->enemy->classname!="holo") || (self->enemy->has_holo == 0 && self->enemy->classname == "holo")) || /* !visible(self.enemy) || */ self->enemy->is_connected == PR_FALSE) //CH
		lvl2_sentry_stand();
	else if (self->ammo_shells <= 0)
		lvl2_sentry_stand();
	else if (Sentry_Fire() == PR_FALSE)
		//WK lvl2_sentry_atk3();
		lvl2_sentry_stand();
}
void lvl2_sentry_atk2()
{
	FRAME_STATE(SentryFrames::FR_LVL2_SHOOT2, lvl2_sentry_atk3);
	ai_face();
	Sentry_Fire();
}
void lvl2_sentry_atk3()
{
	FRAME_STATE(SentryFrames::FR_LVL2_STAND1, lvl2_sentry_atk1);
	ai_face();
	Sentry_Fire();
}

//===========================
// Level 3 Sentry Gun Frames
void lvl3_sentry_atk3();
void lvl3_sentry_atk4();
void lvl3_sentry_stand()	 {FRAME_STATE(SentryFrames::FR_LVL3_STAND1, lvl3_sentry_stand); RemoveGlow(); Sentry_Rotate();}
void lvl3_sentry_atk1()
{
	FRAME_STATE(SentryFrames::FR_LVL3_SHOOT1, lvl3_sentry_atk2);
	ai_face();
	if (self->enemy == world || (self->has_fieldgen > time && self->enemy->classname == "holo") || ((self->enemy->health <= 0 && self->enemy->classname!="holo") || (self->enemy->has_holo == 0 && self->enemy->classname == "holo")) || /* !visible(self.enemy) || */ self->enemy->is_connected == PR_FALSE) //CH
		lvl3_sentry_stand();
	else if (self->ammo_shells <= 0 && self->ammo_rockets <= 0)
		lvl3_sentry_stand();
	else if (self->ammo_shells <= 0 && self->ammo_rockets > 0) //CH rocket check
	{
		RemoveGlow();
		lvl3_sentry_atk4();
	} else if (Sentry_Fire() == PR_FALSE) {
		//WK lvl3_sentry_atk3();
		lvl3_sentry_stand();
	}
}
void lvl3_sentry_atk2()
{
	FRAME_STATE(SentryFrames::FR_LVL3_SHOOT2, lvl3_sentry_atk3);
	ai_face();
	Sentry_Fire();
}
void lvl3_sentry_atk3()
{
	FRAME_STATE(SentryFrames::FR_LVL3_STAND1, lvl3_sentry_atk1);
	ai_face();
	Sentry_Fire();
}
void lvl3_sentry_atk4()
{
	FRAME_STATE(SentryFrames::FR_LVL3_STAND1, lvl3_sentry_atk5);
	ai_face();
	if (self->enemy == world || (self->has_fieldgen > time && self->enemy->classname == "holo") || ((self->enemy->health <= 0 && self->enemy->classname!="holo") || (self->enemy->has_holo == 0 && self->enemy->classname == "holo")) || /* !visible(self.enemy) || */ self->enemy->is_connected == PR_FALSE) //CH
		lvl3_sentry_stand();
	else if (self->ammo_shells <= 0 && self->ammo_rockets <= 0)
		lvl3_sentry_stand();
	else if (Sentry_Fire() == PR_FALSE)
		//WK lvl3_sentry_atk3();
		lvl3_sentry_stand();
}
void lvl3_sentry_atk5()
{
	FRAME_STATE(SentryFrames::FR_LVL3_STAND1, lvl3_sentry_atk4);
	ai_face();
	Sentry_Fire();
}

//=============
void Sentry_Rotate()
{
/*	local string st;
	local float blah;*/

	#ifdef PR_FIXES_FOR_FLOATINGBUILDS
	/* TODO:

	if (!(self.tf_items & #NIT_TURRET))
	//if (!(self.flags & #FL_ONGROUND))
	{
		self.flags = self.flags - (self.flags & #FL_ONGROUND);
		self.velocity_z = -80;
	}*/
	#endif

	// PZ: Prevent the turret from sliding off the base after dropping the turret with the zero gravity gun.
	if (!(self->tf_items & PR_NIT_TURRET) && self->trigger_field->velocity == V({0, 0, 0})) // if the base/stand isn't moving
		self->velocity = V({0, 0, 0});  // don't let the turret move

	if (self->is_malfunctioning & PR_SCREWUP_ONE)
	{
		self->ideal_yaw = self->ideal_yaw + 10;
		ChangeYaw();
		return;
	}

	/*if (!IsInValidLocation(self))
		Sentry_Die();*/

	if (Sentry_FindTarget())
		return;

	//- OfN this does the sentry to not return to rotate status after enemy becomes not visible or died
	// it doesn't alter the real behaviour only the look
	if (self->attack_finished > time)
	{
		self->has_holo=1;
		return;
	}

	if (self->has_holo==1)
	{
		self->has_holo=0;
		sound (self, PR_CHAN_ITEM, "weapons/trr2lost.wav", 0.25, PR_ATTN_NORM);
	}

	#ifdef PR_SENTRY_SMOOTHROTATION

	if (self->heat > PR_SENTRY_ROTATIONSTEPS - 1)
		self->heat = 0;

	float f3;

	// Standard sine
	f3 = getwave(self->heat/PR_SENTRY_ROTATIONSTEPS,PR_GWAVE_USEMINMAX,self->waitmin,self->waitmax,0,0,0);

	// Alternative (more circular wave instead of a pure sine)
	//f3 = getwave(self.heat/#SENTRY_ROTATIONSTEPS,#GWAVE_USEMINMAX | #GWAVE_USESHAPE,self.waitmin,self.waitmax,0,0,0.4);

	self->angles[Y] = anglemod(f3);

	if (self->heat == PR_SENTRY_ROTATIONSTEPS - 1)
	if (random() < 0.1)
		sound (self, PR_CHAN_ITEM, "weapons/turridle.wav", 1, PR_ATTN_NORM);

	self->heat = self->heat + 1;

	#else // not SENTRY_SMOOTHROTATION

	if (self->heat == 0)
	{
		self->ideal_yaw = anglemod(self->waitmin);
		ChangeYaw();

		if ((anglemod(self->angles[Y]) <= (self->waitmin + 1)) && (anglemod(self->angles[Y]) >= (self->waitmin - 2)))  //CH after the && fixed spaz sent!!
		{
			self->heat = 1;
			if (random() < 0.1)
				sound (self, PR_CHAN_ITEM, "weapons/turridle.wav", 1, PR_ATTN_NORM);
		}
	}
	else
	{
		self->ideal_yaw = anglemod(self->waitmax);
		ChangeYaw();

		if ((anglemod(self->angles[Y]) >= (self->waitmax - 1 )) && (anglemod(self->angles[Y]) <= (self->waitmax + 2)))  //CH after the && fixed spaz sent!!
			self->heat = 0;
	}
	#endif // SENTRY_SMOOTHROTATION
}

//====================================================================
// new RANGE_VERYFAR hacked sentries can shot at RANGE_FAR
float rangesentry(entity targ)
{
	vector	spot1, spot2;
	float 	r;

	spot1 = self->origin + self->view_ofs;
	spot2 = targ->origin + targ->view_ofs;

	r = vlen (spot1 - spot2);

	if (r < 120)
		return PR_RANGE_MELEE;
	if (r < 500)
		return PR_RANGE_NEAR;
	if (r < 1000)
		return PR_RANGE_MID;
	if (r < 2000)
		return PR_RANGE_FAR;
	return PR_RANGE_VERYFAR;
}

float Sentry_FindTarget()
{
	#ifdef PR_MAD_SENTRY

	self->enemy = world;

	if (!(self->tf_items & PR_NIT_TURRET))
	{
		if (!visible2x(self,self->real_owner))
			return PR_FALSE;
	}
	else
	{
		if (!visible2(self,self->real_owner))
			return PR_FALSE;
	}

	self->enemy = self->real_owner;


		Sentry_FoundTarget ();


	return PR_TRUE;
	#endif

	self->enemy = world; //CH for sbar

#ifdef PR_QUAKE_WORLD // culled by KK. was QUAKE_WORLD
	/*if (infokey(world,"ceasefire")=="on")
		return #FALSE;*/
	if (ceasefire)
		return PR_FALSE;
#endif

	entity client = world;
	entity te = world;
	float r, gotone, loopc;

	//WK Hack to get floating sentry working
	if (self->tf_items & PR_NIT_TURRET) {
		self->origin[Z] = self->origin[Z] - 40;
	}

	// Try a few checks to make it react faster
	r = 0;
	loopc = 0;
	gotone = PR_FALSE;
	//WK Theortetically this will check every client on the server now
	while (loopc < 32 && gotone == PR_FALSE) //WK 3
	{
		client = checkclient();

		gotone = PR_TRUE;

		if (client == world)
			gotone = PR_FALSE;

		if (!Pharse_Client(client, self, 1, 0, 0, 1))
			gotone = PR_FALSE;

		r = rangesentry (client);

		//- ofn
		//if (r == #RANGE_FAR)
		//	gotone = #FALSE;

		//WK Hack, turret should be able to see in all directions...
		if (!(self->tf_items & PR_NIT_TURRET)) {

			if (r == PR_RANGE_NEAR)
			{
				if (client->show_hostile < time && !infront (client))
					gotone = PR_FALSE;
			}
			else if (r == PR_RANGE_MID)
			{
				if (!infront (client))
					gotone = PR_FALSE;
			}
			else if (r == PR_RANGE_FAR) //- OfN - Sentries with enhanced circuits can shoot at far range
			{
				if (!(self->all_active & PR_IMPROVED_FOUR) || !infront (client))
					gotone = PR_FALSE;
			}
		}
		else //- OfN - a turretized sentry gun...
		{
			if (r == PR_RANGE_FAR && !(self->all_active & PR_IMPROVED_FOUR))
				gotone = PR_FALSE;
		}

		//- OfN ----------------//
		if (r == PR_RANGE_VERYFAR)
			gotone = PR_FALSE;
		//----------------------//

		//WK Solve a bug in sentry targetting?
		if (gotone)
			loopc = 1000;

		loopc = loopc + 1;
	}

	if (!gotone && (self->all_active & PR_IMPROVED_FOUR)) //if no player target found lets scan for monsters and army..
	{
		te = find(world, TO_STR(PR_monsterflag), PR_STRFLAG_MONSTER);

		while (te != world && gotone == PR_FALSE)
		{
			gotone = PR_TRUE;

			if (Teammate(self->real_owner, te->real_owner))
				gotone = PR_FALSE;
			else if (!IsOwnedMonster(te))
				gotone = PR_FALSE;
			else if (te->health <= 0)
				gotone = PR_FALSE;
			else if ( (!(self->tf_items & PR_NIT_TURRET) && !visible2x(self,te)) || ((self->tf_items & PR_NIT_TURRET) && !visible2(self,te)))
				gotone = PR_FALSE;
			else
			{
				r = rangesentry(te);

				//WK Hack, turret should be able to see in all directions...
				if (!(self->tf_items & PR_NIT_TURRET)) {

					if (r == PR_RANGE_NEAR)
					{
						if (!infront (te))
							gotone = PR_FALSE;
					}
					else if (r == PR_RANGE_MID)
					{
						if (!infront (te))
							gotone = PR_FALSE;
					}
					else if (r == PR_RANGE_FAR) //- OfN - Sentries with enhanced circuits can shot at far range
					{
						if (!(self->all_active & PR_IMPROVED_FOUR) || !infront (te))
							gotone = PR_FALSE;
					}
				}
				else //- OfN - a turretized sentry gun...
				{
					if (r == PR_RANGE_FAR && !(self->all_active & PR_IMPROVED_FOUR))
						gotone = PR_FALSE;
				}

				//- OfN ----------------//
				if (r == PR_RANGE_VERYFAR)
					gotone = PR_FALSE;
				//----------------------//
			}

			if (gotone)
				client=te;

			te = find(te, TO_STR(PR_monsterflag), PR_STRFLAG_MONSTER);

		} // while

	} //monster scanning

#ifdef PR_COOP_MODE_ENHANCED
	if ( !gotone ) {
		te = find( world, TO_STR(PR_monsterflag), PR_STRFLAG_MONSTER );

		while ( te != world && !gotone ) {
			gotone = PR_TRUE;

			//r = GetTeam( te );

			if ( !COOP_IsCoopMonster( te ) )
				gotone = PR_FALSE;
			else if (Teammate(self->real_owner, te))
				gotone = PR_FALSE;
			else if ( te->health <= 0 )
				gotone = PR_FALSE;
			else if ( ( !( self->tf_items & PR_NIT_TURRET ) && !visible2x( self, te ) )
				|| ( self->tf_items & PR_NIT_TURRET && !visible2( self, te ) ) )
				gotone = PR_FALSE;
			else {
				r = rangesentry( te );

				//WK Hack, turret should be able to see in all directions...
				if ( !( self->tf_items & PR_NIT_TURRET ) ) {

					if ( r == PR_RANGE_NEAR ) {
						if ( !infront( te ) )
							gotone = PR_FALSE;
					} else if ( r == PR_RANGE_MID ) {
						if ( !infront( te ) )
							gotone = PR_FALSE;
					} else if ( r == PR_RANGE_FAR ) { //- OfN - Sentries with enhanced circuits can shot at far range
						if ( !( self->all_active & PR_IMPROVED_FOUR ) || !infront( te ) )
							gotone = PR_FALSE;
					}
				} else { //- OfN - a turretized sentry gun...
					if ( r == PR_RANGE_FAR && !( self->all_active & PR_IMPROVED_FOUR ) )
						gotone = PR_FALSE;
				}

				//- OfN ----------------//
				if ( r == PR_RANGE_VERYFAR )
					gotone = PR_FALSE;
				//----------------------//
			}

			if ( gotone )
				client = te;

			te = find( te, TO_STR(PR_monsterflag), PR_STRFLAG_MONSTER );
		}
	}
#endif

	//if no player or monsters found lets scan for possible holo fakes.. (if has improved circuits no scan for holos)
	if (!gotone && !(self->all_active & PR_IMPROVED_FOUR))
	{
		te = find(world, "classname","holo");

		while (te != world && gotone == PR_FALSE)
		{
			gotone = PR_TRUE;

			#ifndef PR_HOLO_TEST
			if (Teammate(self->real_owner, te->real_owner))
				gotone = PR_FALSE;
			else
			#endif

			if (self->has_fieldgen > time) // engineers add some seconds of ignore-holo status to sentries when spannered
				gotone = PR_FALSE;
			else if (te->has_holo == 0)
				gotone = PR_FALSE;
			else if ( (!(self->tf_items & PR_NIT_TURRET) && !visible2x(self,te)) || ((self->tf_items & PR_NIT_TURRET) && !visible2(self,te)))
				gotone = PR_FALSE;
			else
			{
				r = rangesentry(te);

				//WK Hack, turret should be able to see in all directions...
				if (!(self->tf_items & PR_NIT_TURRET)) {

					if (r == PR_RANGE_NEAR)
					{
						if (!infront (te))
							gotone = PR_FALSE;
					}
					else if (r == PR_RANGE_MID)
					{
						if (!infront (te))
							gotone = PR_FALSE;
					}
					else if (r == PR_RANGE_FAR) //- OfN - Sentries with enhanced circuits can shot at far range
					{
						if (!(self->all_active & PR_IMPROVED_FOUR) || !infront (te))
							gotone = PR_FALSE;
					}
				}
				else //- OfN - a turretized sentry gun...
				{
					if (r == PR_RANGE_FAR && !(self->all_active & PR_IMPROVED_FOUR))
						gotone = PR_FALSE;
				}

				//- OfN ----------------//
				if (r == PR_RANGE_VERYFAR)
					gotone = PR_FALSE;
				//----------------------//

			}

			if (gotone)
				client=te;

			te = find(te, "classname", "holo");

		} // while

	} //holo scanning

	//WK Unhack our hack
	if (self->tf_items & PR_NIT_TURRET) {
		self->origin[Z] = self->origin[Z] + 40;
	}

	if (!gotone)
		return PR_FALSE;

	// Found a Target
	self->enemy = client;

	//- OfN - what was this for??
	/*if (self.enemy.classname != "player")
	{
		self.enemy = self.enemy.enemy;
		if (self.enemy.classname != "player")
		{
			self.enemy = world;
			return #FALSE;
		}
	}*/

	Sentry_FoundTarget ();

	return PR_TRUE;
}

void Sentry_FoundTarget()
{
	// Cannon Powerup Sound?
	if (self->ammo_shells > 0 || (self->ammo_rockets > 0 && self->weapon == 3)) {
		if (self->attack_finished < time)		//- OfN -
			sound (self, PR_CHAN_VOICE, "weapons/turrspot.wav", 1, PR_ATTN_NORM);
	}

	Sentry_HuntTarget ();

	if (self->super_damage_finished < time) {
		self->super_damage_finished = time + 1.0; //WK Was at .5, which was too fast

		if (self->all_active & PR_IMPROVED_FOUR)
			self->super_damage_finished = time + 0.65; //- OfN - improved circuits! heh
	}
}

void Sentry_HuntTarget()
{
	self->goalentity = self->enemy;
	if (self->weapon == 1)
		self->think = lvl1_sentry_atk1;
	else if (self->weapon == 2)
		self->think = lvl2_sentry_atk1;
	else // if (self.weapon == 3)
	{
		if (self->ammo_shells <= 0 && self->ammo_rockets > 0) //CH has rockets but no shells
			self->think = lvl3_sentry_atk4;
		else
			self->think = lvl3_sentry_atk1;
	}
	self->ideal_yaw = vectoyaw(self->enemy->origin - self->origin);
	self->nextthink = time + 0.1;

	//SUB_AttackFinished (0.5); // WK Does this do anything?
								// - OfN - Nope lol it didnt
	float tfactor;
	tfactor=1;

	//- OfN - Does this sentry has enhanced circuits? if so.. cut down lock time
	if (self->all_active & PR_IMPROVED_FOUR) tfactor=0.5;

	//WK --- Invisible people take longer for sentries to lock onto
	// because they have to use their heat sensors
	if (self->enemy->modelindex == modelindex_null)
		self->nextthink = time + (2*tfactor);	// Must acquire a heat signal
	else if (self->enemy->modelindex == modelindex_eyes)
		self->nextthink = time + (1.5*tfactor);	// Some visual, so its a little easier
	else
		self->nextthink = time + (0.5*tfactor);	// Some visual, so its a little easier
}

void Sentry_Pain(entity attacker, float damage)
{
	// Update the owner's status bar
	self->real_owner->StatusRefreshTime = time + 0.2;
//CH special sbar  for eng.
	self->real_owner->StatusBarScreen = 1;
}

void Sentry_Die()
{
	//RemoveMyTimers(self); // psionic clean-up

	if (self->real_owner->cutf_items & PR_CUTF_DOUBLESENTRY && self->dont_do_triggerwork)
	{
		if (self->dont_do_triggerwork == 1)
			sprint(self->real_owner, PR_PRINT_HIGH, "Your sentry gun #1 was destroyed.\n");
		else if (self->dont_do_triggerwork == 2)
			sprint(self->real_owner, PR_PRINT_HIGH, "Your sentry gun #2 was destroyed.\n");
		else
			sprint(self->real_owner, PR_PRINT_HIGH, "Your sentry gun was destroyed.\n");
	}
	else
		sprint(self->real_owner, PR_PRINT_HIGH, "Your sentry gun was destroyed.\n");

	self->real_owner->has_sentry = self->real_owner->has_sentry - (self->real_owner->has_sentry & self->dont_do_triggerwork);
	if (self->real_owner->has_sentry < 0)
		self->real_owner->has_sentry = 0;

	entity sprite;

	sprite = SpawnSprite(1,PR_SPRITE_AIRBURST,self->origin,V({0, 0, 0}),PR_SPRITEMOVE_UPSLOW,0.1);

	if (sprite != world)
	{
		sprite->effects = PR_EF_DIMLIGHT;

		if (self->real_owner->team_no == 1)
			sprite->effects = PR_EF_DIMLIGHT | PR_EF_BLUE;
		else if (self->real_owner->team_no == 2)
			sprite->effects = PR_EF_DIMLIGHT | PR_EF_RED;
	}

	ThrowGib("progs/t2gib1.mdl", -70, PR_TRUE, 0,0, PR_FALSE);
	ThrowGib("progs/t2gib2.mdl", -70, PR_TRUE, 0,0, PR_FALSE);
	ThrowGib("progs/t2gib3.mdl", -70, PR_TRUE, 0,0, PR_FALSE);
	ThrowGib("progs/t2gib4.mdl", -70, PR_TRUE, self->skin,0, PR_FALSE);

	if (self->classname == "building_sentrygun_base")
	{
		if (self->oldenemy != world)
			dremove(self->oldenemy);
	}
	else
	{
		if (self->trigger_field != world)
			dremove(self->trigger_field);
	}

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

float Sentry_Fire()
{
	vector dir;
	vector miss_factor;

	// PZ: sentries being carried, or flying through the air after being thrown, by ZGG, don't shoot, except in Coop
	if (deathmatch && (self->pickedupby != world || (self->velocity != V({0, 0, 0}) && !(self->flags & PR_FL_ONGROUND)))) return PR_FALSE;

	//- OfN -
	vector soffset = V({0, 0, 0});

	if (!(self->tf_items & PR_NIT_TURRET))
		soffset = V({0, 0, 20});

	// this on the end..
	//self.attack_finished = time + #SENTRY_UNLOCKTIME; // don't rotate immediately after target invisible or dead

#ifdef PR_QUAKE_WORLD // culled by KK. was QUAKE_WORLD
	/*if (infokey(world,"ceasefire")=="on") //Cyto
			return #FALSE;*/
	if (ceasefire)
		return PR_FALSE;
#endif

	// OfN - Psionized sentries don't shot
	/*if (self.tfstate & #TFSTATE_PSIONIZED)
		return #FALSE;*/

	//WK Stop gun from shooting at dead spies
	if (self->enemy->is_feigning)
		return PR_FALSE;

	//WK Hack to get floating sentry working - reset before all the returns
	if (self->tf_items & PR_NIT_TURRET) self->origin[Z] = self->origin[Z] - 40;

	// Only fire if they're within sight
	dir = self->enemy->origin - self->origin;
	if (vlen(dir) > 2048) //WK Don't fire if they're too far away
	{
		if (self->tf_items & PR_NIT_TURRET) self->origin[Z] = self->origin[Z] + 40;
		return PR_FALSE;
	}
	dir = normalize(dir);

	//WK Turret can shoot in any direction
	if (!(self->tf_items & PR_NIT_TURRET)) {
		if ((self->ideal_yaw - anglemod(self->angles[Y])) < -10 || (self->ideal_yaw - anglemod(self->angles[Y])) > 10) {
			return PR_FALSE;
		}
	}
	#ifndef PR_MAD_SENTRY
	if (self->ammo_shells >= 1)
		self->ammo_shells = self->ammo_shells - 1;
	#endif
	if (self->ammo_shells < 1) //WK Disallow half-shells
		self->ammo_shells = 0;
	if ((self->ammo_shells <= 0 && self->weapon != 3) || (self->ammo_shells <= 0 && self->ammo_rockets <= 0 && self->weapon == 3)) //CH stay on target if have rockets
	{
		if (self->tf_items & PR_NIT_TURRET) self->origin[Z] = self->origin[Z] + 40;
		return PR_FALSE;
	}
	//CH this further checks while firing
	traceline (self->origin + soffset, self->enemy->origin, PR_TL_BSP_ONLY, self);
	if (trace_fraction != 1 || trace_endpos != self->enemy->origin)
	{
		if (self->tf_items & PR_NIT_TURRET) self->origin[Z] = self->origin[Z] + 40;
		return PR_FALSE;
	}

	if (self->ammo_shells > 0)
	{
		//WK muzzleflash();
		self->effects = self->effects | PR_EF_DIMLIGHT;
	/*	if (self.tf_items & #NIT_TURRET)
			sound (self ,#CHAN_WEAPON, "weapons/asscan2.wav", 1, #ATTN_NORM);
		else */
		//sound (self ,#CHAN_WEAPON, "weapons/sniper.wav", 1, #ATTN_NORM);
		sound (self ,PR_CHAN_WEAPON, "weapons/sntr666.wav", 1, PR_ATTN_NORM);
		deathmsg = PR_DMSG_SENTRYGUN_BULLET;
	}



////////////
//WK Our hacked attempt to make sentries shoot right
	//Make base not shootable //CH if its not floating it needs this
	if (!(self->tf_items & PR_NIT_TURRET))
	{
		if (self->trigger_field != world) self->trigger_field->solid = PR_SOLID_NOT;
		//soffset = '0 0 20'; // non-turretized sentries correction
	}

	if (self->is_malfunctioning & PR_SCREWUP_TWO)
	{
		miss_factor[Z] = random() * 200 - 100;
		miss_factor[Y] = random() * 200 - 100;
		miss_factor[X] = random() * 200 - 100;
		traceline (self->origin + soffset, self->enemy->origin + miss_factor, PR_TL_ANY_SOLID, self);
	}
	else
		traceline (self->origin + soffset, self->enemy->origin, PR_TL_ANY_SOLID, self);

	if (trace_fraction != 1.0 && trace_ent->takedamage && self->ammo_shells > 0) //Hit something and has shells
	{
		SpawnBlood (trace_endpos, 50);

		float thedmg;	thedmg = 6;
		float therange; therange = rangesentry(trace_ent);

		// OfN - damage now depends on distance to target
		if (therange == PR_RANGE_MELEE)
			thedmg = 12;
		else if (therange == PR_RANGE_NEAR)
			thedmg = 6;
		else if (therange == PR_RANGE_MID)
			thedmg = 4;
		else if (therange == PR_RANGE_FAR) // only circuit hacked sentries shoot at this range
			thedmg = 3;
		else thedmg = 2; // this shouldn't happen ever (RANGE_VERYFAR)

		// OfN - hacked sentries do more damage (improved circuits)
		if (self->all_active & PR_IMPROVED_FOUR)
			thedmg = thedmg * 1.25; // 1.25 too high?

		#ifndef PR_MAD_SENTRY
		TF_T_Damage (trace_ent, self, self, thedmg, PR_TF_TD_NOTTEAM, PR_TF_TD_SHOT);
		#endif
	}
	else if (trace_fraction != 1.0 && trace_ent->classname == "force_field")
	{
	   FieldEvent(trace_ent,trace_endpos,trace_ent);
	   //FieldExplosion(trace_ent,trace_endpos,trace_ent);
	   //PutFieldWork(trace_ent);
	} // Holo fake as target...
	/*else if (trace_fraction != 1.0 && trace_ent.classname == "holo" && self.ammo_shells > 0)
	{

	} */

	//FireBullets (1, dir, '0.1 0.1 0');
	//CH if its not floating it needs this
	if (!(self->tf_items & PR_NIT_TURRET))
	{
		if (self->trigger_field != world) self->trigger_field->solid = PR_SOLID_BBOX;
	}
/////////////

	float exactRange; // PZ
	exactRange = vlen((self->origin + self->view_ofs) - (trace_ent->origin + trace_ent->view_ofs)); // later PZ NOTE: why am I looking at view_ofs?

	// Level 3 Turrets fire rockets every 3 seconds
	if (self->weapon == 3 && self->ammo_rockets > 0 && self->super_damage_finished < time
	    && exactRange >= 210) // PZ: FFS, you are an intelligent machine. Stop fragging yourself when targets are too close.
	{
		sound (self, PR_CHAN_VOICE, PR_SENTRY_ROCKETSND, 1, PR_ATTN_NORM); //UGLY AND STOPS COOL SOUND
		// wtf happens here, why is this the supernailgun sound?
		if (self->is_malfunctioning & PR_SCREWUP_THREE)
		{
			float damg;
			damg = random() * 50 + 120;
			T_RadiusDamage(self, self, damg, world);
			WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
			WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
			WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
			WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
			WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
			if (self->tf_items & PR_NIT_TURRET)
				self->origin[Z] = self->origin[Z] + 40;
			return PR_FALSE;
		}

		newmis = spawn ();
		newmis->owner = self;
		newmis->movetype = PR_MOVETYPE_FLYMISSILE;
		newmis->solid = PR_SOLID_BBOX;

		// set newmis speed
		if (self->is_malfunctioning & PR_SCREWUP_TWO) // SB - oh no we've been hax0red
		{
			miss_factor[Z] = random() * 200 - 100;
			miss_factor[Y] = random() * 200 - 100;
			miss_factor[X] = random() * 200 - 100;
		}
		else
		{
			miss_factor[Z] = 0;
			miss_factor[Y] = 0;
			miss_factor[X] = 0;
		}

		if (self->tf_items & PR_NIT_TURRET)
			newmis->velocity = normalize(self->enemy->origin + miss_factor - (self->origin - V({0, 0, 16}))) * 800;
		else
			newmis->velocity = normalize(self->enemy->origin + miss_factor - (self->origin + V({0, 0, 16}))) * 800;
		newmis->angles = vectoangles(newmis->velocity);
		newmis->weapon = PR_DMSG_SENTRYGUN_ROCKET;
		newmis->touch = T_MissileTouch;

		// set newmis duration
		newmis->nextthink = time + 5;
		newmis->think = SUB_Remove;

		setmodel (newmis, "progs/missile.mdl");
		setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));
		if (self->tf_items & PR_NIT_TURRET) //Determines if rocket should spawn above or below sent
			setorigin (newmis, self->origin + v_forward*8 - V({0, 0, 16})); //CH make rocket 16 below
		else
			setorigin (newmis, self->origin + v_forward*8 + V({0, 0, 16})); //CH make rocket 16 above

		self->super_damage_finished = time + 3;

		// - OfN - if this sentry has enhanced circuits then delay between rockets firing is cut half
		if (self->all_active & PR_IMPROVED_FOUR) self->super_damage_finished = time + 1.5;

		self->ammo_rockets = self->ammo_rockets - 1;

		if (self->ammo_rockets == 5) //CH 10 was too high
			sprint(self->real_owner, PR_PRINT_HIGH, "Sentry Gun is low on rockets.\n");
	}
	// Warn owner that it's low on ammo
	//WK if (self.ammo_shells == 0 && (random() < 0.1))
	if ((self->ammo_shells == 0 && self->weapon != 3) || (self->ammo_shells == 0 && self->weapon == 3 && self->ammo_rockets > 0 && (random() < 0.05))) //CH .05 seems to be good.
		sprint(self->real_owner, PR_PRINT_HIGH, "Sentry Gun is out of shells.\n");
	else if (self->ammo_shells == 20)
		sprint(self->real_owner, PR_PRINT_HIGH, "Sentry Gun is low on shells.\n");

	//WK < 0.1
	if (self->ammo_rockets == 0 && self->weapon == 3 && (random() <= 0.1))
		sprint(self->real_owner, PR_PRINT_HIGH, "Sentry Gun is out of rockets.\n");

	if (self->tf_items & PR_NIT_TURRET) self->origin[Z] = self->origin[Z] + 40;

	self->attack_finished = time + PR_SENTRY_UNLOCKTIME + 2 * random() - 1; // don't rotate immediately after target invisible or dead

	return PR_TRUE;
}

//WK Returns #TRUE if the vector is not a spot a turret can attach to
float BadSpot(const vector& vec)
{
	/*
	if (pointcontents(vec) == #CONTENT_SOLID)
		sprint(self.real_owner,#PRINT_HIGH,"SOLID Point\n");
	else if (pointcontents(vec) == #CONTENT_EMPTY)
		sprint(self.real_owner,#PRINT_HIGH,"EMPTY Point\n");
	else if (pointcontents(vec) == #CONTENT_SKY)
		sprint(self.real_owner,#PRINT_HIGH,"SKY Point\n");
	else if (pointcontents(vec) == #CONTENT_WATER)
		sprint(self.real_owner,#PRINT_HIGH,"WATER Point\n");
	else
		sprint(self.real_owner,#PRINT_HIGH,"BUG: Other Point\n");
	*/
	if (pointcontents(vec) == PR_CONTENT_SKY)
		return PR_TRUE;
	return PR_FALSE;
}

//WK Sentry Touch function
//Will kill bad guy
void Sentry_Touch()
{
	//WK Check for blockage
	if (pointcontents(self->origin) == PR_CONTENT_SKY)
	{
		sprint(self->real_owner, PR_PRINT_HIGH, "Your sentry gun flew away.\n");
		Sentry_Die();
		return;
	}

	// Gizmo - because of gravgun, don't shock people when we're moving
	if (other->takedamage && !(self->tf_items & PR_NIT_TURRET) && self->velocity == V({0, 0, 0})) // OfN - fixme: doesn't check for enemy disguised spies
	{
		deathmsg = PR_DMSG_BUG_ZAPPER;

		// Gizmo - allow non demons and non army soldiers to be affected (coop monsters)
		if (IsMonster(other))
		{
			float temp;

			//temp = GetTeam( other );
			if (!Teammate(other, self->real_owner))
			{
				TF_T_Damage (other, self, self, 400, PR_TF_TD_NOTTEAM, PR_TF_TD_ELECTRICITY);
				return;
			}

			return;
		}
		if (!Teammate(other, self->real_owner) && other->is_undercover != 1) // <== FIXME
			//TF_T_Damage(other, self, self.real_owner, 400, #TF_TD_NOTTEAM, #TF_TD_ELECTRICITY);
			TF_T_Damage(other, self, self, 400, PR_TF_TD_NOTTEAM, PR_TF_TD_ELECTRICITY);
		return;
	}
	//I'm in midflight and hit something
	if (self->tf_items & PR_NIT_TURRET && self->movetype == PR_MOVETYPE_FLY)
	{
		//WK Check to see if we are blocked
		//WK These numbers seem to be about right
		if ( BadSpot(self->origin + V({0, 0, 20})) || BadSpot(self->origin + V({0, 0, 22})))
		{
			sprint(self->real_owner, PR_PRINT_HIGH, "Your sentry gun flew away.\n");
			Sentry_Die();
			return;
		}
		if (other == world) { //The eagle has landed!
		//	sprint(self.real_owner, #PRINT_HIGH, "The eagle has landed!\n");
			self->flags = self->flags | PR_FL_ONGROUND;
			self->movetype = PR_MOVETYPE_STEP;
			self->origin[Z] = self->origin[Z] + 32;//32; // was 40, old custom bug? fixed
			return;
		}
		else if (other->classname == "player")
		{
			deathmsg = PR_DMSG_BUG_ZAPPER;
			//TF_T_Damage (other, self, self.real_owner, 400, #TF_TD_NOTTEAM, #TF_TD_ELECTRICITY);
			TF_T_Damage (other, self, self, 400, PR_TF_TD_NOTTEAM, PR_TF_TD_ELECTRICITY);

			self->velocity[Z] = 200; //- OfN reset velocity, not stop
		}
		else
		{
			deathmsg = PR_DMSG_BUG_ZAPPER;
			//TF_T_Damage(other, self, self.real_owner, 400, #TF_TD_NOTTEAM, #TF_TD_ELECTRICITY);
			if (!Teammate(other->real_owner, self->real_owner))
				TF_T_Damage(other, self, self, 400, PR_TF_TD_NOTTEAM, PR_TF_TD_ELECTRICITY);
			Sentry_Die();
		}
		//sprint(self.real_owner, #PRINT_HIGH, "The eagle has died.\n");
	}
}

} // END namespace Progs
