#ifndef SENTRY_H
#define SENTRY_H

namespace Progs {

void Sentry_Rotate();
float Sentry_FindTarget();
void Sentry_FoundTarget();
void Sentry_HuntTarget();
void Sentry_Pain(entity attacker, float damage);
void Sentry_Die();
float Sentry_Fire();
void Sentry_Touch();
void RemoveGlow();
void lvl1_sentry_atk3();
void lvl1_sentry_stand();
void lvl1_sentry_atk1();
void lvl1_sentry_atk2();
void lvl1_sentry_atk3();
void lvl2_sentry_atk3();
void lvl2_sentry_stand();
void lvl2_sentry_atk1();
void lvl2_sentry_atk2();
void lvl2_sentry_atk3();
void lvl3_sentry_atk3();
void lvl3_sentry_atk4();
void lvl3_sentry_stand();
void lvl3_sentry_atk1();
void lvl3_sentry_atk2();
void lvl3_sentry_atk3();
void lvl3_sentry_atk4();
void lvl3_sentry_atk5();
void Sentry_Rotate();
float rangesentry(entity targ);
float Sentry_FindTarget();
void Sentry_FoundTarget();
void Sentry_HuntTarget();
void Sentry_Pain(entity attacker, float damage);
void Sentry_Die();
float Sentry_Fire();
float BadSpot(const vector& vec);
void Sentry_Touch();

} // END namespace Progs

#endif // END SENTRY_H

