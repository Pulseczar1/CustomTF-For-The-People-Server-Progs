/*
==============================================================================

gremlin (from hipnotic mission pack)

Modified by OfteN [cp] on 04/07/2004

==============================================================================

Usage of entity fields:
-----------------------

For the gremlin entity:

ORIGINAL FIELD	 CURRENT FIELD

.stoleweapon -> .has_fieldgen
.gorging	 -> .has_camera TOFIX: Used on targets for gorging (conflict with camera flag)
.lastvictim  -> .martyr_enemy

==============================================================================
*/

#include "progs.h"
#include "ofndefs.h"
#include "gremlin.h"
#include "debug.h"
#include "weapons.h"
#include "ai.h"
#include "monsters.h"
#include "fight.h"
#include "warlock.h"
#include "subs.h"
#include "cpstuff.h"
#include "items.h"
#include "player.h"

namespace Progs {

//$cd hipwork\models\grem
//$origin 0 0 23
//$scale 2
//$base grembase.asc
//$skin grem

namespace CuTF_GremlinFrames
{
	enum {FR_STAND1, FR_STAND2, FR_STAND3, FR_STAND4, FR_STAND5, FR_STAND6};
	enum {FR_STAND7 = 6, FR_STAND8, FR_STAND9, FR_STAND10, FR_STAND11, FR_STAND12};
	enum {FR_STAND13 = 12, FR_STAND14, FR_STAND15, FR_STAND16, FR_STAND17};

	enum {FR_WALK1 = 17, FR_WALK2, FR_WALK3, FR_WALK4, FR_WALK5, FR_WALK6};
	enum {FR_WALK7 = 23, FR_WALK8, FR_WALK9, FR_WALK10, FR_WALK11, FR_WALK12};

	enum {FR_RUN1 = 29, FR_RUN2, FR_RUN3, FR_RUN4, FR_RUN5, FR_RUN6, FR_RUN7, FR_RUN8};
	enum {FR_RUN9 = 37, FR_RUN10, FR_RUN11, FR_RUN12, FR_RUN13, FR_RUN14, FR_RUN15};

	enum {FR_JUMP1 = 44, FR_JUMP2, FR_JUMP3, FR_JUMP4, FR_JUMP5, FR_JUMP6, FR_JUMP7};
	enum {FR_JUMP8 = 51, FR_JUMP9, FR_JUMP10, FR_JUMP11, FR_JUMP12, FR_JUMP13, FR_JUMP14};
	enum {FR_JUMP15 = 58, FR_JUMP16};

	enum {FR_ATTK1 = 60, FR_ATTK2, FR_ATTK3, FR_ATTK4, FR_ATTK5, FR_ATTK6};
	enum {FR_ATTK7 = 66, FR_ATTK8, FR_ATTK9, FR_ATTK10, FR_ATTK11};

	enum {FR_MAUL1 = 71, FR_MAUL2, FR_MAUL3, FR_MAUL4, FR_MAUL5, FR_MAUL6};
	enum {FR_MAUL7 = 77, FR_MAUL8, FR_MAUL9, FR_MAUL10, FR_MAUL11, FR_MAUL12, FR_MAUL13};

	enum {FR_SPAWN1 = 84, FR_SPAWN2, FR_SPAWN3, FR_SPAWN4, FR_SPAWN5, FR_SPAWN6};

	enum {FR_LOOK1 = 90, FR_LOOK2, FR_LOOK3, FR_LOOK4, FR_LOOK5, FR_LOOK6, FR_LOOK7};
	enum {FR_LOOK8 = 97, FR_LOOK9, FR_LOOK10};

	enum {FR_PAIN1 = 100, FR_PAIN2, FR_PAIN3, FR_PAIN4};

	enum {FR_DEATH1 = 104, FR_DEATH2, FR_DEATH3, FR_DEATH4, FR_DEATH5, FR_DEATH6, FR_DEATH7};
	enum {FR_DEATH8 = 111, FR_DEATH9, FR_DEATH10, FR_DEATH11, FR_DEATH12};

	enum {FR_FLIP1 = 116, FR_FLIP2, FR_FLIP3, FR_FLIP4, FR_FLIP5, FR_FLIP6, FR_FLIP7};
	enum {FR_FLIP8 = 123};

	enum {FR_LUNGE1 = 124, FR_LUNGE2, FR_LUNGE3, FR_LUNGE4, FR_LUNGE5, FR_LUNGE6, FR_LUNGE7};
	enum {FR_LUNGE8 = 131, FR_LUNGE9, FR_LUNGE10, FR_LUNGE11};

	enum {FR_GFIRE1 = 135, FR_GFIRE2, FR_GFIRE3, FR_GFIRE4, FR_GFIRE5, FR_GFIRE6};

	enum {FR_GLOOK1 = 141, FR_GLOOK2, FR_GLOOK3, FR_GLOOK4, FR_GLOOK5, FR_GLOOK6, FR_GLOOK7};
	enum {FR_GLOOK8 = 148, FR_GLOOK9, FR_GLOOK10, FR_GLOOK11, FR_GLOOK12, FR_GLOOK13};
	enum {FR_GLOOK14 = 154, FR_GLOOK15, FR_GLOOK16, FR_GLOOK17, FR_GLOOK18, FR_GLOOK19, FR_GLOOK20};

	enum {FR_GPAIN1 = 161, FR_GPAIN2, FR_GPAIN3};

	enum {FR_GRUN1 = 164, FR_GRUN2, FR_GRUN3, FR_GRUN4, FR_GRUN5, FR_GRUN6, FR_GRUN7};
	enum {FR_GRUN8 = 171, FR_GRUN9, FR_GRUN10, FR_GRUN11, FR_GRUN12, FR_GRUN13, FR_GRUN14, FR_GRUN15};
}

//============================================================================

float NumGremlins;
float NumSpawnGremlins;

void SUB_CopyEntity(entity srcent, entity destent);

void MuzzleFlash(entity ent);

/*
===========
GremlinAttemptWeaponSteal

see if we can steal enemy's weapon
============
*/
entity GremlinFindVictim();
float GremlinFindTarget();

float GremlinAttemptWeaponSteal()
{
	vector delta;
	entity tempself;
	int best;
	entity victim;
	float amount;

	if (self->has_fieldgen)
	{
		RPrint("gremlin trying to steal a weapon again\n");
		return PR_FALSE;
	}
	if (!(self->enemy->flags & PR_FL_CLIENT))
	{
		return PR_FALSE;
	}

	delta = (self->enemy->origin - self->origin);

	if (vlen(delta) > 100)
		return PR_FALSE;
	if (random() < 0.5)
		return PR_FALSE;
	//
	// we are within range so lets go for it
	//
	victim = self->enemy;
	best = victim->weapon;
	if (best == PR_IT_AXE || best == PR_IT_SHOTGUN)// || best == #IT_MJOLNIR)
		return PR_FALSE;
	// take that weapon from the entity
	victim->items = victim->items - (victim->items & best);
	// give it to our gremlin
	self->items = self->items | best;
	self->weapon = best;
	// take some ammo while we are at it
	self->items = self->items - ( self->items & (PR_IT_SHELLS | PR_IT_NAILS | PR_IT_ROCKETS | PR_IT_CELLS ) );
	if (best == PR_IT_SUPER_SHOTGUN)
	{
		amount = victim->ammo_shells;
		if (amount > 20)
			amount = 20;
		victim->ammo_shells = victim->ammo_shells - amount;
		self->ammo_shells = self->ammo_shells + amount;
		self->items = self->items | PR_IT_SHELLS;
		self->currentammo = self->ammo_shells;
		sprint (victim,PR_PRINT_HIGH, "Gremlin stole your Super Shotgun\n");
	}
	else if (best == PR_IT_NAILGUN)
	{
		amount = victim->ammo_nails;
		if (amount > 40)
			amount = 40;
		victim->ammo_nails = victim->ammo_nails - amount;
		self->ammo_nails = self->ammo_nails + amount;
		self->items = self->items | PR_IT_NAILS;
		self->currentammo = self->ammo_nails;
		sprint (victim,PR_PRINT_HIGH, "Gremlin stole your Nailgun\n");
	}
	else if (best == PR_IT_SUPER_NAILGUN)
	{
		amount = victim->ammo_nails;
		if (amount > 40)
			amount = 40;
		victim->ammo_nails = victim->ammo_nails - amount;
		self->ammo_nails = self->ammo_nails + amount;
		self->items = self->items | PR_IT_NAILS;
		self->currentammo = self->ammo_nails;
		sprint (victim,PR_PRINT_HIGH, "Gremlin stole your Super Nailgun\n");
	}
	else if (best == PR_IT_GRENADE_LAUNCHER)
	{
		amount = victim->ammo_rockets;
		if (amount > 5)
			amount = 5;
		victim->ammo_rockets = victim->ammo_rockets - amount;
		self->ammo_rockets = self->ammo_rockets + amount;
		self->items = self->items | PR_IT_ROCKETS;
		self->currentammo = self->ammo_rockets;
		sprint (victim,PR_PRINT_HIGH, "Gremlin stole your Grenade Launcher\n");
	}
	else if (best == PR_IT_ROCKET_LAUNCHER)
	{
		amount = victim->ammo_rockets;
		/*if (amount > 5)
			amount = 5;*/
		amount = amount * 0.75;
		if (amount < 25)
			amount = victim->ammo_rockets;

		victim->ammo_rockets = victim->ammo_rockets - amount;
		self->ammo_rockets = self->ammo_rockets + amount;
		self->items = self->items | PR_IT_ROCKETS;
		self->currentammo = self->ammo_rockets;
		sprint (victim,PR_PRINT_HIGH, "Gremlin stole your Rocket Launcher\n");
	}
	else if (best == PR_IT_LIGHTNING)
	{
		amount = victim->ammo_cells;
		if (amount > 40)
			amount = 40;
		victim->ammo_cells = victim->ammo_cells - amount;
		self->ammo_cells = self->ammo_cells + amount;
		self->items = self->items | PR_IT_CELLS;
		self->currentammo = self->ammo_cells;
		sprint (victim,PR_PRINT_HIGH, "Gremlin stole your Lightning Gun\n");
	}
	/*else if (best == #IT_LASER_CANNON)
	{
		amount = victim.ammo_cells;
		if (amount > 40)
			amount = 40;
		victim.ammo_cells = victim.ammo_cells - amount;
		self.ammo_cells = self.ammo_cells + amount;
		self.items = self.items | #IT_CELLS;
		self.currentammo = self.ammo_cells;
		sprint (victim, "Gremlin stole your Laser Cannon\n");
	}*/
	/*else if (best == #IT_PROXIMITY_GUN)
	{
		amount = victim.ammo_rockets;
		if (amount > 5)
			amount = 5;
		victim.ammo_rockets = victim.ammo_rockets - amount;
		self.ammo_rockets = self.ammo_rockets + amount;
		self.items = self.items | #IT_ROCKETS;
		self.currentammo = self.ammo_rockets;
		sprint (victim, "Gremlin stole your Proximity Gun\n");
	}*/
	tempself = self;
	self = victim;
	self->weapon = W_BestWeapon ();
	W_SetCurrentAmmo ();
	self = tempself;

	// tag the gremlin as having stolen a weapon
	self->has_fieldgen = PR_TRUE;
	self->attack_finished = time;
	// don't fire the first shot at the person we stole the weapon from
	// all the time
	if (random()>0.65)
		self->martyr_enemy = victim;
	else
		self->martyr_enemy = self;
//	self.attack_state = AS_STRAIGHT;
	// find a recipient
	victim = GremlinFindVictim();
	if (victim != world)
	{
		self->enemy = victim;
		FoundTarget();
		self->attack_finished = time;
		self->search_time = time + 1.0;
	}

	return PR_TRUE;
}
//============================================================================

/*
===========
GremlinFindTarget

gremlin is currently not attacking anything, so try to find a target

============
*/
float GremlinFindTarget()
{
	entity	head;
	entity	gorge;
	float		dist, visible_distance;
	float		result;

	if ((self->has_fieldgen==0) && time > self->wait)
	{
		self->wait = time + 1.0;
		dist = 2000;
		gorge = world;
		head = nextent(world);
		while (head!=world)
		{
			if ((head->health < 1) && (head->flags & (PR_FL_MONSTER|PR_FL_CLIENT)))
			{
				result = fabs(head->origin[Z] - self->origin[Z]);
				visible_distance = vlen(self->origin - head->origin);
				if ((visible(head)) && (result<80) && (head->has_camera == PR_FALSE) && (visible_distance<dist))
				{
					dist = visible_distance;
					gorge = head;
				}
			}
			head = nextent(head);
		}
		if ((gorge != world) && (dist < (700*random())))
		{
//			dprint("starting to gorge on ");
//			dprint(gorge.classname);
//			dprint("\n");
			self->oldenemy = self->enemy;
			self->has_camera = PR_TRUE;
			self->enemy = gorge;
			self->search_time = time + 4.0;
			FoundTarget();
			return PR_TRUE;
		}
	}
	else if (self->has_fieldgen)
	{
		head = GremlinFindVictim();
		if (head != world)
		{
			self->enemy = head;
			FoundTarget();
			self->attack_finished = time;
			self->search_time = time + 2.0;
			return PR_TRUE;
		}
	}
	result = FindTarget();
	self->search_time = time + 2.0;
	return result;
}
//============================================================================
/*
=============
gremlin_walk

The monster is walking it's beat
=============
*/
void gremlin_walk(float dist)
{
	movedist = dist;

	// check for noticing a player
	if (GremlinFindTarget ())
		return;

	movetogoal (dist);
}

//============================================================================

/*
=============
gremlin_stand

The monster is staying in one place for a while, with slight angle turns
=============
*/
void gremlin_stand()
{
	if (FindTarget ())
		return;

	if (time > self->pausetime)
	{
		self->th_walk ();
		return;
	}
}

//============================================================================
float GremlinCheckNoAmmo();
void gremlin_glook1();
/*
=============
gremlin_run

The monster has an enemy it is trying to kill
=============
*/
void gremlin_run(float dist)
{
//	local entity	oldtarget;
	float		r;
	vector	d;

	if (self->watertype == PR_CONTENT_LAVA)
	{  // do damage
		T_Damage (self, world, world, 2000);
	}
	movedist = dist;
	if (self->has_fieldgen)
		self->frame = self->frame + CuTF_GremlinFrames::FR_GRUN1 - CuTF_GremlinFrames::FR_RUN1;

	if (self->has_camera)
	{
		traceline(self->origin, self->enemy->origin, PR_TL_BSP_ONLY, self);
		if (trace_fraction != 1.0)
		{
			self->has_camera = PR_FALSE;
			return;
		}
		if (!visible(self->enemy))
		{
			self->has_camera = PR_FALSE;
			return;
		}
		r = vlen(self->enemy->origin - self->origin);
		if (r < 130)
		{
			ai_face();
			if (r < 45)
			{
				self->th_melee ();
				self->attack_state = PR_AS_STRAIGHT;
				return;
			}
			else if (walkmove (self->angles[Y], dist) == PR_FALSE)
			{
				self->has_camera = PR_FALSE;
				return;
			}
			return;
		}
		movetogoal (dist);	  // done in C code...
	}
	else
	{
		if (random()>0.97)
		{
			if (GremlinFindTarget())
				return;
		}
		// get away from player if we stole a weapon
		if (self->has_fieldgen)
		{
			if (self->enemy->health < 0 && self->enemy->classname == "player")
			{
				gremlin_glook1();
				return;
			}
			if (!GremlinCheckNoAmmo())
			{
				if (self->t_length==1)
				{
					remove(self->trigger_field);
					self->goalentity = self->enemy;
					self->t_length = 0;
				}
				return;
			}
			r = vlen(self->enemy->origin - self->origin);
			d = normalize(self->origin-self->enemy->origin);
			if (self->t_length == 0)
			{
				if (r<150)
				{
					self->trigger_field = spawn();
					setsize(self->trigger_field,V({-1, -1, -1}),V({1, 1, 1}));
					self->t_length = 1;
				}
			}
			if (self->t_length==1)
			{
				if (r > 250)
				{
					remove(self->trigger_field);
					self->goalentity = self->enemy;
					self->t_length = 0;
//					self.attack_state = AS_SLIDING;
				}
				else
				{
					if (r < 160)
					{
						vector ang;
						float done;
						vector end;
						float c;

						ang = vectoangles(d);
						done = 0;
						c = 0;
						while (done == 0)
						{
							makevectors(ang);
							end = self->enemy->origin + v_forward * 350;
							traceline(self->enemy->origin,end,PR_TL_ANY_SOLID,self);
							if (trace_fraction == 1.0)
							{
								traceline(self->origin,end,PR_TL_ANY_SOLID,self);
								if (trace_fraction == 1.0)
									done = 1;
							}
							ang[Y] = anglemod(ang[Y] + 36);
							c = c + 1;
							if (c == 10)
							{
								done = 1;
							}
						}
						setorigin(self->trigger_field,end);
					}
					self->goalentity = self->trigger_field;
					self->ideal_yaw = vectoyaw(normalize(self->goalentity->origin-self->origin));
					ChangeYaw ();
					movetogoal (dist);	   // done in C code...
					self->nextthink = time + 0.1;
					return;
				}
			}
		}
		ai_run(dist);
		self->nextthink = time + 0.1;
	}
}

//============================================================================

void Gremlin_JumpTouch();
void Gremlin_FlipTouch();
void Gremlin_Melee(float side);
void Gremlin_Gorge(float side);

void gremlin_stand1() {FRAME_STATE(CuTF_GremlinFrames::FR_STAND1, gremlin_stand2); gremlin_stand();self->nextthink = time + 0.2;}
void gremlin_stand2() {FRAME_STATE(CuTF_GremlinFrames::FR_STAND2, gremlin_stand3); gremlin_stand();self->nextthink = time + 0.2;}
void gremlin_stand3() {FRAME_STATE(CuTF_GremlinFrames::FR_STAND3, gremlin_stand4); gremlin_stand();self->nextthink = time + 0.2;}
void gremlin_stand4() {FRAME_STATE(CuTF_GremlinFrames::FR_STAND4, gremlin_stand5); gremlin_stand();self->nextthink = time + 0.2;}
void gremlin_stand5() {FRAME_STATE(CuTF_GremlinFrames::FR_STAND5, gremlin_stand6); gremlin_stand();self->nextthink = time + 0.2;}
void gremlin_stand6() {FRAME_STATE(CuTF_GremlinFrames::FR_STAND6, gremlin_stand7); gremlin_stand();self->nextthink = time + 0.2;}
void gremlin_stand7() {FRAME_STATE(CuTF_GremlinFrames::FR_STAND7, gremlin_stand8); gremlin_stand();self->nextthink = time + 0.2;}
void gremlin_stand8() {FRAME_STATE(CuTF_GremlinFrames::FR_STAND8, gremlin_stand9); gremlin_stand();self->nextthink = time + 0.2;}
void gremlin_stand9() {FRAME_STATE(CuTF_GremlinFrames::FR_STAND9, gremlin_stand10); gremlin_stand();self->nextthink = time + 0.2;}
void gremlin_stand10() {FRAME_STATE(CuTF_GremlinFrames::FR_STAND10, gremlin_stand11); gremlin_stand();self->nextthink = time + 0.2;}
void gremlin_stand11() {FRAME_STATE(CuTF_GremlinFrames::FR_STAND11, gremlin_stand12); gremlin_stand();self->nextthink = time + 0.2;}
void gremlin_stand12() {FRAME_STATE(CuTF_GremlinFrames::FR_STAND12, gremlin_stand13); gremlin_stand();self->nextthink = time + 0.2;}
void gremlin_stand13() {FRAME_STATE(CuTF_GremlinFrames::FR_STAND13, gremlin_stand14); gremlin_stand();self->nextthink = time + 0.2;}
void gremlin_stand14() {FRAME_STATE(CuTF_GremlinFrames::FR_STAND14, gremlin_stand15); gremlin_stand();self->nextthink = time + 0.2;}
void gremlin_stand15() {FRAME_STATE(CuTF_GremlinFrames::FR_STAND15, gremlin_stand16); gremlin_stand();self->nextthink = time + 0.2;}
void gremlin_stand16() {FRAME_STATE(CuTF_GremlinFrames::FR_STAND16, gremlin_stand17); gremlin_stand();self->nextthink = time + 0.2;}
void gremlin_stand17() {FRAME_STATE(CuTF_GremlinFrames::FR_STAND17, gremlin_stand1); gremlin_stand();self->nextthink = time + 0.2;}

void gremlin_walk1() {
FRAME_STATE(CuTF_GremlinFrames::FR_WALK1, gremlin_walk2);
if (random() < 0.1)
	sound (self, PR_CHAN_VOICE, "grem/idle.wav", 1, PR_ATTN_IDLE);
gremlin_walk(8);}
void gremlin_walk2() {FRAME_STATE(CuTF_GremlinFrames::FR_WALK2, gremlin_walk3); gremlin_walk(8);}
void gremlin_walk3() {FRAME_STATE(CuTF_GremlinFrames::FR_WALK3, gremlin_walk4); gremlin_walk(8);}
void gremlin_walk4() {FRAME_STATE(CuTF_GremlinFrames::FR_WALK4, gremlin_walk5); gremlin_walk(8);}
void gremlin_walk5() {FRAME_STATE(CuTF_GremlinFrames::FR_WALK5, gremlin_walk6); gremlin_walk(8);}
void gremlin_walk6() {FRAME_STATE(CuTF_GremlinFrames::FR_WALK6, gremlin_walk7); gremlin_walk(8);}
void gremlin_walk7() {FRAME_STATE(CuTF_GremlinFrames::FR_WALK7, gremlin_walk8); gremlin_walk(8);}
void gremlin_walk8() {FRAME_STATE(CuTF_GremlinFrames::FR_WALK8, gremlin_walk9); gremlin_walk(8);}
void gremlin_walk9() {FRAME_STATE(CuTF_GremlinFrames::FR_WALK9, gremlin_walk10); gremlin_walk(8);}
void gremlin_walk10() {FRAME_STATE(CuTF_GremlinFrames::FR_WALK10, gremlin_walk11); gremlin_walk(8);}
void gremlin_walk11() {FRAME_STATE(CuTF_GremlinFrames::FR_WALK11, gremlin_walk12); gremlin_walk(8);}
void gremlin_walk12() {FRAME_STATE(CuTF_GremlinFrames::FR_WALK12, gremlin_walk1); gremlin_walk(8);}

void gremlin_run1() {
FRAME_STATE(CuTF_GremlinFrames::FR_RUN1, gremlin_run2);
if (random() < 0.1)
	sound (self, PR_CHAN_VOICE, "grem/idle.wav", 1, PR_ATTN_IDLE);
gremlin_run(0);}
void gremlin_run2() {FRAME_STATE(CuTF_GremlinFrames::FR_RUN2, gremlin_run3); gremlin_run(12);} // original 8
void gremlin_run3() {FRAME_STATE(CuTF_GremlinFrames::FR_RUN3, gremlin_run4); gremlin_run(16);} // original 12
void gremlin_run4() {FRAME_STATE(CuTF_GremlinFrames::FR_RUN4, gremlin_run5); gremlin_run(20);} // original 16
void gremlin_run5() {FRAME_STATE(CuTF_GremlinFrames::FR_RUN5, gremlin_run6); gremlin_run(20);} // original 16
void gremlin_run6() {FRAME_STATE(CuTF_GremlinFrames::FR_RUN6, gremlin_run7); gremlin_run(16);} // original 12
void gremlin_run7() {FRAME_STATE(CuTF_GremlinFrames::FR_RUN7, gremlin_run8); gremlin_run(12);} // original 8
void gremlin_run8() {FRAME_STATE(CuTF_GremlinFrames::FR_RUN8, gremlin_run9); gremlin_run(0);} // original 0
void gremlin_run9() {FRAME_STATE(CuTF_GremlinFrames::FR_RUN9, gremlin_run10); gremlin_run(12);} // original 8
void gremlin_run10() {FRAME_STATE(CuTF_GremlinFrames::FR_RUN10, gremlin_run11); gremlin_run(16);} // original 12
void gremlin_run11() {FRAME_STATE(CuTF_GremlinFrames::FR_RUN11, gremlin_run12); gremlin_run(20);} // original 16
void gremlin_run12() {FRAME_STATE(CuTF_GremlinFrames::FR_RUN12, gremlin_run13); gremlin_run(20);} // original 16
void gremlin_run13() {FRAME_STATE(CuTF_GremlinFrames::FR_RUN13, gremlin_run14); gremlin_run(16);} // original 12
void gremlin_run14() {FRAME_STATE(CuTF_GremlinFrames::FR_RUN14, gremlin_run15); gremlin_run(12);} // original 8
void gremlin_run15() {FRAME_STATE(CuTF_GremlinFrames::FR_RUN15, gremlin_run1); gremlin_run(0);} // original 0

void gremlin_jump1() {FRAME_STATE(CuTF_GremlinFrames::FR_JUMP1, gremlin_jump2); ai_face();}
void gremlin_jump2() {FRAME_STATE(CuTF_GremlinFrames::FR_JUMP2, gremlin_jump3); ai_face();}
void gremlin_jump3() {FRAME_STATE(CuTF_GremlinFrames::FR_JUMP3, gremlin_jump4); ai_face();}
void gremlin_jump4() {FRAME_STATE(CuTF_GremlinFrames::FR_JUMP4, gremlin_jump5); ai_face();}
void gremlin_jump5()
{
	FRAME_STATE(CuTF_GremlinFrames::FR_JUMP5, gremlin_jump6);
	ai_face();

   if (self->flags & PR_FL_ONGROUND)
	  {
	  self->touch = Gremlin_JumpTouch;
	  makevectors (self->angles);
	  self->origin[Z] = self->origin[Z] + 1;
	  self->velocity = v_forward * 300 + V({0, 0, 300});
	  self->flags = self->flags - PR_FL_ONGROUND;
	  }
   else
	  {
	  gremlin_run1();
	  }
}
void gremlin_jump6() {FRAME_STATE(CuTF_GremlinFrames::FR_JUMP6, gremlin_jump7);}
void gremlin_jump7() {FRAME_STATE(CuTF_GremlinFrames::FR_JUMP7, gremlin_jump8);}
void gremlin_jump8() {FRAME_STATE(CuTF_GremlinFrames::FR_JUMP8, gremlin_jump9);}
void gremlin_jump9() {FRAME_STATE(CuTF_GremlinFrames::FR_JUMP9, gremlin_jump10);}
void gremlin_jump10() {FRAME_STATE(CuTF_GremlinFrames::FR_JUMP10, gremlin_jump11);}
void gremlin_jump11()
   {
   FRAME_STATE(CuTF_GremlinFrames::FR_JUMP11, gremlin_jump1);
   self->nextthink = time + 3;
   // if three seconds pass, assume gremlin is stuck and jump again
   }

void gremlin_jump12() {FRAME_STATE(CuTF_GremlinFrames::FR_JUMP12, gremlin_jump13);}
void gremlin_jump13() {FRAME_STATE(CuTF_GremlinFrames::FR_JUMP13, gremlin_jump14);}
void gremlin_jump14() {FRAME_STATE(CuTF_GremlinFrames::FR_JUMP14, gremlin_jump15);}
void gremlin_jump15() {FRAME_STATE(CuTF_GremlinFrames::FR_JUMP15, gremlin_jump16);}
void gremlin_jump16() {FRAME_STATE(CuTF_GremlinFrames::FR_JUMP16, gremlin_run1);}

void gremlin_shot1()  {//self.effects = self.effects | #EF_MUZZLEFLASH;
FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_shot2);
MuzzleFlash(self);
}
void gremlin_shot2()  {FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE2, gremlin_shot3);}
void gremlin_shot3()  {FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE3, gremlin_shot4);}
void gremlin_shot4()  {FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE4, gremlin_shot5);}
void gremlin_shot5()  {FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE5, gremlin_shot6);}
void gremlin_shot6()  {FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE6, gremlin_run1);}


//============================================================================
void Gremlin_FireNailGun(float ox);

void gremlin_nail1()
   {
	//self.effects = self.effects | #EF_MUZZLEFLASH;
	FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_nail2);
	MuzzleFlash(self);
   Gremlin_FireNailGun(4);
   }
void gremlin_nail2()
   {
	//self.effects = self.effects | #EF_MUZZLEFLASH;
	FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_nail3);
	MuzzleFlash(self);
   Gremlin_FireNailGun(4);
   }
void gremlin_nail3()
   {
	//self.effects = self.effects | #EF_MUZZLEFLASH;
	FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_nail4);
	MuzzleFlash(self);
   Gremlin_FireNailGun(4);
   }
void gremlin_nail4()
   {
	//self.effects = self.effects | #EF_MUZZLEFLASH;
	FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_nail5);
	MuzzleFlash(self);
   Gremlin_FireNailGun(4);
   }
void gremlin_nail5()
   {
	//self.effects = self.effects | #EF_MUZZLEFLASH;
	FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_nail6);
	MuzzleFlash(self);
   Gremlin_FireNailGun(4);
   }
void gremlin_nail6()
   {
	//self.effects = self.effects | #EF_MUZZLEFLASH;
	FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_nail7);
	MuzzleFlash(self);
   Gremlin_FireNailGun(4);
   }
void gremlin_nail7()  {FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_run1);}


void Gremlin_FireLaserGun(float ox);

void gremlin_laser1()
   {
	//self.effects = self.effects | #EF_MUZZLEFLASH;
	FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_laser2);
	MuzzleFlash(self);
   Gremlin_FireLaserGun(4);
   }
void gremlin_laser2()
   {
	//self.effects = self.effects | #EF_MUZZLEFLASH;
	FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_laser3);
	MuzzleFlash(self);
   Gremlin_FireLaserGun(4);
   }
void gremlin_laser3()
   {
	//self.effects = self.effects | #EF_MUZZLEFLASH;
	FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_laser4);
	MuzzleFlash(self);
   Gremlin_FireLaserGun(4);
   }
void gremlin_laser4()
   {
	//self.effects = self.effects | #EF_MUZZLEFLASH;
	FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_laser5);
	MuzzleFlash(self);
   Gremlin_FireLaserGun(4);
   }
void gremlin_laser5()
   {
	//self.effects = self.effects | #EF_MUZZLEFLASH;
	FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_laser6);
	MuzzleFlash(self);
   Gremlin_FireLaserGun(4);
   }
void gremlin_laser6()
   {
	//self.effects = self.effects | #EF_MUZZLEFLASH;
	FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_laser7);
	MuzzleFlash(self);
   Gremlin_FireLaserGun(4);
   }
void gremlin_laser7()  {FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_run1);}

//============================================================================

void Gremlin_FireLightningGun();
void gremlin_light1()
   {
   FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_light2);
   Gremlin_FireLightningGun();
   }
void gremlin_light2()
   {
   FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_light3);
   Gremlin_FireLightningGun();
   }
void gremlin_light3()
   {
   FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_light4);
   Gremlin_FireLightningGun();
   }
void gremlin_light4()
{
   FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_light5);
   Gremlin_FireLightningGun();
}
void gremlin_light5()  {FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_run1);}

//============================================================================

void gremlin_rocket1()  {//self.effects = self.effects | #EF_MUZZLEFLASH;
FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE1, gremlin_rocket2);
MuzzleFlash(self);
}
void gremlin_rocket2()  {FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE2, gremlin_rocket3);}
void gremlin_rocket3()  {FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE3, gremlin_rocket4);}
void gremlin_rocket4()  {FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE4, gremlin_rocket5);}
void gremlin_rocket5()  {FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE5, gremlin_rocket6);}
void gremlin_rocket6()  {FRAME_STATE(CuTF_GremlinFrames::FR_GFIRE6, gremlin_run1);}

void gremlin_lunge1() {FRAME_STATE(CuTF_GremlinFrames::FR_LUNGE1, gremlin_lunge2); ai_charge(0);}
void gremlin_lunge2() {FRAME_STATE(CuTF_GremlinFrames::FR_LUNGE2, gremlin_lunge3); ai_charge(0);}
void gremlin_lunge3() {FRAME_STATE(CuTF_GremlinFrames::FR_LUNGE3, gremlin_lunge4); ai_charge(0);}
void gremlin_lunge4() {FRAME_STATE(CuTF_GremlinFrames::FR_LUNGE4, gremlin_lunge5); ai_charge(0);}
void gremlin_lunge5() {FRAME_STATE(CuTF_GremlinFrames::FR_LUNGE5, gremlin_lunge6); ai_charge(0);}
void gremlin_lunge6() {FRAME_STATE(CuTF_GremlinFrames::FR_LUNGE6, gremlin_lunge7); ai_charge(0);}
void gremlin_lunge7() {FRAME_STATE(CuTF_GremlinFrames::FR_LUNGE7, gremlin_lunge8); ai_charge(15);}
void gremlin_lunge8() {FRAME_STATE(CuTF_GremlinFrames::FR_LUNGE8, gremlin_lunge9); ai_charge(0);Gremlin_Melee(0);}
void gremlin_lunge9() {FRAME_STATE(CuTF_GremlinFrames::FR_LUNGE9, gremlin_lunge10); ai_charge(0);}
void gremlin_lunge10() {FRAME_STATE(CuTF_GremlinFrames::FR_LUNGE10, gremlin_lunge11); ai_charge(0);}
void gremlin_lunge11() {FRAME_STATE(CuTF_GremlinFrames::FR_LUNGE11, gremlin_run1); ai_charge(0);}

void gremlin_claw1() {FRAME_STATE(CuTF_GremlinFrames::FR_ATTK1, gremlin_claw2); ai_charge(0);}
void gremlin_claw2() {FRAME_STATE(CuTF_GremlinFrames::FR_ATTK2, gremlin_claw3); ai_charge(0);}
void gremlin_claw3() {FRAME_STATE(CuTF_GremlinFrames::FR_ATTK3, gremlin_claw4); ai_charge(0);}
void gremlin_claw4() {FRAME_STATE(CuTF_GremlinFrames::FR_ATTK4, gremlin_claw5); ai_charge(0);}
void gremlin_claw5() {FRAME_STATE(CuTF_GremlinFrames::FR_ATTK5, gremlin_claw6); ai_charge(0);}
void gremlin_claw6() {FRAME_STATE(CuTF_GremlinFrames::FR_ATTK6, gremlin_claw7); ai_charge(0);Gremlin_Melee(200);}
void gremlin_claw7() {FRAME_STATE(CuTF_GremlinFrames::FR_ATTK7, gremlin_claw8); ai_charge(15);}
void gremlin_claw8() {FRAME_STATE(CuTF_GremlinFrames::FR_ATTK8, gremlin_claw9); ai_charge(0);}
void gremlin_claw9() {FRAME_STATE(CuTF_GremlinFrames::FR_ATTK9, gremlin_claw10); ai_charge(0);}
void gremlin_claw10() {FRAME_STATE(CuTF_GremlinFrames::FR_ATTK10, gremlin_claw11); ai_charge(0);}
void gremlin_claw11() {FRAME_STATE(CuTF_GremlinFrames::FR_ATTK11, gremlin_run1); ai_charge(0);}

void gremlin_gorge1() {FRAME_STATE(CuTF_GremlinFrames::FR_MAUL1, gremlin_gorge2); ai_charge(1);}
void gremlin_gorge2() {FRAME_STATE(CuTF_GremlinFrames::FR_MAUL2, gremlin_gorge3); ai_charge(1);}
void gremlin_gorge3() {FRAME_STATE(CuTF_GremlinFrames::FR_MAUL3, gremlin_gorge4); ai_charge(2);}
void gremlin_gorge4() {FRAME_STATE(CuTF_GremlinFrames::FR_MAUL4, gremlin_gorge5); ai_charge(0);}
void gremlin_gorge5() {FRAME_STATE(CuTF_GremlinFrames::FR_MAUL5, gremlin_gorge6); ai_charge(0);}
void gremlin_gorge6() {FRAME_STATE(CuTF_GremlinFrames::FR_MAUL6, gremlin_gorge7); ai_charge(0);Gremlin_Gorge(200);}
void gremlin_gorge7() {FRAME_STATE(CuTF_GremlinFrames::FR_MAUL7, gremlin_gorge8); ai_charge(0);}
void gremlin_gorge8() {FRAME_STATE(CuTF_GremlinFrames::FR_MAUL8, gremlin_gorge9); ai_charge(0);Gremlin_Gorge(-200);}
void gremlin_gorge9() {FRAME_STATE(CuTF_GremlinFrames::FR_MAUL9, gremlin_gorge10); ai_charge(0);}
void gremlin_gorge10() {FRAME_STATE(CuTF_GremlinFrames::FR_MAUL10, gremlin_gorge11); ai_charge(0);}
void gremlin_gorge11() {FRAME_STATE(CuTF_GremlinFrames::FR_MAUL11, gremlin_gorge12); ai_charge(0);}
void gremlin_gorge12() {FRAME_STATE(CuTF_GremlinFrames::FR_MAUL12, gremlin_gorge13); ai_charge(0);}
void gremlin_gorge13() {FRAME_STATE(CuTF_GremlinFrames::FR_MAUL13, gremlin_gorge1); ai_charge(0);}

void gremlin_look1() {FRAME_STATE(CuTF_GremlinFrames::FR_LOOK1, gremlin_look2); self->nextthink = time + 0.2;}
void gremlin_look2() {FRAME_STATE(CuTF_GremlinFrames::FR_LOOK2, gremlin_look3); self->nextthink = time + 0.2;}
void gremlin_look3() {FRAME_STATE(CuTF_GremlinFrames::FR_LOOK3, gremlin_look4); self->nextthink = time + 0.2;}
void gremlin_look4() {FRAME_STATE(CuTF_GremlinFrames::FR_LOOK4, gremlin_look5); self->nextthink = time + 0.2;}
void gremlin_look5() {FRAME_STATE(CuTF_GremlinFrames::FR_LOOK5, gremlin_look6); self->nextthink = time + 0.2;}
void gremlin_look6() {FRAME_STATE(CuTF_GremlinFrames::FR_LOOK6, gremlin_look7); self->nextthink = time + 0.2;}
void gremlin_look7() {FRAME_STATE(CuTF_GremlinFrames::FR_LOOK7, gremlin_look8); self->nextthink = time + 0.2;}
void gremlin_look8() {FRAME_STATE(CuTF_GremlinFrames::FR_LOOK8, gremlin_look9); self->nextthink = time + 0.2;}
void gremlin_look9()
   {
   //if (self.oldenemy.health > 0)
   FRAME_STATE(CuTF_GremlinFrames::FR_LOOK9, gremlin_run1);
   if (StillValidEnemy(self->oldenemy))
	  {
	  self->enemy = self->oldenemy;
	  HuntTarget ();
	  }
   else
	  {
		MonsterIdle(self);
	  /*if (self.movetarget)
		 self.th_walk ();
	  else
		 self.th_stand ();*/
	  }
   }


void GremlinDropBackpack();
void gremlin_glook1() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK1, gremlin_glook2);}
void gremlin_glook2() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK2, gremlin_glook3);}
void gremlin_glook3() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK3, gremlin_glook4);}
void gremlin_glook4() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK4, gremlin_glook5);}
void gremlin_glook5() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK5, gremlin_glook6);}
void gremlin_glook6() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK6, gremlin_glook7);}
void gremlin_glook7() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK7, gremlin_glook8);}
void gremlin_glook8() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK8, gremlin_glook9);}
void gremlin_glook9() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK9, gremlin_glook10);}
void gremlin_glook10() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK10, gremlin_glook11);}
void gremlin_glook11() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK11, gremlin_glook12);}
void gremlin_glook12() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK12, gremlin_glook13);}
void gremlin_glook13() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK13, gremlin_glook14);}
void gremlin_glook14() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK14, gremlin_glook15);}
void gremlin_glook15() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK15, gremlin_glook16);}
void gremlin_glook16() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK16, gremlin_glook17);}
void gremlin_glook17() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK17, gremlin_glook18);}
void gremlin_glook18() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK18, gremlin_glook19);}
void gremlin_glook19() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK19, gremlin_glook20);}
void gremlin_glook20()
   {
   //GremlinDropBackpack();
   //self.has_fieldgen = #FALSE;

   //if (self.oldenemy.health > 0)
   FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK20, gremlin_glook20);
   if (StillValidEnemy(self->oldenemy))
	  {
	  self->enemy = self->oldenemy;
	  HuntTarget ();
	  }
   else
	  {
		MonsterIdle(self);
	  /*if (self.movetarget)
		 self.th_walk ();
	  else
		 self.th_stand ();*/
	  }
   }

void gremlin_pain1() {FRAME_STATE(CuTF_GremlinFrames::FR_PAIN1, gremlin_pain2); ai_back(4);}
void gremlin_pain2() {FRAME_STATE(CuTF_GremlinFrames::FR_PAIN2, gremlin_pain3); ai_back(4);}
void gremlin_pain3() {FRAME_STATE(CuTF_GremlinFrames::FR_PAIN3, gremlin_pain4); ai_back(2);}
void gremlin_pain4() {FRAME_STATE(CuTF_GremlinFrames::FR_PAIN4, gremlin_run1);}

void gremlin_gunpain1() {FRAME_STATE(CuTF_GremlinFrames::FR_GPAIN1, gremlin_gunpain2); ai_back(4);}
void gremlin_gunpain2() {FRAME_STATE(CuTF_GremlinFrames::FR_GPAIN2, gremlin_gunpain3); ai_back(2);}
void gremlin_gunpain3() {FRAME_STATE(CuTF_GremlinFrames::FR_GPAIN3, gremlin_run1);}

void gremlin_pain(entity attacker, float damage)
{
   if (self->health <= 0)
		return; 	// allready dying, don't go into pain frame

   float r;

   /*if (random()<0.8)
	  {
	  self.has_camera = #FALSE;
	  self.enemy = attacker;
	  FoundTarget();
	  }*/
   if (self->touch == Gremlin_JumpTouch)
		return;

	if (self->pain_finished > time)
		return;

	AI_CheckAttacker(attacker);

	self->pain_finished = time + 1;
//	 if (random()<0.5)
//		sound (self, CHAN_VOICE, "grem/pain2.wav", 1, ATTN_NORM);
//	 else
   r = random();
   if (r<0.33)
	  sound (self, PR_CHAN_VOICE, "grem/pain1.wav", 1, PR_ATTN_NORM);
   else if (r<0.66)
	  sound (self, PR_CHAN_VOICE, "grem/pain2.wav", 1, PR_ATTN_NORM);
   else
	  sound (self, PR_CHAN_VOICE, "grem/pain3.wav", 1, PR_ATTN_NORM);

   if (self->has_fieldgen)
	  {
	  gremlin_gunpain1 ();
	  }
   else
	  {
	  gremlin_pain1 ();
	  }
}

void gremlin_spawn1() {FRAME_STATE(CuTF_GremlinFrames::FR_SPAWN1, gremlin_spawn2); self->nextthink = time + 0.3;
   self->th_pain = SUB_PainNull;
//	  sound (self, CHAN_VOICE, "grem/idle.wav", 1, ATTN_IDLE);
}
void gremlin_spawn2() {FRAME_STATE(CuTF_GremlinFrames::FR_SPAWN2, gremlin_spawn3); self->nextthink = time + 0.3;}
void gremlin_spawn3() {FRAME_STATE(CuTF_GremlinFrames::FR_SPAWN3, gremlin_spawn4); self->nextthink = time + 0.3;}
void gremlin_spawn4() {FRAME_STATE(CuTF_GremlinFrames::FR_SPAWN4, gremlin_spawn5); self->nextthink = time + 0.3;}
void gremlin_spawn5() {FRAME_STATE(CuTF_GremlinFrames::FR_SPAWN5, gremlin_spawn6); self->nextthink = time + 0.3;}
void gremlin_spawn6() {
   FRAME_STATE(CuTF_GremlinFrames::FR_SPAWN6, gremlin_run1);
   self->th_pain = gremlin_pain;
}


void gremlin_die1() {
FRAME_STATE(CuTF_GremlinFrames::FR_DEATH1, gremlin_die2);
sound (self, PR_CHAN_VOICE, "grem/death.wav", 1, PR_ATTN_MONSTERDIE);}
void gremlin_die2() {FRAME_STATE(CuTF_GremlinFrames::FR_DEATH2, gremlin_die3); ai_forward(2);}
void gremlin_die3() {FRAME_STATE(CuTF_GremlinFrames::FR_DEATH3, gremlin_die4); ai_forward(1);}
void gremlin_die4() {FRAME_STATE(CuTF_GremlinFrames::FR_DEATH4, gremlin_die5); ai_forward(2);}
void gremlin_die5() {FRAME_STATE(CuTF_GremlinFrames::FR_DEATH5, gremlin_die6); ai_forward(1);}
void gremlin_die6()
{FRAME_STATE(CuTF_GremlinFrames::FR_DEATH6, gremlin_die7); self->solid = PR_SOLID_NOT;}
void gremlin_die7() {FRAME_STATE(CuTF_GremlinFrames::FR_DEATH7, gremlin_die8); ai_forward(2);}
void gremlin_die8() {FRAME_STATE(CuTF_GremlinFrames::FR_DEATH8, gremlin_die9); ai_forward(1);}
void gremlin_die9() {FRAME_STATE(CuTF_GremlinFrames::FR_DEATH9, gremlin_die10); ai_forward(2);}
void gremlin_die10() {FRAME_STATE(CuTF_GremlinFrames::FR_DEATH10, gremlin_die11); ai_forward(1);}
void gremlin_die11() {FRAME_STATE(CuTF_GremlinFrames::FR_DEATH11, gremlin_die12); ai_forward(2);}
void gremlin_die12() {
	//self.nextthink = time + 40 + 40*random();
	//self.think = SUB_Remove;
	FRAME_STATE(CuTF_GremlinFrames::FR_DEATH12, gremlin_die12);
	MonsterCorpse();
}

void gremlin_gib()
   {
   sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_NORM);
   //ThrowHead ("progs/h_grem.mdl", -35);
   ThrowMonsterHead("progs/h_grem.mdl", -35);
   ThrowGib ("progs/gib1.mdl", -35,PR_TRUE,0,PR_GIB1_KGS, PR_FALSE);
   ThrowGib ("progs/gib1.mdl", -35,PR_TRUE,0,PR_GIB1_KGS, PR_FALSE);
   ThrowGib ("progs/gib1.mdl", -35,PR_TRUE,0,PR_GIB1_KGS, PR_FALSE);
   SpawnBloodEx(self->origin,0.66,9);
   dremove(self);
   }

void gremlin_flip1()
   {
   FRAME_STATE(CuTF_GremlinFrames::FR_FLIP1, gremlin_flip2);
   ai_face();
	makevectors (self->angles);
   self->origin[Z] = self->origin[Z] + 1;
   self->velocity = V({0, 0, 350}) - (v_forward * 200);
	if (self->flags & PR_FL_ONGROUND)
		self->flags = self->flags - PR_FL_ONGROUND;
   sound (self, PR_CHAN_VOICE, "grem/death.wav", 1, PR_ATTN_NORM);
   }
void gremlin_flip2() {FRAME_STATE(CuTF_GremlinFrames::FR_FLIP2, gremlin_flip3); ai_face();}
void gremlin_flip3() {FRAME_STATE(CuTF_GremlinFrames::FR_FLIP3, gremlin_flip4);}
void gremlin_flip4() {FRAME_STATE(CuTF_GremlinFrames::FR_FLIP4, gremlin_flip5);}
void gremlin_flip5() {FRAME_STATE(CuTF_GremlinFrames::FR_FLIP5, gremlin_flip6);}
void gremlin_flip6() {FRAME_STATE(CuTF_GremlinFrames::FR_FLIP6, gremlin_flip7); self->touch = Gremlin_FlipTouch;}
void gremlin_flip7()
   {
   FRAME_STATE(CuTF_GremlinFrames::FR_FLIP7, gremlin_gib);
   self->nextthink = time + 3;
   // if three seconds pass, assume gremlin is stuck and gib him
   }
void gremlin_flip8()
   {
   FRAME_STATE(CuTF_GremlinFrames::FR_FLIP8, gremlin_flip8);
   self->solid = PR_SOLID_NOT;
   if (self->health <= 0) // dead?
   {
		//self.nextthink = time + 40 + 40*random();
		//self.think = SUB_Remove;
		MonsterCorpse();
   }
   }

/*
===============
GremlinDropBackpack
===============
*/
void GremlinDropBackpack()
{
	entity	item;

	item = spawn();
	item->origin = self->origin - V({0, 0, 24});

   self->items = self->items - ( self->items & (PR_IT_SHELLS | PR_IT_NAILS | PR_IT_ROCKETS | PR_IT_CELLS ) );
   item->items = self->items;

   if (item->items & PR_IT_AXE)
		item->netname = "Axe";
   else if (item->items & PR_IT_SHOTGUN)
		item->netname = "Shotgun";
   else if (item->items & PR_IT_SUPER_SHOTGUN)
		item->netname = "Double-barrelled Shotgun";
   else if (item->items & PR_IT_NAILGUN)
		item->netname = "Nailgun";
   else if (item->items & PR_IT_SUPER_NAILGUN)
		item->netname = "Super Nailgun";
   else if (item->items & PR_IT_GRENADE_LAUNCHER)
		item->netname = "Grenade Launcher";
   else if (item->items & PR_IT_ROCKET_LAUNCHER)
		item->netname = "Rocket Launcher";
   else if (item->items & PR_IT_LIGHTNING)
		item->netname = "Thunderbolt";
   /*else if (item.items & #IT_LASER_CANNON)
	  item.netname = "Laser Cannon";
   else if (item.items & #IT_PROXIMITY_GUN)
	  item.netname = "Proximity Gun";
   else if (item.items & #IT_MJOLNIR)
	  item.netname = "Mjolnir";*/
   else
		item->netname = "";

   item->ammo_shells = self->ammo_shells;
	item->ammo_nails = self->ammo_nails;
	item->ammo_rockets = self->ammo_rockets;
	item->ammo_cells = self->ammo_cells;
   if (item->ammo_shells < 0) item->ammo_shells = 0;
   if (item->ammo_nails < 0) item->ammo_nails = 0;
   if (item->ammo_rockets < 0) item->ammo_rockets = 0;
   if (item->ammo_cells < 0) item->ammo_cells = 0;

	item->velocity[Z] = 300;
	item->velocity[X] = -100 + (random() * 200);
	item->velocity[Y] = -100 + (random() * 200);

	item->flags = PR_FL_ITEM;
	item->solid = PR_SOLID_TRIGGER;
	item->movetype = PR_MOVETYPE_TOSS;
	setmodel (item, "progs/backpack.mdl");
	setsize (item, V({-16, -16, 0}), V({16, 16, 56}));
	item->touch = BackpackTouch;

	item->nextthink = time + 120;	// remove after 2 minutes
	item->think = SUB_Remove;
}

void gremlin_die()
{
   self->effects=0;

	if (self->real_owner->classname == "player")
	{
		sprint(self->real_owner,PR_PRINT_HIGH,"Your gremlin ");
		sprint(self->real_owner,PR_PRINT_HIGH,self->netname);
		sprint(self->real_owner,PR_PRINT_HIGH," is dead.\n");
		self->real_owner->job_finished = time + 5; //Can't summon streams of demons SB can so
		ResetSlotPointer(self->real_owner, self->increase_team1);
	}

   vector   vec;
	float 	dot;

// check for gib OfN - Removed #|#IT_LASER_CANNON|#IT_PROXIMITY_GUN
   if (self->items & (PR_IT_SUPER_NAILGUN | PR_IT_SUPER_SHOTGUN|PR_IT_NAILGUN|PR_IT_GRENADE_LAUNCHER|PR_IT_ROCKET_LAUNCHER|PR_IT_LIGHTNING))
	  {
	  GremlinDropBackpack();
	  self->has_fieldgen = PR_FALSE;
	  }

   makevectors (self->angles);
   vec = normalize (damage_attacker->origin - self->origin);
	dot = vec * v_forward;

   if (self->health < -40)
	  {
		sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_MONSTERDIE);
	  ThrowMonsterHead ("progs/h_grem.mdl", self->health);
		ThrowGib ("progs/gib1.mdl", self->health,PR_TRUE,0,PR_GIB1_KGS, PR_FALSE);
		ThrowGib ("progs/gib2.mdl", self->health,PR_TRUE,0,PR_GIB2_KGS, PR_FALSE);
		ThrowGib ("progs/gib3.mdl", self->health,PR_TRUE,0,PR_GIB3_KGS, PR_FALSE);
		SpawnBloodEx(self->origin,0.66,9);
		dremove(self);
		return;
	  }

   // Spawn bubbles if under water/slime
	float contres;
	contres = pointcontents(self->origin);
	if (contres == PR_CONTENT_WATER || contres == PR_CONTENT_SLIME)
		DeathBubbles(5);

   if (dot>0.7 && (random()<0.5) && (self->flags & PR_FL_ONGROUND))
	  {
	  //self.classname = "monster_corpse";
	  MonsterDead();
	  gremlin_flip1();
	  return;
	  }
// regular death
   //self.classname = "monster_corpse";
   MonsterDead();
   gremlin_die1 ();
}


float GremlinWeaponAttack();
void Gremlin_MeleeAttack()
   {
   float num;
   if (self->has_camera)
	  {
	  gremlin_gorge1();
	  }
   else
	  {
	  if (self->has_fieldgen == 1)
		 {
		 objerror("gremlin meleeing with stolen weapon");
		 }
	  else if ((self->enemy->flags & PR_FL_CLIENT) && random() < 0.4)
		 {
		 if (GremlinAttemptWeaponSteal())
			return;
		 }
	  num = random();
	  if (num<0.3)
		 {
		 gremlin_claw1 ();
		 }
	  else if (num < 0.6)
		 {
		 gremlin_lunge1 ();
		 }
	  else
		 {
		 gremlin_claw1 ();
		 }
	  }
   }

/*
============
gremlin_recoil
============
*/
/*
void() gremlin_recoil =
   {
   self.nextthink = time + 0.1;
   self.flags = self.flags - FL_ONGROUND;
   if (vlen(self.velocity)<2)
	  {
	  self.movetype = self.spawnsolidtype;
	  self.think = self.spawnthink;
	  }
   };
*/
/*
============
GremlinRecoil
============
*/
/*
void(vector dir, float mag) GremlinRecoil =
   {
   self.spawnsolidtype = self.movetype;
   self.movetype = MOVETYPE_BOUNCE;
   self.spawnthink = self.think;
   self.think = gremlin_recoil;
   self.nextthink = time;
   self.velocity = mag * dir;
   self.velocity_z = self.velocity_z + 100;
   self.flags = self.flags - FL_ONGROUND;
   };
*/

/*
============
GremlinCheckNoAmmo

attack with a weapon
============
*/
float GremlinCheckNoAmmo()
   {
	if (self->currentammo > 0)
		return PR_TRUE;
   else
	  {
	  self->has_fieldgen = PR_FALSE;
	  return PR_FALSE;
	  }
   }

/*
============
GremlinFindVictim

find a victim to shoot at
============
*/
entity GremlinFindVictim()
   {
   entity head;
   entity selected;
   float dist;
   float head_dist;
//	 local float decision;

   self->search_time = time + 1.0;
// look in our immediate vicinity

   selected = world;
   dist = 1000;
   head = findradius(self->origin, 1000);
   while(head != world)
	  {
	  if(!(head->flags & PR_FL_NOTARGET) && ((head->flags & PR_FL_MONSTER) || (head->flags & PR_FL_CLIENT)))
		 {
		 if (visible(head) && (head->health > 0) && (head !=self))
			{
			head_dist = vlen(head->origin-self->origin);
			if (head == self->martyr_enemy)
			   head_dist = head_dist * 2;
			if (head->flags & PR_FL_CLIENT)
			   head_dist = head_dist / 1.5;
			if (head->classname == self->classname)
			   head_dist = head_dist * 1.5;
			if (head_dist < dist)
			   {
			   selected = head;
			   dist = head_dist;
			   }
			}
		 }
	  head = head->chain;
	  }
   self->martyr_enemy = selected;

   return selected;
   }

/*
============
Gremlin_FireRocket

fire a rocket
============
*/
void Gremlin_FireRocket()
{
	entity missile;//, mpuff;
   vector dir;

	self->currentammo = self->ammo_rockets = self->ammo_rockets - 1;
   //self.effects = self.effects | #EF_MUZZLEFLASH;
   MuzzleFlash(self);

	sound (self, PR_CHAN_WEAPON, "weapons/sgun1.wav", 1, PR_ATTN_NORM);

	//self.punchangle_x = -2;

	missile = spawn ();
	missile->owner = self;
	missile->movetype = PR_MOVETYPE_FLYMISSILE;
	missile->solid = PR_SOLID_BBOX;
	missile->classname = "rocket"; // was "missile"

// set missile speed

   dir = normalize(self->enemy->origin - self->origin);
   self->v_angle = vectoangles(dir);
   makevectors(self->v_angle);
   dir = dir + crandom()* 0.1 * v_right + crandom()* 0.1 *v_up;
   missile->velocity = normalize(dir);
   missile->velocity = missile->velocity * 1000;
	missile->angles = vectoangles(missile->velocity);

	missile->touch = T_MissileTouch;

// set missile duration
	missile->nextthink = time + 5;
	missile->think = SUB_Remove;

	setmodel (missile, "progs/missile.mdl");
	setsize (missile, V({0, 0, 0}), V({0, 0, 0}));
	setorigin (missile, self->origin + v_forward*8 + V({0, 0, 16}));
//	 GremlinRecoil(dir,-1000);
}

/*
============
Gremlin_FireNailGun

fire a nailgun
============
*/
void Gremlin_FireNailGun(float ox)
{
   vector dir;

   self->currentammo = self->ammo_nails = self->ammo_nails - 1;
   //self.effects = self.effects | #EF_MUZZLEFLASH;
   MuzzleFlash(self);

   sound (self, PR_CHAN_WEAPON, "weapons/rocket1i.wav", 1, PR_ATTN_NORM);
   dir = normalize(self->enemy->origin - self->origin);
   self->v_angle = vectoangles(dir);
   makevectors(self->v_angle);
   dir = dir + crandom()* 0.1 * v_right + crandom()* 0.1 *v_up;
   dir = normalize(dir);
   launch_spike (self->origin + V({0, 0, 16}), dir);
}

/*
============
Gremlin_FireLaserGun

fire a laser cannon
============
*/
void Gremlin_FireLaserGun(float ox)
{
   vector dir;

   self->currentammo = self->ammo_cells = self->ammo_cells - 1;
   //self.effects = self.effects | #EF_MUZZLEFLASH;
   MuzzleFlash(self);

   sound (self, PR_CHAN_WEAPON, "weapons/rocket1i.wav", 1, PR_ATTN_NORM);
   dir = normalize(self->enemy->origin - self->origin);
   self->v_angle = vectoangles(dir);
   makevectors(self->v_angle);
   dir = dir + crandom()* 0.1 * v_right + crandom()* 0.1 *v_up;
   dir = normalize(dir);
   HIP_LaunchLaser(self->origin + V({0, 0, 16}), dir, 0);
}

/*
============
Gremlin_FireShotGun

fire a shotgun
============
*/
void Gremlin_FireShotGun()
{
   vector dir;

   self->currentammo = self->ammo_shells = self->ammo_shells - 1;
   //self.effects = self.effects | #EF_MUZZLEFLASH;
   MuzzleFlash(self);

   sound (self, PR_CHAN_WEAPON, "weapons/guncock.wav", 1, PR_ATTN_NORM);
   dir = normalize(self->enemy->origin - self->origin);
   self->v_angle = vectoangles(dir);
   makevectors(self->v_angle);
   dir = dir + crandom()* 0.1 *v_right + crandom()* 0.1 *v_up;
   dir = normalize(dir);
   self->v_angle = vectoangles(dir);
   FireBullets (6, dir, V({0.04, 0.04, 0}),1024);
}

/*
============
Gremlin_FireSuperShotGun

fire a shotgun
============
*/
void Gremlin_FireSuperShotGun()
{
   vector dir;

   self->currentammo = self->ammo_shells = self->ammo_shells - 2;
   //self.effects = self.effects | #EF_MUZZLEFLASH;
   MuzzleFlash(self);

   sound (self ,PR_CHAN_WEAPON, "weapons/shotgn2.wav", 1, PR_ATTN_NORM);
   dir = normalize(self->enemy->origin - self->origin);
   self->v_angle = vectoangles(dir);
   makevectors(self->v_angle);
   dir = dir + crandom()* 0.3 *v_right + crandom()* 0.3 *v_up;
   dir = normalize(dir);
   self->v_angle = vectoangles(dir);
   FireBullets (14, dir, V({0.14, 0.08, 0}),1024);
}

/*
============
Gremlin_FireLightningGun

fire lightning gun
============
*/
void Gremlin_FireLightningGun()
   {
	vector	org, dir;
   float cells;

// explode if under water
   if (self->watertype <= PR_CONTENT_WATER)
	{
		cells = self->ammo_cells;
		self->ammo_cells = 0;
	  //discharged = 1;
		T_RadiusDamage (self, self, 35*cells, world);
	  //discharged = 0;
//		W_SetCurrentAmmo ();
		return;
	}

   //self.effects = self.effects | #EF_MUZZLEFLASH;
   MuzzleFlash(self);

	ai_face ();

   self->currentammo = self->ammo_cells = self->ammo_cells - 2;
   org = self->origin + V({0, 0, 16});

	dir = self->enemy->origin + V({0, 0, 16}) - org;
	dir = normalize (dir);
   dir = normalize(self->enemy->origin - self->origin);
   self->v_angle = vectoangles(dir);
   makevectors(self->v_angle);
   dir = dir + crandom()* 0.1 *v_right + crandom()* 0.1 *v_up;
   dir = normalize(dir);

	traceline (org, self->origin + dir*600, PR_TL_BSP_ONLY, self);

   WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
   WriteByte (PR_MSG_BROADCAST, PR_TE_LIGHTNING2);
	WriteEntity (PR_MSG_BROADCAST, self);
	WriteCoord (PR_MSG_BROADCAST, org[X]);
	WriteCoord (PR_MSG_BROADCAST, org[Y]);
	WriteCoord (PR_MSG_BROADCAST, org[Z]);
	WriteCoord (PR_MSG_BROADCAST, trace_endpos[X]);
	WriteCoord (PR_MSG_BROADCAST, trace_endpos[Y]);
	WriteCoord (PR_MSG_BROADCAST, trace_endpos[Z]);

   #ifdef PR_QUAKE_WORLD
	multicast (org, PR_MULTICAST_PHS);
   #endif

   LightningDamage (org, trace_endpos+(dir*4), self, 30);
}

/*
================
GremlinFireProximityGrenade
================
*/
/*
void() GremlinFireProximityGrenade =
{
   local entity missile;
   local vector dir;

   NumProximityGrenades = NumProximityGrenades + 1;
   self.currentammo = self.ammo_rockets = self.ammo_rockets - 1;

	sound (self, #CHAN_WEAPON, "weapons/grenade.wav", 1, #ATTN_NORM);

	missile = spawn ();
   missile.owner = self;
   missile.martyr_enemy = self;
   missile.movetype = #MOVETYPE_TOSS;
	missile.solid = #SOLID_BBOX;
   missile.classname = "proximity_grenade";
   missile.takedamage = #DAMAGE_NO;
   missile.health = 5;
   missile.state = 0;

// set missile speed

   dir = normalize(self.enemy.origin - self.origin);
   self.v_angle = vectoangles(dir);
   makevectors(self.v_angle);
   dir = dir + crandom()* 0.1 *v_right + crandom()* 0.1 *v_up;
   dir = normalize(dir);
   missile.velocity = dir * 600;
	missile.velocity_z = 200;

   missile.avelocity = '100 600 100';

	missile.angles = vectoangles(missile.velocity);

   missile.touch = ProximityGrenadeTouch;

// set missile duration
   missile.nextthink = time + 2;
   missile.delay = time + 15 + (10*random());
   missile.think = ProximityBomb;
   missile.th_die = ProximityGrenadeExplode;

   setmodel (missile, "progs/proxbomb.mdl");
   setorigin (missile, self.origin);
   setsize (missile, '-1 -1 -1', '1 1 1');
};
*/
/*
============
GremlinWeaponAttack

attack with a weapon
============
*/
void gremlin_shot1();
void gremlin_nail1();
void gremlin_light1();
void gremlin_rocket1();

float GremlinWeaponAttack()
   {
	/*local float	r;
   local entity targ;
   local vector vec;*/


   if (!GremlinCheckNoAmmo ())
	  return PR_FALSE;

   self->show_hostile = time + 1; // wake monsters up

	MonsterAuraPower();

   if (self->weapon == PR_IT_SHOTGUN)
	  {
	  gremlin_shot1 ();
	  Gremlin_FireShotGun();
	  Attack_Finished(1);
	  }
	else if (self->weapon == PR_IT_SUPER_SHOTGUN)
	  {
	  gremlin_shot1 ();
	  Gremlin_FireSuperShotGun();
	  Attack_Finished(1);
	  }
	else if (self->weapon == PR_IT_NAILGUN)
	  {
	  gremlin_nail3 ();
	  Attack_Finished(1);
	  }
	else if (self->weapon == PR_IT_SUPER_NAILGUN)
	  {
	  gremlin_nail3 ();
	  Attack_Finished(1);
	  }
	/*else if (self.weapon == #IT_GRENADE_LAUNCHER)
	  {
	  gremlin_rocket1();
	  OgreFireGrenade();
	  self.currentammo = self.ammo_rockets = self.ammo_rockets - 1;
	  Attack_Finished(1);
	  }*/
	else if (self->weapon == PR_IT_ROCKET_LAUNCHER)
	  {
	  gremlin_rocket1();
	  Gremlin_FireRocket();
	  Attack_Finished(1);
	  }
	else if (self->weapon == PR_IT_LIGHTNING)
	  {
	  gremlin_light1();
	  Attack_Finished(1);
	  sound (self, PR_CHAN_AUTO, "weapons/lstart.wav", 1, PR_ATTN_NORM);
	  }
   /*else if (self.weapon == IT_LASER_CANNON)
	  {
	  gremlin_laser3();
	  Attack_Finished(1);
	  }
   else if (self.weapon == IT_PROXIMITY_GUN)
	  {
	  gremlin_rocket1();
	  GremlinFireProximityGrenade();
	  Attack_Finished(1);
	  }*/
   return PR_TRUE;
   }

void Gremlin_MissileAttack()
   {
   if (self->has_fieldgen)
	  {
	  if (GremlinWeaponAttack())
		 return;
	  else if ((random()<0.1) && (self->flags & PR_FL_ONGROUND))
		 {
		 gremlin_jump1();
		 return;
		 }
	  }
   if (self->flags & PR_FL_ONGROUND)
	  gremlin_jump1();
   }

void Gremlin_Precache()
{
	precache_model ("progs/grem.mdl");
	precache_model ("progs/h_grem.mdl");

	precache_sound ("grem/death.wav");
	precache_sound ("grem/attack.wav");
	precache_sound ("demon/djump.wav");
	precache_sound ("demon/dhit2.wav");
	precache_sound ("grem/pain1.wav");
	precache_sound ("grem/pain2.wav");
	precache_sound ("grem/pain3.wav");
	precache_sound ("grem/idle.wav");
	precache_sound ("grem/sight1.wav");
}

/*QUAKED monster_gremlin (1 0 0) (-32 -32 -24) (32 32 64) Ambush

*/

/*
void() monster_gremlin =
{
   if (deathmatch)
	{
		remove(self);
		return;
	}
   NumGremlins = NumGremlins + 1;
   precache_model ("progs/grem.mdl");
   precache_model ("progs/h_grem.mdl");

   precache_sound ("grem/death.wav");
   precache_sound ("grem/attack.wav");
   precache_sound ("demon/djump.wav");
   precache_sound ("demon/dhit2.wav");
   precache_sound ("grem/pain1.wav");
   precache_sound ("grem/pain2.wav");
   precache_sound ("grem/pain3.wav");
   precache_sound ("grem/idle.wav");
   precache_sound ("grem/sight1.wav");

   self.solid = #SOLID_SLIDEBOX;
	self.movetype = #MOVETYPE_STEP;

   setmodel (self, "progs/grem.mdl");

   setsize (self, #VEC_HULL_MIN, #VEC_HULL_MAX);
   self.health = 100;
   self.max_health = 101;
   self.yaw_speed = 40;

   self.th_stand = gremlin_stand1;
   self.th_walk = gremlin_walk1;
   self.th_run = gremlin_run1;
   self.th_die = gremlin_die;
   self.th_melee = Gremlin_MeleeAttack; 	// one of two attacks
   self.th_missile = Gremlin_MissileAttack; // check for random jump or firing of weapon
   self.th_pain = gremlin_pain;

	walkmonster_start();
};
*/

float GremlinCheckAttack()
   {
	vector	spot1, spot2;
	entity	targ;
	float 	chance;

	targ = self->enemy;

   if (time < self->attack_finished)
	  return PR_FALSE;

// see if any entities are in the way of the shot
   spot1 = self->origin;// + self.view_ofs;
   spot2 = targ->origin;// + targ.view_ofs;

   if ((vlen(spot2 - spot1) <= 90) && (self->has_fieldgen == 0))
	  {
	  self->attack_state = PR_AS_MELEE;
	  return PR_TRUE;
	  }
// missile attack
   chance = 0.03 + self->has_fieldgen;
   if (random() < chance)
	  {
	  self->attack_state = PR_AS_MISSILE;
	  return PR_TRUE;
	  }
	return PR_FALSE;
   }


//===========================================================================

void Gremlin_Melee(float side)
{
	float	ldmg;
	vector	delta;

	ai_face ();

//	 walkmove (self.ideal_yaw, 12);   // allow a little closing


	delta = self->enemy->origin - self->origin;

	if (vlen(delta) > 100)
		return;
	if (!CanDamage (self->enemy, self))
		return;

	deathmsg = 0;

	sound (self, PR_CHAN_WEAPON, "grem/attack.wav", 1, PR_ATTN_NORM);
	ldmg = 40+40*random();//10 + 5*random();
	T_Damage (self->enemy, self, self, ldmg);

	makevectors (self->angles);
	SpawnMeatSpray (self->origin + v_forward*16, side * v_right);
}

//===========================================================================
void Gremlin_ThrowHead(float dm)
   {
   string gibname;
   /*if (self.classname == "monster_ogre")
	  gibname = "progs/h_ogre.mdl";
	else if (self.classname == "monster_knight")
	  gibname = "progs/h_knight.mdl";*/
   if (self->classname == "monster_shambler")
	  gibname = "progs/h_shams.mdl";
   else if (self->classname == "monster_demon1")
	  gibname = "progs/h_demon.mdl";
   else if (self->classname == "monster_wizard")
	  gibname = "progs/h_wizard.mdl";
   /*else if (self.classname == "monster_zombie")
	  gibname = "progs/h_zombie.mdl";
   else if (self.classname == "monster_dog")
	  gibname = "progs/h_dog.mdl";
   else if (self.classname == "monster_hell_knight")
	  gibname = "progs/h_hellkn.mdl";
   else if (self.classname == "monster_enforcer")
	  gibname = "progs/h_mega.mdl";*/
   else if (self->classname == "monster_army")
	  gibname = "progs/h_player.mdl";
   /*else if (self.classname == "monster_shalrath")
	  gibname = "progs/h_shal.mdl";*/
   else if (self->classname == "monster_gremlin")
	  gibname = "progs/h_grem.mdl";
   /*else if (self.classname == "monster_scourge")
	  gibname = "progs/h_scourg.mdl";*/
   else if (self->classname == "monster_fish")
	  gibname = "progs/gib1.mdl";
   else
	  gibname = "progs/h_player.mdl";
   ThrowMonsterHead(gibname,dm);
   }
//===========================================================================

/*
============
Gremlin_Damage

The damage is coming from inflictor, but get mad at attacker
============
*/
void Gremlin_Damage(entity targ, entity inflictor, entity attacker, float damage)
{
// check for godmode or invincibility
	if (targ->flags & PR_FL_GODMODE)
		return;
	if (targ->invincible_finished >= time)
	{
		if (self->invincible_sound < time)
		{
			sound (targ, PR_CHAN_ITEM, "items/protect3.wav", 1, PR_ATTN_NORM);
			self->invincible_sound = time + 2;
		}
		return;
	}

// team play damage avoidance
	if ( (teamplay == 1) && (targ->team > 0)&&(targ->team == attacker->team) )
		return;

// do the damage
   targ->health = targ->health - damage;
   }

void Gremlin_Split()
   {
   entity grem;
   entity temp;
   entity head;
   float done;
   vector ang;
   float c;
   vector pos;
   float proceed;

   if (NumSpawnGremlins >= (NumGremlins*2))
	  return;
   done = 0;
   c = 0;
   ang = self->angles;
   while (done == 0)
	  {
	  makevectors(ang);
	  pos = self->origin + (80 * v_forward);
	  head = findradius(pos, 35);
	  proceed = 1;
	  while (head != world)
		 {
		 if ((head->health > 0) && (head->flags & (PR_FL_MONSTER | PR_FL_CLIENT)))
			proceed = 0;
		 head = head->chain;
		 }
	  traceline(self->origin,pos,PR_TL_ANY_SOLID,self);
	  if (trace_fraction == 1 && (proceed == 1))
		 {
		 traceline(self->origin,(pos-V({40, 40, 0})),PR_TL_ANY_SOLID,self);
		 if (trace_fraction == 1)
			{
			traceline(self->origin,(pos+V({40, 40, 0})),PR_TL_ANY_SOLID,self);
			if (trace_fraction == 1)
			   {
			   traceline(self->origin,(pos + V({0, 0, 64})),PR_TL_ANY_SOLID,self);
			   if (trace_fraction == 1)
				  {
				  traceline(self->origin,(pos - V({0, 0, 64})),PR_TL_ANY_SOLID,self);
				  if (trace_fraction != 1)
					 {
					 done = 1;
					 }
				  }
			   }
			}
		 }
	  if (done == 0)
		 {
		 ang[Y] = ang[Y] + 36;
		 c = c + 1;
		 if (c==10)
			{
			return;
			}
		 }
	  }
   NumSpawnGremlins = NumSpawnGremlins + 1;
   grem = spawn();
   SUB_CopyEntity(self,grem);
   grem->solid = PR_SOLID_SLIDEBOX;
   grem->movetype = PR_MOVETYPE_STEP;
   setmodel (grem, "progs/grem.mdl");
   setsize (grem, PR_VEC_HULL_MIN, PR_VEC_HULL_MAX);
   if (self->health < 100) self->health = 100;
   grem->health = self->health / 2;
   self->health = self->health / 2;
//	 grem.max_health = 101;
//	 grem.has_camera = FALSE;
   grem->has_fieldgen = PR_FALSE;
   grem->items = 0;
   total_monsters = total_monsters + 1;
   /*WriteByte (#MSG_BROADCAST, SVC_UPDATESTAT);
   WriteByte (#MSG_BROADCAST, STAT_TOTALMONSTERS);
   WriteLong (#MSG_BROADCAST, total_monsters);*/
   setorigin(grem, pos);
   temp = self;
   self = grem;
   gremlin_spawn1();
   self->enemy = world;
   self->has_camera = PR_FALSE;
   self = temp;
   }

void Gremlin_Gorge(float side)
   {
	float	ldmg;
	vector	delta;
   entity temp;

//	 ai_face ();
//	 walkmove (self.ideal_yaw, 6);	 // allow a little closing

	delta = self->enemy->origin - self->origin;
//	 self.enemy.takedamage = DAMAGE_YES;

	if (random() < 0.5)
		sound (self, PR_CHAN_WEAPON, "demon/dhit2.wav", 1, PR_ATTN_NORM);
	else
		sound (self, PR_CHAN_WEAPON, "shambler/smack.wav", 1, PR_ATTN_NORM);

//	  sound (self, CHAN_WEAPON, "grem/attack.wav", 1, ATTN_NORM);
   ldmg = 7+5*random();
   Gremlin_Damage (self->enemy, self, self, ldmg);

	makevectors (self->angles);
	SpawnMeatSpray (self->origin + v_forward*16, side * v_right);
   if (self->enemy->health < -200.0) // -200.0
	  {
	  if (self->enemy->has_camera==PR_FALSE)
		 {
		 self->enemy->has_camera = PR_TRUE;
		 sound (self, PR_CHAN_VOICE, "player/udeath.wav", 1, PR_ATTN_NORM);
		 temp = self;
		 self = self->enemy;
		 Gremlin_ThrowHead(-15);
//		   ThrowGib ("progs/gib1.mdl", -15);
//		   ThrowGib ("progs/gib2.mdl", -15);
//		   ThrowGib ("progs/gib3.mdl", -15);
		 self = temp;
		 ldmg = 150 + 100*random();
		 T_Heal(self,ldmg,PR_FALSE);
//		   if (self.health >= self.max_health)
			Gremlin_Split();
		 }
	  self->enemy = world;
	  self->has_camera = PR_FALSE;
/*
	  if (self.has_fieldgen)
		 {
		 temp = GremlinFindVictim();
		 if (temp != world)
			{
			self.enemy = temp;
			FoundTarget();
			self.search_time = time + 1.0;
//			  return;
			}
		 }
*/
//		FindTarget();
/*
	  if (self.oldenemy.health > 0)
		 {
		 self.enemy = self.oldenemy;
		 HuntTarget ();
		 }
	  else
		 {
		 if (self.movetarget)
			self.th_walk ();
		 else
			self.th_stand ();
		 }
*/
	  gremlin_look1();
	  }
   }


void Gremlin_JumpTouch()
{
//	local	float	ldmg;

   if (self->health <= 0)
		return;

/*
	if (other.takedamage)
	{
		if ( vlen(self.velocity) > 400 )
		{
			ldmg = 40 + 10*random();
			T_Damage (other, self, self, ldmg);
		}
	}
*/
	if (!checkbottom(self))
	  {
		if (self->flags & PR_FL_ONGROUND)
		 {
		 //self.touch = SUB_Null;
			 self->touch = MonsterTouch;
		 self->think = gremlin_jump1;
		 self->nextthink = time + 0.1;
		 }
		return; // not on ground yet
	  }

	//self.touch = SUB_Null;
		self->touch = MonsterTouch;
   self->think = gremlin_jump12;
	self->nextthink = time + 0.1;
}

void Gremlin_FlipTouch()
{
//	local	float	ldmg;

	if (!checkbottom(self))
	  {
		if (self->flags & PR_FL_ONGROUND)
		 {
		 //self.touch = SUB_Null;
			 self->touch = MonsterTouch;
		 self->think = gremlin_flip1;
		 self->nextthink = time + 0.1;
		 }
		return; // not on ground yet
	  }

	//self.touch = SUB_Null;
	self->touch = MonsterTouch;
   self->think = gremlin_flip8;
	self->nextthink = time + 0.1;
}

//=========================
//
// Hipnotic Sub-routines
//
//=========================


//=========================
//
// SUB_CopyEntity
//
//=========================

void SUB_CopyEntity(entity srcent, entity destent)
{
   destent->modelindex=srcent->modelindex;
   destent->absmin=srcent->absmin;
   destent->absmax=srcent->absmax;
   destent->ltime=srcent->ltime;
   destent->movetype=srcent->movetype;
   destent->solid=srcent->solid;
   destent->origin=srcent->origin;
   destent->oldorigin=srcent->oldorigin;
   destent->velocity=srcent->velocity;
   destent->angles=srcent->angles;
   destent->avelocity=srcent->avelocity;
   //destent.punchangle=srcent.punchangle;
   destent->classname=srcent->classname;
   destent->model=srcent->model;
   destent->frame=srcent->frame;
   destent->skin=srcent->skin;
   destent->effects=srcent->effects;
   destent->mins=srcent->mins;
   destent->maxs=srcent->maxs;
   destent->size=srcent->size;
   destent->touch=srcent->touch;
   destent->use=srcent->use;
   destent->think=srcent->think;
   destent->blocked=srcent->blocked;
   destent->nextthink=srcent->nextthink;
   destent->groundentity=srcent->groundentity;
   destent->health=srcent->health;
   destent->frags=srcent->frags;
   destent->weapon=srcent->weapon;
   destent->weaponmodel=srcent->weaponmodel;
   destent->weaponframe=srcent->weaponframe;
   destent->currentammo=srcent->currentammo;
   destent->ammo_shells=srcent->ammo_shells;
   destent->ammo_nails=srcent->ammo_nails;
   destent->ammo_rockets=srcent->ammo_rockets;
   destent->ammo_cells=srcent->ammo_cells;
   destent->items=srcent->items;
   destent->takedamage=srcent->takedamage;
   destent->chain=srcent->chain;
   destent->deadflag=srcent->deadflag;
   destent->view_ofs=srcent->view_ofs;
   destent->PR_BUTTON_FIRE=srcent->PR_BUTTON_FIRE;
   destent->PR_BUTTON_USE=srcent->PR_BUTTON_USE;
   destent->PR_BUTTON_JUMP=srcent->PR_BUTTON_JUMP;
   destent->impulse=srcent->impulse;
   destent->fixangle=srcent->fixangle;
   destent->v_angle=srcent->v_angle;
   //destent.idealpitch=srcent.idealpitch;
   destent->netname=srcent->netname;
   destent->enemy=srcent->enemy;
   destent->flags=srcent->flags;
   destent->colormap=srcent->colormap;
   destent->team=srcent->team;
   destent->max_health=srcent->max_health;
   destent->teleport_time=srcent->teleport_time;
   destent->armortype=srcent->armortype;
   destent->armorvalue=srcent->armorvalue;
   destent->waterlevel=srcent->waterlevel;
   destent->watertype=srcent->watertype;
   destent->ideal_yaw=srcent->ideal_yaw;
   destent->yaw_speed=srcent->yaw_speed;
   destent->aiment=srcent->aiment;
   destent->goalentity=srcent->goalentity;
   destent->spawnflags=srcent->spawnflags;
   destent->target=srcent->target;
   destent->targetname=srcent->targetname;
   destent->dmg_take=srcent->dmg_take;
   destent->dmg_save=srcent->dmg_save;
   destent->dmg_inflictor=srcent->dmg_inflictor;
   destent->owner=srcent->owner;
   destent->movedir=srcent->movedir;
   destent->message=srcent->message;
   destent->sounds=srcent->sounds;
   destent->noise=srcent->noise;
   destent->noise1=srcent->noise1;
   destent->noise2=srcent->noise2;
   destent->noise3=srcent->noise3;
   destent->wad=srcent->wad;
   destent->map=srcent->map;
   destent->worldtype=srcent->worldtype;
   destent->killtarget=srcent->killtarget;
   destent->light_lev=srcent->light_lev;
   destent->style=srcent->style;
   destent->th_stand=srcent->th_stand;
   destent->th_walk=srcent->th_walk;
   destent->th_run=srcent->th_run;
   destent->th_missile=srcent->th_missile;
   destent->th_melee=srcent->th_melee;
   destent->th_pain=srcent->th_pain;
   destent->th_die=srcent->th_die;
   //destent.th_turn=srcent.th_turn;
   destent->oldenemy=srcent->oldenemy;
   destent->speed=srcent->speed;
   destent->lefty=srcent->lefty;
   destent->search_time=srcent->search_time;
   destent->attack_state=srcent->attack_state;
   destent->walkframe=srcent->walkframe;
   destent->attack_finished=srcent->attack_finished;
   destent->pain_finished=srcent->pain_finished;
   destent->invincible_finished=srcent->invincible_finished;
   destent->invisible_finished=srcent->invisible_finished;
   destent->super_damage_finished=srcent->super_damage_finished;
   destent->radsuit_finished=srcent->radsuit_finished;
   destent->invincible_time=srcent->invincible_time;
   destent->invincible_sound=srcent->invincible_sound;
   destent->invisible_time=srcent->invisible_time;
   destent->invisible_sound=srcent->invisible_sound;
   destent->super_time=srcent->super_time;
   destent->super_sound=srcent->super_sound;
   destent->rad_time=srcent->rad_time;
   destent->fly_sound=srcent->fly_sound;
   destent->axhitme=srcent->axhitme;
   destent->show_hostile=srcent->show_hostile;
   destent->timeLastJumped = srcent->timeLastJumped;
   destent->swim_flag=srcent->swim_flag;
   destent->air_finished=srcent->air_finished;
   destent->bubble_count=srcent->bubble_count;
   destent->deathtype=srcent->deathtype;
   destent->mdl=srcent->mdl;
   destent->mangle=srcent->mangle;
   destent->oldorigin=srcent->oldorigin;
   destent->t_length=srcent->t_length;
   destent->t_width=srcent->t_width;
   destent->dest=srcent->dest;
   destent->dest1=srcent->dest1;
   destent->dest2=srcent->dest2;
   destent->wait=srcent->wait;
   destent->delay=srcent->delay;
   destent->trigger_field=srcent->trigger_field;
   destent->noise4=srcent->noise4;
   destent->pausetime=srcent->pausetime;
   destent->movetarget=srcent->movetarget;
   destent->aflag=srcent->aflag;
   destent->dmg=srcent->dmg;
   destent->cnt=srcent->cnt;
   destent->think1=srcent->think1;
   destent->finaldest=srcent->finaldest;
   destent->finalangle=srcent->finalangle;
   destent->count=srcent->count;
   destent->lip=srcent->lip;
   destent->state=srcent->state;
   destent->pos1=srcent->pos1;
   destent->pos2=srcent->pos2;
   destent->height=srcent->height;
   destent->waitmin=srcent->waitmin;
   destent->waitmax=srcent->waitmax;
   destent->distance=srcent->distance;
   destent->volume=srcent->volume;
   destent->has_camera=srcent->has_camera;
   destent->has_fieldgen=srcent->has_fieldgen;

   /*destent.spawnfunction=srcent.spawnfunction;
   destent.spawnthink=srcent.spawnthink;
   destent.spawnmodel=srcent.spawnmodel;
   destent.spawnsolidtype=srcent.spawnsolidtype;
   destent.spawnmins=srcent.spawnmins;
   destent.spawnmaxs=srcent.spawnmaxs;
   destent.spawnclassname=srcent.spawnclassname;*/
}

// OfN - Frame stuff added

void gremlin_gstand1() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK1, gremlin_gstand2); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand2() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK2, gremlin_gstand3); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand3() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK3, gremlin_gstand4); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand4() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK4, gremlin_gstand5); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand5() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK5, gremlin_gstand6); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand6() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK6, gremlin_gstand7); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand7() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK7, gremlin_gstand8); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand8() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK8, gremlin_gstand9); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand9() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK9, gremlin_gstand10); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand10() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK10, gremlin_gstand11); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand11() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK11, gremlin_gstand12); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand12() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK12, gremlin_gstand13); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand13() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK13, gremlin_gstand14); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand14() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK14, gremlin_gstand15); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand15() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK15, gremlin_gstand16); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand16() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK16, gremlin_gstand17); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand17() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK17, gremlin_gstand18); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand18() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK18, gremlin_gstand19); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand19() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK19, gremlin_gstand20); gremlin_stand();self->nextthink = time + 0.15;}
void gremlin_gstand20() {FRAME_STATE(CuTF_GremlinFrames::FR_GLOOK20, gremlin_gstand1); gremlin_stand();self->nextthink = time + 0.15;}

void gremlin_gwalk1() {
FRAME_STATE(CuTF_GremlinFrames::FR_GRUN1, gremlin_gwalk2);
if (random() < 0.1)
	sound (self, PR_CHAN_VOICE, "grem/idle.wav", 1, PR_ATTN_IDLE);
gremlin_walk(0);}
void gremlin_gwalk2() {FRAME_STATE(CuTF_GremlinFrames::FR_GRUN2, gremlin_gwalk3); gremlin_walk(8);} // original 8
void gremlin_gwalk3() {FRAME_STATE(CuTF_GremlinFrames::FR_GRUN3, gremlin_gwalk4); gremlin_walk(12);} // original 12
void gremlin_gwalk4() {FRAME_STATE(CuTF_GremlinFrames::FR_GRUN4, gremlin_gwalk5); gremlin_walk(16);} // original 16
void gremlin_gwalk5() {FRAME_STATE(CuTF_GremlinFrames::FR_GRUN5, gremlin_gwalk6); gremlin_walk(16);} // original 16
void gremlin_gwalk6() {FRAME_STATE(CuTF_GremlinFrames::FR_GRUN6, gremlin_gwalk7); gremlin_walk(12);} // original 12
void gremlin_gwalk7() {FRAME_STATE(CuTF_GremlinFrames::FR_GRUN7, gremlin_gwalk8); gremlin_walk(8);} // original 8
void gremlin_gwalk8() {FRAME_STATE(CuTF_GremlinFrames::FR_GRUN8, gremlin_gwalk9); gremlin_walk(0);} // original 0
void gremlin_gwalk9() {FRAME_STATE(CuTF_GremlinFrames::FR_GRUN9, gremlin_gwalk10); gremlin_walk(8);} // original 8
void gremlin_gwalk10() {FRAME_STATE(CuTF_GremlinFrames::FR_GRUN10, gremlin_gwalk11); gremlin_walk(12);} // original 12
void gremlin_gwalk11() {FRAME_STATE(CuTF_GremlinFrames::FR_GRUN11, gremlin_gwalk12); gremlin_walk(16);} // original 16
void gremlin_gwalk12() {FRAME_STATE(CuTF_GremlinFrames::FR_GRUN12, gremlin_gwalk13); gremlin_walk(16);} // original 16
void gremlin_gwalk13() {FRAME_STATE(CuTF_GremlinFrames::FR_GRUN13, gremlin_gwalk14); gremlin_walk(12);} // original 12
void gremlin_gwalk14() {FRAME_STATE(CuTF_GremlinFrames::FR_GRUN14, gremlin_gwalk15); gremlin_walk(8);} // original 8
void gremlin_gwalk15() {FRAME_STATE(CuTF_GremlinFrames::FR_GRUN15, gremlin_gwalk1); gremlin_walk(0);} // original 0

//================================================================
// Stand and walk subs to route to correct animations

void gremlin_main_stand()
{
	if (self->has_fieldgen)
		gremlin_gstand1();
	else
		gremlin_stand1();
}

void gremlin_main_walk()
{
	if (self->has_fieldgen)
		gremlin_gwalk1();
	else
		gremlin_walk1();
}

//===========================================================================
// Returns the best possible weapon to steal from a player

float GREM_BestWeaponToSteal(entity gremlin, entity player)
{
	// Rocket Launcher
	if (player->weapons_carried & PR_WEAP_ROCKET_LAUNCHER && !(gremlin->weapons_carried & PR_WEAP_ROCKET_LAUNCHER))
		return PR_WEAP_ROCKET_LAUNCHER;
	// Lightning gun
	if (player->weapons_carried & PR_WEAP_LIGHTNING && !(gremlin->weapons_carried & PR_WEAP_LIGHTNING))
		return PR_WEAP_LIGHTNING;
	// Super Shotgun
	if (player->weapons_carried & PR_WEAP_SUPER_SHOTGUN && !(gremlin->weapons_carried & PR_WEAP_SUPER_SHOTGUN))
		return PR_WEAP_SUPER_SHOTGUN;
	// Shotgun
	if (player->weapons_carried & PR_WEAP_SHOTGUN && !(gremlin->weapons_carried & PR_WEAP_SHOTGUN))
		return PR_WEAP_SHOTGUN;
	// Super Nail Gun
	if (player->weapons_carried & PR_WEAP_SNG && !(gremlin->weapons_carried & PR_WEAP_SNG))
		return PR_WEAP_SNG;
	// Nailgun
	if (player->weapons_carried & PR_WEAP_NAILGUN && !(gremlin->weapons_carried & PR_WEAP_NAILGUN))
		return PR_WEAP_NAILGUN;

	return 0;
}

} // END namespace Progs
