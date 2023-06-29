// PZ: This is the unity build file that replaces progs.src. All progs source files are included in this file,
// to force a sequential compilation of progs source files, in the same way they were being compiled by the
// QuakeC compiler. Doing it this way keeps me (and you) from having to deal with the nightmare of needing for-
// ward declarations and include statements all over the place. QuakeC did things this way, and so shall we,
// for simplicity in translation to C++.
// PZ NOTE: I EVENTUALLY DECIDED TO ABANDON UNITY BUILDS. SO THIS COMMENT NO LONGER APPLIES.

#define COMPILING_PROGS_CODE  // this is so that we can include engine files and have it only include certain things

#include <math.h>

#include "progdefs.h"    // for interfacing /out/ to the engine
#include "cpqw/server/pr_cmds.h"  // for interfacing /in/ from the engine  (for engine built-in functions)
//#include "cpqw/server/progs.h"  // for access to engine data structures
#include "cpqw/server/qwsvdef.h"  // for access to engine data structures (initially, this was to get progs.h)

enum {X, Y, Z}; // for vector elements

// This makes the compiler ignore field declarations.
#define FIELD(field_list)
//#define FIELD(field_list)([[field_list]])   // what it will eventually be for progdefsgen

// TO_STR() puts doublequotes around macro replacements. (Useful for using macros in string literals.)
#define STRINGIFY(x) #x
#define TO_STR(x)    STRINGIFY(x)

// This replaces model frame state QuakeC macros.
#define FRAME_STATE(frameNum, thinkFunc) \
{                                        \
	self->frame     = (frameNum);        \
	self->think     = (thinkFunc);       \
	self->nextthink = time + 0.1;        \
}

// The following operator overloads allow math on vectors in the same way allowed in QuakeC.
vector operator+(vector v1, const vector& v2);
void operator+=(vector& v1, const vector& v2);
vector operator-(vector v1, const vector& v2);
void operator-=(vector& v1, const vector& v2);
vector operator*(vector v, const double& d);         // scales `v` by `d`
vector operator*(const double& d, vector v);         // scales `v` by `d`
float operator*(const vector& va, const vector& vb); // dot product of `va` and `vb`

// This allows "if (!str)" and "if (!!str)". I don't recommend using it, since you can't use "if (str)".
// Instead, use "if (str == "")" or "if (str.empty())". But it's here if you want it.
bool operator!(const string& s);

// Built-in function name overrides to avoid conflict with standard library function names.
#define  random      PF_random
#define  stof        PF_stof
#define  strcasecmp  PF_strcasecmp
#define  fopen       PF_fopen
#define  fclose      PF_fclose
#define  fgets       PF_fgets
#define  fputs       PF_fputs

// PZ: function references, to give us aliases to the functions
// PZ: I switched these to defines, because I couldn't figure out how to forward declare a function reference.
/*namespace BuiltIns
{
	void (precache_file2) (const string& s);
	void (precache_sound2)(const string& s);
	void (precache_model2)(const string& s);
}*/
#define  precache_file2   precache_file
#define  precache_sound2  precache_sound
#define  precache_model2  precache_model

using namespace BuiltIns;

#include "defs.h"

/*namespace Progs
{
	#include "preopdef.cpp"
	#include "options.cpp"
	#include "messages.cpp"
	#include "skins.cpp"
	#include "defs.cpp"
	#include "tfdefs.cpp"
	#include "protos.cpp"
	#include "ofndefs.cpp"
	#include "prozac.cpp"
	#include "debug.cpp"
	#include "frikbot/waypoints/map_dm1.cpp"
	#include "frikbot/waypoints/map_dm2.cpp"
	#include "frikbot/waypoints/map_dm3.cpp"
	#include "frikbot/waypoints/map_dm4.cpp"
	#include "frikbot/waypoints/map_dm5.cpp"
	#include "frikbot/waypoints/map_dm6.cpp"
	#include "frikbot/waypoints/map_sq1.cpp"
	#include "frikbot/waypoints/map_2farms.cpp"
	#include "frikbot/waypoints/map_canyon1.cpp"
	#include "frikbot/waypoints/map_rock2.cpp"
	#include "frikbot/bot_qw.cpp"
	#include "frikbot/bot_way.cpp"
	#include "frikbot/bot_fight.cpp"
	#include "frikbot/bot_ai.cpp"
	#include "frikbot/bot_misc.cpp"
	#include "frikbot/bot_phys.cpp"
	#include "frikbot/bot_move.cpp"
	#include "frikbot/bot_ed.cpp"
	#include "qw.cpp"
	#include "coop_defs.cpp"
	#include "optimize.cpp"
	#include "sprites.cpp"
	#include "environ.cpp"
	#include "menu.cpp"
	#include "cutfmenu.cpp"
	#include "tinker.cpp"
	#include "custom.cpp"
	#include "sbitems.cpp"
	#include "jobs.cpp"
	#include "guerilla.cpp"
	#include "subs.cpp"
	#include "combat.cpp"
	#include "items.cpp"
	#include "coop_items.cpp"
	#include "admin.cpp"
	#include "gweapons.cpp"
	#include "weapons.cpp"
	#include "world.cpp"
	#include "status.cpp"
	#include "client.cpp"
	#include "obits.cpp"
	#include "player.cpp"
	#include "doors.cpp"
	#include "buttons.cpp"
	#include "triggers.cpp"
	#include "tforttm.cpp"
	#include "plats.cpp"
	#include "misc.cpp"
	#include "warlock.cpp"
	#include "army.cpp"
	#include "monsters.cpp"
// HALFWAY MARK
	#include "ai.cpp"
	#include "fight.cpp"
	#include "demon.cpp"
	#include "shambler.cpp"
	#include "grunty.cpp"
	#include "wizard.cpp"
	#include "fish.cpp"
	#include "gremlin.cpp"
	#include "haxxx.cpp"
	#include "sentry.cpp"
	#include "tesla.cpp"
	#include "field.cpp"
	#include "security.cpp"
	#include "teleport.cpp"
	#include "boss.cpp"
	#include "airfist.cpp"
	#include "pyro.cpp"
	#include "spy.cpp"
	#include "demoman.cpp"
	#include "scout.cpp"
	#include "engineer.cpp"
	#include "tsoldier.cpp"
	#include "sniper.cpp"
	#include "speed.cpp"
	#include "tfort.cpp"
	#include "tforthlp.cpp"
	#include "tfortmap.cpp"
	#include "hook.cpp"
	#include "ctf.cpp"
	#include "coop.cpp"
	#include "actions.cpp"
	#include "medic.cpp"
	#include "crusader.cpp"
	#include "cpstuff.cpp"
	#include "mtfents.cpp"
	#include "rotate.cpp"
	#include "spectate.cpp"
	#include "ofnents.cpp"
	#include "vote.cpp"
	#include "cmnd.cpp"
	#include "runes.cpp"
	#include "megatf.cpp"
	#include "agr.cpp"
	#include "invade.cpp"
	#include "idmonsters/coop_monsters.cpp"
	#include "idmonsters/common.cpp"
	#include "idmonsters/soldier.cpp"
	#include "idmonsters/dog.cpp"
	#include "idmonsters/zombie.cpp"
	#include "idmonsters/demon.cpp"
	#include "idmonsters/boss.cpp"
	#include "idmonsters/ogre.cpp"
	#include "idmonsters/knight.cpp"
	#include "idmonsters/wizard.cpp"
	#include "idmonsters/shambler.cpp"
	#include "idmonsters/enforcer.cpp"
	#include "idmonsters/fish.cpp"
	#include "idmonsters/tarbaby.cpp"
	#include "idmonsters/shalrath.cpp"
	#include "idmonsters/hknight.cpp"
	#include "idmonsters/oldone.cpp"
}*/

// This is here in case you want to try to run the progs code by itself, in a test harness,
// with perhaps an emulated server engine. Or, if you just want to compile all the progs code
// like a normal program, use this.
/*int main(int argc, char** argv)
{

	return 0;
}*/
