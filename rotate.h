#ifndef ROTATE_H
#define ROTATE_H

namespace Progs {

vector SUB_NormalizeAngles( vector ang );
void hurt_setdamage( entity ent, float amount);
void info_rotate();
void RotateTargets();
void RotateTargetsFinal();
void SetTargetOrigin();
void LinkRotateTargets();
void SetDamageOnTargets( float amount);
void rotate_entity_think();
void rotate_entity_use();
void rotate_entity_firstthink();
void func_rotate_entity();
void path_rotate();
void rotate_train_think();
void rotate_train_use();
void rotate_train_wait();
void rotate_train_stop();
void rotate_train_next();
void rotate_train_find();
void rotate_train();
void func_rotate_train();
void movewall_touch();
void movewall_blocked();
void movewall_think();
void func_movewall();
void rotate_object();
void rotate_door_think2();
void rotate_door_think();
void rotate_door_reversedirection();
void rotate_door_group_reversedirection();
void rotate_door_use();
void func_rotate_door();

} // END namespace Progs

#endif // END ROTATE_H

