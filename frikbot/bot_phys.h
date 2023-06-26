#ifndef BOT_PHYS_H
#define BOT_PHYS_H

namespace Progs {

float frik_isBotPressingKey(int key);
void frik_turnInputCommandsIntoActions();
void frik_applyFrictionToBot();
void frik_botWaterJump();
void frik_botDropPunchAngle();
void frik_botAccelerateInAir(const vector& wishvel);
void frik_botAccelerateOnGround(const vector& wishvel);
void frik_botAccelerateInWater();
void frik_botMoveOnLandOrInAir();
void frik_clientThink();
float frik_RunThink();
void frik_applyGravityToBot(float scale);
float frik_botDetermineWaterLevel();
void frik_preventBotThudSound();
void frik_botDetermineOnGroundState();
float frik_botCheckForStep(const vector& dir);
void frik_botBruteForceStep(const vector& dir);
void PostPhysics();
void frik_movePhysObjectThroughAir();
void frik_SV_Physics_Toss();
void frik_SV_Physics_Client();

} // END namespace Progs

#endif // END BOT_PHYS_H

