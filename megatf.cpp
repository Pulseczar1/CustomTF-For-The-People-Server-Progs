/*=======================================================//
// MegaTF.QC - CustomTF 3.2.OfN            - 21/1/2004 - //
// by Sergio Fuma�a Grunwaldt - OfteN [cp]               //
=========================================================//
 Code related to MegaTF mimetized behaviours is here.
 With the exception of map entities, that have their own
 file: 'mtfents.qc'
=========================================================*/

#include "progs.h"
#include "ofndefs.h"

namespace Progs {

#define PR_MEGATF_DEFAULT_MODE PR_MEGATF_MAPENTS | PR_MEGATF_SIGHTS

//==============================================================
// Conditionally precaches needed stuff for current megatf mode

void MegaTF_Precaches()
{
	/* WK Get rid of this, it's unused
	if (megatf & #MEGATF_STEPS)
	{
		precache_sound("player/step1l.wav");
		precache_sound("player/step1r.wav");
		precache_sound("player/step2l.wav");
		precache_sound("player/step2r.wav");
	}
	*/
	
	if (megatf & PR_MEGATF_SIGHTS)
#ifdef PR_ORIENTED_SIGHTS
		// PZ: This sprite has a certain flag set which allows you to orient the sprite in-game. We want to orient
		//     it with the surface (normal). I believe Aberrant/Agent told us about the flag.
		precache_model2("progs/colsight2.spr");
#else
		precache_model2("progs/colsight.spr"); // Original code before July 2022
#endif
	else // Standard sights
		precache_model2("progs/sight.spr");

	/*if (megatf & #MEGATF_DROPS)
	{

	}*/
}

//==============================================================
// Updates MegaTF flags global

void GetMegaTFflags()
{
    string st;

    st = infokey(world,"megatf");
    if (st == "")
        megatf = PR_MEGATF_DEFAULT_MODE;
    else
        megatf = stof(st);
}

//===============================================================
// Called only at map startup, initializes megatf stuff

void InitMegaTF()
{
    GetMegaTFflags();
    MegaTF_Precaches();
}

} // END namespace Progs
