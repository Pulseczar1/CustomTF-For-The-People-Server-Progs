#ifndef DOORS_H
#define DOORS_H

namespace Progs {

void door_blocked();
void door_hit_top();
void door_hit_bottom();
void door_go_down();
void door_go_up();
void door_fire();
void door_use();
void door_trigger_touch();
void door_killed();
void door_touch();
entity spawn_field(const vector& fmins, const vector& fmaxs);
float EntitiesTouching(entity e1, entity e2);
void LinkDoors();
void func_door();
void fd_secret_use();
void fd_secret_pain(entity attacker, float damage);
void fd_secret_move1();
void fd_secret_move2();
void fd_secret_move3();
void fd_secret_move4();
void fd_secret_move5();
void fd_secret_move6();
void fd_secret_done();
void secret_blocked();
void secret_touch();
void func_door_secret();

} // END namespace Progs

#endif // END DOORS_H

