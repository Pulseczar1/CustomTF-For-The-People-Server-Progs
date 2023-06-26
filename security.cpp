/*======================================================
	SECURITY.QC			Custom TeamFortress v3.1

	(c) Craig Hauser			26/3/00
========================================================
Functions for the security camera
======================================================*/

#include "progs.h"
#include "optimize.h"
#include "debug.h"
#include "invade.h"
#include "weapons.h"
#include "cpstuff.h"
#include "tforttm.h"

namespace Progs {

void Security_Camera_Idle();
float Security_Camera_FindTarget();
void Security_Camera_Pain(entity attacker, float damage);
void Security_Camera_Die();
void Security_Camera_PrintTarget();
void Security_Camera_Spawn();
float Security_Camera_FindFakeTarget();
void SecurityCameraTossTouch();

//used for print
string GetTrueTeamName(float tno);
string TeamFortress_GetClassName(float pc);
string TeamFortress_GetJobName(int pc);

//===================================================================
void Security_Camera_Idle()
{
	if (Security_Camera_FindTarget())
		self->nextthink = time + 2; //CH if found wait 2 sec before do another check
	else
		self->nextthink = time + 0.5; //CH else 2 checks per sec
	self->think = Security_Camera_Idle;
}
//========
float Security_Camera_FindTarget()
{
	entity client = world;
	float	/*r,*/ gotone, loopc;

	if (self->is_malfunctioning & PR_SCREWUP_TWO) // SB how tragic, camera can't see
		return PR_FALSE;

	if (self->is_malfunctioning & PR_SCREWUP_THREE) // oh no, we're reading incorrect signals
	{
		if (Security_Camera_FindFakeTarget())
			return PR_TRUE;
		else
			return PR_FALSE;
	}

	// Try a few checks to make it react faster
	//r = 0;
	loopc = 0;
	gotone = PR_FALSE;

    float trange; //- OfN - Hack
    trange=1000;

    if (self->all_active & PR_IMPROVED_ONE)
        trange=1250;

	//CH Theortetically this will check every client on the server now
	while (loopc < 32 && gotone == PR_FALSE)
	{
		client = checkclient();
		gotone = PR_TRUE;

		if (client == world)
			gotone = PR_FALSE;

		if (!Pharse_Client(client, self, 1, trange, 0, 1))
			gotone = PR_FALSE;

		loopc = loopc + 1;
		if (gotone) loopc = 1000;
	}

	if (!gotone)
		return PR_FALSE;

	// Found a Target
	self->enemy = client;
	if (self->enemy->classname != "player")
	{
		self->enemy = self->enemy->enemy;
		if (self->enemy->classname != "player")
		{
			self->enemy = world;
			return PR_FALSE;
		}
	}

	// Spotted sound
	if (self->heat == 3) { //CH so it does not beep all the time.
		sound (self, PR_CHAN_WEAPON, "weapons/cambeep.wav", 1, PR_ATTN_NORM);
		self->heat = 0;
	}
	else
		self->heat = self->heat + 1;

	Security_Camera_PrintTarget ();

	return PR_TRUE;
}

float Security_Camera_FindFakeTarget()
{
	entity client = world;
	float	/*r,*/ gotone, loopc;

	if (self->is_malfunctioning & PR_SCREWUP_THREE) // SB how tragic, camera can't see
		return PR_FALSE;

	// Try a few checks to make it react faster
	//r = 0;
	loopc = 0;
	gotone = PR_FALSE;
	// Pick a random client
	while (loopc < 32 && gotone == PR_FALSE)
	{
		client = checkclient();
		gotone = PR_TRUE;

		if (client == world)
			gotone = PR_FALSE;

		if (random() < 0.05)
			gotone = PR_FALSE;

		if (client->playerclass == PR_PC_UNDEFINED) {
			return PR_FALSE;
		}
		if (client->done_custom & PR_CUSTOM_BUILDING) {
			return PR_FALSE;
		}
		if (client->health <= 0) {
			return PR_FALSE;
		}
		if (!client->is_connected) {
			return PR_FALSE;
		}
		loopc = loopc + 1;
		if (gotone) loopc = 1000;
	}

	if (!gotone)
		return PR_FALSE;

	// Found a Target
	self->enemy = client;
	if (self->enemy->classname != "player")
	{
		self->enemy = self->enemy->enemy;
		if (self->enemy->classname != "player")
		{
			self->enemy = world;
			return PR_FALSE;
		}
	}

	// Spotted sound
	if (self->heat == 3) { //CH so it does not beep all the time.
		sound (self, PR_CHAN_WEAPON, "weapons/cambeep.wav", 1, PR_ATTN_NORM);
		self->heat = 0;
	}
	else
		self->heat = self->heat + 1;

	if (!(self->is_malfunctioning & PR_SCREWUP_ONE))
		Security_Camera_PrintTarget ();

	return PR_TRUE;
}
//========
void Security_Camera_Pain(entity attacker, float damage)
{
	sound(self, PR_CHAN_WEAPON, "weapons/cambeep.wav", 1, PR_ATTN_NORM);
}
//======
void Security_Camera_Die()
{
	sprint(self->real_owner, PR_PRINT_HIGH, "Your security camera was destroyed.\n");
	self->real_owner->has_camera = PR_FALSE;

//	ThrowGib("progs/tgib1.mdl", -70);
//	ThrowGib("progs/tgib2.mdl", -70);
//	ThrowGib("progs/tgib3.mdl", -70);

	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
	WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
#ifdef PR_QUAKE_WORLD
	multicast (self->origin, PR_MULTICAST_PHS);
    dremove(self);
#else
    BecomeExplosion ();
#endif
    //SwitchFromCamera();
}
//==========
//Prints the person to owner/team
//Outputs: "<>SECURITY CAMERA<>  Team_color class/job (netname)\n"
void Security_Camera_PrintTarget()
{
	string st;

	sprint(self->real_owner, PR_PRINT_HIGH, S_("^<^>^bSECURITY CAMERA^<^>^b  ")); //<>Security camera<>
	// PZ: support teamplay 0 (deathmatch, basically)
	if (teamplay)
	{
		st = GetTrueTeamName(self->enemy->team_no);
		sprint(self->real_owner, PR_PRINT_HIGH, st);
		sprint(self->real_owner, PR_PRINT_HIGH, " ");
	}
	if (self->enemy->playerclass != PR_PC_CUSTOM)
		st = TeamFortress_GetClassName(self->enemy->playerclass);
	else
		st = TeamFortress_GetJobName(self->enemy->job);
	sprint(self->real_owner, PR_PRINT_HIGH, st);
	sprint(self->real_owner, PR_PRINT_HIGH, " (");
	sprint(self->real_owner, PR_PRINT_HIGH, self->enemy->netname);
	sprint(self->real_owner, PR_PRINT_HIGH, ")\n");
}
//==========
void Security_Camera_Spawn()
{
	if (!Invade_buffPlayer(self) && (self->ammo_cells < PR_BUILD_COST_CAMERA))
	{
		sprint(self,PR_PRINT_HIGH,"Not enough cells to build a Security Camera\n");
	}
	else
	{
		if (!Invade_buffPlayer(self))
			self->ammo_cells = self->ammo_cells - PR_BUILD_COST_CAMERA;
		self->has_camera = PR_TRUE;
		newmis = spawn();
		newmis->movetype = PR_MOVETYPE_BOUNCE;
		setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));
	//	setsize (newmis, '-8 -8 -8', '8 8 8');
		newmis->solid = PR_SOLID_BBOX;
		newmis->takedamage = PR_DAMAGE_AIM;
		newmis->classname = "building_camera";
		newmis->netname = "security_camera";
		newmis->flags = newmis->flags | PR_FL_MACHINE; // PZ: mark it with the FL_MACHINE flag
		setorigin (newmis, self->origin);
		newmis->owner = world;
		newmis->real_owner = self;
		makevectors (self->v_angle);
		newmis->avelocity = V({0, 0, 0});
		newmis->velocity = v_forward*800 + v_up * 200 + v_right*10 + v_up*10;
		newmis->angles = V({0, 0, 0});
		newmis->angles[Y] = anglemod(self->angles[Y] + 180);
	//	newmis.skin = 1;
		newmis->th_die = Security_Camera_Die; 		// Death function
		newmis->th_pain = Security_Camera_Pain;
		newmis->mdl = "progs/camera.mdl"; //CH temp model
		setmodel (newmis, newmis->mdl);
		newmis->team_no = self->team_no;
		newmis->colormap = self->colormap;
		newmis->heat = 0; //Beeps

		newmis->health = newmis->max_health = PR_BUILD_HEALTH_CAMERA;
		newmis->touch = SecurityCameraTossTouch;

		newmis->all_active=0; // OfN - reset HACKER improvements

		W_SetCurrentAmmo();
	}
}
//========
void SecurityCameraTossTouch()
{
	if (other != world || other == self->real_owner)
		return;
	if (pointcontents(self->origin) == PR_CONTENT_SKY || pointcontents(self->origin + V({0, 0, 2})) == PR_CONTENT_SKY || pointcontents(self->origin) == PR_CONTENT_SOLID)
	{
		Security_Camera_Die();
		return;
	}
	//CH sees where landed and adjusts to proper things
	if (pointcontents(self->origin + V({0, 0, 1})) == PR_CONTENT_SOLID)
		self->origin = self->origin - V({0, 0, 12});
	if (pointcontents(self->origin - V({0, 0, 1})) == PR_CONTENT_SOLID)
		self->origin = self->origin + V({0, 0, 4});
	if (pointcontents(self->origin + V({0, 1, 0})) == PR_CONTENT_SOLID)
		self->origin = self->origin - V({0, 16, 0});
	if (pointcontents(self->origin - V({0, 1, 0})) == PR_CONTENT_SOLID)
		self->origin = self->origin + V({0, 16, 0});
	if (pointcontents(self->origin + V({1, 0, 0})) == PR_CONTENT_SOLID)
		self->origin = self->origin - V({16, 0, 0});
	if (pointcontents(self->origin + V({1, 0, 0})) == PR_CONTENT_SOLID)
		self->origin = self->origin + V({16, 0, 0});
	setorigin (self, self->origin);
	if (pointcontents(self->origin) == PR_CONTENT_SKY || pointcontents(self->origin + V({0, 0, 2})) == PR_CONTENT_SKY || pointcontents(self->origin) == PR_CONTENT_SOLID)
	{
		Security_Camera_Die();
		return;
	}
	sprint (self->real_owner, PR_PRINT_HIGH, "You finish building the Security Camera.\n");

    teamprefixsprint(self->real_owner->team_no,self->real_owner); //- OfN
    teamsprint(self->real_owner->team_no, self->real_owner, self->real_owner->netname);
	teamsprint(self->real_owner->team_no, self->real_owner, " has built a Security Camera.\n");

    self->real_owner->option = time + 2; // so ppl cant destroy it for 2 seconds

	self->movetype = PR_MOVETYPE_NONE;
	setsize (self, V({-16, -16, -6}), V({16, 16, 10}));
	self->solid = PR_SOLID_BBOX;
	self->takedamage = PR_DAMAGE_AIM;
	sound (self, PR_CHAN_WEAPON, "weapons/guerset.wav", 1, PR_ATTN_NORM);
	self->think = Security_Camera_Idle;
	self->nextthink = time + 1;
}

} // END namespace Progs
