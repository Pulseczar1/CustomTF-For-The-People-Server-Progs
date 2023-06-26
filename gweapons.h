#ifndef GWEAPONS_H
#define GWEAPONS_H

namespace Progs {

void RemoveTestEntity();
float InSolid(entity ent);
void zg_drop(entity user, float toss);
float zg_getpickupdist(entity theObject);
void ZGG_precache();
void player_zgg1();
void player_zgg2();
void zg_movetowards(entity user, entity theObject, const vector& destination);
float zg_getpickupdist(entity theObject);
float zg_infront(entity source, entity targ);
float zg_invalidtarget(entity tester, entity test);
entity zg_findtarget(entity source);
void zg_think();
void zg_drop(entity user, float toss);
void zg_pickup(entity user);
void zg_togglepickup(entity user, float toss);

} // END namespace Progs

#endif // END GWEAPONS_H
