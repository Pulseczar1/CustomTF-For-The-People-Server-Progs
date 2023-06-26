#ifndef AI_H
#define AI_H

namespace Progs {

//CH How much to divide speed when tranqed
//distance * (UP / DN)
#define PR_AI_TRANQ_FACTOR_UP		2
#define PR_AI_TRANQ_FACTOR_DN		3

float anglemod(float v);
float AI_Check_Contents(entity test);
void movetarget_f();
void path_corner();
void t_movetarget();
float range(entity targ);
float infront(entity targ);
void HuntTarget();
void SightSound();
void FoundTarget();
entity LookAround(entity scanner);
float FindTarget();
void ai_forward(float dist);
void ai_back(float dist);
void ai_pain(float dist);
void ai_painforward(float dist);
void ai_walk(float dist);
void ai_stand();
void ai_turn();
void ChooseTurn(const vector& dest3);
float FacingIdeal();
float CheckAnyAttack();
void ai_run_melee();
void ai_run_missile();
void ai_run_fire();
void ai_run_slide();
void ai_run(float dist);
float StillValidEnemy(entity thing);
void AI_CheckAttacker(entity attacker);

} // END namespace Progs

#endif // END AI_H

