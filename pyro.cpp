/*
	PYRO.QC

	TeamFortress v2.5	 31/10/96
*/

#include "progs.h"
#include "ofndefs.h"
#include "debug.h"
#include "weapons.h"
#include "combat.h"
#include "sprites.h"
#include "player.h"
#include "custom.h"
#include "qw.h"

namespace Progs {

/*==============================================
 Weapons and functions for the PYRO class and
 associated weaponry
==============================================*/
void NapalmGrenadeTouch();

//WK Extern
float IsBuilding(entity foobar);

//void (vector org, entity shooter) NapalmGrenadeLaunch;
void Napalm_touch();
float RemoveFlameFromQueue(const string& id_flame);

#define PR_DAMAGE_TIME 3

//** different types of flames (decreasing priority)

// 1 : burning flames making light and damage (1 per players or monsters)
// 2 : exploding flames (grenade)
// 3 : burning flames (players, monsters)
// 4 : world flames (on ground)


// create a flame of a given type, maintaining the count for each type
entity FlameSpawn(const string& type, entity p_owner)
{
	if (type != "1")
		return world;


	num_world_flames = num_world_flames + 1;

/*
	db1 = ftos(num_world_flames);
	RPrint("num_world_flames : ");
	RPrint(db1);
	RPrint("\n");
*/

	while (num_world_flames > PR_FLAME_MAXWORLDNUM)
	{
		if (!(RemoveFlameFromQueue(type)))
		{
			// RPrint("Create flame failed: too many\n");
			return world;
		}
	}

	newmis = spawn();

	// to keep track of the number of each type of flames
	if (type == "1")
	{
		newmis->movetype = PR_MOVETYPE_FLYMISSILE;
		newmis->solid = PR_SOLID_BBOX;
		newmis->effects = PR_EF_DIMLIGHT;
		newmis->flame_id = "1";
		setmodel (newmis, "progs/flame2.mdl");
		setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));
	}
	else if (type == "2")
	{
		newmis->movetype = PR_MOVETYPE_BOUNCE;
		newmis->solid = PR_SOLID_BBOX;
		newmis->flame_id = "2";
		setmodel (newmis, "progs/flame2.mdl");
		newmis->frame=1;
		setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));
	}
	else if (type == "3")
	{
		newmis->movetype = PR_MOVETYPE_FLYMISSILE;
		newmis->solid = PR_SOLID_BBOX;
		newmis->flame_id = "3";
		setmodel (newmis, "progs/flame2.mdl");
		setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));
	}
	else if (type == "4")
	{
		newmis->movetype = PR_MOVETYPE_FLYMISSILE;
		newmis->solid = PR_SOLID_BBOX;
		newmis->flame_id = "4";
		setmodel (newmis, "progs/flame2.mdl");
		newmis->frame=1;
		setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));
	}

	newmis->owner = p_owner;
	return newmis;
}


// destroy a given flame, maintaining counters and links in the queue
void FlameDestroy(entity flame)  // PZ: `flame` was called "this"; changed for C++
{
//	local string db1;

	num_world_flames = num_world_flames - 1;

/*
	db1 = ftos(num_world_flames);
	RPrint("num_world_flames : ");
	RPrint(db1);
	RPrint("\n");
*/
	remove(flame);
}

float RemoveFlameFromQueue(const string& id_flame)
{
	entity tmp;
//	local string db1;


	if (num_world_flames < PR_FLAME_MAXWORLDNUM)
	{
		RPrint("ERROR in RemoveFlameFromQueue\n");
		return PR_FALSE;
	}
/*
	db1 = ftos(num_world_flames);
	RPrint("num_world_flames : ");
	RPrint(db1);
	RPrint("\n");
*/

	num_world_flames = num_world_flames - 1;

	tmp = find(world, "flame_id", "4");
	if (tmp == world)
	{
		if (id_flame == "4") // if priority not high enough, don't continue
			return PR_FALSE;

		tmp = find(world, "flame_id", "3");
		if (tmp == world)
		{
			if (id_flame == "3")
				return PR_FALSE;

			tmp = find(world, "flame_id", "2");
			if (tmp == world)
			{
				if (id_flame == "2")
					return PR_FALSE;

				tmp = find(world, "flame_id", "1");
				if (tmp == world)
				{
					// oh shit, no flames found!
					// the queue must be wrong
					RPrint("\n\nRemoveFlameFromQueue():BOOM!\n");
					RPrint("!! please report this bug !!\n");
					return PR_FALSE;
				}
			}
		}
	}

/*
	RPrint("flame removed: flame_id: ");
	RPrint(id_flame);
	RPrint("\n");
*/

/*
	if (tmp.effects == #EF_DIMLIGHT)
	{
		RPrint("** error: this flame should not be removed **\n");
	}
*/

	remove(tmp);
	return PR_TRUE;
}

void RemoveFlame()
{
	FlameDestroy(self);
}

// function used by the flames spawned when the grenade explode : killed in water or when stopped
/* OfN Unused
void() NapalmGrenadeFollow =
{
	traceline(self.origin,self.origin,#TL_BSP_ONLY,self);

	//WK Bugfix
	if (self.enemy.waterlevel > 1)
	{
		sound (self, #CHAN_VOICE, "misc/vapeur2.wav", 1, #ATTN_NORM);

		FlameDestroy(self);
	}

	if (self.velocity == '0 0 0')
		FlameDestroy(self);

	self.nextthink = time + 0.1;
//	self.think = NapalmGrenadeFollow;
};*/

void NapalmGrenadeTouch()
{
	sound (self, PR_CHAN_WEAPON, "weapons/bounce.wav", 1, PR_ATTN_NORM);  // bounce sound
	if (self->velocity == V({0, 0, 0}))
		self->avelocity = V({0, 0, 0});
}

void NapalmGrenThink()
{
	vector exporg;
	float damage, nexttime;

	self->heat = self->heat - 1;

	exporg[X] = self->origin[X] + crandom()*50;
	exporg[Y] = self->origin[Y] + crandom()*50;
	exporg[Z] = self->origin[Z] + random()*25;

	damage = PR_NAPALM_MINDMG + (PR_NAPALM_MAXDMG * random());

	if (random() < 0.5)
	{
		if (random() < 0.75)
			sound (self, PR_CHAN_MISC, "weapons/flmgrexp.wav", 1, PR_ATTN_NORM);

	   //WK 2/26/7 Was doing 120 half the time, 260 half the time.
	   deathmsg = PR_DMSG_NAPALM;
	   T_RadiusFireDamage(exporg,self,self->owner,damage,0.7);
	}
	else
	{
		WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
		WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
		WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
		WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
	#ifdef PR_QUAKE_WORLD
		multicast (self->origin, PR_MULTICAST_PHS);
	#endif

		deathmsg = PR_DMSG_NAPALM;
		T_RadiusFireDamage(self->origin,self,self->owner,PR_NAPALM_MAINDMG,0.85);
	}

	if (random() < 0.3)
		SpawnSprite(1,PR_SPRITE_AIRBURST,exporg,V({0, 0, 0}),PR_SPRITEMOVE_UPSLOW,0.15);
	else
		SpawnSprite(1,PR_SPRITE_AIRBURST,exporg,V({0, 0, 0}),PR_SPRITEMOVE_UP,0.1);

	if (self->heat <= 0)
		dremove(self);
	else
	{
		nexttime = PR_NAPALM_MINRATE + ((PR_NAPALM_MAXRATE - PR_NAPALM_MINRATE) * random());
		self->nextthink = time + nexttime;
	}
}

void NapalmGrenadeExplode()
{
	// Launch flames
	traceline(self->origin,self->origin,PR_TL_BSP_ONLY,self);

	if (trace_inwater == PR_TRUE)
	{
		// OfN - Some FX's added
		sound(self, PR_CHAN_MISC, "misc/vapeur2.wav",1,PR_ATTN_NORM);
		SpawnBubbles(2,self->origin); // Spawn some bubbles
		SpawnSprite(1,PR_SPRITE_ABLAST,self->origin,V({0, 0, 0}),PR_SPRITEMOVE_UPSLOW,0.3);

		dremove(self);
		return;
	}

#ifdef PR_DEMO_STUFF
	// Remove any camera's locks on this missile
	if (self->enemy != world)
		CamProjectileLockOff();
#endif

	entity head;

	// do an incendiary-cannon explosion instead
	self->effects = self->effects | PR_EF_BRIGHTLIGHT;

	// don't do radius damage to the other, because all the damage
	// was done in the impact
	head = findradius(self->origin, 140);

	while (head != world)
	{
			if (head->takedamage && head->health > 0)
			{
				#ifdef PR_OLD_FLAME
				deathmsg = PR_DMSG_FLAME;
				TF_T_Damage (head, self, self->owner, 120, PR_TF_TD_NOTTEAM, PR_TF_TD_FIRE);
				#else
				deathmsg = PR_DMSG_NAPALM;
				TF_T_Damage (head, self, self->owner, PR_NAPALM_FIRSTDMG, PR_TF_TD_NOKNOCK | PR_TF_TD_DONTGIB | PR_TF_TD_NOTTEAM, PR_TF_TD_FIRE);
				#endif

				// set 'em on fire
				other = head;  // i can't believe this works!
				Napalm_touch();
				if (other->classname == "player")
					stuffcmd(other, "bf\nbf\n");

			}
		head = head->chain;
	}

	#ifdef PR_OLD_FLAME

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

	#else // new napalm

	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
	WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
#ifdef PR_QUAKE_WORLD
	multicast (self->origin, PR_MULTICAST_PHS);
#endif

	self->think = NapalmGrenThink;
	self->nextthink = time + PR_NAPALM_MINRATE + ((PR_NAPALM_MAXRATE - PR_NAPALM_MINRATE)*random());

	if (self->netname == "special_mine")
		self->heat = PR_MINE_NAPALM_BURNTIMES;
	else
		self->heat = PR_NAPALM_BURNTIMES;
	#endif // Flame/napalm
}

//=========================================================================
// Launch a flame foe the grenade explosion
/* OfN Unused
void (vector org, entity shooter) NapalmGrenadeLaunch =
{
	local float xdir,ydir,zdir, spin;

	xdir = 150 * random() - 75;
	ydir = 150 * random() - 75;
	zdir = 40 * random();

	newmis = FlameSpawn ("2", shooter);
	if (newmis == world)
		return;

	//self.touch = SUB_Null;

	newmis.classname = "fire";
	newmis.touch = Napalm_touch;
	newmis.think = NapalmGrenadeFollow;
	newmis.nextthink = time + 0.1;

	newmis.enemy = shooter.owner;

	newmis.velocity_x = xdir * 2;
	newmis.velocity_y = ydir * 2;
	newmis.velocity_z = zdir * 15;

	spin = (random() * 10) / 2;
	if (spin <= 0)
		newmis.avelocity='250 300 400';
	if (spin == 1)
		newmis.avelocity='400 250 300';
	if (spin == 2)
		newmis.avelocity='300 400 250';
	if (spin == 3)
		newmis.avelocity='300 300 300';
	if (spin >= 4)
		newmis.avelocity='400 250 400';

	setorigin (newmis, org);
	setsize (newmis, #VEC_ORIGIN, #VEC_ORIGIN);
};*/

void OnPlayerFlame_touch();

void FlameFollow()
{
	vector dir,vtemp,boundsize;
	float damage;

	vtemp = self->enemy->absmin;
	boundsize = self->enemy->size;

	self->solid = PR_SOLID_NOT;
	self->movetype = PR_MOVETYPE_NONE;

	// if no flames, remove itself
	if (self->enemy->numflames == 0)
	{
		FlameDestroy(self);
		return;
	}

	if (self->enemy->health < 1)
	{
		deathmsg = PR_DMSG_FLAME;
		T_RadiusDamage(self,self,10,self);
		self->enemy->numflames = 0;
		FlameDestroy(self);
		return;
	}

	if (self->enemy->armorclass & PR_AT_SAVEFIRE)
	{
//		if ((random()*100) < self.enemy.armorvalue)
		if (self->enemy->armorvalue > 0)
		{
			self->health = 0;
		}
	}

	if (self->health < 1)
	{
		// only remove the flame if it is not the master flame, or if it is the last flame
		if (self->effects != PR_EF_DIMLIGHT || self->enemy->numflames <= 1)
		{
			self->enemy->numflames = self->enemy->numflames - 1;
			self->enemy->numflames = 0;
			FlameDestroy(self);
			return;
		}
	}


	self->health = self->health - 1;

#ifndef PR_QUAKE_WORLD
	dir[X] = (random() * boundsize[X]/2)+boundsize[X]/4;
	dir[Y] = (random() * boundsize[Y]/2)+boundsize[Y]/4;
	dir[Z] = (random() * boundsize[Z]/3)+boundsize[Z]/2;
	vtemp = vtemp + dir;
	setorigin(self, vtemp);
#else

	// if player is moving too fast, hide flames
	if (vlen(self->enemy->velocity) < 50)
	{
		dir[X] = (random() * boundsize[X]/2)+boundsize[X]/4;
		dir[Y] = (random() * boundsize[Y]/2)+boundsize[Y]/4;
		dir[Z] = (random() * boundsize[Z]/3)+boundsize[Z]/2;
		vtemp = vtemp + dir;
		setorigin(self, vtemp);

		if (self->model != "progs/flame2.mdl")
		{
			self->model = "progs/flame2.mdl";
			setmodel(self, self->model);
		}
		self->health = self->health - 2; //WK 2/15/98 - Stop drop and roll!
	}
	else if (self->model == "progs/flame2.mdl")
	{
		self->model = string_null;
		setmodel(self, self->model);
	}

#endif

	//traceline(self.origin,self.origin,#TL_BSP_ONLY,self);

	//WK Bugfix
	if (self->enemy->waterlevel > 1)
	{
		sound (self, PR_CHAN_VOICE, "misc/vapeur2.wav", 1, PR_ATTN_NORM);
		self->enemy->numflames = self->enemy->numflames - 1;
		FlameDestroy(self);
		return;
	}

	self->nextthink = time + 0.1;

	if (self->effects == PR_EF_DIMLIGHT && self->heat >= PR_DAMAGE_TIME)
	{
		damage = self->enemy->numflames * 0.3 * PR_DAMAGE_TIME;
		if (damage < 1)
			damage = 1;

		self->heat = 1;
		deathmsg = PR_DMSG_FLAME;
		TF_T_Damage(self->enemy, self, self->owner, damage, PR_TF_TD_NOTTEAM, PR_TF_TD_FIRE);
	}
	else if (self->effects == PR_EF_DIMLIGHT)
	{
		self->heat = self->heat + 1;
	}
}

// OnPlayerflame : no damage if enemy not dead, spawn flames if touched
void OnPlayerFlame_touch()
{
	entity flame;
	vector vtemp;

	if (other != world && other->health > 0 && other != self->enemy)
	{

		if (other->numflames >= PR_FLAME_MAXPLYRFLAMES)
			return;

		if (other->classname == "player")
		{
			if ((teamplay & PR_TEAMPLAY_NOEXPLOSIVE) && Teammate(other, self->owner))
				return;

			 CenterPrint(other,"You are on fire!\n");
			 stuffcmd (other,"bf\n");
		}

		if (other->numflames < PR_FLAME_NUMLIGHTS)
		{
			flame = FlameSpawn ("1", other);
			sound (flame, PR_CHAN_VOICE, "ambience/fire1.wav", 1, PR_ATTN_NORM);
		}
		else
		{
			flame = FlameSpawn ("3", other);
			if (flame == world)
				return;
		}

		flame->classname = "fire";
		flame->health = PR_FLAME_PLYRMAXTIME;
		other->numflames = other->numflames + 1;

		flame->velocity = other->velocity;
		flame->enemy = other;
		flame->touch = OnPlayerFlame_touch;
		flame->owner = self->owner;

		vtemp = self->origin;

		setorigin(flame, vtemp);

		flame->nextthink = time + 0.1;
		flame->think = FlameFollow;
	}
}


// worldflame : lot of damage, spawn flames if touched
void WorldFlame_touch()
{
	entity flame;
	vector vtemp;//dir,

	deathmsg = PR_DMSG_FLAME;
	TF_T_Damage(other, self, self->enemy, 2, PR_TF_TD_NOTTEAM, PR_TF_TD_FIRE);

	if (other != world && other->solid != PR_SOLID_TRIGGER && other->health > 0)
	{

		if (other->numflames >= PR_FLAME_MAXPLYRFLAMES)
			return;

		if (other->classname == "player")
		{
			if ((teamplay & PR_TEAMPLAY_NOEXPLOSIVE) && Teammate(other, self->owner))
					return;
			CenterPrint(other,"You are on fire!\n");
			stuffcmd (other,"bf\n");
		}

		if (other->numflames < PR_FLAME_NUMLIGHTS)
		{
			flame = FlameSpawn ("1", other);
			sound (flame,PR_CHAN_VOICE, "ambience/fire1.wav", 1, PR_ATTN_NORM);
		}
		else
		{
			flame = FlameSpawn ("3", other);
			if (flame == world)
				return;
		}

		flame->classname = "fire";
		flame->health = 0;
		other->numflames=other->numflames + 1;

		flame->velocity = other->velocity;
		flame->enemy = other;
		flame->touch = OnPlayerFlame_touch;
		flame->owner = self->owner;

		vtemp = self->origin + V({0, 0, 10});

		setorigin(flame, vtemp);

		flame->nextthink = time + 0.15;
		flame->think = FlameFollow;
	}
}

// Like the flamethrower touch, but it doesn't light them on fire (for hoverboots).
void Boot_Flamer_stream_touch()
{
	deathmsg = PR_DMSG_HOVER;


	//WK Sweep for mines at point of contact
	GuerillaMineSweep(self->origin);

	//WK if (other.takedamage && other.classname == "player")
	if (other->takedamage)
		TF_T_Damage(other,self,self->owner, 40, PR_TF_TD_NOTTEAM, PR_TF_TD_FIRE);
	remove(self);
}
// first touch : direct touch with the flamethrower stream or flame from grenade
void Flamer_stream_touch()
{
	entity flame;
	vector vtemp;//dir,

	if (other->classname == "fire")
		return;

	//WK Sweep for mines at point of contact
	GuerillaMineSweep(self->origin);

	if (other != world)
	{
		if (other->takedamage == PR_DAMAGE_AIM && other->health > 0 && time >= self->attack_finished)
		{
			// Gizmo - prevent flame from damaging the enemy a shitload of times if they run into the flame
			// PZ: I'm guessing this is what nerfed (gimped) the flamethrower. Need to compensate with more damage or something.
			// FIXME: change to a macro?
			self->attack_finished = time + 0.50;

			deathmsg = PR_DMSG_FLAME;
			// PZ: I've increased the chance that a flame will catch fire, and I'm increasing the damage here from 15 to 30.
			TF_T_Damage(other,self,self->owner, 30, PR_TF_TD_NOTTEAM, PR_TF_TD_FIRE);

			if (other->numflames >= PR_FLAME_MAXPLYRFLAMES)
				return;

			if (other->armorclass & PR_AT_SAVEFIRE)
			{
//				if ((random()*100) < other.armorvalue)
				if (other->armorvalue > 0)
				{
					return;
				}
			}

			if (other->classname == "player")
			{
				if ((teamplay & PR_TEAMPLAY_NOEXPLOSIVE) && Teammate(other, self->owner))
						return;

				CenterPrint(other,"You are on fire!\n");
				stuffcmd (other,"bf\n");
			}

			if (other->numflames < PR_FLAME_NUMLIGHTS)
			{
				flame = FlameSpawn("1", other);
				sound (flame,PR_CHAN_VOICE, "ambience/fire1.wav", 1, PR_ATTN_NORM);
			}
			else
			{
				flame = FlameSpawn("3", other);
				if (flame == world)
					return;
			}
			flame->classname = "fire";
			flame->health = PR_FLAME_PLYRMAXTIME;
			other->numflames=other->numflames + 1;

			flame->velocity = other->velocity;
			flame->enemy = other;
			flame->touch = OnPlayerFlame_touch;
			flame->owner = self->owner;

			vtemp=self->origin;

			setorigin(flame , vtemp);

			flame->nextthink = time + 0.1;
			flame->think = FlameFollow;
		}
	}
	else
	{
		if (random() < PR_FLAME_BURNRATIO)
		{
			remove(self);
			return;
		}
		flame = FlameSpawn("4", other);
		if (flame != world)
		{
			flame->touch = WorldFlame_touch;
			flame->classname = "fire";

			vtemp=self->origin + V({0, 0, 10});
			setorigin(flame , vtemp);
			flame->nextthink = time + PR_FLAME_MAXBURNTIME;
			flame->think = RemoveFlame;
			flame->enemy = self->owner;
		}

		remove(self);
	}
}


void Napalm_touch()
{

	entity flame;
	vector vtemp;//dir,

	if (other->classname == "fire")
		return;

	if (other != world)
	{
		if (other->takedamage == PR_DAMAGE_AIM && other->health > 0)
		{
			deathmsg = PR_DMSG_FLAME;
			TF_T_Damage(other, self, self->owner, 6, PR_TF_TD_NOTTEAM, PR_TF_TD_FIRE);

			if (other->numflames >= PR_FLAME_MAXPLYRFLAMES)
				return;

			if (other->armorclass & PR_AT_SAVEFIRE && other->armorvalue > 0)
				return;

			if (other->classname == "player")
			{
				if ((teamplay & PR_TEAMPLAY_NOEXPLOSIVE) && Teammate(other, self->owner))
						return;

				CenterPrint(other,"You are on fire!\n");
				stuffcmd (other,"bf\n");
			}

			if (other->numflames < PR_FLAME_NUMLIGHTS) // = 0
			{
				flame = FlameSpawn("1", other);
				sound (flame,PR_CHAN_VOICE, "ambience/fire1.wav", 1, PR_ATTN_NORM);
			}
			else
			{
				flame = FlameSpawn("3", other);
				if (flame == world)
					return;
			}
			flame->classname = "fire";
			flame->health = PR_FLAME_PLYRMAXTIME;
			other->numflames = other->numflames + 1;

			flame->velocity = other->velocity;
			flame->enemy = other;
			flame->touch = OnPlayerFlame_touch;
			flame->owner = self->owner;

			vtemp=self->origin;

			setorigin(flame , vtemp);

			flame->nextthink = time + 0.1;
			flame->think = FlameFollow;
		}
	}
	else
	{
		flame = FlameSpawn("4", other);
		if (flame != world)
		{
			flame->touch = WorldFlame_touch;
			flame->classname = "fire";

			vtemp=self->origin + V({0, 0, 10});
			setorigin(flame , vtemp);
			flame->nextthink = time + PR_NAPALM_MAXBURNTIME;
			flame->think = RemoveFlame;
			flame->enemy = self->owner;
		}

		FlameDestroy(self);
	}
}


// Player.qc declaration
	void DeathBubblesSpawn();


// Slightly varied version of DEATHBUBBLES
void NewBubbles(float num_bubbles, const vector& bub_origin)
{
entity	bubble_spawner;

	bubble_spawner = spawnServerSide(); // PZ: make it a server-side only entity
	setorigin (bubble_spawner, bub_origin);
	bubble_spawner->movetype = PR_MOVETYPE_NONE;
	bubble_spawner->solid = PR_SOLID_NOT;
	bubble_spawner->nextthink = time + 0.1;

	if (self->classname == "player")
		bubble_spawner->owner = self;
	else
		bubble_spawner->owner = self->enemy;

	bubble_spawner->think = DeathBubblesSpawn;
	bubble_spawner->bubble_count = num_bubbles;
	return;
}

// Fires the flamethrower.
void W_FireFlame()
{
	entity flame;
	float rn;

	if (self->waterlevel > 2)
	{
			makevectors (self->v_angle);
			NewBubbles(2, self->origin+v_forward*64);

			rn = random();
			if (rn < 0.5)
					sound (self, PR_CHAN_WEAPON, "misc/water1.wav", 1, PR_ATTN_NORM);
			else
					sound (self, PR_CHAN_WEAPON, "misc/water2.wav", 1, PR_ATTN_NORM);

			return;
	}

	// Take away a cell
	self->currentammo = self->ammo_cells = self->ammo_cells - 1;

	sound (self, PR_CHAN_AUTO, "weapons/flmfire2.wav", 1, PR_ATTN_NORM);

	flame = spawn ();

	flame->owner = self;
	flame->movetype = PR_MOVETYPE_FLYMISSILE;
	flame->solid = PR_SOLID_BBOX;
	flame->classname = "flamerflame";

	// set flame speed

	makevectors (self->v_angle);

	flame->velocity = aim(self, 10000);
	flame->velocity = flame->velocity * 600;

	flame->touch = Flamer_stream_touch;

	flame->think = s_explode1;
	flame->nextthink = time + 0.15;

	flame->effects = PR_EF_DIMLIGHT;

	setmodel (flame, "progs/s_explod.spr");
	setsize (flame, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (flame, self->origin + v_forward * 16 + V({0, 0, 16}));
}


/*======================
Touch function for incendiary cannon rockets
======================*/
void T_IncendiaryTouch()
{
	float	damg;
	//local float	points;
	entity	head;
	//local vector	org;

	if (other == self->owner)
		return; 	// don't explode on owner

	if (pointcontents(self->origin) == PR_CONTENT_SKY)
	{
		remove(self);
		return;
	}
	self->effects = self->effects | PR_EF_BRIGHTLIGHT;

	damg = 20 + random()*20;

	if (other->health)
	{
		deathmsg = PR_DMSG_FLAME;
		TF_T_Damage (other, self, self->owner, damg, PR_TF_TD_NOTTEAM, PR_TF_TD_FIRE);
	}

	// don't do radius damage to the other, because all the damage
	// was done in the impact
	head = findradius(self->origin, 180);

	while (head != world)
	{
			if (head->takedamage && head->health > 0)
			{
				deathmsg = PR_DMSG_FLAME;
				TF_T_Damage (head, self, self->owner, 15, PR_TF_TD_NOTTEAM, PR_TF_TD_FIRE);

				// set 'em on fire
				other = head;  // i can't believe this works!
				Napalm_touch();
				if (other->classname == "player")
					stuffcmd(other, "bf\nbf\n");
				if (IsBuilding(other))
				TF_T_Damage (head, self, self->owner, 35, PR_TF_TD_NOTTEAM, PR_TF_TD_FIRE);

			}
			/*else TOFIX: if i use this to make corpses desptryable, coolexplosion later doesnt work?
			{
				if (head.#corpseflag = #STRFLAG_CORPSE)
					TF_T_Damage (head, self, self.owner, 10 + random()*10, #TF_TD_NOTTEAM, #TF_TD_FIRE);
			}*/

		head = head->chain;
	}

//	sound (self, #CHAN_WEAPON, "weapons/r_exp3.wav", 1, #ATTN_NORM);
	self->origin = self->origin - 8*normalize(self->velocity);

	#ifdef PR_COOL_GIBS
	CoolExplosion(self, 50,10);
	#endif

#ifdef PR_DEMO_STUFF
	// Remove any camera's locks on this missile
	if (self->enemy != world)
		CamProjectileLockOff();
#endif

	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
	WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
#ifdef PR_QUAKE_WORLD
	multicast (self->origin, PR_MULTICAST_PHS);

	head = SpawnSprite(1,PR_SPRITE_AIRBURST,self->origin,V({0, 0, 0}),PR_SPRITEMOVE_UPSLOW,0.1);

	if (head != world)
		head->effects = PR_EF_DIMLIGHT;

	if (other->classname == "force_field") //- OfN - Makes field explosion b4 removing it
		FieldEvent(other,self->origin,self);//FieldExplosion(other,self.origin,self);

	dremove(self);
#else
	BecomeExplosion ();
#endif
}


/*
================
W_FireIncendiaryCannon
================
*/
void W_FireIncendiaryCannon()
{

	if (self->ammo_rockets < 3)
		return;

	self->currentammo = self->ammo_rockets = self->ammo_rockets - 3;

	sound (self, PR_CHAN_WEAPON, "weapons/sgun1.wav", 1, PR_ATTN_NORM);

	KickPlayer(-3, self);

	newmis = spawn ();
	newmis->owner = self;
	newmis->movetype = PR_MOVETYPE_FLYMISSILE;
	newmis->solid = PR_SOLID_BBOX;

	// set newmis speed
	makevectors (self->v_angle);
	newmis->velocity = aim(self, 1000);
	newmis->velocity = newmis->velocity * 650; // original 600
	newmis->angles = vectoangles(newmis->velocity);

	newmis->touch = T_IncendiaryTouch;

	//- OfN - For airfist
	newmis->classname = "rocket";

	// set newmis duration
	newmis->nextthink = time + 5;
	newmis->think = SUB_Remove;

	newmis->weapon = PR_DMSG_INCENDIARY;

	setmodel (newmis, "progs/missile.mdl");
	setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (newmis, self->origin + v_forward*8 + V({0, 0, 16}));

#ifdef PR_DEMO_STUFF
	// Have we got a live camera in projectile mode?
	if (live_camera)
		CamProjectileLockOn();
#endif
}

//=========================================================================
// Incendiary cannon selection function
void TeamFortress_IncendiaryCannon()
{
	if (!(self->weapons_carried & PR_WEAP_INCENDIARY))
		return;

	if (self->ammo_rockets < 3)
	{
		sprint (self, PR_PRINT_HIGH, "not enough ammo.\n");
		return;
	}

	self->current_weapon = PR_WEAP_INCENDIARY;

	W_SetCurrentAmmo();
}
// Flamethrower selection function
void TeamFortress_FlameThrower()
{
	if (!(self->weapons_carried & PR_WEAP_FLAMETHROWER))
		return;

	if (self->ammo_cells < 1)
	{
		sprint (self, PR_PRINT_HIGH, "not enough ammo.\n");
		return;
	}

	self->current_weapon = PR_WEAP_FLAMETHROWER;

	W_SetCurrentAmmo();
}

} // END namespace Progs
