/*======================================================
	MEDIC.QC

	SB-1 Tech			15/12/2000
========================================================
Functions for the MEDIC class and associated weaponry
========================================================*/

#include "progs.h"
#include "ofndefs.h"
#include "tfdefs.h"
#include "custom.h"
#include "cpstuff.h"
#include "tfort.h"
#include "combat.h"
#include "sprites.h"
#include "debug.h"
#include "scout.h"
#include "weapons.h"
#include "gweapons.h"
#include "client.h"
#include "items.h"
#include "spy.h"

namespace Progs {

// Functions within this file:

float BioInfect(entity targ, entity attacker, float prob, entity inflictor);
void BioGrenadeTouch();
void BioGrenadeExplode();

// Functions referenced from this file:

void BioInfection_Decay();
void TeamFortress_SetSpeed(entity p);
float infront2( entity targ, entity check);

//=========================================================================
// Attacker attempts to infect targ.
float BioInfect(entity targ, entity attacker, float prob, entity inflictor)
{
	entity BioInfection;
	float r;

	// If they have medikit, don't infect them.
	if (targ->weapons_carried & PR_WEAP_MEDIKIT)
		return PR_FALSE;

	// Only allow one infection at once.
	if (targ->tfstate & PR_TFSTATE_INFECTED)
		return PR_FALSE;

	if (targ->PR_runes & PR_RUNE_RESIS)
		return PR_FALSE;

	if (Teammate(targ, attacker) && targ != attacker)
		return PR_FALSE;

	if (targ->invincible_finished > time)
		return PR_FALSE;

	r = random();
	if (prob < r)
		return PR_FALSE;

	r = random();
	// Melee armour stops infection 75% of the time
	if (targ->tf_items & PR_NIT_GEL && r < 0.75)
		return PR_FALSE;

	string desc;
	if (inflictor->netname == "special_mine")
		desc = "mine";
	else
		desc = "grenade";

	if (attacker == targ) // we infected ourselves! lol
	{
		sprint(attacker,PR_PRINT_HIGH,"You infect yourself!\n");

		teamprefixsprint(attacker->team_no,attacker);
		teamsprint6(attacker,attacker->netname,"infects himself!\n","","","","");
	}
	else
	{
		if (IsOwnedMonster(targ))
		{
			string tmps;
			tmps = GetMonsterName(targ);
			sprint(attacker,PR_PRINT_HIGH,"Your ",desc," infects the ",tmps," ",targ->netname);
			sprint(attacker,PR_PRINT_HIGH,"!\n");

			// Inform owner
			sprint(targ->real_owner,PR_PRINT_HIGH,"Your ",tmps," ",targ->netname," has been infected!\n");

			teamprefixsprint(targ->real_owner->team_no,targ->real_owner);
			teamsprint6(targ->real_owner,"The ",tmps," ",targ->netname," has been infected!\n","");
		} else {
			sprint (attacker, PR_PRINT_HIGH, "Your ", desc, " infects ", targ->netname, "!\n");

#ifdef PR_COOP_MODE_ENHANCED
			if (!COOP_IsCoopMonster( targ )) {
#endif
				sprint (targ, PR_PRINT_HIGH, "You have been infected by a biological ", desc, "!\n");

				teamprefixsprint(targ->team_no,targ);
				teamsprint6(targ,targ->netname," has been infected!\n","","","","");
#ifdef PR_COOP_MODE_ENHANCED
			}
#endif
		}
	}

	// Gizmo - allow non demons and non army soldiers to be affected (coop monsters)
	if (IsMonster(targ))
	{
		targ->tfstate = targ->tfstate | PR_TFSTATE_INFECTED;

		BioInfection = spawnServerSide(); // PZ: make it a server-side only entity
		BioInfection->classname = "timer";
		BioInfection->netname = "biotimer";
		BioInfection->owner = attacker;
		BioInfection->nextthink = time + 2;
		BioInfection->think = BioInfection_MonsterDecay;
		BioInfection->enemy = targ;

		targ->infection_team_no = attacker->team_no;
	}
	else
	{
		targ->tfstate = targ->tfstate | PR_TFSTATE_INFECTED;

		BioInfection = spawnServerSide(); // PZ: make it a server-side only entity
		BioInfection->classname = "timer";
		BioInfection->netname = "biotimer";
		BioInfection->owner = targ;
		BioInfection->nextthink = time + 2;
		BioInfection->think = BioInfection_Decay;
		BioInfection->enemy = attacker;

		targ->infection_team_no = attacker->team_no;
	}

	return PR_TRUE;
}

//=========================================================================
// Touch function for the bioinfection grenade.

void BioGrenadeTouch()
{
	if (self->velocity == V({0, 0, 0}))
		self->avelocity = V({0, 0, 0});

	if (other == self->owner)
		return; 	// don't explode on owner

	// Thrown grenades don't detonate when hitting an enemy
	sound (self, PR_CHAN_WEAPON, "weapons/bounce.wav", 1, PR_ATTN_NORM);	// bounce sound
}

//=========================================================================
// Biogrenade explosion. Does not do much damage...but infects...

void BioGrenadeExplode()
{
	// Toxic fluids do some damage on explosion
	deathmsg = PR_DMSG_BIOEXPLOSION;
	T_BeamDamage(self,self->owner,60);

	entity dier; // He's the dier and he's dying

	dier = findradius(self->origin, PR_BIO_GREN_RADIUS);
	while (dier != world)
	{
		if (dier->classname == "player" && dier->health > 0)
		{
			//if (visible(dier))
			if (CanDamage(dier,self))
				if (dier->takedamage)
				{
					BioInfect(dier, self->owner, 1, self); // was 0.7
				}
		}
		else
		{
			// Gizmo - allow non demons and non army soldiers to be affected (coop monsters)
			if (IsMonster(dier))
			//if (visible(dier))
			if (CanDamage(dier,self))
				BioInfect(dier, self->owner, 1, self);
		}

		dier = dier->chain;
	}

	sound(self,PR_CHAN_WEAPON,"misc/vapeur2.wav",1,PR_ATTN_NORM);

	SpawnSprite(3,PR_SPRITE_ABLAST,self->origin,V({0, 0, 0}),PR_SPRITEMOVE_UP,0.1);

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
}

void CureAdverseEffects(entity doc, entity patient, const vector& org)
{
	entity te;

	// remove concussion from player
	// Try to find a concusstimer entity for this player
	te = find(world, "classname", "timer");
	while (((te->owner != patient) || (te->think != ConcussionGrenadeTimer)) && (te != world))
	{
		te = find(te, "classname", "timer");
	}

	if (te != world)
	{
		SpawnBlood(org, 20);
		patient->tfstate = patient->tfstate - (patient->tfstate & PR_TFSTATE_CONCUSSIONED);
		if (doc == patient)
			sprint(doc, PR_PRINT_HIGH, "The ground is behaving itself now.\n");
		else
		{
			sprint(patient, PR_PRINT_HIGH, doc->netname, " heals you of your concussion\n");
			sprint(doc, PR_PRINT_HIGH, "You heal ", patient->netname, "'s concussion\n");
		}
		// Give the medic a frag for doing it, only if it was caused by an enemy
		if (!Teammate(doc, te))
		{
			doc->real_frags = doc->real_frags + 1;
			if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
				doc->frags = doc->real_frags;
		}
		TeamFortress_SetSpeed(patient);
		dremove(te);
		//OfN - return #TRUE;
	}

	// remove hallucination from player
	// Try to find a hallucination timer entity for this player
	if (patient->tfstate & PR_TFSTATE_HALLUCINATING)
	{
		te = find(world, "classname", "timer");
		while (((te->owner != patient) || (te->think != HallucinationTimer)) && (te != world))
		{
			te = find(te, "classname", "timer");
		}

		if (te != world)
		{
			patient->tfstate = patient->tfstate - (patient->tfstate & PR_TFSTATE_HALLUCINATING);
			SpawnBlood(org, 20);
			if (patient == doc)
				sprint(doc, PR_PRINT_HIGH, "The visions have stopped.\n");
			else
			{
				sprint(patient, PR_PRINT_HIGH, doc->netname, " heals you of your hallucinations\n");
				sprint(doc, PR_PRINT_HIGH, "You halt ", patient->netname, "'s hallucinations\n");
			}
			// Give the medic a frag for doing it, only if it was caused by an enemy
			if (!Teammate(doc, te))
			{
				doc->real_frags = doc->real_frags + 1;
				if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
					doc->frags = doc->real_frags;
			}

			dremove(te);
			//OfN - return #TRUE;
		}
		else
		{
			RPrint("Warning: Error in Hallucination Timer logic.\n");
		}
	}

	#ifndef PR_OLD_FLASH
	if (patient->tfstate & PR_TFSTATE_PSIONIZED)
	{
		patient->tfstate = patient->tfstate - PR_TFSTATE_PSIONIZED;

		TeamFortress_SetSpeed(patient);

		te = find(world, "netname", "psionictimer");
		while ((te->owner != patient || te->classname != "timer") && (te != world))
			te = find(te, "netname", "psionictimer");

		if (te != world)
		{
			SpawnBlood(org, 20);

			if (doc == patient)
				sprint(doc, PR_PRINT_HIGH, "The psionic influence has stopped.\n");
			else
			{
				sprint(patient, PR_PRINT_HIGH, doc->netname, " restores your psionic problems!\n");
				sprint(doc, PR_PRINT_HIGH, "You restore ", patient->netname, "'s psionic normality\n");
			}

			// Give the medic a frag for doing it, only if it was caused by an enemy
			if (!Teammate(doc, te))
			{
				doc->real_frags = doc->real_frags + 1;
				if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
					doc->frags = doc->real_frags;
			}

			dremove(te);
			//OfN - return #TRUE;
		}
	}
	#endif // PSIONIC

	// remove tranquilisation from player
	// Try to find a tranquilisation timer entity for this player
	if (patient->tfstate & PR_TFSTATE_TRANQUILISED)
	{
		te = find(world, "classname", "timer");
		while (((te->owner != patient) || (te->think != TranquiliserTimer)) && (te != world))
		{
			te = find(te, "classname", "timer");
		}
		if (te != world)
		{
			patient->tfstate = patient->tfstate - (patient->tfstate & PR_TFSTATE_TRANQUILISED);
			TeamFortress_SetSpeed(patient);

			SpawnBlood(org, 20);
			if (doc == patient)
				sprint(doc, PR_PRINT_HIGH, "You feel more alert now.\n");
			else
			{
				sprint(patient, PR_PRINT_HIGH, doc->netname, " heals you of your tranquilisation!\n");
				sprint(doc, PR_PRINT_HIGH, "You heal ", patient->netname, "'s tranquilisation\n");
			}
			// Give the medic a frag for doing it, only if it was caused by an enemy
			if (!Teammate(doc, te))
			{
				doc->real_frags = doc->real_frags + 1;
				if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
					doc->frags = doc->real_frags;
			}

			dremove(te);
			//OfN - return #TRUE;
		}
		else
		{
			RPrint("Warning: Error in Tranquilisation Timer logic.\n");
		}
	}

	// check if the healed player is blinded
	/*if (patient.tfstate & TFSTATE_STASIS)
	{
		patient.tfstate = patient.tfstate - (patient.tfstate & TFSTATE_STASIS);
		TeamFortress_SetSpeed(patient);
		if (doc == patient)
			sprint(doc, #PRINT_HIGH, "You can move freely again!\n");
		else
		{
			sprint(patient, #PRINT_HIGH, doc.netname, " removes the effect of stasis on you!\n");
			sprint(doc, #PRINT_HIGH, "You halt the effects of stasis on ", patient.netname, "!\n");
		}
//		patient.movetype = patient.stasismovetype;
		patient.gravity = patient.stasisGravity;
		patient.effects = patient.effects - (patient.effects & #EF_BRIGHTFIELD);
		return 1;
	}*/

	// check if the healed player is infected
	if (patient->tfstate & PR_TFSTATE_INFECTED)
	{
		float healam;

		healam = rint(patient->health / 2);
		// remove the infection
		patient->tfstate = patient->tfstate - (patient->tfstate & PR_TFSTATE_INFECTED);

		te = find(world, "netname", "biotimer");

		while (te != world)
		{
			if (te->classname == "timer")
				if (te->owner == patient)
					dremove(te);

			te = find(te, "netname", "biotimer");
		}

		// some damage is caused (because of the use of leeches!)
		// remove half their remaining health
		deathmsg = PR_DMSG_MEDIKIT;
		TF_T_Damage(patient, doc, doc, healam, 0, PR_TF_TD_OTHER);

		SpawnBlood(org, 30);
		if (doc == patient)
			sprint(doc, PR_PRINT_HIGH, "Your sores disappear.\n");
		else
		{
			sprint(patient, PR_PRINT_HIGH, doc->netname, " cures your infection!\n");
			if (doc->classname == "player")
			{
				sprint(doc, PR_PRINT_HIGH, "You have healed ");
				sprint(doc, PR_PRINT_HIGH, patient->netname);
				sprint(doc, PR_PRINT_HIGH, " of the infection.\n");

				teamprefixsprintbi(doc->team_no,doc,patient);
				teamsprintbi(doc,patient,doc->netname," cures ",patient->netname,"'s infection\n","","");

				// Give the medic a frag for doing it, only if it was caused by an enemy
				if (!Teammate(patient->infection_team_no, doc)) // PZ TODO: In deathmatch, can you get points for healing your own summon that you infected?
				{
					doc->real_frags = doc->real_frags + 1;
					if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
						doc->frags = doc->real_frags;
				}
			}
		}

		//OfN - return #TRUE;
	}

	// put out the fire if they are burning
	if (patient->numflames > 0)
	{
		sound(patient, PR_CHAN_WEAPON, "items/r_item1.wav", 1, PR_ATTN_NORM);
		patient->numflames = 0;

		if (doc == patient)
			sprint (doc, PR_PRINT_HIGH, "Your flames are extinguished!\n");
		else
		{
			sprint(patient, PR_PRINT_HIGH, doc->netname, " stops you burning!\n");
			if (doc->classname == "player")
			{
				sprint(doc, PR_PRINT_MEDIUM, "You have put out ");
				sprint(doc, PR_PRINT_MEDIUM, patient->netname);
				sprint(doc, PR_PRINT_MEDIUM, "'s fire.\n");
			}
		}

		//OfN - return #TRUE;
	}
}

#ifndef PR_NO_REVIVE
/*
================
ClientRevived
================
*/
#ifdef PR_NEVER_DEFINED
void ClientRevived()
{
	float iszoom, lastframe;

	self->touch = player_touch;
	self->classname = "player";

	self->flags = self->flags - (self->flags & PR_FL_FINDABLE_NONSOLID);

	self->takedamage = PR_DAMAGE_AIM;
	self->solid = PR_SOLID_SLIDEBOX;
	self->movetype = PR_MOVETYPE_WALK;
	self->show_hostile = 0;
	self->is_malfunctioning = 0;
	self->is_abouttodie = 0;

	// OfN
	self->has_holo = 0;
	self->is_killed = PR_FALSE;

	self->message = ""; // reset for gremlin and cool gibs (heads)

	self->FlashTime = 0;
	self->flags = PR_FL_CLIENT;
	self->aura = 0; //- OfN
	self->crusader_inspirator=world; // OfN - needed?
//	self.gravity = 1; //WK
	self->air_finished = time + 12;
	self->dmg = 2;			// initial water damage
	self->super_damage_finished = 0;
	self->radsuit_finished = 0;
	self->invisible_finished = 0;
	self->invincible_finished = 0;
	self->effects = 0;
	self->invincible_time = 0;
	self->reload_shotgun = 0;
	self->reload_super_shotgun = 0;
	self->reload_grenade_launcher = 0;
	self->reload_rocket_launcher = 0;
	self->reload_light_assault = 0;
	self->reload_laser_cannon = 0;

	self->last_attacked_time = 0; //WK For chaplan healing purposes

	// grapple stuff
	self->on_hook = PR_FALSE;
	self->hook_out = PR_FALSE;
	self->fire_held_down = PR_FALSE;

	// some states are kept
	iszoom = 0;

	if (self->tfstate & PR_TFSTATE_ZOOMOFF)
		iszoom = 1;

	self->tfstate = 0;

	if (iszoom == 1)
		self->tfstate = self->tfstate | PR_TFSTATE_ZOOMOFF;

	// Set the speed for the player based on class
	TeamFortress_SetSpeed(self);
	stuffcmd(self, "v_idlescale 0\n");
	self->tfstate = self->tfstate - (self->tfstate & PR_TFSTATE_CONCUSSIONED); // PZ: added this flag for concussing bots
	stuffcmd(self, "v_cshift 0 0 0 0\n");
	//WK Clear Bastard Rotation
	stuffcmd(self, "-left;-right;cl_yawspeed 140\n");

	self->current_weapon = PR_WEAP_AXE;
	lastframe = self->frame;
	W_SetCurrentAmmo ();
	self->frame = lastframe;

	self->attack_finished = time + 0.3;

	self->th_pain = player_pain;
	self->th_die = PlayerDie;

	// make sure that autozoom is reset
	if (self->height != 0)
	{
		self->height = 0;
		TF_zoom(90);
	}

	self->deadflag = PR_DEAD_NO;
	// pausetime is set by teleporters to keep the player from moving a while
	self->pausetime = 0;

/*
	setmodel (self, string_null);
	modelindex_null = self.modelindex;

	setmodel (self, "progs/eyes.mdl");
	modelindex_eyes = self.modelindex;

	setmodel (self, "progs/player.mdl");
	modelindex_player = self.modelindex;
*/

	setsize (self, PR_VEC_HULL_MIN, PR_VEC_HULL_MAX);

	self->view_ofs = V({0, 0, 22});
}
#endif

/*
================
MedicRevive

Revives a dead player.

If deadPlayer is set, this only checks to make sure
it's a valid entity to revive, and ignores things
like if it's a teammate, if it's nearby, and if it's
infront.

WARNING: if deadPlayer is world, makevectors() will
be called and traceline() will also. So you'll have
to save off those values before calling this if you
need them to stay after this returns.
================
*/
float MedicRevive(entity medic, entity deadPlayer, int reviveFlags)
{
	entity  ent;
	vector  vec1, vec2;
	float   oldArmor, oldArmorType, oldArmorClass, oldShells, oldNails, oldRockets, oldCells,
	        oldGren1, oldGren2, oldDetpack, oldC4, oldMedikit;
	float   ftmp1, ftmp2;

	// if deadPlayer is world then try to find one
#ifdef PR_NEVER_DEFINED
	if ( deadPlayer  == world) {
		ent = find( world, "classname", "player" );
		while ( ent != world && deadPlayer == world ) {
			if ( ent->health <= 0 && ent->takedamage && ent->is_connected )
			if ( ent->modelindex == modelindex_player )
			if ( Teammate(medic, ent) )
			if ( vlen( ent->origin - medic->origin ) <= 96 )
			if ( infront2( ent, medic ) )
				deadPlayer = ent;

			if ( deadPlayer  == world)
				ent = find( ent, "classname", "player" );
		}

		if ( deadPlayer  == world)
			return PR_FALSE;
	}
#else
	if (deadPlayer == world)
	{
		makevectors(medic->v_angle);

		vec1 = medic->origin + V({0, 0, 16});
		traceline(vec1, vec1 + v_forward * 96, PR_TL_EVERYTHING, medic); // PZ NOTE: why are we looking for all types of entities?

		if (trace_ent != world)
		if (trace_ent->classname == "player")
		if (Teammate(medic, trace_ent))
			deadPlayer = trace_ent;

		if (deadPlayer == world)
			return PR_FALSE;
	}
#endif

	// try to revive the dead player
	if ( deadPlayer->health > 0 || !deadPlayer->takedamage || !deadPlayer->is_connected || deadPlayer->modelindex != modelindex_player )
		return PR_FALSE;

	if ( reviveFlags & PR_REVIVE_MEDIKITAMMO && medic->ammo_medikit < 10 ) {
		if ( reviveFlags & PR_REVIVE_PRINTMSGS && medic->flags & PR_FL_CLIENT )
			sprint( medic, PR_PRINT_HIGH, "You need at least 10 medikit to revive.\n" );
		return PR_FALSE;
	}

	// make sure there's room
	if ( deadPlayer->classname == "player" ) {
		vec1 = deadPlayer->mins;
		vec2 = deadPlayer->maxs;
		setsize( deadPlayer, PR_VEC_HULL_MIN, PR_VEC_HULL_MAX );

		if ( InSolid( deadPlayer ) ) {
			if ( reviveFlags & PR_REVIVE_PRINTMSGS && medic->flags & PR_FL_CLIENT )
				sprint( medic, PR_PRINT_HIGH, "You need a clear area around him to revive.\n" );
			setsize( deadPlayer, vec1, vec2 );
			return PR_FALSE;
		}
	}

	// tell the medic
	if ( reviveFlags & PR_REVIVE_PRINTMSGS && medic->flags & PR_FL_CLIENT ) {
		sprint( medic, PR_PRINT_HIGH, "You have revived " );
		sprint( medic, PR_PRINT_HIGH, deadPlayer->netname );
		sprint( medic, PR_PRINT_HIGH, ".\n" );
	}
	// tell the dead player
	if ( reviveFlags & PR_REVIVE_PRINTMSGS && deadPlayer->flags & PR_FL_CLIENT ) {
		sprint( deadPlayer, PR_PRINT_HIGH, medic->netname );
		sprint( deadPlayer, PR_PRINT_HIGH, " has revived you!\n" );
	}

	// TODO: give the medic a frag for doing it, only if it was caused by an enemy

	// remove medikit ammo
	if ( reviveFlags & PR_REVIVE_MEDIKITAMMO )
		medic->ammo_medikit = medic->ammo_medikit - 10;

	// save off his ammo because SetClientAlive() will set it to stock amounts
	oldArmor		= deadPlayer->armorvalue;
	oldArmorType	= deadPlayer->armortype;
	oldArmorClass	= deadPlayer->armorclass;
	oldShells		= deadPlayer->ammo_shells;
	oldNails		= deadPlayer->ammo_nails;
	oldRockets		= deadPlayer->ammo_rockets;
	oldCells		= deadPlayer->ammo_cells;
	oldGren1		= deadPlayer->no_grenades_1;
	oldGren2		= deadPlayer->no_grenades_2;
	oldDetpack		= deadPlayer->ammo_detpack;
	oldC4			= deadPlayer->ammo_c4det;
	oldMedikit		= deadPlayer->ammo_medikit;
	ftmp1			= deadPlayer->current_weapon;

	// revive the dead player
	ent = self;
	self = deadPlayer;
	ftmp2 = deadPlayer->frame;
	SetClientAlive();
	deadPlayer->frame = ftmp2;

	// restore his old ammo
	deadPlayer->armorvalue		= oldArmor;
	deadPlayer->armortype		= oldArmorType;
	deadPlayer->armorclass		= oldArmorClass;
	deadPlayer->ammo_shells		= oldShells;
	deadPlayer->ammo_nails		= oldNails;
	deadPlayer->ammo_rockets		= oldRockets;
	deadPlayer->ammo_cells		= oldCells;
	deadPlayer->no_grenades_1	= oldGren1;
	deadPlayer->no_grenades_2	= oldGren2;
	deadPlayer->ammo_detpack		= oldDetpack;
	deadPlayer->ammo_c4det		= oldC4;
	deadPlayer->ammo_medikit		= oldMedikit;
	deadPlayer->current_weapon	= ftmp1;

	ftmp2 = deadPlayer->frame;
	W_SetCurrentAmmo();
	deadPlayer->frame = ftmp2;
	self = ent;

	// Gizmo - give them some temporary bonus health so they stand a chance
	T_Heal( deadPlayer, PR_WEAP_MEDIKIT_OVERHEAL, 1 );

	if ( !( deadPlayer->items & PR_IT_SUPERHEALTH ) ) {
		deadPlayer->items = deadPlayer->items | PR_IT_SUPERHEALTH;
		newmis = spawnServerSide(); // PZ: make it a server-side only entity
		newmis->classname = "medikit_rot";
		newmis->nextthink = time + 10;		// Gizmo - original from medikit is 20
		newmis->think = item_megahealth_rot;
		newmis->owner = deadPlayer;
	}

	deadPlayer->nextthink = time + 0.1;
	deadPlayer->think = spy_upb1;
	deadPlayer->attack_finished = time + 1.5;		// don't let them fire for a little bit

	// make some noise to let others know we're alive
	if ( deadPlayer->waterlevel == 3 )
	{
		if ( random() > 0.5 )
			sound( deadPlayer, PR_CHAN_VOICE, "player/drown1.wav", 1, PR_ATTN_NORM );
		else
			sound( deadPlayer, PR_CHAN_VOICE, "player/drown2.wav", 1, PR_ATTN_NORM );
	} else
		sound( deadPlayer, PR_CHAN_VOICE, "player/gasp2.wav", 1, PR_ATTN_NORM );

	// also print a team message
	teamprefixsprintbi( deadPlayer->team_no, deadPlayer, medic );
	teamsprintbi( deadPlayer, medic, deadPlayer->netname, " has been revived by ", medic->netname, "\n", "", "" );

	return PR_TRUE;
}

} // END namespace Progs

#endif
