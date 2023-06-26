#ifndef HAXXX_H
#define HAXXX_H

namespace Progs {

void Menu_Friend_Hax();
void Menu_FriendHax_Inp(float inp);
void Menu_EnemyHax_Inp(float inp);
void Menu_Enemy_Hax();
void SBFireInterface();
void InitiateInterface(float input);
float GetICELevel(entity hacked);
float ReturnHackDelay(entity hacked, float r);
float ReturnFHackDelay(entity hacker, entity hacked, float r);
float CheckHaxxx();
void SBHackDotTimerThink();
void JobHacker();

} // END namespace Progs

#endif // END HAXXX_H

