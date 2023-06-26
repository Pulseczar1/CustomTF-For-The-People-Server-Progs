/*======================================================
	TSOLDIER.QC

	TeamFortress v2.5	 29/2/97
========================================================
Functions for the SOLDIER class and associated weaponry
========================================================*/

#include "progs.h"
#include "cpstuff.h"
#include "weapons.h"

namespace Progs {

// Functions outside this file

// Functions inside this file
// Nail Grenade Functions
void NailGrenadeTouch();
void NailGrenadeExplode();
void NailGrenadeNailEm();
void NailGrenadeLaunchNail();

//=========================================================================
// Touch Function for Nail Grenade
void NailGrenadeTouch()
{
	if (other == self->owner)
		return;		// don't explode on owner

	// If the Nail Grenade hits a player, it just bounces off

	sound (self, PR_CHAN_WEAPON, "weapons/bounce.wav", 1, PR_ATTN_NORM);
	if (self->velocity == V({0, 0, 0}))
		self->avelocity = V({0, 0, 0});
}

//=========================================================================
// Explode Function for Nail Grenade
void NailGrenadeExplode()
{
	// Raise into the air
	self->movetype = PR_MOVETYPE_FLY;

	setorigin(self, self->origin + V({0, 0, 32}));
	self->avelocity = V({0, 500, 0});

	self->nextthink = time + 0.7;
	self->think = NailGrenadeNailEm;
}

//=========================================================================
// Nail function for Nail Grenade
void NailGrenadeNailEm()
{
	// Rotate and spew Nails
	self->velocity = V({0, 0, 0});

	self->nextthink = time + 0.1;
	self->think = NailGrenadeLaunchNail;
	self->playerclass = 0;
}

//=========================================================================
// Nail function for Nail Grenade
void NailGrenadeLaunchNail()
{
	float i,j;
	float current_yaw;

	i = 0;
#ifdef PR_QUAKEWORLD
	while (i < 1)	// Only throw one nail in QW... a big one :)
#else
	while (i < 3)
#endif
	{
		j = (random() + 2) * 5;
		current_yaw = anglemod(self->angles[Y] + j);
		self->angles[Y] = current_yaw;
		self->angles[X] = 0;
		self->angles[Z] = 0;
		makevectors(self->angles);

        #ifdef PR_OLD_NAILGREN
        deathmsg = PR_DMSG_GREN_NAIL;
		launch_spike(self->origin, v_forward);
		newmis->touch = spike_touch;
		newmis->weapon = PR_DMSG_GREN_NAIL;
        #else
        if (random() < 0.33)
            MuzzleFlash(self);
        launch_directspike(self->origin, v_forward);
        #endif

		i = i + 1;
	}

	self->playerclass = self->playerclass + 1;
	self->nextthink = time + 0.1;

	// Explode
	if (self->playerclass > 50)
	{
		self->weapon = PR_DMSG_GREN_NAIL;
		self->think = GrenadeExplode;
	}
}

} // END namespace Progs
