#ifndef COMMON_H
#define COMMON_H

namespace Progs {

float COOP_IsCoopMonster( entity test);
float COOP_FindShootOffset(entity start, entity end);
float COOP_SightBlocked(entity start, entity end);
float COOP_IsValidTarget( entity us, entity test);
float COOP_GetTargetPriority( entity scanner, entity e);
entity COOP_GetTeamList( float t);
void COOP_AddTeamList( entity list);
void COOP_AddToTeamList( entity e);
entity COOP_FindTarget(entity scanner);
void COOP_SmoothFrame();
void COOP_WalkSmooth( float dist);
float COOP_CanAttackEnemy( entity en);
float COOP_AttackTarget( entity en, float check);
float COOP_NoticeEnemy();
void COOP_Idle();
void COOP_Walk( float dist);
void COOP_Turn();
void COOP_Run(float dist);
void COOP_PrePain( entity attacker);
void COOP_MonsterDead( entity e);
void COOP_SetLives();
void respawn_monster();
void respawn_verify();
void respawn_think();
void COOP_SetupRespawn( entity e);
float COOP_CalcEntityDamage(entity targ, entity attacker, float indmg, float attacktype);

} // END namespace Progs

#endif // END COMMON_H

