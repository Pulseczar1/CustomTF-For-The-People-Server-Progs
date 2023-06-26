/***********************************************
*                                              *
*            FrikBot General AI                *
*     "The I'd rather be playing Quake AI"     *
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
#include "ofndefs.h"
#include "frikbot/bot_qw.h"
#include "frikbot/bot_ai.h"
#include "frikbot/bot_way.h"
#include "frikbot/bot_misc.h"
#include "frikbot/bot_move.h"
#include "frikbot/bot_fight.h"
#include "items.h"
#include "tfortmap.h"
#include "custom.h"
#include "prozac.h"
#include "crusader.h"
#include "spy.h"
#include "haxxx.h"
#include "vote.h"
#include "neo.h"

namespace Progs {

/* PZ NOTES:
 *
Targets (.target[1->4]) are entities which the bot pursues. They can be an item, weapon,
player, monster, waypoint, etc. In the target stack, target1 is the top of the stack,
and is the highest priority target.

(bot).search_time = ?   When it runs out (< time), all targets are dropped.
*/

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_isInTargetStack    (PZ: was called "target_onstack")

checks to see if an entity is on the bot's target stack

`self` = the bot in question

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_isInTargetStack(entity e)
{
	if (e == world)
		return PR_FALSE;
	else if (self->target1 == e)
		return 1;
	else if (self->target2 == e)
		return 2;
	else if (self->target3 == e)
		return 3;
	else if (self->target4 == e)
		return 4;
	else
		return PR_FALSE;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_addToTargetStack   (PZ: was called "target_add")

Adds a new entity to the stack. Since it's a
LIFO stack (PZ: the only kind of stack),
this will be the bot's new target1.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_addToTargetStack(entity ent)
{
	if (ent == world)
		return;
	if (frik_isInTargetStack(ent))
		return;
	// PZ: if the target stack is full, don't allow adding a target, since it will push out the last target (target4)
	if (self->target4 != world)
		return;

	self->target4 = self->target3;
	self->target3 = self->target2;
	self->target2 = self->target1;
	self->target1 = ent;

	self->PR_b_bot_previousTarget1 = self->target1;

// debug prints
if (PZ_DEBUG == self->b_clientno) {
string strtemp;
strtemp = etos(ent);
bprint(2, "ADDED entity ", strtemp, " to target stack; classname = ", ent->classname, ", netname = ", ent->netname, "\n");

strtemp = etos(self->target1);
bprint(2, "target1 = ", strtemp, ", ");

strtemp = etos(self->target2);
bprint(2, "target2 = ", strtemp, ", ");

strtemp = etos(self->target3);
bprint(2, "target3 = ", strtemp, ", ");

strtemp = etos(self->target4);
bprint(2, "target4 = ", strtemp, "\n");
bprint(2, "######################################################\n");
}

	//self.search_time = time + 5; // PZ: not sure yet how I want to do this; so commented this out
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_removeFromTargetStack  (PZ: was called "target_drop")

Removes an entity from the bot's target stack.
The stack will empty everything up to the object.
So, if you have target2 item_health, target1
waypoint, and you drop the health, the waypoint
is gone too.

// PZ: Why are we removing other targets if it's target 2 through 4? No explanation??

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_removeFromTargetStack(entity ent)
{
	float targ;

//stackTrace();

	targ = frik_isInTargetStack(ent);
	if (targ == 1)
	{
		self->target1 = self->target2;
		self->target2 = self->target3;
		self->target3 = self->target4;
		self->target4 = world;
	}
	else if (targ == 2)
	{
		self->target1 = self->target3;
		self->target2 = self->target4;
		self->target3 = self->target4 = world;
	}
	else if (targ == 3)
	{
		self->target1 = self->target4;
		self->target2 = self->target3 = self->target4 = world;
	}
	else if (targ == 4)
		self->target1 = self->target2 = self->target3 = self->target4 = world;

	if (targ >= 1 && targ <= 4)
	{
		// We aren't pursuing `.target1` anymore. So, we need to remove any flags associated with pursuit of that target.
		self->b_aiflags = self->b_aiflags - (self->b_aiflags & PR_AI_ON_PURSUING_WAYPOINT);

// debug prints
if (PZ_DEBUG == self->b_clientno) {
string strtemp;
strtemp = etos(ent);
bprint(2, "REMOVED entity ", strtemp, " from target stack; classname = ", ent->classname, ", netname = ", ent->netname, "\n");

strtemp = etos(self->target1);
bprint(2, "target1 = ", strtemp, ", ");

strtemp = etos(self->target2);
bprint(2, "target2 = ", strtemp, ", ");

strtemp = etos(self->target3);
bprint(2, "target3 = ", strtemp, ", ");

strtemp = etos(self->target4);
bprint(2, "target4 = ", strtemp, "\n");
bprint(2, "######################################################\n");
}
	}

	//self.search_time = time + 5;  PZ: not sure how I want to do this yet; so took this out
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_getGoalTarget

PZ: The goal target is the one lowest (highest
number) on the target stack -- the ultimate target.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
entity frik_getGoalTarget()
{
	if (self->target4 != world) return self->target4;
	if (self->target3 != world) return self->target3;
	if (self->target2 != world) return self->target2;
	if (self->target1 != world) return self->target1;
	return world;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_dropTarget       (PZ: was called "bot_lost")

Bot needs to drop the target. So, remove it from the target
stack. `successfulWithTarget` means the bot successfully
reached, killed, or did whatever it needed to do with its
target.

`self` = current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_dropTarget(entity targ, float successfulWithTarget)
{
	if (targ == world)
		return;

	frik_removeFromTargetStack(targ);
	// PZ: need this? PZ: Not sure why we'd want to mess with the route.
	/*if (targ.classname == "waypoint")
	{
		//targ.b_sound = targ.b_sound - (targ.b_sound & getVectorBitFlagGivenBitNumber(self.b_clientno));
		// PZ: using the vector, .movedir, to store bot route flags now, so that we can support 32 bots
		targ.movedir = clearVectorBitFlag(self.b_clientno, targ.movedir);
	}*/
	// find a new route, if we didn't reach the target
	if (!successfulWithTarget)
	{
		self->target1 = self->target2 = self->target3 = self->target4 = world;
		self->last_way = frik_findWaypointNearestAndVisibleToSelf(self->current_way);
		frik_clearRouteForSelf();
		self->b_aiflags = 0;
	}
	else
	{
		if (targ->classname == "item_artifact_invisibility") // bot got the invisibility
			if (self->items & PR_IT_INVISIBILITY)
				frik_botConsiderStartingConversation(PR_TOPIC_SELF_INFORMS_TEAM_INVIS, 0.2);

		if (targ->flags & PR_FL_ITEM)
		{
			if (targ->model == string_null)
				targ->_prev = world;
			else
				targ->_prev = self;
		}
	}

	// PZ: Took out the search_time check, because stuff in TF already uses search_time, like resupply packs, to determine when to resupply.
	//     Bots were causing resupply packs to constantly resupply without delay, because they were changing their search_time. We may need
	//     to replace this with another variable name, if things really break without it. (2/8/14)
	// PZ: Changed .search_time to .oldkeys. When I took out this condition, bots just hovered on their own flag and resupply packs.
	if (targ->classname != "player")
		targ->oldkeys = time + 5;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_handleBotAiFlags    (PZ: was called "bot_handle_ai")

This is a 0.10 addition. Handles flags set in .b_aiflags.
Note: Not all aiflags are handled here -- just those that
perform some sort of action.

`self` : current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_handleBotAiFlags()
{
	entity e, oldSelf = world;
	vector v;
	float  dist;

	// wait is used to stop the bot until his search time expires / or route changes
/*	if (self.b_aiflags & #AI_WAIT)
		self.keys = self.keys & #KEY_MASK_LOOKING_KEYS;  // stop all movement  */

	if (self->b_aiflags & PR_AI_DOORFLAG) // was on a door when spawned (WARNING: you must put this waypoint right on or above a door)
	{
//bprint(2, "#### 1\n");
		// PZ: self.target1's were self.current_way's
//bprint(2, "#### 2\n");
		oldSelf = self;
		self = self->target1;
		if (!frik_detectAndHandlePlatform(PR_FALSE)) // is there a platform/door NOT beneath `self`?
		{
//bprint(2, "#### 3\n");
			e = frik_findNearestEntityToSelfWithThisClassname("door"); // this is likely the door responsible (crossfingers)
			self = oldSelf;

			if (self->b_aiflags & PR_AI_DOOR_NO_OPEN)
			{
//bprint(2, "#### 4\n");
				if (e->nextthink > time)        {            // door is open // PZ: added "> time"
//bprint(2, "#### 5\n");
					//self.keys = self.keys & #KEY_MASK_LOOKING_KEYS; // wait until it closes before you move
					self->b_aiflags = self->b_aiflags | PR_AI_WAIT; // PZ
				}
				else    {
//bprint(2, "#### 6\n");
					//frik_dropTarget(self.target1, #FALSE);
					frik_removeFromTargetStack(self->target1);
					self->b_aiflags = self->b_aiflags | PR_AI_WAIT; // PZ
				}
			}
			else
			{
				// PZ TODO: this is redundant with the other code
				if (e->targetname != "") // look for a button
				{
					e = find(world, "target", e->targetname);
					if (e->health > 0)  // shootable button
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
					}
				}
				self->b_aiflags = self->b_aiflags - PR_AI_DOORFLAG;  // handled the flag
			}
		}
		else // platform/door is there; so just keep pursuing target1
		{
			self = oldSelf; // PZ NOTE: make sure `self` gets set back to the bot some how
			self->b_aiflags = self->b_aiflags - PR_AI_DOORFLAG;  // handled the flag
		}
	}

	if (self->b_aiflags & PR_AI_JUMP)
	{
		if (self->flags & PR_FL_ONGROUND)
		{
			frik_makeBotJump();
			self->b_aiflags = self->b_aiflags - PR_AI_JUMP;  // handled the flag
		}
	}
	else if (self->b_aiflags & PR_AI_SUPER_JUMP)
	{
//bprint(2, "@@@ 1\n");
		//if (self.weapon != 32)
		if (self->current_weapon != PR_WEAP_ROCKET_LAUNCHER) // PZ: for CuTF (this is good because it actually handles multiple weapons per impulse)
		{
//bprint(2, "@@@ 2\n");
			self->impulse = 7;
		}
		else if (self->flags & PR_FL_ONGROUND)
		{
//bprint(2, "@@@ 3\n");
			if (frik_canBotRocketJump(self))
			{
//bprint(2, "@@@ 4\n");
				frik_makeBotJump();
				self->v_angle[X] = self->b_commandedViewAngle[X] = 80;  // pitch all the way down
				self->PR_BUTTON_FIRE = PR_TRUE;
			}
			else  {
//bprint(2, "@@@ 5\n");
				//frik_dropTarget(self.target1, #FALSE);
				frik_removeFromTargetStack(self->target1); // PZ: for right now; just doing this to make it simpler
			}
			self->b_aiflags = self->b_aiflags - PR_AI_SUPER_JUMP;  // handled the flag
		}
	}

	if (self->b_aiflags & PR_AI_SURFACE)
	{
		if (self->waterlevel > 2)
		{
			// PZ NOTE: could both of these being set possibly be why the bot seems to swim up really fast?
			self->keys = PR_KEY_MOVEUP;
			self->PR_BUTTON_JUMP = PR_TRUE; // swim!
			self->b_aiflags = self->b_aiflags | PR_AI_WAIT; // added on 4/26/15 (don't let main movement code run until this is done)
		}
		else
			self->b_aiflags = self->b_aiflags - PR_AI_SURFACE;  // handled the flag, once waterlevel is <= 2
	}

	// PZ TODO: Let's let the obstruction (gap) detection code tell us when a platform is ready to be walked on, and then
	//          tell us when we can get off a platform. The AI_RIDE_TRAIN flag will simply tell the navigation code to just wait for the train;
	//          don't try to go around or anything.
	// PZ NOTE: this can be a platform ("plat"), train ("train"), and even a door ("door")
	if (self->b_aiflags & PR_AI_RIDE_TRAIN)
	{
		// simple, but effective
		// this can probably be used for a lot of different
		// things, not just trains (door elevators come to mind)
		if (self->target1 != world && self->target1->b_aiflags & PR_AI_RIDE_TRAIN)
		{
			oldSelf = self;
			self = self->target1; // PZ: .target1 is the waypoint he's currently pursuing that had the ride train flag
		}
		if (!frik_detectAndHandlePlatform(PR_FALSE)) // platform not beneath the waypoint we are pursuing?
		{
			if (oldSelf != world) self = oldSelf;
			//self.keys = self.keys & #KEY_MASK_LOOKING_KEYS;  // stop moving
			// PZ: if the platform is beneath the bot, make the bot move to the center of it


			self->b_aiflags = self->b_aiflags | PR_AI_WAIT;
//bprint(2, "WAIT 1\n");
		}
		else // once platform is beneath the waypoint, we are free to continue pursuing it
		{
			if (oldSelf != world) self = oldSelf;
			//self.b_aiflags = self.b_aiflags - (self.b_aiflags & #AI_WAIT);
			if (frik_detectAndHandlePlatform(PR_FALSE)) // platform beneath us?
			{
				// PZ NOTE: move to the center of the platform?
				v = frik_getRealOriginOfEntity(trace_ent) /*+ trace_ent.origin*/ - self->origin;
				v[Z] = 0;
				if (vlen(v) < 24)
				{
					//self.keys = self.keys & #KEY_MASK_LOOKING_KEYS;  // stop moving; we reached the waypoint, so we now have a new target1
					self->b_aiflags = self->b_aiflags | PR_AI_WAIT;
//bprint(2, "WAIT 2\n");
				}
				else
				{
					// Moving to the center of the platform will ensure he touches the waypoint that marks where he should get off.
					// By touching that waypoint, he will now be pursuing the next waypoint, likely not on a train, and he will just
					// walk to it like normal.
					self->b_aiflags = self->b_aiflags | PR_AI_PRECISION;
					self->keys = frik_getRequiredKeysToMoveThisDirection(v);
					self->b_aiflags = self->b_aiflags | PR_AI_WAIT; // don't allow frik_botPursueTarget1() to move us
/*bprint(2, "WAIT 3; ");
local string strtemp;
strtemp = ftos(self.keys);
bprint(2, "self.keys = ", strtemp, "; v = ");
strtemp = vtos(v);
bprint(2, strtemp, "\n");*/
				}
				// ## PZ TODO: we need to clear flag AI_RIDE_TRAIN, once train has moved and is now stopped, some how (remember that target1
				// has now changed.)
				/*if ((trace_ent.classname == "plat" || trace_ent.classname == "door") && trace_ent.state == #STATE_TOP) // if this is an elevator, and we are at the top
				{
					frik_botSay("Reached top of plat");
					self.b_aiflags = self.b_aiflags - #AI_RIDE_TRAIN;  // handled the flag; get off of it
				}*/
			}
		}
	}

	// PZ: This is for normal up and down platforms. A platform can be a "plat" or even a "door".
	// TODO DONE?: allow using this on "door" platforms?
	if (self->b_aiflags & PR_AI_PLAT_BOTTOM)
	{
		// find the nearest entity named "plat" or "door"
		vector platDist = {99999, 99999, 99999}, doorDist = {99999, 99999, 99999};
		entity plat = frik_findNearestEntityToSelfWithThisClassname("plat");
		if (plat != world) platDist = self->origin - frik_getRealOriginOfEntity(plat); // subtracting this way causes bot to move /away/
		entity door = frik_findNearestEntityToSelfWithThisClassname("door");
		if (door != world) doorDist = self->origin - frik_getRealOriginOfEntity(door);
		float fPlatDist = vlen(platDist), fDoorDist = vlen(doorDist);
		if (fDoorDist < fPlatDist) { e = door; v = doorDist; }
		else                       { e = plat; v = platDist; }
		// if platform/door not sitting at bottom position
		if (e->state != PR_STATE_BOTTOM)
		{
			//v = self->origin - frik_getRealOriginOfEntity(e);
			v[Z] = 0;
			if (vlen(v) >= 96) // we are far enough away; stop here
				//self.keys = self.keys & #KEY_MASK_LOOKING_KEYS;  // stop moving
				self->b_aiflags = self->b_aiflags | PR_AI_WAIT;
			else
			{
				v = self->current_way->origin - self->origin; // move toward the Wait For Plat waypoint rather than just away from the plat/door
				frik_botMoveThisWay(v); // move away from the plat (the plat's area where it comes down to)
				self->b_aiflags = self->b_aiflags | PR_AI_WAIT; // WAIT actually keeps the main target1 pursuing function from controling movement
			}
		}
		else
		{
			self->b_aiflags = self->b_aiflags - PR_AI_PLAT_BOTTOM;  // handled the flag
			//self.b_aiflags = self.b_aiflags - (self.b_aiflags & #AI_WAIT); // this is reset every frame anyway
		}
	}

	if (self->b_aiflags & PR_AI_DIRECTIONAL)
	{
		if ((normalize(self->current_way->origin - self->origin) * self->b_dir) > 0.4) // PZ: the most recently reached waypoint is now the .current_way
		{
			self->b_aiflags = self->b_aiflags - PR_AI_DIRECTIONAL;  // handled the flag
			//frik_dropTarget(self.target1, #TRUE);
			frik_removeFromTargetStack(self->target1); // PZ: for right now; just doing this to make it simpler
		}
	}

	if (self->b_aiflags & PR_AI_SNIPER)
	{
		if (isVectorBitFlagSet(PR_ROLE_SNIPER, V({0, 0, 0}), self->b_botRole)) // PZ
			self->b_aiflags = self->b_aiflags | PR_AI_WAIT;
		self->b_aiflags = (self->b_aiflags /*| #AI_WAIT | #AI_PRECISION*/) - PR_AI_SNIPER;  // set AI_WAIT and AI_PRECISION for bot (handled the flag)
		// FIXME: Add a switch to wep command
		// FIXME: increase delay?
	}

	if (self->b_aiflags & PR_AI_AMBUSH)
	{
		self->b_aiflags = (self->b_aiflags | PR_AI_WAIT) - PR_AI_AMBUSH;  // set AI_WAIT for bot (handled the flag)
		// FIXME: Add a switch to wep command
		// FIXME: increase delay?
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_makeNextWaypointInRouteTarget1  (PZ: was called "bot_path")

Bot will follow a route generated by the
frik_startRouteFinding() set of functions in
bot_way.qc. This code, while it works pretty
well, can get confused.

`self` = current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_makeNextWaypointInRouteTarget1()
{
	entity nextWaypointInRoute, tele;

	frik_decideWhetherToDropTarget(self->target1);
	if (self->target1 == world)
	{
		self->keys = 0; // no moving or turning/looking
		return;
	}

	if (frik_isInTargetStack(self->last_way))
		return; // old/previous waypoint is still being hunted

	nextWaypointInRoute = frik_findNextWaypointInOurRoute(self->last_way); // starting with the last waypoint that the bot pursued on its route
	if (nextWaypointInRoute == world)
	{
		// this is an ugly hack
		if (self->target1->current_way != self->last_way)
		{
			if (self->target1->classname != "temp_waypoint")
				if (self->target1->classname != "player")
					frik_dropTarget(self->target1, PR_FALSE);
		}
		return;
	}

	// [update the bot's special AI features]
	// Readahead types are AI conditions to perform while heading to a waypoint.
	// Point types are AI flags that should be executed once reaching a waypoint.
	self->b_aiflags = (nextWaypointInRoute->b_aiflags & PR_AI_ON_PURSUING_WAYPOINT) | (self->last_way->b_aiflags & PR_AI_ON_REACHING_WAYPOINT);

	// PZ NOTE: the next waypoint is now our primary target
	frik_addToTargetStack(nextWaypointInRoute);
	if (self->last_way != world)
	{
		if (frik_doesWaypointLinkToWaypoint(self->last_way, nextWaypointInRoute) == 2) // PZ NOTE: are they linked by teleporter link?
		{
			tele = frik_findNearestEntityToSelfWithThisClassname("trigger_teleport"); // this is probably the teleporter associated with the telelink
			frik_addToTargetStack(tele);
		}
		traceline(self->last_way->origin, nextWaypointInRoute->origin, PR_TL_ANY_SOLID, self); // check for blockage
		if (trace_fraction != 1)
		{
			if (trace_ent->classname == "door" && !(self->b_aiflags & PR_AI_DOOR_NO_OPEN)) // a door blocks the way
			{
				// linked doors fix
				if (trace_ent->owner != world)
					trace_ent = trace_ent->owner;
				if ((trace_ent->health > 0) && (self->enemy == world)) // shootable door
				{
					self->enemy = trace_ent;
					frik_makeWeaponSelection(-1, PR_FALSE);   // PZ: changed 1 to -1
					self->b_aiflags = self->b_aiflags | PR_AI_BLIND; // nick knack paddy hack
				}
				else if (trace_ent->targetname != "") // door with button
				{
					tele = find(world, "target", trace_ent->targetname);
					if (tele->health > 0)  // shootable button
					{
						self->enemy = tele;
						frik_makeWeaponSelection(-1, PR_FALSE);  // PZ: changed 1 to -1
					}
					else // non-shootable button
					{
						//frik_removeFromTargetStack(nextWaypointInRoute);
						frik_addToTargetStack(tele);
						//frik_findRouteToObject(tele, #TRUE);
						self->b_aiflags = self->b_aiflags | PR_AI_BLIND; // give a bot a bone
						return;
					}
				}
			}
			else if (trace_ent->classname == "func_wall")
			{
				// give up
				frik_dropTarget(self->target1, PR_FALSE);
				return;
			}
		}
	}
	// this is used for AI_DIRECTIONAL
	self->b_dir = normalize(nextWaypointInRoute->origin - self->last_way->origin);

	self->last_way = nextWaypointInRoute;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_determinePriorityOfObject    (PZ: was called "priority_for_thing")

Determines the bot's priority for pursuing `object`,
and returns the priority value.

Old Comment:
Bot Priority Look. What a stupid name. This is where
the bot finds things it wants to kill/grab.

priority scale:
 0 - 10: virtually ignore
10 - 30: normal item range
30 - 50: bot will consider this a target worth changing course for
50 - 90: bot will hunt these as vital items

`self` = current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// *!* Make sure you add code to frik_decideWhetherToDropTarget() to remove the target *!*
// This is the most executed function in the bot. Be careful what you do here.
float frik_determinePriorityOfObject(entity object)
{
	float priorityOfObject; priorityOfObject = -9999999; //0;

	// PZ: Took out the search_time check, because stuff in TF already uses search_time, like resupply packs, to determine when to resupply.
	//     Bots were causing resupply packs to constantly resupply without delay, because they were changing their search_time. We may need
	//     to replace this with another variable name, if things really break without it. (2/8/14)
	// PZ: Changed .search_time to .oldkeys. When I took out this condition, bots just hovered on their own flag and resupply packs.
	// PZ: took out check for FL_ITEM, because capture points on canyon1 have that set, likely mistakenly
	//if ((object.flags & #FL_ITEM) && object.model != string_null/* && object.oldkeys < time*/) // PZ: took out oldkeys use for AI overhaul
	//{
		// PZ TOOD: _prev only allows remembering one bot trying to pursue this object; we need a field for each bot (.oldtarget or something)
		//if (object._prev != self)  // PZ NOTE: don't want the bot to pursue this if it was just pursued?
		//	priorityOfObject = 20; // PZ NOTE: every item has a value of 20 by default
		if      (object->classname == "item_artifact_super_damage" && object->model != string_null)
			priorityOfObject = 65;
		else if (object->classname == "item_artifact_invulnerability" && object->model != string_null)
			priorityOfObject = 65;
		else if (object->classname == "item_health" && object->model != string_null)
		{
			if (self->health < self->max_health) //< 100)
			{
				if (self->health < 40)
					priorityOfObject = 70;
				else
					priorityOfObject = 40;
				if (object->spawnflags & 2/*#H_MEGA*/)
					priorityOfObject = priorityOfObject + 20;
			}
			else // can't get health if health is 100+, unless mega health
			{
				if (object->spawnflags & 2/*#H_MEGA*/)
					priorityOfObject = 55;
				else
					priorityOfObject = -9999999; //0;
			}
		}                                             // !!! PZ: WHY ISN'T THIS object.touch??? Changing from `self` to `object`. (10-3-15)
		else if (object->model == "progs/armor.mdl" && object->touch == armor_touch) // PZ: added the self.touch part; shouldn't pursue stuff that just looks like armor
		{
			if (self->armorvalue < self->maxarmor) //< 200)
			{
				priorityOfObject = 20;
				if (object->skin == 2)        // PZ NOTE: probably red armor
					priorityOfObject = 60;
				if (self->armorvalue < 100)
					priorityOfObject = priorityOfObject + 25;
			}
		}
		/* PZ: Weapon items are useless in CuTF.
		else if (object.classname == "weapon_supershotgun")
		{
			if (!(self.items & 2)) // IT_SUPER_SHOTGUN
				priorityOfObject = 25;
		}
		else if (object.classname == "weapon_nailgun")
		{
			if (!(self.items & 4)) // IT_NAILGUN
				priorityOfObject = 30;
		}
		else if (object.classname == "weapon_supernailgun")
		{
			if (!(self.items & 8)) // IT_SUPER_NAILGUN
				priorityOfObject = 35;
		}
		else if (object.classname == "weapon_grenadelauncher")
		{
			if (!(self.items & 16)) // IT_GRENADE_LAUNCHER
				priorityOfObject = 45;
		}
		else if (object.classname == "weapon_rocketlauncher")
		{
			if (!(self.items & 32)) // IT_ROCKET_LAUNCHER
				priorityOfObject = 60;
		}
		else if (object.classname == "weapon_lightning")
		{
			if (!(self.items & 64)) // IT_LIGHTNING
				priorityOfObject = 50;
		}*/
		// PZ: ### RESUPPLY PACKS ### (TODO DONE?: on unholyk, the captured princess is an info_tfgoal)
		else if (object->classname == "info_tfgoal" && object->frags == 0 && object->model != string_null)
		{
			/*if (object.armorvalue >= 100 && self.armorvalue < self.maxarmor)
			{
				if (object.armorvalue >= 200)
					priorityOfObject = 60;
				else if (self.armorvalue < 100)
					priorityOfObject = priorityOfObject + 25;
			}
			if (object.health >= 100 && self.health < self.max_health)
			{
				if (self.health < 100)
					priorityOfObject = 55;
				if (self.health < 40)
					priorityOfObject = priorityOfObject + 50;
			}*/
			// don't pursue packs that you can't use
			if (object->team_no != self->team_no) // belongs to enemy
				return -9999999; //0;

			// PZ TODO: Scale priority by how much the bot's stat is less than its max for the stat.
			// PZ TODO: Check that the pack actually gives the stat.
			float isUseful; isUseful = PR_FALSE;
			priorityOfObject = 0;
			if (self->health < self->max_health)            {isUseful = PR_TRUE; priorityOfObject = priorityOfObject + 20;}
			if (self->armorvalue < self->maxarmor)          {isUseful = PR_TRUE; priorityOfObject = priorityOfObject + 20;}
			if (self->ammo_shells < self->maxammo_shells)   {isUseful = PR_TRUE; priorityOfObject = priorityOfObject + 20;}
			if (self->ammo_nails < self->maxammo_nails)     {isUseful = PR_TRUE; priorityOfObject = priorityOfObject + 20;}
			if (self->ammo_rockets < self->maxammo_rockets) {isUseful = PR_TRUE; priorityOfObject = priorityOfObject + 20;}
			if (self->ammo_cells < self->maxammo_cells)     {isUseful = PR_TRUE; priorityOfObject = priorityOfObject + 20;}
			// TODO: Add grenades, detpacks, medikit ammo, etc.
			if (!isUseful)
			{
				//frik_botSay("I'm ignoring a resupply pack\n");
				return -9999999; //0; // ignore this pack
			}
		}
		// PZ: ### TF FLAGS ###
		else if (object->classname == "item_tfgoal" && object->model != string_null)
		{
			if (/*object.team_no != 0 &&*/ object->team_no == self->team_no) // means this flag is the enemy's flag (goal_no is the id number of the goal/flag)
			{
				//bprint(#PRINT_HIGH, self.netname, " is determining the priority of the enemy flag (", object.broadcast, ")\n");
				// PZ TODO: this should vary according to bot's role in the game (defensive bots should not care about the enemy flag, unless it's nearby)
				if (isVectorBitFlagSet(PR_ROLE_OFFENSE, V({0, 0, 0}), self->b_botRole))
					priorityOfObject = 500; // was 90 // was 180 on 11-12-15 // was 250 on 12-11-15
				else
					priorityOfObject = 20; // hopefully this means we'll only pursue it if it's close to us
			}
			else if (object->team_no != 0) // this must be our flag
			{
				if (isVectorBitFlagSet(PR_ROLE_DEFENSE, V({0, 0, 0}), self->b_botRole))
					priorityOfObject = 180; // was 90
				else
					priorityOfObject = 20;
			}
		}
	//}
	// PZ: ### TF FLAG CAPTURE POINTS ### (see TF documentation for specification, specifically, tfortmap.txt)
	// PZ NOTE: .items_allowed is not used on unholyk capture points, but .has_item_from_group is used instead
	// PZ NOTE: .frags is how many frags the capture point rewards for capture; helps distinguish this as a capture point
	else if (object->classname == "info_tfgoal" && object->frags > 0 && object->goal_state != PR_TFGS_REMOVED) // (resupply packs are also info_tfgoals)
	{
		entity te;
		float gotone;

		// [The following code was taken from tfortmap.qc, and modified slightly.]
		// .items_allowed means the player must be carrying the GoalItem with the ID of `items_allowed` in order to trigger this goal (cap)
		if (object->items_allowed)
		{
			te = Finditem(object->items_allowed);
			// if you have the flag corresponding to this capture point
			if (te->owner == self)
			{
				//bprint(#PRINT_HIGH, self.netname, " is determining the priority of his capture point (", object.netname_broadcast, ") (.items_allowed)\n");
				priorityOfObject = 999999;  // PZ: was 150 on 12-11-15
			}
		}
		else if (object->has_item_from_group) // like .items_allowed, but allows the player to be carrying any GoalItem from a group of GoalItems
		{
			gotone = PR_FALSE;
			te = find(world, "classname", "item_tfgoal");
			while (te != world && !gotone)
			{
				if (te->group_no == object->has_item_from_group && te->owner == self)
					gotone = PR_TRUE;

				te = find(te, "classname", "item_tfgoal");
			}
			if (gotone)
			{
				//bprint(#PRINT_HIGH, self.netname, " is determining the priority of his capture point (", object.netname_broadcast, ") (.has_item_from_group)\n");
				priorityOfObject = 999999;  // PZ: was 150 on 12-11-15
			}
		}
	}
	else if ((object->flags & PR_FL_MONSTER) && object->health > 0 && !(teamplay && Teammate(object->team_no, self->team_no)))
		priorityOfObject = coop ? 45 : 30; // PZ: was 45; I don't want monsters ganged up on
	else if ((object->flags & PR_FL_MACHINE) && object->health > 0 && !(teamplay && Teammate(object->team_no, self->team_no)))  // PZ: added
		priorityOfObject = 40; // PZ: experimenting; Engy is too easy; bots should target builds /more/ but not /always/
	else if ((object->classname == "monster_army") && object->health > 0 && !(teamplay && Teammate(object->team_no, self->team_no)))  // PZ: added
		priorityOfObject = 30;
	else if (object->classname == "player")
	{
		float isTeammateOrDisguisedAsTeammate = PR_FALSE;
		if      (Teammate(self->team_no, object->team_no))         isTeammateOrDisguisedAsTeammate = PR_TRUE;
		else if (Teammate(self->team_no, object->undercover_team)) isTeammateOrDisguisedAsTeammate = PR_TRUE;

		if (object->health > 0)
		{
			if (object == self)
				return -9999999; //0;
			else
			{
				// PZ: added checks for spies and thieves
				if (object->job & PR_JOB_THIEF && object->job & PR_JOB_FULL_HIDE) // completely invisible (standing still)
					priorityOfObject = -9999999; //0;
				// would be great if I could easily check for player rotation with avelocity, but it appears avelocity on a player isn't used
				// IDEA: what if I set their avelocity according to how much they turned since previous frame?
				else if (object->is_feigning && object->velocity == V({0, 0, 0}))  // feigning and not jumping
					priorityOfObject = -9999999; //0;
				else if (object->items & PR_IT_INVISIBILITY ||
				    (object->job & PR_JOB_THIEF && object->job & PR_JOB_ACTIVE && !(object->job & PR_JOB_FULL_HIDE))) // thiefed, but not standing still (eyes visible)
					priorityOfObject = 2;
				else if (coop)
				{
					priorityOfObject = 200;
					if (object->target1->classname == "player") // PZ NOTE: is this a bot pursuing a player?
						if (!object->target1->ishuman)          // PZ NOTE: this makes bots more likely to pursue a human than each other
							return -9999999; //0;
				}
				//else if (teamplay && object.team == self.team)     // PZ: I don't yet know what 'team' is. So, might need to put this case back.
				else if (teamplay && isTeammateOrDisguisedAsTeammate) // PZ: CuTF uses .team_no; added spy support
				{
					// PZ: I don't understand what this is doing. It's not preventing teamkilling.
					/*priorityOfObject = 100;
					if (object.target1.classname == "player")
						return 0;*/  // PZ: commented this
					// PZ: Get infected bots to pursue friendly medics and get medics to pursue friendly infected players. (10-4-15)
					if      (object->tfstate & PR_TFSTATE_INFECTED && self->weapons_carried & PR_WEAP_MEDIKIT)
					{
						if (time - object->last_saveme_sound <= 10) // if they've called for a medic within the last ten seconds, they get a higher priority
							priorityOfObject = 300;
						else
							priorityOfObject = 200;
					}
					else if (object->weapons_carried & PR_WEAP_MEDIKIT && self->tfstate & PR_TFSTATE_INFECTED)
						priorityOfObject = 200;
					else if (object->health < object->max_health && self->weapons_carried & PR_WEAP_MEDIKIT)
					{
						priorityOfObject = (100 - object->health) * 3; // scale priority according to how bad their health is
						if (time - object->last_saveme_sound <= 10) // if they've called for a medic within the last ten seconds, they get a higher priority
							priorityOfObject = priorityOfObject + 100;
					}
					else
						priorityOfObject = -9999999; //0; // PZ: added to stop TKing
				}
				else // normal enemy players
					priorityOfObject = 30;

				if (object->remove_spawnpoint) priorityOfObject += 200; // PZ: Flag carriers are top priority.
				if (neo.isModeActive() && neo.getNeo() == object) priorityOfObject += 200; // PZ: In Neo mode, Neo is top priority.
			}
		}
		else
		{
			// dead players can be revived with medikit
			if (self->weapons_carried & PR_WEAP_MEDIKIT && ((teamplay && isTeammateOrDisguisedAsTeammate) || coop))
			{
				if (time - object->last_saveme_sound <= 10) // if they've called for a medic within the last ten seconds, they get a higher priority
					priorityOfObject = 300;
				else
					priorityOfObject = 200;
			}
		}
	}
	else if (object->classname == "waypoint")
	{
		if (object->b_aiflags & PR_AI_SNIPER)
		{
			if (isVectorBitFlagSet(PR_ROLE_SNIPER, V({0, 0, 0}), self->b_botRole))
				priorityOfObject = 200;
		}
		else if (object->b_aiflags & PR_AI_AMBUSH) // PZ NOTE: I guess these would be like just generic areas to stand guard.
			priorityOfObject = 30;
	}
	if (pointcontents(object->origin) < PR_CONTENT_WATER) // basically, if it's in slime or lava, ignore it, probably so we don't try to follow it into the lava
		return -9999999; //0;

	// PZ: Objects further away from the bot have a lower priority. We are only going to do straight line distance here out of concern that path distance
	//     would be too slow to compute.
	if (priorityOfObject > -9999999)
	{
		float dist;
		dist = vlen(object->origin - self->origin);
		dist = dist * 0.0125; // This multiplier determines how much of a role distance plays.
		priorityOfObject = priorityOfObject - dist; // This will likely usually make the priority negative, but that should be okay.
	}

	// PZ: don't know what this is doing; probably don't need it anymore
	/*if (priorityOfObject)
	{
		if (object.current_way)
		{
			// check to see if it's unreachable
			if (object.current_way.items == -1)
				return 0;
			else
				priorityOfObject = priorityOfObject + (13000 - object.current_way.items) * 0.05;
		}
	}*/
	return priorityOfObject;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_findPriorityObjectToPursue   (PZ: was called "bot_look_for_crap")

Finds the highest priority object, within a given radius, for pursuit.

`largeScope` of 1 means a very large radius. `largeScope` of 0 means a small radius.
If it identifies a priority object, it adds the object to the bot's target stack. If
the target is far away, this function calls frik_findRouteToObject() to find a path to
the object. If the scope is large, that's like the bot is basing his knowledge of the
object on his memory of the map. If the scope is small, that's like he's considering
only objects that he can see around him.

`self` = current bot
Returns:  true if it found something new to pursue

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_findPriorityObjectToPursue(float largeScope)
{
	entity object, priorityObject; priorityObject = world;
	float priority, highestPriority/*, dist*/;

	// PZ: findradius() doesn't appear to go out far enough for a map as large as canyon1. So, I'm just going to use nextent().
	//     *** No, this wasn't actually the problem.
	// find the highest priority object to pursue
	/*if (largeScope)
		object = findradius(self.origin, 9999999); // PZ: I want to make sure this is not limited (it was 13000)
	else
		object = findradius(self.origin, 500);*/
	object = nextent(world);

	highestPriority = -9999999; //1;
	while (object != world)
	{
		priority = frik_determinePriorityOfObject(object);
		if (priority > -9999999)
			if (!largeScope) // this object is supposed to be close; so, you should be able to see it, in order to pursue it
				if (!frik_isVisible_Simple(object))
					priority = -9999999; //0;
		if (priority > highestPriority && !(self->b_aiflags & PR_AI_REJECT_TARGET && object == self->b_lastGoalTarget)) // PZ: don't allow selecting the same target consecutively (TOOK OUT -- 10-3-15)
		{
			highestPriority = priority;
			priorityObject = object;
			//dist = vlen(self.origin - object.origin); // PZ: commented because it wasn't being used
			// PZ TODO DONE? IN frik_determinePriorityOfObject(): I think closer things should be higher priority, though.
		}
		//object = object.chain;
		object = nextent(object);
	}
	if (priorityObject == world)
		return PR_FALSE;

	// add it to the target stack, if it's not already a target, and if this is large scope, find a route to this object
	//if (!frik_isInTargetStack(priorityObject)) // PZ: done by the frik_addToTargetStack function
	//{
if (PZ_DEBUG == self->b_clientno)
{
string temp;
temp = strcat("Found the following object to pursue: ", priorityObject->classname);
temp = strcat(temp, ", ");
temp = strcat(temp, priorityObject->netname);
temp = strcat(temp, ", ");
temp = strcat(temp, priorityObject->model);
temp = strcat(temp, "\n");
bprint(2, temp);
}
		if (priorityObject != self->b_lastGoalTarget)
		{
			self->target1 = self->target2 = self->target3 = self->target4 = world; // PZ: clear the target stack (10-2-15)
			frik_addToTargetStack(priorityObject);

			//if (/*largeScope*/!frik_isVisible_Simple(priorityObject)) // PZ: used frik_isVisible_Simple()
			//{
				//frik_findRouteToObject(priorityObject, #FALSE); // not `direct`
				self->b_routeInvalid = PR_TRUE; // PZ: changed above line to this (10-2-15)
				//self.b_aiflags = self.b_aiflags | #AI_WAIT;      // don't move until we have a path for you to follow
			//}
			self->b_lastGoalTarget = priorityObject;
			return PR_TRUE;
		}
	//}
	return PR_FALSE;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_decideWhetherToDropTarget  (PZ: was called "bot_check_lost")

Decides whether target `targ` should be dropped.

PZ NOTE: I'm guessing that all pursuable items (all priority items,
determined in frik_determinePriorityOfObject()) should be listed here
so that bot can stop pursuing them.

`self` = current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_decideWhetherToDropTarget(entity targ)
{
	vector distanceToTarg;

	/*distanceToTarg = frik_getRealOriginOfEntity(targ) - self.origin;
	distanceToTarg_z = 0;*/                                // PZ: this wasn't being used

	if (targ == world)
		return;

	// waypoints and items are removed from the target stack if bot gets close enough to them,
	// because the bot has reached them successfully
	if (targ->flags & PR_FL_ITEM)
	{
		if (vlen(targ->origin - self->origin) < 32)
		{
//bprint(2, "$$$$$$ (1) DROPPING TARGET FOR: ", targ.owner.netname, "\n");
			frik_removeFromTargetStack(targ);
		}
		// PZ: we can't just look at .model because most capture points don't have a model
		else if (targ->model == string_null && targ->classname != "info_tfgoal") // no model means target is no longer available
		{
//bprint(2, "$$$$$$ (2) DROPPING TARGET FOR: ", targ.owner.netname, "\n");
			frik_removeFromTargetStack(targ);
		}
		// PZ: for TF flags
		else if (targ->classname == "item_tfgoal" /*&& targ.owner.classname == "player"*/)
		{
			// PZ: if anyone has the flag, drop it as a target
			if (targ->owner->classname == "player")
			{
//bprint(2, "$$$$$$ SOMEONE GOT THE FLAG: ", targ.owner.netname, "\n");
				frik_removeFromTargetStack(targ);
			}
			// PZ: If we are pursuing our team's flag, we just need to see the flag; we don't need to touch it. (This is the bot checking on the flag -- hanging around it.)
			//goalTarget = frik_getGoalTarget(); // the highest numbered target
			else if (/*targ.classname == "item_tfgoal" &&*/ targ->model != string_null && targ->team_no != self->team_no && targ->team_no != 0)
			{
				if (frik_isVisible_Simple(targ)) // we are in a position to see the flag
				if (frik_isInFovOfSelf(targ))    // we /can/ see it, if we're close enough
				{
					distanceToTarg = frik_getRealOriginOfEntity(targ) - self->origin; // get direction/distance to our flag
					if (vlen(distanceToTarg) <= 30 * PR_ONE_FOOT)       // we've seen the flag (close enough)
					{
//bprint(2, self.netname, ": $$$$$$ I see my flag; dropping it as my target.""\n");
						frik_removeFromTargetStack(targ);
					}
				}
			}

			// give the flag owner the route table, if a bot, so that the owner can immediately figure out where to go next
			/*if (!targ.owner.ishuman)
			{
				// STOP MOVING THE WRONG WAY WHEN YOU GET THE FLAG
//bprint(2, "$$$$$$ GIVING ROUTE TABLE TO BOT WITH FLAG: ", targ.owner.netname, "\n");
				route_table = targ.owner;
				frik_clearRouteTable();
				targ.owner.enemy = targ.owner.target1 = targ.owner.target2 = targ.owner.target3 = targ.owner.target4 = world;
				targ.owner.b_aiflags | #AI_WAIT;
				targ.owner.keys = targ.owner.keys & #KEY_MASK_LOOKING_KEYS;
			}*/
		}
	}
	/*else if (targ.classname == "waypoint")
	{
		if (!(self.b_aiflags & (#AI_SNIPER | #AI_AMBUSH)))
		{
			if (self.b_aiflags & #AI_RIDE_TRAIN)
			{
				if (vlen(targ.origin - self.origin) < 48)
					frik_removeFromTargetStack(targ);
			}
			else if (self.b_aiflags & #AI_PRECISION)
			{
				if (vlen(targ.origin - self.origin) < 24)
					frik_removeFromTargetStack(targ);
			}
			else if (vlen(targ.origin - self.origin) < 32)
				frik_removeFromTargetStack(targ);
		}
	}
	else if (targ.classname == "temp_waypoint")
	{
		if (vlen(targ.origin - self.origin) < 32)
			frik_removeFromTargetStack(targ);
	}*/
	// this seems to control following players (teammate players can be followed as well)
	else if (targ->classname == "player")
	{
		float isTeammateOrDisguisedAsTeammate;           isTeammateOrDisguisedAsTeammate = PR_FALSE;
		if      (Teammate(self->team_no, targ->team_no))         isTeammateOrDisguisedAsTeammate = PR_TRUE;
		else if (Teammate(self->team_no, targ->undercover_team)) isTeammateOrDisguisedAsTeammate = PR_TRUE;

		if (targ->health <= 0 && !(self->weapons_carried & PR_WEAP_MEDIKIT))
			frik_removeFromTargetStack(targ);
		// if I'm a medic and teammate's health is high and he's not infected, drop him                                           // TODO: check for a spy "teammate" here?
		else if (self->weapons_carried & PR_WEAP_MEDIKIT && targ->health >= targ->max_health && !(targ->tfstate & PR_TFSTATE_INFECTED) && Teammate(self->team_no, targ->team_no))
			frik_removeFromTargetStack(targ);
		// if my targ is a medic, but my health is good and I'm not infected, drop him as my target
		else if (targ->weapons_carried & PR_WEAP_MEDIKIT && self->health >= self->max_health && !(self->tfstate & PR_TFSTATE_INFECTED) && isTeammateOrDisguisedAsTeammate)
			frik_removeFromTargetStack(targ);
		// TODO: I'M TRYING TO FIGURE OUT HOW TO TELL MEDIC WHEN TO STOP PURSUING SOMEONE TO HEAL THEM, BUT IT'S IMPORTANT TO KNOW /WHY/ THE BOT IS PURSUING A PLAYER.
		//       THAT INFORMATION IS CURRENTLY NOT BEING SAVED.
		/*else if ((coop) || (teamplay && targ.team_no == self.team_no)) // PZ: changed .team to .team_no
		{
			if (targ.target1.classname == "player")   // if this target is a friendly bot and its target1 is a player??
			{
				if (!targ.target1.ishuman)            // and if the target1 is a bot??
					frik_removeFromTargetStack(targ);     // ?? don't follow a bot that is following a bot??
			}
			else if (targ.teleport_time > time)       // stop following to avoid telefragging?
			{
				// try not to telefrag teammates
				self.keys = self.keys & #KEY_MASK_LOOKING_KEYS;         // turn off all movement keys
			}
			else if (vlen(targ.origin - self.origin) < 128)
			{
				if (vlen(targ.origin - self.origin) < 48)
					frik_botMoveThisWay(self.origin - targ.origin);     // ?? move back? don't follow too closely?
				else
				{
					self.keys = self.keys & #KEY_MASK_LOOKING_KEYS; // turn off all movement keys; stop following for a moment; close enough to them?
					frik_botConsiderStartingConversation(#TOPIC_SELF_INFORMS_TEAM_COVERING, 0.2); // because we are going to tell them we are following them?
				}
				self.search_time = time + 5; // never time out
			}
			else if (!frik_isVisible_Complex(targ))    // looks like we lost sight of the teammate we were following
				frik_removeFromTargetStack(targ);
		}
		else if (waypoint_mode > #WM_LOADED)            // if we are in the waypoint editor
		{
			if (vlen(targ.origin - self.origin) < 128) // if the bot has reached the player target, remove the player as its target
				frik_removeFromTargetStack(targ);
		}*/
	}
	// buttons are dropped when they are pressed or shot
	/*else if (targ.classname == "func_button")
	{
		if (targ.frame)  // PZ NOTE: Not 0. I'm assuming 0 is the idle, untriggered state.
		{
			frik_removeFromTargetStack(targ);
			if (self.enemy == targ)        // if you are 'attacking' this button, stop attacking it
				self.enemy = world;
			//if (self.target1)
			//	frik_findRouteToObject(self.target1, #TRUE);
		}
	}*/
	// trigger_multiple style triggers are lost if their thinktime changes (PZ NOTE: if they have been triggered, I'm assuming)
	// PZ: ## can this be a waypoint? I made waypoints use #MOVETYPE_NOCLIP
	/*else if ((targ.movetype == #MOVETYPE_NONE) && (targ.solid == #SOLID_TRIGGER))
	{
		if (targ.nextthink >= time)
		{
			frik_removeFromTargetStack(targ);
			//if (self.target1)
			//	frik_findRouteToObject(self.target1, #TRUE);
		}
	}
	// lose any target way above the bot's head (only if the target is very close)
	// FIXME: if the bot can fly in your mod..
	if ((targ.origin_z - self.origin_z) > 64)
	{
		distanceToTarg = targ.origin - self.origin;
		distanceToTarg_z = 0;           // just looking at the horizontal distance
		if (vlen(distanceToTarg) < 32)
			if (self.flags & #FL_ONGROUND)
				if (!frik_detectAndHandlePlatform(#FALSE))
					frik_removeFromTargetStack(targ);
	}
	else if (targ.classname == "train")
	{
		if (frik_detectAndHandlePlatform(#FALSE))
			frik_removeFromTargetStack(targ);
	}
	// targets are lost if the bot's search time has expired
	if (time > self.search_time)
		frik_removeFromTargetStack(targ);*/







// PZ OLD CODE BELOW
return;

	// waypoints and items are removed from the target stack if bot gets close enough to them,
	// because the bot has reached them successfully
	if (targ->flags & PR_FL_ITEM)
	{
		if (vlen(targ->origin - self->origin) < 32)
		{
//bprint(2, "$$$$$$ (1) DROPPING TARGET FOR: ", targ.owner.netname, "\n");
			frik_dropTarget(targ, PR_TRUE);
		}
		// PZ: we can't just look at .model because most capture points don't have a model
		else if (targ->model == string_null && targ->classname != "info_tfgoal") // no model means target is no longer available
		{
//bprint(2, "$$$$$$ (2) DROPPING TARGET FOR: ", targ.owner.netname, "\n");
			frik_dropTarget(targ, PR_TRUE);
		}
		// PZ: if anyone has the flag, drop it as a target
		/*else*/ if (targ->owner->classname == "player")
		{
//bprint(2, "$$$$$$ SOMEONE GOT THE FLAG: ", targ.owner.netname, "\n");
			frik_dropTarget(targ, PR_FALSE);
			// give the flag owner the route table, if a bot, so that the owner can immediately figure out where to go next
			if (!targ->owner->ishuman)
			{
				// STOP MOVING THE WRONG WAY WHEN YOU GET THE FLAG
//bprint(2, "$$$$$$ GIVING ROUTE TABLE TO BOT WITH FLAG: ", targ.owner.netname, "\n");
				route_table = targ->owner;
				frik_clearRouteTable();
				targ->owner->enemy = targ->owner->target1 = targ->owner->target2 = targ->owner->target3 = targ->owner->target4 = world;
				//targ->owner->b_aiflags | PR_AI_WAIT;  // this doesn't do anything; what was I going for?
				targ->owner->keys = targ->owner->keys & PR_KEY_MASK_LOOKING_KEYS;
			}
		}
	}
	else if (targ->classname == "waypoint")
	{
		if (!(self->b_aiflags & (PR_AI_SNIPER | PR_AI_AMBUSH)))
		{
			if (self->b_aiflags & PR_AI_RIDE_TRAIN)
			{
				if (vlen(targ->origin - self->origin) < 48)
					frik_dropTarget(targ, PR_TRUE);
			}
			else if (self->b_aiflags & PR_AI_PRECISION)
			{
				if (vlen(targ->origin - self->origin) < 24)
					frik_dropTarget(targ, PR_TRUE);
			}
			else if (vlen(targ->origin - self->origin) < 32)
				frik_dropTarget(targ, PR_TRUE);
		}
	}
	else if (targ->classname == "temp_waypoint")
	{
		if (vlen(targ->origin - self->origin) < 32)
			frik_dropTarget(targ, PR_TRUE);
	}
	// this seems to control following players (teammate players can be followed as well)
	else if (targ->classname == "player")
	{
		if (targ->health <= 0)
			frik_dropTarget(targ, PR_TRUE);
		else if ((coop) || (teamplay && targ->team_no == self->team_no)) // PZ: changed .team to .team_no
		{
			if (targ->target1->classname == "player")   // if this target is a friendly bot and its target1 is a player??
			{
				if (!targ->target1->ishuman)            // and if the target1 is a bot??
					frik_dropTarget(targ, PR_TRUE);     // ?? don't follow a bot that is following a bot??
			}
			else if (targ->teleport_time > time)       // stop following to avoid telefragging?
			{
				// try not to telefrag teammates
				self->keys = self->keys & PR_KEY_MASK_LOOKING_KEYS;         // turn off all movement keys
			}
			else if (vlen(targ->origin - self->origin) < 128)
			{
				if (vlen(targ->origin - self->origin) < 48)
					frik_botMoveThisWay(self->origin - targ->origin);     // ?? move back? don't follow too closely?
				else
				{
					self->keys = self->keys & PR_KEY_MASK_LOOKING_KEYS; // turn off all movement keys; stop following for a moment; close enough to them?
					frik_botConsiderStartingConversation(PR_TOPIC_SELF_INFORMS_TEAM_COVERING, 0.2); // because we are going to tell them we are following them?
				}
				self->search_time = time + 5; // never time out
			}
			else if (!frik_isVisible_Complex(targ))    // looks like we lost sight of the teammate we were following
				frik_dropTarget(targ, PR_FALSE);
		}
		else if (waypoint_mode > PR_WM_LOADED)            // if we are in the waypoint editor
		{
			if (vlen(targ->origin - self->origin) < 128) // if the bot has reached the player target, remove the player as its target
				frik_dropTarget(targ, PR_TRUE);
		}
	}
	// buttons are dropped when they are pressed or shot
	else if (targ->classname == "func_button")
	{
		if (targ->frame)  // PZ NOTE: Not 0. I'm assuming 0 is the idle, untriggered state.
		{
			frik_dropTarget(targ, PR_TRUE);
			if (self->enemy == targ)        // if you are 'attacking' this button, stop attacking it
				self->enemy = world;
			//if (self.target1)
			//	frik_findRouteToObject(self.target1, #TRUE);
		}
	}
	// trigger_multiple style triggers are lost if their thinktime changes (PZ NOTE: if they have been triggered, I'm assuming)
	// PZ: ## can this be a waypoint? I made waypoints use #MOVETYPE_NOCLIP
	else if ((targ->movetype == PR_MOVETYPE_NONE) && (targ->solid == PR_SOLID_TRIGGER))
	{
		if (targ->nextthink >= time)
		{
			frik_dropTarget(targ, PR_TRUE);
			//if (self.target1)
			//	frik_findRouteToObject(self.target1, #TRUE);
		}
	}
	// lose any target way above the bot's head (only if the target is very close)
	// FIXME: if the bot can fly in your mod..
	if ((targ->origin[Z] - self->origin[Z]) > 64)
	{
		distanceToTarg = targ->origin - self->origin;
		distanceToTarg[Z] = 0;           // just looking at the horizontal distance
		if (vlen(distanceToTarg) < 32)
			if (self->flags & PR_FL_ONGROUND)
				if (!frik_detectAndHandlePlatform(PR_FALSE))
					frik_dropTarget(targ, PR_FALSE);
	}
	else if (targ->classname == "train")
	{
		if (frik_detectAndHandlePlatform(PR_FALSE))
			frik_dropTarget(targ, PR_TRUE);
	}
	// targets are lost if the bot's search time has expired
	if (time > self->search_time)
		frik_dropTarget(targ, PR_FALSE);
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botAdjustAim    (PZ: was called "bot_angle_set")

Sets the bot's commanded/ideal angle (.b_commandedViewAngle).
Then, adjusts its actual aim (.v_angle) according
to .b_commandedViewAngle. Its aiming priority is its .enemy
(its acquired target). Priority 2 is its .target1
(when it has no .enemy, but has something to pursue).

For b_commandedViewAngle and v_angle:
  x = pitch (vertical aim)
  y = yaw   (horizontal aim)
  z = would assumably be roll (lean), but I don't think it works when you set it.

## frik_turnInputCommandsIntoActions() is currently handling some aiming as well. PZ TODO We should put all aiming code in one place. ##

`self` = current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_botAdjustAim()
{
	float h;
	vector vectorToAim;

	if (self->is_feigning) return; // PZ: don't turn while feigning

	// ######################################################
	// [set the commanded angle (.b_commandedViewAngle)]
	// ######################################################
	// if the bot has an enemy (something that it is attacking)
	if (self->enemy != world)
	{
		if (self->enemy->items & PR_IT_INVISIBILITY) // PZ TODO: do we need to add thief and so on here? yes
			if (random() > 0.01) // 99% chance on each iteration we won't aim at him
				return;
		// PZ NOTE: .missile_speed is set when aim() is called for the bot. It is the speed given to the aim() function, so that the
		//          function can lead shots on moving targets, based on the speed of the missile that they are firing.
		if (self->missile_speed == 0)
			self->missile_speed = 10000;

		if (self->enemy->solid == PR_SOLID_BSP)  // ?if our enemy is a shootable trigger/button
			vectorToAim = ((self->enemy->absmin + self->enemy->absmax) * 0.5) - self->origin;
		else
		{
			// PZ TODO: bot doesn't lead his target well, even nightmare
			float levels = 3;
			vector shootAtFeet;         // PZ: added
			// PZ: For weapons with explode-on-contact radius damage: If the bot is high-skilled, it jumps first and shoots at the enemy's feet,
			//     but only if the enemy is not above them.                     // +20 for jump height          // -24 for feet position (mins_z)
			if (self->b_skill >= PR_SKILL_HARD && (self->origin[Z] + self->view_ofs[Z]/* + 20*/) >= (self->enemy->origin[Z] - 24) &&
			    (self->current_weapon == PR_WEAP_ROCKET_LAUNCHER || self->current_weapon == PR_WEAP_INCENDIARY || self->current_weapon == PR_WEAP_DAEDALUS) &&
			    self->enemy->velocity[Z] <= 0)   // he should be on the ground, or falling to the ground
				shootAtFeet = V({0, 0, 28}); // or should that be 24? (mins_z)
			else
				shootAtFeet = V({0, 0, 0});
			vectorToAim = self->enemy->origin - shootAtFeet;
			while (levels)
			{
				h = vlen(vectorToAim - self->origin) / self->missile_speed;
				if (self->enemy->flags & PR_FL_ONGROUND)
					vectorToAim = self->enemy->velocity * h + V({0, 0, -20});
				else
					vectorToAim = (self->enemy->velocity - (sv_gravity * V({0, 0, 1})) * h) * h;
				vectorToAim = self->enemy->origin - shootAtFeet + vectorToAim;
				traceline(self->enemy->origin - shootAtFeet, vectorToAim, PR_TL_ANY_SOLID, self);
				vectorToAim = trace_endpos;
				levels = levels - 1;
			}
			vectorToAim = normalize(vectorToAim - self->origin);
		}
		vectorToAim = vectoangles(vectorToAim);
		vectorToAim[X] = vectorToAim[X] * -1;
		self->b_commandedViewAngle = vectorToAim;
	}
	// if the bot doesn't have an enemy, but has a priority object that it is pursuing
	else if (self->target1 != world)
	{
		vectorToAim = frik_getRealOriginOfEntity(self->target1); // get position
		if (self->target1->flags & PR_FL_ITEM)
			vectorToAim = vectorToAim + V({0, 0, 48}); // raise it up by 48
		vectorToAim = vectorToAim - (self->origin + self->view_ofs);
		vectorToAim = vectoangles(vectorToAim);
		vectorToAim[X] = vectorToAim[X] * -1;
		self->b_commandedViewAngle = vectorToAim;
	}
	// if the bot isn't attacking or pursuing anything
	else
		self->b_commandedViewAngle[X] = 0; // pitch should be level

	// PZ TODO: ### Organize the below code somewhere!

	// ###########################################################################################
	// [use the commanded angle to make actual aiming adjustments (true aiming angle is .v_angle)]
	// ###########################################################################################
	// PZ TODO: we should have all aiming code in one area, if we can do that
	// HACK HACK HACK HACK
	// PZ NOTE: just snap the aim to the target
	if (self->b_skill == PR_SKILL_NIGHTMARE)
	{
		self->keys = self->keys & PR_KEY_MASK_MOVEMENT_KEYS;  // turn off look keys
		self->v_angle = self->b_commandedViewAngle;
		while (self->v_angle[X] < -180)
			self->v_angle[X] = self->v_angle[X] + 360;
		while (self->v_angle[X] > 180)
			self->v_angle[X] = self->v_angle[X] - 360;
	}
	// The bot falls off ledges a lot because of "turning around"
	// so let the bot use instant turn around when not hunting a player (PZ: why not include with above block?)
	// PZ: Took this out on Feb 17, 2015. I want the bots to appear realistic. This is not realistic. It especially
	// is annoying to watch this when spectating the bots in first-person perspective. I'm fine with nightmare bots
	// working this way, though, because they are supposed to be insanely good.
	/*else if ((self.enemy == world || self.enemy.movetype == #MOVETYPE_PUSH) && self.target1.classname != "player")
	{
		self.keys = self.keys & #KEY_MASK_MOVEMENT_KEYS;  // turn off look keys
		self.v_angle = self.b_commandedViewAngle;
		while (self.v_angle_x < -180)
			self.v_angle_x = self.v_angle_x + 360;
		while (self.v_angle_x > 180)
			self.v_angle_x = self.v_angle_x - 360;
	}*/
	// PZ: this now only handles EASY; was "< #SKILL_HARD" (aiming for MEDIUM was too bad)
	// PZ NOTE: I think this is meant to simulate a keyboarder. I think keyboarders should exist in easy skill only.
	else if (self->b_skill == PR_SKILL_EASY) // skill 2 handled in bot_phys (PZ: medium and hard handled there now)
	{
		if (self->b_commandedViewAngle[X] > 180)
			self->b_commandedViewAngle[X] = self->b_commandedViewAngle[X] - 360;
		self->keys = self->keys & PR_KEY_MASK_MOVEMENT_KEYS;  // release turning/looking keys

		// PZ: yaw also sucked at 10 deg; bot couldn't hit anything; and never corrected
		if (frik_angleSubtraction(self->b_commandedViewAngle[Y], self->v_angle[Y]) > 5)  // yaw
			self->keys = self->keys | PR_KEY_LOOKLEFT;
		else if (frik_angleSubtraction(self->b_commandedViewAngle[Y], self->v_angle[Y]) < -5)
			self->keys = self->keys | PR_KEY_LOOKRIGHT;
		// PZ: pitch was in toleration of 10 deg, but it was way too far off
		if (frik_angleSubtraction(self->b_commandedViewAngle[X], self->v_angle[X]) < -5) // pitch
			self->keys = self->keys | PR_KEY_LOOKUP;
		else if (frik_angleSubtraction(self->b_commandedViewAngle[X], self->v_angle[X]) > 5)
			self->keys = self->keys | PR_KEY_LOOKDOWN;
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botChooseRole

This is called by the bot's role timer. This makes the bot choose a role (ROLE_*)
every few minutes. A role is whether it's on offense, defense, a capper, engineer,
etc.

`self`                 = bot's role timer
`self.owner`           = the bot this timer runs for
`self.owner.b_botRole` = the bot's role

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_botChooseRole()
{
	entity bot;
	bot = self->owner;

	bot->b_botRole = V({0, 0, 0}); // initialize to no role

	float rnum;
	rnum = random();

	// what's his class (.playerclass), or what weapons does he have?
	if (bot->playerclass == PR_PC_SCOUT)
	{
		bot->b_botRole = setVectorBitFlag(PR_ROLE_OFFENSE, bot->b_botRole);
		bot->b_botRole = setVectorBitFlag(PR_ROLE_CAPPER, bot->b_botRole);
	}
	else if (bot->weapons_carried & PR_WEAP_SNIPER_RIFLE)
	{
		bot->b_botRole = setVectorBitFlag(PR_ROLE_DEFENSE, bot->b_botRole);
		bot->b_botRole = setVectorBitFlag(PR_ROLE_SNIPER, bot->b_botRole);
	}
	else if (bot->playerclass == PR_PC_SOLDIER || bot->playerclass == PR_PC_DEMOMAN || bot->playerclass == PR_PC_HVYWEAP || bot->playerclass == PR_PC_PYRO)
	{
		// he has a 1/5 chance he's a deathmatcher; else, it's split half and half between offense and defense (There are too many guys on D. Making D less often.)
		if      (rnum >= 0.40/*0.60*/) bot->b_botRole = setVectorBitFlag(PR_ROLE_OFFENSE, bot->b_botRole);
		else if (rnum >= 0.20) bot->b_botRole = setVectorBitFlag(PR_ROLE_DEFENSE, bot->b_botRole);
		// else, neither flag is set; he's a roamer/deathmatcher
	}
	else if (bot->playerclass == PR_PC_MEDIC)
	{
		if (rnum >= 0.25/*0.5*/) bot->b_botRole = setVectorBitFlag(PR_ROLE_OFFENSE, bot->b_botRole);
		else             bot->b_botRole = setVectorBitFlag(PR_ROLE_DEFENSE, bot->b_botRole);
		bot->b_botRole = setVectorBitFlag(PR_ROLE_MEDIC, bot->b_botRole);
	}
	else if (bot->playerclass == PR_PC_SPY)
	{
		bot->b_botRole = setVectorBitFlag(PR_ROLE_OFFENSE, bot->b_botRole); // maybe one day I'll add a defensive spy but not right now
		bot->b_botRole = setVectorBitFlag(PR_ROLE_SPY, bot->b_botRole);
	}
	else if (bot->playerclass == PR_PC_ENGINEER)
	{
		bot->b_botRole = setVectorBitFlag(PR_ROLE_DEFENSE, bot->b_botRole); // maybe one day I'll add a offensive engineer but not right now
		bot->b_botRole = setVectorBitFlag(PR_ROLE_ENGINEER, bot->b_botRole);
	}

//bot.b_botRole = setVectorBitFlag(#ROLE_OFFENSE, bot.b_botRole);
//bot.b_botRole = clearVectorBitFlag(#ROLE_DEFENSE, bot.b_botRole);

	// how soon should we choose a role again?
	self->nextthink = time + 5*60 + random()*5*60;  // between 5 and 10 minutes from now
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botRoamWaypoints

PZ: Makes the bot randomly traverse waypoints.

`self` = current bot
`self.target1`     = waypoint bot is pursuing
`self.current_way` = waypoint bot last reached
`self.last_way`    = waypoint bot reached before that

[last_way]--------------[current_way]-------->      [target1]

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_botRoamWaypoints()
{
	//local entity startingWaypoint;
	vector v;

//self.b_aiflags = (self.target1.b_aiflags & #AI_ON_PURSUING_WAYPOINT) | (self.last_way.b_aiflags & #AI_ON_REACHING_WAYPOINT);

	if (self->target1 == world)
	{
		string tstr;
		//self.target1 = frik_findWaypointNearestAndVisibleToSelf(world);
		self->target1 = frik_findWaypointToRoamTo();
		// Readahead types are AI instructions to perform while heading to a waypoint.
		self->b_aiflags = self->b_aiflags | (self->target1->b_aiflags & PR_AI_ON_PURSUING_WAYPOINT);
		//tstr = vtos(self.target1.
		//bprint(2, "Found ", self.target1);
		//eprint(self.target1);
	}
	// move to it...
	v = frik_getRealOriginOfEntity(self->target1) - self->origin; // get distance to self.target1
	if (vlen(v) < 32) // we've reached the waypoint
	{
		self->last_way = self->current_way;
		self->current_way = self->target1;
		self->target1 = world; // remove target1 instead, so that next on stack can move up
		// Point types are AI instructions that should be executed once reaching a waypoint.
		self->b_aiflags = self->b_aiflags | (self->current_way->b_aiflags & PR_AI_ON_REACHING_WAYPOINT);
		self->keys = self->keys & PR_KEY_MASK_LOOKING_KEYS; // stop moving; we are on our self.target1, basically
	}
	else
	{
		// set the needed keys to make the bot move toward its .target1
		v = normalize(v); // get direction to target1
		self->keys = (self->keys & PR_KEY_MASK_LOOKING_KEYS) + frik_getRequiredKeysToMoveThisDirection(v);
	}
}

float stagger_think;
/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_AI_Main            (PZ: was called "BotAI")

This is the main AI routine. It's called every frame,
though not everything in the bot's AI runs every frame (TODO: make this a true statement).

`self` = current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float runOnce;
void frik_AI_Main()
{
	float randomNumber; // PZ
	entity e = world;   // PZ

	// #########################################
	// Initialization for AI routines.
	// #########################################

	// every frame we initialize the bot's buttons to unpressed; various functions will turn them on by AI end
	// PZ: Check to see whether bot was asked to jump by external code, not inside its frame of execution. (Currently only used by frik_checkForStuckBot().)
	if (self->keys & PR_KEY_AUX_JUMP) self->PR_BUTTON_JUMP = PR_TRUE;
	else self->PR_BUTTON_JUMP = 0;
	self->PR_BUTTON_FIRE = 0;
	self->impulse = 0;       // added on 12-28-2015
	self->keys = 0;
	self->b_aiflags = self->b_aiflags - (self->b_aiflags & PR_AI_WAIT); // make sure not waiting

	if (self->b_chattime > 0) return; // PZ: he can't really do anything, if he's busy typing

	// #########################################
	// Handle menus.
	// #########################################

	// PZ: deal with menus when you join the server
	//if (/*self.current_menu == 1 || */self.current_menu == 5) // #MENU_DEFAULT || #MENU_INTRO  // warning, bot couldn't seem to respawn when using 1 here
	if (self->motd < PR_MOTD_FINISHED - 2)
	{
		self->impulse = 10; // skip message
		return;
	}
	if (self->current_menu == 2) // #MENU_TEAM
	{
		// give a random chance that bot will not use autoteam and will not make teams even (completely even teams (skill and playercount wise) make for
		// boring games)
		if (number_of_teams == 2 && playersOnTeam1 && playersOnTeam2) // if it's a standard TF map, and both teams have at least 1 player
		{
			randomNumber = random();
			if (randomNumber >= 0.80)       // was 0.75
				self->impulse = 1;          // select team 1
			else if (randomNumber >= 0.60)  // was 0.50
				self->impulse = 2;          // select team 2
			else
				self->impulse = 5;          // select autoteam
		}
		else                                // likely has 4 teams
			self->impulse = 5;
		return;
	}
	if (self->current_menu == 3) // #MENU_CLASS
	{
		// [Determine our number of options.]
		float numberOfOptions;
		if (agr) // Attackers Go Red mode
		{
			if (self->team_no == 1)   // defense (blue)
				numberOfOptions = 7;  // no scouts and no spies
			else                      // offense (red)
				numberOfOptions = 8;  // no engineer  (what about snipers?)
		}
		else     // not AGR
			numberOfOptions = 9;

		// [Pick a random option from the available options.]
		// Generate a number from 1 to `numberOfOptions`.
		float numberOfSteps;
		numberOfSteps = random() * numberOfOptions;
		numberOfSteps = ceil(numberOfSteps);
		if (numberOfSteps == 0) numberOfSteps = 1;

		self->impulse = 0;
		while (numberOfSteps > 0)
		{
			self->impulse = self->impulse + 1;
			// account for Attackers Go Red class restrictions on bots
			if (agr)
			{
				if (self->team_no == 1)    // defense (blue)
				{
					if (self->impulse == 1 || self->impulse == 8) // no scouts and no spies
						self->impulse = self->impulse + 1;
				}
				else                       // offense (red)
				{
					if (self->impulse == 9)                       // no engineer  (what about snipers?)
						self->impulse = self->impulse + 1;
				}
			}

			if (self->impulse > 9) self->impulse = 1; // shouldn't happen, but just in case
			numberOfSteps = numberOfSteps - 1;
		}

		// PZ NOTE: the old class selection code
		/*randomNumber = random();
		randomNumber = randomNumber * 8; // 9 choices for class, minus 1 for the number 0
		randomNumber = rint(randomNumber);
		randomNumber = randomNumber + 1;
		self.impulse = randomNumber;*/

		// set up a timer entity for the bot that will make the bot choose a role every so often
		entity timer;
		timer = spawnServerSide();
		timer->classname = "bot_role_selection_timer";
		timer->owner = self;
		timer->think = frik_botChooseRole;
		timer->nextthink = time + 1.5; // it will choose a role in about 1.5 seconds
		timer->deadflag = PR_REMOVE_TIMER_ON_DISCONNECT; // remove this timer when owning bot leaves

		// set up a timer entity for the bot that will check whether the bot is stuck every second or so
		timer = spawnServerSide();
		timer->classname = "bot_stuck_detection_timer";
		timer->owner = self;
		timer->origin = self->origin;
		timer->think = frik_checkForStuckBot;
		timer->nextthink = time + 2; // it will start checking in about 2 seconds
		timer->deadflag = PR_REMOVE_TIMER_ON_DISCONNECT; // remove this timer when owning bot leaves

		return;
	}

	// #########################################
	// Handle (re)spawning.
	// #########################################

	// am I dead? Fire randomly until I respawn
	// health < 1 is used because fractional healths show up as 0 on normal player
	// status bars, and the mod probably already compensated for that
	// PZ: They now progress through map changes (through intermissions) and don't respawn when there are no humans in the server. (8/10/15)
	if (((num_players > bot_count || num_specs > 0) && self->health < 1 && self->respawn_time <= time) || intermission_running)
	{
		self->PR_BUTTON_FIRE = floor(random() * 2);
		self->PR_BUTTON_JUMP = 0;
		self->keys = 0;
		self->b_aiflags = 0;
		frik_clearRouteForSelf();
		self->target1 = self->target2 = self->target3 = self->target4 = self->enemy = world;
		self->last_way = self->current_way = world;
		return;
	}

	// if we haven't spawned yet, don't continue
	//if (self.modelindex == modelindex_null || self.model == string_null)   // PZ NOTE: This messes up times when bot is invisible.
	if (self->health <= 0 || self->deadflag != PR_DEAD_NO || self->playerclass == PR_PC_UNDEFINED)
		return;

	// ##########################################
	// Gather data that is needed.
	// #########################################

	// Call a function here to determine whether .b_botInEnemyFOV. Don't call it every frame.
	// Then, later, the AI can use that information to make decisions, like whether to hide (stop moving) while thiefing.
	isBotInEnemyFieldOfView();

	// Don't know where else to put this right now. I just want to make sure bots are playing the correct role (offense/defense) while in AGR or Invade.
	if (agr)
	{
		if (self->team_no == 1) // defense (blue)
		{
			self->b_botRole = clearVectorBitFlag(PR_ROLE_OFFENSE, self->b_botRole);
			self->b_botRole =   setVectorBitFlag(PR_ROLE_DEFENSE, self->b_botRole);
		}
		else                    // offense (red)
		{
			self->b_botRole = clearVectorBitFlag(PR_ROLE_DEFENSE, self->b_botRole);
			self->b_botRole =   setVectorBitFlag(PR_ROLE_OFFENSE, self->b_botRole);
		}
	}
	else if (invade)
	{
		if (self->team_no == invade_teamOnDef) // defense
		{
			self->b_botRole = clearVectorBitFlag(PR_ROLE_OFFENSE, self->b_botRole);
			self->b_botRole =   setVectorBitFlag(PR_ROLE_DEFENSE, self->b_botRole);
		}
		else                                  // offense
		{
			self->b_botRole = clearVectorBitFlag(PR_ROLE_DEFENSE, self->b_botRole);
			self->b_botRole =   setVectorBitFlag(PR_ROLE_OFFENSE, self->b_botRole);
		}
	}

	// ##########################################
	// Decide action to take.
	// Determine target for pursuit. (e.g., find a waypoint to roam to, find an item to pursue, find an enemy to attack, etc.)
	// ##########################################

	// [Decide on something to pursue.]
	float remainder, dividend;
	bool botHasNewGoalTarget = false; // bool
	entity goalTarget = frik_getGoalTarget(); // the highest numbered target

	frik_decideWhetherToDropTarget(goalTarget); // it will drop the target if it decides to
string temp;
	// look for something to pursue; if we have something to pursue, find route to it
	// TODO: make it so that the bot can't look for a target but so often (an amount of time has to pass inbetween searches)
	if (self->b_roamingWaypoints || (/**/self->target1 == world &&/**/ !self->b_pursuingEnemy)) // added check for b_roamingWaypoints (9-25-15) DIDN'T WORK FOR SOME REASON.
	{
		// We don't need this running every frame (we may be able to make use of the field, .ai_time, as well).
		// NO LONGER TRUE ---> bot_frameCounter loops back to 1 every 32 frames. So, it counts 1 to 32, and back to 1.
		// This means run the following every 32 frames (interval). Make sure the frame interval is a multiple of 32.
		dividend  = bot_frameCounter + self->b_clientno - 1; // this offsets the frame counter by the bot's number
		remainder = modulo(dividend, 60);//max_clients); // This allows all bots to run this every 60 frames, but with bots evenly spread out over these frames.
		if (remainder == 0) // Using `b_clientno` as we are doing makes this never run more than one bot on a frame.  */
		// Way simpler way...   <--- Not anymore...
		//if (modulo(bot_frameCounter, self.b_clientno) == 0)
		// TODO: rewrite frik_determinePriorityOfObject() to make it like the weapon selection code, and needs to take into account distance to objects
		{
//temp = ftos(e.count);
//bprint(2, "Running frik_findPriorityObjectToPursue() for ", self.netname, "\n");
			botHasNewGoalTarget = frik_findPriorityObjectToPursue(PR_TRUE); // true means look far and wide; we will use the route table to find a path to any far away object
		}
	}
	if ((botHasNewGoalTarget && self->target1 != world) && self->b_routeInvalid && !self->b_pursuingEnemy)
	{
//bprint(2, "Running frik_findRouteToObject() for ", self.netname, "\n");
		frik_findRouteToObject(self->b_lastGoalTarget, PR_FALSE);
		self->b_roamingWaypoints = PR_FALSE;
	}
	// use waypoints for navigating to target, if needed
	if (self->target1 != world && self->target1->classname != "waypoint" && self->target2->classname != "waypoint" &&
	                              self->target3->classname != "waypoint" && self->target4->classname != "waypoint")
	{
		//if (!frik_isVisible_Simple(self.target1))
		//{
			//e = frik_findNextWaypointInOurRoute(self.current_way);
			e = findNextWaypointInRouteForBot(self->current_way, self);
			if (e != world)
			{

if (PZ_DEBUG == self->b_clientno) {
temp = ftos(e->count);
bprint(2, "Next Waypoint in my route is waypoint number: ", temp, "\n");
}
				frik_addToTargetStack(e);
				self->b_aiflags = self->b_aiflags | (e->b_aiflags & PR_AI_ON_PURSUING_WAYPOINT); // NOTE: THIS DOES _NOT_ CLEAR OUT THE BOT'S AI FLAGS.
			}
		//}
	}

//bprint(2, "My Target Stack: ", self.target1.classname, ", ", self.target2.classname, ", ");
//bprint(2, self.target3.classname, ", ", self.target4.classname, "\n");

	// Just roam, if we don't have a target.
	//if (!isVectorBitFlagSet(#ROLE_OFFENSE, '0 0 0', self.b_botRole) && !isVectorBitFlagSet(#ROLE_DEFENSE, '0 0 0', self.b_botRole))
		if (self->target1 == world && !self->b_pursuingEnemy)
		{
			e = frik_findWaypointToRoamTo();
			self->b_roamingWaypoints = PR_TRUE;
			frik_addToTargetStack(e); // PZ: makes .target1 `e`
			self->b_aiflags = self->b_aiflags | (e->b_aiflags & PR_AI_ON_PURSUING_WAYPOINT);
		}

	// [Decide on something to attack.]
	// PZ: Lower-skilled bots are going to look less often. This will affect how quickly they are to notice and react to things,
	// like enemies going around a corner, exposing themselves to the bot.
	float attentiveness = 0;
	/*if      (self.b_skill == #SKILL_NIGHTMARE) attentiveness = 0.30; // he notices every damn thing, immediately (don't want this running constantly though, for lag reasons)
	else if (self.b_skill == #SKILL_HARD     ) attentiveness = 0.08; // the rest of these have human-level reaction speeds
	else if (self.b_skill == #SKILL_MEDIUM   ) attentiveness = 0.05;
	else if (self.b_skill == #SKILL_EASY     ) attentiveness = 0.03;
	if (random() <= attentiveness)*/
	// PZ: We are now basing this on frame number, rather than a random number, that way bots can't randomly ever all run at the same time, causing lag spikes.
	// NO LONGER TRUE ---->  bot_frameCounter loops back to 1 every 32 frames. So, it counts 1 to 32, and back to 1.
	// NO LONGER TRUE ---->  This means run the following every 32 frames (interval). Make sure the frame interval is a multiple of 32.
	// `attentiveness` is how often they run this code. An attentiveness of 3 means it runs every 3 frames. An attentiveness of 3 runs on frames 3, 6, 9, 12, 15, ...
	if      (self->b_skill == PR_SKILL_NIGHTMARE) attentiveness = 32; // he notices every damn thing, immediately (don't want this running constantly though, for lag reasons)
	else if (self->b_skill == PR_SKILL_HARD     ) attentiveness = 60; // the rest of these have human-level reaction speeds
	else if (self->b_skill == PR_SKILL_MEDIUM   ) attentiveness = 90;
	else if (self->b_skill == PR_SKILL_EASY     ) attentiveness = 120;
	dividend  = bot_frameCounter + self->b_clientno - 1; // this offsets the frame counter by the bot's number
	remainder = modulo(dividend, attentiveness);
	if (remainder == 0) // Using `b_clientno` as we are doing makes this never run more than one bot on a frame.   */
	{
//bprint(2, "Running frik_acquireTargetForAttacking() for ", self.netname, "\n");
		//frik_lookForHazardousObject();
		if (self->job & PR_JOB_THIEF && self->job & PR_JOB_ACTIVE)
		{
			self->enemy = world;
			if (!isVectorBitFlagSet(PR_ROLE_CAPPER, V({0, 0, 0}), self->b_botRole))
				if (random() <= 0.05)
					frik_acquireTargetForAttacking();
		}
		else
			frik_acquireTargetForAttacking(); // TODO: check first to see if our target1 should be our enemy in this function
	}

	// PZ TODO ###: Check to see whether there are any targets that should be dropped, like any reached buttons.

	// ##########################################
	// Perform action that you decided on.
	// Take action. (e.g., pursue target, fight enemy, perform task, etc.) // performing a task might be blowing up a wall with det or building a sentry
	// ##########################################

	// PZ TODO: Create an aim override AI flag. Use it above anywhere you want to set the bot's aim and don't want this function to change the aim.
	// [look/aim wherever you need to]
	frik_botAdjustAim();

	// If you have a .target1, pursue it.
	frik_botPursueTarget1();

	// If you have a .enemy, attack it. (You will only actually pursue your enemy, if he's also your .target1.)
	// Enemy is probably a field QC coders will most likely use a lot for their own needs, since it's unused on a normal player.
	// PZ TODO ###: I should probably take care of this at some point. It doesn't look like there are serious problems with this right now,
	// but the player entity does appear to use .enemy sometimes for things, in the CuTF code.
	if (self->enemy == world) self->b_pursuingEnemy = PR_FALSE; // just resetting the flag here; flag may get set in frik_engageTheEnemy(); // PZ: added condition on 10-4-15

	if (self->enemy != world)
	{
		if (self->job & PR_JOB_THIEF && self->job & PR_JOB_ACTIVE)
		{
			if (!isVectorBitFlagSet(PR_ROLE_CAPPER, V({0, 0, 0}), self->b_botRole))
				if (random() <= 0.05)
					frik_engageTheEnemy();
		}
		else
			frik_engageTheEnemy();
		handleCrusader();      // PZ: Hack to get bots to use crusader skill.
	}
	else if (random() < 0.002)
		frik_makeWeaponSelection(-1, PR_FALSE); // PZ NOTE: -1 because you don't have an enemy to have a range to
	else // reload sometimes when you aren't engaged in a fight, so that you will be ready for next fight
	{
		// PZ TODO: I don't like the impulse system. What if an impulse follows and clobbers this impulse? We need a function call instead, trySettingImpulse(),
		//          that first checks to see whether an impulse is already set. If one is already set, don't change it, or maybe allow changing if it's
		//          a high priority impulse.
		if      (self->b_skill == PR_SKILL_NIGHTMARE && random() <= 0.10)   self->impulse = PR_TF_RELOAD;
		else if (self->b_skill == PR_SKILL_HARD      && random() <= 0.05)   self->impulse = PR_TF_RELOAD;
		else if (self->b_skill == PR_SKILL_MEDIUM    && random() <= 0.025)  self->impulse = PR_TF_RELOAD;
		else if (self->b_skill == PR_SKILL_EASY      && random() <= 0.0125) self->impulse = PR_TF_RELOAD;
	}

	// TODO: Above, movement is performed to either make the bot pursue its target1 or to make it engage an enemy. Here, we want to override that movement if there is
	// a hazard in the way.


	// PZ TODO: Create an aim override AI flag. Use it above anywhere you want to set the bot's aim and don't want this function to change the aim.
	// [look/aim wherever you need to]
	// moved this up a few lines (4/21/15)
	//frik_botAdjustAim();

	// PZ: Call for medic if infected. (10-4-15)
	if (self->tfstate & PR_TFSTATE_INFECTED && random() <= 0.05)
		self->impulse = PR_TF_MEDIC_HELPME;

	if (self->cutf_items & PR_CUTF_SPY_KIT) handleSpy(); // PZ: Hack to get bots to use the spy kit.

	if (self->job_finished <= time)
	{
		if      (self->job & PR_JOB_JUDOKA) handleJudo();
		else if (self->job & PR_JOB_THIEF)  handleThief();
		else if (self->job & PR_JOB_HACKER) handleHacker();
	}

return; ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OLD CODE BELOW
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef PR_USE_OLD_CODE

	// ##########################################
	// PZ: Roam waypoints, if you don't have a target or enemy to pursue.
	// ##########################################

	if (/*!self.target1 &&*/ self->enemy == world)
		frik_botRoamWaypoints();

	// Put this below the above, so that obstacle avoiding .target1's can supersede the one from above, temporarily.
	frik_botLookForAndHandleObstacles();

	// ##########################################
	// Engage enemies.
	// ##########################################

	// Fight my enemy. Enemy is probably a field QC coders will most likely use a lot
	// for their own needs, since it's unused on a normal player.
	// PZ TODO: I should probably take care of this at some point. It doesn't look like there are serious problems with this right now,
	// but the player entity does appear to use .enemy sometimes for things, in the CuTF code.
	// FIXME
	if (self->enemy != world)
		frik_engageTheEnemy();
	else if (random() < 0.2)
		if (random() < 0.2)
			frik_makeWeaponSelection(-1, PR_FALSE); // PZ NOTE: -1 because you don't have an enemy to have a range to
	else // reload sometimes when you aren't engaged in a fight, so that you will be ready for next fight
	{
		// PZ TODO: I don't like the impulse system. What if an impulse follows and clobbers this impulse? We need a function call instead, trySettingImpulse(),
		//          that first checks to see whether an impulse is already set. If one is already set, don't change it, or maybe allow changing if it's
		//          a high priority impulse.
		if      (self->b_skill == PR_SKILL_NIGHTMARE && random() <= 0.10)   self->impulse = PR_TF_RELOAD;
		else if (self->b_skill == PR_SKILL_HARD      && random() <= 0.05)   self->impulse = PR_TF_RELOAD;
		else if (self->b_skill == PR_SKILL_MEDIUM    && random() <= 0.025)  self->impulse = PR_TF_RELOAD;
		else if (self->b_skill == PR_SKILL_EASY      && random() <= 0.0125) self->impulse = PR_TF_RELOAD;
	}

	// ##########################################
	// PZ: Look in the direction of your .enemy or your .target1.
	// ##########################################

	frik_botAdjustAim();

	// ##########################################
	// PZ: Handle AI flags to do things like open doors and ride platforms.
	// ##########################################

	// .b_aiflags handling
	if (self->b_aiflags)
		frik_handleBotAiFlags();
	//else
	//	frik_botChat(); // don't want chat to screw him up if he's rjing or something

return; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ORIGINAL CODE

//if (!runOnce) {traceon(); runOnce = 1; }
	// #########################################
	// Handle AI Execution Rate
	// #########################################

	// PZ TODO: PUT ANY LIMIT ON EXECUTION RATE FOR THE AI MAIN FUNCTION IN THE BOT MAIN FUNCTION
	// stagger the bot's AI out so they all don't think at the same time, causing game
	// 'spikes' (PZ: seems to only stagger if their skill is less than 2 (Hard)
	if (1 /*self.b_skill < #SKILL_HARD*/) // PZ: make it always stagger
	{
		if (self->ai_time > time)
			return;

		// PZ FIXME: DOESN'T THIS MAKE THEM ALL RUN AT THE SAME TIME ANYWAY? IT JUST DELAYS THEM ALL SOME MORE?
		self->ai_time = time + 0.05;
		if (bot_count > 0)
		{
			if ((time - stagger_think) < (0.1 / bot_count))
				self->ai_time = self->ai_time + 0.1 / (2 * bot_count);
		}
		else
			return;
	}

	// #########################################
	// Check for end of the map.
	// #########################################
	if (self->view_ofs == V({0, 0, 0}))
		frik_botConsiderStartingConversation(PR_TOPIC_MAP_ENDED, 0.2);
	stagger_think = time;

	// #########################################
	// Bot's gameplay AI.
	// #########################################

	// .target1 is like goalentity in normal Quake monster AI.
	// It's the bot's most immediate target _for pursuit_. .enemy is the bot's target for attacking.
	if (route_table == self) // if the route table is currently assigned for use by this bot, then we can look for far away targets
	{
		if (busy_waypoints <= 0) // don't look for a new object if we're currently already finding a route to an object (?)
		{
			if (waypoint_mode < PR_WM_EDITOR) // if we're not in the waypoint editor
				frik_findPriorityObjectToPursue(PR_TRUE); // true means look far and wide; we will use the route table to find a path to any far away object
		}
		self->b_aiflags = 0;
		self->keys = 0;
	}
	else if (self->target1 != world)   // if we have a priority object to pursue
	{
		frik_botPursueTarget1();                // walk directly towards our .target1
		frik_makeNextWaypointInRouteTarget1();  // if we are done pursuing the current waypoint, make the next waypoint in our route our .target1
	}
	else
	{
		if (waypoint_mode < PR_WM_EDITOR) // if we're not in the waypoint editor
		{
			if (self->route_failed)
			{
				if (waypoint_mode == PR_WM_DYNAMIC)         // essentially waypointless mode
					frik_botRoamDynamically();
				else
					self->keys = 0;
				self->route_failed = 0;
			}
			else if (!frik_startRouteFinding())          // if you can't start finding a route
			{
				frik_findPriorityObjectToPursue(PR_FALSE); // look nearby for something to pursue
				self->keys = 0;
			}
		}
		else
		{
			self->b_aiflags = PR_AI_WAIT;
			self->keys = 0;
		}
	}

	// makes the bot aim at its current object of interest (.enemy when fighting; .target1 when just pursuing something)
	frik_botAdjustAim();

	// Fight my enemy. Enemy is probably a field QC coders will most likely use a lot
	// for their own needs, since it's unused on a normal player.
	// PZ TODO: I should probably take care of this at some point. It doesn't look like there are serious problems with this right now,
	// but the player entity does appear to use .enemy sometimes for things, in the CuTF code.
	// FIXME
	if (self->enemy != world)
		frik_engageTheEnemy();
	else if (random() < 0.2)
		if (random() < 0.2)
			frik_makeWeaponSelection(-1, PR_FALSE); // PZ NOTE: -1 because you don't have an enemy to have a range to
	else // reload sometimes when you aren't engaged in a fight, so that you will be ready for next fight
	{
		// PZ TODO: I don't like the impulse system. What if an impulse follows and clobbers this impulse? We need a function call instead, trySettingImpulse(),
		//          that first checks to see whether an impulse is already set. If one is already set, don't change it, or maybe allow changing if it's
		//          a high priority impulse.
		if      (self->b_skill == PR_SKILL_NIGHTMARE && random() <= 0.10)   self->impulse = PR_TF_RELOAD;
		else if (self->b_skill == PR_SKILL_HARD      && random() <= 0.05)   self->impulse = PR_TF_RELOAD;
		else if (self->b_skill == PR_SKILL_MEDIUM    && random() <= 0.025)  self->impulse = PR_TF_RELOAD;
		else if (self->b_skill == PR_SKILL_EASY      && random() <= 0.0125) self->impulse = PR_TF_RELOAD;
	}
	// PZ: Lower-skilled bots are going to look less often. This will affect how quickly they are to notice and react to things,
	// like enemies going around a corner, exposing themselves to the bot.
	float attentiveness;
	if      (self->b_skill == PR_SKILL_NIGHTMARE) attentiveness = 1.0; // he notices every damn thing, immediately
	else if (self->b_skill == PR_SKILL_HARD     ) attentiveness = 0.2; // the rest of these have human-level reaction speeds
	else if (self->b_skill == PR_SKILL_MEDIUM   ) attentiveness = 0.1;
	else if (self->b_skill == PR_SKILL_EASY     ) attentiveness = 0.05;
	if (random() <= attentiveness)
	{
		frik_lookForHazardousObject();
		frik_acquireTargetForAttacking();     // PZ: Put this here.
	}

	// checks to see if bot needs to start going up for air
	if (self->waterlevel > 2)
	{
		if (time > (self->air_finished - 2))
		{
			traceline(self->origin, self->origin + V({0, 0, 6800}), PR_TL_BSP_ONLY, self);
			if (trace_inopen) // if we hit air at some point when drawing the traceline...
			{
				self->keys = PR_KEY_MOVEUP;
				self->PR_BUTTON_JUMP = PR_TRUE; // swim!
				return; // skip AI flags for now - this is life or death
			}
		}
    }

	// .b_aiflags handling
	if (self->b_aiflags)
		frik_handleBotAiFlags();
	else
		frik_botChat(); // don't want chat to screw him up if he's rjing or something

//traceoff();
#endif // (USE_OLD_CODE)
}

// `self` = the current bot
void handleCrusader()
{
	float dividend, remainder, rnum;
	const float FREQUENCY = 32;

	if (!(self->job & PR_JOB_CRUSADER)) return;
	// don't allow to run but so often
	dividend  = bot_frameCounter + self->b_clientno - 1; // This offsets the frame counter by the bot's number.
	remainder = modulo(dividend, FREQUENCY);            // This allows all bots to run this every `FREQUENCY` frames, but with bots evenly spread out over these frames.
	if (remainder != 0) return;

	//if (self.enemy == world) return;              // this is checked already where this is called
	if (self->enemy->classname != "player") return;

	float isTeammateOrDisguisedAsTeammate;                 isTeammateOrDisguisedAsTeammate = PR_FALSE;
	if      (Teammate(self->team_no, self->enemy->team_no))         isTeammateOrDisguisedAsTeammate = PR_TRUE;
	else if (Teammate(self->team_no, self->enemy->undercover_team)) isTeammateOrDisguisedAsTeammate = PR_TRUE;

	if (!(teamplay && isTeammateOrDisguisedAsTeammate)) return;
	if (self->enemy->health < 50) return;   // don't use on dead or players that may die soon
	if (!frik_isVisible_Complex(self->enemy)) return;

	if (self->enemy->aura) return; // don't give aura to someone that has an aura

	// generate a number from 1 to 6
	/*rnum = (random() * 5) + 1;      // took this out because each number doesn't have an equal chance
	rnum = roundFloat(rnum);*/
	rnum = random() * 6;
	rnum = ceil(rnum);
	if (rnum == 0) rnum = 1;

	if      (rnum == 1)
		AddAura(PR_AURA_POWER);
	else if (rnum == 2)
		AddAura(PR_AURA_RESIS);
	else if (rnum == 3)
		AddAura(PR_AURA_HASTE);
	else if (rnum == 4)
		AddAura(PR_AURA_REGEN);
	else if (rnum == 5)
		AddAura(PR_AURA_SPEED);
	else if (rnum == 6)
		CrusaderAddInvisibility();
}

// `self` = the current bot
void handleSpy()
{
	float dividend, remainder, rnum, selection;
	const float FREQUENCY = 32;

	//if (!(self.cutf_items & #CUTF_SPY_KIT)) return;   // checked where this function is called, instead, for speed
	// don't allow to run but so often
	dividend  = bot_frameCounter + self->b_clientno - 1; // This offsets the frame counter by the bot's number.
	remainder = modulo(dividend, FREQUENCY);             // This allows all bots to run this every `FREQUENCY` frames, but with bots evenly spread out over these frames.
	if (remainder != 0) return;

	if (self->is_undercover == 2) return;                // If you are in the process of going undercover, don't try to disguise.

	// [### handle feigning ###]
	if (self->is_feigning)
	{
		// cancel any attempt to jump or to move, made by the bot, this frame
		self->keys = self->PR_BUTTON_JUMP = 0;

		// [if you are not currently visible to an enemy, unfeign]
		// we are going to check all entities that have any sort of agency/perception
		float isVisibleToEnemy, isFamily; entity e;
		e = nextent(world); isVisibleToEnemy = 0;
		while (e != world && !isVisibleToEnemy)
		{
			if ((e->classname == "player" && e->is_connected) || e->flags & PR_FL_MONSTER || e->classname == "building_sentrygun" || e->classname == "building_tesla") // perceptive?
			{
				isFamily = areFamilyMembers(e, self);
				if ((!teamplay && !isFamily) || (teamplay && e->team_no && !Teammate(e->team_no, self->team_no))) // enemy?   // TODO: check undercover_team?
				{
					entity oldSelf;
					oldSelf = self;
					self = e;
					if (frik_isVisible_Complex(oldSelf))
						if (frik_isInFovOfSelf(oldSelf)) isVisibleToEnemy = PR_TRUE;
					self = oldSelf;
				}
			}
			e = nextent(e);
		}
		if (!isVisibleToEnemy) self->impulse = PR_TF_SPY_DIE; // use the "feign" command to unfeign
	}

	// [### handle disguising ###]
	if (!teamplay || number_of_teams < 2) return;                     // No point in disguising in deathmatch.
	if (self->effects & (PR_EF_DIMLIGHT | PR_EF_BRIGHTLIGHT)) return; // Can't disguise while glowing.
	if (self->is_unabletospy == 1)                            return; // Saw this in another area, but not sure when it's set.

	// It seems to me that it would be best if the spy is always disguised, if he can be disguised.
	if (self->undercover_team == 0 && !neo.isModeActive()) // Don't allow changing color in Neo mode.
	{
		// Pick a random team that is not our own. Generate a number from 1 to (`number_of_teams` - 1).
		/*rnum = (random() * (number_of_teams - 2)) + 1;
		rnum = roundFloat(rnum);*/
		rnum = random() * (number_of_teams - 1);
		rnum = ceil(rnum);
		if (rnum == 0) rnum = 1;

		selection = 0;
		while (rnum)
		{
			selection = selection + 1;
			if (selection == self->team_no)              // skip your real team number
				selection = selection + 1;
			rnum = rnum - 1;
		}
		TeamFortress_SpyChangeColor(selection);
	}
	else if (self->undercover_skin == 0)
	{
		// Pick a random skin that is not the spy. Generate a number from 1 to 8 (the number of skins not including the spy).
		rnum = random() * 8;
		rnum = ceil(rnum);
		if (rnum == 0) rnum = 1;

		selection = 0;
		while (rnum)
		{
			selection = selection + 1;
			if (selection == 8)                         // skip your real skin number
				selection = selection + 1;
			rnum = rnum - 1;
		}
		TeamFortress_SpyChangeSkin(selection);
	}
}

// `self` = the current bot
void handleHacker()
{
	float dividend, remainder/*, rnum, selection*/;
	const float FREQUENCY = 32;

	//if (!(self.job & #JOB_HACKER)) return;            // checked where this function is called, instead, for speed
	// don't allow to run but so often
	dividend  = bot_frameCounter + self->b_clientno - 1; // This offsets the frame counter by the bot's number.
	remainder = modulo(dividend, FREQUENCY);            // This allows all bots to run this every `FREQUENCY` frames, but with bots evenly spread out over these frames.
	if (remainder != 0) return;

	if (!self->is_haxxxoring)
	{
		// * Check to see whether we are near a building/machine. I want engineers to hack friendly builds that they come across, and I want spies to hack
		// * enemy builds that they come across, but only while disguised. They need to have a small chance each check that they will start hacking, to make it so that
		// * they don't always choose to hack, and also to make it so that spies don't start hacking as soon as they encounter the device, because that would give them away.
		entity nearMachineInSight; nearMachineInSight = world; // TODO: rename this to an accurate name
		float isFamily, isEnemy; isEnemy = 0;  entity e;
		e = nextent(world);
		while (e != world && nearMachineInSight == world)
		{                                                                          // .real_owner indicates that it's done being built
			if (e->flags & PR_FL_MACHINE && e->classname != "building_sentrygun_base" && e->real_owner != world)  // TODO: allow hacking cyber-augmented players
			{
				// is the machine in sight and within reach of hacking?
				if (frik_isVisible_Complex(e))
				{
					vector v1;  float dist;
					v1 = frik_getRealOriginOfEntity(e);
					dist = vlen((self->origin + V({0, 0, 16})) - v1);
					if (dist <= 500)  // hacking max range is 500 units
					{
						//nearMachineInSight = e;
						// is this machine an enemy's?
						isFamily = areFamilyMembers(e, self);
						if ((!teamplay && !isFamily) || (teamplay && e->team_no && !Teammate(e->team_no, self->team_no))) // enemy?
							isEnemy = PR_TRUE;
						// determine whether to attempt hacking this machine
						if (isEnemy)
						{
							if (self->cutf_items & PR_CUTF_SPY_KIT && self->undercover_team > 0) // if you are disguised
								if (random() <= 0.05)
								{
									if (botAttemptHacking(e, isEnemy)) nearMachineInSight = e;
									else                               nearMachineInSight = world; // continue searching for something to hack, if we couldn't hack this one
								}
						}
						else
						{
							if (self->weapons_carried & PR_WEAP_SPANNER) // are you [likely] an engineer?
								if (random() <= 0.20)
								{
									if (botAttemptHacking(e, isEnemy)) nearMachineInSight = e;
									else                               nearMachineInSight = world; // continue searching for something to hack, if we couldn't hack this one
								}
						}
					}
				}
			}
			e = nextent(e);
		}
	}
}
// `self` = the current bot
// `e`    = entity to hack
// Returns whether it attempted to start a hack.
float botAttemptHacking(entity e, float isEnemy)
{
	float selection; selection = 0;
	float numberOfOptions; numberOfOptions = 0;
	float canSelect1, canSelect2, canSelect3, canSelect4;
	canSelect1 = canSelect2 = canSelect3 = canSelect4 = 0;

	self->demon_two = e; // the target to hack

	// [determine what options are available to hack]
	// harmful hacks
	if (isEnemy)
	{
		if (!(self->demon_two->is_malfunctioning & PR_SCREWUP_ONE))
		{
			canSelect1 = PR_TRUE;  numberOfOptions = numberOfOptions + 1;
		}
		if (!(self->demon_two->is_malfunctioning & PR_SCREWUP_TWO))
		{
			canSelect2 = PR_TRUE;  numberOfOptions = numberOfOptions + 1;
		}
		if (!(self->demon_two->is_malfunctioning & PR_SCREWUP_THREE))
		{
			canSelect3 = PR_TRUE;  numberOfOptions = numberOfOptions + 1;
		}
		if (!(self->demon_two->is_malfunctioning & PR_SCREWUP_FOUR))
		{
			canSelect4 = PR_TRUE;  numberOfOptions = numberOfOptions + 1;
		}
	}
	// helpful hacks
	else
	{
		if (!(self->demon_two->all_active & PR_IMPROVED_ONE))        // capacity, sometimes range
		{
			canSelect1 = PR_TRUE;  numberOfOptions = numberOfOptions + 1;
		}
		if (!(self->demon_two->all_active & PR_IMPROVED_SIX))        // level 3 of security
		{
			canSelect2 = PR_TRUE;  numberOfOptions = numberOfOptions + 1;
		}
		if (!(self->demon_two->all_active & PR_IMPROVED_THREE))      // armor
		{
			canSelect3 = PR_TRUE;  numberOfOptions = numberOfOptions + 1;
		}
		if (self->demon_two->classname != "building_sensor")
		{
			if (self->demon_two->classname != "building_camera")
			if (!(self->demon_two->all_active & PR_IMPROVED_FOUR))   // enhance circuits, spy revealer, increase range
			{
				canSelect4 = PR_TRUE;  numberOfOptions = numberOfOptions + 1;
			}
		}
		else // is a sensor
		{
			if (!(self->demon_two->all_active & PR_IMPROVED_SEVEN))  // thief revealer
			{
				canSelect4 = PR_TRUE;  numberOfOptions = numberOfOptions + 1;
			}
		}
	}
	// [if there is something on this device that we can hack..]
	if (numberOfOptions > 0)
	{
		// [Pick a random option from the available options.]
		// Generate a number from 1 to `numberOfOptions`.
		float numberOfSteps;
		numberOfSteps = random() * numberOfOptions;
		numberOfSteps = ceil(numberOfSteps);
		if (numberOfSteps == 0) numberOfSteps = 1;

		// yay for not having arrays in QuakeC...
		while (numberOfSteps > 0)
		{
			selection = selection + 1;
			if (selection == 1 && !canSelect1) selection = selection + 1;
			if (selection == 2 && !canSelect2) selection = selection + 1;
			if (selection == 3 && !canSelect3) selection = selection + 1;
			if (selection == 4 && !canSelect4) selection = 1;   // only happens if there is a bug
			numberOfSteps = numberOfSteps - 1;
		}
	}
	// [start hacking `e`.]
	if (selection > 0)
	{
/*local string tempstr;
tempstr = ftos(selection);
bprint(2, "attempting to hack on selection ", tempstr, "\n");*/
		InitiateInterface(selection);  // TODO FIXME: for some reason, bots won't hack a sentry gun once it has been picked up by ZGG
		return PR_TRUE;
	}
	else
	{
		self->demon_two = world;
		return PR_FALSE;
	}
}

// `self` = the current bot
void handleThief()
{
	float /*dividend, remainder,*/ rnum/*, selection*/;
	//local float FREQUENCY = 32;

	//if (!(self.job & #JOB_THIEF)) return;             // checked where this function is called, instead, for speed
	// don't allow to run but so often

	// PZ: If this doesn't run every frame, then we can't stop bot from moving, below. This function does so little, anyway.
	/*dividend  = bot_frameCounter + self.b_clientno - 1; // This offsets the frame counter by the bot's number.
	remainder = modulo(dividend, FREQUENCY);            // This allows all bots to run this every `FREQUENCY` frames, but with bots evenly spread out over these frames.
	if (remainder != 0) return;*/

	// self.job & #JOB_ACTIVE --- self.job & #JOB_FULL_HIDE
	// [determine whether to start hiding]
	if (!(self->job & PR_JOB_ACTIVE))
	{
		rnum = random();
		if (isVectorBitFlagSet(PR_ROLE_CAPPER, V({0, 0, 0}), self->b_botRole)) // bots focused on capping will be more likely to hide
		{
			if (rnum <= 0.002) self->impulse = PR_TF_SKILL;
		}
		else
		{
			if (rnum <= 0.001) self->impulse = PR_TF_SKILL;
		}
	}
	// [determine what to do while hiding]
	else
	{
		// [if an enemy player is watching, stand still to fully hide]
		// I don't think sentries (including tesla coils) can see moving thieves. Not sure about monsters.
		if (self->b_botInEnemyPlayerFOV)
			self->keys = self->PR_BUTTON_JUMP = 0; // cancel any attempt to jump or to move, made by the bot, this frame
	}
}

// `self` = the current bot
void handleJudo()
{
	float dividend, remainder/*, rnum, selection*/;
	const float FREQUENCY = 16;

	//if (!(self.job & #JOB_JUDOKA)) return;            // checked where this function is called, instead, for speed
	// don't allow to run but so often
	dividend  = bot_frameCounter + self->b_clientno - 1; // This offsets the frame counter by the bot's number.
	remainder = modulo(dividend, FREQUENCY);            // This allows all bots to run this every `FREQUENCY` frames, with bots spread out on frames.
	if (remainder != 0) return;

	// TODO: When I write the code in C, calculate distance to enemy ONE TIME at the start of each frame. Probably should do the same for target1.
	self->impulse = PR_TF_SKILL;
}

// `self` = the current bot
void isBotInEnemyFieldOfView()
{
	float dividend, remainder/*, rnum, selection*/;
	const float FREQUENCY = 32;

	// don't allow to run but so often
	dividend  = bot_frameCounter + self->b_clientno - 1; // This offsets the frame counter by the bot's number.
	remainder = modulo(dividend, FREQUENCY);            // This allows all bots to run this every `FREQUENCY` frames, with bots spread out on frames.
	if (remainder != 0) return;

	// we are going to check all entities that have any sort of agency/perception
	float isFamily; entity e;
	e = nextent(world);  self->b_botInEnemyFOV = self->b_botInEnemyPlayerFOV = self->b_botInEnemyMachineFOV = PR_FALSE;
	while (e != world && (!self->b_botInEnemyFOV || !self->b_botInEnemyPlayerFOV || !self->b_botInEnemyMachineFOV))
	{
		if ((e->classname == "player" && e->is_connected) || e->flags & PR_FL_MONSTER || e->classname == "building_sentrygun" || e->classname == "building_tesla") // perceptive?
		{
			isFamily = areFamilyMembers(e, self);
			if ((!teamplay && !isFamily) || (teamplay && e->team_no && !Teammate(e->team_no, self->team_no))) // enemy?   // TODO: check undercover_team?
			{
				entity oldSelf;
				oldSelf = self;
				self = e;
				if (frik_isVisible_Complex(oldSelf))
				{
					if (frik_isInFovOfSelf(oldSelf))
					{
						oldSelf->b_botInEnemyFOV = PR_TRUE;
						if (e->classname == "player")      oldSelf->b_botInEnemyPlayerFOV  = PR_TRUE;
						if (e->flags & PR_FL_MACHINE)      oldSelf->b_botInEnemyMachineFOV = PR_TRUE;
					}
				}
				self = oldSelf;
			}
		}
		e = nextent(e);
	}
}

// `self` = undefined, initially (don't use it here)
//
void callback_botTakesDamage(entity bot, float damage, entity damagingObject, entity responsibleOne, float T_flags, float T_AttackType)
{
	entity oldSelf, oldSelf2;   float rnum, messages, spyMessages;   string speech;

	oldSelf = self;
	self = bot;

	// damaged by a teammate
	if (bot != responsibleOne && responsibleOne->ishuman && Teammate(bot->team_no, responsibleOne->team_no))
	{
		// determine whether to complain to teammate for hurting you
		rnum = random();
		if (damage >= 5 && rnum < damage / 100) // damage essentially is equivalent to the percentage chance that you will complain
		{
			/* !!!!!!!!!! */ messages = 9;  spyMessages = 4; /* !!!!!!!!!! */
			if (bot->undercover_team != 0) messages = messages + spyMessages;
			// Generate a number from 1 to `messages`.
			rnum = random() * messages;
			rnum = ceil(rnum);
			if (rnum == 0) rnum = 1;

			// messages
			if      (rnum == 1)  speech = "same team";
			else if (rnum == 2)  speech = "same team, asshat";
			else if (rnum == 3)  speech = "same team, dude";
			else if (rnum == 4)  speech = "jesus h christ! i'm on your side!";
			else if (rnum == 5)  speech = "wtf, man?";
			else if (rnum == 6)  speech = "what you shooting at?";
			else if (rnum == 7)  speech = "team killer?";
			else if (rnum == 8)  speech = "keep it up, man.. we'll see who can get cursed first";
			else if (rnum == 9)  speech = "this guy is tking me";
			// spy messages
			else if (rnum == 10) speech = "i'm a spy. don't shoot";
			else if (rnum == 11) speech = "I'M A SPY";
			else if (rnum == 12) speech = "how can you not tell that i'm a spy?";
			else if (rnum == 13) speech = "i was doing a good job acting like someone on the other team, huh?";
			// add exclamation marks based on damage, and then the \n character
			if      (damage >= 100) speech = strcat(speech, "!!!!!\n");
			else if (damage >= 80)  speech = strcat(speech, "!!!!\n");
			else if (damage >= 60)  speech = strcat(speech, "!!!\n");
			else if (damage >= 40)  speech = strcat(speech, "!!\n");
			else if (damage >= 20)  speech = strcat(speech, "!\n");
			else                    speech = strcat(speech, "\n");
			frik_botSayTeam(speech, -1);
		}
	}
	// damaged by some entity
	else if (bot != responsibleOne && /*responsibleOne.classname == "player"*/ responsibleOne != world)
	{
		// handle feigning
		if (bot->cutf_items & PR_CUTF_SPY_KIT && !bot->is_feigning && random() <= 0.20)
		{
			// [check whether to make the death sound (if someone can see you)]
			// we are just going to check humans for right now
			float isVisibleToEnemyHuman, counter; entity e;
			e = nextent(world); counter = 1; isVisibleToEnemyHuman = 0;
			while (counter <= max_clients && !isVisibleToEnemyHuman)
			{
				if (e->ishuman && e->is_connected && !Teammate(e->team_no, bot->team_no)) // TODO: check undercover_team?
				{
					oldSelf2 = self;
					self = e;
					if (frik_isVisible_Complex(bot))
						if (frik_isInFovOfSelf(bot)) isVisibleToEnemyHuman = PR_TRUE;
					self = oldSelf2;
				}
				e = nextent(e); counter = counter + 1;
			}
			if (isVisibleToEnemyHuman) bot->impulse = PR_TF_SPY_DIE;  // feign with vocal sound
			else                       bot->impulse = PR_TF_SPY_DIE2; // feign silently
		}
	}

	// CLEANUP
	self = oldSelf;
}

// PZ: Remainder Division
//     Returns `a` mod `b`.  (WARNING: I haven't yet thought about how it handles negative values.)
//     Don't let `b` == 0.
float modulo(float a, float b)
{
	float remainder;

	remainder = a - (b * floor(a / b));
	return remainder;
}

} // END namespace Progs
