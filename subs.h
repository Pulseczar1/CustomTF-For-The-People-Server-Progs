#ifndef SUBS_H
#define SUBS_H

namespace Progs {

void SUB_Null();
void SUB_PainNull(entity attacker, float damage);
void SetMovedir();
void InitTrigger();
void SUB_CalcMoveDone();
void SUB_CalcAngleMoveDone();
void DelayThink();
void SUB_UseTargets();
void SUB_CalcMoveEnt(entity ent, const vector& tdest, float tspeed, void (*func)());
void SUB_CalcMove(const vector& tdest, float tspeed, void (*func)());
void SUB_CalcAngleMoveEnt(entity ent, const vector& destangle, float tspeed, void (*func)());
void SUB_CalcAngleMove(const vector& destangle, float tspeed, void (*func)());
void SUB_CheckRefire(void (*thinkst)());

} // END namespace Progs

#endif // END SUBS_H

