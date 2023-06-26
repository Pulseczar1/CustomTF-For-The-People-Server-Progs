#ifndef ADMIN_H
#define ADMIN_H

namespace Progs {

#define PR_DEFAULT_SILENCE_TIME 20 // minutes

float HasValidAdminTarget(entity theAdmin);
float HasValidAdminTarget2(entity theAdmin);
float ValidClientState(entity player);
void Admin_Kick_Cycle();
void Admin_Kick_Person();
void Admin_Ban_Person();
void Admin_Curse();
void Admin_Assign(float targetteam, bool usedChangeTeamCommand, bool silent = false);
void Admin_Cmd(const string& cmd);
void Admin_Cuff(const string& arg);
void Admin_Mute(const string& arg);
void Admin_Call_Ceasefire();
void Admin_KillStuff();
float GoodCeasefireImpulse(float inp);
void PrintAdmins(float none, float isadmin);
void NotifyAdmins(const string& text);
void PrintToAdmins(const string& s1, const string& s2, const string& s3, const string& s4, const string& s5, const string& s6, const string& s7, const string& s8);
void AdminLoggedIn(entity admin);
void AdminLoggedOut(entity admin);
float Admin_Tell(entity user, const string& str);
void Admin_EndGame();
void Admin_Sermon(const string& st, float action);
float Admin_Silence(float mode, float mutetime);
void Admin_Check();
void Admin_Use();
entity Admin_SpawnEntityAtLocation( const string& entity_classname, const vector& pos);
void Admin_Spawn( const string& entity_classname, float entity_team);
void Admin_MassSpawn( const string& entity_classname, float entity_team, float ecount);
float Admin_ValidEntityToKill( entity te);
void Admin_Kill();

} // END namespace Progs

#endif // END ADMIN_H

