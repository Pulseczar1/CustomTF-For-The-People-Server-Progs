#ifndef OPTIMIZE_H
#define OPTIMIZE_H

namespace Progs {

float Pharse_Client(entity targ, entity checker, float chkvis, float chkrng, float istesla, float chkcease);
void Give_Frags_Out(entity atk, entity targ, float numfrags, float fraggetlog, float howgive, float chkreal, float chkvamp);
void Find_And_Dmg(const string& search, entity person, float chkown, int type);
float isMelee();
float GetByte1(int value);
float GetByte2(int value);
float GetByte3(int value);
float MakeByte2(float value);
float MakeByte3(float value);
int AssignByte1(int targetval, float value);
int AssignByte2(int targetval, float value);
int AssignByte3(int targetval, float value);
int GetWarlock1(int value);
float GetWarlock2(int value);
float MakeWarlock2(float value);
int AssignWarlock1(int targetval, float value);
int AssignWarlock2(int targetval, float value);
int itob(int input);

} // END namespace Progs

#endif // END OPTIMIZE_H

