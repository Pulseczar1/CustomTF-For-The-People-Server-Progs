#ifndef COMBAT_H
#define COMBAT_H

namespace Progs {

void monster_death_use();
float CanDamage(entity targ, entity inflictor);
void Killed(entity targ, entity attacker);
void T_Damage(entity targ, entity inflictor, entity attacker, float damage);
void TF_T_Damage(entity targ, entity inflictor, entity attacker, float damage, int T_flags, float T_AttackType);
void T_RadiusFireDamage(const vector& place, entity inflictor, entity attacker, float damage, float fireprob);
void T_RadiusDamage(entity inflictor, entity attacker, float damage, entity ignore);
void CoolExplosion(entity inflictor, float radius, float extent);
void T_BeamDamage(entity inflictor, entity attacker, float damage);

} // END namespace Progs

#endif // END COMBAT_H

