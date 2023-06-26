#ifndef ENVIRON_H
#define ENVIRON_H

namespace Progs {

void InitEnvironment();
float validatelit();
float validatesky();
float validatehrt();
void ApplyMapFlags();
void GetEnvironmentGlobals();
void EnvironmentPrecachees();
void UpdateEnvironment(float oldstorm, float oldearthquake, float olddaytime, float oldrunes);
void World_FadeIn(float delaytime, float fadetime, float factor);
void World_FadeOut(float delaytime, float fadetime, float factor);
void World_FadeMid(float delaytime, float fadetime, float factor);
void World_FadeEnd(float delaytime, float fadetime, float factor);
string ReturnBrightness(float brightness);
void SetWorldBrightness(float brightness);
entity FadeWorld(float brightness, float fadetime, float delaytime, float factor);
void FadeWorld_think();
void RemoveFadeTimer();
void InitStorm();
void Storm_think();
float GetThunderDelay(entity tstorm);
void CleanUpStorm(float report);
void StormThunder(entity tstorm);
void Thunder_think();
void SetLightningBrightness(float bright, entity stormevent);
void StormEvent_think();
void StormLightning(entity tstorm, float thunderdelay);
void ThunderSound(float thundernum, float vol);
float IsKillableByStorm(entity thing);
void UpdateVulnerability(entity thing);
void StormSpecial_think();
float StormSpecialEvent(entity tstorm);
entity StormSelectMachine(entity tstorm);
entity StormSelectPlayer(entity tstorm);
void InitEarthquake();
void Earthquake_think();
void EarthquakeStart(entity eqent);
void EarthquakeApply_think();
void EarthquakeEnd(entity eqent);
float GetNextEarthquakeTime(entity eqent);
void CleanUpEarthquake(float report);
void InitDayTime();
void DayTime_think();
void CleanUpDayTime(float report);

} // END namespace Progs

#endif // END ENVIRON_H

