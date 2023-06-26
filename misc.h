#ifndef MISC_H
#define MISC_H

namespace Progs {

void info_null();
void info_notnull();
void light_use();
void light();
void light_fluoro();
void light_fluorospark();
void light_globe();
void FireAmbient();
void light_torch_small_walltorch();
void light_flame_large_yellow();
void light_flame_small_yellow();
void light_flame_small_white();
void misc_fireball();
void fire_fly();
void fire_touch();
void barrel_explode();
void misc_explobox();
void misc_explobox2();
void Laser_Touch();
void LaunchLaser(const vector& org, vector vec);
void TFFireRocket(const vector& org, vector vec);
void TFFireGrenade(const vector& org, vector vec);
void TFFireFlame(const vector& org, vector vec);
void spikeshooter_use();
void shooter_think();
void trap_spikeshooter();
void trap_tf_spikeshooter();
void trap_shooter();
void trap_tf_shooter();
void air_bubbles();
void make_bubbles();
void bubble_split();
void bubble_remove();
void bubble_bob();
void viewthing();
void func_wall_use();
void func_wall();
void func_illusionary();
void func_episodegate();
void func_bossgate();
void ambient_suck_wind();
void ambient_drone();
void ambient_flouro_buzz();
void ambient_drip();
void ambient_comp_hum();
void ambient_thunder();
void ambient_light_buzz();
void ambient_swamp1();
void ambient_swamp2();
void noise_think();
void misc_noisemaker();

} // END namespace Progs

#endif // END MISC_H

