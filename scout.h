#ifndef SCOUT_H
#define SCOUT_H

namespace Progs {

void FlashGrenadeTouch();
void FlashTimer();
void FlashGrenadeExplode();
void PsionicGrenadeTouch();
void PsionicTimer();
void PsionicGrenadeExplode();
void ConcussionGrenadeTouch();
void ConcussionGrenadeExplode();
void ConcussionGrenadeTimer();
void TeamFortress_Scan(float scanrange,float inAuto);
void T_RadiusBounce(entity inflictor, entity attacker, float bounce, entity ignore);
float Scanner_Check_Player(entity scan, entity targ, float enemies, float friends);
entity T_RadiusScan(entity scanner, float scanrange, float enemies, float friends);
void CaltropGrenadeExplode();
void CreateCaltrop(const vector& org, entity shooter);
void CaltropPostTouch();
void CaltropTouch();

} // END namespace Progs

#endif // END SCOUT_H

