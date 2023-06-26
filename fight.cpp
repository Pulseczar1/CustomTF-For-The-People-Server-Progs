#include "progs.h"
#include "weapons.h"

namespace Progs {

/*

A monster is in fight mode if it thinks it can effectively attack its
enemy.

When it decides it can't attack, it goes into hunt mode.

*/

float anglemod(float v);

//WK Selective AIs
#ifdef PR_COOP_MODE
void knight_atk1();
void knight_runatk1();
void ogre_smash1();
void ogre_swing1();

void sham_smash1();
void sham_swingr1();
void sham_swingl1();
#endif

float DemonCheckAttack();
void Demon_Melee(float side);

void ChooseTurn(const vector& dest);

void ai_face();



#ifdef PR_COOP_MODE
void knight_attack()
{
	float		len;

// decide if now is a good swing time
	len = vlen(self->enemy->origin+self->enemy->view_ofs - (self->origin+self->view_ofs));

	if (len < 80)
		knight_atk1 ();
	else
		knight_runatk1 ();
}
#endif

//=============================================================================

/*
===========
CheckAttack

The player is in view, so decide to move or launch an attack
Returns FALSE if movement should continue
============
*/
float CheckAttack()
{
	vector	spot1, spot2;
	entity	targ;
	float		chance;

	targ = self->enemy;

// see if any entities are in the way of the shot
	spot1 = self->origin + self->view_ofs;
	spot2 = targ->origin + targ->view_ofs;

	traceline (spot1, spot2, PR_TL_ANY_SOLID, self);

	if (trace_ent != targ)
		return PR_FALSE;		// don't have a clear shot

	if (trace_inopen && trace_inwater)
		return PR_FALSE;			// sight line crossed contents

	if (enemy_range == PR_RANGE_MELEE)
	{	// melee attack
		if (self->th_melee != SUB_Null)
		{
#ifdef PR_COOP_MODE
			if (self->classname == "monster_knight")
				knight_attack ();
			else
#endif
				self->th_melee ();
			return PR_TRUE;
		}
	}

// missile attack
	if (self->th_missile == SUB_Null)
		return PR_FALSE;

	if (time < self->attack_finished)
		return PR_FALSE;

	if (enemy_range == PR_RANGE_FAR)
		return PR_FALSE;

	if (enemy_range == PR_RANGE_MELEE)
	{
		chance = 0.9;
		self->attack_finished = 0;
	}
	else if (enemy_range == PR_RANGE_NEAR)
	{
		if (self->th_melee != SUB_Null)
			chance = 0.2;
		else
			chance = 0.4;
	}
	else if (enemy_range == PR_RANGE_MID)
	{
		if (self->th_melee != SUB_Null)
			chance = 0.05;
		else
			chance = 0.1;
	}
	else
		chance = 0;

	if (random () < chance)
	{
		self->th_missile ();
		//SUB_AttackFinished (2*random());
        Attack_Finished(2*random());
		return PR_TRUE;
	}

	return PR_FALSE;
}


/*
=============
ai_face

Stay facing the enemy
=============

void() ai_face =
{
	self.ideal_yaw = vectoyaw(self.enemy.origin - self.origin);
	ChangeYaw ();
};
*/

//WK Ai face for everyone
void ai_face()
{
    // OfN - Fixes 'yaw to world origin' bug when holos r targetted by a sentry and disappear
    if (self->classname=="building_sentrygun" && self->enemy->classname == "holo" && self->enemy->has_holo == 0)
        return;

    self->ideal_yaw = vectoyaw(self->enemy->origin - self->origin);
	self->ideal_yaw = anglemod(self->ideal_yaw);

	//WK Turrets swivel instanter
	/* if (self.tf_items & #NIT_TURRET) { //Auto-swivel
		self.angles_y = self.ideal_yaw;
	} */
	//WK Fix a sentry stuck bug?
	if (self->classname == "building_sentrygun") { //Auto-swivel
         self->angles[Y] = self->ideal_yaw;
	}
	else
		ChangeYaw ();
}

/*
=============
ai_charge

The monster is in a melee attack, so get as close as possible to .enemy
=============
*/
float visible(entity targ);
float infront(entity targ);
float range(entity targ);

void ai_charge(float d)
{
	ai_face ();
	movetogoal (d);		// done in C code...
}

void ai_charge_side()
{
	vector	dtemp;
	float	heading;

// aim to the left of the enemy for a flyby

	self->ideal_yaw = vectoyaw(self->enemy->origin - self->origin);
	ChangeYaw ();

	makevectors (self->angles);
	dtemp = self->enemy->origin - 30*v_right;
	heading = vectoyaw(dtemp - self->origin);

	walkmove(heading, 20);
}


/*
=============
ai_melee

=============
*/
void ai_melee()
{
	vector	delta;
	float 	ldmg;

	if (self->enemy == world)
		return;		// removed before stroke

	delta = self->enemy->origin - self->origin;

	if (vlen(delta) > 60)
		return;

	ldmg = (random() + random() + random()) * 3;
	T_Damage (self->enemy, self, self, ldmg);
}


void ai_melee_side()
{
	vector	delta;
	float 	ldmg;

	if (self->enemy == world)
		return;		// removed before stroke

	ai_charge_side();

	delta = self->enemy->origin - self->origin;

	if (vlen(delta) > 60)
		return;
	if (!CanDamage (self->enemy, self))
		return;
	ldmg = (random() + random() + random()) * 3;
	T_Damage (self->enemy, self, self, ldmg);
}


//=============================================================================

/*
===========
SoldierCheckAttack

The player is in view, so decide to move or launch an attack
Returns FALSE if movement should continue
============
*/
/*
CH removed due to not used?

float() SoldierCheckAttack =
{
	local vector	spot1, spot2;
	local entity	targ;
	local float		chance;

	targ = self.enemy;

// see if any entities are in the way of the shot
	spot1 = self.origin + self.view_ofs;
	spot2 = targ.origin + targ.view_ofs;

	traceline (spot1, spot2, #TL_ANY_SOLID, self);

	if (trace_inopen && trace_inwater)
		return #FALSE;			// sight line crossed contents

	if (trace_ent != targ)
		return #FALSE;	// don't have a clear shot


// missile attack
	if (time < self.attack_finished)
		return #FALSE;

	if (enemy_range == #RANGE_FAR)
		return #FALSE;

	if (enemy_range == #RANGE_MELEE)
		chance = 0.9;
	else if (enemy_range == #RANGE_NEAR)
		chance = 0.4;
	else if (enemy_range == #RANGE_MID)
		chance = 0.05;
	else
		chance = 0;

	if (random () < chance)
	{
		self.th_missile ();
		SUB_AttackFinished (1 + random());
		if (random() < 0.3)
			self.lefty = !self.lefty;

		return #TRUE;
	}

	return #FALSE;
};
//=============================================================================
*/
/*
===========
ShamCheckAttack

The player is in view, so decide to move or launch an attack
Returns FALSE if movement should continue
============
*/
/*
CH also removed from not used

float() ShamCheckAttack =
{
	local vector	spot1, spot2;
	local entity	targ;
	local float		chance;
	local float		enemy_yaw;

	if (enemy_range == #RANGE_MELEE)
	{
		if (CanDamage (self.enemy, self))
		{
			self.attack_state = #AS_MELEE;
			return #TRUE;
		}
	}

	if (time < self.attack_finished)
		return #FALSE;

	if (!enemy_vis)
		return #FALSE;

	targ = self.enemy;

// see if any entities are in the way of the shot
	spot1 = self.origin + self.view_ofs;
	spot2 = targ.origin + targ.view_ofs;

	if (vlen(spot1 - spot2) > 600)
		return #FALSE;

	traceline (spot1, spot2, #TL_ANY_SOLID, self);

	if (trace_inopen && trace_inwater)
		return #FALSE;			// sight line crossed contents

	if (trace_ent != targ)
	{
		return #FALSE;	// don't have a clear shot
	}

// missile attack
	if (enemy_range == #RANGE_FAR)
		return #FALSE;

	self.attack_state = #AS_MISSILE;
	SUB_AttackFinished (2 + 2*random());
	return #TRUE;
};

//============================================================================
*/
/*
===========
OgreCheckAttack

The player is in view, so decide to move or launch an attack
Returns FALSE if movement should continue
============
*/
/*
CH yet another remove from not used

float() OgreCheckAttack =
{
	local vector	spot1, spot2;
	local entity	targ;
	local float		chance;

	if (enemy_range == #RANGE_MELEE)
	{
		if (CanDamage (self.enemy, self))
		{
			self.attack_state = #AS_MELEE;
			return #TRUE;
		}
	}

	if (time < self.attack_finished)
		return #FALSE;

	if (!enemy_vis)
		return #FALSE;

	targ = self.enemy;

// see if any entities are in the way of the shot
	spot1 = self.origin + self.view_ofs;
	spot2 = targ.origin + targ.view_ofs;

	traceline (spot1, spot2, #TL_ANY_SOLID, self);

	if (trace_inopen && trace_inwater)
		return #FALSE;			// sight line crossed contents

	if (trace_ent != targ)
	{
		return #FALSE;	// don't have a clear shot
	}

// missile attack
	if (time < self.attack_finished)
		return #FALSE;

	if (enemy_range == #RANGE_FAR)
		return #FALSE;

	else if (enemy_range == #RANGE_NEAR)
		chance = 0.10;
	else if (enemy_range == #RANGE_MID)
		chance = 0.05;
	else
		chance = 0;

	self.attack_state = #AS_MISSILE;
	SUB_AttackFinished (1 + 2*random());
	return #TRUE;
};
*/
//CH it needs this or else it wont compile?

} // END namespace Progs
