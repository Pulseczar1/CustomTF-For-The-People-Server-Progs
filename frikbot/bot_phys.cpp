/***********************************************
*                                              *
*                FrikBot Physics               *
*        The near-perfect emulation of         *
*                Client movement               *
*                                              *
*         Special Thanks to: Asdf, Frog        *
*             Alan "Strider" Kivlin            *
*                                              *
*                                              *
***********************************************/

/*
This program is in the Public Domain. My crack legal
team would like to add:

RYAN "FRIKAC" SMITH IS PROVIDING THIS SOFTWARE "AS IS"
AND MAKES NO WARRANTY, EXPRESS OR IMPLIED, AS TO THE
ACCURACY, CAPABILITY, EFFICIENCY, MERCHANTABILITY, OR
FUNCTIONING OF THIS SOFTWARE AND/OR DOCUMENTATION. IN
NO EVENT WILL RYAN "FRIKAC" SMITH BE LIABLE FOR ANY
GENERAL, CONSEQUENTIAL, INDIRECT, INCIDENTAL,
EXEMPLARY, OR SPECIAL DAMAGES, EVEN IF RYAN "FRIKAC"
SMITH HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
DAMAGES, IRRESPECTIVE OF THE CAUSE OF SUCH DAMAGES.

You accept this software on the condition that you
indemnify and hold harmless Ryan "FrikaC" Smith from
any and all liability or damages to third parties,
including attorney fees, court costs, and other
related costs and expenses, arising out of your use
of this software irrespective of the cause of said
liability.

The export from the United States or the subsequent
reexport of this software is subject to compliance
with United States export control and munitions
control restrictions. You agree that in the event you
seek to export this software, you assume full
responsibility for obtaining all necessary export
licenses and approvals and for assuring compliance
with applicable reexport restrictions.

Any reproduction of this software must contain
this notice in its entirety.
*/

#include "progs.h"
#include "frikbot/bot_qw.h"
#include "frikbot/bot_misc.h"

namespace Progs {

/*
=========================================

Stuff mimicking cl_input.c code

=========================================
*/

/*
=========================================

frik_isBotPressingKey     (PZ: was "CL_KeyState")

`key` = the key
`self` = the current bot

=========================================
*/
// PZ NOTE: Since we don't have 'inline', can we make this a define? No. cppreqcc doesn't seem to support it.
float frik_isBotPressingKey(int key)
{
	return ((self->keys & key) > 0);
}

/*
=========================================

frik_turnInputCommandsIntoActions     (PZ: was "CL_KeyMove")

`key` = the key
`self` = the current bot

=========================================
*/
// PZ TODO: ALL AIMING CODE SHOULD BE IN ONE PLACE, IF WE CAN DO THAT
void frik_turnInputCommandsIntoActions() // CL_BaseMove + CL_AdjustAngles (referring to C code)
{
	float anglespeed;
	vector view = V({0, 0, 0});

	// [handle movement keys (speed is halved unless AI_PRECISION is not set)]
	if (self->keys != self->oldkeys)
	{
		self->movevect = V({0, 0, 0});
		// PZ TODO: looks like different directions have different speeds, but my current code applies same speed to all directions
		self->movevect[Y] = self->movevect[Y] + ((self->maxspeed / 2) /*350*/ * frik_isBotPressingKey(PR_KEY_MOVERIGHT));   // 350 is the default cl_sidespeed
		self->movevect[Y] = self->movevect[Y] - ((self->maxspeed / 2) /*350*/ * frik_isBotPressingKey(PR_KEY_MOVELEFT));    // 350 is the default cl_sidespeed
		self->movevect[X] = self->movevect[X] + ((self->maxspeed / 2) /*200*/ * frik_isBotPressingKey(PR_KEY_MOVEFORWARD)); // 200 is the default cl_forwardspeed
		self->movevect[X] = self->movevect[X] - ((self->maxspeed / 2) /*200*/ * frik_isBotPressingKey(PR_KEY_MOVEBACK));    // 200 is the default cl_backspeed
		self->movevect[Z] = self->movevect[Z] + ((self->maxspeed / 2) /*200*/ * frik_isBotPressingKey(PR_KEY_MOVEUP));      // 200 is the default cl_upspeed
		self->movevect[Z] = self->movevect[Z] - ((self->maxspeed / 2) /*200*/ * frik_isBotPressingKey(PR_KEY_MOVEDOWN));    // 200 is the default cl_upspeed
		if (!(self->b_aiflags & PR_AI_PRECISION))
			self->movevect = self->movevect * 2; // 2 is the default cl_movespeedkey & bot always has +speed
	}
	self->oldkeys = self->keys;

	// [handle turning/aiming]
	// PZ NOTE: this is for keyboard aiming emulation
	if (self->b_skill == PR_SKILL_EASY) // PZ: MEDIUM is no longer a keyboarder; only EASY is a keyboarder (was "!= #SKILL_HARD")
	{
		anglespeed = 1.5 * real_frametime;   // 1.5 is the default cl_anglespeedkey & bot always has +speed
		self->v_angle[Y] = self->v_angle[Y] + anglespeed * frik_isBotPressingKey(PR_KEY_LOOKLEFT) * 140;  // 140 is default cl_yawspeed
		self->v_angle[Y] = self->v_angle[Y] - anglespeed * frik_isBotPressingKey(PR_KEY_LOOKRIGHT) * 140; // 140 is default cl_yawspeed
		self->v_angle[X] = self->v_angle[X] - anglespeed * frik_isBotPressingKey(PR_KEY_LOOKUP) * 150;    // 150 is default cl_pitchspeed
		self->v_angle[X] = self->v_angle[X] + anglespeed * frik_isBotPressingKey(PR_KEY_LOOKDOWN) * 150;  // 150 is default cl_pitchspeed
	}
	// PZ NOTE: this is for mouse aiming emulation (for MEDIUM and HARD)
	else  // PZ NOTE: NIGHTMARE's is done in frik_botAdjustAim(). His aim just snaps to the targets.
	{
		// gets the difference between the correct aiming angle (.b_commandedViewAngle) and the current aimed angle
		view[X] = frik_angleSubtraction(self->b_commandedViewAngle[X], self->v_angle[X]);
		view[Y] = frik_angleSubtraction(self->b_commandedViewAngle[Y], self->v_angle[Y]);
		// PZ: in MEDIUM, add some error to aiming
// !!!!!!!!PZ TODO: IT'S ONLY DOING THIS FOR MEDIUM; NO WONDER CONCUSSION SEEMS WEAK AND ASSCAN IS STILL OVERPOWERED
		if (self->b_skill == PR_SKILL_MEDIUM)
		{
			float randomNumber, rtemp, maxError;

			if (self->current_weapon == PR_WEAP_SNIPER_RIFLE)
				maxError = 2;
			else if (self->current_weapon == PR_WEAP_ASSAULT_CANNON)
				maxError = 7;
			else
				maxError = 5;

			// if the bot has the target in his sights, don't mess with his aim, unless he is firing the assault cannon
			if (view[X] <= 0.25 && view[Y] <= 0.25 && !(self->current_weapon == PR_WEAP_ASSAULT_CANNON && self->PR_BUTTON_FIRE))
				maxError = 0;

			// no matter what, if he is concussed, mess up his aim really badly
			if (self->tfstate & PR_TFSTATE_CONCUSSIONED)
				maxError = maxError + 35;

			if (maxError)
			{
				// for _x (pitch)
				randomNumber = random();                 // random() generates a number between 0 and 1
				randomNumber = randomNumber * maxError;  // error will now be between 0 and `maxError` degrees
				rtemp = random();
				if (rtemp >= 0.505) randomNumber = randomNumber * -1; // 50% chance that it will be a negative number
				view[X] = view[X] + randomNumber;
				// for _y (yaw)
				randomNumber = random();                 // random() generates a number between 0 and 1
				randomNumber = randomNumber * maxError;  // error will now be between 0 and `maxError` degrees
				rtemp = random();
				if (rtemp >= 0.505) randomNumber = randomNumber * -1; // 50% chance that it will be a negative number
				view[Y] = view[Y] + randomNumber;
			}
		}

		if (vlen(view) > 30)  // PZ NOTE: if aim is way off move mouse faster?
		{
			self->mouse_emu = self->mouse_emu + (view * 30);
			if (vlen(self->mouse_emu) > 180)
				self->mouse_emu = normalize(self->mouse_emu) * 180;
		}
		else
			self->mouse_emu = view * (1 / real_frametime);
		self->v_angle = self->v_angle + self->mouse_emu * real_frametime;
	}

	// limit his viewing the way a player would be limited
	if (self->v_angle[X] > 80)
		self->v_angle[X] = 80;
	else if (self->v_angle[X] < -70)
		self->v_angle[X] = -70;

	if (self->v_angle[Z] > 50)       // PZ FIXME?: z? isn't that roll? can a player's view even roll? does when dead
		self->v_angle[Z] = 50;
	else if (self->v_angle[Z] < -50)
		self->v_angle[Z] = -50;

	self->v_angle[Y] = frik_getAngleMod360(self->v_angle[Y]); // limit yaw angle to 0 to 360
}

#ifdef PR_USE_FRIKBOT_PHYSICS_RATHER_THAN_C_PHYSICS

/*
=========================================

Stuff mimicking sv_user.c

=========================================
*/

/*
=========================================

frik_applyFrictionToBot   (PZ: was "SV_UserFriction")

`self` = the current bot

=========================================
*/
void frik_applyFrictionToBot()
{
	vector vel, start, stop;
	float sped, friction, newspeed;

	vel = self->velocity;
	vel[Z] = 0;
	sped = vlen(vel);
	vel = self->velocity;

	if (!sped)
		return;

	// if the leading edge is over a dropoff, increase friction
	start[X] = stop[X] = self->origin[X] + vel[X] / (sped * 16);
	start[Y] = stop[Y] = self->origin[Y] + vel[Y] / (sped * 16);
	start[Z] = self->origin[Z] + self->mins[Z];
	stop[Z] = start[Z] - 34;

	traceline(start, stop, PR_TL_BSP_ONLY, self);

	if (trace_fraction == 1)
		friction = sv_friction * 2; // 2 is default edgefriction, removed for QW compatibility
	else
		friction = sv_friction;
	if (sped < sv_stopspeed)
		newspeed = sped - real_frametime * sv_stopspeed * friction;
	else
		newspeed = sped - real_frametime * sped * friction;

	if (newspeed < 0)
		newspeed = 0;
	newspeed = newspeed / sped;

	self->velocity[Y] = vel[Y] * newspeed;
	self->velocity[X] = vel[X] * newspeed;
}

/*
=========================================

frik_botWaterJump    (PZ: was "SV_WaterJump")

PZ NOTE: I don't know what this function is doing, because it doesn't affect the bot's z velocity

`self` = the current bot

=========================================
*/
void frik_botWaterJump()
{
	// PZ NOTE: I think teleport_time indicates when a bot/player has just gone through a teleporter
	if (time > self->teleport_time || !self->waterlevel)
	{
		self->flags = self->flags - (self->flags & PR_FL_WATERJUMP);
		self->teleport_time = 0;
	}
	self->velocity[X] = self->movedir[X];
	self->velocity[Y] = self->movedir[Y];
	// PZ: Bot is having trouble getting out of water. Added the following line.
	// This doesn't seem to be getting called.
	//self.velocity_z = self.velocity_z + 2000;
}

/*
=========================================

frik_botDropPunchAngle    (PZ: was "DropPunchAngle")

PZ NOTE: not sure what exactly .punchangle does

`self` = the current bot

=========================================
*/
void frik_botDropPunchAngle()
{
	float len;
	len = vlen(self->punchangle);
	self->punchangle = normalize(self->punchangle);
	len = len - 10 * real_frametime;
	if (len < 0)
		len = 0;
	self->punchangle = self->punchangle * len;
}

/*
=========================================

frik_botAccelerateInAir    (PZ: was "SV_AirAccelerate")

`self` = the current bot

=========================================
*/
void frik_botAccelerateInAir(const vector& wishvel)
{
	float addspeed, wishspd, accelspeed, currentspeed;

	wishspd = vlen(wishvel);
	wishvel = normalize(wishvel);
	if (wishspd > 30)
		wishspd = 30;
	currentspeed = self->velocity * wishvel;
	addspeed = wishspd - currentspeed;
	if (addspeed <= 0)
		return;
	accelspeed = 10 * sv_accelerate * wishspd * real_frametime;
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	self->velocity = self->velocity + accelspeed * wishvel;
}

/*
=========================================

frik_botAccelerateOnGround    (PZ: was "SV_Accelerate")

`self` = the current bot

=========================================
*/
void frik_botAccelerateOnGround(const vector& wishvel)
{
	float addspeed, wishspd, accelspeed, currentspeed;

	wishspd = vlen(wishvel);
	wishvel = normalize(wishvel);

	currentspeed = self->velocity * wishvel;
	addspeed = wishspd - currentspeed;
	if (addspeed <= 0)
		return;
	accelspeed = sv_accelerate * wishspd * real_frametime;
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	self->velocity = self->velocity + accelspeed * wishvel;
}

/*
=========================================

frik_botAccelerateInWater    (PZ: was "SV_WaterMove")

`self` = the current bot

=========================================
*/
void frik_botAccelerateInWater()
{
	vector wishvel;
	float wishspeed, addspeed, currentSpeed, newspeed;
	makevectors(self->v_angle);
	wishvel = v_right * self->movevect[Y] + v_forward * self->movevect[X];

	if (self->movevect == V({0, 0, 0}))   // PZ NOTE: sink when not moving
		wishvel[Z] = wishvel[Z] - 60;
	else
		wishvel[Z] = wishvel[Z] + self->movevect[Z];
	wishspeed = vlen(wishvel);

	if (wishspeed > sv_maxspeed)
	{
		wishvel = (sv_maxspeed / wishspeed) * wishvel;
		wishspeed = sv_maxspeed;
	}
	wishspeed = wishspeed * 0.7;
	currentSpeed = vlen(self->velocity);
	if (currentSpeed)
	{
		newspeed = currentSpeed - (real_frametime * currentSpeed * sv_friction);
		if (newspeed < 0)
			newspeed = 0;
		self->velocity = self->velocity * (newspeed / currentSpeed);
	}
	else
		newspeed = 0;

	if (!wishspeed)
		return;
	addspeed = wishspeed - newspeed;
	if (addspeed <= 0)
		return;
	wishvel = normalize(wishvel);
	currentSpeed = sv_accelerate * wishspeed * real_frametime;
	if (currentSpeed > addspeed)
		currentSpeed = addspeed;
	self->velocity = self->velocity + currentSpeed * wishvel;
}

/*
=========================================

frik_botMoveOnLandOrInAir      (PZ: was "SV_AirMove")

`self` = the current bot

=========================================
*/
void frik_botMoveOnLandOrInAir()
{
	vector wishvel, vangle;

	vangle = self->v_angle;
	vangle[X] = vangle[Z] = 0;
	makevectors(vangle);
	if (time < self->teleport_time && (self->movevect[X] < 0))
		self->movevect[X] = 0;
	wishvel = v_right * self->movevect[Y] + v_forward * self->movevect[X];

	if (self->movetype != PR_MOVETYPE_WALK)
		wishvel[Z] = self->movevect[Z];
	else
		wishvel[Z] = 0;

	if (vlen(wishvel) > sv_maxspeed)
		wishvel = normalize(wishvel) * sv_maxspeed;

	if (self->movetype == PR_MOVETYPE_NOCLIP)
		self->velocity = wishvel;
	else if (self->flags & PR_FL_ONGROUND)
	{
		frik_applyFrictionToBot();
		frik_botAccelerateOnGround(wishvel);
	}
	else
		frik_botAccelerateInAir(wishvel);
}

/*
=========================================

frik_clientThink      (PZ: was "SV_ClientThink")

PZ TODO: Give this a better name.

`self` = the current bot

=========================================
*/
void frik_clientThink()
{
	vector vangle;

	if (self->movetype == PR_MOVETYPE_NONE)
		return;

	frik_botDropPunchAngle();

	if (self->health <= 0)
		return;

	// handle the model's orientation angles
	self->v_angle[Z] = 0; // V_CalcRoll removed, sucks
	self->angles[Z] = self->v_angle[Z] * 4;              // PZ NOTE: how about you just set both to 0??
	vangle = self->v_angle + self->punchangle;
	if (!self->fixangle)
	{
		self->angles[X] = (vangle[X] / -3);    // PZ NOTE: pitch (apparently, the negative direction in .angles_x is opposite that of .v_angle_x)
		self->angles[Y] = vangle[Y];           // PZ NOTE: yaw
	}
	else // PZ NOTE: perhaps this is used when you spawn or teleport??
	{
		self->v_angle = self->angles;
		self->fixangle = 0; // PZ NOTE: flag to tell game to set view angle according to model's angle?
	}

	// handle movement
	if (self->flags & PR_FL_WATERJUMP)
	{
		frik_botWaterJump();
		return;
	}
	if ((self->waterlevel >= 2) && (self->movetype != PR_MOVETYPE_NOCLIP))
	{
		frik_botAccelerateInWater();
		return;
	}
	frik_botMoveOnLandOrInAir();
}

/*
=========================================

Stuff mimicking sv_phys.c

=========================================
*/

/*
=========================================

frik_RunThink      (PZ: was "SV_RunThink")

PZ NOTE: Give this a better name.

`self` = the current bot

=========================================
*/
float frik_RunThink()
{
	float thinktime, bkuptime;
	thinktime = self->nextthink;
	bkuptime = time;
	if (thinktime <= 0 || thinktime > (time + real_frametime))
		return PR_TRUE;
	if (thinktime < time)
		thinktime = time;
	self->nextthink = 0;
	time = thinktime;
	other = world;
	makevectors(self->v_angle); // hack
	self->think();
	time = bkuptime;
	return PR_TRUE;
}

/*
=========================================

frik_applyGravityToBot      (PZ: was "SV_AddGravity")

`self` = the current bot

=========================================
*/
void frik_applyGravityToBot(float scale)
{
	self->velocity[Z] = self->velocity[Z] - (scale * sv_gravity * real_frametime);
}

/*
=========================================

frik_botDetermineWaterLevel   (PZ: was "SV_CheckWater")

Sets the .waterlevel and .watertype for `self`.
If water is at least up to about the bot's waist,
returns TRUE. Otherwise, returns FALSE.

`self` = the current bot

=========================================
*/
float frik_botDetermineWaterLevel()
{
	vector point;
	float cont;

	point[X] = self->origin[X];
	point[Y] = self->origin[Y];
	self->waterlevel = 0;
	self->watertype = PR_CONTENT_EMPTY;
	point[Z] = self->origin[Z] + self->mins[Z] + 1; // PZ NOTE: in water at all
	cont = pointcontents(point);
	if (cont <= PR_CONTENT_WATER)
	{
		self->watertype = cont;
		self->waterlevel = 1;
		point[Z] = self->origin[Z] + (self->mins[Z] + self->maxs[Z]) * 0.5;  // PZ NOTE: at least waist or chest level
		cont = pointcontents(point);
		if (cont <= PR_CONTENT_WATER)
		{
			self->waterlevel = 2;
			point[Z] = self->origin[Z] + self->view_ofs[Z];  // PZ NOTE: at least eye level
			cont = pointcontents(point);
			if (cont <= PR_CONTENT_WATER)
				self->waterlevel = 3;
		}
	}
	return (self->waterlevel > 1);
}

/*
=========================================

frik_preventBotThudSound      (PZ: was "RemoveThud")

`self` = the current bot

=========================================
*/
void frik_preventBotThudSound() // well sometimes
{
	entity oself;
	if (other == world)
	{
		if (self->flags & PR_FL_ONGROUND)
		{
			self->flags = self->flags - PR_FL_ONGROUND;
		}
	}
	else
	{
		if (other->solid == PR_SOLID_BSP && (self->flags & PR_FL_ONGROUND))
		{
			// RM: Does this break anything?
			// If not, then some more thuds have been removed.
			self->flags = self->flags - PR_FL_ONGROUND;
		}
		if (other == self->owner)
			return;
		if (self->owner->solid == PR_SOLID_NOT)
			return;
		oself = other;
		other = self->owner;
		self = oself;
		if (self->solid == PR_SOLID_BSP)
			if (self->touch != SUB_Null)
				self->touch();
	}
}

/*
=========================================

frik_botDetermineOnGroundState      (PZ: was "SV_CheckOnGround")

`self` = the current bot

=========================================
*/
void frik_botDetermineOnGroundState()
{
	vector org, v;
	float currentflags;

	org = self->origin;
	currentflags = self->flags;
	self->flags = self->flags | PR_FL_ONGROUND | PR_FL_PARTIALGROUND;
	walkmove(0, 0); // perform C touch function
	self->flags = currentflags | PR_FL_ONGROUND;
	if ((org[X] != self->origin[X]) || (org[Y] != self->origin[Y]))
		org = self->origin;
	else
		self->origin = org;
	v = org;
	v[Z] = self->maxs[Z] + org[Z] + 1;
	traceline(org, v, PR_TL_BSP_ONLY, self);
	if ((self->waterlevel == 3) && (self->movetype == PR_MOVETYPE_WALK))
		self->flags = self->flags - PR_FL_ONGROUND;
	else if ((trace_plane_normal[Z] <= 0.7) && (trace_fraction != 1))
		self->flags = self->flags - PR_FL_ONGROUND;
	//else if (!droptofloor(0,0)) // PZ: what droptofloor() is he using that uses 2 parms? Is it like that in regular Quake?
	else if (!droptofloor())
		self->flags = self->flags - PR_FL_ONGROUND;
	else if (org[Z] - self->origin[Z] < 2)
		self->flags = self->flags | PR_FL_ONGROUND;
	else
		self->flags = self->flags - PR_FL_ONGROUND;
	setorigin(self, org);
}

/*
=========================================

frik_botCheckForStep      (PZ: was "botCheckForStep")

PZ NOTE: Figure out what this is doing and write it here.

`self` = the current bot

=========================================
*/
// PZ: added this; I tried making this really high, but bots still can't go up ladders on bam4. It would be nice if bots had exact same movement code as real players.
#define PR_MAX_STEP_HEIGHT 16

// Thanks to Alan Kivlin for this function
// modified heavily by me
float frik_botCheckForStep(const vector& dir)
{
	vector currentorigin, v;
	float currentflags, yaw, stepdistance, movedistance;
	currentorigin = self->origin;
	currentflags = self->flags;
	self->flags = PR_FL_ONGROUND | PR_FL_PARTIALGROUND;
	dir = normalize(dir);
	dir[Z] = 0;
	yaw = vectoyaw(dir);
	if (walkmove(yaw, 3))
	{
		//if(droptofloor(0,0)) // PZ: what droptofloor() is he using that uses 2 parms? Is it like that in regular Quake?
		if (droptofloor())
		{
			stepdistance = self->origin[Z] - currentorigin[Z];
			v = self->origin - currentorigin;
			v[Z] = 0;
			movedistance = vlen(v);
//bprint(2, "ran frik_botCheckForStep()\n");
			if ((stepdistance > 0 && stepdistance <= PR_MAX_STEP_HEIGHT) && movedistance != 0) // PZ: 'MAX_STEP_HEIGHT' was '16'
			{
				self->flags = currentflags | PR_FL_PARTIALGROUND;
				return 1;
			}
		}
	}
	self->flags = currentflags;
	v = currentorigin;
	v[Z] = self->origin[Z];
	if (vlen(v - self->origin) <= 4)
		setorigin(self, currentorigin);
	return 0;
}

/*
=========================================

frik_botBruteForceStep      (PZ: was "BruteForceStep")

PZ NOTE: Give this a better name.

`self` = the current bot

=========================================
*/
// this is merely here to fix a problem with e3m5
void frik_botBruteForceStep(const vector& dir)
{
	vector currentorigin;
	float currentflags, i = 0, len;

	currentorigin = self->origin;
	currentflags = self->flags;
	len = vlen(dir);
//bprint(2, "ran frik_botBruteForceStep()\n");
	if (len > PR_MAX_STEP_HEIGHT)                    // PZ: 'MAX_STEP_HEIGHT' was '16'
		dir = normalize(dir) * PR_MAX_STEP_HEIGHT;   // PZ: 'MAX_STEP_HEIGHT' was '16'

	setorigin(self, currentorigin + dir);

	while(i < 18 && !walkmove(0, 0))
	{
		self->origin[Z] = currentorigin[Z] + i;
		i = i + 2;
	}
	self->flags = currentflags;
	if (i >=18)
		setorigin(self, currentorigin);
}

/*
=========================================

frik_PostPhysics     (PZ: was "PostPhysics")

PZ NOTE: Give this a better name.

`self` = the current bot

=========================================
*/
void PostPhysics()
{
	vector obstr, org;
	float back, dst,cflags;

	self = self->owner;

	self->velocity = self->velocity - self->phys_obj->dest1 + self->phys_obj->velocity;
	if (self->phys_obj->dest2 == self->origin)
	{
		setorigin(self, self->phys_obj->origin);
		// might've been moved during other person's physics
		// (teleporters / plats)

		if (self->movetype == PR_MOVETYPE_WALK)
		{
			if (self->phys_obj->dest1[X] || self->phys_obj->dest1[Y])
			{
				if ((self->flags & PR_FL_ONGROUND) || (self->waterlevel <= 2))
				{
					obstr = self->phys_obj->movedir - self->origin;
					obstr[Z] = 0;
					if (vlen(obstr) > 0.1)
					{
						dst = vlen(obstr);
						back = vectoyaw(obstr);
						cflags = self->flags;
						self->flags = self->flags | PR_FL_PARTIALGROUND;
						if (walkmove(back, dst))
						{
							self->flags = cflags;
							self->phys_obj->dest1[Z] = 0;
							self->velocity = self->velocity + self->phys_obj->dest1 - self->phys_obj->velocity;
						}
						else
						{
							//if (dst > 1)
							//	frik_botHandleObstruction(obstr, #FALSE); // PZ: Don't do AI code inside the physics module. Keep the code simple and organized.

							org = self->origin;
							self->flags = cflags;
							obstr = self->phys_obj->dest1;
							obstr[X] = 0;
							if (!frik_botCheckForStep(obstr))
							{
								obstr = self->phys_obj->dest1;
								obstr[Y] = 0;
								if (!frik_botCheckForStep(obstr))
								{
									// if no steps were found, bot is really obstucted
									frik_botBruteForceStep(self->phys_obj->dest1);
								}
							}
						}
					}
				}
			}
		}
	}

	frik_botDetermineOnGroundState();

	PlayerPostThink();
	//frik_AI_Main(); // PZ: I can't deal with having calls to this function in 4 different places. Going to try putting it in frik_mainRoutine().
	self->dmg_take = self->dmg_save = 0;
}

/*
=========================================

frik_movePhysObjectThroughAir     (PZ: was "SV_FlyMove")

PZ NOTE: Give this a better name.

`self` = the current bot

=========================================
*/
// Avoid calling frik_AI_Main() and the physics at the same time
// Can trip the runaway loop counter
void frik_movePhysObjectThroughAir()
{
	// This is nothing like the Quake function.

	if (self->phys_obj == world)
	{
		self->phys_obj = find(world,"classname","phys_obj");
		while (self->phys_obj->owner != self)
		{
			self->phys_obj = find(self->phys_obj,"classname","phys_obj");
			if (self->phys_obj == world)
			{
				error("No physics entity spawned!\nMake sure frik_initializeFrikBotModule() was called\n");
			}
		}
	}

	setmodel(self->phys_obj, string_null);
	self->phys_obj->movetype = PR_MOVETYPE_STEP;

	self->phys_obj->solid = PR_SOLID_TRIGGER;
	self->phys_obj->touch = frik_preventBotThudSound;
	setsize(self->phys_obj, self->mins, self->maxs);
	self->phys_obj->dest2 = self->phys_obj->origin = self->origin;
	self->phys_obj->watertype = 0;
	self->phys_obj->movedir = self->origin + real_frametime * self->velocity;
	self->phys_obj->dest1 = self->phys_obj->velocity = self->velocity;
	self->phys_obj->velocity[Z] = self->phys_obj->velocity[Z] + sv_gravity * real_frametime;
	self->phys_obj->flags = 0;
	self->phys_obj->think = PostPhysics;
	self->phys_obj->nextthink = time;
}

/*
=========================================

frik_SV_Physics_Toss     (PZ: was "SV_Physics_Toss")

PZ NOTE: Give this a better name.

`self` = the current bot

=========================================
*/
void frik_SV_Physics_Toss()
{
	if (!frik_RunThink())
		return;
	if (self->flags & PR_FL_ONGROUND)
	{
		self->velocity = V({0, 0, 0});
		//frik_AI_Main(); // PZ: I can't deal with having calls to this function in 4 different places. Going to try putting it in frik_mainRoutine().
		return;
	}
	if (self->movetype != PR_MOVETYPE_FLY)
		frik_applyGravityToBot(1);
	self->angles = self->angles + real_frametime * self->avelocity;
	frik_movePhysObjectThroughAir();
}

/*
=========================================

frik_SV_Physics_Client     (PZ: was "SV_Physics_Client")

PZ NOTE: Give this a better name.
         ***SV_Physics_Client() doesn't exist in QuakeWorld, by the way.***

`self` = the current bot

=========================================
*/
// PZ TODO: this should be simplified; it's way too complicated
void frik_SV_Physics_Client()
{
	PlayerPreThink(); // PZ NOTE: called before bot's physics are ran, for this bot?

	if (self->movetype == PR_MOVETYPE_NONE)
	{
		if (!frik_RunThink())
			return;
		PlayerPostThink(); // PZ NOTE: called after bot's physics are ran, for this bot?
		//frik_AI_Main(); // PZ: I can't deal with having calls to this function in 4 different places. Going to try putting it in frik_mainRoutine().
	}
	else if ((self->movetype == PR_MOVETYPE_WALK) || (self->movetype == PR_MOVETYPE_STEP))
	{
		if (!frik_RunThink())
			return;
		if (!(frik_botDetermineWaterLevel()) && (!(self->flags & PR_FL_WATERJUMP)))
			frik_applyGravityToBot(1);
		frik_movePhysObjectThroughAir();
	}
	else if ((self->movetype == PR_MOVETYPE_TOSS) || (self->movetype == PR_MOVETYPE_BOUNCE))
	{
		frik_SV_Physics_Toss();
	}
	else if (self->movetype == PR_MOVETYPE_FLY)
	{
		if (!frik_RunThink())
			return;
		frik_movePhysObjectThroughAir();
	}
	else if (self->movetype == PR_MOVETYPE_NOCLIP)
	{
		if (!frik_RunThink())
			return;
		self->origin = self->origin + real_frametime * self->velocity; // PZ NOTE: you don't need to use setorigin() instead?

		PlayerPostThink(); // PZ NOTE: called after bot's physics are ran, for this bot?
		//frik_AI_Main(); // PZ: I can't deal with having calls to this function in 4 different places. Going to try putting it in frik_mainRoutine().
	}
	else
		error("frik_SV_Physics_Client(): Bad Movetype (bot)");

	// PZ: the bots' speed is not getting capped correctly (so, I added this; taken from bunny hop limiting code in Prozac)
	/*if (self.flags & #FL_ONGROUND || self.waterlevel > 1 || (self.tf_items & #NIT_HOVER_BOOTS && self.#BUTTON_JUMP))
	{
		local vector tempvel;
		local float tempspeed;

		if (self.maxspeed < 0) self.maxspeed = 0;
		//tempvel_x = self.velocity_x; // we only care about horizontal movement
		//tempvel_y = self.velocity_y;
		//tempvel_z = self.velocity_z; // was "= 0"
		tempvel = self.velocity; // PZ: when they are in water, they can still move up and down too fast
		tempspeed = vlen(tempvel);
		if (tempspeed > self.maxspeed)
		{
			tempspeed = self.maxspeed / tempspeed; // PZ: why not just make the speed equal the .maxspeed? would be abrupt slow-down?
			// (### maybe this is why bots don't seem to slow down much when shot with sniper rifle in legs ###)

			//self.velocity_x = self.velocity_x * tempspeed; // only slow down horizontal movement
			//self.velocity_y = self.velocity_y * tempspeed;
			//self.velocity_z = self.velocity_z * tempspeed; // added this
			self.velocity = self.velocity * tempspeed;
		}
	}*/
	// PZ: END
}

#endif

} // END namespace Progs
