#ifndef WORLD_H
#define WORLD_H

namespace Progs {

void Main();
void InsaneCoop_CheckDeadPlayers();
void spawn_think();
void worldspawn();
void StartFrame();
void bodyque();
void InitBodyQue();
void CopyToBodyQue(entity ent);
entity GetQueEntry();

} // END namespace Progs

#endif // END WORLD_H

