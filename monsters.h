#ifndef MONSTERS_H
#define MONSTERS_H

namespace Progs {

entity CreateWaypoint(const vector& location, float life, float type, entity sld);
string GetMonsterName(entity themonster);
void KillSoul(entity warlock, float soulnum);
float visible(entity targ);
float visible2(entity targ, entity check);
float visible2x(entity targ, entity check);
void FoundTarget();
void monster_use();
void set_monster_health();
void monster_death_use();
void walkmonster_start_go();
void walkmonster_start();
void flymonster_start_go();
void flymonster_start();
void swimmonster_start_go();
void swimmonster_start();
void MonsterDead();
void MonsterCorpse();
void MonsterAuraPower();

} // END namespace Progs

#endif // END MONSTERS_H
