/*=======================================================//
// ofndefs.QC - CustomTF 3.2.OfN           - 30/1/2001 - //
// by Sergio Fuma�a Grunwaldt - OfteN [cp]               //
=========================================================//
 Debug flags and other *IMPORTANT* settings and flags
 --------------------------------------------------------
 Special number chars reference:  (why here? dunno..)

 0123456789  --> Normally white
 ����������  --> Normally red
   --> Normally gold
 ����������  --> Normally gold (2nd variation)

 The actual color depends on the gfx.wad the client uses
 or the charset loaded into fuhquake.
=========================================================*/

// This file does not have any QuakeC, only preprocessor stuff, so..
//#pragma DONT_COMPILE_THIS_FILE

#ifndef OFNDEFS_H
#define OFNDEFS_H

namespace Progs {

//================================================================================//
// MAIN BUILD CONFIGURATION  (un/comment any of the following lines to change it) //
//________________________________________________________________________________//

#define PR__RELEASE_BUILD_   // <-- ENABLE THIS FOR ANY BUILDS TO BE PLAYED!!
#define PR__FANCY_STUFF_     // <-- Turns on all fancy stuff, like cool gibs etc..

//================================================================================//

//==============================//
// Debug/Test flags & enablers  //
//==============================//

//------------------------------------------//
#ifndef PR__RELEASE_BUILD_                     //
//------------------------------------------//
//#define VERBOSE_GRUNTY
//#define WARLOCK_TEST // free summon
//#define ARMY_TEST // only 5 seconds for tele the soldier
//#define CRUSADER_TEST
//#define ALIASES_TEST
//#define GRENADES_TEST // Infinite grenades
//#define HOLO_TEST //Sentries, monsters and teslas attack friendly holos?
//#define FIELD_TEST
//#define VOTING_TEST
//#define XMAS_TEST
//#define CRASH_TEST
//#define STORM_TEST // Enables special admin cmnd "thunder"

//#define TESTRANGE_EXTRA
//#define SNIPERSIGHT_SHOWPOS
//#define MINE_DEBUG // to debug guerilla

//#define MONSTERS_AURA 2 // for testing 1 = power 2 = resis 5 = Regen
//#define MONSTERS_STATE #TFSTATE_INFECTED

//#define GRUNTY_EXTRA_WEAPONS #WEAP_ROCKET_LAUNCHER | #WEAP_SUPER_SHOTGUN // 0

// Attack owner for testing purposes?
//#define MAD_GRUNTY
#define PR_MAD_MONSTERS
//#define MAD_TESLA
//#define MAD_SENTRY

//------------------------------------------//
#endif // _RELEASE_BUILD_                   //
//------------------------------------------//

//===================================================//
// UNIMPLEMENTED STUFF DISABLERS                     //
#define PR_GREMLIN_NOTDONE // <-- Disables gremlin
//#define FISH_NOTDONE // <-- Disables piranha

//===================================================//
// Dispensable visual/sound/gameplay stuff enablers  //
//===================================================//

//------------------------------------------//
#ifdef PR__FANCY_STUFF_                     //
//------------------------------------------//
#define PR_KICKABLE_HEADS           // <-- Allows heads to be kickable
#define PR_GIBABLE_CORPSES          // <-- Makes corpses gibable
#define PR_COOL_GIBS                // <-- Makes gibs pushable by explosions
#define PR_COOL_GIBS_TRACE          // <-- Makes gibs pushable by trace attacks
#define PR_COOL_METAL_GIBS          // <-- Enables the same for machine gibs
#define PR_MENU_SOUNDS              // <-- Enables all menu sounds
#define PR_SHAMBLER_SHOCK_SOUND     // <-- Electric sound for shambler lightning
#define PR_EXTRA_DETPACK_EXPLOSIONS // <-- Makes extra explosions for detpacks
#define PR_TEAMITEMS_COLOURED       // <-- Gives color to team-specific tf items
#define PR_IDABLE_CORPSES           // <-- Allows corpses to be ID'ed
#define PR_IDABLE_TFITEMS           // <-- Allows tf items to be ID'ed
#define PR_IDABLE_RUNES             // <-- Allows runes to be ID'ed
#define PR_ORIENTED_SIGHTS          // <-- Uses and sets the "oriented" sniper sight sprite
//------------------------------------------//
#endif // _FANCY_STUFF_                     //
//------------------------------------------//

//==================================//
// Tweakable settings from here...  //
//==================================//

#define PR_ORIENTED_SIGHT_OFFSET 0.1

#define PR_MOTD_REFRESHRATE        1

#define PR_GRUNTY_HP             600 // initial HP for soldier
#define PR_GRUNT_MAX_HP         1200 // OfN - Max health for the damn soldier, no 5000 hp grunts anymore! =)

/* moved to warlock.qc due to new hp system for monsters - Remains here as a reference to old values
#define SHAMBLER_MAX_HP          2600 // 2000 - 3000 - 2600
#define DEMON_MAX_HP             1800 // 1200 - 2000 - 1400
#define SCRAG_MAX_HP             1000 // 800
#define FISH_MAX_HP              1000 // was 850
#define GREMLIN_MAX_HP           2000*/

#define PR_OTR_DMG_FACTOR        1.15 // Damage an otr bullet does respect a normal one
#define PR_OTR_AMMO_COST         3 // Shells an OTR bullet takes when used

#define PR_SCRAG_DMG              42 // damage the scrag does // was 29
#define PR_WAYPOINT_LIFE         240 // 120 after this amount of seconds any unused waypoint will be removed
#define PR_MAXAMMO_CELLS         300 //WK Was 275 // was 320 // Engineer max is set on defs.qc PC_ENGINNER_XX
#define PR_SENTRY_UNLOCKTIME       2.5 // (must be greater than 1.1) time for the sentries to begin to rotate after they have no target (+/- 1 second)
#define PR_HAX_UNSTABLEDIST      300 // distance at which enemy hacks take longer cause of "unstable connection"
#define PR_WAYPOINT_AI_LIFE       10 // life for grunty ai created waypoints - last seen enemy mark
#define PR_ATTN_MONSTERDIE         PR_ATTN_NONE // #ATTN_NORM
#define PR_MINE_SCANRATE           0.6 // mines look again for nearby enemy after this time - it was 1 which caused high ping players to not trigger mines sometimes walking around them fast
#define PR_EXPBODY_DMG           160 // damage exp. body does /was 140

#define PR_BERSERKER_HP_COST      40 // was 30 (i think 50 originally)

//----- Damage factor settings --------//
#define PR_BERSERKER_DMGFACTOR        2.1 //2.4 //-was 2.65 was 2 then 2.2
#define PR_CHAPLAN_DMGFACTOR          1.8 // Original was 1.5??
#define PR_POWERAURA_DMGFACTOR        2

#define PR_RESISAURA_DMGFACTOR        0.6
#define PR_RESISRUNE_DMGFACTOR        0.5
#define PR_CHAPLAN_DEFENSE_DMGFACTOR  0.66

#define PR_MAX_OUT_DMGFACTOR          5
#define PR_MIN_IN_DMGFACTOR           0.25
//-------------------------------------//

#define PR_LASER_CANNON_DMG       36//26 // was 28

#define PR_GRUNTY_SEEKTIME         8

#define PR_BIO_GREN_RADIUS       200 // was 150

// Field generator
#define PR_FIELDGEN_NUMCELLS    150 // Was 120

// Holograph
#define PR_HOLO_POWER_COST     10   //
#define PR_HOLO_CYCLE_COST      2   //
#define PR_HOLO_CYCLE_TIME      1.5 //

#define PR_FLARE_DURATION      60   //

//#define TEAM_MINE_REPORTS

// field generator ranges
#define PR_FIELDGEN_RANGE          285 // 275 - max distance between the two generators to work
#define PR_FIELDGEN_HACKEDRANGE    475 // 450 - hacked generator distance
#define PR_FIELDGEN_HACKEDRANGE2   600 // Range for both gens hacked

//#define FIELD_FORCEMODE    // coment to get rid of the mode switch of the force field
#define PR_NO_FIELDGEN_INTERFERENCE

#define PR_BUILD_DISPENSER_MAX_DETS         2

#define PR_DISPENSER_MEDIKIT_MINIMUM        5 //15- Minimum medikit on dispenser to cure/heal player

#define PR_TELEPORTER_TURRETIZE_CELLS_COST  80 // was 125

#define PR_AURA_MONSTER_REGEN_FACTOR 2.5

// detpack fx
#ifdef PR_EXTRA_DETPACK_EXPLOSIONS
    #define PR_EXTRA_DETEXPLOSIONS       4 // 5 // was 6
#endif

#define PR_DETPACK_EARTHQUAKE // <-- Shakes players near detpacks explosion

#ifdef PR_DETPACK_EARTHQUAKE
    #define PR_DETPACK_EARTHQUAKE_RANGE  1200
#endif

#define PR_ADMIN_CMND_ALLOWED

#define PR_CEASEFIRE_ALLOWS_TO_MOVE
#define PR_CEASEFIRE_CPRINT_RATE   0.5

#define PR_AIRFIST_DONT_PUSH_TEAMMATES
#define PR_AIRFIST_DONT_HURT_TEAMMATES

#define PR_AIRFIST_DMG_REDUCTION    0.7 // was 0.65
#define PR_AIRFIST_RECOIL_REDUCTION 0.6
#define PR_AIRFIST_PUSH_REDUCTION   0.5 // Only applies to players pushed

#define PR_DONT_SET_AURA_TO_ENEMIES // unless they r spies, of course..
#define PR_ALLOW_AURAS_ON_CRUSADERS

#define PR_PUSHABLE_SCRAG // scrag pushable by the airfist?
#define PR_PUSHABLE_ARMY
#define PR_PUSHABLE_FIEND

#define PR_HUNTED_YELLOWTEAM_FACTOR 0.5 // this multiplied to current red players is max for yellow team on huntedr
#define PR_BORDER_FACTOR 0.5

#define PR_SENTRY_ROCKETSND "weapons/spike2.wav"
//#define SENTRY_ROCKETSND "weapons/rocket1i.wav"

#define PR_MAPCYCLE_SPEED 0.3 // Seconds, rate of map cycle advance (see client.qc)
#define PR_CYCLE_TIMEOUT 1 // Amount of seconds to delay the check for error on map cycle (see client.qc)

#define PR_GAS_GREN_CYCLES  9 // Original value = 3

//#define MINHP_TOKILL_MONSTERS   300 // Minimum hp monsters/soldiers must have to be able to unsummon/detonate them
// Obsolete --> #define MAX_PLAYER_HP    150 // Maximum player hp allowed

// Gibs/Heads/Monster corpses duration settings (in seconds)
#define PR_MIN_GIB_TIME    20
#define PR_MAX_GIB_TIME    40
#define PR_MIN_HEAD_TIME   30
#define PR_MAX_HEAD_TIME   60
#define PR_MIN_CORPSE_TIME 40 // Monster corpses only
#define PR_MAX_CORPSE_TIME 80 // Monster corpses only

// Corpses damage tolerance (strength until they get gibbed)
#ifdef PR_GIBABLE_CORPSES
#define PR_CORPSE_HP_PLAYER   180//270//360//180
#define PR_CORPSE_HP_SCRAG    140//210//280//140
#define PR_CORPSE_HP_FISH     120//180//240//120
#define PR_CORPSE_HP_DEMON    200//300//400//200
#define PR_CORPSE_HP_GREMLIN  220//330//440//220
#define PR_CORPSE_HP_SHAMBLER 340//510//680//340
#define PR_CORPSE_HP_ARMY     PR_CORPSE_HP_PLAYER
#endif

#define PR_MAX_CORPSE_BLOOD   15

// Kickable heads settings
#ifdef PR_KICKABLE_HEADS
#define PR_KICKHEAD_RATE           1
#define PR_KICKHEAD_RATE_PERUSER   1.5
#define PR_KICKHEAD_RATE_ALIVE     3
#endif

#define PR_DEFAULT_BODYQUE_SIZE    10 // Maximum player corpses/heads present in que (original was 4)

//#define CUSTOM_PC_USES_ITEMS
//#define CUSTOM_PC_ITEMS_PLAYING

// Old stuff enablers (should be commented out normally)
//#define OLD_HP_SYSTEM
//#define OLD_FLASH // Warning: If this one is ever defined it should be in defs.qc or earlier, not here
//#define OLD_CONCS
//#define OLD_FLAME
//#define OLD_NAILGREN
//#define OLD_TEAM_ANNOUNCEMENTS
//#define OLD_SBARLOOK

// Napalm grens
#define PR_NAPALM_FIRSTDMG 40
#define PR_NAPALM_MINRATE 0.1
#define PR_NAPALM_MAXRATE 0.4
#define PR_NAPALM_BURNTIMES 8
#define PR_NAPALM_MINDMG 60
#define PR_NAPALM_MAXDMG 120
#define PR_NAPALM_MAINDMG 140

#define PR_AIRFIST_CELLS_COST    5

#define PR_PSIONICS_TIME 4.5 // Seconds for the psionic gren effects to last
#define PR_PSIONIC_MOVEFACTOR 0.5
#define PR_PSIONIC_EXPLOSIONRANGE 250

#define PR_SENTRIES_MINIMUMHP_TODETONATE_FACTOR 0.5

#define PR_MINES_IGNORE_TEAMMATETOUCH
#define PR_MINE_MIRV_NUM 6
#define PR_MINE_NAPALM_BURNTIMES 12
#define PR_MINE_PSIONICS_TIME 6.5

#define PR_NAILGREN_DMG  20 // <-- original value was 12

#define PR_HACKER_ATTEMPTRATE 1

#define PR_SENTRY_SMOOTHROTATION
#define PR_SENTRY_ROTATIONSTEPS  36
#define PR_SENTRY_ROTATIONWIDTH  50

#define PR_INTERMISSION_LENFORCED  10 // Minimum seconds an intermission will last, Original was 5

#define PR_SPEEDAURA_FACTOR 1.3

// Rune settings
#define PR_SPEEDRUNE_FACTOR 1.35
#define PR_SPEEDRUNE_RELOADMULT 0.8

#define PR_FINALIZED_RATE_TEAMFLASH 1
#define PR_FINALIZED_DELAY_TOACTION 40

#define PR_MIN_MINUTES_TOVOTEMAP 0.2 // Minimum minutes to allow a map change using voting  // PZ: was 1

#define PR_TFITEM_SAFEDROP_TIME 2 // seconds for tfitems to not be pickeable by same player after tossing them

// Items/players color glowing settings
#ifdef PR_TEAMITEMS_COLOURED
#define PR_TEAMITEMS_COLORGLOW   // If defined, tf items only able to be picked by blue or red teams (like flags), will glow accordingly (only if 2 teams)
#define PR_TEAMITEMS_PLAYERCOLOR // If defined, players will glow with corresponding color if picked a team restricted tf item (like flags) (only if 2 teams)
#define PR_TEAMITEMS_FORCEDGLOW  // If TEAMITEMS_COLORGLOW and this are defined, team restricted items will always glow when droped, even if the contrary was specified
#endif

//#define FIXES_FOR_FLOATINGBUILDS
#define PR_REALISTIC_SHOTGUN_PARTS

#define PR_SUICIDE_RESPAWN_DELAY 3 // If ppl complain, set it to 8 :P Original was 5   // PZ: why 10? seems ridiculously long.. back to 5.. no, 3

#define PR_DEFAULT_CYCLEDIR cpcycle // Default was "qwmcycle"
#define PR_UNABLE_TOVOTE_FORSAME_MAP

//#define XMAS_STUFF
//#define LIGHTNING_PUSHES

//__________________________________________________________//
//                                                          //
// Stuff from here are NOT settings and they are ESSENTIAL  //
//__________________________________________________________//

// String flags (used for fast find() searches)
#define PR_STRFLAG_COOL               "cool"
#define PR_STRFLAG_MONSTER            "XX"
#define PR_STRFLAG_CORPSE             "corpse"
#define PR_STRFLAG_STORM_TARGET       "strmtrgt"
#define PR_STRFLAG_EARTHQUAKE_TARGET  "eqtarget"

//------------------------------------------------------------------//
// DEFINED FIELDS - Method to easily change them using preprocessor //
//------------------------------------------------------------------//

#define PR_runes     runes_owned
#define PR_mapflags  cutf_items   // PZ: was has_holo; need an int value

#define PR_monster_type  increase_team3
#define PR_monster_level increase_team4

#define PR_sname1 t_s_h
#define PR_sname2 t_s_m
#define PR_sname3 t_s_c

#define PR_stormflag       n_s_m
#define PR_earthquakeflag  n_s_h
#define PR_coolflag        message

#ifdef PR_COOP_MODE_ENHANCED
#define PR_monsterflag	team_str_carried
#else
#define PR_monsterflag     message
#endif

#define PR_corpseflag      message

#define PR_corpse_HP      if_group_is_removed

#define PR_disabledstuff1 display_item_status1
#define PR_disabledstuff2 display_item_status2
#define PR_disabledstuff3 display_item_status3
#define PR_disabledstuff4 display_item_status4
#define PR_disabledstuff5 restore_group_no
#define PR_givenstuff1    if_goal_is_active
#define PR_givenstuff2    if_goal_is_inactive
#define PR_givenstuff3    if_goal_is_removed
#define PR_givenstuff4    if_group_is_active
#define PR_givenstuff5    if_group_is_inactive

#define PR_cprint_fx rotate_type  // PZ: This was changed from `endtime` because it needs to be an int for C++.

//==========================================//
// Flag values and important type values    //
//==========================================//

// OfN - Possible map bit flags on world.#mapflags field
#define PR_MAPFLAG_WATERVISED  1
#define PR_MAPFLAG_SHOULDRAIN  2
#define PR_MAPFLAG_SHOULDSNOW  4
#define PR_MAPFLAG_FOG         8
#define PR_MAPFLAG_STORM       16
#define PR_MAPFLAG_EARTHQUAKE  32
#define PR_MAPFLAG_DAYTIME     64
#define PR_MAPFLAG_RUNES       128
#define PR_MAPFLAG_NOCUSTOM    256
#define PR_MAPFLAG_NOSTOCK     512
#define PR_MAPFLAG_NOEXTRAS    1024
#define PR_MAPFLAG_STOCKFREE   2048
#define PR_MAPFLAG_OBSERVER    4096
#define PR_MAPFLAG_NOTEAMSCORE 8192

// Client types - Mirror any changes with cpqwsv source code!!
#define PR_CLTYPE_DEFAULT    1
#define PR_CLTYPE_PROZACQW   2
#define PR_CLTYPE_FUHQUAKE   3
#define PR_CLTYPE_AMFQUAKE   4
#define PR_CLTYPE_ZQUAKE     5
#define PR_CLTYPE_QUAKEFORGE 6
#define PR_CLTYPE_FTEQUAKE   7
#define PR_CLTYPE_EZQUAKE    8
#define PR_CLTYPE_DARKPLACES 9  // PZ: added

// Rune bit flags - DO NOT MODIFY
#define PR_RUNE_TRIAD  1
#define PR_RUNE_RESIS  2
#define PR_RUNE_SPEED  4
#define PR_RUNE_REGEN  8

// Aura types - DO NOT MODIFY
#define PR_AURA_POWER	1
#define PR_AURA_RESIS  2
#define PR_AURA_HASTE	3
#define PR_AURA_INVIS	4
#define PR_AURA_REGEN  5
#define PR_AURA_SPEED  6

// Gib Kilograms <-> gib models (they *MUST* be integer values)
#define PR_GIB1_KGS 1 // this corresponds to the model: "progs/gib1.mdl" (the smallest one)
#define PR_GIB2_KGS 3 // this corresponds to the model: "progs/gib2.mdl" (the biggest one)
#define PR_GIB3_KGS 2 // this corresponds to the model: "progs/gib3.mdl" (the medium one)

// MegaTF mode flags
#define PR_MEGATF_MAPENTS  1
#define PR_MEGATF_STEPS    2
#define PR_MEGATF_SIGHTS   4
//#define MEGATF_DROPS    8

// Server state flags
/*#define SVSTATE_RAIN     1
#define SVSTATE_SNOW     2
#define SVSTATE_FOG      4
#define SVSTATE_HASLIT   8
#define SVSTATE_HASHRT   16
#define SVSTATE_HASSKY   32
#define SVSTATE_TEAM1POP 64
#define SVSTATE_TEAM2POP 64
#define SVSTATE_TEAM3POP 64
#define SVSTATE_TEAM4POP 64
*/

//====================================//
// Composed definitions               //
//====================================//

#ifdef PR_GIBABLE_CORPSES
    #define PR_USE_CORPSE_CODE
#endif

#ifdef PR_IDABLE_CORPSES
    #ifndef PR_USE_CORPSE_CODE
        #define PR_USE_CORPSE_CODE
    #endif
#endif

} // END namespace Progs

#endif // END OFNDEFS_H
