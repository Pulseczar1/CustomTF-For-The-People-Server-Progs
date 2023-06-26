#ifndef INVADE_H
#define INVADE_H

namespace Progs {

#define PR_INVADE_PM_BUILD_PROTECT  0.10 // damage towards defense builds during prematch is multiplied by this
#define PR_INVADE_PM_PLAYER_PROTECT 0.33 // damage towards defense players during prematch is multiplied by this
#define PR_INVADE_DEF_RESPAWN_DELAY 5    // how long defense team has to wait to respawn
                                         // (to try make up for time offense has to travel to contested area)

void Invade_initialize();
void Invade_think();
void Invade_startPrematch();
float Invade_buffPlayer(entity player);
void Invade_startTimer();
void Invade_stopTimer();
void Invade_resetTimer();
void Invade_introducePlayer();
void Invade_invPrint();
void Invade_playTimerSounds();
void Invade_precacheSounds();

} // END namespace Progs

#endif // END INVADE_H
