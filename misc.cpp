#include "progs.h"
#include "debug.h"
#include "tfortmap.h"
#include "combat.h"
#include "weapons.h"
#include "subs.h"

namespace Progs {

/*QUAKED info_null (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for spotlights, etc.
*/
void info_null()
{
	dremove(self);
}

/*QUAKED info_notnull (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for lightning.
*/
void info_notnull()
{
}

//============================================================================

#define PR_START_OFF	1

void light_use()
{
	if (self->spawnflags & PR_START_OFF)
	{
		lightstyle(self->style, "m");
		self->spawnflags = self->spawnflags - PR_START_OFF;
	}
	else
	{
		lightstyle(self->style, "a");
		self->spawnflags = self->spawnflags + PR_START_OFF;
	}
}

/*QUAKED light (0 1 0) (-8 -8 -8) (8 8 8) #START_OFF
Non-displayed light.
Default light value is 300
Default style is 0
If targeted, it will toggle between on or off.
*/
void light()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	if (self->targetname == "")
	{	// inert light
		dremove(self);
		return;
	}

	if (self->style >= 32)
	{
		self->use = light_use;
		if (self->spawnflags & PR_START_OFF)
			lightstyle(self->style, "a");
		else
			lightstyle(self->style, "m");
	}
}

/*QUAKED light_fluoro (0 1 0) (-8 -8 -8) (8 8 8) #START_OFF
Non-displayed light.
Default light value is 300
Default style is 0
If targeted, it will toggle between on or off.
Makes steady fluorescent humming sound
*/
void light_fluoro()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	if (self->style >= 32)
	{
		self->use = light_use;
		if (self->spawnflags & PR_START_OFF)
			lightstyle(self->style, "a");
		else
			lightstyle(self->style, "m");
	}

	precache_sound ("ambience/fl_hum1.wav");
	ambientsound (self->origin, "ambience/fl_hum1.wav", 0.5, PR_ATTN_STATIC);
}

/*QUAKED light_fluorospark (0 1 0) (-8 -8 -8) (8 8 8)
Non-displayed light.
Default light value is 300
Default style is 10
Makes sparking, broken fluorescent sound
*/
void light_fluorospark()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	if (!self->style)
		self->style = 10;

	precache_sound ("ambience/buzz1.wav");
	ambientsound (self->origin, "ambience/buzz1.wav", 0.5, PR_ATTN_STATIC);
}

/*QUAKED light_globe (0 1 0) (-8 -8 -8) (8 8 8)
Sphere globe light.
Default light value is 300
Default style is 0
*/
void light_globe()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	precache_model ("progs/s_light.spr");
	setmodel (self, "progs/s_light.spr");
	makestatic (self);
}

void FireAmbient()
{
	precache_sound ("ambience/fire1.wav");
// attenuate fast
	ambientsound (self->origin, "ambience/fire1.wav", 0.5, PR_ATTN_STATIC);
}

/*QUAKED light_torch_small_walltorch (0 .5 0) (-10 -10 -20) (10 10 20)
Short wall torch
Default light value is 200
Default style is 0
*/
void light_torch_small_walltorch()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	precache_model ("progs/flame.mdl");
	setmodel (self, "progs/flame.mdl");
	FireAmbient ();
	makestatic (self);
}

/*QUAKED light_flame_large_yellow (0 1 0) (-10 -10 -12) (12 12 18)
Large yellow flame ball
*/
void light_flame_large_yellow()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	precache_model ("progs/flame2.mdl");
	setmodel (self, "progs/flame2.mdl");
	self->frame = 1;
	FireAmbient ();
	makestatic (self);
}

/*QUAKED light_flame_small_yellow (0 1 0) (-8 -8 -8) (8 8 8) #START_OFF
Small yellow flame ball
*/
void light_flame_small_yellow()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	precache_model ("progs/flame2.mdl");
	setmodel (self, "progs/flame2.mdl");
	FireAmbient ();
	makestatic (self);
}

/*QUAKED light_flame_small_white (0 1 0) (-10 -10 -40) (10 10 40) #START_OFF
Small white flame ball
*/
void light_flame_small_white()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	precache_model ("progs/flame2.mdl");
	setmodel (self, "progs/flame2.mdl");
	FireAmbient ();
	makestatic (self);
}

//============================================================================


/*QUAKED misc_fireball (0 .5 .8) (-8 -8 -8) (8 8 8)
Lava Balls
*/

void fire_fly();
void fire_touch();
void misc_fireball()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	precache_model ("progs/lavaball.mdl");
	self->classname = "fireball";
	self->nextthink = time + (random() * 5);
	self->think = fire_fly;
	if (!self->speed)
		self->speed = 1000;
}

void fire_fly()
{
	newmis = spawn();
	newmis->solid = PR_SOLID_TRIGGER;
	newmis->movetype = PR_MOVETYPE_TOSS;
	newmis->velocity = V({0, 0, 1000});
	newmis->velocity[X] = (random() * 100) - 50;
	newmis->velocity[Y] = (random() * 100) - 50;
	newmis->velocity[Z] = self->speed + (random() * 200);
	newmis->classname = "fireball";
	setmodel (newmis, "progs/lavaball.mdl");
	setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (newmis, self->origin);
	newmis->nextthink = time + 5;
	newmis->think = SUB_Remove;
	newmis->touch = fire_touch;

	self->nextthink = time + (random() * 5) + 3;
	self->think = fire_fly;
}


void fire_touch()
{
	if (other->takedamage)
		TF_T_Damage (other, self, self, 20, 0, PR_TF_TD_FIRE);
	dremove(self);
}

//============================================================================


void barrel_explode()
{
	self->takedamage = PR_DAMAGE_NO;
	self->classname = "explo_box";
	// did say self.owner
	T_RadiusDamage (self, self, 160, world);

#ifndef PR_QUAKE_WORLD
	sound (self, PR_CHAN_VOICE, "weapons/r_exp3.wav", 1, PR_ATTN_NORM);
	particle (self->origin, V({0, 0, 0}), 75, 255);

	self->origin[Z] = self->origin[Z] + 32;
	BecomeExplosion ();
#else
	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
	WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Z]+32);
	multicast (self->origin, PR_MULTICAST_PHS);
	remove (self);
#endif
}



/*QUAKED misc_explobox (0 .5 .8) (0 0 0) (32 32 64)
TESTING THING
*/

void misc_explobox()
{
	float	oldz;

	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	self->solid = PR_SOLID_BBOX;
	self->movetype = PR_MOVETYPE_NONE;
	precache_model ("maps/b_explob.bsp");
	setmodel (self, "maps/b_explob.bsp");
	setsize (self, V({0, 0, 0}), V({32, 32, 64}));
	precache_sound ("weapons/r_exp3.wav");
	self->health = 20;
	self->th_die = barrel_explode;
	self->takedamage = PR_DAMAGE_AIM;

	self->origin[Z] = self->origin[Z] + 2;
	oldz = self->origin[Z];
	droptofloor();
	if (oldz - self->origin[Z] > 250)
	{
		RPrint ("item fell out of level at ");
		RPrint (vtos(self->origin));
		RPrint ("\n");
		dremove(self);
	}
}




/*QUAKED misc_explobox2 (0 .5 .8) (0 0 0) (32 32 64)
Smaller exploding box, REGISTERED ONLY
*/

void misc_explobox2()
{
	float	oldz;

	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	self->solid = PR_SOLID_BBOX;
	self->movetype = PR_MOVETYPE_NONE;
	precache_model2 ("maps/b_exbox2.bsp");
	setmodel (self, "maps/b_exbox2.bsp");
	setsize (self, V({0, 0, 0}), V({32, 32, 32}));
	precache_sound ("weapons/r_exp3.wav");
	self->health = 20;
	self->th_die = barrel_explode;
	self->takedamage = PR_DAMAGE_AIM;

	self->origin[Z] = self->origin[Z] + 2;
	oldz = self->origin[Z];
	droptofloor();
	if (oldz - self->origin[Z] > 250)
	{
		RPrint ("item fell out of level at ");
		RPrint (vtos(self->origin));
		RPrint ("\n");
		dremove(self);
	}
}

//============================================================================

void LaunchLaser(const vector& org, vector vec);
void Laser_Touch();
#ifndef PR_COOP_MODE
/*=====================
from ENFORCER.QC
======================*/
void Laser_Touch()
{
	vector org;

	if (other == self->owner)
		return;		// don't explode on owner

	if (pointcontents(self->origin) == PR_CONTENT_SKY)
	{
		dremove(self);
		return;
	}

	sound (self, PR_CHAN_WEAPON, "enforcer/enfstop.wav", 1, PR_ATTN_STATIC);
	org = self->origin - 8*normalize(self->velocity);

	if (other->health)
	{
		SpawnBlood (org, 15);
		TF_T_Damage (other, self, self->owner, 15, 0, PR_TF_TD_ELECTRICITY);
	}
	else
	{
		WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_BROADCAST, PR_TE_GUNSHOT);
	#ifdef PR_QUAKE_WORLD
		WriteByte (PR_MSG_MULTICAST, 5);
	#endif
		WriteCoord (PR_MSG_BROADCAST, org[X]);
		WriteCoord (PR_MSG_BROADCAST, org[Y]);
		WriteCoord (PR_MSG_BROADCAST, org[Z]);
	#ifdef PR_QUAKE_WORLD
		multicast (org, PR_MULTICAST_PVS);
	#endif
	}

	dremove(self);
}

/*=====================
from ENFORCER.QC
======================*/

void LaunchLaser(const vector& org, vector vec)
{
	if (self->classname == "monster_enforcer")
		sound (self, PR_CHAN_WEAPON, "enforcer/enfire.wav", 1, PR_ATTN_NORM);

	vec = normalize(vec);

	newmis = spawn();
	newmis->owner = self;
	newmis->movetype = PR_MOVETYPE_FLY;
	newmis->solid = PR_SOLID_BBOX;
	newmis->effects = PR_EF_DIMLIGHT;

	setmodel (newmis, "progs/laser.mdl");
	setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));

	setorigin (newmis, org);

	newmis->velocity = vec * 600;
	newmis->angles = vectoangles(newmis->velocity);

	newmis->nextthink = time + 5;
	newmis->think = SUB_Remove;
	newmis->touch = Laser_Touch;
}
#endif

//
//CH  ====From weapons.qc====
//
void TFFireRocket(const vector& org, vector vec)
{
	vec = normalize(vec);

//?	KickPlayer(-2, self);
	newmis = spawn ();
	newmis->owner = self;
	newmis->movetype = PR_MOVETYPE_FLYMISSILE;
	newmis->solid = PR_SOLID_BBOX;

	// set newmis speed
	makevectors (self->v_angle);
	newmis->velocity = vec * 900;
	newmis->angles = vectoangles(newmis->velocity);

	newmis->touch = T_MissileTouch;

	// set newmis duration
	newmis->nextthink = time + 5;
	newmis->think = SUB_Remove;

	newmis->weapon = PR_DMSG_ROCKETL;
	setmodel (newmis, "progs/missile.mdl");
	setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (newmis, org);
}

/*
================
W_FireGrenade
================
*/
void TFFireGrenade(const vector& org, vector vec)
{
	vec = normalize(vec);

//?	KickPlayer(-2, self);
	newmis = spawn ();
	newmis->owner = self;
	newmis->movetype = PR_MOVETYPE_BOUNCE;
	newmis->solid = PR_SOLID_BBOX;

	newmis->weapon = PR_DMSG_GRENADEL;
	newmis->classname = "grenade";
	newmis->skin = 1;
	newmis->touch = GrenadeTouch;
	newmis->nextthink = time + 1.5; //CH 2.5 is too long for the gren

	// set newmis speed
	makevectors (self->v_angle);
	if (self->v_angle[X])
		newmis->velocity = v_forward*600 + v_up * 200 + crandom()*v_right*10 + crandom()*v_up*10;
	else
	{
		newmis->velocity = aim(self, 10000);
		newmis->velocity = vec * 600;
		newmis->velocity[Z] = 100; //was 200
	}
	newmis->avelocity = V({300, 300, 300});
	newmis->angles = vectoangles(newmis->velocity);

	newmis->think = GrenadeExplode;
	setmodel (newmis, "progs/grenade2.mdl");
	setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (newmis, org);
}

//
//CH  ====pyro.qc====
//
void Flamer_stream_touch();
void TFFireFlame(const vector& org, vector vec)
{
	entity flame;

	vec = normalize(vec);

    flame = spawn ();

    flame->owner = self;
    flame->movetype = PR_MOVETYPE_FLYMISSILE;
    flame->solid = PR_SOLID_BBOX;
    flame->classname = "flamerflame";

	makevectors (self->v_angle);

    flame->velocity = aim(self, 10000);
    flame->velocity = vec * 600;

    flame->angles = vectoangles(flame->velocity);

    flame->touch = Flamer_stream_touch;

    flame->think = s_explode1;
    flame->nextthink = time + 0.15;

    setmodel (flame, "progs/s_explod.spr");
    setsize (flame, V({0, 0, 0}), V({0, 0, 0}));
    setorigin (flame, org);
}

void spikeshooter_use()
{
	if (self->spawnflags & PR_SPAWNFLAG_LASER)
	{
		sound (self, PR_CHAN_VOICE, "enforcer/enfire.wav", 1, PR_ATTN_NORM);
		LaunchLaser (self->origin, self->movedir);
	}
	else if (self->spawnflags & PR_SPAWNFLAG_TFROCKET)
	{
		sound (self, PR_CHAN_VOICE, "weapons/sgun1.wav", 1, PR_ATTN_NORM);
		TFFireRocket (self->origin, self->movedir);
	}
	else if (self->spawnflags & PR_SPAWNFLAG_TFGRENADE)
	{
		sound (self, PR_CHAN_VOICE, "weapons/grenade.wav", 1, PR_ATTN_NORM);
		TFFireGrenade (self->origin, self->movedir);
	}
	else if (self->spawnflags & PR_SPAWNFLAG_TFFLAME)
	{
		sound (self, PR_CHAN_VOICE, "weapons/flmfire2.wav", 1, PR_ATTN_NORM);
		TFFireFlame (self->origin, self->movedir);
	}
	else
	{
		sound (self, PR_CHAN_VOICE, "weapons/spike2.wav", 1, PR_ATTN_NORM);
		launch_spike (self->origin, self->movedir);
		newmis->velocity = self->movedir * 500;
		if (self->spawnflags & PR_SPAWNFLAG_SUPERSPIKE)
			newmis->touch = spike_touch;
	}
}

void shooter_think()
{
	spikeshooter_use ();
	self->nextthink = time + self->wait;
	newmis->velocity = self->movedir * 500;
}


/*QUAKED trap_spikeshooter (0 .5 .8) (-8 -8 -8) (8 8 8) superspike laser
When triggered, fires a spike in the direction set in QuakeEd.
Laser is only for REGISTERED.
*/

void trap_spikeshooter()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	SetMovedir ();
	self->use = spikeshooter_use;
	if (self->spawnflags & PR_SPAWNFLAG_LASER)
	{
		precache_model2 ("progs/laser.mdl");

		precache_sound2 ("enforcer/enfire.wav");
		precache_sound2 ("enforcer/enfstop.wav");
	}
	else
		precache_sound ("weapons/spike2.wav");
}

void trap_tf_spikeshooter()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	SetMovedir ();
	self->use = spikeshooter_use;
	if (self->spawnflags & PR_SPAWNFLAG_LASER)
	{
		precache_model2 ("progs/laser.mdl");

		precache_sound2 ("enforcer/enfire.wav");
		precache_sound2 ("enforcer/enfstop.wav");
	}
	else
		precache_sound ("weapons/spike2.wav");
}


/*QUAKED trap_shooter (0 .5 .8) (-8 -8 -8) (8 8 8) superspike laser
Continuously fires spikes.
"wait" time between spike (1.0 default)
"nextthink" delay before firing first spike, so multiple shooters can be stagered.
*/
void trap_shooter()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	trap_spikeshooter ();

	if (self->wait == 0)
		self->wait = 1;
	self->nextthink = self->nextthink + self->wait + self->ltime;
	self->think = shooter_think;
}

void trap_tf_shooter()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	trap_spikeshooter ();

	if (self->wait == 0)
		self->wait = 1;
	self->nextthink = self->nextthink + self->wait + self->ltime;
	self->think = shooter_think;
}


/*
===============================================================================


===============================================================================
*/


void make_bubbles();
void bubble_remove();
void bubble_bob();

/*QUAKED air_bubbles (0 .5 .8) (-8 -8 -8) (8 8 8)

testing air bubbles
*/

void air_bubbles()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

#ifndef PR_QUAKE_WORLD
	if (deathmatch)
	{
		remove (self);
		return;
	}
	precache_model ("progs/s_bubble.spr");
	self->nextthink = time + 1;
	self->think = make_bubbles;
#else
	dremove(self);
#endif
}

void make_bubbles()
{
	newmis = spawn();
	setmodel (newmis, "progs/s_bubble.spr");
	setorigin (newmis, self->origin);
	newmis->movetype = PR_MOVETYPE_NOCLIP;
	newmis->solid = PR_SOLID_NOT;
	newmis->velocity = V({0, 0, 15});
	newmis->nextthink = time + 0.5;
	newmis->think = bubble_bob;
	newmis->touch = bubble_remove;
	newmis->classname = "bubble";
	newmis->frame = 0;
	newmis->cnt = 0;
	setsize (newmis, V({-8, -8, -8}), V({8, 8, 8}));
	self->nextthink = time + random() + 0.5;
	self->think = make_bubbles;
}

void bubble_split()
{
	newmis = spawn();
	setmodel (newmis, "progs/s_bubble.spr");
	setorigin (newmis, self->origin);
	newmis->movetype = PR_MOVETYPE_NOCLIP;
	newmis->solid = PR_SOLID_NOT;
	newmis->velocity = self->velocity;
	newmis->nextthink = time + 0.5;
	newmis->think = bubble_bob;
	newmis->touch = bubble_remove;
	newmis->classname = "bubble";
	newmis->frame = 1;
	newmis->cnt = 10;
	setsize (newmis, V({-8, -8, -8}), V({8, 8, 8}));
	self->frame = 1;
	self->cnt = 10;
	if (self->waterlevel != 3)
		remove (self);
}

void bubble_remove()
{
	if (other->classname == self->classname)
	{
//		RPrint ("bump");
		return;
	}
	dremove(self);
}

void bubble_bob()
{
float		rnd1, rnd2, rnd3;
//local vector	vtmp1, modi;

	self->cnt = self->cnt + 1;
	if (self->cnt == 4)
		bubble_split();
	if (self->cnt == 20)
		dremove(self);

	rnd1 = self->velocity[X] + (-10 + (random() * 20));
	rnd2 = self->velocity[Y] + (-10 + (random() * 20));
	rnd3 = self->velocity[Z] + 10 + random() * 10;

	if (rnd1 > 10)
		rnd1 = 5;
	if (rnd1 < -10)
		rnd1 = -5;

	if (rnd2 > 10)
		rnd2 = 5;
	if (rnd2 < -10)
		rnd2 = -5;

	if (rnd3 < 10)
		rnd3 = 15;
	if (rnd3 > 30)
		rnd3 = 25;

	self->velocity[X] = rnd1;
	self->velocity[Y] = rnd2;
	self->velocity[Z] = rnd3;

	self->nextthink = time + 0.5;
	self->think = bubble_bob;
}

/*~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>
~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~<~>~*/

/*QUAKED viewthing (0 .5 .8) (-8 -8 -8) (8 8 8)

Just for the debugging level.  Don't use
*/

void viewthing()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	self->movetype = PR_MOVETYPE_NONE;
	self->solid = PR_SOLID_NOT;
	precache_model ("progs/player.mdl");
	setmodel (self, "progs/player.mdl");
}


/*
==============================================================================

SIMPLE BMODELS

==============================================================================
*/

void func_wall_use()
{	// change to alternate textures
	self->frame = 1 - self->frame;
}

/*QUAKED func_wall (0 .5 .8) ?
This is just a solid wall if not inhibitted
*/
void func_wall()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	self->angles = V({0, 0, 0});
	self->movetype = PR_MOVETYPE_PUSH;	// so it doesn't get pushed by anything
	self->solid = PR_SOLID_BSP;
	self->use = func_wall_use;
	setmodel (self, self->model);
}


/*QUAKED func_illusionary (0 .5 .8) ?
A simple entity that looks solid but lets you walk through it.
*/
void func_illusionary()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	self->angles = V({0, 0, 0});
	self->movetype = PR_MOVETYPE_NONE;
	self->solid = PR_SOLID_NOT;
	setmodel (self, self->model);
	makestatic (self); //CRASH This used to have no args. Possible?
}

/*QUAKED func_episodegate (0 .5 .8) ? E1 E2 E3 E4
This bmodel will appear if the episode has allready been completed, so players can't reenter it.
*/
void func_episodegate()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	if (!(serverflags & self->spawnflags))
		return;			// can still enter episode

	self->angles = V({0, 0, 0});
	self->movetype = PR_MOVETYPE_PUSH;	// so it doesn't get pushed by anything
	self->solid = PR_SOLID_BSP;
	self->use = func_wall_use;
	setmodel (self, self->model);
}

/*QUAKED func_bossgate (0 .5 .8) ?
This bmodel appears unless players have all of the episode sigils.
*/
void func_bossgate()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	if ( (serverflags & 15) == 15)
		return;		// all episodes completed
	self->angles = V({0, 0, 0});
	self->movetype = PR_MOVETYPE_PUSH;	// so it doesn't get pushed by anything
	self->solid = PR_SOLID_BSP;
	self->use = func_wall_use;
	setmodel (self, self->model);
}

//============================================================================
/*QUAKED ambient_suck_wind (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
void ambient_suck_wind()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	precache_sound ("ambience/suck1.wav");
	ambientsound (self->origin, "ambience/suck1.wav", 1, PR_ATTN_STATIC);
}

/*QUAKED ambient_drone (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
void ambient_drone()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	precache_sound ("ambience/drone6.wav");
	ambientsound (self->origin, "ambience/drone6.wav", 0.5, PR_ATTN_STATIC);
}

/*QUAKED ambient_flouro_buzz (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
void ambient_flouro_buzz()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	precache_sound ("ambience/buzz1.wav");
	ambientsound (self->origin, "ambience/buzz1.wav", 1, PR_ATTN_STATIC);
}
/*QUAKED ambient_drip (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
void ambient_drip()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	precache_sound ("ambience/drip1.wav");
	ambientsound (self->origin, "ambience/drip1.wav", 0.5, PR_ATTN_STATIC);
}
/*QUAKED ambient_comp_hum (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
void ambient_comp_hum()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	precache_sound ("ambience/comp1.wav");
	ambientsound (self->origin, "ambience/comp1.wav", 1, PR_ATTN_STATIC);
}
/*QUAKED ambient_thunder (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
void ambient_thunder()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	precache_sound ("ambience/thunder1.wav");
	ambientsound (self->origin, "ambience/thunder1.wav", 0.5, PR_ATTN_STATIC);
}
/*QUAKED ambient_light_buzz (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
void ambient_light_buzz()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	precache_sound ("ambience/fl_hum1.wav");
	ambientsound (self->origin, "ambience/fl_hum1.wav", 0.5, PR_ATTN_STATIC);
}
/*QUAKED ambient_swamp1 (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
void ambient_swamp1()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	precache_sound ("ambience/swamp1.wav");
	ambientsound (self->origin, "ambience/swamp1.wav", 0.5, PR_ATTN_STATIC);
}
/*QUAKED ambient_swamp2 (0.3 0.1 0.6) (-10 -10 -8) (10 10 8)
*/
void ambient_swamp2()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	precache_sound ("ambience/swamp2.wav");
	ambientsound (self->origin, "ambience/swamp2.wav", 0.5, PR_ATTN_STATIC);
}

//============================================================================

void noise_think()
{
	self->nextthink = time + 0.5;
	sound (self, 1, "enforcer/enfire.wav", 1, PR_ATTN_NORM);
	sound (self, 2, "enforcer/enfstop.wav", 1, PR_ATTN_NORM);
	sound (self, 3, "enforcer/sight1.wav", 1, PR_ATTN_NORM);
	sound (self, 4, "enforcer/sight2.wav", 1, PR_ATTN_NORM);
	sound (self, 5, "enforcer/sight3.wav", 1, PR_ATTN_NORM);
	sound (self, 6, "enforcer/sight4.wav", 1, PR_ATTN_NORM);
	sound (self, 7, "enforcer/pain1.wav", 1, PR_ATTN_NORM);
}

/*QUAKED misc_noisemaker (1 0.5 0) (-10 -10 -10) (10 10 10)

For optimzation testing, starts a lot of sounds.
*/

void misc_noisemaker()

{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	precache_sound2 ("enforcer/enfire.wav");
	precache_sound2 ("enforcer/enfstop.wav");
	precache_sound2 ("enforcer/sight1.wav");
	precache_sound2 ("enforcer/sight2.wav");
	precache_sound2 ("enforcer/sight3.wav");
	precache_sound2 ("enforcer/sight4.wav");
	precache_sound2 ("enforcer/pain1.wav");
	//UNUSED_TEST precache_sound2 ("enforcer/pain2.wav");
	//UNUSED_TEST precache_sound2 ("enforcer/death1.wav");
	//UNUSED_TEST precache_sound2 ("enforcer/idle1.wav");

	self->nextthink = time + 0.1 + random();
	self->think = noise_think;
}

} // END namespace Progs
