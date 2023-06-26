#ifndef WARLOCK_H
#define WARLOCK_H

namespace Progs {

// Warlock settings
#define PR_WARLOCK_BONUS1   28 // Number of hearts to get 1st bonus
#define PR_WARLOCK_BONUS2   42 // Number of hearts to get 2nd bonus

// Monster types
#define PR_MONSTER_SCRAG    1
#define PR_MONSTER_FISH     2
#define PR_MONSTER_DEMON    3
#define PR_MONSTER_GREMLIN  4
#define PR_MONSTER_SHAMBLER 5
#define PR_MONSTER_ARMY     6

// Monster health settings
#define PR_SHAMBLER_MAX_HP          4000
#define PR_GREMLIN_MAX_HP           3200
#define PR_DEMON_MAX_HP             3000
#define PR_SCRAG_MAX_HP             2000
#define PR_FISH_MAX_HP              2000

#define PR_SHAMBLER_MIN_HP          1200
#define PR_GREMLIN_MIN_HP           900
#define PR_DEMON_MIN_HP             800
#define PR_SCRAG_MIN_HP             500
#define PR_FISH_MIN_HP              500

// Meat/heads settings
#define PR_SHAMBLER_MAX_HEADS       40
#define PR_GREMLIN_MAX_HEADS        35
#define PR_DEMON_MAX_HEADS          30
#define PR_SCRAG_MAX_HEADS          20
#define PR_FISH_MAX_HEADS           20

#define PR_SHAMBLER_MAX_MEAT        240
#define PR_GREMLIN_MAX_MEAT         200
#define PR_DEMON_MAX_MEAT           160
#define PR_SCRAG_MAX_MEAT           120
#define PR_FISH_MAX_MEAT            120

// monster HP regeneration
#define PR_SCRAG_MAXREGEN      3
#define PR_SCRAG_REGENRATE     1.5

#define PR_FISH_MAXREGEN       4
#define PR_FISH_REGENRATE      1

#define PR_FIEND_MAXREGEN      3
#define PR_FIEND_REGENRATE     1.5

#define PR_GREMLIN_MAXREGEN    3.5
#define PR_GREMLIN_REGENRATE   1.5

#define PR_SHAMBLER_MAXREGEN   4
#define PR_SHAMBLER_REGENRATE  1.4

#define PR_AURA_ARMY_REGEN       3
#define PR_AURA_ARMY_REGENRATE   1.5

//#define AURA_REGEN_MINHEAL    3

// Monster feature settings
#ifdef PR_XMAS_STUFF
#define PR_SHAMBLER_XMAS_RANGE   5000 // too much?
#define PR_SHAMBLER_XMAS_DMG      400
#define PR_SHAMBLER_XMAS_MAXHITS    4
#endif

// Gizmo - monster modes
#define PR_SUMMON_MODE_HOLDPOSITION	0
#define PR_SUMMON_MODE_FOLLOWOWNER	1
#define PR_SUMMON_MODE_PATROLZONE	2

#define PR_summon_mode			penance_time

// Functions
void SummonThink();
entity CreateWaypoint(const vector& location, float life, float type, entity sld);
string GetMonsterName(entity themonster);
void MonsterTouch();
void MonsterIdle(entity themonster);
void spawnFOG(const vector& where);
void PutMonsterStand(entity themonster);
void PutMonsterWalk(entity themonster);
float GetHearts(entity player);
void SetHearts(entity player, float knifekills);
float GetNumSummons(entity warlock);
void SetCurrentSoul(entity player, float soul);
float GetCurrentSoul(entity player);
float GetMeatUsage(entity player);
float GetHeadsUsage(entity player);
entity GetSummon(entity warlock, float numsummon);
void KillSoul(entity warlock, float soulnum);
float GetNeededHearts(entity warlock, float input);
float GetMonsterLevel(entity monster);
float GetHPforMeat(entity monster);
void UpdateReserves(entity player);
void Gremlin_MeleeAttack();
void Gremlin_MissileAttack();
void gremlin_pain(entity attacker, float damage);
void RemoveWaypoint(entity wyp, entity soldier);
void ResetSlotPointer(entity player, float slot);
entity ReturnEasyWaypoint(entity sold, entity viewpoint);
float WizardCheckAttack();
float GremlinCheckAttack();
float IsSummon(entity thing);
void MonsterDie(entity themonster);
string GetMonsterName(entity themonster);
void PutMonsterStand(entity themonster);
float IsOffenseBuilding(entity thebuilding);
void PutMonsterWalk(entity themonster);
float CheckAttack();
float StillValidEnemy(entity thing);
void MonsterIdle(entity themonster);
void walkmonster_start();
void flymonster_start();
void swimmonster_start();
void flymonster_start_go();
void Menu_Demon();
void SummonThink();
void Menu_Demon_Input(float inp);
float IsNameTypeFree(entity player, float nametype);
float GetNameType(entity player, float slot, float forced);
void MakeMonsterName(entity monster);
void custom_demon_create(float points, float type);
void custom_demon_precache();
void kill_my_demons();
void MonsterTouch();
void PutMonsterStand(entity themonster);
void PutMonsterWalk(entity themonster);
void MonsterIdle(entity themonster);
void SetHearts(entity player, float knifekills);
void SetMeat(entity player, float numgibs);
void SetHeads(entity player, float numheads);
void SetCurrentSoul(entity player, float soul);
void SetMeatUsage(entity player, float usage);
void SetHeadsUsage(entity player, float usage);
float GetHearts(entity player);
float GetMeat(entity player);
float GetHeads(entity player);
float GetCurrentSoul(entity player);
float GetMeatUsage(entity player);
float GetHeadsUsage(entity player);
float GetNumSummons(entity warlock);
entity GetSummon(entity warlock, float numsummon);
float GetNumTypeMonster(entity player, float type);
void GibPickSound(entity warlock);
void WarlockKnifeKill(entity attacker);
float GetMonsterLevel(entity monster);
float GetHPforMeat(entity monster);
void monster_head_touch();
void ThrowMonsterHead(const string& gibname, float dm);
float GetNeededHearts(entity warlock, float input);
void KillSoul(entity warlock, float soulnum);
void ResetSlotPointer(entity player, float slot);
void AI_Regenerate(entity ai_entity);
string Summon_GetActionString( entity printDest, entity se);
void Summon_PrintDetails( entity printDest, entity se);

} // END namespace Progs

#endif // END WARLOCK_H
