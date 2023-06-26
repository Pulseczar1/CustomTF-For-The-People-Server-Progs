/*======================================================
	OPTIONS.QC			TeamFortress v2.5

	(c) TeamFortress Software Pty Ltd 	29/2/97
========================================================
Defines for the compilable options within TF.
========================================================*/
//#pragma DONT_COMPILE_THIS_FILE

#ifndef OPTIONS_H
#define OPTIONS_H

namespace Progs {

//#define COOP_MODE
#define PR_COOP_MODE_ENHANCED			// Gizmo - includes the new coop mode, does not make it a coop only compile
#define PR_MIRV_VELOCITY				// Gizmo - enables the spamlets of mirvs to take on the original mirv's velocity

// Gizmo - these are all the macros that can be used to compile for classic prozac
// but a few minor things like vote code, bug fixes, end of game awards, no stock sniper caltrops (standard grenades instead), tesla power supply upgrade after health tinker, etc. are still included
#define PR_NO_PREFAB_TESLA_UPGRADE		// Gizmo - no prefab upgraded teslas allowed (only 1/2/1 teslas will be allowed even with upgrade)
#define PR_CLASSIC_BUILD_HEALTHS			// Gizmo - engy builds have the health levels on classic prozac instead of the current
//#define NO_GRUNTY_EMERGENCY_TELE		// Gizmo - players are not allowed to teleport an existing soldier to their location in trade for reseting the waiting soldier timer (counts as if they tele'd a new soldier)
#define PR_NO_STOCK_DEMOMAN_HIGHJUMP		// Gizmo - the stock demoman doesn't start with highjump
//#define NO_STOCK_DEMOMAN_BLASTARMOR	// Gizmo - the stock demoman doesn't start with blast armor  // PZ: Turned on blast armor for demoman. He seemed too hard to keep alive.
#define PR_NO_STOCK_SPY_STEALTH		// Gizmo - the stock spy doesn't start with stealth
#define PR_CLASSIC_STOCK_ENGY_MAXCELLS	// Gizmo - classic maximum amount of cells for engineer, also sets initial cells to 200
#define PR_NO_STOCK_ENGY_HIGHJUMP		// Gizmo - the stock engineer doesn't start with highjump
#define PR_CLASSIC_SNIPER_RIFLE		// Gizmo - classic prozac sniper rifle
#define PR_CLASSIC_LIGHTNING_GUN		// Gizmo - classic prozac lightning gun
//#define NO_REVIVE				// Gizmo - disables medics being able to revive dead players before they respawn; if crusader, enables mass revive
//#define NO_ZEROGRAVGUN			// Gizmo - disables the Zero-Gravity Gun


#define PR_ZGG_FRIENDLY_DETPACK_EXPLODE_CHANCE 0.5
#define PR_ZGG_ENEMY_DETPACK_EXPLODE_CHANCE    0.1

#define PR_NET_SERVER
//#define BOTS			// doesn't do anything yet
//#define DEMO_STUFF
#define PR_STATUSBAR
#define PR_QUAKE_WORLD

#ifdef PR_QUAKE_WORLD
//	#pragma PROGS_DAT "../qwprogs.dat"

	#undef PR_COOP_MODE
	#define PR_NET_SERVER
#endif

#ifdef PR_NET_SERVER
	#define PR_SPY_INVIS_ONLY		 PR_OFF	// Spy becomes invisible instead of changing skin/color
    #define PR_DEFAULT_AUTOZOOM     PR_OFF
	#define PR_WEINER_SNIPER			 	// autoaiming for sniper rifle  (PZ NOTE: This doesn't seem to be getting used, thankfully...)
	#define PR_FLAME_MAXWORLDNUM	 20	 	// maximum number of flames in the world. DO NOT PUT BELOW 20.
	#define PR_MAX_WORLD_PIPEBOMBS	 20	 	// This is divided between teams - this is the most you should have on a net server
	#define PR_MAX_WORLD_AMMOBOXES	 6		// This is divided between teams - this is the most you should have on a net server
	#define PR_GR_TYPE_MIRV_NO		 4	 	// Number of Mirvs a Mirv Grenade breaks into
	#define PR_GR_TYPE_NAPALM_NO	 8 	 	// Number of flames napalm grenade breaks into (unused if net server)
	#define PR_MEDIKIT_IS_BIOWEAPON 	 	// WK THIS MUST BE DEFINED OR SHIT BREAKS NOW
//	#define RESTORE_DEAD_SPEEDS     	// If defined, we restore the cl_speeds
#else
	// LAN options
	#define PR_SPY_INVIS_ONLY		 PR_OFF	// Spy changes skin/color instead of becoming invisible
	#define PR_DEFAULT_AUTOZOOM 	 PR_OFF
	#define PR_FLAME_MAXWORLDNUM	 60		// maximum number of flames in the world. DO NOT PUT BELOW 20.
	#define PR_MAX_WORLD_PIPEBOMBS	 30 	// This is divided between teams
	#define PR_MAX_WORLD_AMMOBOXES	 6 	// This is divided between teams
	#define PR_GR_TYPE_MIRV_NO		 12		// Number of Mirvs a Mirv Grenade breaks into
	#define PR_GR_TYPE_NAPALM_NO	 12  	// Number of flames napalm grenade breaks into
#endif

// these defines are ignored if quakeworld is on
#define PR_GRAPPLING_HOOK		// Allow players to use the Hook on any map

#define PR_TEAM_HELP_RATE   60	// used only if teamplay bit 64 (help team with lower score) is set.
							// 60 is a mild setting, and won't make too much difference
							// increasing it _decreases_ the amount of help the losing team gets
							// Minimum setting is 1, which would really help the losing team

#define PR_PLAYER_PUSHING		// You can push your team mates around!

#define PR_DISPLAY_CLASS_HELP	PR_ON		// Change this to #OFF if you don't want the class help to
									// appear whenever a player connects
#define PR_NEVER_TEAMFRAGS		PR_ON 	// teamfrags options always off (i didn't realise people wouldn't like it)
#define PR_ALWAYS_TEAMFRAGS	PR_OFF	// teamfrags options always on
//#define CHECK_SPEEDS		#ON		// makes sure players aren't moving too fast
#define PR_CHECK_SPEEDS		PR_OFF	// OfN Disabled due to buggy behaviour
#define PR_SNIPER_RIFLE_RELOAD_TIME	1.5	// seconds
#define PR_STOP_MOUSE_MOVEMENT	PR_ON		// stops players from moving forward/back with the mouse, and cheating with it.
#define PR_RESPAWN_DELAY_TIME  5		// this is the respawn delay, if the RESPAWN_DELAY option is
									// turned on with temp1.  QuakeWorld servers can use
									// serverinfo respawn_delay to set their own time.
									// see qw.txt for more info.

//#define SPEECH					// Don't enable this, ever.
#define PR_MEDIKIT_IS_BIOWEAPON 	 	// WK THIS MUST BE DEFINED OR SHIT BREAKS NOW

#ifdef PR_QUAKE_WORLD
	#undef PR_COOP_MODE
//	#undef NET_SERVER
    #undef PR_PLAYER_PUSHING

	#define PR_NET_SERVER
#endif

// Message Options
//#define CHEAT_WARNINGS   	// If defined, the server will be told when players
							// were caught with different skins/colors.

// Debug Options
//#define MAP_DEBUG			// Debug for Map code. I suggest running in a hi-res
							// mode and/or piping the output from the server to a file.
//#define VERBOSE			// Verbose Debugging on/off

//#define PRINT_DMG		//CH prints out dmg from TF_T_Damage

} // END namespace Progs

#endif // END OPTIONS_H
