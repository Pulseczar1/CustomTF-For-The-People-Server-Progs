/***********************************************
*                                              *
*             FrikBot Waypoints                *
*         "The better than roaming AI"         *
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
#include "frikbot/bot_way.h"
#include "frikbot/bot_qw.h"
#include "frikbot/bot_misc.h"
#include "frikbot/bot_ai.h"
#include "frikbot/bot_move.h"
#include "debug.h"

namespace Progs {

// =======================
// Waypoint Linking code
// =======================

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_doesWaypointLinkToWaypoint   (PZ: was called "CheckLinked")

Checks whether `waypoint1` has a link directed to `waypoint2`.
Returns #TRUE if it does. In the case of a teleporter
link, it returns 2.

`self` = current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_doesWaypointLinkToWaypoint(entity waypoint1, entity waypoint2)
{
	if ((waypoint1 == waypoint2) || (waypoint2 == world) || (waypoint1 == world))
		return PR_FALSE;

	else if (waypoint1->target1 == waypoint2)
	{
		if (waypoint1->b_aiflags &PR_AI_TELELINK_1)
			return 2;
		else return PR_TRUE;
	}
	else if (waypoint1->target2 == waypoint2)
	{
		if (waypoint1->b_aiflags & PR_AI_TELELINK_2)
			return 2;
		else return PR_TRUE;
	}
	else if (waypoint1->target3 == waypoint2)
	{
		if (waypoint1->b_aiflags & PR_AI_TELELINK_3)
			return 2;
		else return PR_TRUE;
	}
	else if (waypoint1->target4 == waypoint2)
	{
		if (waypoint1->b_aiflags & PR_AI_TELELINK_4)
			return 2;
		else return PR_TRUE;
	}

	else return PR_FALSE;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_linkWaypointToWaypoint  (PZ: was called "LinkWays")

Creates a link from `waypoint1` to `waypoint2`.
If `waypoint1` already has 4 links, function fails,
and returns #FALSE. Returns #TRUE on success.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_linkWaypointToWaypoint(entity waypoint1, entity waypoint2)
{
	if ((waypoint1 == waypoint2) || (waypoint2 == world) || (waypoint1 == world))
		return PR_FALSE;
	else if (frik_doesWaypointLinkToWaypoint(waypoint1, waypoint2))
		return PR_FALSE; // already linked

	if (waypoint1->target1 == world)
	{
		waypoint1->target1 = waypoint2;
		return PR_TRUE;
	}
	else if (waypoint1->target2 == world)
	{
		waypoint1->target2 = waypoint2;
		return PR_TRUE;
	}
	else if (waypoint1->target3 == world)
	{
		waypoint1->target3 = waypoint2;
		return PR_TRUE;
	}
	else if (waypoint1->target4 == world)
	{
		waypoint1->target4 = waypoint2;
		return PR_TRUE;
	}

	else return PR_FALSE;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_telelinkWaypointToWaypoint  (PZ: was called "TeleLinkWays")

Creates a teleporter link from `waypoint1` to `waypoint2`.
If `waypoint1` already has 4 links, function fails,
and returns #FALSE. Returns #TRUE on success.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_telelinkWaypointToWaypoint(entity waypoint1, entity waypoint2)
{
	if ((waypoint1 == waypoint2) || (waypoint2 == world) || (waypoint1 == world))
		return PR_FALSE;
	else if (frik_doesWaypointLinkToWaypoint(waypoint1, waypoint2))
		return PR_FALSE; // already linked!!!

	if (waypoint1->target1 == world)
	{
		waypoint1->target1 = waypoint2;
		waypoint1->b_aiflags = waypoint1->b_aiflags |PR_AI_TELELINK_1;
		return PR_TRUE;
	}
	else if (waypoint1->target2 == world)
	{
		waypoint1->target2 = waypoint2;
		waypoint1->b_aiflags = waypoint1->b_aiflags | PR_AI_TELELINK_2;
		return PR_TRUE;
	}
	else if (waypoint1->target3 == world)
	{
		waypoint1->target3 = waypoint2;
		waypoint1->b_aiflags = waypoint1->b_aiflags | PR_AI_TELELINK_3;
            return PR_TRUE;
	}
	else if (waypoint1->target4 == world)
	{
		waypoint1->target4 = waypoint2;
		waypoint1->b_aiflags = waypoint1->b_aiflags | PR_AI_TELELINK_4;
		return PR_TRUE;
	}

	else return PR_FALSE;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_unlinkWaypointToWaypoint  (PZ: was called "UnlinkWays")

Removes the link from `waypoint1` to `waypoint2`.
Will remove any telelink, also.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_unlinkWaypointToWaypoint(entity waypoint1, entity waypoint2)
{
	if ((waypoint1 == waypoint2) || (waypoint2 == world) || (waypoint1 == world))
		return;
	else if (!frik_doesWaypointLinkToWaypoint(waypoint1, waypoint2))
		return;

	if (waypoint1->target1 == waypoint2)
	{
		waypoint1->b_aiflags = waypoint1->b_aiflags - (waypoint1->b_aiflags &PR_AI_TELELINK_1);
		waypoint1->target1 = world;
	}
	if (waypoint1->target2 == waypoint2)
	{
		waypoint1->b_aiflags = waypoint1->b_aiflags - (waypoint1->b_aiflags & PR_AI_TELELINK_2);
		waypoint1->target2 = world;
	}
	if (waypoint1->target3 == waypoint2)
	{
		waypoint1->b_aiflags = waypoint1->b_aiflags - (waypoint1->b_aiflags & PR_AI_TELELINK_3);
		waypoint1->target3 = world;
	}
	if (waypoint1->target4 == waypoint2)
	{
		waypoint1->b_aiflags = waypoint1->b_aiflags - (waypoint1->b_aiflags & PR_AI_TELELINK_4);
		waypoint1->target4 = world;
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_findWaypointNearestAndVisibleToSelf    (PZ: was called "FindWaypoint")

(This is used quite a bit by many different functions.
Big lag causer.)

Finds the closest waypoint to `self`, that frik_isVisible_Simple()
to `self`.

PZ FIXME:
`startingWaypoint` doesn't seem to do anything.

`self` = usually the current bot, but looks like it can be other things

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
entity frik_findWaypointNearestAndVisibleToSelf(entity startingWaypoint)
{
	entity waypoint;
	vector originOfSelf;
	float dist, tdst;
	entity best;

	if (self->classname != "spec") // PZ
		originOfSelf = frik_getRealOriginOfEntity(self);
	else
		originOfSelf = self->origin;

	// PZ NOTE: WTF IS THIS DOING? IT'S GOING TO FIND THE CLOSEST WAYPOINT TO SELF, REGARDLESS, RIGHT?
	// Looks like it's just eliminating some checks after the "tdst < dist".
	// PZ: took this out
	/*if (startingWaypoint != world)
	{
		dist = vlen(startingWaypoint.origin - originOfSelf);
		best = startingWaypoint;
	}*/
	// PZ NOTE: END NOTE
	//else
	//{
		dist = 9999999;  // PZ: was 100000
		best = world;
	//}

	waypoint = way_head; // start at the beginning of the waypoint list
	while (waypoint != world)
	{
		// real players cut through ignore types
		if (dist <= 20) // 20 is good enough, stop here
			return best;
		//if (!(waypoint.b_aiflags & #AI_IGNORE_TYPES) || self.ishuman) // PZ: took this out
		//{
			tdst = vlen(waypoint->origin - originOfSelf);
			if (tdst < dist)
			{
				if (frik_isVisible_Simple(waypoint)) // PZ NOTE: may want to make this use frik_isVisible_Waypoint() some day
				{
					dist = tdst;
					best = waypoint;
				}
			}
		//}
		waypoint = waypoint->_next;
	}
	return best;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_findWaypointToRoamTo

PZ: I wrote this function, to allow bots to simply roam their waypoints.
It returns the next waypoint to roam to.

`self` = the current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
entity frik_findWaypointToRoamTo()
{
string strtemp;
	vector originOfSelf;
	entity linkedList = world, ptr = world, e = world;  // PZ: initialized linkedList to world to get rid of C++ warning
	float nodeCount = 0;
	vector realOriginOfEntity;

	const float bool_Debug = 0;

if (bool_Debug) bprint(2, "@@@@ BEGIN frik_findWaypointToRoamTo()\n");

	originOfSelf = frik_getRealOriginOfEntity(self);

	// if we aren't currently on a waypoint, find closest one to move to
	if (self->current_way == world)
		return frik_findWaypointNearestAndVisibleToSelf(world);

/*if (self.current_way.target1 != world && self.current_way.target1 != self.last_way)
return self.current_way.target1;
if (self.current_way.target2 != world && self.current_way.target2 != self.last_way)
return self.current_way.target2;
if (self.current_way.target3 != world && self.current_way.target3 != self.last_way)
return self.current_way.target3;
if (self.current_way.target4 != world && self.current_way.target4 != self.last_way)
return self.current_way.target4;
return self.last_way;*/

	// If we are currently on a waypoint, check each of its links to see which one we should follow.
	// The further the waypoint is off your facing direction, the less of a chance you will pick it.
	// We have to use frik_getRealOriginOfEntity() here because teleporter entities are brush entities, and do not have origin
	// values. They do have .absmin and .absmax though.
	if (self->current_way->target1 != world && self->current_way->target1 != self->PR_b_bot_previousTarget1) // LINK 1
	{
		ptr = LinkedList_addNodeToEndOf(linkedList, "frik_findWaypointToRoamTo()");
		if (linkedList == world) linkedList = ptr;
		nodeCount = nodeCount + 1;
		ptr->owner = self->current_way->target1;
		// If this is a telelink, we need to look at the position of the teleporter, not the destination waypoint.
		if (self->current_way->b_aiflags &PR_AI_TELELINK_1)
			e = frik_findNearestEntityToSelfWithThisClassname("trigger_teleport");
		else
			e = self->current_way->target1;
		realOriginOfEntity = frik_getRealOriginOfEntity(e);
		ptr->angles = vectoangles(realOriginOfEntity - originOfSelf);  // angle relative to the world
		ptr->angles[Y] = frik_angleSubtraction(ptr->angles[Y], self->v_angle[Y]);        // angle relative to bot's line of sight
		ptr->angles[Y] = fabs(ptr->angles[Y]);                                         // don't care which direction (absolute value)
if (bool_Debug) {
strtemp = ftos(ptr->angles[Y]);
bprint(2, "waypoint 1's angle: " , strtemp, "\n");}
	}
	if (self->current_way->target2 != world && self->current_way->target2 != self->PR_b_bot_previousTarget1) // LINK 2
	{
		ptr = LinkedList_addNodeToEndOf(linkedList, "frik_findWaypointToRoamTo()");
		if (linkedList == world) linkedList = ptr;
		nodeCount = nodeCount + 1;
		ptr->owner = self->current_way->target2;
		// If this is a telelink, we need to look at the position of the teleporter, not the destination waypoint.
		if (self->current_way->b_aiflags & PR_AI_TELELINK_2)
			e = frik_findNearestEntityToSelfWithThisClassname("trigger_teleport");
		else
			e = self->current_way->target2;
		realOriginOfEntity = frik_getRealOriginOfEntity(e);
		ptr->angles = vectoangles(realOriginOfEntity - originOfSelf); // angle relative to the world
		ptr->angles[Y] = frik_angleSubtraction(ptr->angles[Y], self->v_angle[Y]);       // angle relative to bot's line of sight
		ptr->angles[Y] = fabs(ptr->angles[Y]);                                        // don't care which direction (absolute value)
if (bool_Debug) {
strtemp = ftos(ptr->angles[Y]);
bprint(2, "waypoint 2's angle: " , strtemp, "\n");}
	}
	if (self->current_way->target3 != world && self->current_way->target3 != self->PR_b_bot_previousTarget1) // LINK 3
	{
		ptr = LinkedList_addNodeToEndOf(linkedList, "frik_findWaypointToRoamTo()");
		if (linkedList == world) linkedList = ptr;
		nodeCount = nodeCount + 1;
		ptr->owner = self->current_way->target3;
		// If this is a telelink, we need to look at the position of the teleporter, not the destination waypoint.
		if (self->current_way->b_aiflags & PR_AI_TELELINK_3)
			e = frik_findNearestEntityToSelfWithThisClassname("trigger_teleport");
		else
			e = self->current_way->target3;
		realOriginOfEntity = frik_getRealOriginOfEntity(e);
		ptr->angles = vectoangles(realOriginOfEntity - originOfSelf); // angle relative to the world
		ptr->angles[Y] = frik_angleSubtraction(ptr->angles[Y], self->v_angle[Y]);       // angle relative to bot's line of sight
		ptr->angles[Y] = fabs(ptr->angles[Y]);                                        // don't care which direction (absolute value)
if (bool_Debug) {
strtemp = ftos(ptr->angles[Y]);
bprint(2, "waypoint 3's angle: " , strtemp, "\n");}
	}
	if (self->current_way->target4 != world && self->current_way->target4 != self->PR_b_bot_previousTarget1) // LINK 4
	{
		ptr = LinkedList_addNodeToEndOf(linkedList, "frik_findWaypointToRoamTo()");
		if (linkedList == world) linkedList = ptr;
		nodeCount = nodeCount + 1;
		ptr->owner = self->current_way->target4;
		// If this is a telelink, we need to look at the position of the teleporter, not the destination waypoint.
		if (self->current_way->b_aiflags & PR_AI_TELELINK_4)
			e = frik_findNearestEntityToSelfWithThisClassname("trigger_teleport");
		else
			e = self->current_way->target4;
		realOriginOfEntity = frik_getRealOriginOfEntity(e);
		ptr->angles = vectoangles(realOriginOfEntity - originOfSelf); // angle relative to the world
		ptr->angles[Y] = frik_angleSubtraction(ptr->angles[Y], self->v_angle[Y]);       // angle relative to bot's line of sight
		ptr->angles[Y] = fabs(ptr->angles[Y]);                                        // don't care which direction (absolute value)
if (bool_Debug) {
strtemp = ftos(ptr->angles[Y]);
bprint(2, "waypoint 4's angle: " , strtemp, "\n");}
	}

	// sort the list of waypoints in order of closest off line-of-sight to furthest off (bubble sort)
	float doLoop = PR_TRUE;
//bprint(2, "STARTING LOOP 1\n");
	while (doLoop)
	{
		doLoop = PR_FALSE;
		ptr = linkedList;
//bprint(2, "STARTING LOOP 2\n");
		while (ptr != world)
		{
			if (ptr->_next != world && ptr->_next->angles[Y] < ptr->angles[Y])  // y is yaw, x is pitch
			{
				linkedList = LinkedList_swapNodes(linkedList, ptr, ptr->_next);
				doLoop = PR_TRUE;
			}
			else ptr = ptr->_next;
		}
	}

// print list of waypoints
if (bool_Debug) {
ptr = linkedList;
bprint(2, "WAYPOINT LIST:\n");
while (ptr != world)
{
	strtemp = ftos(ptr->angles[Y]);
	bprint(2, "waypoint's angles_y: ", strtemp, "\n");
	ptr = ptr->_next;
}
}
	// Pick a waypoint. The further it is off your line of sight, the less likely you are to pick it.
	// It really depends directly on how far off degree-wise each waypoint option is. It's not just based on sorted order.
	// Pathways that are within a certain amount (currently 95 deg) get equal chance to be selected.
	float listDescent, rnum, chance, chance1, chance2, chance3, num, totalChanceOverCutoff = 0, numberWithinCutOffLimit;
	entity firstMoreThanCutoff = world;
	chance = chance1 = chance2 = chance3 = rnum = 0;
	rnum = random();          //      best          2nd best         3rd best       4th best gets whatever percentage is left over
	//#### old way
#ifdef PR_FDSLAJDFILFDFDFDFDF
	if      (nodeCount == 4)   { chance1 = 0.85; chance2 = 0.95; chance3 = 0.98; }
	else if (nodeCount == 3)   { chance1 = 0.88; chance2 = 0.98; chance3 = 1.00; }
	else if (nodeCount == 2)   { chance1 = 0.98; chance2 = 1.00; chance3 = 1.00; }
	else /* (nodeCount == 1)*/ { chance1 = 1.00; chance2 = 1.00; chance3 = 1.00; }
	if      (rnum <= chance1) listDescent = 0; // choose waypoint closest to our line of sight 85% of the time, when we have 4 choices
	else if (rnum <= chance2) listDescent = 1; // choose waypoint 2nd closest to our line of sight 10% of the time, when we have 4 choices
	else if (rnum <= chance3) listDescent = 2; // choose waypoint 3rd closest to our line of sight 3% of the time, when we have 4 choices
	else /* (rnum <= 1.00) */ listDescent = 3; // choose waypoint 4th closest to our line of sight 2% of the time, when we have 4 choices
#endif

	//#### new way
	// Look at the ones that are more than 95 deg off. Determine a chance for them based on how far off they are.
	// Split the remaining total percentage evenly among the ones that are not more than 95 deg off line of sight.
	const float cutoffAngle = 95;
	// find the first one that is more than the cutoff angle
	ptr = linkedList; num = 1;
	while (ptr != world && ptr->angles[Y] <= cutoffAngle) { num = num+1; ptr = ptr->_next; }
	numberWithinCutOffLimit = num - 1;
	// for each further than the cutoff, determine their chance, based on how far off they are from line of sight
	while (ptr != world)
	{
		if (firstMoreThanCutoff == world) firstMoreThanCutoff = ptr;
		// I looked to see whether QCCX (compiler being used) can do simple arrays, but it could only seem to do some sort of weird array.
		// FTEQCC apparently does arrays very well, but I'm hesitent to try FTEQCC, after having issues with bugs in FTEQWSV, and perhaps
		// the compiler, before.
		chance = (1 / nodeCount) * ((180 - ptr->angles[Y]) / (180 - cutoffAngle));
		if (chance < 0.01) chance = 0.01; // set a minimum chance of 1% for all waypoint/pathway options
		totalChanceOverCutoff = totalChanceOverCutoff + chance;
		if      (num == 1) chance1 = chance;
		else if (num == 2) chance2 = chance;
		else if (num == 3) chance3 = chance;
		//else if (num == 4) chance4 = chance;  // not really needed because we just use the remainder
		num = num+1; ptr = ptr->_next;
	}
	// if there were none within the cutoff, allow the first one to take the rest of the 100%
	if (firstMoreThanCutoff == linkedList) chance1 = chance1 + (1.0 - totalChanceOverCutoff);
	//if (!firstMoreThanCutoff) firstMoreThanCutoff = linkedList; // in case there were none that exceeded the cutoff angle
	// split the remaining total percentage evenly among the ones that are not more than the cutoff angle off line of sight
	ptr = linkedList; num = 1;
	while (ptr != world && ptr != firstMoreThanCutoff && numberWithinCutOffLimit > 0)
	{
		chance = (1.0 - totalChanceOverCutoff) / numberWithinCutOffLimit;
		if      (num == 1) chance1 = chance;
		else if (num == 2) chance2 = chance;
		else if (num == 3) chance3 = chance;
		//else if (num == 4) chance4 = chance;  // not really needed because we just use the remainder
		num = num+1; ptr = ptr->_next;
	}
	// randomly pick a waypoint based on the computed chances for each waypoint
	if      (rnum <= chance1)                     listDescent = 0;
	else if (rnum <= chance1 + chance2)           listDescent = 1;
	else if (rnum <= chance1 + chance2 + chance3) listDescent = 2;
	else /* (rnum <= 1.00) // the remainder */    listDescent = 3;

if (bool_Debug) {
bprint(2, "CHANCE LIST:\n");
strtemp = ftos(chance1);
bprint(2, "chance1 = ", strtemp, "\n");
strtemp = ftos(chance2);
bprint(2, "chance2 = ", strtemp, "\n");
strtemp = ftos(chance3);
bprint(2, "chance3 = ", strtemp, "\n");
//strtemp = ftos(chance4);
//bprint(2, "chance4 = ", strtemp, "\n");
strtemp = ftos(listDescent + 1);
bprint(2, "Randomly chose waypoint ", strtemp, "\n");
}

	ptr = linkedList;
//bprint(2, "STARTING LOOP 3\n");
	while (ptr->_next != world && listDescent)
	{
		ptr = ptr->_next;
		listDescent = listDescent - 1;
	}
	// ptr is about to be deleted; we want its owner (the actual waypoint, not the node)
if (bool_Debug) {
strtemp = ftos(ptr->angles[Y]);
bprint(2, "CHOSE TO SEEK WAYPOINT WITH HEADING OF: " , strtemp, "\n");}
	ptr = ptr->owner;

	// give back memory that we used for the LinkedList
	LinkedList_deleteList(linkedList);

	// [return waypoint to roam to]
	// If we didn't find one, because maybe there are no good options, return any in sight.
	if (ptr == world)
		return frik_findWaypointNearestAndVisibleToSelf(world);

	return ptr;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

Waypoint Spawning Code

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_createBlankWaypointHere   (PZ: was called "make_waypoint")

Creates a "blank" waypoint at origin `position`, and
returns it.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
entity frik_createBlankWaypointHere(const vector& position)
{
	entity waypoint;
	waypoint = spawnServerSide();          // PZ: this was calling spawn(), but clients don't need to know about waypoint entities
	waypoint->classname = "waypoint";
	waypoint->solid = PR_SOLID_TRIGGER;
	//waypoint.movetype = #MOVETYPE_NONE;
	//waypoint->movetype = PR_MOVETYPE_NOCLIP; // PZ: in QuakeWorld, entities with MOVETYPE_NONE can't be made visible
	// PZ (Mar 2, 2023): However, NOCLIP can move, according to SV_Physics_Noclip(), and I have a problem with my waypoints randomly
	// getting a velocity and running off the map. So, because I have `waypoint_visual_marker` entities, that use MOVETYPE_NOCLIP,
	// to make waypoints visible in the waypoint editor, I can make real waypoints MOVETYPE_NONE.
	waypoint->movetype = PR_MOVETYPE_NONE;
	waypoint->velocity = V({0, 0, 0});     // PZ: I will also make sure the waypoint's velocity is 0, on creation.
	//waypoint.items = -1;                 // PZ: What is this? Am I still using it? Pretty sure I'm not. Taking it out.
	setorigin(waypoint, position);

	setsize(waypoint, PR_VEC_HULL_MIN, PR_VEC_HULL_MAX);
	waypoints = waypoints + 1;
	if (way_head == world)
	{
		way_head = waypoint;
		way_tail = waypoint;
	}
	else
	{
		way_tail->_next = waypoint;
		waypoint->_prev = way_tail;
		way_tail = waypoint;
	}

	waypoint->count = waypoints;
	if (waypoint_mode > PR_WM_LOADED) // editor modes
		//setmodel(waypoint, "progs/s_bubble.spr");
		//frik_createWaypointVisualMarker(waypoint);   // PZ
		frik_updateWaypointVisualMarkers();            // PZ
	return waypoint;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_createWaypointVisualMarker

PZ: Creates a visual waypoint marker for `waypoint`.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_createWaypointVisualMarker(entity waypoint)
{
	if (waypoint->owner != world) return; // `waypoint` already has a visual marker

	waypoint->owner = spawn();
	waypoint->owner->classname = "waypoint_visual_marker";
	waypoint->owner->movetype = PR_MOVETYPE_NOCLIP; // PZ: I note, above, that, in QuakeWorld, you can't use MOVETYPE_NONE on entities you want to be visible.
	waypoint->owner->owner = waypoint;
	// Is `waypoint` the selected waypoint for any human player using the waypoint editor?
	entity player; float counter;
	player = nextent(world); counter = 1;
	while (counter <= max_clients)
	{
		if (player->ishuman && player->current_way == waypoint)
			counter = PR_LARGEST_INTEGER; // marks that we found a player that has `waypoint` selected, and exits the loop
		else
			{ player = nextent(player); counter = counter + 1; }
	}
	if (counter == PR_LARGEST_INTEGER) setmodel(waypoint->owner, "progs/s_light.spr");  // big globe (selected)
	else                               setmodel(waypoint->owner, "progs/s_bubble.spr"); // bubble (not selected)
	setorigin(waypoint->owner, waypoint->origin);
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_deleteWaypointVisualMarker

PZ: Deletes the visual waypoint marker for `waypoint`.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_deleteWaypointVisualMarker(entity waypoint)
{
	if (waypoint->owner != world)
	{
		dremove(waypoint->owner);
		waypoint->owner = world;
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_updateWaypointVisualMarkers

PZ: This goes through the list of waypoints and makes sure all waypoints that are in the players'
sight have visual representations (waypoint visual markers), and that all waypoints that aren't in
the players' sight do not have visual representations. Only players using the waypoint editor are
considered. Waypoint copies are made to flash on and off, instead.

This addresses the issue of invisible waypoints and waypoint links that occurs when there are too
many visible entities in one area for the network protocol to handle.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
/*static*/ float frik_updateWaypointVisualMarkers_lastTimeToggled;
/*static*/ float frik_updateWaypointVisualMarkers_copiesVisible;
void frik_updateWaypointVisualMarkers()
{
	entity wp, player, oldSelf;
	float counter, inSightOfAUser, selectedByAUser;

	if (waypoint_mode != PR_WM_EDITOR) return;

	// for flashing waypoint copies, determine whether it's time to flash on or off
	if (time > frik_updateWaypointVisualMarkers_lastTimeToggled + 0.25)
	{
		frik_updateWaypointVisualMarkers_copiesVisible = !frik_updateWaypointVisualMarkers_copiesVisible;
		frik_updateWaypointVisualMarkers_lastTimeToggled = time;
	}

	wp = way_head;
	while (wp != world)
	{
		// is this waypoint in the sight of a waypoint editor user?
		inSightOfAUser = selectedByAUser = PR_FALSE;
		player = nextent(world); counter = 1;
		while (counter <= max_clients && (!inSightOfAUser || !selectedByAUser))
		{
			if (player->ishuman && player->admin_flag)
			{
				oldSelf = self;
				self = player;
				if (frik_isVisible_Simple(wp))
				{
					if (frik_isInFovOfSelf(wp))  // NOTE: make sure you never get more than one return value per statement, in QuakeC
						inSightOfAUser = PR_TRUE;
				}
				self = oldSelf;
				if (player->current_way == wp)
					selectedByAUser = PR_TRUE;
			}
			player = nextent(player); counter = counter + 1;
		}
		// if so, make sure it has a visual marker (waypoint copies are to flash on and off)
		if (inSightOfAUser && (wp->classname != "waypoint_copy" || frik_updateWaypointVisualMarkers_copiesVisible))
		{
			if (wp->owner == world) frik_createWaypointVisualMarker(wp);
			if (selectedByAUser) setmodel(wp->owner, "progs/s_light.spr");  // big globe (selected)
			else                 setmodel(wp->owner, "progs/s_bubble.spr"); // bubble (not selected)
		}
		// otherwise, make sure it doesn't have a visual marker
		else
			if (wp->owner != world) frik_deleteWaypointVisualMarker(wp);

		wp = wp->_next;
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_dynamicWaypointing   (PZ: was called "DynamicWaypoint")

Dynamic Waypoint spawning and linking. Not
very good all things considered.

PZ NOTE: probably won't ever use this
It seems to only run the first little bit. It appears either dynamic waypointing must be running
or we must be using some sort of dynamic waypointing in the waypoint editor in order for this to run
very far.

PZ NOTE: This function used to handle the waypoint editor's continuous updates (like currently selected waypoint),
but it's such a mess. So, I moved all of that code to its own function.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
#ifdef PR_SFSDFSFSDFFDD
void frik_dynamicWaypointing()
{
	entity t;
	float dist, dynlink, dynpoint = 0, editor = 0;

	// PZ NOTE: WTF IS THIS AND WHY IS IT IN HERE?
	if (self->teleport_time > self->portal_time)
	{
		if (!(self->flags & PR_FL_WATERJUMP))
		{
			self->dyn_flags = 2;
			if (!self->ishuman)
			{
				frik_dropTarget(self->target1, PR_TRUE);
				self->enemy = world;
			}
		}
		self->portal_time = self->teleport_time;
	}
	// stacking everything on waypoint_mode might've been good for the editor,
	// but it sucks to beat hell for this code.

	// convert waypoint_mode to something more usable..
	if (waypoint_mode >= PR_WM_EDITOR)
	{
		if (self->ishuman)
		{
			if (waypoint_mode == PR_WM_EDITOR_DYNLINK)
				dynlink = 1;
			else if (waypoint_mode == PR_WM_EDITOR_DYNAMIC)
				dynlink = dynpoint = 1;
			editor = 1;
		}
	}
	else if (waypoint_mode == PR_WM_DYNAMIC)
		dynlink = dynpoint = 1;

	// if there's nothing for dynamic to do..
	if (!dynpoint)
	{
		if (!editor)
			return;
	}
	// for speed sake, I won't have bots dynamic waypoint in coop
	if (!self->ishuman)
		if (coop)
			return;

	// don't waypoint in single player
	if (max_clients < 2)
		return;
	// if you're dead
	else if (self->health <= 0)
	{
		if (dynpoint)
		{
			if (self->current_way != world)
			{
				if (pointcontents(self->origin) < -4)
				{
					if (self->current_way->b_aiflags & PR_AI_BLIND)
						self->current_way->b_aiflags = self->current_way->b_aiflags | PR_AI_PRECISION;
					else
						self->current_way->b_aiflags = self->current_way->b_aiflags | PR_AI_BLIND;
				}
			}
		}
		self->dyn_dest = V({0, 0, 0});
		self->current_way = world;
		self->dyn_flags = 0;
		return;
	}

	// you shouldn't be making waypoints mid air
	if (dynpoint)
	{
		if (!((self->flags & PR_FL_ONGROUND) || self->waterlevel == 3))
		{
			if (self->dyn_flags != 2)
			{
				self->dyn_flags = 1;
			}
			return;
		}
	}
	// keep from doing the rest of this every frame
	if (self->dyn_time > time)
		return;
	self->dyn_time = time + 0.2;

	// display the links for editor mode
	if (editor)
	{
		if (self->current_way != world)
		{
			if (self->current_way->target1 != world)
				frik_graphicallyDisplayWaypointLink(self->current_way, self->current_way->target1, self->current_way->b_aiflags & PR_AI_TELELINK_1);
			if (self->current_way->target2 != world)
				frik_graphicallyDisplayWaypointLink(self->current_way, self->current_way->target2, self->current_way->b_aiflags & PR_AI_TELELINK_2);
			if (self->current_way->target3 != world)
				frik_graphicallyDisplayWaypointLink(self->current_way, self->current_way->target3, self->current_way->b_aiflags & PR_AI_TELELINK_3);
			if (self->current_way->target4 != world)
				frik_graphicallyDisplayWaypointLink(self->current_way, self->current_way->target4, self->current_way->b_aiflags & PR_AI_TELELINK_4);
		}
		if (self->b_aiflags & PR_AI_HOLD_SELECT)
			return;
	}

	t = frik_findWaypointNearestAndVisibleToSelf(self->current_way);
	if (t != world)
	{
		dist = vlen(self->origin - t->origin);
		if (dist < 192)
		{
			if (dist < 64)
			{
				if (t != self->current_way)
				{
					if (dynlink)
					{
						if (!self->dyn_flags)
						{
							if (frik_isVisible_Waypoint(t, self->current_way))
								frik_linkWaypointToWaypoint(t, self->current_way);
						}
						if (self->dyn_flags == 2)
							frik_telelinkWaypointToWaypoint(self->current_way, t);
						else if (frik_isVisible_Waypoint(t, self->current_way))
							frik_linkWaypointToWaypoint(self->current_way, t);
					}
					if (editor)
					{
						setmodel(t, "progs/s_light.spr");
						if (self->current_way != world)
							setmodel(self->current_way, "progs/s_bubble.spr");
					}
				}
				self->current_way = t;
				self->dyn_flags = 0;
			}
			self->dyn_dest = self->origin + self->view_ofs;
			return;
		}
	}

	if (frik_detectAndHandlePlatform(PR_FALSE))
	{
		if (vlen(trace_ent->velocity) > 0)
		{
			if (self->dyn_plat)
				return;
			self->dyn_plat = PR_TRUE;
			if (!self->dyn_flags)
				self->dyn_flags = 1;
			//bprint(#PRINT_HIGH, "on a plat!!!!!\n");
		}
		else
			self->dyn_plat = PR_FALSE;
	}
	else
		self->dyn_plat = PR_FALSE;

	if (self->dyn_flags == 2)
		self->dyn_dest = self->origin + self->view_ofs;
	else if (self->dyn_dest == V({0, 0, 0}))
		self->dyn_dest = self->origin + self->view_ofs;
	if (!dynpoint)
		return;
	t = frik_createBlankWaypointHere(self->dyn_dest);

	if (!self->dyn_flags)
	{
		if (frik_isVisible_Waypoint(t, self->current_way))
			frik_linkWaypointToWaypoint(t, self->current_way);
	}
	if (self->dyn_flags == 2)
		frik_telelinkWaypointToWaypoint(self->current_way, t);
	else if (frik_isVisible_Waypoint(t, self->current_way))
		frik_linkWaypointToWaypoint(self->current_way, t);

	if (editor)
	{
		setmodel(t, "progs/s_light.spr");
		if (self->current_way != world)
			setmodel(self->current_way, "progs/s_bubble.spr");
	}
	self->current_way = t;
	self->dyn_flags = 0;

	self->dyn_dest = self->origin + self->view_ofs;

	if (frik_detectAndHandlePlatform(PR_FALSE))
	{
		if (trace_ent->classname == "door")
			t->b_aiflags = t->b_aiflags | PR_AI_DOORFLAG;
	}
}
#endif

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

Waypoint Loading from file

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_deleteAllWaypoints   (PZ: was called "ClearAllWays")

Deletes all waypoint entities.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_deleteAllWaypoints()
{
	entity t, n;
	t = way_head;
	while (t != world)
	{
		n = t->_next;
		frik_deleteWaypointVisualMarker(t); // PZ
		dremove(t);
		t = n;
	}
	way_head = world;
	way_tail = world;
	waypoints = 0;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_retrieveWaypointByNumber  (PZ: was called "WaypointForNum")

Returns the waypoint entity corresponding to the given
waypoint number (`num`). This number lives in the
waypoint's `.count`.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
entity frik_retrieveWaypointByNumber(float num)
{
	entity t;
	if (num < 1)
		return world;

	t = way_head;
	while (t != world)
	{
		if (t->count == num)
			return t;
		t = t->_next;
	}
	return world;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_updateWaypointLinkPointers

PZ: Instead of doing the below old way (frik_fixThisWaypoint(), etc.)
to update .target# fields, for waypoints, this does it in a loop, all
in one frame. This updates the pointer/entity waypoint links using the
numeral links.

If you give it a `waypoint`, it will only update that `waypoint`.
Otherwise, it does all waypoints (when given `world`).

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_updateWaypointLinkPointers(entity waypoint)
{
	entity ptr;

	if (waypoint != world && waypoint->classname != "waypoint" && waypoint->classname != "waypoint_copy")
		return;

	if (waypoint == world)
		ptr = way_head;
	else
		ptr = waypoint;
	while (ptr != world)
	{
		ptr->target1 = frik_retrieveWaypointByNumber(ptr->b_pants);
		ptr->target2 = frik_retrieveWaypointByNumber(ptr->b_skill);
		ptr->target3 = frik_retrieveWaypointByNumber(ptr->b_shirt);
		ptr->target4 = frik_retrieveWaypointByNumber(ptr->b_frags);
		if (waypoint == world) ptr = ptr->_next;
		else           ptr = world;
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_updateWaypointLinkNumbers

PZ: This updates the numeral waypoint links using the pointer/entity links.

If you give it a `waypoint`, it will only update that `waypoint`.
Otherwise, it does all waypoints (when given `world`).

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_updateWaypointLinkNumbers(entity waypoint)
{
	entity ptr;

	if (waypoint != world && waypoint->classname != "waypoint" && waypoint->classname != "waypoint_copy")
		return;

	if (waypoint == world)
		ptr = way_head;
	else
		ptr = waypoint;
	while (ptr != world)
	{
		ptr->b_pants = ptr->target1->count;
		ptr->b_skill = ptr->target2->count;
		ptr->b_shirt = ptr->target3->count;
		ptr->b_frags = ptr->target4->count;
		if (waypoint == world) ptr = ptr->_next;
		else           ptr = world;
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_fixThisWaypoint  (PZ: was called "FixThisWaypoint")

Makes a waypoint's .target# fields correspond
correctly to its .b_pants, etc., fields. .b_pants,
etc., fields store waypoint links 1 to 4 by waypoint
number. target1 through 4 store waypoint links by
entity pointer.

`self` = the 'fixer' entity, below

// PZ NOTE: This function is not currently being used. Replaced by above two functions.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_fixThisWaypoint()
{
	// PZ NOTE: .enemy here is just being used to loop through waypoints
	self->enemy->target1 = frik_retrieveWaypointByNumber(self->enemy->b_pants);
	self->enemy->target2 = frik_retrieveWaypointByNumber(self->enemy->b_skill);
	self->enemy->target3 = frik_retrieveWaypointByNumber(self->enemy->b_shirt);
	self->enemy->target4 = frik_retrieveWaypointByNumber(self->enemy->b_frags);

	self->enemy = self->enemy->_next; // go to the next waypoint
	self->nextthink = time;
	if (self->enemy == world)
	{
		dremove(self);
		fixer = world;
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_fixAllWaypoints  (PZ: was called "FixWaypoints")

Makes all waypoints' .target# fields correspond
correctly to their .b_pants, etc., fields.

// PZ NOTE: This function is not currently being used.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_fixAllWaypoints()
{
	if (fixer == world)
		fixer = spawnServerSide();       // PZ: make it a server-side-only entity
	fixer->classname = "waypoint_fixer";  // PZ: it's good for all entities to have a classname, for debugging
	fixer->nextthink = time;
	fixer->think = frik_fixThisWaypoint;
	fixer->enemy = way_head;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_deleteWaypoint  (PZ: was called "delete_waypoint")

Removes `waypoint` from the list of waypoints.
See the comments in the function for full details
on what this function does.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_deleteWaypoint(entity waypoint)
{
	entity ptr;

	if (waypoint == world || (waypoint->classname != "waypoint" && waypoint->classname != "waypoint_copy"))
	{
		bprint(PR_PRINT_HIGH, "Error: frik_deleteWaypoint() attempted to delete an invalid entity.\n");
		return;
	}

	// Update the waypoint linked list.
	if (way_head == waypoint) way_head = waypoint->_next;
	if (way_tail == waypoint) way_tail = waypoint->_prev;
	if (waypoint->_prev != world)       waypoint->_prev->_next = waypoint->_next;
	if (waypoint->_next != world)       waypoint->_next->_prev = waypoint->_prev;

	// Recount and re-number the waypoints. Also, delete all links (.target#) to `waypoint`.
	waypoints = 0;
	ptr = way_head;
	while (ptr != world)
	{
		ptr->count = waypoints = waypoints + 1; // set the waypoint's number
		if (frik_doesWaypointLinkToWaypoint(ptr, waypoint))
			frik_unlinkWaypointToWaypoint(ptr, waypoint);
		ptr = ptr->_next;
	}
	// Update all waypoints' numeral links.
	frik_updateWaypointLinkNumbers(world);   // `world` tells it to do the entire list of waypoints

	// For all players with `waypoint` selected in the editor, deselect `waypoint`.
	entity player; float counter;
	player = nextent(world); counter = 1;
	while (counter <= max_clients)
	{
		if (player->ishuman && player->current_way == waypoint)
			player->current_way = world;
		player = nextent(player); counter = counter + 1;
	}

	// Remove `waypoint`'s visual representation (if it has one) and remove `waypoint`.
	if (waypoint->owner != world) frik_deleteWaypointVisualMarker(waypoint);
	dremove(waypoint);
}

// frik_findNearestEntityToSelfWithThisClassname() and frik_findNextWaypointInOurRoute() are used by the pathing code in bot_ai.qc.
/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_findNearestEntityToSelfWithThisClassname    (PZ: was called "FindThing")

Finds the nearest entity to `self` whose classname is `s`.

`self` = typically, the current bot, but doesn't have to be

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
entity frik_findNearestEntityToSelfWithThisClassname(const string& s)
{
	entity t;
	float tdst, dst;
	entity best;
	dst = 100000;
	best = world;
	t = find(world, "classname", s);
	while (t != world)
	{
		tdst = vlen(((t->absmin + t->absmax) * 0.5) - self->origin);
		if (tdst < dst)
		{
			dst = tdst;
			best = t;
		}
		t = find(t, "classname", s);
	}
	return best;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_findNextWaypointInOurRoute   (PZ: was called "FindRoute")

This is a key function in the pathing. The name is a bit
misleading. This code finds the closest waypoint that is
part of a route calculated by the frik_startRouteFinding()
and end_route() routines. This is a definite path to an
object.
PZ NOTE: THERE DOESN'T APPEAR TO BE AN "end_route()" FUNCTION.

`self` = current bot
`lastone` = appears to be the last waypoint we were just on
`.movedir` = appears to be whether or not this waypoint is part of the route created for any given bot (bitfield)

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// kinda like frik_findWaypointNearestAndVisibleToSelf(), only of this bot's route though
entity frik_findNextWaypointInOurRoute(entity lastone)
{
	entity t, best;
	float leastDistance, dist/*, flag*/;

	//flag = getVectorBitFlagGivenBitNumber(self.b_clientno); // PZ: removed to support 32 bots

	if (lastone != world)
	{
		// PZ: not sure how this worked before, but this needs to be checking that it doesn't pick the one we were just on
		//if      (lastone.target1.b_sound & flag)
		if (lastone->target1 != self->last_way)
		{
			if (isVectorBitFlagSet(self->b_clientno, V({0, 0, 0}), lastone->target1->movedir)) // PZ: changed to support 32 bots
				return lastone->target1;
		}
		//else if (lastone.target2.b_sound & flag)
		if (lastone->target2 != self->last_way)
		{
			if (isVectorBitFlagSet(self->b_clientno, V({0, 0, 0}), lastone->target2->movedir)) // PZ: changed to support 32 bots
				return lastone->target2;
		}
		//else if (lastone.target3.b_sound & flag)
		if (lastone->target3 != self->last_way)
		{
			if (isVectorBitFlagSet(self->b_clientno, V({0, 0, 0}), lastone->target3->movedir)) // PZ: changed to support 32 bots
				return lastone->target3;
		}
		//else if (lastone.target4.b_sound & flag)
		if (lastone->target4 != self->last_way)
		{
			if (isVectorBitFlagSet(self->b_clientno, V({0, 0, 0}), lastone->target4->movedir)) // PZ: changed to support 32 bots
				return lastone->target4;
		}
	}

	// If we got here, the route is no good anymore. Find a new route.
	entity goalTarget;
	goalTarget = frik_getGoalTarget();
	frik_findRouteToObject(goalTarget, PR_FALSE); // not `direct`
	self->current_way = self->last_way = world;

if (PZ_DEBUG == self->b_clientno) {
bprint(2, "WTFWTFWTFWTFWTFWTF!!\n");
string temp;
temp = etos(lastone);
bprint(2, "lastone = ", temp, "\n");
}

	// PZ NOTE: find nearest waypoint that is part this bot's current route
	t = way_head;
	leastDistance = 100000;
	best = world;
	while (t != world)
	{
		dist = vlen(t->origin - self->origin);
		//if ((dist < leastDistance) && (t.b_sound & flag))
		if (dist < leastDistance && isVectorBitFlagSet(self->b_clientno, V({0, 0, 0}), t->movedir))  // PZ: changed to support 32 bots
		{
			leastDistance = dist;
			best = t;
		}
		t = t->_next;
	}
	if (best != self->current_way && best != self->last_way) // PZ: don't allow it to ever return the one we just came from
		return best;
	else
		return world;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

Route & path table management

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_clearRouteTable   (PZ: was called "ClearRouteTable")

PZ NOTE: This does not affect who owns the route table.
Which is noted by the global `route_table`.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// cleans up route table
void frik_clearRouteTable()
{
	entity t;

	busy_waypoints = 0;  // PZ: added this
	t = way_head;
	while (t != world)
	{
		t->keys = PR_FALSE;
		t->enemy = world;
		t->items = -1; // not in table
		t = t->_next;
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_clearRouteForSelf          (PZ: was called "ClearMyRoute")

For each waypoint, it removes the flag on the waypoint
that indicates that this waypoint is part of the
bot's (self's) current route, if that flag is set.

`self` = the current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_clearRouteForSelf()
{
	/*local float flag;*/
	entity t;

	//flag = getVectorBitFlagGivenBitNumber(self.b_clientno);        // PZ: removed to support 32 bots
	t = way_head;
	while (t != world)
	{
		//t.b_sound = t.b_sound - (t.b_sound & flag);
		t->movedir = clearVectorBitFlag(self->b_clientno, t->movedir);  // PZ: changed to support 32 bots
		t = t->_next;
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_storeSelfsRoute      (PZ: was called "mark_path")

After the route has been found, mark it with
bitflags so the table can be used for a
different bot.

Finds a waypoint nearest to `this`.
Marks a path for bot, `self`.

################## STUDY THIS MORE

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_storeSelfsRoute(entity waypointStart/*this*/)
{
	entity t/*, oldSelf*/;
	/*local float flag;*/

	frik_clearRouteForSelf();

	t = waypointStart;
	// loops through route table, using .enemy like a ".next"
if (PZ_DEBUG == self->b_clientno) bprint(2, "Storing route in .movedir: \n");
	while (t != world)
	{
		// if this waypoint is already a part of `self`'s route
		//if (t.b_sound & flag)
//		if (isVectorBitFlagSet(self.b_clientno, '0 0 0', t.movedir))  // PZ: changed to support 32 bots
//			return;
//		if (t == self.last_way)
//			return;

		// add the waypoint to `self`'s route
		//t.b_sound = t.b_sound | flag;
if (PZ_DEBUG == self->b_clientno) {
string temp;
temp = ftos(t->count);
bprint(2, "waypoint number = ", temp, ", classname = ", t->classname);
temp = etos(t);
bprint(2, ", entity number = ", temp);
}
		t->movedir = setVectorBitFlag(self->b_clientno, t->movedir);     // PZ: changed to support 32 bots
		t = t->enemy;
if (PZ_DEBUG == self->b_clientno) bprint(2, "\n");
	}
if (PZ_DEBUG == self->b_clientno) bprint(2, "\n");

return;
// OLD CODE
/*
	oldSelf = self;
	self = this;
	t = frik_findWaypointNearestAndVisibleToSelf(this.current_way);
	self = oldSelf;
	// FIXME
	// ugh, better way to find players please!!!
	if (this.classname != "player")
		this.current_way = t;

	if (t.enemy == world)
	{
		frik_dropTarget(this, #FALSE);
		self.route_failed = #TRUE;
		return;
	}

	//flag = getVectorBitFlagGivenBitNumber(self.b_clientno);         // PZ: removed to support 32 bots

	// loops through route table, using .enemy like a ".next"
	while (t)
	{
		 // if this waypoint is already a part of `self`'s route
		//if (t.b_sound & flag)
		if (isVectorBitFlagSet(self.b_clientno, '0 0 0', t.movedir))  // PZ: changed to support 32 bots
			return;
		if (t == self.last_way)
			return;
		// add the waypoint to `self`'s route
		//t.b_sound = t.b_sound | flag;
		t.movedir = setVectorBitFlag(self.b_clientno, t.movedir);     // PZ: changed to support 32 bots
		t = t.enemy;
	}*/
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

WaypointThink

Calculates the routes. We use thinks to avoid
tripping the runaway loop counter

`self` = the waypoint that we are following a link from

################## STUDY THIS MORE

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void FollowLink(entity linkedToWaypoint, int teleLink)   // `teleLink` was `b_bit`
{
	float dist;  // PZ NOTE: this is the cost of taking the link

	if (self->b_aiflags & teleLink) // if this link from the previous waypoint goes through a teleporter
		dist = self->items;
	else
		dist = vlen(self->origin - linkedToWaypoint->origin) + self->items;

	// check if this is an RJ link
	if (linkedToWaypoint->b_aiflags & PR_AI_SUPER_JUMP)
	{
		if (!frik_canBotRocketJump(route_table))
			return;
	}

	if (linkedToWaypoint->b_aiflags & PR_AI_DIFFICULT) // PZ NOTE: allows us to manually flag a waypoint as being a difficult route that should cost more
		dist = dist + 1000;

	if ((dist < linkedToWaypoint->items) || (linkedToWaypoint->items == -1))
	{
		if (!linkedToWaypoint->keys)
			busy_waypoints = busy_waypoints + 1;
		linkedToWaypoint->keys = PR_TRUE;
		linkedToWaypoint->items = dist;
		linkedToWaypoint->think = WaypointThink;
		linkedToWaypoint->nextthink = time;
		linkedToWaypoint->enemy = self;
	}
}

void WaypointThink()
{
	entity oldSelf;

	if (self->items == -1) // if we don't have a distance to last waypoint or starting object
		return;
	// can you say ugly?
	if (self->b_aiflags & PR_AI_TRACE_TEST) // only attempt to build a route through a link if there's nothing blocking traversal to the linked waypoint
	{
		if (self->target1 != world)
		{
			traceline(self->origin, self->target1->origin, PR_TL_BSP_ONLY, self);
			if (trace_fraction == 1)
				FollowLink(self->target1,PR_AI_TELELINK_1);
		}
		if (self->target2 != world)
		{
			traceline(self->origin, self->target2->origin, PR_TL_BSP_ONLY, self);
			if (trace_fraction == 1)
				FollowLink(self->target2, PR_AI_TELELINK_2);
		}
		if (self->target3 != world)
		{
			traceline(self->origin, self->target3->origin, PR_TL_BSP_ONLY, self);
			if (trace_fraction == 1)
				FollowLink(self->target3, PR_AI_TELELINK_3);
		}
		if (self->target4 != world)
		{
			traceline(self->origin, self->target4->origin, PR_TL_BSP_ONLY, self);
			if (trace_fraction == 1)
				FollowLink(self->target4, PR_AI_TELELINK_4);
		}
 	}
	else
	{
		if (self->target1 != world)
			FollowLink(self->target1, PR_AI_TELELINK_1);
		if (self->target2 != world)
			FollowLink(self->target2, PR_AI_TELELINK_2);
		if (self->target3 != world)
			FollowLink(self->target3, PR_AI_TELELINK_3);
		if (self->target4 != world)
			FollowLink(self->target4, PR_AI_TELELINK_4);
	}

	// this waypoint is done being used for routing(?)
	busy_waypoints = busy_waypoints - 1;
	self->keys = PR_FALSE;

	if (busy_waypoints <= 0)
	{
		if (direct_route)
		{
			oldSelf = self;
			self = route_table;
			frik_findRouteToObject(self->target1, PR_FALSE); // FALSE == `direct`
			self = oldSelf;
			direct_route = PR_FALSE;
		}
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_startRouteFinding      (PZ: was called "begin_route")

PLEASE NOTE: frik_findRouteToObject() replaces
the old calls to frik_startRouteFinding().

Routing isn't done all at once now, but in two
stages, the bot will calc a route *THEN*
choose a target, *THEN* mark a path.

Boy it's confusing.

`self` = the current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_startRouteFinding()
{
	if (busy_waypoints > 0)
		return PR_FALSE;

	if (route_table != world) // if the route table is already being used by another bot
	{
		if (!route_table->ishuman) // if it's not a bot? sounds like an error
		{
			if (route_table->b_clientno != -1) // not a client either? sounds like an error
				return PR_FALSE;
		}
	}

//frik_botSay("I have the route_table!\n");

	route_table = self; // acquire the route table for use
	frik_clearRouteTable();
	self->last_way = frik_findWaypointNearestAndVisibleToSelf(self->current_way); // find the closest waypoint to the bot

	if (self->last_way != world) // if we found a waypoint nearest and visible to the bot
	{
		self->last_way->items = vlen(self->last_way->origin - self->origin);
		self->last_way->nextthink = time;
		self->last_way->think = WaypointThink;
		self->last_way->keys = PR_TRUE;
		busy_waypoints = 1;
		return PR_TRUE;
	}
	else // allow use of the route table by another bot
	{
		route_table = world;
		busy_waypoints = 0;
		return PR_FALSE;
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_findRouteToObject     (PZ: was called "bot_get_path")

PLEASE NOTE: This function replaces the old calls
to frik_startRouteFinding().

Routing isn't done all at once now, but in two
stages, the bot will calc a route *THEN*
choose a target, *THEN* mark a path.

Boy it's confusing.

PZ NOTE: This is the main path-finding function.

`self`   = the current bot
`object` = the object we are getting a route to
`direct` = ?

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_findRouteToObject(entity object/*this*/, float direct)
{
	entity waypointStart, waypointGoal, oldSelf;
	float routeCost;

	if (object == world)
		return;

	waypointStart = frik_findWaypointNearestAndVisibleToSelf(way_head);
	oldSelf = self;
	self = object;
	waypointGoal = frik_findWaypointNearestAndVisibleToSelf(way_head);
	self = oldSelf;

if (PZ_DEBUG == self->b_clientno)
{
string temp;
temp = etos(waypointStart);
bprint(2, "Calling findRoute(", temp, ", ");
temp = etos(waypointGoal);
bprint(2, temp, ", ", self->netname, ")\n");
}
	routeCost = findRoute(waypointStart, waypointGoal, self);
	/*if (routeCost >= 0)
		frik_storeSelfsRoute(waypointStart);
	else
		frik_removeFromTargetStack(object);*/
	if (routeCost < 0)
		frik_removeFromTargetStack(object);
	// route should be good
	else
	{
		// this makes it select the first waypoint in the bot's route, when it selects the 'next' waypoint in the route
		self->current_way = world;
		self->b_routeInvalid = PR_FALSE;
	}

return;
// OLD CODE

	if (route_table == self)  // if the route table is currently being used for bot, `self`
	{
		if (busy_waypoints <= 0)  // was the route-finding successful?
		{
			route_table = world;  // free up the route table for use by other bots
			frik_storeSelfsRoute(object);  // store `self`'s route in .movedir
		}
		return;
	}
	if (direct) // PZ NOTE: is this for objects that are within sight? a direct path to the objects?
	{
		if (frik_startRouteFinding())
			direct_route = PR_TRUE;
		else
			frik_dropTarget(object, PR_FALSE);
		return;
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

BSP/QC Waypoint loading

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_    (PZ: was called "waypoint")



`self` = new waypoint

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// PZ: this doesn't appear to be getting called anywhere; so, I have commented it out for now
/*void() waypoint =
{
	self.solid = #SOLID_TRIGGER;
	self.movetype = #MOVETYPE_NONE;
	setorigin(self, self.origin);

	setsize(self, #VEC_HULL_MIN, #VEC_HULL_MAX);
	waypoints = waypoints + 1;
	if (!way_head)
	{
		way_head = self;
		way_tail = self;
	}
	else
	{
		way_tail._next = self;
		self._prev = way_tail;
		way_tail = self;
	}

	self.count = waypoints;
	waypoint_mode = #WM_LOADED;
	if (self.count == 1)
	{
		localcmd("echo BSP waypoints detected\n");
		self.think = frik_fixAllWaypoints; // wait until all bsp loaded points are spawned
		self.nextthink = time;
	}
};*/

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_createWaypoint    (PZ: was called "make_way")

Creates a new waypoint at `position`, with links to
waypoints (by number), in `link1_2_and_3` and `link4`,
and with `AI_flags`.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_createWaypoint(const vector& position, const vector& link1_2_and_3, float link4, float AI_flags)
{
	entity waypoint;
	//waypoint_mode = #WM_LOADED; // PZ: commented out
	waypoint = frik_createBlankWaypointHere(position); // instantiate it
	waypoint->b_aiflags = AI_flags;
	waypoint->b_pants = link1_2_and_3[X];
	waypoint->b_skill = link1_2_and_3[Y];
	waypoint->b_shirt = link1_2_and_3[Z];
	waypoint->b_frags = link4;
	// PZ: I'm now updating all waypoint .target# fields in a loop, in the same frame that the waypoints are created.
	// See frik_updateWaypointLinkPointers().
	/*if (waypoint.count == 1) // PZ NOTE: This was done on first waypoint to print out the message one time.
	{
		//localcmd("echo QuakeC waypoints detected\n");  // PZ: moved
		// PZ NOTE: this won't run until the next frame; all waypoints will have been loaded this frame
		waypoint.think = frik_fixAllWaypoints; // wait until all QC loaded points are spawned
		waypoint.nextthink = time;
	}*/
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_copyWaypoint

PZ: Creates a copy of `waypoint` and returns the copy.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
entity frik_copyWaypoint(entity waypoint)
{
	entity waypointCopy;
	vector positionOfCopy;

	if (waypoint == world) return world;

	// determine the position of the waypoint copy
	positionOfCopy = waypoint->origin;
	if (b_copyAcrossXAxis)
	{
		positionOfCopy[Y] = b_mapCenter[Y] - (waypoint->origin[Y] - b_mapCenter[Y]);
		if (b_copyMode == 0 /*ROTATED_COPY*/) // Rotated     // PZ NOTE: Stupid precompiler requires pre-declarations of defines.
			positionOfCopy[X] = b_mapCenter[X] - (waypoint->origin[X] - b_mapCenter[X]);
	}
	else // across Y's axis
	{
		positionOfCopy[X] = b_mapCenter[X] - (waypoint->origin[X] - b_mapCenter[X]);
		if (b_copyMode == 0 /*ROTATED_COPY*/) // Rotated
			positionOfCopy[Y] = b_mapCenter[Y] - (waypoint->origin[Y] - b_mapCenter[Y]);
	}
	// instantiate it
	waypointCopy = frik_createBlankWaypointHere(positionOfCopy);
	if (b_waypointCopyList == world) b_waypointCopyList = waypointCopy;
	waypointCopy->classname = "waypoint_copy";         // need a way to tell the difference between a copy and a real waypoint
	waypointCopy->b_aiflags = waypoint->b_aiflags;

	// links are copied over later, once all copies are created

	return waypointCopy;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_createTemporaryWaypoint    (PZ: was called "SpawnTempWaypoint")

Temporary Marker code

PZ NOTE: this is only used by dynamic pathing (bots creating their own waypoints)

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_createTemporaryWaypoint(const vector& org)
{
	entity tep;

	if (self->temp_way == world)
		self->temp_way = tep = spawnServerSide(); // PZ: make it a server-side-only entity
	else
		tep = self->temp_way;

	tep->classname = "temp_waypoint";
	tep->solid = PR_SOLID_TRIGGER;
	tep->movetype = PR_MOVETYPE_NOCLIP;
	setorigin(tep, org);
	frik_addToTargetStack(tep);
	setsize(tep, PR_VEC_HULL_MIN, PR_VEC_HULL_MAX); // FIXME: convert these to numerical
}

} // END namespace Progs
