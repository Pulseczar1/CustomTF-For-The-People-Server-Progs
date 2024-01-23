/*======================================================
	ENGINEER.QC 		Custom TeamFortress v3.1

	(c) TeamFortress Software Pty Ltd	2/3/97
	(c) William Kerney			4/14/00
	(c) Craig Hauser				4/14/00
========================================================
Weapons and functions for the ENGINEER class and associated weaponry
=======================================================*/

#include "progs.h"
#include "ofndefs.h"
#include "debug.h"
#include "guerilla.h"
#include "weapons.h"
#include "items.h"
#include "demoman.h"
#include "custom.h"
#include "menu.h"
#include "gweapons.h"
#include "field.h"
#include "invade.h"
#include "security.h"
#include "cpstuff.h"
#include "tforttm.h"
#include "sentry.h"
#include "tesla.h"
#include "teleport.h"
#include "environ.h"
#include "sprites.h"
#include "neo.h"

namespace Progs {

//float modelindex_tesla; //CH
// Weapon Functions
void LaserBolt_Touch();
void LaserBolt_Think();
void W_FireLaser();

// EMP Grenade Functions
void EMPExplode();
void EMPGrenadeTouch();
void EMPGrenadeExplode();

// Building Functions
void TeamFortress_EngineerBuild();
void TeamFortress_Build(float objtobuild, int devicenum);
void TeamFortress_FinishedBuilding();
void T_Dispenser();
void Dispenser_Die();
void Engineer_UseDispenser(entity disp);
void Engineer_UseSentryGun(entity gun);
void Engineer_UseCamera(entity cam);
void CheckDistance();
float CheckArea(entity obj, entity builder);
//WK
void Sentry_Touch();
float HasFlag(float myteam);

//- OfN
void FieldGen_Built(entity fieldgen);
float EnoughToBuild(entity player, float buildtype);

//=========================================================================
// Laserbolt think function
void LaserBolt_Think()
{
	if (time > self->heat) {
		dremove(self);
		return;
	}
	self->solid = PR_SOLID_TRIGGER;
	self->movetype = PR_MOVETYPE_FLYMISSILE;
	self->velocity = self->oldorigin;
	self->touch = LaserBolt_Touch;
	setmodel(self, "progs/e_spike2.mdl");

	self->nextthink = time + 1.0;
	self->think = SUB_Remove;
}

//=========================================================================
// Laserbolt touch function. Just moves through the player and comes out
// the other side.
void LaserBolt_Touch()
{
	vector org;

	if (time > self->heat) {
		dremove(self);
		return;
	}

	if (other == self->owner)
		return;

	if (other == self->enemy && self->enemy != world)
		return; 	// don't explode on same person twice

	if (pointcontents(self->origin) == PR_CONTENT_SKY)
	{
		dremove(self);
		return;
	}

	//WK Sweep mines at point of impact
	GuerillaMineSweep(self->origin);

	org = self->origin - 8*normalize(self->velocity);

	if (other->health)
	{
		SpawnBlood (org, 15);
		deathmsg = PR_DMSG_LASERBOLT;
		TF_T_Damage (other, self, self->enemy, 25, 0, PR_TF_TD_ELECTRICITY);
		self->velocity = self->oldorigin;
		self->owner = other;

		setmodel (self, string_null);
		self->touch = SUB_Null;
//		self.solid = #SOLID_NOT;
//		self.movetype = #MOVETYPE_NOCLIP;

		self->nextthink = time + 0.1;
		self->think = LaserBolt_Think;
		return;
	}
	else
	{
		//WK Fly through walls!
		setmodel (self, string_null);
		self->touch = SUB_Null;
		self->solid = PR_SOLID_NOT;
		self->movetype = PR_MOVETYPE_NOCLIP;

		self->nextthink = time + 0.1;
		self->think = LaserBolt_Think;
		return;
		WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_BROADCAST, PR_TE_SPIKE);
		WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
	#ifdef PR_QUAKE_WORLD
		multicast (self->origin, PR_MULTICAST_PHS);
	#endif
	}

	dremove(self);
}

//=========================================================================
// Fire a laserbolt (actually, RAILGUN)
//WK 1/7/7 Note: This is the railgun, not the thunderbolt, nor the lasercannon.  PZ: Why the name, then?
void W_FireLaser()
{
	vector	vec, org;

	self->currentammo = self->ammo_nails = self->ammo_nails - 1;

	makevectors(self->v_angle);
	org = self->origin + (v_forward * 8);
	vec = aim(self, 10000);
	vec = normalize(vec);

	newmis = spawn();
	newmis->owner = self;
	newmis->enemy = self;	// The real owner
	newmis->movetype = PR_MOVETYPE_FLYMISSILE;
	newmis->solid = PR_SOLID_TRIGGER;

	setmodel (newmis, "progs/e_spike1.mdl");
	setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));

// PZ: TEST CODE: remove me later
// shoot from upper-left corner of bounding box

// DOESN'T WORK
/*org_x = self.origin_x + (v_right_x * self.mins_x);
org_y = self.origin_y + (v_right_y * self.maxs_y);
org_z = self.origin_z + self.maxs_z;*/

// THIS WORKS
// top left
/*org = self.origin + (v_right * self.mins_x);
org_z = self.origin_z + self.maxs_z;
// top right
org = self.origin + (v_right * self.maxs_x);
org_z = self.origin_z + self.maxs_z;*/


//org = self.origin + (v_right * (self.mins_x + 1));
/*org = self.origin + (v_right * (self.maxs_x - 1));
org_z = self.origin_z + self.maxs_z;


setorigin(newmis, org);*/
// END TEST CODE

	// PZ: ######## UNCOMMENT THIS WHEN DONE ########
	setorigin (newmis, org + V({0, 0, 16}));

	newmis->velocity = vec * 1500;
	newmis->angles = vectoangles(newmis->velocity);
	newmis->oldorigin = newmis->velocity;

	newmis->nextthink = time + 1.5;
	newmis->heat = time + 1.5;

	newmis->think = SUB_Remove;
	newmis->touch = LaserBolt_Touch;
}

//=========================================================================
// Ammo/Weapon exploded by the EMP grenade
void EMPExplode()
{
	float expsize;

	expsize = 10;
	// Weapon?
	if (self->touch == weapon_touch)
		expsize = 60;
	else if (self->classname == "item_shells")
		expsize = 50 + self->aflag;
	else if (self->classname == "item_spikes")
		expsize = 40;
	else if (self->classname == "item_rockets")
		expsize = 100 + (self->aflag * 4);
	else if (self->classname == "item_cells")
		expsize = 100 + (self->aflag * 3);
	else if (self->classname == "item_weapon")
		expsize = 60;
	else
	{
		RPrint("EMPExplode: Attempting to explode a ");
		RPrint(self->classname);
		RPrint("\n");
		return;
	}

	deathmsg = PR_DMSG_GREN_EMP_AMMO;
	T_RadiusDamage (self, self->enemy, expsize, world);

	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
	WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
#ifdef PR_QUAKE_WORLD
	multicast (self->origin, PR_MULTICAST_PHS);
#endif

	// Respawn
	Respawn_Item(self, self->enemy);
}

//=========================================================================
// Touch Function for EMP Grenade
void EMPGrenadeTouch()
{
	// If the EMP Grenade hits a player, it just bounces off
	sound (self, PR_CHAN_WEAPON, "weapons/bounce.wav", 1, PR_ATTN_NORM);
	if (self->velocity == V({0, 0, 0}))
		self->avelocity = V({0, 0, 0});
}

//=========================================================================
// EMP Grenade explode function, for when the PRIMETIME runs out
void EMPGrenadeExplode()
{
	float expsize;
	entity te, oldself;
	float total_exp;
//CH Slice gave idea of an emp gren getting rated based on blast so i added..
	total_exp = 0;

	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_TAREXPLOSION);
	WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
#ifdef PR_QUAKE_WORLD
	multicast (self->origin, PR_MULTICAST_PHS);
#endif

	string desc;
	desc = "grenade";

	if (self->netname == "special_mine")
		desc = "mine";

	// Find all ammo in the area
	te = findradius(self->origin, 240);
	while (te != world)
	{
		if (!CanDamage(te,self)) // OfN
		{
			#ifdef PR_EMP_DISCHARGE // TODO: Make this work when enabled
			if (te->classname == "building_sentrygun" || te->classname == "building_tesla")
			{ // Sentries and teslas
				if (te->health > 0)
				{
					if (te->classname == "building_tesla")
						sprint(self->owner,PR_PRINT_HIGH,"Your psionic ",desc," has disabled ",te->real_owner->netname,"'s tesla!\n");
					else
						sprint(self->owner,PR_PRINT_HIGH,"Your psionic ",desc," has disabled ",te->real_owner->netname,"'s sentry gun!\n");

					sound(te, PR_CHAN_MISC, "effects/crunch.wav",1,PR_ATTN_NORM);

					SpawnSprite(1,PR_SPRITE_ABLAST,te->origin,V({0, 0, 0}),PR_SPRITEMOVE_UP,0.1);

					if (te->tfstate & PR_TFSTATE_PSIONIZED) // Already affected
					{
						// find the timer for psionics
						entity psion;

						psion = find(world,"netname","psionictimer");

						while (psion != world)
						{
							if (psion->owner == te)
							{
								// reset time
								ttime = psionicstime;

								// ICE'd stuff lowers the time of effect
								if (te->all_active & PR_IMPROVED_SIX) // ICE3
									ttime = psionicstime - (psionicstime*0.6);
								else if (te->all_active & PR_IMPROVED_FIVE) // ICE2
									ttime = psionicstime - (psionicstime*0.4);
								else if (te->all_active & PR_IMPROVED_TWO) // ICE1
									ttime = psionicstime - (psionicstime*0.2);

								psion->nextthink = time + ttime;
							}

							psion = find(psion,"netname","psionictimer");
						}

						if (te->classname == "building_tesla")
							sprint(te->real_owner,PR_PRINT_HIGH,"Your tesla accepts another psionic discharge\n");
						else
							sprint(te->real_owner,PR_PRINT_HIGH,"Your sentry gun accepts another psionic discharge\n");

					}
					else // not affected yet
					{
						// create psionic timer
						newmis = spawnServerSide(); // PZ: make it a server-side only entity

						newmis->classname = "timer";
						newmis->netname = "psionictimer";
						newmis->team_no = self->owner->team_no;
						newmis->owner = te;
						newmis->think = PsionicTimer;

						ttime = psionicstime;

						// ICE'd stuff lowers the time of effect
						if (te->all_active & PR_IMPROVED_SIX) // ICE3
							ttime = psionicstime - (psionicstime*0.6);
						else if (te->all_active & PR_IMPROVED_FIVE) // ICE2
							ttime = psionicstime - (psionicstime*0.4);
						else if (te->all_active & PR_IMPROVED_TWO) // ICE1
							ttime = psionicstime - (psionicstime*0.2);

						newmis->nextthink = time + ttime;

						te->tfstate = te->tfstate | PR_TFSTATE_PSIONIZED;

						if (te->classname == "building_tesla")
							sprint(te->real_owner,PR_PRINT_HIGH,"Your tesla receives a psionic discharge!\n");
						else
							sprint(te->real_owner,PR_PRINT_HIGH,"Your sentry gun receives a psionic discharge!\n");
					}
				}
			}
			else if (te->classname == "building_fieldgen") // psionics disable the fieldgens for a while
			{
				if (te->health > 0)
				if (te->martyr_enemy != world)
				{
					// ice effect reduction
					float icelevel, disabletime, tmp;

					icelevel = FieldIsImproved(te->martyr_enemy);

					if (icelevel & PR_IMPROVED_SIX)
						icelevel = 3;
					else if (icelevel & PR_IMPROVED_FIVE)
						icelevel = 2;
					else if (icelevel & PR_IMPROVED_TWO)
						icelevel = 1;
					else icelevel = 0;

					tmp = 2.5 + random() * 2.5;

					disabletime = tmp - ((icelevel / 3) * tmp * 0.5);

					DisableField(te->martyr_enemy, disabletime);

					SpawnSprite(1,PR_SPRITE_ABLAST,te->origin,V({0, 0, 0}),PR_SPRITEMOVE_UP,0.1);

					sound(te, PR_CHAN_MISC, "effects/crunch.wav",1,PR_ATTN_NORM);

					sprint(self->owner,PR_PRINT_HIGH,"your psionic ",desc," disabled the field generator!\n");

					sprint(te->real_owner,PR_PRINT_HIGH,"your field generator has received a psionic discharge!\n");
				}
			}
			else if (te->classname == "force_field") // psionics disable the fieldgens of a field for a shorter time
			{
				// ice handling
				float icelevel, disabletime, tmp;

				icelevel = FieldIsImproved(te);

				if (icelevel & PR_IMPROVED_SIX)
					icelevel = 3;
				else if (icelevel & PR_IMPROVED_FIVE)
					icelevel = 2;
				else if (icelevel & PR_IMPROVED_TWO)
					icelevel = 1;
				else icelevel = 0;

				tmp = 2 + random() * 2;

				disabletime = tmp - ((icelevel / 3) * tmp * 0.5);

				DisableField(te, disabletime);

				SpawnSprite(2,PR_SPRITE_ABLAST,te->origin,V({0, 0, 0}),PR_SPRITEMOVE_UP,0.2);

				sprint(self->owner,PR_PRINT_HIGH,"your psionic ",desc," disabled the force field!\n");

				sprint(te->real_owner,PR_PRINT_HIGH,"your force field has been shuted down by psionics!\n");
			}
			else
			#endif
				SUB_Null();
		}
		else if (te->touch == ammo_touch || te->touch == weapon_touch) // Ammo/Weapon?
		{
			// Make sure it isn't picked up in the next second
			te->solid = PR_SOLID_NOT;
			te->enemy = self->owner;
			te->nextthink = time + 1 + (random() * 2);
			te->think = EMPExplode;
		}
		// Detpack?
		else if (te->think == TeamFortress_DetpackExplode)
		{
			//te.solid = #SOLID_NOT;
			te->nextthink = time + 225 * random() + 30; //WK Scramble the detpack's timer. :)
			dremove(te->oldenemy); // Countdown ent

			sprint(self->owner,PR_PRINT_HIGH,"Your EMP ",desc," damages the detpack circuits\n");
			sprint(te->owner,PR_PRINT_HIGH,"Your detpack circuits have been damaged!\n");
		}
		// Pipebomb?
		else if (te->classname == "pipebomb")
		{
			te->nextthink = time + 0.1 + random();
		}
		// Mine?
		else if (te->classname == "grenade" && te->netname == "land_mine")
		{
			te->think = GuerillaExplode;
			te->nextthink = time + 0.1;
		}
		// Building?
		else if (IsBuilding(te)) {
			total_exp = total_exp + 120;
			TF_T_Damage(te,self,self->owner, 120, 0, PR_TF_TD_OTHER);
		}
		// Ammobox?
		else if (te->classname == "ammobox")
		{
			expsize = 0;
			expsize = expsize + (te->ammo_shells * 0.75);
			expsize = expsize + ((te->ammo_rockets * 0.75) * 2);
			expsize = expsize + ((te->ammo_cells * 0.75) * 2);

			if (expsize > 0)
			{
				te->solid = PR_SOLID_NOT;

				// Damage player and explode
				deathmsg = PR_DMSG_GREN_EMP;
				total_exp = total_exp + expsize;
				if (expsize > 300) //CH so they are not too powerfull //WK was 500
					expsize = 300;
				T_RadiusDamage (te, self->owner, expsize, te);

				te->think = SUB_Remove;
				te->nextthink = time + 0.1;

				WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
				WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
				WriteCoord (PR_MSG_BROADCAST, te->origin[X]);
				WriteCoord (PR_MSG_BROADCAST, te->origin[Y]);
				WriteCoord (PR_MSG_BROADCAST, te->origin[Z]);
			#ifdef PR_QUAKE_WORLD
				multicast (te->origin, PR_MULTICAST_PHS);
			#endif
			}
		}
		// Backpack/Player?
#ifdef PR_COOP_MODE_ENHANCED
		else if ((te->classname == "player" && te->health > 0) || (te->classname=="monster_army") || (te->classname=="monster_gremlin") || (te->touch == BackpackTouch) || COOP_IsCoopMonster( te ))
#else
		else if ((te->classname == "player" && te->health > 0) || (te->classname=="monster_army") || (te->classname=="monster_gremlin") || (te->touch == BackpackTouch))
#endif
		{
			expsize = 0;
			// calculate explosion size
			expsize = expsize + (te->ammo_shells * 0.75);
			expsize = expsize + ((te->ammo_rockets * 0.75) * 2);
			if (!(te->weapons_carried & PR_WEAP_SPANNER || te->touch == BackpackTouch))
				expsize = expsize + (te->ammo_cells * 0.75);

			if (expsize > 0)
			{
				// Damage player and explode
				deathmsg = PR_DMSG_GREN_EMP;
				total_exp = total_exp + expsize;
				//WK Make players not explode radially!
				if (te->touch == BackpackTouch)
					T_RadiusDamage (te, self->owner, expsize / 2, te); //WK Dampen backpack damage
				if (te->touch != BackpackTouch)
				{
					TF_T_Damage (te, self, self->owner, expsize, 0, PR_TF_TD_EXPLOSION);

					// Remove ammo
					te->ammo_shells = ceil(te->ammo_shells * 0.25);
					te->ammo_rockets = ceil(te->ammo_rockets * 0.25);
					if (!(te->cutf_items & PR_CUTF_SENTRYGUN || te->tf_items & PR_NIT_TESLA))
						te->ammo_cells = ceil(te->ammo_cells * 0.25);

					if (te->classname=="player")
					{
						// Update console
						oldself = self;
						self = te;
						W_SetCurrentAmmo();
						self = oldself;

						WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
						WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
						WriteCoord (PR_MSG_BROADCAST, te->origin[X]);
						WriteCoord (PR_MSG_BROADCAST, te->origin[Y]);
						WriteCoord (PR_MSG_BROADCAST, te->origin[Z]);
					#ifdef PR_QUAKE_WORLD
						multicast (te->origin, PR_MULTICAST_PHS);
					#endif
					}
				}
				else
				{
					te->think = SUB_Remove;
					te->nextthink = time + 0.1;
				}

				WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
				WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
				WriteCoord (PR_MSG_BROADCAST, te->origin[X]);
				WriteCoord (PR_MSG_BROADCAST, te->origin[Y]);
				WriteCoord (PR_MSG_BROADCAST, te->origin[Z]);
			#ifdef PR_QUAKE_WORLD
				multicast (te->origin, PR_MULTICAST_PHS);
			#endif
			}
		}

		te = te->chain;
	}

	sound (self, PR_CHAN_WEAPON, "weapons/gren_emp.wav", 1, PR_ATTN_NORM); //- OfN - cool sound! :)

if (total_exp > 0) {//CH Thanks Slice for the idea :)

	sprint(self->owner,PR_PRINT_HIGH, "Your EMP ",desc," inflicted ");
	string st;
	st = ftos(total_exp);
	sprint(self->owner,PR_PRINT_HIGH, st);
	sprint(self->owner,PR_PRINT_HIGH, " damage\n");
}

#ifdef PR_DEMO_STUFF
	// Remove any camera's locks on this missile
	if (self->enemy != world)
		CamProjectileLockOff();
#endif

#ifdef PR_QUAKE_WORLD
	dremove(self);
#else
	BecomeExplosion();
#endif
}


//=========================================================================
// Tests whether a team is allowed to build or not

float HasFlag(float myteam)
{

	if ( mapname != "steal4d" )
		return PR_TRUE;

	if (myteam == team_with_flag){
		return PR_TRUE;
		}
	return PR_FALSE;
}

//=========================================================================
// Function handling the Engineer's build impulse
void TeamFortress_EngineerBuild()
{
	entity te;

	// Can't build in the air
	// WK Yes you can, why not? You can do it by pulling up this menu anyway
	/*
	if (!(self.flags & #FL_ONGROUND))
	{
		CenterPrint(self, "You can't build in the air!\n\n");
		return;
	}
	*/



	if (self->is_detpacking == 1) {
		CenterPrint(self, "You can't build while detpacking\n");
		return;
	}

	if (self->is_feigning == 1) {
		CenterPrint(self, "You can't build while feigning\n");
		return;
	}

	// Pop up the menu
	if (self->is_building == 0)
	{
		// Check to see if they've got enuf metal to build anything
		if (!(invade && invade_gameState == InvadeGameState::IN_PREMATCH && self->team_no == invade_teamOnDef))
		if (self->ammo_cells < PR_BUILD_COST_CAMERA && self->has_dispenser == PR_FALSE && self->has_sentry == PR_FALSE && self->has_tesla == PR_FALSE && self->has_camera == PR_FALSE && self->has_teleporter == PR_FALSE && self->has_sensor == PR_FALSE && self->has_fieldgen == PR_FALSE)
		{
			CenterPrint(self, "You don't have enough metal to \nbuild anything.\n\n");
			return;
		}

		self->current_menu = PR_MENU_ENGINEER;
		self->menu_count = PR_MENU_REFRESH_RATE;
	}
	else if (self->is_building == 1)
	{
		sprint(self, PR_PRINT_HIGH, "You stop building.\n");

		self->is_building = 0;
		self->tfstate = self->tfstate - (self->tfstate & PR_TFSTATE_CANT_MOVE);

#ifdef PR_QUAKE_WORLD
		TeamFortress_SetSpeed(self);
#else
		self->pausetime = time;
#endif

		// Remove the timer
		te = find(world, "netname", "build_timer");
		while (te != world)
		{
			if (te->owner == self)
			{
				dremove(te);
				te = world;
			}
			else
			{
				te = find(te, "netname", "build_timer");
			}
		}

		self->current_weapon = self->weapon;
		W_SetCurrentAmmo();
	}
}


float CheckArea(entity obj, entity builder)
{
/*
	local vector src, end;
	local float pos;

	// Check the origin
	pos = pointcontents(obj.origin);
	if (pos == #CONTENT_SOLID || pos == #CONTENT_SKY)
		return #FALSE;

	// Check the surrounding area
	src_x = obj.origin_x + obj.maxs_x + 16;
	src_y = obj.origin_y + obj.maxs_y + 16;
	src_z = obj.origin_z + obj.maxs_z + 16;  // check upwards more

	pos = pointcontents(src);
	if (pos == #CONTENT_SOLID || pos == #CONTENT_SKY)
		return #FALSE;

	end_x = obj.origin_x + obj.mins_x - 16;
	end_y = obj.origin_y + obj.mins_y - 16;
	end_z = obj.origin_z + obj.mins_z - 16;
	traceline (src, end, #TL_BSP_ONLY, obj);
	if (trace_fraction != 1)
		return #FALSE;

	pos = pointcontents(end);
	if (pos == #CONTENT_SOLID || pos == #CONTENT_SKY)
		return #FALSE;

	// extend the size a little
	src_x = obj.origin_x + obj.mins_x - 16;
	src_y = obj.origin_y + obj.maxs_y + 16;
	src_z = obj.origin_z + obj.maxs_z + 16;

	pos = pointcontents(src);
	if (pos == #CONTENT_SOLID || pos == #CONTENT_SKY)
		return #FALSE;

	end_x = obj.origin_x + obj.maxs_x + 16;
	end_y = obj.origin_y + obj.mins_y - 16;
	end_z = obj.origin_z + obj.mins_z - 16; 	// check downwards less

	traceline (src, end, #TL_BSP_ONLY, obj);

	if (trace_fraction != 1)
		return #FALSE;

	pos = pointcontents(end);
	if (pos == #CONTENT_SOLID || pos == #CONTENT_SKY)
		return #FALSE;
*/

	if (InSolid(obj))
		return PR_FALSE;

	// Trace a line from the player to the object too
	traceline(builder->origin, obj->origin, PR_TL_BSP_ONLY, builder);

	if (trace_fraction != 1)
		return PR_FALSE;

	// may add in more checks later

	return PR_TRUE;
}

//////////////////////////////////////////////////////////////////////////
// rehashed version of TF_Build
void TeamFortress_Build(float objtobuild, int devicenum)
{
	// OfN - the following is needed because we can build via menus or by cmnd
	if (self->is_building == 1)
	{
		sprint(self, PR_PRINT_HIGH, "You are already building something!\n");
		return;
	}
	if (intermission_running || ceasefire)
		return;
	if (self->flags & PR_FL_FINALIZED)
		return;
	if (self->health <= 0 || self->deadflag != PR_DEAD_NO)
		return;
	if (prematch >= time)
		return;
	if (self->done_custom & PR_CUSTOM_BUILDING) // no ppl customizing
		return;
	if (self->playerclass == PR_PC_UNDEFINED) // no observers
		return;
	if (self->penance_time >= time) // Don't let lame teamkillers build anything
		return;

	if (!EnoughToBuild(self,objtobuild))
	{
		sprint(self, PR_PRINT_HIGH, "You don't have enough metal to build that!\n");
		return;
	}

	// OfN - Psionized guys can't build
	if (self->tfstate & PR_TFSTATE_PSIONIZED)
	{
		sprint(self, PR_PRINT_HIGH, "You are not able to build while psionized!\n");
		return;
	}

	//WK Cheat Fix
	if (self->is_feigning) {
			sprint(self, PR_PRINT_HIGH, "Try standing up first!\n");
			return;
	}

	//OfN - Cheat Fix, heh
	if (self->is_haxxxoring) {
			sprint(self, PR_PRINT_HIGH, "You can't build while hacking!\n");
			return;
	}

	if (self->is_detpacking || self->is_toffingadet)
	{
		sprint (self, PR_PRINT_HIGH, "You can't build while manipulating a detpack!\n");
		return;
	}

	float btime;
	//local entity te;
	vector tmp1, tmp2;

	newmis = spawn();
	newmis->flags = newmis->flags | PR_FL_MACHINE; // PZ: mark it with the FL_MACHINE flag

	// get an origin
	makevectors(self->v_angle);
	v_forward[Z] = 0;
	v_forward = normalize(v_forward) * 64;

	if (objtobuild == PR_BUILD_FIELDGEN) //- OfN - Field generators must be lined up to work
		newmis->origin = WhereGen(self->origin + v_forward);
	else
		newmis->origin = self->origin + v_forward;

	if (objtobuild == PR_BUILD_DISPENSER)
	{
		if (self->has_dispenser)
		{
			sprint(self, PR_PRINT_HIGH, "You can only have one dispenser.\nTry dismantling your old one.\n");
			dremove(newmis);
			return;
		}
		if (!(self->cutf_items & PR_CUTF_DISPENSER))
		{
			sprint(self, PR_PRINT_HIGH, "You must buy the dispenser before you can build it.\n");
			dremove(newmis);
			return;
		}
		// Gizmo - removed the dispenser jump crap
		tmp1 = V({-8, -8, 0});
		tmp2 = V({8, 8, 24});
//		tmp1 = '-16 -16 0';
//		tmp2 = '16 16 48';

		newmis->mdl = "progs/disp.mdl";

		newmis->netname = "dispenser";

		if (Invade_buffPlayer(self))
			btime = time + PR_BUILD_TIME_INVADE_PM;
		else
			btime = time + PR_BUILD_TIME_DISPENSER;
	}
	else if (objtobuild == PR_BUILD_SENTRYGUN)
	{
		if (!(self->cutf_items & PR_CUTF_SENTRYGUN))
		{
			sprint(self, PR_PRINT_HIGH, "You must buy a sentry gun before you can build one.\n");
			dremove(newmis);
			return;
		}

		if (self->cutf_items & PR_CUTF_DOUBLESENTRY)
		{
			if (self->has_sentry & devicenum)
			{
				sprint(self, PR_PRINT_HIGH, "You already have that sentry gun up!\n");
				dremove(newmis);
				return;
			}
		}
		else
		{
			if (self->has_sentry)
			{
				sprint(self, PR_PRINT_HIGH, "You can only have one sentry gun.\nTry dismantling your old one.\n");
				dremove(newmis);
				return;
			}
		}

		newmis->dont_do_triggerwork = devicenum;

		tmp1 = V({-16, -16, 0});
		tmp2 = V({16, 16, 48});
		//newmis.mdl = "progs/turrbase.mdl";
		newmis->mdl = "progs/trrbs2.mdl";
		newmis->netname = "sentrygun";

		if (Invade_buffPlayer(self))
			btime = time + PR_BUILD_TIME_INVADE_PM;
		else
			btime = time + PR_BUILD_TIME_SENTRYGUN;
	}
	else if (objtobuild == PR_BUILD_TESLA)
	{
		if (!(self->tf_items & PR_NIT_TESLA))
		{
			sprint(self, PR_PRINT_HIGH, "You must buy a tesla sentry before you can build one.\n");
			dremove(newmis);
			return;
		}

		if (self->tf_items & PR_NIT_DOUBLETESLA)
		{
			if (self->has_tesla & devicenum)
			{
				sprint(self, PR_PRINT_HIGH, "You already have that tesla coil up!\n");
				dremove(newmis);
				return;
			}
		}
		else
		{
			if (self->has_tesla)
			{
				sprint(self, PR_PRINT_HIGH, "You can only have one tesla gun.\nTry dismantling your old one.\n");
				dremove(newmis);
				return;
			}
		}

		newmis->dont_do_triggerwork = devicenum;

		//tmp1 = '-16 -16 0'; //- OfN commented by
		tmp1 = V({-16, -16, -25});
		//tmp2 = '16 16 48'; //WK 62 is better, but crashes?
		tmp2 = V({16, 16, 23});
		//newmis.mdl = "progs/newtesla.mdl";
		newmis->mdl = "progs/coil.mdl";
		newmis->netname = "tesla";
		newmis->origin = newmis->origin + V({0, 0, 25});

		if (Invade_buffPlayer(self))
			btime = time + PR_BUILD_TIME_INVADE_PM;
		else
			btime = time + PR_BUILD_TIME_TESLA;
	}
	else if (objtobuild == PR_BUILD_SECURITY_CAMERA)
	{
		dremove(newmis);

		if (self->has_camera)
		{
			sprint(self, PR_PRINT_HIGH, "You can only have one security camera.\nTry dismantling your old one.\n");
			return;
		}
		if (!(self->tf_items & PR_NIT_SECURITY_CAMERA))
		{
			sprint(self, PR_PRINT_HIGH, "You must buy a security camera before you can build one.\n");
			return;
		}

		Security_Camera_Spawn();
		return;
	}
	else if (objtobuild == PR_BUILD_TELEPORTER)
	{
		if (self->has_teleporter >= 2)
		{
			sprint(self, PR_PRINT_HIGH, "You can only have 2 teleporters.\nTry dismantling an old one.\n");
			dremove(newmis);
			return;
		}
		if (!(self->tf_items & PR_NIT_TELEPORTER))
		{
			sprint(self, PR_PRINT_HIGH, "You must buy a teleporter before you can build one.\n");
			dremove(newmis);
			return;
		}

		tmp1 = V({-16, -16, 0});
		tmp2 = V({16, 16, 4});
		newmis->mdl = "progs/telepad.mdl";
		newmis->netname = "teleporter";

		if (Invade_buffPlayer(self))
			btime = time + PR_BUILD_TIME_INVADE_PM;
		else
			btime = time + PR_BUILD_TIME_TELEPORTER;
	}
	else if (objtobuild == PR_BUILD_FIELDGEN)
	{
		if (self->has_fieldgen >= 2)
		{
			sprint(self, PR_PRINT_HIGH, "You already have 2 field generators.\nTry dismantling an old one.\n");
			dremove(newmis);
			return;
		}
		if (!(self->cutf_items & PR_CUTF_FIELDGEN))
		{
			sprint(self, PR_PRINT_HIGH, "You must buy a field generator before you can build one.\n");
			dremove(newmis);
			return;
		}

		tmp1 = V({-6, -6, 0});
		tmp2 = V({6, 6, 54});
		newmis->mdl = "progs/ffgen2.mdl";
		newmis->netname = "field generator";

		if (Invade_buffPlayer(self))
			btime = time + PR_BUILD_TIME_INVADE_PM;
		else
			btime = time + PR_BUILD_TIME_FIELDGEN;
	} else {
		error ("TeamFortress_Build: unknown object to build");

		// Gizmo - just to get rid of qfcc warnings
		btime = 0;
		tmp1 = tmp2 = V({0, 0, 0});
	}

	// before we start building it, check it out
	// check for validity of point
	setmodel (newmis, newmis->mdl);
	setsize (newmis, tmp1, tmp2);
	setorigin (newmis, newmis->origin);
	if (CheckArea(newmis, self) == PR_FALSE)
	{
		sprint(self, PR_PRINT_HIGH, "Not enough room to build here\n");
		dremove(newmis);
		return;
	}

	self->is_building = 1;
	makeImmune(self,time + 2);
	//self.immune_to_check = time + 2;
	self->tfstate = self->tfstate | PR_TFSTATE_CANT_MOVE;

	// Save the current weapon and remove it
	self->weapon = self->current_weapon;
	self->current_weapon = 0;
	self->weaponmodel = "";
	self->weaponframe = 0;

#ifdef PR_QUAKE_WORLD
	TeamFortress_SetSpeed(self);
#else
	self->pausetime = btime;
#endif

	newmis->owner = self;
	newmis->classname = "timer";
	newmis->netname = "build_timer";
	newmis->nextthink = btime;
	newmis->think = TeamFortress_FinishedBuilding;
	newmis->colormap = self->colormap;
	newmis->weapon = objtobuild;
	newmis->angles[Y] = anglemod(self->angles[Y] + 180);

	newmis->velocity = V({0, 0, 8});
	newmis->movetype = PR_MOVETYPE_TOSS;

	newmis->solid = PR_SOLID_BBOX;

	if (objtobuild==PR_BUILD_TESLA)
	{
		if (neo.isModeActive()) // for Neo mode
		{
			if (neo.getNeo() != newmis->owner)
				newmis->skin = 3;
			else
				newmis->skin = 1;
		}
		else
		{
			newmis->skin = self->team_no;
			if (self->team_no==3) newmis->skin=0;
			else if (self->team_no==4) newmis->skin=3;
		}
	}

	newmis->flags = newmis->flags - (newmis->flags & PR_FL_ONGROUND);
}

void DispenserThink()
{
	float iI; // is Improved?

	iI=1;

	if (self->all_active & PR_IMPROVED_ONE)
		iI=2;


	// dispenser refilling itself 5%
	if (!(self->is_malfunctioning & PR_SCREWUP_THREE)) // SB
	{
		self->ammo_shells = self->ammo_shells + rint((PR_BUILD_DISPENSER_MAX_SHELLS*iI) / 10);
		self->ammo_cells = self->ammo_cells + rint((PR_BUILD_DISPENSER_MAX_CELLS*iI) / 10);
		self->ammo_nails = self->ammo_nails + rint((PR_BUILD_DISPENSER_MAX_NAILS*iI) / 10);
		self->ammo_rockets = self->ammo_rockets + rint((PR_BUILD_DISPENSER_MAX_ROCKETS*iI) / 10);
		self->armorvalue = self->armorvalue + rint((PR_BUILD_DISPENSER_MAX_ARMOR*iI) / 10);

		if (self->ammo_shells > PR_BUILD_DISPENSER_MAX_SHELLS*iI)
			self->ammo_shells = PR_BUILD_DISPENSER_MAX_SHELLS*iI;
		if (self->ammo_nails > PR_BUILD_DISPENSER_MAX_NAILS*iI)
			self->ammo_nails = PR_BUILD_DISPENSER_MAX_NAILS*iI;
		if (self->ammo_rockets > PR_BUILD_DISPENSER_MAX_ROCKETS*iI)
			self->ammo_rockets = PR_BUILD_DISPENSER_MAX_ROCKETS*iI;
		if (self->ammo_cells > PR_BUILD_DISPENSER_MAX_CELLS*iI)
			self->ammo_cells = PR_BUILD_DISPENSER_MAX_CELLS*iI;
		if (self->armorvalue > PR_BUILD_DISPENSER_MAX_ARMOR*iI)
			self->armorvalue = PR_BUILD_DISPENSER_MAX_ARMOR*iI;

		if (self->all_active & PR_IMPROVED_FOUR) // Upgraded dispenser
		{
			self->ammo_detpack = self->ammo_detpack + (PR_BUILD_DISPENSER_MAX_DETS*iI) /10;
			self->ammo_c4det = self->ammo_c4det + (PR_BUILD_DISPENSER_MAX_DETS*iI) /10;
			self->ammo_medikit = self->ammo_medikit + rint((PR_BUILD_DISPENSER_MAX_MEDIKIT*iI) /10);

			if (self->ammo_detpack > PR_BUILD_DISPENSER_MAX_DETS*iI)
				self->ammo_detpack = PR_BUILD_DISPENSER_MAX_DETS*iI;
			if (self->ammo_c4det > PR_BUILD_DISPENSER_MAX_DETS*iI)
				self->ammo_c4det = PR_BUILD_DISPENSER_MAX_DETS*iI;
			if (self->ammo_medikit > PR_BUILD_DISPENSER_MAX_MEDIKIT*iI)
				self->ammo_medikit = PR_BUILD_DISPENSER_MAX_MEDIKIT*iI;
		}
	}
	self->nextthink = time + 10;
}

// PZ NOTE: `self` is changed from the timer, or initial building, to the player that built it, at the beginning of the function.
void TeamFortress_FinishedBuilding()
{
	entity oldself;
//	local float current_yaw;
//	local vector source;

	if (self->owner->is_building != 1)
		return;

	oldself = self;
	self = self->owner;
	oldself->owner = world;
	oldself->real_owner = self;

	self->is_building = 0;
	self->tfstate = self->tfstate - (self->tfstate & PR_TFSTATE_CANT_MOVE);
	self->current_weapon = self->weapon;
	self->StatusRefreshTime = time + 0.1;

	TeamFortress_SetSpeed(self);

	if (oldself->weapon == PR_BUILD_DISPENSER)
	{
		self->has_dispenser = PR_TRUE;
		sprint (self, PR_PRINT_HIGH, "You finish building the dispenser.\n");

		teamprefixsprint(self->team_no,self); //- OfN

		teamsprint(self->team_no, self, self->netname);
		teamsprint(self->team_no, self, " has built a Dispenser.\n");

		if (!Invade_buffPlayer(self))
			self->ammo_cells = self->ammo_cells - PR_BUILD_COST_DISPENSER;

		// Create the dispenser
		oldself->classname = "building_dispenser";
		oldself->netname = "dispenser";
		oldself->blocked = T_Dispenser;		// Actual touch function
		oldself->touch = T_Dispenser;
		oldself->max_health = PR_BUILD_HEALTH_DISPENSER;
		oldself->health = PR_BUILD_HEALTH_DISPENSER;
		oldself->think = DispenserThink;
		oldself->nextthink = time + 5;
		oldself->th_die = Dispenser_Die; 	// Death function

		oldself->mdl = "progs/disp.mdl"; 	// Actual mdl

		oldself->team_no = self->team_no;

		oldself->all_active=0; // OfN - reset HACKER improvements
		oldself->is_malfunctioning = 0;

		oldself->real_owner = self;			// The Engineer owns this item
		oldself->colormap = self->colormap;	// Set the Color
		oldself->takedamage = PR_DAMAGE_AIM;
		oldself->owner = world;
		oldself->movetype = PR_MOVETYPE_TOSS;
		oldself->velocity = V({0, 0, 8});
		oldself->flags = oldself->flags - (oldself->flags & PR_FL_ONGROUND);

		// Put some ammo in the Dispenser
		oldself->ammo_shells = ceil(self->ammo_shells * 0.25);
		oldself->ammo_nails = ceil(self->ammo_nails * 0.25);
		oldself->ammo_rockets = ceil(self->ammo_rockets * 0.25);
		oldself->ammo_cells = ceil(self->ammo_cells * 0.25);
		oldself->armorvalue = ceil(self->armorvalue * 0.25);

		oldself->ammo_medikit = ceil(self->ammo_medikit * 0.25);

		oldself->ammo_detpack = ceil(self->ammo_detpack * 0.25);
		oldself->ammo_c4det = ceil(self->ammo_c4det * 0.25);

		// Remove ours

		if (!Invade_buffPlayer(self))
		{
			self->ammo_shells = ceil(self->ammo_shells * 0.75);
			self->ammo_nails = ceil(self->ammo_nails * 0.75);
			self->ammo_rockets = ceil(self->ammo_rockets * 0.75);
			self->ammo_cells = ceil(self->ammo_cells * 0.75);
			self->armorvalue = ceil(self->armorvalue * 0.75);

			self->ammo_detpack = ceil(self->ammo_detpack * 0.75);
			self->ammo_c4det = ceil(self->ammo_c4det * 0.75);

			self->ammo_medikit = ceil(self->ammo_medikit * 0.75);
		}

		oldself->solid = PR_SOLID_BBOX;
		setmodel(oldself, oldself->mdl);
//		setsize (oldself, '-8 -8 0', '8 8 24');
	}
	else if (oldself->weapon == PR_BUILD_SENTRYGUN)
	{
		//CH special sbar for eng.
		self->StatusBarScreen = 1;
		//self.has_sentry = #TRUE;
		self->has_sentry = self->has_sentry | oldself->dont_do_triggerwork;
		sprint (self, PR_PRINT_HIGH, "You finish building the sentry gun.\n");

		teamprefixsprint(self->team_no,self); //- OfN

		teamsprint(self->team_no, self, self->netname);
		teamsprint(self->team_no, self, " has built a Sentry Gun.\n");

		oldself->classname = "building_sentrygun_base";
		oldself->netname = "sentry gun";
		oldself->takedamage = 0;
		oldself->th_die = Sentry_Die;		// Death function
		if (!Invade_buffPlayer(self))
			self->ammo_cells = self->ammo_cells - PR_BUILD_COST_SENTRYGUN;

		setsize (oldself, V({-16, -16, 0}), V({16, 16, 4})); // '-16 -16 0' '16 16 4'
		// PZ NOTE: now spawn the top/turret
		newmis = spawn();
		newmis->dont_do_triggerwork = oldself->dont_do_triggerwork;
		newmis->classname = "building_sentrygun";
		newmis->flags = newmis->flags | PR_FL_MACHINE; // PZ: mark it with the FL_MACHINE flag
		newmis->health = PR_BUILD_HEALTH_SENTRYGUN;
		newmis->max_health = newmis->health;
		newmis->tf_items = PR_NIT_KEVLAR; //Start with kevlar armor
		newmis->armorclass = PR_AT_SAVESHOT; //kevlar armor
		newmis->weapon = 1;					// Level 1 Turret
		newmis->th_die = Sentry_Die; 		// Death function
		newmis->th_pain = Sentry_Pain;
		//newmis.mdl = "progs/turrgun.mdl";
		newmis->mdl = "progs/trrgn2.mdl";
		sound (oldself, PR_CHAN_ITEM, "weapons/turrset.wav", 1, PR_ATTN_NORM);
		newmis->solid = PR_SOLID_BBOX;
		setmodel(newmis, newmis->mdl);
		setsize (newmis, V({-16, -16, 0}), V({16, 16, 48}));
		setorigin(newmis, oldself->origin + V({0, 0, 8})); // PZ NOTE: set its origin to the base/stand's origin + 8 up
		newmis->real_owner = oldself->real_owner; // The Engineer owns this item
		newmis->trigger_field = oldself;     // PZ NOTE: Points to the sentry's stand/base.
		oldself->oldenemy = newmis;          // PZ NOTE: Pointer from the base to the turret.
		newmis->movetype = PR_MOVETYPE_STEP;
		oldself->colormap = self->colormap;	// Set the Base Color
		newmis->colormap = self->colormap;	// Set the Color

		if (neo.isModeActive()) // for Neo mode
		{
			if (neo.getNeo() != newmis->real_owner)
				newmis->skin = 3;
			else
				newmis->skin = 0;
		}
		else newmis->skin = self->team_no - 1;

		newmis->takedamage = PR_DAMAGE_AIM;
		newmis->velocity = V({0, 0, -8});
		newmis->flags = newmis->flags - (newmis->flags & PR_FL_ONGROUND);
		oldself->flags = oldself->flags - (oldself->flags & PR_FL_ONGROUND);

		newmis->frags = 0; //CH how many people has your sent killed?

		newmis->team_no = self->team_no;
		newmis->think = lvl1_sentry_stand;
		newmis->nextthink = time + 0.5;
		newmis->touch = Sentry_Touch;
		// Rotate Details
		newmis->yaw_speed = 10;	//Turn rate
		newmis->heat = 0;	// Turn Right
		newmis->angles[X] = 0;
		// PZ: Why have we been having the sentry gun built pointed at the owner all of these years?? I added `+ 180`.
		//     Not sure what the purpose of ceil() here is, either. Why not have the sentry gun directly face where the
		//     engineer is looking? .angles might actually be more accurate than .v_angle, too.
		//newmis->angles[Y] = ceil(oldself->angles[Y]); //CH remove decimal  (PZ: original line)
		newmis->angles[Y] = oldself->angles[Y] + 180;   //CH remove decimal
		newmis->angles[Z] = 0;

		newmis->angles[Y] = ((ceil(newmis->angles[Y] / 10)) * 10); //CH set last int to 0

		#ifdef PR_SENTRY_SMOOTHROTATION
		newmis->waitmin = newmis->angles[Y] - PR_SENTRY_ROTATIONWIDTH; // removed anglemod
		newmis->waitmax = newmis->angles[Y] + PR_SENTRY_ROTATIONWIDTH;
		#else
		newmis->waitmin = anglemod(newmis->angles[Y] - PR_SENTRY_ROTATIONWIDTH);
		newmis->waitmax = anglemod(newmis->angles[Y] + PR_SENTRY_ROTATIONWIDTH);
		#endif

		// Give the Gun some ammo
		newmis->ammo_shells = 75; //WK
		newmis->maxammo_shells = PR_SENTRY_AMMO_L1;
		newmis->maxammo_rockets = 20;

		newmis->all_active=0; // OfN - reset HACKER improvements
		newmis->is_malfunctioning = 0;
		newmis->attack_finished = time; //- controls time after losing track of enemy to not be rotating
		newmis->has_holo = 0;

		newmis->has_fieldgen = time; // resets invulnerability of attacking holos 'syndrome'

	}
	else if (oldself->weapon == PR_BUILD_TESLA)
	{
		//CH special sbar for eng.
		self->StatusBarScreen = 4;
		//self.has_tesla = #TRUE;
		self->has_tesla = self->has_tesla | oldself->dont_do_triggerwork;
		sprint (self, PR_PRINT_HIGH, "You finish building the tesla.\n");

		teamprefixsprint(self->team_no,self); //- OfN

		teamsprint(self->team_no, self, self->netname);
		teamsprint(self->team_no, self, " has built a tesla.\n");
		sound (oldself, PR_CHAN_ITEM, "weapons/guerset.wav", 1, PR_ATTN_NORM);

		oldself->classname = "building_tesla";
		oldself->netname = "tesla";
		oldself->takedamage = PR_DAMAGE_AIM;
		oldself->solid = PR_SOLID_BBOX;
		oldself->th_die = Tesla_Die; 		// Death function
		oldself->th_pain = Tesla_Pain;
		if (!Invade_buffPlayer(self))
			self->ammo_cells = self->ammo_cells - PR_BUILD_COST_TESLA;

		oldself->health = PR_BUILD_HEALTH_TESLA;
		oldself->movetype = PR_MOVETYPE_TOSS;
		oldself->colormap = self->colormap;	// Set the Base Color
		oldself->velocity = V({0, 0, -8});
		oldself->avelocity = V({0, 0, 0});
		oldself->flags = oldself->flags - (oldself->flags & PR_FL_ONGROUND);
		oldself->team_no = self->team_no;

		//- OfN -
		//oldself.think = Tesla_Idle;
		oldself->think = tsla_on1;
		//oldself.nextthink = time + 2;
		oldself->nextthink = time + 0.1;
		oldself->has_holo = time + 2; // next Tesla_Idle run
		oldself->job = 0; // this flag will determine which frame animation is currently on
		//oldself.job_finished = time; // change for frame animation purposes, instead of increasing its nextthing during charging
		oldself->no_grenades_1 = PR_FALSE; // first think reset
		oldself->no_grenades_2 = 0; // cloak touch delay reset

		oldself->touch = Tesla_Touch;
		oldself->enemy = world;
		oldself->oldenemy = world; //CH for sbar

		//Set all initial tesla values here
		oldself->maxammo_shells = 0; //Voltage == 0
		oldself->maxammo_nails = 0; //Amps == 0
		oldself->maxammo_rockets = 0; //Battery == 0
		oldself->max_health = oldself->health;
		oldself->ammo_cells = PR_MAXCELLS0; //Initial ammo allocation
		oldself->maxammo_cells = PR_MAXCELLS0; //Initial maxammo
		oldself->tf_items = PR_NIT_CERAMIC; //Start with shock armor
		oldself->armorclass = PR_AT_SAVEELECTRICITY; //Shock armor
		if (self->tf_items & PR_NIT_TESLA_UPGRADE) {
			oldself->has_sentry = 6; //Normal Upgrades
			oldself->has_tesla = 2; //Misc Upgrade
		}
		else
		{
			oldself->has_sentry = 4; //Normal Upgrades
			oldself->has_tesla = 1; //Misc Upgrade
		}
		oldself->has_teleporter = 0; //CH for frag related upgrades
		if (!(self->weapons_carried & PR_WEAP_SPANNER)) { //No spanner
#ifndef PR_NO_PREFAB_TESLA_UPGRADE
			// Gizmo - make use of the upgrade if you have no spanner
			if ( self->tf_items & PR_NIT_TESLA_UPGRADE ) {
				// building an upgraded prefab tesla uses all cells
				self->ammo_cells = 0;

				oldself->ammo_shells = 2;
				oldself->ammo_nails = 2;
				oldself->ammo_rockets = 2;
				oldself->has_sentry = oldself->has_sentry - 6;
				oldself->max_health = 325;
				oldself->ammo_cells = PR_MAXCELLS2; //Initial ammo allocation
				oldself->maxammo_cells = PR_MAXCELLS2;
			} else {
				oldself->ammo_shells = 1;
				oldself->ammo_nails = 2;
				oldself->ammo_rockets = 1;
				oldself->has_sentry = oldself->has_sentry - 4; //Take away 3
				oldself->max_health = 225;
				oldself->ammo_cells = PR_MAXCELLS1; //Initial ammo allocation
				oldself->maxammo_cells = PR_MAXCELLS1;
			}
#else
			oldself->ammo_shells = 1;
			oldself->ammo_nails = 2;
			oldself->ammo_rockets = 1;
			oldself->has_sentry = oldself->has_sentry - 4; //Take away 3
			oldself->max_health = 225;
			oldself->ammo_cells = PR_MAXCELLS1; //Initial ammo allocation
			oldself->maxammo_cells = PR_MAXCELLS1;
#endif
		}
		oldself->health = oldself->max_health;
		oldself->waitmin = (oldself->ammo_shells + 2) * (oldself->ammo_nails + 2);
		oldself->waitmax = PR_FALSE; //No target yet

		oldself->frags = 0; //CH how many people has your sent killed?
		oldself->lip   = 0; //WK How many tinkers have been done
		modelindex_tesla = oldself->modelindex; //CH

		oldself->all_active=0; // OfN - reset HACKER improvements
		oldself->is_malfunctioning = 0;
		oldself->has_fieldgen = time;
		oldself->is_haxxxoring=0; // isnt flying
	}
	else if (oldself->weapon == PR_BUILD_TELEPORTER)
	{
		self->has_teleporter = (self->has_teleporter + 1);
		sprint (self, PR_PRINT_HIGH, "You finish building the Teleporter Pad.\n");

		teamprefixsprint(self->team_no,self); //- OfN

		teamsprint(self->team_no, self, self->netname);
		teamsprint(self->team_no, self, " has built a Teleporter Pad.\n");
		sound (oldself, PR_CHAN_ITEM, "weapons/guerset.wav", 1, PR_ATTN_NORM);

		if (!Invade_buffPlayer(self))
			self->ammo_cells = self->ammo_cells - PR_BUILD_COST_TELEPORTER;

		// Create the teleporter
		oldself->classname = "building_teleporter";
		oldself->netname = "teleporter";
//		oldself.blocked = Teleporter_touch; 	// Actual touch function
		oldself->touch = Teleporter_touch;
		oldself->max_health = PR_BUILD_HEALTH_TELEPORTER;
		oldself->health = PR_BUILD_HEALTH_TELEPORTER;
		oldself->th_die = Teleporter_Die;		// Death function
		oldself->mdl = "progs/telepad.mdl";		// Actual mdl
		oldself->team_no = self->team_no;
		oldself->maxammo_cells = 200;	//CH Max of 20 teleports
		oldself->ammo_cells = 100;	//CH start not at full

		oldself->real_owner = self;			// The Engineer owns this item
		oldself->colormap = self->colormap;	// Set the Color
		oldself->takedamage = PR_DAMAGE_AIM;
		oldself->owner = world;
		oldself->movetype = PR_MOVETYPE_TOSS;
		oldself->velocity = V({0, 0, 8});
		oldself->flags = oldself->flags - (oldself->flags & PR_FL_ONGROUND);

		oldself->solid = PR_SOLID_BBOX;
		setmodel(oldself, oldself->mdl);
//		setsize (oldself, '-16 -16 0', '16 16 4');

		oldself->heat = 4; //dont come on for 4 seconds
		oldself->think = Teleporter_heat_think;
		oldself->nextthink = time + 1;
		oldself->spawnflags = 4; //CH cause extensive checks for height

		oldself->is_malfunctioning = 0;
		oldself->all_active=0; // OfN - reset HACKER improvements
	}
	else if (oldself->weapon == PR_BUILD_FIELDGEN)
	{
		self->has_fieldgen = (self->has_fieldgen + 1);
		sprint (self, PR_PRINT_HIGH, "You finish building the Field Generator.\n");

		teamprefixsprint(self->team_no,self); //- OfN

		teamsprint(self->team_no, self, self->netname);
		teamsprint(self->team_no, self, " has built a Field Generator.\n");
		sound (oldself, PR_CHAN_ITEM, "weapons/guerset.wav", 1, PR_ATTN_NORM);

		if (!Invade_buffPlayer(self))
			self->ammo_cells = self->ammo_cells - PR_BUILD_COST_FIELDGEN;

		// Create the field generator
		oldself->classname = "building_fieldgen";
		oldself->netname = "field generator";
//		oldself.blocked = Teleporter_touch; 	// Actual touch function
		//oldself.touch = Teleporter_touch;
		oldself->max_health = PR_BUILD_HEALTH_FIELDGEN;
		oldself->health = PR_BUILD_HEALTH_FIELDGEN;
		oldself->th_die = FieldGen_Die;		// Death function
		oldself->mdl = "progs/ffgen2.mdl";		// Actual mdl
		oldself->team_no = self->team_no;

		oldself->maxammo_cells = PR_FIELDGEN_NUMCELLS; //CH Max of 20 field recharges
		oldself->ammo_cells = PR_FIELDGEN_NUMCELLS / 2;	//CH start not at full*/

		oldself->real_owner = self;			// The Engineer owns this item
		oldself->colormap = self->colormap;	// Set the Color
		oldself->takedamage = PR_DAMAGE_AIM;
		oldself->owner = world;
		oldself->movetype = PR_MOVETYPE_TOSS;
		oldself->velocity = V({0, 0, 8});
		oldself->flags = oldself->flags - (oldself->flags & PR_FL_ONGROUND);

		oldself->solid = PR_SOLID_BBOX;

		oldself->angles = V({0, 0, 0});

		setmodel(oldself, oldself->mdl);
//		setsize (oldself, '-6 -6 0', '6 6 54');


		//oldself.heat = 4; //dont come on for 4 seconds
		/*oldself.think = Teleporter_heat_think;
		oldself.nextthink = time + 1;*/

		oldself->spawnflags = 4; //CH cause extensive checks for height

		oldself->is_malfunctioning = 0;
		oldself->all_active=0; // OfN - reset HACKER improvements

		FieldGen_Built(oldself);
	}

	// Add string flags used for earthquake and storm find()'s
	if (oldself->weapon == PR_BUILD_SENTRYGUN)
		UpdateVulnerability(newmis);
	else
		UpdateVulnerability(oldself);

	if (self->ammo_cells < 0)
		self->ammo_cells = 0;

	W_SetCurrentAmmo();
}

//=========================================================================
// Dispenser Touch function. Allows players to get stuff from the Dispenser.

void T_Dispenser()
{
	entity dist_checker;

	if (other->classname != "player")
		return;

	if (self->is_malfunctioning & PR_SCREWUP_ONE)
		return;

	if (self->is_malfunctioning & PR_SCREWUP_TWO)
		TF_T_Damage(self, self, self, 200, 0, PR_TF_TD_OTHER);

	// Ignore any engineer working on this dispenser
	if (other->building == world && other->building_wait < time)
	{
		// Pop up the menu
		other->current_menu = PR_MENU_DISPENSER;
		other->menu_count = PR_MENU_REFRESH_RATE;

		other->building = self;

		// Start a Distance checker, which removes the menu if the player
		// gets too far away from the Dispenser.
		dist_checker = spawnServerSide(); // PZ: make it a server-side only entity
		dist_checker->classname = "timer";
		dist_checker->owner = other;
		dist_checker->enemy = self;
		dist_checker->think = CheckDistance;
		dist_checker->nextthink = time + 0.3;
	}
}

//============================================================
// this is needed to avoid stack overflow

void Dispenser_Explode()
{
	T_RadiusDamage(self->demon_one, self->demon_one, self->has_holo, world);

	entity sprite;

	sprite = SpawnSprite(1,PR_SPRITE_AIRBURST,self->demon_one->origin,V({0, 0, 0}),PR_SPRITEMOVE_UPSLOW,0.1);

	if (sprite != world)
	{
		sprite->effects = PR_EF_DIMLIGHT;

		if (self->demon_one->real_owner->team_no == 1)
			sprite->effects = PR_EF_DIMLIGHT | PR_EF_BLUE;
		else if (self->demon_one->real_owner->team_no == 2)
			sprite->effects = PR_EF_DIMLIGHT | PR_EF_RED;
	}

	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
	WriteCoord (PR_MSG_BROADCAST, self->demon_one->origin[X]);
	WriteCoord (PR_MSG_BROADCAST, self->demon_one->origin[Y]);
	WriteCoord (PR_MSG_BROADCAST, self->demon_one->origin[Z]);
#ifdef PR_QUAKE_WORLD
	multicast (self->demon_one->origin, PR_MULTICAST_PHS);
	dremove(self->demon_one);
#else
	BecomeExplosion ();
#endif

	dremove(self); // remove explosion timer for this dispenser
}

void Dispenser_Die()
{
	float damg;
	self->real_owner->has_dispenser = PR_FALSE;

	damg = self->ammo_cells * 0.25;
	damg = damg + self->ammo_nails * 0.1;
	damg = damg + self->ammo_shells * 0.2;
	damg = damg + self->ammo_rockets / 2;

	//- OfN causes overflow so we need a timer for dispenser explosion
	newmis = spawnServerSide(); // PZ: make it a server-side only entity
	newmis->has_holo = damg;
	newmis->demon_one = self;
	newmis->nextthink = time + 0.1;
	newmis->think = Dispenser_Explode;
	/*T_RadiusDamage(self, self, damg, world); // OfN - Fixme, stack overflow??*/


	sprint(self->real_owner, PR_PRINT_HIGH, "Your dispenser was destroyed.\n");

	ThrowGib("progs/dgib1.mdl", -30, PR_TRUE, 0, 0, PR_FALSE);
	ThrowGib("progs/dgib2.mdl", -50, PR_TRUE, 0, 0, PR_FALSE);
	ThrowGib("progs/dgib3.mdl", -50, PR_TRUE, 0, 0, PR_FALSE);
}

//=========================================================================
// Engineer has used a Spanner on the Dispenser
void Engineer_UseDispenser(entity disp)
{
	entity dist_checker;
	string st;

	// Print the dispenser's details
	sprint (self, PR_PRINT_HIGH, "Dispenser has ");
	st = ftos(disp->health);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_("^b/^b"));
	st = ftos(disp->max_health);
	sprint(self, PR_PRINT_HIGH, st);
	sprint (self, PR_PRINT_HIGH, S_(" ^bhealth^b\n"));
	st = ftos(disp->ammo_shells);
	sprint (self, PR_PRINT_HIGH, st);
	sprint (self, PR_PRINT_HIGH, S_(" ^bshells^b, "));
	st = ftos(disp->ammo_nails);
	sprint (self, PR_PRINT_HIGH, st);
	sprint (self, PR_PRINT_HIGH, S_(" ^bnails^b, "));
	st = ftos(disp->ammo_rockets);
	sprint (self, PR_PRINT_HIGH, st);
	sprint (self, PR_PRINT_HIGH, S_(" ^brockets^b\n"));
	st = ftos(disp->ammo_cells);
	sprint (self, PR_PRINT_HIGH, st);
	sprint (self, PR_PRINT_HIGH, S_(" ^bcells^b, and "));
	st = ftos(disp->armorvalue);
	sprint (self, PR_PRINT_HIGH, st);
	sprint (self, PR_PRINT_HIGH, S_(" ^barmor^b\n"));

	if (disp->all_active & PR_IMPROVED_FOUR)
	{
		st = ftos(disp->ammo_medikit);
		sprint (self, PR_PRINT_HIGH, st);


		float f;
		f = 1;

		if (disp->all_active & PR_IMPROVED_ONE)
			f = 2;

		sprint (self, PR_PRINT_HIGH, S_("^b/^b"));

		st = ftos(PR_BUILD_DISPENSER_MAX_MEDIKIT * f);
		sprint (self, PR_PRINT_HIGH, st);
		sprint (self, PR_PRINT_HIGH, S_(" ^bMedikit^b\n"));

		st = ftos(floor(disp->ammo_detpack));
		sprint (self, PR_PRINT_HIGH, st);
		sprint (self, PR_PRINT_HIGH, S_(" ^bDetpacks^b "));

		st = ftos(floor(disp->ammo_c4det));
		sprint (self, PR_PRINT_HIGH, st);
		sprint (self, PR_PRINT_HIGH, S_(" ^bC^4 Detpacks^b\n"));
	}

	// Pop up the menu
	self->current_menu = PR_MENU_ENGINEER_FIX_DISPENSER;
	self->menu_count = PR_MENU_REFRESH_RATE;

	self->building = disp;

	// Start a Distance checker, which removes the menu if the player
	// gets too far away from the Dispenser.
	dist_checker = spawnServerSide(); // PZ: make it a server-side only entity
	dist_checker->classname = "timer";
	dist_checker->owner = self;
	dist_checker->enemy = disp;
	dist_checker->think = CheckDistance;
	dist_checker->nextthink = time + 0.3;
}

//=========================================================================
// Engineer has used a Spanner on the SentryGun
void Engineer_UseSentryGun(entity gun)
{
	entity dist_checker;
	string st;

	// Print the gun's details
	sprint(self, PR_PRINT_HIGH, S_("^bLevel^b "));
	st = Return_Colored_Num(gun->weapon);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, " Sentry Gun has ");
	st = ftos(gun->health);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_("^b/^b"));
	st = ftos(gun->max_health);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_(" ^bhealth^b, "));
	st = ftos(gun->ammo_shells);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_(" ^bshells^b"));
	if (gun->weapon == 3)
	{
		st = ftos(gun->ammo_rockets);
		sprint(self, PR_PRINT_HIGH, ", ");
		sprint(self, PR_PRINT_HIGH, st);
		sprint(self, PR_PRINT_HIGH, S_(" ^brockets^b"));
	}
	sprint(self, PR_PRINT_HIGH, "\n");

	sprint(self, PR_PRINT_HIGH, S_("Sentry Gun ^bFRAGS^b: ")); //CH Displays kills of sent
		st = ftos(gun->frags);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, "\n");

	// Pop up the menu
	self->current_menu = PR_MENU_ENGINEER_FIX_SENTRYGUN;
	self->menu_count = PR_MENU_REFRESH_RATE;

	self->building = gun;

	//dodgy (PZ NOTE: this just makes enemies to the building automatically dismantle it upon spannering it)
	if (!Teammate(self->building->real_owner, self)) {
		Menu_EngineerFix_SentryGun_Input(5);
		return;
	}

	// - OfN - Adds 4-6 seconds of smart sentry gun targetting (no holos)
	gun->has_fieldgen = time + 4 + 2*random();

	if (gun->enemy->classname=="holo" && gun->enemy->has_holo == 1)
		sprint(self, PR_PRINT_HIGH,"You adjust the sentry gun to ignore the holo\n");

	// Start a Distance checker, which removes the menu if the player
	// gets too far away from the sentry.
	dist_checker = spawnServerSide(); // PZ: make it a server-side only entity
	dist_checker->classname = "timer";
	dist_checker->owner = self;
	dist_checker->enemy = gun;
	dist_checker->think = CheckDistance;
	dist_checker->nextthink = time + 0.3;
}

////////////////////////////////////////////////
// Engineer has used a Spanner on the Tesla
void Engineer_UseTesla(entity gun)
{
	entity dist_checker;
	string st;

	// Print the gun's details
	sprint(self, PR_PRINT_HIGH, S_("^bHealth^b:"));
	st = ftos(gun->health);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_("^b/^b"));
	st = ftos(gun->max_health);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_(" ^bCells^b:"));
	st = ftos(gun->ammo_cells);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_("^b/^b"));
	st = ftos(gun->maxammo_cells);
	sprint(self, PR_PRINT_HIGH, st);
	st = ftos(gun->has_sentry);
	sprint(self, PR_PRINT_HIGH, S_(" ^bNormal^b Ups Left:"));
	sprint(self, PR_PRINT_HIGH, st);
	st = ftos(gun->has_tesla);
	sprint(self, PR_PRINT_HIGH, S_(" ^bMisc^b Ups Left:"));
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, "\n");

	sprint(self, PR_PRINT_HIGH, S_("^bVoltage^b:"));
	st = ftos(gun->ammo_shells);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_(" ^bAmperage^b:"));
	st = ftos(gun->ammo_nails);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_(" ^bPower Supply^b:"));
	st = ftos(gun->ammo_rockets);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, "\n");
	if (gun->tf_items) {
		float num;
		num = 0;
		if (gun->tf_items & PR_NIT_TURRET) {
			sprint(self, PR_PRINT_HIGH, "TeslaTurret(tm). ");
			num = num + 1;
		}
		if (gun->tf_items & PR_NIT_SCANNER) {
			sprint(self, PR_PRINT_HIGH, "Improved Targeter. ");
			num = num + 1;
		}
		if (gun->tf_items & PR_NIT_AUTOID) {
			sprint(self, PR_PRINT_HIGH, "Spy Detector. ");
			num = num + 1;
		}
		if (gun->tf_items & PR_NIT_KEVLAR) {
			sprint(self, PR_PRINT_HIGH, "Kevlar Armor. ");
			num = num + 1;
		}
		if (gun->tf_items & PR_NIT_BLAST) {
			sprint(self, PR_PRINT_HIGH, "Blast Armor. ");
			num = num + 1;
		}
		if (gun->tf_items & PR_NIT_ASBESTOS) {
			sprint(self, PR_PRINT_HIGH, "Asbestos Armor. ");
			num = num + 1;
		}
		if (gun->tf_items & PR_NIT_TELEPORTER) {
			sprint(self, PR_PRINT_HIGH, "Upgrades from frags. ");
			num = num + 1;
		}
		if (gun->tf_items & PR_NIT_TESLA_CLOAKING) {
			sprint(self, PR_PRINT_HIGH, "Cloaking Device. ");
			num = num + 1;
		}
		if (num != 0)
			sprint(self, PR_PRINT_HIGH, "\n");
	}

	sprint(self, PR_PRINT_HIGH, S_("Tesla Sentry ^bFRAGS^b: ")); //CH Displays kills of tesla
		st = ftos(gun->frags);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, "\n");

	// Pop up the menu
	self->current_menu = PR_MENU_ENGINEER_FIX_TESLA;
	self->menu_count = PR_MENU_REFRESH_RATE;

	self->building = gun;

	// dodgy (PZ NOTE: this just makes enemies to the building automatically dismantle it upon spannering it)
	if (/*teamplay != 0 &&*/ !Teammate(self->building->real_owner, self)) {
		Menu_EngineerFix_Tesla_Input(8);
		return;
	}

	// - OfN - Adds 6-9 seconds of smart tesla targetting (no holos)
	gun->has_fieldgen = time + 6 + 3*random();

	if (gun->enemy->classname == "holo" && gun->enemy->has_holo == 1)
	{
		sprint(self, PR_PRINT_HIGH,"You adjust the tesla sentry to ignore the holo\n");
	}

	// Start a Distance checker, which removes the menu if the player
	// gets too far away from the tesla.
	dist_checker = spawnServerSide(); // PZ: make it a server-side only entity
	dist_checker->classname = "timer";
	dist_checker->owner = self;
	dist_checker->enemy = gun;
	dist_checker->think = CheckDistance;
	dist_checker->nextthink = time + 0.3;
}
//=================
void Engineer_UseSensor(entity cam)
{
	entity dist_checker;
	string st;

	sprint(self, PR_PRINT_HIGH, "Motion Sensor has ");
	st = ftos(cam->health);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_("^b/^b"));
	st = ftos(ceil(cam->max_health));
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_(" ^bhealth^b\n"));

	// Pop up the menu
	self->current_menu = PR_MENU_ENGINEER_FIX_SENSOR;
	self->menu_count = PR_MENU_REFRESH_RATE;

	self->building = cam;

	// (PZ NOTE: this just makes enemies to the building automatically dismantle it upon spannering it)
	if (/*teamplay != 0 &&*/ !Teammate(self->building->real_owner, self)) {
		Menu_EngineerFix_Sensor_Input(3);
		return;
	}

	// Start a Distance checker, which removes the menu if the player
	// gets too far away from the camera.
	dist_checker = spawnServerSide(); // PZ: make it a server-side only entity
	dist_checker->classname = "timer";
	dist_checker->owner = self;
	dist_checker->enemy = cam;
	dist_checker->think = CheckDistance;
	dist_checker->nextthink = time + 0.3;

}
//=================
void Engineer_UseCamera(entity cam)
{
	entity dist_checker;
	string st;

	sprint(self, PR_PRINT_HIGH, "Security Camera has ");
	st = ftos(cam->health);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_("^b/^b"));
	st = ftos(ceil(cam->max_health));
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_(" ^bhealth^b\n"));

	// Pop up the menu
	self->current_menu = PR_MENU_ENGINEER_FIX_CAMERA;
	self->menu_count = PR_MENU_REFRESH_RATE;

	self->building = cam;

	// (PZ NOTE: this just makes enemies to the building automatically dismantle it upon spannering it)
	if (/*teamplay != 0 &&*/ !Teammate(self->building->real_owner, self)) {
		Menu_EngineerFix_Camera_Input(3);
		return;
	}

	// Start a Distance checker, which removes the menu if the player
	// gets too far away from the camera.
	dist_checker = spawnServerSide(); // PZ: make it a server-side only entity
	dist_checker->classname = "timer";
	dist_checker->owner = self;
	dist_checker->enemy = cam;
	dist_checker->think = CheckDistance;
	dist_checker->nextthink = time + 0.3;

}
//=================
void Engineer_UseTeleporter(entity tele)
{
	entity dist_checker;
	string st;

	sprint(self, PR_PRINT_HIGH, "Teleporter Pad has ");
	st = ftos(tele->health);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_("^b/^b"));
	st = ftos(ceil(tele->max_health));
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_(" ^bhealth^b  "));
	st = ftos(tele->ammo_cells);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_("^b/^b"));
	st = ftos(ceil(tele->maxammo_cells));
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_(" ^bcells^b\n"));


	// Pop up the menu
	self->current_menu = PR_MENU_ENGINEER_FIX_TELEPORTER;
	self->menu_count = PR_MENU_REFRESH_RATE;

	self->building = tele;

	// (PZ NOTE: this just makes enemies to the building automatically dismantle it upon spannering it)
	if (/*teamplay != 0 &&*/ !Teammate(self->building->real_owner, self)) {
		Menu_EngineerFix_Teleporter_Input(8); // PZ: fixed bug; was 4
		return;
	}

	// Start a Distance checker, which removes the menu if the player
	// gets too far away from the teleporter.
	dist_checker = spawnServerSide(); // PZ: make it a server-side only entity();
	dist_checker->classname = "timer";
	dist_checker->owner = self;
	dist_checker->enemy = tele;
	dist_checker->think = CheckDistance;
	dist_checker->nextthink = time + 0.3;

}
//=========================================================================
// Think function for the timer which checks the distance between the
// Engineer and the building he's using
void CheckDistance()
{
	vector dist;

	// Check to see if the Engineer's spanner'ed a different building
	// without leaving the area of this one.
	if (self->owner->building != self->enemy)
	{
		dremove(self);
		return;
	}

	dist = self->enemy->origin - self->owner->origin;
//WK	if (vlen(dist) > 64)
	if (vlen(dist) > 98)
	{
		CenterPrint(self->owner, "\n");
		self->owner->menu_count = PR_MENU_REFRESH_RATE;
		self->owner->current_menu = PR_MENU_DEFAULT;
		self->owner->building = world;
		dremove(self);
		return;
	}

	self->nextthink = time + 0.3;
}

} // END namespace Progs
