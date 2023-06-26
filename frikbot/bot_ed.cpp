/***********************************************
*                                              *
*          FrikBot Waypoint Editor             *
*   "The 'wtf is this doing in my mod' code"   *
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
#include "frikbot/bot_way.h"
#include "frikbot/bot_ai.h"
#include "debug.h"

namespace Progs {

// PZ: Had to add #PRINT_HIGH to all the sprint() and bprint() calls in this file. FrikBotX does
// not come with waypoint editor support for QuakeWorld. So, I've had to work on making it work in QW.

// this var has not been added to progdefs; ERROR: apparently the below doesn't work.. I had an invalid address
entity slideMenuController; //= &sv.edicts->v; // the player who has control of the Slide Copies menu (IF YOU INIT TO world HERE, IT WON'T BE RIGHT, BECAUSE world DOESN'T GET SET IMMEDIATELY AT STARTUP)

void frik_printAllWaypointsInBSPFormat();
void frik_printAllWaypointsInQuakeCFormat();
//void() DumpWaypoints;
void frik_saveAllWaypointsInWayFormat();

// PZ NOTE: I'm not really keeping the following comments up to date. They don't seem very useful to me.
/*
// source for the menu strings...

\b-- Main Menu --\b\n
\[\1\] >> Waypoint Management\n
\[\2\] >> Link Management    \n
\[\3\] >> AI Flag Management \n
\[\4\] >> Bot Management     \n
\[\5\] >> Waylist Management \n
\[\6\] \[\{133}\] Noclip            \n
\[\7\] \[\{133}\] Godmode           \n
\[\8\] \[\{133}\] Hold Selection    \n
\[\9\] Teleport to Way #     \n
\[\0\] Close Menu            \n

// missing from main is show way info
// iffy on the teleport to way thing being on main...seems like either a bot or way list thing

\b-- Waypoint Management --\b\n
\[\1\] Move Waypoint         \n
\[\2\] Delete Waypoint       \n
\[\3\] Make Waypoint         \n
\[\4\] Make Way + Link       \n
\[\5\] Make Way + Link X2    \n
\[\6\] Make Way + Telelink   \n
\[\7\] Show waypoint info    \n
\[\8\] >> Link Management    \n
\[\9\] >> AI Flag Management \n
\[\0\] >> Main Menu          \n

\b-- Link Management --\b\n
\[\1\] Unlink Waypoint       \n
\[\2\] Create Link           \n
\[\3\] Create Telelink       \n
\[\4\] Delete Link           \n
\[\5\] Create Link X2        \n
\[\6\] Delete Link X2        \n
\[\7\] > Make Waypoint       \n
\[\8\] >> Waypoint Management\n
\[\9\] >> AI Flag Management \n
\[\0\] >> Main Menu          \n

// Ai flags...ugh

\b-- AI Flag Management --\b\n
\[\1\] \[\{133}\] Door Flag         \n
\[\2\] \[\{133}\] Precision         \n
\[\3\] \[\{133}\] Surface for air   \n
\[\4\] \[\{133}\] Blind mode        \n
\[\5\] \[\{133}\] Jump              \n
\[\6\] \[\{133}\] Directional       \n
\[\7\] \[\{133}\] Super Jump        \n
\n
\[\9\] >> AI Flags page 2    \n
\[\0\] >> Main Menu          \n

\b-- AI Flags pg. 2--\b\n
\[\1\] \[\{133}\] Difficult         \n
\[\2\] \[\{133}\] Wait for plat     \n
\[\3\] \[\{133}\] Ride train        \n
\[\4\] \[\{133}\] Door flag no open \n
\[\5\] \[\{133}\] Ambush            \n
\[\6\] \[\{133}\] Snipe             \n
\[\7\] \[\{133}\] Trace Test        \n
\n
\[\9\] >> AI Flag Management \n
\[\0\] >> Main Menu          \n

\b-- Bot Management --\b\n
\[\1\] Add a Test Bot        \n
\[\2\] Order Test Bot here   \n
\[\3\] Remove Test Bot       \n
\[\4\] Stop Test Bot         \n
\[\5\] Teleport Bot here     \n
\[\6\] Teleport to Way #     \n
\n
\n
\n
\[\0\] >> Main Menu          \n

\b-- Waylist Management --\b\n
\[\1\] Delete ALL Waypoints \n
\[\2\] Dump Waypoints       \n
\[\3\] Check For Errors     \n
\[\4\] Save Waypoints       \n
\[\5\] \[\{133}\] Dynamic Mode     \n
\[\6\] \[\{133}\] Dynamic Link     \n
\[\7\] \[\{133}\] WAY output       \n
\[\8\] \[\{133}\] QC output        \n
\[\9\] \[\{133}\] BSP ents output  \n
\[\0\] Main Menu            \n

\b-- Misc Commands --\b\n
\[\1\] Teleport to Selected \n
\[\2\] Select               \n
\[\3\] Swap to old          \n
\[\4\] Trace select         \n
\[\5\] Create way in path   \n
\[\6\] Delete Way from path \n

\b-- Editor Memory --\b\n
\[\1\] Store as slot 1      \n
\[\2\] Store as slot 2      \n
\[\3\] Store as slot 3      \n
\[\4\] Store as slot 4      \n
\[\5\] Recall slot 1        \n
\[\6\] Recall slot 2        \n
\[\7\] Recall slot 3        \n
\[\8\] Recall slot 4        \n
*/

// PZ: Waypoint copying modes (stored in `b_copyMode`). A copy of one side of the map is performed, and then...
#define PR_ROTATED_COPY       0   // ..rotated and positioned on the other side of the map.   (like 2fort5, well6, rock2, etc.)
#define PR_MIRRORED_COPY      1   // ..mirrored and positioned on the other side of the map.  (like 2farms, 2night2)
// ### We don't need this. All we need is ability to rotate copies. And for maps like 4f4, all we need is the ability to repeat
//     copies, without overwriting areas where we are copying to.
#define PR_UNALTERED_COPY     2   // ..only positioned on the other side of the map. (TODO)   (like ditch2)

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_toggleWaypointEditorOnOrOff    (PZ: was "bot_way_edit")

`self` = the current player

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_toggleWaypointEditorOnOrOff()
{
	entity t;
	//float f;

	// PZ NOTE: Handles the teleport to waypoint menu option. (PZ: moved to frik_updateWaypointEditorForSelf())
	/*if (self.b_menu_value)
	{
		if (self.b_menu == #MENU_ENTRY_TELEPORT_TO_WAY)
		{
			t = frik_retrieveWaypointByNumber(self.b_menu_value);
			if (t)
				setorigin(self, t.origin - self.view_ofs);
			else
				sprint(self, #PRINT_HIGH, "No waypoint with that number\n");

			self.b_menu = #MENU_MAIN;
			self.b_timeToRedrawWaypointEditorMenu = time;
		}
		self.b_menu_value = 0;
		return;
	}*/
	// entering editor
	if (waypoint_mode < PR_WM_EDITOR)
	{
		waypoint_mode = PR_WM_EDITOR;
		localcmd("timelimit 0; fraglimit 0\n");
		sprint(self, PR_PRINT_HIGH, "The waypoint editor is now ON. 'timelimit' set to 0. You will have to manually set timelimit back.\n");
		//self.b_menu = #MENU_MAIN; // PZ: do it later
		//self.b_timeToRedrawWaypointEditorMenu = time;    // PZ: do it later
		//cvar_set("saved2", "0"); // PZ: need? QuakeWorld doesn't seem to know what saved2 is.
		//WriteByte(#MSG_ALL, 8);
		//WriteByte(#MSG_ALL, 1);
		//WriteString(#MSG_ALL, "MAKE SURE THE FOLLOWING LINE CONTAINS -CONDEBUG BEFORE PROCEEDING\n");
		//localcmd("cmdline\n");  // PZ: this doesn't seem to work in QuakeWorld
		// PZ: Originally, this would make waypoints visible by giving them a model, but now I have waypoints in the server-side-only
		// entities (numbers 512 and up) -- entities that don't get sent to clients. So, to make the waypoints visible, I make new waypoint
		// marker entities, and make those visible, at the same location as their corresponding waypoints. Did similarly, below, where waypoints
		// are hidden.

		// PZ: Instead of doing the below, call frik_updateWaypointVisualMarkers().
		frik_updateWaypointVisualMarkers();
		/*t = way_head;
		while (t)
		{
			frik_createWaypointVisualMarker(t); // TODO (??): make the below code a function
			t = t._next;
		}
		if (self.current_way)
			setmodel(self.current_way.owner, "progs/s_light.spr");*/
	}
	// leaving editor
	else
	{
		//saved2 = cvar("saved2");  // PZ: need? QuakeWorld doesn't seem to know what saved2 is.
		/*if (saved2 != 0)          // PZ: What is this section doing? Took it out on Dec 26, 2015.
		{
			f = self.b_menu;
			self.b_menu = floor(saved2/16);
			self.impulse = saved2 & 15;
			frik_displayAndManageWaypointEditorMenu();
			if (self.b_menu == floor(saved2/16))
				self.b_menu = f;
			//cvar_set("saved2", "0"); // PZ: need? QuakeWorld doesn't seem to know what saved2 is.
			return;
		}*/
		waypoint_mode = PR_WM_LOADED;
		sprint(self, PR_PRINT_HIGH, "The waypoint editor is now OFF. You will have to manually set timelimit back (also fraglimit), if you haven't already.\n");
		self->b_menu = 0;
		centerprint(self, "\n"); // PZ: Make it immediately remove the waypoint editor menu from the screen.

		// hide all waypoints
		t = way_head;
		while (t != world)
		{
			//setmodel(t, string_null);         // hide the waypoint
			frik_deleteWaypointVisualMarker(t); // remove its corresponding waypoint marker
			t = t->_next;
		}
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_updateWaypointEditorForSelf

PZ: I took this code from DynamicWaypoint(). It handles
the waypoint editor's continuous updates (like currently
selected waypoint).

The function this was a part of was such a mess. So, I
moved all of this code to its own function, here.

`self` = player using editor

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// PZ TODO (FIXED): NOT COMPATIBLE WITH MULTIPLE PLAYERS IN THE EDITOR.
//float frik_updateWaypointEditorForSelf_jumpLastPressed;
//float frik_updateWaypointEditorForSelf_fireLastPressed;
//float frik_updateWaypointEditorForSelf_lastTimeToggled;    // only need one storage location for this value
//float frik_updateWaypointEditorForSelf_copiesVisible;      // for toggling visible/invisible for waypoint copies
void frik_updateWaypointEditorForSelf()
{
	entity t;
	float dist;
	bool movingHorizontally = PR_FALSE;

	// determine the currently selected waypoint (the waypoint we are nearest to)
	if (!(self->b_aiflags & PR_AI_HOLD_SELECT)) // if we aren't in the 'hold selected waypoint' mode
	{
		t = frik_findWaypointNearestAndVisibleToSelf(self->current_way);
		if (t != world)
		{
			dist = vlen(self->origin - t->origin);
			if (dist < 64)
			{
				if (t != self->current_way)
				{
					// the waypoint visual marker update routine will handle this now
					/*setmodel(t.owner, "progs/s_light.spr"); // set its marker's model to the big globe (selected)
					if (self.current_way)
						setmodel(self.current_way.owner, "progs/s_bubble.spr"); // set its marker's model to the bubble (not selected)*/
					self->current_way = t;
				}
			}
		}
	}
	// display any links for the selected waypoint
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
	// handle the Teleport To Waypoint menu option
	if (self->b_menu == PR_MENU_ENTRY_TELEPORT_TO_WAY)
	{
		// Detect the player pressing Enter/jump. In water, I couldn't get it to detect jump properly. So, it
		// just checks that you are swimming upward. Tries to avoid triggering on swimming upward when using movement keys.
		if      (fabs(self->velocity[X]) > 0.1) movingHorizontally = PR_TRUE; // NOTE: This was done this way because you can't get more than one return value in one statement.
		else if (fabs(self->velocity[Y]) > 0.1) movingHorizontally = PR_TRUE;
		if (self->PR_BUTTON_JUMP || /*!(self.flags & #FL_JUMPRELEASED) ||*/
		    ((self->waterlevel >= 2 || self->classname == "spec") && self->velocity[Z] > 0 && !movingHorizontally)) // only moving vertically
		{
			// if the player entered a number, attempt to teleport to that waypoint
			if (self->b_menu_value)
			{
				t = frik_retrieveWaypointByNumber(self->b_menu_value);
				if (t != world)
					setorigin(self, t->origin - self->view_ofs);
				else
					sprint(self, PR_PRINT_HIGH, "No waypoint with that number\n");
			}
			// go back to main menu
			self->b_menu = PR_MENU_MAIN;
			self->b_timeToRedrawWaypointEditorMenu = time;
			self->b_menu_value = 0;
		}
	}
	// handle the Go To Coordinate menu option
	else if (self->b_menu >= PR_MENU_ENTRY_GO_TO_COORD_X && self->b_menu <= PR_MENU_ENTRY_GO_TO_COORD_Z)
	{
		// Shooting will negate the entry.
		if (self->PR_BUTTON_FIRE && !self->ed_fireLastPressed && self->b_menu_value != PR_LARGEST_INTEGER)
			self->b_menu_value = 0 - self->b_menu_value;
		// Detect the player pressing Enter/jump. In water, I couldn't get it to detect jump properly. So, it
		// just checks that you are swimming upward. Tries to avoid triggering on swimming upward when using movement keys.
		if      (fabs(self->velocity[X]) > 0.1) movingHorizontally = PR_TRUE; // NOTE: This was done this way because you can't get more than one return value in one statement.
		else if (fabs(self->velocity[Y]) > 0.1) movingHorizontally = PR_TRUE;
		if (self->PR_BUTTON_JUMP || /*!(self.flags & #FL_JUMPRELEASED) ||*/
		    ((self->waterlevel >= 2 || self->classname == "spec") && self->velocity[Z] > 0 && !movingHorizontally)) // only moving vertically
		{
			if (!self->ed_jumpLastPressed)
			{
				self->ed_jumpLastPressed = PR_TRUE;               // I was having issues with it going through all 3 of these in one go. So, hopefully this line will prevent that.
				// PZ TODO: NOT COMPATIBLE WITH MULTIPLE PLAYERS IN THE EDITOR. b_goToCoordinate is global rather than field.
				if      (self->b_menu == PR_MENU_ENTRY_GO_TO_COORD_X) // X entry
				{
					if (self->b_menu_value == PR_LARGEST_INTEGER) // means no entry; use player's x position
						b_goToCoordinate[X] = self->origin[X];
					else
						b_goToCoordinate[X] = self->b_menu_value;
					self->b_menu = PR_MENU_ENTRY_GO_TO_COORD_Y;
					self->b_menu_value = PR_LARGEST_INTEGER;
				}
				else if (self->b_menu == PR_MENU_ENTRY_GO_TO_COORD_Y) // Y entry
				{
					if (self->b_menu_value == PR_LARGEST_INTEGER) // means no entry; use player's y position
						b_goToCoordinate[Y] = self->origin[Y];
					else
						b_goToCoordinate[Y] = self->b_menu_value;
					self->b_menu = PR_MENU_ENTRY_GO_TO_COORD_Z;
					self->b_menu_value = PR_LARGEST_INTEGER;
				}
				else if (self->b_menu == PR_MENU_ENTRY_GO_TO_COORD_Z) // Z entry
				{
					if (self->b_menu_value == PR_LARGEST_INTEGER) // means no entry; use player's z position
						b_goToCoordinate[Z] = self->origin[Z];
					else
						b_goToCoordinate[Z] = self->b_menu_value;
					// move to coordinate
					setorigin(self, b_goToCoordinate);

					self->b_menu = PR_MENU_COPY_WAYS; // Copy Waypoints menu
					self->b_menu_value = 0;
				}
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
		}
		else
			self->ed_jumpLastPressed = PR_FALSE;
	}
	// handle continous updates for the Copy Waypoints menu
	if (self->b_menu >= PR_MENU_COPY_WAYS && self->b_menu <= PR_MENU_SLIDE_WAY_COPIES)
	{
		// display the axis for the copy
		entity e;
		e = find(world, "classname", "waypoint_editor_map_center"); // FIXME: could speed this up by remembering the entity in a global
		WriteByte(PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
		WriteByte(PR_MSG_BROADCAST, PR_TE_LIGHTNING2);
		WriteEntity(PR_MSG_BROADCAST, e);
		if (b_copyAcrossXAxis)
		{
			WriteCoord(PR_MSG_BROADCAST, e->origin[X] - 60);
			WriteCoord(PR_MSG_BROADCAST, e->origin[Y]);
			WriteCoord(PR_MSG_BROADCAST, e->origin[Z]);
			WriteCoord(PR_MSG_BROADCAST, e->origin[X] + 40);
			WriteCoord(PR_MSG_BROADCAST, e->origin[Y]);
			WriteCoord(PR_MSG_BROADCAST, e->origin[Z]);
		}
		else
		{
			WriteCoord(PR_MSG_BROADCAST, e->origin[X]);
			WriteCoord(PR_MSG_BROADCAST, e->origin[Y] - 60);
			WriteCoord(PR_MSG_BROADCAST, e->origin[Z]);
			WriteCoord(PR_MSG_BROADCAST, e->origin[X]);
			WriteCoord(PR_MSG_BROADCAST, e->origin[Y] + 40);
			WriteCoord(PR_MSG_BROADCAST, e->origin[Z]);
		}
		multicast(e->origin, PR_MULTICAST_PVS);

		// for all waypoint copies, make their waypoint marker entities flash from visible to invisible
		// NOTE: We are now handling this in frik_updateWaypointVisualMarkers().
		/*if (time > frik_updateWaypointEditorForSelf_lastTimeToggled + 0.25)
		{
			local entity wp;
			wp = b_waypointCopyList;
			while (wp != world && wp.classname == "waypoint_copy")
			{
				//wp.owner.frame = !wp.owner.frame; // makes it toggle from frame 0 to frame 1 (big bubble to small bubble)
				// Don't like the little bubbles. Looks like it's going far away. Just flash instead.
				//if (wp.owner.frame == 0) wp.owner.frame = 999; // isn't making bubbles go invis
				//else                     wp.owner.frame = 0;
				//if (wp.owner.model == "")  // previously did it this way
				if (wp.owner) // if it has a waypoint marker
				{
					if (!frik_updateWaypointEditorForSelf_copiesVisible)
					{
						if (wp == self.current_way) setmodel(wp.owner, "progs/s_light.spr");  // big globe (selected)
						else                        setmodel(wp.owner, "progs/s_bubble.spr"); // bubble (not selected)
					}
					else                            setmodel(wp.owner, ""); // invisible
				}
				wp = wp._next;
			}
			frik_updateWaypointEditorForSelf_copiesVisible = !frik_updateWaypointEditorForSelf_copiesVisible;
			frik_updateWaypointEditorForSelf_lastTimeToggled = time;
		}*/
	}
	// PZ: Handle sliding waypoint copies for the "Copy Waypoints -> Slide" menu
	// PZ TODO DONE: NOT COMPATIBLE WITH MULTIPLE PLAYERS IN THE EDITOR. I think this will get confused as hell with more than one player in the editor.
	// PZ IDEA: What if we made it so that a player can't enter this menu, if another player is already in it?
	//          -How about, instead, just create an ent variable that points to the player with a lock on the slide menu. Whoever enters the menu first gets control of it,
	//           until they leave the menu. Maybe write at the bottom who has control of it.
	if (self->b_menu == PR_MENU_SLIDE_WAY_COPIES)
	{
		float  facingDirection;
		vector directionToMove, velocityOfMove;
		directionToMove = velocityOfMove = V({0, 0, 0});
		//bprint(2, vtos(directionToMove)); // for some reason, this bprint only goes to server console (???)

		// when a player in control of the slide menu leaves the slide menu, release the lock
		if (slideMenuController != world && slideMenuController->b_menu != PR_MENU_SLIDE_WAY_COPIES) slideMenuController = world;
		// if no one has a lock on the slide menu, give `self` control (the lock)
		if (slideMenuController == world) slideMenuController = self;
		// if `self` has a lock on the slide menu, allow them to control it
		if (slideMenuController == self)
		{
			if (b_slideDirection) // is player asking for waypoint copies to move?
			{
				// determine direction player is closest to facing
				//                          90 deg: +Y  North
				//  180 deg: -X  West                                 0 deg: +X  East
				//                         -90 deg: -Y  South
				if      (self->angles[Y] >=   45 && self->angles[Y] <  135) facingDirection = 0; // North
				else if (self->angles[Y] >=  135 || self->angles[Y] < -135) facingDirection = 1; // West
				else if (self->angles[Y] >= -135 && self->angles[Y] <  -45) facingDirection = 2; // South
				else                                                        facingDirection = 3; // East
				// determine absolute direction waypoint copies need to move
				// (There is probably a neater, more-mathematical way to do this, but for now this is it.)
				//         directionToMove_y =  1; // North   // for reference
				//         directionToMove_y = -1; // South
				//         directionToMove_x =  1; // East
				//         directionToMove_x = -1; // West
				if      (facingDirection == 0) // North
				{
					if      (b_slideDirection == 1) // Left
						directionToMove[X] = -1; // West
					else if (b_slideDirection == 2) // Right
						directionToMove[X] =  1; // East
					else if (b_slideDirection == 3) // Forward
						directionToMove[Y] =  1; // North
					else                            // Back
						directionToMove[Y] = -1; // South
				}
				else if (facingDirection == 1) // West
				{
					if      (b_slideDirection == 1) // Left
						directionToMove[Y] = -1; // South
					else if (b_slideDirection == 2) // Right
						directionToMove[Y] =  1; // North
					else if (b_slideDirection == 3) // Forward
						directionToMove[X] = -1; // West
					else                            // Back
						directionToMove[X] =  1; // East
				}
				else if (facingDirection == 2) // South
				{
					if      (b_slideDirection == 1) // Left
						directionToMove[X] =  1; // East
					else if (b_slideDirection == 2) // Right
						directionToMove[X] = -1; // West
					else if (b_slideDirection == 3) // Forward
						directionToMove[Y] = -1; // South
					else                            // Back
						directionToMove[Y] =  1; // North
				}
				else                           // East
				{
					if      (b_slideDirection == 1) // Left
						directionToMove[Y] =  1; // North
					else if (b_slideDirection == 2) // Right
						directionToMove[Y] = -1; // South
					else if (b_slideDirection == 3) // Forward
						directionToMove[X] =  1; // East
					else                            // Back
						directionToMove[X] = -1; // West
				}
				// determine how fast to move in given direction
				if      (b_slideSpeed == 0) // slow
					velocityOfMove =   5 * directionToMove;
				else if (b_slideSpeed == 1) // medium
					velocityOfMove =  50 * directionToMove;
				else                        // fast
					velocityOfMove = 250 * directionToMove;
			}
			// set the velocity/movement of waypoint copies
			entity wp = b_waypointCopyList;
			while (wp != world && wp->classname == "waypoint_copy")
			{
				wp->velocity = velocityOfMove;
				if (wp->owner != world) wp->owner->velocity = velocityOfMove; // move the waypoint's visual marker, too
				wp = wp->_next;
			}
		}
	}

	//self.ed_jumpLastPressed = self.#BUTTON_JUMP;  // I took this out because I need it to check not just jumping but also moving up while swimming or spectating.
	self->ed_fireLastPressed = self->PR_BUTTON_FIRE;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_displayAndManageWaypointEditorMenu    (PZ: was "bot_menu_display")

`self` = the current player

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_displayAndManageWaypointEditorMenu()
{
	// build options
	string s1, s2, s3, s4, s5, s6, s7, h;
	entity t, oldSelf;

	// ####################################################
	// MENU INPUT
	// ####################################################
	if (self->b_menu && self->impulse >= 1 && self->impulse <= 10)
	{
		if (self->b_menu == PR_MENU_MAIN) // [Main Menu]
		{
			if (self->impulse == 1) // >> Waypoint Management
			{
				self->b_menu = PR_MENU_WAYPOINTS;
				self->b_timeToRedrawWaypointEditorMenu = time;    // "time" means now
			}
			else if (self->impulse == 2) // >> Link Management
			{
				self->b_menu = PR_MENU_LINKS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 3) // >> AI Flag Management
			{
				self->b_menu = PR_MENU_FLAGS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 4) // >> Bot Management
			{
				self->b_menu = PR_MENU_BOTS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 5) // >> Waylist Management
			{
				self->b_menu = PR_MENU_WAYLIST;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 6) // Noclip
			{
				if (self->movetype == PR_MOVETYPE_NOCLIP)
					self->movetype = PR_MOVETYPE_WALK;
				else
					self->movetype = PR_MOVETYPE_NOCLIP;
				self->b_timeToRedrawWaypointEditorMenu = time;

			}
			else if (self->impulse == 7) // Godmode
			{
				if (self->flags & PR_FL_GODMODE)
					self->flags = self->flags - PR_FL_GODMODE;
				else
					self->flags = self->flags | PR_FL_GODMODE;
				self->b_timeToRedrawWaypointEditorMenu = time;

			}
			else if (self->impulse == 8) // Hold Selection
			{
				if (self->b_aiflags & PR_AI_HOLD_SELECT)
					self->b_aiflags = self->b_aiflags - PR_AI_HOLD_SELECT;
				else
					self->b_aiflags = self->b_aiflags | PR_AI_HOLD_SELECT;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 9) // Teleport to Way No.
			{
				self->b_menu = PR_MENU_ENTRY_TELEPORT_TO_WAY;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 10) // << Exit Waypoint Editor
				frik_toggleWaypointEditorOnOrOff();
		}
		else if (self->b_menu == PR_MENU_WAYPOINTS) // [Waypoint Management]
		{
			if (self->impulse == 1) // Move Waypoint
			{
				if (self->current_way != world)
				{
					// PZ NOTE: If this is just setting the origin, why do waypoints often slowly move to your location?
					//          I think that's just the way the Quake engine works. It only does that when moving ents a short distance.
					setorigin(self->current_way, self->origin + self->view_ofs);
					if (self->current_way->owner != world) setorigin(self->current_way->owner, self->origin + self->view_ofs); // PZ: for waypoint visual marker
				}
			}
			else if (self->impulse == 2) // Delete Waypoint
			{
				if (self->current_way != world)
				{
					self->b_menu = PR_MENU_ENTRY_DELETE_WAY;
					self->b_timeToRedrawWaypointEditorMenu = time;
					self->last_way = self->current_way;
				}
			}
			else if (self->impulse == 3) // Make Waypoint
			{
				frik_createBlankWaypointHere(self->origin + self->view_ofs);
			}
			else if (self->impulse == 4) // Make Way + Link
			{
				t = frik_createBlankWaypointHere(self->origin + self->view_ofs);
				if (!frik_linkWaypointToWaypoint(self->current_way, t))
					sprint(self, PR_PRINT_HIGH, "Unable to link them\n");
			}
			else if (self->impulse == 5) // Make Way + Link X2
			{
				t = frik_createBlankWaypointHere(self->origin + self->view_ofs);
				if (!frik_linkWaypointToWaypoint(self->current_way, t))
					sprint(self, PR_PRINT_HIGH, "Unable to link old to new\n");
				frik_linkWaypointToWaypoint(t, self->current_way);
			}
			else if (self->impulse == 6) // Make Way + Telelink
			{
				t = frik_createBlankWaypointHere(self->origin + self->view_ofs);
				if (!frik_telelinkWaypointToWaypoint(self->current_way, t))
					sprint(self, PR_PRINT_HIGH, "Unable to link them\n");
			}
			else if (self->impulse == 7) // Show waypoint info
			{
				if (self->current_way != world)
				{
					sprint(self, PR_PRINT_HIGH, "Waypoint info for waypoint: ");
					h = ftos(self->current_way->count);
					sprint(self, PR_PRINT_HIGH, h);
					h = etos(self->current_way);
					sprint(self, PR_PRINT_HIGH, " (Entity ", h, ")\n");
					h = vtos(self->current_way->origin);                          // PZ: output the waypoint's origin
					sprint(self, PR_PRINT_HIGH, "Location: ", h, "\n");           // PZ
					sprint(self, PR_PRINT_HIGH, "AI Flags value: ");
					h = ftos(self->current_way->b_aiflags);
					sprint(self, PR_PRINT_HIGH, h, "\n");

					float printSeparator;
					if (self->current_way->target1 != world)
					{
						printSeparator = PR_TRUE;
						h = ftos(self->current_way->target1->count);
						if (self->current_way->b_aiflags & PR_AI_TELELINK_1)
							sprint(self, PR_PRINT_HIGH, "Telelink1 to: ");
						else
							sprint(self, PR_PRINT_HIGH, "Link1 to: ");
						sprint(self, PR_PRINT_HIGH, h);
					}
					if (self->current_way->target2 != world)
					{
						if (printSeparator) sprint(self, PR_PRINT_HIGH, " , ");
						printSeparator = PR_TRUE;
						h = ftos(self->current_way->target2->count);
						if (self->current_way->b_aiflags & PR_AI_TELELINK_2)
							sprint(self, PR_PRINT_HIGH, "Telelink2 to: ");
						else
							sprint(self, PR_PRINT_HIGH, "Link2 to: ");
						sprint(self, PR_PRINT_HIGH, h);
					}
					if (self->current_way->target3 != world)
					{
						if (printSeparator) sprint(self, PR_PRINT_HIGH, " , ");
						printSeparator = PR_TRUE;
						h = ftos(self->current_way->target3->count);
						if (self->current_way->b_aiflags & PR_AI_TELELINK_3)
							sprint(self, PR_PRINT_HIGH, "Telelink3 to: ");
						else
							sprint(self, PR_PRINT_HIGH, "Link3 to: ");
						sprint(self, PR_PRINT_HIGH, h);
					}
					if (self->current_way->target4 != world)
					{
						if (printSeparator) sprint(self, PR_PRINT_HIGH, " , ");
						h = ftos(self->current_way->target4->count);
						if (self->current_way->b_aiflags & PR_AI_TELELINK_4)
							sprint(self, PR_PRINT_HIGH, "Telelink4 to: ");
						else
							sprint(self, PR_PRINT_HIGH, "Link4 to: ");
						sprint(self, PR_PRINT_HIGH, h);
					}
					sprint(self, PR_PRINT_HIGH, "\n");
				}
				else
					sprint(self, PR_PRINT_HIGH, "You must select a waypoint, first.\n");
			}
			if (self->impulse == 8) // >> Link Management
			{
				self->b_menu = PR_MENU_LINKS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 9) // >> AI Flag Management
			{
				self->b_menu = PR_MENU_FLAGS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 10) // << Main Menu
			{
				self->b_menu = PR_MENU_MAIN;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
		}
		else if (self->b_menu == PR_MENU_LINKS) // [Link Management]
		{
			if (self->impulse == 1) // Unlink Waypoint
			{
				self->b_menu = PR_MENU_ENTRY_UNLINK_WAY;
				self->b_timeToRedrawWaypointEditorMenu = time;
				self->last_way = self->current_way;
			}
			else if (self->impulse == 2) // Create Link
			{
				self->b_menu = PR_MENU_ENTRY_LINK_WAYS;
				self->b_timeToRedrawWaypointEditorMenu = time;
				self->last_way = self->current_way;
			}
			else if (self->impulse == 3) // Create Telelink
			{
				self->b_menu = PR_MENU_ENTRY_TELELINK_WAYS;
				self->b_timeToRedrawWaypointEditorMenu = time;
				self->last_way = self->current_way;
			}
			else if (self->impulse == 4) // Delete Link
			{
				self->b_menu = PR_MENU_ENTRY_DELETE_LINK;
				self->b_timeToRedrawWaypointEditorMenu = time;
				self->last_way = self->current_way;
			}
			else if (self->impulse == 5) // Create Link X2
			{
				self->b_menu = PR_MENU_ENTRY_CREATE_LINK_X2;
				self->b_timeToRedrawWaypointEditorMenu = time;
				self->last_way = self->current_way;
			}
			else if (self->impulse == 6) // Delete Link X2
			{
				self->b_menu = PR_MENU_ENTRY_DELETE_LINK_X2;
				self->b_timeToRedrawWaypointEditorMenu = time;
				self->last_way = self->current_way;
			}
			else if (self->impulse == 7) // > Make Waypoint
				frik_createBlankWaypointHere(self->origin + self->view_ofs);
			else if (self->impulse == 8) // >> Waypoint Management
			{
				self->b_menu = PR_MENU_WAYPOINTS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 9) // >> AI Flag Management
			{
				self->b_menu = PR_MENU_FLAGS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 10) // << Main Menu
			{
				self->b_menu = PR_MENU_MAIN;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
		}
		else if (self->b_menu == PR_MENU_FLAGS) // [AI Flag Management]
		{
			if (self->current_way != world)
			{
				if (self->impulse == 1) // Door Flag
				{
					if (self->current_way->b_aiflags & PR_AI_DOORFLAG)
						self->current_way->b_aiflags = self->current_way->b_aiflags - (self->current_way->b_aiflags & PR_AI_DOORFLAG);
					else
						self->current_way->b_aiflags = self->current_way->b_aiflags | PR_AI_DOORFLAG;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
				else if (self->impulse == 2) // Precision
				{
					if (self->current_way->b_aiflags & PR_AI_PRECISION)
						self->current_way->b_aiflags = self->current_way->b_aiflags - (self->current_way->b_aiflags & PR_AI_PRECISION);
					else
						self->current_way->b_aiflags = self->current_way->b_aiflags | PR_AI_PRECISION;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
				else if (self->impulse == 3) // Surface for air
				{
					if (self->current_way->b_aiflags & PR_AI_SURFACE)
						self->current_way->b_aiflags = self->current_way->b_aiflags - (self->current_way->b_aiflags & PR_AI_SURFACE);
					else
						self->current_way->b_aiflags = self->current_way->b_aiflags | PR_AI_SURFACE;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
				else if (self->impulse == 4) // Blind mode
				{
					if (self->current_way->b_aiflags & PR_AI_BLIND)
						self->current_way->b_aiflags = self->current_way->b_aiflags - (self->current_way->b_aiflags & PR_AI_BLIND);
					else
						self->current_way->b_aiflags = self->current_way->b_aiflags | PR_AI_BLIND;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
				else if (self->impulse == 5) // Jump
				{
					if (self->current_way->b_aiflags & PR_AI_JUMP)
						self->current_way->b_aiflags = self->current_way->b_aiflags - (self->current_way->b_aiflags & PR_AI_JUMP);
					else
						self->current_way->b_aiflags = self->current_way->b_aiflags | PR_AI_JUMP;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
				else if (self->impulse == 6) // Directional
				{
					if (self->current_way->b_aiflags & PR_AI_DIRECTIONAL)
						self->current_way->b_aiflags = self->current_way->b_aiflags - (self->current_way->b_aiflags & PR_AI_DIRECTIONAL);
					else
						self->current_way->b_aiflags = self->current_way->b_aiflags | PR_AI_DIRECTIONAL;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
				else if (self->impulse == 7) // Rocket/Super Jump
				{
					if (self->current_way->b_aiflags & PR_AI_SUPER_JUMP)
						self->current_way->b_aiflags = self->current_way->b_aiflags - (self->current_way->b_aiflags & PR_AI_SUPER_JUMP);
					else
						self->current_way->b_aiflags = self->current_way->b_aiflags | PR_AI_SUPER_JUMP;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
			}
			if (self->impulse == 9) // >> AI Flags page 2
			{
				self->b_menu = PR_MENU_FLAGS2;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 10) // << Main Menu
			{
				self->b_menu = PR_MENU_MAIN;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
		}
		else if (self->b_menu == PR_MENU_FLAGS2) // [AI Flags pg. 2]
		{
			if (self->current_way != world)
			{
				if (self->impulse == 1) // Difficult
				{
					if (self->current_way->b_aiflags & PR_AI_DIFFICULT)
						self->current_way->b_aiflags = self->current_way->b_aiflags - (self->current_way->b_aiflags & PR_AI_DIFFICULT);
					else
						self->current_way->b_aiflags = self->current_way->b_aiflags | PR_AI_DIFFICULT;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
				else if (self->impulse == 2) // Wait for plat
				{
					if (self->current_way->b_aiflags & PR_AI_PLAT_BOTTOM)
						self->current_way->b_aiflags = self->current_way->b_aiflags - (self->current_way->b_aiflags & PR_AI_PLAT_BOTTOM);
					else
						self->current_way->b_aiflags = self->current_way->b_aiflags | PR_AI_PLAT_BOTTOM;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
				else if (self->impulse == 3) // Ride train
				{
					if (self->current_way->b_aiflags & PR_AI_RIDE_TRAIN)
						self->current_way->b_aiflags = self->current_way->b_aiflags - (self->current_way->b_aiflags & PR_AI_RIDE_TRAIN);
					else
						self->current_way->b_aiflags = self->current_way->b_aiflags | PR_AI_RIDE_TRAIN;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
				else if (self->impulse == 4) // Door flag no open
				{
					if (self->current_way->b_aiflags & PR_AI_DOOR_NO_OPEN)
						self->current_way->b_aiflags = self->current_way->b_aiflags - (self->current_way->b_aiflags & PR_AI_DOOR_NO_OPEN);
					else
						self->current_way->b_aiflags = self->current_way->b_aiflags | PR_AI_DOOR_NO_OPEN;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
				else if (self->impulse == 5) // Ambush
				{
					if (self->current_way->b_aiflags & PR_AI_AMBUSH)
						self->current_way->b_aiflags = self->current_way->b_aiflags - (self->current_way->b_aiflags & PR_AI_AMBUSH);
					else
						self->current_way->b_aiflags = self->current_way->b_aiflags | PR_AI_AMBUSH;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
				else if (self->impulse == 6) // Snipe
				{
					if (self->current_way->b_aiflags & PR_AI_SNIPER)
						self->current_way->b_aiflags = self->current_way->b_aiflags - (self->current_way->b_aiflags & PR_AI_SNIPER);
					else
						self->current_way->b_aiflags = self->current_way->b_aiflags | PR_AI_SNIPER;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
				else if (self->impulse == 7) // Trace Test
				{
					if (self->current_way->b_aiflags & PR_AI_TRACE_TEST)
						self->current_way->b_aiflags = self->current_way->b_aiflags - (self->current_way->b_aiflags & PR_AI_TRACE_TEST);
					else
						self->current_way->b_aiflags = self->current_way->b_aiflags | PR_AI_TRACE_TEST;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
			}
			if (self->impulse == 9) // >> AI Flag Management
			{
				self->b_menu = PR_MENU_FLAGS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 10) // << Main Menu
			{
				self->b_menu = PR_MENU_MAIN;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
		}
		else if (self->b_menu == PR_MENU_BOTS) // [Bot Management]
		{
			if (self->impulse == 1) // Add a Test Bot
			{
				//self.impulse = #IMPULSE_SPAWN_BOT;   // PZ: was 100
				string tempstr, tempstr2;
				float tempnum;
				tempstr = infokey(world, "idealp");
				tempnum = stof(tempstr);
				if (tempnum < max_clients)
				{
					tempnum = tempnum + 1;
					tempstr = ftos(tempnum);
					tempstr2 = strcat("localinfo idealp ", tempstr);
					tempstr2 = strcat(tempstr2, "\n");
					localcmd(tempstr2);
					sprint(self, PR_PRINT_HIGH, "Set localinfo idealp to ", tempstr, "\n");
				}
			}
			else if (self->impulse == 2) // Order Test Bot here
			{
				sprint(self, PR_PRINT_HIGH, "Ordering bots here...\n"); // PZ
				oldSelf = self;
				self = player_head;
				while (self != world)
				{
					if (!self->ishuman)
					{
						frik_addToTargetStack(oldSelf);
						frik_findRouteToObject(oldSelf, PR_TRUE); // TRUE == `direct`
						frik_botSay("On my way!\n");            // PZ
						//self = world;
					}   // PZ: Commented out the above and below lines. Why was I only doing this for the first bot?
					//else
						self = self->_next;
				}
				self = oldSelf;
			}
			else if (self->impulse == 3) // Remove Test Bot
			{
				//self.impulse = #IMPULSE_REMOVE_BOT;  // PZ: was 102
				string tempstr, tempstr2;
				float tempnum;
				tempstr = infokey(world, "idealp");
				tempnum = stof(tempstr);
				if (tempnum > 0)
				{
					tempnum = tempnum - 1;
					tempstr = ftos(tempnum);
					tempstr2 = strcat("localinfo idealp ", tempstr);
					tempstr2 = strcat(tempstr2, "\n");
					localcmd(tempstr2);
					sprint(self, PR_PRINT_HIGH, "Set localinfo idealp to ", tempstr, "\n");
				}
			}
			else if (self->impulse == 4) // Stop Test Bot
			{
				oldSelf = self;
				self = player_head;
				while (self != world)
				{
					if (!self->ishuman)
					{
						self->target1 = self->target2 = self->target3 = self->target4 = world;
						route_table = world;
					}
					self = self->_next;
				}
				self = oldSelf;
			}
			else if (self->impulse == 5) // Teleport Bot here
			{
				if (self->current_way != world)
				{
					sprint(self, PR_PRINT_HIGH, "Teleporting bots to selected waypoint...\n");
					oldSelf = self;
					self = player_head;
					while (self != world)
					{
						if (!self->ishuman)
						{
							// PZ: added these
							self->b_aiflags = 0;
							frik_clearRouteForSelf();
							self->target1 = self->target2 = self->target3 = self->target4 = self->enemy = world;
							self->last_way = self->current_way = world;
							self->velocity = V({0, 0, 0});
							// PZ: END
							setorigin(self, oldSelf->current_way->origin);
							self->angles = oldSelf->angles;
							self->v_angle = self->b_commandedViewAngle = oldSelf->v_angle;
						}
						self = self->_next;
					}
					self = oldSelf;
				}
				else
					sprint(self, PR_PRINT_HIGH, "Select a waypoint, first.\n");
			}
			else if (self->impulse == 6) // Teleport to Way No.
			{
				self->b_menu = PR_MENU_ENTRY_TELEPORT_TO_WAY;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 10) // << Main Menu
			{
				self->b_menu = PR_MENU_MAIN;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
		}
		else if (self->b_menu == PR_MENU_WAYLIST) // [Waylist Management]
		{
			if (self->impulse == 1) // Delete ALL Waypoints
			{
				self->b_menu = PR_MENU_ENTRY_DELETE_ALL_WAYS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			// PZ: This is now "Copy Waypoints". This was the Dump Waypoints option.
			else if (self->impulse == 2) // >> Copy Waypoints
			{
				/*if (dump_mode == 0)
					{}//DumpWaypoints(); // PZ: this is the old .way format
				else if (dump_mode == 1)
					frik_printAllWaypointsInQuakeCFormat();
				else if (dump_mode == 2)
					frik_printAllWaypointsInBSPFormat();*/

				// initialize copy function
				entity e = spawn();
				//e.movetype = #MOVETYPE_NONE;
				e->classname = "waypoint_editor_map_center";
				//setmodel(e, "progs/lavaball.mdl");
				//setmodel(e, "progs/s_explod.spr");
				setmodel(e, "progs/flame.mdl");
				//e.frame = 1;
				setorigin(e, b_mapCenter);  // variable should always initialize to '0 0 0'
				//ambientsound(e.origin, "ambience/fire1.wav", 0.5, #ATTN_NORM); // won't work
				e->effects = PR_EF_BRIGHTLIGHT | PR_EF_RED;
				//e.avelocity_x = 100;
				//e.avelocity_y = 200;
				//e.avelocity_z = 100;

				// this isn't working
				/*e.movetype = #MOVETYPE_NONE;
				e.velocity = '0 0 0';
				e.touch = SUB_Null;
				setmodel(e, "progs/s_explod.spr");
				e.solid = #SOLID_NOT;
				s_explode1();*/

				// explosions make boom noise (NO)
				/*WriteByte(#MSG_BROADCAST, #SVC_TEMPENTITY);
				WriteByte(#MSG_BROADCAST, #TE_EXPLOSION);
				WriteCoord(#MSG_BROADCAST, e.origin_x);
				WriteCoord(#MSG_BROADCAST, e.origin_y);
				WriteCoord(#MSG_BROADCAST, e.origin_z);
				multicast(e.origin, #MULTICAST_PHS);*/

				self->b_menu = PR_MENU_COPY_WAYS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 3) // Check For Errors
			{
				t = way_head;
				while (t != world)
				{
					// check for no links
					if ((t->target1 == world) && (t->target2 == world) && (t->target3 == world) && (t->target4 == world))
					{
						sprint(self, PR_PRINT_HIGH, "Waypoint ");
						h = ftos(t->count);
						sprint(self, PR_PRINT_HIGH, h);
						sprint(self, PR_PRINT_HIGH, " has no outbound links\n");
					}
					// check for links to itself
					if ((t->target1 == t) || (t->target2 == t) || (t->target3 == t) || (t->target4 == t))
					{
						sprint(self, PR_PRINT_HIGH, "Waypoint ");
						h = ftos(t->count);
						sprint(self, PR_PRINT_HIGH, h);
						sprint(self, PR_PRINT_HIGH, " links to itself (?)\n");
					}
					t = t->_next;
				}
				sprint(self, PR_PRINT_HIGH, "Error check complete\n");
			}
			else if (self->impulse == 4) // Save Waypoints
			{
				// FIXME: detect engine feature and skip prompt (PZ NOTE: Huh?)
				// PZ: Don't do the warning about needing the file access engine mod. I added the file access stuff.
				// PZ: Menu 17 is now an "Are you sure?" screen.
				self->b_menu = PR_MENU_ENTRY_SAVE_WAYS;
				self->b_timeToRedrawWaypointEditorMenu = time;
				//frik_saveAllWaypointsInWayFormat();              // PZ: this is done in menu 17.
			}
			// PZ: This was "Dynamic Mode" option.
			else if (self->impulse == 5) // Dynamic Mode
			{
				/*if (waypoint_mode == PR_WM_EDITOR_DYNAMIC)
					waypoint_mode = PR_WM_EDITOR;
				else
					waypoint_mode = PR_WM_EDITOR_DYNAMIC;*/
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			// PZ: This was "Dynamic Link" option.
			else if (self->impulse == 6) // Dynamic Link
			{
				/*if (waypoint_mode == PR_WM_EDITOR_DYNLINK)
					waypoint_mode = PR_WM_EDITOR;
				else
					waypoint_mode = PR_WM_EDITOR_DYNLINK;*/
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			// PZ: This was "WAY output" option.
			else if (self->impulse == 7) // WAY output
			{
				dump_mode = 0;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			// PZ: This was "QC output" option.
			else if (self->impulse == 8) // QC output
			{
				//dump_mode = 1;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			// PZ: This was "BSP ents output" option.
			else if (self->impulse == 9) // BSP ents output
			{
				//dump_mode = 2;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 10) // << Main Menu
			{
				self->b_menu = PR_MENU_MAIN;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
		}
		else if (self->b_menu == PR_MENU_ENTRY_LINK_WAYS) // [Link Ways]
		{
			if (self->impulse == 1) // ok
			{
				if (self->current_way != world)
				{
					if (!frik_linkWaypointToWaypoint(self->last_way, self->current_way))
						sprint(self, PR_PRINT_HIGH, "Unable to link them\n");
					self->b_menu = PR_MENU_LINKS;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
			}
			else if (self->impulse == 2) // cancel
			{
				self->b_menu = PR_MENU_LINKS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
		}
		else if (self->b_menu == PR_MENU_ENTRY_TELELINK_WAYS) // [Telelink Ways]
		{
			if (self->impulse == 1) // ok
			{
				if (self->current_way != world)
				{
					if (!frik_telelinkWaypointToWaypoint(self->last_way, self->current_way))
						sprint(self, PR_PRINT_HIGH, "Unable to link them\n");
					self->b_menu = PR_MENU_LINKS;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
			}
			else if (self->impulse == 2) // cancel
			{
				self->b_menu = PR_MENU_LINKS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
		}
		else if (self->b_menu == PR_MENU_ENTRY_DELETE_LINK) // [Delete Link]
		{
			if (self->impulse == 1) // ok
			{
				if (self->current_way != world)
				{
					frik_unlinkWaypointToWaypoint(self->last_way, self->current_way);
					self->b_menu = PR_MENU_LINKS;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
			}
			else if (self->impulse == 2) // cancel
			{
				self->b_menu = PR_MENU_LINKS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
		}
		else if (self->b_menu == PR_MENU_ENTRY_CREATE_LINK_X2) // [Create Link X2]
		{
			if (self->impulse == 1) // ok
			{
				if (self->current_way != world)
				{
					if (!frik_linkWaypointToWaypoint(self->last_way, self->current_way))
						sprint(self, PR_PRINT_HIGH, "Unable to link 1st to 2nd.\n");
					if (!frik_linkWaypointToWaypoint(self->current_way, self->last_way))
						sprint(self, PR_PRINT_HIGH, "Unable to link 2nd to 1st.\n");
					self->b_menu = PR_MENU_LINKS;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
			}
			else if (self->impulse == 2) // cancel
			{
				self->b_menu = PR_MENU_LINKS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
		}
		else if (self->b_menu == PR_MENU_ENTRY_DELETE_LINK_X2) // [Delete Link X2]
		{
			if (self->impulse == 1) // ok
			{
				if (self->current_way != world)
				{
					frik_unlinkWaypointToWaypoint(self->last_way, self->current_way);
					frik_unlinkWaypointToWaypoint(self->current_way, self->last_way);
					self->b_menu = PR_MENU_LINKS;
					self->b_timeToRedrawWaypointEditorMenu = time;
				}
			}
			else if (self->impulse == 2) // cancel
			{
				self->b_menu = PR_MENU_LINKS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
		}
		else if (self->b_menu == PR_MENU_ENTRY_DELETE_ALL_WAYS) // [Delete ALL Ways]
		{
			if (self->impulse == 1) // ok
			{
				frik_deleteAllWaypoints();
				self->b_menu = PR_MENU_WAYLIST;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 2) // cancel
			{
				self->b_menu = PR_MENU_WAYLIST;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
		}
		else if (self->b_menu == PR_MENU_ENTRY_TELEPORT_TO_WAY ||                                              // [Teleport to Waypoint]
		         (self->b_menu >= PR_MENU_ENTRY_GO_TO_COORD_X && self->b_menu <= PR_MENU_ENTRY_GO_TO_COORD_Z)) // [Go To Coordinate]
		{
			if (self->impulse == 10)
				self->impulse = 0;
			if (self->b_menu_value == PR_LARGEST_INTEGER) // this marks no entry
				self->b_menu_value = 0;
			self->b_menu_value = self->b_menu_value * 10 + self->impulse; // PZ NOTE: slide previous numbers left, and add the new number to the right
			self->b_timeToRedrawWaypointEditorMenu = time;
		}
		else if (self->b_menu == PR_MENU_ENTRY_DELETE_WAY) // [Delete Waypoint]
		{
			if (self->impulse == 1) // ok
			{
				frik_deleteWaypoint(self->last_way);
				self->b_menu = PR_MENU_WAYPOINTS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 2) // cancel
			{
				self->b_menu = PR_MENU_WAYPOINTS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
		}
		else if (self->b_menu == PR_MENU_ENTRY_UNLINK_WAY) // [Unlink Waypoint]
		{
			if (self->impulse == 1) // ok
			{
				if (self->last_way != world)
				{
					self->last_way->target1 = self->last_way->target2 = self->last_way->target3 = self->last_way->target4 = world;
					self->last_way->b_aiflags = self->last_way->b_aiflags - (self->last_way->b_aiflags & (PR_AI_TELELINK_1 | PR_AI_TELELINK_2 | PR_AI_TELELINK_3 | PR_AI_TELELINK_4)/*15*/);
				}
				self->b_menu = PR_MENU_LINKS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
			else if (self->impulse == 2) // cancel
			{
				self->b_menu = PR_MENU_LINKS;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
		}
		else if (self->b_menu == PR_MENU_ENTRY_SAVE_WAYS) // [Save Waypoints]
		{
			if (self->impulse == 1) // ok
			{
				frik_saveAllWaypointsInWayFormat();     // PZ TODO: Check that bots get along with this correctly, when done while they are in the game.
				setupWaypointsForPathfinding(way_head); // PZ: Makes the pathfinder reinitialize its waypoint data.
			}
			if (self->impulse <= 2) // cancel
			{
				self->b_menu = PR_MENU_WAYLIST;
				self->b_timeToRedrawWaypointEditorMenu = time;
			}
		}
		// PZ: Added "Copy Waypoints" menu. TODO DONE?: Some links aren't getting copied. --> I think they actually are. The client just doesn't get to see everything, I think.
		else if (self->b_menu == PR_MENU_COPY_WAYS) // [Copy Waypoints]
		{
			if      (self->impulse == 1) // Rotated Copy
			{
				b_copyMode = PR_ROTATED_COPY;
			}
			else if (self->impulse == 2) // Mirrored Copy
			{
				b_copyMode = PR_MIRRORED_COPY;
			}
			else if (self->impulse == 3) // Go To Coordinate
			{
				self->b_menu_value = PR_LARGEST_INTEGER;
				self->b_menu = PR_MENU_ENTRY_GO_TO_COORD_X;
			}
			else if (self->impulse == 4) // Face Direction
			{
				if      (b_directionToFace[ENT_TO_NUM(self)-1] == 0) // North  (+Y)
				{
					self->angles = V({0, 90, 0});
					b_directionToFace[ENT_TO_NUM(self)-1] = 1;
				}
				else if (b_directionToFace[ENT_TO_NUM(self)-1] == 1) // East   (+X)
				{
					self->angles = V({0, 0, 0});
					b_directionToFace[ENT_TO_NUM(self)-1] = 2;
				}
				else if (b_directionToFace[ENT_TO_NUM(self)-1] == 2) // South  (-Y)
				{
					self->angles = V({0, -90, 0});
					b_directionToFace[ENT_TO_NUM(self)-1] = 3;
				}
				else // (b_directionToFace[ENT_TO_NUM(self)-1] == 3) // West   (-X)
				{
					self->angles = V({0, 180, 0});
					b_directionToFace[ENT_TO_NUM(self)-1] = 0;
				}
				self->fixangle = 1; // make them actually turn
			}
			else if (self->impulse == 5) // Set Map Center
			{
				b_mapCenter = self->origin + self->view_ofs;
				// We are going to recreate the entity so that we can instantly move it. There is something weird in the engine,
				// where, if you tell something to move far, by setting its origin, it will immediately move, but if you tell it
				// to move a short distance, it will slowly crawl. One of these days I need to study the engine code to see why
				// this happens, and see if there's a better workaround or something.
				entity oldEnt = find(world, "classname", "waypoint_editor_map_center");
				entity e = spawn();
				e->classname = "waypoint_editor_map_center";
				setmodel(e, "progs/flame.mdl");
				setorigin(e, b_mapCenter);
				e->effects = PR_EF_BRIGHTLIGHT | PR_EF_RED;
				if (oldEnt != world) dremove(oldEnt);
			}
			else if (self->impulse == 6) // Toggle Copy Axis
			{
				b_copyAcrossXAxis = !b_copyAcrossXAxis;
			}
			// PZ TODO: Add a confirmation menu that tells them to stand on the side that they want to copy.
			else if (self->impulse == 7) // Perform Copy  (the side being copied is the side `self` is standing on)
			{
				entity wp = way_head;
				float waypointsCopied = 0, stopCopying = PR_FALSE;
				// Remove all waypoints on the side that we are copying to (where `self` is not standing).
				while (wp != world && !stopCopying) // we'll remove /all/ waypoints on the copy-to side, including waypoint copies (unfinalized waypoints)
				{
					if (b_copyAcrossXAxis)
					{
						if      (self->origin[Y] > b_mapCenter[Y]) // we will copy side player is on (+Y relative to center marker)
						{
							if (wp->origin[Y] < b_mapCenter[Y])    // is waypoint on side to be copied to?
								frik_deleteWaypoint(wp);
						}
						else if (self->origin[Y] < b_mapCenter[Y]) // we will copy side player is on (-Y relative to center marker)
						{
							if (wp->origin[Y] > b_mapCenter[Y])    // is waypoint on side to be copied to?
								frik_deleteWaypoint(wp);
						}
						else
						{
							sprint(self, PR_PRINT_HIGH, "Move to the side that you want to make a copy of, and Perform Copy again.\n");
							stopCopying = PR_TRUE; // 'break' doesn't work with QCCX
						}
					}
					else // across Y axis
					{
						if      (self->origin[X] > b_mapCenter[X]) // we will copy side player is on (+X relative to center marker)
						{
							if (wp->origin[X] < b_mapCenter[X])    // is waypoint on side to be copied to?
								frik_deleteWaypoint(wp);
						}
						else if (self->origin[X] < b_mapCenter[X]) // we will copy side player is on (-X relative to center marker)
						{
							if (wp->origin[X] > b_mapCenter[X])    // is waypoint on side to be copied to?
								frik_deleteWaypoint(wp);
						}
						else
						{
							sprint(self, PR_PRINT_HIGH, "Move to the side that you want to make a copy of, and Perform Copy again.\n");
							stopCopying = PR_TRUE; // 'break' doesn't work with QCCX
						}
					}
					wp = wp->_next;
				}
				// Go through the current list of waypoints, and make a copy of each waypoint that is on the half of the map being copied.
				wp = way_head;
				while (wp != world && wp->classname == "waypoint" && !stopCopying) // don't make copies of copies
				{
					if (b_copyAcrossXAxis)
					{
						if      (self->origin[Y] > b_mapCenter[Y]) // copy side player is on (+Y relative to center marker)
						{
							if (wp->origin[Y] > b_mapCenter[Y])    // is waypoint on side to be copied?
								{frik_copyWaypoint(wp); waypointsCopied = waypointsCopied + 1;}
						}
						else if (self->origin[Y] < b_mapCenter[Y]) // copy side player is on (-Y relative to center marker)
						{
							if (wp->origin[Y] < b_mapCenter[Y])    // is waypoint on side to be copied?
								{frik_copyWaypoint(wp); waypointsCopied = waypointsCopied + 1;}
						}
						else
						{
							sprint(self, PR_PRINT_HIGH, "Move to the side that you want to make a copy of, and Perform Copy again.\n");
							stopCopying = PR_TRUE; // 'break' doesn't work with QCCX
						}
					}
					else // across Y axis
					{
						if      (self->origin[X] > b_mapCenter[X]) // copy side player is on (+X relative to center marker)
						{
							if (wp->origin[X] > b_mapCenter[X])    // is waypoint on side to be copied?
								{frik_copyWaypoint(wp); waypointsCopied = waypointsCopied + 1;}
						}
						else if (self->origin[X] < b_mapCenter[X]) // copy side player is on (-X relative to center marker)
						{
							if (wp->origin[X] < b_mapCenter[X])    // is waypoint on side to be copied?
								{frik_copyWaypoint(wp); waypointsCopied = waypointsCopied + 1;}
						}
						else
						{
							sprint(self, PR_PRINT_HIGH, "Move to the side that you want to make a copy of, and Perform Copy again.\n");
							stopCopying = PR_TRUE; // 'break' doesn't work with QCCX
						}
					}
					wp = wp->_next;
				}
				// [After making a copy of each waypoint, we need to go through all the copies and update the links. They all need to be
				// offset by the same value -- the number of real waypoints (non-copies).]
				// count the number of real waypoints
				if (!stopCopying)
				{
					float numberOfRealWaypoints = 0;
					wp = way_head;
					while (wp != world && wp->classname == "waypoint")
					{
						numberOfRealWaypoints = numberOfRealWaypoints + 1;
						wp = wp->_next;
					}
					// go through all the waypoint copies, and copy the links from their corresponding original waypoints
					entity originalWaypoint;
					wp = b_waypointCopyList;
					while (wp != world && wp->classname == "waypoint_copy")
					{
						originalWaypoint = frik_retrieveWaypointByNumber(wp->count - numberOfRealWaypoints);
						// do the links by waypoint number
						if (originalWaypoint->b_pants) wp->b_pants = originalWaypoint->b_pants + numberOfRealWaypoints;
						if (originalWaypoint->b_skill) wp->b_skill = originalWaypoint->b_skill + numberOfRealWaypoints;
						if (originalWaypoint->b_shirt) wp->b_shirt = originalWaypoint->b_shirt + numberOfRealWaypoints;
						if (originalWaypoint->b_frags) wp->b_frags = originalWaypoint->b_frags + numberOfRealWaypoints;
						// now do the links by waypoint entities
						wp->target1 = frik_retrieveWaypointByNumber(wp->b_pants);
						wp->target2 = frik_retrieveWaypointByNumber(wp->b_skill);
						wp->target3 = frik_retrieveWaypointByNumber(wp->b_shirt);
						wp->target4 = frik_retrieveWaypointByNumber(wp->b_frags);
						wp = wp->_next;
					}
				}
				// tell user what was done
				string strtemp = ftos(waypointsCopied);
				sprint(self, PR_PRINT_HIGH, "Copied ", strtemp, " waypoints.\n");
			}
			else if (self->impulse == 8)   // Slide Copies
			{
				if (b_waypointCopyList != world)  // if copies have been made
					self->b_menu = PR_MENU_SLIDE_WAY_COPIES;
				else
					sprint(self, PR_PRINT_HIGH, "You must first perform a copy.\n");
			}
			else if (self->impulse == 9)   // Finalize Copy
			{
				if (b_waypointCopyList != world)  // if copies have been made
				{
					// make the waypoint copies actual waypoints
					entity wp = b_waypointCopyList;
					while (wp != world && wp->classname == "waypoint_copy")
					{
						wp->classname = "waypoint";
						// make sure its associated marker entity is visible, if it has one, because it will stop flashing now
						if (wp->owner != world)
						{
							// TODO DONE: this is only taking into account one player's selected waypoint
							entity player = nextent(world);
							bool isSelectedBySomeone = false;
							while (player->classname == "player")
							{
								if (wp == player->current_way) { isSelectedBySomeone = true;  break; }
								player = nextent(player);
							}
							if (isSelectedBySomeone) setmodel(wp->owner, "progs/s_light.spr");  // big globe (selected)
							else                     setmodel(wp->owner, "progs/s_bubble.spr"); // bubble (not selected)
						}
						wp = wp->_next;
					}
					b_waypointCopyList = world; // now empty
					bprint(PR_PRINT_HIGH, "All waypoint copies are now officially waypoints.\n");
				}
				else
					sprint(self, PR_PRINT_HIGH, "You must first perform a copy.\n");
			}
			else if (self->impulse == 10) // << Waylist Management
			{
				// remove/reset Copy Waypoint related things
				entity prev = world;
				entity e = find(world, "classname", "waypoint_editor_map_center");
				if (e != world) dremove(e);
				// remove any waypoint copies
				if (b_waypointCopyList != world)
				{
					e = b_waypointCopyList;
					while (e != world && e->classname == "waypoint_copy")
					{
						if (e->owner != world) dremove(e->owner); // remove its marker entity, first
						prev = e;
						e = e->_next;
						dremove(prev);
					}
					b_waypointCopyList = world; // now empty
				}
				self->b_menu = PR_MENU_WAYLIST;
			}
			self->b_timeToRedrawWaypointEditorMenu = time;
		}
		// PZ: Added "Copy Waypoints -> Slide" menu
		// PZ TODO DONE: As mentioned above, make a lock so that only one player can use this menu at a time.
		// PZ TODO?: Rename this to "Transform Waypoint Copies", and include the ability to rotate copies.
		else if (self->b_menu == PR_MENU_SLIDE_WAY_COPIES) // [Slide Waypoint Copies]
		{
			// if `self` doesn't have control of the slide menu, then ignore their inputs
			if (self != slideMenuController && self->impulse != 10) self->impulse = 0;

			if      (self->impulse == 1) // Slide Left
			{
				if (b_slideDirection == 1) b_slideDirection = 0;
				else                       b_slideDirection = 1;
			}
			else if (self->impulse == 2) // Slide Right
			{
				if (b_slideDirection == 2) b_slideDirection = 0;
				else                       b_slideDirection = 2;
			}
			else if (self->impulse == 3) // Slide Forward
			{
				if (b_slideDirection == 3) b_slideDirection = 0;
				else                       b_slideDirection = 3;
			}
			else if (self->impulse == 4) // Slide Back
			{
				if (b_slideDirection == 4) b_slideDirection = 0;
				else                       b_slideDirection = 4;
			}
			else if (self->impulse == 5) // Slide Speed
			{
				if (b_slideSpeed >= 2) b_slideSpeed = 0;
				else                   b_slideSpeed++;
			}
			else if (self->impulse == 10) // << Copy Waypoints
			{
				if (self == slideMenuController) b_slideDirection = 0;
				self->b_menu = PR_MENU_COPY_WAYS;
			}
			self->b_timeToRedrawWaypointEditorMenu = time;
		}
		// we've handled the impulse
		self->impulse = 0;
	}
	// ####################################################
	// MENU DISPLAY
	// ####################################################
	if (time >= self->b_timeToRedrawWaypointEditorMenu)   // PZ: Was worded like the following, which I think is harder to understand: self.b_timeToRedrawWaypointEditorMenu <= time
	{
		if (self->b_menu == PR_MENU_MAIN)
		{
			s1 = S_("^b-- Main Menu --^b\n^[^1^] >> Waypoint Management\n^[^2^] >> Link Management    \n^[^3^] >> AI Flag Management \n^[^4^] >> Bot Management     \n^[^5^] >> Waylist Management \n");
			if (self->movetype == PR_MOVETYPE_NOCLIP)
				s2 = S_("^[^6^] ^[\x85^] ^bNoclip^b            \n");
			else
				s2 = S_("^[^6^] ^[ ^] ^bNoclip^b            \n");

			if (self->flags & PR_FL_GODMODE)
				s3 = S_("^[^7^] ^[\x85^] ^bGodmode^b           \n");
			else
				s3 = S_("^[^7^] ^[ ^] ^bGodmode^b           \n");
			if (self->b_aiflags & PR_AI_HOLD_SELECT)
				s4 = S_("^[^8^] ^[\x85^] Hold Selection    \n");
			else
				s4 = S_("^[^8^] ^[ ^] Hold Selection    \n");
			s5 = S_("^[^9^] Teleport to Way No.   \n^[^0^] Exit Waypoint Editor  \n");
		}
		else if (self->b_menu == PR_MENU_WAYPOINTS)
		{
			s1 = S_("^b-- Waypoint Management --^b\n^[^1^] Move Waypoint         \n^[^2^] Delete Waypoint       \n^[^3^] Make Waypoint         \n^[^4^] Make Way + Link       \n^[^5^] Make Way + Link X2    \n^[^6^] Make Way + Telelink   \n^[^7^] Show waypoint info    \n^[^8^] >> Link Management    \n^[^9^] >> AI Flag Management \n^[^0^] << Main Menu          \n");
		}
		else if (self->b_menu == PR_MENU_LINKS)
		{
			s1 = S_("^b-- Link Management --^b\n^[^1^] Unlink Waypoint       \n^[^2^] Create Link           \n^[^3^] Create Telelink       \n^[^4^] Delete Link           \n^[^5^] Create Link X2        \n^[^6^] Delete Link X2        \n^[^7^] > Make Waypoint       \n^[^8^] >> Waypoint Management\n^[^9^] >> AI Flag Management \n^[^0^] << Main Menu          \n");
		}
		else if (self->b_menu == PR_MENU_FLAGS)
		{
			if (self->current_way->b_aiflags & PR_AI_DOORFLAG)
				s1 = S_("^b-- AI Flag Management --^b\n^[^1^] ^[\x85^] Door Flag         \n");
			else
				s1 = S_("^b-- AI Flag Management --^b\n^[^1^] ^[ ^] Door Flag         \n");

			if (self->current_way->b_aiflags & PR_AI_PRECISION)
				s2 = S_("^[^2^] ^[\x85^] Precision         \n");
			else
				s2 = S_("^[^2^] ^[ ^] Precision         \n");

			if (self->current_way->b_aiflags & PR_AI_SURFACE)
				s3 = S_("^[^3^] ^[\x85^] Surface for air   \n");
			else
				s3 = S_("^[^3^] ^[ ^] Surface for air   \n");

			if (self->current_way->b_aiflags & PR_AI_BLIND)
				s4 = S_("^[^4^] ^[\x85^] Blind mode        \n");
			else
				s4 = S_("^[^4^] ^[ ^] Blind mode        \n");

			if (self->current_way->b_aiflags & PR_AI_JUMP)
				s5 = S_("^[^5^] ^[\x85^] Jump              \n");
			else
				s5 = S_("^[^5^] ^[ ^] Jump              \n");

			if (self->current_way->b_aiflags & PR_AI_DIRECTIONAL)
				s6 = S_("^[^6^] ^[\x85^] Directional       \n");
			else
				s6 = S_("^[^6^] ^[ ^] Directional       \n");

			if (self->current_way->b_aiflags & PR_AI_SUPER_JUMP)
				s7 = S_("^[^7^] ^[\x85^] Rocket/Super Jump \n\n^[^9^] >> AI Flags page 2    \n^[^0^] << Main Menu          \n");
			else
				s7 = S_("^[^7^] ^[ ^] Rocket/Super Jump \n\n^[^9^] >> AI Flags page 2    \n^[^0^] << Main Menu          \n");
		}
		else if (self->b_menu == PR_MENU_FLAGS2)
		{
			if (self->current_way->b_aiflags & PR_AI_DIFFICULT)
				s1 = S_("^b-- AI Flags pg. 2 --^b\n^[^1^] ^[\x85^] Difficult         \n");
			else
				s1 = S_("^b-- AI Flags pg. 2 --^b\n^[^1^] ^[ ^] Difficult         \n");

			if (self->current_way->b_aiflags & PR_AI_PLAT_BOTTOM)
				s2 = S_("^[^2^] ^[\x85^] Wait for plat     \n");
			else
				s2 = S_("^[^2^] ^[ ^] Wait for plat     \n");

			if (self->current_way->b_aiflags & PR_AI_RIDE_TRAIN)
				s3 = S_("^[^3^] ^[\x85^] Ride train        \n");
			else
				s3 = S_("^[^3^] ^[ ^] Ride train        \n");

			if (self->current_way->b_aiflags & PR_AI_DOOR_NO_OPEN)
				s4 = S_("^[^4^] ^[\x85^] Door flag no open \n");
			else
				s4 = S_("^[^4^] ^[ ^] Door flag no open \n");

			if (self->current_way->b_aiflags & PR_AI_AMBUSH)
				s5 = S_("^[^5^] ^[\x85^] Ambush            \n");
			else
				s5 = S_("^[^5^] ^[ ^] Ambush            \n");

			if (self->current_way->b_aiflags & PR_AI_SNIPER)
				s6 = S_("^[^6^] ^[\x85^] Snipe             \n");
			else
				s6 = S_("^[^6^] ^[ ^] Snipe             \n");

			if (self->current_way->b_aiflags & PR_AI_TRACE_TEST)
				s7 = S_("^[^7^] ^[\x85^] Trace Test        \n\n^[^9^] >> AI Flag Management \n^[^0^] << Main Menu          \n");
			else
				s7 = S_("^[^7^] ^[ ^] Trace Test        \n\n^[^9^] >> AI Flag Management \n^[^0^] << Main Menu          \n");

		}
		else if (self->b_menu == PR_MENU_BOTS)
		{
			s1 = S_("^b-- Bot Management --^b\n"
			     "^[^1^] Add a Test Bot        \n"
			     "^[^2^] Order Test Bot here   \n"
			     "^[^3^] Remove Test Bot       \n"
			     "^[^4^] Stop Test Bot         \n"
			     "^[^5^] Teleport Bot here     \n"
			     "^[^6^] Teleport to Way No.   \n"
			     "\n\n\n"
			     "^[^0^] << Main Menu          \n");
		}
		else if (self->b_menu == PR_MENU_WAYLIST)
		{
			// PZ: This is the original menu. I don't need a lot of this stuff.
#ifdef PR_0
			s1 = S_("^b-- Waylist Management --^b\n"
			     "^[^1^] Delete ALL Waypoints \n"
				 "^[^2^] Dump Waypoints       \n"
				 "^[^3^] Check For Errors     \n"
				 "^[^4^] Save Waypoints       \n");

			if (waypoint_mode == PR_WM_EDITOR_DYNAMIC)
				s2 = S_("^[^5^] ^[\x85^] Dynamic Mode     \n"
				     "^[^6^] ^[\x85^] Dynamic Link     \n");
			else if (waypoint_mode == PR_WM_EDITOR_DYNLINK)
				s2 = S_("^[^5^] ^[ ^] Dynamic Mode     \n"
				     "^[^6^] ^[\x85^] Dynamic Link     \n");
			else
				s2 = S_("^[^5^] ^[ ^] Dynamic Mode     \n"
				     "^[^6^] ^[ ^] Dynamic Link     \n");
			if (dump_mode == 0)
				s3 = S_("^[^7^] ^[\x85^] WAY output       \n"
				     "^[^8^] ^[ ^] QC output        \n"
					 "^[^9^] ^[ ^] BSP ents output  \n"
					 "^[^0^] Main Menu            \n");
			else if (dump_mode == 1)
				s3 = S_("^[^7^] ^[ ^] WAY output       \n"
				     "^[^8^] ^[\x85^] QC output        \n"
					 "^[^9^] ^[ ^] BSP ents output  \n"
					 "^[^0^] Main Menu            \n");
			else if (dump_mode == 2)
				s3 = S_("^[^7^] ^[ ^] WAY output       \n"
				     "^[^8^] ^[ ^] QC output        \n"
					 "^[^9^] ^[\x85^] BSP ents output  \n"
					 "^[^0^] Main Menu            \n");
#endif
			s1 = S_("^b-- Waylist Management --^b\n"
			     "^[^1^] Delete ALL Waypoints  \n"
				 "^[^2^] >> Copy Waypoints     \n"
				 "^[^3^] Check For Errors      \n"
				 "^[^4^] Save Waypoints        \n");

			if (waypoint_mode == PR_WM_EDITOR_DYNAMIC)
				s2 = S_("^[^5^] ^[\x85^] ^bDynamic Mode^b      \n"
				     "^[^6^] ^[\x85^] ^bDynamic Link^      \n");
			else if (waypoint_mode == PR_WM_EDITOR_DYNLINK)
				s2 = S_("^[^5^] ^[ ^] ^bDynamic Mode^b      \n"
				     "^[^6^] ^[\x85^] ^bDynamic Link^b      \n");
			else
				s2 = S_("^[^5^] ^[ ^] ^bDynamic Mode^b      \n"
				     "^[^6^] ^[ ^] ^bDynamic Link^b      \n");
			if (dump_mode == 0)
				s3 = S_("^[^7^] ^[\x85^] WAY output        \n"
				     "^[^8^] ^[ ^] ^bQC output^b         \n"
					 "^[^9^] ^[ ^] ^bBSP ents output^b   \n"
					 "^[^0^] << Main Menu          \n");
			else if (dump_mode == 1)
				s3 = S_("^[^7^] ^[ ^] WAY output        \n"
				     "^[^8^] ^[\x85^] ^bQC output^b         \n"
					 "^[^9^] ^[ ^] ^bBSP ents output^b   \n"
					 "^[^0^] << Main Menu          \n");
			else if (dump_mode == 2)
				s3 = S_("^[^7^] ^[ ^] WAY output        \n"
				     "^[^8^] ^[ ^] ^bQC output^b         \n"
					 "^[^9^] ^[\x85^] ^bBSP ents output^b   \n"
					 "^[^0^] << Main Menu          \n");
		}
		else if (self->b_menu == PR_MENU_ENTRY_LINK_WAYS /*8*/)
			s1 = S_("^b-- Link Ways --^b\n\nSelect another way and push 1\nor press 2 to cancel");
		else if (self->b_menu == PR_MENU_ENTRY_TELELINK_WAYS /*9*/)
			s1 = S_("^b-- Telelink Ways --^b\n\nSelect another way and push 1\nor press 2 to cancel");
		else if (self->b_menu == PR_MENU_ENTRY_DELETE_LINK /*10*/)
			s1 = S_("^b-- Delete Link --^b\n\nSelect another way and push 1\nor press 2 to cancel");
		else if (self->b_menu == PR_MENU_ENTRY_CREATE_LINK_X2 /*11*/)
			s1 = S_("^b-- Create Link X2 --^b\n\nSelect another way and push 1\nor press 2 to cancel");
		else if (self->b_menu == PR_MENU_ENTRY_DELETE_LINK_X2 /*12*/)
			s1 = S_("^b-- Delete Link X2 --^b\n\nSelect another way and push 1\nor press 2 to cancel");
		else if (self->b_menu == PR_MENU_ENTRY_DELETE_ALL_WAYS /*13*/)
			s1 = S_("^b-- Delete ALL Ways --^b\n\nAre you sure? Push 1 to go\nthrough with it, 2 to cancel");
		else if (self->b_menu == PR_MENU_ENTRY_TELEPORT_TO_WAY /*14*/)
		{
			s1 = S_("^b-- Teleport to Waypoint --^b\n\nType waypoint number and\npress Enter, or jump, to warp.\n\nWaypoint ");
			s2 = ftos(self->b_menu_value);
		}
		else if (self->b_menu == PR_MENU_ENTRY_DELETE_WAY /*15*/)
			s1 = S_("^b-- Delete Waypoint --^b\n\nAre you sure? Push 1 to go\nthrough with it, 2 to cancel");
		else if (self->b_menu == PR_MENU_ENTRY_UNLINK_WAY /*16*/)
			s1 = S_("^b-- Unlink Waypoint --^b\n\nAre you sure? Push 1 to go\nthrough with it, 2 to cancel");
		// PZ: turned this warning into an "Are you sure?" screen.
		/*else if (self.b_menu == 17)
			s1 = "\b-- Save Waypoints --\b\n\nSave Waypoints requires\nFrikaC's file access tutorial from\nwww.quakesrc.org to be present\n in the engine code.\n Push 1 to continue, 2 to cancel";*/
		else if (self->b_menu == PR_MENU_ENTRY_SAVE_WAYS /*17*/)
			s1 = S_("^b-- Save Waypoints --^b\n\nAre you sure? Push 1 to go\nthrough with it, 2 to cancel");
		// PZ: Added "Copy Waypoints" menu
		else if (self->b_menu == PR_MENU_COPY_WAYS /*18*/)
		{
			    s1 = S_("^b-- Copy Waypoints --^b\n");
			if      (b_copyMode == PR_ROTATED_COPY) // Rotate
				s2 = S_("^[^1^] ^[\x85^] Rotated Copy      \n"
				     "^[^2^] ^[ ^] Mirrored Copy     \n");
			else if (b_copyMode == PR_MIRRORED_COPY) // Mirror
				s2 = S_("^[^1^] ^[ ^] Rotated Copy      \n"
				     "^[^2^] ^[\x85^] Mirrored Copy     \n");
			    s3 = S_("^[^3^] Go To Coordinate      \n");
			if      (b_directionToFace[ENT_TO_NUM(self)-1] == 0)
				s4 = S_("^[^4^] Face Direction: North \n");
			else if (b_directionToFace[ENT_TO_NUM(self)-1] == 1)
				s4 = S_("^[^4^] Face Direction: East  \n");
			else if (b_directionToFace[ENT_TO_NUM(self)-1] == 2)
				s4 = S_("^[^4^] Face Direction: South \n");
			else if (b_directionToFace[ENT_TO_NUM(self)-1] == 3)
				s4 = S_("^[^4^] Face Direction: West  \n");
			if      (b_copyMode == PR_ROTATED_COPY) // Rotate
				s5 = S_("^[^5^] Set Point Of Rotation \n");
			else if (b_copyMode == PR_MIRRORED_COPY) // Mirror
				s5 = S_("^[^5^] Set Position Of Mirror\n");
				s6 = S_("^[^6^] Toggle Copy Axis      \n"
				     "^[^7^] Perform Copy          \n");
			if (b_waypointCopyList != world)  // have copies been made?
				s7 = S_("^[^8^] Slide Copy            \n"
				     "^[^9^] Finalize Copy         \n");
			else
				s7 = S_("^[^8^] ^bSlide Copy^b            \n"
				     "^[^9^] ^bFinalize Copy^b         \n");
			s7 = strcat(s7, S_("^[^0^] << Waylist Management \n"));
		}
		else if (self->b_menu >= PR_MENU_ENTRY_GO_TO_COORD_X /*19*/ && self->b_menu <= PR_MENU_ENTRY_GO_TO_COORD_Z /*21*/) // PZ: added
		{
				s1 = S_("^b-- Go To Coordinate --^b\n"
					 "\n");
			if      (self->b_menu == PR_MENU_ENTRY_GO_TO_COORD_X)
				s2 = "Type X coordinate and\n";
			else if (self->b_menu == PR_MENU_ENTRY_GO_TO_COORD_Y)
				s2 = "Type Y coordinate and\n";
			else if (self->b_menu == PR_MENU_ENTRY_GO_TO_COORD_Z)
				s2 = "Type Z coordinate and\n";
				s3 = "press Enter, or jump.\n"
					 "\n";
			if      (self->b_menu == PR_MENU_ENTRY_GO_TO_COORD_X)
				s4 = "X = ";
			else if (self->b_menu == PR_MENU_ENTRY_GO_TO_COORD_Y)
				s4 = "Y = ";
			else if (self->b_menu == PR_MENU_ENTRY_GO_TO_COORD_Z)
				s4 = "Z = ";
			if (self->b_menu_value != PR_LARGEST_INTEGER)
				s5 = ftos(self->b_menu_value);
				s6 = "\n\n";
			if      (self->b_menu == PR_MENU_ENTRY_GO_TO_COORD_X)
				s7 = "Leave blank to use current X value.\n"
					 "Press fire/shoot button for negative.\n";
			else if (self->b_menu == PR_MENU_ENTRY_GO_TO_COORD_Y)
				s7 = "Leave blank to use current Y value.\n"
					 "Press fire/shoot button for negative.\n";
			else if (self->b_menu == PR_MENU_ENTRY_GO_TO_COORD_Z)
				s7 = "Leave blank to use current Z value.\n"
					 "Press fire/shoot button for negative.\n";
		}
		// PZ: Slide Copy menu
		else if (self->b_menu == PR_MENU_SLIDE_WAY_COPIES /*22*/)
		{
			    s1 = S_("^b-- Slide Waypoint Copies --^b\n");
			if      (b_slideDirection == 1)
				s2 = S_("^[^1^] ^[\x85^] Slide Left        \n");
			else
				s2 = S_("^[^1^] ^[ ^] Slide Left        \n");
			if      (b_slideDirection == 2)
				s3 = S_("^[^2^] ^[\x85^] Slide Right       \n");
			else
				s3 = S_("^[^2^] ^[ ^] Slide Right       \n");
			if      (b_slideDirection == 3)
				s4 = S_("^[^3^] ^[\x85^] Slide Forward     \n");
			else
				s4 = S_("^[^3^] ^[ ^] Slide Forward     \n");
			if      (b_slideDirection == 4)
				s5 = S_("^[^4^] ^[\x85^] Slide Back        \n");
			else
				s5 = S_("^[^4^] ^[ ^] Slide Back        \n");
			if      (b_slideSpeed == 0)
				s6 = S_("^[^5^] Slide Speed: Slow     \n");
			else if (b_slideSpeed == 1)
				s6 = S_("^[^5^] Slide Speed: Medium   \n");
			else if (b_slideSpeed == 2)
				s6 = S_("^[^5^] Slide Speed: Fast     \n");
				s7 = S_("\n\n\n\n"
				     "^[^0^] << Copy Waypoints     \n");
			// Add the current Slide menu controller to the end of the menu.
			if (slideMenuController != world)
				s7 += "\nCurrent Menu Controller:\n" + slideMenuController->netname + "\n";
		}

		// PZ: print the current waypoint number at the bottom of each menu
		if (self->b_menu >= PR_MENU_MAIN && self->b_menu <= PR_MENU_WAYLIST)
		{
			string tempstr;
			s7 = strcat(s7, "\nSelected Waypoint: ");
			if (self->current_way != world)
			{
				tempstr = ftos(self->current_way->count);
				s7 = strcat(s7, tempstr);
				s7 = strcat(s7, " (Entity ");
				tempstr = etos(self->current_way);
				s7 = strcat(s7, tempstr);
				s7 = strcat(s7, ")");
			}
			else s7 = strcat(s7, "None");
		}

		// PZ: push the menu up higher on the screen
		s7 = strcat(s7, "\n\n\n\n\n");

		//frik_big_centerprint(self, s1, s2, s3, s4, s5, s6, s7); // PZ: replaced with the below
		centerprint(self, s1, s2, s3, s4, s5, s6, s7);

		// PZ NOTE: This ishow often menu is printed out (was 1.25). It also gets immediately refreshed whenever the user enters something.
		self->b_timeToRedrawWaypointEditorMenu = time + 0.25;
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

Waypoint Saving to file.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

// bytecount is really iffy
// since there is no true way to determine the length of an ftos
// it uses an approximate of 5
// various other things are guesses, but I don't cut it at the absolute
// max so it should be okay

// PZ NOTE: This is used to print out (dump) .way files, but it's using FrikaC's format, not mine.
//          So, this is currently not used.
/*
void() PrintWaypoint =
{
	local entity t;
	local float needcolon;
	local string h;

	if (self.enemy == world)
		t = way_head;
	else
		t = self.enemy._next;
	if (bytecounter >= 8000)
	{
		bprint(#PRINT_HIGH, "exec maps/");
		bprint(#PRINT_HIGH, mapname);
		bprint(#PRINT_HIGH, ".wa");
		h = ftos(filecount);
		bprint(#PRINT_HIGH, h);
		filecount = filecount + 1;
		bprint(#PRINT_HIGH, "\n// **** break here ****\n");	// 2001-09-10 FrikBot support by FrikaC/Maddes
		bytecounter = 26;
	}
	if (t == world)
	{
		remove(self);
		fixer = world;
		bprint(#PRINT_HIGH, "saved4 3\n// end waypoint dump\n");
		bytecounter = bytecounter + 27;
		return;
	}
	// 2001-09-10 FrikBot support by FrikaC/Maddes  start
	t.origin_x = rint(t.origin_x);
	t.origin_y = rint(t.origin_y);
	t.origin_z = rint(t.origin_z);
	// 2001-09-10 FrikBot support by FrikaC/Maddes  end
	if ((t.origin_x != saved1) || (t.count == 1))
	{
		bprint(#PRINT_HIGH, "saved1 ");
		h = ftos(t.origin_x);
		bprint(#PRINT_HIGH, h);
		saved1 = t.origin_x;
		bytecounter = bytecounter + 12;
		needcolon = #TRUE;
	}
	if ((t.origin_y != saved2) || (t.count == 1))
	{
		if (needcolon)
		{
			bprint(#PRINT_HIGH, "; ");
			bytecounter = bytecounter + 2;
		}
		else
			needcolon = #TRUE;
		bprint(#PRINT_HIGH, "saved2 ");
		h = ftos(t.origin_y);
		bprint(#PRINT_HIGH, h);
		bytecounter = bytecounter + 12;
		saved2 = t.origin_y;
	}
	if ((t.origin_z != saved3) || (t.count == 1))
	{
		if (needcolon)
		{
			bprint(#PRINT_HIGH, "; ");
			bytecounter = bytecounter + 2;
		}
		else
			needcolon = #TRUE;
		bprint(#PRINT_HIGH, "saved3 ");
		h = ftos(t.origin_z);
		bprint(#PRINT_HIGH, h);
		bytecounter = bytecounter + 12;
		saved3 = t.origin_z;
	}
	if (needcolon)	// 2001-09-10 FrikBot support by FrikaC/Maddes
	{
		bytecounter = bytecounter + 1;
		bprint(#PRINT_HIGH, "\n");
		needcolon = #FALSE;
	}
	if ((scratch1 != t.target1.count) || t.count == 1)
	{
		needcolon = #TRUE;
		bprint(#PRINT_HIGH, "scratch1 ");
		bytecounter = bytecounter + 14;
		h = ftos(t.target1.count);
		bprint(#PRINT_HIGH, h);
		scratch1 = t.target1.count;
	}
	if ((scratch2 != t.target2.count) || t.count == 1)
	{
		if (needcolon)
		{
			bprint(#PRINT_HIGH, "; ");
			bytecounter = bytecounter + 2;
		}
		else
			needcolon = #TRUE;
		bprint(#PRINT_HIGH, "scratch2 ");
		bytecounter = bytecounter + 14;
		h = ftos(t.target2.count);
		bprint(#PRINT_HIGH, h);
		scratch2 = t.target2.count;
	}
	if ((scratch3 != t.target3.count) || t.count == 1)
	{
		if (needcolon)
		{
			bprint(#PRINT_HIGH, "; ");
			bytecounter = bytecounter + 2;
		}
		else
			needcolon = #TRUE;
		bprint(#PRINT_HIGH, "scratch3 ");
		bytecounter = bytecounter + 14;
		h = ftos(t.target3.count);
		bprint(#PRINT_HIGH, h);
		scratch3 = t.target3.count;
	}
	if ((scratch4 != t.target4.count) || t.count == 1)
	{
		if (needcolon)
		{
			bprint(#PRINT_HIGH, "; ");
			bytecounter = bytecounter + 2;
		}
		else
			needcolon = #TRUE;
		bprint(#PRINT_HIGH, "scratch4 ");
		bytecounter = bytecounter + 14;
		h = ftos(t.target4.count);
		bprint(#PRINT_HIGH, h);
		scratch4 = t.target4.count;
	}
	if (needcolon)	// 2001-09-10 FrikBot support by FrikaC/Maddes
	{
		bprint(#PRINT_HIGH, "\n");
		needcolon = #FALSE;
	}
	bprint(#PRINT_HIGH, "saved4 ");
	bytecounter = bytecounter + 19;
	if (t.count != 1)
		h = ftos(t.b_aiflags * 4 + 2);
	else
		h = ftos(t.b_aiflags * 4 + 1);
	bprint(#PRINT_HIGH, h);
	bprint(#PRINT_HIGH, "; wait\n");
	self.nextthink = time + 0.01;
	self.enemy = t;
};*/

// to allow for 100+ waypoints, we need to trick the runaway loop counter
/* PZ: This also uses the old .way format. Commented it out.
void() DumpWaypoints =
{
	bytecounter = 50;
	filecount = 1;

	bprint(#PRINT_HIGH, "// ");
	bprint(#PRINT_HIGH, world.message);
	bprint(#PRINT_HIGH, " - maps/");    // 2001-09-10 FrikBot support by FrikaC/Maddes
	bprint(#PRINT_HIGH, mapname);
	bprint(#PRINT_HIGH, ".way\n");
	bprint(#PRINT_HIGH, "// Ways by ");
	bprint(#PRINT_HIGH, self.netname);
	bprint(#PRINT_HIGH, "\n");
	if (!fixer)
	{
		fixer = spawnServerSide();      // PZ: make it a server-side-only entity
		fixer.nextthink = time + 0.01;
		fixer.think = PrintWaypoint;
		fixer.enemy = world;
	}
};*/

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_printWaypointInQuakeCFormat   (PZ: was "PrintQCWaypoint")

Prints out a single waypoint to the console, in QuakeC format.

`self` = the fixer entity

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_printWaypointInQuakeCFormat()
{
	entity t;
	string h;

	if (self->enemy == world)
		t = way_head;
	else
		t = self->enemy->_next;

	if (t == world)
	{
		remove(self);
		fixer = world;
		bprint(PR_PRINT_HIGH, "};\n// End dump\n");   // 2001-09-10 FrikBot support by FrikaC/Maddes
		return;
	}
	bprint(PR_PRINT_HIGH, "\x09frik_createWaypoint(");
	// 2001-09-10 FrikBot support by FrikaC/Maddes  start
	t->origin[X] = rint(t->origin[X]);
	t->origin[Y] = rint(t->origin[Y]);
	t->origin[Z] = rint(t->origin[Z]);
	// 2001-09-10 FrikBot support by FrikaC/Maddes  end
	h = vtos(t->origin);
	bprint(PR_PRINT_HIGH, h);
	bprint(PR_PRINT_HIGH, ", '");
	h = ftos(t->target1->count);
	bprint(PR_PRINT_HIGH, h);
	bprint(PR_PRINT_HIGH, " ");
	h = ftos(t->target2->count);
	bprint(PR_PRINT_HIGH, h);
	bprint(PR_PRINT_HIGH, " ");
	h = ftos(t->target3->count);
	bprint(PR_PRINT_HIGH, h);
	bprint(PR_PRINT_HIGH, "', ");
	h = ftos(t->target4->count);
	bprint(PR_PRINT_HIGH, h);
	bprint(PR_PRINT_HIGH, ", ");
	h = ftos(t->b_aiflags);
	bprint(PR_PRINT_HIGH, h);
	bprint(PR_PRINT_HIGH, ");\n");
	self->nextthink = time + 0.01;
	self->enemy = t;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_printAllWaypointsInQuakeCFormat   (PZ: was "QCDumpWaypoints")

Prints out all waypoints to the console, in QuakeC format.

`self` = the player that triggered this

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_printAllWaypointsInQuakeCFormat()
{
	// PZ: cppreqcc doesn't consider that comments might be inside quotes.. replaced /* */ with //
	bprint(PR_PRINT_HIGH, "// QC Waypoint Dump - src/frikbot/map_");
	bprint(PR_PRINT_HIGH, mapname);
	bprint(PR_PRINT_HIGH, ".qc\n// For instructions please read the\n// readme.html that comes with FrikBot\n\nvoid(vector org, vector bit1, float bit4, float flargs) frik_createWaypoint;\n");
	bprint(PR_PRINT_HIGH, "// Ways by ");
	bprint(PR_PRINT_HIGH, self->netname);
	bprint(PR_PRINT_HIGH, "\n\n");

	bprint(PR_PRINT_HIGH, "void() map_");
	bprint(PR_PRINT_HIGH, mapname);
	bprint(PR_PRINT_HIGH, " =\n{\n");

	if (fixer == world)
	{
		fixer = spawnServerSide();      // PZ: make it a server-side-only entity
		fixer->nextthink = time + 0.01;
		fixer->think = frik_printWaypointInQuakeCFormat;
		fixer->enemy = world;
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_printWaypointInBSPFormat   (PZ: was "PrintBSPWaypoint")

Prints out a single waypoint to the console, in BSP entity format.

`self` = the fixer entity

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_printWaypointInBSPFormat()
{
	entity t;
	string h;

	if (self->enemy == world)
		t = way_head;
	else
		t = self->enemy->_next;

	if (t == world)
	{
		bprint(PR_PRINT_HIGH, "// End dump\n");   // 2001-09-10 FrikBot support by FrikaC/Maddes
		remove(self);
		fixer = world;
		return;
	}
	bprint(PR_PRINT_HIGH, "{\n\"classname\" \"waypoint\"\n\"origin\" \"");
	// 2001-09-10 FrikBot support by FrikaC/Maddes  start
	t->origin[X] = rint(t->origin[X]);
	t->origin[Y] = rint(t->origin[Y]);
	t->origin[Z] = rint(t->origin[Z]);
	// 2001-09-10 FrikBot support by FrikaC/Maddes  end
	h = ftos(t->origin[X]);
	bprint(PR_PRINT_HIGH, h);
	bprint(PR_PRINT_HIGH, " ");
	h = ftos(t->origin[Y]);
	bprint(PR_PRINT_HIGH, h);
	bprint(PR_PRINT_HIGH, " ");
	h = ftos(t->origin[Z]);
	bprint(PR_PRINT_HIGH, h);
	if (t->target1->count)
	{
		bprint(PR_PRINT_HIGH, "\"\n\"b_pants\" \"");
		h = ftos(t->target1->count);
		bprint(PR_PRINT_HIGH, h);
	}
	if (t->target2->count)
	{
		bprint(PR_PRINT_HIGH, "\"\n\"b_skill\" \"");
		h = ftos(t->target2->count);
		bprint(PR_PRINT_HIGH, h);
	}
	if (t->target3->count)
	{
		bprint(PR_PRINT_HIGH, "\"\n\"b_shirt\" \"");
		h = ftos(t->target3->count);
		bprint(PR_PRINT_HIGH, h);
	}
	if (t->target4->count)
	{
		bprint(PR_PRINT_HIGH, "\"\n\"b_frags\" \"");
		h = ftos(t->target4->count);
		bprint(PR_PRINT_HIGH, h);
	}
	if (t->b_aiflags)
	{
		bprint(PR_PRINT_HIGH, "\"\n\"b_aiflags\" \"");
		h = ftos(t->b_aiflags);
		bprint(PR_PRINT_HIGH, h);
	}
	bprint(PR_PRINT_HIGH, "\"\n}\n");
	self->nextthink = time + 0.01;
	self->enemy = t;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_printAllWaypointsInBSPFormat   (PZ: was "BSPDumpWaypoints")

Prints out all waypoints to the console, in BSP entity format.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_printAllWaypointsInBSPFormat()
{
	// PZ: cppreqcc doesn't consider that comments might be inside quotes.. replaced /* */ with //
	bprint(PR_PRINT_HIGH, "// BSP entities Dump - maps/");
	bprint(PR_PRINT_HIGH, mapname);
	bprint(PR_PRINT_HIGH, ".ent\n// For instructions please read the\n// readme.html that comes with FrikBot\n\n");	// 2001-09-10 FrikBot support by FrikaC/Maddes

	if (fixer == world)
	{
		fixer = spawnServerSide(); // PZ: make it a server-side-only entity
		fixer->nextthink = time + 0.01;
		fixer->think = frik_printWaypointInBSPFormat;
		fixer->enemy = world;
	}
}

// FrikaC's file access tutorial...
// PZ: Often put in most if not all of the string functions, already.
float file;

// PZ: I have added the required file access builtin functions. (see defs.qc)
// PZ: Often provided makestr() and delstr(), rather than zone() and unzone(). So, below, everywhere you see these functions, they used to be
// the zone and unzone functions.
// PZ: I'm not using the function aliases that were given in the tutorial. Those aliases were "open", "close", "read", and "write".
// PZ: Not using Frika's .way format because I don't need to do it that way. That way relies on saved# and scratch# variables that I don't need
// to rely on. So, I wrote my own SaveWaypoint(), below.
/*
// This uses the original .way format.
void() SaveWaypoint =
{
	local entity t;
	local float needcolon;
	local string h, j, k, l;

	if (self.enemy == world)
		t = way_head;
	else
		t = self.enemy._next;
	if (bytecounter >= 8000)
	{
		h = "maps/waypoints/";                      // PZ: added the waypoints directory
		j = ftos(filecount);
		k = h = makestr(strcat("maps/waypoints/", mapname)); // PZ: made "zone" "makestr"; added the waypoints directory
		l = h = makestr(strcat(h, ".wa"));          // PZ
		delstr(k);                                  // PZ
		h = makestr(strcat(h, j));                  // PZ
		delstr(l);                                  // PZ
		fputs(file, "exec ");
		fputs(file, h);
		fputs(file, "\n");
		filecount = filecount + 1;
		fclose(file);
		file = fopen(h, #FILE_WRITE);               // PZ: use define
		delstr(h);                                  // PZ
		if (file == -1)
		{
			remove(self);
			fixer = world;
			bprint(#PRINT_HIGH, "Error: failed to save file\n");
			return;
		}
		bytecounter = 0;
	}
	if (t == world)
	{
		remove(self);
		fixer = world;
		fputs(file, "saved4 3\n");
		fclose(file);
		bprint(#PRINT_HIGH, "waypoints saved.\n");
		bytecounter = bytecounter + 27;
		return;
	}
	// 2001-09-10 FrikBot support by FrikaC/Maddes  start
	t.origin_x = rint(t.origin_x);
	t.origin_y = rint(t.origin_y);
	t.origin_z = rint(t.origin_z);
	// 2001-09-10 FrikBot support by FrikaC/Maddes  end
	if ((t.origin_x != saved1) || (t.count == 1))
	{
		fputs(file, "saved1 ");
		h = ftos(t.origin_x);
		fputs(file, h);
		saved1 = t.origin_x;
		bytecounter = bytecounter + 12;
		needcolon = #TRUE;
	}
	if ((t.origin_y != saved2) || (t.count == 1))
	{
		if (needcolon)
		{
			fputs(file, "; ");
			bytecounter = bytecounter + 2;
		}
		else
			needcolon = #TRUE;
		fputs(file, "saved2 ");
		h = ftos(t.origin_y);
		fputs(file, h);
		bytecounter = bytecounter + 12;
		saved2 = t.origin_y;
	}
	if ((t.origin_z != saved3) || (t.count == 1))
	{
		if (needcolon)
		{
			fputs(file, "; ");
			bytecounter = bytecounter + 2;
		}
		else
			needcolon = #TRUE;
		fputs(file, "saved3 ");
		h = ftos(t.origin_z);
		fputs(file, h);
		bytecounter = bytecounter + 12;
		saved3 = t.origin_z;
	}
	if (needcolon)	// 2001-09-10 FrikBot support by FrikaC/Maddes
	{
		bytecounter = bytecounter + 1;
		fputs(file, "\n");
		needcolon = #FALSE;
	}
	if ((scratch1 != t.target1.count) || t.count == 1)
	{
		needcolon = #TRUE;
		fputs(file, "scratch1 ");
		bytecounter = bytecounter + 14;
		h = ftos(t.target1.count);
		fputs(file, h);
		scratch1 = t.target1.count;
	}
	if ((scratch2 != t.target2.count) || t.count == 1)
	{
		if (needcolon)
		{
			fputs(file, "; ");
			bytecounter = bytecounter + 2;
		}
		else
			needcolon = #TRUE;
		fputs(file, "scratch2 ");
		bytecounter = bytecounter + 14;
		h = ftos(t.target2.count);
		fputs(file, h);
		scratch2 = t.target2.count;
	}
	if ((scratch3 != t.target3.count) || t.count == 1)
	{
		if (needcolon)
		{
			fputs(file, "; ");
			bytecounter = bytecounter + 2;
		}
		else
			needcolon = #TRUE;
		fputs(file, "scratch3 ");
		bytecounter = bytecounter + 14;
		h = ftos(t.target3.count);
		fputs(file, h);
		scratch3 = t.target3.count;
	}
	if ((scratch4 != t.target4.count) || t.count == 1)
	{
		if (needcolon)
		{
			fputs(file, "; ");
			bytecounter = bytecounter + 2;
		}
		else
			needcolon = #TRUE;
		fputs(file, "scratch4 ");
		bytecounter = bytecounter + 14;
		h = ftos(t.target4.count);
		fputs(file, h);
		scratch4 = t.target4.count;
	}
	if (needcolon)	// 2001-09-10 FrikBot support by FrikaC/Maddes
	{
		fputs(file, "\n");
		needcolon = #FALSE;
	}
	fputs(file, "saved4 ");
	bytecounter = bytecounter + 19;
	if (t.count != 1)
		h = ftos(t.b_aiflags * 4 + 2);
	else
		h = ftos(t.b_aiflags * 4 + 1);
	fputs(file, h);
	fputs(file, "; wait\n");
	self.nextthink = time + 0.01;
	self.enemy = t;
};
*/

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_saveWaypointInWayFormat   (PZ: was "SaveWaypoint")

PZ: Rewrote this function for my own .way format. Waypoint lists are saved in "maps/waypoints/<mapname>.way".
This saves a single waypoint each run. I modeled this after FrikaC's QuakeC waypoint list format.

QuakeC Format (origin, first three targets, 4th target, flags):
    frik_createWaypoint('-58.0 -692.0 -402.0', '2 14 0', 0, 0);      // example

My .way Format:
//  Origin                   Targets         Flags
    -58.0 -692.0 -402.0      2 14 0 0        0                 // spacing is left up to personal preference

`self` is the "fixer", timer entity.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_saveWaypointInWayFormat()
{
	entity t;
	//float needcolon;
	string h, j, k, l;

	if (self->enemy == world) // means this is the first run
		t = way_head;
	else
		t = self->enemy->_next;

	if (t == world) // means we're at the end of the list of waypoints
	{
		remove(self);
		fixer = world;
		fclose(file);
		bprint(PR_PRINT_HIGH, "Waypoints saved.\n");
		return;
	}

	// PZ: If you want to output any tabs, literally give it a tab character (press tab key). The compiler doesn't understand "\t".
	h = ftos(t->origin[X]); h = padstr(h, 16); fputs(file, h);
	h = ftos(t->origin[Y]); h = padstr(h, 16); fputs(file, h);
	h = ftos(t->origin[Z]); h = padstr(h, 16); fputs(file, h);

	h = ftos(t->target1->count); h = padstr(h, 8); fputs(file, h);
	h = ftos(t->target2->count); h = padstr(h, 8); fputs(file, h);
	h = ftos(t->target3->count); h = padstr(h, 8); fputs(file, h);
	h = ftos(t->target4->count); h = padstr(h, 12); fputs(file, h);

	if (t->count != 1)
		h = ftos(t->b_aiflags/* * 4 + 2*/); // PZ: what is this commented part? it's making ways tele links
	else
		h = ftos(t->b_aiflags/* * 4 + 1*/); // PZ: what is this commented part?
	h = padstr(h, 8);
	fputs(file, h); fputs(file, "\n");

	self->nextthink = time + 0.01;
	self->enemy = t;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_saveAllWaypointsInWayFormat   (PZ: was "SaveWays")

Saves all waypoints to maps/waypoints/<mapname>.way.

To allow for 100+ waypoints, we need to trick the runaway loop counter. So, we use callback .think's.
PZ TODO: Just do it now in a loop. This is likely why saving waypoints is so slow. We don't need to
         worry about the above with C++, unless we lag the current frame, which I can't see this doing,
         especially if we buffer the string before sending it to the file.

PZ: I have added the required file access builtin functions.
PZ: Often provided makestr() and delstr(), rather than zone() and unzone(). So, below, everywhere you
	see these functions, they used to be the zone() and unzone() functions.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_saveAllWaypointsInWayFormat()
{
	string h, f;
	//bytecounter = 50;   // PZ: don't need
	//filecount = 1;      // PZ: don't need
	h = strcat("maps/waypoints/", mapname);  // PZ: added the waypoints directory
	f = h = makestr(h);                      // PZ
	h = strcat(h, ".way");
	delstr(f);                               // PZ
	file = fopen(h, PR_FILE_WRITE);          // PZ: use define
	if (file == -1)
	{
		bprint(PR_PRINT_HIGH, "Error: failed to open .way file for writing\n");
		return;
	}
	// PZ: my own commenting at the top of the .way file
	fputs(file, "// Waypoint list for "); fputs(file, mapname); fputs(file, ", listed in order by waypoint number.\n");
	fputs(file, "// Each entry must be separated by any number of spaces.\n");
	fputs(file, "// Origin (X, Y, Z)                             Targets (1 through 4)               AI Flags\n");
	if (fixer == world)
	{
		fixer = spawnServerSide(); // PZ: make it a server-side-only entity
		fixer->nextthink = time + 0.01;
		fixer->think = frik_saveWaypointInWayFormat;
		fixer->enemy = world;
	}
}

} // END namespace Progs
