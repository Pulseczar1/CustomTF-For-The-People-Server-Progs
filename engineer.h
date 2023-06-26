#ifndef ENGINEER_H
#define ENGINEER_H

namespace Progs {

void LaserBolt_Think();
void LaserBolt_Touch();
void W_FireLaser();
void EMPExplode();
void EMPGrenadeTouch();
void EMPGrenadeExplode();
float HasFlag(float myteam);
void TeamFortress_EngineerBuild();
float CheckArea(entity obj, entity builder);
void TeamFortress_Build(float objtobuild, int devicenum);
void DispenserThink();
void TeamFortress_FinishedBuilding();
void T_Dispenser();
void Dispenser_Explode();
void Dispenser_Die();
void Engineer_UseDispenser(entity disp);
void Engineer_UseSentryGun(entity gun);
void Engineer_UseTesla(entity gun);
void Engineer_UseSensor(entity cam);
void Engineer_UseCamera(entity cam);
void Engineer_UseTeleporter(entity tele);
void CheckDistance();

} // END namespace Progs

#endif // END ENGINEER_H

