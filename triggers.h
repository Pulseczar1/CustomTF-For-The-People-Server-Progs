#ifndef TRIGGERS_H
#define TRIGGERS_H

namespace Progs {

void trigger_reactivate();
void multi_wait();
void multi_trigger();
void multi_killed();
void multi_use();
void multi_touch();
void trigger_multiple();
void trigger_once();
void trigger_relay();
void trigger_secret();
void counter_use();
void trigger_counter();
void play_teleport();
void spawn_tfog(const vector& org);
void tdeath_touch();
void spawn_tdeath(const vector& org, entity death_owner);
void teleport_touch();
void info_teleport_destination();
void teleport_use();
void trigger_teleport();
void trigger_skill_touch();
void trigger_setskill();
void trigger_onlyregistered_touch();
void trigger_onlyregistered();
void hurt_on();
void hurt_touch();
void trigger_hurt();
void trigger_push_touch();
void trigger_push();
void trigger_monsterjump_touch();
void trigger_monsterjump();

} // END namespace Progs

#endif // END TRIGGERS_H

