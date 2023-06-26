/*======================================================
	TELEPORT.QC 		Custom TeamFortress v2.2

	(c) Craig Hauser			23/3/00
========================================================
Functions for the teleporters
======================================================*/

#include "progs.h"
#include "tfdefs.h"
#include "triggers.h"
#include "custom.h"
#include "combat.h"
#include "debug.h"

namespace Progs {

void Teleporter_touch();
void Teleporter_Die();
void Teleporter_heat_think();
float Teleporter_check_person(entity targ);

//- OfN
float IsOutWorld(entity player);

#define PR_TELEPORTER_CELLS	8 //How many cells to use on teleport
#define PR_TELEPORTER_WAIT 	2  //How long to wait after teleport (whole number)

//====================
void Teleporter_touch()
{
	entity te;

	if (self->heat == 0)
	{
		if (Teleporter_check_person(other))
		{
			te = find(world, "classname", "building_teleporter");
			while (!(te->real_owner == self->real_owner && te != self) && te != world)
			{
				te = find(te, "classname", "building_teleporter");
			}
			if (te != world)
			{
				vector vec;
				vec = V({0, 0, 32});

				if (te->tf_items & PR_NIT_TURRET) //- OfN turretized teleporters
					vec = V({0, 0, -40});

				if (te->heat == 0 && te->ammo_cells >= PR_TELEPORTER_CELLS)
				{
					self->heat = PR_TELEPORTER_WAIT;
					te->heat = PR_TELEPORTER_WAIT;
					if (self->is_malfunctioning & PR_SCREWUP_FOUR || te->is_malfunctioning & PR_SCREWUP_FOUR)
					{
						CenterPrint(other, "The other teleporter is out of power.\n");
						return;
					}

					self->ammo_cells = self->ammo_cells - PR_TELEPORTER_CELLS;
					te->ammo_cells = te->ammo_cells - PR_TELEPORTER_CELLS;

					entity ote;
					ote = te;
					if (self->is_malfunctioning & PR_SCREWUP_THREE || te->is_malfunctioning & PR_SCREWUP_THREE) //sb
					{
						te = self->real_owner;
						spawn_tdeath(te->origin + vec, te);
					}
					spawn_tfog (other->origin);
					spawn_tdeath(te->origin + vec, other);
					makeImmune(other,time + 2);
					setorigin(other, te->origin + vec);

					//- OfN
					if (IsOutWorld(other))
					{
#ifndef PR_NO_ZEROGRAVGUN
						// Gizmo - if other was forced into a wall; we should die before PlayerPostThink() resets it
						if ( self->pickedupby  != world)
						if ( Teammate(self->pickedupby, other) )
							other->PR_teleporter_teamkiller = self->pickedupby;
#endif
						deathmsg = PR_DMSG_TELEOUTWORLD;
						TF_T_Damage(other, other, other,1000,0,PR_TF_TD_OTHER);
						return;
					}

#ifndef PR_NO_ZEROGRAVGUN
					// Gizmo - if other was forced to teleport into lava, make sure we count it as a teamkill
					if ( pointcontents( other->origin ) == PR_CONTENT_LAVA || !checkbottom( other ) )
					if ( self->pickedupby  != world)
					if ( Teammate(self->pickedupby, other) ) {
						other->flags = other->flags - ( other->flags & ( PR_FL_ONGROUND | PR_FL_PARTIALGROUND ) );
						other->PR_teleporter_teamkiller = self->pickedupby;
					}
#endif

					if (self->is_malfunctioning & PR_SCREWUP_ONE || te->is_malfunctioning & PR_SCREWUP_ONE) // SB
					{
						other->ammo_cells = 0;
						other->ammo_shells = 0;
						other->ammo_nails = 0;
						other->ammo_rockets = 0;
						other->armorvalue = 0;
						other->ammo_detpack = 0;
						other->ammo_c4det = 0;
						other->ammo_medikit = 0;
					}
					if (self->is_malfunctioning & PR_SCREWUP_TWO || te->is_malfunctioning & PR_SCREWUP_TWO) // SB
					{
						if (self->is_malfunctioning & PR_SCREWUP_TWO)
							te->martyr_enemy = self->martyr_enemy;
						else if (te->is_malfunctioning & PR_SCREWUP_TWO)
							self->martyr_enemy = te->martyr_enemy;
						TF_T_Damage(other, self, self, 500, 0, PR_TF_TD_OTHER);
					}
					spawn_tfog (te->origin + vec);
					if (self->is_malfunctioning & PR_SCREWUP_THREE || ote->is_malfunctioning & PR_SCREWUP_THREE)
						te = ote;
					if ((te->ammo_cells >= PR_TELEPORTER_CELLS && te->ammo_cells <= PR_TELEPORTER_CELLS * 2) || (self->ammo_cells >= PR_TELEPORTER_CELLS && self->ammo_cells <= PR_TELEPORTER_CELLS * 2))
						sprint (self->real_owner, PR_PRINT_HIGH, "A teleporter is getting low on power.\n");
				}
				else if (te->ammo_cells < PR_TELEPORTER_CELLS)
					CenterPrint(other, "The other teleporter is out of power.\n");
			}
		}
	}

	if (other == world && self->tf_items & PR_NIT_TURRET && !(self->flags & PR_FL_ONGROUND))
	{ //The tele has landed! //- needed?
		sprint(self->real_owner, PR_PRINT_HIGH, "Teleporter turretized succesfully!\n");
		self->solid = PR_SOLID_BBOX; //#SOLID_BBOX;//SOLID_SLIDEBOX// SOLID_TRIGGER
		self->flags = self->flags | PR_FL_ONGROUND;
	   // setsize(self, self.mins, self.maxs);
		//self.movetype = #MOVETYPE_STEP;
		self->origin[Z] = self->origin[Z] + 46;//32; // was 40, old custom bug? fixed
		setorigin(self,self->origin);

		self->velocity = V({0, 0, 0});
		//setsize (self.building, '-16 -16 -48', '16 16 -52');
		//setsize (self.building, '-16 -16 0', '16 16 1');
		//return;
	}

	/*if (other.solid == #SOLID_BSP && self.tf_items & #NIT_TURRET && !(self.flags & #FL_ONGROUND))
	{
		sprint(self.real_owner, #PRINT_HIGH, "Teleporter turretized succesfully!\n");
		self.solid = #SOLID_TRIGGER; //#SOLID_BBOX;//SOLID_SLIDEBOX// SOLID_TRIGGER
		self.flags = self.flags | #FL_ONGROUND; //MOVETYPE_NONE
		self.movetype = #MOVETYPE_NONE;
		self.origin_z = self.origin_z + 8;//32; // was 40, old custom bug? fixed
		//setsize (self.building, '-16 -16 -48', '16 16 -52');
		//setsize (self.building, '-16 -16 -0', '16 16 -4');
		//return;
	}*/


}
//===============
float Teleporter_check_person(entity targ)
{
	// PZ: teleporters being carried, or flying through the air after being thrown, by ZGG, are inactive, except in Coop
	if (deathmatch && (self->pickedupby != world || (self->velocity != V({0, 0, 0}) && !(self->flags & PR_FL_ONGROUND)))) return PR_FALSE;

	if (self->ammo_cells < PR_TELEPORTER_CELLS) {
		if (Teammate(targ, self)) //only to team member
			CenterPrint(other, "This teleporter is out of power.\n");
		return PR_FALSE;
	}
	if (self->real_owner->has_teleporter == 1) {
		if (Teammate(targ, self)) //only to team member
			CenterPrint(other, "There is no other teleporter!\n");
		return PR_FALSE;
	}
#ifdef PR_QUAKE_WORLD // culled by KK. was QUAKE_WORLD
	/*if (infokey(world,"ceasefire")=="on") //To not cause loops
		return #FALSE;*/
	if (ceasefire)
		return PR_FALSE;
#endif
	if (targ == world)//that would be bad.
		return PR_FALSE;
	if (targ->done_custom & PR_CUSTOM_BUILDING)
		return PR_FALSE;
	if (targ->classname != "player")
		return PR_FALSE;
	if (targ->velocity != V({0, 0, 0}))
		return PR_FALSE;
	if (targ->health <= 0)
		return PR_FALSE;
	if (!targ->is_connected)
		return PR_FALSE;
	if (targ->playerclass == PR_PC_UNDEFINED)
		return PR_FALSE;
	if (targ->is_feigning)
		return PR_FALSE;
	if (targ->is_building)
		return PR_FALSE;
	if (targ->is_detpacking)
		return PR_FALSE;
	if (targ->playerclass == PR_PC_CIVILIAN) {
		CenterPrint(targ, "Civilian + Teleporter = No, NFC!\n");
		return PR_FALSE;
	}
	// PZ: remove_spawnpoint keeps track of number of goals attached
	if ((targ->effects & (PR_EF_BRIGHTLIGHT | PR_EF_DIMLIGHT | PR_EF_BRIGHTFIELD)) && targ->remove_spawnpoint) {//no flag for you!
		CenterPrint(targ, "Can't use teleporter while carrying a goal item!\n"); // PZ: Changed from "while glowing".
		return PR_FALSE;
	}

//ALL NEW CHECKS _MUST_ BE ABOVE THIS LINE
	if (teamplay)
	{
		if (Teammate(targ, self))
			return PR_TRUE;
		if (Teammate(targ->undercover_team, self))
			return PR_TRUE;
	}
	else
		return PR_TRUE;
	return PR_FALSE;
}
//===============
void Teleporter_heat_think()
{
	//- OfN turretizing and stoped?
	if (self->tf_items & PR_NIT_TURRET && !(self->flags & PR_FL_ONGROUND))
		self->velocity[Z] = 200;

//CH used to reduce the heat and check pos
	float pos, num;
	vector loc;

	if (self->heat <= 0)
		self->heat = 0;
	else
		self->heat = self->heat - 1;

	self->think = Teleporter_heat_think; //keep in loop
	self->nextthink = time + 1;

//CH if spawnflags is >0 do extensive height checks b/c the pad just spawned.
	if (self->spawnflags > 0) {
		self->spawnflags = self->spawnflags - 1;
		num = 0; //16 checks
	}
	else
		num = 15; //1 check
	while (num < 16)
	{
		//sprint (self.real_owner, #PRINT_HIGH, "Checking space!\n");
		loc = self->origin;
		loc[Z] = loc[Z] + (rint(random() * 8 + 1) * 8); //1-8 * 8  x+8 -> x+64
		loc[X] = loc[X] + ((rint(random() * 3 + 1) * 16) - 32);
		loc[Y] = loc[Y] + ((rint(random() * 3 + 1) * 16) - 32);

		pos = pointcontents(loc);					   //-
		if ( ( (pos == PR_CONTENT_SOLID || pos == PR_CONTENT_SKY) && !(self->tf_items & PR_NIT_TURRET)) || (self->tf_items & PR_NIT_TURRET && pos == PR_CONTENT_SKY) )
		{
			TF_T_Damage(self, world, world, self->health+1, 0, 0);

			if (self->tf_items & PR_NIT_TURRET) //- ofn
				sprint (self->real_owner, PR_PRINT_HIGH, "Your teleporter flies away.\n");
			else
				sprint (self->real_owner, PR_PRINT_HIGH, "Not enough space for teleportation.\n");

			num = 100; //Get out of loop
		}
		num = num + 1;
	}

}
//================
void Teleporter_Die()
{
	sprint(self->real_owner, PR_PRINT_HIGH, "Your Teleporter Pad was destroyed.\n");
	self->real_owner->has_teleporter = (self->real_owner->has_teleporter - 1);

//	ThrowGib("progs/tgib1.mdl", -70);
//	ThrowGib("progs/tgib2.mdl", -70);
//	ThrowGib("progs/tgib3.mdl", -70);

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

} // END namespace Progs
