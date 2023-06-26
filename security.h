#ifndef SECURITY_H
#define SECURITY_H

namespace Progs {

void Security_Camera_Idle();
float Security_Camera_FindTarget();
float Security_Camera_FindFakeTarget();
void Security_Camera_Pain(entity attacker, float damage);
void Security_Camera_Die();
void Security_Camera_PrintTarget();
void Security_Camera_Spawn();
void SecurityCameraTossTouch();

} // END namespace Progs

#endif // END SECURITY_H

