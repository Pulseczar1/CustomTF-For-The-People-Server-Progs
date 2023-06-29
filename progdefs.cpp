// =============================================================================================================
// These arrays allow the engine code to access progs global variables, entity field variables, and functions,
// by name, chosen at runtime rather than compile time. This is achieved by mapping names to addresses or memory
// offsets. Only variables and functions in these arrays will be accessible in this manner.  -Pulseczar
// =============================================================================================================

#include "cpqw/server/qwsvdef.h"
#include "cpqw/server/pr_cmds.h"  // for interfacing /in/ from the engine  (for engine built-in functions)

// The following operator overloads allow math on vectors in the same way allowed in QuakeC.
vector operator+(vector v1, const vector& v2)
{
	v1[0] += v2[0];
	v1[1] += v2[1];
	v1[2] += v2[2];
	return v1;
}
void operator+=(vector& v1, const vector& v2)
{
	v1[0] += v2[0];
	v1[1] += v2[1];
	v1[2] += v2[2];
}

vector operator-(vector v1, const vector& v2)
{
	v1[0] -= v2[0];
	v1[1] -= v2[1];
	v1[2] -= v2[2];
	return v1;
}
void operator-=(vector& v1, const vector& v2)
{
	v1[0] -= v2[0];
	v1[1] -= v2[1];
	v1[2] -= v2[2];
}

vector operator*(vector v, const double& d)   // scales `v` by `d`
{
	v[0] *= d;
	v[1] *= d;
	v[2] *= d;
	return v;
}
void operator*=(vector& v, const double& d)   // scales `v` by `d`
{
	v[0] *= d;
	v[1] *= d;
	v[2] *= d;
}

vector operator*(const double& d, vector v)   // scales `v` by `d`
{
	v[0] *= d;
	v[1] *= d;
	v[2] *= d;
	return v;
}

float operator*(const vector& va, const vector& vb) // dot product of `va` and `vb`
{
	return (va[0] * vb[0]) + (va[1] * vb[1]) + (va[2] * vb[2]);
}

// This allows "if (!str)" and "if (!!str)". I don't recommend using it, since you can't use "if (str)".
// Instead, use "if (str == "")" or "if (str.empty())". But it's here if you want it.
bool operator!(const string& s)
{
	return s.empty();
}

// PZ NOTE: I couldn't get the following operator overloads to work.
/*bool operator bool(const string& s)    // for being able to check whether a string is empty with "if (string)"
{
	return !(s.empty());
}*/

/*bool operator()(const string& s)
{
	return !(s.empty());
}*/

/*bool operator!=(vector& v, const std::initializer_list<float> list)
{
	return (v != vector(list));
}*/

// This doesn't work, because C++ requires that an overload of the () operator resides in a class.
/*bool operator()(entity e)
{
	return e != Progs::world;
}*/

// PZ: function references, to give us aliases to the functions
// PZ: I switched these to defines in progs.h, because I couldn't figure out how to forward declare a function reference.
/*namespace BuiltIns
{
	void (&precache_file2) (const string& s) = precache_file;
	void (&precache_sound2)(const string& s) = precache_sound;
	void (&precache_model2)(const string& s) = precache_model;
}*/

#define INIT_ARRAY(arr, cnt) for (int i = 0; i < (cnt); i++) (arr)[i] = 0;

// Global variable ddef_t's
ddef_t pr_globaldefs[] =
{
	/*{ev_entity, "world", -1, &Progs::world},
	{ev_float,  "time",  -1, &Progs::time},
	{ev_entity, "self",  -1, &Progs::self},
	{ev_entity, "other", -1, &Progs::other},*/

	{ev_entity, "world", -1, &Progs::world},
	{ev_entity, "self", -1, &Progs::self},
	{ev_entity, "other", -1, &Progs::other},
	{ev_entity, "trace_ent", -1, &Progs::trace_ent},
	{ev_entity, "msg_entity", -1, &Progs::msg_entity},
	{ev_entity, "newmis", -1, &Progs::newmis},
	{ev_float,  "time", -1, &Progs::time},
	{ev_float,  "frametime", -1, &Progs::frametime},
	{ev_float,  "force_retouch", -1, &Progs::force_retouch},
	{ev_vector, "v_forward", -1, &Progs::v_forward},
	{ev_vector, "v_right", -1, &Progs::v_right},
	{ev_vector, "v_up", -1, &Progs::v_up},
	{ev_vector, "trace_endpos", -1, &Progs::trace_endpos},
	{ev_vector, "trace_plane_normal", -1, &Progs::trace_plane_normal},
	{ev_float,  "trace_allsolid", -1, &Progs::trace_allsolid},
	{ev_float,  "trace_startsolid", -1, &Progs::trace_startsolid},
	{ev_float,  "trace_fraction", -1, &Progs::trace_fraction},
	{ev_float,  "trace_inwater", -1, &Progs::trace_inwater},
	{ev_float,  "trace_inopen", -1, &Progs::trace_inopen},
	{ev_float,  "trace_plane_dist", -1, &Progs::trace_plane_dist},
	{ev_float,  "total_secrets", -1, &Progs::total_secrets},
	{ev_float,  "found_secrets", -1, &Progs::found_secrets},
	{ev_float,  "total_monsters", -1, &Progs::total_monsters},
	{ev_float,  "killed_monsters", -1, &Progs::killed_monsters},
	{ev_float,  "parm1", -1, &Progs::parm1},
	{ev_float,  "parm2", -1, &Progs::parm2},
	{ev_float,  "parm3", -1, &Progs::parm3},
	{ev_float,  "parm4", -1, &Progs::parm4},
	{ev_float,  "parm5", -1, &Progs::parm5},
	{ev_float,  "parm6", -1, &Progs::parm6},
	{ev_float,  "parm7", -1, &Progs::parm7},
	{ev_float,  "parm8", -1, &Progs::parm8},
	{ev_float,  "parm9", -1, &Progs::parm9},
	{ev_float,  "parm10", -1, &Progs::parm10},
	{ev_float,  "parm11", -1, &Progs::parm11},
	{ev_float,  "parm12", -1, &Progs::parm12},
	{ev_float,  "parm13", -1, &Progs::parm13},
	{ev_float,  "parm14", -1, &Progs::parm14},
	{ev_float,  "parm15", -1, &Progs::parm15},
	{ev_float,  "parm16", -1, &Progs::parm16},
	{ev_int,    "serverflags", -1, &Progs::serverflags},
	{ev_string, "mapname", -1, &Progs::mapname},
	// these came from files outside of defs.cpp
	{ev_entity, "activator", -1, &Progs::activator},
	{ev_entity, "damage_attacker", -1, &Progs::damage_attacker},
	//{ev_entity, "damage_inflictor", -1, &Progs::damage_inflictor},  // PZ: Apparently this vanilla progs global was removed from CuTF progs (or renamed).
	{ev_entity, "stemp", -1, &Progs::stemp},
	{ev_entity, "otemp", -1, &Progs::otemp},
	{ev_entity, "s", -1, &Progs::s},
	{ev_entity, "old", -1, &Progs::old},
	{ev_entity, "multi_ent", -1, &Progs::multi_ent},
	{ev_entity, "lastspawn", -1, &Progs::lastspawn},
	{ev_entity, "bodyque_head", -1, &Progs::bodyque_head},
	{ev_vector, "blood_org", -1, &Progs::blood_org},
	{ev_vector, "puff_org", -1, &Progs::puff_org},
	{ev_string, "string_null", -1, &Progs::string_null},
	{ev_string, "nextmap", -1, &Progs::nextmap},
	{ev_float,  "intermission_running", -1, &Progs::intermission_running},
	{ev_float,  "intermission_exittime", -1, &Progs::intermission_exittime},
	{ev_float,  "modelindex_eyes", -1, &Progs::modelindex_eyes},
	{ev_float,  "modelindex_player", -1, &Progs::modelindex_player},
	{ev_float,  "deathmatch", -1, &Progs::deathmatch},
	{ev_float,  "empty_float", -1, &Progs::empty_float},
	{ev_float,  "fraglimit", -1, &Progs::fraglimit},
	{ev_float,  "framecount", -1, &Progs::framecount},
	{ev_float,  "movedist", -1, &Progs::movedist},
	{ev_float,  "coop", -1, &Progs::coop},
	{ev_float,  "rj", -1, &Progs::rj},
	{ev_float,  "teamplay", -1, &Progs::teamplay},
	{ev_float,  "timelimit", -1, &Progs::timelimit},
	{ev_float,  "blood_count", -1, &Progs::blood_count},
	{ev_float,  "multi_damage", -1, &Progs::multi_damage},
	{ev_float,  "puff_count", -1, &Progs::puff_count},

	// REQUIRED: marks end of list
	{ev_null,   "",      -1, NULL}
}; // END pr_globaldefs

// Field variable ddef_t's
ddef_t pr_fielddefs[] =
{
	//{ev_float,  "health", (char*)&(entvars_t::health) - (char*)&entvars_t, 0}
	//{ev_float,  "health", (char*)&Progs::world->health - (char*)&Progs::world, 0}      // <- this one should work
//	{ev_float,  "health", offsetof(entvars_t, health), NULL},

	/*{ev_vector, "angles", offsetof(entvars_t, angles), NULL},
	{ev_vector, "v_angle", offsetof(entvars_t, v_angle), NULL},
	{ev_vector, "view_ofs", offsetof(entvars_t, view_ofs), NULL},
	{ev_vector, "mangle", offsetof(entvars_t, mangle), NULL},
	{ev_vector, "finalangle", offsetof(entvars_t, finalangle), NULL},
	{ev_vector, "movedir", offsetof(entvars_t, movedir), NULL},
	{ev_vector, "origin", offsetof(entvars_t, origin), NULL},
	{ev_vector, "oldorigin", offsetof(entvars_t, oldorigin), NULL},
	{ev_vector, "velocity", offsetof(entvars_t, velocity), NULL},
	{ev_vector, "avelocity", offsetof(entvars_t, avelocity), NULL},
	{ev_vector, "finaldest", offsetof(entvars_t, finaldest), NULL},
	{ev_vector, "dest", offsetof(entvars_t, dest), NULL},
	{ev_vector, "dest1", offsetof(entvars_t, dest1), NULL},
	{ev_vector, "dest2", offsetof(entvars_t, dest2), NULL},
	{ev_vector, "size", offsetof(entvars_t, size), NULL},
	{ev_vector, "mins", offsetof(entvars_t, mins), NULL},
	{ev_vector, "maxs", offsetof(entvars_t, maxs), NULL},
	{ev_vector, "absmin", offsetof(entvars_t, absmin), NULL},
	{ev_vector, "absmax", offsetof(entvars_t, absmax), NULL},
	{ev_vector, "pos1", offsetof(entvars_t, pos1), NULL},
	{ev_vector, "pos2", offsetof(entvars_t, pos2), NULL},
	{ev_string, "model", offsetof(entvars_t, model), NULL},
	{ev_string, "mdl", offsetof(entvars_t, mdl), NULL},
	{ev_string, "map", offsetof(entvars_t, map), NULL},
	{ev_string, "classname", offsetof(entvars_t, classname), NULL},
	{ev_string, "netname", offsetof(entvars_t, netname), NULL},
	{ev_string, "message", offsetof(entvars_t, message), NULL},
	{ev_string, "targetname", offsetof(entvars_t, targetname), NULL},
	{ev_string, "deathtype", offsetof(entvars_t, deathtype), NULL},
	{ev_string, "noise", offsetof(entvars_t, noise), NULL},
	{ev_string, "noise1", offsetof(entvars_t, noise1), NULL},
	{ev_string, "noise2", offsetof(entvars_t, noise2), NULL},
	{ev_string, "noise3", offsetof(entvars_t, noise3), NULL},
	{ev_string, "noise4", offsetof(entvars_t, noise4), NULL},
	{ev_string, "target", offsetof(entvars_t, target), NULL},
	{ev_string, "killtarget", offsetof(entvars_t, killtarget), NULL},
	{ev_string, "weaponmodel", offsetof(entvars_t, weaponmodel), NULL},
	{ev_string, "wad", offsetof(entvars_t, wad), NULL},
	{ev_int, "button0", offsetof(entvars_t, button0), NULL},
	{ev_int, "button1", offsetof(entvars_t, button1), NULL},
	{ev_int, "button2", offsetof(entvars_t, button2), NULL},
	{ev_int, "items", offsetof(entvars_t, items), NULL},
	{ev_int, "flags", offsetof(entvars_t, flags), NULL},
	{ev_int, "movetype", offsetof(entvars_t, movetype), NULL},
	{ev_int, "spawnflags", offsetof(entvars_t, spawnflags), NULL},
	{ev_int, "skin", offsetof(entvars_t, skin), NULL},
	{ev_int, "weapon", offsetof(entvars_t, weapon), NULL},
	{ev_int, "effects", offsetof(entvars_t, effects), NULL},
	{ev_int, "deadflag", offsetof(entvars_t, deadflag), NULL},
	{ev_int, "watertype", offsetof(entvars_t, watertype), NULL},
	{ev_float, "walkframe", offsetof(entvars_t, walkframe), NULL},
	{ev_int, "bubble_count", offsetof(entvars_t, bubble_count), NULL},
	{ev_float, "aflag", offsetof(entvars_t, aflag), NULL},
	{ev_float, "impulse", offsetof(entvars_t, impulse), NULL},
	{ev_float, "worldtype", offsetof(entvars_t, worldtype), NULL},
	{ev_float, "axhitme", offsetof(entvars_t, axhitme), NULL},
	{ev_float, "weaponframe", offsetof(entvars_t, weaponframe), NULL},
	{ev_float, "show_hostile", offsetof(entvars_t, show_hostile), NULL},
	{ev_float, "dmg_take", offsetof(entvars_t, dmg_take), NULL},
	{ev_float, "dmg_save", offsetof(entvars_t, dmg_save), NULL},
	{ev_float, "lip", offsetof(entvars_t, lip), NULL},
	{ev_float, "voided", offsetof(entvars_t, voided), NULL},
	{ev_float, "colormap", offsetof(entvars_t, colormap), NULL},
	{ev_float, "frags", offsetof(entvars_t, frags), NULL},
	{ev_float, "swim_flag", offsetof(entvars_t, swim_flag), NULL},
	{ev_float, "air_finished", offsetof(entvars_t, air_finished), NULL},
	{ev_float, "attack_finished", offsetof(entvars_t, attack_finished), NULL},
	{ev_float, "pain_finished", offsetof(entvars_t, pain_finished), NULL},
	{ev_float, "super_sound", offsetof(entvars_t, super_sound), NULL},
	{ev_float, "super_time", offsetof(entvars_t, super_time), NULL},
	{ev_float, "super_damage_finished", offsetof(entvars_t, super_damage_finished), NULL},
	{ev_float, "invisible_time", offsetof(entvars_t, invisible_time), NULL},
	{ev_float, "invisible_finished", offsetof(entvars_t, invisible_finished), NULL},
	{ev_float, "invincible_time", offsetof(entvars_t, invincible_time), NULL},
	{ev_float, "invincible_finished", offsetof(entvars_t, invincible_finished), NULL},
	{ev_float, "rad_time", offsetof(entvars_t, rad_time), NULL},
	{ev_float, "dmgtime", offsetof(entvars_t, dmgtime), NULL},
	{ev_float, "radsuit_finished", offsetof(entvars_t, radsuit_finished), NULL},
	{ev_float, "currentammo", offsetof(entvars_t, currentammo), NULL},
	{ev_float, "armortype", offsetof(entvars_t, armortype), NULL},
	{ev_float, "armorvalue", offsetof(entvars_t, armorvalue), NULL},
	{ev_float, "takedamage", offsetof(entvars_t, takedamage), NULL},
	{ev_float, "modelindex", offsetof(entvars_t, modelindex), NULL},
	{ev_float, "health", offsetof(entvars_t, health), NULL},
	{ev_float, "healamount", offsetof(entvars_t, healamount), NULL},
	{ev_float, "healtype", offsetof(entvars_t, healtype), NULL},
	{ev_float, "ammo_cells", offsetof(entvars_t, ammo_cells), NULL},
	{ev_float, "ammo_shells", offsetof(entvars_t, ammo_shells), NULL},
	{ev_float, "ammo_rockets", offsetof(entvars_t, ammo_rockets), NULL},
	{ev_float, "ammo_nails", offsetof(entvars_t, ammo_nails), NULL},
	{ev_float, "solid", offsetof(entvars_t, solid), NULL},
	{ev_float, "height", offsetof(entvars_t, height), NULL},
	{ev_float, "nextthink", offsetof(entvars_t, nextthink), NULL},
	{ev_float, "t_width", offsetof(entvars_t, t_width), NULL},
	{ev_float, "t_length", offsetof(entvars_t, t_length), NULL},
	{ev_float, "ltime", offsetof(entvars_t, ltime), NULL},
	{ev_float, "delay", offsetof(entvars_t, delay), NULL},
	{ev_float, "pausetime", offsetof(entvars_t, pausetime), NULL},
	{ev_float, "ideal_yaw", offsetof(entvars_t, ideal_yaw), NULL},
	{ev_float, "fly_sound", offsetof(entvars_t, fly_sound), NULL},
	{ev_float, "fixangle", offsetof(entvars_t, fixangle), NULL},
	{ev_float, "prev_velocity_z", offsetof(entvars_t, prev_velocity_z), NULL},
	{ev_float, "frame", offsetof(entvars_t, frame), NULL},
	{ev_float, "cnt", offsetof(entvars_t, cnt), NULL},
	{ev_float, "teleport_time", offsetof(entvars_t, teleport_time), NULL},
	{ev_float, "count", offsetof(entvars_t, count), NULL},
	{ev_float, "max_health", offsetof(entvars_t, max_health), NULL},
	{ev_float, "invisible_sound", offsetof(entvars_t, invisible_sound), NULL},
	{ev_float, "invincible_sound", offsetof(entvars_t, invincible_sound), NULL},
	{ev_float, "waterlevel", offsetof(entvars_t, waterlevel), NULL},
	{ev_float, "wait", offsetof(entvars_t, wait), NULL},
	{ev_float, "speed", offsetof(entvars_t, speed), NULL},
	{ev_float, "sounds", offsetof(entvars_t, sounds), NULL},
	{ev_float, "dmg", offsetof(entvars_t, dmg), NULL},
	{ev_float, "style", offsetof(entvars_t, style), NULL},
	{ev_float, "state", offsetof(entvars_t, state), NULL},
	{ev_float, "team", offsetof(entvars_t, team), NULL},
	{ev_float, "yaw_speed", offsetof(entvars_t, yaw_speed), NULL},
	{ev_float, "lastruntime", offsetof(entvars_t, lastruntime), NULL},
	{ev_float, "ishuman", offsetof(entvars_t, ishuman), NULL},            // Not used in vanilla QuakeWorld, but easier to add here than it is to remove all bot code, essentially.
	{ev_float, "admin_flag", offsetof(entvars_t, admin_flag), NULL},         // Not used in vanilla QuakeWorld, but easier to just add here than it is to undo all engine code that uses it. (TODO. Don't require progs to have this field.)
	{ev_float, "current_menu", offsetof(entvars_t, current_menu), NULL},       // Not used in vanilla QuakeWorld, but easier to just add here than it is to undo all engine code that uses it. (TODO. Don't require progs to have this field.)
	{ev_float, "light_lev", offsetof(entvars_t, light_lev), NULL},
	{ev_entity, "dmg_inflictor", offsetof(entvars_t, dmg_inflictor), NULL},
	{ev_entity, "owner", offsetof(entvars_t, owner), NULL},
	{ev_entity, "enemy", offsetof(entvars_t, enemy), NULL},
	{ev_entity, "movetarget", offsetof(entvars_t, movetarget), NULL},
	{ev_entity, "goalentity", offsetof(entvars_t, goalentity), NULL},
	{ev_entity, "trigger_field", offsetof(entvars_t, trigger_field), NULL},
	{ev_entity, "chain", offsetof(entvars_t, chain), NULL},
	{ev_entity, "groundentity", offsetof(entvars_t, groundentity), NULL},
	{ev_function, "think", offsetof(entvars_t, think), NULL},
	{ev_function, "think1", offsetof(entvars_t, think1), NULL},
	{ev_function, "th_stand", offsetof(entvars_t, th_stand), NULL},
	{ev_function, "touch", offsetof(entvars_t, touch), NULL},
	{ev_function, "use", offsetof(entvars_t, use), NULL},
	{ev_function, "th_die", offsetof(entvars_t, th_die), NULL},
	{ev_function, "th_pain", offsetof(entvars_t, th_pain), NULL},
	{ev_function, "blocked", offsetof(entvars_t, blocked), NULL},*/



	{ev_vector, "angles", offsetof(entvars_t, angles), NULL},
	{ev_vector, "v_angle", offsetof(entvars_t, v_angle), NULL},
	{ev_vector, "view_ofs", offsetof(entvars_t, view_ofs), NULL},
	{ev_vector, "mangle", offsetof(entvars_t, mangle), NULL},
	{ev_vector, "finalangle", offsetof(entvars_t, finalangle), NULL},
	{ev_vector, "movedir", offsetof(entvars_t, movedir), NULL},
	{ev_vector, "origin", offsetof(entvars_t, origin), NULL},
	{ev_vector, "oldorigin", offsetof(entvars_t, oldorigin), NULL},
	{ev_vector, "velocity", offsetof(entvars_t, velocity), NULL},
	{ev_vector, "avelocity", offsetof(entvars_t, avelocity), NULL},
	{ev_vector, "finaldest", offsetof(entvars_t, finaldest), NULL},
	{ev_vector, "dest", offsetof(entvars_t, dest), NULL},
	{ev_vector, "dest1", offsetof(entvars_t, dest1), NULL},
	{ev_vector, "dest2", offsetof(entvars_t, dest2), NULL},
	{ev_vector, "size", offsetof(entvars_t, size), NULL},
	{ev_vector, "mins", offsetof(entvars_t, mins), NULL},
	{ev_vector, "maxs", offsetof(entvars_t, maxs), NULL},
	{ev_vector, "absmin", offsetof(entvars_t, absmin), NULL},
	{ev_vector, "absmax", offsetof(entvars_t, absmax), NULL},
	{ev_vector, "pos1", offsetof(entvars_t, pos1), NULL},
	{ev_vector, "pos2", offsetof(entvars_t, pos2), NULL},
	{ev_string, "model", offsetof(entvars_t, model), NULL},
	{ev_string, "mdl", offsetof(entvars_t, mdl), NULL},
	{ev_string, "map", offsetof(entvars_t, map), NULL},
	{ev_string, "classname", offsetof(entvars_t, classname), NULL},
	{ev_string, "netname", offsetof(entvars_t, netname), NULL},
	{ev_string, "message", offsetof(entvars_t, message), NULL},
	{ev_string, "targetname", offsetof(entvars_t, targetname), NULL},
	{ev_string, "deathtype", offsetof(entvars_t, deathtype), NULL},
	{ev_string, "noise", offsetof(entvars_t, noise), NULL},
	{ev_string, "noise1", offsetof(entvars_t, noise1), NULL},
	{ev_string, "noise2", offsetof(entvars_t, noise2), NULL},
	{ev_string, "noise3", offsetof(entvars_t, noise3), NULL},
	{ev_string, "noise4", offsetof(entvars_t, noise4), NULL},
	{ev_string, "target", offsetof(entvars_t, target), NULL},
	{ev_string, "killtarget", offsetof(entvars_t, killtarget), NULL},
	{ev_string, "weaponmodel", offsetof(entvars_t, weaponmodel), NULL},
	{ev_string, "wad", offsetof(entvars_t, wad), NULL},
	{ev_int, "button0", offsetof(entvars_t, button0), NULL},
	{ev_int, "button1", offsetof(entvars_t, button1), NULL},
	{ev_int, "button2", offsetof(entvars_t, button2), NULL},
	{ev_int, "items", offsetof(entvars_t, items), NULL},
	{ev_int, "flags", offsetof(entvars_t, flags), NULL},
	{ev_int, "movetype", offsetof(entvars_t, movetype), NULL},
	{ev_int, "spawnflags", offsetof(entvars_t, spawnflags), NULL},
	{ev_int, "skin", offsetof(entvars_t, skin), NULL},
	{ev_int, "weapon", offsetof(entvars_t, weapon), NULL},
	{ev_int, "effects", offsetof(entvars_t, effects), NULL},
	{ev_int, "deadflag", offsetof(entvars_t, deadflag), NULL},
	{ev_int, "watertype", offsetof(entvars_t, watertype), NULL},
	{ev_float, "walkframe", offsetof(entvars_t, walkframe), NULL},
	{ev_int, "bubble_count", offsetof(entvars_t, bubble_count), NULL},
	{ev_int, "aflag", offsetof(entvars_t, aflag), NULL},
	{ev_float, "impulse", offsetof(entvars_t, impulse), NULL},
	{ev_float, "worldtype", offsetof(entvars_t, worldtype), NULL},
	{ev_float, "axhitme", offsetof(entvars_t, axhitme), NULL},
	{ev_float, "weaponframe", offsetof(entvars_t, weaponframe), NULL},
	{ev_float, "show_hostile", offsetof(entvars_t, show_hostile), NULL},
	{ev_float, "dmg_take", offsetof(entvars_t, dmg_take), NULL},
	{ev_float, "dmg_save", offsetof(entvars_t, dmg_save), NULL},
	{ev_float, "lip", offsetof(entvars_t, lip), NULL},
	{ev_float, "voided", offsetof(entvars_t, voided), NULL},
	{ev_float, "colormap", offsetof(entvars_t, colormap), NULL},
	{ev_float, "frags", offsetof(entvars_t, frags), NULL},
	{ev_float, "swim_flag", offsetof(entvars_t, swim_flag), NULL},
	{ev_float, "air_finished", offsetof(entvars_t, air_finished), NULL},
	{ev_float, "attack_finished", offsetof(entvars_t, attack_finished), NULL},
	{ev_float, "pain_finished", offsetof(entvars_t, pain_finished), NULL},
	{ev_float, "super_sound", offsetof(entvars_t, super_sound), NULL},
	{ev_float, "super_time", offsetof(entvars_t, super_time), NULL},
	{ev_float, "super_damage_finished", offsetof(entvars_t, super_damage_finished), NULL},
	{ev_float, "invisible_time", offsetof(entvars_t, invisible_time), NULL},
	{ev_float, "invisible_finished", offsetof(entvars_t, invisible_finished), NULL},
	{ev_float, "invincible_time", offsetof(entvars_t, invincible_time), NULL},
	{ev_float, "invincible_finished", offsetof(entvars_t, invincible_finished), NULL},
	{ev_float, "rad_time", offsetof(entvars_t, rad_time), NULL},
	{ev_float, "dmgtime", offsetof(entvars_t, dmgtime), NULL},
	{ev_float, "radsuit_finished", offsetof(entvars_t, radsuit_finished), NULL},
	{ev_float, "currentammo", offsetof(entvars_t, currentammo), NULL},
	{ev_float, "armortype", offsetof(entvars_t, armortype), NULL},
	{ev_float, "armorvalue", offsetof(entvars_t, armorvalue), NULL},
	{ev_float, "takedamage", offsetof(entvars_t, takedamage), NULL},
	{ev_float, "modelindex", offsetof(entvars_t, modelindex), NULL},
	{ev_float, "health", offsetof(entvars_t, health), NULL},
	{ev_float, "healamount", offsetof(entvars_t, healamount), NULL},
	{ev_float, "healtype", offsetof(entvars_t, healtype), NULL},
	{ev_float, "ammo_cells", offsetof(entvars_t, ammo_cells), NULL},
	{ev_float, "ammo_shells", offsetof(entvars_t, ammo_shells), NULL},
	{ev_float, "ammo_rockets", offsetof(entvars_t, ammo_rockets), NULL},
	{ev_float, "ammo_nails", offsetof(entvars_t, ammo_nails), NULL},
	{ev_float, "solid", offsetof(entvars_t, solid), NULL},
	{ev_float, "height", offsetof(entvars_t, height), NULL},
	{ev_float, "nextthink", offsetof(entvars_t, nextthink), NULL},
	{ev_float, "t_width", offsetof(entvars_t, t_width), NULL},
	{ev_float, "t_length", offsetof(entvars_t, t_length), NULL},
	{ev_float, "ltime", offsetof(entvars_t, ltime), NULL},
	{ev_float, "delay", offsetof(entvars_t, delay), NULL},
	{ev_float, "pausetime", offsetof(entvars_t, pausetime), NULL},
	{ev_float, "ideal_yaw", offsetof(entvars_t, ideal_yaw), NULL},
	{ev_float, "fly_sound", offsetof(entvars_t, fly_sound), NULL},
	{ev_float, "fixangle", offsetof(entvars_t, fixangle), NULL},
	{ev_float, "timeLastJumped", offsetof(entvars_t, timeLastJumped), NULL},
	{ev_float, "frame", offsetof(entvars_t, frame), NULL},
	{ev_float, "cnt", offsetof(entvars_t, cnt), NULL},
	{ev_float, "teleport_time", offsetof(entvars_t, teleport_time), NULL},
	{ev_float, "count", offsetof(entvars_t, count), NULL},
	{ev_float, "max_health", offsetof(entvars_t, max_health), NULL},
	{ev_float, "invisible_sound", offsetof(entvars_t, invisible_sound), NULL},
	{ev_float, "invincible_sound", offsetof(entvars_t, invincible_sound), NULL},
	{ev_float, "waterlevel", offsetof(entvars_t, waterlevel), NULL},
	{ev_float, "wait", offsetof(entvars_t, wait), NULL},
	{ev_float, "speed", offsetof(entvars_t, speed), NULL},
	{ev_float, "sounds", offsetof(entvars_t, sounds), NULL},
	{ev_float, "dmg", offsetof(entvars_t, dmg), NULL},
	{ev_float, "style", offsetof(entvars_t, style), NULL},
	{ev_float, "state", offsetof(entvars_t, state), NULL},
	{ev_float, "team", offsetof(entvars_t, team), NULL},
	{ev_float, "yaw_speed", offsetof(entvars_t, yaw_speed), NULL},
	{ev_entity, "aiment", offsetof(entvars_t, aiment), NULL},
	{ev_float, "lastruntime", offsetof(entvars_t, lastruntime), NULL},
	{ev_float, "ishuman", offsetof(entvars_t, ishuman), NULL},
	{ev_float, "admin_flag", offsetof(entvars_t, admin_flag), NULL},
	{ev_float, "current_menu", offsetof(entvars_t, current_menu), NULL},
	{ev_float, "light_lev", offsetof(entvars_t, light_lev), NULL},
	{ev_entity, "dmg_inflictor", offsetof(entvars_t, dmg_inflictor), NULL},
	{ev_entity, "owner", offsetof(entvars_t, owner), NULL},
	{ev_entity, "enemy", offsetof(entvars_t, enemy), NULL},
	{ev_entity, "movetarget", offsetof(entvars_t, movetarget), NULL},
	{ev_entity, "goalentity", offsetof(entvars_t, goalentity), NULL},
	{ev_entity, "trigger_field", offsetof(entvars_t, trigger_field), NULL},
	{ev_entity, "chain", offsetof(entvars_t, chain), NULL},
	{ev_entity, "groundentity", offsetof(entvars_t, groundentity), NULL},
	{ev_function, "think", offsetof(entvars_t, think), NULL},
	{ev_function, "think1", offsetof(entvars_t, think1), NULL},
	{ev_function, "th_stand", offsetof(entvars_t, th_stand), NULL},
	{ev_function, "touch", offsetof(entvars_t, touch), NULL},
	{ev_function, "use", offsetof(entvars_t, use), NULL},
	{ev_function, "th_die", offsetof(entvars_t, th_die), NULL},
	{ev_function, "th_walk", offsetof(entvars_t, th_walk), NULL},
	{ev_function, "th_run", offsetof(entvars_t, th_run), NULL},
	{ev_function, "th_missile", offsetof(entvars_t, th_missile), NULL},
	{ev_function, "th_melee", offsetof(entvars_t, th_melee), NULL},
	{ev_function, "th_pain", offsetof(entvars_t, th_pain), NULL},
	{ev_function, "blocked", offsetof(entvars_t, blocked), NULL},
	/*{ev_int, "b_aiflags", offsetof(entvars_t, b_aiflags), NULL},
	{ev_int, "team_no", offsetof(entvars_t, team_no), NULL},
	{ev_int, "playerclass", offsetof(entvars_t, playerclass), NULL},
	{ev_int, "tf_items", offsetof(entvars_t, tf_items), NULL},
	{ev_int, "cutf_items", offsetof(entvars_t, cutf_items), NULL},
	{ev_int, "weapons_carried", offsetof(entvars_t, weapons_carried), NULL},
	{ev_int, "job", offsetof(entvars_t, job), NULL},
	{ev_int, "undercover_skin", offsetof(entvars_t, undercover_skin), NULL},
	{ev_float, "custom_speed", offsetof(entvars_t, custom_speed), NULL},
	{ev_entity, "_next", offsetof(entvars_t, _next), NULL},
	{ev_entity, "target1", offsetof(entvars_t, target1), NULL},
	{ev_entity, "target2", offsetof(entvars_t, target2), NULL},
	{ev_entity, "target3", offsetof(entvars_t, target3), NULL},
	{ev_entity, "target4", offsetof(entvars_t, target4), NULL},*/
	{ev_entity, "oldenemy", offsetof(entvars_t, oldenemy), NULL},
	{ev_float, "lefty", offsetof(entvars_t, lefty), NULL},
	{ev_float, "search_time", offsetof(entvars_t, search_time), NULL},
	{ev_float, "attack_state", offsetof(entvars_t, attack_state), NULL},
	{ev_float, "maxspeed", offsetof(entvars_t, maxspeed), NULL},
	{ev_float, "gravity", offsetof(entvars_t, gravity), NULL},
	{ev_float, "money", offsetof(entvars_t, money), NULL},
	{ev_float, "custom_speed", offsetof(entvars_t, custom_speed), NULL},
	{ev_float, "ff_count", offsetof(entvars_t, ff_count), NULL},
	{ev_float, "penance_time", offsetof(entvars_t, penance_time), NULL},
	{ev_float, "last_attacked_time", offsetof(entvars_t, last_attacked_time), NULL},
	{ev_entity, "inspirator", offsetof(entvars_t, inspirator), NULL},
	{ev_float, "has_cheated", offsetof(entvars_t, has_cheated), NULL},
	{ev_float, "demon_blood", offsetof(entvars_t, demon_blood), NULL},
	{ev_entity, "demon_one", offsetof(entvars_t, demon_one), NULL},
	{ev_entity, "demon_two", offsetof(entvars_t, demon_two), NULL},
	{ev_entity, "demon_three", offsetof(entvars_t, demon_three), NULL},
	{ev_function, "th_fireball", offsetof(entvars_t, th_fireball), NULL},
	{ev_float, "hover_time", offsetof(entvars_t, hover_time), NULL},
	{ev_int, "done_custom", offsetof(entvars_t, done_custom), NULL},
	{ev_entity, "scaned", offsetof(entvars_t, scaned), NULL},
	{ev_float, "prev_velocity_z", offsetof(entvars_t, prev_velocity_z), NULL},
	{ev_float, "waitmin", offsetof(entvars_t, waitmin), NULL},
	{ev_float, "waitmax", offsetof(entvars_t, waitmax), NULL},
	{ev_float, "distance", offsetof(entvars_t, distance), NULL},
	{ev_float, "volume", offsetof(entvars_t, volume), NULL},
	{ev_float, "playerclass", offsetof(entvars_t, playerclass), NULL},
	{ev_float, "nextpc", offsetof(entvars_t, nextpc), NULL},
	{ev_float, "last_impulse", offsetof(entvars_t, last_impulse), NULL},
	{ev_int, "armorclass", offsetof(entvars_t, armorclass), NULL},
	{ev_int, "tf_items", offsetof(entvars_t, tf_items), NULL},
	{ev_int, "job", offsetof(entvars_t, job), NULL},
	{ev_float, "job_finished", offsetof(entvars_t, job_finished), NULL},
	{ev_int, "tf_items_flags", offsetof(entvars_t, tf_items_flags), NULL},
	{ev_float, "no_grenades_1", offsetof(entvars_t, no_grenades_1), NULL},
	{ev_float, "no_grenades_2", offsetof(entvars_t, no_grenades_2), NULL},
	{ev_float, "tp_grenades_1", offsetof(entvars_t, tp_grenades_1), NULL},
	{ev_float, "tp_grenades_2", offsetof(entvars_t, tp_grenades_2), NULL},
	{ev_float, "got_aliases", offsetof(entvars_t, got_aliases), NULL},
	{ev_float, "cheat_check", offsetof(entvars_t, cheat_check), NULL},
	{ev_float, "is_removed", offsetof(entvars_t, is_removed), NULL},
	{ev_float, "is_undercover", offsetof(entvars_t, is_undercover), NULL},
	{ev_float, "is_building", offsetof(entvars_t, is_building), NULL},
	{ev_float, "is_detpacking", offsetof(entvars_t, is_detpacking), NULL},
	{ev_float, "is_feigning", offsetof(entvars_t, is_feigning), NULL},
	{ev_float, "is_haxxxoring", offsetof(entvars_t, is_haxxxoring), NULL},
	{ev_float, "is_toffingadet", offsetof(entvars_t, is_toffingadet), NULL},
	{ev_float, "is_unabletospy", offsetof(entvars_t, is_unabletospy), NULL},
	{ev_int, "is_malfunctioning", offsetof(entvars_t, is_malfunctioning), NULL},
	{ev_float, "is_abouttodie", offsetof(entvars_t, is_abouttodie), NULL},
	{ev_float, "is_killed", offsetof(entvars_t, is_killed), NULL},
	{ev_float, "option", offsetof(entvars_t, option), NULL},
	{ev_float, "option2", offsetof(entvars_t, option2), NULL},
	{ev_float, "is_connected", offsetof(entvars_t, is_connected), NULL},
	{ev_float, "has_dispenser", offsetof(entvars_t, has_dispenser), NULL},
	{ev_int, "has_sentry", offsetof(entvars_t, has_sentry), NULL},
	{ev_int, "has_tesla", offsetof(entvars_t, has_tesla), NULL},
	{ev_int, "has_camera", offsetof(entvars_t, has_camera), NULL},
	{ev_float, "has_sensor", offsetof(entvars_t, has_sensor), NULL},
	{ev_float, "has_teleporter", offsetof(entvars_t, has_teleporter), NULL},
	{ev_float, "has_fieldgen", offsetof(entvars_t, has_fieldgen), NULL},
	{ev_float, "admin_flag", offsetof(entvars_t, admin_flag), NULL},
	{ev_entity, "admin_kick", offsetof(entvars_t, admin_kick), NULL},
	{ev_entity, "martyr_enemy", offsetof(entvars_t, martyr_enemy), NULL},
	{ev_float, "stored_deathmsg", offsetof(entvars_t, stored_deathmsg), NULL},
	{ev_float, "cluster_mode", offsetof(entvars_t, cluster_mode), NULL},
	{ev_int, "has_holo", offsetof(entvars_t, has_holo), NULL},
	{ev_vector, "neworigin", offsetof(entvars_t, neworigin), NULL},
	{ev_vector, "rotate", offsetof(entvars_t, rotate), NULL},
	{ev_float, "endtime", offsetof(entvars_t, endtime), NULL},
	{ev_int, "rotate_type", offsetof(entvars_t, rotate_type), NULL},
	{ev_string, "path", offsetof(entvars_t, path), NULL},
	{ev_string, "group", offsetof(entvars_t, group), NULL},
	{ev_string, "event", offsetof(entvars_t, event), NULL},
	{ev_float, "duration", offsetof(entvars_t, duration), NULL},
	{ev_float, "AIRG_Timeout", offsetof(entvars_t, AIRG_Timeout), NULL},
	{ev_float, "AIRG_FireCount", offsetof(entvars_t, AIRG_FireCount), NULL},
	{ev_int, "AIRG_Flags", offsetof(entvars_t, AIRG_Flags), NULL},
	{ev_entity, "AIRG_FlyTracker", offsetof(entvars_t, AIRG_FlyTracker), NULL},
	{ev_float, "aura", offsetof(entvars_t, aura), NULL},
	{ev_float, "aura_time", offsetof(entvars_t, aura_time), NULL},
	{ev_entity, "crusader_inspirator", offsetof(entvars_t, crusader_inspirator), NULL},
	{ev_float, "reload_laser_cannon", offsetof(entvars_t, reload_laser_cannon), NULL},
	{ev_int, "tfstate", offsetof(entvars_t, tfstate), NULL},
	{ev_entity, "linked_list", offsetof(entvars_t, linked_list), NULL},
	{ev_entity, "observer_list", offsetof(entvars_t, observer_list), NULL},
	{ev_float, "maxammo_shells", offsetof(entvars_t, maxammo_shells), NULL},
	{ev_float, "maxammo_nails", offsetof(entvars_t, maxammo_nails), NULL},
	{ev_float, "maxammo_cells", offsetof(entvars_t, maxammo_cells), NULL},
	{ev_float, "maxammo_rockets", offsetof(entvars_t, maxammo_rockets), NULL},
	{ev_int, "items_allowed", offsetof(entvars_t, items_allowed), NULL},
	{ev_float, "armor_allowed", offsetof(entvars_t, armor_allowed), NULL},
	{ev_float, "maxarmor", offsetof(entvars_t, maxarmor), NULL},
	{ev_float, "weaponmode", offsetof(entvars_t, weaponmode), NULL},
	{ev_float, "motd", offsetof(entvars_t, motd), NULL},
	{ev_float, "current_menu", offsetof(entvars_t, current_menu), NULL},
	{ev_float, "menu_count", offsetof(entvars_t, menu_count), NULL},
	{ev_float, "menu_displaytime", offsetof(entvars_t, menu_displaytime), NULL},
	{ev_float, "team_no", offsetof(entvars_t, team_no), NULL},
	{ev_float, "new_team_no", offsetof(entvars_t, new_team_no), NULL},
	{ev_float, "lives", offsetof(entvars_t, lives), NULL},
	{ev_float, "infection_team_no", offsetof(entvars_t, infection_team_no), NULL},
	{ev_entity, "building", offsetof(entvars_t, building), NULL},
	{ev_float, "building_wait", offsetof(entvars_t, building_wait), NULL},
	{ev_entity, "real_owner", offsetof(entvars_t, real_owner), NULL},
	{ev_float, "real_frags", offsetof(entvars_t, real_frags), NULL},
	{ev_float, "respawn_time", offsetof(entvars_t, respawn_time), NULL},
	{ev_float, "suicide_time", offsetof(entvars_t, suicide_time), NULL},
	{ev_int, "weapons_carried", offsetof(entvars_t, weapons_carried), NULL},
	{ev_int, "current_weapon", offsetof(entvars_t, current_weapon), NULL},
	{ev_int, "cutf_items", offsetof(entvars_t, cutf_items), NULL},
	{ev_int, "cutf_moreitems", offsetof(entvars_t, cutf_moreitems), NULL},
	{ev_float, "ammo_medikit", offsetof(entvars_t, ammo_medikit), NULL},
	{ev_float, "maxammo_medikit", offsetof(entvars_t, maxammo_medikit), NULL},
	{ev_float, "ammo_detpack", offsetof(entvars_t, ammo_detpack), NULL},
	{ev_float, "maxammo_detpack", offsetof(entvars_t, maxammo_detpack), NULL},
	{ev_float, "ammo_c4det", offsetof(entvars_t, ammo_c4det), NULL},
	{ev_float, "reload_shotgun", offsetof(entvars_t, reload_shotgun), NULL},
	{ev_float, "reload_super_shotgun", offsetof(entvars_t, reload_super_shotgun), NULL},
	{ev_float, "reload_grenade_launcher", offsetof(entvars_t, reload_grenade_launcher), NULL},
	{ev_float, "reload_rocket_launcher", offsetof(entvars_t, reload_rocket_launcher), NULL},
	{ev_float, "reload_light_assault", offsetof(entvars_t, reload_light_assault), NULL},
	{ev_float, "heat", offsetof(entvars_t, heat), NULL},
	{ev_float, "immune_to_chec", offsetof(entvars_t, immune_to_chec), NULL},
	{ev_float, "last_saveme_sound", offsetof(entvars_t, last_saveme_sound), NULL},
	{ev_float, "last_grenade_primed", offsetof(entvars_t, last_grenade_primed), NULL},
	{ev_float, "goal_no", offsetof(entvars_t, goal_no), NULL},
	{ev_float, "group_no", offsetof(entvars_t, group_no), NULL},
	{ev_float, "goal_state", offsetof(entvars_t, goal_state), NULL},
	{ev_float, "owned_by", offsetof(entvars_t, owned_by), NULL},
	{ev_int, "goal_activation", offsetof(entvars_t, goal_activation), NULL},
	{ev_int, "goal_effects", offsetof(entvars_t, goal_effects), NULL},
	{ev_int, "goal_result", offsetof(entvars_t, goal_result), NULL},
	{ev_float, "goal_group", offsetof(entvars_t, goal_group), NULL},
	{ev_float, "else_goal", offsetof(entvars_t, else_goal), NULL},
	{ev_int, "if_goal_is_active", offsetof(entvars_t, if_goal_is_active), NULL},
	{ev_int, "if_goal_is_inactive", offsetof(entvars_t, if_goal_is_inactive), NULL},
	{ev_int, "if_goal_is_removed", offsetof(entvars_t, if_goal_is_removed), NULL},
	{ev_int, "if_group_is_active", offsetof(entvars_t, if_group_is_active), NULL},
	{ev_int, "if_group_is_inactive", offsetof(entvars_t, if_group_is_inactive), NULL},
	{ev_int, "if_group_is_removed", offsetof(entvars_t, if_group_is_removed), NULL},
	{ev_float, "activate_goal_no", offsetof(entvars_t, activate_goal_no), NULL},
	{ev_float, "inactivate_goal_no", offsetof(entvars_t, inactivate_goal_no), NULL},
	{ev_float, "remove_goal_no", offsetof(entvars_t, remove_goal_no), NULL},
	{ev_float, "restore_goal_no", offsetof(entvars_t, restore_goal_no), NULL},
	{ev_float, "activate_group_no", offsetof(entvars_t, activate_group_no), NULL},
	{ev_float, "inactivate_group_no", offsetof(entvars_t, inactivate_group_no), NULL},
	{ev_float, "remove_group_no", offsetof(entvars_t, remove_group_no), NULL},
	{ev_int, "restore_group_no", offsetof(entvars_t, restore_group_no), NULL},
	{ev_float, "has_item_from_group", offsetof(entvars_t, has_item_from_group), NULL},
	{ev_float, "remove_item_group", offsetof(entvars_t, remove_item_group), NULL},
	{ev_float, "return_item_no", offsetof(entvars_t, return_item_no), NULL},
	{ev_float, "if_item_has_moved", offsetof(entvars_t, if_item_has_moved), NULL},
	{ev_float, "if_item_hasnt_moved", offsetof(entvars_t, if_item_hasnt_moved), NULL},
	{ev_float, "remove_spawnpoint", offsetof(entvars_t, remove_spawnpoint), NULL},
	{ev_float, "restore_spawnpoint", offsetof(entvars_t, restore_spawnpoint), NULL},
	{ev_float, "remove_spawngroup", offsetof(entvars_t, remove_spawngroup), NULL},
	{ev_float, "restore_spawngroup", offsetof(entvars_t, restore_spawngroup), NULL},
	{ev_int, "display_item_status1", offsetof(entvars_t, display_item_status1), NULL},
	{ev_int, "display_item_status2", offsetof(entvars_t, display_item_status2), NULL},
	{ev_int, "display_item_status3", offsetof(entvars_t, display_item_status3), NULL},
	{ev_int, "display_item_status4", offsetof(entvars_t, display_item_status4), NULL},
	{ev_string, "team_str_home", offsetof(entvars_t, team_str_home), NULL},
	{ev_string, "team_str_moved", offsetof(entvars_t, team_str_moved), NULL},
	{ev_string, "team_str_carried", offsetof(entvars_t, team_str_carried), NULL},
	{ev_string, "non_team_str_home", offsetof(entvars_t, non_team_str_home), NULL},
	{ev_string, "non_team_str_moved", offsetof(entvars_t, non_team_str_moved), NULL},
	{ev_string, "non_team_str_carried", offsetof(entvars_t, non_team_str_carried), NULL},
	{ev_vector, "goal_min", offsetof(entvars_t, goal_min), NULL},
	{ev_vector, "goal_max", offsetof(entvars_t, goal_max), NULL},
	{ev_float, "ex_skill_min", offsetof(entvars_t, ex_skill_min), NULL},
	{ev_float, "ex_skill_max", offsetof(entvars_t, ex_skill_max), NULL},
	{ev_float, "increase_team1", offsetof(entvars_t, increase_team1), NULL},
	{ev_float, "increase_team2", offsetof(entvars_t, increase_team2), NULL},
	{ev_float, "increase_team3", offsetof(entvars_t, increase_team3), NULL},
	{ev_float, "increase_team4", offsetof(entvars_t, increase_team4), NULL},
	{ev_string, "broadcast", offsetof(entvars_t, broadcast), NULL},
	{ev_string, "team_broadcast", offsetof(entvars_t, team_broadcast), NULL},
	{ev_string, "non_team_broadcast", offsetof(entvars_t, non_team_broadcast), NULL},
	{ev_string, "owners_team_broadcast", offsetof(entvars_t, owners_team_broadcast), NULL},
	{ev_string, "netname_broadcast", offsetof(entvars_t, netname_broadcast), NULL},
	{ev_string, "netname_team_broadcast", offsetof(entvars_t, netname_team_broadcast), NULL},
	{ev_string, "netname_non_team_broadcast", offsetof(entvars_t, netname_non_team_broadcast), NULL},
	{ev_string, "netname_owners_team_broadcast", offsetof(entvars_t, netname_owners_team_broadcast), NULL},
	{ev_string, "team_drop", offsetof(entvars_t, team_drop), NULL},
	{ev_string, "non_team_drop", offsetof(entvars_t, non_team_drop), NULL},
	{ev_string, "netname_team_drop", offsetof(entvars_t, netname_team_drop), NULL},
	{ev_string, "netname_non_team_drop", offsetof(entvars_t, netname_non_team_drop), NULL},
	{ev_int, "all_active", offsetof(entvars_t, all_active), NULL},
	{ev_int, "item_list", offsetof(entvars_t, item_list), NULL},
	{ev_float, "delay_time", offsetof(entvars_t, delay_time), NULL},
	{ev_int, "dont_do_triggerwork", offsetof(entvars_t, dont_do_triggerwork), NULL},
	{ev_float, "g_a", offsetof(entvars_t, g_a), NULL},
	{ev_float, "g_e", offsetof(entvars_t, g_e), NULL},
	{ev_string, "t_s_h", offsetof(entvars_t, t_s_h), NULL},
	{ev_string, "t_s_m", offsetof(entvars_t, t_s_m), NULL},
	{ev_string, "t_s_c", offsetof(entvars_t, t_s_c), NULL},
	{ev_string, "n_s_h", offsetof(entvars_t, n_s_h), NULL},
	{ev_string, "n_s_m", offsetof(entvars_t, n_s_m), NULL},
	{ev_string, "n_s_c", offsetof(entvars_t, n_s_c), NULL},
	{ev_string, "b_b", offsetof(entvars_t, b_b), NULL},
	{ev_string, "b_t", offsetof(entvars_t, b_t), NULL},
	{ev_string, "b_n", offsetof(entvars_t, b_n), NULL},
	{ev_string, "b_o", offsetof(entvars_t, b_o), NULL},
	{ev_string, "n_b", offsetof(entvars_t, n_b), NULL},
	{ev_string, "n_t", offsetof(entvars_t, n_t), NULL},
	{ev_string, "n_n", offsetof(entvars_t, n_n), NULL},
	{ev_string, "n_o", offsetof(entvars_t, n_o), NULL},
	{ev_string, "d_t", offsetof(entvars_t, d_t), NULL},
	{ev_string, "d_n", offsetof(entvars_t, d_n), NULL},
	{ev_string, "d_n_t", offsetof(entvars_t, d_n_t), NULL},
	{ev_string, "d_n_n", offsetof(entvars_t, d_n_n), NULL},
	{ev_float, "numflames", offsetof(entvars_t, numflames), NULL},
	{ev_string, "flame_id", offsetof(entvars_t, flame_id), NULL},
	{ev_float, "undercover_team", offsetof(entvars_t, undercover_team), NULL},
	{ev_float, "undercover_skin", offsetof(entvars_t, undercover_skin), NULL},
	{ev_string, "undercover_name", offsetof(entvars_t, undercover_name), NULL},
	{ev_float, "hook_out", offsetof(entvars_t, hook_out), NULL},
	{ev_float, "camdist", offsetof(entvars_t, camdist), NULL},
	{ev_vector, "camangle", offsetof(entvars_t, camangle), NULL},
	{ev_entity, "camera_list", offsetof(entvars_t, camera_list), NULL},
	{ev_entity, "hook", offsetof(entvars_t, hook), NULL},
	{ev_float, "on_hook", offsetof(entvars_t, on_hook), NULL},
	{ev_float, "fire_held_down", offsetof(entvars_t, fire_held_down), NULL},
	{ev_vector, "head_shot_vector", offsetof(entvars_t, head_shot_vector), NULL},
	{ev_float, "leg_damage", offsetof(entvars_t, leg_damage), NULL},
	{ev_float, "cheat_level", offsetof(entvars_t, cheat_level), NULL},
	{ev_float, "speed_level", offsetof(entvars_t, speed_level), NULL},
	{ev_float, "FlashTime", offsetof(entvars_t, FlashTime), NULL},
	{ev_float, "StatusRefreshTime", offsetof(entvars_t, StatusRefreshTime), NULL},
	{ev_float, "StatusBarSize", offsetof(entvars_t, StatusBarSize), NULL},
	{ev_float, "StatusBarRes", offsetof(entvars_t, StatusBarRes), NULL},
	{ev_float, "StatusBarScreen", offsetof(entvars_t, StatusBarScreen), NULL},
	{ev_int, "runes_owned", offsetof(entvars_t, runes_owned), NULL},
	{ev_float, "clversion", offsetof(entvars_t, clversion), NULL},
	{ev_float, "cltype", offsetof(entvars_t, cltype), NULL},
	{ev_entity, "npc_nextTeamList", offsetof(entvars_t, npc_nextTeamList), NULL},
	{ev_entity, "npc_nextEntInTeamList", offsetof(entvars_t, npc_nextEntInTeamList), NULL},
	{ev_float, "wallhug", offsetof(entvars_t, wallhug), NULL},
	{ev_int, "keys", offsetof(entvars_t, keys), NULL},
	{ev_float, "oldkeys", offsetof(entvars_t, oldkeys), NULL},
	{ev_float, "ishuman", offsetof(entvars_t, ishuman), NULL},
	{ev_float, "b_frags", offsetof(entvars_t, b_frags), NULL},
	{ev_float, "b_clientno", offsetof(entvars_t, b_clientno), NULL},
	{ev_float, "b_shirt", offsetof(entvars_t, b_shirt), NULL},
	{ev_float, "b_pants", offsetof(entvars_t, b_pants), NULL},
	{ev_float, "ai_time", offsetof(entvars_t, ai_time), NULL},
	{ev_float, "b_sound", offsetof(entvars_t, b_sound), NULL},
	{ev_float, "missile_speed", offsetof(entvars_t, missile_speed), NULL},
	{ev_float, "portal_time", offsetof(entvars_t, portal_time), NULL},
	{ev_int, "b_skill", offsetof(entvars_t, b_skill), NULL},
	{ev_int, "b_aiflags", offsetof(entvars_t, b_aiflags), NULL},
	{ev_float, "b_num", offsetof(entvars_t, b_num), NULL},
	{ev_float, "b_chattime", offsetof(entvars_t, b_chattime), NULL},
	{ev_float, "b_userid", offsetof(entvars_t, b_userid), NULL},
	{ev_float, "b_menu", offsetof(entvars_t, b_menu), NULL},
	{ev_float, "b_timeToRedrawWaypointEditorMenu", offsetof(entvars_t, b_timeToRedrawWaypointEditorMenu), NULL},
	{ev_float, "b_menu_value", offsetof(entvars_t, b_menu_value), NULL},
	{ev_float, "b_routeInvalid", offsetof(entvars_t, b_routeInvalid), NULL},
	{ev_float, "b_pursuingEnemy", offsetof(entvars_t, b_pursuingEnemy), NULL},
	{ev_float, "dyn_time", offsetof(entvars_t, dyn_time), NULL},
	{ev_float, "dyn_plat", offsetof(entvars_t, dyn_plat), NULL},
	{ev_float, "b_roamingWaypoints", offsetof(entvars_t, b_roamingWaypoints), NULL},
	{ev_entity, "temp_way", offsetof(entvars_t, temp_way), NULL},
	{ev_entity, "target1", offsetof(entvars_t, target1), NULL},
	{ev_entity, "target2", offsetof(entvars_t, target2), NULL},
	{ev_entity, "target3", offsetof(entvars_t, target3), NULL},
	{ev_entity, "target4", offsetof(entvars_t, target4), NULL},
	{ev_entity, "b_lastGoalTarget", offsetof(entvars_t, b_lastGoalTarget), NULL},
	{ev_entity, "_prev", offsetof(entvars_t, _prev), NULL},
	{ev_entity, "_next", offsetof(entvars_t, _next), NULL},
	{ev_entity, "last_way", offsetof(entvars_t, last_way), NULL},
	{ev_entity, "current_way", offsetof(entvars_t, current_way), NULL},
	{ev_entity, "b_entityToAvoid", offsetof(entvars_t, b_entityToAvoid), NULL},
	{ev_vector, "b_commandedViewAngle", offsetof(entvars_t, b_commandedViewAngle), NULL},
	{ev_vector, "b_dest", offsetof(entvars_t, b_dest), NULL},
	{ev_vector, "mouse_emu", offsetof(entvars_t, mouse_emu), NULL},
	{ev_vector, "b_obstructionDirection", offsetof(entvars_t, b_obstructionDirection), NULL},
	{ev_vector, "movevect", offsetof(entvars_t, movevect), NULL},
	{ev_vector, "b_dir", offsetof(entvars_t, b_dir), NULL},
	{ev_vector, "dyn_dest", offsetof(entvars_t, dyn_dest), NULL},
	{ev_vector, "b_botRole", offsetof(entvars_t, b_botRole), NULL},
	{ev_float, "b_botInEnemyFOV", offsetof(entvars_t, b_botInEnemyFOV), NULL},
	{ev_float, "b_botInEnemyPlayerFOV", offsetof(entvars_t, b_botInEnemyPlayerFOV), NULL},
	{ev_float, "b_botInEnemyMachineFOV", offsetof(entvars_t, b_botInEnemyMachineFOV), NULL},
	{ev_float, "ed_jumpLastPressed", offsetof(entvars_t, ed_jumpLastPressed), NULL},
	{ev_float, "ed_fireLastPressed", offsetof(entvars_t, ed_fireLastPressed), NULL},
	{ev_function, "npc_oldthink", offsetof(entvars_t, npc_oldthink), NULL},
	{ev_float, "monsterweight", offsetof(entvars_t, monsterweight), NULL},
	{ev_int, "monsterflags", offsetof(entvars_t, monsterflags), NULL},
	{ev_float, "healamount", offsetof(entvars_t, healamount), NULL},
	{ev_float, "healtype", offsetof(entvars_t, healtype), NULL},
	{ev_entity, "pickedupobject_hands", offsetof(entvars_t, pickedupobject_hands), NULL},
	{ev_entity, "pickupthink_hands", offsetof(entvars_t, pickupthink_hands), NULL},
	{ev_entity, "_oldowner", offsetof(entvars_t, _oldowner), NULL},
	{ev_entity, "pickedupby", offsetof(entvars_t, pickedupby), NULL},
	{ev_float, "oldmovetype", offsetof(entvars_t, oldmovetype), NULL},
	{ev_float, "dmgtime", offsetof(entvars_t, dmgtime), NULL},
	{ev_function, "OnNoticeEnemy", offsetof(entvars_t, OnNoticeEnemy), NULL},
	{ev_function, "OnPreAttack", offsetof(entvars_t, OnPreAttack), NULL},
	{ev_entity, "npc_nextTeamList", offsetof(entvars_t, npc_nextTeamList), NULL},
	{ev_entity, "npc_nextEntInTeamList", offsetof(entvars_t, npc_nextEntInTeamList), NULL},
	{ev_float, "inpain", offsetof(entvars_t, inpain), NULL},



	// REQUIRED: marks end of list
	{ev_null,   "",      -1,                           NULL}
}; // END pr_fielddefs

// Function dfunction_t's
dfunction_t pr_functions[] =
{
	{ev_void,   "StartFrame", (void*)&Progs::StartFrame},
	{ev_void,   "worldspawn", (void*)&Progs::worldspawn},
	{ev_void,   "func_button", (void*)&Progs::func_button},
	{ev_void,   "info_intermission", (void*)&Progs::info_intermission},
	{ev_void,   "info_player_coop", (void*)&Progs::info_player_coop},
	{ev_void,   "info_player_deathmatch", (void*)&Progs::info_player_deathmatch},
	{ev_void,   "info_player_start", (void*)&Progs::info_player_start},
	{ev_void,   "info_player_start2", (void*)&Progs::info_player_start2},
	{ev_void,   "trigger_changelevel", (void*)&Progs::trigger_changelevel},
	{ev_void,   "func_door", (void*)&Progs::func_door},
	{ev_void,   "func_door_secret", (void*)&Progs::func_door_secret},
	{ev_void,   "item_armor1", (void*)&Progs::item_armor1},
	{ev_void,   "item_armor2", (void*)&Progs::item_armor2},
	{ev_void,   "item_armorInv", (void*)&Progs::item_armorInv},
	{ev_void,   "item_artifact_envirosuit", (void*)&Progs::item_artifact_envirosuit},
	{ev_void,   "item_artifact_invisibility", (void*)&Progs::item_artifact_invisibility},
	{ev_void,   "item_artifact_invulnerability", (void*)&Progs::item_artifact_invulnerability},
	{ev_void,   "item_artifact_super_damage", (void*)&Progs::item_artifact_super_damage},
	{ev_void,   "item_cells", (void*)&Progs::item_cells},
	{ev_void,   "item_health", (void*)&Progs::item_health},
	{ev_void,   "item_key1", (void*)&Progs::item_key1},
	{ev_void,   "item_key2", (void*)&Progs::item_key2},
	{ev_void,   "item_rockets", (void*)&Progs::item_rockets},
	{ev_void,   "item_shells", (void*)&Progs::item_shells},
	{ev_void,   "item_sigil", (void*)&Progs::item_sigil},
	{ev_void,   "item_spikes", (void*)&Progs::item_spikes},
	{ev_void,   "item_weapon", (void*)&Progs::item_weapon},
	{ev_void,   "weapon_grenadelauncher", (void*)&Progs::weapon_grenadelauncher},
	{ev_void,   "weapon_lightning", (void*)&Progs::weapon_lightning},
	{ev_void,   "weapon_nailgun", (void*)&Progs::weapon_nailgun},
	{ev_void,   "weapon_rocketlauncher", (void*)&Progs::weapon_rocketlauncher},
	{ev_void,   "weapon_supernailgun", (void*)&Progs::weapon_supernailgun},
	{ev_void,   "weapon_supershotgun", (void*)&Progs::weapon_supershotgun},
	{ev_void,   "air_bubbles", (void*)&Progs::air_bubbles},
	{ev_void,   "info_null", (void*)&Progs::info_null},
	{ev_void,   "info_notnull", (void*)&Progs::info_notnull},
	{ev_void,   "light", (void*)&Progs::light},
	{ev_void,   "light_fluoro", (void*)&Progs::light_fluoro},
	{ev_void,   "light_fluorospark", (void*)&Progs::light_fluorospark},
	{ev_void,   "light_globe", (void*)&Progs::light_globe},
	{ev_void,   "light_torch_small_walltorch", (void*)&Progs::light_torch_small_walltorch},
	{ev_void,   "light_flame_large_yellow", (void*)&Progs::light_flame_large_yellow},
	{ev_void,   "light_flame_small_yellow", (void*)&Progs::light_flame_small_yellow},
	{ev_void,   "light_flame_small_white", (void*)&Progs::light_flame_small_white},
	{ev_void,   "misc_fireball", (void*)&Progs::misc_fireball},
	{ev_void,   "misc_explobox", (void*)&Progs::misc_explobox},
	{ev_void,   "misc_explobox2", (void*)&Progs::misc_explobox2},
	{ev_void,   "trap_spikeshooter", (void*)&Progs::trap_spikeshooter},
	{ev_void,   "trap_shooter", (void*)&Progs::trap_shooter},
	{ev_void,   "viewthing", (void*)&Progs::viewthing},
	{ev_void,   "func_wall", (void*)&Progs::func_wall},
	{ev_void,   "func_illusionary", (void*)&Progs::func_illusionary},
	{ev_void,   "func_episodegate", (void*)&Progs::func_episodegate},
	{ev_void,   "func_bossgate", (void*)&Progs::func_bossgate},
	{ev_void,   "ambient_suck_wind", (void*)&Progs::ambient_suck_wind},
	{ev_void,   "ambient_drone", (void*)&Progs::ambient_drone},
	{ev_void,   "ambient_flouro_buzz", (void*)&Progs::ambient_flouro_buzz},
	{ev_void,   "ambient_drip", (void*)&Progs::ambient_drip},
	{ev_void,   "ambient_comp_hum", (void*)&Progs::ambient_comp_hum},
	{ev_void,   "ambient_thunder", (void*)&Progs::ambient_thunder},
	{ev_void,   "ambient_light_buzz", (void*)&Progs::ambient_light_buzz},
	{ev_void,   "ambient_swamp1", (void*)&Progs::ambient_swamp1},
	{ev_void,   "ambient_swamp2", (void*)&Progs::ambient_swamp2},
	{ev_void,   "misc_noisemaker", (void*)&Progs::misc_noisemaker},
	{ev_void,   "func_plat", (void*)&Progs::func_plat},
	{ev_void,   "func_train", (void*)&Progs::func_train},
	{ev_void,   "misc_teleporttrain", (void*)&Progs::misc_teleporttrain},
	{ev_void,   "path_corner", (void*)&Progs::path_corner},
	{ev_void,   "info_teleport_destination", (void*)&Progs::info_teleport_destination},
	{ev_void,   "trigger_monsterjump", (void*)&Progs::trigger_monsterjump},
	{ev_void,   "trigger_multiple", (void*)&Progs::trigger_multiple},
	{ev_void,   "trigger_once", (void*)&Progs::trigger_once},
	{ev_void,   "trigger_onlyregistered", (void*)&Progs::trigger_onlyregistered},
	{ev_void,   "trigger_push", (void*)&Progs::trigger_push},
	{ev_void,   "trigger_relay", (void*)&Progs::trigger_relay},
	{ev_void,   "trigger_secret", (void*)&Progs::trigger_secret},
	{ev_void,   "trigger_setskill", (void*)&Progs::trigger_setskill},
	{ev_void,   "trigger_teleport", (void*)&Progs::trigger_teleport},
	{ev_void,   "trigger_counter", (void*)&Progs::trigger_counter},
	{ev_void,   "monster_ogre", (void*)&Progs::monster_ogre},
	{ev_void,   "monster_demon1", (void*)&Progs::monster_demon1},
	{ev_void,   "monster_shambler", (void*)&Progs::monster_shambler},
	{ev_void,   "monster_knight", (void*)&Progs::monster_knight},
	{ev_void,   "monster_army", (void*)&Progs::monster_army},
	{ev_void,   "monster_wizard", (void*)&Progs::monster_wizard},
	{ev_void,   "monster_dog", (void*)&Progs::monster_dog},
	{ev_void,   "monster_zombie", (void*)&Progs::monster_zombie},
	{ev_void,   "monster_boss", (void*)&Progs::monster_boss},
	{ev_void,   "monster_tarbaby", (void*)&Progs::monster_tarbaby},
	{ev_void,   "monster_hell_knight", (void*)&Progs::monster_hell_knight},
	{ev_void,   "monster_fish", (void*)&Progs::monster_fish},
	{ev_void,   "monster_shalrath", (void*)&Progs::monster_shalrath},
	{ev_void,   "monster_enforcer", (void*)&Progs::monster_enforcer},
	{ev_void,   "monster_oldone", (void*)&Progs::monster_oldone},
	{ev_void,   "event_lightning", (void*)&Progs::event_lightning},
	{ev_void,   "trigger_hurt", (void*)&Progs::trigger_hurt},

	// CuTF (TF)
	{ev_float,  "ClientCommand", (void*)&Progs::ClientCommand},
	{ev_void,   "info_tfdetect", (void*)&Progs::info_tfdetect},
	{ev_void,   "info_player_teamspawn", (void*)&Progs::info_player_teamspawn},
	{ev_void,   "info_tfgoal", (void*)&Progs::info_tfgoal},
	{ev_void,   "info_tfgoal_timer", (void*)&Progs::info_tfgoal_timer},
	{ev_void,   "item_tfgoal", (void*)&Progs::item_tfgoal},
	{ev_void,   "i_p_t", (void*)&Progs::i_p_t},
	{ev_void,   "i_t_g", (void*)&Progs::i_t_g},
	{ev_void,   "i_t_t", (void*)&Progs::i_t_t},
	// CuTF (MegaTF)
	{ev_void,   "ambient_sound", (void*)&Progs::ambient_sound},
	{ev_void,   "effect_random_sound", (void*)&Progs::effect_random_sound},
	{ev_void,   "ambient_jungle", (void*)&Progs::ambient_jungle},
	{ev_void,   "ambient_diesel", (void*)&Progs::ambient_diesel},
	{ev_void,   "ambient_chopper", (void*)&Progs::ambient_chopper},
	{ev_void,   "ambient_eerie", (void*)&Progs::ambient_eerie},
	{ev_void,   "ambient_ocean", (void*)&Progs::ambient_ocean},
	{ev_void,   "ambient_peakwind", (void*)&Progs::ambient_peakwind},
	{ev_void,   "ambient_lavapit", (void*)&Progs::ambient_lavapit},
	{ev_void,   "ambient_unholy", (void*)&Progs::ambient_unholy},
	{ev_void,   "ambient_flagflap", (void*)&Progs::ambient_flagflap},
	{ev_void,   "ambient_high_wind", (void*)&Progs::ambient_high_wind},
	{ev_void,   "ambient_meadow", (void*)&Progs::ambient_meadow},
	{ev_void,   "ambient_brook", (void*)&Progs::ambient_brook},
	{ev_void,   "ambient_ice_moving", (void*)&Progs::ambient_ice_moving},
	{ev_void,   "ambient_nightpond", (void*)&Progs::ambient_nightpond},
	{ev_void,   "ambient_alert", (void*)&Progs::ambient_alert},
	{ev_void,   "ambient_chant", (void*)&Progs::ambient_chant},
	{ev_void,   "ambient_onboard", (void*)&Progs::ambient_onboard},
	{ev_void,   "ambient_rocket_engine", (void*)&Progs::ambient_rocket_engine},
	{ev_void,   "ambient_weaponfire", (void*)&Progs::ambient_weaponfire},
	{ev_void,   "effect_rockfall", (void*)&Progs::effect_rockfall},
	{ev_void,   "effect_airburst", (void*)&Progs::effect_airburst},
	{ev_void,   "func_bobbingwater", (void*)&Progs::func_bobbingwater},
	{ev_void,   "func_glass", (void*)&Progs::func_glass},
	{ev_void,   "effect_lightning", (void*)&Progs::effect_lightning},
	{ev_void,   "monster_dog", (void*)&Progs::monster_dog},
	{ev_void,   "monster_turret", (void*)&Progs::monster_turret},
	{ev_void,   "monster_fish", (void*)&Progs::monster_fish},
	{ev_void,   "monster_zombie", (void*)&Progs::monster_zombie},

	// REQUIRED: marks end of list
	{ev_null,   "",           NULL}
}; // END pr_functions


// This should be here, because it will be auto-generated.
// IMPORTANT: This should never clear the `_edict` field.
void Progs::clearEntityFields(entity e)
{
	e->angles.fill(0);
	e->v_angle.fill(0);
	e->view_ofs.fill(0);
	e->mangle.fill(0);
	e->finalangle.fill(0);
	e->movedir.fill(0);
	e->origin.fill(0);
	e->oldorigin.fill(0);
	e->velocity.fill(0);
	e->avelocity.fill(0);
	e->finaldest.fill(0);
	e->dest.fill(0);
	e->dest1.fill(0);
	e->dest2.fill(0);
	e->size.fill(0);
	e->mins.fill(0);
	e->maxs.fill(0);
	e->absmin.fill(0);
	e->absmax.fill(0);
	e->pos1.fill(0);
	e->pos2.fill(0);
	e->model = "";
	e->mdl = "";
	e->map = "";
	e->classname = "";
	e->netname = "";
	e->message = "";
	e->targetname = "";
	e->deathtype = "";
	e->noise = "";
	e->noise1 = "";
	e->noise2 = "";
	e->noise3 = "";
	e->noise4 = "";
	e->target = "";
	e->killtarget = "";
	e->weaponmodel = "";
	e->wad = "";
	e->button0 = 0;             // PZ TODO: add bool type for ones like this?
	e->button1 = 0;
	e->button2 = 0;
	e->items = 0;
	e->flags = 0;
	e->movetype = 0;
	e->spawnflags = 0;
	e->skin = 0;
	e->weapon = 0;
	e->effects = 0;
	e->deadflag = 0;
	e->watertype = 0;
	e->walkframe = 0;
	e->bubble_count = 0;
	e->aflag = 0;
	e->impulse = 0;
	e->worldtype = 0;
	e->axhitme = 0;
	e->weaponframe = 0;
	e->show_hostile = 0;
	e->dmg_take = 0;
	e->dmg_save = 0;
	e->lip = 0;
	e->voided = 0;
	e->colormap = 0;
	e->frags = 0;
	e->swim_flag = 0;
	e->air_finished = 0;
	e->attack_finished = 0;
	e->pain_finished = 0;
	e->super_sound = 0;
	e->super_time = 0;
	e->super_damage_finished = 0;
	e->invisible_time = 0;
	e->invisible_finished = 0;
	e->invincible_time = 0;
	e->invincible_finished = 0;
	e->rad_time = 0;
	e->dmgtime = 0;
	e->radsuit_finished = 0;
	e->currentammo = 0;
	e->armortype = 0;
	e->armorvalue = 0;
	e->takedamage = 0;
	e->modelindex = 0;
	e->health = 0;
	e->healamount = 0;
	e->healtype = 0;
	e->ammo_cells = 0;
	e->ammo_shells = 0;
	e->ammo_rockets = 0;
	e->ammo_nails = 0;
	e->solid = 0;
	e->height = 0;
	e->nextthink = 0;
	e->t_width = 0;
	e->t_length = 0;
	e->ltime = 0;
	e->delay = 0;
	e->pausetime = 0;
	e->ideal_yaw = 0;
	e->fly_sound = 0;
	e->fixangle = 0;
	e->prev_velocity_z = 0;
	e->frame = 0;
	e->cnt = 0;
	e->teleport_time = 0;
	e->count = 0;
	e->max_health = 0;
	e->invisible_sound = 0;
	e->invincible_sound = 0;
	e->waterlevel = 0;
	e->wait = 0;
	e->speed = 0;
	e->sounds = 0;
	e->dmg = 0;
	e->style = 0;
	e->state = 0;
	e->team = 0;
	e->yaw_speed = 0;
	e->lastruntime = 0;
	e->ishuman = 0;        // Not used in vanilla QuakeWorld, but easier to add here than it is to remove all bot code, essentially.
	e->admin_flag = 0;     // Not used in vanilla QuakeWorld, but easier to just add here than it is to undo all engine code that uses it. (TODO. Don't require progs to have this field.)
	e->current_menu = 0;   // Not used in vanilla QuakeWorld, but easier to just add here than it is to undo all engine code that uses it. (TODO. Don't require progs to have this field.)
	e->light_lev = 0;
	e->dmg_inflictor = &sv.edicts->v;
	e->owner = &sv.edicts->v;
	e->enemy = &sv.edicts->v;
	e->movetarget = &sv.edicts->v;
	e->goalentity = &sv.edicts->v;
	e->trigger_field = &sv.edicts->v;
	e->chain = &sv.edicts->v;
	e->groundentity = &sv.edicts->v;
	e->think = SUB_Null;
	e->think1 = SUB_Null;
	e->th_stand = SUB_Null;
	e->touch = SUB_Null;
	e->use = SUB_Null;
	e->th_die = SUB_Null;
	e->th_walk = SUB_Null;
	e->th_run = SUB_Null;
	e->th_missile = SUB_Null;
	e->th_melee = SUB_Null;
	e->th_pain = (void (*)(entity, float))SUB_Null;
	e->blocked = SUB_Null;

	// CuTF
	// Entities
	e->groundentity = &sv.edicts->v;
	e->chain = &sv.edicts->v;
	e->enemy = &sv.edicts->v;
	e->aiment = &sv.edicts->v;
	e->goalentity = &sv.edicts->v;
	e->dmg_inflictor = &sv.edicts->v;
	e->owner = &sv.edicts->v;
	e->oldenemy = &sv.edicts->v;
	e->inspirator = &sv.edicts->v;
	e->demon_one = &sv.edicts->v;
	e->demon_two = &sv.edicts->v;
	e->demon_three = &sv.edicts->v;
	e->scaned = &sv.edicts->v;
	e->trigger_field = &sv.edicts->v;
	e->movetarget = &sv.edicts->v;
	e->admin_kick = &sv.edicts->v;
	e->martyr_enemy = &sv.edicts->v;
	e->AIRG_FlyTracker = &sv.edicts->v;
	e->crusader_inspirator = &sv.edicts->v;
	e->linked_list = &sv.edicts->v;
	e->observer_list = &sv.edicts->v;
	e->building = &sv.edicts->v;
	e->real_owner = &sv.edicts->v;
	e->camera_list = &sv.edicts->v;
	e->hook = &sv.edicts->v;
	e->npc_nextTeamList = &sv.edicts->v;
	e->npc_nextEntInTeamList = &sv.edicts->v;
	e->temp_way = &sv.edicts->v;
	e->target1 = &sv.edicts->v;
	e->target2 = &sv.edicts->v;
	e->target3 = &sv.edicts->v;
	e->target4 = &sv.edicts->v;
	e->b_lastGoalTarget = &sv.edicts->v;
	e->_prev = &sv.edicts->v;
	e->_next = &sv.edicts->v;
	e->last_way = &sv.edicts->v;
	e->current_way = &sv.edicts->v;
	e->b_entityToAvoid = &sv.edicts->v;
	e->pickedupobject_hands = &sv.edicts->v;
	e->pickupthink_hands = &sv.edicts->v;
	e->_oldowner = &sv.edicts->v;
	e->pickedupby = &sv.edicts->v;
	e->npc_nextTeamList = &sv.edicts->v;
	e->npc_nextEntInTeamList = &sv.edicts->v;
	// Function Pointers
	e->npc_oldthink = SUB_Null;
	e->OnNoticeEnemy = (float (*)())SUB_Null;
	e->OnPreAttack = (float (*)())SUB_Null;
	e->th_fireball = SUB_Null;
	// Ints and Floats
	e->lefty = 0;
	e->search_time = 0;
	e->attack_state = 0;
	e->maxspeed = 0;
	e->gravity = 0;
	e->money = 0;
	e->custom_speed = 0;
	e->ff_count = 0;
	e->penance_time = 0;
	e->last_attacked_time = 0;
	e->has_cheated = 0;
	e->demon_blood = 0;
	e->hover_time = 0;
	e->done_custom = 0;
	e->prev_velocity_z = 0;
	e->waitmin = 0;
	e->waitmax = 0;
	e->distance = 0;
	e->volume = 0;
	e->playerclass = 0;
	e->nextpc = 0;
	e->last_impulse = 0;
	e->armorclass = 0;
	e->tf_items = 0;
	e->job = 0;
	e->job_finished = 0;
	e->tf_items_flags = 0;
	e->no_grenades_1 = 0;
	e->no_grenades_2 = 0;
	e->tp_grenades_1 = 0;
	e->tp_grenades_2 = 0;
	e->got_aliases = 0;
	e->cheat_check = 0;
	e->is_removed = 0;
	e->is_undercover = 0;
	e->is_building = 0;
	e->is_detpacking = 0;
	e->is_feigning = 0;
	e->is_haxxxoring = 0;
	e->is_toffingadet = 0;
	e->is_unabletospy = 0;
	e->is_malfunctioning = 0;
	e->is_abouttodie = 0;
	e->is_killed = 0;
	e->option = 0;
	e->option2 = 0;
	e->is_connected = 0;
	e->has_dispenser = 0;
	e->has_sentry = 0;
	e->has_tesla = 0;
	e->has_camera = 0;
	e->has_sensor = 0;
	e->has_teleporter = 0;
	e->has_fieldgen = 0;
	e->admin_flag = 0;
	e->stored_deathmsg = 0;
	e->cluster_mode = 0;
	e->has_holo = 0;
	e->endtime = 0;
	e->rotate_type = 0;
	e->duration = 0;
	e->AIRG_Timeout = 0;
	e->AIRG_FireCount = 0;
	e->AIRG_Flags = 0;
	e->aura = 0;
	e->aura_time = 0;
	e->reload_laser_cannon = 0;
	e->tfstate = 0;
	e->maxammo_shells = 0;
	e->maxammo_nails = 0;
	e->maxammo_cells = 0;
	e->maxammo_rockets = 0;
	e->items_allowed = 0;
	e->armor_allowed = 0;
	e->maxarmor = 0;
	e->weaponmode = 0;
	e->motd = 0;
	e->current_menu = 0;
	e->menu_count = 0;
	e->menu_displaytime = 0;
	e->team_no = 0;
	e->new_team_no = 0;
	e->lives = 0;
	e->infection_team_no = 0;
	e->building_wait = 0;
	e->real_frags = 0;
	e->respawn_time = 0;
	e->suicide_time = 0;
	e->weapons_carried = 0;
	e->current_weapon = 0;
	e->cutf_items = 0;
	e->cutf_moreitems = 0;
	e->ammo_medikit = 0;
	e->maxammo_medikit = 0;
	e->ammo_detpack = 0;
	e->maxammo_detpack = 0;
	e->ammo_c4det = 0;
	e->reload_shotgun = 0;
	e->reload_super_shotgun = 0;
	e->reload_grenade_launcher = 0;
	e->reload_rocket_launcher = 0;
	e->reload_light_assault = 0;
	e->heat = 0;
	e->immune_to_chec = 0;
	e->last_saveme_sound = 0;
	e->last_grenade_primed = 0;
	e->goal_no = 0;
	e->group_no = 0;
	e->goal_state = 0;
	e->owned_by = 0;
	e->goal_activation = 0;
	e->goal_effects = 0;
	e->goal_result = 0;
	e->goal_group = 0;
	e->else_goal = 0;
	e->if_goal_is_active = 0;
	e->if_goal_is_inactive = 0;
	e->if_goal_is_removed = 0;
	e->if_group_is_active = 0;
	e->if_group_is_inactive = 0;
	e->if_group_is_removed = 0;
	e->activate_goal_no = 0;
	e->inactivate_goal_no = 0;
	e->remove_goal_no = 0;
	e->restore_goal_no = 0;
	e->activate_group_no = 0;
	e->inactivate_group_no = 0;
	e->remove_group_no = 0;
	e->restore_group_no = 0;
	e->has_item_from_group = 0;
	e->remove_item_group = 0;
	e->return_item_no = 0;
	e->if_item_has_moved = 0;
	e->if_item_hasnt_moved = 0;
	e->remove_spawnpoint = 0;
	e->restore_spawnpoint = 0;
	e->remove_spawngroup = 0;
	e->restore_spawngroup = 0;
	e->display_item_status1 = 0;
	e->display_item_status2 = 0;
	e->display_item_status3 = 0;
	e->display_item_status4 = 0;
	e->ex_skill_min = 0;
	e->ex_skill_max = 0;
	e->increase_team1 = 0;
	e->increase_team2 = 0;
	e->increase_team3 = 0;
	e->increase_team4 = 0;
	e->all_active = 0;
	e->item_list = 0;
	e->delay_time = 0;
	e->dont_do_triggerwork = 0;
	e->g_a = 0;
	e->g_e = 0;
	e->numflames = 0;
	e->undercover_team = 0;
	e->undercover_skin = 0;
	e->hook_out = 0;
	e->camdist = 0;
	e->on_hook = 0;
	e->fire_held_down = 0;
	e->leg_damage = 0;
	e->cheat_level = 0;
	e->speed_level = 0;
	e->FlashTime = 0;
	e->StatusRefreshTime = 0;
	e->StatusBarSize = 0;
	e->StatusBarRes = 0;
	e->StatusBarScreen = 0;
	e->runes_owned = 0;
	e->clversion = 0;
	e->cltype = 0;
	e->wallhug = 0;
	e->keys = 0;
	e->oldkeys = 0;
	e->ishuman = 0;
	e->b_frags = 0;
	e->b_clientno = 0;
	e->b_shirt = 0;
	e->b_pants = 0;
	e->ai_time = 0;
	e->b_sound = 0;
	e->missile_speed = 0;
	e->portal_time = 0;
	e->b_skill = 0;
	e->b_aiflags = 0;
	e->b_num = 0;
	e->b_chattime = 0;
	e->b_userid = 0;
	e->b_menu = 0;
	e->b_timeToRedrawWaypointEditorMenu = 0;
	e->b_menu_value = 0;
	e->b_routeInvalid = 0;
	e->b_pursuingEnemy = 0;
	e->dyn_time = 0;
	e->dyn_plat = 0;
	e->b_roamingWaypoints = 0;
	e->b_botInEnemyFOV = 0;
	e->b_botInEnemyPlayerFOV = 0;
	e->b_botInEnemyMachineFOV = 0;
	e->ed_jumpLastPressed = 0;
	e->ed_fireLastPressed = 0;
	e->monsterweight = 0;
	e->monsterflags = 0;
	e->healamount = 0;
	e->healtype = 0;
	e->oldmovetype = 0;
	e->dmgtime = 0;
	e->inpain = 0;
	// Strings
	e->path = "";
	e->group = "";
	e->event = "";
	e->team_str_home = "";
	e->team_str_moved = "";
	e->team_str_carried = "";
	e->non_team_str_home = "";
	e->non_team_str_moved = "";
	e->non_team_str_carried = "";
	e->broadcast = "";
	e->team_broadcast = "";
	e->non_team_broadcast = "";
	e->owners_team_broadcast = "";
	e->netname_broadcast = "";
	e->netname_team_broadcast = "";
	e->netname_non_team_broadcast = "";
	e->netname_owners_team_broadcast = "";
	e->team_drop = "";
	e->non_team_drop = "";
	e->netname_team_drop = "";
	e->netname_non_team_drop = "";
	e->t_s_h = "";
	e->t_s_m = "";
	e->t_s_c = "";
	e->n_s_h = "";
	e->n_s_m = "";
	e->n_s_c = "";
	e->b_b = "";
	e->b_t = "";
	e->b_n = "";
	e->b_o = "";
	e->n_b = "";
	e->n_t = "";
	e->n_n = "";
	e->n_o = "";
	e->d_t = "";
	e->d_n = "";
	e->d_n_t = "";
	e->d_n_n = "";
	e->flame_id = "";
	e->undercover_name = "";
	// Vectors
	e->neworigin.fill(0);
	e->rotate.fill(0);
	e->goal_min.fill(0);
	e->goal_max.fill(0);
	e->camangle.fill(0);
	e->head_shot_vector.fill(0);
	e->b_commandedViewAngle.fill(0);
	e->b_dest.fill(0);
	e->mouse_emu.fill(0);
	e->b_obstructionDirection.fill(0);
	e->movevect.fill(0);
	e->b_dir.fill(0);
	e->dyn_dest.fill(0);
	e->b_botRole.fill(0);
} // END clearEntityFields()

// When the Progs restarts, all non-const global variables need to be reinitialized to 0 (numbers), empty (strings), `world` (entities), and `SUB_Null` (function pointers).
// Progs `entity`s should never be NULL, nor should Progs function pointers. `entity`s should always point to an entity, and function pointers should always point to /some/
// function, because that is how the QuakeC code was written, and we want the server to get along with general Progs code, without requiring much translation.
void Progs::initGlobalVariables()
{
	// vectors
/*	Progs::trace_endpos.fill(0);
	Progs::trace_plane_normal.fill(0);
	Progs::v_forward.fill(0);
	Progs::v_right.fill(0);
	Progs::v_up.fill(0);
	Progs::blood_org.fill(0);
	Progs::puff_org.fill(0);
	// strings
	Progs::string_null = "";
	Progs::nextmap = "";
	Progs::mapname = "";
	// floats
	Progs::intermission_exittime = 0;
	Progs::intermission_running = 0;
	Progs::modelindex_eyes = 0;
	Progs::modelindex_player = 0;
	Progs::deathmatch = 0;
	Progs::empty_float = 0;
	Progs::force_retouch = 0;
	Progs::found_secrets = 0;
	Progs::fraglimit = 0;
	Progs::framecount = 0;
	Progs::frametime = 0;
	Progs::killed_monsters = 0;
	Progs::movedist = 0;
	Progs::parm1 = 0;
	Progs::parm2 = 0;
	Progs::parm3 = 0;
	Progs::parm4 = 0;
	Progs::parm5 = 0;
	Progs::parm6 = 0;
	Progs::parm7 = 0;
	Progs::parm8 = 0;
	Progs::parm9 = 0;
	Progs::parm10 = 0;
	Progs::parm11 = 0;
	Progs::parm12 = 0;
	Progs::parm13 = 0;
	Progs::parm14 = 0;
	Progs::parm15 = 0;
	Progs::parm16 = 0;
	Progs::rj = 0;
	Progs::teamplay = 0;
	Progs::time = 0;
	Progs::timelimit = 0;
	Progs::total_monsters = 0;
	Progs::total_secrets = 0;
	Progs::trace_allsolid = 0;
	Progs::trace_fraction = 0;
	Progs::trace_inopen = 0;
	Progs::trace_inwater = 0;
	Progs::trace_plane_dist = 0;
	Progs::trace_startsolid = 0;
	Progs::blood_count = 0;
	Progs::multi_damage = 0;
	Progs::puff_count = 0;
	// ints
	Progs::serverflags = 0;
	// entities
	Progs::world = &sv.edicts->v;
	Progs::self = &sv.edicts->v;
	Progs::other = &sv.edicts->v;
	Progs::newmis = &sv.edicts->v;
	Progs::trace_ent = &sv.edicts->v;
	Progs::msg_entity = &sv.edicts->v;
	Progs::activator = &sv.edicts->v;
	Progs::damage_attacker = &sv.edicts->v;
	//Progs::damage_inflictor = &sv.edicts->v;  // PZ: Apparently this vanilla progs global was removed from CuTF progs (or renamed).
	Progs::stemp = &sv.edicts->v;
	Progs::otemp = &sv.edicts->v;
	Progs::s = &sv.edicts->v;
	Progs::old = &sv.edicts->v;
	Progs::multi_ent = &sv.edicts->v;
	Progs::lastspawn = &sv.edicts->v;
	Progs::bodyque_head = &sv.edicts->v;

	// CuTF Entities
	Progs::self = &sv.edicts->v;
	Progs::other = &sv.edicts->v;
	Progs::world = &sv.edicts->v;
	Progs::newmis = &sv.edicts->v;
	Progs::trace_ent = &sv.edicts->v;
	Progs::msg_entity = &sv.edicts->v;
	Progs::activator = &sv.edicts->v;
	Progs::damage_attacker = &sv.edicts->v;
	Progs::debug_target = &sv.edicts->v;
	Progs::current_voteent = &sv.edicts->v;
	Progs::newmis = &sv.edicts->v;
	Progs::headActiveSound = &sv.edicts->v;
	Progs::fixer = &sv.edicts->v;
	Progs::b_waypointCopyList = &sv.edicts->v;
	Progs::route_table = &sv.edicts->v;
	Progs::player_head = &sv.edicts->v;
	Progs::phys_head = &sv.edicts->v;
	Progs::way_head = &sv.edicts->v;
	Progs::way_tail = &sv.edicts->v;
	Progs::b_topicStarter = &sv.edicts->v;
	Progs::solidTestEntity = &sv.edicts->v;
	Progs::multi_ent = &sv.edicts->v;
	Progs::lastspawn = &sv.edicts->v;
	Progs::bodyque_head = &sv.edicts->v;
	Progs::lastspawn_team1 = &sv.edicts->v;
	Progs::lastspawn_team2 = &sv.edicts->v;
	Progs::lastspawn_team3 = &sv.edicts->v;
	Progs::lastspawn_team4 = &sv.edicts->v;
	Progs::stemp = &sv.edicts->v;
	Progs::otemp = &sv.edicts->v;
	Progs::s = &sv.edicts->v;
	Progs::old = &sv.edicts->v;
	Progs::sight_entity = &sv.edicts->v;
	//Progs::_time = &sv.edicts->v;
	Progs::le1 = &sv.edicts->v;
	Progs::le2 = &sv.edicts->v;
	Progs::npc_teamLists = &sv.edicts->v;
	Progs::_npc_lastTeamEntity = &sv.edicts->v;
	Progs::le1 = &sv.edicts->v;
	Progs::le2 = &sv.edicts->v;
	Progs::shub = &sv.edicts->v;   */

	// NEW FULL LIST
	// Entities
	Progs::self = &sv.edicts->v;
	Progs::other = &sv.edicts->v;
	Progs::world = &sv.edicts->v;
	Progs::newmis = &sv.edicts->v;
	Progs::trace_ent = &sv.edicts->v;
	Progs::msg_entity = &sv.edicts->v;
	Progs::activator = &sv.edicts->v;
	Progs::damage_attacker = &sv.edicts->v;
	Progs::debug_target = &sv.edicts->v;
	Progs::current_voteent = &sv.edicts->v;
	Progs::newmis = &sv.edicts->v;
	Progs::headActiveSound = &sv.edicts->v;
	Progs::fixer = &sv.edicts->v;
	Progs::b_waypointCopyList = &sv.edicts->v;
	Progs::route_table = &sv.edicts->v;
	Progs::player_head = &sv.edicts->v;
	Progs::phys_head = &sv.edicts->v;
	Progs::way_head = &sv.edicts->v;
	Progs::way_tail = &sv.edicts->v;
	Progs::b_topicStarter = &sv.edicts->v;
	Progs::solidTestEntity = &sv.edicts->v;
	Progs::multi_ent = &sv.edicts->v;
	Progs::lastspawn = &sv.edicts->v;
	Progs::bodyque_head = &sv.edicts->v;
	Progs::lastspawn_team1 = &sv.edicts->v;
	Progs::lastspawn_team2 = &sv.edicts->v;
	Progs::lastspawn_team3 = &sv.edicts->v;
	Progs::lastspawn_team4 = &sv.edicts->v;
	Progs::stemp = &sv.edicts->v;
	Progs::otemp = &sv.edicts->v;
	Progs::s = &sv.edicts->v;
	Progs::old = &sv.edicts->v;
	Progs::sight_entity = &sv.edicts->v;
	Progs::le1 = &sv.edicts->v;
	Progs::le2 = &sv.edicts->v;
	Progs::npc_teamLists = &sv.edicts->v;
	Progs::_npc_lastTeamEntity = &sv.edicts->v;
	Progs::le1 = &sv.edicts->v;
	Progs::le2 = &sv.edicts->v;
	Progs::shub = &sv.edicts->v;
	Progs::slideMenuController = &sv.edicts->v;
	// Ints and Floats
	Progs::time = 0;
	Progs::frametime = 0;
	Progs::force_retouch = 0;
	Progs::deathmatch = 0;
	Progs::coop = 0;
	Progs::teamplay = 0;
	Progs::serverflags = 0;
	Progs::total_secrets = 0;
	Progs::total_monsters = 0;
	Progs::found_secrets = 0;
	Progs::killed_monsters = 0;
	Progs::parm1 = 0;
	Progs::parm2 = 0;
	Progs::parm3 = 0;
	Progs::parm4 = 0;
	Progs::parm5 = 0;
	Progs::parm6 = 0;
	Progs::parm7 = 0;
	Progs::parm8 = 0;
	Progs::parm9 = 0;
	Progs::parm10 = 0;
	Progs::parm11 = 0;
	Progs::parm12 = 0;
	Progs::parm13 = 0;
	Progs::parm14 = 0;
	Progs::parm15 = 0;
	Progs::parm16 = 0;
	Progs::trace_allsolid = 0;
	Progs::trace_startsolid = 0;
	Progs::trace_fraction = 0;
	Progs::trace_plane_dist = 0;
	Progs::trace_inopen = 0;
	Progs::trace_inwater = 0;
	Progs::movedist = 0;
	//Progs::gameover = 0;
	Progs::empty_float = 0;
	Progs::framecount = 0;
	Progs::skill = 0;
	Progs::teamplay = 0;
	Progs::timelimit = 0;
	Progs::fraglimit = 0;
	Progs::deathmatch = 0;
	Progs::toggleflags = 0;
	Progs::respawn_delay_time = 0;
	Progs::number_of_teams = 0;
	Progs::illegalclasses = 0;
	Progs::illegalclasses1 = 0;
	Progs::illegalclasses2 = 0;
	Progs::illegalclasses3 = 0;
	Progs::illegalclasses4 = 0;
	Progs::civilianteams = 0;
	Progs::team1col = 0;
	Progs::team2col = 0;
	Progs::team3col = 0;
	Progs::team4col = 0;
	Progs::team1score = 0;
	Progs::team2score = 0;
	Progs::team3score = 0;
	Progs::team4score = 0;
	Progs::team1lives = 0;
	Progs::team2lives = 0;
	Progs::team3lives = 0;
	Progs::team4lives = 0;
	Progs::pay_msgs = 0;
	Progs::team_prefix = 0;
	Progs::ceasefire = 0;
	Progs::drop_items = 0;
	Progs::no_grapple = 0;
	Progs::custom_mode = 0;
	Progs::stock_mode = 0;
	Progs::extras_mode = 0;
	Progs::nicecolors = 0;
	Progs::relax_cheatcheck = 0;
	Progs::army_delay = 0;
	Progs::no_detpush = 0;
	Progs::allow_debug = 0;
	Progs::allow_watermonsters = 0;
	Progs::spec_menu = 0;
	Progs::bodyque_size = 0;
	Progs::num_clients = 0;
	Progs::num_players = 0;
	Progs::num_specs = 0;
	Progs::current_brightness = 0;
	Progs::daylight = 0;
	Progs::storm = 0;
	Progs::earthquake = 0;
	Progs::daytime = 0;
	Progs::runes = 0;
	Progs::noon_light = 0;
	Progs::midnight_light = 0;
	Progs::specialstate = 0;
	Progs::intermission_running = 0;
	Progs::sniper_factor = 0;
	Progs::no_punish = 0;
	Progs::no_votemap = 0;
	Progs::no_bright = 0;
	Progs::disabledstuff1 = 0;
	Progs::disabledstuff2 = 0;
	Progs::disabledstuff3 = 0;
	Progs::disabledstuff4 = 0;
	Progs::disabledstuff5 = 0;
	Progs::givenstuff1 = 0;
	Progs::givenstuff2 = 0;
	Progs::givenstuff3 = 0;
	Progs::givenstuff4 = 0;
	Progs::givenstuff5 = 0;
	Progs::megatf = 0;
	Progs::PZ_DEBUG = 0;
	Progs::votesleeping = 0;
	Progs::team1maxplayers = 0;
	Progs::team2maxplayers = 0;
	Progs::team3maxplayers = 0;
	Progs::team4maxplayers = 0;
	Progs::team1advantage = 0;
	Progs::team2advantage = 0;
	Progs::team3advantage = 0;
	Progs::team4advantage = 0;
	Progs::team1nextspam = 0;
	Progs::team2nextspam = 0;
	Progs::team3nextspam = 0;
	Progs::team4nextspam = 0;
	Progs::CTF_Map = 0;
	Progs::coop = 0;
	Progs::rj = 0;
	Progs::item_list_bit = 0;
	Progs::agr = 0;
	Progs::agr_frags = 0;
	Progs::agr_minutes = 0;
	Progs::agr_vote_max = 0;
	Progs::agr_teamfrags = 0;
	Progs::agrTimeToRewardBlue = 0;
	Progs::agrTimerRunning = 0;
	Progs::agrTimeOfLastSound = 0;
	Progs::agrFlagRespawnWait = 0;
	Progs::invade = 0;
	Progs::invade_frags = 0;
	Progs::invade_minutes = 0;
	Progs::invade_teamfrags = 0;
	Progs::invade_prematchTime = 0;
	Progs::invade_gameState = InvadeGameState::STOPPED;
	Progs::invade_teamOnDef = 0;
	Progs::invade_swapRoles = 0;
	Progs::invade_timePrematchEnds = 0;
	Progs::invade_timeOfLastSound = 0;
	Progs::invade_timeToRewardDef = 0;
	//Progs::neo = 0;
	//Progs::neoLives = world;
	//Progs::lastNadeRegen = 0;
	//Progs::lastDetRegen = 0;
	//Progs::neoDamageFactor = 0;
	//Progs::neoResistFactor = 0;
	//Progs::neoRegenFactor = 0;
	Progs::playersOnTeam1 = 0;
	Progs::playersOnTeam2 = 0;
	Progs::playersOnTeam3 = 0;
	Progs::playersOnTeam4 = 0;
	Progs::num_world_flames = 0;
	Progs::num_world_pipebombs = 0;
	Progs::num_team_pipebombs_1 = 0;
	Progs::num_team_pipebombs_2 = 0;
	Progs::num_team_pipebombs_3 = 0;
	Progs::num_team_pipebombs_4 = 0;
	Progs::num_world_ammoboxes = 0;
	Progs::num_team_ammoboxes_1 = 0;
	Progs::num_team_ammoboxes_2 = 0;
	Progs::num_team_ammoboxes_3 = 0;
	Progs::num_team_ammoboxes_4 = 0;
	Progs::spy_off = 0;
	Progs::invis_only = 0;
	Progs::allow_hook = 0;
	Progs::jello = 0;
	Progs::light_damage = 0;
	Progs::prematch = 0;
	Progs::bounty = 0;
	Progs::custom_money = 0;
	Progs::team_with_flag = 0;
	Progs::friends1_mask = 0;
	Progs::friends2_mask = 0;
	Progs::friends3_mask = 0;
	Progs::friends4_mask = 0;
	Progs::normalQuakeMap = 0;
	//Progs::live_camera = 0;
	Progs::already_chosen_map = 0;
	Progs::triggered_cycle = 0;
	Progs::deathmsg = 0;
	Progs::server_spawned = 0;
	//Progs::precached_sounds = 0;
	//Progs::precached_models = 0;
	Progs::modelindex_eyes = 0;
	Progs::modelindex_player = 0;
	Progs::modelindex_null = 0;
	Progs::max_clients = 0;
	Progs::real_frametime = 0;
	Progs::averageFrameTime = 0;
	Progs::bot_frameCounter = 0;
	Progs::bot_count = 0;
	Progs::b_options = 0;
	Progs::lasttime = 0;
	Progs::waypoint_mode = 0;
	Progs::dump_mode = 0;
	Progs::b_copyMode = 0;
	Progs::b_copyAcrossXAxis = 0;
	INIT_ARRAY(Progs::b_directionToFace, 32);
	Progs::b_slideDirection = 0;
	Progs::b_slideSpeed = 0;
	Progs::waypoints = 0;
	Progs::direct_route = 0;
	Progs::userid = 0;
	Progs::sv_friction = 0;
	Progs::sv_gravity = 0;
	Progs::sv_accelerate = 0;
	Progs::sv_maxspeed = 0;
	Progs::sv_stopspeed = 0;
	Progs::busy_waypoints = 0;
	Progs::saved_bots = 0;
	Progs::saved_skills1 = 0;
	Progs::saved_skills2 = 0;
	Progs::current_bots = 0;
	Progs::saved1 = 0;
	Progs::saved2 = 0;
	Progs::saved3 = 0;
	Progs::scratch1 = 0;
	Progs::scratch2 = 0;
	Progs::scratch3 = 0;
	Progs::scratch4 = 0;
	Progs::bytecounter = 0;
	Progs::filecount = 0;
	Progs::coop = 0;
	Progs::collectiveHumanScore = 0;
	Progs::lastCollectiveHumanScore = 0;
	Progs::frik_mainRoutine_frameTimeAccumulator = 0;
	Progs::lastTimeBotCountAdjusted = 0;
	Progs::lastTimeDecrementedBotSkill = 0;
	Progs::frik_mainRoutine_LastNumberOfHumans = 0;
	Progs::frik_updateWaypointVisualMarkers_lastTimeToggled = 0;
	Progs::frik_updateWaypointVisualMarkers_copiesVisible = 0;
	Progs::modelindex_eyes = 0;
	Progs::stagger_think = 0;
	Progs::runOnce = 0;
	Progs::b_topic = 0;
	Progs::file = 0;
	Progs::menu_sounds = 0;
	Progs::modelindex_eyes = 0;
	Progs::modelindex_player = 0;
	Progs::modelindex_null = 0;
	Progs::modelindex_eyes = 0;
	Progs::modelindex_player = 0;
	Progs::modelindex_null = 0;
	Progs::cool_gibs = 0;
	Progs::unlockedZeroGrav = 0;
	Progs::multi_damage = 0;
	Progs::blood_count = 0;
	Progs::puff_count = 0;
	Progs::StartFrame_initialized = 0;
	Progs::modelindex_eyes = 0;
	Progs::modelindex_player = 0;
	Progs::modelindex_null = 0;
	Progs::intermission_exittime = 0;
	Progs::already_cycled = 0;
	Progs::kickable_heads = 0;
	Progs::current_yaw = 0;
	Progs::enemy_yaw = 0;
	Progs::enemy_vis = 0;
	Progs::enemy_infront = 0;
	Progs::enemy_range = 0;
	Progs::sight_entity_time = 0;
	Progs::NumGremlins = 0;
	Progs::NumSpawnGremlins = 0;
	Progs::modelindex_tesla = 0;
	Progs::lightning_end = 0;
	Progs::modelindex_tesla = 0;
	// DON'T KNOW WHY AGR AND INVADE STUFF IS IN PROGDEFS.H+CPP TWICE.
	/*Progs::agr = 0;
	Progs::agr_frags = 0;
	Progs::agr_minutes = 0;
	Progs::agr_vote_max = 0;
	Progs::agr_teamfrags = 0;
	Progs::agrTimeToRewardBlue = 0;
	Progs::agrTimerRunning = 0;
	Progs::agrTimeOfLastSound = 0;
	Progs::agrFlagRespawnWait = 0;
	Progs::invade = 0;
	Progs::invade_teamfrags = 0;
	Progs::invade_prematchTime = 0;
	Progs::invade_teamOnDef = 0;
	Progs::invade_swapRoles = 0;
	Progs::invade_prematch = 0;
	Progs::invade_timePrematchEnds = 0;
	Progs::invade_gameRunning = 0;*/
	Progs::spawner_id = 0;
	Progs::monster_mode = 0;
	Progs::worldscan_delay = 0;
	Progs::lightning_end = 0;
	Progs::hknight_type = 0;
	// Strings
	Progs::mapname = "";
	Progs::string_null = "";
	Progs::last_vote_starter = "";
	Progs::team_menu_string = "";
	Progs::nextmap = "";
	// Vectors
	Progs::v_forward.fill(0);
	Progs::v_up.fill(0);
	Progs::v_right.fill(0);
	Progs::trace_endpos.fill(0);
	Progs::trace_plane_normal.fill(0);
	Progs::vector_null.fill(0);
	Progs::b_activeClientSlots.fill(0);
	Progs::b_mapCenter.fill(0);
	Progs::b_goToCoordinate.fill(0);
	Progs::blood_org.fill(0);
	Progs::puff_org.fill(0);
} // END initGlobalVariables()
