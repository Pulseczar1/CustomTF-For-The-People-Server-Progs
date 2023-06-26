#ifndef BOT_MOVE_H
#define BOT_MOVE_H

namespace Progs {

void frik_makeBotJump();
float frik_canBotRocketJump(entity e);
float frik_detectAndHandlePlatform(float affectBotMovement);
float frik_getRequiredKeysToMoveThisDirection(const vector& movementDirectionVector);
void frik_checkForStuckBot();
void frik_botLookForAndHandleObstacles();
void frik_botHandleObstruction(const vector& directionOfObstruction, float dangerInOurPath);
void frik_botHandleObstructedFlag();
void frik_botPursueTarget1();
float frik_botMoveThisWay(const vector& direction);
void frik_botRoamDynamically();

} // END namespace Progs

#endif // END BOT_MOVE_H

