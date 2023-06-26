#include "progs.h"
#include "debug.h"

namespace Progs {

void DoGoalWork(entity Goal, entity AP);
void DoGroupWork(entity Goal, entity AP);

void SUB_Null() {}
void SUB_PainNull(entity attacker, float damage) {}

void SUB_Remove() {dremove(self);}

/*
QuakeEd only writes a single float for angles (bad idea), so up and down are
just constant angles.
*/
void SetMovedir()  // PZ: This did have return type `vector`, but it wasn't returning anything.
{
	if (self->angles == V({0, -1, 0}))
		self->movedir = V({0, 0, 1});
	else if (self->angles == V({0, -2, 0}))
		self->movedir = V({0, 0, -1});
	else
	{
		makevectors (self->angles);
		self->movedir = v_forward;
	}

	self->angles = V({0, 0, 0});
}

/*
================
InitTrigger
================
*/
void InitTrigger()
{
// trigger angles are used for one-way touches.  An angle of 0 is assumed
// to mean no restrictions, so use a yaw of 360 instead.
	if (self->angles != V({0, 0, 0}))
		SetMovedir ();
	self->solid = PR_SOLID_TRIGGER;
	setmodel (self, self->model);	// set size and link into world
	self->movetype = PR_MOVETYPE_NONE;
	self->modelindex = 0;
	self->model = "";
}

/*
=============
SUB_CalcMove

calculate self.velocity and self.nextthink to reach dest from
self.origin traveling at speed
===============
*/
void SUB_CalcMoveEnt(entity ent, const vector& tdest, float tspeed, void (*func)())
{
entity	stemp;
	stemp = self;
	self = ent;

	SUB_CalcMove (tdest, tspeed, func);
	self = stemp;
}

void SUB_CalcMove(const vector& tdest, float tspeed, void (*func)())
{
vector	vdestdelta;
float		len, traveltime;

	if (!tspeed)
		objerror("No speed is defined!");

	self->think1 = func;
	self->finaldest = tdest;
	self->think = SUB_CalcMoveDone;

	if (tdest == self->origin)
	{
		self->velocity = V({0, 0, 0});
		self->nextthink = self->ltime + 0.1;
		return;
	}

// set destdelta to the vector needed to move
	vdestdelta = tdest - self->origin;

// calculate length of vector
	len = vlen (vdestdelta);

// divide by speed to get time to reach dest
	traveltime = len / tspeed;

#ifndef PR_QUAKE_WORLD
	if (traveltime < 0.1)
	{
		self->velocity = V({0, 0, 0});
		self->nextthink = self->ltime + 0.1;
		return;
	}
#else
	if (traveltime < 0.03)
		traveltime = 0.03;
#endif

// set nextthink to trigger a think when dest is reached
	self->nextthink = self->ltime + traveltime;

// scale the destdelta vector by the time spent traveling to get velocity
	self->velocity = vdestdelta * (1/traveltime);	// qcc won't take vec/float
}

/*
============
After moving, set origin to exact final destination
============
*/
void SUB_CalcMoveDone()
{
	setorigin(self, self->finaldest);
	self->velocity = V({0, 0, 0});
	self->nextthink = -1;
	if (self->think1 != SUB_Null)
		self->think1();
}

/*
=============
SUB_CalcAngleMove

calculate self.avelocity and self.nextthink to reach destangle from
self.angles rotating

The calling function should make sure self.think is valid
===============
*/
void SUB_CalcAngleMoveEnt(entity ent, const vector& destangle, float tspeed, void (*func)())
{
entity		stemp;
	stemp = self;
	self = ent;
	SUB_CalcAngleMove (destangle, tspeed, func);
	self = stemp;
}

void SUB_CalcAngleMove(const vector& destangle, float tspeed, void (*func)())
{
vector	destdelta;
float		len, traveltime;

	if (!tspeed)
		objerror("No speed is defined!");

// set destdelta to the vector needed to move
	destdelta = destangle - self->angles;

// calculate length of vector
	len = vlen (destdelta);

// divide by speed to get time to reach dest
	traveltime = len / tspeed;

// set nextthink to trigger a think when dest is reached
	self->nextthink = self->ltime + traveltime;

// scale the destdelta vector by the time spent traveling to get velocity
	self->avelocity = destdelta * (1 / traveltime);

	self->think1 = func;
	self->finalangle = destangle;
	self->think = SUB_CalcAngleMoveDone;
}

/*
============
After rotating, set angle to exact final angle
============
*/
void SUB_CalcAngleMoveDone()
{
	self->angles = self->finalangle;
	self->avelocity = V({0, 0, 0});
	self->nextthink = -1;
	if (self->think1 != SUB_Null)
		self->think1();
}


//=============================================================================

void DelayThink()
{
	activator = self->enemy;
	SUB_UseTargets ();
	dremove(self);
}

/*
==============================
SUB_UseTargets

the global "activator" should be set to the entity that initiated the firing.

If self.delay is set, a DelayedUse entity will be created that will actually
do the SUB_UseTargets after that many seconds have passed.

Centerprints any self.message to the activator.

Removes all entities with a targetname that match self.killtarget,
and removes them, so some events can remove other triggers.

Search for (string)targetname in all entities that
match (string)self.target and call their .use function

==============================
*/
void SUB_UseTargets()
{
	entity t, stemp, otemp, act;

	// If the trigger's been activated by a spanner, then it'll
	// have it's trigger-behaviour handled by the goal code,
	// so get out of here.
	if (self->dont_do_triggerwork)
	{
		self->dont_do_triggerwork = PR_FALSE;
		return;
	}

	// check for a delay
	if (self->delay)
	{
		// create a temp object to fire at a later time
		t = spawn(); // PZ: this could probably be a server-side entity, but I'm not entirely sure; so I'm letting it stay a client-visible entity
		t->classname = "DelayedUse";
		t->nextthink = time + self->delay;
		t->think = DelayThink;
		t->enemy = activator;
		t->message = self->message;
		t->killtarget = self->killtarget;
		t->target = self->target;
		return;
	}

	// print the message
	if (activator->classname == "player" && self->message != "")
	{
		CenterPrint (activator, self->message);
		if (self->noise == "")
			sound (activator, PR_CHAN_VOICE, "misc/talk.wav", 1, PR_ATTN_NORM);
	}

	// TeamFortress Goal Checking
	if (activator->classname == "player")
	{
		// Do Goal Group checking
		DoGroupWork(self, activator);

		// Do Goal checking
		DoGoalWork(self, activator);
	}

	// kill the killtagets
	if (self->killtarget != "")
	{
		t = world;
		do
		{
			t = find (t, "targetname", self->killtarget);
			if (t == world)
				return;
			remove (t);
		} while ( 1 );
	}

	// fire targets
	if (self->target != "")
	{
		act = activator;
		t = world;
		do
		{
			t = find (t, "targetname", self->target);
			if (t == world)
			{
				return;
			}
			stemp = self;
			otemp = other;
			self = t;
			other = stemp;
			if (self->use != SUB_Null)
			{
				if (self->use != SUB_Null)
					self->use ();
			}
			self = stemp;
			other = otemp;
			activator = act;
		} while ( 1 );
	}
}


/*

in nightmare mode, all attack_finished times become 0
some monsters refire twice automatically

*/

/* OfN Unused, we use Attack_Finished() void(float normal) SUB_AttackFinished =
{
	self.cnt = 0;		// refire count for nightmare
	if (skill < 3)
		self.attack_finished = time + normal;
};*/

float visible(entity targ);

void SUB_CheckRefire(void (*thinkst)())
{
	if (skill < 3)
		return;
	if (self->cnt == 1)
		return;
	if (!visible (self->enemy))
		return;
	self->cnt = 1;
	self->think = thinkst;
}

} // END namespace Progs