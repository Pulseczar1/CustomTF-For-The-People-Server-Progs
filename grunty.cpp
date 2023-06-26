/* ------------------------------------------------------------------------------------
** ------------------------------------GRUNTY(tm)--------------------------------------
** -- Your artificially intelligent mercenary from beyond the Void					 --
** -- Use with caution																  -
** -- Do not expose to excessive amounts of radiation								  -
** -- Share and Enjoy!																 --
** --- Copyright(c)2000 By Dwarven Star Interactive Industrial Industry Enterprises ---
** ----------------------------------------------------------------------------------*/

#include "progs.h"
#include "grunty.h"
#include "army.h"
#include "ofndefs.h"
#include "tfdefs.h"
#include "weapons.h"
#include "monsters.h"
#include "ai.h"
#include "custom.h"
#include "optimize.h"
#include "cpstuff.h"
#include "warlock.h"
#include "debug.h"
#include "player.h"
#include "jobs.h"
#include "tforttm.h"

namespace Progs {

/* ALREADY DEFINED IN ARMY.QC

#define WAYPOINT_TYPE_PRIMARY       0
#define WAYPOINT_TYPE_SECONDARY     1
#define WAYPOINT_TYPE_ENEMYLASTSEEN 2

*/

#define PR_GRUNTY_PAINTIME 			2

// Function Prototypes
// Whenever a function is added within the grunty, add it up here

void RemoveWaypoint(entity wyp, entity soldier);

void GruntyThink();
void GRun();
void GruntyScanTargets();
void GruntyCheckFire();
void GetRank(entity targ);
void Grunty_Check_Frags();
float botmovedist(float angle, float dist);
float ReturnWeaponVelocity();
void GruntyPayThink();
void GruntyDrawPay();


// External Functions
// List all external functions used here
float isMelee();
//entity (entity scanner) LookAround;

//=- OfN -=//
void PrintFromSoldier(entity sld, entity player, const string& msg, float priority);
string GetOwnerMessage(entity sld);
string GetFriendlyMessage(entity sld);
entity ReturnEasyWaypoint(entity sold, entity viewpoint);
string GetEnemyName(entity thething);
void CheckWaterJump();


// The Frames of Grunty

//$cd /raid/quake/id1/models/player_4
//$origin 0 -6 24
//$base base
//$skin skin0
//$skin skin1
//$skin skin2
//$skin skin3

// Frames, from player.qc

namespace GruntyFrames
{
	//
	// running
	//
	enum {FR_AXRUN1, FR_AXRUN2, FR_AXRUN3, FR_AXRUN4, FR_AXRUN5, FR_AXRUN6};

	enum {FR_ROCKRUN1 = 6, FR_ROCKRUN2, FR_ROCKRUN3, FR_ROCKRUN4, FR_ROCKRUN5, FR_ROCKRUN6};

	//
	// standing
	//

	enum {FR_STAND1 = 12, FR_STAND2, FR_STAND3, FR_STAND4, FR_STAND5};

	enum {FR_AXSTND1 = 17, FR_AXSTND2, FR_AXSTND3, FR_AXSTND4, FR_AXSTND5, FR_AXSTND6};
	enum {FR_AXSTND7 = 23, FR_AXSTND8, FR_AXSTND9, FR_AXSTND10, FR_AXSTND11, FR_AXSTND12};


	//
	// pain
	//
	enum {FR_AXPAIN1 = 29, FR_AXPAIN2, FR_AXPAIN3, FR_AXPAIN4, FR_AXPAIN5, FR_AXPAIN6};

	enum {FR_PAIN1 = 35, FR_PAIN2, FR_PAIN3, FR_PAIN4, FR_PAIN5, FR_PAIN6};


	//
	// death
	//

	enum {FR_AXDETH1 = 41, FR_AXDETH2, FR_AXDETH3, FR_AXDETH4, FR_AXDETH5, FR_AXDETH6};
	enum {FR_AXDETH7 = 47, FR_AXDETH8, FR_AXDETH9};

	enum {FR_DEATHA1 = 50, FR_DEATHA2, FR_DEATHA3, FR_DEATHA4, FR_DEATHA5, FR_DEATHA6, FR_DEATHA7, FR_DEATHA8};
	enum {FR_DEATHA9 = 58, FR_DEATHA10, FR_DEATHA11};

	enum {FR_DEATHB1 = 61, FR_DEATHB2, FR_DEATHB3, FR_DEATHB4, FR_DEATHB5, FR_DEATHB6, FR_DEATHB7, FR_DEATHB8};
	enum {FR_DEATHB9 = 69};

	enum {FR_DEATHC1 = 70, FR_DEATHC2, FR_DEATHC3, FR_DEATHC4, FR_DEATHC5, FR_DEATHC6, FR_DEATHC7, FR_DEATHC8};
	enum {FR_DEATHC9 = 78, FR_DEATHC10, FR_DEATHC11, FR_DEATHC12, FR_DEATHC13, FR_DEATHC14, FR_DEATHC15};

	enum {FR_DEATHD1 = 85, FR_DEATHD2, FR_DEATHD3, FR_DEATHD4, FR_DEATHD5, FR_DEATHD6, FR_DEATHD7};
	enum {FR_DEATHD8 = 92, FR_DEATHD9};

	enum {FR_DEATHE1 = 94, FR_DEATHE2, FR_DEATHE3, FR_DEATHE4, FR_DEATHE5, FR_DEATHE6, FR_DEATHE7};
	enum {FR_DEATHE8 = 101, FR_DEATHE9};

	//
	// attacks
	//
	enum {FR_NAILATT1 = 103, FR_NAILATT2};

	enum {FR_LIGHT1 = 105, FR_LIGHT2};

	enum {FR_ROCKATT1 = 107, FR_ROCKATT2, FR_ROCKATT3, FR_ROCKATT4, FR_ROCKATT5, FR_ROCKATT6};

	enum {FR_SHOTATT1 = 113, FR_SHOTATT2, FR_SHOTATT3, FR_SHOTATT4, FR_SHOTATT5, FR_SHOTATT6};

	enum {FR_AXATT1 = 119, FR_AXATT2, FR_AXATT3, FR_AXATT4, FR_AXATT5, FR_AXATT6};

	enum {FR_AXATTB1 = 125, FR_AXATTB2, FR_AXATTB3, FR_AXATTB4, FR_AXATTB5, FR_AXATTB6};

	enum {FR_AXATTC1 = 131, FR_AXATTC2, FR_AXATTC3, FR_AXATTC4, FR_AXATTC5, FR_AXATTC6};

	enum {FR_AXATTD1 = 137, FR_AXATTD2, FR_AXATTD3, FR_AXATTD4, FR_AXATTD5, FR_AXATTD6};
}

void grunty_setstartframe(entity grunty)
{
	grunty->frame = GruntyFrames::FR_STAND1;
}

/* ---------------------------------------------------------------------------------- */
// The Fun Bit

/* ---------------------------------------------------------------------------------- */
//						GRUNTY - PART ONE
//					Standing Around
//
//						Grunty is standing around having a good time
/* ---------------------------------------------------------------------------------- */

// Grunty is standing with his axe or other melee weapon
// Note that he must do a GruntyThink each frame
// (each frame is 0.05 seconds)
// However, he may only animate every 0.1 seconds, hence the flooring and odd incrementing
// of his frame variable. (0.05 animation is very smooth and twice as fast as normal)

void grunty_run();

void grunty_stand() // We need two stand sections for axe and weapon
{
	//self.nextthink = time + 0.05; //needed?? nope
	if (self->health > 0) self->think = grunty_stand;

	self->weaponframe = 0;

	if (isMelee())
	{
		if (self->walkframe >= 12)
			self->walkframe = 0;
		self->frame = GruntyFrames::FR_AXSTND1 + floor(self->walkframe);
	}
	else
	{
		if (self->walkframe >= 5)
			self->walkframe = 0;
		self->frame = GruntyFrames::FR_STAND1 + floor(self->walkframe);
	}

	self->walkframe = self->walkframe + 0.5;
	//self.walkframe = self.walkframe + 0.5;

	// allow grunty pick up ammo when standing
	//touchworld();

	#ifdef PR_VERBOSE_GRUNTY
	if (self->super_time <= time)
	{
		sprint(self->real_owner, PR_PRINT_HIGH, "Soldier in position and awaiting target.\n");
		self->super_time = time + 5;
	}
	#endif

	Grunty_Check_Frags();

	if (self->goalentity != world)
	{
		self->walkframe = 0;
		self->think = grunty_run;
		//return;
	}

	GruntyThink();
	//sprint(self.real_owner, #PRINT_HIGH, "Done gruntythink in stand().\n");
}
/* ---------------------------------------------------------------------------------- */
//						GRUNTY - PART TWO
//					Running Around
//
//						Grunty is charging about the place
/* ---------------------------------------------------------------------------------- */

// Again we have a choice of which animation to use
void grunty_run()
{
//	local string st;

	if (self->health > 0) self->think = grunty_run;

	self->weaponframe = 0; //needed?

	if (isMelee())
	{
		if (self->walkframe >= 6)
			self->walkframe = 0;
		self->frame = GruntyFrames::FR_AXRUN1 + floor(self->walkframe);
	}
	else
	{
		if (self->walkframe >= 6)
			self->walkframe = 0;
		self->frame = GruntyFrames::FR_ROCKRUN1 + floor(self->walkframe);
	}

	self->walkframe = self->walkframe + 0.5;

	#ifdef PR_VERBOSE_GRUNTY
	if (self->super_time <= time)
	{
		sprint(self->real_owner, PR_PRINT_HIGH, "Hello, commander! I am currently running at speed ");
		st = ftos(self->custom_speed);
		sprint(self->real_owner, PR_PRINT_HIGH, st);
		sprint(self->real_owner, PR_PRINT_HIGH, " towards ");
		sprint(self->real_owner, PR_PRINT_HIGH, self->goalentity->netname);
		sprint(self->real_owner, PR_PRINT_HIGH, "\n");
		self->super_time = time + 5;
	}
	#endif

	if (self->goalentity == world)
	{
		self->walkframe = 0;
		self->think = grunty_stand;
		//return;
	}

	GruntyThink();

	#ifdef PR_VERBOSE_GRUNTY
	sprint(self->real_owner, PR_PRINT_HIGH, "I have just finished GruntyThink (self.enemy = ");
	sprint(self->real_owner, PR_PRINT_HIGH, self->enemy);
	sprint(self->real_owner, PR_PRINT_HIGH, ")\n");
	#endif

	GRun();

	#ifdef PR_VERBOSE_GRUNTY
	sprint(self->real_owner, PR_PRINT_HIGH, "I have just finished GRun (self.enemy = ");
	sprint(self->real_owner, PR_PRINT_HIGH, self->enemy);
	sprint(self->real_owner, PR_PRINT_HIGH, ")\n");
	#endif

	Grunty_Check_Frags();
}

void stand_frames() //- ofn - used when soldier is following us, and when hes stuck and set to not jump
{
	self->weaponframe=0;

	if (isMelee())
	{
		if (self->walkframe >= 12)
			self->walkframe = 0;
		self->frame = GruntyFrames::FR_AXSTND1 + floor(self->walkframe);
	}
	else
	{
		if (self->walkframe >= 5)
			self->walkframe = 0;
		self->frame = GruntyFrames::FR_STAND1 + floor(self->walkframe);
	}
	self->walkframe = self->walkframe + 0.1;
}

/* ---------------------------------------------------------------------------------- */
//						GRUNTY - PART THREE
//					Axing People
//
//						Grunty goes psycho
/* ---------------------------------------------------------------------------------- */

// Grunty's first axe attack
void grunty_axeatta()
{
	self->frame = GruntyFrames::FR_AXATT1 + floor(self->weaponframe);

	if (self->weaponframe == 1)
	{
		if (self->current_weapon == PR_WEAP_AXE)
			W_FireAxe();
		else
			W_FireSpanner();
	}
	self->weaponframe = self->weaponframe + 0.5;
	GruntyThink();
	GRun();
	///self.nextthink = time + 0.05;
	if (self->weaponframe > 3)
		if (self->health > 0) self->think = grunty_run;
}

// Grunty's second axe attack
void grunty_axeattb()
{
	self->frame = GruntyFrames::FR_AXATTB1 + floor(self->weaponframe);

	if (self->weaponframe == 2)
	{
		if (self->current_weapon == PR_WEAP_AXE)
			W_FireAxe();
		else
			W_FireSpanner();
	}
	self->weaponframe = self->weaponframe + 0.5;
	GruntyThink();
	GRun();
	///self.nextthink = time + 0.05;
	if (self->weaponframe > 3)
		if (self->health > 0) self->think = grunty_run;
}

// Grunty's third axe attack
void grunty_axeattc()
{
	self->frame = GruntyFrames::FR_AXATTC1 + floor(self->weaponframe);

	if (self->weaponframe == 2)
	{
		if (self->current_weapon == PR_WEAP_AXE)
			W_FireAxe();
		else
			W_FireSpanner();
	}
	self->weaponframe = self->weaponframe + 0.5;
	GruntyThink();
	GRun();
	///self.nextthink = time + 0.05;
	if (self->weaponframe > 3)
		if (self->health > 0) self->think = grunty_run;
}

// Grunty's fourth axe attack
void grunty_axeattd()
{
	self->frame = GruntyFrames::FR_AXATTD1 + floor(self->weaponframe);

	if (self->weaponframe == 2)
	{
		if (self->current_weapon == PR_WEAP_AXE)
			W_FireAxe();
		else
			W_FireSpanner();
	}
	self->weaponframe = self->weaponframe + 0.5;
	GruntyThink();
	GRun();
	///self.nextthink = time + 0.05;
	if (self->weaponframe > 3)
		if (self->health > 0) self->think = grunty_run;
}

// Axe attack chooser
void grunty_axeatt()
{
	MonsterAuraPower();

	float r;

	self->walkframe = 0; // Reset walkframe to avoid freaky animations?

	r = random();

	if (r < 0.25)
		grunty_axeatta();
	else if (r < 0.5)
		grunty_axeattb();
	else if (r < 0.75)
		grunty_axeattc();
	else
		grunty_axeattd();
}

/* ---------------------------------------------------------------------------------- */
//						GRUNTY - PART FOUR
//					Shooting Time
//
//						Grunty goes postal
/* ---------------------------------------------------------------------------------- */

// Grunty uses the shotgun
// The firing is done after he chooses the attack
void grunty_shotgun()
{
	MonsterAuraPower();

	self->frame = GruntyFrames::FR_SHOTATT1 + floor(self->weaponframe);

	if (self->weaponframe == 0)
		self->effects = self->effects | PR_EF_DIMLIGHT;
	else if (self->weaponframe == 0.5)
		self->effects = self->effects - (self->effects & PR_EF_DIMLIGHT);

	self->weaponframe = self->weaponframe + 0.5;
	GruntyThink();
	GRun();
	///self.nextthink = time + 0.05;
	if (self->weaponframe > 5)
	{
		self->walkframe = 0;
		if (self->ammo_shells < 1)
			PrintFromSoldier(self,self->real_owner,"i've run out of shells!\n",PR_PRINT_LOW);
		if (self->health > 0) self->think = grunty_run;
	}
}

// Grunty fires his rocket launcher! muahahahahaha
// The firing is done after he chooses the attack
void grunty_rocket()
{
	MonsterAuraPower();

	self->frame = GruntyFrames::FR_ROCKATT1 + floor(self->weaponframe);

	if (self->weaponframe == 0)
		self->effects = self->effects | PR_EF_DIMLIGHT;
	else if (self->weaponframe == 0.5)
		self->effects = self->effects - (self->effects & PR_EF_DIMLIGHT);

	self->weaponframe = self->weaponframe + 0.5;
	GruntyThink();
	GRun();
	///self.nextthink = time + 0.05;
	if (self->weaponframe > 5)
	{
		self->walkframe = 0;
		if (self->ammo_rockets < 1)
			PrintFromSoldier(self,self->real_owner,"i've run out of rockets!\n",PR_PRINT_LOW);
		if (self->health > 0) self->think = grunty_run;
	}
}

//- ofn - this to compensate no prediction
//- GRUNTY'S NAILS GO FASTER AS HIS RANK IS INCREASED (upto 2200)-//
void grunty_spike(const vector& org, const vector& dir)
{
	newmis = spawn ();
	newmis->owner = self;
	newmis->movetype = PR_MOVETYPE_FLYMISSILE;
	newmis->solid = PR_SOLID_BBOX;

	newmis->angles = vectoangles(dir);

	newmis->touch = spike_touch;
	newmis->weapon = PR_DMSG_NAILGUN;
	newmis->classname = "spike";
	newmis->think = SUB_Remove;
	newmis->nextthink = time + 6;
	setmodel (newmis, "progs/spike.mdl");
	setsize (newmis, PR_VEC_ORIGIN, PR_VEC_ORIGIN);
	setorigin (newmis, org);

	newmis->velocity = dir*(1100+1100*(self->has_sensor/17));
}

// Grunty uses the nailgun
void grunty_nail1()
{
	FRAME_STATE(GruntyFrames::FR_NAILATT1, grunty_nail2);
	MonsterAuraPower();

	GruntyThink(); GRun(); self->effects = self->effects | PR_EF_DIMLIGHT;
	if (self->ammo_nails > 0)
	{
		W_FireSpikes(4);
		if (self->ammo_nails < 1) PrintFromSoldier(self,self->real_owner,"i've run out of nails!\n",PR_PRINT_LOW);
	}
}
void grunty_nail2()
{
	FRAME_STATE(GruntyFrames::FR_NAILATT1, grunty_nail3);
	GruntyThink(); GRun(); self->effects = self->effects | PR_EF_DIMLIGHT;
}
void grunty_nail3()
{
	FRAME_STATE(GruntyFrames::FR_NAILATT2, grunty_nail4);
	GruntyThink(); GRun(); self->effects = self->effects - (self->effects & PR_EF_DIMLIGHT);
	if (self->ammo_nails > 0)
	{
		W_FireSpikes(-4);
		if (self->ammo_nails < 1) PrintFromSoldier(self,self->real_owner,"i've run out of nails!\n",PR_PRINT_LOW);
	}
}
float GruntyPickBestWeapon( entity bot, int allowedweaps, float dist);
void grunty_nail4()
{
	FRAME_STATE(GruntyFrames::FR_NAILATT2, grunty_nail1);
	GruntyThink(); GRun(); self->effects = self->effects - (self->effects & PR_EF_DIMLIGHT);

	float vis, dist;

	vis = visible(self->enemy);
	dist = vlen(self->origin - self->enemy->origin);

	// Gizmo - removed "dist < 200" from this, and added GruntyPickBestWeapon() check
	// Gizmo - grunty can be limited on his weapon's use (defaults to all weapons allowed)
	if (!vis || self->ammo_nails < 1 || self->enemy->health <= 0 || random() < 0.025
		|| GruntyPickBestWeapon( self, self->PR_allowed_weapons, dist ) != PR_WEAP_NAILGUN )
	{
		self->walkframe = 0;
		grunty_run();
		return;
	}
}

vector Grunty_LeadShot()
{
	vector pvel, dir, loc;//, lead;
	vector src, targ;

	src = self->origin + V({0, 0, 16});
	targ = self->enemy->origin;


	pvel = V({0, 0, 0}); // ofn - no prediction

	/*if (self.enemy.classname == "player")
		pvel='0 0 0';//pvel = self.enemy.velocity;
	else if (self.enemy.classname == "monster_army")
	{
		makevectors(self.enemy.angles);
		pvel = self.enemy.velocity + v_forward * self.enemy.custom_speed * 20;// * 10; //- ? was 20
	}
	else
		pvel = '0 0 0';*/

	//if (!self.enemy.flags & #FL_ONGROUND)
	//	pvel_z = pvel_z / 20;

	//lead = pvel * (vlen(src - targ) / ReturnWeaponVelocity());
	loc = targ;// + lead;

	// do the following do anything?-----//
	//lead = pvel * (vlen(src - loc) / ReturnWeaponVelocity());
	//lead_x = lead_x + (10 - random() * 20);
	//lead_y = lead_y + (10 - random() * 20);
	//------------------------------------//	self.view_ofs

	traceline(src, targ, PR_TL_BSP_ONLY, self);
	if (trace_fraction != 1.0)
	{
		loc[Z] = loc[Z] + self->enemy->maxs[Z]; // already commented
		//if (self.current_weapon == #WEAP_ROCKET_LAUNCHER)
		//	loc_z = loc_z + self.enemy.mins_z * -1;
	}
	else
	{
		//- OFN - makes grunty aim to the ground with RL sometimes, so evil! ^_^
		if (self->current_weapon == PR_WEAP_ROCKET_LAUNCHER && random() > 0.3) {
			// Gizmo - made a lot of changes to make him aim at feet better
			if ( self->enemy->flags & PR_FL_ONGROUND ) {
				loc[Z] = self->enemy->absmin[Z];

				// Gizmo - make sure their feet are actually visible
				traceline( src, loc, PR_TL_ANY_SOLID, self );

				// if feet aren't visible, aim back up
				if ( trace_fraction != 1 )
				if ( trace_ent != self->enemy )
					loc[Z] = self->enemy->origin[Z];
			}
		}
	}

	dir = normalize (loc - src);

	return dir;
}

/* ---------------------------------------------------------------------------------- */
//						GRUNTY - PART FIVE
//					Thinking Skills
//
//						Grunty reflects on what he has done
/* ---------------------------------------------------------------------------------- */

void GruntyThink()
{
	AI_Check_Contents(self);

	if (self->health <= 0) return;

	self->nextthink = time + 0.05; //was 0.05

	// check for Follow Me Grunty's owner death
	if (self->goalentity == self->real_owner)
		if (self->goalentity->health <= 0)
			self->goalentity = world;

	// Check to buy
	if (self->suicide_time <= time)
		GruntyPayThink();
	// scan for targets
	GruntyScanTargets();
}

/*
==============
IsValidGruntyTarget

Gizmo - Made a function from the code in LookAroundGrunty(),
and fixed up some pointless code that wasn't very efficient.
==============
*/
float IsValidGruntyTarget(entity gtarget, entity grunty)
{
#ifdef PR_MAD_MONSTERS
	if ( gtarget->classname == "player" && IsSummon( grunty ) )
		return PR_TRUE;
#else
	if ( gtarget->classname == "player" && !Teammate(gtarget, grunty->real_owner) ) {
		if ( gtarget->is_undercover == 1 ) {  // PZ: added "== 1" (2 is disguisING status)
			if ( gtarget->cutf_items & PR_CUTF_JAMMER || !( grunty->tf_items & PR_NIT_SCANNER ) )
				return PR_FALSE;
		}
		if ( gtarget->modelindex == modelindex_null ) {
			if ( gtarget->cutf_items & PR_CUTF_JAMMER || !( grunty->tf_items & PR_NIT_SCANNER ) )
				return PR_FALSE;
		}
		if ( gtarget->modelindex == modelindex_eyes ) {
			if ( gtarget->cutf_items & PR_CUTF_JAMMER || !( grunty->tf_items & PR_NIT_SCANNER ) ) {
				if ( random() < 2 - grunty->has_tesla * random() )
					return PR_FALSE;
			}
		}

		// Gizmo - used to be ( gtarget, grunty, 1, 0, 0, 0 ), but we already check visible before calling this
		return Pharse_Client( gtarget, grunty, 0, 0, 0, 0 );
	}
#endif

	// Gizmo - allow non demons and non army soldiers to be affected (coop monsters)
	else if (IsMonster(gtarget) && gtarget->health > 0)
	{
		//float temp = GetTeam(gtarget);
		if (!Teammate(gtarget, grunty->real_owner))
			return PR_TRUE;
	}
	else if (gtarget->classname == "grenade" && gtarget->netname == "land_mine")
	{
		if (!Teammate(gtarget->owner, grunty->real_owner))
			return PR_TRUE;
	}
	else if (!Teammate(gtarget, grunty->real_owner)) // && gtarget.classname != "building_sentrygun_base"
	{
		if (IsOffenseBuilding(gtarget)) // for teslas isoffensiveb only returns true if not cloaked
			return PR_TRUE;
	}

	return PR_FALSE;
}

// LookAround for grunty
entity LookAroundGrunty(entity scanner)
{
	entity		client;
//	local entity		list_client;
//	local entity		list;

	//if (infokey(world,"ceasefire")=="on") //OfN
	if (ceasefire)
		return scanner;

	client = findradius(scanner->origin, 2500);

	while (client != world)
	{
		// Gizmo - added a traceline so he doesn't shoot through teammates
		traceline( scanner->origin + V({0, 0, 16}), client->origin, PR_TL_ANY_SOLID, scanner );

		if ( trace_ent == client )
		if (client != scanner && visible2(client, scanner))
		{
			// Gizmo - use this function instead of the code commented out below
			if ( IsValidGruntyTarget( client, scanner ) )
				return client;
/*
			#ifdef MAD_MONSTERS

			if (client.classname == "player" && IsSummon(scanner)) gotatarget = 1;

			#else

			if (client.classname == "player" && !Teammate(client.team_no, scanner.real_owner.team_no))
			{
				gotatarget = Pharse_Client(client, scanner, 1, 0, 0, 0);

				if (client.is_undercover)
					if (client.cutf_items & #CUTF_JAMMER || !(scanner.tf_items & #NIT_SCANNER))
						gotatarget=0;
				if (client.modelindex == modelindex_null)
					if (client.cutf_items & #CUTF_JAMMER || !(scanner.tf_items & #NIT_SCANNER))
						gotatarget=0;
				if (client.modelindex == modelindex_eyes)
					if (client.cutf_items & #CUTF_JAMMER || !(scanner.tf_items & #NIT_SCANNER))
						if (random() < 2 - scanner.has_tesla * random())
							gotatarget=0;
			}

			#endif

			// Gizmo - allow non demons and non army soldiers to be affected (coop monsters)
			else if (IsMonster(client) && client.health > 0)
			{
				local float temp;

				temp = GetTeam( client );
				if (!Teammate(temp, scanner.real_owner.team_no))
					gotatarget = 1;
			}
			else if (client.classname == "grenade" && client.netname == "land_mine")
			{
				if (!Teammate(client.owner.team_no,scanner.real_owner.team_no))
					gotatarget = 1;
			}
			else if (!Teammate(client.team_no, scanner.real_owner.team_no))
			{ //  && client.classname != "building_sentrygun_base"
				if (IsOffenseBuilding(client)) // for teslas isoffensiveb only returns true if not cloaked
					gotatarget = 1;
			}
*/
		}
/*
		if (gotatarget)
			return client;
*/

		client = client->chain;
	}

	return scanner;
}


entity GruntyPharse()
{
	float r;

	r = random();

	#ifdef PR_MAD_GRUNTY

	if (r < 0.30 && (visible(self->real_owner)) && self->real_owner->health > 0)
		return self->real_owner;

	#else

	//- ofn - lot of shit changed in lookaround to make them target well
	if (r < 0.30)
		return LookAroundGrunty(self);

	#endif

	return world;
}

void GruntyScanTargets()
{
	entity	targ; // our hapless foe
	string	st;

	if (self->enemy != world)
		if (self->enemy->health <= 0 || !self->enemy->is_connected)
		{
			self->enemy = world;
			self->goalentity = ReturnEasyWaypoint(self,self);

		   //// self.enemy = world;
			//// self.goalentity = world;
			if (self->demon_one != world)
			{
				RemoveWaypoint(self->demon_one, self);
				self->demon_one = world;
			}

			self->has_teleporter = 0;
			self->effects = self->effects - (self->effects & PR_EF_DIMLIGHT);
			return;
		}
	// If we have a target already
	if (self->enemy != world)
	{
		float vis;
		vis = visible(self->enemy);

		// Gizmo - added a traceline so he doesn't shoot through teammates
		traceline( self->origin + V({0, 0, 16}), self->enemy->origin, PR_TL_ANY_SOLID, self );

		if ( vis && trace_ent == self->enemy )
		{
			if (self->demon_one != world)
			{
				RemoveWaypoint(self->demon_one, self);
				self->demon_one = world;
			} //often =world
			self->search_time = time + 3;
			if (self->attack_finished <= time)
				GruntyCheckFire();
		}
		else
		{
			if (self->is_malfunctioning == 0 || self->is_malfunctioning == 2)
			{
				self->enemy = world;
				self->goalentity = ReturnEasyWaypoint(self, self);

				self->has_teleporter = 0;
				self->effects = self->effects - (self->effects & PR_EF_DIMLIGHT);

				return;
			}
			// tactic

			// Gizmo - if we're seeking out an enemy, check if another one is visible so we don't get lamed
			targ=GruntyPharse();
			if ( targ != self )
			if ( targ != world )
				self->search_time = 0;	// Gizmo - hack to reuse the code below

			if (self->demon_one == world) // if we don't have a enemy last seen marker
			{
				self->demon_one = CreateWaypoint(self->enemy->origin - self->enemy->velocity * 0.05, PR_WAYPOINT_AI_LIFE, PR_WAYPOINT_TYPE_ENEMYLASTSEEN, self);
				self->demon_one->goalentity = ReturnEasyWaypoint(self, self->demon_one);
				self->goalentity = self->demon_one;
				self->search_time = time + PR_GRUNTY_SEEKTIME; // was 8
				//- OfN -
				self->has_teleporter = 0;
			}
			else if (self->search_time <= time) // we seeked enemy for GRUNTY_SEEKTIME already so...
			{
				self->enemy = world;
				self->goalentity = world;

				if (self->demon_one != world) // d1 remove contingency check
				{
					RemoveWaypoint(self->demon_one, self);
					self->demon_one = world;
					self->goalentity = ReturnEasyWaypoint(self, self);
				}
			}

		}
	}
	else // otherwise look for a target
	{
		targ=GruntyPharse();

		if (targ == self)
			return;
		if (targ == world)
			return;

		if (!self->is_detpacking || ceasefire)//infokey(world,"ceasefire")=="on")
		{
			if (!IsBuilding(targ) && self->super_time < time && targ->netname != "land_mine")
			{
				st = strcat("i can see ",targ->netname);
				st = strcat(st," around there...\n");

				PrintFromSoldier(self,self->real_owner,st,PR_PRINT_HIGH);
				//sprint(self.real_owner, #PRINT_HIGH, targ.netname);
				//sprint(self.real_owner, #PRINT_HIGH, " around there...\n");
				self->super_time = time + 4;
			}
			return;
		}

		if (self->super_time < time)
		{
			//local string targetstr;
			//targetstr=GetEnemyName(targ);//targ.netname;

			st = strcat("Target ",targ->netname);
			st = strcat(st, " spotted!\n Engaging target...\n");
			PrintFromSoldier(self,self->real_owner,st,PR_PRINT_HIGH);

			//sprint(self.real_owner, #PRINT_HIGH, targ.netname);
			//sprint(self.real_owner, #PRINT_HIGH, " spotted!\n Engaging target...\n");
			self->super_time = time + 2.5;
		}

		self->enemy = targ;
		// Gizmo - check for static mode
		if ( self->is_malfunctioning != 2 )
			self->goalentity = self->enemy;
		self->search_time = time + 3;
	}
}

// Gizmo
float GruntyPickBestWeapon( entity bot, int allowedweaps, float dist) {
	float enemySpeed;

	// used for deciding which weapon to use
	if ( bot->enemy->flags & PR_FL_ONGROUND && IsMonster( bot->enemy ) )
		enemySpeed = 0;
	else {
		// Gizmo - this works, but I want the bot to ignore forward and backward enemy movement because only enemies strafing around the grunt matter
		//enemySpeed = vlen( bot.enemy.velocity );
		// Gizmo - NOTE: I don't think the previous v_* vectors need to be restored upon return
		makevectors( bot->angles );
		enemySpeed = bot->enemy->velocity * ( v_right + v_up );
		enemySpeed = fabs( enemySpeed );
	}

	if ( dist < 64 ) {
		// use the knife and super shotgun first
		if ( bot->weapons_carried & PR_WEAP_AXE && allowedweaps & PR_WEAP_AXE && bot->cutf_items & PR_CUTF_KNIFE )
			return PR_WEAP_AXE;
		if ( bot->weapons_carried & PR_WEAP_SUPER_SHOTGUN && allowedweaps & PR_WEAP_SUPER_SHOTGUN
			&& bot->ammo_shells > 1 )
			return PR_WEAP_SUPER_SHOTGUN;

		// opportunistic weapon usage
		if ( bot->weapons_carried & PR_WEAP_NAILGUN && allowedweaps & PR_WEAP_NAILGUN
			&& bot->ammo_nails > 150 )
			return PR_WEAP_NAILGUN;

		// standard weapon priority
		if ( bot->weapons_carried & PR_WEAP_AXE && allowedweaps & PR_WEAP_AXE )
			return PR_WEAP_AXE;
		if ( bot->weapons_carried & PR_WEAP_SHOTGUN && allowedweaps & PR_WEAP_SHOTGUN
			&& bot->ammo_shells > 0 )
			return PR_WEAP_SHOTGUN;
		if ( bot->weapons_carried & PR_WEAP_NAILGUN && allowedweaps & PR_WEAP_NAILGUN
			&& bot->ammo_nails > 0 )
			return PR_WEAP_NAILGUN;
		if ( bot->weapons_carried & PR_WEAP_ROCKET_LAUNCHER && allowedweaps & PR_WEAP_ROCKET_LAUNCHER
			&& bot->ammo_rockets > 0 )
			return PR_WEAP_ROCKET_LAUNCHER;

		// at this range we try them all
		return 0;
	} else if ( dist < 200 ) {
		// use the super shotgun first
		if ( bot->weapons_carried & PR_WEAP_SUPER_SHOTGUN && allowedweaps & PR_WEAP_SUPER_SHOTGUN
			&& bot->ammo_shells > 1 )
			return PR_WEAP_SUPER_SHOTGUN;

		// opportunistic weapon usage
		if ( bot->weapons_carried & PR_WEAP_NAILGUN && allowedweaps & PR_WEAP_NAILGUN
			&& bot->ammo_nails > 100 )
			return PR_WEAP_NAILGUN;

		// standard weapon priority
		if ( bot->weapons_carried & PR_WEAP_SHOTGUN && allowedweaps & PR_WEAP_SHOTGUN
			&& bot->ammo_shells > 0 )
			return PR_WEAP_SHOTGUN;
		if ( bot->weapons_carried & PR_WEAP_ROCKET_LAUNCHER && allowedweaps & PR_WEAP_ROCKET_LAUNCHER
			&& bot->ammo_rockets > 0 )
			return PR_WEAP_ROCKET_LAUNCHER;
		if ( bot->weapons_carried & PR_WEAP_NAILGUN && allowedweaps & PR_WEAP_NAILGUN
			&& bot->ammo_nails > 0 )
			return PR_WEAP_NAILGUN;
	} else if ( dist < 400 ) {
		// use the rocket launcher first
		if ( bot->weapons_carried & PR_WEAP_ROCKET_LAUNCHER && allowedweaps & PR_WEAP_ROCKET_LAUNCHER
			&& bot->ammo_rockets > 0 )
			return PR_WEAP_ROCKET_LAUNCHER;

		// opportunistic weapon usage
		if ( bot->weapons_carried & PR_WEAP_NAILGUN && allowedweaps & PR_WEAP_NAILGUN
			&& bot->ammo_nails > 50 && enemySpeed < 100 )
			return PR_WEAP_NAILGUN;

		// standard weapon priority
		if ( bot->weapons_carried & PR_WEAP_SHOTGUN && allowedweaps & PR_WEAP_SHOTGUN
			&& bot->ammo_shells > 0 )
			return PR_WEAP_SHOTGUN;
		if ( bot->weapons_carried & PR_WEAP_SUPER_SHOTGUN && allowedweaps & PR_WEAP_SUPER_SHOTGUN
			&& bot->ammo_shells > 1 )
			return PR_WEAP_SUPER_SHOTGUN;
		if ( bot->weapons_carried & PR_WEAP_NAILGUN && allowedweaps & PR_WEAP_NAILGUN
			&& bot->ammo_nails > 0 )
			return PR_WEAP_NAILGUN;
	} else if ( dist < 1100 ) {
		// use the rocket launcher first
		if ( bot->weapons_carried & PR_WEAP_ROCKET_LAUNCHER && allowedweaps & PR_WEAP_ROCKET_LAUNCHER
			&& bot->ammo_rockets > 0 )
			return PR_WEAP_ROCKET_LAUNCHER;

		// opportunistic weapon usage
		if ( bot->weapons_carried & PR_WEAP_NAILGUN && allowedweaps & PR_WEAP_NAILGUN
			&& bot->ammo_nails > 0 && enemySpeed < 100 )
			return PR_WEAP_NAILGUN;

		// standard weapon priority
		if ( bot->weapons_carried & PR_WEAP_SHOTGUN && allowedweaps & PR_WEAP_SHOTGUN
			&& bot->ammo_shells > 0 )
			return PR_WEAP_SHOTGUN;
		if ( bot->weapons_carried & PR_WEAP_NAILGUN && allowedweaps & PR_WEAP_NAILGUN
			&& bot->ammo_nails > 0 )
			return PR_WEAP_NAILGUN;
		if ( bot->weapons_carried & PR_WEAP_SUPER_SHOTGUN && allowedweaps & PR_WEAP_SUPER_SHOTGUN
			&& bot->ammo_shells > 1 )
			return PR_WEAP_SUPER_SHOTGUN;
	} else if ( dist < 1400 ) {
		// try the rocket launcher first
		if ( bot->weapons_carried & PR_WEAP_ROCKET_LAUNCHER && allowedweaps & PR_WEAP_ROCKET_LAUNCHER
			&& bot->ammo_rockets > 0 && random() > 0.75 )
			return PR_WEAP_ROCKET_LAUNCHER;

		// opportunistic weapon usage
		if ( bot->weapons_carried & PR_WEAP_NAILGUN && allowedweaps & PR_WEAP_NAILGUN
			&& bot->ammo_nails > 0 && enemySpeed < 75 )
			return PR_WEAP_NAILGUN;

		// standard weapon priority
		if ( bot->weapons_carried & PR_WEAP_SHOTGUN && allowedweaps & PR_WEAP_SHOTGUN
			&& bot->ammo_shells > 0 )
			return PR_WEAP_SHOTGUN;
		if ( bot->weapons_carried & PR_WEAP_NAILGUN && allowedweaps & PR_WEAP_NAILGUN
			&& bot->ammo_nails > 0 )
			return PR_WEAP_NAILGUN;
		if ( bot->weapons_carried & PR_WEAP_ROCKET_LAUNCHER && allowedweaps & PR_WEAP_ROCKET_LAUNCHER
			&& bot->ammo_rockets > 0 )
			return PR_WEAP_ROCKET_LAUNCHER;

		// this is probably out of the super shotgun range
		if ( bot->weapons_carried & PR_WEAP_SUPER_SHOTGUN && allowedweaps & PR_WEAP_SUPER_SHOTGUN
			&& bot->ammo_shells > 1 )
			return PR_WEAP_SUPER_SHOTGUN;
	} else if ( dist < 2000 ) {
		if ( bot->weapons_carried & PR_WEAP_ROCKET_LAUNCHER && allowedweaps & PR_WEAP_ROCKET_LAUNCHER
			&& bot->ammo_rockets > 0 && random() > 0.98 )
			return PR_WEAP_ROCKET_LAUNCHER;

		// standard weapon priority
		if ( bot->weapons_carried & PR_WEAP_NAILGUN && allowedweaps & PR_WEAP_NAILGUN
			&& bot->ammo_nails > 0 )
			return PR_WEAP_NAILGUN;
		if ( bot->weapons_carried & PR_WEAP_ROCKET_LAUNCHER && allowedweaps & PR_WEAP_ROCKET_LAUNCHER
			&& bot->ammo_rockets > 0 )
			return PR_WEAP_ROCKET_LAUNCHER;

		// these weapons won't do much if anything at this range
		if ( bot->weapons_carried & PR_WEAP_SHOTGUN && allowedweaps & PR_WEAP_SHOTGUN
			&& bot->ammo_shells > 0 )
			return PR_WEAP_SHOTGUN;
		if ( bot->weapons_carried & PR_WEAP_SUPER_SHOTGUN && allowedweaps & PR_WEAP_SUPER_SHOTGUN
			&& bot->ammo_shells > 1 )
			return PR_WEAP_SUPER_SHOTGUN;
	} else {
		// use the rocket launcher and nailgun randomly
		if ( bot->weapons_carried & PR_WEAP_ROCKET_LAUNCHER && allowedweaps & PR_WEAP_ROCKET_LAUNCHER
			&& bot->ammo_rockets > 0 && random() > 0.5 )
			return PR_WEAP_ROCKET_LAUNCHER;

		// standard weapon priority
		if ( bot->weapons_carried & PR_WEAP_NAILGUN && allowedweaps & PR_WEAP_NAILGUN
			&& bot->ammo_nails > 0 )
			return PR_WEAP_NAILGUN;
		if ( bot->weapons_carried & PR_WEAP_ROCKET_LAUNCHER && allowedweaps & PR_WEAP_ROCKET_LAUNCHER
			&& bot->ammo_rockets > 0 )
			return PR_WEAP_ROCKET_LAUNCHER;

		// these weapons won't do much if anything at this range
		if ( bot->weapons_carried & PR_WEAP_SHOTGUN && allowedweaps & PR_WEAP_SHOTGUN
			&& bot->ammo_shells > 0 )
			return PR_WEAP_SHOTGUN;
		if ( bot->weapons_carried & PR_WEAP_SUPER_SHOTGUN && allowedweaps & PR_WEAP_SUPER_SHOTGUN
			&& bot->ammo_shells > 1 )
			return PR_WEAP_SUPER_SHOTGUN;
	}

	// all we can use is an axe, oh well, it'll look funny and it'll let the owner know he's out of ammo
	if ( bot->weapons_carried & PR_WEAP_AXE && allowedweaps & PR_WEAP_AXE )
		return PR_WEAP_AXE;
	return 0;
}


void GruntyCheckFire()
{
	float dist, vis, allowedweaps;

	if (self->attack_finished > time)
		return;

	if (self->enemy == world)
		return;

	//if (infokey(world,"ceasefire")=="on")
	if (ceasefire)
	{
		self->goalentity=ReturnEasyWaypoint(self,self);
		self->enemy=world;
		return;
	}

	vis = visible(self->enemy);
	if (!vis)
	{
		#ifdef PR_VERBOSE_GRUNTY
		//if (self.super_time <= time)
		//{
			sprint(self->real_owner, PR_PRINT_HIGH, "Target not visible! Cancelling firing...\n");
			self->super_time = time + 5;
		//}
		#endif

		//self.weaponframe = 0;
		//self.think = grunty_run;
		//self.nextthink = time + 0.05;
		return;
	}

	//-- OFN --// no need as yaw is set in GRun
	//enemy_yaw = vectoyaw(self.enemy.origin - self.origin);
	//self.ideal_yaw = enemy_yaw;
	//ChangeYaw();
	//---------//

	/*if (!FacingIdeal())
		return; */

	dist = vlen(self->origin - self->enemy->origin);
	self->weaponframe = 0;

	// Gizmo - grunty can be limited on his weapon's use (defaults to all weapons allowed)
	allowedweaps = self->PR_allowed_weapons;

	if ( !allowedweaps )
		return;

#ifdef PR_NEVER_DEFINED
	if (dist < 64 && self->weapons_carried & PR_WEAP_AXE && allowedweaps & PR_WEAP_AXE && self->cutf_items & PR_CUTF_KNIFE)
	{
		self->current_weapon = PR_WEAP_AXE;
		/*if (self.cutf_items & #CUTF_KNIFE)
			self.attack_finished = time + 0.5;
		else
			self.attack_finished = time + 0.5;*/
		Attack_Finished(0.5);
		if (self->health > 0) self->think = grunty_axeatt;
		#ifdef PR_VERBOSE_GRUNTY
		sprint(self->real_owner, PR_PRINT_HIGH, "Attempting to knife...\n");
		#endif
		return;
	}
	if (dist < 200 && self->weapons_carried & PR_WEAP_SUPER_SHOTGUN && allowedweaps & PR_WEAP_SUPER_SHOTGUN)
	{
		if (self->ammo_shells > 1)
		{
			self->current_weapon = PR_WEAP_SUPER_SHOTGUN;
			//self.attack_finished = time + 0.7;
			Attack_Finished(0.7);
			W_FireSuperShotgun();
			if (self->health > 0) self->think = grunty_shotgun;
			#ifdef PR_VERBOSE_GRUNTY
			sprint(self->real_owner, PR_PRINT_HIGH, "Attempting to super shotgun...\n");
			#endif
			return;
		}
	}
	if (dist < 64 && self->weapons_carried & PR_WEAP_AXE && allowedweaps & PR_WEAP_AXE)
	{
		self->current_weapon = PR_WEAP_AXE;
		/*if (self.cutf_items & #CUTF_KNIFE)
			self.attack_finished = time + 0.5;
		else
			self.attack_finished = time + 0.5;*/
		Attack_Finished(0.5);
		if (self->health > 0) self->think = grunty_axeatt;
		#ifdef PR_VERBOSE_GRUNTY
		sprint(self->real_owner, PR_PRINT_HIGH, "Attempting to axe...\n");
		#endif
		return;
	}						//was 800
	if (dist > 200 && dist < 1100 && self->weapons_carried & PR_WEAP_ROCKET_LAUNCHER && allowedweaps & PR_WEAP_ROCKET_LAUNCHER)
	{
		if (self->ammo_rockets > 1)
		{
			self->current_weapon = PR_WEAP_ROCKET_LAUNCHER;
			//self.attack_finished = time + 0.8;
			Attack_Finished(0.8);
			W_FireRocket();
			if (self->health > 0) self->think = grunty_rocket;
			#ifdef PR_VERBOSE_GRUNTY
			sprint(self->real_owner, PR_PRINT_HIGH, "Attempting to rocket...\n");
			#endif
			return;
		}
	}
	if (dist < 1400 && self->weapons_carried & PR_WEAP_ROCKET_LAUNCHER && random() > 0.75 && allowedweaps & PR_WEAP_ROCKET_LAUNCHER)
	{
		if (self->ammo_rockets > 1)
		{
			self->current_weapon = PR_WEAP_ROCKET_LAUNCHER;
			//self.attack_finished = time + 0.8;
			Attack_Finished(0.8);
			W_FireRocket();
			if (self->health > 0) self->think = grunty_rocket;
			return;
		}
	}
	if (dist < 2000 && self->weapons_carried & PR_WEAP_ROCKET_LAUNCHER && self->ammo_rockets > 1 && random() > 0.98 && allowedweaps & PR_WEAP_ROCKET_LAUNCHER)
	{
		self->current_weapon = PR_WEAP_ROCKET_LAUNCHER;
		//self.attack_finished = time + 0.8;
		Attack_Finished(0.8);
		W_FireRocket();
		if (self->health > 0) self->think = grunty_rocket;
		return;
	}

	//- OfN
	if (dist < 400 && self->weapons_carried & PR_WEAP_SHOTGUN && self->ammo_shells > 0 && allowedweaps & PR_WEAP_SHOTGUN)
	{
		//if (self.ammo_shells > 0)
		//{
			self->current_weapon = PR_WEAP_SHOTGUN;
			//self.attack_finished = time + 0.5; // TODO: MAKE ATTACK FINISHED
			Attack_Finished(0.5);
			W_FireShotgun();
			if (self->health > 0) self->think = grunty_shotgun;
			#ifdef PR_VERBOSE_GRUNTY
			sprint(self->real_owner, PR_PRINT_HIGH, "Attempting to shotgun...\n");
			#endif
//			  RPrint("SHOTGUN\n");
			return;
		//}
	}
	if (dist < 2000 && self->weapons_carried & PR_WEAP_NAILGUN && self->ammo_nails > 0 && allowedweaps & PR_WEAP_NAILGUN)
	{
		//if (self.ammo_nails > 0)
		//{
			self->current_weapon = PR_WEAP_NAILGUN;
			//self.attack_finished = time + 0.2;//+ 0.1;
			Attack_Finished(0.2);
			if (self->health > 0) self->think = grunty_nail1;
			#ifdef PR_VERBOSE_GRUNTY
			sprint(self->real_owner, PR_PRINT_HIGH, "Attempting to nail...\n");
			#endif
//			  RPrint("NAIL\n");
			return;
		//}
	}
	else
	{
		// Do whatever you want
		self->effects = self->effects - (self->effects & PR_EF_DIMLIGHT);
		//self.walkframe = 0; ofn
		if (self->health > 0) self->think = grunty_run;
		#ifdef PR_VERBOSE_GRUNTY
		sprint(self->real_owner, PR_PRINT_HIGH, "GruntyCheckFire defaulting to none...\n");
		#endif
//		  RPrint("NONE\n");
	}
#endif

	allowedweaps = GruntyPickBestWeapon( self, allowedweaps, dist );
	if ( allowedweaps == PR_WEAP_AXE ) {
		self->current_weapon = PR_WEAP_AXE;
		Attack_Finished(0.5);
		if (self->health > 0) self->think = grunty_axeatt;
		#ifdef PR_VERBOSE_GRUNTY
		sprint(self->real_owner, PR_PRINT_HIGH, "Attempting to axe/knife...\n");
		#endif
		return;
	}
	if ( allowedweaps == PR_WEAP_SHOTGUN ) {
		self->current_weapon = PR_WEAP_SHOTGUN;
		Attack_Finished(0.5);
		W_FireShotgun();
		if (self->health > 0) self->think = grunty_shotgun;
		#ifdef PR_VERBOSE_GRUNTY
		sprint(self->real_owner, PR_PRINT_HIGH, "Attempting to shotgun...\n");
		#endif
		return;
	}
	if ( allowedweaps == PR_WEAP_SUPER_SHOTGUN ) {
		self->current_weapon = PR_WEAP_SUPER_SHOTGUN;
		Attack_Finished(0.7);
		W_FireSuperShotgun();
		if (self->health > 0) self->think = grunty_shotgun;
		#ifdef PR_VERBOSE_GRUNTY
		sprint(self->real_owner, PR_PRINT_HIGH, "Attempting to super shotgun...\n");
		#endif
		return;
	}
	if ( allowedweaps == PR_WEAP_ROCKET_LAUNCHER ) {
		self->current_weapon = PR_WEAP_ROCKET_LAUNCHER;
		Attack_Finished(0.8);
		W_FireRocket();
		if (self->health > 0) self->think = grunty_rocket;
		#ifdef PR_VERBOSE_GRUNTY
		sprint(self->real_owner, PR_PRINT_HIGH, "Attempting to rocket...\n");
		#endif
		return;
	}
	if ( allowedweaps == PR_WEAP_NAILGUN ) {
		self->current_weapon = PR_WEAP_NAILGUN;
		Attack_Finished(0.2);
		if (self->health > 0) self->think = grunty_nail1;
		#ifdef PR_VERBOSE_GRUNTY
		sprint(self->real_owner, PR_PRINT_HIGH, "Attempting to nail...\n");
		#endif
		return;
	}

	// nothing to shoot
	self->effects = self->effects - (self->effects & PR_EF_DIMLIGHT);
	if (self->health > 0) self->think = grunty_run;
	#ifdef PR_VERBOSE_GRUNTY
	sprint(self->real_owner, PR_PRINT_HIGH, "GruntyCheckFire defaulting to none...\n");
	#endif
}

/*
==============
GruntyPickClosestVector

Picks the closest vector to the vector specified.
Simulates real player movement by limiting to forward,
backward, strafe left, and strafe right like normal players.

NOTE: makevectors must be called first
==============
*/
vector GruntyPickClosestVector(vector desired)
{
	vector	bestv, tempv;
	float		bestdot;

	// flatten it out
	desired[Z] = 0;
	desired = normalize( desired );

	// start with forward
	bestv = v_forward;
	bestdot = desired * v_forward;

	tempv = v_forward * -1;
	if ( desired * tempv > bestdot ) {
		bestv = tempv;
		bestdot = desired * tempv;
	}
	tempv = v_right;
	if ( desired * tempv > bestdot ) {
		bestv = tempv;
		bestdot = desired * tempv;
	}
	tempv = v_right * -1;
	if ( desired * tempv > bestdot ) {
		bestv = tempv;
		bestdot = desired * tempv;
	}

	tempv = normalize( v_forward + v_right );
	if ( desired * tempv > bestdot ) {
		bestv = tempv;
		bestdot = desired * tempv;
	}
	tempv = normalize( v_forward + v_right * -1 );
	if ( desired * tempv > bestdot ) {
		bestv = tempv;
		bestdot = desired * tempv;
	}
	tempv = normalize( v_forward * -1 + v_right );
	if ( desired * tempv > bestdot ) {
		bestv = tempv;
		bestdot = desired * tempv;
	}
	tempv = normalize( v_forward * -1 + v_right * -1 );
	if ( desired * tempv > bestdot ) {
		bestv = tempv;
		bestdot = desired * tempv;
	}

	return bestv;
}

/*
==============
GruntyAutoObstacle

Gizmo - This should only be called when the grunty
is on the "auto" "on obstacle" mode.

Returns 0 if the grunty should jump, returns 1
if the grunty should stop (like when attacking an enemy),
and returns 2 when the grunty should attempt to
navigate to his goalentity (like monsters do).

NOTE: makevectors must be called first
==============
*/
float GruntyAutoObstacle( entity g) {
	vector	vtemp;
	float		zdif, vis, zdif2;

	// we hit an obstacle with no enemy
	if ( g->enemy == world ) {
		// jump if we're not on valid ground
		if ( !checkbottom( g ) )
			return 0;

		// FIXME: use absmin_z
		zdif = ( g->goalentity->origin[Z] + g->goalentity->mins[Z] ) - ( g->origin[Z] + g->mins[Z] );

		// check for visibility
		traceline( g->origin + g->view_ofs, g->goalentity->origin + g->goalentity->view_ofs, PR_TL_ANY_SOLID, g );
		if ( trace_inopen && trace_inwater )
			vis = PR_FALSE;
		else if ( trace_ent == g->goalentity || trace_fraction == 1 )
			vis = PR_TRUE;
		else
			vis = PR_FALSE;

		// if we're already navigating then use the vector facing our goalentity
		if ( g->motd > time ) {
			vtemp = g->goalentity->origin - g->origin;
			vtemp[Z] = 0;
			vtemp = normalize( vtemp );
		} else
			vtemp = v_forward;

		traceline( g->origin + V({0, 0, -8}), g->origin + V({0, 0, -8}) + vtemp * 96, PR_TL_ANY_SOLID, g );

		// goalentity is on a higher/lower level
		if ( zdif && trace_fraction == 1 ) {		// only jump if we have a clear path infront of us and our goalentity is up/down somewhere
			// don't jump if we've just got done attacking an enemy, so we don't jump in the wrong direction
			if ( g->attack_finished - time > -1 )
				return 2;

			// check if it's safe to jump here
			traceline( trace_endpos, trace_endpos + v_up * -8192, PR_TL_ANY_SOLID, g );

			// don't jump into water/slime/lava
			if ( trace_inopen && trace_inwater )
				return 2;

			zdif2 = ( g->goalentity->origin[Z] + g->goalentity->mins[Z] ) - trace_endpos[Z];
			traceline( trace_endpos, g->goalentity->origin, PR_TL_ANY_SOLID, g );

			// if our goalentity isn't visible at the estimated landing position then don't jump
			if ( trace_ent != g->goalentity && trace_fraction != 1 )
				return 2;

			// if our landing position is farther away z wise then don't jump
			zdif2 = fabs( zdif2 );		// Gizmo - I've seen QC compiler/vm bugs when two functions calls are in the same expression
			if ( zdif2 > fabs( zdif ) )
				return 2;

			return 0;
		}
		if ( zdif && trace_fraction != 1 ) {
			// don't jump if we've just got done attacking an enemy, so we don't jump in the wrong direction
			if ( g->attack_finished - time > -1 )
				return 2;

			if ( vis )
				return 0;

			return 2;
		}
		// we're perfectly even with our goalentity, assuming it's a ground based target
		if ( !zdif )
			return 2;
		return 2;
	}

	// seek tactic mode
	if ( g->is_malfunctioning == 1 ) {
		// only allow navigating on seek mode if our enemy isn't visible
		// don't ever jump on seek mode, to avoid being lured into water/slime/lava
		if ( visible2( g->enemy, g ) )
			return 1;
		return 2;
	}
	// static tactic mode
	if ( g->is_malfunctioning == 2 ) {
		// always stop on static mode when attacking
		return 1;
	}

	// default to stop
	return 1;
}

void GRun()
{
	float		dist;
	vector	loc;
	float		r;

	// Gizmo - .motd is set to a future time to disallow ChangeYaw() while we're calling movetogoal()
	if ( self->motd <= time || self->is_toffingadet != 2 ) {
		// ok, we're trying to get somewhere
		// first look at where we are trying to go
		// Version 1.0 of GruntyLead(tm)
		// Gizmo - he wasn't actually using his last seen enemy waypoint, I fixed it so he does; this was all that was needed...
		if ( self->enemy != world && ( self->goalentity != self->demon_one || self->demon_one == world ) )
		{
			vector pvel;

			if (self->enemy->classname == "player")
				pvel = self->enemy->velocity;
			else if (self->enemy->classname == "monster_army")
			{
				makevectors(self->enemy->angles);
				pvel = self->enemy->velocity + v_forward * self->enemy->custom_speed * 20;
			}
			else
				pvel = V({0, 0, 0});

			if (ReturnWeaponVelocity() == 99999999)
				pvel = V({0, 0, 0});

			loc = self->enemy->origin + pvel * (vlen(self->enemy->origin - self->origin) / ReturnWeaponVelocity());
			self->ideal_yaw = vectoyaw(loc - self->origin);
			self->ideal_yaw = anglemod(self->ideal_yaw);
		}
		else
		{
			if (self->goalentity!=world) //- OfN - dont change yaw if we r not after a goal
			{
				self->ideal_yaw = vectoyaw(self->goalentity->origin - self->origin);
				self->ideal_yaw = anglemod(self->ideal_yaw);
			}
		}

		ChangeYaw();
	}

	// makevectors sets the global vars v_forward, v_right and v_up
	makevectors(self->angles);

	dist = self->custom_speed;

	if (self->tfstate & PR_TFSTATE_TRANQUILISED)
		dist = dist * (PR_AI_TRANQ_FACTOR_UP / PR_AI_TRANQ_FACTOR_DN); //Tranq

	// Gizmo - when on static mode, we want to face our enemy while moving towards our goalentity
	if ( self->is_malfunctioning == 2 ) {
		if ( self->goalentity != world )
			botmovedist( vectoyaw( GruntyPickClosestVector( self->goalentity->origin - self->origin ) ), dist );
		return;
	}

	/* - OfN -if (self.enemy.classname == "monster_shambler" || self.enemy.weapons_carried & #WEAP_ASSAULT_CANNON)
		if (self.has_teleporter == 0)
			self.has_teleporter = 3;*/

	if (random() < 0.02) // to prevent us from strafing or whatever forever.
		self->has_teleporter = 0;

	// Test to see if we want to do an evasive maneuver
	// If we're already doing a move, don't do another
	if (self->has_teleporter == -1) // left dodge move
	{
		// Gizmo - if blocked while doing strafe do another move
		if ( !botmovedist( vectoyaw( v_right * -1 ), dist ) )	// move left
			self->has_teleporter = 0;
	}
	else if (self->has_teleporter == 1) // right dodge move
	{
		// Gizmo - if blocked while doing strafe do another move
		if ( !botmovedist( vectoyaw( v_right ), dist ) )	// move right
			self->has_teleporter = 0;
	}
	else if (self->has_teleporter == 2) // jump!
	{
		if (self->flags & PR_FL_ONGROUND) // only jump if we're on the ground
		{
			self->velocity[Z] = self->velocity[Z] + 270; // boing
			//sound (self, #CHAN_BODY, "player/plyrjmp8.wav", 1, #ATTN_NORM);
			sound (self, PR_CHAN_VOICE, "player/plyrjmp8.wav", 1, PR_ATTN_NORM);
		}
		self->has_teleporter = 0; // don't jump
	}
	else if (self->has_teleporter == 3) //(backwards)// HE'S AFTER US, RUN!@#$ HE WILL KILL US ALL!!@
	{
		if ( !botmovedist( vectoyaw( v_forward ), dist * -1 ) ) { // flee, I tell you!
			// Gizmo - if he's blocked while trying to run backwards with preferred range, then strafe
			if ( self->enemy != world ) {
				// check if preferred range is set
				if ( self->is_malfunctioning != 2 && self->delay_time ) {
					if ( random() < 0.5 )
						self->has_teleporter = 1;
					else
						self->has_teleporter = -1;
				}
			}
		}
	}
	else if (self->has_teleporter == 0) // no evasive, standard move (forward)
	{
		botmovedist(vectoyaw(v_forward), dist); // move

		//- OfN - force move to forward if we r going to a last seen enemy waypoint
		// Gizmo - removed the duplicate botmovedist() call and moved this after the main botmovedist() call
		if ( self->goalentity == self->demon_one && self->demon_one != world )
			return;

		// Gizmo - modified this for preferred range
		if ( self->enemy != world ) {
			// check if preferred range is set and we need to stay within range
			if ( self->is_malfunctioning != 2 && self->delay_time ) {
				r = vlen( self->enemy->origin - self->origin );

				if ( self->delay_time == 1 ) {
					if ( r >= 200 ) {
						self->has_teleporter = 0;
						return;
					}
				} else if ( self->delay_time == 2 ) {
					if ( r >= 1100 ) {
						self->has_teleporter = 0;
						return;
					} else if ( r < 400 ) {
						self->has_teleporter = 3;
						return;
					}
				} else if ( self->delay_time == 3 ) {
					if ( r < 800 ) {
						self->has_teleporter = 3;
						return;
					}
				}
			}
			if (random() * 20 < 1 ) {					// if we get a score of one on a d20, special
				r = random();
				if (r < 0.25)
					self->has_teleporter = -1; // left
				else if (r < 0.5)
					self->has_teleporter = 1; // right
				else if (r < 0.75)
					self->has_teleporter = 2; // jump!
				else
					self->has_teleporter = 3; // italian style!
			}
		}
	}
}

// Gizmo - changed return type to float
float botmovedist(float angle, float dist)
{
	float		success;
	float		obstacleMode;

	//- OfN - Don't annoy owner if following him! heh
	#ifndef PR_MAD_GRUNTY
	if (self->goalentity==self->real_owner) //if in "follow me!" state...
	{
		float dist2;

		dist2 = vlen(self->origin - self->real_owner->origin);

		// Gizmo - prevent grunties from cornering you in tight places
		if ( dist2 < 40 )
			dist = dist * -1;
		else if (dist2 < 64)
		{
			// Gizmo - only do stand frames if not in an attack
			if ( self->attack_finished < time )
				stand_frames();
			self->has_cheated = time + 0.5;
			walkmove(angle, 0);
			return PR_TRUE;
		}
	}///////////////////////////////*/
	#endif

	/*if (pointcontents(self.origin) == #CONTENT_EMPTY) // We're in open air
	{*/

		if ( self->motd > time && self->is_toffingadet == 2 ) {
			success = PR_TRUE;
			movetogoal( dist );

			// Gizmo - check if we should still be navigating
			if ( GruntyAutoObstacle( self ) == 2 ) {
				// Gizmo - check if our path to our goal entity is still blocked
				vector	lastOrigin;
				float		tempYaw;

				lastOrigin = self->origin;
				tempYaw = vectoyaw( self->goalentity->origin - self->origin );

				if ( !walkmove( tempYaw, dist ) )
					self->motd = time + 1;
				else
					setorigin( self, lastOrigin );
			} else
				self->motd = 0;
		} else
			success = walkmove(angle, dist);

		if (success) // We walked sucessfully, woohoo
		{
			self->has_cheated = time + 0.5;
			return PR_TRUE;
		}
		else // We just ran into something...
		{
			if (self->is_toffingadet==1) //if we r on stop on obstacles mode
			{
				self->has_cheated = time + 0.5;
				if (self->attack_finished < time) stand_frames();

				// Gizmo - let owner know he's blocked
				if (self->enemy  == world&& self->super_time < time) {
					PrintFromSoldier(self, self->real_owner, "Hey! I'm trapped over here!\n", PR_PRINT_MEDIUM);
					self->super_time = time + 10;
				}
				return PR_FALSE;
			}

			// Gizmo - decide what we should do
			if ( !self->is_toffingadet )
				obstacleMode = 0;
			else
				obstacleMode = GruntyAutoObstacle( self );

			// Gizmo - let him accelerate in the air, he always instantly goes to 30 though if lower than that
			if ( !obstacleMode ) // Gizmo - only air forward if we've decided to jump
			if ( !( self->flags & PR_FL_ONGROUND ) && self->velocity[Z] < 40 && self->velocity[Z] > -10 ) {
				float addspeed;

				// v_forward is the desired direction
				makevectors( self->angles );
				addspeed = 30 - self->velocity * v_forward;
				if ( addspeed > 0 )
					self->velocity = self->velocity + addspeed * v_forward;
			}

			if (self->has_cheated > time)
				return PR_FALSE;
			self->has_teleporter = 0; // abort dodging
			makevectors(self->angles);
			// First, we'll see if jumping would be suitable
			if ( !obstacleMode ) { // Gizmo - only jump if we've decided to
				if (self->flags & PR_FL_ONGROUND) // don't bother if we aren't on the ground
				{
					traceline(self->origin + V({0, 0, 20}), self->origin + V({0, 0, 20}) + v_forward * 10, PR_TL_ANY_SOLID, self);
					if (trace_fraction == 1.0)
					{
						self->velocity = self->velocity + v_forward * self->custom_speed * 20;
						self->velocity[Z] = self->velocity[Z] + 270;
						//sound (self, #CHAN_BODY, "player/plyrjmp8.wav", 1, #ATTN_NORM);
						sound (self, PR_CHAN_VOICE, "player/plyrjmp8.wav", 1, PR_ATTN_NORM);
						self->has_cheated = time + 0.5;
						return PR_FALSE;
					}
				}
			} else if ( obstacleMode == 2 ) {
				movetogoal( dist );
				// Gizmo - disallow ChangeYaw() while we're wondering around
				self->motd = time + 1;
			}

			// Gizmo - if we're not on valid ground then fall
			if ( !checkbottom( self ) )
				self->flags = self->flags - ( self->flags & PR_FL_ONGROUND );
		}

		return PR_FALSE;
	/*}
	else
	{
		self.velocity = normalize(self.goalentity.origin - self.origin) * self.custom_speed * 20;
		if (self.teleport_time < time)
			CheckWaterJump();
	}*/

}

/* ---------------------------------------------------------------------------------- */
//						GRUNTY - PART SIX
//					Time to Die
//
//						Grunty sucks it down
/* ---------------------------------------------------------------------------------- */

void gruntyDead()
{
	//self.think = SUB_Remove;
	//self.nextthink = time + 40 + 40*random();
	MonsterCorpse();
}


void grunty_diea1()	 {FRAME_STATE(GruntyFrames::FR_DEATHA1, grunty_diea2);}
void grunty_diea2()	 {FRAME_STATE(GruntyFrames::FR_DEATHA2, grunty_diea3);}
void grunty_diea3()	 {FRAME_STATE(GruntyFrames::FR_DEATHA3, grunty_diea4);}
void grunty_diea4()	 {FRAME_STATE(GruntyFrames::FR_DEATHA4, grunty_diea5);}
void grunty_diea5()	 {FRAME_STATE(GruntyFrames::FR_DEATHA5, grunty_diea6);}
void grunty_diea6()	 {FRAME_STATE(GruntyFrames::FR_DEATHA6, grunty_diea7);}
void grunty_diea7()	 {FRAME_STATE(GruntyFrames::FR_DEATHA7, grunty_diea8);}
void grunty_diea8()	 {FRAME_STATE(GruntyFrames::FR_DEATHA8, grunty_diea9);}
void grunty_diea9()	 {FRAME_STATE(GruntyFrames::FR_DEATHA9, grunty_diea10);}
void grunty_diea10()	 {FRAME_STATE(GruntyFrames::FR_DEATHA10, grunty_diea11);}
void grunty_diea11()	 {FRAME_STATE(GruntyFrames::FR_DEATHA11, grunty_diea11); gruntyDead();}

void grunty_dieb1()	 {FRAME_STATE(GruntyFrames::FR_DEATHB1, grunty_dieb2);}
void grunty_dieb2()	 {FRAME_STATE(GruntyFrames::FR_DEATHB2, grunty_dieb3);}
void grunty_dieb3()	 {FRAME_STATE(GruntyFrames::FR_DEATHB3, grunty_dieb4);}
void grunty_dieb4()	 {FRAME_STATE(GruntyFrames::FR_DEATHB4, grunty_dieb5);}
void grunty_dieb5()	 {FRAME_STATE(GruntyFrames::FR_DEATHB5, grunty_dieb6);}
void grunty_dieb6()	 {FRAME_STATE(GruntyFrames::FR_DEATHB6, grunty_dieb7);}
void grunty_dieb7()	 {FRAME_STATE(GruntyFrames::FR_DEATHB7, grunty_dieb8);}
void grunty_dieb8()	 {FRAME_STATE(GruntyFrames::FR_DEATHB8, grunty_dieb9);}
void grunty_dieb9()	 {FRAME_STATE(GruntyFrames::FR_DEATHB9, grunty_dieb9); gruntyDead();}

void grunty_diec1()	 {FRAME_STATE(GruntyFrames::FR_DEATHC1, grunty_diec2);}
void grunty_diec2()	 {FRAME_STATE(GruntyFrames::FR_DEATHC2, grunty_diec3);}
void grunty_diec3()	 {FRAME_STATE(GruntyFrames::FR_DEATHC3, grunty_diec4);}
void grunty_diec4()	 {FRAME_STATE(GruntyFrames::FR_DEATHC4, grunty_diec5);}
void grunty_diec5()	 {FRAME_STATE(GruntyFrames::FR_DEATHC5, grunty_diec6);}
void grunty_diec6()	 {FRAME_STATE(GruntyFrames::FR_DEATHC6, grunty_diec7);}
void grunty_diec7()	 {FRAME_STATE(GruntyFrames::FR_DEATHC7, grunty_diec8);}
void grunty_diec8()	 {FRAME_STATE(GruntyFrames::FR_DEATHC8, grunty_diec9);}
void grunty_diec9()	 {FRAME_STATE(GruntyFrames::FR_DEATHC9, grunty_diec10);}
void grunty_diec10()	 {FRAME_STATE(GruntyFrames::FR_DEATHC10, grunty_diec11);}
void grunty_diec11()	 {FRAME_STATE(GruntyFrames::FR_DEATHC11, grunty_diec12);}
void grunty_diec12()	 {FRAME_STATE(GruntyFrames::FR_DEATHC12, grunty_diec13);}
void grunty_diec13()	 {FRAME_STATE(GruntyFrames::FR_DEATHC13, grunty_diec14);}
void grunty_diec14()	 {FRAME_STATE(GruntyFrames::FR_DEATHC14, grunty_diec15);}
void grunty_diec15()	 {FRAME_STATE(GruntyFrames::FR_DEATHC15, grunty_diec15); gruntyDead();}

void grunty_died1()	 {FRAME_STATE(GruntyFrames::FR_DEATHD1, grunty_died2);}
void grunty_died2()	 {FRAME_STATE(GruntyFrames::FR_DEATHD2, grunty_died3);}
void grunty_died3()	 {FRAME_STATE(GruntyFrames::FR_DEATHD3, grunty_died4);}
void grunty_died4()	 {FRAME_STATE(GruntyFrames::FR_DEATHD4, grunty_died5);}
void grunty_died5()	 {FRAME_STATE(GruntyFrames::FR_DEATHD5, grunty_died6);}
void grunty_died6()	 {FRAME_STATE(GruntyFrames::FR_DEATHD6, grunty_died7);}
void grunty_died7()	 {FRAME_STATE(GruntyFrames::FR_DEATHD7, grunty_died8);}
void grunty_died8()	 {FRAME_STATE(GruntyFrames::FR_DEATHD8, grunty_died9);}
void grunty_died9()	 {FRAME_STATE(GruntyFrames::FR_DEATHD9, grunty_died9); gruntyDead();}

void grunty_diee1()	 {FRAME_STATE(GruntyFrames::FR_DEATHE1, grunty_diee2);}
void grunty_diee2()	 {FRAME_STATE(GruntyFrames::FR_DEATHE2, grunty_diee3);}
void grunty_diee3()	 {FRAME_STATE(GruntyFrames::FR_DEATHE3, grunty_diee4);}
void grunty_diee4()	 {FRAME_STATE(GruntyFrames::FR_DEATHE4, grunty_diee5);}
void grunty_diee5()	 {FRAME_STATE(GruntyFrames::FR_DEATHE5, grunty_diee6);}
void grunty_diee6()	 {FRAME_STATE(GruntyFrames::FR_DEATHE6, grunty_diee7);}
void grunty_diee7()	 {FRAME_STATE(GruntyFrames::FR_DEATHE7, grunty_diee8);}
void grunty_diee8()	 {FRAME_STATE(GruntyFrames::FR_DEATHE8, grunty_diee9);}
void grunty_diee9()	 {FRAME_STATE(GruntyFrames::FR_DEATHE9, grunty_diee9); gruntyDead();}

void grunty_die_ax1()	 {FRAME_STATE(GruntyFrames::FR_AXDETH1, grunty_die_ax2);}
void grunty_die_ax2()	 {FRAME_STATE(GruntyFrames::FR_AXDETH2, grunty_die_ax3);}
void grunty_die_ax3()	 {FRAME_STATE(GruntyFrames::FR_AXDETH3, grunty_die_ax4);}
void grunty_die_ax4()	 {FRAME_STATE(GruntyFrames::FR_AXDETH4, grunty_die_ax5);}
void grunty_die_ax5()	 {FRAME_STATE(GruntyFrames::FR_AXDETH5, grunty_die_ax6);}
void grunty_die_ax6()	 {FRAME_STATE(GruntyFrames::FR_AXDETH6, grunty_die_ax7);}
void grunty_die_ax7()	 {FRAME_STATE(GruntyFrames::FR_AXDETH7, grunty_die_ax8);}
void grunty_die_ax8()	 {FRAME_STATE(GruntyFrames::FR_AXDETH8, grunty_die_ax9);}
void grunty_die_ax9()	 {FRAME_STATE(GruntyFrames::FR_AXDETH9, grunty_die_ax9); gruntyDead();}

// For now we'll just have this
// Later we'll have hard and soft deaths
void ArmyDeathSound();

void custom_grunt_die()
{
	self->effects=0;

	if (self->real_owner->classname == "player")
	{
		sprint(self->real_owner,PR_PRINT_HIGH,"Your soldier ");
		sprint(self->real_owner,PR_PRINT_HIGH,self->netname);
		sprint(self->real_owner,PR_PRINT_HIGH," is dead.\n");
		self->real_owner->job_finished = time + 2; //Can't summon streams of demons SB can so
		ResetSlotPointer(self->real_owner, self->increase_team1);

		if (self->martyr_enemy != world)
		RemoveWaypoint(self->martyr_enemy, self);

		if (self->demon_one != world)
		RemoveWaypoint(self->demon_one, self);

		if (self->demon_two != world)
		RemoveWaypoint(self->demon_two, self);

		entity oself;
		oself=self;
		self=self->real_owner;
		UpdateReserves(self);
		SetArmyTimer(self,PR_FALSE);
		self=oself;
	}

	if (self->health < -40)
	{
		if (random() < 0.5)
			sound (self, PR_CHAN_VOICE, "player/gib.wav", 1, PR_ATTN_MONSTERDIE);
		else
			sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_MONSTERDIE);

		ThrowMonsterHead ("progs/h_player.mdl", self->health);
		ThrowGib ("progs/gib1.mdl", self->health, PR_TRUE, 0, PR_GIB1_KGS, PR_FALSE);
		ThrowGib ("progs/gib2.mdl", self->health, PR_TRUE, 0, PR_GIB2_KGS, PR_FALSE);
		ThrowGib ("progs/gib3.mdl", self->health, PR_TRUE, 0, PR_GIB3_KGS, PR_FALSE);
		SpawnBloodEx(self->origin,0.66,9);
		//ThrowGib ("progs/gib3.mdl", self.health);
		dremove(self);
		return;
	}

	self->solid = PR_SOLID_NOT;

	ArmyDeathSound();
	//self.classname = "monster_corpse";
	MonsterDead();

	self->think=SUB_Null;
	self->touch=SUB_Null;

	if (isMelee())
	{
		grunty_die_ax1();
	}
	else
	{
		float r;
		r=random();
		if (r<0.2)
		  grunty_diea1();
		else if (r<0.4)
		  grunty_dieb1();
		else if (r<0.6)
		  grunty_diec1();
		else if (r<0.8)
		  grunty_died1();
		else grunty_diee1();
	}
}

void ArmyDeathSound()
{
	float 	rs;

	// water death sounds
	if (self->waterlevel == 3)
	{
		DeathBubbles(7); // was 10
		sound (self, PR_CHAN_VOICE, "player/h2odeath.wav", 1, PR_ATTN_MONSTERDIE);
		return;
	}

	rs = rint ((random() * 4) + 1);
	if (rs == 1)
		self->noise = "player/death1.wav";
	if (rs == 2)
		self->noise = "player/death2.wav";
	if (rs == 3)
		self->noise = "player/death3.wav";
	if (rs == 4)
		self->noise = "player/death4.wav";
	if (rs == 5)
		self->noise = "player/death5.wav";

	sound (self, PR_CHAN_VOICE, self->noise, 1, PR_ATTN_MONSTERDIE);
}

// We'll include pain here

void grunty_pain1()  {FRAME_STATE(GruntyFrames::FR_PAIN1, grunty_pain2); self->weaponframe=0;self->effects=0;}
void grunty_pain2()  {FRAME_STATE(GruntyFrames::FR_PAIN2, grunty_pain3);}
void grunty_pain3()  {FRAME_STATE(GruntyFrames::FR_PAIN3, grunty_pain4);}
void grunty_pain4()  {FRAME_STATE(GruntyFrames::FR_PAIN4, grunty_pain5);}
void grunty_pain5()  {FRAME_STATE(GruntyFrames::FR_PAIN5, grunty_pain6);}
void grunty_pain6()
{
	FRAME_STATE(GruntyFrames::FR_PAIN6, grunty_run);
	self->walkframe = 0;
}

void grunty_axepain1()	 {FRAME_STATE(GruntyFrames::FR_AXPAIN1, grunty_axepain2); self->weaponframe=0;self->effects=0;}
void grunty_axepain2()	 {FRAME_STATE(GruntyFrames::FR_AXPAIN2, grunty_axepain3);}
void grunty_axepain3()	 {FRAME_STATE(GruntyFrames::FR_AXPAIN3, grunty_axepain4);}
void grunty_axepain4()	 {FRAME_STATE(GruntyFrames::FR_AXPAIN4, grunty_axepain5);}
void grunty_axepain5()	 {FRAME_STATE(GruntyFrames::FR_AXPAIN5, grunty_axepain6);}
void grunty_axepain6()
{
	FRAME_STATE(GruntyFrames::FR_AXPAIN6, grunty_run);
	self->walkframe = 0; 									// - 1)/16);
}

void grunty_pain(entity attacker, float damage)
{
	//self.real_owner.StatusRefreshTime = time + 0.2;
	//self.real_owner.StatusBarScreen = 3;

#ifdef PR_VERBOSE_GRUNTY
//	  RPrint(self.real_owner, #PRINT_HIGH, "Grunty PAIN!\n");
#endif

	// Gizmo - if on seek mode and we don't have a target, then try to seek out whatever just hurt us
	if ( self->PR_grunty_tacticMode == PR_GRUNTY_TACTICMODE_SEEK )
	if ( self->enemy == world )
	if ( IsValidGruntyTarget( attacker, self ) )
		self->enemy = self->goalentity = attacker;

	if (self->pain_finished > time)
		return;

	// Gizmo - always play pain sound if time; this used to be in pain1 and axpain1 frames
	PainSound();
	// Gizmo - this used to be at the end of each pain animation
	self->pain_finished = time + random()*1.5*(self->has_sensor/17) + PR_GRUNTY_PAINTIME;

	//if (random()*40 > damage) // random()*40+40*((self.has_sensor-1)/16)
	// Gizmo - don't need this anymore
//	if (random()*50 + 60*((self.has_sensor - 1)/16) > damage)
//		return;

	// Gizmo - don't do pain frames if we're in mid battle or following something/someone
	if ( self->enemy != world || self->goalentity != world )
		return;

	if (isMelee())
		grunty_axepain1();
	else
		grunty_pain1();
}

/* ---------------------------------------------------------------------------------- */
//						GRUNTY - PART SEVEN
//					Moving with style
//
//						Grunty needs waypoints
/* ---------------------------------------------------------------------------------- */

/*void() Grunty_PhysUpdate =
{
	setorigin(self, self.origin + self.velocity * 0.05);
};*/

void Waypoint_Touch()
{
	// If the toucher isn't a grunt, return.
	if (other->classname != "monster_army")
		return;

	// If the toucher isn't trying to reach us, return.
	// We could do this with self.owner but it might accidentally touch a home waypoint
	// making this a bad option.
	if (other->goalentity != self)
		return;

	if (self->has_sensor == PR_WAYPOINT_TYPE_ENEMYLASTSEEN)
	{
		// Gizmo - if they're still not visible when we reach the waypoint, then forget about them
		// Gizmo - no need to remove the waypoint or set grunty's demon_one to world, because that will be done by the grunty
		other->search_time = 0;
/*
		if (self.real_owner.demon_one == self)
		   self.real_owner.demon_one = world;
		dremove(self);
*/
		return;
	}

	if (self->has_sensor == PR_WAYPOINT_TYPE_PRIMARY)
		self->real_owner->job=2;
	else if (self->has_sensor == PR_WAYPOINT_TYPE_SECONDARY)
		self->real_owner->job=1;

	self->all_active = time + self->has_holo; // reset entity life time
	//has_holo is the asigned life time in createwaypoint

	other->goalentity = self->goalentity; // Set grunty's movement target
}

void Waypoint_DoNothing() // A waypoint checks for redundancy
{
	self->nextthink = time + 5; // next check in 5 seconds

	//- OfN - remove waypoint entity?
	if (!self->owner->is_connected
		|| !(self->owner->job & PR_JOB_ARMY)
		|| !(HasMonster(self->owner))
		|| self->real_owner->classname != "monster_army"
		|| self->owner->classname != "player")
	{
	   dremove(self); // no pointer set to world needed cause soldier shouldnt be present on game
	   return;
	}

	if (self->all_active < time) // if it wasnt touched for specified life-time remove it
	{
		// Gizmo - make sure we don't remove the waypoint if the grunty only has one and is standing on it
		if ( self->real_owner->PR_grunty_mode == PR_GRUNTY_MODE_USEWAYPOINTS )
		if ( vlen( self->origin - self->real_owner->origin ) <= 96 ) {
			self->all_active = time + self->has_holo;	// reset entity life time
										// has_holo is the asigned life time in createwaypoint
			return;
		}

		// FIXME: Do .goalentity clean up on other waypoints (using removewaypoint() instead?)

		if (self->has_sensor == PR_WAYPOINT_TYPE_PRIMARY
			&& self->real_owner->martyr_enemy == self)
		{
			self->real_owner->martyr_enemy = world;
			PrintFromSoldier(self->real_owner,self->owner,"Hmmm.. I've forgotten my primary waypoint sir!\n",PR_PRINT_HIGH);
		}
		else if (self->has_sensor == PR_WAYPOINT_TYPE_SECONDARY
			&& self->real_owner->demon_two == self)
		{
			self->real_owner->demon_two = world;
			PrintFromSoldier(self->real_owner,self->owner,"Hmmm.. I've forgotten my secondary waypoint sir!\n",PR_PRINT_HIGH);
		}
		else if (self->has_sensor == PR_WAYPOINT_TYPE_ENEMYLASTSEEN
			&& self->real_owner->demon_one == self)
			self->real_owner->demon_one = world;

		dremove(self);
		return;
	}
}

/* CreateWaypoint
 *
 * Creates a waypoint for use by grunty
 * waypoint.goalentity = next target
 * waypoint.owner = owner grunt
 */

entity CreateWaypoint(const vector& location, float life, float type, entity sld)
{
	newmis = spawnServerSide(); // PZ: make it a server-side only entity   // Spawn a new entity (the waypoint)

 // set the origin.
	setsize (newmis, V({-20, -20, -20}), V({20, 20, 20})); // set the size of the waypoint

	newmis->solid = PR_SOLID_TRIGGER; // We're a solid trigger type - touch, but don't block

	//newmis.netname = "grunty_waypoint"; // set the name so grunty can talk about us
	newmis->classname = "grunty_waypoint";
	newmis->touch = Waypoint_Touch; // Set the waypoint's touch function
	newmis->think = Waypoint_DoNothing; // Set the waypoint's redundancy check function
	newmis->nextthink = time + 2; // Think in 2 seconds

	if (self->classname == "player")
	{
		newmis->owner = self;
		newmis->real_owner = sld;
	}
	else if (self->classname == "monster_army")
	{
		newmis->owner = self->real_owner;
		newmis->real_owner = sld;
	}

	newmis->all_active = time + life; //#WAYPOINT_LIFE;
	newmis->has_holo = life;
	newmis->has_sensor = type; // sets the type of waypoint to has_sensor

	setorigin(newmis, location);

	return newmis; // return the entity
}

/* ---------------------------------------------------------------------------------- */
//						GRUNTY - PART EIGHT
//					Life and Happiness
//
//						Grunty earns hard cash
/* ---------------------------------------------------------------------------------- */

void GetRank(entity targ)
{
	if (targ->has_sensor == 1)
		targ->undercover_name = "Private Class III ";
	else if (targ->has_sensor == 2)
		targ->undercover_name = "Private Class II ";
	else if (targ->has_sensor == 3)
		targ->undercover_name = "Private Class I ";
	else if (targ->has_sensor == 4)
		targ->undercover_name = "Lance Corporal ";
	else if (targ->has_sensor == 5)
		targ->undercover_name = "Corporal ";
	else if (targ->has_sensor == 6)
		targ->undercover_name = "Sergeant ";
	else if (targ->has_sensor == 7)
		targ->undercover_name = "Staff Sergeant ";
	else if (targ->has_sensor == 8)
		targ->undercover_name = "Warrant Officer II ";
	else if (targ->has_sensor == 9)
		targ->undercover_name = "Warrant Officer I ";
	else if (targ->has_sensor == 10)
		targ->undercover_name = "Lieutenant Class II ";
	else if (targ->has_sensor == 11)
		targ->undercover_name = "Lieutenant Class I ";
	else if (targ->has_sensor == 12)
		targ->undercover_name = "Captain ";
	else if (targ->has_sensor == 13)
		targ->undercover_name = "Major ";
	else if (targ->has_sensor == 14)
		targ->undercover_name = "Colonel ";
	else if (targ->has_sensor == 15)
		targ->undercover_name = "Brigadier ";
	else if (targ->has_sensor == 16)
		targ->undercover_name = "General ";
	else if (targ->has_sensor == 17)
		targ->undercover_name = "Field Marshal ";
	else
		targ->undercover_name = "Error producer ";
}

void Grunty_Check_Frags()
{
	if (self->frags >= self->has_sentry && self->has_sensor < 17)
	{
		self->has_tesla = self->has_tesla + 0.1;
		self->has_sensor = self->has_sensor + 1;
		self->has_sentry = self->has_sentry + 2;
		//self.health = self.health + self.has_sensor * 25; //-this before
		self->health = self->health + 20 + self->has_sensor * 12; //-this now
		self->max_health = self->max_health + 20 + self->has_sensor * 16;//-was 25

		//ofn
		if (self->max_health > PR_GRUNT_MAX_HP)
			self->max_health = PR_GRUNT_MAX_HP;

		if (self->health > self->max_health)
			self->health = self->max_health;

		GetRank(self);

		sprint(self->real_owner, PR_PRINT_HIGH, "Your soldier, ");
		sprint(self->real_owner, PR_PRINT_HIGH, self->netname);
		sprint(self->real_owner, PR_PRINT_HIGH, ", has reached the rank of ");
		sprint(self->real_owner, PR_PRINT_HIGH, self->undercover_name);
		sprint(self->real_owner, PR_PRINT_HIGH, "\n");

		if (extras_mode == 2)
			return;

		if (self->has_sensor == 17)
		{
			float extras;
			extras = GetJobExtras(self->real_owner);

			if (extras < 2)
				sprint(self->real_owner, PR_PRINT_HIGH, "The generals are impressed by your tactical skills..\n");
			else
				return;

			if (!extras)
			{
				sprint(self->real_owner, PR_PRINT_HIGH,"You are rewarded with an extra soldier reserve!\n");
				SetJobExtras(self->real_owner,1);
				UpdateReserves(self->real_owner);
			}
			else if (extras == 1)
			{
				sprint(self->real_owner, PR_PRINT_HIGH,"You now have access to 3 soldier reserves!\n");
				SetJobExtras(self->real_owner,2);
				UpdateReserves(self->real_owner);
			}
		}
	}
}

float ReturnWeaponVelocity()
{
	if (self->current_weapon == PR_WEAP_NAILGUN)
//		return 1250;
		return 99999999;
	else if (self->current_weapon == PR_WEAP_ROCKET_LAUNCHER)
		//return 1100;
		return 99999999;
	else
		return 99999999;
}

#define PR_GRUNTY_COST_HEALTHPACK		35 // was 20
#define PR_GRUNTY_COST_SHOTGUN 		100 // was 50
#define PR_GRUNTY_COST_NAILGUN 		300 // was 300
#define PR_GRUNTY_COST_SUPER_SHOTGUN	500 // was 500
#define PR_GRUNTY_COST_KNIFE			500 // was 700 // then 400
#define PR_GRUNTY_COST_ROCKET_LAUNCHER 3000 // was 3000 // then 2000
#define PR_GRUNTY_COST_SCANNER 		1200 // was 1000 // then 800
#define PR_GRUNTY_COST_SHELL			4 // was 2
#define PR_GRUNTY_COST_NAIL			2 // was 1
#define PR_GRUNTY_COST_ROCKET			12 // was 3
//#define GRUNTY_COST_CELL			2
#define PR_GRUNTY_COST_MAXHEALTH		120 // was 100
#define PR_GRUNTY_COST_SPEED			800 // was 400 // then 400
#define PR_GRUNTY_MAX_SPEED			13 //- was 30

void GruntyPayThink()
{
	float chance;
	float r;
	float spendalloc;
	// If it's salary time, draw pay
	if (self->respawn_time < time)
	{
		GruntyDrawPay();
		self->respawn_time = time + 20;
	}
	// Grunty is on a SHOPPING SPREE!
	if (!(self->weapons_carried & PR_WEAP_ROCKET_LAUNCHER))
		spendalloc = self->money * 0.2; // amount to spend on health/speed upgrades
	else
		spendalloc = self->money;
	// Consider buying more max health		)//- added
	if (spendalloc >= PR_GRUNTY_COST_MAXHEALTH && self->max_health < PR_GRUNT_MAX_HP)
	{
		if (self->weapons_carried & PR_WEAP_ROCKET_LAUNCHER)
			chance = 0.75; //- was 0.9
		else
			chance = 0.25;

		r = random();
		if (r < chance)
		{
			self->max_health = self->max_health + 25;
			self->money = self->money - PR_GRUNTY_COST_MAXHEALTH;
			spendalloc = spendalloc - PR_GRUNTY_COST_MAXHEALTH;
			//		#ifdef VERBOSE_GRUNTY
			PrintFromSoldier(self,self->real_owner,"my health maximum has been increased!\n",PR_PRINT_MEDIUM);
//			sprint(self.real_owner, #PRINT_HIGH, "BUYS more MAX HP\n");
			//	  #endif
		}
	}
	// Consider buying a health pack
	if (self->health < self->max_health && self->money >= PR_GRUNTY_COST_HEALTHPACK)
	{
		float loops, bought;

		loops = 0;
		bought = 0;

		while (loops < (0.75+(self->has_sensor/4)) && self->money >= PR_GRUNTY_COST_HEALTHPACK && self->max_health > self->health)
		{
			if (self->health < self->max_health - 25)
				chance = 1;
			else if (self->health < self->max_health - 15)
				chance = 0.25;
			else
				chance = 0.1;

			r = random();
			if (r < chance)
			{
				self->health = self->health + 25;
				if (self->health > self->max_health)
					self->health = self->max_health;
				self->money = self->money - PR_GRUNTY_COST_HEALTHPACK;
			  //					#ifdef VERBOSE_GRUNTY

//				  sprint(self.real_owner, #PRINT_HIGH, "BUYS A HEALTHPACK\n");
				//PrintFromSoldier(self,self.real_owner,"i've bought a healthpack.\n");
				bought=bought+1;
				//					#endif
			}
			loops = loops + 1;
		}

		if (bought>0)
		{
			if (bought==1)
				PrintFromSoldier(self,self->real_owner,"i've bought a healthpack.\n",PR_PRINT_MEDIUM);
			else
			{
				string tempst, st2;
				st2=ftos(bought);
				tempst = strcat("i've bought ",st2);
				tempst = strcat(tempst," healthpacks.\n");

				PrintFromSoldier(self,self->real_owner,tempst,PR_PRINT_MEDIUM);
				//sprint(self.real_owner,#PRINT_HIGH,tempst);
				//sprint(self.real_owner,#PRINT_HIGH," healthpacks.\n");

			}

			sound(self,PR_CHAN_MISC,"items/r_item1.wav",1,PR_ATTN_IDLE);
			SpawnBlood (self->origin, 20);
		}

	}

	// Consider buying a shotgun
	if (!(self->weapons_carried & PR_WEAP_SHOTGUN))
		if (self->money >= PR_GRUNTY_COST_SHOTGUN)
		{
			r = random();
			if (r < 0.5)
			{
				self->weapons_carried = self->weapons_carried | PR_WEAP_SHOTGUN;
				self->money = self->money - PR_GRUNTY_COST_SHOTGUN;
				PrintFromSoldier(self,self->real_owner,"i have bought a shotgun.\n",PR_PRINT_MEDIUM);
				//sprint(self.real_owner, #PRINT_HIGH, "Your soldier has just purchased a shotgun.\n");
			}
		}
	// Consider buying a knife
	if (!(self->cutf_items & PR_CUTF_KNIFE))
		if (self->money >= PR_GRUNTY_COST_KNIFE)
		{
			r = random();
			if (r < 0.5)
			{
				self->cutf_items = self->cutf_items | PR_CUTF_KNIFE;
				self->money = self->money - PR_GRUNTY_COST_KNIFE;
				PrintFromSoldier(self,self->real_owner,"i have bought a knife.\n",PR_PRINT_MEDIUM);
				//sprint(self.real_owner, #PRINT_HIGH, "Your soldier has just purchased a knife.\n");
			}
		}
	// Consider buying a nailgun
	if (!(self->weapons_carried & PR_WEAP_NAILGUN))
		if (self->money >= PR_GRUNTY_COST_NAILGUN)
		{
			r = random();
			if (r < 0.25)
			{
				self->weapons_carried = self->weapons_carried | PR_WEAP_NAILGUN;
				self->money = self->money - PR_GRUNTY_COST_NAILGUN;
				PrintFromSoldier(self,self->real_owner,"i now own a nailgun.\n",PR_PRINT_MEDIUM);
				//sprint(self.real_owner, #PRINT_HIGH, "Your soldier now owns a nailgun.\n");
			}
		}
	// Consider buying a double barrel
	if (!(self->weapons_carried & PR_WEAP_SUPER_SHOTGUN))
		if (self->money >= PR_GRUNTY_COST_SUPER_SHOTGUN)
		{
			r = random();
			if (r < 0.25)
			{
				self->weapons_carried = self->weapons_carried | PR_WEAP_SUPER_SHOTGUN;
				self->money = self->money - PR_GRUNTY_COST_SUPER_SHOTGUN;
				PrintFromSoldier(self,self->real_owner,"i've just bought a super shotgun.\n",PR_PRINT_MEDIUM);
				//sprint(self.real_owner, #PRINT_HIGH, "Your soldier just bought a super shotgun.\n");

			}
		}
	// Consider buying a scanner
	if (!(self->tf_items & PR_NIT_SCANNER))
		if (self->money >= PR_GRUNTY_COST_SCANNER)
		{
			r = random();
			if (r < 0.15)
			{
				self->tf_items = self->tf_items | PR_NIT_SCANNER;
				self->money = self->money - PR_GRUNTY_COST_SCANNER;
				PrintFromSoldier(self,self->real_owner,"i now own a scanner.\n",PR_PRINT_MEDIUM);
				//sprint(self.real_owner, #PRINT_HIGH, "Your soldier now owns a scanner.\n");
			}
		}
	// Consider buying...a rocket launcher.
	if (!(self->weapons_carried & PR_WEAP_ROCKET_LAUNCHER))
		if (self->money >= PR_GRUNTY_COST_ROCKET_LAUNCHER)
		{
			r = random();
			if (r < 0.3) //- was 0.2
			{
				self->weapons_carried = self->weapons_carried | PR_WEAP_ROCKET_LAUNCHER;
				self->money = self->money - PR_GRUNTY_COST_ROCKET_LAUNCHER;
				PrintFromSoldier(self,self->real_owner,"i have bought a ROCKET LAUNCHER!! hehe\n",PR_PRINT_MEDIUM);
				//sprint(self.real_owner, #PRINT_HIGH, "Your soldier is now the proud owner of a brand new rocket launcher.\n");
			}
		}

	// Buy ammo
	if ((self->weapons_carried & PR_WEAP_ROCKET_LAUNCHER) && self->ammo_rockets < self->maxammo_rockets)
	{
		float rocketstobuy;

		rocketstobuy = self->maxammo_rockets - self->ammo_rockets;
		if (rocketstobuy > 4)
			rocketstobuy = 4;
		if (self->money >= rocketstobuy * PR_GRUNTY_COST_ROCKET)
		{
			self->ammo_rockets = self->ammo_rockets + rocketstobuy;
			self->money = self->money - rocketstobuy * PR_GRUNTY_COST_ROCKET;

			//	  #ifdef VERBOSE_GRUNTY
			PrintFromSoldier(self,self->real_owner,"i've bought some rockets.\n",PR_PRINT_LOW);
			//sprint(self.real_owner, #PRINT_HIGH, "BUYS SOME ROCKETS\n");
			//	  #endif
		}
	}

	if ((self->weapons_carried & PR_WEAP_NAILGUN) && self->ammo_nails < self->maxammo_nails)
	{
		float nailstobuy;

		nailstobuy = self->maxammo_nails - self->ammo_nails;
		if (nailstobuy > 50) // was 50
			nailstobuy = 50;
		if (self->money >= nailstobuy * PR_GRUNTY_COST_NAIL)
		{
			self->ammo_nails = self->ammo_nails + nailstobuy;
			self->money = self->money - nailstobuy * PR_GRUNTY_COST_NAIL;
			//		#ifdef VERBOSE_GRUNTY
			PrintFromSoldier(self,self->real_owner,"i've bought some nails.\n",PR_PRINT_LOW);
			//sprint(self.real_owner, #PRINT_HIGH, "BUYS SOME NAILS\n");
			//	  #endif

		}
	}

	if ((self->weapons_carried & PR_WEAP_SHOTGUN ||
		self->weapons_carried & PR_WEAP_SUPER_SHOTGUN)
		&& self->ammo_shells < self->maxammo_shells)
	{
		float shellstobuy;

		shellstobuy = self->maxammo_shells - self->ammo_shells;
		if (shellstobuy > 15)
			shellstobuy = 15;
		if (self->money >= shellstobuy * PR_GRUNTY_COST_SHELL)
		{
			self->ammo_shells = self->ammo_shells + shellstobuy;
			self->money = self->money - shellstobuy * PR_GRUNTY_COST_SHELL;
			//		#ifdef VERBOSE_GRUNTY
			PrintFromSoldier(self,self->real_owner,"i've bought some shells.\n",PR_PRINT_LOW);
			//sprint(self.real_owner, #PRINT_HIGH, "BUYS SOME SHELLS\n");
			//	  #endif
		}
	}

	// Consider buying more speed
	if (spendalloc >= PR_GRUNTY_COST_SPEED)
	{
		if (self->custom_speed < PR_GRUNTY_MAX_SPEED)
		{
			r = random();
			if (r < 0.3)
			{
				self->custom_speed = self->custom_speed + 1;
				self->money = self->money - PR_GRUNTY_COST_SPEED;
				spendalloc = spendalloc - PR_GRUNTY_COST_SPEED;

				PrintFromSoldier(self,self->real_owner,"i'm faster now.\n",PR_PRINT_MEDIUM);
				//sprint(self.real_owner, #PRINT_HIGH, "Your soldier is now faster.\n");
			}
		}
	}

	//- OfteN: don't let grunt have more HP than #GRUNT_MAX_HP -//
	if (self->health > PR_GRUNT_MAX_HP)
		self->health = PR_GRUNT_MAX_HP;

	if (self->max_health > PR_GRUNT_MAX_HP)
		self->max_health = PR_GRUNT_MAX_HP;

	self->suicide_time = time + 10;
}

void GruntyDrawPay()
{
	self->money = self->money + 70 + floor((self->has_sensor/2)+1) * 25;//-was 30 and has_sensor wasnt/2 // then was 30 again
	self->respawn_time = time + 20;
	return;
}

//-----------------------------------------------------//
void grunty_touch()
{
	if (self->health <= 0) // If we r dead, go away
		return;

	entity Bio;
	string tmps;

	if (self->tfstate & PR_TFSTATE_INFECTED)
	{
		if (other->classname == "player")
		{
			if (!(other->invincible_finished > time)) // Dont infect if we r invincible
			if (!(other->weapons_carried & PR_WEAP_MEDIKIT)) // neither if we got medikit
			if (!(other->tfstate & PR_TFSTATE_INFECTED)) // Dont if we r already infected
			{
				// infect 'em
				Bio = spawnServerSide(); // PZ: make it a server-side only entity
				Bio->nextthink = time + 2;
				Bio->think = BioInfection_Decay;
				Bio->enemy = self; //Person causing infection
				Bio->owner = other; //Person getting hurt
				Bio->classname = "timer";
				Bio->netname = "biotimer";

				other->tfstate = other->tfstate | PR_TFSTATE_INFECTED;
				other->infection_team_no = self->infection_team_no;

				tmps = GetMonsterName(self);

				sprint(other, PR_PRINT_MEDIUM, "You have been infected by the ");
				sprint(other, PR_PRINT_MEDIUM, tmps);
				sprint(other, PR_PRINT_MEDIUM, " ");
				sprint(other, PR_PRINT_MEDIUM, self->netname);
				sprint(other, PR_PRINT_MEDIUM, "!\n");

				teamprefixsprint(other->team_no,other);
				teamsprint(other->team_no,other, other->netname);
				teamsprint(other->team_no,other, " has been infected!\n");
			}
		}
		else if (IsOwnedMonster(other))
		{
			if (!(other->tfstate & PR_TFSTATE_INFECTED))
			{
				// Report to owner
				tmps = GetMonsterName(other);
				sprint(other->real_owner,PR_PRINT_HIGH,"Your ",tmps," ",other->netname," has been infected!\n");

				// Report to teammates of owner
				string tmps;
				tmps = GetMonsterName(other);
				teamprefixsprint(other->real_owner->team_no,other->real_owner);
				teamsprint6(other->real_owner,"The ",tmps," ", other->netname," has been infected!\n","");

				// Update infection flag
				other->tfstate = other->tfstate | PR_TFSTATE_INFECTED;

				// Infect it!
				Bio = spawnServerSide(); // PZ: make it a server-side only entity
				Bio->classname = "timer";
				Bio->classname = "biotimer";
				Bio->nextthink = time + 2;
				Bio->think = BioInfection_MonsterDecay;
				Bio->owner = self;
				Bio->enemy = other;

				other->infection_team_no = self->infection_team_no;
			}
		}

#ifdef PR_COOP_MODE_ENHANCED
		else if ( COOP_IsCoopMonster( other ) ) {
			if ( !( other->tfstate & PR_TFSTATE_INFECTED ) ) {
				// Update infection flag
				other->tfstate = other->tfstate | PR_TFSTATE_INFECTED;

				// Infect it!
				Bio = spawnServerSide(); // PZ: make it a server-side only entity
				Bio->classname = "timer";
				Bio->classname = "biotimer";
				Bio->nextthink = time + 2;
				Bio->think = BioInfection_MonsterDecay;
				Bio->owner = self;
				Bio->enemy = other;

				other->infection_team_no = self->infection_team_no;
			}
		}
#endif

		return; // Dont display funny messages if we r going to infect half of our own team =)
	}

	if (other->classname=="player" && self->has_holo < time && self->enemy == world)
	{
		string st;

		if (self->real_owner==other)
		{
			st=GetOwnerMessage(self);
			PrintFromSoldier(self,other,st,PR_PRINT_CHAT);

			self->has_holo=time+5;
		}
		else if (Teammate(self->real_owner, other))
		{
			float rnum;
			rnum=random();

			if (rnum < 0.4)
			{
				PrintFromSoldier(self,other,"Hi ",PR_PRINT_CHAT);
				sprint(other,PR_PRINT_HIGH,other->netname);
				sprint(other,PR_PRINT_HIGH,", I'm ");
				sprint(other,PR_PRINT_HIGH,self->netname);
				sprint(other,PR_PRINT_HIGH,", ");
				sprint(other,PR_PRINT_HIGH,self->real_owner->netname);
				sprint(other,PR_PRINT_HIGH,"'s soldier!\n");

			}
			else
			{
				st=GetFriendlyMessage(self);
				PrintFromSoldier(self,other,st,PR_PRINT_CHAT);
			}

			self->has_holo=time+5;
		}
	}
}

//========================================================================
// lets make the server to not crash when removing waypoints ok? :)

void RemoveWaypoint(entity wyp, entity soldier)
{
	if (wyp==world) return;

	if (wyp->classname == "grunty_waypoint" && wyp->owner == soldier->real_owner)
	{
		if (soldier->martyr_enemy != world)
		{
			if (soldier->martyr_enemy->goalentity == wyp)
				soldier->martyr_enemy->goalentity = world;
		}

		if (soldier->demon_two != world)
		{
			if (soldier->demon_two->goalentity == wyp)
				soldier->demon_two->goalentity = world;
		}

		if (wyp->has_sensor == PR_WAYPOINT_TYPE_PRIMARY
		 && soldier->martyr_enemy == wyp)
			soldier->martyr_enemy = world;
		else if (wyp->has_sensor == PR_WAYPOINT_TYPE_SECONDARY
		 && soldier->demon_two == wyp)
			soldier->demon_two = world;
		else if (wyp->has_sensor == PR_WAYPOINT_TYPE_ENEMYLASTSEEN
		 && soldier->demon_one == wyp)
			soldier->demon_one = world;

		dremove(wyp);
	}
}

//=======================================================================//
// OfN returns the goalentity the grunt should be going to if its able

entity ReturnEasyWaypoint(entity sold, entity viewpoint)
{
	if (sold->penance_time==2) return world;
	if (sold->penance_time==0) return world;

	if (sold->penance_time==1)
	{
		if (sold->real_owner->health <= 0)
		{
			sold->penance_time=0;
			return world;
		}

		if (visible2(sold,sold->real_owner))
			return sold->real_owner;
		else
		{
			PrintFromSoldier(sold,sold->real_owner,"i was following you but i can't see you now!\n",PR_PRINT_HIGH);
			return world;
		}
	}

	if (sold->martyr_enemy!=world && sold->job==1 && visible2(viewpoint,sold->martyr_enemy))
		return sold->martyr_enemy;

	if (sold->demon_two!=world && sold->job==2 && visible2(viewpoint,sold->demon_two))
		return sold->demon_two;

	// no last waypoint walk so lets see whats easier to reach...

	entity retENT;
	retENT=world;

	if (sold->demon_two!=world && sold->martyr_enemy!=world) // both waypoints assigned? ok...
	{
		if (visible2(viewpoint,sold->martyr_enemy) && visible2(viewpoint,sold->demon_two)) // both waypoints visible? lets see which one is closest...
		{
			float dist1,dist2;
			dist1 = vlen(viewpoint->origin - sold->martyr_enemy->origin);
			dist2 = vlen(viewpoint->origin - sold->demon_two->origin);

			if (dist1 > dist2) // which is the closest one?
				retENT=sold->demon_two;
			else
				retENT=sold->martyr_enemy;
		}
		else if (visible2(viewpoint,sold->martyr_enemy)) // only primary wayp is visible?
			retENT=sold->martyr_enemy;
		else if (visible2(viewpoint,sold->demon_two)) // only secondary wayp is visible?
			retENT=sold->demon_two;
	}
	else if (sold->demon_two!=world) // only secondary wayp is assigned?
	{
		if (visible2(viewpoint,sold->demon_two))
			retENT=sold->demon_two;
	}
	else if (sold->martyr_enemy!=world) // only primary wayp is assigned?
	{
		if (visible2(viewpoint,sold->martyr_enemy))
			retENT=sold->martyr_enemy;
	}

	if (sold->penance_time == 3 && retENT==world)
		PrintFromSoldier(sold,sold->real_owner,"can't see my waypoints around here!\n",PR_PRINT_HIGH);

	return retENT;
}

//== OfN, used when picking up ammo ==//
void grunty_boundammo(entity sld)
{
	if (sld->classname!="monster_army") return;

	if (sld->ammo_nails > sld->maxammo_nails) sld->ammo_nails = sld->maxammo_nails;
	if (sld->ammo_cells > sld->maxammo_cells) sld->ammo_cells = sld->maxammo_cells;
	if (sld->ammo_shells > sld->maxammo_shells) sld->ammo_shells = sld->maxammo_shells;
	if (sld->ammo_rockets > sld->maxammo_rockets) sld->ammo_rockets = sld->maxammo_rockets;
}

//=====================================================================

/*
==============
Grunty_GetTacticModeString
==============
*/
string Grunty_GetTacticModeString( entity g) {
	if ( g->PR_grunty_tacticMode == PR_GRUNTY_TACTICMODE_NORMAL )
		return "normal";
	if ( g->PR_grunty_tacticMode == PR_GRUNTY_TACTICMODE_SEEK )
		return "seek";
	return "static";
}

/*
==============
Grunty_GetEngageEnemyString
==============
*/
string Grunty_GetEngageEnemyString( entity g) {
	if ( g->PR_grunty_engageEnemy == PR_GRUNTY_ENGAGEENEMY_NO )
		return "no";
	return "yes";
}

/*
==============
Grunty_GetOnObstacleString
==============
*/
string Grunty_GetOnObstacleString( entity g) {
	if ( g->PR_grunty_onObstacle == PR_GRUNTY_ONOBSTACLE_JUMP )
		return "jump";
	if ( g->PR_grunty_onObstacle == PR_GRUNTY_ONOBSTACLE_STOP )
		return "stop";
	return "auto";
}

/*
==============
Grunty_GetAttackRangeString

Returns an empty string if attack range doesn't apply with
the current tactic mode.
==============
*/
string Grunty_GetAttackRangeString( entity g) {
	if ( g->PR_grunty_tacticMode == PR_GRUNTY_TACTICMODE_STATIC )
		return "";

	if ( g->PR_grunty_attackRange == PR_GRUNTY_ATTACKRANGE_ANY )
		return "any";
	if ( g->PR_grunty_attackRange == PR_GRUNTY_ATTACKRANGE_SMALL )
		return "small";
	if ( g->PR_grunty_attackRange == PR_GRUNTY_ATTACKRANGE_MEDIUM )
		return "medium";
	return "large";
}

/*
==============
Grunty_GetActionString

printDest can be world.
==============
*/
string Grunty_GetActionString( entity printDest, entity g) {
	if ( g->PR_grunty_mode == PR_GRUNTY_MODE_HOLDPOSITION )
		return "holding position";
	if ( g->PR_grunty_mode == PR_GRUNTY_MODE_FOLLOWOWNER ) {
		if ( printDest == g->real_owner )
			return "following you";
		return "following it's owner";
	}
	return "following waypoints";
}

/*
==============
Grunty_PrintDetails

Gizmo - This used to be Grunty_StateInv(), I changed
it up some because I needed a more generic version of it.
==============
*/
void Grunty_PrintDetails( entity printDest, entity g, float printLevel) {
	string	talk;

	talk = ftos(g->health);
	if ( printDest == g->real_owner )
		sprint(printDest, PR_PRINT_HIGH, S_("Your ^bsoldier^b "), g->netname, " has ");
	else
		sprint(printDest, PR_PRINT_HIGH, S_("The ^bsoldier^b "), g->netname, " has ");
	sprint(printDest, PR_PRINT_HIGH, talk);
	talk = ftos(g->max_health);
	sprint(printDest, PR_PRINT_HIGH, S_("^b/^b"));
	sprint(printDest, PR_PRINT_HIGH, talk);
	sprint(printDest, PR_PRINT_HIGH, S_(" ^bhealth^b"));

	// if we don't care about the inventory
	if ( !printLevel ) {
		if ( g->enemy != world ) {
			talk = GetEnemyName( g->enemy );
			sprint( printDest, PR_PRINT_HIGH, S_(", and is ^battacking^b "), talk, "\n" );
		} else {
			talk = Grunty_GetActionString( printDest, g );
			sprint( printDest, PR_PRINT_HIGH, ", and is ", talk, "\n" );
		}
		return;
	}

	if (g->weapons_carried & PR_WEAP_AXE)
	{
		if (g->cutf_items & PR_CUTF_KNIFE)
			sprint(printDest, PR_PRINT_HIGH, ", a combat knife");
		else
			sprint(printDest, PR_PRINT_HIGH, ", an axe");
	}
	if (g->weapons_carried & PR_WEAP_SHOTGUN)
		sprint(printDest, PR_PRINT_HIGH, ", a shotgun");
	if (g->weapons_carried & PR_WEAP_NAILGUN)
		sprint(printDest, PR_PRINT_HIGH, ", a nailgun");
	if (g->weapons_carried & PR_WEAP_SUPER_SHOTGUN)
		sprint(printDest, PR_PRINT_HIGH, ", a double barreled shotgun");
	if (g->weapons_carried & PR_WEAP_ROCKET_LAUNCHER)
		sprint(printDest, PR_PRINT_HIGH, ", a rocket launcher");
	if (g->tf_items & PR_NIT_SCANNER)
		sprint(printDest, PR_PRINT_HIGH, ", a scanner");
	talk = ftos(g->custom_speed * 20);
	sprint(printDest, PR_PRINT_HIGH, ", moves at speed ");
	sprint(printDest, PR_PRINT_HIGH, talk);
	talk = ftos(g->money);
	sprint(printDest, PR_PRINT_HIGH, " with $");
	sprint(printDest, PR_PRINT_HIGH, talk);
	sprint(printDest, PR_PRINT_HIGH, ", and is ");

	if ( g->enemy != world ) {
		talk = GetEnemyName( g->enemy );
		sprint( printDest, PR_PRINT_HIGH, S_("^battacking^b "), talk, "\n" );
	} else {
		talk = Grunty_GetActionString( printDest, g );
		sprint( printDest, PR_PRINT_HIGH, talk, "\n" );
	}

	sprint(printDest, PR_PRINT_HIGH, S_("^bammo^b: "));

	talk = ftos(g->ammo_shells);
	sprint(printDest, PR_PRINT_HIGH, talk);
	sprint(printDest, PR_PRINT_HIGH, " shells, ");

		talk = ftos(g->ammo_nails);
	sprint(printDest, PR_PRINT_HIGH, talk);
	sprint(printDest, PR_PRINT_HIGH, " nails and ");

	/*	  talk = ftos(g.ammo_cells);
	sprint(printDest, #PRINT_HIGH, talk);
	sprint(printDest, #PRINT_HIGH, " cells\n");*/

	talk = ftos(g->ammo_rockets);
	sprint(printDest, PR_PRINT_HIGH, talk);
	sprint(printDest, PR_PRINT_HIGH, " rockets\n");

	if ( printLevel != 2 )
		return;

	// tactical stuff now
	talk = Grunty_GetTacticModeString( g );
	sprint( printDest, PR_PRINT_HIGH, S_("^bTactic mode^b:"), talk );

	talk = Grunty_GetEngageEnemyString( g );
	sprint( printDest, PR_PRINT_HIGH, S_(" ^bEngage enemy^b:"), talk );

	talk = Grunty_GetOnObstacleString( g );
	sprint( printDest, PR_PRINT_HIGH, S_(" ^bOn obstacle^b:"), talk );

	talk = Grunty_GetAttackRangeString( g );
	if ( talk != "" )
		sprint( printDest, PR_PRINT_HIGH, S_(" ^bAttack range^b:"), talk );
	sprint( printDest, PR_PRINT_HIGH, "\n" );

/*
	// allowed weapons
	sprint( printDest, #PRINT_HIGH, " Allowed weapons: " );
	if ( !g.#allowed_weapons )
		sprint( printDest, #PRINT_HIGH, " None\n" );
	else {
		if ( g.#allowed_weapons & #WEAP_AXE )
			sprint( printDest, #PRINT_HIGH, " Axe" );
		if ( g.#allowed_weapons & #WEAP_SHOTGUN )
			sprint( printDest, #PRINT_HIGH, " Shotgun" );
		if ( g.#allowed_weapons & #WEAP_SUPER_SHOTGUN )
			sprint( printDest, #PRINT_HIGH, " Super Shotgun" );
		if ( g.#allowed_weapons & #WEAP_NAILGUN )
			sprint( printDest, #PRINT_HIGH, " Nailgun" );
		if ( g.#allowed_weapons & #WEAP_ROCKET_LAUNCHER )
			sprint( printDest, #PRINT_HIGH, " Rocket Launcher" );
	}
	sprint( printDest, #PRINT_HIGH, "\n" );
*/
}

} // END namespace Progs
