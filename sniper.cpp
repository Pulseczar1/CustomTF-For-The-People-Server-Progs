/*======================================================
	SNIPER.QC

	TeamFortress v2.5	 29/2/97
========================================================
Functions for the SNIPER class and associated weaponry
========================================================*/

#include "progs.h"
#include "ofndefs.h"
#include "weapons.h"
#include "debug.h"

namespace Progs {

// Functions outside this file

// Functions inside this file
void TeamFortress_SniperWeapon();
// Autozoom functions
void TF_zoom(float zoom_level);
void SniperSight_Update();
void SniperSight_Update2();
void SniperSight_Create(float type);
void RevealThief(entity targ, float pain);

//=========================================================================
// Sniper/Auto Rifle selection function
void TeamFortress_SniperWeapon()
{
	//local	float	it;

	self->impulse = 0;

    if (self->tfstate & PR_TFSTATE_RELOADING)
   		return;

	if (!((self->weapons_carried & PR_WEAP_SNIPER_RIFLE) && (self->weapons_carried & PR_WEAP_AUTO_RIFLE)))
		return;

	if (self->ammo_shells < 1)
	{	// don't have the ammo
		sprint (self, PR_PRINT_HIGH, "Not enough ammo.\n");
		return;
	}

	if (self->current_weapon == PR_WEAP_SNIPER_RIFLE)
	{
		self->current_weapon = PR_WEAP_AUTO_RIFLE;
	}
	else
	{
		self->current_weapon = PR_WEAP_SNIPER_RIFLE;
	}

	W_SetCurrentAmmo ();
}

//=========================================================================
// Do the autozoom of the sniper rifle
void TF_zoom(float zoom_level)
{
	string zl;

	if (self->tfstate & PR_TFSTATE_ZOOMOFF)
		return;

	zl = ftos(zoom_level);

	stuffcmd(self, "fov ");
	stuffcmd(self, zl);
	stuffcmd(self, "\n");
}


//=========================================================================
// Move the sight to the point the rifle's aiming at
void SniperSight_Update()
{
	vector org;

	if (!(self->owner->tfstate & PR_TFSTATE_AIMING) || (self->owner->current_weapon != PR_WEAP_SNIPER_RIFLE))
	{
		self->owner->tfstate = self->owner->tfstate - (self->owner->tfstate & PR_TFSTATE_AIMING);
		TeamFortress_SetSpeed(self->owner);
		self->owner->heat = 0;
		dremove(self);
		return;
	}

	makevectors(self->owner->v_angle);

	org = self->owner->origin + v_forward*10;
	org[Z] = self->owner->absmin[Z] + self->owner->size[Z] * 0.7;

	traceline(org, org + v_forward*9192, PR_TL_ANY_SOLID, self);

	if (trace_fraction == 1.0)
	{
		setorigin(self, self->owner->origin);
		return;
	}

#ifdef PR_ORIENTED_SIGHTS
	// PZ: Orient the dot with the wall.
	self->angles = vectoangles(V({0, 0, 0}) - trace_plane_normal);
	self->angles[X] = self->angles[X] * -1;
	// PZ: Move it out very slightly from the wall, in the direction of `trace_plane_normal`, to keep the sprite from
	//     "z fighting" with the wall (showing up partially inside the wall).
	setorigin(self, trace_endpos + trace_plane_normal * PR_ORIENTED_SIGHT_OFFSET);
#else
	self->angles = vectoangles(v_forward);
	setorigin(self, trace_endpos);
#endif

#ifdef PR_SNIPERSIGHT_SHOWPOS
	string st;
	st = vtos(trace_endpos);
	sprint(self->owner,PR_PRINT_HIGH,"Sniper dot pos: ",st,"\n");
#endif

	self->nextthink = time + 0.1;
}

//CH used for the rl
void SniperSight_Update2()
{
	//bprint(#PRINT_HIGH, "Sight Think..\n");
	vector org;

	if (!(self->owner->tfstate & PR_TFSTATE_RL_LASER) || (self->owner->current_weapon != PR_WEAP_ROCKET_LAUNCHER))
	{
		self->owner->tfstate = self->owner->tfstate - (self->owner->tfstate & PR_TFSTATE_RL_LASER);
		TeamFortress_SetSpeed(self->owner);
		self->owner->heat = 0;
		dremove(self);
		return;
	}

	makevectors(self->owner->v_angle);

	org = self->owner->origin + v_forward*10;
	org[Z] = self->owner->absmin[Z] + self->owner->size[Z] * 0.7;

	traceline(org, org + v_forward*9192, PR_TL_ANY_SOLID, self);

	if (trace_fraction == 1.0)
	{
		setorigin(self, self->owner->origin );
		return;
	}

#ifdef PR_ORIENTED_SIGHTS
	// PZ: Orient the dot with the wall.
	self->angles = vectoangles(V({0, 0, 0}) - trace_plane_normal);
	self->angles[X] = self->angles[X] * -1;
	// PZ: Move it out very slightly from the wall, in the direction of `trace_plane_normal`, to keep the sprite from
	//     "z fighting" with the wall (showing up partially inside the wall).
	setorigin(self, trace_endpos + trace_plane_normal * PR_ORIENTED_SIGHT_OFFSET);
#else
	self->angles = vectoangles(v_forward);
	setorigin(self, trace_endpos);
#endif

	self->nextthink = time + 0.1;
}


//=========================================================================
// Create the sight
//CH type 1 = rl
void SniperSight_Create(float type)
{
	entity sight;
	if (type == 1)
		self->tfstate = self->tfstate | PR_TFSTATE_RL_LASER;
	else
		self->tfstate = self->tfstate | PR_TFSTATE_AIMING;

	sight = spawn();
	sight->owner = self;
	sight->movetype = PR_MOVETYPE_NOCLIP;
	sight->solid = PR_SOLID_NOT;

	if (megatf & PR_MEGATF_SIGHTS)
	{
		// PZ: We found that we could use `trace_plane_normal` to orient sniper dots flat with surfaces.
		//     A certain flag has to be set in the sprite file to allow it to be orientable. colsight2 has that flag set.
#ifdef PR_ORIENTED_SIGHTS
		setmodel(sight, "progs/colsight2.spr");
#else
		setmodel(sight, "progs/colsight.spr");   // Original code before July 2022
#endif
		// PZ: added condition so that when there are no teams (teamplay 0) dots all show up as red (rather than invisible)
		if (self->team_no >= 1 && self->team_no <= 4) sight->frame = self->team_no - 1;
		else sight->frame = 1; // team 2 (red)
	}
	else
		setmodel(sight, "progs/sight.spr");

	sight->classname = "timer";   // PZ NOTE: despite this being called a timer, it has a model; so it can't be serverside-only
	sight->netname = "sniperdot"; // OfN

	setorigin(sight, self->origin);
	if (type == 1)
		sight->think = SniperSight_Update2;
	else
		sight->think = SniperSight_Update;
	sight->nextthink = time + 0.05;

	// Prevent theif snipers.
	if (self->job & PR_JOB_THIEF && (self->job & PR_JOB_ACTIVE || self->job & PR_JOB_FULL_HIDE))
		RevealThief(self,PR_FALSE);
}

// this toggles the sniper's autozoom on/off
void TeamFortress_AutoZoomToggle()
{
	if (self->tfstate & PR_TFSTATE_ZOOMOFF)
	{
		self->tfstate = self->tfstate - PR_TFSTATE_ZOOMOFF;
		sprint(self, PR_PRINT_HIGH, "autozoom ON\n");
	}
	else
	{
		self->tfstate = self->tfstate | PR_TFSTATE_ZOOMOFF;
		sprint(self, PR_PRINT_HIGH, "autozoom OFF\n");
	}

}

} // END namespace Progs
