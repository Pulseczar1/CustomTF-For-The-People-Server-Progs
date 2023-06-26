#ifndef QW_H
#define QW_H

namespace Progs {

#ifndef PR_QUAKE_WORLD
void bprint(float level, const string& s);
void sprint(entity client, float level, const string& s);
void bprint2(float level, const string& s1, const string& s2);
void sprint2(entity client, float level, const string& s1, const string& s2);
void bprint3(float level, const string& s1, const string& s2, const string& s3);
void sprint3(entity client, float level, const string& s1, const string& s2, const string& s3);
void bprint4(float level, const string& s1, const string& s2, const string& s3, const string& s4);
void sprint4(entity client, float level, const string& s1, const string& s2, const string& s3, const string& s4);
void bprint5(float level, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5);
void sprint5(entity client, float level, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5);
void bprint6(float level, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5, const string& s6);
void sprint6(entity client, float level, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5, const string& s6);
void bprint7(float level, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5, const string& s6, const string& s7);
#endif

void KickPlayer(float psize, entity p);
void muzzleflash();

} // END namespace Progs

#endif // END QW_H

