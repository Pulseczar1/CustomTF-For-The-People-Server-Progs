/*=======================================================//
// cmnd.QC - CustomTF 3.2.OfN			   - 18/3/2004 - //
// by Sergio Fuma�a Grunwaldt - OfteN [cp]				 //
=========================================================//
 Client Command stuff - Requires PROZAC qwsv
=========================================================//
 This is a giant command system that expands the possible
 commands from users without using aliases, neither
 having almost any limitation.
 --------------------------------------------------------
 Our server code will call ClientCommand() function with
 the appropiate arguments set after a client enters
 any command in console with the "cmd" prefix.
=========================================================*/

#include "progs.h"
#include "ofndefs.h"
#include "cpstuff.h"
#include "vote.h"
#include "admin.h"
#include "spectate.h"
#include "debug.h"
#include "environ.h"
#include "frikbot/bot_ed.h"
#include "optimize.h"
#include "actions.h"
#include "runes.h"
#include "engineer.h"
#include "sbitems.h"
#include "warlock.h"
#include "weapons.h"
#include "neo.h"

namespace Progs {

// Flags

// Settings

#define PR_CMD_MSGRATE 0.4  // Minimum delay to occur between error and help prints
#define PR_CMD_MAX_ADMINATTEMPTS 5 // Maximum times a client is allowed to attempt admin password
#define PR_CMD_MAX_MAPREQUESTS 5 // Maximum custom map requests

#define PR_MAX_SNAME_LEN 24 // WARNING: Our qwsv max for perm strings is 32 chars (PZ NOTE: I increased that max to 1024 chars.)

#define PR_SECS_SNAMECHANGE_THRES 15
#define PR_NUM_SNAMECHANGE_THRES 3
#define PR_TIME_SNAME_INHIBITED 30

#define PR_CMD_DEFAULT_MUTE_TIME 20
#define PR_CMD_DEFAULT_CUFF_TIME 10

/*===============================================================================================

EXPLANATION OF HOW THE ENTITY FIELDS ARE USED (thnx? np.. :P)
---------------------------------------------

For player entity:
------------------

.option2		- Controls the flow of messages displayed to client (some messages skip this check)
.ex_skill_max	- Controls the flow of client commands (avoids multiple commands to be executed)
.ex_skill_min	- Counter of the times the user attempted to be admin by supplying a bad password
.t_s_h; 		- Monster 1 name string (string using permanent buffer) (#sname1)
.t_s_m; 		- Monster 2 name string (string using permanent buffer) (#sname2)
.t_s_c; 		- Monster 3 name string (string using permanent buffer) (#sname3)
.increase_team1 - Holds number of custom map voting requests this user have made
.increase_team2 - Number of monster name changes this user has performed consecutively
.increase_team3 - Time of silent monster name changes for user or time of last change

================================================================================================

 Commands Structure:
 -------------------

help
  (command)

admin
  login password
  logout
  online (shows logged admins)
  tell

  // the following are only accessible by a current admin
  ceasefire [1/on|0/off] (if no argument, just toggle)
  get [user]
  kick [user]
  ban [user]
  mute [user [time]]
  curse [user]
  team 1|2|3|4 [user]
  cmnd "command/s" [user]
  TODO: getalias
  poll                                  // PZ: Starts a yes or no vote. Reports results. Does no further action.
  bots                                  // PZ: Used to set the ideal number of players that bots will try to maintain.
  waypoints                             // PZ: Used to turn on the bot waypoint editor.

  // exist but never shown on help, even if an admin    // PZ: why??
  debug
  infos
  daylight
  storm
  earthquake
  daytime
  runes
  killstuff
  endgame
  silence
  use							        // Gizmo - uses the entity that the admin is looking at
  spawn <classname> [team]			    // Gizmo - spawns an entity, usually a monster in coop mode
  mass_spawn <classname> [count] [team]	// Gizmo - spawns count entities (4 is default if not specified)
  kill						            // Gizmo - kills the entity that the admin is looking at (if allowed)

  stuff
	makefree ITEM PAGE|all
	delfree ITEM PAGE|all
	disable ITEM PAGE|all
	enable ITEM PAGE|all

vote
  map xxxxxx
	neo // Neo mode
	agr         // PZ - Attackers Go Red mode
	invade      // PZ - Invade mode
	tf	        // Gizmo - Team Fortress (non-coop) mode
	coop	    // Gizmo - Co-op mode
	insane_coop	// Gizmo - Insane coop mode
	dm x        // PZ TODO - Deathmatch (if x is supplied, have that many teams; otherwise, no teams)
  nextmap xxxx
	agr         // PZ - Attackers Go Red mode
	invade      // PZ - Invade mode
	neo         // PZ - Neo mode
	tf	        // Gizmo - Team Fortress (non-coop) mode
	coop	    // Gizmo - Co-op mode
	insane_coop	// Gizmo - Insane coop mode
	dm x        // PZ TODO - Deathmatch (if x is supplied, have that many teams; otherwise, no teams)
  kick xxxxx
  curse xxxx
  ban xxxxx
  mute xxxx
  use							// Gizmo - vote to use an object

Cheater xxxxx "reasons for accusing.."

game
	//summon xxxxx
	sname x monstername
	build xxxxxxx
	fgens [1/on|0/off] (if no argument just toggle)
	//det x
	//disguise x
	//spyskin x
	drop
		items
		runes
		rune x
		cells x
		rockets x
		nails x
		shells x
	weapon weaponname		// Gizmo

==========================================================================================*/

float validatemap(string mapstr);
float DisableItem(float itemnum, float page, float permanent);
float EnableItem(float itemnum, float page, float permanent);

float MakeFree(float itemnum,float page, float permanent);
float DelFree(float itemnum,float page, float permanent);

// External functions
void UpdateRuneStuff(float oldrunes);
float DropSpecificRune(entity player, float runenum);

//======================================================================================
// The following is used to crash the server, yeah
// I wanted to see server crashing with ents >512 error to test crash logging on qwsv

#ifdef PR_CRASH_TEST
void CrashThink()
{
	newmis = spawnServerSide(); // PZ: make it a server-side only entity
	self->nextthink = time + 0.1;
}

void StartCrash()
{
	entity ent;
	ent = spawnServerSide(); // PZ: make it a server-side only entity
	ent->think = CrashThink;
	ent->nextthink = time + 0.1;
}
#endif

//====================================================================================
// To avoid problems, error and help messages on client commands have a limited rate

void CmdMsg(const string& text)
{
	if (self->option2 > time)
		return;

	self->option2 = time + PR_CMD_MSGRATE;
	sprint(self,PR_PRINT_HIGH,text);
}

// Possible help command requests:

#define PR_CMDHELP_DEFAULT              0 // displays all available commands

#define PR_CMDHELP_ADMIN                1 // admin commands
	// Any player allowed admin commmands
	#define PR_CMDHELP_ADMIN_LOGIN      2
	#define PR_CMDHELP_ADMIN_LOGOUT     3
	#define PR_CMDHELP_ADMIN_ONLINE     4
	#define PR_CMDHELP_ADMIN_TELL       5
	// Admin-only allowed admin commmands
	#define PR_CMDHELP_ADMIN_CEASEFIRE  16
	#define PR_CMDHELP_ADMIN_GET        17
	#define PR_CMDHELP_ADMIN_KICK       18
	#define PR_CMDHELP_ADMIN_BAN        19
	#define PR_CMDHELP_ADMIN_MUTE       20
	#define PR_CMDHELP_ADMIN_CUFF       21
	#define PR_CMDHELP_ADMIN_CURSE      22
	#define PR_CMDHELP_ADMIN_TEAM       23
	#define PR_CMDHELP_ADMIN_CMND       24
	#define PR_CMDHELP_ADMIN_ENDGAME    25
	#define PR_CMDHELP_ADMIN_CHECK      26
	#define PR_CMDHELP_ADMIN_POLL       27  // PZ: generic player polling
	#define PR_CMDHELP_ADMIN_BOTS       28  // PZ: ideal number of players
	#define PR_CMDHELP_ADMIN_WAYPOINTS  29  // PZ: waypoint editor

// Voting
#define PR_CMDHELP_VOTE                 32
	#define PR_CMDHELP_MAP              33
	#define PR_CMDHELP_NEXTMAP          34
	#define PR_CMDHELP_KICK             35
	#define PR_CMDHELP_BAN              36
	#define PR_CMDHELP_MUTE             37
	#define PR_CMDHELP_CURSE            38
	#define PR_CMDHELP_CUFF             39

// Misc
#define PR_CMDHELP_CHEATER              64

// Game commands
#define PR_CMDHELP_GAME                 128
	#define PR_CMDHELP_BUILD            129
	//#define CMDHELP_SUMMON            130
	#define PR_CMDHELP_SNAME            131
	#define PR_CMDHELP_DROP             132
	#define PR_CMDHELP_WEAPON           133 // Gizmo

// Special commands
#define PR_CMDHELP_SPECIAL              160
	#define PR_CMDHELP_GETLIT           161


//=========================================================================
// Displays help about specific command or subcommand

// Gizmo - modified this function to not use strcat() so we can have exceptionally long strings
void CmdHelpParse(const string& cmdstr, const string& helpstr, string cmdlist, const string& finalstr)
{
	// this function doesn't use CmdMsg()
	if (self->option2 > time)
		return;

	self->option2 = time + PR_CMD_MSGRATE;

	if (cmdstr != "")
	{
		sprint( self, PR_PRINT_HIGH, "Command \xA2" );
		sprint( self, PR_PRINT_HIGH, cmdstr );
		sprint( self, PR_PRINT_HIGH, "\xA2 description:\n" );
	}

	sprint( self, PR_PRINT_HIGH, helpstr );

	if (cmdlist != "")
	{
		sprint( self, PR_PRINT_HIGH, "[" );

		cmdlist = colstr( cmdlist, PR_COLSTR_RED );		// may not color the string if too long
		sprint( self, PR_PRINT_HIGH, cmdlist );

		sprint( self, PR_PRINT_HIGH, "]\n" );
	}

	if (finalstr != "")
	{
		sprint( self, PR_PRINT_HIGH, finalstr );
	}
}

#define PR_CMDS "Help Game Vote Admin Special Cheater"
//#define CMDS_ADMIN_ADMIN "LogOut OnLine Get Team Kick Ban Mute Cuff CeaseFire Poll" // PZ: added Poll option
// PZ: made all admin commands visible in help
#define PR_CMDS_ADMIN_ADMIN "Ban Bots CeaseFire Check Cmnd Cuff Curse Daylight Daytime Debug Earthquake EndGame Get Infos Kick Kill KillStuff Light LogOut Mass_Spawn Mute OnLine Poll Runes Sermon Silence Spawn Storm Stuff Team Tell Thunder Use Waypoints"
#define PR_CMDS_ADMIN_USER "OnLine Tell LogIn LogOut"
#define PR_CMDS_BUILD "Sentry Tesla Camera Sensor Teleporter Dispenser FieldGen"
#define PR_CMDS_GAME "Drop SName Build Weapon" // summon
#define PR_CMDS_WEAPON "Hook Medikit Spanner Axe Sniper AutoRifle Shotgun SuperShotgun Nailgun LightAssault Grenade Pipe Flame Rocket Cluster Incendiary Assault Lightning Daedalus Tranq Railgun Mauser Airfist LaserCannon SNG ZeroGrav"
#define PR_CMDS_VOTE "Map NextMap Kick Ban Mute Curse Cuff"
#define PR_CMDS_SPECIAL "GetLit GetSky GetHRT"

void CmdHelp(float command)
{
	if (command == PR_CMDHELP_DEFAULT)
		CmdHelpParse("","Possible client commands are\xBA\n",PR_CMDS,"To get help on specific commands just type \xA2""cmd help COMMAND\xA2\n");
	else if (command == PR_CMDHELP_BUILD)
		CmdHelpParse("Build","Builds an item if you are able to, possible items are:\n",PR_CMDS_BUILD,"");
	else if ( command == PR_CMDHELP_WEAPON )
		CmdHelpParse( "Weapon", "Switches to the specified weapon if available, possible weapons are:\n", PR_CMDS_WEAPON, "" );
	/*else if (command == #CMDHELP_SUMMON)
		CmdMsg("Command \{162}Summon\{162} Description\{186}\nSummons a monster or teleports a soldier if you are able to, the parameter for this command should be one of:\n(\bScrag Piranha Fiend Shambler Soldier\b)\n");
	*/
	else if (command == PR_CMDHELP_ADMIN)
	{
		if (self->admin_flag)
			CmdHelpParse("Admin","Performs several admin actions, this command requires a subcommand to be one of:\n",PR_CMDS_ADMIN_ADMIN,"");
		else
			CmdHelpParse("Admin","Admin related actions, this command requires a subcommand to be one of:\n",PR_CMDS_ADMIN_USER,"You must log in to use the other admin commands.\n");
	}
	else if (command == PR_CMDHELP_ADMIN_LOGIN)
		CmdHelpParse("LogIn","Logs in as an administrator, you must supply a valid password.\n","","");
	else if (command == PR_CMDHELP_ADMIN_LOGOUT)
		CmdHelpParse("LogOut","Logs out as an administrator.\n","","");
	else if (command == PR_CMDHELP_ADMIN_ONLINE)
		CmdHelpParse("OnLine","Displays list of current admins that logged on server.\n","","");
	else if (command == PR_CMDHELP_ADMIN_TELL)
		CmdHelpParse("Tell","Tells a private message to all admins logged on server, if any.\nSyntax\xBA cmd admin tell \"message to admins\"\n","","");
	else if (command == PR_CMDHELP_ADMIN_CEASEFIRE)
		CmdHelpParse("CeaseFire","Forces a ceasefire on the server, players will not be able to attack until ceasefire ends.\nSyntax\xBA cmd admin ceasefire [on|off]\n","","");
	else if (command == PR_CMDHELP_ADMIN_TEAM)
		CmdHelpParse("Team","Assign given player to the specified team.\nSyntax\xBA cmd admin team n [user]\n","","");
	else if (command == PR_CMDHELP_ADMIN_CMND)
		CmdHelpParse("Cmnd","Executes given commands for the currently selected or specified client.\nSyntax\xBA cmd admin cmnd \"commands\" [user]\n","","");
	else if (command == PR_CMDHELP_ADMIN_GET)
		CmdHelpParse("Get","Tries to select the given client as target for admin actions, if no parameter is supplied just cycles thru possible clients.\nSyntax\xBA cmd admin get [user]\n","","");
	else if (command == PR_CMDHELP_ADMIN_KICK)
		CmdHelpParse("Kick","Kicks given client, if none specified, currently selected one.\nSyntax\xBA cmd admin kick [user]\n","","");
	else if (command == PR_CMDHELP_ADMIN_BAN)
		CmdHelpParse("Ban","Bans given client from server, if none is specified uses currently selected one.\nSyntax\xBA cmd admin ban [user]\n","","");
	else if (command == PR_CMDHELP_ADMIN_MUTE)
		CmdHelpParse("Mute","Mutes a client, if none is specified uses currently selected one.\nSyntax\xBA cmd admin mute [user [time]]\n","","");
	else if (command == PR_CMDHELP_ADMIN_CUFF)
		CmdHelpParse("Cuff","Cuffs a client for the specified time.\nSyntax\xBA cmd admin cuff [user [time]]\n","","");
	else if (command == PR_CMDHELP_ADMIN_CURSE)
		CmdHelpParse("Curse","Curses given player, if none is specified uses currently selected one.\nSyntax\xBA cmd admin curse [user]\n","","");
	else if (command == PR_CMDHELP_ADMIN_ENDGAME)
		CmdHelpParse("EndGame","Ends the current map.\n","","");
	else if (command == PR_CMDHELP_ADMIN_CHECK)
		CmdHelpParse("Check","Prints information about the specified client.\n","","");
	else if (command == PR_CMDHELP_ADMIN_POLL) // PZ
		CmdHelpParse("Poll","Polls all players on any given yes or no question.\nSyntax\xBA cmd admin poll \"<question>\"\n","","");
	else if (command == PR_CMDHELP_ADMIN_BOTS) // PZ
	{
		string h, temp;
		h = infokey(world, "idealp");
		temp = "Sets the ideal number of players that bots will maintain.\nSyntax\xBA cmd admin bots <player count>\n";
		temp = strcat(temp, "The current number is set to ");
		temp = strcat(temp, h);
		temp = strcat(temp, ".\n");
		CmdHelpParse("Bots",temp,"","");
	}
	else if (command == PR_CMDHELP_ADMIN_WAYPOINTS) // PZ
		CmdHelpParse("Waypoints","Turns the bot waypoint editor on and off (toggles).\nSyntax\xBA cmd admin waypoints\n","","");
	else if (command == PR_CMDHELP_CHEATER)
		CmdHelpParse("Cheater","Accuses given player of cheating, warning other users and logging it on server.\nSyntax\xBA cmd cheater (user) [\"Reasons for accusation\"]\n","","");
	else if (command == PR_CMDHELP_VOTE)
		CmdHelpParse("Vote","Commands for starting several kind of votes.\nAvailable vote types are:\n",PR_CMDS_VOTE,"");
	else if (command == PR_CMDHELP_MAP)
		CmdHelpParse("Map",S_("Starts voting for a switch to the specified map.\nSyntax\xBA cmd vote map (MapName) [agr|invade|neo|tf|coop|insane_coop]\n"),"","");
	else if (command == PR_CMDHELP_NEXTMAP)
		CmdHelpParse("NextMap",S_("Suggests the specified map as next map.\nSyntax\xBA cmd vote nextmap (MapName) [agr|invade|neo|tf|coop|insane_coop]\n"),"","");
	else if (command == PR_CMDHELP_KICK)
		CmdHelpParse("Kick","Proposes to kick the specified user from server.\nSyntax\xBA cmd vote kick (user)\n","","");
	else if (command == PR_CMDHELP_CURSE)
		CmdHelpParse("Curse","Proposes to curse the specified player.\nSyntax\xBA cmd vote curse (user)\n","","");
	else if (command == PR_CMDHELP_BAN)
		CmdHelpParse("Ban","Proposes to ban the specified user from server.\nSyntax\xBA cmd vote ban (user)\n","","");
	else if (command == PR_CMDHELP_MUTE)
		CmdHelpParse("Mute","Proposes to mute the specified client.\nSyntax\xBA cmd vote mute (user)\n","","");
	else if (command == PR_CMDHELP_CUFF)
		CmdHelpParse("Cuff","Proposes to cuff the specified player.\nSyntax\xBA cmd vote cuff (user)\n","","");
	else if (command == PR_CMDHELP_GAME)
		CmdHelpParse("Game","Several in-game related commands.\nAvailable commands are:\n",PR_CMDS_GAME,"");
	else if (command == PR_CMDHELP_SNAME)
		CmdHelpParse("SName","Displays or changes the custom names for summons and army soldiers.\nSyntax\xBA cmd game sname n [\"CustomName\"]\n","","");
	else if (command == PR_CMDHELP_DROP)
		CmdHelpParse("Drop","Drops items, runes or ammo. Depending on what to be dropped a number should be specified.\nSyntax\xBA cmd game drop (items|runes|rune n|shells n|nails n|rockets n|cells n)\n","","");
	else if (command == PR_CMDHELP_SPECIAL)
		CmdHelpParse("Special","Several special actions, a subcommand must be one of:\n",PR_CMDS_SPECIAL,"");
	else if (command == PR_CMDHELP_GETLIT)
		CmdHelpParse("GetLit","Attempts to download a coloured lighting file for current map, if it exists.\n","","");
}

//========================================================================
// Reports a bad argument error

void CmdBadArg(const string& text1, const string& badarg, const string& text2)
{
	string tmp;

	tmp = strcat(text1, badarg);
	tmp = strcat(tmp,text2);

	CmdMsg(tmp);
}

//========================================================================
// Reports an invalid string as target client for admin or vote command

void CmdClientErr(const string& text)
{
	sprint(self,PR_PRINT_HIGH,"Can't find an unique match for \xA2");
	sprint(self,PR_PRINT_HIGH,text);
	sprint(self,PR_PRINT_HIGH,"\xA2!\nYou must specify an user ID or name substring.\n");
}

//==============================================================
// Returns TRUE if self should be able to start a map vote

float HisMapVoteAllowed()
{
	if (self->goal_no > time)
	{
		CmdMsg("You just have ran a vote!\n");
		return PR_FALSE;
	}

	if (current_voteent != world)
	{
		CmdMsg("Already running a vote!\n");
		return PR_FALSE;
	}

	if (time/60 < PR_MIN_MINUTES_TOVOTEMAP)
#ifdef PR_COOP_MODE_ENHANCED
	if ( deathmatch || mapname != "start" )
#endif
	{
		CmdMsg("Too early for a map change!\n");
		return PR_FALSE;
	}

	if (ServerUpdating())
	{
		CmdMsg("Server update is taking place, no map changes are allowed!\n");
		return PR_FALSE;
	}

	return PR_TRUE;
}

//==============================================================
// Returns TRUE if self has a "punisheable" valid client

float HisPunishmentAllowed()
{
	if (self->goal_no > time)
	{
		CmdMsg("You just have ran a vote!\n");
		return PR_FALSE;
	}
	else if (current_voteent != world)
	{
		CmdMsg("Already running a vote!\n");
		return PR_FALSE;
	}
	else if (self->admin_kick == world)
	{
		CmdMsg("No user selected!\n");
		return PR_FALSE;
	}
	else if (!self->admin_kick->is_connected)
	{
		CmdMsg("User has left the server!\n");
		return PR_FALSE;
	}
	#ifndef PR_VOTING_TEST
	else if (self->admin_kick == self)
	{
		CmdMsg("Are you trying to punish yourself? weird..\n");
		self->admin_kick = world;
		return PR_FALSE;
	}
	#endif
	else if (self->g_a == PR_PUNISH_CURSE)
	{
		if (self->admin_kick->classname=="spec")
		{
			CmdMsg("Can't curse spectators!\n");
			return PR_FALSE;
		}
		else if (self->admin_kick->playerclass == PR_PC_UNDEFINED)
		{
			CmdBadArg("Player ",self->admin_kick->netname," is observing the game!\n");
			return PR_FALSE;
		}
		else if (self->admin_kick->done_custom & PR_CUSTOM_BUILDING)
		{
			CmdBadArg("Player ",self->admin_kick->netname," is still customizing!\n");
			return PR_FALSE;
		}
	}
	else if (self->g_a == PR_PUNISH_CUFF)
	{
		if (self->admin_kick->classname=="spec")
		{
			CmdMsg("Can't cuff spectators!\n");
			return PR_FALSE;
		}
	}

	if (self->admin_kick->classname != "spec" && self->admin_kick->classname != "player")
	{
		CmdMsg("Invalid user selected!\n");
		return PR_FALSE;
	}

	return PR_TRUE;
}

//==============================================================
// Generic is vote allowed function

float IsVoteAllowedGeneric() {
	if ( current_voteent != world ) {
		CmdMsg( "Already running a vote!\n" );
		return PR_FALSE;
	}
	return PR_TRUE;
}

//===================================================================
// This is the anti-spam mechanism used for monster name changes
// Also used for anti-spam on "cheater" commands

float MonsterNameAllowed()
{
	if (self->increase_team3 < time)
	{
		if (time - self->increase_team3 < PR_SECS_SNAMECHANGE_THRES)
		{
			self->increase_team2 = self->increase_team2 +1;
			if (self->increase_team2 >= PR_NUM_SNAMECHANGE_THRES)
				self->increase_team3 = time + PR_TIME_SNAME_INHIBITED;
		}
		else
		{
			self->increase_team2 = 0;
			self->increase_team3 = time;
		}

		return PR_TRUE;
	}
	else
	{
		CmdMsg("Don't spam the server please!\n");
		return PR_FALSE;
	}
}

//===========================================================================================
// A client just typed a command and server hasn't found a regular command handler for it
// If this function returns 0 (FALSE), unhandling is assumed, and "Bad user command" message is shown
// It should return a non-zero value (like TRUE) if it processes the command, even with errors,
// which should be reported by this function itself

bool ClientCommand(int num_args, const string& arg1, const string& arg2, const string& arg3, const string& arg4, const string& arg5, const string& arg6, const string& arg7)
{
	// First of all, check if we allow a client command from this user right now..
	if (self->ex_skill_max > time)
		return PR_TRUE; // dont report anything!

	if (self->flags & PR_FL_FINALIZED)
		return PR_TRUE;

	if (intermission_running)
	{
		CmdMsg("No client commands are allowed during an intermission!\n");
		return PR_TRUE;
	}

	if (!self->is_connected)
		return PR_FALSE;

	if (self->classname != "player" && self->classname != "spec")
		return PR_TRUE;

	if (num_args <= 0) //NOT NEEDED
		return PR_FALSE;

	#ifdef PR_CRASH_TEST
	if (!strcasecmp(arg1,"crash"))
	{
		sprint(self,PR_PRINT_HIGH,"Crashing server... just a few seconds :P\n");
		StartCrash();
		return PR_TRUE;
	}
	#endif

	entity targ;
	string st;
	float tmpf, tmpf2;

	// Player/Spectator valid commands:
	if (!strcasecmp(arg1,"admin")) // ADMIN
	{
		// No arguments
		if (num_args < 2)
		{
			CmdHelp(PR_CMDHELP_ADMIN);
			return PR_TRUE;
		}

		// Any-player admin commands
		if (!strcasecmp(arg2,"login")) // ADMIN - LOGIN
		{
			// Check if client is trying to hack the admin password
			if (self->ex_skill_min >= PR_CMD_MAX_ADMINATTEMPTS)
			{
				CmdMsg("Go away, hacker wannabe!\n");
				return PR_TRUE;
			}

			// No argument
			if (num_args < 3)
			{
				CmdHelp(PR_CMDHELP_ADMIN_LOGIN);
				return PR_TRUE;
			}

			st = infokey(world,"adminpwd");

			// If no adminpwd is set, abort and report it
			if (st == "")
			{
				CmdMsg("Unable to log in as admin, no password has been set!\n");
				return PR_TRUE;
			}

			if (arg3 == st)
			{
				if (self->admin_flag)
				{
					CmdMsg("Already logged as admin!\n");
					return PR_TRUE;
				}
				else
				{
					PrintAdmins(PR_FALSE,PR_TRUE); // show our colleagues, if any
					sprint(self,PR_PRINT_HIGH,"Login\x8D You logged in as admin.\n");
					PlayClientSound(self,"doors/baseuse.wav");
					AdminLoggedIn(self);
					self->admin_flag = PR_TRUE;
					return PR_TRUE;
				}
			}
			else
			{
				CmdMsg("Login\x8D Bad admin password!\n");
				PlayClientSound(self,"doors/basetry.wav");

				if (!self->admin_flag) // Not an admin?
					self->ex_skill_max = time + 5; // then do not allow another attempt for x seconds from this user
				else // he was an admin lol
					AdminLoggedOut(self);

				// increase our hacker detection counter :)
				self->ex_skill_min = self->ex_skill_min + 1;

				self->admin_flag = PR_FALSE;
				return PR_TRUE;
			}
		}
		else if (!strcasecmp(arg2,"logout")) // ADMIN - LOGOUT
		{
			if (self->admin_flag)
			{
				sprint(self,PR_PRINT_HIGH,"You logged out as admin.\n");
				self->admin_flag = PR_FALSE;
				AdminLoggedOut(self);
				return PR_TRUE;
			}
			else
			{
				CmdMsg("Not logged as admin!\n");
				return PR_TRUE;
			}
		}
		else if (!strcasecmp(arg2,"online")) // ADMIN - Show online admins
		{
			if (self->option2 < time)
			{
				PrintAdmins(PR_TRUE,PR_FALSE);
				self->option2 = time + PR_CMD_MSGRATE;
			}

			return PR_TRUE;
		}
		else if (!strcasecmp(arg2,"tell"))
		{
			if (num_args == 2)
			{
				CmdHelp(PR_CMDHELP_ADMIN_TELL);
				return PR_TRUE;
			}

			if (num_args > 3)
			{
				CmdMsg("Use your message text inside \"quotes like this\"!\n");
				return PR_TRUE;
			}

			Admin_Tell(self,arg3);
			return PR_TRUE;
		}

		// Admin-only admin commands
		if (self->admin_flag)
		{
			if (mutedtime(self))
			{
				CmdMsg("You have been muted being an admin. Now think about why...\n");
				return PR_TRUE;
			}

			if (!strcasecmp(arg2,"get"))
			{
				if (num_args == 2)
				{
					Admin_Kick_Cycle();
					return PR_TRUE;
				}

				targ = getclient(arg3);

				if (targ != world)
				{
					self->admin_kick = targ; //Set current selected person

					if (self->admin_kick->classname == "spec")
						sprint(self, PR_PRINT_HIGH, S_("^bClient (^bSpectator^b)^b: "));
					else
						sprint(self, PR_PRINT_HIGH, S_("^bClient^b: "));

					sprint(self, PR_PRINT_HIGH, self->admin_kick->netname);
					sprint(self, PR_PRINT_HIGH, S_(" selected ^b(^b"));

					st = ftos(getuid(self->admin_kick));
					st = colstr(st,PR_COLSTR_NUMBER);

					sprint(self, PR_PRINT_HIGH, S_("^bUserID^b\xBA"),st,S_(" ^bIP^b\xBA"));
					st = infokey(self->admin_kick,"ip");
					st = colstr(st,PR_COLSTR_NUMBER);
					sprint(self,PR_PRINT_HIGH,st,S_("^b)^b\n"));

					return PR_TRUE;
				}
				else
				{
					CmdClientErr(arg3);
					return PR_TRUE;
				}
			}
			else if (!strcasecmp(arg2,"debug"))
			{
				MakeMeDebug(self);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg2,"infos"))
			{
				UpdateInfos(PR_FALSE);
				if (debug_target!=self) sprint(self,PR_PRINT_HIGH,"Localinfos updated succesfully.\n");
				return PR_TRUE;
			}
			else if (!strcasecmp(arg2,"light"))
			{
				st = ftos(current_brightness);

				sprint(self,PR_PRINT_HIGH,"Current Brightness = ",st,"\n");
				return PR_TRUE;
			}
			else if (!strcasecmp(arg2,"endgame"))
			{
				Admin_EndGame();
				return PR_TRUE;
			}
			else if (!strcasecmp(arg2,"killstuff"))
			{
				if (num_args == 2)
				{
					Admin_KillStuff();
					return PR_TRUE;
				}

				targ = getclient(arg3);
				if (targ != world)
				{
					self->admin_kick = targ;
					Admin_KillStuff();
					return PR_TRUE;
				}
				else
				{
					CmdClientErr(arg3);
					return PR_TRUE;
				}
			}
			#ifdef PR_STORM_TEST
			else if (!strcasecmp(arg2,"thunder"))
			{
				StormSpecialEvent(self);
				return PR_TRUE;
			}
			#endif
			else if (!strcasecmp(arg2,"storm"))
			{
				if (num_args == 2)
				{
					st = ftos(storm);

					sprint(self,PR_PRINT_HIGH,"Storm setting is ",st,"\n");
					return PR_TRUE;
				}

				tmpf = storm;

				storm = stof(arg3);

				if (storm < 0 || storm > 1)
				{
					sprint(self,PR_PRINT_HIGH,"Invalid storm setting!\n");
					storm = tmpf;
					return PR_TRUE;
				}

				if (storm != tmpf)
				{
					sprint(self,PR_PRINT_HIGH,"New storm setting is applied\n");
					UpdateEnvironment(tmpf,earthquake,daytime,runes);
					return PR_TRUE;
				}

				sprint(self,PR_PRINT_HIGH,"No change on storm setting!\n");
				return PR_TRUE;
			}
			else if (!strcasecmp(arg2,"earthquake"))
			{
				if (num_args == 2)
				{
					st = ftos(earthquake);

					sprint(self,PR_PRINT_HIGH,"Earthquake setting is ",st,"\n");
					return PR_TRUE;
				}

				tmpf = earthquake;

				earthquake = stof(arg3);

				if (earthquake < 0 || earthquake > 1)
				{
					sprint(self,PR_PRINT_HIGH,"Invalid earthquake setting!\n");
					storm = tmpf;
					return PR_TRUE;
				}

				if (earthquake != tmpf)
				{
					sprint(self,PR_PRINT_HIGH,"New earthquake setting is applied\n");
					UpdateEnvironment(storm,tmpf,daytime,runes);
					return PR_TRUE;
				}

				sprint(self,PR_PRINT_HIGH,"No change on earthquake setting!\n");
				return PR_TRUE;
			}
			else if (!strcasecmp(arg2,"runes"))
			{
				if (num_args == 2)
				{
					st = ftos(runes);

					sprint(self,PR_PRINT_HIGH,"Runes setting is ",st,"\n");
					return PR_TRUE;
				}

				// Neo mode: don't allow admins to spawn runes
				if (neo.isModeActive())
				{
					sprint(self, PR_PRINT_HIGH, "Rune-spawning command is disabled in Neo mode.\nIn Neo mode, it doesn't work right.\n");
					return true;
				}
				tmpf = runes;

				runes = floor(stof(arg3));

				if (runes < 0 || runes > 4)
				{
					sprint(self,PR_PRINT_HIGH,"Invalid runes setting! (0-4 allowed)\n");
					runes = tmpf;
					return PR_TRUE;
				}

				if (runes != tmpf)
				{
					sprint(self,PR_PRINT_HIGH,"New runes setting is applied\n");
					UpdateEnvironment(storm,earthquake,daytime,tmpf);
					return PR_TRUE;
				}

				sprint(self,PR_PRINT_HIGH,"No change on the runes setting!\n");
				return PR_TRUE;
			}
			else if (!strcasecmp(arg2,"daytime"))
			{
				if (num_args == 2)
				{
					st = ftos(daytime);

					sprint(self,PR_PRINT_HIGH,"DayTime setting is ",st,"\n");
					return PR_TRUE;
				}

				tmpf = daytime;

				daytime = stof(arg3);

				if (daytime < 0 || daytime > 360)
				{
					sprint(self,PR_PRINT_HIGH,"Invalid daytime setting!\n");
					daytime = tmpf;
					return PR_TRUE;
				}

				if (daytime != tmpf)
				{
					sprint(self,PR_PRINT_HIGH,"New DayTime setting is applied\n");
					UpdateEnvironment(storm,earthquake,tmpf,runes);
					return PR_TRUE;
				}

				sprint(self,PR_PRINT_HIGH,"No change on DayTime setting!\n");
				return PR_TRUE;
			}
			else if (!strcasecmp(arg2,"daylight"))
			{
				if (daytime != 0)
				{
					sprint(self,PR_PRINT_HIGH,"Unable to adjust brighness with DayTime enabled!\n");
					return PR_TRUE;
				}

				if (num_args == 2)
				{
					st = ftos(daylight);
					sprint(self,PR_PRINT_HIGH,"Daylight intensity is ",st,"\n");
					return PR_TRUE;
				}

				tmpf = stof(arg3);
				tmpf = rint(tmpf);

				if (tmpf < 0 || tmpf > 25)
				{
					sprint(self,PR_PRINT_HIGH,"Valid daylight value ranges from 0 to 25!\n");
					return PR_TRUE;
				}

				targ = FadeWorld(tmpf,0,0,0);

				if (targ != world)
					targ->has_teleporter = 3; // Set daylight along with fade

				return PR_TRUE;
			}
			else if (!strcasecmp(arg2,"silence"))
			{
				if (num_args == 2)
				{
					Admin_Silence(0,PR_DEFAULT_SILENCE_TIME);
					return PR_TRUE;
				}

				tmpf = stof(arg3);

				if (num_args == 3)
				{
					Admin_Silence(0,tmpf);
					return PR_TRUE;
				}

				if (!strcasecmp(arg4,"players"))
				{
					Admin_Silence(1,tmpf);
					return PR_TRUE;
				}
				else if (!strcasecmp(arg4,"specs") || !strcasecmp(arg4,"spectators"))
				{
					Admin_Silence(2,tmpf);
					return PR_TRUE;
				}

				sprint(self,PR_PRINT_HIGH,"Bad syntax!\n");
				return PR_TRUE;
			}
			else if (!strcasecmp(arg2,"cuff"))
			{
				st = ftos(PR_CMD_DEFAULT_CUFF_TIME);

				if (num_args == 2)
				{
					Admin_Cuff(st);
					return PR_TRUE;
				}

				targ = getclient(arg3);
				if (targ != world)
				{
					self->admin_kick = targ;

					if (num_args == 3)
						Admin_Cuff(st);
					else
						Admin_Cuff(arg4);

					return PR_TRUE;
				}
				else
				{
					CmdClientErr(arg3);
					return PR_TRUE;
				}
			}
			else if (!strcasecmp(arg2,"sermon"))
			{
				if (num_args < 4)
				{
					sprint(self,PR_PRINT_HIGH,"Sermon command syntax is: cmd admin sermon (player) \"Sermon text\" [kick|ban]\n");
					return PR_TRUE;
				}

				targ = getclient(arg3);
				if (targ != world)
				{
					self->admin_kick = targ;

					if (num_args > 4)
					{
						if (!strcasecmp(arg5,"ban"))
							Admin_Sermon(arg4,2);
						else if (!strcasecmp(arg5,"kick"))
							Admin_Sermon(arg4,1);
						else
							Admin_Sermon(arg4,0);
					}
					else
						Admin_Sermon(arg4,0);

					return PR_TRUE;
				}
				else
				{
					CmdClientErr(arg3);
					return PR_TRUE;
				}
			}
			else if (!strcasecmp(arg2,"mute"))
			{
				st = ftos(PR_CMD_DEFAULT_MUTE_TIME);

				if (num_args == 2)
				{
					Admin_Mute(st);
					return PR_TRUE;
				}

				targ = getclient(arg3);
				if (targ != world)
				{
					self->admin_kick = targ;

					if (num_args == 3)
						Admin_Mute(st);
					else
						Admin_Mute(arg4);

					return PR_TRUE;
				}
				else
				{
					CmdClientErr(arg3);
					return PR_TRUE;
				}
			}
			else if (!strcasecmp(arg2,"curse"))
			{
				if (num_args == 2)
				{
					Admin_Curse();
					return PR_TRUE;
				}

				targ = getclient(arg3);
				if (targ != world)
				{
					self->admin_kick = targ;
					Admin_Curse();
					return PR_TRUE;
				}
				else
				{
					CmdClientErr(arg3);
					return PR_TRUE;
				}
			}
			else if (!strcasecmp(arg2,"kick"))
			{
				if (num_args == 2)
				{
					Admin_Kick_Person();
					return PR_TRUE;
				}

				targ = getclient(arg3);
				if (targ != world)
				{
					self->admin_kick = targ;
					Admin_Kick_Person();
					return PR_TRUE;
				}
				else
				{
					CmdClientErr(arg3);
					return PR_TRUE;
				}
			}
			else if (!strcasecmp(arg2,"ban"))
			{
				if (num_args == 2)
				{
					Admin_Ban_Person();
					return PR_TRUE;
				}

				targ = getclient(arg3);
				if (targ != world)
				{
					self->admin_kick = targ;
					Admin_Ban_Person();
					return PR_TRUE;
				}
				else
				{
					CmdClientErr(arg3);
					return PR_TRUE;
				}
			}
			else if (!strcasecmp(arg2,"ceasefire"))
			{
				// No argument.. so just toggle it
				if (num_args == 2)
				{
					Admin_Call_Ceasefire();
					return PR_TRUE;
				}

				if (arg3=="0" || !strcasecmp("off",arg3))
				{
					if (ceasefire)
					{
						Admin_Call_Ceasefire();
						return PR_TRUE;
					}
					else
					{
						sprint(self,PR_PRINT_HIGH,"Ceasefire is already off!\n");
						return PR_TRUE;
					}
				}
				else if (arg3=="1" || !strcasecmp("on",arg3))
				{
					if (!ceasefire)
					{
						Admin_Call_Ceasefire();
						return PR_TRUE;
					}
					else
					{
						sprint(self,PR_PRINT_HIGH,"Already on a Ceasefire!\n");
						return PR_TRUE;
					}
				}

				// bad ceasefire argument
				CmdBadArg(S_("^bCeaseFire^b: Bad parameter \xA2"),arg3,"\xA2 passed in!\nType \xA2""cmd help ceasefire\xA2 to see possible parameters.\n");
				return PR_TRUE;
			}
			else if (!strcasecmp(arg2,"team")) // Team assignment
			{
				// No argument
				if (num_args == 2)
				{
					CmdHelp(PR_CMDHELP_ADMIN_TEAM); // so show help
					return PR_TRUE;
				}

				// Neo mode: don't allow admins to assign teams to players
				if (neo.isModeActive())
				{
					sprint(self, PR_PRINT_HIGH, "Team assignment command is disabled in Neo mode.\nIn Neo mode, it doesn't work right and there should be no use for it.\n");
					return true;
				}
				// Check if valid team number parameter
				tmpf = stof(arg3);

				if (tmpf < 1 || tmpf > 4)
				{
					CmdBadArg(S_("^bTeam^b: Bad team number \xA2"),arg3,"\xA2 passed in!\nType \xA2""cmd help team\xA2 to see possible parameters.\n");
					return PR_TRUE;
				}

				// No client argument
				if (num_args == 3)
				{
					Admin_Assign(tmpf, PR_FALSE);
					return PR_TRUE;
				}

				// Parse Given client argument
				targ = getclient(arg4);
				if (targ != world)
				{
					self->admin_kick = targ;
					Admin_Assign(tmpf, PR_FALSE);
					return PR_TRUE;
				}
				else
				{
					CmdClientErr(arg4);
					return PR_TRUE;
				}
			}
			else if (!strcasecmp(arg2,"cmnd")) // CMND
			{
				// No argument
				if (num_args == 2)
				{
					CmdHelp(PR_CMDHELP_ADMIN_CMND); // so show help
					return PR_TRUE;
				}

				// No client argument
				if (num_args == 3)
				{
					Admin_Cmd(arg3);
					return PR_TRUE;
				}

				// Parse Given client argument
				targ = getclient(arg4);
				if (targ != world)
				{
					self->admin_kick = targ;
					Admin_Cmd(arg3);
					return PR_TRUE;
				}
				else
				{
					CmdClientErr(arg4);
					return PR_TRUE;
				}
			}
			else if (!strcasecmp(arg2,"check")) // Check
			{
				if (num_args == 2)
				{
					if (self->admin_kick == world)
					{
						CmdHelp(PR_CMDHELP_ADMIN_CHECK);
						return PR_TRUE;
					}
					else
					{
						Admin_Check();
						return PR_TRUE;
					}
				}

				// Parse Given client argument
				targ = getclient(arg3);
				if (targ != world)
				{
					self->admin_kick = targ;
					Admin_Check();
					return PR_TRUE;
				}
				else
				{
					CmdClientErr(arg3);
					return PR_TRUE;
				}

			}
			else if (!strcasecmp(arg2,"stuff")) // Stuff
			{
				if (num_args == 2)
				{
					sprint(self,PR_PRINT_HIGH,"Syntax: cmd admin stuff disable|enable|makefree|delfree (item) (page) [permanent]\n");
					return PR_TRUE;
				}

				if (!strcasecmp(arg3,"disable"))
				{
					if (num_args < 5)
					{
						sprint(self,PR_PRINT_HIGH,"You need to specify a page and item number!\n");
						return PR_TRUE;
					}

					tmpf = stof(arg4);
					tmpf2 = stof(arg5);

					if (tmpf2 < 1 || tmpf2 > 17 || tmpf < 1 || tmpf > 7)  // PZ: upped 15 to 17 (there are 17 item pages now)
					{
						sprint(self,PR_PRINT_HIGH,"Invalid page/item specified!\n");
						return PR_TRUE;
					}

					sprint(self,PR_PRINT_HIGH,"Item ",arg4," in page ",arg5," has been disabled");

					if (!(strcasecmp(arg6,"perm")) || !(strcasecmp(arg6,"permanent")))
					{
						sprint(self,PR_PRINT_HIGH," permanently");
						tmpf = DisableItem(tmpf,tmpf2,PR_TRUE);
					}
					else
						tmpf = DisableItem(tmpf,tmpf2,PR_FALSE);

					if (tmpf)
						sprint(self,PR_PRINT_HIGH,".\n");
					else
						sprint(self,PR_PRINT_HIGH," (no change).\n");

					return PR_TRUE;
				}
				else if(!strcasecmp(arg3,"enable"))
				{
					if (num_args < 5)
					{
						if (num_args == 4)
						if (!strcasecmp(arg4,"all"))
						{
							if (disabledstuff1 | disabledstuff2 | disabledstuff3 | disabledstuff4 | disabledstuff5)
							{
								disabledstuff1 = disabledstuff2 = disabledstuff3 = disabledstuff4 = disabledstuff5 = 0;
								sprint(self,PR_PRINT_HIGH,"All items have been enabled.\n");
							}
							else
								sprint(self,PR_PRINT_HIGH,"No items disabled currently!\n");

							return PR_TRUE;
						}

						sprint(self,PR_PRINT_HIGH,"You need to specify a page and item number!\n");
						return PR_TRUE;
					}

					tmpf = stof(arg4);
					tmpf2 = stof(arg5);

					if (tmpf2 < 1 || tmpf2 > 15 || tmpf < 1 || tmpf > 7)
					{
						sprint(self,PR_PRINT_HIGH,"Invalid page/item specified!\n");
						return PR_TRUE;
					}

					sprint(self,PR_PRINT_HIGH,"Item ",arg4," in page ",arg5," has been enabled");

					if (!(strcasecmp(arg6,"perm")) || !(strcasecmp(arg6,"permanent")))
					{
						sprint(self,PR_PRINT_HIGH," permanently");
						tmpf = EnableItem(tmpf,tmpf2,PR_TRUE);
					}
					else
						tmpf = EnableItem(tmpf,tmpf2,PR_FALSE);

					if (tmpf)
						sprint(self,PR_PRINT_HIGH,".\n");
					else
						sprint(self,PR_PRINT_HIGH," (no change).\n");

					return PR_TRUE;
				}
				else if(!strcasecmp(arg3,"makefree"))
				{
					if (num_args < 5)
					{
						sprint(self,PR_PRINT_HIGH,"You need to specify a page and item number!\n");
						return PR_TRUE;
					}

					tmpf = stof(arg4);
					tmpf2 = stof(arg5);

					if (tmpf2 < 1 || tmpf2 > 15 || tmpf < 1 || tmpf > 7)
					{
						sprint(self,PR_PRINT_HIGH,"Invalid page/item specified!\n");
						return PR_TRUE;
					}

					sprint(self,PR_PRINT_HIGH,"Item ",arg4," in page ",arg5," is now free");

					if (!(strcasecmp(arg6,"perm")) || !(strcasecmp(arg6,"permanent")))
					{
						sprint(self,PR_PRINT_HIGH," permanently");
						tmpf = MakeFree(tmpf,tmpf2,PR_TRUE);
					}
					else
						tmpf = MakeFree(tmpf,tmpf2,PR_FALSE);

					if (tmpf)
						sprint(self,PR_PRINT_HIGH,".\n");
					else
						sprint(self,PR_PRINT_HIGH," (no change).\n");

					return PR_TRUE;
				}
				else if(!strcasecmp(arg3,"delfree"))
				{
					if (num_args < 5)
					{
						if (num_args == 4)
						if (!strcasecmp(arg4,"all"))
						{
							if (givenstuff1 | givenstuff2 | givenstuff3 | givenstuff4 | givenstuff5)
							{
								givenstuff1 = givenstuff2 = givenstuff3 = givenstuff4 = givenstuff5 = 0;
								sprint(self,PR_PRINT_HIGH,"All free items removed.\n");
							}
							else
								sprint(self,PR_PRINT_HIGH,"No free items currently!\n");

							return PR_TRUE;
						}

						sprint(self,PR_PRINT_HIGH,"You need to specify a page and item number!\n");
						return PR_TRUE;
					}

					tmpf = stof(arg4);
					tmpf2 = stof(arg5);

					if (tmpf2 < 1 || tmpf2 > 15 || tmpf < 1 || tmpf > 7)
					{
						sprint(self,PR_PRINT_HIGH,"Invalid page/item specified!\n");
						return PR_TRUE;
					}

					sprint(self,PR_PRINT_HIGH,"Item ",arg4," in page ",arg5," has been removed from free items");

					if (!(strcasecmp(arg6,"perm")) || !(strcasecmp(arg6,"permanent")))
					{
						sprint(self,PR_PRINT_HIGH," permanently");
						tmpf = DelFree(tmpf,tmpf2,PR_TRUE);
					}
					else
						tmpf = DelFree(tmpf,tmpf2,PR_FALSE);

					if (tmpf)
						sprint(self,PR_PRINT_HIGH,".\n");
					else
						sprint(self,PR_PRINT_HIGH," (no change).\n");

					return PR_TRUE;
				}

				return PR_TRUE;
			} else if (!strcasecmp(arg2, "use")) {
				Admin_Use ();
				return PR_TRUE;
			}

#ifdef PR_COOP_MODE_ENHANCED
			else if ( !strcasecmp( arg2, "spawn" ) ) {
				if ( num_args < 3 ) {
					sprint( self, PR_PRINT_HIGH, "You must specify a classname!\nSyntax\xBA cmd admin spawn <classname> [team]\n" );
					return PR_TRUE;
				}

				if ( num_args == 3 )
					Admin_Spawn( arg3, 0 );
				else {
					tmpf = stof( arg4 );
					Admin_Spawn( arg3, tmpf );
				}
				return PR_TRUE;
			} else if ( !strcasecmp( arg2, "mass_spawn" ) ) {
				if ( num_args < 3 ) {
					sprint( self, PR_PRINT_HIGH, "You must specify a classname!\nSyntax\xBA cmd admin mass_spawn <classname> [count] [team]\n" );
					return PR_TRUE;
				}

				if ( num_args == 3 )
					Admin_MassSpawn( arg3, 0, 0 );
				else if ( num_args == 4 ) {
					tmpf = stof( arg4 );
					Admin_MassSpawn( arg3, 0, tmpf );
				} else if ( num_args == 5 ) {
					tmpf = stof( arg4 );
					tmpf2 = stof( arg5 );
					Admin_MassSpawn( arg3, tmpf2, tmpf );
				}
				return PR_TRUE;
			}
#endif
			else if ( !strcasecmp( arg2, "kill" ) ) {
				Admin_Kill();
				return PR_TRUE;
			}
			// PZ: generic player polling
			else if (!strcasecmp(arg2, "poll"))
			{
				if (num_args < 3)
				{
					CmdHelp(PR_CMDHELP_ADMIN_POLL);
					return PR_TRUE;
				}
				if (num_args > 3)
				{
					CmdMsg("Put your question text inside quotes, like this: \"<question>\"\n");
					return PR_TRUE;
				}
				StartPoll(arg3);
				return PR_TRUE;
			}
			// PZ: sets the ideal number of players that bots will maintain
			else if (!strcasecmp(arg2, "bots"))
			{
				float /*f,*/ newIdealP;
				string h, temp;
				if (num_args != 3)
				{
					CmdHelp(PR_CMDHELP_ADMIN_BOTS);
					return PR_TRUE;
				}
				newIdealP = stof(arg3);
				newIdealP = floor(newIdealP);
				if (newIdealP > 32 || newIdealP < 0)
				{
					CmdMsg("The number of players must be in the range of 0 to 32.\n");
					return PR_TRUE;
				}
				/*h = infokey(world, "skill");
				f = stof(h);
				newIdealP = newIdealP - bot_count;
				while (newIdealP > 0)
				{
					frik_addBotToGame(0, f);
					newIdealP = newIdealP - 1;
				}
				while (newIdealP < 0)
				{
					frik_removeABot();
					newIdealP = newIdealP + 1;
				}*/
				// instead of doing the above, just set the idealp for the current map only
				string cmd;
				cmd = "localinfo idealp ";
				cmd = strcat(cmd, arg3);
				cmd = strcat(cmd, "\n");
				localcmd(cmd);
				return PR_TRUE;
			}
			// PZ: turns on the bot waypoint editor
			else if (!strcasecmp(arg2, "waypoints"))
			{
				frik_toggleWaypointEditorOnOrOff();
				/*if (waypoint_mode == #WM_EDITOR)
					CmdMsg("The waypoint editor is now ON. Bots will cease normal activity.\n");
				else
					CmdMsg("The waypoint editor is now OFF. Bots will resume normal activity.\n");*/
				return PR_TRUE;
			}
		}

		// non-valid admin command
		CmdBadArg(S_("^bAdmin^b: Bad admin command \xA2"),arg2,"\xA2 passed in!\nType \xA2""cmd help admin\xA2 to see possible admin commands.\n");
		return PR_TRUE;
	}
	else if (!strcasecmp(arg1,"help")) // HELP
	{
		// Main help (no argument)
		if (num_args == 1)
		{
			CmdHelp(PR_CMDHELP_DEFAULT);
			return PR_TRUE;
		}

		// help items
		/*if (!strcasecmp(arg2,"build"))
		{
			CmdHelp(#CMDHELP_BUILD);
			return #TRUE;
		}
		else if (!strcasecmp(arg2,"summon"))
		{
			CmdHelp(#CMDHELP_SUMMON);
			return #TRUE;
		}*/

		if (!strcasecmp(arg2,"special"))
		{
			if (num_args == 2)
			{
				CmdHelp(PR_CMDHELP_SPECIAL);
				return PR_TRUE;
			}

			if (!strcasecmp(arg3,"getlit"))
			{
				CmdHelp(PR_CMDHELP_GETLIT);
				return PR_TRUE;
			}

			CmdBadArg(S_("^bHelp^b: Invalid special command \xA2"),arg3,"\xA2 passed in!\nType \xA2""cmd help special\xA2 to see possible commands.\n");
			return PR_TRUE;
		}
		else if (!strcasecmp(arg2,"cheater"))
		{
			CmdHelp(PR_CMDHELP_CHEATER);
			return PR_TRUE;
		}
		else if (!strcasecmp(arg2,"game"))
		{
			if (num_args == 2)
			{
				CmdHelp(PR_CMDHELP_GAME);
				return PR_TRUE;
			}

			if (!strcasecmp(arg3,"build"))
			{
				CmdHelp(PR_CMDHELP_BUILD);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"sname"))
			{
				CmdHelp(PR_CMDHELP_SNAME);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"drop"))
			{
				CmdHelp(PR_CMDHELP_DROP);
				return PR_TRUE;
			}
			else if ( !strcasecmp( arg3, "weapon" ) )
			{
				CmdHelp( PR_CMDHELP_WEAPON );
				return PR_TRUE;
			}

			// Unknown game command help request
			CmdBadArg(S_("^bHelp^b: Invalid game command \xA2"),arg3,"\xA2 passed in!\nType \xA2""cmd help game\xA2 to see possible commands.\n");
			return PR_TRUE;
		}
		else if (!strcasecmp(arg2,"vote"))
		{
			if (num_args == 2)
			{
				CmdHelp(PR_CMDHELP_VOTE);
				return PR_TRUE;
			}

			if (!strcasecmp(arg3,"map"))
			{
				CmdHelp(PR_CMDHELP_MAP);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"nextmap"))
			{
				CmdHelp(PR_CMDHELP_NEXTMAP);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"kick"))
			{
				CmdHelp(PR_CMDHELP_KICK);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"ban"))
			{
				CmdHelp(PR_CMDHELP_BAN);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"cuff"))
			{
				CmdHelp(PR_CMDHELP_CUFF);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"curse"))
			{
				CmdHelp(PR_CMDHELP_CURSE);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"mute"))
			{
				CmdHelp(PR_CMDHELP_MUTE);
				return PR_TRUE;
			}

			// Unknown vote command help request
			CmdBadArg(S_("^bHelp^b: Invalid vote command \xA2"),arg3,"\xA2 passed in!\nType \xA2""cmd help vote\xA2 to see possible commands.\n");
			return PR_TRUE;
		}
		else if (!strcasecmp(arg2,"admin"))
		{
			if (num_args == 2)
			{
				CmdHelp(PR_CMDHELP_ADMIN);
				return PR_TRUE;
			}

			if (!strcasecmp(arg3,"login"))
			{
				CmdHelp(PR_CMDHELP_ADMIN_LOGIN);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"logout"))
			{
				CmdHelp(PR_CMDHELP_ADMIN_LOGOUT);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"online"))
			{
				CmdHelp(PR_CMDHELP_ADMIN_ONLINE);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"ceasefire"))
			{
				CmdHelp(PR_CMDHELP_ADMIN_CEASEFIRE);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"get"))
			{
				CmdHelp(PR_CMDHELP_ADMIN_GET);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"kick"))
			{
				CmdHelp(PR_CMDHELP_ADMIN_KICK);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"ban"))
			{
				CmdHelp(PR_CMDHELP_ADMIN_BAN);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"mute"))
			{
				CmdHelp(PR_CMDHELP_ADMIN_MUTE);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"curse"))
			{
				CmdHelp(PR_CMDHELP_ADMIN_CURSE);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"team"))
			{
				CmdHelp(PR_CMDHELP_ADMIN_TEAM);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"cmnd"))
			{
				CmdHelp(PR_CMDHELP_ADMIN_CMND);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"cuff"))
			{
				CmdHelp(PR_CMDHELP_ADMIN_CUFF);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"poll")) // PZ
			{
				CmdHelp(PR_CMDHELP_ADMIN_POLL);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"bots")) // PZ
			{
				CmdHelp(PR_CMDHELP_ADMIN_BOTS);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"waypoints")) // PZ
			{
				CmdHelp(PR_CMDHELP_ADMIN_WAYPOINTS);
				return PR_TRUE;
			}

			// Unknown admin command help request
			CmdBadArg(S_("^bHelp^b: Invalid admin command \xA2"),arg3,"\xA2 passed in!\nType \xA2""cmd help admin\xA2 to see possible commands.\n");
			return PR_TRUE;
		}

		// Unknown command help request
		CmdBadArg(S_("^bHelp^b: Unknown command \xA2"),arg2,"\xA2 passed in!\nType \xA2""cmd help\xA2 to see possible commands.\n");
		return PR_TRUE;
	}
	else if (!strcasecmp("cheater",arg1))
	{
		// If this player is muted, go away
		if (mutedtime(self))
		{
			CmdMsg("Don't spam the server, please.\n");
			return PR_TRUE;
		}

		// Extra anti-spam safety
		if (!MonsterNameAllowed())
			return PR_TRUE;

		if (num_args == 1) // no arguments?
		{
			CmdHelp(PR_CMDHELP_CHEATER); // then show help
			return PR_TRUE;
		}

		targ = getclient(arg2);
		if (targ != world)
		{
			#ifndef PR_VOTING_TEST
			if (targ == self)
			{
				CmdMsg("You want to accuse yourself of cheating? weird..\n");
				return PR_TRUE;
			}
			#endif

			if (targ->classname != "player")
			{
				CmdMsg("You can only accuse of cheating a player!\n");
				return PR_TRUE;
			}

			RPrint(self->netname);
			RPrint(" accuses ");
			RPrint(targ->netname);
			if (num_args == 2)
				RPrint(" of cheating\n");
			else
			{
				RPrint(" of cheating. Reason:\n");
				RPrint(arg3);
				RPrint("\n");
			}

			bprint(PR_PRINT_HIGH,S_("^(logged^) "));
			bprint(PR_PRINT_HIGH,self->netname);
			bprint(PR_PRINT_HIGH," accuses ");
			bprint(PR_PRINT_HIGH,targ->netname);

			if (num_args == 2)
				bprint(PR_PRINT_HIGH,S_(" of ^bCHEATING^b!\n"));
			else
			{
				bprint(PR_PRINT_HIGH,S_(" of ^bCHEATING^b!\n^bReason^b/^bs^b: "));
				bprint(PR_PRINT_HIGH,arg3);
				bprint(PR_PRINT_HIGH,"\n");
			}

			st = GetClientDescription(targ);

			bprint(PR_PRINT_HIGH,"[",targ->netname," claims to be using ",st,"]\n");

			BroadcastSound("zombie/z_hit");
			putsaytime(self);
			return PR_TRUE;
		}
		else
		{
			CmdClientErr(arg2);
			return PR_TRUE;
		}
	}
	else if (!strcasecmp("vote",arg1))
	{
		if (num_args == 1)
		{
			CmdHelp(PR_CMDHELP_VOTE);
			return PR_TRUE;
		}

		if (mutedtime(self))
		{
			CmdMsg("You are muted, unable to vote!\n");
			return PR_TRUE;
		}

		if (!strcasecmp("nextmap", arg2))
		{
			if (num_args == 2 || arg3 == "")
			{
				CmdHelp(PR_CMDHELP_NEXTMAP);
				return PR_TRUE;
			}

			if (HisMapVoteAllowed())
			{
				if (self->increase_team1 < PR_CMD_MAX_MAPREQUESTS)
				{
					if (validatemap(arg3))
					{
						// PZ: require the map be listed in the "votemap" menu
						/*string temp;
						for (int i = 1; i <= 36; i++) // THERE ARE MORE THAN 36 MAPS IN VOTEMAP NOW. BUT NO LONGER USING THIS CODE ANYWAY. Opening up all maps for voting.
						{
							temp = GetMapString(i);
							if (temp == arg3)
								break;
						}*/
						if (1 /*|| i <= 36*/) // PZ: END
						{
							// Gizmo - set the first byte to the vote type
							self->group_no = PR_VOTE_NEXTMAP;

							if (arg4 != "") // PZ - begin AGR code
							{
								if      (!strcasecmp(arg4, "agr"))
									self->group_no = AssignByte2( self->group_no, PR_VOTEMAP_AGR ); // next map mode with AGR mode
								else if (!strcasecmp(arg4, "invade"))
									self->group_no = AssignByte2( self->group_no, PR_VOTEMAP_INVADE ); // next map mode with Invade mode
								else if (!strcasecmp(arg4, "neo"))
									self->group_no = AssignByte2( self->group_no, PR_VOTEMAP_NEO ); // next map mode with Neo mode
#ifdef PR_COOP_MODE_ENHANCED
								else if ( !strcasecmp( arg4, "tf" ) )
									self->group_no = AssignByte2( self->group_no, PR_VOTEMAP_TF );
								else if ( !strcasecmp( arg4, "coop" ) )
									self->group_no = AssignByte2( self->group_no, PR_VOTEMAP_COOP );
								else if ( !strcasecmp( arg4, "insane_coop" ) )
									self->group_no = AssignByte2( self->group_no, PR_VOTEMAP_INSANECOOP );
#endif
								else
								{
									sprint(self, PR_PRINT_HIGH, "Unrecognized command argument: ", arg4, "\n");
									return PR_TRUE;
								}
							}
							else            // PZ - end AGR code
								self->group_no = AssignByte2( self->group_no, PR_VOTEMAP_DEFAULT );

							StartVoteMap(0,arg3);
							return PR_TRUE;
						}
						else // PZ
							sprint(self, PR_PRINT_HIGH, "You can only vote for bot-supported maps. They are listed in the \xA2votemap\xA2 menu.\n");
					}
					else
					{
						sprint(self,PR_PRINT_HIGH,"The server doesn't have the \xA2");
						sprint(self,PR_PRINT_HIGH,arg3);
						sprint(self,PR_PRINT_HIGH,"\xA2 map\n");
					}

					self->increase_team1 = self->increase_team1 + 1;
					return PR_TRUE;
				}
				else
				{
					st = ftos(PR_CMD_MAX_MAPREQUESTS);
					st = strcat("Only ",st);
					st = strcat(st," custom map requests per game is allowed, sorry\n");
					CmdMsg(st);
					return PR_TRUE;
				}
			}
			else
				return PR_TRUE;
		}
		else if (!strcasecmp("map", arg2))
		{
			if (num_args == 2 || arg3 == "")
			{
				CmdHelp(PR_CMDHELP_MAP);
				return PR_TRUE;
			}

			if (HisMapVoteAllowed())
			{
				if (self->increase_team1 < PR_CMD_MAX_MAPREQUESTS)
				{
					if (validatemap(arg3))
					{
						// PZ: require the map be listed in the "votemap" menu
						/*string temp;
						for (int i = 1; i <= 36; i++) // THERE ARE MORE THAN 36 MAPS IN VOTEMAP NOW. BUT NO LONGER USING THIS CODE ANYWAY. Opening up all maps for voting.
						{
							temp = GetMapString(i);
							if (temp == arg3)
								break;
						}*/
						if (1 /*|| i <= 36*/) // PZ: END
						{
							// Gizmo - set the first byte to the vote type
							self->group_no = PR_VOTE_SWITCHMAP;

							if (arg4 != "") // PZ - begin AGR code
							{
								if      (!strcasecmp(arg4, "agr"))
									self->group_no = AssignByte2( self->group_no, PR_VOTEMAP_AGR ); // switch map mode with AGR mode
								else if (!strcasecmp(arg4, "invade"))
									self->group_no = AssignByte2( self->group_no, PR_VOTEMAP_INVADE ); // switch map mode with Invade mode
								else if (!strcasecmp(arg4, "neo"))
									self->group_no = AssignByte2( self->group_no, PR_VOTEMAP_NEO ); // switch map mode with Neo mode 
#ifdef PR_COOP_MODE_ENHANCED
								else if ( !strcasecmp( arg4, "tf" ) )
									self->group_no = AssignByte2( self->group_no, PR_VOTEMAP_TF );
								else if ( !strcasecmp( arg4, "coop" ) )
									self->group_no = AssignByte2( self->group_no, PR_VOTEMAP_COOP );
								else if ( !strcasecmp( arg4, "insane_coop" ) )
									self->group_no = AssignByte2( self->group_no, PR_VOTEMAP_INSANECOOP );
#endif
								else
								{
									sprint(self, PR_PRINT_HIGH, "Unrecognized command argument: ", arg4, "\n");
									return PR_TRUE;
								}
							}
							else            // PZ - end AGR code
								self->group_no = AssignByte2( self->group_no, PR_VOTEMAP_DEFAULT );

							StartVoteMap(0,arg3);
							return PR_TRUE;
						}
						else // PZ
							sprint(self, PR_PRINT_HIGH, "You can only vote for bot-supported maps. They are listed in the \xA2votemap\xA2 menu.\n");
					}
					else
					{
						sprint(self,PR_PRINT_HIGH,"The server doesn't have the \xA2");
						sprint(self,PR_PRINT_HIGH,arg3);
						sprint(self,PR_PRINT_HIGH,"\xA2 map\n");
					}

					self->increase_team1 = self->increase_team1 + 1;
					return PR_TRUE;
				}
				else
				{
					st = ftos(PR_CMD_MAX_MAPREQUESTS);
					st = strcat("Only ",st);
					st = strcat(st," custom map requests per game is allowed, sorry\n");
					CmdMsg(st);
					return PR_TRUE;
				}
			}
			else
				return PR_TRUE;
		}
		else if (!strcasecmp(arg2,"kick"))
		{
			if (num_args == 2)
			{
				CmdHelp(PR_CMDHELP_KICK);
				return PR_TRUE;
			}

			targ = getclient(arg3);

			if (targ != world)
			{
				self->admin_kick = targ;
				self->g_a = PR_PUNISH_KICK;

				if (HisPunishmentAllowed())
					StartPunishmentVote();

				return PR_TRUE;
			}
			else
			{
				CmdClientErr(arg3);
				return PR_TRUE;
			}
		}
		else if (!strcasecmp(arg2,"curse"))
		{
			if (num_args == 2)
			{
				CmdHelp(PR_CMDHELP_CURSE);
				return PR_TRUE;
			}

			targ = getclient(arg3);

			if (targ != world)
			{
				self->admin_kick = targ;
				self->g_a = PR_PUNISH_CURSE;

				if (HisPunishmentAllowed())
					StartPunishmentVote();

				return PR_TRUE;
			}
			else
			{
				CmdClientErr(arg3);
				return PR_TRUE;
			}
		}
		else if (!strcasecmp(arg2,"mute"))
		{
			if (num_args == 2)
			{
				CmdHelp(PR_CMDHELP_MUTE);
				return PR_TRUE;
			}

			targ = getclient(arg3);

			if (targ != world)
			{
				self->admin_kick = targ;
				self->g_a = PR_PUNISH_MUTE;

				if (HisPunishmentAllowed())
					StartPunishmentVote();

				return PR_TRUE;
			}
			else
			{
				CmdClientErr(arg3);
				return PR_TRUE;
			}
		}
		else if (!strcasecmp(arg2,"cuff"))
		{
			if (num_args == 2)
			{
				CmdHelp(PR_CMDHELP_CUFF);
				return PR_TRUE;
			}

			targ = getclient(arg3);

			if (targ != world)
			{
				self->admin_kick = targ;
				self->g_a = PR_PUNISH_CUFF;

				if (HisPunishmentAllowed())
					StartPunishmentVote();

				return PR_TRUE;
			}
			else
			{
				CmdClientErr(arg3);
				return PR_TRUE;
			}
		}
		else if (!strcasecmp(arg2,"ban"))
		{
			if (num_args == 2)
			{
				CmdHelp(PR_CMDHELP_BAN);
				return PR_TRUE;
			}

			targ = getclient(arg3);

			if (targ != world)
			{
				self->admin_kick = targ;
				self->g_a = PR_PUNISH_BAN;

				if (HisPunishmentAllowed())
					StartPunishmentVote();

				return PR_TRUE;
			}
			else
			{
				CmdClientErr(arg3);
				return PR_TRUE;
			}
		}
		else if ( !strcasecmp( arg2, "use" ) ) {
			if ( IsVoteAllowedGeneric() )
				StartUseObjectVote ();
			return PR_TRUE;
		}

		// Unknown vote command
		CmdBadArg(S_("^bVote^b: Unknown vote command \xA2"),arg2,"\xA2 passed in!\nType \xA2""cmd help vote\xA2 to see possible commands.\n");
		return PR_TRUE;
	}
	else if (!strcasecmp(arg1,"special"))
	{
		if (num_args == 1)
		{
			CmdHelp(PR_CMDHELP_SPECIAL);
			return PR_TRUE;
		}

		if (!strcasecmp(arg2,"getlit"))
		{
			if (self->cltype != PR_CLTYPE_PROZACQW && self->cltype != PR_CLTYPE_FUHQUAKE && self->cltype != PR_CLTYPE_AMFQUAKE)
			{
				CmdMsg("You must use a coloured lighting capable and compatible client to use this feature, sorry!\n");
				return PR_TRUE;
			}

			if (validatelit())
			{
				sprint(self,PR_PRINT_HIGH,"Attempting to download coloured lighting file for the map...\n");
				stuffcmd(self,"download maps/");
				stuffcmd(self,mapname);
				stuffcmd(self,".lit\n");
			}
			else
			{
				CmdMsg("The server doesn't have a coloured lighting file for this map!\n");
			}

			return PR_TRUE;
		}
		else if (!strcasecmp(arg2,"getsky"))
		{
			if (self->cltype != PR_CLTYPE_PROZACQW)
			{
				CmdMsg("Feature not compatible with your client, sorry!\n");
				return PR_TRUE;
			}

			CmdMsg("Still unimplemented, sorry!\n");
			return PR_TRUE;
		}
		else if (!strcasecmp(arg2,"gethrt"))
		{
			if (self->cltype != PR_CLTYPE_PROZACQW)
			{
				CmdMsg("Feature not compatible with your client, sorry!\n");
				return PR_TRUE;
			}

			CmdMsg("Still unimplemented, sorry!\n");
			return PR_TRUE;
		}

		// Unknown/invalid special command
		CmdBadArg(S_("^bSpecial^b: Unknown special command \xA2"),arg2,"\xA2 passed in!\nType \xA2""cmd help special\xA2 to see possible commands.\n");
		return PR_TRUE;
	}

	// if we are an spectator, go away.. (return FALSE --> will make server display error to client)
	if (self->classname != "player")
		return PR_FALSE;

	// Player-only commands:
	if (!strcasecmp(arg1, "game"))
	{
		if (num_args == 1)
		{
			CmdHelp(PR_CMDHELP_GAME);
			return PR_TRUE;
		}

		if (!strcasecmp(arg2,"drop"))
		{
			if (num_args == 2)
			{
				CmdHelp(PR_CMDHELP_DROP);
				return PR_TRUE;
			}

			if (!strcasecmp(arg3,"items"))
			{
				tmpf = TeamFortress_DropItems();

				if (tmpf == 0)
					CmdMsg("You have no items to drop!\n");
				else
				{
					if (tmpf > 1)
					{	st = ftos(tmpf);
						st = strcat("You have dropped ",st);
						st = strcat(st," items.\n");
					}
					else
						st = "You have dropped one item.\n";

					CmdMsg(st);
				}

				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"runes"))
			{
				// Neo mode
				if (neo.isModeActive())
				{
					CmdMsg("This command is disabled in Neo mode.\n");
					return true;
				}
				
				if (self->PR_runes == 0)
				{
					CmdMsg("You have no runes to drop!\n");
					return PR_TRUE;
				}

				PlayerDropRunes(self);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"rune"))
			{
				// Neo mode
				if (neo.isModeActive())
				{
					CmdMsg("This command is disabled in Neo mode.\n");
					return true;
				}
				if (num_args == 3)
				{
					CmdMsg("You should specify which rune to drop!\n");
					return PR_TRUE;
				}

				tmpf = floor(stof(arg4));

				if (tmpf < 1 || tmpf > 4)
				{
					CmdMsg("Valid rune numbers are from 1 to 4!\n");
					return PR_TRUE;
				}

				if (DropSpecificRune(self,tmpf))
					return PR_TRUE;
				else
				{
					CmdMsg("You do not have that rune!\n");
					return PR_TRUE;
				}
			}
		}
		else if (!strcasecmp(arg2,"build")) // BUILD
		{
			if (num_args == 2) // We didnt specify something to build
			{
				CmdHelp(PR_CMDHELP_BUILD); //..so show help
				return PR_TRUE;
			}

			// What should we build?
			if (!strcasecmp(arg3,"sentry"))
			{
				TeamFortress_Build(PR_BUILD_SENTRYGUN,1);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"tesla"))
			{
				TeamFortress_Build(PR_BUILD_TESLA,1);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"sensor"))
			{
			// TODO    TeamFortress_Build(#BUILD_SENSOR);
				SBBuildSensor();
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"camera"))
			{
				TeamFortress_Build(PR_BUILD_SECURITY_CAMERA,1);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"teleporter"))
			{
				TeamFortress_Build(PR_BUILD_TELEPORTER,1);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"fieldgen"))
			{
				TeamFortress_Build(PR_BUILD_FIELDGEN,1);
				return PR_TRUE;
			}
			else if (!strcasecmp(arg3,"dispenser"))
			{
				TeamFortress_Build(PR_BUILD_DISPENSER,1);
				return PR_TRUE;
			}

			// Not a valid item to build
			CmdBadArg(S_("^bBuild^b: Bad item \xA2"),arg3,"\xA2 passed in!\nUse \xA2""cmd help game build\xA2 to see possible choices.\n");
			return PR_TRUE;
		}
		else if (!strcasecmp(arg2,"sname"))
		{
			if (num_args == 2)
			{
				CmdHelp(PR_CMDHELP_SNAME);
				return PR_TRUE;
			}

			tmpf = stof(arg3);
			tmpf = rint(tmpf);

			if (tmpf < 1 || tmpf > 3)
			{
				CmdBadArg(S_("^bSName^b: Bad name slot \xA2"),arg3,"\xA2 specified!\nUse \xA2""cmd help game sname\xA2 to see syntax.\n");
				return PR_TRUE;
			}

			if (num_args == 3) // Display name?
			{
				if (tmpf == 1)
				{
					if (self->PR_sname1 == "")
					{
						CmdMsg("No custom name on slot 1\n");
						return PR_TRUE;
					}
					else
					{
						st = strcat("SName 1 is: ",self->PR_sname1);
						st = strcat(st,"\n");
						CmdMsg(st);
						return PR_TRUE;
					}
				}
				else if (tmpf == 2)
				{
					if (self->PR_sname2 == "")
					{
						CmdMsg("No custom name on slot 2\n");
						return PR_TRUE;
					}
					else
					{
						st = strcat("SName 2 is: ",self->PR_sname2);
						st = strcat(st,"\n");
						CmdMsg(st);
						return PR_TRUE;
					}
				}
				else if (tmpf == 3)
				{
					if (self->PR_sname3 == "")
					{
						CmdMsg("No custom name on slot 3\n");
						return PR_TRUE;
					}
					else
					{
						st = strcat("SName 3 is: ",self->PR_sname3);
						st = strcat(st,"\n");
						CmdMsg(st);
						return PR_TRUE;
					}
				}
			}

			// Check for valid string length
			if (strlen(arg4) > PR_MAX_SNAME_LEN)
			{
				CmdMsg("Custom name too long! (maximum is " TO_STR(PR_MAX_SNAME_LEN) " chars)\n");
				return PR_TRUE;
			}

			// We should set the name then
			if (tmpf == 1)
			{
				if (arg4 == "") // Empty string as name argument
				{
					if (self->PR_sname1 != "") // he has a custom name set
					{
						sprint(self,PR_PRINT_HIGH,"Custom name 1 has been removed.\n");

						if (self->demon_one != world && (self->job & PR_JOB_WARLOCK || self->job & PR_JOB_ARMY))
						{
							self->demon_one->increase_team2 = GetNameType(self,0,PR_TRUE); // default name type
							MakeMonsterName(self->demon_one);

							if (MonsterNameAllowed())
							if (!mutedtime(self))
							{
								st = GetMonsterName(self->demon_one);
								bprint(PR_PRINT_HIGH,"The ");
								bprint(PR_PRINT_HIGH,st);
								bprint(PR_PRINT_HIGH," ");
								bprint(PR_PRINT_HIGH,self->PR_sname1);
								bprint(PR_PRINT_HIGH," (");
								bprint(PR_PRINT_HIGH,self->netname);
								bprint(PR_PRINT_HIGH,") is now known as ");
								bprint(PR_PRINT_HIGH,self->demon_one->netname);
								bprint(PR_PRINT_HIGH,"\n");
							}
						}

						delstr(self->PR_sname1);
						self->PR_sname1 = "";
					}
					else
						CmdMsg("Custom name 1 is already empty!\n");
				}
				else // String with something as name argument
				{
					if (self->PR_sname1 == arg4)
					{
						CmdMsg("No changes on custom name 1\n");
						return PR_TRUE;
					}

					if (self->PR_sname1 != "") // he has a custom name set
					{
						sprint(self,PR_PRINT_HIGH,"Custom name 1 has been changed.\n");

						if (self->demon_one != world && (self->job & PR_JOB_WARLOCK || self->job & PR_JOB_ARMY))
						{
							if (MonsterNameAllowed())
							if (!mutedtime(self))
							{
								st = GetMonsterName(self->demon_one);
								bprint(PR_PRINT_HIGH,"The ");
								bprint(PR_PRINT_HIGH,st);
								bprint(PR_PRINT_HIGH," ");
								bprint(PR_PRINT_HIGH,self->PR_sname1);
								bprint(PR_PRINT_HIGH," (");
								bprint(PR_PRINT_HIGH,self->netname);
								bprint(PR_PRINT_HIGH,") is now known as ");
								bprint(PR_PRINT_HIGH,arg4);
								bprint(PR_PRINT_HIGH,"\n");
							}
						}

						delstr(self->PR_sname1);
						self->PR_sname1 = makestr(arg4);
					}
					else // no custom name currently..
					{
						sprint(self,PR_PRINT_HIGH,"Custom name 1 has been set.\n");

						if (self->demon_one != world && (self->job & PR_JOB_WARLOCK || self->job & PR_JOB_ARMY))
						{
							self->demon_one->increase_team2 = 0; // custom name type

							if (MonsterNameAllowed())
							if (!mutedtime(self))
							{
								st = GetMonsterName(self->demon_one);
								bprint(PR_PRINT_HIGH,"The ");
								bprint(PR_PRINT_HIGH,st);
								bprint(PR_PRINT_HIGH," ");
								bprint(PR_PRINT_HIGH,self->demon_one->netname);
								bprint(PR_PRINT_HIGH," (");
								bprint(PR_PRINT_HIGH,self->netname);
								bprint(PR_PRINT_HIGH,") is now known as ");
								bprint(PR_PRINT_HIGH,arg4);
								bprint(PR_PRINT_HIGH,"\n");
							}
						}

						self->PR_sname1 = makestr(arg4);

						if (self->demon_one != world && (self->job & PR_JOB_WARLOCK || self->job & PR_JOB_ARMY))
							self->demon_one->netname = self->PR_sname1;
					}
				}
			}
			else if (tmpf == 2)
			{
				if (arg4 == "") // Empty string as name argument
				{
					if (self->PR_sname2 != "") // he has a custom name set
					{
						sprint(self,PR_PRINT_HIGH,"Custom name 2 has been removed.\n");

						if (self->demon_two != world && (self->job & PR_JOB_WARLOCK || self->job & PR_JOB_ARMY))
						{
							self->demon_two->increase_team2 = GetNameType(self,1,PR_TRUE); // default name type
							MakeMonsterName(self->demon_two);

							if (MonsterNameAllowed())
							if (!mutedtime(self))
							{
								st = GetMonsterName(self->demon_two);
								bprint(PR_PRINT_HIGH,"The ");
								bprint(PR_PRINT_HIGH,st);
								bprint(PR_PRINT_HIGH," ");
								bprint(PR_PRINT_HIGH,self->PR_sname2);
								bprint(PR_PRINT_HIGH," (");
								bprint(PR_PRINT_HIGH,self->netname);
								bprint(PR_PRINT_HIGH,") is now known as ");
								bprint(PR_PRINT_HIGH,self->demon_two->netname);
								bprint(PR_PRINT_HIGH,"\n");
							}
						}

						delstr(self->PR_sname2);
						self->PR_sname2 = "";
					}
					else
						CmdMsg("Custom name 2 is already empty!\n");
				}
				else // String with something as name argument
				{
					if (self->PR_sname2 == arg4)
					{
						CmdMsg("No changes on custom name 2\n");
						return PR_TRUE;
					}

					if (self->PR_sname2 != "") // he has a custom name set
					{
						sprint(self,PR_PRINT_HIGH,"Custom name 2 has been changed.\n");

						if (self->demon_two != world && (self->job & PR_JOB_WARLOCK || self->job & PR_JOB_ARMY))
						{
							if (MonsterNameAllowed())
							if (!mutedtime(self))
							{
								st = GetMonsterName(self->demon_two);
								bprint(PR_PRINT_HIGH,"The ");
								bprint(PR_PRINT_HIGH,st);
								bprint(PR_PRINT_HIGH," ");
								bprint(PR_PRINT_HIGH,self->PR_sname2);
								bprint(PR_PRINT_HIGH," (");
								bprint(PR_PRINT_HIGH,self->netname);
								bprint(PR_PRINT_HIGH,") is now known as ");
								bprint(PR_PRINT_HIGH,arg4);
								bprint(PR_PRINT_HIGH,"\n");
							}
						}

						delstr(self->PR_sname2);
						self->PR_sname2 = makestr(arg4);
					}
					else // no custom name currently..
					{
						sprint(self,PR_PRINT_HIGH,"Custom name 2 has been set.\n");

						if (self->demon_two != world && (self->job & PR_JOB_WARLOCK || self->job & PR_JOB_ARMY))
						{
							self->demon_two->increase_team2 = 0; // custom name type

							if (MonsterNameAllowed())
							if (!mutedtime(self))
							{
								st = GetMonsterName(self->demon_two);
								bprint(PR_PRINT_HIGH,"The ");
								bprint(PR_PRINT_HIGH,st);
								bprint(PR_PRINT_HIGH," ");
								bprint(PR_PRINT_HIGH,self->demon_two->netname);
								bprint(PR_PRINT_HIGH," (");
								bprint(PR_PRINT_HIGH,self->netname);
								bprint(PR_PRINT_HIGH,") is now known as ");
								bprint(PR_PRINT_HIGH,arg4);
								bprint(PR_PRINT_HIGH,"\n");
							}
						}

						self->PR_sname2 = makestr(arg4);

						if (self->demon_two != world && (self->job & PR_JOB_WARLOCK || self->job & PR_JOB_ARMY))
							self->demon_two->netname = self->PR_sname2;
					}
				}
			}
			else
			{
				if (arg4 == "") // Empty string as name argument
				{
					if (self->PR_sname3 != "") // he has a custom name set
					{
						sprint(self,PR_PRINT_HIGH,"Custom name 3 has been removed.\n");

						if (self->demon_three != world && (self->job & PR_JOB_WARLOCK || self->job & PR_JOB_ARMY))
						{
							self->demon_one->increase_team2 = GetNameType(self,2,PR_TRUE); // default name type
							MakeMonsterName(self->demon_three);

							if (MonsterNameAllowed())
							if (!mutedtime(self))
							{
								st = GetMonsterName(self->demon_three);
								bprint(PR_PRINT_HIGH,"The ");
								bprint(PR_PRINT_HIGH,st);
								bprint(PR_PRINT_HIGH," ");
								bprint(PR_PRINT_HIGH,self->PR_sname3);
								bprint(PR_PRINT_HIGH," (");
								bprint(PR_PRINT_HIGH,self->netname);
								bprint(PR_PRINT_HIGH,") is now known as ");
								bprint(PR_PRINT_HIGH,self->demon_three->netname);
								bprint(PR_PRINT_HIGH,"\n");
							}
						}

						delstr(self->PR_sname3);
						self->PR_sname3 = "";
					}
					else
						CmdMsg("Custom name 3 is already empty!\n");
				}
				else // String with something as name argument
				{
					if (self->PR_sname3 == arg4)
					{
						CmdMsg("No changes on custom name 3\n");
						return PR_TRUE;
					}

					if (self->PR_sname3 != "") // he has a custom name set
					{
						sprint(self,PR_PRINT_HIGH,"Custom name 3 has been changed.\n");

						if (self->demon_three != world && (self->job & PR_JOB_WARLOCK || self->job & PR_JOB_ARMY))
						{
							if (MonsterNameAllowed())
							if (!mutedtime(self))
							{
								st = GetMonsterName(self->demon_three);
								bprint(PR_PRINT_HIGH,"The ");
								bprint(PR_PRINT_HIGH,st);
								bprint(PR_PRINT_HIGH," ");
								bprint(PR_PRINT_HIGH,self->PR_sname3);
								bprint(PR_PRINT_HIGH," (");
								bprint(PR_PRINT_HIGH,self->netname);
								bprint(PR_PRINT_HIGH,") is now known as ");
								bprint(PR_PRINT_HIGH,arg4);
								bprint(PR_PRINT_HIGH,"\n");
							}
						}

						delstr(self->PR_sname3);
						self->PR_sname3 = makestr(arg4);
					}
					else // no custom name currently..
					{
						sprint(self,PR_PRINT_HIGH,"Custom name 3 has been set.\n");

						if (self->demon_three != world && (self->job & PR_JOB_WARLOCK || self->job & PR_JOB_ARMY))
						{
							self->demon_three->increase_team2 = 0; // custom name type

							if (MonsterNameAllowed())
							if (!mutedtime(self))
							{
								st = GetMonsterName(self->demon_three);
								bprint(PR_PRINT_HIGH,"The ");
								bprint(PR_PRINT_HIGH,st);
								bprint(PR_PRINT_HIGH," ");
								bprint(PR_PRINT_HIGH,self->demon_three->netname);
								bprint(PR_PRINT_HIGH," (");
								bprint(PR_PRINT_HIGH,self->netname);
								bprint(PR_PRINT_HIGH,") is now known as ");
								bprint(PR_PRINT_HIGH,arg4);
								bprint(PR_PRINT_HIGH,"\n");
							}
						}

						self->PR_sname3 = makestr(arg4);

						if (self->demon_three != world && (self->job & PR_JOB_WARLOCK || self->job & PR_JOB_ARMY))
							self->demon_three->netname = self->PR_sname3;
					}
				}
			}

			return PR_TRUE;
		}
		else if ( !strcasecmp( arg2, "weapon" ) ) {
			if ( num_args == 2 ) {
				CmdHelp( PR_CMDHELP_WEAPON );
				return PR_TRUE;
			}

			ChangeToWeapon( arg3 );
			return PR_TRUE;
		}

		// Not a recognized game command
		CmdBadArg(S_("^bGame^b: Unrecognized command \xA2"),arg2,"\xA2 passed in!\nUse \xA2""cmd help game\xA2 to see available commands.\n");
		return PR_TRUE;
	}/*
	else if (!strcasecmp(arg1,"summon")) // SUMMON
	{
		if (num_args == 1) // We didnt specify something to summon
		{
			CmdHelp(#CMDHELP_SUMMON); //..so show help
			return #TRUE;
		}

		// What should we summon?
		if (!strcasecmp(arg2,"scrag"))
		{
			CmdMsg("You summon a scrag!\n");
			return #TRUE;
		}
		else if (!strcasecmp(arg2,"piranha"))
		{
			CmdMsg("You summon a piranha!\n");
			return #TRUE;
		}
		else if (!strcasecmp(arg2,"fiend"))
		{
			CmdMsg("You summon a fiend!\n");
			return #TRUE;
		}
		else if (!strcasecmp(arg2,"shambler"))
		{
			CmdMsg("You summon a shambler!\n");
			return #TRUE;
		}
		else if (!strcasecmp(arg2,"soldier"))
		{
			CmdMsg("You teleport a soldier!\n");
			return #TRUE;
		}

		// Not a recognized parameter
		CmdBadArg("\bSummon\b: Bad argument \{162}",arg2,"\{162} passed in!\nUse \{162}cmd help summon\{162} to see possible choices.\n");
		return #TRUE;
	}*/

	// Not a command handled by us..
	return PR_FALSE;
}

//========================================================================
// Subroutine to check if a map exists on server

float validatemap(string mapstr)
{
	mapstr = strcat("maps/",mapstr);
	mapstr = strcat(mapstr,".bsp");

	return validatefile(mapstr);
}

//========================================================================
// Functions to enable/disable specific items using "cmd" commands

float DisableItem(float itemnum, float page, float permanent)
{
	float fieldnum;
	int curval;
	int bit;

	fieldnum = floor((page - 1) /3)+1;
	curval = DisabledStuff(fieldnum);
	bit = (itemnum + ((page - 1)*7)) - ((fieldnum - 1)*3*7);
	bit = itob(bit);

	if (curval & bit)
		return PR_FALSE;
	else
	{
		SetDisabledStuff(fieldnum,curval | bit);

		if (permanent)
		{
			string st;
			st = ftos(fieldnum);
			localcmd("localinfo dstuff");
			localcmd(st);
			st = ftos(curval | bit);
			localcmd(" ");
			localcmd(st);
			localcmd("\n");
		}

		return PR_TRUE;
	}
}

float EnableItem(float itemnum, float page, float permanent)
{
	float fieldnum;
	int curval;
	int bit;

	fieldnum = floor((page - 1)/3)+1;
	curval = DisabledStuff(fieldnum);
	bit = (itemnum + ((page - 1)*7)) - ((fieldnum - 1)*3*7);
	bit = itob(bit);

	if (curval & bit)
	{
		SetDisabledStuff(fieldnum,curval - bit);

		if (permanent)
		{
			string st;
			st = ftos(fieldnum);
			localcmd("localinfo dstuff");
			localcmd(st);
			st = ftos(curval - bit);
			localcmd(" ");
			localcmd(st);
			localcmd("\n");
		}

		return PR_TRUE;
	}
	else
		return PR_FALSE;
}

float MakeFree(float itemnum, float page, float permanent)
{
	float fieldnum;
	int curval;
	int bit;

	fieldnum = floor((page - 1)/3)+1;
	curval = GivenStuff(fieldnum);
	bit = (itemnum + ((page - 1)*7)) - ((fieldnum - 1)*3*7);
	bit = itob(bit);

	if (curval & bit)
		return PR_FALSE;
	else
	{
		SetGivenStuff(fieldnum,curval | bit);

		if (permanent)
		{
			string st;
			st = ftos(fieldnum);
			localcmd("localinfo gstuff");
			localcmd(st);
			st = ftos(curval | bit);
			localcmd(" ");
			localcmd(st);
			localcmd("\n");
		}

		return PR_TRUE;
	}
}

float DelFree(float itemnum, float page, float permanent)
{
	float fieldnum;
	int curval;
	int bit;

	fieldnum = floor((page - 1)/3)+1;
	curval = GivenStuff(fieldnum);
	bit = (itemnum + ((page - 1)*7)) - ((fieldnum - 1)*3*7);
	bit = itob(bit);

	if (curval & bit)
	{
		SetGivenStuff(fieldnum,curval - bit);

		if (permanent)
		{
			string st;
			st = ftos(fieldnum);
			localcmd("localinfo gstuff");
			localcmd(st);
			st = ftos(curval - bit);
			localcmd(" ");
			localcmd(st);
			localcmd("\n");
		}

		return PR_TRUE;
	}
	else
		return PR_FALSE;
}

} // END namespace Progs
