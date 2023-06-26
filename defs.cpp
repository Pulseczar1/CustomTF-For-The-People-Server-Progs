/*
==============================================================================

            SOURCE FOR GLOBALVARS_T C STRUCTURE

==============================================================================
*/

#include "progs.h"

namespace Progs {

//
// system globals
//
entity      self;
entity      other;
entity      world;              // PZ NOTE: I'm not making this const yet, because there is engine code that sets this to the first entity. (FIXME?)
float       time;
float       frametime;

#ifdef PR_QUAKE_WORLD
entity      newmis;             // if this is set, the entity that just
                                // run created a new missile that should
                                // be simulated immediately
#endif

float       force_retouch;      // force all entities to touch triggers
                                // next frame.  this is needed because
                                // non-moving things don't normally scan
                                // for triggers, and when a trigger is
                                // created (like a teleport trigger), it
                                // needs to catch everything.
                                // decremented each frame, so set to 2
                                // to guarantee everything is touched
string      mapname;

#ifndef PR_QUAKE_WORLD
float       deathmatch;
float       coop;
float       teamplay;
#endif

int         serverflags;        // propagated from level to level, used to   // PZ: was float
                                // keep track of completed episodes

float       total_secrets;
float       total_monsters;

float       found_secrets;      // number of secrets found
float       killed_monsters;    // number of monsters killed


// spawnparms are used to encode information about clients across server
// level changes
float       parm1, parm2, parm3, parm4, parm5, parm6, parm7, parm8, parm9, parm10, parm11, parm12, parm13, parm14, parm15, parm16;

//
// global variables set by built in functions
//
vector      v_forward, v_up, v_right;   // set by makevectors()

// set by traceline / tracebox
float       trace_allsolid;
float       trace_startsolid;
float       trace_fraction;
vector      trace_endpos;
vector      trace_plane_normal;
float       trace_plane_dist;
entity      trace_ent;
float       trace_inopen;
float       trace_inwater;

entity      msg_entity;             // destination of single entity writes

//================================================
// PZ: The compiler stops listing out field variables into progdefs.h, when it encounters this variable. Why would we not want it to list
// them all? The list of variables and such in progdefs.h allows the C code to know the names of all the QuakeC stuff. If it doesn't know the
// names, then debug stuff, like traceon(), will display "???" in place of the name. Commenting this out. (There was a lot to change in the C
// code to make this work.)
//void        end_sys_globals;        // flag for structure dumping
//================================================

/*
==============================================================================

            SOURCE FOR ENTVARS_T C STRUCTURE

==============================================================================
*/

//
// system fields (*** = do not set in prog code, maintained by C code)
//
FIELD(".float      modelindex;")     // *** model index in the precached list
FIELD(".vector     absmin, absmax;") // *** origin + mins / maxs

FIELD(".float      ltime;")          // local time for entity (Used for MOVETYPE_PUSH stuff)
#ifdef PR_QUAKE_WORLD
FIELD(".float      lastruntime;")    // *** to allow entities to run out of sequence
#endif

FIELD(".float      movetype;")
FIELD(".float      solid;")

FIELD(".vector     origin;")         // ***
FIELD(".vector     oldorigin;")      // ***
FIELD(".vector     velocity;")
FIELD(".vector     angles;")
FIELD(".vector     avelocity;")

#ifndef PR_QUAKE_WORLD
FIELD(".vector     punchangle;")     // temp angle adjust from damage or recoil
#endif

FIELD(".string     classname;")      // spawn function
FIELD(".string     model;")
FIELD(".float      frame;")
FIELD(".int        skin;")
FIELD(".float      effects;")

FIELD(".vector     mins, maxs;")     // bounding box extents reletive to origin
FIELD(".vector     size;")           // maxs - mins

FIELD(".void touch();")
FIELD(".void use();")
FIELD(".void think();")
FIELD(".void blocked();")        // for doors or plats, called when can't push other

FIELD(".float      nextthink;")
FIELD(".entity     groundentity;")

// stats
FIELD(".float      health;")
FIELD(".float      frags;")
FIELD(".float      weapon;")         // one of the IT_SHOTGUN, etc flags
FIELD(".string     weaponmodel;")
FIELD(".float      weaponframe;")
FIELD(".float      currentammo;")
FIELD(".float      ammo_shells, ammo_nails, ammo_rockets, ammo_cells;")

FIELD(".float      items;")          // bit flags

FIELD(".float      takedamage;")
FIELD(".entity     chain;")
FIELD(".float      deadflag;")

FIELD(".vector     view_ofs;")       // add to origin to get eye point


FIELD(".float      button0;")        // fire
FIELD(".float      button1;")        // use
FIELD(".float      button2;")        // jump


FIELD(".float      impulse;")        // weapon changes

FIELD(".float      fixangle;")
FIELD(".vector     v_angle;")        // view / targeting angle for players
#ifndef PR_QUAKE_WORLD
FIELD(".float      idealpitch;")     // calculated pitch angle for lookup up slopes
#endif


FIELD(".string     netname;")

FIELD(".entity     enemy;")          //WK We use this on players now, for the martyr code

FIELD(".float      flags;")

FIELD(".float      colormap;")
FIELD(".float      team;")

FIELD(".float      max_health;")     // players maximum health is stored here

FIELD(".float      teleport_time;")  // don't back up

FIELD(".float      armortype;")      // save this fraction of incoming damage


FIELD(".float      armorvalue;")     //Current NUMBER of armor being worn

FIELD(".float      waterlevel;")     // 0 = not in, 1 = feet, 2 = wast, 3 = eyes
FIELD(".float      watertype;")      // a contents value

FIELD(".float      ideal_yaw;")
FIELD(".float      yaw_speed;")

FIELD(".entity     aiment;")

FIELD(".entity     goalentity;")     // a movetarget or an enemy

FIELD(".float      spawnflags;")

FIELD(".string     target;")
FIELD(".string     targetname;")

// damage is accumulated through a frame. and sent as one single
// message, so the super shotgun doesn't generate huge messages
FIELD(".float      dmg_take;")
FIELD(".float      dmg_save;")
FIELD(".entity     dmg_inflictor;")

FIELD(".entity     owner;")          // who launched a missile
FIELD(".vector     movedir;")        // mostly for doors, but also used for waterjump

FIELD(".string     message;")        // trigger messages

FIELD(".float      sounds;")         // either a cd track number or sound number

FIELD(".string     noise, noise1, noise2, noise3;")  // contains names of wavs to play

//================================================
// PZ: The compiler stops listing out field variables into progdefs.h, when it encounters this variable. Why would we not want it to list
// them all? The list of variables and such in progdefs.h allows the C code to know the names of all the QuakeC stuff. If it doesn't know the
// names, then debug stuff, like traceon(), will display "???" in place of the name. Commenting this out.
//void        end_sys_fields;         // flag for structure dumping
//================================================

/*
==============================================================================

                VARS NOT REFERENCED BY C CODE

==============================================================================
*/

//
// globals
//
float       movedist;

#ifndef     PR_QUAKE_WORLD
float       gameover;           // set when a rule exits
#endif

string      string_null;        // null string, nothing should be held here
float       empty_float;
vector      vector_null;        // CH empty vector

entity      activator;          // the entity that activated a trigger or brush

entity      damage_attacker;    // set by T_Damage
float       framecount;

float       skill;

//
// cvars checked each frame
//
int         teamplay;    // PZ: Changed from float to int for bitwise operations.
float       timelimit;
float       fraglimit;
float       deathmatch;

//================================================

//
// world fields (FIXME: make globals)
//
FIELD(".string     wad;")
FIELD(".string     map;")
FIELD(".float      worldtype;")  // 0=medieval 1=metal 2=base

//================================================

FIELD(".string     killtarget;")

//
// quakeed fields
//
FIELD(".float      light_lev;")      // not used by game, but parsed by light util
FIELD(".float      style;")

//
// monster ai
//

FIELD(".void th_stand();")
FIELD(".void th_walk();")
FIELD(".void th_run();")
FIELD(".void th_missile();")
FIELD(".void th_melee();")
FIELD(".void th_pain(entity attacker, float damage);")
FIELD(".void th_die();")

FIELD(".entity     oldenemy;")       // mad at this player before taking damage

FIELD(".float      speed;")

FIELD(".float      lefty;")

FIELD(".float      search_time;")    //WK Used for hover boots. :/
FIELD(".float      attack_state;")   //WK Not usable?

//
// player only fields
//
FIELD(".float      walkframe;")

//WK
FIELD(".float      maxspeed;")
FIELD(".float      gravity;")      // PZ NOTE: This appears to be a percentage value between 0 and 1, relative to the sv_gravity setting. Am I right?
//WK -- Holds how much money we have left while building, and
// for toggling between engineer and spy menus in the game (hackish, yes)
FIELD(".float      money;")
FIELD(".float      custom_speed;")
FIELD(".float      ff_count;")     //How many friendlies killed
//.float    be_count;     //Bad Engy count, break threshold, RESTICTED BUILD
FIELD(".float      penance_time;") //Time your penance will cease
FIELD(".float      last_attacked_time;") //Time that you were last shot at
FIELD(".entity     inspirator;")   //Holds pointer to your chaplan
FIELD(".float      has_cheated;")  //Holds if the user has tried to cheat or not
// SB - for demons
//.float    demon_points;  UNUSED - OfN
//.float    demon_choice;
FIELD(".float      demon_blood;")
FIELD(".entity     demon_one;")
FIELD(".entity     demon_two;")    //- OfN
FIELD(".entity     demon_three;")  // OfN
//.entity   demon_two;
//.float    has_monster;
//.entity   demon_p;      // current demon selected

//- OfN -
//.float    army_ready; //- if #TRUE, soldier ready for teleport, this is set by the army_timer's
                //--> Uses delay_time now
//.float    num_mines; -->all_active // OfN Number of guerilla mines the player has
                       // Also used for friendly hacks on machines

//- OfN - Cool gibs data stored on each TF_T_Damage
//.vector   gb_origin; // Origin of last damage
//.float    gb_disable; // if #TRUE don't use cool gibs

// hacker defences
//.float    hacker_defence; // -OfN by SB, I use all_active instead, for hacker improvements/ice

//For the demon
FIELD(".void th_fireball();")

FIELD(".float      hover_time;") //How much fuel we have left in boots

FIELD(".int        done_custom;")

//END WK

FIELD(".entity     scaned;") //CH used for scanner.

FIELD(".float      attack_finished;")
FIELD(".float      pain_finished;")

FIELD(".float      invincible_finished;")
FIELD(".float      invisible_finished;")
FIELD(".float      super_damage_finished;")
FIELD(".float      radsuit_finished;")

FIELD(".float      invincible_time, invincible_sound;")
FIELD(".float      invisible_time, invisible_sound;")
FIELD(".float      super_time, super_sound;")
FIELD(".float      rad_time;")
FIELD(".float      fly_sound;")

FIELD(".float      axhitme;")

FIELD(".float      show_hostile;")   // set to time+0.2 whenever a client fires a
                                     // weapon or takes damage.  Used to alert
                                     // monsters that otherwise would let the player go
FIELD(".float      timeLastJumped;") // PZ: this was useless; repurposed as a timer to indicate how long since a player jumped  // OLD: player jump flag (was called "jump_flag")
FIELD(".float      prev_velocity_z;")// PZ: I had broken 'landing detection' by changing this. So, I put things back the way they were, but renamed 'jump_flag' to 'prev_velocity_z'. (8-15-15)
FIELD(".float      swim_flag;")      // player swimming sound flag
FIELD(".float      air_finished;")   // when time > air_finished, start drowning
FIELD(".float      bubble_count;")   // keeps track of the number of bubbles
FIELD(".string     deathtype;")      // keeps track of how the player died

//
// object stuff
//
FIELD(".string     mdl;")
FIELD(".vector     mangle;")         // angle at start

FIELD(".vector     oldorigin;")      // only used by secret door

FIELD(".float      t_length, t_width;")

//
// doors, etc
//
FIELD(".vector     dest, dest1, dest2;")
FIELD(".float      wait;")           // time from firing to restarting
FIELD(".float      delay;")          // time from activation to firing
FIELD(".entity     trigger_field;")  // door's trigger entity
FIELD(".string     noise4;")

//
// monsters
//
FIELD(".float      pausetime;")
FIELD(".entity     movetarget;")

//
// doors
//
FIELD(".int        aflag;")
FIELD(".float      dmg;")            // damage done by door when hit

//
// misc
//
FIELD(".float      cnt;")            // misc flag

//
// subs
//
FIELD(".void think1();")
FIELD(".vector     finaldest, finalangle;")

//
// triggers
//
FIELD(".float      count;")          // for counting triggers

//
// plats / doors / buttons
//
FIELD(".float      lip;")
FIELD(".float      state;")
FIELD(".vector     pos1, pos2;")     // top and bottom positions
FIELD(".float      height;")

//
// sounds
//
FIELD(".float      waitmin, waitmax;")
FIELD(".float      distance;")
FIELD(".float      volume;")

//===========================================================================


//===========================================================================
// TEAMFORTRESS Defs
//===========================================================================
FIELD(".float      playerclass;")
FIELD(".float      nextpc;")             // The playerclass you'll respawn as
FIELD(".float      last_impulse;")       // The previous impulse command from this player
FIELD(".int        armorclass;")         // Type of *special* armor being worn
FIELD(".int        tf_items;")           // Another flag for player items
FIELD(".int        job;")                // WK Yet another flag for player items //WK CAUTION: IF YOU EVER CHANGE THIS, CPQWSV THIEF CODE WILL BREAK
FIELD(".float      job_finished;")       // WK Time for job ability to end/reload
FIELD(".int        tf_items_flags;")     // Flags for the player items
FIELD(".float      no_grenades_1;")      // Number of grenades of type 1 being carried
FIELD(".float      no_grenades_2;")      // Number of grenades of type 2 being carried
FIELD(".float      tp_grenades_1;")      // 1st type of grenades being carried
FIELD(".float      tp_grenades_2;")      // 2nd type of grenades being carried
FIELD(".float      got_aliases;")        // TRUE if the player has TeamFortress aliases
FIELD(".float      cheat_check;")        // Time when we'll next check for team cheats
FIELD(".float      is_removed;")         // TRUE if the entity has been removed
FIELD(".float      is_undercover;")      // TRUE for a SPY if they're undercover
FIELD(".float      is_building;")        // TRUE for an ENGINEER if they're building something
FIELD(".float      is_detpacking;")      // TRUE for a DEMOMAN if they're setting a detpack
FIELD(".float      is_feigning;")        // TRUE for a SPY if they're feigning death
FIELD(".float      is_haxxxoring;")      // TRUE if the player is hax0ring something
FIELD(".float      is_toffingadet;")     // TRUE if the player is arming a C4 det
FIELD(".float      is_unabletospy;")     // TRUE for a SPY if they can't go undercover
FIELD(".int        is_malfunctioning;")  //- SB/OfN For buildings and cyberaug players to determine if screwed up
FIELD(".float      is_abouttodie;")      // Oh no I am a martyr and about to die
//.float    is_cameraviewing;   // We're gazing out of the camera

FIELD(".float      is_killed;")          // avoids stack overflow on killed() ?? (only skips killed() for buildings)
FIELD(".float      option;")             // - OfN - used for megatf effect_random_sound support, and for camera/sensor build-destroy overflow control
FIELD(".float      option2;")            // - OfN - used for megatf effect_random_sound support, and in cmnd.qc

FIELD(".float      is_connected;")       // FALSE if the player has disconnected
FIELD(".float      has_dispenser;")      // TRUE for an ENGINEER if he has a dispenser
FIELD(".int        has_sentry;")         // TRUE for an ENGINEER if he has a sentry
FIELD(".int        has_tesla;")          // TRUE for an ENGINEER if he has a tesla
FIELD(".int        has_camera;")         // TRUE for an ENGINEER if he has a camera
FIELD(".float      has_sensor;")         // TRUE for an ENGINEER if he has a motion sensor
FIELD(".float      has_teleporter;")     // contains # of teleporters
FIELD(".float      has_fieldgen;")       // contains # of field generators
FIELD(".float      admin_flag;")         // TRUE if player has admin access
FIELD(".entity     admin_kick;")         // Contains the current client to kick
FIELD(".entity     martyr_enemy;")       // The last guy that shot the martyr. self = nobody
FIELD(".float      stored_deathmsg;")    // OfN - UNUSED? - ofn nope hehe
FIELD(".float      cluster_mode;")       // 0=nothing 1=normal 2=cluster

//- OfN fields ---------------------------------//
FIELD(".float has_holo;")      // holds player's holo status   // PZ NOTE: this needs to be a float, because it's often used to hold a time value
// Rotation entities fields
FIELD(".vector neworigin;")
FIELD(".vector rotate;")
FIELD(".float  endtime;")
FIELD(".int    rotate_type;")  // PZ: Changed from float to int, so that I could use this for PR_cprint_fx, rather than using endtime, which must be a float.
FIELD(".string path;")
FIELD(".string group;")
FIELD(".string event;")
FIELD(".float duration;")
// AirFist fields -
FIELD(".float AIRG_Timeout;")
FIELD(".float AIRG_FireCount;")
FIELD(".int AIRG_Flags;")
FIELD(".entity AIRG_FlyTracker;")

// crusader fields
FIELD(".float  aura;")       // The aura of the player
FIELD(".float  aura_time;")  // When the aura runs out
FIELD(".entity  crusader_inspirator;") // different than inspirator for chapplan

// Runes field
//.float runes_owned;

// laser cannon fields -
// NONE, i used alternative ones
//.vector old_velocity; // I use neworigin
FIELD(".float reload_laser_cannon;")

//----------------------------------------------//

// TeamFortress State Flags
FIELD(".int  tfstate;")                        // State flags for TeamFortress

FIELD(".entity linked_list;")        // Used just like chain. Has to be separate so
                            // it doesn't interfere with chain. See T_RadiusScan
FIELD(".entity observer_list;")      // Used by undefined classes, see TF_MovePlayer

// Playerclass handling variables
FIELD(".float  maxammo_shells;")                 // Class holding details
FIELD(".float  maxammo_nails;")
FIELD(".float  maxammo_cells;")
FIELD(".float  maxammo_rockets;")
FIELD(".int    items_allowed;")
FIELD(".float  armor_allowed;")                  // Maximum color armor we can have. 0.3 for scouts...
FIELD(".float  maxarmor;")                       // Maximum number of armor we can have. 200 for soldiers...
//WK .float maxfbspeed;                 // Maximum forward/back speed
//WK .float maxstrafespeed;             // Maximum side speed

FIELD(".float  weaponmode;")                     // Used for multiple mode weapons

FIELD(".float  motd;")                           // Used to display MOTD
FIELD(".float  current_menu;")                   // is set to the number of the current menu, is 0 if they are not in a menu
FIELD(".float  menu_count;")                     // keeps track of display times for menus
FIELD(".float  menu_displaytime;")               // keep track of how many times this menu is refreshed

/*==================================================*/
/* Toggleable Game Settings                         */
/*==================================================*/
int toggleflags;                      // toggleable flags

float respawn_delay_time;

// FortressMap stuff
float number_of_teams;  // number of teams supported by the map
int   illegalclasses;   // Illegal playerclasses for all teams
int   illegalclasses1;  // Illegal playerclasses for team 1
int   illegalclasses2;  // Illegal playerclasses for team 2
int   illegalclasses3;  // Illegal playerclasses for team 3
int   illegalclasses4;  // Illegal playerclasses for team 4
int   civilianteams;    // Bitfield holding Civilian teams

float team1col;     // Colours for each of the 4 teams
float team2col;     // If there's enough demand, we'll
float team3col;     // allow more than four teams...
float team4col;     // but there's no demand at all, so no
float team1score;   // Goal Score of each team
float team2score;
float team3score;
float team4score;
float team1lives;   // Number of lives each team's players have
float team2lives;
float team3lives;
float team4lives;

//--- OfN global vars ----//
float pay_msgs;
float team_prefix;
float ceasefire;
float drop_items;

float no_grapple;
//float headless;

float custom_mode;
float stock_mode;
float extras_mode;

float nicecolors;
float relax_cheatcheck;

//float cool_gibs;
//float smooth_sentry;

float army_delay;
//float no_monstercolors;
float no_detpush;
//- REMOTE DEBUG GLOBALS -//
entity debug_target; // server dprints will go to this client too
float allow_debug;
//-------------------------//
float allow_watermonsters; // if so, grunts and monsters can be in water, in check_contents ai.qc

float spec_menu;

float bodyque_size;

float num_clients;  // PZ: Added. This is the number of all client slots in use. Includes players downloading/connecting.
float num_players;  // PZ NOTE: This is the number of players that are actually in the game. Does not include downloaders.
float num_specs;

float current_brightness;
float daylight;

int storm;
int earthquake;
int daytime;
int runes;

float noon_light;
float midnight_light;
float specialstate;

float intermission_running; // OfN - moved from client.qc

float sniper_factor;

float no_punish;
float no_votemap;

float no_bright;

int disabledstuff1;
int disabledstuff2;
int disabledstuff3;
int disabledstuff4;
int disabledstuff5;

int givenstuff1;
int givenstuff2;
int givenstuff3;
int givenstuff4;
int givenstuff5;

int megatf;

float PZ_DEBUG; // PZ: when this is on, (debug) messages will show, that otherwise would not have shown

//string lastip_vote;
//string lastip_tk;

//-----//------//
entity current_voteent;
string last_vote_starter; // added for AGR mode.. refers to which player started the current vote -PZ
float votesleeping; // TODO --> Make bit inside server status float that include blink team score flags?

// Chris' Teamplay Plus Mode
/*float chris; // Is Chris mode on or off

float team1rounds; // No. of rounds won per team
float team2rounds;
float team3rounds;
float team4rounds;

float team1total; // no. of players alive per team
float team2total;
float team3total;
float team4total;

float roundstowin; // No. of rounds required to win
float roundendtime; // The end time of the current round
float roundtime; // Time per round
float livesperguy; // Number of lives per guy
// end Chris*/

float team1maxplayers;  // Max number of players allowed in each team
float team2maxplayers;
float team3maxplayers;
float team4maxplayers;
float team1advantage;  // only used if the teamplay equalisation bits are set
float team2advantage;  // stores the damage ratio players take/give
float team3advantage;
float team4advantage;
//WK Holds the next time that we'll allow a spam grenade to be thrown.
float team1nextspam;
float team2nextspam;
float team3nextspam;
float team4nextspam;

FIELD(".float team_no;")     // The team you belong to
//.float old_team_no; // WK The team you used to belong to
FIELD(".float new_team_no;")   // PZ: took out .old_team_no because it wasn't being used; replaced with this for "changeteam" command
FIELD(".float lives;")       // The number of lives you have left

FIELD(".float infection_team_no;") // The team_no of the person who infected you

// CTF stuff
float CTF_Map;

#ifdef PR_QUAKE_WORLD
float       coop;
float       rj;                 // Rocket Jump Modifier
#endif

FIELD(".entity building;")     // The building the ENGINEER is using
FIELD(".float building_wait;") // Used to prevent using a building again immediately
FIELD(".entity real_owner;")

FIELD(".float real_frags;")  // Used to store the players frags when TeamFrags is On

FIELD(".float respawn_time;")            // players can't respawn within this time.
FIELD(".float suicide_time;")            // players can't suicide within this time.

/*==================================================*/
/* New Weapon Defines                               */
/*==================================================*/
FIELD(".int weapons_carried;")           // the weapons the player is carrying
FIELD(".int current_weapon;")            // the currently selected weapon being carried in the player's hands

//WK On to the third 24-wide bitfield for holding purchases
FIELD(".int cutf_items;")

// Gizmo - fourth bitfield; for the more optional equipment page, and anything else that remotely fits into that category
FIELD(".int cutf_moreitems;")                  // cutfMoreItems

// Medikit
FIELD(".float ammo_medikit;")                        // Ammo used for the medikit
FIELD(".float maxammo_medikit;")

// Detpack
FIELD(".float ammo_detpack;")                    // Ammo used for the detpack
FIELD(".float maxammo_detpack;")
FIELD(".float ammo_c4det;") // ammo for c4 throwable det

/*==================================================*/
/* Variables used for New Weapons and Reloading     */
/*==================================================*/
FIELD(".float reload_shotgun;")
FIELD(".float reload_super_shotgun;")
FIELD(".float reload_grenade_launcher;")
FIELD(".float reload_rocket_launcher;")
FIELD(".float reload_light_assault;")

// Assault Cannon
FIELD(".float heat;")

// Team Color Cheat Checking
FIELD(".float immune_to_chec;") //xxxx

// Make sure people don't do too many saveme sounds
FIELD(".float last_saveme_sound;")
FIELD(".float last_grenade_primed;") //WK 2/8/7 Only allow priming one grenade per 3 seconds

// Variables used in the Goals
FIELD(".float goal_no;")                 // Goal number of this goal
FIELD(".float group_no;")                // Goal group this goal is in
FIELD(".int goal_state;")                // State of this goal
FIELD(".float owned_by;")                // The team this goal/item/whatever belongs to

// Goal Activation details
FIELD(".int goal_activation;")           // Bitfields. Determines how this goal is activated
FIELD(".int goal_effects;")              // Bitfields. Determines which players are affected by this goal
FIELD(".int goal_result;")               // Bitfields. Determines the results of activation
FIELD(".float goal_group;")              // Bitfields. Determines how groups are affected by activation of this goal

FIELD(".float else_goal;")               // A Goal that's activated if this one fails to activate

FIELD(".int if_goal_is_active;")
FIELD(".int if_goal_is_inactive;")
FIELD(".int if_goal_is_removed;")
FIELD(".int if_group_is_active;")
FIELD(".int if_group_is_inactive;")
FIELD(".int if_group_is_removed;")

// Goal Result details
FIELD(".float activate_goal_no;")
FIELD(".float inactivate_goal_no;")
FIELD(".float remove_goal_no;")
FIELD(".float restore_goal_no;")
FIELD(".float activate_group_no;")
FIELD(".float inactivate_group_no;")
FIELD(".float remove_group_no;")
FIELD(".int   restore_group_no;")

// Item State behaviour
FIELD(".float has_item_from_group;")
FIELD(".float remove_item_group;")
FIELD(".float return_item_no;")
FIELD(".float if_item_has_moved;")
FIELD(".float if_item_hasnt_moved;")

// Spawnpoint behaviour
FIELD(".float remove_spawnpoint;")
FIELD(".float restore_spawnpoint;")
FIELD(".float remove_spawngroup;")
FIELD(".float restore_spawngroup;")

// Item Displaying details
FIELD(".int display_item_status1;")      // Goal displays the status of these items
FIELD(".int display_item_status2;")
FIELD(".int display_item_status3;")
FIELD(".int display_item_status4;")
FIELD(".string team_str_home;")          // Displayed when the item is at home base
FIELD(".string team_str_moved;")         // Displayed when the item has been moved
FIELD(".string team_str_carried;")       // Displayed when the item is being carried
FIELD(".string non_team_str_home;")      // Displayed when the item is at home base
FIELD(".string non_team_str_moved;")     // Displayed when the item has been moved
FIELD(".string non_team_str_carried;")   // Displayed when the item is being carried

FIELD(".vector goal_min;") //CH
FIELD(".vector goal_max;") //CH

// Goal/Timer/GoalItem/Trigger existence checking
FIELD(".float ex_skill_min;")            // Exists when the skill is >= this value
FIELD(".float ex_skill_max;")            // Exists when the skill is <= this value
//.float ex_num_players_min;    // Exists when the number of players is >= this value
//.float ex_num_players_max;    // Exists when the number of players is <= this value

// Score increases
FIELD(".float increase_team1;")          // Increase the scores of particular teams
FIELD(".float increase_team2;")
FIELD(".float increase_team3;")
FIELD(".float increase_team4;")

// Centerprinting
FIELD(".string broadcast;")              // Centerprinted to all, overridden by the next two
FIELD(".string team_broadcast;")         // Centerprinted to AP's team members, but not the AP
FIELD(".string non_team_broadcast;")     // Centerprinted to non AP's team members
FIELD(".string owners_team_broadcast;")  // Centerprinted to the members of the team that own the Goal/Item
FIELD(".string netname_broadcast;")              // same as above, prepended by AP netname and bprinted
FIELD(".string netname_team_broadcast;")         // same as above, prepended by AP netname and bprinted
FIELD(".string netname_non_team_broadcast;")     // same as above, prepended by AP netname and bprinted
FIELD(".string netname_owners_team_broadcast;")  // same as above, prepended by AP netname and bprinted

FIELD(".string team_drop;")              // Centerprinted to item owners team
FIELD(".string non_team_drop;")          // Centerprinted to everone not on item owners team
FIELD(".string netname_team_drop;")      // same as above, prepended by AP netname and bprinted
FIELD(".string netname_non_team_drop;")  // same as above, prepended by AP netname and bprinted

string team_menu_string;
FIELD(".int all_active;")

FIELD(".int item_list;")    // Used to keep track of which goalitems are
                            // affecting the player at any time.
                            // GoalItems use it to keep track of their own
                            // mask to apply to a player's item_list
float item_list_bit;        // Global, used to determine what the bit of
                            // each new GoalItem will be.

FIELD(".float delay_time;")          // For delayed goal results
FIELD(".int dont_do_triggerwork;")

// Abbreviations for the above
FIELD(".float g_a;")                 // goal_activation
FIELD(".float g_e;")                 // goal_effects

FIELD(".string t_s_h;")              // team_str_home
FIELD(".string t_s_m;")              // team_str_moved
FIELD(".string t_s_c;")              // team_str_carried
FIELD(".string n_s_h;")              // non_team_str_home
FIELD(".string n_s_m;")              // non_team_str_moved
FIELD(".string n_s_c;")              // non_team_str_carried

FIELD(".string b_b;")                // broadcast
FIELD(".string b_t;")                // team_broadcast
FIELD(".string b_n;")                // non_team_broadcast
FIELD(".string b_o;")                // owners_team_broadcast
FIELD(".string n_b;")                // netname_broadcast
FIELD(".string n_t;")                // netname_team_broadcast
FIELD(".string n_n;")                // netname_non_team_broadcast
FIELD(".string n_o;")                // netname_owners_team_broadcast

FIELD(".string d_t;")                // team_drop
FIELD(".string d_n;")                // non_team_drop
FIELD(".string d_n_t;")              // netname_team_drop
FIELD(".string d_n_n;")              // netname_non_team_drop

// Attackers Go Red mode -PZ
// cvars - requires 'localinfo' to set these
float agr;                        // globar cvar, just on or off (requires 'localinfo agr 1' to be on)
float agr_frags;                  // frags blue gets for successfully defending for agr_minutes minutes
float agr_minutes;                // number of minutes blue has to successfully defend in order to be
                                  // rewarded frags
float agr_vote_max;               // overrides default maximum number of players allowed to vote for AGR
float agr_teamfrags;              // overrides the serverinfo 'teamfrags' (or 't') var when AGR is active,
                                  // which controls whether or not each players' frags are equal to their
                                  // team's score
// globals
float agrTimeToRewardBlue;        // don't try to set it (it's set by functions)
float agrTimerRunning;            // boolean
float agrTimeOfLastSound;         // in seconds, what the timer was on last timer sound
float agrFlagRespawnWait;         // is flag currently waiting to spawn? (done after captures)
// end AGR

// MOVED THESE VARIABLES TO THE INVADE MODULE.
// Invade mode -PZ  (Like AGR, but teams alternate between offense and defense, like on the map invade4a.)
// globals for cvars - requires 'localinfo' to set these (see below)
// TODO: NEED TO MAKE SURE THESE VARIABLES ARE UPDATED IN progdefs.h AND progdefs.cpp, SO THAT THE ENGINE CAN SEE THEM.
/*float invade;                      // globar cvar, just on or off (requires 'localinfo invade 1' to be on)
float invade_frags;                // frags defense gets for successfully defending for invade_minutes minutes
float invade_minutes;              // how long offensive team has to try to cap the flag
float invade_teamfrags;            // overrides the serverinfo 'teamfrags' (or 't') var when Invade is active,
                                   // which controls whether or not each players' frags are equal to their
                                   // team's score
float invade_prematchTime;         // how long Invade prematches will last (in seconds)

// non-cvar globals
//enum class InvadeGameState { STOPPED, IN_PREMATCH, GAME_RUNNING };  // MOVED TO progdefs.h.
InvadeGameState invade_gameState; // NOTE: When game is running, if we are using a capture timer, it is running.
float invade_teamOnDef;
float invade_swapRoles;            // flag is set when it's time to swap team roles
float invade_timePrematchEnds;     // when [time] reaches this time, Invade prematch ends
float invade_timeOfLastSound;      // last time we played a timer sound
float invade_timeToRewardDef;      // don't try to set it (it's set by functions)
*/
// END Invade

// PZ - can't believe these globals didn't already exist.. created them for AGR mode.
float playersOnTeam1;
float playersOnTeam2;
float playersOnTeam3;
float playersOnTeam4;

/*==========================================================================*/
/* Flamethrower                                                             */
/*==========================================================================*/
float num_world_flames;

FIELD(".float numflames;") //number of flames on entity
// .float timecount; //timelength of the flames on entity
// this uses .health now
FIELD(".string flame_id;")  // has to be string so that the C function find() can be used

/*==========================================================================*/
/* Limit handling for various things                                        */
/*==========================================================================*/
// pipebomb limits
// when the limit is exceeded, older pipebombs explode
float num_world_pipebombs;
float num_team_pipebombs_1;
float num_team_pipebombs_2;
float num_team_pipebombs_3;
float num_team_pipebombs_4;

// ammobox limits
// when the limit is exceeded, older ammoboxes disappear
float num_world_ammoboxes;
float num_team_ammoboxes_1;
float num_team_ammoboxes_2;
float num_team_ammoboxes_3;
float num_team_ammoboxes_4;

/*==================================================*/
/* Spy variables                                    */
/*==================================================*/
FIELD(".float undercover_team;")     // The team the Spy is pretending to be in
FIELD(".float undercover_skin;")     // The skin the Spy is pretending to have
FIELD(".string undercover_name;")    // The name of the player the Spy is pretending to be

float spy_off;              // Spy class off/on
float invis_only;           // Spy invisible instead of color/skin changing
float allow_hook;           // Allow players to use the hook
float jello;                // WK Jello water! If this is true, you bounce on water
float light_damage;         // WK Armor more expensive, damage at 75%
float prematch;             // WK Jello water! If this is true, you bounce on water
float bounty;               // WK Bounty system off/on
float custom_money;         // WK Set starting money
float team_with_flag;       // CG steal4d who has flag code
int friends1_mask;        // KK masks of friendly teams, code for > 2 team maps
int friends2_mask;
int friends3_mask;
int friends4_mask;
float normalQuakeMap;       // PZ: this is set to true when map was detected as not a TF or CTF map

FIELD(".float  hook_out;")

/*==================================================*/
/* Camera defines                                   */
/*==================================================*/
#ifdef PR_DEMO_STUFF
float live_camera;
FIELD(".float camdist;")
FIELD(".vector camangle;")
FIELD(".entity camera_list;")
#endif

/*==================================================*/
/* QuakeWorld defines                               */
/*==================================================*/
// some redecs for printing in QW

float already_chosen_map;
float triggered_cycle;

#ifndef PR_QUAKE_WORLD
entity      newmis;         // if this is set, the entity that just
                            // run created a new missile that should
                            // be simulated immediately
#endif

// grappling hook variables
FIELD(".entity hook;")
FIELD(".float  on_hook;")
FIELD(".float  fire_held_down;")   // flag - TRUE if player is still holding down the
                                   // fire button after throwing a hook.

// sniper location damage stuff
FIELD(".vector head_shot_vector;")
FIELD(".float leg_damage;")

FIELD(".float cheat_level;")
FIELD(".float speed_level;")

// flash grenade level
FIELD(".float FlashTime;")

// status bar stuff
FIELD(".float StatusRefreshTime;")   // time to next refresh the status bar
                                     // any function that uses a CenterPrintnt
                                     // should reset this value
FIELD(".float StatusBarSize;")       // the size of the bar. 1 is default, 0 is off
FIELD(".float StatusBarRes;")        // the gfx res the player's running in
FIELD(".float StatusBarScreen;")     //CH what screen to jump to when next called.

/*==================================================*/
/* Death Message defines                            */
/*==================================================*/
float deathmsg;             // Global, which is set before every T_Damage, to indicate
                            // the death message that should be used.

// OfN - Prozac C+QuakeC fields
FIELD(".int runes_owned;")
FIELD(".float clversion;")
FIELD(".float cltype;")

} // END namespace Progs
