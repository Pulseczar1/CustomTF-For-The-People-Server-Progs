#ifndef TFORTMAP_H
#define TFORTMAP_H

namespace Progs {

void UpdateAbbreviations(entity Goal);
void TF_PlaceItem();
void TF_StartItem();
void TF_PlaceGoal();
void TF_StartGoal();
float CheckExistence();
void info_tfdetect();
void info_player_teamspawn();
void i_p_t();
void info_tfgoal();
void i_t_g();
void info_tfgoal_timer();
void i_t_t();
void item_tfgoal();
void ParseTFDetect(entity AD);
entity Finditem(float ino);
entity Findgoal(float gno);
entity Findteamspawn(float gno);
void InactivateGoal(entity Goal);
void RestoreGoal(entity Goal);
void RemoveGoal(entity Goal);
float IsAffectedBy(entity Goal, entity Player, entity AP);
void Apply_Results(entity Goal, entity Player, entity AP, float addb);
void RemoveResults(entity Goal, entity Player);
float APMeetsCriteria(entity Goal, entity AP);
void SetupRespawn(entity Goal);
void DoRespawn();
float Activated(entity Goal, entity AP);
void AttemptToActivate(entity Goal, entity AP, entity ActivatingGoal);
void DoGoalWork(entity Goal, entity AP);
void DoGroupWork(entity Goal, entity AP);
void DoItemGroupWork(entity Item, entity AP);
void DoTriggerWork(entity Goal, entity AP);
void DelayedResult();
void DoResults(entity Goal, entity AP, float addb);
void tfgoal_touch();
void info_tfgoal_use();
void tfgoal_timer_tick();
void item_tfgoal_touch();
void tfgoalitem_GiveToPlayer(entity Item, entity AP, entity Goal);
void Update_team_with_flag_touch(float item);
void Update_team_with_flag_drop(float item);
void ReturnItem();
void tfgoalitem_RemoveFromPlayer(entity Item, entity AP, float method);
void tfgoalitem_dropthink();
void tfgoalitem_dropthink2();
void tfgoalitem_drop(entity Item, float tossed);
void tfgoalitem_settouchandthink();
void tfgoalitem_remove();
void tfgoalitem_checkoriginagain();
void tfgoalitem_checkoriginagain2();
void tfgoalitem_checkoriginagain3();
void tfgoalitem_checkgoalreturn(entity Item);
void DisplayItemStatus(entity Goal, entity Player, entity Item);
void info_player_team1();
void info_player_team2();
void item_flag_team2();
void item_flag_team1();
void CTF_FlagCheck();

} // END namespace Progs

#endif // END TFORTMAP_H

