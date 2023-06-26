#ifndef CPSTUFF_H
#define CPSTUFF_H

namespace Progs {

void UpdateInfos(float startup);
void teamprefixsprint(float tno, entity ignore);
void teamprefixsprintbi(float tno, entity ignore, entity ignore2);
void teamsprint6(entity ignore, const string& st, const string& st2, const string& st3, const string& st4, const string& st5, const string& st6);
void teamsprintbi(entity ignore, entity ignore2, const string& st, const string& st2, const string& st3, const string& st4, const string& st5, const string& st6);
float Is_TF_Skin(const string& skin_str);
string TeamGetNiceColor(float tno);
float IsValidTopColor(float tno, float theColor);
void UpdateCells(entity player);
void ActivateHolo(entity player);
void HoloThink();
void RemoveHolo(entity player);
void FlareGrenadeTouch();
void FlareBounce();
void FlareGrenadeExplode();
float IsMonster( entity thing);
float IsOwnedMonster(entity thing);
float IsSummon(entity thing);
float HasMonster(entity player);
string GetMonsterName(entity themonster);
void MonsterDie(entity themonster);
string GetBuildingName(entity thebuilding);
float IsOffenseBuilding(entity thebuilding);
string GetEnemyName(entity thething);
void PrintOwnedMonsterInfo( entity printDest, entity thething);
void PrintOwnedMonsterDetails( entity printDest, entity thething);
void ExpBody(entity body);
void ExpBodyThink();
void spawnFOG(const vector& where);
float CheckEnemyDismantle();
float vis2orig(const vector& spot1, const vector& spot2);
float GetNoPlayers(float checkIP, float noIdlers);
entity GetBestPlayer();
entity GetBestKiller();
entity GetBestTeamKiller();
entity GetBestDamager();
entity GetBiggestTarget();
void bprintline();
void PrintResults();
void MonsterKill(entity attacker);
float GetMaxGrens(entity theplayer, float grenslot);
float IsOutWorld(entity player);
float GoodIntermissionImpulse(float imp);
float ServerUpdating();
void PrintGameSettings(float broadcastmsg);
float GetDamageOutFactor(entity thing);
float GetDamageInFactor(entity thing);
void MuzzleFlash(entity ent);
float HurtLegs(entity player, float damage);
void HealLegs(entity player);
void PlayerFinal(entity player, const string& st, float action, float predefined);
void FinalizedPerformAction();
void SpawnBloodEx(const vector& where, float bloodtype, float bloodammount);
void SpawnSpikeFX(const vector& where, float spiketype);
void SpawnDmgFX(entity targetent, const vector& where, float spiketype, float bloodtype, float bloodammount);
float EnoughToBuild(entity player, float buildtype);
void LaunchMap(const string& nfmap);
void ExecCycleStandard();
float StripDisallowed(entity player);
float StripSpecificItem(entity player, float itemid, float storechange, float resetstored);
float GiveSpecificItem(entity player, float itemid, float storechange, float resetstored);
float GiveFreeStuff(entity player);
float SetItemInside(entity ent, float type, float itemid, float value);
float RestoreOriginalItems(entity player);
float DisabledStuff(float fieldnum);
void SetDisabledStuff(float fieldnum, float value);
float GivenStuff(float fieldnum);
void SetGivenStuff(float fieldnum, float value);
void ResetStripedAndGiven(entity player);
string GetClientDescription(entity client);
string GetBracedClDesc(entity client);
string ver_ftos(float ver);
entity SpawnMeatSprayEx(const vector& org, const vector& dir, float meatduration);
void GibCorpse(entity corpse);
void DamageCorpse(entity corpse, float damage, vector where, entity attacker, float directdmg);
void MakeCorpse(entity deadone, float corpse_dmg_tolerance);
void NoCprintFX();
void BackFlash(entity client, float ammount);
float IsValidUseObject( entity object);
string GetUseObjectAction( entity object);
string GetUseObjectName( entity object);
void UseObject( entity object);
float GetTeam( entity e);
float infront2( entity targ, entity check);
void ExtendedCPrint(entity tgt, const string& str, float iterations, float delayBtwPrints, float flash);
void ExtendedCPrintThink();

} // END namespace Progs

#endif // END CPSTUFF_H

