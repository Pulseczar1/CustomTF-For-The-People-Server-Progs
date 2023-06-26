// QW CenterPrinting hack
// these functions are only used if in normal quake

#include "progs.h"

namespace Progs {

#ifndef PR_QUAKE_WORLD

void bprint(float level, const string& s)
{
	nqw_bprint(s);
}

void sprint(entity client, float level, const string& s)
{
	nqw_sprint(client, s);
}

void bprint2(float level, const string& s1, const string& s2)
{
	nqw_bprint(s1, s2);
}

void sprint2(entity client, float level, const string& s1, const string& s2)
{
	nqw_sprint(client, s1, s2);
}

void bprint3(float level, const string& s1, const string& s2, const string& s3)
{
	nqw_bprint(s1, s2, s3);
}

void sprint3(entity client, float level, const string& s1, const string& s2, const string& s3)
{
	nqw_sprint(client, s1, s2, s3);
}

void bprint4(float level, const string& s1, const string& s2, const string& s3, const string& s4)
{
	nqw_bprint(s1, s2, s3, s4);
}

void sprint4(entity client, float level, const string& s1, const string& s2, const string& s3, const string& s4)
{
	nqw_sprint(client, s1, s2, s3, s4);
}

void bprint5(float level, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5)
{
	nqw_bprint(s1, s2, s3, s4, s5);
}

void sprint5(entity client, float level, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5)
{
	nqw_sprint(client, s1, s2, s3, s4, s5);
}

void bprint6(float level, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5, const string& s6)
{
	nqw_bprint(s1, s2, s3, s4, s5, s6);
}

void sprint6(entity client, float level, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5, const string& s6)
{
	nqw_sprint(client, s1, s2, s3, s4, s5, s6);
}

void bprint7(float level, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5, const string& s6, const string& s7)
{
	nqw_bprint(s1, s2, s3, s4, s5, s6, s7);
}

// these functions won't compile
/*
void(entity client, float level, string s1, string s2, string s3, string s4, string s5, string s6, string s7) sprint7 =
{
	nqw_sprint(client, s1, s2, s3, s4, s5, s6, s7);
};

void(float level, string s1, string s2, string s3, string s4, string s5, string s6, string s7, string s8) bprint8 =
{
	nqw_bprint(s1, s2, s3, s4, s5, s6, s7, s8);
};
*/
#endif

// replaces punchangle in QW
void KickPlayer(float psize, entity p)
{
#ifndef PR_QUAKE_WORLD
	self->punchangle[X] = psize;
#else
	msg_entity = p;
	if (psize > -3)
		WriteByte (PR_MSG_ONE, PR_SVC_SMALLKICK);
	else // (size < -3)
		WriteByte (PR_MSG_ONE, PR_SVC_BIGKICK);
#endif
}

void muzzleflash()
{
#ifdef PR_QUAKE_WORLD
	WriteByte (PR_MSG_MULTICAST, PR_SVC_MUZZLEFLASH);
	WriteEntity (PR_MSG_MULTICAST, self);
	multicast (self->origin, PR_MULTICAST_PVS);
#else
	self->effects = self->effects | PR_EF_MUZZLEFLASH;
#endif
}

} // END namespace Progs
