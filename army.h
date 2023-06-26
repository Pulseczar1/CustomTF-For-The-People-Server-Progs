#ifndef ARMY_H
#define ARMY_H

namespace Progs {

#define PR_WAYPOINT_TYPE_PRIMARY        0
#define PR_WAYPOINT_TYPE_SECONDARY      1
#define PR_WAYPOINT_TYPE_ENEMYLASTSEEN  2

#define PR_ARMY_MAXFRAGS_PRESTIGE       40

// Gizmo - used for the grunty entity
#define PR_GRUNTY_MODE_FOLLOWOWNER      1
#define PR_GRUNTY_MODE_HOLDPOSITION     2
#define PR_GRUNTY_MODE_USEWAYPOINTS     3
#define PR_grunty_mode                  penance_time

#define PR_GRUNTY_TACTICMODE_NORMAL     0
#define PR_GRUNTY_TACTICMODE_SEEK       1
#define PR_GRUNTY_TACTICMODE_STATIC     2
#define PR_grunty_tacticMode            is_malfunctioning

#define PR_GRUNTY_ENGAGEENEMY_NO        0
#define PR_GRUNTY_ENGAGEENEMY_YES       1
#define PR_grunty_engageEnemy           is_detpacking

#define PR_GRUNTY_ONOBSTACLE_JUMP       0
#define PR_GRUNTY_ONOBSTACLE_STOP       1
#define PR_GRUNTY_ONOBSTACLE_AUTO       2
#define PR_grunty_onObstacle            is_toffingadet

#define PR_GRUNTY_ATTACKRANGE_ANY       0
#define PR_GRUNTY_ATTACKRANGE_SMALL     1
#define PR_GRUNTY_ATTACKRANGE_MEDIUM    2
#define PR_GRUNTY_ATTACKRANGE_LARGE     3
#define PR_grunty_attackRange           delay_time

// Functions
void JobArmy();
void Menu_Army();
void Menu_Army1(entity soul, float currentsoul);
void Menu_Army2(entity soul, float currentsoul);
void Menu_Army3( entity soul, float currentsoul);
void Menu_Army_Input(float inp);
void Menu_Army_Input3( float inp, entity soul, float currentsoul);
void Menu_Army_Input2(float inp, entity soul, float currentsoul);
void Menu_Army_Input1(float inp, entity soul, float currentsoul);
float TeleSoldier(float slot);
void SetArmyTimer(entity player, float start);
void ArmyTimerThink();
entity GetArmyTimer(entity player);
void RemoveArmyTimer();
void PrintArmyTime(entity player);
string Digitize(float num);
void PrintFromSoldier(entity sld, entity player, const string& msg, float priority);
string GetOwnerMessage(entity sld);
string GetFriendlyMessage(entity sld);
float GetFreeReserve(entity player);
void UpdateReserves(entity player);
void ArmyUpdateRating(entity player);
float GetMyAverageTeamScore(entity player);
void ArmyInit(entity player);
void ArmyRatingLeave(entity player);
void ArmyRatingJoin(entity player);
void AdjustArmyRating(entity player, float avg_score);

} // END namespace Progs

#endif // END ARMY_H

