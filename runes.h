#ifndef RUNES_H
#define RUNES_H

namespace Progs {

void PrecacheRuneModels();
void GetRunesGlobal();
void UpdateRuneStuff(float oldrunes);
void StartRunes();
void EndRunes();
vector GetRuneSpot();
float CheckRuneSpot(const vector& spot);
float RuneOnSky(entity rune);
void Rune_think();
void Rune_touch();
entity SpawnRune(float runetype);
void SpawnNewRune(float runetype);
void InvalidateRune(entity rune);
void PlayerTakesRune(entity player, entity rune);
void PlayerDropRunes(entity player);
float GetPlayerNoRunes(entity player);
string GetRuneString(float runetype);
void ReportRuneTaken(entity player, float runetype);
void ReportRuneDrop(entity player, float runetype);
void RuneDisappear(entity rune);
void RuneAppear(entity rune);
float DropSpecificRune(entity player, float runenum);
void RuneStartEffects(entity player, int runetype);
void RuneEndEffects(entity player, int runetype);
void RegenRuneTimer_think();
void RemoveRegenRuneTimer(entity player);
void SpawnRegenRuneTimer(entity player);
void RespawnRune(entity rune, float nice);

} // END namespace Progs

#endif // END RUNES_H

