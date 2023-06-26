#ifndef MENU_H
#define MENU_H

namespace Progs {

#define PR_MENU_DEFAULT                      1
#define PR_MENU_TEAM                         2
#define PR_MENU_CLASS                        3
#define PR_MENU_DROP                         4
#define PR_MENU_INTRO                        5
#define PR_MENU_CLASSHELP                    6
#define PR_MENU_CLASSHELP2                   7
#define PR_MENU_REPEATHELP                   8
#define PR_MENU_PICKBINDS                    9
#define PR_MENU_SHOWBINDS1                  10
#define PR_MENU_SHOWBINDS2                  11
#define PR_MENU_SPY                         12
#define PR_MENU_SPY_SKIN                    13
#define PR_MENU_SPY_COLOR                   14
#define PR_MENU_ENGINEER                    15
#define PR_MENU_ENGINEER_FIX_DISPENSER      16
#define PR_MENU_ENGINEER_FIX_SENTRYGUN      17
#define PR_MENU_ENGINEER_FIX_TESLA          18
#define PR_MENU_ENGINEER_FIX_TESLA2         19
#define PR_MENU_DISPENSER                   20
#define PR_MENU_ENGINEER_FIX_CAMERA         21
#define PR_MENU_ENGINEER_FIX_TELEPORTER     22
#define PR_MENU_ENGINEER_FIX_SENSOR         23
#define PR_MENU_ENGINEER_FIX_FIELDGEN       24
#define PR_MENU_ENGINEER_CONFIRM_DISMANTLE  25  // PZ - used for dismantle/destroy confirmation menu

#define PR_MENU_DEMON                       26  // SB menu for demon summon, I use 26 in the hope that nobody
                                                // has made anything conflicting with this for future vers.
                                                // Not that I expect anyone else to ever do anything...
#define PR_MENU_ARMY                        27
#define PR_MENU_F_HACK                      28
#define PR_MENU_E_HACK                      29
#define PR_MENU_CRUSADER                    30
#define PR_MENU_GUERILLA                    31

//- Voting System -//
#define PR_MENU_VOTE                        32  // PZ: Added general vote menu, where all votable options are reachable.
#define PR_MENU_PUNISH                      33
#define PR_MENU_VOTEMAP                     34
#define PR_MENU_VOTEBOT                     35  // PZ: for "votebot"  (probably won't keep this command; use general 'vote' menu, instead)

//WK - Menus for making a custom class
#define PR_MENU_PRIMARY_WEAPON              36
#define PR_MENU_SECONDARY_WEAPON            37
#define PR_MENU_MISC_WEAPON                 38
#define PR_MENU_LEGS                        39
#define PR_MENU_HEALTH                      40
#define PR_MENU_ARMOR                       41
#define PR_MENU_SPECIAL                     42
#define PR_MENU_SPECIAL2                    43
#define PR_MENU_GREN1                       44
#define PR_MENU_GREN2                       45
#define PR_MENU_ENGINEERING                 46
#define PR_MENU_PROFESSION                  47
#define PR_MENU_PROFESSION2                 48
#define PR_MENU_PROFICIENCY                 49
#define PR_MENU_OPTION                      50
#define PR_MENU_TOOLS                       51
#define PR_MENU_MOREOPTION                  52

// Gizmo - added
#define PR_MENU_CUSTOM_MIN                  36
#define PR_MENU_CUSTOM_MAX                  52

// Gizmo - no need to search for "MENU_OPTION" anymore, just change the above min/max macros
//If we extend this, search for "MENU_OPTION" below
//WK ------------------------------------

#define PR_MENU_REFRESH_RATE                25

#define PR_MENUSOUND_BROWSE   1
#define PR_MENUSOUND_BUY      2
#define PR_MENUSOUND_SELL     3
#define PR_MENUSOUND_WRONG    4

// tesla stuff
#define PR_UPGRADE    100 //Cells per upgrade //WK 2/8/7 125
//#define MAXUPGRADES 6
#define PR_MAXCELLS0  50
#define PR_MAXCELLS1  120
#define PR_MAXCELLS2  200
#define PR_MAXCELLS3  300
#define PR_ADDCELLS1  70 //how much to add with upgrade
#define PR_ADDCELLS2  80
#define PR_ADDCELLS3  100

void MenuPrecache();
void CuTFMenuSound(float sample);
void ResetMenu();
void Player_Menu();
void Menu_Team();
void Menu_Team_Input(float inp);
void Menu_Class();
void Menu_Class_Input(float inp);
void Menu_Drop();
void Menu_RepeatHelp();
void Menu_Drop_Input(float inp);
void Menu_DoBindings_Input(float inp);
void Menu_Input(float inp);
void Menu_Intro();
void Menu_ClassHelp();
void Menu_StuffClassHelp();
void Menu_ClassHelp2();
void Menu_ClassHelp_Input(float inp);
void Menu_DoBindings();
void Menu_ShowBindings1();
void Menu_ShowBindings2();
void Menu_Spy();
void Menu_Spy_Input(float inp);
void Menu_Spy_Skin();
void Menu_Spy_Skin_Input(float inp);
void Menu_Spy_Color();
void Menu_Spy_Color_Input(float inp);
void Menu_EngineerConfirmDismantle();
void Menu_EngineerConfirmDismantle_Input(float inp);
void Menu_EngineerDismantle();
void Menu_EngineerDestroy();
void Menu_Engineer();
void Menu_Engineer_Input(float inp);
void Menu_EngineerFix_Dispenser();
void Menu_EngineerFix_Dispenser_Input(float inp);
void Menu_EngineerFix_SentryGun();
void Menu_EngineerFix_SentryGun_Input(float inp);
void Menu_EngineerFix_Sensor();
void Menu_EngineerFix_Sensor_Input(float inp);
void Menu_EngineerFix_Camera();
void Menu_EngineerFix_Camera_Input(float inp);
void Menu_EngineerFix_Teleporter();
void Menu_EngineerFix_Teleporter_Input(float inp);
void Menu_EngineerFix_FieldGen();
void Menu_EngineerFix_FieldGen_Input(float inp);
string Return_Colored_Num(float num);
float Check_Tesla_Ups(float ups, entity person, entity sent, float type, float menu);
void Menu_EngineerRepair_Tesla();
float Menu_EngineerDismantle_Tesla();
void Tesla_Add_Rand_Upgrade(entity sent, entity who);
void Menu_EngineerFix_Tesla();
void Menu_EngineerFix_Tesla_Input(float inp);
void Menu_EngineerFix_Tesla2();
void Menu_EngineerFix_Tesla_Input2(float inp);
void Menu_Dispenser();
void Menu_Dispenser_Input(float inp);
void MenuResetCount();

} // END namespace Progs

#endif // END MENU_H

