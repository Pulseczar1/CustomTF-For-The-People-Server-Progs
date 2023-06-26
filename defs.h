#ifndef DEFS_H
#define DEFS_H

namespace Progs {

// PZ: These four files used to precede defs.qc in the progs.src file (unity build).
#include "preopdef.h"
#include "options.h"
#include "messages.h"
#include "skins.h"
#include "ofndefs.h"   // PZ: I added this here. So, it should now be removed from all files.

//
// required prog functions
//
void Main();                   // only for testing
void StartFrame();
void PlayerPreThink();
void PlayerPostThink();
void ClientKill();
void ClientConnect();
void PutClientInServer();      // call after setting the parm1... parms
void ClientDisconnect();
void SetNewParms();            // called when a client first connects to
                               // a server. sets parms so they can be
                               // saved off for restarts
void SetChangeParms();         // call to set parms for self so they can
                               // be saved for a level transition

//
// constants
//

// PZ: defines for readability
#define     PR_BUTTON_FIRE     button0
#define     PR_BUTTON_USE      button1
#define     PR_BUTTON_JUMP     button2

// PZ: defines for readability
#define     PR_GREEN_ARMOR     0.3
#define     PR_YELLOW_ARMOR    0.6
#define     PR_RED_ARMOR       0.8

// edict.flags
#define PR_FL_FLY                  1
#define PR_FL_SWIM                 2
// PZ NOTE: notice that there is no 4 (I don't know why)
#define PR_FL_CLIENT               8   // set for all client edicts
#define PR_FL_INWATER              16  // for enter / leave water splash
#define PR_FL_MONSTER              32
#define PR_FL_GODMODE              64  // player cheat
#define PR_FL_NOTARGET             128 // player cheat
#define PR_FL_ITEM                 256 // extra wide size for bonus items
#define PR_FL_ONGROUND             512 // standing on something
#define PR_FL_PARTIALGROUND        1024 // not all corners are valid
#define PR_FL_WATERJUMP            2048 // player jumping out of water
#define PR_FL_JUMPRELEASED         4096 // for jump debouncing
#define PR_FL_FINALIZED            8192 // OfN - Finalized (SVC_FINALE) player (Warning: Hard coded on server!)
#define PR_FL_FINDABLE_NONSOLID    16384 // OfN - Any entity with this can be "found" with findradius(), even if SOLID_NOT
#define PR_FL_MACHINE              32768 // OfN - Engineer build or other mechanical thing

// edict.movetype values
#define PR_MOVETYPE_NONE           0   // never moves
//#define MOVETYPE_ANGLENOCLIP     1
//#define MOVETYPE_ANGLECLIP       2
#define PR_MOVETYPE_WALK           3   // players only
#define PR_MOVETYPE_STEP           4   // discrete, not real time unless fall  (PZ NOTE: for monsters)
#define PR_MOVETYPE_FLY            5
#define PR_MOVETYPE_TOSS           6   // gravity
#define PR_MOVETYPE_PUSH           7   // no clip to world, push and crush
#define PR_MOVETYPE_NOCLIP         8
#define PR_MOVETYPE_FLYMISSILE     9   // fly with extra size against monsters
#define PR_MOVETYPE_BOUNCE         10
#define PR_MOVETYPE_BOUNCEMISSILE  11  // bounce with extra size

// edict.solid values
#define PR_SOLID_NOT       0   // no interaction with other objects
#define PR_SOLID_TRIGGER   1   // touch on edge, but not blocking
#define PR_SOLID_BBOX      2   // touch on edge, block
#define PR_SOLID_SLIDEBOX  3   // touch on edge, but not an onground
#define PR_SOLID_BSP       4   // bsp clip, touch on edge, block

// range values
#define PR_RANGE_MELEE     0
#define PR_RANGE_NEAR      1
#define PR_RANGE_MID       2
#define PR_RANGE_FAR       3
#define PR_RANGE_VERYFAR   4 //- OfN - Used for sentries

// deadflag values
#define PR_DEAD_NO             0
#define PR_DEAD_DYING          1
#define PR_DEAD_DEAD           2
#define PR_DEAD_RESPAWNABLE    3

// takedamage values
#define PR_DAMAGE_NO           0
#define PR_DAMAGE_YES          1
#define PR_DAMAGE_AIM          2

// items
#define PR_IT_AXE              4096
#define PR_IT_SHOTGUN          1
#define PR_IT_SUPER_SHOTGUN    2
#define PR_IT_NAILGUN          4
#define PR_IT_LIGHT_ASSAULT    8
#define PR_IT_SUPER_NAILGUN    8 // OfN
#define PR_IT_GRENADE_LAUNCHER 16
#define PR_IT_ROCKET_LAUNCHER  32
#define PR_IT_LIGHTNING        64
#define PR_IT_EXTRA_WEAPON     128

#define PR_IT_SHELLS           256
#define PR_IT_NAILS            512
#define PR_IT_ROCKETS          1024
#define PR_IT_CELLS            2048

#define PR_IT_ARMOR1           8192
#define PR_IT_ARMOR2           16384
#define PR_IT_ARMOR3           32768
#define PR_IT_SUPERHEALTH      65536

#define PR_IT_KEY1             131072
#define PR_IT_KEY2             262144

#define PR_IT_INVISIBILITY     524288
#define PR_IT_INVULNERABILITY  1048576
#define PR_IT_SUIT             2097152
#define PR_IT_QUAD             4194304
#define PR_IT_HOOK             8388608 // Last BIT!!!

// point content values
#define PR_CONTENT_EMPTY       -1
#define PR_CONTENT_SOLID       -2
#define PR_CONTENT_WATER       -3
#define PR_CONTENT_SLIME       -4
#define PR_CONTENT_LAVA        -5
#define PR_CONTENT_SKY         -6

#define PR_STATE_TOP           0   // at top      (PZ NOTES)
#define PR_STATE_BOTTOM        1   // at bottom
#define PR_STATE_UP            2   // going up
#define PR_STATE_DOWN          3   // going down

// PZ: I got this off the net. This might be a lot more useful than talking about Quake units or player widths (32 units).
// Wolfenstein 3D, DOOM and QUAKE use the same coordinate/unit system:
// 8 foot (96 inch) height wall == 64 pixel units
// 1.5 inches per pixel unit
// 1.0 pixel unit / 1.5 inch == 0.666666 pixel units per inch
// QuakeEd shows the *eye* height to be ~46 units high
// 46 units * 1.5 inch ratio == 69 inches / 12 inches == 5 foot 9 inch *eye level*
#define PR_ONE_FOOT            8                     // 8 units = 1 foot
//#define ONE_METER

// Metric system. Multiply these by the number you want to convert.
#define PR_INCH_TO_METER       (0.0254)
#define PR_QUAKE_TO_METER      (1.5 * PR_INCH_TO_METER)  // 0.0381 meters per quake pixel unit
#define PR_METER_TO_QUAKE      (1.0 / PR_QUAKE_TO_METER) // 26.24671916 quake pixel units per meter
// PZ: END

#define PR_LARGEST_INTEGER  16777215   // PZ: the largest possible integer with QuakeC floats (assuming we can only use 3 of the 4 bytes)

// PZ NOTE: Bounding boxes are considerably larger than the actual character models (you can easily see this with the sniper dot).
//          The player width of 32 is very close to elbow to elbow measurement, though. But not good of course for chest to back measurement.
// PZ NOTE: You can change an entity's size at any time with setsize(), and it will work fine when testing collision with other entities,
//          but entities will always have their size rounded to one of the following sizes for testing collision with the world:
//          point size, player size (HULL), shambler size (HULL2)
#define PR_VEC_ORIGIN          V({0, 0, 0})
#define PR_VEC_HULL_MIN        V({-16, -16, -24})   // PZ NOTE: the Quake player's bounding box is 32 units wide and 56 units tall
#define PR_VEC_HULL_MAX        V({16, 16, 32})

#define PR_VEC_HULL2_MIN       V({-32, -32, -24})   // PZ NOTE: I think this is used for large monsters
#define PR_VEC_HULL2_MAX       V({32, 32, 64})

// protocol bytes (server to client)   (PZ: only 9 of these already existed here in the QuakeC code; I added the rest from the C code)
#define	PR_SVC_BAD                 0
#define	PR_SVC_NOP                 1
#define	PR_SVC_DISCONNECT          2
#define	PR_SVC_UPDATESTAT          3	// [byte] [byte]
//define	SVC_VERSION            4	// [long] server version
#define	PR_SVC_SETVIEWPORT         5	// [short] entity number    (called "SVC_SETVIEW" in C code)
#define	PR_SVC_SOUND               6	// <see code>
//define	SVC_TIME               7	// [float] server time
#define	PR_SVC_PRINT               8	// [byte] id [string] null terminated string
#define	PR_SVC_STUFFTEXT           9	// [string] stuffed into client's console buffer; the string should be \n terminated
#define	PR_SVC_SETANGLES           10	// [angle3] set the view angle to this absolute value   (called "SVC_SETANGLE" in C code)
#define	PR_SVC_SERVERDATA          11	// [long] protocol ...
#define	PR_SVC_LIGHTSTYLE          12	// [byte] [string]
//define	SVC_UPDATENAME         13	// [byte] [string]
#define	PR_SVC_UPDATEFRAGS         14	// [byte] [short]
//define	SVC_CLIENTDATA         15	// <shortbits + data>
#define	PR_SVC_STOPSOUND           16	// <see code>
//define SVC_UPDATECOLORS          17	// [byte] [byte] [byte]
//define	SVC_PARTICLE           18	// [vec3] <variable>
#define	PR_SVC_DAMAGE              19
#define	PR_SVC_SPAWNSTATIC         20
//	SVC_SPAWNBINARY                21
#define	PR_SVC_SPAWNBASELINE       22
#define	PR_SVC_TEMPENTITY          23	// variable    (called "SVC_TEMP_ENTITY" in C code)
#define	PR_SVC_SETPAUSE            24	// [byte] on / off
//	SVC_SIGNONNUM                  25	// [byte]  used for the signon sequence
#define	PR_SVC_CENTERPRINT         26	// [string] to put in center of the screen
#define	PR_SVC_KILLEDMONSTER       27
#define	PR_SVC_FOUNDSECRET         28
#define	PR_SVC_SPAWNSTATICSOUND    29	// [coord3] [byte] samp [byte] vol [byte] aten
#define	PR_SVC_INTERMISSION        30	// [vec3_t] origin [vec3_t] angle
#define	PR_SVC_FINALE              31	// [string] text
#define	PR_SVC_CDTRACK             32	// [byte] track
#define PR_SVC_SELLSCREEN          33
#define	PR_SVC_SMALLKICK           34	// set client punchangle to 2
#define	PR_SVC_BIGKICK             35	// set client punchangle to 4
#define	PR_SVC_UPDATEPING          36	// [byte] [short]
#define	PR_SVC_UPDATEENTERTIME     37	// [byte] [float]
#define	PR_SVC_UPDATESTATLONG      38	// [byte] [long]
#define	PR_SVC_MUZZLEFLASH         39	// [short] entity
#define	PR_SVC_UPDATEUSERINFO      40	// [byte] slot [long] uid    // [string] userinfo
#define	PR_SVC_DOWNLOAD            41	// [short] size [size bytes]
#define	PR_SVC_PLAYERINFO          42	// variable
#define	PR_SVC_NAILS               43	// [byte] num [48 bits] xyzpy 12 12 12 4 8
#define	PR_SVC_CHOKECOUNT          44	// [byte] packets choked
#define	PR_SVC_MODELLIST           45	// [strings]
#define	PR_SVC_SOUNDLIST           46	// [strings]
#define	PR_SVC_PACKETENTITIES      47	// [...]
#define	PR_SVC_DELTAPACKETENTITIES 48	// [...]
#define PR_SVC_MAXSPEED            49	// maxspeed change, for prediction
#define PR_SVC_ENTGRAVITY          50	// gravity change, for prediction
#define PR_SVC_SETINFO             51	// setinfo on a client
#define PR_SVC_SERVERINFO          52	// serverinfo
#define PR_SVC_UPDATEPL            53	// [byte] [byte]

// protocol bytes (client to server) // PZ: added all of these from the C code
#define	PR_CLC_BAD                 0
#define	PR_CLC_NOP                 1
//define	CLC_DOUBLEMOVE         2
#define	PR_CLC_MOVE                3	// [[usercmd_t]
#define	PR_CLC_STRINGCMD           4	// [string] message
#define	PR_CLC_DELTA               5	// [byte] sequence number, requests delta compression of message
#define PR_CLC_TMOVE               6	// teleport request, spectator only
#define PR_CLC_UPLOAD              7	// teleport request, spectator only

/* PZ: who cares about non-QW, when it comes to a multiplayer game?
#ifdef QUAKE_WORLD
    #define SVC_SMALLKICK   34
    #define SVC_BIGKICK     35
    #define SVC_MUZZLEFLASH 39
#endif */

#define PR_TE_SPIKE            0
#define PR_TE_SUPERSPIKE       1
#define PR_TE_GUNSHOT          2
#define PR_TE_EXPLOSION        3
#define PR_TE_TAREXPLOSION     4        // PZ NOTE: EMP grenade
#define PR_TE_LIGHTNING1       5        // PZ NOTE: thick bolt
#define PR_TE_LIGHTNING2       6        // PZ NOTE: thin bolt
#define PR_TE_WIZSPIKE         7
#define PR_TE_KNIGHTSPIKE      8
#define PR_TE_LIGHTNING3       9        // PZ NOTE: large bolt (used on Chthon terminals)
#define PR_TE_LAVASPLASH       10       // PZ NOTE: gas grenade
#define PR_TE_TELEPORT         11
#define PR_TE_BLOOD            12
#define PR_TE_LIGHTNINGBLOOD   13

// sound channels
// channel 0 never willingly overrides
// other channels (1-7) allways override a playing sound on that channel
#define PR_CHAN_AUTO           0
#define PR_CHAN_WEAPON         1
#define PR_CHAN_VOICE          2
#define PR_CHAN_ITEM           3
#define PR_CHAN_BODY           4
#define PR_CHAN_MUSIC          5 //WK For orff.wav
#define PR_CHAN_MISC           6
#define PR_CHAN_EARTHQUAKE     7
//#define CHAN_MONSTER         7
#define PR_CHAN_NO_PHS_ADD     8    // ie: CHAN_BODY+CHAN_NO_PHS_ADD

#define PR_ATTN_NONE           0
#define PR_ATTN_NORM           1
#define PR_ATTN_IDLE           2
#define PR_ATTN_STATIC         3
//#define ???? 4

// update types
#define PR_UPDATE_GENERAL      0
#define PR_UPDATE_STATIC       1
#define PR_UPDATE_BINARY       2
#define PR_UPDATE_TEMP         3

// entity effects
#define PR_EF_BRIGHTFIELD      1
#define PR_EF_MUZZLEFLASH      2
#define PR_EF_BRIGHTLIGHT      4
#define PR_EF_DIMLIGHT         8
//#define EF_FLAG1             16
//#define EF_FLAG2             32
#define PR_EF_BLUE             64
#define PR_EF_RED              128
#define PR_EF_PURPLE           PR_EF_RED | PR_EF_BLUE

// messages
// Since BROADCAST is never used in QW 1.5, and MULTICAST is used instead,
// just define BROADCAST as MULTICAST for QW 1.5
#define PR_MSG_MULTICAST       4
#ifdef PR_QUAKE_WORLD
    #define PR_MSG_BROADCAST   PR_MSG_MULTICAST
#else
    #define PR_MSG_BROADCAST   0       // unreliable to all
#endif

#define PR_MSG_ONE             1       // reliable to one (msg_entity)
#define PR_MSG_ALL             2       // reliable to all
#define PR_MSG_INIT            3       // write to the init string
#define	PR_MSG_SERVER          5       // PZ: to server (for emulating client to server messages; I don't think I got this working)

// message levels
#define PR_PRINT_LOW           0       // pickup messages
#define PR_PRINT_MEDIUM        1       // death messages
#define PR_PRINT_HIGH          2       // critical messages
#define PR_PRINT_CHAT          3       // also goes to chat console

// multicast sets
#define PR_MULTICAST_ALL       0       // every client
#define PR_MULTICAST_PHS       1       // within hearing
#define PR_MULTICAST_PVS       2       // within sight
#define PR_MULTICAST_ALL_R     3       // every client, reliable
#define PR_MULTICAST_PHS_R     4       // within hearing, reliable
#define PR_MULTICAST_PVS_R     5       // within sight, reliable

#define PR_AS_STRAIGHT         1
#define PR_AS_SLIDING          2
#define PR_AS_MELEE            3
#define PR_AS_MISSILE          4
#define PR_AS_FIREBALL         5

#define PR_MAX_HOVER_FUEL 5   //Boots will recharge up to this maximum

#define PR_CUSTOM_ON_SPAWN 1  //When dead, rebuild
#define PR_CUSTOM_FINISHED 2  //Normal gameplay
#define PR_CUSTOM_BUILDING 4  //Making a class
#define PR_CUSTOM_OVERRIDE 8  //Used internally by menu.qc
#define PR_CUSTOM_SELLING  16 //Person wants to sell his frags for money
#define PR_CUSTOM_EDITING  32 // PZ: When player enters 'custom'/'upgrade', they keep their previous class, so they can simply edit it. Entering 'custom' again will clean the slate.

#define PR_DEFAULT_MONEY   10000 //How much to start with if serverinfo m is not set

//===========================================================================
//
// builtin functions
//

//void makevectors(const vector& ang) #1       // sets v_forward, etc globals
//void setorigin(entity e, const vector& o) #2
//void setmodel(entity e, const string& m) #3       // set movetype and solid first
//void setsize(entity e, const vector& min, const vector& max) #4
// #5 was removed
//void Break() #6       // PZ: renamed to capital 'B', so that it's not confused with the 'break' keyword
//float random() #7       // returns 0 - 1
#ifdef PR_COOP_MODE_ENHANCED
//void sound_C(entity e, float chan, const string& samp, float vol, float atten) #8
#else
//void(entity e, float chan, string samp, float vol, float atten) sound = #8; // removed for Frikbot
#endif
//vector normalize(const vector& v) #9
//void error(const string& e) #10
//void objerror(const string& e) #11
//float vlen(const vector& v) #12
//float vectoyaw(const vector& v) #13
//entity spawn() #14
//void remove(entity e) #15

// OfN - Traceline modes  // PZ: Moved this here next to traceline() signature.
#define PR_TL_ANY_SOLID    0 // will hit any solid, but not triggers
#define PR_TL_BSP_ONLY     1 // will hit BSP only
// Warning: do not use 2, it's used internally by server C code
#define PR_TL_TRIGGERS     3 // Scan for triggers (Triggers must be flagged with FL_FINDABLE_NONSOLID)
#define PR_TL_EVERYTHING   4 // Scan for anything (Any NonSolid/Trigger entities must be flagged with FL_FINDABLE_NONSOLID)

// sets trace_* globals
// `nomonsters` is a filter. Use one of the modes above.
// An entity will also be ignored for testing if forent == test, forent->owner == test, or test->owner == forent.
// a forent of world is ignored
//void traceline(const vector& v1, const vector& v2, float nomonsters, entity forent) #16

//entity checkclient() #17  // returns a client to look for
//entity find(entity start, const string& fld, const string& match) #18
//string precache_sound(const string& s) #19
//string precache_model(const string& s) #20
//void stuffcmd(entity client, const string& s) #21
//entity findradius(const vector& org, float rad) #22

#ifndef PR_QUAKE_WORLD
//void(string s) bprint                 = #23;
//void(entity client, string s) sprint  = #24;
//void nqw_bprint(...) #23
//void nqw_sprint(...) #24

// used for QW compatibility
/*
void(float level, string s) bprint;
void(entity client, float level, string s) sprint;

void(float level, string s1, string s2) bprint2;
void(entity client, float level, string s1, string s2) sprint2;

void(float level, string s1, string s2, string s3) bprint3;
void(entity client, float level, string s1, string s2, string s3) sprint3;

void(float level, string s1, string s2, string s3, string s4) bprint4;
void(entity client, float level, string s1, string s2, string s3, string s4) sprint4;

void(float level, string s1, string s2, string s3, string s4, string s5) bprint5;
void(entity client, float level, string s1, string s2, string s3, string s4, string s5) sprint5;

void(float level, string s1, string s2, string s3, string s4, string s5, string s6) bprint6;
void(entity client, float level, string s1, string s2, string s3, string s4, string s5, string s6) sprint6;

void(float level, string s1, string s2, string s3, string s4, string s5, string s6, string s7) bprint7;
void(entity client, float level, string s1, string s2, string s3, string s4, string s5, string s6, string s7) sprint7;

void(float level, string s1, string s2, string s3, string s4, string s5, string s6, string s7, string s8) bprint8;
*/
#else
//void(float level, string s) bprint    = #23;
//void(entity client, float level, string s) sprint = #24;
//void bprint(...) #23
//void sprint(...) #24

//void bprint2(...) #23
//void sprint2(...) #24

//void bprint3(...) #23
//void sprint3(...) #24

//void bprint4(...) #23
//void sprint4(...) #24

//void bprint5(...) #23
//void sprint5(...) #24

//void bprint6(...) #23
//void sprint6(...) #24

//void bprint7(...) #23
//void sprint7(...) #24

//void bprint8(...) #23
#endif

//void dprint(const string& s) #25
//string ftos(float f) #26
//string vtos(const vector& v) #27
//void coredump() #28      // prints all edicts
//void traceon() #29      // turns statement trace on
//void traceoff() #30
//void eprint(entity e) #31      // prints an entire edict
//float walkmove(float yaw, float dist) #32      // returns #TRUE or #FALSE
// #33 was removed
//float droptofloor() #34      // #TRUE if landed on floor
//void lightstyle(float style, const string& value) #35
//float rint(float v) #36      // round to nearest int
//float floor(float v) #37      // largest integer <= v
//float ceil(float v) #38      // smallest integer >= v
// #39 was removed
//float checkbottom(entity e) #40      // true if self is on ground
//float pointcontents(const vector& v) #41      // returns a CONTENT_*
// #42 was removed
//float fabs(float f) #43
//vector(entity e, float speed) aim     = #44;      // returns the shooting vector    // PZ: removed for Frikbot
//float cvar(const string& s) #45      // return cvar.value
//void localcmd(const string& s) #46      // put string into local que
//entity nextent(entity e) #47      // for looping through all ents
//void particle(const vector& o, const vector& d, float color, float count) #48 // start a particle effect
//void ChangeYaw() #49      // turn towards self.ideal_yaw
                                                    // at self.yaw_speed
// #50 was removed
//vector vectoangles(const vector& v) #51

//
// direct client message generation
//
//void(float to, float f) WriteByte     = #52;      // PZ: removed for Frikbot
//void(float to, float f) WriteChar     = #53;      // PZ: removed for Frikbot
//void(float to, float f) WriteShort    = #54;      // PZ: removed for Frikbot
//void(float to, float f) WriteLong     = #55;      // PZ: removed for Frikbot
//void(float to, float f) WriteCoord    = #56;      // PZ: removed for Frikbot
//void(float to, float f) WriteAngle    = #57;      // PZ: removed for Frikbot
//void(float to, string s) WriteString  = #58;      // PZ: removed for Frikbot
//void(float to, entity s) WriteEntity  = #59;      // PZ: removed for Frikbot

//void movetogoal(float step) #67

//string precache_file(const string& s) #68         // no effect except for -copy
//void makestatic(entity e) #69
//void changelevel(const string& s) #70

//#71 was removed

//void cvar_set(const string& var, const string& val) #72      // sets cvar.value

// PZ NOTE: The standard client has a max width of 40 characters for CenterPrint(), and 1024 total characters.
// With 1024 total characters, that means you can print a box of 40 x 25, with 24 characters on the 26th line,
// IF every single column space is occupied by a character. You get more lines, otherwise (most cases).
// Remember, though, that client settings determine how much of a centerprint will fit on the screen. Put the
// less important stuff at the bottom. I think that's what always gets pruned, when out of room.
//void(entity client, string s) centerprint = #73;  // sprint, but in middle
//void centerprint(...) #73
#ifndef PR_STATUSBAR
//void CenterPrint(entity pl, const string& s1) #73
//void CenterPrint2(entity pl, const string& s1, const string& s2) #73
#endif

//void ambientsound(const vector& pos, const string& samp, float vol, float atten) #74

//string precache_model2(const string& s) #75      // registered version only
//string precache_sound2(const string& s) #76      // registered version only
//string precache_file2(const string& s) #77       // registered version only

//void(entity e) setspawnparms          = #78;     // set parm1... to the // removed for Frikbot
                                                   // values at level start
                                                   // for coop respawn

#ifdef PR_QUAKE_WORLD
//void logfrag(entity killer, entity killee) #79       // add to stats
//string infokey(entity e, const string& key) #80      // get a key value (world = serverinfo)
//float stof(const string& s) #81                      // convert string to float
//void multicast(const vector& where, float set) #82   // sends the temp message to a set of clients, possibly in PVS or PHS
#endif

// OfN - Stuff added to cpqwsv for prozac
//float getuid(entity client) #83
//string strcat(const string& st1, const string& st2) #84
//string padstr(const string& st, float len) #85          // PZ: I added more options to this function. See pr_cmds.h and pr_cmds.cpp.
//string colstr(const string& srcstr, float action) #86

// colstr() possible action values
#define PR_COLSTR_WHITE   0 // converts any red chars to white
#define PR_COLSTR_MIX1    1 // mix red and white chars
#define PR_COLSTR_RED     2 // converts any white chars to red
#define PR_COLSTR_MIX2    3 // mix red and white chars
#define PR_COLSTR_NUMBER  4 // converts any numbers to special number chars
#define PR_COLSTR_NUMBERV 5 // second variant of special number chars (only different on rare charsets)

//float strcasecmp(const string& st1, const string& st2) #87
//float strlen(const string& st) #88
//entity getclient(const string& st) #89

//float mutedtime(entity client) #90
//float validatefile(const string& st) #91
//void putsaytime(entity client) #92

// PZ NOTE: For creating and deleting 'permanent' strings. Other people seem to usually call this strzone() and strunzone().
//string makestr(const string& st) #93
//void delstr(const string& st) #94

//float getwave(float inputnum, float modes, float minnum, float maxnum, float balance, float offset, float shape) #95

// GetWave possible mode flags
#define PR_GWAVE_STANDARD    0
#define PR_GWAVE_USEMINMAX   1
#define PR_GWAVE_USEBALANCE  2 // TODO: Unimplemented yet
#define PR_GWAVE_USEOFFSET   4
#define PR_GWAVE_USESHAPE    8

//void clientsound(entity e, float chan, const string& samp, float vol, float atten) #96

//void touchworld() #97

//entity addBot(const string& userinfo) #99       // PZ
// PZ: makes SV_DropClient() get called for bots, which then calls ClientDisconnect()
//void removeBot(entity bot) #100
//void clearAllEntityFields(entity e) #101 // PZ
//void flushcmd() #102                     // PZ: Used to force buffered localcmd()s to go ahead and execute. DANGER: seems like this breaks things
// PZ: added these so that we could load and save waypoint list files (.way)
// 2001-09-20 QuakeC file access by FrikaC -- START
#define PR_FILE_READ         0
#define PR_FILE_APPEND       1
#define PR_FILE_WRITE        2
//float fopen(const string& filename, float mode) #103 // returns a file handle/number
//void fclose(float fhandle) #104
//string fgets(float fhandle) #105              // reads one line up to finding a \n or \r; returns "" at end of file
//void fputs(float fhandle, const string& s) #106
//string substring(const string& str, float offset, float length) #107 // equivalent to strncpy(result, str + offset, length);
// 2001-09-20 QuakeC file access by FrikaC -- END
// PZ: Give it a string to tokenize, and a string of delimiting characters. Each call, it will return a new token
// that doesn't contain any delimiting characters. It will not modify the given string, and it will remember where
// it left off on the previous call. So only use on one string at a time.
//string strtok(const string& str, const string& delimiters) #108
//float strstr(const string& str, const string& substring) #109   // PZ: just like C's strstr() but returns an index rather than a pointer
//entity spawnServerSide() #110   // PZ: creates an entity that clients don't need to know about (i.e., invisible, non-solid ones) (these start at entity number 512)
//string etos(entity e) #111      // PZ: Returns entity e's number as a string.
//void setupWaypointsForPathfinding(entity waypointList) #112 // PZ: initializes the pathfinder with the waypoint data; called at map start and when saving waypoints in waypoint editor
//float findRoute(entity waypointStart, entity waypointGoal, entity forBot) #113 // PZ
//entity findNextWaypointInRouteForBot(entity fromWaypoint, entity forBot) #114 // PZ
//void stackTrace() #115 // PZ: prints the current QuakeC call stack
//void setinfo(entity cl, const string& key, const string& value) #116 // PZ: for setting client userinfo without using stuffcmd()
//float getClientEntityCount() #117 // PZ: Returns the number of edicts/entities in use below edict index 512.
// PZ: Allows a bot to send an input command to the C code, similar to how a client does it. This allows us to use the real physics code on the bots.
//void sendInputCmdForBot(entity bot, const vector& orientation, float forwardSpeed, float rightSpeed, float verticalSpeed, float buttons) #118
//float getNumberOfTotalClients() #119 // PZ: Get the total count of clients, including downloading/connecting players.
//string alignTextBlock(const string& text, float alignment, float preserveAlignSideSpacing) #120 // PZ: For aligning a block of text for centerprint().
#define PR_ALIGN_LEFT   0
#define PR_ALIGN_CENTER 1
#define PR_ALIGN_RIGHT  2
// PZ: Replaces all instances of `toReplace` in `str` with `replacementStr`. If `replacementStr` is empty, however, it uses `replacementFlt`.
//string replaceStr(const string& str, const string& toReplace, const string& replacementStr, float replacementFlt) #121

//============================================================================

//
// subs.qc
//
void SUB_CalcMove(const vector& tdest, float tspeed, void (*func)());
void SUB_CalcMoveEnt(entity ent, const vector& tdest, float tspeed, void (*func)());
void SUB_CalcAngleMove(const vector& destangle, float tspeed, void (*func)());
void SUB_CalcMoveDone();
void SUB_CalcAngleMoveDone();
void SUB_Null();
void SUB_UseTargets();
void SUB_Remove();

//
//  combat.qc
//
void T_Damage(entity targ, entity inflictor, entity attacker, float damage);

float T_Heal(entity e, float healamount, float ignore); // health function

float CanDamage(entity targ, entity inflictor);

//===========================================================================
// TEAMFORTRESS Defs
//===========================================================================

// TeamFortress State Flags  (.tfstate)
#define PR_TFSTATE_GRENPRIMED              1       // Whether the player has a primed grenade
#define PR_TFSTATE_RELOADING               2       // Whether the player is reloading
#define PR_TFSTATE_CONCUSSIONED            4       // If the person is dizzy and fumbling
#define PR_TFSTATE_RANDOMPC                8       // Whether Playerclass is random, new one each respawn
#define PR_TFSTATE_INFECTED                16      // set when player is infected by the bioweapon
#define PR_TFSTATE_INVINCIBLE              32      // Player has permanent Invincibility (Usually by GoalItem)
#define PR_TFSTATE_INVISIBLE               64      // Player has permanent Invisibility (Usually by GoalItem)
#define PR_TFSTATE_QUAD                    128     // Player has permanent Quad Damage (Usually by GoalItem)
#define PR_TFSTATE_RADSUIT                 256     // Player has permanent Radsuit (Usually by GoalItem)
#define PR_TFSTATE_BURNING                 512     // Is on fire
#define PR_TFSTATE_GRENTHROWING            1024    // is throwing a grenade
#define PR_TFSTATE_AIMING                  2048    // is using the laser sight
#define PR_TFSTATE_ZOOMOFF                 4096    // doesn't want the FOV changed when zooming
#define PR_TFSTATE_RESPAWN_READY           8192    // is waiting for respawn, and has pressed fire
#define PR_TFSTATE_HALLUCINATING           16384   // set when player is hallucinating
#define PR_TFSTATE_TRANQUILISED            32768   // set when player is tranquilised
#define PR_TFSTATE_CANT_MOVE               65536   // set when player is setting a detpack
//WK
#define PR_TFSTATE_INSPIRED                131072  // set when chaplan is in their area
//CH
#define PR_TFSTATE_RL_LASER                262144  // CH set when aiming laser with rl
//SB
#define PR_TFSTATE_C4THROW                 524288  // we're priming the detpack
//OfN
#define PR_TFSTATE_PSIONIZED               1048576 // We are affected by psionic energies
#define PR_TFSTATE_NOTENOUGHMONEY          2097152

// Defines used by TF_T_Damage (see combat.qc)
#define PR_TF_TD_IGNOREARMOUR              1  // Bypasses the armour of the target
#define PR_TF_TD_NOTTEAM                   2  // Doesn't damage a team member (indicates direct fire weapon)
#define PR_TF_TD_NOTSELF                   4  // Doesn't damage self
#define PR_TF_TD_NOKNOCK                   8  // OfN Flag that impides player knockation
#define PR_TF_TD_DONTGIB                   16 // OfN Flag to impide the damage to gib the target
#define PR_TF_TD_NOPAIN                    32 // OfN Makes damage function to not call any pain think's
#define PR_TF_TD_IGNORERESIST              64 // OfN Bypass aura of resist or rune in calculations

#define PR_TF_TD_OTHER                     0  // Ignore armorclass
#define PR_TF_TD_SHOT                      1  // Bullet damage
#define PR_TF_TD_NAIL                      2  // Nail damage
#define PR_TF_TD_EXPLOSION                 4  // Explosion damage
#define PR_TF_TD_ELECTRICITY               8  // Electric damage
#define PR_TF_TD_FIRE                      16  // Fire damage
#define PR_TF_TD_MELEE                     32  // Gizmo - Melee damage, like shamblers, demons, etc.
#define PR_TF_TD_NOSOUND                   256 // Special damage. Makes no sound/painframe, etc

/*==================================================*/
/* Toggleable Game Settings                         */
/*==================================================*/
#define PR_TF_RESPAWNDELAY1                5   // seconds of waiting before player can respawn
#define PR_TF_RESPAWNDELAY2                10  // seconds of waiting before player can respawn
#define PR_TF_RESPAWNDELAY3                20  // seconds of waiting before player can respawn

#define PR_TEAMPLAY_NORMAL                 1
#define PR_TEAMPLAY_HALFDIRECT             2
#define PR_TEAMPLAY_NODIRECT               4
#define PR_TEAMPLAY_HALFEXPLOSIVE          8
#define PR_TEAMPLAY_NOEXPLOSIVE            16
#define PR_TEAMPLAY_LESSPLAYERSHELP        32
#define PR_TEAMPLAY_LESSSCOREHELP          64
#define PR_TEAMPLAY_HALFARMORDIRECT        128
#define PR_TEAMPLAY_NOARMORDIRECT          256
#define PR_TEAMPLAY_HALFARMOREXPLOSIVE     512
#define PR_TEAMPLAY_NOARMOREXPLOSIVE       1024
#define PR_TEAMPLAY_HALFMIRRORDIRECT       2048
#define PR_TEAMPLAY_FULLMIRRORDIRECT       4096
#define PR_TEAMPLAY_HALFMIRROREXPLOSIVE    8192
#define PR_TEAMPLAY_FULLMIRROREXPLOSIVE    16384
#define PR_TEAMPLAY_VAMPIRE                32768
//#define TEAMPLAY_LIGHT_DAMAGE            131072

// FortressMap stuff
//int   civilianteams;         // Bitfield holding Civilian teams
#define PR_TEAM1_CIVILIANS 1
#define PR_TEAM2_CIVILIANS 2
#define PR_TEAM3_CIVILIANS 4
#define PR_TEAM4_CIVILIANS 8

// Defines for the playerclass
#define PR_PC_UNDEFINED    0
#define PR_PC_SCOUT        1
#define PR_PC_SNIPER       2
#define PR_PC_SOLDIER      3
#define PR_PC_DEMOMAN      4
#define PR_PC_MEDIC        5
#define PR_PC_HVYWEAP      6
#define PR_PC_PYRO         7
#define PR_PC_SPY          8
#define PR_PC_ENGINEER     9

// Insert new class definitions here
// PC_RANDOM _MUST_ be the third last class WK - Why?
#define PR_PC_RANDOM       10      // Random playerclass
#define PR_PC_CUSTOM       11      // WK - THE NEW CLASS BEEEATCH
#define PR_PC_CIVILIAN     12      // Civilians are a special class. They cannot
                                   // be chosen by players, only enforced by maps
#define PR_PC_LASTCLASS    13      // Use this as the high-boundary for any loops
                                   // through the playerclass.

/*==================================================*/
/* Impulse Defines                                  */
/*==================================================*/
// PZ: I sorted all the impulses and neatened them. I had to re-number a few to neatly sort them. (They are marked 're-numbered'.)
// Impulses 1 through 12 are also in use, mostly as original Quake impulses.

// Alias check to see whether they already have the aliases
#define PR_TF_ALIAS_CHECK          13

// CTF Support Impulses
#define PR_HOOK_IMP1               22
#define PR_FLAG_INFO               23
#define PR_HOOK_IMP2               39

// Axe
#define PR_AXE_IMP                 40

// Camera Impulse
#ifdef PR_DEMO_STUFF
#define PR_TF_CAM_TARGET           50
#define PR_TF_CAM_ZOOM             51
#define PR_TF_CAM_ANGLE            52
#define PR_TF_CAM_VEC              53
#define PR_TF_CAM_PROJECTILE       54
#define PR_TF_CAM_PROJECTILE_Z     55
#define PR_TF_CAM_REVANGLE         56
#define PR_TF_CAM_OFFSET           57
#define PR_TF_CAM_DROP             58
#define PR_TF_CAM_FADETOBLACK      59
#define PR_TF_CAM_FADEFROMBLACK    60
#define PR_TF_CAM_FADETOWHITE      61
#define PR_TF_CAM_FADEFROMWHITE    62
#endif

// Status Bar Resolution Settings.  Same as CTF to maintain ease of use.
#define PR_TF_STATUSBAR_RES_START  71
#define PR_TF_STATUSBAR_RES_END    81

// Added to PC_??? to get impulse to use if this clashes with your
// own impulses, just change this value, not the PC_??
#define PR_TF_CHANGEPC             100
// The next few impulses are all the class selections
//PC_SCOUT                         101
//PC_SNIPER                        102
//PC_SOLDIER                       103
//PC_DEMOMAN                       104
//PC_MEDIC                         105
//PC_HVYWEAP                       106
//PC_PYRO                          107
//PC_RANDOM                        108
//PC_CIVILIAN                      109     // Cannot be used
//PC_SPY                           110
//PC_ENGINEER                      111     // Changed, this is Custom now

// Help impulses
#define PR_TF_DISPLAYLOCATION      118     // PZ NOTE: VERY USEFUL FOR DEBUGGING. DISPLAYS YOUR ORIGIN.
#define PR_TF_STATUS_QUERY         119
#define PR_TF_CHANGECLASS          120     // PZ: got this working
#define PR_TF_UNCUSTOM             121
#define PR_TF_CHANGETEAM           122     // PZ: added
#define PR_TF_HELP_MAP             131

// Information impulses
#define PR_TF_INVENTORY            135
#define PR_TF_SHOWTF               136
#define PR_TF_SHOWLEGALCLASSES     137

// Team Impulses
#define PR_TF_TEAM_1               140     // Join Team 1
#define PR_TF_TEAM_2               141     // Join Team 2
#define PR_TF_TEAM_3               142     // Join Team 3
#define PR_TF_TEAM_4               143     // Join Team 4
#define PR_TF_TEAM_CLASSES         144     // Impulse to display team classes
#define PR_TF_TEAM_SCORES          145     // Impulse to display team scores
#define PR_TF_TEAM_LIST            146     // Impulse to display the players in each team.
#define PR_TF_TEAM_ENUM            147     // WK WK

// Grenade Impulses
#define PR_TF_GRENADE_1            150     // Prime grenade type 1
#define PR_TF_GRENADE_2            151     // Prime grenade type 2
#define PR_TF_GRENADE_T            152     // Throw primed grenade

// Impulses for new items
#define PR_TF_SCAN                 159     // Scanner Pre-Impulse
#define PR_TF_SCAN_ENEMY           160     // Impulses to toggle scanning of enemies
#define PR_TF_SCAN_FRIENDLY        161     // Impulses to toggle scanning of friendlies
#define PR_TF_SCAN_10              162     // Scan using 10 enery (1 cell)
#define PR_TF_SCAN_30              163     // Scan using 30 energy (2 cells)
#define PR_TF_SCAN_100             164     // Scan using 100 energy (5 cells)
#define PR_TF_DETPACK_5            165     // Detpack set to 5 seconds
#define PR_TF_DETPACK_20           166     // Detpack set to 20 seconds
#define PR_TF_DETPACK_50           167     // Detpack set to 50 seconds
#define PR_TF_DETPACK              168     // Detpack Pre-Impulse
#define PR_TF_DETPACK_STOP         169     // Impulse to stop setting detpack
#define PR_TF_PB_DETONATE          170     // Detonate Pipebombs

// Special skill
#define PR_TF_SPECIAL_SKILL        171

// Ammo Drop impulse
#define PR_TF_DROP_AMMO            172

// Reload impulse
#define PR_TF_RELOAD               173

// auto-zoom toggle
#define PR_TF_AUTOZOOM             174

// drop/pass commands
#define PR_TF_DROPKEY              175

// Select Medikit
#define PR_TF_MEDIKIT              176

// Spy Impulses
#define PR_TF_SPY_SPY              177     // Bring up spy menu
#define PR_TF_SPY_DIE              178     // Feign Death
#define PR_TF_SPY_DIE2             179     // 213  // CH "sfeign" // PZ: re-numbered to neaten impulse list (this is the silent feign)

// Engineer Impulses
#define PR_TF_ENGINEER_BUILD       180     // 179                 // PZ: re-numbered to neaten impulse list
//#define TF_ENGINEER_SANDBAG      180

// Medic!!
#define PR_TF_MEDIC_HELPME         181

// Status bar
#define PR_TF_STATUSBAR_ON         182
#define PR_TF_STATUSBAR_OFF        183

// Discard impulse
#define PR_TF_DISCARD              184

// ID Player impulse
#define PR_TF_ID                   185

// WK Sell off frags
#define PR_TF_SELL                 187

// SB new impulses
//#define IMPULSE_PUNISH           190
#define PR_IMPULSE_THROWDETPACK    191
#define PR_IMPULSE_HOLO            192     // OfN
/*#define IMPULSE_CHECKADMIN       193
#define IMPULSE_UPDATEINFO         194
#define IMPULSE_DEBUG              195*/
#define PR_IMPULSE_TESTRANGE       196

//#define SWITCH_CAMERA            199

#define PR_TF_SKILL                200
#define PR_TF_LAY                  201     // MegaTFer killer

#define PR_TF_DROPITEMS            202     // CH

// taunts
#define PR_TF_TAUNT                203     // WK
#define PR_TF_TAUNT2               204     // WK
#define PR_TF_TAUNT3               205     // WK
#define PR_TF_TAUNT4               206     // WK
#define PR_TF_TAUNT5               207     // WK

//Admin controls
/*#define TF_ADMIN_KICK_CYCLE      208     // 'getplayer' -- Cycles through people
#define TF_ADMIN_KICK_PERSON       209     // 'kick' -- kicks current one
#define TF_ADMIN_BAN_PERSON        210     // 'ban' -- bans current one
#define TF_ADMIN_CEASEFIRE         211*/   // ceasefire

//#define IMPULSE_STUFFCMD         214     // PZ: Since this isn't in use, I'm going to use this to add "votebot".

#define PR_IMPULSE_VOTEBOT         214     // PZ: for "votebot"
#define PR_IMPULSE_PUNISH          215
#define PR_IMPULSE_VOTEMAP         216
#define PR_IMPULSE_VOTEYES         217
#define PR_IMPULSE_VOTENO          218

//CH in admin.qc is a list of impulses that can be used during ceasefire
//To fool proof the cheat impulses
#define PR_I_CHEAT_ONE             219     // 155   // PZ: re-numbered to neaten impulse list
#define PR_I_CHEAT_TWO             220     // 83    // PZ: re-numbered to neaten impulse list
#define PR_I_CHEAT_THREE           221     // 212   // PZ: re-numbered to neaten impulse list

// PZ: For FrikBot. I will likely remove these, but for now...
#define PR_IMPULSE_SPAWN_BOT       230
#define PR_IMPULSE_REMOVE_BOT      231
#define PR_IMPULSE_WAYPOINT_EDITOR 232

/*==================================================*/
/*  Colors                                          */
/*==================================================*/
#define PR_WHITE                   1
#define PR_BROWN                   2
#define PR_BLUE                    3
#define PR_GREEN                   4
#define PR_RED                     5
#define PR_TAN                     6
#define PR_PINK                    7
#define PR_ORANGE                  8
#define PR_PURPLE                  9
#define PR_DARKPURPLE              10
#define PR_GREY                    11
#define PR_DARKGREEN               12
#define PR_YELLOW                  13
#define PR_DARKBLUE                14

/*==================================================*/
/* Defines for the ENGINEER's Building ability      */
/*==================================================*/
// Ammo costs
#define PR_AMMO_COST_SHELLS        3       // Metal needed to make 1 shell
#define PR_AMMO_COST_NAILS         2
#define PR_AMMO_COST_ROCKETS       5
#define PR_AMMO_COST_CELLS         5

// Building types
#define PR_BUILD_DISPENSER         1
#define PR_BUILD_SENTRYGUN         2
#define PR_BUILD_TESLA             3
#define PR_BUILD_SECURITY_CAMERA   4
#define PR_BUILD_TELEPORTER        5
#define PR_BUILD_FIELDGEN          6
#define PR_BUILD_SENSOR            7       // OfN

// Building metal costs
#define PR_BUILD_COST_DISPENSER    100     // Built thru the menu
#define PR_BUILD_COST_SENTRYGUN    130     // Built thru the menu
#define PR_BUILD_COST_TURRET       75      // Built thru the menu
#define PR_BUILD_COST_TESLA        100     // Built thru the menu
#define PR_BUILD_COST_CAMERA       55      //NOTE is used to check if can build stuff
#define PR_BUILD_COST_TELEPORTER   90
#define PR_BUILD_COST_SENSOR       60
#define PR_BUILD_COST_FIELDGEN     120     // was 140

// Building times
#define PR_BUILD_TIME_DISPENSER    2       // 2 seconds to build
#define PR_BUILD_TIME_SENTRYGUN    5       // 5 seconds to build
#define PR_BUILD_TIME_TESLA        6       // 8 seconds to build
#define PR_BUILD_TIME_TELEPORTER   3       // 3 seconds to build
#define PR_BUILD_TIME_FIELDGEN     4       // 7 seconds to build
#define PR_BUILD_TIME_INVADE_PM    0.5

//Camera is thrown, no build time - OfN - sensor neither

// Building health levels
#define PR_BUILD_HEALTH_DISPENSER      150     // Built thru the menu
#ifdef PR_CLASSIC_BUILD_HEALTHS
    #define PR_BUILD_HEALTH_SENTRYGUN  150     // Built thru the menu
    #define PR_BUILD_HEALTH_TESLA      150     // Built thru the menu
#else
    #define PR_BUILD_HEALTH_SENTRYGUN  180     // Built thru the menu
    #define PR_BUILD_HEALTH_TESLA      200     // Built thru the menu
#endif
#define PR_BUILD_HEALTH_CAMERA         400     // They dont do much // was 600
#define PR_BUILD_HEALTH_SENSOR         250     // may not do much, but v. annoying // was 200
#define PR_BUILD_HEALTH_TELEPORTER     300
#define PR_BUILD_HEALTH_FIELDGEN       300     //

// Build upgrade/hacking levels
#ifdef PR_CLASSIC_BUILD_HEALTHS
    //#define SENTRY_HEALTH_L1         150
    #define PR_SENTRY_HEALTH_L2        180
    #define PR_SENTRY_HEALTH_L3        220
#else
    //#define SENTRY_HEALTH_L1         180
    #define PR_SENTRY_HEALTH_L2        210
    #define PR_SENTRY_HEALTH_L3        240
#endif
#define PR_SENTRY_ARMORHACKFACTOR      2

#define PR_SENTRY_AMMO_L1              100
#define PR_SENTRY_AMMO_L2              120
#define PR_SENTRY_AMMO_L3              144
#define PR_SENTRY_AMMOHACKFACTOR       2

#define PR_TESLA_HEALTH_P1             225
#define PR_TESLA_HEALTH_P2             325
#define PR_TESLA_HEALTH_P3             400
#define PR_TESLA_ARMORHACKFACTOR       2

// Building screwups
#define PR_SCREWUP_ONE                 1
#define PR_SCREWUP_TWO                 2
#define PR_SCREWUP_THREE               4
#define PR_SCREWUP_FOUR                8       //(usually takeover xcept for teleporter and dispensern)
//#define SCREWUP_FIVE                 16

//- OfN Building hack-impovements -//
#define PR_IMPROVED_ONE                1       //
#define PR_IMPROVED_TWO                2       // level 1 ice (more time to hack it)
#define PR_IMPROVED_THREE              4       //
#define PR_IMPROVED_FOUR               8       //
#define PR_IMPROVED_FIVE               16      // Level 2 ice (unable to hack it)
#define PR_IMPROVED_SIX                32      // Level 3 ice (kills hacker)
#define PR_IMPROVED_SEVEN              64      // used for 2nd sensor revealer hack (for thieves)
//---------------------------------//

//CH Max distance for teleporter
#define PR_TELEPORTER_RANGE            3072

// Dispenser's maximum carrying capability
#define PR_BUILD_DISPENSER_MAX_SHELLS  400
#define PR_BUILD_DISPENSER_MAX_NAILS   600
#define PR_BUILD_DISPENSER_MAX_ROCKETS 300
#define PR_BUILD_DISPENSER_MAX_CELLS   650
#define PR_BUILD_DISPENSER_MAX_ARMOR   500
#define PR_BUILD_DISPENSER_MAX_MEDIKIT 250     //-125

/*==================================================*/
/* Ammo quantities for dropping                     */
/*==================================================*/
#define PR_DROP_SHELLS             20
#define PR_DROP_NAILS              20
#define PR_DROP_ROCKETS            10
#define PR_DROP_CELLS              10
#define PR_DROP_ARMOR              40

/*==================================================*/
/* Team Defines                                     */
/*==================================================*/
#define PR_TM_MAX_NO   4           // Max number of teams. Simply changing this value isn't enough.
                                   // A new global to hold new team colors is needed, and more flags
                                   // in the spawnpoint spawnflags may need to be used.
                                   // Basically, don't change this unless you know what you're doing :)

/*===========================================================*/
/* New Weapon Defines (.weapons_carried and .current_weapon) */
/*===========================================================*/

// WK -- Removed "weapons" to free up space for shootable things
// We can remove some more by giving the weapons a weapon mode (like the rifle)
#define PR_WEAP_HOOK               1
#define PR_WEAP_MEDIKIT            4
#define PR_WEAP_SPANNER            8
#define PR_WEAP_AXE                16
#define PR_WEAP_SNIPER_RIFLE       32
#define PR_WEAP_AUTO_RIFLE         64
#define PR_WEAP_SHOTGUN            128
#define PR_WEAP_SUPER_SHOTGUN      256
#define PR_WEAP_NAILGUN            512
#define PR_WEAP_LIGHT_ASSAULT      1024
#define PR_WEAP_GRENADE_LAUNCHER   2048
#define PR_WEAP_FLAMETHROWER       4096
#define PR_WEAP_ROCKET_LAUNCHER    8192
#define PR_WEAP_INCENDIARY         16384
#define PR_WEAP_ASSAULT_CANNON     32768
#define PR_WEAP_LIGHTNING          65536
#define PR_WEAP_DAEDALUS           131072
//#define WEAP_DETPACK             131072 DONT MAKE ANY AMMOTYPES USE THIS NUMBER!
#define PR_WEAP_TRANQ              262144
#define PR_WEAP_RAILGUN            524288
#define PR_WEAP_MAUSER             1048576

#define PR_WEAP_AIRF               2097152  //- OfN - airfist
#define PR_WEAP_LASERCANNON        4194304  //- OfN - LASER CANNON
#define PR_WEAP_SNG                8388608  // Last BIT!!!
#define PR_WEAP_ZEROGRAVITY        16777216 // Last BIT, Gizmo - add another weapons flag var  (PZ NOTE: THIS IS BIT 25! So, you can actually use the last byte of floats for flags??)
//MAX 24 BITS OF PRECISION IN FLOAT!

/*====================================================*/
/* New Items WK -- Things to buy in custom!           */
/* Treated as weapons, but be sure to check the right */
/* bitfield -- .weapons_carried vs .tf_items!         */
/*====================================================*/

//WK these are carried in self.tf_items
#define PR_NIT_SCANNER                 1
#define PR_NIT_TURRET                  2 //Turret
#define PR_NIT_KEVLAR                  4 //Special Armors
#define PR_NIT_GEL                     8
#define PR_NIT_BLAST                   16
#define PR_NIT_CERAMIC                 32
#define PR_NIT_ASBESTOS                64
#define PR_NIT_AMMO_BACKPACK           128 //Increases ammo capacity
#define PR_NIT_AMMO_BANDOLIER          256 //Increases starting ammo
#define PR_NIT_SCUBA                   512 //The Scuba Commando!
//WK this next one is only used on teslas, not people, hence the conflict
#define PR_NIT_TESLA_CLOAKING          512 //CH Tesla got a cloaking device
#define PR_NIT_AUTOID                  1024
#define PR_NIT_AUTOMEDIC               2048
#define PR_NIT_RESPAWN_GUARD           4096
#define PR_NIT_AUTOSCANNER             8192
#define PR_NIT_HOVER_BOOTS             16384  //Like a jet pack
#define PR_NIT_TESLA                   32768  //The *real* bug zapper!
#define PR_NIT_SECURITY_CAMERA         65536  //CH Security Camera
#define PR_NIT_TELEPORTER              131072 //CH Teleporter
#define PR_NIT_TESLA_UPGRADE           262144 //Get more tesla upgrades
#define PR_NIT_HOVER_BOOTS_UPGRADE     524288 //CH you get 2x fuel
#define PR_NIT_RL_LASER_SIGHT          1048576 //CH
#define PR_NIT_CLUSTER_ROCKETS         2097152 //CH
#define PR_NIT_DOUBLETESLA             4194304 // OfN - Player gets 2 teslas
//1 more allowed, before we run out of bits in a .float

//WK On to the third 24-wide bitfield for holding purchases  (.cutf_items)
#define PR_CUTF_DETPACK                1
#define PR_CUTF_KNIFE                  2  //More like a powerup
#define PR_CUTF_SPY_KIT                4  //Allows disguise
#define PR_CUTF_ASPECT_OF_HWGUY        8  //Person doesn't bounce      // PZ: This was just called "CUTF_HWGUY".
#define PR_CUTF_SENTRYGUN              16 //Allows building a gun
#define PR_CUTF_JAMMER                 32 // SB scanner jammer
#define PR_CUTF_FULLARMOUR             64 // SB full armour when spawning
#define PR_CUTF_CYBERAUG               128 // SB cybernetic augmentation
#define PR_CUTF_SENSOR                 256 // SB motion sensor
//#define CUTF_INTERFACE               512 // SB tech interface - for hax0ring SGs
#define PR_CUTF_TOSSABLEDET            512 // C4 tossable detpack
#define PR_CUTF_DISPENSER              1024 // the new buyable dispenc0r
#define PR_CUTF_STEALTH                2048 // balance proficiency - done
#define PR_CUTF_HIGHJUMP               4096 // highjump proficiency - done
#define PR_CUTF_GYMNAST                8192 // Gymnast proficiency
#define PR_CUTF_DEMONLORE              16384 // demon lore proficiency
#define PR_CUTF_EXPBODY                32768 // Exploding body
#define PR_CUTF_CLOSECOMBAT            65536 // close combat guy
//----------------------------------------------------------------------------//
#define PR_CUTF_HOLO                   131072 // OfN - Holograph
#define PR_CUTF_CLIPEXTEND             262144 // OfN - Clip extender
#define PR_CUTF_FIELDGEN               524288 // OfN - field generators
#define PR_CUTF_AIRFISTUPGRADE         1048576
#define PR_CUTF_OTR                    2097152
#define PR_CUTF_DOUBLESENTRY           4194304 // OfN - Player gets 2 sentries

// Gizmo - fourth bitfield; for the more optional equipment page, and anything else that remotely fits into that category
//.int cutf_moreitems;                     // cutfMoreItems
#define PR_CUTF_MI_FASTERROCKETS       1   // Gizmo - avirox's faster rocket mod
#define PR_CUTF_MI_FRIENDORFOE         4   // Gizmo - changes the crosshair to blue or red when looking at enemy or friend

//Time to be invincible from the respawn guard
#ifdef PR_COOP_MODE_ENHANCED
#define PR_COOP_RESPAWN_GUARD_TIME     2
#endif
#define PR_RESPAWN_GUARD_TIME          5

//JOBS AVAILABLE
#define PR_JOB_THIEF                   1   //WK CAUTION: IF YOU EVER CHANGE THIS, CPQWSV THIEF CODE WILL BREAK
#define PR_JOB_RUNNER                  2
#define PR_JOB_WARLOCK                 4
#define PR_JOB_CHAPLAN                 8
#define PR_JOB_BERSERKER               16
#define PR_JOB_GUERILLA                32
#define PR_JOB_JUDOKA                  64
//- OfN - New jobs -
#define PR_JOB_ARMY                    128
#define PR_JOB_HACKER                  256
#define PR_JOB_MARTYR                  512
#define PR_JOB_CRUSADER                1024

//This is the sum of all jobs...
#define PR_JOB_ALL                     2047 //Not a job, used to simplify bit operators

//STATES WE ARE IN FOR JOB
//Every time we add one, make sure it gets reset in tfort.qc!
#define PR_JOB_ACTIVE                  2048  //Job is 'on' like currently hiding //WK CAUTION: IF YOU EVER CHANGE THIS, CPQWSV THIEF CODE WILL BREAK
#define PR_JOB_FULL_HIDE               4096  //Job is in phase two of "hide"
#define PR_JOB_TIRED                   8192  //Recovering from a sprint
#define PR_JOB_BLOODY_KNIFE            16384 //We've gone OJ on someone
//#define JOB_DEMON_OUT                32768 //We have a demon in the world  //OfN obsolete
//#define JOB_MARTYR_ENEMY             ^double //We have been shot, and enemy remembered
#define PR_JOB_EXTRA1                  65536 // Warlock, guerilla and army extra
#define PR_JOB_EXTRA2                  131072// 2nd extra

/*==================================================*/
/* New Weapon Related Defines                       */
/*==================================================*/
// shots per reload
#define PR_RE_SHOTGUN                      8
#define PR_RE_SUPER_SHOTGUN                16 // 8 shots
#define PR_RE_GRENADE_LAUNCHER             6
#define PR_RE_ROCKET_LAUNCHER              4
#define PR_RE_LIGHT_ASSAULT                30 // WK 1/7/7 36 // was 40
#define PR_RE_LASER_CANNON                 6

// reload times
#define PR_RE_SHOTGUN_TIME                 2
#define PR_RE_SUPER_SHOTGUN_TIME           3
#define PR_RE_GRENADE_LAUNCHER_TIME        4
#define PR_RE_ROCKET_LAUNCHER_TIME         5
#define PR_RE_LIGHT_ASSAULT_TIME           3.2 //WK 1/7/7 3.3 // was 3.5
#define PR_RE_LASER_CANNON_TIME            1.5

//CLIP EXTENDER reload values
//laser cannon
#define PR_RE_LASER_CANNON_EX              10 // Original 6
#define PR_RE_LASER_CANNON_TIME_EX         1.5
//light assault
#define PR_RE_LIGHT_ASSAULT_EX             36 //WK 1/7/7 48 // Original 40 - 30
#define PR_RE_LIGHT_ASSAULT_TIME_EX        3.4 //WK 1/7/7 3.3 Original 3.5 - 3.2
//shotgun
#define PR_RE_SHOTGUN_EX                   12 // Original 8
#define PR_RE_SHOTGUN_TIME_EX              2
//super-shotgun
#define PR_RE_SUPER_SHOTGUN_EX             24 // Original 16
#define PR_RE_SUPER_SHOTGUN_TIME_EX        3
//rocket launcher
#define PR_RE_ROCKET_LAUNCHER_EX           5 // Original 4
#define PR_RE_ROCKET_LAUNCHER_TIME_EX      5
//grenade launcher
#define PR_RE_GRENADE_LAUNCHER_EX          8 // Original 6
#define PR_RE_GRENADE_LAUNCHER_TIME_EX     4

#ifdef PR_CLASSIC_SNIPER_RIFLE
    #define PR_WEAP_SNIPER_RIFLE_MAX_MOVE  50      // Maximum velocity you can move and fire the Sniper Rifle
    #define PR_WEAP_SNIPER_RIFLE_MAX_TIME  1.8
    #define PR_WEAP_SNIPER_RIFLE_MIN_DAMG  50
    #define PR_WEAP_SNIPER_RIFLE_MAX_DAMG  400
#else
    #define PR_WEAP_SNIPER_RIFLE_MAX_MOVE  20      // Maximum velocity you can move and fire the Sniper Rifle
    #define PR_WEAP_SNIPER_RIFLE_MAX_TIME  11.5
    #define PR_WEAP_SNIPER_RIFLE_MIN_DAMG  60
    #define PR_WEAP_SNIPER_RIFLE_MAX_DAMG  600     // 380
#endif

// Medikit
#define PR_WEAP_MEDIKIT_HEAL               200  // Amount medikit heals per hit
#define PR_WEAP_MEDIKIT_OVERHEAL           50 // Amount of superhealth over max_health the medikit will dispense

// Spanner
#define PR_WEAP_SPANNER_REPAIR             50

// Detpack
#define PR_WEAP_DETPACK_DISARMTIME         1       //Time it takes to disarm a Detpack
#define PR_WEAP_DETPACK_SETTIME            4       // Time it takes to set a Detpack
#define PR_WEAP_DETPACK_SIZE               800
#define PR_WEAP_DETPACK_BITS_NO            12      // Bits that detpack explodes into

// Tranquiliser Gun
#define PR_TRANQ_TIME          5

// Grenades
#define PR_GR_PRIMETIME        3

// Grenade types
#define PR_GR_TYPE_NONE        0
#define PR_GR_TYPE_NORMAL      1
#define PR_GR_TYPE_CONCUSSION  2
#define PR_GR_TYPE_NAIL        3
#define PR_GR_TYPE_MIRV        4
#define PR_GR_TYPE_NAPALM      5
#define PR_GR_TYPE_FLAME       5 //WK Our special napalm grenade ;)
#define PR_GR_TYPE_FLARE       6
#define PR_GR_TYPE_GAS         7
#define PR_GR_TYPE_EMP         8
#ifdef PR_OLD_FLASH
#define PR_GR_TYPE_FLASH       9
#else
#define PR_GR_TYPE_PSIONIC     9
#endif
#define PR_GR_TYPE_FRAG        10
#define PR_GR_TYPE_KRAC        11
#define PR_GR_TYPE_ANTIGRAV    12
#define PR_GR_TYPE_BIO         13
#define PR_GR_TYPE_CALTROP     14

// Defines for WeaponMode
#define PR_GL_NORMAL           0
#define PR_GL_PIPEBOMB         1

// Defines for Concussion Grenade
#define PR_GR_CONCUSS_TIME     2 //WK 5, so 10s now down from 25s
//WK Note: Concs have 100 health, this much comes off each tick
#define PR_GR_CONCUSS_DEC      20

// Defines for the Gas Grenade
#define PR_GR_HALLU_TIME       0.1
#define PR_GR_HALLU_DEC        2.5

#define PR_NIT_SILVER_DOOR_OPENED      PR_IT_KEY1    // 131072
#define PR_NIT_GOLD_DOOR_OPENED        PR_IT_KEY2    // 262144

/*==================================================*/
/* New Item Flags                                   */
/*==================================================*/
#define PR_NIT_SCANNER_ENEMY           1   // Detect enemies
#define PR_NIT_SCANNER_FRIENDLY        2   // Detect friendlies (team members)
#define PR_NIT_SCANNER_MOVEMENT        4   // Motion detection. Only report moving entities.

/*==================================================*/
/* New Item Related Defines                         */
/*==================================================*/
#define PR_NIT_SCANNER_POWER           100 // The amount of power spent on a scan with the scanner
                                        // is multiplied by this to get the scanrange.
#define PR_NIT_SCANNER_MAXCELL         50  // The maximum number of cells than can be used in one scan
#define PR_NIT_SCANNER_MIN_MOVEMENT    50  // The minimum velocity an entity must have to be detected
                                        // by scanners that only detect movement

// Armor Classes : Bitfields. Use the ".armorclass" of armor for the Armor Type.
#define PR_AT_SAVESHOT                     1   // Kevlar   : Reduces bullet damage by 15%
#define PR_AT_SAVEMELEE                    2   // Gel      : Reduces melee attacks by 15%      // PZ NOTE: now called "Impact"
#define PR_AT_SAVEEXPLOSION                4   // Blast    : Reduces explosion damage 15%
#define PR_AT_SAVEELECTRICITY              8   // Shock    : Reduces electricity damage by 15% // ceramic
#define PR_AT_SAVEFIRE                     16  // Asbestos : Reduces fire damage by 15%

/*==========================================================================*/
/* TEAMFORTRESS CLASS DETAILS                                               */
/*==========================================================================*/

// Class Details for SCOUT
//#define PC_SCOUT_SKIN                    4       // Skin for this class when Classkin is on.
#define PR_PC_SCOUT_MAXHEALTH              75      // Maximum Health Level
#define PR_PC_SCOUT_MAXSPEED               420     // Maximum movement speed
//#define PC_SCOUT_MAXSTRAFESPEED          420     // Maximum strafing movement speed
#define PR_PC_SCOUT_MAXARMOR               50      // Maximum Armor Level, of any armor class
#define PR_PC_SCOUT_INITARMOR              25      // Armor level when respawned
#define PR_PC_SCOUT_MAXARMORTYPE           0.3     // Maximum level of Armor absorption
#define PR_PC_SCOUT_INITARMORTYPE          0.3     // Absorption Level of armor when respawned
//#define PC_SCOUT_ARMORCLASSES            3       // #AT_SAVESHOT | #AT_SAVEMELEE         <-Armor Classes allowed for this class
#define PR_PC_SCOUT_INITARMORCLASS         0       // Armorclass worn when respawned
#define PR_PC_SCOUT_WEAPONS                PR_WEAP_AXE | PR_WEAP_SHOTGUN | PR_WEAP_NAILGUN | PR_WEAP_AIRF
#define PR_PC_SCOUT_MAXAMMO_SHOT           50      // Maximum amount of shot ammo this class can carry
#define PR_PC_SCOUT_MAXAMMO_NAIL           100     // Maximum amount of nail ammo this class can carry
#define PR_PC_SCOUT_MAXAMMO_CELL           100     // Maximum amount of cell ammo this class can carry
#define PR_PC_SCOUT_MAXAMMO_ROCKET         25      // Maximum amount of rocket ammo this class can carry
#define PR_PC_SCOUT_INITAMMO_SHOT          25      // Amount of shot ammo this class has when respawned
#define PR_PC_SCOUT_INITAMMO_NAIL          100     // Amount of nail ammo this class has when respawned
#define PR_PC_SCOUT_INITAMMO_CELL          50      // Amount of cell ammo this class has when respawned
#define PR_PC_SCOUT_INITAMMO_ROCKET        0       // Amount of rocket ammo this class has when respawned
#define PR_PC_SCOUT_GRENADE_TYPE_1         PR_GR_TYPE_CALTROP        //    <- 1st Type of Grenade this class has
#define PR_PC_SCOUT_GRENADE_TYPE_2         PR_GR_TYPE_CONCUSSION     //  <- 2nd Type of Grenade this class has
#define PR_PC_SCOUT_GRENADE_INIT_1         4       // Number of grenades of Type 1 this class has when respawned
#define PR_PC_SCOUT_GRENADE_INIT_2         4       // Number of grenades of Type 2 this class has when respawned    // PZ: Added respawn guard, below, for stock class editing.
#define PR_PC_SCOUT_TF_ITEMS               PR_NIT_SECURITY_CAMERA | PR_NIT_SCANNER | PR_NIT_AUTOSCANNER | PR_NIT_HOVER_BOOTS | PR_NIT_HOVER_BOOTS_UPGRADE | PR_NIT_RESPAWN_GUARD // OfN Now with hover boots
#define PR_PC_SCOUT_CUTF_ITEMS             PR_CUTF_HOLO | PR_CUTF_AIRFISTUPGRADE // | #CUTF_CYBERAUG // OfN Scouts with holo now and cyberaug!
#define PR_PC_SCOUT_JOB                    PR_JOB_THIEF

//#define PC_SCOUT_MOTION_MIN_I            0.5     // < Short range
//#define PC_SCOUT_MOTION_MIN_MOVE         50      // Minimum vlen of player velocity to be picked up by motion detector

// Class Details for SNIPER
//#define PC_SNIPER_SKIN                   5
#define PR_PC_SNIPER_MAXHEALTH             90
#define PR_PC_SNIPER_MAXSPEED              320
//#define PC_SNIPER_MAXSTRAFESPEED         320
#define PR_PC_SNIPER_MAXARMOR              50
#define PR_PC_SNIPER_INITARMOR             50
#define PR_PC_SNIPER_MAXARMORTYPE          0.3
#define PR_PC_SNIPER_INITARMORTYPE         0.3
//#define PC_SNIPER_ARMORCLASSES           3       // #AT_SAVESHOT | #AT_SAVEMELEE
#define PR_PC_SNIPER_INITARMORCLASS        PR_AT_SAVESHOT   // PZ: for some reason, someone wrote "1" here instead of the define alias
#define PR_PC_SNIPER_WEAPONS               PR_WEAP_SNIPER_RIFLE | PR_WEAP_AUTO_RIFLE | PR_WEAP_AXE | PR_WEAP_NAILGUN
#define PR_PC_SNIPER_MAXAMMO_SHOT          75
#define PR_PC_SNIPER_MAXAMMO_NAIL          100
#define PR_PC_SNIPER_MAXAMMO_CELL          150
#define PR_PC_SNIPER_MAXAMMO_ROCKET        25
#define PR_PC_SNIPER_INITAMMO_SHOT         60
#define PR_PC_SNIPER_INITAMMO_NAIL         50
#define PR_PC_SNIPER_INITAMMO_CELL         0
#define PR_PC_SNIPER_INITAMMO_ROCKET       0
#define PR_PC_SNIPER_GRENADE_TYPE_1        PR_GR_TYPE_NORMAL // Gizmo - changed this to normal to be more like Regular TF
#define PR_PC_SNIPER_GRENADE_TYPE_2        PR_GR_TYPE_FLARE //- OfN it was #GR_TYPE_FLASH
#define PR_PC_SNIPER_GRENADE_INIT_1        4
#define PR_PC_SNIPER_GRENADE_INIT_2        6 // OfN it was 4                      // PZ: Added respawn guard, below, for stock class editing.
#define PR_PC_SNIPER_TF_ITEMS              PR_NIT_SECURITY_CAMERA | PR_NIT_KEVLAR | PR_NIT_RESPAWN_GUARD // | #NIT_TESLA  // PZ: Added kevlar, because sniper has "saveshot" armor. This makes it show up when editing stock class.
#define PR_PC_SNIPER_CUTF_ITEMS            0 // #CUTF_OTR
#define PR_PC_SNIPER_JOB                   PR_JOB_JUDOKA

// Class Details for SOLDIER
//#define PC_SOLDIER_SKIN                  6
#define PR_PC_SOLDIER_MAXHEALTH            100
#define PR_PC_SOLDIER_MAXSPEED             250
//#define PC_SOLDIER_MAXSTRAFESPEED        250
#define PR_PC_SOLDIER_MAXARMOR             200
#define PR_PC_SOLDIER_INITARMOR            100
#define PR_PC_SOLDIER_MAXARMORTYPE         0.8
#define PR_PC_SOLDIER_INITARMORTYPE        0.8
//#define PC_SOLDIER_ARMORCLASSES          31      // ALL
#define PR_PC_SOLDIER_INITARMORCLASS       0
#define PR_PC_SOLDIER_WEAPONS              PR_WEAP_AXE | PR_WEAP_SHOTGUN | PR_WEAP_SUPER_SHOTGUN | PR_WEAP_ROCKET_LAUNCHER
#define PR_PC_SOLDIER_MAXAMMO_SHOT         100
#define PR_PC_SOLDIER_MAXAMMO_NAIL         100
#define PR_PC_SOLDIER_MAXAMMO_CELL         50
#define PR_PC_SOLDIER_MAXAMMO_ROCKET       40
#define PR_PC_SOLDIER_INITAMMO_SHOT        50
#define PR_PC_SOLDIER_INITAMMO_NAIL        0
#define PR_PC_SOLDIER_INITAMMO_CELL        0
#define PR_PC_SOLDIER_INITAMMO_ROCKET      20
#define PR_PC_SOLDIER_GRENADE_TYPE_1       PR_GR_TYPE_NORMAL
#define PR_PC_SOLDIER_GRENADE_TYPE_2       PR_GR_TYPE_NAIL
#define PR_PC_SOLDIER_GRENADE_INIT_1       4
#define PR_PC_SOLDIER_GRENADE_INIT_2       2
#define PR_PC_SOLDIER_TF_ITEMS             PR_NIT_RESPAWN_GUARD   //#NIT_RL_LASER_SIGHT //WK no Cluster - OfN no hover boots  // PZ: Added respawn guard for stock class editing.
#define PR_PC_SOLDIER_CUTF_ITEMS           0   // PZ: Added this to make it easier to add CuTF items to the stock soldier in the future.
#define PR_PC_SOLDIER_JOB                  PR_JOB_ARMY //#JOB_RUNNER

// Class Details for DEMOLITION MAN
//#define PC_DEMOMAN_SKIN                  1
#define PR_PC_DEMOMAN_MAXHEALTH            90
#define PR_PC_DEMOMAN_MAXSPEED             290
//#define PC_DEMOMAN_MAXSTRAFESPEED        290
#define PR_PC_DEMOMAN_MAXARMOR             120 //WK 2/8/7 100
#define PR_PC_DEMOMAN_INITARMOR            50
#define PR_PC_DEMOMAN_MAXARMORTYPE         0.6
#define PR_PC_DEMOMAN_INITARMORTYPE        0.6
//#define PC_DEMOMAN_ARMORCLASSES          31      // ALL
#ifdef PR_NO_STOCK_DEMOMAN_BLASTARMOR
    #define PR_PC_DEMOMAN_INITARMORCLASS   0       // AT_SAVEEXPLOSION
#else
    #define PR_PC_DEMOMAN_INITARMORCLASS   PR_AT_SAVEEXPLOSION       // AT_SAVEEXPLOSION
#endif
#define PR_PC_DEMOMAN_WEAPONS              PR_WEAP_AXE | PR_WEAP_SHOTGUN | PR_WEAP_GRENADE_LAUNCHER
#define PR_PC_DEMOMAN_MAXAMMO_SHOT         75
#define PR_PC_DEMOMAN_MAXAMMO_NAIL         50
#define PR_PC_DEMOMAN_MAXAMMO_CELL         50
#define PR_PC_DEMOMAN_MAXAMMO_ROCKET       50
#define PR_PC_DEMOMAN_MAXAMMO_DETPACK      1
#define PR_PC_DEMOMAN_INITAMMO_SHOT        30
#define PR_PC_DEMOMAN_INITAMMO_NAIL        0
#define PR_PC_DEMOMAN_INITAMMO_CELL        0
#define PR_PC_DEMOMAN_INITAMMO_ROCKET      20
#define PR_PC_DEMOMAN_INITAMMO_DETPACK     1
#define PR_PC_DEMOMAN_GRENADE_TYPE_1       PR_GR_TYPE_NORMAL
#define PR_PC_DEMOMAN_GRENADE_TYPE_2       PR_GR_TYPE_MIRV
#define PR_PC_DEMOMAN_GRENADE_INIT_1       4
#define PR_PC_DEMOMAN_GRENADE_INIT_2       3
#ifdef PR_NO_STOCK_DEMOMAN_BLASTARMOR      // PZ: Added condition so that I could set #NIT_BLAST, so that it would show up when editing demoman stock class.
    #define PR_PC_DEMOMAN_TF_ITEMS         PR_NIT_RESPAWN_GUARD                  // PZ: Added respawn guard for stock class editing.
#else
    #define PR_PC_DEMOMAN_TF_ITEMS         PR_NIT_RESPAWN_GUARD | PR_NIT_BLAST   // PZ: Added respawn guard for stock class editing.
#endif
#define PR_PC_DEMOMAN_JOB                  PR_JOB_GUERILLA
#ifdef PR_NO_STOCK_DEMOMAN_HIGHJUMP
    #define PR_PC_DEMOMAN_CUTF_ITEMS       PR_CUTF_DETPACK | PR_CUTF_TOSSABLEDET
#else
    #define PR_PC_DEMOMAN_CUTF_ITEMS       PR_CUTF_DETPACK | PR_CUTF_TOSSABLEDET | PR_CUTF_HIGHJUMP //WK 2/8/7 Added highjump
#endif

// Class Details for COMBAT MEDIC
//#define PC_MEDIC_SKIN                    3
#define PR_PC_MEDIC_MAXHEALTH              90
#define PR_PC_MEDIC_MAXSPEED               340
//#define PC_MEDIC_MAXSTRAFESPEED          340
#define PR_PC_MEDIC_MAXARMOR               100
#define PR_PC_MEDIC_INITARMOR              50
#define PR_PC_MEDIC_MAXARMORTYPE           0.6
#define PR_PC_MEDIC_INITARMORTYPE          0.3
//#define PC_MEDIC_ARMORCLASSES            11      // ALL except EXPLOSION
#define PR_PC_MEDIC_INITARMORCLASS         0
#define PR_PC_MEDIC_WEAPONS                PR_WEAP_MEDIKIT | PR_WEAP_SHOTGUN | PR_WEAP_SUPER_SHOTGUN | PR_WEAP_LIGHT_ASSAULT
#define PR_PC_MEDIC_MAXAMMO_SHOT           75
#define PR_PC_MEDIC_MAXAMMO_NAIL           150
#define PR_PC_MEDIC_MAXAMMO_CELL           50
#define PR_PC_MEDIC_MAXAMMO_ROCKET         25
#define PR_PC_MEDIC_MAXAMMO_MEDIKIT        100
#define PR_PC_MEDIC_INITAMMO_SHOT          50
#define PR_PC_MEDIC_INITAMMO_NAIL          50
#define PR_PC_MEDIC_INITAMMO_CELL          0
#define PR_PC_MEDIC_INITAMMO_ROCKET        0
#define PR_PC_MEDIC_INITAMMO_MEDIKIT       50
#define PR_PC_MEDIC_GRENADE_TYPE_1         PR_GR_TYPE_NORMAL
#define PR_PC_MEDIC_GRENADE_TYPE_2         PR_GR_TYPE_BIO  //#GR_TYPE_CONCUSSION
#define PR_PC_MEDIC_GRENADE_INIT_1         3
#define PR_PC_MEDIC_GRENADE_INIT_2         2
#define PR_PC_MEDIC_TF_ITEMS               PR_NIT_RESPAWN_GUARD   //#NIT_AUTOMEDIC // SB no more automedic - OfN no way! :)  // PZ: Added respawn guard for stock class editing.
#define PR_PC_MEDIC_CUTF_ITEMS             0   // PZ: Added this to make it easier to add CuTF items to the stock medic in the future.
#define PR_PC_MEDIC_REGEN_TIME             3   // Number of seconds between each regen.
#define PR_PC_MEDIC_REGEN_AMOUNT           6   // Amount of health regenerated each regen.
#define PR_PC_MEDIC_JOB                    PR_JOB_CRUSADER

// Class Details for HVYWEAP
//#define PC_HVYWEAP_SKIN                  2
#define PR_PC_HVYWEAP_MAXHEALTH            100
#define PR_PC_HVYWEAP_MAXSPEED             250
//#define PC_HVYWEAP_MAXSTRAFESPEED        250
#define PR_PC_HVYWEAP_MAXARMOR             300
#define PR_PC_HVYWEAP_INITARMOR            300
#define PR_PC_HVYWEAP_MAXARMORTYPE         0.8
#define PR_PC_HVYWEAP_INITARMORTYPE        0.8
//#define PC_HVYWEAP_ARMORCLASSES          31          // ALL
#define PR_PC_HVYWEAP_INITARMORCLASS       0
#define PR_PC_HVYWEAP_WEAPONS              PR_WEAP_ASSAULT_CANNON | PR_WEAP_AXE | PR_WEAP_SHOTGUN | PR_WEAP_SUPER_SHOTGUN
#define PR_PC_HVYWEAP_MAXAMMO_SHOT         200
#define PR_PC_HVYWEAP_MAXAMMO_NAIL         200
#define PR_PC_HVYWEAP_MAXAMMO_CELL         50
#define PR_PC_HVYWEAP_MAXAMMO_ROCKET       25
#define PR_PC_HVYWEAP_INITAMMO_SHOT        200
#define PR_PC_HVYWEAP_INITAMMO_NAIL        0
#define PR_PC_HVYWEAP_INITAMMO_CELL        24
#define PR_PC_HVYWEAP_INITAMMO_ROCKET      0
#define PR_PC_HVYWEAP_GRENADE_TYPE_1       PR_GR_TYPE_NORMAL
#define PR_PC_HVYWEAP_GRENADE_TYPE_2       PR_GR_TYPE_MIRV
#define PR_PC_HVYWEAP_GRENADE_INIT_1       4
#define PR_PC_HVYWEAP_GRENADE_INIT_2       2
#define PR_PC_HVYWEAP_TF_ITEMS             PR_NIT_RESPAWN_GUARD                           // PZ: Added respawn guard for stock class editing.
#define PR_PC_HVYWEAP_CUTF_ITEMS           PR_CUTF_ASPECT_OF_HWGUY | PR_CUTF_FULLARMOUR   // PZ: added CUTF_FULLARMOUR so that full armor would be selected when editing Hwguy class
#define PR_PC_HVYWEAP_JOB                  PR_JOB_RUNNER

// Class Details for PYRO
//#define PC_PYRO_SKIN                     21
#define PR_PC_PYRO_MAXHEALTH               100
#define PR_PC_PYRO_MAXSPEED                290
//#define PC_PYRO_MAXSTRAFESPEED           290
#define PR_PC_PYRO_MAXARMOR                150
#define PR_PC_PYRO_INITARMOR               50
#define PR_PC_PYRO_MAXARMORTYPE            0.6
#define PR_PC_PYRO_INITARMORTYPE           0.6
//#define PC_PYRO_ARMORCLASSES             27      // ALL except EXPLOSION
#define PR_PC_PYRO_INITARMORCLASS          PR_AT_SAVEFIRE //WK Replaced 16 with #define
#define PR_PC_PYRO_WEAPONS                 PR_WEAP_INCENDIARY | PR_WEAP_FLAMETHROWER | PR_WEAP_AXE | PR_WEAP_SHOTGUN
#define PR_PC_PYRO_MAXAMMO_SHOT            40
#define PR_PC_PYRO_MAXAMMO_NAIL            50
#define PR_PC_PYRO_MAXAMMO_CELL            200
#define PR_PC_PYRO_MAXAMMO_ROCKET          60
#define PR_PC_PYRO_INITAMMO_SHOT           20
#define PR_PC_PYRO_INITAMMO_NAIL           0
#define PR_PC_PYRO_INITAMMO_CELL           120
#define PR_PC_PYRO_INITAMMO_ROCKET         24
#define PR_PC_PYRO_GRENADE_TYPE_1          PR_GR_TYPE_NORMAL
#define PR_PC_PYRO_GRENADE_TYPE_2          PR_GR_TYPE_NAPALM
#define PR_PC_PYRO_GRENADE_INIT_1          4
#define PR_PC_PYRO_GRENADE_INIT_2          4                          // PZ: added asbestos and respawn guard, below, so that when editing pyro class, they show up
#define PR_PC_PYRO_TF_ITEMS                PR_NIT_HOVER_BOOTS | PR_NIT_HOVER_BOOTS_UPGRADE | PR_NIT_ASBESTOS | PR_NIT_RESPAWN_GUARD
#define PR_PC_PYRO_CUTF_ITEMS              0               // PZ: Added this to make it easier to add CuTF items to the stock pyro in the future.
#define PR_PC_PYRO_JOB                     PR_JOB_THIEF

// Class Details for SPY
//#define PC_SPY_SKIN                      22
#define PR_PC_SPY_MAXHEALTH                90
#define PR_PC_SPY_MAXSPEED                 320
//#define PC_SPY_MAXSTRAFESPEED            320
#define PR_PC_SPY_MAXARMOR                 100
#define PR_PC_SPY_INITARMOR                50
#define PR_PC_SPY_MAXARMORTYPE             0.6
#define PR_PC_SPY_INITARMORTYPE            0.6
//#define PC_SPY_ARMORCLASSES              27      // ALL except EXPLOSION
#define PR_PC_SPY_INITARMORCLASS           0
#define PR_PC_SPY_WEAPONS                  PR_WEAP_AXE | PR_WEAP_TRANQ | PR_WEAP_SUPER_SHOTGUN | PR_WEAP_NAILGUN/* | #WEAP_HOOK*/
#define PR_PC_SPY_MAXAMMO_SHOT             40
#define PR_PC_SPY_MAXAMMO_NAIL             50
#define PR_PC_SPY_MAXAMMO_CELL             200
#define PR_PC_SPY_MAXAMMO_ROCKET           15
#define PR_PC_SPY_INITAMMO_SHOT            40
#define PR_PC_SPY_INITAMMO_NAIL            25
#define PR_PC_SPY_INITAMMO_CELL            100
#define PR_PC_SPY_INITAMMO_ROCKET          0
#define PR_PC_SPY_GRENADE_TYPE_1           PR_GR_TYPE_NORMAL
#define PR_PC_SPY_GRENADE_TYPE_2           PR_GR_TYPE_GAS
#define PR_PC_SPY_GRENADE_INIT_1           4
#define PR_PC_SPY_GRENADE_INIT_2           2
#define PR_PC_SPY_TF_ITEMS                 PR_NIT_SECURITY_CAMERA | PR_NIT_TELEPORTER | PR_NIT_RESPAWN_GUARD  // PZ: Added respawn guard for stock class editing.
#define PR_PC_SPY_CELL_REGEN_TIME          0.05
#define PR_PC_SPY_CELL_REGEN_AMOUNT        10
#define PR_PC_SPY_CELL_USAGE               25  // Amount of cells spent while invisible
#define PR_PC_SPY_GO_UNDERCOVER_TIME       4   // Time it takes to go undercover
#define PR_PC_SPY_JOB                      PR_JOB_HACKER // OfN - It was JOB_WARLOCK
#ifdef PR_NO_STOCK_SPY_STEALTH
    #define PR_PC_SPY_CUTF_ITEMS           PR_CUTF_SPY_KIT | PR_CUTF_KNIFE
#else
    #define PR_PC_SPY_CUTF_ITEMS           PR_CUTF_SPY_KIT | PR_CUTF_KNIFE | PR_CUTF_STEALTH
#endif

// Class Details for ENGINEER
//#define PC_ENGINEER_SKIN                 22      // Not used anymore
#define PR_PC_ENGINEER_MAXHEALTH           100
#define PR_PC_ENGINEER_MAXSPEED            290
//#define PC_ENGINEER_MAXSTRAFESPEED       290
#define PR_PC_ENGINEER_MAXARMOR            75
#define PR_PC_ENGINEER_INITARMOR           75
#define PR_PC_ENGINEER_MAXARMORTYPE        0.6
#define PR_PC_ENGINEER_INITARMORTYPE       0.3
//#define PC_ENGINEER_ARMORCLASSES         31      // ALL
#define PR_PC_ENGINEER_INITARMORCLASS      0
#ifndef PR_NO_ZEROGRAVGUN
    #define PR_PC_ENGINEER_WEAPONS         PR_WEAP_SPANNER | PR_WEAP_RAILGUN | PR_WEAP_SUPER_SHOTGUN | PR_WEAP_ZEROGRAVITY
#else
    #define PR_PC_ENGINEER_WEAPONS         PR_WEAP_SPANNER | PR_WEAP_RAILGUN | PR_WEAP_SUPER_SHOTGUN
#endif
#define PR_PC_ENGINEER_MAXAMMO_SHOT        75
#define PR_PC_ENGINEER_MAXAMMO_NAIL        50
#ifdef PR_CLASSIC_STOCK_ENGY_MAXCELLS
    #define PR_PC_ENGINEER_MAXAMMO_CELL    275 // synonymous with metal
#else
    #define PR_PC_ENGINEER_MAXAMMO_CELL    300 // synonymous with metal
#endif
#define PR_PC_ENGINEER_MAXAMMO_ROCKET      30
#define PR_PC_ENGINEER_INITAMMO_SHOT       30
#define PR_PC_ENGINEER_INITAMMO_NAIL       25
#ifdef PR_CLASSIC_STOCK_ENGY_MAXCELLS
    #define PR_PC_ENGINEER_INITAMMO_CELL   200     // synonymous with metal
#else
    #define PR_PC_ENGINEER_INITAMMO_CELL   150     // synonymous with metal
#endif
#define PR_PC_ENGINEER_INITAMMO_ROCKET     0
#define PR_PC_ENGINEER_GRENADE_TYPE_1      PR_GR_TYPE_NORMAL
#define PR_PC_ENGINEER_GRENADE_TYPE_2      PR_GR_TYPE_EMP
#define PR_PC_ENGINEER_GRENADE_INIT_1      4
#define PR_PC_ENGINEER_GRENADE_INIT_2      4
#define PR_PC_ENGINEER_TF_ITEMS            PR_NIT_TESLA | PR_NIT_SECURITY_CAMERA | PR_NIT_TELEPORTER | PR_NIT_RESPAWN_GUARD  // PZ: Added respawn guard for stock class editing.
#ifdef PR_NO_STOCK_ENGY_HIGHJUMP
    #define PR_PC_ENGINEER_CUTF_ITEMS      PR_CUTF_SENTRYGUN | PR_CUTF_DISPENSER | PR_CUTF_SENSOR | PR_CUTF_FIELDGEN
#else
    #define PR_PC_ENGINEER_CUTF_ITEMS      PR_CUTF_SENTRYGUN | PR_CUTF_DISPENSER | PR_CUTF_SENSOR | PR_CUTF_FIELDGEN | PR_CUTF_HIGHJUMP
#endif
#define PR_PC_ENGINEER_JOB                 PR_JOB_HACKER //#JOB_JUDOKA

// Class Details for CIVILIAN
//#define PC_CIVILIAN_SKIN                 22
#define PR_PC_CIVILIAN_MAXHEALTH           90
#define PR_PC_CIVILIAN_MAXSPEED            290
//#define PC_CIVILIAN_MAXSTRAFESPEED       290
#define PR_PC_CIVILIAN_MAXARMOR            75
#define PR_PC_CIVILIAN_INITARMOR           75
#define PR_PC_CIVILIAN_MAXARMORTYPE        0.3
#define PR_PC_CIVILIAN_INITARMORTYPE       0.3
//#define PC_CIVILIAN_ARMORCLASSES         0
#define PR_PC_CIVILIAN_INITARMORCLASS      0
#define PR_PC_CIVILIAN_WEAPONS             PR_WEAP_AXE | PR_WEAP_SUPER_SHOTGUN | PR_WEAP_SHOTGUN
#define PR_PC_CIVILIAN_MAXAMMO_SHOT        50
#define PR_PC_CIVILIAN_MAXAMMO_NAIL        0
#define PR_PC_CIVILIAN_MAXAMMO_CELL        0
#define PR_PC_CIVILIAN_MAXAMMO_ROCKET      0
#define PR_PC_CIVILIAN_INITAMMO_SHOT       30
#define PR_PC_CIVILIAN_INITAMMO_NAIL       0
#define PR_PC_CIVILIAN_INITAMMO_CELL       0
#define PR_PC_CIVILIAN_INITAMMO_ROCKET     0
#define PR_PC_CIVILIAN_GRENADE_TYPE_1      PR_GR_TYPE_NORMAL
#define PR_PC_CIVILIAN_GRENADE_TYPE_2      0
#define PR_PC_CIVILIAN_GRENADE_INIT_1      2
#define PR_PC_CIVILIAN_GRENADE_INIT_2      0
#define PR_PC_CIVILIAN_TF_ITEMS            0
#define PR_PC_CIVILIAN_JOB                 PR_JOB_THIEF
#define PR_PC_CIVILIAN_CUTF_ITEMS          PR_CUTF_KNIFE

/*==========================================================================*/
/* TEAMFORTRESS GOALS                                                       */
/*==========================================================================*/
// For all these defines, see the tfortmap.txt that came with the zip
// for complete descriptions.
// Defines for Goal Activation types : .goal_activation (in goals)
#define PR_TFGA_TOUCH              1  // Activated when touched
#define PR_TFGA_TOUCH_DETPACK      2  // Activated when touched by a detpack explosion
#define PR_TFGA_REVERSE_AP         4  // Activated when AP details are _not_ met
#define PR_TFGA_SPANNER            8  // Activated when hit by an engineer's spanner

// Defines for Goal Effects types : .goal_effect
#define PR_TFGE_AP                 1   // AP is affected. Default.
#define PR_TFGE_AP_TEAM            2   // All of the AP's team.
#define PR_TFGE_NOT_AP_TEAM        4   // All except AP's team.
#define PR_TFGE_NOT_AP             8   // All except AP.
#define PR_TFGE_WALL               16  // If set, walls stop the Radius effects
#define PR_TFGE_SAME_ENVIRONMENT   32  // If set, players in a different environment to the Goal are not affected
#define PR_TFGE_TIMER_CHECK_AP     64  // If set, Timer Goals check their critera for all players fitting their effects
#define PR_TFGE_CAUSE_EXPLOSION    128 // CH if set spawn explosion and use t_length

// Defines for Goal Result types : .goal_result
#define PR_TFGR_SINGLE             1   // Goal can only be activated once
#define PR_TFGR_ADD_BONUSES        2   // Any Goals activated by this one give their bonuses
#define PR_TFGR_ENDGAME            4   // Goal fires Intermission, displays scores, and ends level
#define PR_TFGR_NO_ITEM_RESULTS    8   // GoalItems given by this Goal don't do results
#define PR_TFGR_REMOVE_DISGUISE    16  // Prevent/Remove undercover from any Spy
#define PR_TFGR_CAUSERESPAWN       32  // CH force respawn, not die
#define PR_TFGR_DROPITEMS          4096// CH allows user to drop item with "dropitems"

// Defines for Goal Group Result types : .goal_group
// None!
// But I'm leaving this variable in there, since it's fairly likely
// that some will show up sometime.

// Defines for Goal Item types : .goal_activation (in items)
#define PR_TFGI_GLOW               1    // Players carrying this GoalItem will glow
#define PR_TFGI_SLOW               2    // Players carrying this GoalItem will move at half-speed
#define PR_TFGI_DROP               4    // Players dying with this item will drop it
#define PR_TFGI_RETURN_DROP        8    // Return if a player with it dies
#define PR_TFGI_RETURN_GOAL        16   // Return if a player with it has it removed by a goal's activation
#define PR_TFGI_RETURN_REMOVE      32   // Return if it is removed by TFGI_REMOVE
#define PR_TFGI_REVERSE_AP         64   // Only pickup if the player _doesn't_ match AP Details
#define PR_TFGI_REMOVE             128  // Remove if left untouched for 2 minutes after being dropped
#define PR_TFGI_KEEP               256  // Players keep this item even when they die
#define PR_TFGI_ITEMGLOWS          512  // Item glows when on the ground
#define PR_TFGI_DONTREMOVERES      1024 // Don't remove results when the item is removed
#define PR_TFGI_GOAL_TO_GROUND     2048 // CH GoalItem drops to ground when spawns
#define PR_TFGI_GOAL_IS_SOLID      8192 // CH GoalItem is solid

// Defines for TeamSpawnpoints : .goal_activation (in teamspawns)
#define PR_TFSP_MULTIPLEITEMS      1  // Give out the GoalItem multiple times
#define PR_TFSP_MULTIPLEMSGS       2  // Display the message multiple times

// Defines for TeamSpawnpoints : .goal_effects (in teamspawns)
#define PR_TFSP_REMOVESELF         1  // Remove itself after being spawned on

// Defines for Goal States
#define PR_TFGS_ACTIVE             1
#define PR_TFGS_INACTIVE           2
#define PR_TFGS_REMOVED            3
#define PR_TFGS_DELAYED            4

// Legal Playerclass Handling
#define PR_TF_ILL_SCOUT            1
#define PR_TF_ILL_SNIPER           2
#define PR_TF_ILL_SOLDIER          4
#define PR_TF_ILL_DEMOMAN          8
#define PR_TF_ILL_MEDIC            16
#define PR_TF_ILL_HVYWEP           32
#define PR_TF_ILL_PYRO             64
#define PR_TF_ILL_RANDOMPC         128
#define PR_TF_ILL_SPY              256
#define PR_TF_ILL_ENGINEER         512

// Attackers Go Red mode -PZ
#define PR_AGR_ROTATION_OFF PR_TRUE      // on a map triggered by map rotation, AGR is never on
#define PR_AGR_VOTE_MAX 0                // default maximum number of players allowed for an AGR vote (0 means no max)
#define PR_AGR_BLUE_RESPAWN_DELAY 5      // how long blue has to wait to respawn
                                         // (to make up for time red has to travel to contested area)
#define PR_AGR_FLAG_RESPAWN_DELAY 15     // when red captures the flag, how long does it take the flag to respawn?
//#define AGR_WHEN_BLUE_SCORES_RETURN_FLAG

// Invade mode -PZ  (Like AGR, but teams alternate between offense and defense, like on the map invade4a. UNFINISHED GAME MODE, BUT MOSTLY DONE, I THINK.)
#define PR_INVADE_PM_BUILD_PROTECT  0.10 // damage towards defense builds during prematch is multiplied by this
#define PR_INVADE_PM_PLAYER_PROTECT 0.33 // damage towards defense players during prematch is multiplied by this
#define PR_INVADE_DEF_RESPAWN_DELAY 5    // how long defense team has to wait to respawn
                                         // (to make up for time offense has to travel to contested area)

/*==========================================================================*/
/* Flamethrower                                                             */
/*==========================================================================*/
#define PR_FLAME_PLYRMAXTIME     45   // lifetime in 0.1 sec of a flame on a player
#define PR_FLAME_MAXBURNTIME     8    // lifetime in seconds of a flame on the world (big ones)
#define PR_NAPALM_MAXBURNTIME    20   // lifetime in seconds of flame from a napalm grenade
#define PR_FLAME_MAXPLYRFLAMES   4    // maximum number of flames on a player
#define PR_FLAME_NUMLIGHTS       1    // maximum number of light flame
// PZ: it seems really hard to set players on fire. Changed the following from 0.3 to 0.15. (10-25-14)
#define PR_FLAME_BURNRATIO       0.15 // the chance of a flame not 'sticking'
#define PR_GR_TYPE_FLAMES_NO     15   // number of flames spawned when a grenade explode

/*==================================================*/
/* CTF Support defines                              */
/*==================================================*/
#define PR_CTF_FLAG1       1
#define PR_CTF_FLAG2       2
#define PR_CTF_DROPOFF1    3
#define PR_CTF_DROPOFF2    4
#define PR_CTF_SCORE1      5
#define PR_CTF_SCORE2      6

/*==================================================*/
/* Death Message defines  (for global, `deathmsg`?) */
/*==================================================*/
#define PR_DMSG_SHOTGUN            1
#define PR_DMSG_SSHOTGUN           2
#define PR_DMSG_NAILGUN            3
#define PR_DMSG_LIGHT_ASSAULT      4
#define PR_DMSG_GRENADEL           5
#define PR_DMSG_ROCKETL            6
#define PR_DMSG_LIGHTNING          7
#define PR_DMSG_GREN_HAND          8
#define PR_DMSG_GREN_NAIL          9
#define PR_DMSG_GREN_MIRV          10
#define PR_DMSG_GREN_PIPE          11
#define PR_DMSG_DETPACK            12
#define PR_DMSG_BIOWEAPON          13
#define PR_DMSG_BIOWEAPON_ATT      14
#define PR_DMSG_FLAME              15
#define PR_DMSG_DETPACK_DIS        16
#define PR_DMSG_AXE                17
#define PR_DMSG_SNIPERRIFLE        18
#define PR_DMSG_AUTORIFLE          19
#define PR_DMSG_ASSAULTCANNON      20
#define PR_DMSG_HOOK               21
#define PR_DMSG_BACKSTAB           22
#define PR_DMSG_MEDIKIT            23
#define PR_DMSG_GREN_GAS           24
#define PR_DMSG_TRANQ              25
#define PR_DMSG_LASERBOLT          26
#define PR_DMSG_SENTRYGUN_BULLET   27
#define PR_DMSG_SNIPERLEGSHOT      28
#define PR_DMSG_SNIPERHEADSHOT     29
#define PR_DMSG_GREN_EMP           30
#define PR_DMSG_GREN_EMP_AMMO      31
#define PR_DMSG_SPANNER            32
#define PR_DMSG_INCENDIARY         33
#define PR_DMSG_SENTRYGUN_ROCKET   34
#define PR_DMSG_GREN_FLASH         35
#define PR_DMSG_TRIGGER            36
#define PR_DMSG_BUG_ZAPPER         37
#define PR_DMSG_GREN_FRAG          38
#define PR_DMSG_JUDOKA             39
#define PR_DMSG_MARTYR             40
#define PR_DMSG_HOVER              41  //Boot exhaust!
#define PR_DMSG_LAND_MINE          42
#define PR_DMSG_TESLA              43
#define PR_DMSG_DEMON_FIRE         44
#define PR_DMSG_BERSERK            45
#define PR_DMSG_MAUSER             50
#define PR_DMSG_DAEDALUS           51
#define PR_DMSG_EXPBODY            52  //- OfN -
#define PR_DMSG_ANTIHACK           53
#define PR_DMSG_ANTIDISM           54
#define PR_DMSG_CYBERNET           55
#define PR_DMSG_AIRG               56
#define PR_DMSG_AIRG_WATER         57
#define PR_DMSG_LASERCANNON        58
#define PR_DMSG_DISPEL             59
#define PR_DMSG_CALTROP            60
#define PR_DMSG_CLUSTER_ROCKET     61
#define PR_DMSG_FLYCALTROP         62
#define PR_DMSG_SNG                63
#define PR_DMSG_FORCEFIELD         64
#define PR_DMSG_STUCK_FORCEFIELD   65
#define PR_DMSG_SPANNERFIELD       66
#define PR_DMSG_FGTRAP             67
#define PR_DMSG_TELEOUTWORLD       68
#define PR_DMSG_BIOEXPLOSION       69
#define PR_DMSG_NAPALM             70  // Napalm grens
#define PR_DMSG_XMAS               71
#define PR_DMSG_EARTHQUAKE         72
#define PR_DMSG_STORM              73
#define PR_DMSG_ADMINKILL          74  // Gizmo - "cmd admin kill" command

// Defs for objects a spikeshooter can spawn  (Misc.qc)
#define PR_SPAWNFLAG_SUPERSPIKE    1
#define PR_SPAWNFLAG_LASER         2
// CH: add new things a trap can spawn
#define PR_SPAWNFLAG_TFROCKET      4
#define PR_SPAWNFLAG_TFGRENADE     8
#define PR_SPAWNFLAG_TFFLAME       16

// PZ NOTE: the standard client has a max width of 40 characters for CenterPrint(), and 1024 total characters
void CenterPrint(entity pl, const string& s1);
void CenterPrint2(entity pl, const string& s1, const string& s2);
void CenterPrint3(entity pl, const string& s1, const string& s2, const string& s3);
void CenterPrint4(entity pl, const string& s1, const string& s2, const string& s3, const string& s4);
void CenterPrint5(entity pl, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5);
void CenterPrint6(entity pl, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5, const string& s6);
void CenterPrint7(entity pl, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5, const string& s6, const string& s7);

void BackFlash(entity client, float ammount);

// PZ: I'm toying with the idea of putting all function declarations needed in other files right here, instead of making header files for each module.
//     We'll see how that goes. ==> After trying it this way, I determined this would take forever and be very ugly, because there are so many functions that
//     would have to be put here. So, going with header files.
/*void dremove(entity te);
void W_SetCurrentAmmo();
void increment_team_ammoboxes(float tno);
float num_team_ammoboxes(float tno);
void RemoveOldAmmobox(float tno);*/

} // END namespace Progs

#endif // END DEFS_H

