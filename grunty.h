#ifndef GRUNTY_H
#define GRUNTY_H

namespace Progs {

void RemoveWaypoint(entity wyp, entity soldier);
void GruntyThink();
void GRun();
void GruntyScanTargets();
void GruntyCheckFire();
void GetRank(entity targ);
void Grunty_Check_Frags();
float botmovedist(float angle, float dist);
float ReturnWeaponVelocity();
void GruntyPayThink();
void GruntyDrawPay();
float isMelee();
void PrintFromSoldier(entity sld, entity player, const string& msg, float priority);
string GetOwnerMessage(entity sld);
string GetFriendlyMessage(entity sld);
entity ReturnEasyWaypoint(entity sold, entity viewpoint);
string GetEnemyName(entity thething);
void CheckWaterJump();
void grunty_setstartframe(entity grunty);
void grunty_run();
void grunty_stand();
void grunty_run();
void stand_frames();
void grunty_axeatta();
void grunty_axeattb();
void grunty_axeattc();
void grunty_axeattd();
void grunty_axeatt();
void grunty_shotgun();
void grunty_rocket();
void grunty_spike(const vector& org, const vector& dir);
void grunty_nail1();
void grunty_nail2();
void grunty_nail3();
float GruntyPickBestWeapon( entity bot, int allowedweaps, float dist);
void grunty_nail4();
vector Grunty_LeadShot();
void GruntyThink();
float IsValidGruntyTarget( entity gtarget, entity grunty);
entity LookAroundGrunty( entity scanner);
entity GruntyPharse();
void GruntyScanTargets();
float GruntyPickBestWeapon( entity bot, int allowedweaps, float dist);
void GruntyCheckFire();
vector GruntyPickClosestVector(vector desired);
float GruntyAutoObstacle( entity g);
void GRun();
float botmovedist(float angle, float dist);
void gruntyDead();
void grunty_diea1();
void grunty_diea2();
void grunty_diea3();
void grunty_diea4();
void grunty_diea5();
void grunty_diea6();
void grunty_diea7();
void grunty_diea8();
void grunty_diea9();
void grunty_diea10();
void grunty_diea11();
void grunty_dieb1();
void grunty_dieb2();
void grunty_dieb3();
void grunty_dieb4();
void grunty_dieb5();
void grunty_dieb6();
void grunty_dieb7();
void grunty_dieb8();
void grunty_dieb9();
void grunty_diec1();
void grunty_diec2();
void grunty_diec3();
void grunty_diec4();
void grunty_diec5();
void grunty_diec6();
void grunty_diec7();
void grunty_diec8();
void grunty_diec9();
void grunty_diec10();
void grunty_diec11();
void grunty_diec12();
void grunty_diec13();
void grunty_diec14();
void grunty_diec15();
void grunty_died1();
void grunty_died2();
void grunty_died3();
void grunty_died4();
void grunty_died5();
void grunty_died6();
void grunty_died7();
void grunty_died8();
void grunty_died9();
void grunty_diee1();
void grunty_diee2();
void grunty_diee3();
void grunty_diee4();
void grunty_diee5();
void grunty_diee6();
void grunty_diee7();
void grunty_diee8();
void grunty_diee9();
void grunty_die_ax1();
void grunty_die_ax2();
void grunty_die_ax3();
void grunty_die_ax4();
void grunty_die_ax5();
void grunty_die_ax6();
void grunty_die_ax7();
void grunty_die_ax8();
void grunty_die_ax9();
void ArmyDeathSound();
void custom_grunt_die();
void ArmyDeathSound();
void grunty_pain1();
void grunty_pain2();
void grunty_pain3();
void grunty_pain4();
void grunty_pain5();
void grunty_pain6();
void grunty_axepain1();
void grunty_axepain2();
void grunty_axepain3();
void grunty_axepain4();
void grunty_axepain5();
void grunty_axepain6();
void grunty_pain(entity attacker, float damage);
void Waypoint_Touch();
void Waypoint_DoNothing();
entity CreateWaypoint(const vector& location, float life, float type, entity sld);
void GetRank(entity targ);
void Grunty_Check_Frags();
float ReturnWeaponVelocity();
void GruntyPayThink();
void GruntyDrawPay();
void grunty_touch();
void RemoveWaypoint(entity wyp, entity soldier);
entity ReturnEasyWaypoint(entity sold, entity viewpoint);
void grunty_boundammo(entity sld);
string Grunty_GetTacticModeString( entity g);
string Grunty_GetEngageEnemyString( entity g);
string Grunty_GetOnObstacleString( entity g);
string Grunty_GetAttackRangeString( entity g);
string Grunty_GetActionString( entity printDest, entity g);
void Grunty_PrintDetails( entity printDest, entity g, float printLevel);

} // END namespace Progs

#endif // END GRUNTY_H

