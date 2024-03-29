/*============================
	coop.qc

This file handles all the
cooperative mode functions
============================*/

#include "progs.h"
#include "items.h"

namespace Progs {

void DroppedKeyThink()
{
	// let the throwing player pick it up again
	self->think = SUB_Null;
	self->touch = key_touch;
	self->owner = world;
}

void DropKey()
{
	if ((self->items & PR_IT_KEY1) || (self->items & PR_IT_KEY2))
	{
		newmis = spawn();

		if (self->items & PR_IT_KEY1)
		{
			self->items = self->items - (self->items & PR_IT_KEY1);
			newmis->items = PR_IT_KEY1;
			if (world->worldtype == 0)
			{
				setmodel (newmis, "progs/w_s_key.mdl");
				newmis->netname = "silver key";
				newmis->noise = "misc/medkey.wav";
			}
			else if (world->worldtype == 1)
			{
				setmodel (newmis, "progs/m_s_key.mdl");
				newmis->netname = "silver runekey";
				newmis->noise = "misc/runekey.wav";
			}
			else if (world->worldtype == 2)
			{
				setmodel (newmis, "progs/b_s_key.mdl");
				newmis->netname = "silver keycard";
				newmis->noise = "misc/basekey.wav";
			}
		}
		else if (self->items & PR_IT_KEY2)
		{
			self->items = self->items - (self->items & PR_IT_KEY2);
			newmis->items = PR_IT_KEY2;
			if (world->worldtype == 0)
			{
				setmodel (newmis, "progs/w_g_key.mdl");
				newmis->netname = "gold key";
				newmis->noise = "misc/medkey.wav";
			}
			else if (world->worldtype == 1)
			{
				setmodel (newmis, "progs/m_g_key.mdl");
				newmis->netname = "gold runekey";
				newmis->noise = "misc/runekey.wav";
			}
			else if (world->worldtype == 2)
			{
				setmodel (newmis, "progs/b_g_key.mdl");
				newmis->netname = "gold keycard";
				newmis->noise = "misc/basekey.wav";
			}
		}

		newmis->owner = self;
		newmis->touch = SUB_Null;
		setorigin(newmis, self->origin + V({0, 0, 16}));
		makevectors(self->v_angle);
		newmis->velocity = normalize(v_forward) * 300 + V({0, 0, 200});
		newmis->movetype = PR_MOVETYPE_TOSS;
		newmis->solid = PR_SOLID_TRIGGER;

		newmis->deadflag = PR_TRUE;
		setsize (newmis, V({-16, -16, -24}), V({16, 16, 32}));
		newmis->think = DroppedKeyThink;
		newmis->nextthink = time + 1.5;
	}
	else
	{
		sprint (self, PR_PRINT_HIGH, "You don't have a key\n");
	}
}

/*==================================
	DoorShouldOpen

This function is only for key doors in
coop mode 2.

It returns true	if all players have keyed
the door.
==================================*/

float DoorShouldOpen()
{
	entity ptr;
	float plyrcount;
	entity plyr1 = world;
	entity plyr2 = world;

	if (coop != 2)
		return PR_TRUE;

	// Gizmo - although this might show up as a warning, it's impossible for plyr1 and plyr2
	// to be used without being initialized, if you look at the below code, you'll see why
//	plyr1 = plyr2 = world;

	plyrcount = 0;
	ptr = find(world, "classname", "player");
	while (ptr != world)
	{
		if (!(ptr->tf_items & self->items) && ptr->playerclass != PR_PC_UNDEFINED
										 && ptr->solid != PR_SOLID_NOT
										 && ptr->model != string_null)
		{
			plyrcount = plyrcount + 1;
			if (plyrcount == 1)
				plyr1 = ptr;
			else if(plyrcount == 2)
				plyr2 = ptr;
		}

		ptr = find(ptr, "classname", "player");
	}

	if (plyrcount != 0)
	{
		if (plyrcount == 1)
		{
			bprint(PR_PRINT_HIGH, plyr1->netname);
			bprint(PR_PRINT_HIGH, " needs");
		}
		else if (plyrcount == 2)
		{
			bprint(PR_PRINT_HIGH, plyr1->netname);
			bprint(PR_PRINT_HIGH, " and ");
			bprint(PR_PRINT_HIGH, plyr2->netname);
			bprint(PR_PRINT_HIGH, " need");
		}
		else
		{
			bprint(PR_PRINT_HIGH, "More players need");
		}

		bprint(PR_PRINT_HIGH, " to unlock the ");
		if (self->items & PR_IT_KEY1)
			bprint(PR_PRINT_HIGH, "silver");
		else
			bprint(PR_PRINT_HIGH, "gold");
		bprint(PR_PRINT_HIGH, " door\n");

		return PR_FALSE;
	}

	bprint(PR_PRINT_HIGH, "The ");
	if (self->items & PR_IT_KEY1)
		bprint(PR_PRINT_HIGH, "silver");
	else
		bprint(PR_PRINT_HIGH, "gold");
	bprint(PR_PRINT_HIGH, " door has been unlocked\n");

	return PR_TRUE;
}

} // END namespace Progs
