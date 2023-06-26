#ifndef JOBS_H
#define JOBS_H

namespace Progs {

// chaplain defines
#define PR_GUIDE_TIME         1   //Period of how often lightning guides are shown. Must be less than...
#define PR_CHAPLAN_TIME       1   //Period of seconds how often it fires
#define PR_INSPIRE_TIME       6   //How long someone stays inspired
#define PR_CHAPLAN_RADIUS     320 //About the radius of brightglow
#define PR_CHAPLAN_HEAL       50  //If you have a medikit you'll heal friends this much
#define PR_CHAPLAN_HEAL_DELAY 3   //You can't have been shot in last three seconds to be healed

float GetJobExtras(entity player);
void SetJobExtras(entity player, float extras);
void JobThief();
void RunnerThink();
void JobRunner();
void JobWarlock();
void ChaplanInspire();
void ChaplanGuides();
void ChaplanThink();
void JobChaplan();
void MartyrThink();
void JobMartyr();
void BerserkerKillTimer();
void JobBerserker();
void JudokaRearm();
void JobJudoka();
void UseJobSkill();
void RevealThief(entity targ, float pain);
void InitJobStuff();

} // END namespace Progs

#endif // END JOBS_H

