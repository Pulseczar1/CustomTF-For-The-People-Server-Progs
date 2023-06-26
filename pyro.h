#ifndef PYRO_H
#define PYRO_H

namespace Progs {

entity FlameSpawn(const string& type, entity p_owner);
float RemoveFlameFromQueue(const string& id_flame);
void RemoveFlame();
void NapalmGrenadeTouch();
void NapalmGrenThink();
void NapalmGrenadeExplode();
void FlameFollow();
void OnPlayerFlame_touch();
void WorldFlame_touch();
void Boot_Flamer_stream_touch();
void Flamer_stream_touch();
void Napalm_touch();
void NewBubbles(float num_bubbles, const vector& bub_origin);
void W_FireFlame();
void T_IncendiaryTouch();
void W_FireIncendiaryCannon();
void TeamFortress_IncendiaryCannon();
void TeamFortress_FlameThrower();

} // END namespace Progs

#endif // END PYRO_H

