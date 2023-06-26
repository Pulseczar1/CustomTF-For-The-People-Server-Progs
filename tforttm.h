#ifndef TFORTTM_H
#define TFORTTM_H

namespace Progs {

float TeamFortress_TeamPutPlayerInTeam(float performChecks);
float TeamFortress_TeamGetColor(float tno);
void TeamFortress_TeamSetColor(float tno);
string GetTeamName(float tno);
string GetTrueTeamName(float tno);
void SetTeamName(entity p);
float TeamFortress_TeamSet(float tno, float performChecks);
void TeamFortress_CheckTeamCheats();
void TeamFortress_TeamIncreaseScore(float tno, float scoretoadd, entity playerThatTriggeredScore);
float TeamFortress_TeamGetScore(float tno);
float TeamFortress_TeamGetLives(float tno);
float TeamFortress_TeamGetNoPlayers(float tno);
float TeamFortress_TeamGetMaxPlayers(float tno);
float TeamFortress_TeamGetWinner();
void TeamFortress_TeamShowScores(float all, float teamscored, float scorepoints);
string TeamFortress_TeamGetColorString(float tno);
void TeamFortress_TeamShowMemberClasses(entity Player);
void CalculateTeamEqualiser();
void SetupTeamEqualiser();
float TeamEqualiseDamage(entity targ, entity attacker, float damage);
void TeamFortress_StatusQuery();
int TeamFortress_TeamGetIllegalClasses(float tno);
float TeamFortress_TeamIsCivilian(float tno);
void teamsprint(float tno, entity ignore, const string& st);

} // END namespace Progs

#endif // END TFORTTM_H

