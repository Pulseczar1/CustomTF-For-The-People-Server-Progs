// PZ NOTE: File handles both vertical platforms -- classname "plat" -- and trains (horizontal platforms) -- classname "train".
//          Not sure why Shaka (Bill Kearney) put his initials here.
//WK

#include "progs.h"
#include "tfortmap.h"
#include "debug.h"

namespace Progs {

float IsBuilding(entity foo);

void plat_center_touch();
void plat_outside_touch();
void plat_trigger_use();
void plat_go_up();
void plat_go_down();
void plat_crush();
#define PR_PLAT_LOW_TRIGGER	1

void plat_spawn_inside_trigger()
{
	entity	trigger;
	vector	tmin, tmax;

	//
	// middle trigger
	//
	trigger = spawn();  // PZ: this might could be a server-side only entity, but can't tell on quick inspection
	trigger->touch = plat_center_touch;
	trigger->movetype = PR_MOVETYPE_NONE;
	trigger->solid = PR_SOLID_TRIGGER;
	trigger->enemy = self;

	// TeamFortress goal code
	trigger->team_no = self->team_no;
	trigger->playerclass = self->playerclass;
	trigger->items_allowed = self->items_allowed;
	trigger->activate_goal_no = self->activate_goal_no;
	trigger->inactivate_goal_no = self->inactivate_goal_no;
	trigger->remove_goal_no = self->remove_goal_no;
	trigger->restore_goal_no = self->restore_goal_no;
	trigger->activate_group_no =	self->activate_group_no;
	trigger->inactivate_group_no = self->inactivate_group_no;
	trigger->remove_group_no	= self->remove_group_no;
	trigger->restore_group_no = self->restore_group_no;
	trigger->goal_activation = self->goal_activation;
	trigger->goal_effects = self->goal_effects;
	trigger->goal_result = self->goal_result;
	trigger->goal_group = self->goal_group;

	tmin = self->mins + V({25, 25, 0});
	tmax = self->maxs - V({25, 25, -8});
	tmin[Z] = tmax[Z] - (self->pos1[Z] - self->pos2[Z] + 8);
	if (self->spawnflags & PR_PLAT_LOW_TRIGGER)
		tmax[Z] = tmin[Z] + 8;

	if (self->size[X] <= 50)
	{
		tmin[X] = (self->mins[X] + self->maxs[X]) / 2;
		tmax[X] = tmin[X] + 1;
	}
	if (self->size[Y] <= 50)
	{
		tmin[Y] = (self->mins[Y] + self->maxs[Y]) / 2;
		tmax[Y] = tmin[Y] + 1;
	}

	setsize (trigger, tmin, tmax);
}

void plat_hit_top()
{
	sound (self, PR_CHAN_NO_PHS_ADD + PR_CHAN_VOICE, self->noise1, 1, PR_ATTN_NORM);
	self->state = PR_STATE_TOP;
	self->think = plat_go_down;
	self->nextthink = self->ltime + 3;
}

void plat_hit_bottom()
{
	sound (self, PR_CHAN_NO_PHS_ADD + PR_CHAN_VOICE, self->noise1, 1, PR_ATTN_NORM);
	self->state = PR_STATE_BOTTOM;
}

void plat_go_down()
{
	sound (self, PR_CHAN_VOICE, self->noise, 1, PR_ATTN_NORM);
	self->state = PR_STATE_DOWN;
	SUB_CalcMove (self->pos2, self->speed, plat_hit_bottom);
}

void plat_go_up()
{
	sound (self, PR_CHAN_VOICE, self->noise, 1, PR_ATTN_NORM);
	self->state = PR_STATE_UP;
	SUB_CalcMove (self->pos1, self->speed, plat_hit_top);
}

void plat_center_touch()
{
	entity te;

	if (other->classname != "player")
		return;

	if (!Activated(self,other))
	{
		// If an else goal should be activated, activate it
		if (self->else_goal != 0)
		{
			te = Findgoal(self->else_goal);
			if (te != world)
				DoResults(te, other, (self->goal_result & PR_TFGR_ADD_BONUSES));
		}

		return;
	}

	if (other->health <= 0)
		return;

	self = self->enemy;
	if (self->state == PR_STATE_BOTTOM)
		plat_go_up ();
	else if (self->state == PR_STATE_TOP)
		self->nextthink = self->ltime + 1;	// delay going down
}

void plat_outside_touch()
{
	entity te;

	if (other->classname != "player")
		return;

	if (!Activated(self,other))
	{
		// If an else goal should be activated, activate it
		if (self->else_goal != 0)
		{
			te = Findgoal(self->else_goal);
			if (te != world)
				DoResults(te, other, (self->goal_result & PR_TFGR_ADD_BONUSES));
		}

		return;
	}

	if (other->health <= 0)
		return;

//RPrint ("plat_outside_touch\n");
	self = self->enemy;
	if (self->state == PR_STATE_TOP)
		plat_go_down ();
}

void plat_trigger_use()
{
	if (self->think != SUB_Null)
		return;		// allready activated
	plat_go_down();
}


void plat_crush()
{
//RPrint ("plat_crush\n");
	if (other->takedamage) {
		if (IsBuilding(other))
			T_Damage (other, self, self, other->health + 1);
		else
			T_Damage (other, self, self, 1);
	}
	if (other->classname == "detpack") {
		dremove(other->oldenemy);	// remove count down
		dremove(other);        	// remove detpack
	}
	if (self->state == PR_STATE_UP)
		plat_go_down ();
	else if (self->state == PR_STATE_DOWN)
		plat_go_up ();
	else
		objerror ("plat_crush: bad self.state\n");
}

void plat_use()
{
	self->use = SUB_Null;
	if (self->state != PR_STATE_UP)
		objerror ("plat_use: not in up state");
	plat_go_down();
}


/*QUAKED func_plat (0 .5 .8) ? #PLAT_LOW_TRIGGER
speed	default 150

Plats are always drawn in the extended position, so they will light correctly.

If the plat is the target of another trigger or button, it will start out disabled in the extended position until it is trigger, when it will lower and become a normal plat.

If the "height" key is set, that will determine the amount the plat moves, instead of being implicitly determined by the model's height.
Set "sounds" to one of the following:
1) base fast
2) chain slow
*/


void func_plat()

{
//local entity t;

	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	if (!self->t_length)
		self->t_length = 80;
	if (!self->t_width)
		self->t_width = 10;

	if (self->sounds == 0)
		self->sounds = 2;
// FIX THIS TO LOAD A GENERIC PLAT SOUND

	if (self->sounds == 1)
	{
		precache_sound ("plats/plat1.wav");
		precache_sound ("plats/plat2.wav");
		self->noise = "plats/plat1.wav";
		self->noise1 = "plats/plat2.wav";
	}

	if (self->sounds == 2)
	{
		precache_sound ("plats/medplat1.wav");
		precache_sound ("plats/medplat2.wav");
		self->noise = "plats/medplat1.wav";
		self->noise1 = "plats/medplat2.wav";
	}


	self->mangle = self->angles;
	self->angles = V({0, 0, 0});

	self->classname = "plat";
	self->solid = PR_SOLID_BSP;
	self->movetype = PR_MOVETYPE_PUSH;
	setorigin (self, self->origin);
	setmodel (self, self->model);
	setsize (self, self->mins , self->maxs);

	self->blocked = plat_crush;
	if (!self->speed)
		self->speed = 150;

// pos1 is the top position, pos2 is the bottom
	self->pos1 = self->origin;
	self->pos2 = self->origin;
	if (self->height)
		self->pos2[Z] = self->origin[Z] - self->height;
	else
		self->pos2[Z] = self->origin[Z] - self->size[Z] + 8;

	self->use = plat_trigger_use;

	plat_spawn_inside_trigger ();	// the "start moving" trigger

	if (self->targetname != "")
	{
		self->state = PR_STATE_UP;
		self->use = plat_use;
	}
	else
	{
		setorigin (self, self->pos2);
		self->state = PR_STATE_BOTTOM;
	}
}

//============================================================================

void train_next();
void func_train_find();

void train_blocked()
{
	if (time < self->attack_finished)
		return;
	self->attack_finished = time + 0.5;
	T_Damage (other, self, self, self->dmg);
}
void train_use()
{
	if (self->think != func_train_find)
		return;		// already activated
	train_next();
}

void train_wait()
{
	if (self->wait)
	{
		self->nextthink = self->ltime + self->wait;
		sound (self, PR_CHAN_NO_PHS_ADD + PR_CHAN_VOICE, self->noise, 1, PR_ATTN_NORM);
	}
	else
		self->nextthink = self->ltime + 0.1;

	self->think = train_next;
}

void train_next()
{
	entity	targ;

	targ = find (world, "targetname", self->target);
	self->target = targ->target;
	if (self->target == "")
		objerror ("train_next: no next target");
	if (targ->wait)
		self->wait = targ->wait;
	else
		self->wait = 0;
	sound (self, PR_CHAN_VOICE, self->noise1, 1, PR_ATTN_NORM);
	SUB_CalcMove (targ->origin - self->mins, self->speed, train_wait);
}

void func_train_find()
{
	entity	targ;

	targ = find (world, "targetname", self->target);
	self->target = targ->target;
	setorigin (self, targ->origin - self->mins);
	if (self->targetname == "")
	{	// not triggered, so start immediately
		self->nextthink = self->ltime + 0.1;
		self->think = train_next;
	}
}

/*QUAKED func_train (0 .5 .8) ?
Trains are moving platforms that players can ride.
The targets origin specifies the min point of the train at each corner.
The train spawns at the first target it is pointing at.
If the train is the target of a button or trigger, it will not begin moving until activated.
speed	default 100
dmg		default	2
sounds
1) ratchet metal

*/
void func_train()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	if (!self->speed)
		self->speed = 100;
	if (self->target == "")
		objerror ("func_train without a target");
	if (!self->dmg)
		self->dmg = 2;

	if (self->sounds == 0)
	{
		self->noise = ("misc/null.wav");
		precache_sound ("misc/null.wav");
		self->noise1 = ("misc/null.wav");
		precache_sound ("misc/null.wav");
	}

	if (self->sounds == 1)
	{
		self->noise = ("plats/train2.wav");
		precache_sound ("plats/train2.wav");
		self->noise1 = ("plats/train1.wav");
		precache_sound ("plats/train1.wav");
	}

	self->cnt = 1;
	self->solid = PR_SOLID_BSP;
	self->movetype = PR_MOVETYPE_PUSH;
	self->blocked = train_blocked;
	self->use = train_use;
	self->classname = "train";

	setmodel (self, self->model);
	setsize (self, self->mins , self->maxs);
	setorigin (self, self->origin);

// start trains on the second frame, to make sure their targets have had
// a chance to spawn
	self->nextthink = self->ltime + 0.1;
	self->think = func_train_find;
}

/*QUAKED misc_teleporttrain (0 .5 .8) (-8 -8 -8) (8 8 8)
This is used for the final bos
*/
void misc_teleporttrain()
{
	if (CheckExistence() == PR_FALSE)
	{
		dremove(self);
		return;
	}

	if (!self->speed)
		self->speed = 100;
	if (self->target == "")
		objerror ("func_train without a target");

	self->cnt = 1;
	self->solid = PR_SOLID_NOT;
	self->movetype = PR_MOVETYPE_PUSH;
	self->blocked = train_blocked;
	self->use = train_use;
	self->avelocity = V({100, 200, 300});

	self->noise = ("misc/null.wav");
	precache_sound ("misc/null.wav");
	self->noise1 = ("misc/null.wav");
	precache_sound ("misc/null.wav");

	precache_model2 ("progs/teleport.mdl");
	setmodel (self, "progs/teleport.mdl");
#ifndef PR_COOP_MODE_ENHANCED
	setsize (self, self->mins , self->maxs);
#else
	setsize (self, V({0, 0, 0}), V({0, 0, 0}));
#endif
	setorigin (self, self->origin);

// start trains on the second frame, to make sure their targets have had
// a chance to spawn
	self->nextthink = self->ltime + 0.1;
	self->think = func_train_find;
}

} // END namespace Progs
