/*======================================================
	OPTIMIZE.QC			Custom TeamFortress v2.1
	(c) Craig Hauser				10/4/00
========================================================
This has the optimizations that I have done, basically if
I saw something similar being run in multi places I made
a function and put it here.
======================================================*/

#include "progs.h"
#include "custom.h"

namespace Progs {

float visible2(entity targ, entity check);
float visible2x(entity targ, entity check);
void TF_T_Damage(entity targ, entity inflictor, entity attacker, float damage, int T_flags, float T_AttackType);
#ifdef PR_COOP_MODE_ENHANCED
float COOP_IsCoopMonster( entity test);
#endif
//checks client passed into and returns true if valid; used for sents, etc.
//entity looking at, entity thats looking, (0-1) checks visible, (0-range) checks range, (0-1) do special tesla checks, (0-1) should check for ceasefire
float Pharse_Client(entity targ, entity checker, float chkvis, float chkrng, float istesla, float chkcease)
{
		if (targ->classname != "player")
			return PR_FALSE;
#ifdef PR_QUAKE_WORLD // culled by KK. was QUAKE_WORLD
		if (chkcease)
			/*if (infokey(world,"ceasefire")=="on")
				return #FALSE;*/
            if (ceasefire)
                return PR_FALSE;
#endif
		if (targ->playerclass == PR_PC_UNDEFINED)
			return PR_FALSE;
		if (targ->done_custom & PR_CUSTOM_BUILDING)
			return PR_FALSE;
		if (targ->health <= 0)
			return PR_FALSE;
		if (!targ->is_connected)
			return PR_FALSE;
		if (teamplay)
		{
			if (Teammate(targ, checker))
				{
					if (istesla)
					{
						if (!(checker->tf_items & PR_NIT_SECURITY_CAMERA)) //CH Cyto's idea
							return PR_FALSE;
					} else
					{
						return PR_FALSE;
					}
				}
			if (Teammate(targ->undercover_team, checker))
				{
					if (istesla)
					{
						//- OfN jammer only for scanner -// nope
                        if (!(checker->tf_items & PR_NIT_AUTOID) || targ->cutf_items & PR_CUTF_JAMMER)
                        //if (!(checker.tf_items & #NIT_AUTOID))
							return PR_FALSE;
					}
					else
					{
                        if (!(checker->classname == "building_sensor" && checker->all_active & PR_IMPROVED_FOUR))
                            return PR_FALSE;

                        // - OfN return #FALSE;
					}
				}
		}
		if (targ == checker->real_owner)
		{
			if (istesla)
			{
				if (!(checker->tf_items & PR_NIT_SECURITY_CAMERA)) //CH Cyto's idea
					return PR_FALSE;
			} else
			{
				return PR_FALSE;
			}
		}
		if (targ->is_feigning)
		{
			if (istesla)
			{
				if (!(checker->tf_items & PR_NIT_AUTOID))
					return PR_FALSE;
			}
			else //if (checker.classname != "monster_army") - OfN soldiers cant detect feigning spies now
			{
				return PR_FALSE;
			}
		}
		if (targ->flags & PR_FL_NOTARGET)
			return PR_FALSE;
		if (targ->items & PR_IT_INVISIBILITY && (checker->classname != "building_sensor")) //|| checker.classname != "monster_army"))
			return PR_FALSE;
#ifdef PR_COOP_MODE_ENHANCED
		// never check thief here if checker is a monster
		if (targ->job & PR_JOB_THIEF && (targ->job & PR_JOB_ACTIVE || targ->job & PR_JOB_FULL_HIDE) && !COOP_IsCoopMonster( checker ))
#else
		if (targ->job & PR_JOB_THIEF && (targ->job & PR_JOB_ACTIVE || targ->job & PR_JOB_FULL_HIDE))
#endif
		{
			if (istesla)
			{
				//- OfN jammer only for scanner -// nope
                if (!(checker->tf_items & PR_NIT_AUTOID) || targ->cutf_items & PR_CUTF_JAMMER)
                //if (!(checker.tf_items & #NIT_AUTOID))
					return PR_FALSE;
			}
			else if (checker->classname != "building_sensor")// && checker.classname != "monster_army" && checker.classname != "monster_demon1")
				return PR_FALSE;
		}
		if (chkvis && checker->classname == "building_sentrygun" && !(checker->tf_items & PR_NIT_TURRET)) // - OfN - hackish fix for sentry gun targetting
		{
            if (!visible2x(targ,checker)) // - OfN - hackish fix for sentry gun targetting
 				return PR_FALSE;
        }
        else if (chkvis)
			if (!visible2(targ,checker))
 				return PR_FALSE;
		if (chkrng)
		{
			float r;

			r = vlen(targ->origin - checker->origin);
			if (r > chkrng)
				return PR_FALSE;
		}
	return PR_TRUE;
}
//gives entity passed into number of frags passed
//entity gets frag, entity gave frag, give this many frags, (0-1) log frags, 1=real_frag 2=frag, (0-1) checks real_frag, (0-1) check frag vamp (takes from targ)

void Give_Frags_Out(entity atk, entity targ, float numfrags, float fraggetlog, float howgive, float chkreal, float chkvamp)
{
/*
local string st;
bprint(#PRINT_HIGH, "atk:");
bprint(#PRINT_HIGH, atk.classname);
bprint(#PRINT_HIGH, " targ:");
bprint(#PRINT_HIGH, targ.classname);
bprint(#PRINT_HIGH, " Frag:");
st = ftos(numfrags);
bprint(#PRINT_HIGH, st);
bprint(#PRINT_HIGH, " log:");
if (fraggetlog)
	bprint(#PRINT_HIGH, "yes");
else
	bprint(#PRINT_HIGH, "no");
bprint(#PRINT_HIGH, " howgive:");
if (howgive == 1)
	bprint(#PRINT_HIGH, "real_frags");
else if (howgive == 2)
	bprint(#PRINT_HIGH, "frags");
else
	bprint(#PRINT_HIGH, "ERROR");
bprint(#PRINT_HIGH, " real:");
if (chkreal)
	bprint(#PRINT_HIGH, "yes");
else
	bprint(#PRINT_HIGH, "no");
bprint(#PRINT_HIGH, " vamp:");
if (chkvamp)
	bprint(#PRINT_HIGH, "yes");
else
	bprint(#PRINT_HIGH, "no");
bprint(#PRINT_HIGH, "\n");
*/
	if (atk->classname == "monster_army")
	{
		atk->frags = atk->frags + numfrags;
		atk->real_frags = atk->real_frags;
	}
	else
	{
		if (howgive == 2)
			atk->frags = atk->frags + numfrags;
		else if (howgive == 1)
			atk->real_frags = atk->real_frags + numfrags;
		else
			bprint(PR_PRINT_HIGH, "Error in Give_Frags_Out!\n");

		#ifdef PR_QUAKE_WORLD
			if (fraggetlog)
			{
				logfrag (targ, atk);
			}
		#endif
		if (chkvamp)
		{
			if (teamplay & PR_TEAMPLAY_VAMPIRE)
			{
				targ->real_frags = targ->real_frags - numfrags;
				if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
					targ->frags = targ->real_frags;
			}
		}

		if (chkreal)
		{
		 	if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
		 	{
				atk->frags = atk->real_frags;
			}
		}
	}
}
//replaces det guns
//classname to find, entity checking, (0-1) check for ownership
void Find_And_Dmg(const string& search, entity person, float chkown, int type)
{
	entity te;

    te = find(world, "classname", search);

    if (type)
    {
        while (te != world)
        {
            if (chkown)
            {
                if (te->real_owner == person && type & te->dont_do_triggerwork)
                    TF_T_Damage(te, world, world, te->health+100, 0, 0);
            }
            else
            {
                if (type & te->dont_do_triggerwork)
                    TF_T_Damage(te, world, world, te->health+100, 0, 0);
            }
            te = find(te, "classname", search);
        }

        return;
    }

    while (te != world)
	{
		if (chkown)
		{
			if (te->real_owner == person)
				TF_T_Damage(te, world, world, te->health+100, 0, 0);
		}
		else
		{
			TF_T_Damage(te, world, world, te->health+100, 0, 0);
		}
		te = find(te, "classname", search);
	}
}

float isMelee()
{
	if (self->current_weapon == PR_WEAP_AXE || self->current_weapon == PR_WEAP_SPANNER || self->current_weapon == PR_WEAP_MEDIKIT)
		return PR_TRUE;
	else
		return PR_FALSE;
}

/*
==========================================================================
OfN - Optimize what?

---------------------------
Bit/Byte/Decimal reference:
---------------------------
1  1 1
2  2 2
3  3 4
4  4 8
5  5 16
6  6 32
7  7 64
8  8 128

9  1 256
10 2 512
11 3 1024
12 4 2048
13 5 4096
14 6 8192
15 7 16384
16 8 32768

17 1 65536
18 2 131072
19 3 262144
20 4 524288
21 5 1048576
22 6 2097152
23 7 4194304
24 8 8388608
---------------------------------
QuakeC floats are all 24-bit wide
So, we can use 3 bytes on them !!
---------------------------------

Functions to handle multiple integers (byte wide) on a single float:
==========================================================================
*/

//==========================================
// Retrieval of byte values

float GetByte1(int value)
{
    return value & 255;
}

float GetByte2(int value)
{
    float res;

    res = value & 65280;
    res = res / 256;

    return res;
}

float GetByte3(int value)
{
    float res;

    res = value & 16711680;
    res = res / 65536;

    return res;
}

//=======================================
// Transform values into different bytes

float MakeByte2(float value)
{
    value = floor(value);
    if (value < 0)
        value = 0;
    if (value > 255)
        value = 255;
    return value * 256;
}

float MakeByte3(float value)
{
    value = floor(value);
    if (value < 0)
        value = 0;
    if (value > 255)
        value = 255;
    return value * 65536;
}

//=====================================
// Assign value to different bytes

int AssignByte1(int targetval, float value)
{
    value = floor(value);
    if (value < 0)
        value = 0;
    if (value > 255)
        value = 255;
    targetval = targetval - (targetval & 255);
    targetval = targetval + value;

    return targetval;
}

int AssignByte2(int targetval, float value)
{
    targetval = targetval - (targetval & 65280);
    targetval = targetval + MakeByte2(value);

    return targetval;
}

int AssignByte3(int targetval, float value)
{
    targetval = targetval - (targetval & 16711680);
    targetval = targetval + MakeByte3(value);

    return targetval;
}

//===============================================================================
// Special bitfield handling for warlock's .demon_blood float
// bit usage is: 6 bits (hearts) -> 10 bits (meat) -> 8 bits (heads)
// so the 3rd value doesnt require special functions

int GetWarlock1(int value)
{
    return value & 63;
}

float GetWarlock2(int value)
{
    float res;

    res = value & 65472;
    res = res / 64;

    return res;
}

float MakeWarlock2(float value)
{
    value = floor(value);
    if (value < 0)
        value = 0;
    if (value > 1023)
        value = 1023;
    return value * 64;
}

int AssignWarlock1(int targetval, float value)
{
    value = floor(value);
    if (value < 0)
        value = 0;
    if (value > 63)
        value = 63;

    targetval = targetval - (targetval & 63);
    targetval = targetval + value;

    return targetval;
}

int AssignWarlock2(int targetval, float value)
{
    targetval = targetval - (targetval & 65472);
    targetval = targetval + MakeWarlock2(value);

    return targetval;
}

//==============================================================
// Returns the power of 2 corresponding to the given integer

int itob(int input)
{
    if (input == 1) return 1;
    if (input == 2) return 2;
    if (input == 3) return 4;
    if (input == 4) return 8;
    if (input == 5) return 16;
    if (input == 6) return 32;
    if (input == 7) return 64;
    if (input == 8) return 128;
    if (input == 9) return 256;
    if (input == 10) return 512;
    if (input == 11) return 1024;
    if (input == 12) return 2048;
    if (input == 13) return 4096;
    if (input == 14) return 8192;
    if (input == 15) return 16384;
    if (input == 16) return 32768;
    if (input == 17) return 65536;
    if (input == 18) return 131072;
    if (input == 19) return 262144;
    if (input == 20) return 524288;
    if (input == 21) return 1048576;
    if (input == 22) return 2097152;
    if (input == 23) return 4194304;
    if (input == 24) return 8388608;

    return 0;
}

} // END namespace Progs
