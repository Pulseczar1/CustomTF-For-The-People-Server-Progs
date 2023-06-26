/***********************************************
*                                              *
*            FrikBot Movement AI               *
*     "The slightly better movement AI"        *
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
#include "frikbot/bot_move.h"
#include "frikbot/bot_qw.h"
#include "frikbot/bot_misc.h"
#include "frikbot/bot_ai.h"
#include "frikbot/bot_way.h"
#include "frikbot/bot_fight.h"

namespace Progs {

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_makeBotJump             (PZ: was "bot_jump")

Makes the bot jump.

`self` = the current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_makeBotJump()
{
	// TODO check for precision, etc.
	self->PR_BUTTON_JUMP = PR_TRUE;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_canBotRocketJump         (PZ: was "bot_can_rj")

This returns true if the bot can rocket ("super") jump.
You might want to use this to check for grapple, too.

`e` = the bot in question

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_canBotRocketJump(entity e)
{
	// if your mod doesn't have an RL you can just return FALSE all the time
	// if it has a hook or some other means for the bot to get to high places
	// you can check here for that capability

	// am I dumb?
	//if (e.b_skill == #SKILL_EASY)
	//	return #FALSE;

	// quad = bad
	if (e->items & PR_IT_QUAD)
		return PR_FALSE;

	// do I have rockets and RL?
	if (!((e->weapons_carried & PR_WEAP_ROCKET_LAUNCHER) && (e->ammo_rockets > 0)))  // PZ: made .items .weapons_carried
		return PR_FALSE;

	// do I have pent?
	if (e->items & PR_IT_INVULNERABILITY)
		//return #TRUE;
		return PR_FALSE; // PZ: can't RJ while invulnerable (rocket explosion won't push you)

	if (e->health > 50)
		return PR_TRUE;
	else
		return PR_FALSE;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_detectAndHandlePlatform     (PZ: was "frik_recognize_plat")

Returns true if it detects a platform. False, otherwise.
Can also affect the bot's movement. See below.

`self` = a waypoint or bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_detectAndHandlePlatform(float affectBotMovement)
{
	if (self->classname != "waypoint" && !(self->flags & PR_FL_ONGROUND))
		return PR_FALSE;
	// look right below yourself
	traceline(self->origin, self->origin - V({0, 0, 64}), PR_TL_BSP_ONLY, self);
	if (trace_ent != world)
	{
		if (affectBotMovement) // affect bot movement, too
		{
			// PZ NOTE: if the trace_ent is moving up or down, it's likely a platform (right?)
			if (self->keys & PR_KEY_MOVEUP)
			{
				if (trace_ent->velocity[Z] > 0)
					self->keys = self->keys & PR_KEY_MASK_LOOKING_KEYS; // turn off movement keys
			}
			else if (self->keys & PR_KEY_MOVEDOWN)
			{
				if (trace_ent->velocity[Z] < 0)
					self->keys = self->keys & PR_KEY_MASK_LOOKING_KEYS; // turn off movement keys
			}
		}
		return PR_TRUE;
	}
	else
		return PR_FALSE;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_getRequiredKeysToMoveThisDirection  (PZ: was "frik_KeysForDir")

Returns the keys that should be pressed by the bot
to move in `movementDirection`.

`self` = a bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_getRequiredKeysToMoveThisDirection(const vector& movementDirectionVector)
{

	vector movementDirectionAngles;
	float outputKeys, tempAngle;
	outputKeys = 0;
	// horizontal movement
	if (movementDirectionVector[X] || movementDirectionVector[Y])
	{
		// Everything is tested against 60 degrees.
		// This allows the bot to overlap the keys
		// 30 degrees on each diagonal. 45 degrees
		// might look more realistic.
		// PZ TODO: don't allow easy bot to strafe?
 		movementDirectionAngles = vectoangles(movementDirectionVector);
		tempAngle = frik_angleSubtraction(movementDirectionAngles[Y], self->v_angle[Y]); // yaw
		if ((tempAngle <= 150) && (tempAngle >= 30))
			outputKeys = outputKeys + PR_KEY_MOVELEFT;
		else if ((tempAngle >= -150) && (tempAngle <= -30))
			outputKeys = outputKeys + PR_KEY_MOVERIGHT;
		// can strafe and move forward/backward simultaneously
		if (fabs(tempAngle) <= 60)
			outputKeys = outputKeys + PR_KEY_MOVEFORWARD;
		else if (fabs(tempAngle) >= 120)
			outputKeys = outputKeys + PR_KEY_MOVEBACK;
	}
	// vertical movement
	if (movementDirectionVector[Z] > 0)
		outputKeys = outputKeys + PR_KEY_MOVEUP;
	else if (movementDirectionVector[Z] < 0)
		outputKeys = outputKeys + PR_KEY_MOVEDOWN;

//if (outputKeys & #KEY_MOVEDOWN)
//bprint(2, "trying to use the movedown key\n");

	return outputKeys;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_checkForStuckBot

PZ: This is called by the bot's stuck detection timer every few seconds.
If it determines that the bot is stuck, it will have the bot drop
its target1.

`self`              = bot's stuck detection timer
`self.owner`        = the bot this timer runs for
`self.origin`       = the bot's previous position
`self.count`        = the number of strikes counting toward determining whether bot is stuck
`self.groundentity` = the platform the bot was last on

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_checkForStuckBot()
{
	//local float distMoved;
	float prevDistToTarget, currDistToTarget;

	//local vector positionRelativeToTarget;
	entity oldSelf, bot, goalTarget, currentPlatform = world;
	float timeLimit, speedOfBot;
	bool isBotStuckThisFrame = false;

	bot = self->owner;

	// don't run on bots that aren't, or shouldn't be trying to, move right now
	if (bot->is_feigning || (bot->job & PR_JOB_THIEF && bot->job & /*#JOB_ACTIVE*/PR_JOB_FULL_HIDE))
	{
		// reset the stuck checker
		self->count = 0; // Bot isn't trying to move right now. So clear out all strikes.
		self->origin = bot->origin; // remember the bot's current position
		self->nextthink = time + 0.5; // check again in 0.5 seconds // MAKE SURE THIS IS ALWAYS DONE OR THE STUCK DETECTOR WILL STOP RUNNING
		return;
	}

	// determine whether bot is currently on a platform
	oldSelf = self;
	self = bot;
	if (frik_detectAndHandlePlatform(PR_FALSE))
		currentPlatform = trace_ent;
	self = oldSelf;

	if (bot->b_aiflags & PR_AI_WAIT) timeLimit = 30;
	else                             timeLimit = 5;
	if (currentPlatform != world)    timeLimit = 50;

	speedOfBot = vlen(bot->velocity);

	// if the bot is trying to move, or is currently waiting for something
	// FIXME: If I have the code like this, bots get stuck in back and forth over waypoint above their head, because they stop every so often to change direction,
	//        at which point, the counter here is reset, because they aren't trying to move for a moment. If I take this condition out, then they consider simply
	//        turning during navigation as a time to check for being stuck.
	if (/*1*/ (bot->keys & PR_KEY_MASK_MOVEMENT_KEYS || speedOfBot > 1) || /*!*/(bot->b_aiflags & PR_AI_WAIT) || currentPlatform != world)
	{
/*oldSelf = self;
self = bot;
frik_botSayDebug("Trying to move.\n");
self = oldSelf;*/
		//isBotStuckThisFrame = PR_FALSE;

		// OLD CODE: This was checking how much the bot moved over time. The new code checks whether the bot is getting any closer to its target1.
		//           The new code makes it so that bots don't get stuck as much chasing things (back and forth), that are above them.
		/*distMoved = vlen(self.origin - bot.origin);
		//positionRelativeToTarget = bot.target1.origin - bot.origin;
		positionRelativeToTarget_x = fabs(bot.target1.origin_x - bot.origin_x);
		positionRelativeToTarget_y = fabs(bot.target1.origin_y - bot.origin_y);
		positionRelativeToTarget_z = fabs(bot.target1.origin_z - bot.origin_z);

		// if he's moved very little or on the same platform he was on a half second ago
		if (distMoved < (32 * 3) || (currentPlatform != world && currentPlatform == self.groundentity))   // the width of 3 players
		{
			isBotStuckThisFrame = #TRUE;
			self.count = self.count + 1;
		}
		// if his target is directly above or below him
		//if (positionRelativeToTarget_x < 32 && positionRelativeToTarget_y < 32)  // doesn't work
		if (positionRelativeToTarget_z // INSTEAD, LOOK FOR DIFFERENCE IN Z DISTANCE TO TARGET1.
		{
			isBotStuckThisFrame = #TRUE;
			self.count = self.count + 2;
		}*/

		// NEW CODE: Is the bot getting any closer to its target1? If not, consider it stuck this frame.
		prevDistToTarget = vlen(bot->target1->origin - self->origin);
		currDistToTarget = vlen(bot->target1->origin - bot->origin);
		if (currDistToTarget /*+ 32*/ >= prevDistToTarget) // Tweak the number if needed. To make bots consider themselves stuck more often, increase it. Decrease it to do the opposite.
		{
			isBotStuckThisFrame = PR_TRUE;
			self->count = self->count + 1;
		}

		if (isBotStuckThisFrame)
		{
			//self.count = self.count + 1;
			// The bot might be able to jump over what's in his way.                          // don't do it while swimming
			if (self->count >= 2 && currentPlatform == world && !(bot->b_aiflags & PR_AI_WAIT) && (bot->flags & PR_FL_ONGROUND)) // No jumping if waiting for something, though.
			{
				// TODO DONE?: eliminate or decrease chance that they jump while waiting (did it above)
				//bot.#BUTTON_JUMP = #TRUE;
				bot->keys = bot->keys | PR_KEY_AUX_JUMP; // the C code wipes .#BUTTON_JUMP (.button2) before the bot's code gets the flag
			}
			if (self->count >= timeLimit) // considered stuck; try to get unstuck
			{
				oldSelf = self;
				self = bot;
//if (1/*PZ_DEBUG == self.b_clientno*/) frik_botSayDebug("I'm stuck. Dropping my goal target.\n");
				goalTarget = frik_getGoalTarget(); // the highest numbered target
				frik_removeFromTargetStack(goalTarget); // was removing target1
				// PZ: added this stuff on 3-23-15
				self->b_aiflags = PR_AI_REJECT_TARGET; // throw away all flags and set this one, to keep from pursuing the same target again at this time
				frik_clearRouteForSelf();
				self->target1 = self->target2 = self->target3 = self->target4 = self->enemy = world;
				self->last_way = self->current_way = world;
				// PZ: END
				self = oldSelf;
				// reset the stuck checker
				self->count = 0; // Bot isn't trying to move right now. So clear out all strikes.
				self->origin = bot->origin; // remember the bot's current position
			}
		}
		else
		{
			// reset the stuck checker
			self->count = 0; // Bot isn't trying to move right now. So clear out all strikes.
			self->origin = bot->origin; // remember the bot's current position
		}
	}
	else
	{
		// reset the stuck checker
		//self.count = 0; // Bot isn't trying to move right now. So clear out all strikes.
		if (self->count > 0) self->count = self->count - 1; // Tick down, instead, in case the bot is only stopping movement for a second.
		self->origin = bot->origin; // remember the bot's current position
	}

	// remember the last platform/train the bot was on
	//oldSelf = self;
	//self = bot;
	//frik_detectAndHandlePlatform(#FALSE);
	//self.groundentity = trace_ent;
	self->groundentity = currentPlatform;
	//self = oldSelf;

	self->nextthink = time + 0.5; // check again in 0.5 seconds // MAKE SURE THIS IS ALWAYS DONE OR THE STUCK DETECTOR WILL STOP RUNNING
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botLookForAndHandleObstacles   (PZ: was "frik_obstacles")

Detects things in the way of the bot, like things the bot needs to jump over,
drops the bot should avoid falling into, and objects the bot needs to navigate
around.

Any `return` here will result in the bot not detecting any sort
of obstacle. The bot will continue moving in its direction of travel.

`self` = a bot

### PZ TODO: Rewrite this function. It's very messy and I don't think the logic is very good.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_botLookForAndHandleObstacles()
{
	vector start, stop, ang, direction, v;
	float test, contents, dist, heightMeasurement;
	entity e;

	//if (!(self.flags & #FL_ONGROUND)) // PZ: not sure why this is here
	//	return;

	// [Automatic detection and handling of interactive obstacles, like doors and teleporters.]
	// PZ: Handle teleporters, and perform automatic detection of doors and other such obstacles.
	// 'Automatic' means that the bot wasn't explicitly told by waypoint AI flags to do something. The bot
	// can be explicitly told that there is a door, for example, but if he isn't, it's caught here, if possible.
	if (self->current_way != world)
	{
		if (frik_doesWaypointLinkToWaypoint(self->current_way, self->target1) == 2) // are they linked by teleporter link?
		{
			if (self->b_aiflags & (PR_AI_TELELINK_1 | PR_AI_TELELINK_2 | PR_AI_TELELINK_3 | PR_AI_TELELINK_4))
			{
//bprint(2, "#### ATTEMPTING TO ADD TELEPORTER TO TARGET STACK\n");
				e = frik_findNearestEntityToSelfWithThisClassname("trigger_teleport"); // this is probably the teleporter associated with the telelink
				frik_addToTargetStack(e); // PZ NOTE: new target1 now; old one has been pushed down to target2 until this target is reached
				// we've handled the telelink AI flag; so remove it
				self->b_aiflags = self->b_aiflags - (self->b_aiflags & (PR_AI_TELELINK_1 | PR_AI_TELELINK_2 | PR_AI_TELELINK_3 | PR_AI_TELELINK_4));
			}
		}
		// [Look for certain things blocking the path from our current waypoint to target1.]
		// should always use the 'real origin' because target1 could be something, like a teleporter, that doesn't have an origin vector
		v = frik_getRealOriginOfEntity(self->target1);
		traceline(self->current_way->origin, v, PR_TL_ANY_SOLID, self);
		if (trace_fraction != 1) // something is in the way
		{
			// a door blocks the way
			if (trace_ent->classname == "door" && !(self->b_aiflags & PR_AI_DOOR_NO_OPEN) && !(self->b_aiflags & PR_AI_HANDLING_OBSTRUCTION) &&
			    !(self->b_aiflags & PR_AI_RIDE_TRAIN)) // PZ: if we are currently dealing with a train/platform, don't perform door handling logic
			{
				self->b_aiflags = self->b_aiflags | PR_AI_HANDLING_OBSTRUCTION; // PZ: we don't want to run this code more than once
				// linked doors fix
				if (trace_ent->owner != world) // have a button?
					trace_ent = trace_ent->owner;
				// PZ: allow us to supercede our .enemy for a moment, to shoot the button (TODO: need enemy stack, like target stack?)
				if ((trace_ent->health > 0) && (self->enemy == world)) // shootable door or shootable button?
				{
					self->enemy = trace_ent;
					v = frik_getRealOriginOfEntity(self->enemy);
					dist = vlen(v - self->origin);
					frik_makeWeaponSelection(dist, PR_FALSE);   // PZ: gave it the range to target
					self->b_aiflags = self->b_aiflags | PR_AI_BLIND; // PZ NOTE: I guess this is to make sure he goes for it and isn't distracted
				}
				else if (trace_ent->targetname != "") // if the door was not shootable, does it have a button?
				{
					e = find(world, "target", trace_ent->targetname); // PZ NOTE ###: is "target" the correct field name?
					if (e->health > 0)  // shootable
					{
						self->enemy = e;
						v = frik_getRealOriginOfEntity(self->enemy);
						dist = vlen(v - self->origin);
						frik_makeWeaponSelection(dist, PR_FALSE);  // PZ: gave it the range to target
					}
					else               // non-shootable button
					{
						frik_addToTargetStack(e); // it's now target1; previous target1 is now target2
						self->b_aiflags = self->b_aiflags | PR_AI_BLIND; // PZ NOTE: I guess this is to make sure he goes for it and isn't distracted
						//return;
					}
				}
			}
			else if (trace_ent->classname == "func_wall")
			{
				// give up
				//frik_dropTarget(self.target1, #FALSE); // PZ: this does a bunch of extra stuff
				frik_removeFromTargetStack(self->target1); // PZ: just do this for now
				return;
			}
		}
	}

	// PZ: This was at the top of the file and was messing up teleporters. For right now, I'm going to put this here. May want to change at some point.
	if (self->b_aiflags & PR_AI_BLIND) // PZ: when blind flag is set, he's to just move straight toward his target1
		return;

	// [Look for all other obstacles, like walls, drops/lava, and friendly/non-combatant entities.]
	// ### THIS DOESN'T NEED TO BE PERFECT FOR RIGHT NOW. JUST WORRY ABOUT MAKING SOMETHING THAT WORKS DECENTLY. ###
	// PZ TODO: use the code below as a reference, but start from scratch here

	float centerBlocked = false, upperLeftCornerBlocked = false, /*lowerLeftCornerBlocked,*/ upperRightCornerBlocked = false/*, lowerRightCornerBlocked*/;
	vector selfTrueOrigin, targetTrueOrigin, anglesToTarget, orig_v_right;

	// [Compute geometric data.]
	selfTrueOrigin = frik_getRealOriginOfEntity(self);
	targetTrueOrigin = frik_getRealOriginOfEntity(self->target1);
	direction = targetTrueOrigin - selfTrueOrigin;
	if (direction[Z] < 0)
		direction[Z] = 0; // MAKE THIS NOT TAKE INTO ACCOUNT Z DIRECTION, JUST HORIZONTAL.
	dist = vlen(direction);
	anglesToTarget = vectoangles(direction);

	//makevectors(self.angles); // not based on bot's facing direction, but instead his direction to target
	makevectors(anglesToTarget);
	orig_v_right = v_right;
	direction = normalize(direction); // make values from 0 to 1

	// [Determine whether there is an object that we need to jump over in the way.]
	//ang = normalize(self.velocity);
	//ang_z = 0;
	start = self->origin + (direction * 18); // look 32 units ahead, in the direction of your target1 // changed to 18  (1/8/16)
	start[Z] = start[Z] + self->maxs[Z]; // PZ NOTE: level with where the top of our head will be
	stop = start;
	stop[Z] = (stop[Z] - self->maxs[Z]) + self->mins[Z]; // PZ NOTE: level with where the bottoms of our feet will be
	// PZ: Do this original traceline from above, here. Now it is looking for objects in the way.
	traceline(start, stop - V({0, 0, 256}), PR_TL_BSP_ONLY, self);
	heightMeasurement = trace_endpos[Z] - stop[Z];
	if (heightMeasurement > 18) // PZ NOTE: 18 is the max height a player can step up. See define `STEPSIZE` in the C code.
		if (self->flags & PR_FL_ONGROUND) // not if swimming
			frik_makeBotJump();
	// TODO: What if it's less than 0? A drop.




	// [Is there anything between us and our .target1? Draw 5 lines to the target -- from the center of the bot and from all four corners.]
	if (dist > 32*2)
		dist = 32*2; // let's just check about 2 player characters ahead for right now
	// center
	//traceline(selfTrueOrigin, targetTrueOrigin, #TL_ANY_SOLID, self);
	//traceline(selfTrueOrigin, selfTrueOrigin + (direction * dist), #TL_ANY_SOLID, self);
	traceline(self->origin, self->origin + (direction * dist), PR_TL_ANY_SOLID, self);
	if (trace_fraction != 1 && trace_ent != self->target1) // something is in the way
	{
		centerBlocked = PR_TRUE;
	}
	// PZ TODO: test shooting a gun or just launching an entity with these same vectors, so that I can see exactly where it's tracing lines
	// Using the railgun works well for this purpose. Code is in engineer.qc: W_FireLaser().
	// make an impulse to do this and make it shoot from myself?
	// upper left corner
	//start_x = self.origin_x + (v_right_x * self.mins_x);
	//start_y = self.origin_y + (v_right_y * self.maxs_y);
	start = self->origin + (v_right * (self->mins[X] + 1));
	start[Z] = self->origin[Z] /*+ self.maxs_z - 2*/;
	traceline(start, start + (direction * dist), PR_TL_ANY_SOLID, self);
	if (trace_fraction != 1 && trace_ent != self->target1) // something is in the way
	{
		upperLeftCornerBlocked = PR_TRUE;
	}
	// lower left corner
	start[Z] = self->origin[Z] + self->mins[Z] + 19;  // according to original FrikBot code, it looks like 18 may be the max step height
	traceline(start, start + (direction * dist), PR_TL_ANY_SOLID, self);
	if (trace_fraction != 1 && trace_ent != self->target1) // something is in the way
	{
		//lowerLeftCornerBlocked = PR_TRUE;
	}
	// lower right corner
	//start_x = self.origin_x + (v_right_x * self.maxs_x);
	//start_y = self.origin_y + (v_right_y * self.maxs_y);
	start = self->origin + (v_right * (self->maxs[X] - 1));
	start[Z] = self->origin[Z] + self->mins[Z] + 19;  // according to original FrikBot code, it looks like 18 may be the max step height
	traceline(start, start + (direction * dist), PR_TL_ANY_SOLID, self);
	if (trace_fraction != 1 && trace_ent != self->target1) // something is in the way
	{
		//lowerRightCornerBlocked = PR_TRUE;
	}
	// upper right corner
	start[Z] = self->origin[Z] /*+ self.maxs_z - 2*/;
	traceline(start, start + (direction * dist), PR_TL_ANY_SOLID, self);
	if (trace_fraction != 1 && trace_ent != self->target1) // something is in the way
	{
		upperRightCornerBlocked = PR_TRUE;
	}

/*if (upperLeftCornerBlocked || lowerLeftCornerBlocked)
	frik_botSay("BLOCKED LEFT\n");
if (upperRightCornerBlocked || lowerRightCornerBlocked)
	frik_botSay("BLOCKED RIGHT\n");
if (centerBlocked)
	frik_botSay("BLOCKED CENTER\n");*/

	// [Make bot attempt to move around obstacle.]
	if      ((upperLeftCornerBlocked /*|| lowerLeftCornerBlocked*/) && (!upperRightCornerBlocked /*&& !lowerRightCornerBlocked*/))
	{
		// move right
//		frik_botSay("blocked left; moving right\n");
		v_right[Z] = 0;
		self->keys = frik_getRequiredKeysToMoveThisDirection(v_right);
		self->b_aiflags = self->b_aiflags | PR_AI_WAIT; // don't allow frik_botPursueTarget1() to move us
	}
	else if ((upperRightCornerBlocked /*|| lowerRightCornerBlocked*/) && (!upperLeftCornerBlocked /*&& !lowerLeftCornerBlocked*/))
	{
		// move left
//		frik_botSay("blocked right; moving left\n");
		v_right[Z] = 0;
		self->keys = frik_getRequiredKeysToMoveThisDirection(v_right * -1);
		self->b_aiflags = self->b_aiflags | PR_AI_WAIT; // don't allow frik_botPursueTarget1() to move us
	}
	else if ((upperLeftCornerBlocked /*|| lowerLeftCornerBlocked*/) && (upperRightCornerBlocked /*|| lowerRightCornerBlocked*/))
	{
//		frik_botSay("blocked on both sides; ");
		// Take a sampling further to the right and to the left. Whichever has the longest line out determines which way you go.
		// If one way is just as good as another, just stop. Bot will soon consider himself stuck, and look for another target1.
		vector angleToLook;
		float distanceOnRight, distanceOnLeft;
		angleToLook = anglesToTarget;
		// look to the right 30 degrees
		angleToLook[Y] = angleToLook[Y] - 30;
		angleToLook[Y] = frik_getAngleMod180(angleToLook[Y]);
		makevectors(angleToLook);
		traceline(self->origin, self->origin + (v_forward * 99999), PR_TL_ANY_SOLID, self);
		distanceOnRight = trace_fraction;
		// look to the left 30 degrees
		angleToLook[Y] = angleToLook[Y] + 60; // extra 30 to undo the last 30
		angleToLook[Y] = frik_getAngleMod180(angleToLook[Y]);
		makevectors(angleToLook);
		traceline(self->origin, self->origin + (v_forward * 99999), PR_TL_ANY_SOLID, self);
		distanceOnLeft = trace_fraction;
		if      (distanceOnRight > distanceOnLeft)
		{
			// move right
//			frik_botSay("path is more open on the right side; moving right\n");
			orig_v_right[Z] = 0;
			self->keys = frik_getRequiredKeysToMoveThisDirection(orig_v_right);
			self->b_aiflags = self->b_aiflags | PR_AI_WAIT; // don't allow frik_botPursueTarget1() to move us
		}
		else if (distanceOnLeft > distanceOnRight)
		{
			// move left
//			frik_botSay("path is more open on the left side; moving left\n");
			orig_v_right[Z] = 0;
			self->keys = frik_getRequiredKeysToMoveThisDirection(orig_v_right * -1);
			self->b_aiflags = self->b_aiflags | PR_AI_WAIT; // don't allow frik_botPursueTarget1() to move us
		}
		else
		{
			// just drop this target
//			frik_botSay("path is not better on either side; dropping target\n");
			frik_removeFromTargetStack(self->target1);
		}
	}
	else if (centerBlocked) // but not sides
	{
		// move either direction (MOVES RIGHT FOR RIGHT NOW)
//		frik_botSay("blocked center; moving right\n");
		v_right[Z] = 0;
		self->keys = frik_getRequiredKeysToMoveThisDirection(v_right);
		self->b_aiflags = self->b_aiflags | PR_AI_WAIT; // don't allow frik_botPursueTarget1() to move us
	}


// PZ: OLD CODE BELOW #######################################################################################
return;

	ang = normalize(self->velocity);
	ang[Z] = 0;
	start = self->origin + (ang * 32); // look 32 units ahead, in your traveling direction
	start[Z] = self->origin[Z] + self->maxs[Z]; // PZ NOTE: level with the top of our head
	stop = start;
	stop[Z] = self->origin[Z] + self->mins[Z]; // PZ NOTE: level with our feet
	// PZ NOTE: How the hell is this not considered an obstacle? It's not just checking below.
	//          It's checking in the bot's path, and then down several feet.
	//traceline(start, stop - '0 0 256', #TL_BSP_ONLY, self);
	// PZ: Since here it wants to know whether there is ground to walk on, not whether there is an object in the way,
	//     let's make it just check the ground in front.
	// PZ NOTE: ######### THIS IS BAD. IT JUST CHECKS THAT THE FLOOR IS GOOD AND IF IT IS, IT SAYS THERE MUST BE NO OBSTACLE INTHE WAY. #######
	traceline(stop, stop - V({0, 0, 256}), PR_TL_BSP_ONLY, self);
	if (trace_allsolid || trace_startsolid) // if it's all solid ground below this spot, there's no gap/obstacle
		return;

	// PZ: Do this original traceline from above, here. Now it is looking for objects in the way.
	traceline(start, stop - V({0, 0, 256}), PR_TL_BSP_ONLY, self);
	heightMeasurement = trace_endpos[Z] - stop[Z];
	if (heightMeasurement > 18)
	{
		frik_makeBotJump();
		return;
	}
	if (heightMeasurement >= 0)
		return; // do nothing; no obstacle (PZ NOTE: apparently he can step up this)

	// what is right above the surface we (may) have found? lava?
	contents = pointcontents(trace_endpos + V({0, 0, 4}));

	// PZ NOTE: check for gap that you can step over
	start = stop - V({0, 0, 8});    // PZ NOTE: 8 units below our feet
	stop = start + ang * 256;  // PZ NOTE: 8 units below our feet, and 256 units into our direction of travel
	traceline(start, stop, PR_TL_BSP_ONLY, self);
	test = vlen(trace_endpos - start);
	if (test <= 20)
		return; // it's a walkable gap, do nothing
	// PZ NOTE: measure the width of any gap
	ang[X] = self->velocity[Y] * -1;
	ang[Y] = self->velocity[X];
	ang = normalize(ang);
	traceline(start - (ang * 10), start + (ang * 10), PR_TL_BSP_ONLY, self);
	if ((trace_fraction != 1) || trace_startsolid)
		return; // gap is only 20 wide, walkable

	ang = self->velocity;
	ang[Z] = 0;
	// PZ NOTE: I think this 540 should be .maxspeed. I think his 540 is close to a middle ground of 550 between 700 (max side speed) and 400 (max forward speed).
	// He probably chose 540 to be a little conservative in judging jumps. If bot does a strafing jump, he may make 540, but a plain forward jump will make
	// him miss, because it can only achieve 400 speed. I need to make this based on the bot's actual .maxspeed, because it varies in TF.
	// Instead of using, 540 or .maxspeed, should we just use .velocity? Could that 540 be the jumping speed, that will negate gravity? Jumping velocity is only
	// 270, though (see PlayerJump()). 270*2 is 540.
	dist = ((540 / sv_gravity) * vlen(ang))/* + 32*/;  // PZ TODO: this should take the bot's movement speed into account (which I think it does)
	if (test > dist) // I can't make it
	{
		if (contents < -3) // bad stuff down dare
		{
			frik_botHandleObstruction(ang, PR_TRUE);
			return;
		}
		else
		{
			if (self->target1 != world)
			{
				stop = frik_getRealOriginOfEntity(self->target1);
				if ((stop[Z] - self->origin[Z]) < -32)
					return; // safe to fall (PZ NOTE: falling will take us closer to out target, possibly)
			}
			frik_botHandleObstruction(ang, PR_FALSE);
			return;
		}
	}
	else  // bot can make the jump
	{
		ang = normalize(ang);
		// look for a ledge
		traceline(self->origin, self->origin + (ang * (test + 20)), PR_TL_BSP_ONLY, self);
		if (trace_fraction != 1)
		{
			if (contents < -3) // bad stuff down dare
			{
				frik_botHandleObstruction(ang, PR_TRUE);
				return;
			}
			else
			{
				if (self->target1 != world)
				{
					stop = frik_getRealOriginOfEntity(self->target1);
					if ((stop[Z] - self->origin[Z]) < -32)
						return; // safe to fall (PZ NOTE: falling will take us closer to out target, possibly)
				}
				frik_botHandleObstruction(ang, PR_FALSE);
				return;
			}
		}

		if (self->target1 != world)
		{
			// getting further away from my target?
			test = vlen(self->target1->origin - (ang + self->origin));
			if (test > vlen(self->target1->origin - self->origin))
			{
				if (contents < -3) // bad stuff down dare
				{
					frik_botHandleObstruction(ang, PR_TRUE);
					return;
				}
				else
				{
					frik_botHandleObstruction(ang, PR_FALSE);
					return;
				}
			}
		}
	}
	// PZ NOTE: Is there a hole in front of us (that we can't jump out of) that has not been handled yet?
	if (heightMeasurement < -18)
	{
		if (contents < -3) // bad stuff down dare
		{
			frik_botHandleObstruction(ang, PR_TRUE);
			return;
		}
		if (self->target1 != world)
		{
			stop = frik_getRealOriginOfEntity(self->target1);
			if ((stop[Z] - self->origin[Z]) < -32)
				return; // safe to fall (PZ NOTE: falling will take us closer to out target, possibly)
		}
		frik_makeBotJump();
	}
	// go for it
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botHandleObstruction      (PZ: was "frik_obstructed")

Bot has hit a ledge or wall that he should
manuever around. Sets the bot's .wallhug, to
strafe around obstructions.

`self` = a bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_botHandleObstruction(const vector& directionOfObstruction, float dangerInOurPath)
{
	float dist;
	vector direction = V({0, 0, 0}), positionOfTarget1;
	// TODO: something
	if (self->b_aiflags & PR_AI_BLIND)
		return;

	positionOfTarget1 = frik_getRealOriginOfEntity(self->target1);
	if (dangerInOurPath)
	{
		self->b_aiflags = self->b_aiflags | PR_AI_DANGER;
		self->keys = frik_getRequiredKeysToMoveThisDirection(V({0, 0, 0}) - directionOfObstruction); // PZ NOTE: makes bot move backwards?
	}
	if (self->b_aiflags & PR_AI_PRECISION)
		return;                         // don't deviate at all from the waypoint pathway

	if (self->target1 != world) // if we have a primary target to pursue
	{
		if (self->b_aiflags & PR_AI_HANDLING_OBSTRUCTION)
		{
			if (!(self->b_aiflags & PR_AI_DANGER))
			{
				self->b_aiflags = self->b_aiflags - PR_AI_HANDLING_OBSTRUCTION;
				return;
			}
			else if (!dangerInOurPath)
				return;
		}
		self->b_obstructionDirection = directionOfObstruction;
		direction[X] = directionOfObstruction[Y] * -1;
		direction[Y] = directionOfObstruction[X];
		dist = vlen(positionOfTarget1 - (self->origin + direction));
		direction[X] = directionOfObstruction[Y];
		direction[Y] = directionOfObstruction[X] * -1;
		self->wallhug = vlen(positionOfTarget1 - (self->origin + direction)) > dist;
		self->b_aiflags = self->b_aiflags | PR_AI_HANDLING_OBSTRUCTION;
	}
	else // no target1
	{
		direction[X] = directionOfObstruction[Y] * -1;
		direction[Y] = directionOfObstruction[X];
		dist = vlen(direction - self->b_obstructionDirection);
		direction[X] = directionOfObstruction[Y];
		direction[Y] = directionOfObstruction[X] * -1;
		self->wallhug = vlen(direction - self->b_obstructionDirection) < dist;
		self->b_obstructionDirection = directionOfObstruction;
		self->b_aiflags = self->b_aiflags | PR_AI_HANDLING_OBSTRUCTION;
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botHandleObstructedFlag    (PZ: was "frik_dodge_obstruction")

After frik_botHandleObstruction(), the bot uses the
following function to move "around" the obstacle.

I have no idea how well it will work.

`self` = a bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_botHandleObstructedFlag()
{
	vector way, org;
	float oldFlags, yaw;

	if (!(self->b_aiflags & PR_AI_HANDLING_OBSTRUCTION)) // PZ NOTE: we are handling the AI_HANDLING_OBSTRUCTION flag
		return;
	if ((self->b_aiflags & (PR_AI_BLIND | PR_AI_PRECISION)) || !(self->flags & PR_FL_ONGROUND))
	{
		self->b_aiflags = self->b_aiflags - PR_AI_HANDLING_OBSTRUCTION;
		return;
	}

	// Perform a walkmove() check to see if the b_obstructionDirection is still obstructed.
	// walkmove() is less forgiving than frik_botLookForAndHandleObstacles(). So, I dunno
	// how well this will work.
	oldFlags = self->flags;
	org = self->origin;

	yaw = vectoyaw(self->b_obstructionDirection);
	if (walkmove(yaw, 32)) // PZ NOTE: does this actually make the bot move, or just performs a test? Looks like just a test, because it then restores origin below.
		self->b_aiflags = self->b_aiflags - PR_AI_HANDLING_OBSTRUCTION;
	else
	{
		if (self->b_aiflags & PR_AI_DANGER)
		{
			way = V({0, 0, 0}) - self->b_obstructionDirection; // PZ NOTE: go opposite direction?
		}
		else if (self->wallhug) // PZ NOTE: I think these two cases make the bot strafe left or right in reference to the obstruction
		{
			way[X] = self->b_obstructionDirection[Y] * -1;
			way[Y] = self->b_obstructionDirection[X];
		}
		else
		{
			way[X] = self->b_obstructionDirection[Y];
			way[Y] = self->b_obstructionDirection[X] * -1;
		}
		self->keys = (self->keys & PR_KEY_MASK_LOOKING_KEYS) + frik_getRequiredKeysToMoveThisDirection(way); // set its movement keys to make it move
	}

	// fix the bot
	// PZ NOTE ####### This teleporting business right here might be what's getting bots stuck inside each other.
	self->origin = org; // PZ NOTE: Also, I have read that you aren't supposed to do it this way. It messes with the physics. You are supposed to use setorigin().
	self->flags = oldFlags;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

movetogoal() and walkmove() replacements

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botPursueTarget1       (PZ: was "frik_movetogoal")

Moves the bot toward its .target1 entity -- its
primary object of pursuit. Can be a waypoint
or any other entity in the game.

`self` = a bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_botPursueTarget1()
{
	vector v/*, v2, start, stop, ang*/;
	float g;
	//entity goalTarget;

	if (self->target1 == world)
	{
		/*makevectors(self.v_angle);    // PZ: Took this out.
		frik_botMoveThisWay(v_forward); */  // PZ NOTE: simply move forward if we don't have a target1?
		if (!self->b_pursuingEnemy)
			self->keys = self->keys & PR_KEY_MASK_LOOKING_KEYS; // stop moving
		return;
	}

	// PZ: no, we can't have these getting set over and over; we'll never be able to consider them handled
	/*if (self.target1.classname == "waypoint")
	{
		// Readahead types are AI instructions to perform while heading to a waypoint.
		self.b_aiflags = self.b_aiflags | (self.target1.b_aiflags & #AI_ON_PURSUING_WAYPOINT);
	}*/

	// [Check whether we've reached target1 yet. Some things, like teleporters, are determined elsewhere.
	//  For instance, we've reached a teleporter, when we're being teleported by the teleporter.]

	// PZ: If we are pursuing our team's flag, we just need to see the flag; we don't need to touch it. (This is the bot checking on the flag -- hanging around it.)
	/*goalTarget = frik_getGoalTarget(); // the highest numbered target
	if (goalTarget.classname == "item_tfgoal" && goalTarget.model != string_null && goalTarget.team_no != self.team_no && goalTarget.team_no != 0)
	{
		if (frik_isVisible_Simple(goalTarget)) // we are in a position to see the flag
		if (frik_isInFovOfSelf(goalTarget))    // we /can/ see it, if we're close enough
		{
			v2 = frik_getRealOriginOfEntity(goalTarget) - self.origin; // get direction/distance to our flag
			if (vlen(v2) <= 30 * #ONE_FOOT)           // we've seen the flag (close enough)
			{
				frik_removeFromTargetStack(goalTarget);
				return;
			}
		}
	}*/
	// [Check whether we've reached target1 yet. Some things, like teleporters, are determined elsewhere.
	//  For instance, we've reached a teleporter, when we're being teleported by the teleporter.]
	// If we are not pursuing our flag, then we need to actually basically touch target1 to consider ourselves having reached it.
	v = frik_getRealOriginOfEntity(self->target1) - (self->origin + self->view_ofs); // get direction to target1  // added view_ofs (1/8/16)
	// Some items are not detected as reached according to how close the bot gets, but according to something else, like whether the bot triggers the item.
	if (self->target1->classname != "trigger_teleport" && self->target1->classname != "func_button")
	{
		if (vlen(v) < 40) // we've reached target1  // PZ: that 40 was 25; 25 wouldn't work
		{
			if (self->target1->classname == "waypoint")
			{
				self->last_way = self->current_way;
				self->current_way = self->target1;
				// Point types are AI instructions that should be executed once reaching a waypoint.
				// PZ: When we reach a waypoint, make sure we clear out all old flags. They were for previous waypoint.
				//self.b_aiflags = self.b_aiflags | (self.current_way.b_aiflags & #AI_ON_REACHING_WAYPOINT);
				self->b_aiflags = self->current_way->b_aiflags & PR_AI_ON_REACHING_WAYPOINT; // NOTE: THIS IS WIPING OUT ALL AI FLAGS, BEFORE SETTING THEM.
			}
			frik_removeFromTargetStack(self->target1);
			//self.keys = self.keys & #KEY_MASK_LOOKING_KEYS; // stop moving; we are on our target1, basically
			//self.b_aiflags = self.b_aiflags | #AI_WAIT;
			return;
		}
	}

	// PZ NOTE: THESE FUNCTIONS SHOULD NOT SET KEYS. THEY SHOULD ONLY CAUSE WAIT FLAG TO BE SET IF BOT SHOULD
	//          STOP AND WAIT. TO GET BOT TO MOVE A CERTAIN WAY, THEY SHOULD ADD A NEW TARGET, INCLUDING TEMP
	//          WAYPOINTS. TEMP WAYPOINTS SHOULD BE IN SIGHT OF ACTUAL GOAL AND THE BOT, IF POSSIBLE. SHOULD PROBABLY
	//          BE SIMPLER THOUGH, LIKE JUST MOVING SOME TO THE RIGHT OR LEFT UNTIL YOU HAVE A CLEAR PATH TO YOUR
	//          NEXT TARGET. THE BOT IS BASICALLY ALWAYS GOING TO PURSUE TARGET1. THE WAIT FLAG BASICALLY STOPS THIS.

	// [handle the AI flags (which are usually given to the bot when pursuing/reaching a waypoint)]
	frik_handleBotAiFlags();

	if (self->b_aiflags & PR_AI_WAIT || self->target1 == world)
	{
		//self.keys = self.keys & #KEY_MASK_LOOKING_KEYS;
		return;
	}

	// [determine whether there is an obstacle of any sort between us and target1]
	frik_botLookForAndHandleObstacles();

	if (self->b_aiflags & PR_AI_WAIT || self->target1 == world)
	{
		//self.keys = self.keys & #KEY_MASK_LOOKING_KEYS;
		return;
	}

	// [set the needed keys to make the bot move toward its .target1]
	// if `target1` is over your head, try jumping
	if (v[Z] > 30) // if the target is higher than the bot     (from his origin to the top of his head is 32 units)
	{
		if ((v[X] <= (4 * PR_ONE_FOOT)) && (v[Y] <= (4 * PR_ONE_FOOT))) // and if the bot is under the target
			frik_makeBotJump();
	}
	v = normalize(v); // get direction to target1
	self->keys = (self->keys & PR_KEY_MASK_LOOKING_KEYS) + frik_getRequiredKeysToMoveThisDirection(v);

	/* PZ: not doing this here for right now
	// if obstructed by something, deal with the obstruction
	frik_botHandleObstructedFlag();
	// handle being on a platform (?)
	frik_detectAndHandlePlatform(#TRUE); */

	// PZ NOTE: If AI_PRECISION is set or bot is low-skilled, don't let bot move until he is looking where he needs to move.
	if (self->b_aiflags & PR_AI_PRECISION || self->b_skill == PR_SKILL_EASY)
	{
		// if we aren't yet aiming the correct direction to walk, stop moving, until we are aiming the correct direction
		// PZ: took this out on 4/18/2015 because it was messing up bot's ability to swim up and down
		//g = frik_angleSubtraction(self.v_angle_x, self.b_commandedViewAngle_x); // pitch
		//if (fabs(g) > 10)
			//self.keys = self.keys & #KEY_MASK_LOOKING_KEYS;
		g = frik_angleSubtraction(self->v_angle[Y], self->b_commandedViewAngle[Y]); // yaw
		if (fabs(g) > 10)
			self->keys = self->keys & PR_KEY_MASK_LOOKING_KEYS;
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botMoveThisWay         (PZ: was "frik_walkmove")

Makes the bot walk in direction, `direction`.
If bot was AI_HANDLING_OBSTRUCTION at the end of the routine,
returns false (failed). Else, returns true (succeeded).

`self` = a bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_botMoveThisWay(const vector& direction)
{
	// okay so it's not walkmove
	// sue me
	self->keys = (self->keys & PR_KEY_MASK_LOOKING_KEYS) + frik_getRequiredKeysToMoveThisDirection(direction);
	return true;

/* PZ: I think we need to do this stuff once in the main AI routine.
	// deal with any obstruction
	frik_botHandleObstructedFlag();
	// deal with any platform
	frik_detectAndHandlePlatform(#TRUE);
	// if we are still obstructed, don't bother trying to move any more for now
	if (self.b_aiflags & #AI_HANDLING_OBSTRUCTION)
		return #FALSE;
	else
		return #TRUE;
*/
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botRoamDynamically      (PZ: was "frik_bot_roam")

The "hook" method of navigation. This nav
system is copyrighted 1999 by Ryan "Frika C"
Smith, keep that in mind when you steal it.

I brought this back because normal roaming
won't work - the bot gets distracted by its
own waypoints.

// PZ: THIS FUNCTION IS NOT BEING USED.
###################################################
PZ NOTE: As long as we are relying on waypoints for
navigation, this function is not called.
###################################################

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_botRoamDynamically()
{
	vector org, ang, org1;
	float loopcount, flag, dist;

	loopcount = 26;
	flag = PR_FALSE;
	while ((loopcount > 0) && !flag)
	{
		loopcount = loopcount - 1;
		org = self->origin + self->view_ofs;
		ang = self->angles;
		ang[Y] = frik_getAngleMod360(ang[Y] - 90 + (random() * 180));
		ang[X] = 0; // avoid upward sloping
		makevectors(ang);
		traceline(org, org + v_forward * 2300, PR_TL_BSP_ONLY, self);
		if (trace_fraction != 1)
		{
			org1 = trace_endpos;
			ang = normalize(trace_plane_normal);
			ang[Z] = 0; // avoid upward sloping
			traceline(org1, org1 + (ang * 2300), PR_TL_BSP_ONLY, self);
			if ((trace_fraction != 1) && (vlen(trace_endpos - org1) >= 64))
			{
				org = trace_endpos;
				traceline(org, self->origin + self->view_ofs, PR_TL_BSP_ONLY, self);
				if (trace_fraction != 1)
				{
					dist = vlen(org1 - org) / 2;
					org = org1 + (ang * dist);
					traceline(org, org - V({0, 0, 48}), PR_TL_BSP_ONLY, self);
					if (trace_fraction != 1)
					{
						frik_createTemporaryWaypoint(org);
						flag = PR_TRUE;
					}
				}
			}
		}
	}
	self->b_commandedViewAngle[Y] = self->v_angle[Y] + 10;
	frik_botMoveThisWay(v_forward);
}

} // END namespace Progs
