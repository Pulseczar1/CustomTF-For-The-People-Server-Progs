#ifndef CUTFMENU_H
#define CUTFMENU_H

namespace Progs {

// Menu Item Strings
// PAGE 1 - Primary Weapons
#define PR_STR_SNIPERRIFLE          "Sniper Rifle    "
#define PR_STR_ASSAULTCANNON        "Assault Cannon  "
#define PR_STR_ROCKETLAUNCHER       "Rocket Launcher "
#define PR_STR_PYROTOYS             "Pyro Toys       "
#define PR_STR_GRENADELAUNCHER      "Grenade Set     "
#define PR_STR_LIGHTNING            "Lightning Gun   "
#define PR_STR_LIGHTASSAULT         "Light Assault   "
// PAGE 2 - Secondary Weapons
#define PR_STR_SNG                  "Super Nailgun   "
#define PR_STR_KNIFE                "Knife           "
#define PR_STR_SUPERSHOTGUN         "Double Barrel   "
#define PR_STR_RAILGUN              "Rail Gun        "
#define PR_STR_NAILGUN              "Nail Gun        "
#define PR_STR_TRANQ                "Tranquilizer    "
#define PR_STR_SHOTGUN              "Single Barrel   "
// PAGE 3 - Misc/Extra Weapons
#define PR_STR_MAUSER               "Mauser          "
#define PR_STR_AIRF                 "Air-Fist        "
#define PR_STR_AIRFISTUPGRADE       "Airfist upgrade "
#define PR_STR_DAEDALUS             "Impulse Rifle   "
#define PR_STR_LASERCANNON          "Laser Cannon    "
#define PR_STR_HOLO                 "Holo Device     "
#define PR_STR_CLIPEXTEND           "Clip Extender   "
// PAGE 4 - Tools
#define PR_STR_ZEROGRAVITY          "Zero-Gravity Gun"
// PAGE 5 - Leg Enhancements
#define PR_STR_SPEED1               "Cheetah         "
#define PR_STR_SPEED2               "Cougar          "
#define PR_STR_SPEED3               "White Rhino     "
#define PR_STR_SPEED4               "Scrub Jay       "
#define PR_STR_SPEED5               "KGB Officer     "
#define PR_STR_SPEED6               "Black Mamba     "
#define PR_STR_SPEED7               "ImperialPenguin "
// PAGE 6 - Health/Armor
#define PR_STR_HP1                  S_("50^b/^b75 Green     ")
#define PR_STR_HP2                  S_("50^b/^b90 Green     ")
#define PR_STR_HP3                  S_("100^b/^b80 Yellow   ")
#define PR_STR_HP4                  S_("120^b/^b90 Yellow   ")
#define PR_STR_HP5                  S_("150^b/^b100 Yellow  ")
#define PR_STR_HP6                  S_("200^b/^b100 Red     ")
#define PR_STR_HP7                  S_("300^b/^b100 Red     ")
// PAGE 7 - Special Armors & Ammo
#define PR_STR_CERAMIC              "Ceramic Armor   "
#define PR_STR_GET                  "Impact Armor    "
#define PR_STR_ASBESTOS             "Asbestos Armor  "
#define PR_STR_KEVLAR               "Kevlar Armor    "
#define PR_STR_BLAST                "Blast Armor     "
#define PR_STR_AMMOBACKPACK         "Ammo Backpack   "
#define PR_STR_AMMOBANDOLIER        "Ammo Bandolier  "
// PAGE 8 - Special Items
#define PR_STR_SCANNER              "Scanner         "
#define PR_STR_AUTOSCANNER          "  Auto-Scanner  "
#define PR_STR_DETPACK              "Detpack         "
#define PR_STR_MEDIKIT              "Medikit         "
#define PR_STR_SPYKIT               "Spy Kit         "
#define PR_STR_SCUBA                "Scuba Gear      "
#define PR_STR_HOOK                 "Grapple         "
// PAGE 9 - More Special Items
#define PR_STR_JAMMER               "Scanner Jammer  "
#define PR_STR_CYBERAUG             "CyberAug Unit   "
#define PR_STR_SENSOR               "Motion Sensor   "
#define PR_STR_FIELDGEN             "Field Generator "
#define PR_STR_FULLARMOUR           "Full Armour     "
#define PR_STR_TOSSABLEDET          "C4 Tossable Det "
#define PR_STR_DISPENSER            "Dispenser       "
// PAGE 10 - Combat Grenades
#define PR_STR_GR_NORMAL            "Normal          "
#define PR_STR_GR_NAPALM            "Napalm          "
#define PR_STR_GR_GAS               "Gas             "
#define PR_STR_GR_EMP               "EMP             "
#define PR_STR_GR_FRAG              "Frag            "
#define PR_STR_GR_NAIL              "Nail            "
#define PR_STR_GR_MIRV              "MIRV            "
// PAGE 11 - Utility Grenades
#define PR_STR_GR_FLARE             "Flare           "
#define PR_STR_GR_CALTROP           "Caltrop         "
#define PR_STR_GR_CONCUSSION        "Concussion      "
#define PR_STR_GR_PSIONIC           "Psionic         "
#define PR_STR_GR_ANTIGRAV          "AntiGrav        "
#define PR_STR_GR_BIOLOGICAL        "Biological      "
#define PR_STR_GR_KRAC              "Krac            "
// PAGE 12 - Engineering
#define PR_STR_SPANNER              "Spanner         "
#define PR_STR_SENTRYGUN            "Sentry Gun      "
#define PR_STR_SENTRYUPGRADE        " Sentry Upgrade "
#define PR_STR_TESLACOIL            "Tesla Coil      "
#define PR_STR_TESLAUPGRADE         "  Tesla Upgrade "
#define PR_STR_SECURITYCAMERA       "Security Camera "
#define PR_STR_TELEPORTER           "Teleporter Pads "
// PAGE 13 - Professions
#define PR_STR_THIEF                S_("Thief     ^b(^bHide^b)^b")
#define PR_STR_RUNNER               S_("Runner  ^b(^bSprint^b)^b")
#define PR_STR_WARLOCK              S_("Warlock ^b(^bSummon^b)^b")
  #define PR_STR_WARLOCK_EXTRA1     "2nd Extra Soul  "
  #define PR_STR_WARLOCK_EXTRA2     "3rd Extra Soul  "
#define PR_STR_CHAPLAN              S_("Chaplan^b(^bInspire^b)^b")
#define PR_STR_BERSERKER            S_("Berserker ^b(^bFury^b)^b")
#define PR_STR_GUERILLA             S_("Guerilla ^b(^bMines^b)^b")
  #define PR_STR_GUERILLA_EXTRA1    "Mine Technician "
  #define PR_STR_GUERILLA_EXTRA2    "Mine Specialist "
#define PR_STR_JUDOKA               S_("Judoka  ^b(^bDisarm^b)^b")
// PAGE 14 - More Professions
#define PR_STR_ARMY                 S_("Army  ^b(^bSoldiers^b)^b")
  #define PR_STR_ARMY_EXTRA1        "Extra Reserve 2 "
  #define PR_STR_ARMY_EXTRA2        "Extra Reserve 3 "
#define PR_STR_HACKER               S_("CyberNet  ^b(^bHack^b)^b")
#define PR_STR_MARTYR               S_("Martyr ^b(^bSuicide^b)^b")
#define PR_STR_CRUSADER             S_("Crusader ^b(^bAuras^b)^b")
// PAGE 15 - Proficiencies
#define PR_STR_STEALTH              "Stealth         "
#define PR_STR_HIGHJUMP             "Highjump        "
#define PR_STR_HWGUY                "Aspect of HWGuy "
#define PR_STR_EXPBODY              "Exp. Body       "
#define PR_STR_GYMNAST              "Gymnast         "
#define PR_STR_DEMONLORE            "Demon Lore      "
#define PR_STR_CLOSECOMBAT          "Close Combat    "
// PAGE 16 - Optional Equipment
#define PR_STR_AUTOID               "Auto-ID         "
#define PR_STR_RESPAWNGUARD         "Respawn Guard   "
#define PR_STR_HOVERBOOTS           "Hover Boots     "
#define PR_STR_BOOTUPGRADE          "  Boot Upgrade  "
#define PR_STR_RLASERGUIDED         "Laser Guided RL "
#define PR_STR_OTR                  "OTR Bullets     "
#define PR_STR_CLUSTER              "Cluster Rockets "
// PAGE 17 - More Equipment
#define PR_STR_FASTERROCKETS        "Faster Rockets  "
#define PR_STR_FRIENDORFOE          "Friend or Foe   "

#define PR_STR_SELLEXTRAS           "Sell stuff back "

// For the love of god, we needed this before!! (easily tweakable, SINGLE VALUES!)
// PAGE 1 - Primary Weapons
#ifdef PR_CLASSIC_SNIPER_RIFLE
    #define PR_COST_SNIPERRIFLE     5000
#else
    #define PR_COST_SNIPERRIFLE     4800
#endif
#define PR_COST_ASSAULTCANNON       3000
#define PR_COST_ROCKETLAUNCHER      3000
#define PR_COST_PYROTOYS            3000
#define PR_COST_GRENADELAUNCHER     2200
#define PR_COST_LIGHTNING           2200
#define PR_COST_LIGHTASSAULT        1800
// PAGE 2 - Secondary Weapons
#define PR_COST_SNG                 800
#define PR_COST_KNIFE               650
#define PR_COST_SUPERSHOTGUN        575
#define PR_COST_RAILGUN             500
#define PR_COST_NAILGUN             400
#define PR_COST_TRANQ               300
#define PR_COST_SHOTGUN             50
// PAGE 3 - Misc/Extra Weapons
#define PR_COST_MAUSER              2400
#define PR_COST_AIRF                2000
#define PR_COST_AIRFISTUPGRADE      1850
#define PR_COST_DAEDALUS            1500
#define PR_COST_LASERCANNON         1400
#define PR_COST_HOLO                1200
#define PR_COST_CLIPEXTEND          600
// PAGE 4 - Tools
#define PR_COST_ZEROGRAVITY         1000
// PAGE 5 - Leg Enhancements
#define PR_COST_SPEED1              9000
#define PR_COST_SPEED2              6000
#define PR_COST_SPEED3              3250
#define PR_COST_SPEED4              2500
#define PR_COST_SPEED5              1250
#define PR_COST_SPEED6              500
// PAGE 6 - Health/Armor
#define PR_COST_HP1                 150
#define PR_COST_HP2                 450
#define PR_COST_HP3                 700
#define PR_COST_HP4                 1200
#define PR_COST_HP5                 1800
#define PR_COST_HP6                 2700
#define PR_COST_HP7                 4100
// PAGE 7 - Special Armors & Ammo
#define PR_COST_CERAMIC             575
#define PR_COST_GEL                 625    // PZ: this was called "COST_GET"; probably a typo
#define PR_COST_ASBESTOS            750
#define PR_COST_KEVLAR              1200
#define PR_COST_BLAST               1500
#define PR_COST_AMMOBACKPACK        300
#define PR_COST_AMMOBANDOLIER       200
// PAGE 8 - Special Items
#define PR_COST_SCANNER             650
#define PR_COST_AUTOSCANNER         250
#define PR_COST_DETPACK             800
#define PR_COST_MEDIKIT             1550
#define PR_COST_SPYKIT              2000
#define PR_COST_SCUBA               1000
#define PR_COST_HOOK                3000
// PAGE 9 - More Special Items
#define PR_COST_JAMMER              1000
#define PR_COST_CYBERAUG            800
#define PR_COST_SENSOR              450
#define PR_COST_FIELDGEN            1400
#define PR_COST_FULLARMOUR          250
#define PR_COST_TOSSABLEDET         1200
#define PR_COST_DISPENSER           600
// PAGE 10 - Combat Grenades
#define PR_COST_GR_NORMAL           500
#define PR_COST_GR_NAPALM           1400
#define PR_COST_GR_GAS              1300
#define PR_COST_GR_EMP              1200
#define PR_COST_GR_FRAG             1300
#define PR_COST_GR_NAIL             1400
#define PR_COST_GR_MIRV             1500
// PAGE 11 - Utility Grenades
#define PR_COST_GR_FLARE            300
#define PR_COST_GR_CALTROP          500
#define PR_COST_GR_CONCUSSION       1000
#define PR_COST_GR_PSIONIC          1200
#define PR_COST_GR_ANTIGRAV         1000
#define PR_COST_GR_BIOLOGICAL       1100
#define PR_COST_GR_KRAC             1400
// PAGE 12 - Engineering
#define PR_COST_SPANNER             1200
#define PR_COST_SENTRYGUN           1500
#define PR_COST_SENTRYUPGRADE       700
#define PR_COST_TESLACOIL           1350
#define PR_COST_TESLAUPGRADE        1650
#define PR_COST_SECURITYCAMERA      350
#define PR_COST_TELEPORTER          1100
// PAGE 13 - Professions
#define PR_COST_THIEF               950
#define PR_COST_RUNNER              400
#define PR_COST_WARLOCK             1150
  #define PR_COST_WARLOCK_EXTRA1    700
  #define PR_COST_WARLOCK_EXTRA2    950
#define PR_COST_CHAPLAN             1200
#define PR_COST_BERSERKER           1200
#define PR_COST_GUERILLA            1600
  #define PR_COST_GUERILLA_EXTRA1   700
  #define PR_COST_GUERILLA_EXTRA2   950
#define PR_COST_JUDOKA              1250
// PAGE 14 - More Professions
#define PR_COST_ARMY                1250
  #define PR_COST_ARMY_EXTRA1       700
  #define PR_COST_ARMY_EXTRA2       950
#define PR_COST_HACKER              1200
#define PR_COST_MARTYR              1000
#define PR_COST_CRUSADER            1600
// PAGE 15 - Proficiencies
#define PR_COST_STEALTH             400
#define PR_COST_HIGHJUMP            500 // Original was 200
#define PR_COST_HWGUY               450
#define PR_COST_EXPBODY             1200
#define PR_COST_GYMNAST             450
#define PR_COST_DEMONLORE           300
#define PR_COST_CLOSECOMBAT         600
// PAGE 16 - Optional Equipment
#define PR_COST_AUTOID              25
#define PR_COST_RESPAWNGUARD        25
#define PR_COST_HOVERBOOTS          850
#define PR_COST_BOOTUPGRADE         350
#define PR_COST_RLASERGUIDED        1000
#define PR_COST_OTR                 1850
#define PR_COST_CLUSTER             3250
// PAGE 17 - More Equipment
#define PR_COST_FASTERROCKETS       800
#define PR_COST_FRIENDORFOE         25

// Leg Options (Speeds)
#define PR_CUSTOM_SPEED1            410 // Cheetah (shaka original = 650)
#define PR_CUSTOM_SPEED2            370 // Cougar (shaka original = 420)
#define PR_CUSTOM_SPEED3            330
#define PR_CUSTOM_SPEED4            310
#define PR_CUSTOM_SPEED5            290
#define PR_CUSTOM_SPEED6            270
#define PR_CUSTOM_SPEED7            250 // Imperial Penguin

// Health and Armor Options
// PZ: Created these defines and replaced hard-coded values with the defines, to make editing the code (and these values) easier.
// IMPORTANT: If you edit these, make the strings that show up in the menu match these values (search above for STR_HP1 to STR_HP7).
#define PR_CUSTOM_ARMOR_AMOUNT_1    50
#define PR_CUSTOM_ARMOR_TYPE_1      PR_GREEN_ARMOR
#define PR_CUSTOM_HEALTH_1          75
#define PR_CUSTOM_ARMOR_AMOUNT_2    50
#define PR_CUSTOM_ARMOR_TYPE_2      PR_GREEN_ARMOR
#define PR_CUSTOM_HEALTH_2          90
#define PR_CUSTOM_ARMOR_AMOUNT_3    100
#define PR_CUSTOM_ARMOR_TYPE_3      PR_YELLOW_ARMOR
#define PR_CUSTOM_HEALTH_3          80
#define PR_CUSTOM_ARMOR_AMOUNT_4    120
#define PR_CUSTOM_ARMOR_TYPE_4      PR_YELLOW_ARMOR
#define PR_CUSTOM_HEALTH_4          90
#define PR_CUSTOM_ARMOR_AMOUNT_5    150
#define PR_CUSTOM_ARMOR_TYPE_5      PR_YELLOW_ARMOR
#define PR_CUSTOM_HEALTH_5          100
#define PR_CUSTOM_ARMOR_AMOUNT_6    200
#define PR_CUSTOM_ARMOR_TYPE_6      PR_RED_ARMOR
#define PR_CUSTOM_HEALTH_6          100
#define PR_CUSTOM_ARMOR_AMOUNT_7    300
#define PR_CUSTOM_ARMOR_TYPE_7      PR_RED_ARMOR
#define PR_CUSTOM_HEALTH_7          100

// Char used to visually indicate a purchased item (DO NOT USE QUOTES!)
//#define CHAR_BOUGHT *      // <-- Classic "*" char
#define PR_CHAR_BOUGHT \x9C  // small yellow dot

// Functions
void Menu_PrimaryWeapon();
void Menu_PrimaryWeapon_Input(float inp);
void Menu_SecondaryWeapon();
void Menu_SecondaryWeapon_Input(float inp);
void Menu_MiscWeapon();
void Menu_MiscWeapon_Input(float inp);
void Menu_ToolWeapon();
void Menu_ToolWeapon_Input(float inp);
void Menu_Legs();
void Menu_Legs_Input(float inp);
void Menu_Health();
void Menu_Health_Input(float inp);
void Menu_Health();
float GetHPCost(float numitem);
void Menu_Health_Input(float inp);
void Menu_Armor();
void Menu_Armor_Input(float inp);
void Menu_Special();
void Menu_Special_Input(float inp);
void Menu_Special2();
void Menu_Special2_Input(float inp);
void Menu_Gren1();
void Menu_Gren1_Input(float inp);
void Menu_Gren2();
void Menu_Gren2_Input(float inp);
void Menu_Engineering();
void Menu_Engineering_Input(float inp);
void Menu_Profession();
void Menu_Profession_Input(float inp);
void Menu_Profession2();
void Menu_Profession2_Input(float inp);
void Menu_Proficiency();
void Menu_Proficiency_Input(float inp);
void Menu_Option();
void Menu_Option_Input(float inp);
void Menu_MoreOption();
void Menu_MoreOption_Input(float inp);
string ConstructMenuItemStr(float itemnum, string itemdesc, float cost, int field, int bit);
string ConstructMenuSentryStr(float itemnum, string itemdesc, float cost);
string ConstructMenuTeslaStr(float itemnum, string itemdesc, float cost);
string ConstructMenuJobStr(float itemnum, string itemdesc, int jobid, float cost, float extra1cost, float extra2cost, const string& extra1st, const string& extra2st);
float SpentOnJob();
string ConstructMenuGrenStr(float itemnum, string itemdesc, float cost, float grentype);
float GetGrenCost(float grentype);
float SpentOnGrens();
float SpentOnLegs();
float GetCurrentLegs();
float GetPreviousLegs();
string ConstructMenuSpeedStr(float itemnum, string itemdesc, float cost);
float SpentOnHP();
float GetCurrentHPItem();
string ConstructMenuHPStr(float itemnum, string itemdesc, float cost);
void UpdateArmorItem(entity player);
float getTotalSpent();

} // END namespace Progs

#endif // END CUTFMENU_H

