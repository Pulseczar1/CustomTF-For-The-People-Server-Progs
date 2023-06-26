#ifndef BOT_QW_H
#define BOT_QW_H

namespace Progs {

// PZ: aliases for 'overloaded' fields (to avoid having to create a new field.. for EVERY entity..)
// PZ (2-18-2023): Using these two `npc` variables breaks Coop mode. The two, below, should work fine.
//#define PR_b_bot_previousTarget1   npc_nextTeamList
//#define PR_b_bot_weaponCycler      npc_nextEntInTeamList
#define PR_b_bot_previousTarget1     camera_list
#define PR_b_bot_weaponCycler        aiment

// --------defines-----
#define PR_NUMBER_OF_BOT_NAMES    32        // PZ: added this

// used for the physics & movement AI
#define PR_KEY_MOVEUP             1
#define PR_KEY_MOVEDOWN           2
#define PR_KEY_MOVELEFT           4
#define PR_KEY_MOVERIGHT          8
#define PR_KEY_MOVEFORWARD        16
#define PR_KEY_MOVEBACK           32
#define PR_KEY_LOOKUP             64
#define PR_KEY_LOOKDOWN           128
#define PR_KEY_LOOKLEFT           256
#define PR_KEY_LOOKRIGHT          512
// PZ: Added this. Auxiliary jump flag. Usually we use .#BUTTON_JUMP (.button2), but in the bot stuck detection timer, I need to be able to tell the bot to jump when it starts its
//     next frame. The problem is that the server wipes .#BUTTON_JUMP before the bot's frame runs. So, we'll store the jump flag here, where it won't get wiped.
#define PR_KEY_AUX_JUMP           1024
// PZ: added for readability
#define PR_KEY_MASK_MOVEMENT_KEYS 1087
#define PR_KEY_MASK_LOOKING_KEYS  960

// these are `.b_aiflags` for waypoints and bot behavior
// some overlap to the bot
// PZ TODO: WRITE DESCRIPTIONS FOR EACH OF THE AI FLAGS
// PZ NOTE: "readahead" means we perform an action before reaching the waypoint.
//          "point"     means we perform an action when reaching the waypoint.
#define PR_AI_TELELINK_1           1      // 1  the waypoint's link 1 is a teleporter link
#define PR_AI_TELELINK_2           2      // 2  the waypoint's link 2 is a teleporter link
#define PR_AI_TELELINK_3           4      // 3  the waypoint's link 3 is a teleporter link
#define PR_AI_TELELINK_4           8      // 4  the waypoint's link 4 is a teleporter link
#define PR_AI_DOORFLAG             16     // 5  read ahead
#define PR_AI_PRECISION            32     // 6  read ahead + point
#define PR_AI_SURFACE              64     // 7  point
#define PR_AI_BLIND                128    // 8  read ahead + point // I can use this to mean "just move towards the waypoint; don't worry about perceived obstructions, etc."
#define PR_AI_JUMP                 256    // 9  point + ignore
#define PR_AI_DIRECTIONAL          512    // 10 read ahead + ignore
#define PR_AI_PLAT_BOTTOM          1024   // 11 read ahead   // called "Wait For Plat" in waypoint editor // PZ: changed to "ON REACHING"
#define PR_AI_RIDE_TRAIN           2048   // 12 read ahead
#define PR_AI_SUPER_JUMP           4096   // 13 point + ignore + route test  (PZ NOTE: this likely isn't called "rocket jump" so that you can use it for other types of large jumps or maybe even the grappling hook)
#define PR_AI_SNIPER               8192   // 14 point type
#define PR_AI_AMBUSH               16384  // 15 point type
#define PR_AI_DOOR_NO_OPEN         32768  // 16 read ahead
#define PR_AI_DIFFICULT            65536  // 17 route test
#define PR_AI_TRACE_TEST           131072 // 18 route test

// these are flags for bots/players (dynamic/editor flags)
#define PR_AI_HANDLING_OBSTRUCTION 262144   // 19 PZ: was 1 and was called "AI_OBSTRUCTED"
#define PR_AI_HOLD_SELECT          2        //    PZ: I think this is only used on player in waypoint editor.
#define PR_AI_REJECT_TARGET        524288   // 20 PZ: was 2 // ### this doesn't seem to be used (PZ: renamed from "AI_ROUTE_FAILED")
#define PR_AI_WAIT                 1048576  // 21 PZ: was 4 // will probably be renamed to "AI_OVERRIDE_MOVEMENT"; also will likely need an "AI_OVERRIDE_AIMING"
#define PR_AI_DANGER               2097152  // 22 PZ: was 8

// addition masks  // PZ: He just had the totaled numbers here. I listed out the types to make it clear what was in the list and what wasn't.
// PZ: These were float constants. Changed to define to allow me to list this way rather than just a number like 29152. Unfortunately, constants
// won't allow you to do any addition or anything like "float CONSTANT = 1 + 4 + 16;". Make damn sure you include parentheses when doing it this
// way, with defines. The preprocessor litterally replaces the first token with the rest of the line, everywhere it finds the first token in the code.
// PZ: Added telelink flags to readahead types. Renamed AI_POINT_TYPES to AI_ON_REACHING_WAYPOINT, and AI_READAHEAD_TYPES to AI_ON_PURSUING_WAYPOINT.
#define PR_AI_ON_REACHING_WAYPOINT (PR_AI_TELELINK_1 | PR_AI_TELELINK_2 | PR_AI_TELELINK_3 | PR_AI_TELELINK_4 | PR_AI_PRECISION | PR_AI_SURFACE | PR_AI_BLIND | PR_AI_JUMP | PR_AI_PLAT_BOTTOM | PR_AI_SUPER_JUMP | PR_AI_SNIPER | PR_AI_AMBUSH) // was just 29152
#define PR_AI_ON_PURSUING_WAYPOINT (PR_AI_DOORFLAG | PR_AI_PRECISION | PR_AI_BLIND | PR_AI_DIRECTIONAL | PR_AI_RIDE_TRAIN | PR_AI_DOOR_NO_OPEN) // was just 36528
#define PR_AI_IGNORE_TYPES         (PR_AI_JUMP | PR_AI_DIRECTIONAL | PR_AI_SUPER_JUMP)                                                          // was just 4864

#define PR_WM_UNINIT               0
#define PR_WM_DYNAMIC              1
#define PR_WM_LOADING              2
#define PR_WM_LOADED               3
// editor modes aren't available in QW, but we retain support of them (PZ: I made them available in this code.)
// since the editor is still built into the AI in places
#define PR_WM_EDITOR               4
#define PR_WM_EDITOR_DYNAMIC       5
#define PR_WM_EDITOR_DYNLINK       6

// Waypoint Editor Menus
#define PR_MENU_MAIN                       1
#define PR_MENU_WAYPOINTS                  2
#define PR_MENU_LINKS                      3
#define PR_MENU_FLAGS                      4
#define PR_MENU_FLAGS2                     5
#define PR_MENU_BOTS                       6
#define PR_MENU_WAYLIST                    7
#define PR_MENU_ENTRY_LINK_WAYS            8
#define PR_MENU_ENTRY_TELELINK_WAYS        9
#define PR_MENU_ENTRY_DELETE_LINK          10
#define PR_MENU_ENTRY_CREATE_LINK_X2       11
#define PR_MENU_ENTRY_DELETE_LINK_X2       12
#define PR_MENU_ENTRY_DELETE_ALL_WAYS      13
#define PR_MENU_ENTRY_TELEPORT_TO_WAY      14
#define PR_MENU_ENTRY_DELETE_WAY           15
#define PR_MENU_ENTRY_UNLINK_WAY           16
#define PR_MENU_ENTRY_SAVE_WAYS            17
#define PR_MENU_COPY_WAYS                  18
#define PR_MENU_ENTRY_GO_TO_COORD_X        19
#define PR_MENU_ENTRY_GO_TO_COORD_Y        20
#define PR_MENU_ENTRY_GO_TO_COORD_Z        21
#define PR_MENU_SLIDE_WAY_COPIES           22

#define PR_OPT_SAVEBOTS            1  // PZ: over from bot.qc, but probably won't use the bot restore code
#define PR_OPT_NOCHAT              2

// PZ: added these defines for readability
#define PR_SKILL_EASY            0    // terribad         (this one is an emulated keyboarder)
#define PR_SKILL_MEDIUM          1    // okay             (Quake calls this "normal", but I'm calling it "medium")
#define PR_SKILL_HARD            2    // good player      (hard and medium emulate using the mouse; medium just isn't as good)
#define PR_SKILL_NIGHTMARE       3    // cheater, kind of (his aim just snaps to the targets)

// PZ: added these defines for readability
#define PR_TOPIC_NONE                          0
#define PR_TOPIC_SELF_JOINS_GAME               1
#define PR_TOPIC_SELF_GOT_A_KILL               2
#define PR_TOPIC_SELF_INFORMS_TEAM_INVIS       3
#define PR_TOPIC_SELF_INFORMS_TEAM_COVERING    4
#define PR_TOPIC_SELF_INFORMS_TEAM_NEED_BACKUP 5
#define PR_TOPIC_SELF_DIED                     6
// PZ: added these:
#define PR_TOPIC_SELF_LEAVES_GAME              7
// PZ: END
#define PR_TOPIC_MAP_ENDED                     8
#define PR_TOPIC_TARG_JOINS_GAME               9
#define PR_TOPIC_TARG_KILLED_THEIRSELF         10
#define PR_TOPIC_TARG_KILLED_BY_ENVIRONMENT    11
#define PR_TOPIC_LAG                           12

// PZ: .b_botRole: the bot's current roles (can have more than one at a time; some combinations make no sense, of course, like a defensive capper)
// NOTE: VECTOR BIT FLAGS MUST BE ENUMERATED BY FLAG NUMBER (1 TO 72) RATHER THAN FLAG VALUE. Because QuakeC can't deal with integers larger than 3 bytes.
#define PR_ROLE_DEFENSE                        1 // 1   protects flag/base   // Shouldn't have both offense and defense at the same time.
#define PR_ROLE_OFFENSE                        2 // 2   attacks enemy base   // Separate bits because neither set indicates a roamer or deathmatcher.
#define PR_ROLE_CAPPER                         3 // 4   Offensive bots without capper role will be cappers, but less focused on capping.
#define PR_ROLE_ENGINEER                       4 // 8
#define PR_ROLE_SNIPER                         5 // 16
#define PR_ROLE_SPY                            6 // 32
#define PR_ROLE_HACKER                         7 // 64
#define PR_ROLE_MEDIC                          8 // 128
//#define ROLE_                                9 // 256

// PZ: bot timer removal bitflags
// Every entity with "timer" in its classname and owned by a bot dying or disconnecting, will be checked
// for removal based on these flags. The flags will be stored in .deadflag in the bot timers.
#define PR_REMOVE_TIMER_ON_DEATH               1
#define PR_REMOVE_TIMER_ON_DISCONNECT          2

#define PR_BOT_SAY       0
#define PR_BOT_SAY_TEAM  1

/*// -------Bot ProtoTypes------
// external
void ClientConnect();
void ClientDisconnect();
void SetNewParms();

// rankings
vector getVectorBitFlagGivenBitNumber(float bitNumber);                                  // PZ: now returns a vector, for supporting 32 bots
bool isVectorBitFlagSet(float bitNumber, const vector& bitFlag, const vector& bitFlags); // PZ: for supporting 32 bots
//float() frik_getNextAvailableClientNumber;
void frik_addBotToGame(float whatbot, float whatskill);
void frik_handleBotDisconnection(entity bot);
//void(float clientno) BotInvalidClientNo;
//void(entity who) UpdateClient;

// waypointing
//void() frik_dynamicWaypointing;
entity frik_createBlankWaypointHere(const vector& org);
void frik_deleteAllWaypoints();
void frik_fixAllWaypoints();
float frik_startRouteFinding();
void frik_findRouteToObject(entity object, float direct);  // PZ: changed `this` to `object` for C++
void WaypointThink();
entity frik_findWaypointNearestAndVisibleToSelf(entity start);
void frik_attemptToLoadWaypointsForCurrentMap();
void frik_clearRouteForSelf();

// physics & movement
float frik_canBotRocketJump(entity e);
void frik_makeBotJump();
void frik_botRoamDynamically();
float frik_botMoveThisWay(const vector& weird);
void frik_botPursueTarget1();
void frik_botLookForAndHandleObstacles();
float frik_detectAndHandlePlatform(float flag);
float frik_getRequiredKeysToMoveThisDirection(const vector& sdir);
void frik_botHandleObstruction(const vector& whichway, float danger);
// PZ: We are no longer using the FrikBot's physics. Bots now use the same physics that real players use, in the C code.
//void() frik_SV_Physics_Client;
//void() frik_clientThink;
void frik_turnInputCommandsIntoActions();

// AI & Misc
void frik_AI_Main();
void frik_decideWhetherToDropTarget(entity targ);
string frik_getRandomNameForBot();
float frik_isInFovOfSelf(entity targ);
float frik_angleSubtraction(float y1, float y2);
float frik_isVisible_Waypoint(entity targ1, entity targ2);
float frik_isVisible_Simple(entity targ);
float frik_isVisible_Complex(entity targ);
vector frik_getRealOriginOfEntity(entity ent);
void frik_removeFromTargetStack(entity ent);
void frik_addToTargetStack(entity ent);
void frik_removeABot();
void frik_botImpulses();
void frik_dropTarget(entity targ, float success);
string frik_getNameForBotGivenNumber(float r);
float frik_getAngleMod360(float v);
void frik_botChat();
void frik_botConsiderStartingConversation(float tpic, float chance);

// editor stuff
void frik_toggleWaypointEditorOnOrOff();
void frik_displayAndManageWaypointEditorMenu();*/

// Functions
void setspawnparms(entity e);
vector aim(entity e, float missileSpeed);
void sound(entity e, float chan, const string& samp, float vol, float atten);
/*void WriteChar(float to, float f);
void WriteShort(float to, float f);
void WriteLong(float to, float f);
void WriteCoord(float to, float f);
void WriteAngle(float to, float f);
void WriteString(float to, const string& s);
void WriteEntity(float to, entity s);*/
void frik_initializeFrikBotModule();
void frik_attemptToLoadWaypointsForCurrentMap();
float frik_isClientSlotActive(float clientNumber);
vector getVectorBitFlagGivenBitNumber(float bitNumber);
vector setVectorBitFlag(float bitNumber, vector bitFlags);
vector clearVectorBitFlag(float bitNumber, vector bitFlags);
bool isVectorBitFlagSet(float bitNumber, const vector& bitFlag, const vector& bitFlags);
float frik_getNextAvailableClientNumber();
void UpdateClient(entity who);
void frik_setupNewClient();
void frik_handleClientDisconnection();
float frik_botPreThink();
float frik_botPostThink();
void frik_botSay(const string& h);
void frik_botSayDebug(const string& h);
void frik_botSayInit();
void frik_botSay2(const string& h);
void frik_botSayTeam(const string& msg, float timeDelay);
void frik_graphicallyDisplayWaypointLink(entity waypoint1, entity waypoint2, float bool_telelink);
entity frik_getClientEntityByNumber(float clientNumber);
void frik_addBotToGame(float botNameNumber, float botSkill);
void frik_handleBotDisconnection(entity bot);
void frik_mainRoutine();
void frik_botImpulses();
void countBotClasses(int (&result)[4][9]);

} // END namespace Progs

#endif // END BOT_QW_H

