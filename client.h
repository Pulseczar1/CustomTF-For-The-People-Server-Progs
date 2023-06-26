#ifndef CLIENT_H
#define CLIENT_H

namespace Progs {

void info_intermission();
void SetChangeParms();
void SetNewParms();
void autoteam_think();
void PrematchBegin();
void TeamAllPlayers();
void DecodeLevelParms();
entity FindIntermission();
entity FindNextIntermission(entity start_point);
void TF_MovePlayer();
float DoExtraCycle();
void cycle_timer_think();
void cycle_timeout_think();
void cycle_error_think();
void SetCycleTimeoutTimer();
void SetCycleTimer();
void SetCycleErrorCheck();
void fail_think();
void CheckFailedMapChange();
void GotoNextMap();
void ExitIntermission();
void IntermissionThink();
void execute_changelevel();
void changelevel_use();
void changelevel_touch();
void trigger_changelevel();
void respawn();
void ClientKill();
entity FindTeamSpawnPoint(float team_num);
entity SelectSpawnPoint();
void CleanUpEverything();
void SetClientAlive();
void PutClientInServer();
float ValidSpawnPoint( entity point);
void info_player_start();
void info_player_start2();
void testplayerstart();
void info_player_deathmatch();
void info_player_coop();
void PrintClientScore(entity c);
void DumpScore();
void NextLevel();
void CheckRules();
void PlayerDeathThink();
void PlayerJump();
void WaterMove();
void CheckWaterJump();
void PlayerPreThink();
void CheckPowerups();
void PlayerPostThink();
void ClientConnect();
void ClientDisconnect();
void PlayerJoined();
void PlayerQuit();
void UpdateMapSettings();

} // END namespace Progs

#endif // END CLIENT_H

