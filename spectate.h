#ifndef SPECTATE_H
#define SPECTATE_H

namespace Progs {

#define PR_CHARGE_SENSORS_TIME  4

// MENU Id's
#define PR_SPEC_MENU_NONE       0
#define PR_SPEC_MENU_DEFAULT    1
#define PR_SPEC_MENU_PUNISH     2
#define PR_SPEC_MENU_VOTEMAP    3
#define PR_SPEC_MENU_VOTEBOT    4   // PZ: for "votebot"

// Functions
void PlayClientSound(entity player, const string& thesound);
void BroadcastSound(const string& thesound);
void PlayDelayedSound_Think();
void PlayDelayedSound(entity player, const string& theSound, float delayDuration);
void chargesound();
void GetTeslaSpectatorScan( float team_num);
void TeslaSpectatorScan();
void GetSentryGunSpectatorScan( float team_num);
void SentryGunSpectatorScan();
void GetHelperSpectatorScan( float team_num);
void HelperSpectatorScan();
void SpectatorConnect();
void SpectatorDisconnect();
void SpectatorImpulseCommand();
void SpectatorThink();
void SpecResetMenu();
void Menu_Default();
void Menu_Default_Inp(float inp);
void Spec_ID();
void Menu_Charging();
void sprintlineb();
void sprintlineteam(float num);
void MachineryScan();
void GetTeamMachineryScan(float team_num);
void HelpersScan();
void GetTeamHelpersScan(float team_num);
void ClassScan();
void GetTeamClassScan(float teamnum);

} // END namespace Progs

#endif // END SPECTATE_H

