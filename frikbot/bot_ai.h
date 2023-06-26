#ifndef BOT_AI_H
#define BOT_AI_H

namespace Progs {

float frik_isInTargetStack(entity e);
void frik_addToTargetStack(entity ent);
void frik_removeFromTargetStack(entity ent);
entity frik_getGoalTarget();
void frik_dropTarget(entity targ, float successfulWithTarget);
void frik_handleBotAiFlags();
void frik_makeNextWaypointInRouteTarget1();
float frik_determinePriorityOfObject(entity object);
float frik_findPriorityObjectToPursue(float largeScope);
void frik_decideWhetherToDropTarget(entity targ);
void frik_botAdjustAim();
void frik_botChooseRole();
void frik_botRoamWaypoints();
void frik_AI_Main();
void handleCrusader();
void handleSpy();
void handleHacker();
float botAttemptHacking(entity e, float isEnemy);
void handleThief();
void handleJudo();
void isBotInEnemyFieldOfView();
void callback_botTakesDamage(entity bot, float damage, entity damagingObject, entity responsibleOne, float T_flags, float T_AttackType);
float modulo(float a, float b);

} // END namespace Progs

#endif // END BOT_AI_H

