#ifndef TESLA_H
#define TESLA_H

namespace Progs {

float Tesla_FindTarget();
void Tesla_FoundTarget();
void Tesla_Pain(entity attacker, float damage);
void Tesla_Die();
float Tesla_Fire();
void Tesla_Idle();
void Tesla_Touch();
float ReturnTeslaDelay();
entity Tesla_RadiusScan(entity scanner, float scanrange);
void Tesla_Check_Frags();
void Tesla_Lose_Glow();
void Tesla_Give_Glow();
void TeslaThink();
void tsla_on1();
void tsla_on2();
void tsla_on3();
void tsla_on4();
void tsla_on5();
void tsla_on6();
void tsla_on7();
void tsla_on8();
void tsla_on9();
void tsla_on10();
void tsla_on11();
void tsla_on12();
void tsla_on13();
void tsla_on14();
void tsla_fire1();
void tsla_fire2();
void tsla_fire3();
void tsla_fire4();
void tsla_fire5();
void tsla_fire6();
void tsla_fire7();
void tsla_fire8();
void tsla_fire9();
void tsla_fire10();
void tsla_fire11();
void tsla_fire12();
void TeslaThink();
void Tesla_Idle();
float Tesla_FindTarget();
void Tesla_FoundTarget();
void Tesla_Pain(entity attacker, float damage);
void Tesla_Die();
float DoorsAt(const vector& where);
float Tesla_Fire();
void Tesla_Touch();
float ReturnTeslaDelay();
entity Tesla_RadiusScan(entity scanner, float scanrange);
float Tesla_IsCloaked( entity tes);

} // END namespace Progs

#endif // END TESLA_H

