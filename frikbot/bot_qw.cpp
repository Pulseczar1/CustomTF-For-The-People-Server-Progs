/*
======================================
FrikBot X (Version 0.10.2) QW
======================================

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


======================================
These installation instructions only apply to QuakeWorld (as does this entire
file). For Normal Quake, please refer to bot.qc

--------------------------------------
To install on a new mod, do all this:
--------------------------------------
Place all included bot*.qc files in the subdirectory "frikbot"
in your source folder, then...

* Add the following lines to progs.src right after the defs.qc line
frikbot/bot_qw.qc
frikbot/bot_way.qc
frikbot/bot_fight.qc
frikbot/bot_ai.qc
frikbot/bot_misc.qc
frikbot/bot_phys.qc
frikbot/bot_move.qc
--------------------------------------
* Comment out the following functions in defs.qc
sound, stuffcmd, sprint, aim, centerprint, setspawnparms
WriteByte, WriteChar, WriteShort, WriteLong, WriteCoord
WriteAngle, WriteString, WriteEntity
--------------------------------------
* Add this to worldspawn() in world.qc, right at the very top, before InitBodyQue();
BotInit();  // FrikBot
--------------------------------------
* add this line to StartFrame() in world.qc, at the very top
BotFrame(); // FrikBot
--------------------------------------
* Add these two lines to PlayerPreThink in client.qc at the very top
if (BotPreFrame()) // FrikBot
	return;
--------------------------------------
* Add this line to PlayerPostThink in client.qc at the very top
if (BotPostFrame()) // FrikBot
	return;
--------------------------------------
* Add the following line to the very top of Client Connect in client.qc
ClientInRankings(); // FrikBot         // PZ: not using
--------------------------------------
* Add these lines to the very top of ClientDisconnect in client.qc
ClientDisconnected(); // FrikBot
--------------------------------------
* Add these lines to the very top of SpectatorConnect in spectate.qc
ClientFixRankings(); // FrikBot        // PZ: not using
--------------------------------------

To install the waypoints:

add

waypoints/map_dm1.qc
waypoints/map_dm2.qc
waypoints/map_dm3.qc
waypoints/map_dm4.qc
waypoints/map_dm5.qc
waypoints/map_dm6.qc

to progs.src, immediately after defs.qc.

To remove these waypoints:

Delete the code in frik_attemptToLoadWaypointsForCurrentMap(), located below.
*/

#include "progs.h"
#include "frikbot/bot_qw.h"
#include "frikbot/bot_way.h"
#include "frikbot/bot_ed.h"
#include "frikbot/bot_misc.h"
#include "frikbot/bot_ai.h"
#include "frikbot/bot_phys.h"
#include "debug.h"
#include "weapons.h"
#include "vote2.h"
#include "admin.h"

namespace Progs {

// PZ: moved frik_attemptToLoadWaypointsForCurrentMap() below declarations

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

Variables and shtuff

bot.qc (or bot_qw.qc) has become pretty much a
header file for all variables in the bot...

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

// -------globals-----
//float	active_clients1, active_clients2;
// PZ NOTE: It's confusing whether this variable is actually just for humans or not, but it doesn't appear that these flags are in use anymore
// anyway. So, I should probably just comment out everywhere that active_clients1 and 2 and b_activeClientSlots is used.
vector  b_activeClientSlots; // PZ: active_clients1 and 2 are now referred to as this; _x is highest value; _z is lowest value (supports all 32 clients)
float	max_clients, real_frametime, averageFrameTime, bot_frameCounter;
float	bot_count, lasttime;
int     b_options;
float	waypoint_mode, dump_mode, b_copyMode, b_copyAcrossXAxis, b_directionToFace[32], b_slideDirection, b_slideSpeed;
vector	b_mapCenter, b_goToCoordinate;
float	waypoints, direct_route, userid;
float	sv_friction, sv_gravity;
float	sv_accelerate, sv_maxspeed, sv_stopspeed;
entity	fixer, b_waypointCopyList;
entity	route_table;
//entity	b_temp1, b_temp2, b_temp3;
entity	player_head, phys_head, way_head, way_tail;  // PZ: I renamed "way_foot" to "way_tail"
float	busy_waypoints;
float	saved_bots, saved_skills1, saved_skills2, current_bots; // PZ: over from bot.qc, but probably won't use the bot restore code
float	saved1, saved2, saved3, scratch1, scratch2, scratch3, scratch4;  // PZ: moved from bot_ed.qc
float	bytecounter, filecount;                                          // PZ: moved from bot_ed.qc
//float	coop/* = 0*/; // hack     // PZ: why make it = 0? That forces it to be immutable (constant).
// PZ: Adding this to provide dynamic bot difficulty. Every time a human kills an enemy bot, this variable is incremented. Every time a human
//     gets killed by a bot (or kills a teammate? not doing this yet), this variable is decremented. When negative, the skill of the bots is
//     decreased. When positive, the skill of the bots is increased.
float	collectiveHumanScore, lastCollectiveHumanScore;
#define PR_COLLECTIVE_HUMAN_SCORE_MEMORY 4 // PZ: this is how large `collectiveHumanScore` is allowed to grow in the positive or negative direction
const int initialBotSkillLevel = PR_SKILL_HARD;
float skillAdjustmentFactor = 1;

//float testArray[10]; // PZ: I have the compiler recognizing this, but it doesn't do anything with the array length.

// PZ: The following variables are for the "votebot" menu. Numbers are -1 when unset. (91 variables)
/* #ifdef DO_NOT_COMPILE_THIS
// count
float vote_idealNumPlayers, vote_percentTeam1, vote_percentTeam2, vote_percentTeam3, vote_percentTeam4, vote_forceHumansToTeam;
// skill level
float vote_botMinSkillOverall,  vote_botMinSkillTeam1,  vote_botMinSkillTeam2,  vote_botMinSkillTeam3,  vote_botMinSkillTeam4;
float vote_botMaxSkillOverall,  vote_botMaxSkillTeam1,  vote_botMaxSkillTeam2,  vote_botMaxSkillTeam3,  vote_botMaxSkillTeam4;
float vote_botLockSkillOverall, vote_botLockSkillTeam1, vote_botLockSkillTeam2, vote_botLockSkillTeam3, vote_botLockSkillTeam4;
// classes
float vote_botScoutsOverall,    vote_botScoutsTeam1,    vote_botScoutsTeam2,    vote_botScoutsTeam3,    vote_botScoutsTeam4;
float vote_botSnipersOverall,   vote_botSnipersTeam1,   vote_botSnipersTeam2,   vote_botSnipersTeam3,   vote_botSnipersTeam4;
float vote_botSoldiersOverall,  vote_botSoldiersTeam1,  vote_botSoldiersTeam2,  vote_botSoldiersTeam3,  vote_botSoldiersTeam4;
float vote_botDemomenOverall,   vote_botDemomenTeam1,   vote_botDemomenTeam2,   vote_botDemomenTeam3,   vote_botDemomenTeam4;
float vote_botMedicsOverall,    vote_botMedicsTeam1,    vote_botMedicsTeam2,    vote_botMedicsTeam3,    vote_botMedicsTeam4;
float vote_botHyguysOverall,    vote_botHyguysTeam1,    vote_botHyguysTeam2,    vote_botHyguysTeam3,    vote_botHyguysTeam4;
float vote_botPyrosOverall,     vote_botPyrosTeam1,     vote_botPyrosTeam2,     vote_botPyrosTeam3,     vote_botPyrosTeam4;
float vote_botSpiesOverall,     vote_botSpiesTeam1,     vote_botSpiesTeam2,     vote_botSpiesTeam3,     vote_botSpiesTeam4;
float vote_botEngineersOverall, vote_botEngineersTeam1, vote_botEngineersTeam2, vote_botEngineersTeam3, vote_botEngineersTeam4;
float vote_botRandomOverall,    vote_botRandomTeam1,    vote_botRandomTeam2,    vote_botRandomTeam3,    vote_botRandomTeam4;
// behavior
float vote_botDefenseOverall, vote_botDefenseTeam1, vote_botDefenseTeam2, vote_botDefenseTeam3, vote_botDefenseTeam4;
float vote_botOffenseOverall, vote_botOffenseTeam1, vote_botOffenseTeam2, vote_botOffenseTeam3, vote_botOffenseTeam4;
float vote_botNeitherOverall, vote_botNeitherTeam1, vote_botNeitherTeam2, vote_botNeitherTeam3, vote_botNeitherTeam4;
float vote_botRandomOverall,  vote_botRandomTeam1,  vote_botRandomTeam2,  vote_botRandomTeam3,  vote_botRandomTeam4;
#endif */

/* PZ: descriptions for the FrikBot's global variables

//active_clients1 : indicates which client slots (1  to 16) are being used by human players (bitflags)
//active_clients2 : indicates which client slots (17 to 32) are being used by human players (bitflags)
b_activeClientSlots : indicates which clients slots are currently in use (bit flags); _x are the highest values; _z are lowest values (1 to 24)
max_clients :
real_frametime : how often the main routine is running
b_options :
lasttime :
waypoint_mode :
dump_mode :
waypoints :
route_table : the bot currently using the route table
direct_route :
player_head : first player in linked list of players (for fast traversal of the player entities)
phys_head : first phys_obj in linked list of phys_obj's
way_head : first waypoint in linked list of waypoints (for fast traversal of the waypoint entities)
way_tail : the last waypoint in the list of waypoints
busy_waypoints : the number of waypoints currently being used for route finding

*/

// PZ TODO: WE NEED TO DEFINE WTF THESE VARIABLES ARE USED FOR, IF IT'S NOT OBVIOUS.
//          ALSO ADDING DEFINE MACROS FOR FIELDS MIGHT BE A GOOD IDEA TO MAKE THE CODE EASIER TO UNDERSTAND.
// ----- entity fields ---
FIELD(".float wallhug, oldkeys, ishuman;")
FIELD(".int   keys;")
FIELD(".float b_frags, b_clientno, b_shirt, b_pants;")
FIELD(".float /*priority, */ai_time, b_sound, missile_speed;") // PZ: `.priority` is never used, and conflicts with CuTF var names
FIELD(".float portal_time/*, switch_wallhug*/;")  // PZ: `.switch_wallhug` seems poorly named. It only controls whether ClientFixRankings() can run.
FIELD(".int b_skill;")
FIELD(".float b_num, b_chattime;")
FIELD(".int   b_aiflags;")
FIELD(".float /*b_entertime,*/ b_userid;") // QW shtuff
FIELD(".float b_menu, b_timeToRedrawWaypointEditorMenu, b_menu_value;")
FIELD(".float b_routeInvalid, b_pursuingEnemy, dyn_time;") // PZ: `b_pursuingEnemy` was `dyn_flags`; `b_routeInvalid` was `route_failed`
FIELD(".float dyn_plat, b_roamingWaypoints;")
FIELD(".entity temp_way/*, phys_obj*/;")  // PZ: We are free from the FrikBot's physics. Now using the real physics done by the C code.
FIELD(".entity target1, target2, target3, target4, b_lastGoalTarget;")
FIELD(".entity _prev, _next;")             // PZ: _prev was "_last"
FIELD(".entity last_way, current_way;")
FIELD(".entity b_entityToAvoid;")  // PZ NOTE: the entity the bot tries to keep distance from (renamed from "avoid")
FIELD(".vector b_commandedViewAngle /*PZ: was b_angle*/, b_dest, mouse_emu, b_obstructionDirection;") // PZ: named "obs_dir" "b_obstructionDirection"
FIELD(".vector movevect, b_dir;")
FIELD(".vector dyn_dest;")
FIELD(".vector punchangle;") // HACK - Don't want to screw with bot_phys
FIELD(".vector b_botRole;")  // PZ: the bot's current role in the game (defense, offense, capper, sniper, engineer, spy, etc.): 72 bit flags
// PZ: I'm going to rewrite all of the bot code in C (or C++), anyway. So, it's not going to hurt to temporarily make a few fields.
FIELD(".float b_botInEnemyFOV;")        // whether the bot is in a location where he can be seen by an enemy
FIELD(".float b_botInEnemyPlayerFOV;")  // whether the bot is in a location where he can be seen by an enemy player
FIELD(".float b_botInEnemyMachineFOV;") // whether the bot is in a location where he can be seen by an enemy machine
// PZ: for waypoint editor
FIELD(".float ed_jumpLastPressed;")
FIELD(".float ed_fireLastPressed;")

/* PZ: descriptions for the FrikBot's fields (rename fields if there is a better name)

### Bots ###
.b_commandedViewAngle = the bot's commanded aiming direction (his actual aiming direction is .v_angle)
.b_sound  = entity recently made a sound if this is greater than `time`
.last_way = the last waypoint that the bot pursued on its route (PZ: WARNING: I MAY HAVE CHANGED THIS.)
.b_routeInvalid = means don't use your current route; compute a new one before following your route
.#b_bot_previousTarget1 = the bot's previous .target1
.#b_bot_weaponCycler = the bot's weapon cycling timer entity, if it currently has one (for weapons that share impulses)

### Waypoints ###
.keys     = whether or not this waypoint is currently being used for route finding
.enemy    = holds the 'route table' route, before it is copied over to .movedir for each bot, like a linked list
.items    = a distance from the waypoint to [something else]
._next    = the waypoint that follows this one in the waypoint list
._prev    = the waypoint preceding this one in the waypoint list
//.b_sound  = waypoint is currently being used to route for a bot if a bot's "client bitflag" is set in this variable
// PZ: using this vector, so that we can store more than 24 bots; _x is the highest float; _z is the lowest float
.movedir  = waypoint is currently being used to route for a bot if a bot's "client bitflag" is set in this variable
.target1  = link 1 (by waypoint entity)
.target2  = link 2 (by waypoint entity)
.target3  = link 3 (by waypoint entity)
.target4  = link 4 (by waypoint entity)
.b_pants  = link 1 (by waypoint number)
.b_skill  = link 2 (by waypoint number)
.b_shirt  = link 3 (by waypoint number)
.b_frags  = link 4 (by waypoint number)
.count    = the number assigned to this waypoint
.owner    = PZ: this waypoint's waypoint marker; the marker's .owner will point back to this waypoint

*/

// PZ: #defines were here.
// PZ: Bot function prototypes were here.

// ----------Commands/Overrides---------
//void frik_sound(entity e, float chan, const string& samp, float vol, float atten) #8
//void(entity client, string s) frik_stuffcmd = #21;            // PZ: not replacing stuffcmd()
//void(entity client, float level, string s) frik_sprint = #24; // PZ: not replacing sprint()
//vector frik_aim(entity e, float speed) #44
//void(entity client, string s) frik_centerprint = #73;         // PZ: not replacing centerprint()
//void frik_setspawnparms(entity e) #78
//void frik_WriteByte(float to, float f) #52
//void frik_WriteChar(float to, float f) #53
//void frik_WriteShort(float to, float f) #54
//void frik_WriteLong(float to, float f) #55
//void frik_WriteCoord(float to, float f) #56
//void frik_WriteAngle(float to, float f) #57
//void frik_WriteString(float to, const string& s) #58
//void frik_WriteEntity(float to, entity s) #59

//----------------------------------------------------------------------------------------------------

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

Function redeclarations. These allow functions
designed to work for clients (sprint(), and so
forth) to mainly not complain when working with
a bot.

Although these shouldn't be needed anymore,
as the bots truly are clients now, if you don't
stop the SZ_ buffer from filling up by disabling
direct messages to the bots, it crashes quake. :-(

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// PZ: The server will support certain stuffcmds for the bot (team, color, etc.), and trash the rest.
/*void(entity	client, string s) stuffcmd =
{
	if (client.ishuman)
		frik_stuffcmd(client, s);
};*/

void setspawnparms(entity e)
{
	if (e->ishuman)
		BuiltIns::setspawnparms(e);   // PZ: was frik_setspawnparms(e);
	else
		SetNewParms();
}

// PZ: I modified the server to check .ishuman before trying to sprint or centerprint, due to issues with variable argument lists ("void(...)").
//        So, no need to wrap sprint() and centerprint().
/*void(entity client, float level, string s) sprint =
{
	if (client.ishuman)
		frik_sprint(client, level, s);
};
void(entity client, string s) centerprint =
{
	if (client.ishuman)
		frik_centerprint(client, s);
};*/
// PZ END

vector aim(entity e, float missileSpeed)
{
	e->missile_speed = missileSpeed;
	return BuiltIns::aim(e, missileSpeed);       // PZ: was frik_aim(e, missileSpeed);
}

void sound(entity e, float chan, const string& samp, float vol, float atten)
{
	BuiltIns::sound(e, chan, samp, vol, atten);  // PZ: was frik_sound(e, chan, samp, vol, atten);
	if (samp == "items/inv3.wav")
		return;
	else if (e->classname == "player")
		e->b_sound = time + 1;             // PZ NOTE: .b_sound is used in client entities to attract bots to them
	else if (other->classname == "player") // PZ NOTE: What is `other` here? Other is probably who caused a sound to happen by making object react.
		other->b_sound = time + 1;
}

// PZ: Engine modifications remove the need for these function overrides.
/*void WriteByte(float to, float f)
{
	if ((to == PR_MSG_ONE) && (msg_entity->ishuman != PR_TRUE))
		return;
	frik_WriteByte(to, f);
}

void WriteChar(float to, float f)
{
	if ((to == PR_MSG_ONE) && (msg_entity->ishuman != PR_TRUE))
		return;
	frik_WriteChar(to, f);
}

void WriteShort(float to, float f)
{
	if ((to == PR_MSG_ONE) && (msg_entity->ishuman != PR_TRUE))
		return;
	frik_WriteShort(to, f);
}

void WriteLong(float to, float f)
{
	if ((to == PR_MSG_ONE) && (msg_entity->ishuman != PR_TRUE))
		return;
	frik_WriteLong(to, f);
}

void WriteCoord(float to, float f)
{
	if ((to == PR_MSG_ONE) && (msg_entity->ishuman != PR_TRUE))
		return;
	frik_WriteCoord(to, f);
}

void WriteAngle(float to, float f)
{
	if ((to == PR_MSG_ONE) && (msg_entity->ishuman != PR_TRUE))
		return;
	frik_WriteAngle(to, f);
}

void WriteString(float to, const string& s)
{
	if ((to == PR_MSG_ONE) && (msg_entity->ishuman != PR_TRUE))
		return;
	frik_WriteString(to, s);
}

void WriteEntity(float to, entity s)
{
	if ((to == PR_MSG_ONE) && (msg_entity->ishuman != PR_TRUE))
		return;
	frik_WriteEntity(to, s);
}*/
// ######################################################################
// END OF BUILT-IN FUNCTION OVERRIDES
// ######################################################################

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_initializeFrikBotModule    (PZ: was "BotInit")

This runs once in worldspawn(), at start-up.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_initializeFrikBotModule()
{
	//entity ent /*, physicsEntity*/;
	//float i;

	// [init bot globals]
	bot_frameCounter = 0;

	// PZ NOTE: I broke this by moving where this function is called.
	// count the max number of client slots (based on the number of entities currently existing??)
	/*ent = nextent(world);  // starting with the first client entity
	max_clients = 0;
	while (ent != world)
	{
		max_clients = max_clients + 1;
		ent = nextent(ent);
	}*/
	// PZ: ..So, do it this way, instead.
	string strtemp;
	strtemp = infokey(world, "maxclients");
	max_clients = stof(strtemp);

	// PZ: We are no longer using the Frikbot's physics. The bots now use the same C code physics that is used on real players.
	// spawn physics entities for the bots' physics (PZ NOTE: this is the entity that detects hits, and used to be the one you saw)
	/*ent = nextent(world);
	physicsEntity = world;
	i = 0;
	while (i < max_clients) // for all client slots (1 to 32)
	{
		// PZ NOTE: WHY DOES HE APPEAR TO BE MAKING phys_head point to the end of the list (the most recent created)?
		// I guess he just builds it like stack.
		phys_head = spawn();
		if (physicsEntity)
			physicsEntity._next = phys_head;
		phys_head._prev = physicsEntity;
		physicsEntity = phys_head;
		ent.phys_obj = phys_head;
		phys_head.classname = "phys_obj";
		phys_head.owner = ent;
		i = i + 1;
		ent = nextent(ent);
	}*/

	frik_attemptToLoadWaypointsForCurrentMap(); // PZ: this reads the waypoints from a .way file, or from compiled QC code
	setupWaypointsForPathfinding(way_head);     // PZ: new A* pathfinding in the C code

	// PZ: for waypoint editor
	//precache_model("progs/s_light.spr");
	//precache_model("progs/s_bubble.spr");
}

void map_dm1(); void map_dm2(); void map_dm3(); void map_dm4(); void map_dm5(); void map_dm6();

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_attemptToLoadWaypointsForCurrentMap     (PZ: was called "bot_map_load")

Loads waypoints from .way files or from within the
QuakeC source code.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// PZ: moved from the top
// PZ: HOW TO MAKE QUAKEC WAYPOINTS: To add a new QuakeC waypoint list, create a waypoint list in-game with 'cmd admin waypoints'. (Warning: set timelimit to 0)
// Use the waypoint editor to dump waypoints as QuakeC. Copy them from the server console to a new waypoint file in <src>/frikbot/waypoints. Add a link to
// that new waypoint function here (directly above). In preprogs.src, make it compile the new source file by adding it to the list with the waypoints. Make
// the newly-waypointed map vote-able by adding it to cpserver.cfg, to the m# list of localinfo variables. Add it to the map rotation by editing fortress/
// cpcycle/ files, and add a <mapname>.cfg file for the new map in cpcycle.
// PZ: I MADE IT SO THAT YOU CAN LOAD AND SAVE .WAY FILES. DO THAT INSTEAD.
void frik_attemptToLoadWaypointsForCurrentMap()
{
	string h, f;        // PZ
	float file;         // PZ
	float foundWayFile; // PZ

	// PZ: first check to see whether we have a .way file for this map in maps/waypoints/
	h = strcat("maps/waypoints/", mapname);
	f = h = makestr(h);       // PZ: makes it a permanent string (I don't understand why f was used in frik_saveAllWaypointsInWayFormat(), but leaving for now)
	h = strcat(h, ".way");
	delstr(f);
	file = fopen(h, PR_FILE_READ);
	if (file == -1)
	{
		localcmd("echo Could not open: ");
		localcmd(h);
		localcmd("\n");
		foundWayFile = 0;
	}
	else
	{
		localcmd("echo Using waypoints in: ");
		localcmd(h);
		localcmd("\n");

		// parse way file and create waypoint entities
		string waypoint;
		float  readALine;
		//waypoint = "go";
		readALine = PR_TRUE;
		//while (waypoint != "")
		while (readALine)
		{
			waypoint = fgets(file);
			waypoint = makestr(waypoint);
			string token;
			vector wpOrigin = V({0, 0, 0}), first3Targets = V({0, 0, 0});
			float  t4 = 0;
			float  wpFlags = 0;

			// origin_x
			token = strtok(waypoint, " \x09"); // delimiters are space and tab (can't use \t)
			if (token != "" && token != "//")
			{
				wpOrigin[X] = stof(token);

				// origin_y
				token = strtok(waypoint, " \x09"); // delimiters are space and tab (can't use \t)
				if (token != "" && token != "//")
					wpOrigin[Y] = stof(token);

				// origin_z
				token = strtok(waypoint, " \x09"); // delimiters are space and tab (can't use \t)
				if (token != "" && token != "//")
					wpOrigin[Z] = stof(token);

				// target 1
				token = strtok(waypoint, " \x09"); // delimiters are space and tab (can't use \t)
				if (token != "" && token != "//")
					first3Targets[X] = stof(token);

				// target 2
				token = strtok(waypoint, " \x09"); // delimiters are space and tab (can't use \t)
				if (token != "" && token != "//")
					first3Targets[Y] = stof(token);

				// target 3
				token = strtok(waypoint, " \x09"); // delimiters are space and tab (can't use \t)
				if (token != "" && token != "//")
					first3Targets[Z] = stof(token);

				// target 4
				token = strtok(waypoint, " \x09"); // delimiters are space and tab (can't use \t)
				if (token != "" && token != "//")
					t4 = stof(token);

				// flags
				token = strtok(waypoint, " \x09"); // delimiters are space and tab (can't use \t)
				if (token != "" && token != "//")
					wpFlags = stof(token);

				// can use frik_createBlankWaypointHere() or frik_createWaypoint() to make waypoints; frik_createWaypoint() would probably be best
				frik_createWaypoint(wpOrigin, first3Targets, t4, wpFlags);

				/*local string temp, temp2;
				temp = "made waypoint: ";
				temp2 = vtos(wpOrigin);
				temp = strcat(temp, temp2); temp = strcat(temp, " ");
				temp2 = vtos(first3Targets);
				temp = strcat(temp, temp2); temp = strcat(temp, " ");
				temp2 = ftos(t4);
				temp = strcat(temp, temp2); temp = strcat(temp, " ");
				temp2 = ftos(wpFlags);
				temp = strcat(temp, temp2); temp = strcat(temp, "\n");
				bprint(2, temp);*/
			}

			/*while (token != "" && token != "//") // stop at any "//" for comments
			{
				bprint(2, token, " ");
				token = strtok(waypoint, " 	"); // delimiters are space and tab (can't use \t)
			}*/
			//bprint(2, "\n");
			if (waypoint == "") readALine = PR_FALSE;
			delstr(waypoint);
		}
		fclose(file);
		frik_updateWaypointLinkPointers(world); // PZ: we are now doing this in the same frame as when the waypoints are created, all at once
		foundWayFile = 1;
		waypoint_mode = PR_WM_LOADED;    // PZ: moved this here
	}
	// PZ: END

	// If it couldn't load a .way file, it will look for QuakeC waypoints.
	// place your qc loaded waypoints here
	if (!foundWayFile)
	{
		float foundQcWaypoints;

		foundQcWaypoints = PR_FALSE;

		// PZ: Not using QC waypoints anymore, for right now, at least.
		/*if (mapname == "dm1")
			{map_dm1(); foundQcWaypoints = PR_TRUE;}
		else if (mapname == "dm2")
			{map_dm2(); foundQcWaypoints = PR_TRUE;}
		else if (mapname == "dm3")
			{map_dm3(); foundQcWaypoints = PR_TRUE;}
		else if (mapname == "dm4")
			{map_dm4(); foundQcWaypoints = PR_TRUE;}
		else if (mapname == "dm5")
			{map_dm5(); foundQcWaypoints = PR_TRUE;}
		else if (mapname == "dm6")
			{map_dm6(); foundQcWaypoints = PR_TRUE;}*/

		// PZ: These are now old.
		/*else if (mapname == "sq1")
			{map_sq1(); foundQcWaypoints = #TRUE;}
		else if (mapname == "2farms")
			{map_2farms(); foundQcWaypoints = #TRUE;}
		else if (mapname == "canyon1")
			{map_canyon1(); foundQcWaypoints = #TRUE;}
		else if (mapname == "rock2")
			{map_rock2(); foundQcWaypoints = #TRUE;}*/

		if (foundQcWaypoints)
		{
			frik_updateWaypointLinkPointers(world); // PZ: we are now doing this in the same frame as when the waypoints are created, all at once
			waypoint_mode = PR_WM_LOADED;    // PZ: moved this here
			localcmd("echo QuakeC waypoints loaded\n"); // PZ: moved here
		}
	}

	// PZ: for when there are no waypoints
	//else // shortcut for now
		//waypoint_mode = #WM_DYNAMIC; // Can't make waypoints for maps until I get it to stop creating waypoints when there are no bots in the server.
	// PZ: After taking this out (above), they still seem to dynamically path. They don't just sit in the resupply room. They also don't make so many damn
	//     waypoints that the server crashes. They do act a little stupid, but they seem to function better than they did with this in there. I verified
	//     that it is making waypoints.

	/*local string tempstr;
	tempstr = ftos(waypoint_mode);
	bprint(#PRINT_HIGH, "waypoint_mode = ", tempstr, "\n");*/
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

Stuff mentioned up top
it just links the bot into the mod

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_isClientSlotActive        (PZ: was called "ClientIsActive")

This determines whether a client slot is active.
`active_clients1` holds bitflags for the first
16 clients. `active_clients2` holds bitflags
for the last 16 clients (total of 32).

If the bitflag for `clientNumber` is set, it returns
true. Else, it returns false.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// PZ TODO: determine whether this is needed (SEARCHED FOR IT, AND IT DOESN'T APPEAR TO BE USED)
float frik_isClientSlotActive(float clientNumber)
{
	/*if (clientNumber >= 17) // for clients 17 to 32
	{
		if (active_clients2 & getVectorBitFlagGivenBitNumber(clientNumber - 16))
			return #TRUE;
		else
			return #FALSE;
	}
	else // for clients 1 to 16
	{
		if (active_clients1 & getVectorBitFlagGivenBitNumber(clientNumber))
			return #TRUE;
		else
			return #FALSE;
	}*/
	// PZ: using the vector, `b_activeClientSlots`, instead, to keep things neater
	return isVectorBitFlagSet(clientNumber, V({0, 0, 0}), b_activeClientSlots);
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_getBitFlagForClient  (PZ: was called "ClientBitFlag")

Generates a bitflag between bin[1] and bin[10000000 00000000 00000000]
for client, `clientNumber`.

PZ NOTE: This doesn't by itself support 32 bots. A float can only hold up to 24 bits.
HOWEVER, if the `clientNumber` is adjusted, as it sometimes is in the code, it can
generate correct bits, if you handle the answer appropriately.

It is recommended that you don't give this function a client number greater than
16. For client numbers greater than 16, pass them to this function subtracted
by 16.

PZ FIXME: WHY IS THIS ALLOWING A CLIENT NUMBER OF 0?

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
/*float(float clientNumber) frik_getBitFlagForClient =
{
	local float bitflag;
	bitflag = 1;
	while (clientNumber > 0)
	{
		bitflag = bitflag * 2;
		clientNumber = clientNumber - 1;
	}
	return bitflag;
};*/
// PZ: Modified to directly support 32 clients. Client bit flags are stored in a vector,
// with the high values in _x, and the low values in _y. This also correctly starts with
// the first bit. The above function seemed to not use the first bit, using only 23 bits.
/*vector (float clientNumber) frik_getBitFlagForClient =
{
	local vector bitFlags;

	if (clientNumber < 1 || clientNumber > 32) return '0 0 0';

	if (clientNumber > 24)
	{
		clientNumber = clientNumber - 24;
		bitFlags_x = 1;
		while (clientNumber > 1)
		{
			bitFlags_x = bitFlags_x * 2;
			clientNumber = clientNumber - 1;
		}
	}
	else
	{
		bitFlags_y = 1;
		while (clientNumber > 1)
		{
			bitFlags_y = bitFlags_y * 2;
			clientNumber = clientNumber - 1;
		}
	}

	return bitFlags;
};*/

// GENERIC
// PZ: For vector bit flags. Allows for 24*3 flags. Returns a vector holding the set bit, given the number of that bit.
//     If you give it 49, for example, it will return a vector of value '1 0 0'. The first bit of the X component will be set.
//     X is the most significant float. Z is the least significant float.
vector getVectorBitFlagGivenBitNumber(float bitNumber)
{
	vector bitFlags = V({0, 0, 0});

	if (bitNumber < 1 || bitNumber > 72) return V({0, 0, 0});  // 72 bits in a vector

	if (bitNumber > 48)
	{
		bitNumber = bitNumber - 48;
		bitFlags[X] = 1;
		while (bitNumber > 1)
		{
			bitFlags[X] = bitFlags[X] * 2;
			bitNumber = bitNumber - 1;
		}
	}
	else if (bitNumber > 24)
	{
		bitNumber = bitNumber - 24;
		bitFlags[Y] = 1;
		while (bitNumber > 1)
		{
			bitFlags[Y] = bitFlags[Y] * 2;
			bitNumber = bitNumber - 1;
		}
	}
	else
	{
		bitFlags[Z] = 1;
		while (bitNumber > 1)
		{
			bitFlags[Z] = bitFlags[Z] * 2;
			bitNumber = bitNumber - 1;
		}
	}

	return bitFlags;
}

// PZ:
/*vector (float clientNumber, vector clientBitFlags) frik_addClientToBitFlags =
{
	local vector temp;
	temp = frik_getBitFlagForClient(clientNumber);
	clientBitFlags_x = clientBitFlags_x | temp_x;
	clientBitFlags_y = clientBitFlags_y | temp_y;
	return clientBitFlags;
};*/

// GENERIC
// PZ: For vector bit flags. Allows for 24*3 flags. Sets bit `bitNumber` in `bitFlags`, and returns `bitFlags`.
vector setVectorBitFlag(float bitNumber, vector bitFlags)
{
	vector temp;
	temp = getVectorBitFlagGivenBitNumber(bitNumber);
	bitFlags[X] = (int)bitFlags[X] | (int)temp[X];
	bitFlags[Y] = (int)bitFlags[Y] | (int)temp[Y];
	bitFlags[Z] = (int)bitFlags[Z] | (int)temp[Z];
	return bitFlags;
}

// PZ:
/*vector (float clientNumber, vector clientBitFlags) frik_removeClientFromBitFlags =
{
	local vector temp;
	temp = frik_getBitFlagForClient(clientNumber);
	clientBitFlags_x = clientBitFlags_x - (clientBitFlags_x & temp_x);
	clientBitFlags_y = clientBitFlags_y - (clientBitFlags_y & temp_y);
	return clientBitFlags;
};*/

// GENERIC
// PZ: For vector bit flags. Allows for 24*3 flags. Clears bit `bitNumber` in `bitFlags`, and returns `bitFlags`.
vector clearVectorBitFlag(float bitNumber, vector bitFlags)
{
	vector temp;
	temp = getVectorBitFlagGivenBitNumber(bitNumber);
	bitFlags[X] = bitFlags[X] - ((int)bitFlags[X] & (int)temp[X]);
	bitFlags[Y] = bitFlags[Y] - ((int)bitFlags[Y] & (int)temp[Y]);
	bitFlags[Z] = bitFlags[Z] - ((int)bitFlags[Z] & (int)temp[Z]);
	return bitFlags;
}

// PZ: Checks to see whether a bot client is in `clientBitFlags`. If you give it a valid `clientNumber`,
// it will use that. Otherwise, it will attempt to use `clientFlag`.
/*float (float clientNumber, vector clientFlag, vector clientBitFlags) frik_isClientInBitFlags =
{
	local vector temp;
	if (clientNumber >= 1 && clientNumber <= 32) temp = frik_getBitFlagForClient(clientNumber);
	else                                         temp = clientFlag;
	return (clientBitFlags_x & temp_x || clientBitFlags_y & temp_y);
};*/

// GENERIC
// PZ: For vector bit flags. Allows for 24*3 flags. Checks to see whether flag is set in `bitFlags`.
// If you give it a valid `bitNumber`, it will use that. Otherwise, it will attempt to use `bitFlag`.
// Examples: isVectorBitFlagSet(30, '0 0 0', flags) => Will check whether the 6th bit in flags_y is set.
//           isVectorBitFlagSet(0, '0 32 0', flags) => Will check whether the 6th bit in flags_y is set.
bool isVectorBitFlagSet(float bitNumber, const vector& bitFlag, const vector& bitFlags)
{
	vector temp;
	if (bitNumber >= 1 && bitNumber <= 72) temp = getVectorBitFlagGivenBitNumber(bitNumber);
	else                                   temp = bitFlag;
	return ((int)bitFlags[X] & (int)temp[X] || (int)bitFlags[Y] & (int)temp[Y] || (int)bitFlags[Z] & (int)temp[Z]);
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_getNextAvailableClientNumber  (PZ: was called "ClientNextAvailable")

Returns the next available client number. If one isn't
available, it returns -1.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_getNextAvailableClientNumber()
{
	float clientNumber;
	// I want to do this top down, but QW won't let me
	clientNumber = 0;
	while (clientNumber < max_clients)
	{
		clientNumber = clientNumber + 1;

		if (!frik_isClientSlotActive(clientNumber))
			return clientNumber;
	}
	return -1; // all clients are active; no client number is available
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_            (PZ: was called "ClientFixRankings")

`self` = current bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// PZ: I think the server C code is handling all this. Commenting out.
//     Yeah, this isn't used.
/*void() ClientFixRankings =
{
	local float clientNumber;
	if (self.switch_wallhug > time) // PZ NOTE: it seems .switch_wallhug only controls whether this runs
		return;
	self.switch_wallhug = 0;

	b_temp2 = nextent(world); // PZ NOTE: first 32 after world are client entities
	clientNumber = 0;

	while (clientNumber < max_clients)
	{
		if (!b_temp2.ishuman)
		{
			if (frik_isClientSlotActive(clientNumber))
				{}//UpdateClient(b_temp2); // PZ: doing this all in the C code
		}
		clientNumber = clientNumber + 1;
		b_temp2 = nextent(b_temp2);
	}
};*/

// #############################################################################
#ifdef PR_IS_THIS_TRASHABLE // PZ: need this?
/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Rankings 'utilities'. Written by Alan Kivlin,
this code just fools clients by sending precisely
the same network messages as when a real player
signs on to the server.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// PZ: doing all of this serverside, like a real client now (this function is no longer called)
void UpdateClient(entity who)
{
string temp;

return;

temp = ftos(who->b_userid);
bprint(PR_PRINT_HIGH, "UpdateClient() called for ", who->netname, ". (.b_userid = ", temp, ")\n");
	WriteByte(2, 14 );                // PZ: svc_updatefrags
	WriteByte(2, who->b_clientno);
	WriteShort(2, who->frags);
	WriteByte(2, 36);                 // PZ: svc_updateping (server is currently overriding this in SV_Pings_f())
	WriteByte(2, who->b_clientno);
	WriteShort(2, 100 * (3 - who->b_skill));
	WriteByte(2, 37);                // update entertime
	WriteByte(2, who->b_clientno);          // client number
	WriteLong(2, time - who->b_entertime);   // client entertime
	WriteByte(2, 40 );            // update userinfo
	WriteByte(2, who->b_clientno);      // client number
	WriteLong(2, who->b_userid);      // client userid
	WriteByte(2, 92);  // \      DON'T "\ CONTINUE" THIS LINE, COMPILER
	/*WriteByte(2, 98);  // b
	WriteByte(2, 111); // o
	WriteByte(2, 116); // t
	WriteByte(2, 116); // t
	WriteByte(2, 111); // o
	WriteByte(2, 109); // m
	WriteByte(2, 99);  // c
	WriteByte(2, 111); // o
	WriteByte(2, 108); // l
	WriteByte(2, 111); // o
	WriteByte(2, 114); // r
	WriteByte(2, 92);  // \      DON'T "\ CONTINUE" THIS LINE, COMPILER
	if(who.b_pants > 9)
	{
		WriteByte(2, 49);
		WriteByte(2, 38 + who.b_pants);
	}
	else
		WriteByte(2, 48 + who.b_pants);
	WriteByte(2, 92);  // \      DON'T "\ CONTINUE" THIS LINE, COMPILER
	WriteByte(2, 116); // t
	WriteByte(2, 111); // o
	WriteByte(2, 112); // p
	WriteByte(2, 99);  // c
	WriteByte(2, 111); // o
	WriteByte(2, 108); // l
	WriteByte(2, 111); // o
	WriteByte(2, 114); // r
	WriteByte(2, 92);  // \      DON'T "\ CONTINUE" THIS LINE, COMPILER
	if(who.b_shirt > 9)
	{
		WriteByte(2, 49);
		WriteByte(2, 38 + who.b_shirt);
	}
	else
		WriteByte(2, 48 + who.b_shirt);
	WriteByte(2, 92);  // \      DON'T "\ CONTINUE" THIS LINE, COMPILER
	WriteByte(2, 116); // t
	WriteByte(2, 101); // e
	WriteByte(2, 97);  // a
	WriteByte(2, 109); // m
	WriteByte(2, 92);  // \      DON'T "\ CONTINUE" THIS LINE, COMPILER
	WriteByte(2, 98);  // b
	WriteByte(2, 111); // o
	WriteByte(2, 116); // t
	// FIXME: do teams properly
	// note this has no effect on infokey
	WriteByte(2, 92 ); // \      DON'T "\ CONTINUE" THIS LINE, COMPILER
	WriteByte(2, 115); // s
	WriteByte(2, 107); // k
	WriteByte(2, 105); // i
	WriteByte(2, 110); // n
	WriteByte(2, 92);  // \      DON'T "\ CONTINUE" THIS LINE, COMPILER
	WriteByte(2, 98);  // b
	WriteByte(2, 97);  // a
	WriteByte(2, 115); // s
	WriteByte(2, 101); // e
	WriteByte(2, 92);  // \*/
	WriteByte(2, 110); // n
	WriteByte(2, 97);  // a
	WriteByte(2, 109); // m
	WriteByte(2, 101); // e
	WriteByte(2, 92);  // \      DON'T "\ CONTINUE" THIS LINE, COMPILER
	WriteString( 2, who->netname);
	// PZ
	temp = strcat("name ", who->netname);
	temp = strcat(temp, "\n");
	stuffcmd(who, temp);
}
#endif
// #############################################################################

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_setupNewClient        (PZ: was "ClientInRankings")

`self` = the new client

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_setupNewClient()
{
	float clientNumber;
	entity player; // PZ
	//local float goodUserID; // PZ
	//local float temp; // PZ
	//local string stemp; // PZ
	//local float temp2; // PZ
	//local float breakLoop; // PZ

	clientNumber = self->colormap - 1;
	//BotInvalidClientNo (clientNumber); // PZ: shouldn't need this

	// PZ NOTE: update the player entity linked list
	/*if (player_head)
		player_head._prev = self;
	self._next = player_head;
	player_head = self;*/
	// PZ NOTE: For some reason, he grows the list from the beginning of the list. Because that just seems awkward to me, I'm redoing it.
	player = player_head;
	if (player == world) player_head = self;
	else
	{
		while (player->_next != world) player = player->_next; // go to the end of the list
		player->_next = self;
		self->_prev = player;
	}

	// PZ: Don't allow the bot to take a userid of a player or another bot.
	/*userid = 1;
	goodUserID = #FALSE;

	while (!goodUserID)
	{
		breakLoop = #FALSE;
		// is `userid` an open slot?
		player = nextent(world);
		while (player != world && !breakLoop)
		{
			if (player.classname == "player")
			{
				temp2 = getuid(player);
				//if (userid == getuid(player))
				if (userid == temp2 || userid == player.b_userid)
					breakLoop = #TRUE;
			}
			player = nextent(player);
		}
		if (player == world)
			goodUserID = #TRUE;
		else
			userid = userid + 1;
	}*/
	// PZ: END
	//userid = userid + 1; // PZ: the original code
	self->b_userid = getuid(self); // PZ: server gave him an ID

	// PZ: We are no longer using the FrikBot's physics. Bots now use the same C code physics that real players use.
	// PZ NOTE: set up the physics client for this bot (FIXME why are we doing this for player clients too???)
	// PZ NOTE: I think it's just being set up for the client slot, for when bots are using the slot.
	/*if (!self.phys_obj)
	{
		local entity po;
		po = phys_head;
		while (po != world && po.owner != self)
			po = po._next;
		self.phys_obj = po;
	}*/

	// PZ NOTE: for initializing bots
	if (self->ishuman == 2)
	{
		self->ishuman = PR_FALSE;
		return;
	}
	// PZ NOTE: THE REST IS ONLY RAN ON HUMAN PLAYERS.

	/*if (clientNumber >= 17)
		active_clients2 = active_clients2 | getVectorBitFlagGivenBitNumber(clientNumber - 16);
	else
		active_clients1 = active_clients1 | getVectorBitFlagGivenBitNumber(clientNumber);*/
	// PZ: using the vector, `b_activeClientSlots`, instead, to keep things neater
	b_activeClientSlots = setVectorBitFlag(clientNumber, b_activeClientSlots);

	self->b_clientno = clientNumber;
	self->ishuman = PR_TRUE;
	//self.switch_wallhug = time + 1; // PZ NOTE: tells it to run scoreboard updating stuff, which we aren't using
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_handleClientDisconnection    (PZ: was "ClientDisconnected")

`self` = the client that has disconnected

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// PZ TODO: determine what is need here
void frik_handleClientDisconnection()
{
	// updates the active player linked list
	if (player_head == self)
		player_head = self->_next;
	if (self->_next != world)
		self->_next->_prev = self->_prev;
	if (self->_prev != world)
		self->_prev->_next = self->_next;

	/*if (self.b_clientno >= 17)
		active_clients2 = active_clients2 - (active_clients2 & getVectorBitFlagGivenBitNumber(self.b_clientno - 16));
	else
		active_clients1 = active_clients1 - (active_clients1 & getVectorBitFlagGivenBitNumber(self.b_clientno));*/
	// PZ: using the vector, `b_activeClientSlots`, instead, to keep things neater
	b_activeClientSlots = clearVectorBitFlag(self->b_clientno, b_activeClientSlots);

	// PZ: Clearing out the client entity that this bot used, so that the next time this entity is used, it's initialized correctly.
	// PZ: We only need to hold on to its .phys_obj.
	// PZ: moved here; this was being done last in this function; we want it to also clear the entity when a human leaves (moved 4/12/2015)
	// PZ: We are no longer using the FrikBot's physics. Bots now use the same physics that real players use, in the C code.
	//local entity po;
	//po = self.phys_obj;
	clearAllEntityFields(self);
	//self.phys_obj = po;

	if (self->ishuman) return;
	// THE FOLLOWING IS FOR BOTS ONLY

	// PZ: We are no longer using the FrikBot's physics. Bots now use the same physics that real players use, in the C code.
	// PZ: make sure the bot's phys_obj becomes invisible (without this, 'statues' of the bot are left behind)
	//if (self.phys_obj)
	//	setmodel(self.phys_obj, string_null);

	// PZ: Without setting this, when bots using a previously used client slot rejoined, they would start playing immediately at the location
	// of the initial 'join the server, be in menus' area. They would be active players, rather than just in menus.
	// I don't know why this gets reset for real players when they leave, but for some reason not for bots.
	self->playerclass = PR_PC_UNDEFINED;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botPreThink() & frik_botPostThink(), used to make the
bot easier to install

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botPreThink          (PZ: was "BotPreFrame")

Called every frame before physics are ran.

`self` = any player, human or bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_botPreThink()
{
	// PZ NOTE: if this is an invalid client, don't run this function
	if (self->b_clientno == -1)
		return PR_FALSE; // PZ: let's return #FALSE when we DON'T run

	// Handle waypoint editor menus, and impulses for the FrikBot. (PZ: moved from frik_botPostThink())
	if (self->ishuman && self->admin_flag && !(self->_edict->client->spectator && self->_edict->client->spec_track)) // must be logged in as admin (cmd admin login <password>)
	{
		// Waypoint Editor Maintenance
		if (waypoint_mode >= PR_WM_EDITOR && self->current_menu == 0) // .current_menu is the Team Fortress menu
		{
			// if the admin joins while the waypoint editor is already active, or loses their menu some other way, restore it
			if (self->b_menu == 0) // .b_menu is the FrikBot (waypoint editor) menu
			{
				self->b_menu = PR_MENU_MAIN;
				self->b_timeToRedrawWaypointEditorMenu = time; // now
				// PZ: handled elsewhere now; this will cause a crash, if you try to set model of world
				//if (self.current_way)
				//	setmodel(self.current_way.owner, "progs/s_light.spr");
			}
			frik_updateWaypointEditorForSelf();
			frik_displayAndManageWaypointEditorMenu();
		}
		// Impulses only allowed while logged in as admin.
		frik_botImpulses();
	}

	// PZ: don't do anything for spectators after this point
	if (self->classname == "spec") return PR_TRUE;
return PR_TRUE; // PZ: for right now, don't do anything else below


	/*if (self.ishuman) // PZ: Took this out. We aren't running scoreboard updating stuff here. That's done in the C code.
		if (self.switch_wallhug)
			ClientFixRankings();*/

	// PZ NOTE: handle talking, dealing with killing something or dying
	// PZ: Had to add "!self.ishuman &&". It was doing bot conversation with humans starting the conversation, causing bugs.
	if (!self->ishuman && self->b_frags != self->frags)
	{
		if (self->b_frags > self->frags) // bot got a minus 1
		{
			if (pointcontents(self->origin) == PR_CONTENT_LAVA)
				frik_botConsiderStartingConversation(PR_TOPIC_TARG_KILLED_BY_ENVIRONMENT, 0.2);
			else
				frik_botConsiderStartingConversation(PR_TOPIC_TARG_KILLED_THEIRSELF, 0.2);
		}
		else // bot got a frag
			frik_botConsiderStartingConversation(PR_TOPIC_SELF_GOT_A_KILL, 0.2);

		self->b_frags = self->frags;
		// PZ: the server should be handling updating all clients with bot client info (2/3/14)
		/*if (!self.ishuman)
		{
			WriteByte(2, 14);
			WriteByte(2, self.b_clientno);
			WriteShort(2, self.frags);
		}*/
	}

	// PZ NOTE: This only really does something if we are using dynamic waypointing, or
	// if we are using some sort of dynamic mode in the waypoint editor.
	//frik_dynamicWaypointing();

	return PR_TRUE; // PZ: return #TRUE, instead, when we DO run
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botPostThink          (PZ: was "BotPostFrame")

Called every frame after physics are ran.
This only does things for human players.

`self` = any player, human or bot

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_botPostThink()
{
	// PZ NOTE: if this is an invalid client, don't run this function
	if (self->b_clientno == -1)
		return PR_FALSE; // PZ: let's return #FALSE when we DON'T run

	// PZ: nothing here for right now

	return PR_TRUE; // PZ: return #TRUE, instead, when we DO run
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

Bot Chat code

The rest of this code is in bot_rank.qc

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botSay          (PZ: was "BotSay")

Makes bot say `h`.

`self` = the bot that is speaking

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_botSay(const string& h) // simulate talking by composing a 'chat' message
{
	string temp;
	temp = strcat(": ", h);              // PZ: let's not require putting ": " before every bot message in the code
	WriteByte(PR_MSG_ALL, PR_SVC_PRINT);
	WriteByte(PR_MSG_ALL, PR_PRINT_CHAT);
	WriteByte(PR_MSG_ALL, 1);              // PZ NOTE: 1 or 2 as first character makes it colored (doesn't appear to make a difference whether it's 1 or 2)
	WriteString(PR_MSG_ALL, self->netname);
	WriteByte(PR_MSG_ALL, PR_SVC_PRINT);
	WriteByte(PR_MSG_ALL, PR_PRINT_CHAT);
	WriteByte(PR_MSG_ALL, 2);              // PZ NOTE continuation about colored text: see Con_Print()
	WriteString(PR_MSG_ALL, temp);         // PZ: temp instead of h

	self->b_chattime = 0; // PZ NOTE: bot is done talking
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botSayDebug

PZ: Makes bot say `h`, but done as a bprint()
because they get immediately printed out. Stuff
that goes through the network buffer has a delay.
I tried making this like the above, but with
PRINT_HIGH rather than PRINT_CHAT, but it had
the same delay.

`self` = the bot that is speaking

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_botSayDebug(const string& h) // simulate talking by composing a 'chat' message
{
	bprint(PR_PRINT_HIGH, self->netname, ": ", h);
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botSayInit          (PZ: was "BotSayInit")

Initializes frik_botSay2(). (Prints "<name>: ")

`self` = the bot that is speaking

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_botSayInit()
{
	string temp;
	temp = strcat(self->netname, ": "); // PZ: let's not require putting ": " before every bot message in the code
	WriteByte(PR_MSG_ALL, PR_SVC_PRINT);   // svc_print
	WriteByte(PR_MSG_ALL, PR_PRINT_CHAT);
	WriteByte(PR_MSG_ALL, 1);            // PZ NOTE: 1 or 2 as first character makes it colored (doesn't appear to make a difference whether it's 1 or 2)
	WriteString(PR_MSG_ALL, temp);       // PZ: temp instead of self.netname
}
/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botSay2          (PZ: was "BotSay2")

Allows you to give multiple calls to print out one chat message.
Call frik_botSayInit() to print the bot's "<name>: ", first.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_botSay2(const string& h)
{
	WriteByte(PR_MSG_ALL, PR_SVC_PRINT);
	WriteByte(PR_MSG_ALL, PR_PRINT_CHAT);
	WriteByte(PR_MSG_ALL, 2);            // PZ NOTE continuation about colored text: see Con_Print()
	WriteString(PR_MSG_ALL, h);

	self->b_chattime = 0; // PZ NOTE: bot is done talking
}
/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botSayTeam          (PZ: was "BotSayTeam")

Makes bot say `msg` to teammates.

`self` = the bot that is speaking
`timeDelay` = how long to wait (in seconds) before saying the message (-1 means auto, based on number of characters in message)

## Provide \n at the end. ##
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_botSayTeam(const string& msg, float timeDelay)
{
	// FBX QW doesn't support teamplay...yet (PZ NOTE: well, seems to now, but there may be some parts of the bot still not working in QW)

	if (!teamplay && !coop)  // PZ: what about coop? added "&& !coop"
		return;

	// PZ: create a timer entity so that we can delay this message to simulate typing it
	entity timer;
	timer = spawnServerSide();
	timer->classname = "bot_msg_typing_timer";
	timer->owner = self;
	timer->flags = PR_BOT_SAY_TEAM;
	timer->message = makestr(msg);  // PZ: had to make the string permanent (it's deleted when the timer is removed)
	timer->think = timerThink_botFinishTypingMsg;
	if (timeDelay >= 0)
		timer->nextthink = time + timeDelay;
	else
	{
		// Simulates them being able to type 6 characters a second (or 72 wpm, if a word is 5 characters).
		// This would includes spaces and punctuation. I'm not sure whether people normally include those in a "word".
		timeDelay = strlen(msg) / 6;
		timer->nextthink = time + timeDelay + (crandom() * (timeDelay / 3));
	}

	self->b_chattime = 999999; // bot is typing  (done typing when this is set to 0)
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_graphicallyDisplayWaypointLink  (PZ: was "DeveloperLightning")

Graphically displays waypoint links for the
waypoint editor, using temporary entities.

Regular links show up as red 'clouds' (blood mist).
Teleporter links show up as lightning bolts between the
two waypoints.

PZ: Copied this from bot.qc.

`self` = player using the editor

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_graphicallyDisplayWaypointLink(entity waypoint1, entity waypoint2, float bool_telelink)
{
	//if (AI_flags & #AI_TELELINK_1 || AI_flags & #AI_TELELINK_2 || AI_flags & #AI_TELELINK_3 || AI_flags & #AI_TELELINK_4)
	if (bool_telelink)
	{
		// PZ: This code would have a lightning bolt show up very rarely.
		/*WriteByte(#MSG_BROADCAST, #SVC_TEMPENTITY);
		WriteByte(#MSG_BROADCAST, #TE_LIGHTNING2);
		WriteEntity(#MSG_BROADCAST, waypoint2);
		WriteCoord(#MSG_BROADCAST, waypoint1.origin_x);
		WriteCoord(#MSG_BROADCAST, waypoint1.origin_y);
		WriteCoord(#MSG_BROADCAST, waypoint1.origin_z);
		WriteCoord(#MSG_BROADCAST, waypoint2.origin_x);
		WriteCoord(#MSG_BROADCAST, waypoint2.origin_y);
		WriteCoord(#MSG_BROADCAST, waypoint2.origin_z);*/
		// PZ: This code works.
		WriteByte(PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
		WriteByte(PR_MSG_BROADCAST, PR_TE_LIGHTNING2);
		WriteEntity(PR_MSG_BROADCAST, waypoint1);
		WriteCoord(PR_MSG_BROADCAST, waypoint1->origin[X]);
		WriteCoord(PR_MSG_BROADCAST, waypoint1->origin[Y]);
		WriteCoord(PR_MSG_BROADCAST, waypoint1->origin[Z]);
		WriteCoord(PR_MSG_BROADCAST, waypoint2->origin[X]);
		WriteCoord(PR_MSG_BROADCAST, waypoint2->origin[Y]);
		WriteCoord(PR_MSG_BROADCAST, waypoint2->origin[Z]);
		multicast(waypoint1->origin, PR_MULTICAST_PVS);
	}
	// PZ: The original function causes crashing (at least in QuakeWorld).
	// The use of TE_LIGHTNINGBLOOD in a message wasn't right.
	// ## What is 13 in NetQuake? Actually intending to use TE_LIGHTNINGBLOOD?
	else
	{
		// PZ NOTE: This old code way making everyone's link indicators visible to all, causing confusion
		//          when multiple people were waypointing in the same area.
		/*WriteByte(PR_MSG_MULTICAST, PR_SVC_TEMPENTITY);
		WriteByte(PR_MSG_MULTICAST, PR_TE_LIGHTNINGBLOOD);
		WriteCoord(PR_MSG_MULTICAST, waypoint2->origin[X]);
		WriteCoord(PR_MSG_MULTICAST, waypoint2->origin[Y]);
		WriteCoord(PR_MSG_MULTICAST, waypoint2->origin[Z]);
		multicast(waypoint2->origin, PR_MULTICAST_PVS);*/
		// PZ: Let's send this only to the player that owns the waypoint selection.
		msg_entity = self;
		WriteByte(PR_MSG_ONE, PR_SVC_TEMPENTITY);
		WriteByte(PR_MSG_ONE, PR_TE_LIGHTNINGBLOOD);
		WriteCoord(PR_MSG_ONE, waypoint2->origin[X]);
		WriteCoord(PR_MSG_ONE, waypoint2->origin[Y]);
		WriteCoord(PR_MSG_ONE, waypoint2->origin[Z]);
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_getClientEntityByNumber   (PZ: was "GetClientEntity")

Retrieves client's entity, given the client's number.

FIXME?: It seems to think client numbers start at
0, rather than 1. Should it?

############# WARNING ##############
This seems to be broken. A client number of 1 seems to
return the second client entity. This isn't currently being
used anyway.
############# WARNING ##############

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
entity frik_getClientEntityByNumber(float clientNumber)
{
	entity e;
	e = world;         // start at the first entity (world)
	clientNumber = clientNumber + 1;
	while (clientNumber > 0)
	{
		clientNumber = clientNumber - 1;
		e = nextent(e); // the client entities always follow the world entity
	}
	return e;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_addBotToGame   (PZ: was "BotConnect")

Adds a bot to the game.

`self` = initially, the player that adds the bot, if a player added it (otherwise undefined)

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_addBotToGame(float botNameNumber, float botSkill)
{
	float  clientNumber = 0;  // PZ: set to 0 to get rid of C++ warning
	entity oldSelf;
	string userinfo; // PZ
	string botName;  // PZ
	string str;      // PZ

	oldSelf = self; // PZ: moved here (this is the player that created bot with impulse 100)

	self = spawnServerSide(); // PZ: temporary entity to hold bot data

	if (botNameNumber >= 1 && botNameNumber <= PR_NUMBER_OF_BOT_NAMES)
		botName = frik_getNameForBotGivenNumber(botNameNumber);
	else
		botName = frik_getRandomNameForBot();

	// PZ: generate starting userinfo
	userinfo = strcat("\\skin\\base\\name\\", botName);
	if (teamplay)
		userinfo = strcat(userinfo, "\\topcolor\\0\\bottomcolor\\0");
	else
	{
		userinfo = strcat(userinfo, "\\topcolor\\");
		str = ftos(self->b_shirt);
		userinfo = strcat(userinfo, str);
		userinfo = strcat(userinfo, "\\bottomcolor\\");
		str = ftos(self->b_pants);
		userinfo = strcat(userinfo, str);
	}
	// PZ TODO: should we use MakeImmune() here to keep the bot from getting kicked, since the kicking happens right when he joins?

	remove(self); // PZ: remove temporary entity

	self = addBot(userinfo); // PZ: new builtin, to get the bot a real serverside client entity
	if (self == world)
	{
		sprint(oldSelf, PR_PRINT_HIGH, "Server is full.\n");
		self = oldSelf;
		return;
	}

	self->netname = botName;
	bot_count = bot_count + 1;

	// give him a chance to say something when he joins the server
	frik_botConsiderStartingConversation(PR_TOPIC_SELF_JOINS_GAME, 0.2);

	// PZ: need this?
	self->colormap = self->b_clientno/* + 1*/; // PZ: self.b_clientno was clientNumber

	// set bot's skill level
	botSkill = rint(botSkill);
	if (botSkill > 3)
		botSkill = 3;
	else if (botSkill < 0)
		botSkill = 0;
	self->b_skill = botSkill;

	//self.b_entertime = time;    // PZ: done in C code, like a real client
	self->team = self->b_pants + 1;

	SetNewParms();     // PZ: need?
	self->ishuman = 2; // PZ: the bot is in the setup state; when it's ready, it becomes 0

	ClientConnect();
	PutClientInServer();

	// PZ: need?
	/*if (clientNumber >= 17)
		active_clients2 = active_clients2 | getVectorBitFlagGivenBitNumber(clientNumber - 16);
	else
		active_clients1 = active_clients1 | getVectorBitFlagGivenBitNumber(clientNumber);*/
	// PZ: using the vector, `b_activeClientSlots`, instead, to keep things neater
	b_activeClientSlots = setVectorBitFlag(clientNumber, b_activeClientSlots);

	self = oldSelf;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_handleBotDisconnection   (PZ: was "BotDisconnect")

Called in ClientDisconnect() for bot clients.

`self` = initially, the bot that is leaving

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// PZ: This is called at the beginning of ClientDisconnect(). removeBot() is called by frik_removeABot().
//     This ensures that frik_handleBotDisconnection() is always called even when entering 'kick' on the server console.
void frik_handleBotDisconnection(entity bot)
{
	//bprint(2, "RAN frik_handleBotDisconnection() on ", bot.netname, "\n");
	//local string h;
	entity oldSelf;
	oldSelf = self;
	self = bot;

	bot_count = bot_count - 1;
	// PZ: There's a problem with this function sending this value negative. Probably due to this function getting called on map change
	// after server has already reset all the variables to 0, including bot_count. Anyway, I hope this will work and all will be fine.
	if (bot_count < 0) bot_count = 0;

	//removeBot(bot);     // PZ: makes SV_DropClient() (C code) get called for the bot // PZ: doing this in frik_removeABot()
	//ClientDisconnect(); // PZ: don't call this because the removeBot() builtin function will make it get called

	//if (self.b_clientno != -1)
	//{
		// the bot's client number is not in use by a real player so we
		// must remove it's entry in the rankings
		// Quake engine sets all fields to 0, can only do the most important here
		self->b_frags = self->frags = 0;
		//self.netname = "";
		//self.classname = "";
		self->health = 0;
		self->items = 0;
		self->armorvalue = 0;
		self->weaponmodel = "";
		self->b_pants = 0;
		self->b_shirt = 0;
		self->ammo_shells = self->ammo_nails = self->ammo_rockets = self->ammo_cells = 0;

		/*if (self.b_clientno >= 17)
			active_clients2 = active_clients2 - (active_clients2 & getVectorBitFlagGivenBitNumber(self.b_clientno - 16));
		else
			active_clients1 = active_clients1 - (active_clients1 & getVectorBitFlagGivenBitNumber(self.b_clientno));*/
		// PZ: using the vector, `b_activeClientSlots`, instead, to keep things neater
		b_activeClientSlots = clearVectorBitFlag(self->b_clientno, b_activeClientSlots);

		self->b_clientno = -1;
		//self.ishuman = 1; // PZ: reset client slot to human
	//}

	// PZ: remove the bot's role selection timer entity, which it should have exactly one of
	/*local entity e;
	e = nextent(world);
	while (e)
	{
		if (e.classname == "bot_role_selection_timer" && e.owner == self)
		{
			dremove(e);
			break;        // PZ NOTE: if you ever use this code again, don't use the break statement.. it doesn't work
		}
		e = nextent(e);
	}*/
	// PZ: remove any of its timers set for removal on death of the bot
	entity e;
	e = nextent(world);
	while (e != world)
	{
		if (e->owner == self)
		{
			if (e->deadflag & PR_REMOVE_TIMER_ON_DISCONNECT)
			{
				if (strstr(e->classname, "timer") >= 0)
				{
					//bprint(2, "found entity for removal: ", e.classname, ", ", e.netname, "\n");
					if (e->th_die == SUB_Null) // is there not a special cleanup function?
						dremove(e);
					else
						e->th_die(); // we need to call cleanup function (do more than just delete the timer)
				}
			}
		}
		e = nextent(e);
	}

	self = oldSelf;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                  (PZ: was "BotInvalidClientNo")
Kicks a bot if a player connects and takes the bot's space.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
/* PZ: we're not using this
void(float clientno) BotInvalidClientNo =
{
	local entity bot;

	if (frik_isClientSlotActive(clientno))
	{
		bot = frik_getClientEntityByNumber(clientno);

		if (bot.b_clientno == clientno)
		{
			bot.b_clientno = -1;
			frik_handleBotDisconnection(bot);
			//if (clientno > 16)
			//	active_clients2 = active_clients2 | (active_clients2 & getVectorBitFlagGivenBitNumber(clientno - 16));
			//else
			//	active_clients1 = active_clients1 | (active_clients1 & getVectorBitFlagGivenBitNumber(clientno));
			// PZ: using the vector, `b_activeClientSlots`, instead, to keep things neater
			b_activeClientSlots = setVectorBitFlag(clientno, b_activeClientSlots);
			frik_addBotToGame(self.b_num, self.b_skill);
			return;
		}
	}
};*/

// PZ NOTE: probably have no use for these 3 functions now
// PZ: copied over these 3 functions from bot.qc (for waypoints)
/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

Waypoint Loading from file
PZ NOTE: this is for the original .way format

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
/* PZ: we've not using this
void() LoadWaypoint =
{
	local vector org;
	local entity tep;
	local float r;
	org_x = cvar("saved1");
	org_y = cvar("saved2");
	org_z = cvar("saved3");

	tep = frik_createBlankWaypointHere(org);

	r = cvar("saved4");

	tep.b_aiflags = floor(r / 4);
	tep.b_pants = cvar("scratch1");
	tep.b_skill = cvar("scratch2");
	tep.b_shirt = cvar("scratch3");
	tep.b_frags = cvar("scratch4");
};*/
/*
void() bot_return =
{
	if (time > 2)
	{
		if ((waypoint_mode == #WM_DYNAMIC) || (waypoint_mode == #WM_LOADED))
		{
			// minor precaution

			if (saved_bots & 1) frik_addBotToGame(0, 1, saved_skills1 & 3);
			if (saved_bots & 2) frik_addBotToGame(0, 2, (saved_skills1 & 12) / 4);
			if (saved_bots & 4) frik_addBotToGame(0, 3, (saved_skills1 & 48) / 16);
			if (saved_bots & 8) frik_addBotToGame(0, 4, (saved_skills1 & 192) / 64);
			if (saved_bots & 16) frik_addBotToGame(0, 5, (saved_skills1 & 768) / 256);
			if (saved_bots & 32) frik_addBotToGame(0, 6, (saved_skills1 & 3072) / 1024);
			if (saved_bots & 64) frik_addBotToGame(0, 7, (saved_skills1 & 12288) / 4096);
			if (saved_bots & 128) frik_addBotToGame(0, 8, (saved_skills1 & 49152) / 16384);
			if (saved_bots & 256) frik_addBotToGame(0, 9, saved_skills2 & 3);
			if (saved_bots & 512) frik_addBotToGame(0, 10, (saved_skills2 & 12) / 4);
			if (saved_bots & 1024) frik_addBotToGame(0, 11, (saved_skills2& 48) / 16);
			if (saved_bots & 2048) frik_addBotToGame(0, 12, (saved_skills2 & 192) / 64);
			if (saved_bots & 4096) frik_addBotToGame(0, 13, (saved_skills2 & 768) / 256);
			if (saved_bots & 8192) frik_addBotToGame(0, 14, (saved_skills2 & 3072) / 1024);
			if (saved_bots & 16384) frik_addBotToGame(0, 15, (saved_skills2 & 12288) / 4096);
			if (saved_bots & 32768) frik_addBotToGame(0, 16, (saved_skills2 & 49152) / 16384);
			saved_bots = 0;
		}
	}
};*/

// PZ NOTE: would be nice if he explained somewhere what he's using these saved# and scratch# variables for
/* PZ: we're not using this
void() WaypointWatch =
{
	// Waypoint Baywatch
	local float bigboobs;
	local string h;
	if (framecount < 4)
		return;
	if (max_clients < 2)
		return;
	if (waypoint_mode != #WM_UNINIT)
	{
		bigboobs = cvar("saved4");
		if (bigboobs != 0)
		{
			if ((bigboobs & 3) == 1)
				frik_deleteAllWaypoints();
			else if ((bigboobs & 3) == 3)
			{
				frik_fixAllWaypoints();
				h = ftos(b_options);
				cvar_set("saved1", h);
				cvar_set("saved4", "0");
				cvar_set("scratch1", "0");
				waypoint_mode = #WM_LOADED;
				return;
			}
			LoadWaypoint();
			waypoint_mode = #WM_LOADING;
			cvar_set("saved4", "0");
		}
	}
	else
	{
		// the bots return!
		b_options = cvar("saved1");
		if (coop || (b_options & #OPT_SAVEBOTS))
		{
			saved_bots = cvar("scratch1");
			saved_skills1 = cvar("scratch2");
			saved_skills2 = cvar("scratch3");
		}
		cvar_set ("saved4", "0");
		if (max_clients > 1)
		{
			localcmd("exec maps/");
			localcmd(mapname);
			localcmd(".way\n");
			waypoint_mode = #WM_DYNAMIC;
			frik_attemptToLoadWaypointsForCurrentMap();
		}
		else
			waypoint_mode = #WM_LOADED;
	}
};*/
// PZ: END 3 functions

// static variables
float frik_mainRoutine_frameTimeAccumulator;
float lastTimeBotCountAdjusted; // PZ: needed static variable for the following function
float lastTimeDecrementedBotSkill; // PZ: another static variable for the following function
float frik_mainRoutine_LastNumberOfHumans;   // PZ: another static variable for the following function
//float frik_mainRoutine_lastLockedSkillLevel; // PZ: need static

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_mainRoutine           (PZ: was "BotFrame")

Called every frame in StartFrame().

`self` = steps through client entities

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
//entity debug1, debug2, debug3, debug4; // for debugging
void frik_mainRoutine()
{
	string h;
	float num;

	// ####################################
	// FRAME RATE CALCULATIONS
	// ####################################

	// PZ NOTE: The C code runs the QuakeC code at about 10 to 20 Hz. More often when there are players.
	// PZ NOTE: Actually, the average framerate seems to be hanging around 30 frames/sec, when players are in the game, and when lag/delay is low. (10-18-2015)
	// Frame rate is limited by `sv_mintic`, which is set at 0.03 (so it's capped at 1 / 0.03 = 33.33 Hz).
	// It will not allow a frame to run, if that much time has not passed since last frame.
	// `sv_mintic` and `sv_maxtic` are adjustable console variables.
	real_frametime = time - lasttime; // in QW, frametime is fuxx0red
	lasttime = time;

	// Maintain the frame counter for AI timing.
	// WARNING: This is used as a counter for bot AI call timing.
	if (bot_frameCounter < PR_LARGEST_INTEGER) // restart every `max_clients` frames; no, restart every #LARGEST_INTEGER frames, so that we can use intervals that aren't multiples of `max_clients`
		bot_frameCounter = bot_frameCounter + 1;
	else // On the #LARGEST_INTEGERth frame, go back to 1. If the server were to run at 30 frames/sec, this would last for 6+ days before restarting. No map will ever run that long.
		bot_frameCounter = 1;

	// Compute an average frame time, over the last 30 frames.
	frik_mainRoutine_frameTimeAccumulator = frik_mainRoutine_frameTimeAccumulator + real_frametime;
	if (modulo(bot_frameCounter, 30) == 0)
	{
		averageFrameTime = frik_mainRoutine_frameTimeAccumulator / 30;
		frik_mainRoutine_frameTimeAccumulator = 0;
	}

	//h = ftos(real_frametime);
	//bprint(2, "Running frame. Time between running last frame and running this frame: ", h, "\n");

	// ####################################
	// FRAME INITIALIZATION
	// ####################################

	h = infokey(world, "bot_options");
	b_options = stof(h);

	sv_maxspeed = cvar("sv_maxspeed");
	sv_gravity = cvar("sv_gravity");
	sv_friction = cvar("sv_friction");
	sv_accelerate = cvar("sv_accelerate");
	sv_stopspeed = cvar("sv_stopspeed");

	// ####################################
	// GENERAL UPDATES
	// ####################################

	// For the Waypoint Editor, when active.
	if (waypoint_mode == PR_WM_EDITOR)
	{
		// update the waypoint visual markers (the visual representations of waypoints)
		// TODO? Don't call this so often?
		frik_updateWaypointVisualMarkers();
	}

	// ####################################
	// BOT FRAMES
	// ####################################

	// Call bot routines (mostly AI) for each bot.
	self = nextent(world);
	num = 0;
	while (num < max_clients)
	{
		if (!self->ishuman)
		{
			if (self->b_clientno > 0)
			{
				// WARNING: Comment said, "Avoid calling frik_AI_Main() and the physics at the same time. Can trip the runaway loop counter."
				frik_AI_Main(); // PZ: added for 2015 AI overhaul

				//frik_botLookForAndHandleObstacles();

				// [Handle Physics]
				frik_turnInputCommandsIntoActions(); // PZ TODO: this function is a bit dirty; clean it up
				// PZ: We are no longer using the FrikBot's physics. Bots now use the same physics that real players use, in the engine.
				sendInputCmdForBot(self, self->v_angle, self->movevect[X], self->movevect[Y], self->movevect[Z], (self->PR_BUTTON_JUMP * 2) | (self->PR_BUTTON_FIRE)); // *2 == <<1
				// PZ: So, that also means that the bot's phys_obj is no longer used.
				// TODO TODO TODO Make sure we don't need anything that happens in the following two functions.
				//frik_clientThink();
				//frik_SV_Physics_Client();
				// this is sickening
				// PZ: Shouldn't need any of this stuff, since the physics entity is no longer visible.
				/*if (self.phys_obj)
				{
					if (self.phys_obj.modelindex != self.modelindex)
					{
						// PZ: The client entity is now truly visible. Client data for bots is sent to human clients.
						// If you want to see the bot's real position, make its physics object visible right here.
						// That will allow you to see how well the client is predicting the position of the bots.
						//setmodel(self.phys_obj, "progs/player.mdl"); // PZ OLD NOTE: bot is invisible if its phys_obj is invisible
						self.phys_obj.modelindex = self.modelindex;
					}
					self.phys_obj.frame = self.frame;
					self.phys_obj.angles = self.angles;
					self.phys_obj.colormap = self.colormap;
					self.phys_obj.effects = self.effects;
				}*/
				// PZ: end of removed physics code

				/*if (debug1 != self.target1 || debug2 != self.target2 || debug3 != self.target3 || debug4 != self.target4)
				{
					bprint(2, ".target1 = ", self.target1.classname, " ; ", self.target1.netname, "\n");
					bprint(2, ".target2 = ", self.target2.classname, " ; ", self.target2.netname, "\n");
					bprint(2, ".target3 = ", self.target3.classname, " ; ", self.target3.netname, "\n");
					bprint(2, ".target4 = ", self.target4.classname, " ; ", self.target4.netname, "\n");
				}
				debug1 = self.target1; debug2 = self.target2; debug3 = self.target3; debug4 = self.target4;*/
			}
		}
		self = nextent(self);
		num = num + 1;
	}

	// ####################################
	// BOT COUNT MAINTENANCE
	// ####################################

	// PZ: Maintain an ideal number of players by controlling the bot count.
	// ! This is some how causing crashing on map rotation (I'm pretty sure this is causing it.)
	// Maybe because bots leave at the end of a map? And it's fighting to maintain a bot count. I don't know.
	// For whatever reason, I found that only adding/removing a bot every 5 seconds works nicely.
	if (time < lastTimeBotCountAdjusted + 1) return;

	h = infokey(world, "idealp");
	if (h == "") num = max_clients;
	else         num = stof(h);

	// if players voted for a different value, use it
	if (botVoteMenuData[CURRENT].idealNumPlayers != 255) num = botVoteMenuData[CURRENT].idealNumPlayers;
	int idealNumPlayers = num; // for use way down below

	// PZ: Using num_clients here so that we can take downloaders into account and allow up to 31 bots at a time.
	//     num_players doesn't take into account downloaders. Make sure we always have an open slot for a player to join,
	//     if the server isn't full of human players.
	if (num_players < num && (num_clients /*num_players*/ < max_clients - 1))
	{
		//h = infokey(world, "skill");
		//num = stof(h);
#if 0
		// PZ: Creating a straight-up skill level randomizer for right now.
		local float rnum;
		rnum = random();
		if      (rnum >= 0.96) num = #SKILL_NIGHTMARE;
		else if (rnum >= 0.80) num = #SKILL_HARD;
		else if (rnum >= 0.30) num = #SKILL_MEDIUM;
		else if (rnum >= 0.00) num = #SKILL_EASY;
		// PZ: End randomizer.
#elif 0 // 12/8/2022: Was this way for a long time. No more. See below.
		// PZ: Make bot's skill depend on the collective human kill-to-death ratio.
		num = collectiveHumanScore / PR_COLLECTIVE_HUMAN_SCORE_MEMORY;
		// Start easy, because players are probably more likely to leave when it's too hard, than when it's too easy.
		if      (num <= 0.50)     num = PR_SKILL_EASY;       // was 0
		else if (num >= 9999)     num = PR_SKILL_NIGHTMARE;  // was 1   These were too hard, especially if you made one cap while bots were still joining.
		else if (num >= 9999)     num = PR_SKILL_HARD;       // was 0.75
		else if (num >= 0.50)     num = PR_SKILL_MEDIUM;
		else /*if (num >= 0.25)*/ num = PR_SKILL_EASY;
#else
		// 12/8/2022: The problem with the above way is that people will join the server and see the bots behaving very stupidly, because they are
		// on easy difficulty, and not be aware that the bots adjust to the humans or that they can control it with `votebot`, and prematurely write-
		// off the bots as "terrible". So, I'm going to start off with them on hard, and make it so that they adjust to players much more quickly.
		// When a bot joins the game, have him join as the average skill level of all the bots, unless he is the first bot (then use initial level).
		int botCount = 0;    // NOTE: We have a global bot counter, but this makes sure the count is right for the calculation.
		int averageBotSkillLevel = 0;
		entity client = nextent(world);
		for (int i = 0; i < max_clients; ++i)
		{
			if (!client->ishuman && client->b_clientno > 0)
			{
				++botCount;
				averageBotSkillLevel += client->b_skill;
			}
			client = nextent(client);
		}
		// If this is the first bot in the game, give him the initial skill level.
		if (botCount <= 0) num = initialBotSkillLevel;
		// Otherwise, give him the average bot skill level.
		else
		{
			averageBotSkillLevel /= botCount; // NOTE: By being divided here, we preclude the possibility of division by 0.
			num = averageBotSkillLevel;
		}
#endif
		// Add the bot to the server.
		frik_addBotToGame(0, num);            // NOTE: 0 tells it to select a random name.
		lastTimeBotCountAdjusted = time;
	}                          // make sure we keep an open slot for players to join
	else if (num_players > num || num_clients /*num_players*/ >= max_clients)
	{
		if (bot_count > 0)
		{
			frik_removeABot();
			lastTimeBotCountAdjusted = time;
		}
	}

	// ####################################
	// BOT DIFFICULTY MAINTENANCE
	// ####################################

	// PZ: [Allow locking the bots' skill level to a given level.] TODO DONE?: Allow setting upper and lower limits that still use dynamic system.
	string strtemp;
	int lockedSkillLevel[5], minimumSkillLevel[5], maximumSkillLevel[5]; // for overall, team 1, team 2, etc.
	entity client;

	// initialize arrays
	for (int i = 0; i < 5; i++) lockedSkillLevel[i] = minimumSkillLevel[i] = maximumSkillLevel[i] = 0;

	// these values only affect overall skill level
	strtemp = infokey(world, "bot_locked_skill");
	lockedSkillLevel[0] = stof(strtemp);
	if (!(lockedSkillLevel[0] >= 1 && lockedSkillLevel[0] <= 4)) lockedSkillLevel[0] = 0;

	strtemp = infokey(world, "bot_min_skill");
	minimumSkillLevel[0] = stof(strtemp);
	if (!(minimumSkillLevel[0] >= 1 && minimumSkillLevel[0] <= 4)) minimumSkillLevel[0] = 0;

	strtemp = infokey(world, "bot_max_skill");
	maximumSkillLevel[0] = stof(strtemp);
	if (!(maximumSkillLevel[0] >= 1 && maximumSkillLevel[0] <= 4)) maximumSkillLevel[0] = 0;

	for (int i = 0; i < 5; i++) // for overall, team 1, team 2, etc.
	{
		// if players voted for different values, use them
		if (botVoteMenuData[CURRENT].botLockedSkillLevel[i] != 255) lockedSkillLevel[i]  = botVoteMenuData[CURRENT].botLockedSkillLevel[i];
		if (botVoteMenuData[CURRENT].botMinSkillLevel[i] != 255)    minimumSkillLevel[i] = botVoteMenuData[CURRENT].botMinSkillLevel[i];
		if (botVoteMenuData[CURRENT].botMaxSkillLevel[i] != 255)    maximumSkillLevel[i] = botVoteMenuData[CURRENT].botMaxSkillLevel[i];

		if (maximumSkillLevel[i] > 0 && minimumSkillLevel[i] > maximumSkillLevel[i]) minimumSkillLevel[i] = maximumSkillLevel[i];

		if (lockedSkillLevel[i] || minimumSkillLevel[i] || maximumSkillLevel[i])
		{
			client = nextent(world);
			for (num = 1; num <= MAX_CLIENTS; num++)
			{
				if (!client->ishuman && client->is_connected && (i == 0 || i == client->team_no))
				{
					if (lockedSkillLevel[i])
						client->b_skill = lockedSkillLevel[i] - 1;
					else
					{
						if      (minimumSkillLevel[i] && ((client->b_skill + 1) < minimumSkillLevel[i])) client->b_skill = minimumSkillLevel[i] - 1;
						else if (maximumSkillLevel[i] && ((client->b_skill + 1) > maximumSkillLevel[i])) client->b_skill = maximumSkillLevel[i] - 1;
					}
				}
				client = nextent(client);
			}
		}
	}
	//frik_mainRoutine_lastLockedSkillLevel = lockedSkillLevel;

	// PZ: Adjust the skill level of the bots to make sure that players have appropriately-matched bot opponents.
	//     This is currently trying to keep the collective player kill-to-death ratio versus bots to 1:1. This should
	//     be changed if the ratio of most fun appears to be something else. The goal is to maximize fun. Would probably
	//     vary with each person, though.
	// TODO: Allow setting the ideal ratio, functioning as a difficulty setting.
	// When a player joins after server was empty, make sure collectiveHumanScore is reset to 0.
	if (bot_count < num_players && frik_mainRoutine_LastNumberOfHumans <= 0 /*collectiveHumanScore == -#COLLECTIVE_HUMAN_SCORE_MEMORY - 1*/)
		collectiveHumanScore = lastCollectiveHumanScore = 0;
	// [normal dynamic skill adjustment]
	if (collectiveHumanScore != lastCollectiveHumanScore /*&& !lockedSkillLevel*/)
	{
		int adjustment, loopCounter;
		adjustment = 0;
		/*if (collectiveHumanScore > lastCollectiveHumanScore)
			adjustment = collectiveHumanScore - lastCollectiveHumanScore;
		else if (collectiveHumanScore < lastCollectiveHumanScore)
			adjustment = coll*/
		// Determine whether we need to make an adjustment.
		// Only make an adjustment if we aren't moving closer to the ideal kill-to-death ratio (a score of 0, for right now).
		// 12/8/2022: I added `skillAdjustmentFactor` to increase how quickly bots change their skill.
		//            To view this 'function' on a graph, go here, https://www.desmos.com/calculator, and enter: y = x * 1/5
		//            It's a linear climb with a minimum value of 2.
		skillAdjustmentFactor = MAX(2.0, bot_count / 5.0);
		if      (collectiveHumanScore > 0 && collectiveHumanScore > lastCollectiveHumanScore)
			//adjustment = 1;
			adjustment = round((collectiveHumanScore - lastCollectiveHumanScore) * skillAdjustmentFactor);
		else if (collectiveHumanScore < 0 && collectiveHumanScore < lastCollectiveHumanScore)
			//adjustment = -1;
			adjustment = round((collectiveHumanScore - lastCollectiveHumanScore) * skillAdjustmentFactor);

		// Start with a random bot client each time.
		float rnum = random() * (MAX_CLIENTS - 1);
		rnum = floor(rnum);
		rnum += 1;   // make it 1 to 32
		// Set `num` to the bot we selected above.
		client = nextent(world);
		num = 1;
		while (rnum >= 2 && bot_count)
		{
			client = nextent(client);
			++num;
			if (num > MAX_CLIENTS) { num = 1; client = nextent(world); }
			// only decrement if this is an active bot client
			if (client->classname == "player" && !client->ishuman)
				--rnum;
		}
		// Look for a bot to adjust and make the needed adjustment.
		loopCounter = 1;
		while (/*num <= MAX_CLIENTS ||*/ adjustment != 0 && loopCounter <= 5)
		{
			// if we are at the end of the list, go back to the beginning
			if (num > MAX_CLIENTS)
			{
				client = nextent(world);
				num = 1;
				++loopCounter;
			}
			// if this client is a bot
			if (client->classname == "player" && !client->ishuman)
			{
				// determine minimum and maximum skill level to use for this player (get overall values, and override with team-specific ones, if set)
				int i = client->team_no;
				int maxSkillLevel = 0, minSkillLevel = 0;
				if (maximumSkillLevel[0]) maxSkillLevel = maximumSkillLevel[0]; // get the overall value, if set
				if (maximumSkillLevel[i]) maxSkillLevel = maximumSkillLevel[i]; // override with team value, if set
				if (minimumSkillLevel[0]) minSkillLevel = minimumSkillLevel[0]; // get the overall value, if set
				if (minimumSkillLevel[i]) minSkillLevel = minimumSkillLevel[i]; // override with team value, if set
				// player(s) needs more of a challenge  // changed this so that it doesn't make any 4s until all bots are at least 3  (8/7/15)
				if      (adjustment > 0 && ((client->b_skill <= PR_SKILL_MEDIUM) || (loopCounter >= 3 && client->b_skill < PR_SKILL_NIGHTMARE)))
				{
					if (!(maxSkillLevel >= 1 && (client->b_skill + 1) >= maxSkillLevel))
					{
						client->b_skill += 1;
						//bprint(2, "++INCREASED SKILL OF: ", client.netname, "\n");
						--adjustment;
						loopCounter = 1;
					}
				}
				// player(s) needs less of a challenge  // changed this so that 4s are the first to be downgraded  (8/7/15)
				else if (adjustment < 0 && ((client->b_skill == PR_SKILL_NIGHTMARE) || (loopCounter >= 3 && client->b_skill > PR_SKILL_EASY)))
				{
					if (!(minSkillLevel >= 1 && (client->b_skill + 1) <= minSkillLevel))
					{
						client->b_skill -= 1;
						//bprint(2, "--DECREASED SKILL OF: ", client.netname, "\n");
						++adjustment;
						loopCounter = 1;
					}
				}
				// PZ TODO? Make bot start a topic to tell the players that he has adjusted how hard he is playing?
			}
			client = nextent(client);
			++num;
		}
	}
	// Cap collectiveHumanScore, so that it doesn't remember so far back, which requires longer to make adjustments,
	// if the score goes in either direction for very long and then starts moving back to 0. Don't cap collectiveHumanScore
	// until it's been tested above for an increase or decrease.
	if      (collectiveHumanScore >  PR_COLLECTIVE_HUMAN_SCORE_MEMORY) collectiveHumanScore =  PR_COLLECTIVE_HUMAN_SCORE_MEMORY;
	else if (collectiveHumanScore < -PR_COLLECTIVE_HUMAN_SCORE_MEMORY) collectiveHumanScore = -PR_COLLECTIVE_HUMAN_SCORE_MEMORY;
	lastCollectiveHumanScore = collectiveHumanScore;
	// When there are no humans in the game, slowly return the bots back to their initial skill level.
	if (bot_count >= num_players)
	{
		if (time >= (lastTimeDecrementedBotSkill + (20 / bot_count))) // Will take about 1 minute to return all bots to skill 1 if all bots are at skill 4.
		{
			// 12/8/2022: We don't want to decrement them to lowest level (easy). We want to change them back to their initial skill level.
			//collectiveHumanScore = -PR_COLLECTIVE_HUMAN_SCORE_MEMORY - 1;    // previous way
			// New way: Loop through all bot clients, finding bots whose skill level does not match the initial skill level and increment
			//          them towards the initial level. TODO: Need a clean way to randomly iterate through the bots.
			entity client = nextent(world);
			for (int i = 0; i < max_clients; ++i)
			{
				if (!client->ishuman && client->b_clientno > 0)
				{
					if      (client->b_skill > initialBotSkillLevel) { --client->b_skill; break; }
					else if (client->b_skill < initialBotSkillLevel) { ++client->b_skill; break; }
				}
				client = nextent(client);
			}
			lastTimeDecrementedBotSkill = time;
		}
	}
	frik_mainRoutine_LastNumberOfHumans = num_players - bot_count;

	// PZ TODO: make the game pause when there are no players or spectators (THIS PROBABLY NEEDS TO BE DONE IN C CODE, BECAUSE BOT CODE WON'T RUN WHILE PAUSED)
	/*if (num_players == 0)
	{
		localcmd("pauseable 1\n");
		stuffcmd(self, "pause\n");
	}*/

	// ####################################
	// 'votebot' MAINTENANCE
	// ####################################

	// [Check that everyone is on the team that they should be on.]
	// Check humans. (Force Humans To Team option)
	int forceHumansToTeam = 0;
	if (botVoteMenuData[CURRENT].forceHumansToTeam != 255) forceHumansToTeam = botVoteMenuData[CURRENT].forceHumansToTeam;
	if (forceHumansToTeam)
	{
		entity oldSelf = self;
		self = spawnServerSide();
		entity cl = nextent(world);
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			if (cl->classname == "player" && cl->is_connected && cl->ishuman && cl->team_no != forceHumansToTeam)
			{
				self->admin_kick = cl;
				Admin_Assign(forceHumansToTeam, PR_FALSE, true);
			}
			cl = nextent(cl);
		}
		dremove(self);
		self = oldSelf;
	}
	// Check bots.   bot_count, num_players, playersOnTeam1, etc.
	float percentPlyrsPerTeam[4] = {255, 255, 255, 255};
	for (int i = 0; i < 4; i++) percentPlyrsPerTeam[i] = botVoteMenuData[CURRENT].percentPlyrsPerTeam[i] / 100.0;
	int playersOnTeam[4] = {playersOnTeam1, playersOnTeam2, playersOnTeam3, playersOnTeam4}; // TODO: change these globals to ints; also, this should be the number of bots, not all players
	// Go through each team, checking that the team does not exceed voted-for player count limits. For every team that does exceed,
	// move a bot from that team to another team with room for more.
	for (int i = 0;  i < 4 && (i < number_of_teams);  i++)
	{
		if (botVoteMenuData[CURRENT].percentPlyrsPerTeam[i] != 255)
		{
			//percentPlyrsPerTeam[i] = botVoteMenuData[CURRENT].percentPlyrsPerTeam[i] / 100.0;
			if ((playersOnTeam[i] / (float)num_players) > percentPlyrsPerTeam[i])
			{
				// [Randomly switch a bot on this team to a team with not enough players, if one exists. Otherwise, do nothing.]
				// Find team with not enough players.
				//bool breakLoop = false;
				for (int k = 0;  k < 4 && (k < number_of_teams) /*&& !breakLoop*/;  k++)
				{
					if (botVoteMenuData[CURRENT].percentPlyrsPerTeam[k] != 255)
					{
						//percentPlyrsPerTeam[k] = botVoteMenuData[CURRENT].percentPlyrsPerTeam[k] / 100.0;
						if (((playersOnTeam[k] + 1) / (float)num_players) <= percentPlyrsPerTeam[k]) // we add 1 here to check that there is room for a player to be added to this team
						{
							// Make a random bot on team |i+1| switch to team |k+1|.
							entity oldSelf = self;
							self = spawnServerSide();
							entity cl = nextent(world);
							for (int m = 0; m < MAX_CLIENTS; m++)
							{
								if (cl->classname == "player" && cl->is_connected && !cl->ishuman && cl->team_no == i+1)
								{
									self->admin_kick = cl;
									Admin_Assign(k+1, PR_FALSE, true);
									//breakLoop = true;
									break;
								}
								cl = nextent(cl);
							}
							dremove(self);
							self = oldSelf;
							break;
						}
					}
				}
			}
		}
	}

	// [Check that all bots are the class they should be, given their team.]
	// Note: When changing their class, make them change class, but then see if there is a way to kill them so that they respawn as the new class
	//       without giving them a negative frag. Might can use PutClientInServer() for that. Maybe look to see what kill command does.
	//       Call the code that sets a player's class (as though they selected a new class), and then PutClientInServer() to send them back to spawn.
	//       May want to make sure they are active in the game, by checking that they have a model, before moving them with PutClientInServer().
	//       Look at what AdminAssign() does for team changing. Probably can do the same for class changing.
	int botClassCounter[4][9];          countBotClasses(botClassCounter);
	bool useOverallSettings[4] = {0, 0, 0, 0};
	float botClassPercentage[5][10]; // we use -1 here to indicate no entry (rather than 255)
	for (int i = 0; i < 5; i++) for (int k = 0; k < 10; k++)
	{
		if (botVoteMenuData[CURRENT].botClassPercentage[i][k] != 255)
			botClassPercentage[i][k] = botVoteMenuData[CURRENT].botClassPercentage[i][k] / 100.0;
		else botClassPercentage[i][k] = -1;
	}
	// [For voting bot classes, Overall changes to classes will affect only teams that don't have any percentages set. It would be too complicated
	// to do any other way. Determine whether team should use Overall settings.]
	// Are Overall settings set to anything?
	for (int k = 0; k < 10; k++)
	{
		if (botClassPercentage[0][k] >= 0)
		{
			for (int i = 0; i < 4; i++) useOverallSettings[i] = true;
			break;
		}
	}
	// If a team has no class settings, but there are Overall settings to use, use them.
	if (useOverallSettings[0]) // only need to check first value, because they will all be the same at this point
	{
		for (int i = 1; i < 5; i++) for (int k = 0; k < 10; k++)
		{
			if (botClassPercentage[i][k] >= 0)
			{
				useOverallSettings[i-1] = false;
				k = 100; // break (done checking team `i`)
			}
		}
	}
	// For each team and each possible class, does the class exceed the voted for amount, for that team?
    for (int i = 1;  i < 5 && (i <= number_of_teams) && playersOnTeam[i-1];  i++)
	{
		for (int k = 0; k < 9; k++) // TODO: ignoring random (index 9) for right now
		{
			int teamIndex = useOverallSettings[i-1] ? 0 : i; // do we need to look at Overall settings or Team settings?
			if ((botClassCounter[i-1][k] / (float)playersOnTeam[i-1]) > botClassPercentage[teamIndex][k])
			{
				// [Move a bot from this class to another class that has room for more.]
				// Find a class with not enough bots.
				for (int m = 0; m < 9; m++)
				{
					if (botClassPercentage[teamIndex][m] >= 0)
					{
						if (((botClassCounter[i-1][m] + 1) / (float)playersOnTeam[i-1]) <= botClassPercentage[teamIndex][m]) // we add 1 here to check that there is room for a bot to be added to this class
						{
							// Make a random bot on team `i`, of class |k+1|, switch to class |m+1|.
							entity oldSelf;
							entity cl = nextent(world);
							for (int o = 0; o < MAX_CLIENTS; o++)
							{
								if (cl->classname == "player" && cl->is_connected && !cl->ishuman && cl->team_no == i && cl->skin == k+1)
								{
									cl->nextpc = m+1;     // set the class they will become after respawning
									oldSelf = self;
									self = cl;
									PutClientInServer();  // respawn them
									self = oldSelf;
									break;
								}
								cl = nextent(cl);
							}
							break;
						}
					}
				}
			}
		}
	}
}

void countBotClasses(int (&result)[4][9]) // parentheses need to be this way to keep it from thinking it's an array of references
{
	for (int i = 0; i < 4; i++) for (int k = 0; k < 9; k++) result[i][k] = 0;

	entity cl = nextent(world);
    for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (cl->classname == "player" && cl->is_connected && !cl->ishuman)
		{
            switch (cl->skin)
            {
				case PR_PC_SCOUT:     result[(int)cl->team_no-1][0]++;  break;
				case PR_PC_SNIPER:    result[(int)cl->team_no-1][1]++;  break;
				case PR_PC_SOLDIER:   result[(int)cl->team_no-1][2]++;  break;
				case PR_PC_DEMOMAN:   result[(int)cl->team_no-1][3]++;  break;
				case PR_PC_MEDIC:     result[(int)cl->team_no-1][4]++;  break;
				case PR_PC_HVYWEAP:   result[(int)cl->team_no-1][5]++;  break;
				case PR_PC_PYRO:      result[(int)cl->team_no-1][6]++;  break;
				case PR_PC_SPY:       result[(int)cl->team_no-1][7]++;  break;
				case PR_PC_ENGINEER:  result[(int)cl->team_no-1][8]++;  break;
            }
		}
		cl = nextent(cl);
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botImpulses         (PZ: was "BotImpulses")

Allows the player to perform bot-related impulses (commands).

`self` = the current player

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// PZ: we could trash this, if we wanted
void frik_botImpulses()
{
	//local float f;
	//local string h;

	// PZ: these are now done through "cmd admin bots"
	/*if (self.impulse == #IMPULSE_SPAWN_BOT)            // PZ: 230; was 100 but conflicted with CuTF impulses
	{
		h = infokey(world, "skill");
		f = stof(h);
		frik_addBotToGame(0, f);
	}
	else if (self.impulse == #IMPULSE_REMOVE_BOT)      // PZ: 231; was 102 but conflicted with CuTF impulses
		frik_removeABot();*/
	// PZ: copied from bot.qc for Waypoint editor (now done with "cmd admin waypoints")
	/*if (self.impulse == #IMPULSE_WAYPOINT_EDITOR) // PZ: 232; was 104 but conflicted with CuTF impulses
		frik_toggleWaypointEditorOnOrOff();*/
	if (self->impulse == 233)
	{
		string temp;
		temp = vtos(self->origin);
		sprint(self, PR_PRINT_HIGH, "my location (.origin) is: ", temp, "\n");
		temp = vtos(self->angles);
		sprint(self, PR_PRINT_HIGH, "my model's orientation (.angles) is: ", temp, "\n");
		temp = vtos(self->v_angle);
		sprint(self, PR_PRINT_HIGH, "my facing direction (.v_angle) is: ", temp, "\n");
	}
	else if (self->impulse == 234)
	{
		string tempstr;
		makevectors(self->v_angle);
		traceline(self->origin + V({0, 0, 16}), (self->origin + V({0, 0, 16})) + v_forward * 4096, PR_TL_EVERYTHING, self);
		tempstr = etos(trace_ent);
		sprint(self, PR_PRINT_HIGH, "You are looking at entity ", tempstr, ". classname = ", trace_ent->classname);
		sprint(self, PR_PRINT_HIGH, ", netname = ", trace_ent->netname, "\n");
	}
	else if (self->impulse == 235)
	{
		float entcount;
		entcount = getClientEntityCount();
		string tempstr;
		tempstr = ftos(entcount);
		sprint(self, PR_PRINT_HIGH, "Number of visible entities (those below index 512): ", tempstr, "\n");
	}

	// !!!!!!! PZ: THIS IS A GOOD PLACE TO PUT IMPULSES THAT PRINT OUT TESTS OF VECTOR MATH !!!!!!!

	/*else if (self.impulse == 234)
	{
		local entity ent;
		ent = spawn();
		ent.classname = "testy_ent";
		setorigin(ent, self.origin);
		setmodel(ent, "progs/missile.mdl");
		ent.frame = 0;
		ent.solid = #SOLID_NOT;
		ent.movetype = #MOVETYPE_NOCLIP;

		local float entNum;	entNum = 0;
		local entity te;
		te = nextent(world);
		while (te)
		{
			entNum = entNum + 1;
			if (te.classname == "testy_ent")
				break;                        // PZ NOTE: if you ever use this code again, don't use the break statement.. it doesn't work
			te = nextent(te);
		}
		local string temp2;
		temp2 = ftos(entNum);
		bprint(2, "created entity ", temp2, "\n");
	}*/
	// PZ: END
	else
		return;
	self->impulse = 0;
}

} // END namespace Progs
