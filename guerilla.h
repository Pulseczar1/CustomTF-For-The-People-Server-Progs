#ifndef GUERILLA_H
#define GUERILLA_H

namespace Progs {

void Menu_Guerilla();
void Menu_Guerilla_Input(float inp);
void GuerillaExplode();
void DetonateMines(entity mine_owner);
void GuerillaMineSweep(const vector& startpos);
void GuerillaThink();
void GuerillaTouch();
void GuerillaTossTouch();
void JobGuerilla();
void GuerillaSetMine(float minetype, float converted);
void MineKill(entity player);
float GetMineSpentPoints(entity player);
float GetMinePoints(entity player);
float GetMineEarnedPoints(entity player);
void SetMineSpentPoints(entity player,float spentpoints);
void SetMinePoints(entity player,float points);
void SetMineEarnedPoints(entity player,float points);
string GetGrenadeText(float typ);
float IsMineConvertible(float typ);
string GetMineStr(float minetype);
float ConvertGrenIDToMine(float grentype);
void MineUp(entity player, entity mine);
void MineDown(entity player, entity mine);
float AttemptToSetMine(entity player, float minetype, int converted);
float MineNeededPoints(float minetype);
void GuerillaInit(entity player);
float GetMinesUp(entity player);
void SetMinesUp(entity player, float minesup);
float GetMineConvBitfield(entity player);
void SetMineConvBitfield(entity player, float bitfield);
float GuerillaGetMaxPointsForJob(entity player);
void GuerillaUpgradeJob(entity player);

} // END namespace Progs

#endif // END GUERILLA_H

