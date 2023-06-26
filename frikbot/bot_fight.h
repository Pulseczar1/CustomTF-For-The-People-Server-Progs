#ifndef BOT_FIGHT_H
#define BOT_FIGHT_H

namespace Progs {

float frik_doesWeaponHurtTeammates(float weap);
float frik_sizeUpPlayer(entity e);
void frik_lookForHazardousObject();
void frik_acquireTargetForAttacking();
vector frik_getRangeOfWeapon(float wpn);
void frik_makeWeaponSelection(float rangeToTarget, float onlyAllowBetterWeapon);
entity swapInWeaponList(entity e1, entity e2, entity firstWeapon);
void frik_makeWeaponSelection(float rangeToTarget);
void frik_botFireWeaponIfShotGood();
void frik_botThrowHandGrenade();
void frik_engageTheEnemy();

} // END namespace Progs

#endif // END BOT_FIGHT_H

