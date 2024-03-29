/*======================================================
	ACTIONS.QC			Custom TeamFortress v3.2

	(c) TeamFortress Software Pty Ltd	13/5/97
	(c) William Kerney			2/9/00
	(c) Craig Hauser				19/3/00
========================================================
Non Class-Specific Impulse Commands
=======================================================*/

#include "progs.h"
#include "coop_defs.h"
#include "ofndefs.h"
#include "debug.h"
#include "weapons.h"
#include "tfort.h"
#include "gweapons.h"
#include "custom.h"
#include "monsters.h"
#include "warlock.h"

namespace Progs {

// Prototypes
void TeamFortress_Discard();
void TeamFortress_SaveMe();
void TeamFortress_ID(float inAuto);
void TeamFortress_ReloadCurrentWeapon();

//CH dropitems
float TeamFortress_DropItems();
void tfgoalitem_RemoveFromPlayer(entity Item, entity AP, float method);

//=========================================================================
// Discard Command. Drops all ammo useless to the player's class.
void TeamFortress_Discard()
{
	// Create a backpack
	newmis = spawn();
//CH to save space. Dont do class specific checks
//
//CH new setup, checks if you have weapon that uses that ammo
//and if you dont, throws it away
	if (self->playerclass != PR_PC_UNDEFINED)
	{
		if (!(self->weapons_carried & PR_WEAP_SNIPER_RIFLE || self->weapons_carried & PR_WEAP_AUTO_RIFLE || self->weapons_carried & PR_WEAP_SHOTGUN || self->weapons_carried & PR_WEAP_SUPER_SHOTGUN || self->weapons_carried & PR_WEAP_ASSAULT_CANNON || (self->cutf_items & PR_CUTF_SENTRYGUN && self->weapons_carried & PR_WEAP_SPANNER)))
			newmis->ammo_shells = self->ammo_shells;
		if (!(self->weapons_carried & PR_WEAP_NAILGUN || self->weapons_carried & PR_WEAP_SNG || self->weapons_carried & PR_WEAP_LIGHT_ASSAULT || self->weapons_carried & PR_WEAP_RAILGUN || self->weapons_carried & PR_WEAP_TRANQ || self->weapons_carried & PR_WEAP_MAUSER)) //CH is it just me or are there not many nail weapons? //WK It's just you
			newmis->ammo_nails = self->ammo_nails;
		if (!(self->weapons_carried & PR_WEAP_ROCKET_LAUNCHER || self->weapons_carried & PR_WEAP_INCENDIARY || self->weapons_carried & PR_WEAP_GRENADE_LAUNCHER || (self->cutf_items & PR_CUTF_SENTRYGUN && self->weapons_carried & PR_WEAP_SPANNER) || self->job & PR_JOB_GUERILLA))
			newmis->ammo_rockets = self->ammo_rockets;
		//OfN old bug, WEAP_SUPERSHOTGUN if (!(self.cutf_items & #CUTF_CYBERAUG || self.cutf_items & #CUTF_HOLO || self.weapons_carried & #WEAP_LASERCANNON || self.weapons_carried & #WEAP_INCENDIARY || self.weapons_carried & #WEAP_LIGHTNING || self.weapons_carried & #WEAP_SUPER_SHOTGUN || self.weapons_carried & #WEAP_ASSAULT_CANNON || self.weapons_carried & #WEAP_DAEDALUS || (self.cutf_items & #CUTF_SENTRYGUN && self.weapons_carried & #WEAP_SPANNER) || (self.weapons_carried & #WEAP_SPANNER && self.tf_items & #NIT_TESLA) || (self.weapons_carried & #WEAP_SPANNER && self.tf_items & #NIT_SECURITY_CAMERA) || (self.weapons_carried & #WEAP_SPANNER && self.tf_items & #NIT_TELEPORTER)))
		//done -  FIXME: Add fieldgens
		//if (!(self.cutf_items & #CUTF_CYBERAUG || self.cutf_items & #CUTF_HOLO || self.weapons_carried & #WEAP_LASERCANNON || self.weapons_carried & #WEAP_INCENDIARY || self.weapons_carried & #WEAP_LIGHTNING || self.weapons_carried & #WEAP_AIRF || self.weapons_carried & #WEAP_ASSAULT_CANNON || self.weapons_carried & #WEAP_DAEDALUS || (self.cutf_items & #CUTF_SENTRYGUN && self.weapons_carried & #WEAP_SPANNER) || (self.weapons_carried & #WEAP_SPANNER && self.tf_items & #NIT_TESLA) || (self.weapons_carried & #WEAP_SPANNER && self.tf_items & #NIT_SECURITY_CAMERA) || (self.weapons_carried & #WEAP_SPANNER && self.tf_items & #NIT_TELEPORTER) || (self.weapons_carried & #WEAP_SPANNER && self.cutf_items & #CUTF_FIELDGEN)))
		//if (!(self.cutf_items & #CUTF_CYBERAUG || self.cutf_items & #CUTF_HOLO || self.weapons_carried & #WEAP_LASERCANNON || self.weapons_carried & #WEAP_INCENDIARY || self.weapons_carried & #WEAP_LIGHTNING || self.weapons_carried & #WEAP_AIRF || self.weapons_carried & #WEAP_ASSAULT_CANNON || self.weapons_carried & #WEAP_DAEDALUS || (self.cutf_items & #CUTF_SENTRYGUN && self.weapons_carried & #WEAP_SPANNER) || (self.weapons_carried & #WEAP_SPANNER && self.tf_items & #NIT_TESLA) || (self.weapons_carried & #WEAP_SPANNER && self.tf_items & #NIT_SECURITY_CAMERA) || (self.weapons_carried & #WEAP_SPANNER && self.tf_items & #NIT_TELEPORTER) || (self.weapons_carried & #WEAP_SPANNER && self.cutf_items & #CUTF_FIELDGEN) || (self.weapons_carried & #WEAP_SPANNER && self.cutf_items & #CUTF_SENSOR)))
		if (!(self->cutf_items & (PR_CUTF_CYBERAUG | PR_CUTF_HOLO) || self->weapons_carried & (PR_WEAP_LASERCANNON | PR_WEAP_INCENDIARY | PR_WEAP_LIGHTNING | PR_WEAP_ASSAULT_CANNON | PR_WEAP_DAEDALUS | PR_WEAP_AIRF) || (self->cutf_items & PR_CUTF_SENTRYGUN && self->weapons_carried & PR_WEAP_SPANNER) || (self->weapons_carried & PR_WEAP_SPANNER && self->tf_items & PR_NIT_TESLA) || (self->weapons_carried & PR_WEAP_SPANNER && self->tf_items & PR_NIT_SECURITY_CAMERA) || (self->weapons_carried & PR_WEAP_SPANNER && self->tf_items & PR_NIT_TELEPORTER) || (self->weapons_carried & PR_WEAP_SPANNER && self->cutf_items & PR_CUTF_FIELDGEN) || (self->weapons_carried & PR_WEAP_SPANNER && self->cutf_items & PR_CUTF_SENSOR)))
		//if (!(self.weapons_carried & #WEAP_SPANER && (self.cutf_items & (CUTF_SENTRYGUN |   )  )))
			newmis->ammo_cells = self->ammo_cells;
	}

	// If there's nothing in the backpack, remove it and return
	if (!(newmis->ammo_shells + newmis->ammo_nails + newmis->ammo_rockets + newmis->ammo_cells))
	{
		dremove(newmis);
		return;
	}

	// Remove the ammo from the player
	if (newmis->ammo_shells)
		self->ammo_shells = 0;
	if (newmis->ammo_nails)
		self->ammo_nails = 0;
	if (newmis->ammo_rockets)
		self->ammo_rockets = 0;
	if (newmis->ammo_cells)
		self->ammo_cells = 0;
	W_SetCurrentAmmo();

	sound(self, PR_CHAN_ITEM, "weapons/lock4.wav", 1, PR_ATTN_NORM);

	// The backpack is treated as an ammobox, so people can't crash svrs
	// by making too many of them.
	if (self->team_no != 0)
	{
		increment_team_ammoboxes(self->team_no);
		if (num_team_ammoboxes(self->team_no) > (PR_MAX_WORLD_AMMOBOXES / number_of_teams))
			RemoveOldAmmobox(self->team_no);
	}
	else
	{
		num_world_ammoboxes = num_world_ammoboxes + 1;
		if (num_world_ammoboxes > PR_MAX_WORLD_AMMOBOXES)
			RemoveOldAmmobox(0);
	}

	// Throw the backpack
	newmis->enemy = self;
	newmis->health = time;
	newmis->weapon = 0;
	newmis->movetype = PR_MOVETYPE_TOSS;
	newmis->solid = PR_SOLID_TRIGGER;
	newmis->classname = "ammobox";
	newmis->team_no = self->team_no;
	makevectors (self->v_angle);
	if (self->v_angle[X])
	{
		newmis->velocity = v_forward*400 + v_up * 200;
	}
	else
	{
		newmis->velocity = aim(self, 10000);
		newmis->velocity = newmis->velocity * 400;
		newmis->velocity[Z] = 200;
	}
	newmis->avelocity = V({0, 300, 0});
	setsize (newmis, V({-16, -16, 0}), V({16, 16, 56}));
	setorigin (newmis, self->origin);
	newmis->nextthink = time + 30;	// remove after 30 seconds
	newmis->think = SUB_Remove;
	newmis->touch = TeamFortress_AmmoboxTouch;
	setmodel (newmis, "progs/backpack.mdl");

	// Gizmo - make sure it's not going to fall off the map, because of the new size
	newmis->owner = self;
	if ( InSolid( newmis ) ) {
		setorigin( newmis, self->origin - V({0, 0, 24}) );
	}
	newmis->owner = world;
}

//=========================================================================
// Shows any medics/engineers on your team that you need help.
// Spies see all requests.
void TeamFortress_SaveMe()
{
	entity te, tl;

	if (self->last_saveme_sound < time)
	{
		if (random() < 0.8)
			sound(self, PR_CHAN_WEAPON, "speech/saveme1.wav", 1, PR_ATTN_NORM); // MEDIC!
		else
			sound(self, PR_CHAN_WEAPON, "speech/saveme2.wav", 1, PR_ATTN_NORM); // Excuse me...
		self->last_saveme_sound = time + 4;
	}

	te = find(world, "classname", "player");
	while (te != world)
	{
		//if (self.weapons_carried & #WEAP_MEDIKIT) OfN WTF?
		  // Was ==, wtf?
		// PZ: I took out the below line. Let's always make sure people can see their own effect, and the effect of teammates.
		//if (self != te || te->weapons_carried & PR_WEAP_MEDIKIT || te->weapons_carried & PR_WEAP_SPANNER || te->cutf_items & PR_CUTF_SPY_KIT) //WK Custom class friendly
		{
			if (Teammate(self, te) || (te->cutf_items & PR_CUTF_SPY_KIT))
			{
				// If the other teammate is visible, show them I need help
				if (visible(te))
				{
					msg_entity = te;
					tl = spawn();
					tl->origin = self->origin;
					tl->origin[Z] = tl->origin[Z] + 32;
					WriteByte (PR_MSG_ONE, PR_SVC_TEMPENTITY);
					WriteByte (PR_MSG_ONE, PR_TE_LIGHTNING3);
					WriteEntity (PR_MSG_ONE, tl);
					WriteCoord (PR_MSG_ONE, tl->origin[X]);
					WriteCoord (PR_MSG_ONE, tl->origin[Y]);
					WriteCoord (PR_MSG_ONE, tl->origin[Z] + 24);
					WriteCoord (PR_MSG_ONE, self->origin[X]);
					WriteCoord (PR_MSG_ONE, self->origin[Y]);
					WriteCoord (PR_MSG_ONE, self->origin[Z]);
					dremove(tl);
				}
			}
		}

		te = find(te, "classname", "player");
	}
}

#ifdef PR_COOP_MODE_ENHANCED
/*
	ConstructString - Gizmo - made this function to make things easier
*/

string ConstructString(const string& s1, const string& s2, const string& s3, const string& s4, const string& s5, const string& s6, const string& s7, const string& s8)
{
	string ls;

	ls = strcat (s1, s2);

	if (s3 != string_null)
		ls = strcat (ls, s3);
	else
		return ls;

	if (s4 != string_null)
		ls = strcat (ls, s4);
	else
		return ls;

	if (s5 != string_null)
		ls = strcat (ls, s5);
	else
		return ls;

	if (s6 != string_null)
		ls = strcat (ls, s6);
	else
		return ls;

	if (s7 != string_null)
		ls = strcat (ls, s7);
	else
		return ls;

	if (s8 != string_null)
		ls = strcat (ls, s8);
	else
		return ls;

	return ls;
}
#endif

//=========================================================================
// ID's the player in your sights, and if you're a medic or engineer,
// reports their health and armor.
void TeamFortress_ID(float inAuto)
{
	//WK All the inAuto and spacer stuff I added
	vector src;
	string st, cls;
	string spacer;
	src = self->origin + v_forward*10;
	src[Z] = self->absmin[Z] + self->size[Z] * 0.7;
	if (!inAuto) //WK I.e. just someone is doing it normally
		traceline (src, src + v_forward*2048, PR_TL_EVERYTHING, self); //#TL_ANY_SOLID
	else		//WK I.e. it is an autoscan
		traceline (src, src + v_forward*1024, PR_TL_ANY_SOLID, self); //#TL_ANY_SOLID

	if (inAuto)
	{
		if (trace_ent->classname == "player" && trace_ent->job & (PR_JOB_THIEF + PR_JOB_ACTIVE))
			return;

		// Auto ID shouldn't pertubate menus
		if (self->current_menu != 0)
		   return;
	}

	//if (!inAuto)
		//spacer = "\n\n\n\n\n";
		spacer = "\n";
	//else
	//	spacer = "\n\n\n\n\n\n\n";

	if (trace_ent != world && trace_ent->origin != world->origin )
	{
		#ifdef PR_IDABLE_CORPSES
		if (trace_ent->PR_corpseflag == PR_STRFLAG_CORPSE)
		{
			self->StatusRefreshTime = time + 1.5;
			if (trace_ent->classname == "player")
			{
				if (trace_ent->playerclass == PR_PC_CUSTOM)
					cls = TeamFortress_GetJobName(trace_ent->job);
				else
					cls = TeamFortress_GetClassName(trace_ent->playerclass);

				if (Teammate(self, trace_ent))
					centerprint(self, spacer, trace_ent->netname, "'s corpse!\n\nNo ",cls," deserves this..");
				else
					centerprint(self, spacer, "This enemy ",cls," looks like\n\n",trace_ent->netname,"'s corpse!");

				return;
			}
			else if (trace_ent->classname == "monster_corpse")
			{
				if (trace_ent->PR_monster_type == PR_MONSTER_ARMY)
				{
					if (Teammate(self, trace_ent))
						centerprint(self, spacer, "Dead army comrade");
					else
						centerprint(self, spacer, "Dead enemy army guy");
				}
				else
				{
					if (Teammate(self, trace_ent))
						centerprint(self, spacer, "This good beast was helping");
					else
						centerprint(self, spacer, "This thing isn't alive\n\nIt smells badly though");
				}

				return;
			}

			// Player corpse on bodyque (unsafe to get name)
			if (Teammate(self, trace_ent))
				centerprint(self, spacer,"\nFallen teammate");
			else
				centerprint(self, spacer,"\nAn enemy rotten corpse");

			return;
		}
		else
		#endif
		#ifdef PR_IDABLE_TFITEMS
		if (trace_ent->classname == "item_tfgoal")
		{
			if (trace_ent->netname != "")
			{
				self->StatusRefreshTime = time + 1.5;
				centerprint(self,trace_ent->netname);
			}

			return;
		}
		else
		#endif
		#ifdef PR_IDABLE_RUNES
		if (trace_ent->classname == "rune")
		{
			self->StatusRefreshTime = time + 1.5;
			centerprint(self,trace_ent->netname);

			return;
		}
		else
		#endif
		/*if (trace_ent.classname == "grenade" && trace_ent.netname == "land_mine")
		{
			self.StatusRefreshTime = time + 1.5;
			if (Teammate(self.team_no,trace_ent.owner.team_no))
				centerprint(self,"Friendly Mine");
			else
				centerprint(self,"����� Mine");
			return;
		}*/
		if (trace_ent->classname == "player" && trace_ent->health > 0)
		{
			self->StatusRefreshTime = time + 1.5;

			if (Teammate(trace_ent, self))
			{
				if (trace_ent->playerclass == PR_PC_CUSTOM)
					cls = TeamFortress_GetJobName(trace_ent->job);
				else
					cls = TeamFortress_GetClassName(trace_ent->playerclass);

				if (self->weapons_carried & PR_WEAP_MEDIKIT) //WK
				{
					st = ftos(trace_ent->health);
					centerprint(self, spacer, trace_ent->netname, "\n\nFriendly ", cls, "\n\n", st, " health\n");
//centerprint(self, "\n\n\n\n", trace_ent.netname, "\nFriendly ", cls, "\n", st, //" health\n");
					return;
				}
				else if (self->weapons_carried & PR_WEAP_SPANNER) //WK
				{
					st = ftos(trace_ent->armorvalue);
					centerprint(self, spacer, trace_ent->netname, "\n\nFriendly ", cls, "\n\n", st, " armor\n");
					return;
				}
				else
					centerprint(self, spacer, trace_ent->netname, "\n\nFriendly ", cls,"\n");
				return;
			}

			if (trace_ent->is_feigning) //WK Can't id enemy spies feigning
				return;

			if (trace_ent->cutf_items & PR_CUTF_SPY_KIT)
			{
				cls = TeamFortress_GetClassName(trace_ent->undercover_skin);
				// Report a false name
				if (self->team_no != 0 && (self->team_no == trace_ent->undercover_team))
				{
					if (self->weapons_carried & PR_WEAP_MEDIKIT)
					{
						st = ftos(trace_ent->health);
						if (trace_ent->undercover_skin != 0)
							centerprint(self, spacer, trace_ent->undercover_name, "\n\nFriendly ", cls, "\n\n", st, " health\n");
						else
							centerprint(self, spacer, trace_ent->undercover_name, "\n\nFriendly Spy\n\n", st, " health\n");
						return;
					}
					else if (self->weapons_carried & PR_WEAP_SPANNER)
					{
						st = ftos(trace_ent->armorvalue);
						if (trace_ent->undercover_skin != 0)
							centerprint(self, spacer, trace_ent->undercover_name, "\n\nFriendly ", cls, "\n\n", st, " armor\n");
						else
							centerprint(self, spacer, trace_ent->undercover_name, "\n\nFriendly Spy\n\n", st, " armor\n");
						return;
					}
					else
					{
						st = ftos(trace_ent->armorvalue);
						if (trace_ent->undercover_skin != 0)
							centerprint(self, spacer, trace_ent->undercover_name, "\n\nFriendly ", cls, "\n");
						else
							centerprint(self, spacer, trace_ent->undercover_name, "\n\nFriendly Spy\n");
						return;
					}
				}

				if (trace_ent->undercover_name != string_null)
				{
					if (trace_ent->undercover_skin != 0)
						centerprint(self, spacer, trace_ent->undercover_name, S_("\n\n^bEnemy^b "), cls, "\n");
					else
						centerprint(self, spacer, trace_ent->undercover_name, S_("\n\n^bEnemy^b Spy"));
				}
				else
				{
					if (trace_ent->undercover_skin != 0)
						centerprint(self, spacer, trace_ent->netname, S_("\n\n^bEnemy^b "), cls, "\n");
					else
						centerprint(self, spacer, trace_ent->netname, S_("\n\n^bEnemy^b Spy"));
				}
			}
			else
			{
				if (trace_ent->playerclass == PR_PC_CUSTOM) {
					cls = TeamFortress_GetJobName(trace_ent->job);
					centerprint(self, spacer, trace_ent->netname, S_("\n\n^bEnemy^b "), cls, "\n");
				}
				else {
					cls = TeamFortress_GetClassName(trace_ent->playerclass);
					centerprint(self, spacer, trace_ent->netname, S_("\n\n^bEnemy^b "), cls, "\n");
				}
			}
		}
		else if (trace_ent->classname == "building_dispenser")
		{
			self->StatusRefreshTime = time + 1.5;

			if (self == trace_ent->real_owner)
				centerprint(self, spacer,"Your Dispenser\n");
			else if (!teamplay)
				centerprint(self, spacer,"Dispenser made by\n\n", trace_ent->real_owner->netname);
			else if (Teammate(trace_ent, self))
				centerprint(self, spacer,"Friendly Dispenser made by\n\n", trace_ent->real_owner->netname, "\n");
			else
				centerprint(self, spacer,S_("^bEnemy^b Dispenser made by\n\n"), trace_ent->real_owner->netname, "\n");

		}
		else if (trace_ent->classname == "building_fieldgen")
		{
			self->StatusRefreshTime = time + 1.5;

			if (self == trace_ent->real_owner)
				centerprint(self, spacer,"Your Field Generator.\n");
			else if (!teamplay)
				centerprint(self, spacer,"Field Generator made by\n\n", trace_ent->real_owner->netname);
			else if (Teammate(trace_ent, self))
				centerprint(self, spacer,"Friendly Field Generator made by\n\n", trace_ent->real_owner->netname, "\n");
			else
				centerprint(self, spacer,S_("^bEnemy^b Field Generator made by\n\n"), trace_ent->real_owner->netname, "\n");

		}

		else if (trace_ent->classname == "building_sentrygun" || trace_ent->classname == "building_sentrygun_base")
		{
			self->StatusRefreshTime = time + 1.5;
			if (self == trace_ent->real_owner)
			{
				if (self->cutf_items & PR_CUTF_DOUBLESENTRY)
				{
					if (trace_ent->dont_do_triggerwork == 1)
						centerprint(self, spacer,"Your Sentry Gun #1\n");
					else if (trace_ent->dont_do_triggerwork == 2)
						centerprint(self, spacer,"Your Sentry Gun #2\n");
					else
						centerprint(self, spacer,"Your Sentry Gun\n");
				}
				else
					centerprint(self, spacer,"Your Sentry Gun\n");
			}
			else if (!teamplay)
				centerprint(self, spacer,"Sentry Gun made by\n\n", trace_ent->real_owner->netname,"\n");
			else if (Teammate(trace_ent, self))
				centerprint(self, spacer,"Friendly Sentry Gun made by\n\n", trace_ent->real_owner->netname,"\n");
			else
				centerprint(self, spacer,S_("^bEnemy^b Sentry Gun made by\n\n"), trace_ent->real_owner->netname,"\n");
		}
		else if (trace_ent->classname == "building_tesla")
		{
			self->StatusRefreshTime = time + 1.5;
			if (self == trace_ent->real_owner)
			{
				if (self->tf_items & PR_NIT_DOUBLETESLA)
				{
					if (trace_ent->dont_do_triggerwork == 1)
						centerprint(self, spacer,"Your Tesla Coil #1\n");
					else if (trace_ent->dont_do_triggerwork == 2)
						centerprint(self, spacer,"Your Tesla Coil #2\n");
					else
						centerprint(self, spacer,"Your Tesla Coil\n");
				}
				else
					centerprint(self, spacer,"Your Tesla Coil\n");
			}
			else if (!teamplay)
				centerprint(self, spacer,"Tesla Coil made by\n\n", trace_ent->real_owner->netname,"\n");
			else if (Teammate(trace_ent, self))
				centerprint(self, spacer,"Friendly Tesla made by\n\n", trace_ent->real_owner->netname,"\n");
			else
				centerprint(self, spacer,S_("^bEnemy^b Tesla made by\n\n"), trace_ent->real_owner->netname,"\n");
		}
		else if (trace_ent->classname == "building_camera")
		{
			self->StatusRefreshTime = time + 1.5;
			if (self == trace_ent->real_owner)
				centerprint(self, spacer,"Your favorite Security Camera\n");
			else if (!teamplay)
				centerprint(self, spacer,"Security Camera made by\n\n", trace_ent->real_owner->netname,"\n");
			else if (Teammate(trace_ent, self))
				centerprint(self, spacer,"Friendly Security Camera made by\n\n", trace_ent->real_owner->netname,"\n");
			else
				centerprint(self, spacer,S_("^bEnemy^b Security Camera made by\n\n"), trace_ent->real_owner->netname,"\n");
		}
		else if (trace_ent->classname == "building_sensor")
		{
			self->StatusRefreshTime = time + 1.5;
			if (self == trace_ent->real_owner)
				centerprint(self, spacer,"Your favorite Motion Sensor\n");
			else if (!teamplay)
				centerprint(self, spacer,"Motion Sensor made by\n\n", trace_ent->real_owner->netname,"\n");
			else if (Teammate(trace_ent, self))
				centerprint(self, spacer,"Friendly Motion Sensor made by\n\n", trace_ent->real_owner->netname,"\n");
			else
				centerprint(self, spacer,S_("^bEnemy^b Motion Sensor made by\n\n"), trace_ent->real_owner->netname,"\n");
		}
		else if (trace_ent->classname == "building_teleporter")
		{
			self->StatusRefreshTime = time + 1.5;
			if (self == trace_ent->real_owner)
				centerprint(self, spacer,"One of your favorite Teleporter Pads\n"); //CH b/c you have 2
			else if (!teamplay)
				centerprint(self, spacer,"Teleporter Pad made by\n\n", trace_ent->real_owner->netname,"\n");
			else if (Teammate(trace_ent, self))
				centerprint(self, spacer,"Friendly Teleporter Pad made by\n\n", trace_ent->real_owner->netname,"\n");
			else
				centerprint(self, spacer,S_("^bEnemy^b Teleporter Pad made by\n\n"), trace_ent->real_owner->netname,"\n");
		}
		else if (trace_ent->classname == "monster_demon1")
		{
			//custom_demon_name(trace_ent); //CH
			self->StatusRefreshTime = time + 1.5;
			st = ftos(trace_ent->health);
			if (self == trace_ent->real_owner)
				centerprint(self, spacer,"Your Pet Demon ",trace_ent->netname, "\n\n", st, " health");
			else if (!teamplay)
				centerprint(self, spacer,"Demon summoned by\n\n", trace_ent->real_owner->netname);
			else if (Teammate(trace_ent->real_owner, self))
			{
				if (self->cutf_items & PR_CUTF_DEMONLORE)
					centerprint(self, spacer,trace_ent->netname,"\n\nfriendly demon summoned by\n\n", trace_ent->real_owner->netname, "\n\n", st, " health");
				else
					centerprint(self, spacer,trace_ent->netname,"\n\nfriendly demon summoned by\n\n", trace_ent->real_owner->netname);
			}
			else
			{
				if (self->cutf_items & PR_CUTF_DEMONLORE)
					centerprint(self, spacer,trace_ent->netname,S_("\n\n^benemy^b demon summoned by\n\n"), trace_ent->real_owner->netname, "\n\n", st, " health");
				else
					centerprint(self, spacer,trace_ent->netname,S_("\n\n^benemy^b demon summoned by\n\n"), trace_ent->real_owner->netname, "\n\n", st, " health");
			}
		}
		else if (trace_ent->classname == "monster_army")
		{
			//custom_demon_name(trace_ent); //CH
			self->StatusRefreshTime = time + 1.5;
			st = ftos(trace_ent->health);
			if (self == trace_ent->real_owner)
				centerprint(self, spacer,"Your mercenary soldier ",trace_ent->netname, "\n\n", st, " health");
			else if (!teamplay)
				centerprint(self, spacer,"Soldier in the pay of\n\n", trace_ent->real_owner->netname);
			else if (Teammate(trace_ent->real_owner, self))
			{
				if (self->cutf_items & PR_CUTF_DEMONLORE)
					centerprint(self, spacer,trace_ent->netname,"\n\nfriendly soldier owned by\n\n", trace_ent->real_owner->netname, "\n\n", st, " health");
				else
					centerprint(self, spacer,trace_ent->netname,"\n\nfriendly soldier owned by\n\n", trace_ent->real_owner->netname);
			}
			else
			{
				if (self->cutf_items & PR_CUTF_DEMONLORE)
					centerprint(self, spacer,trace_ent->netname,S_("\n\n^benemy^b soldier owned by\n\n"), trace_ent->real_owner->netname, "\n\n", st, " health");
				else
					centerprint(self, spacer,trace_ent->netname,S_("\n\n^benemy^b soldier owned by\n\n"), trace_ent->real_owner->netname);
			}
		}
		else if (trace_ent->classname == "monster_shambler")
		{
			//custom_demon_name(trace_ent); //CH
			self->StatusRefreshTime = time + 1.5;
			st = ftos(trace_ent->health);
			if (self == trace_ent->real_owner)
				centerprint(self, spacer,"Your Pet Shambler ",trace_ent->netname, "\n\n", st, " health");
			else if (!teamplay)
				centerprint(self, spacer,"Shambler under control of\n\n", trace_ent->real_owner->netname);
			else if (Teammate(trace_ent->real_owner, self))
			{
				if (self->cutf_items & PR_CUTF_DEMONLORE)
					centerprint(self, spacer,trace_ent->netname,"\n\nfriendly shambler summoned by\n\n", trace_ent->real_owner->netname, "\n\n", st, " health");
				else
					centerprint(self, spacer,trace_ent->netname,"\n\nfriendly shambler summoned by\n\n", trace_ent->real_owner->netname);
			}
			else
			{
				if (self->cutf_items & PR_CUTF_DEMONLORE)
					centerprint(self, spacer,trace_ent->netname,S_("\n\n^benemy^b shambler summoned by\n\n"), trace_ent->real_owner->netname, "\n\n", st, " health");
				else
					centerprint(self, spacer,trace_ent->netname,S_("\n\n^benemy^b shambler summoned by\n\n "), trace_ent->real_owner->netname);
			}
		}
		else if (trace_ent->classname == "monster_fish") //- OfN
		{
			//custom_demon_name(trace_ent);
			self->StatusRefreshTime = time + 1.5;
			st = ftos(trace_ent->health);
			if (self == trace_ent->real_owner)
				centerprint(self, spacer,"Your swimming Piranha ",trace_ent->netname, "\n\n", st, " health");
			else if (!teamplay)
				centerprint(self, spacer,"Piranha under control of\n\n", trace_ent->real_owner->netname);
			else if (Teammate(trace_ent->real_owner, self))
			{
				if (self->cutf_items & PR_CUTF_DEMONLORE)
					centerprint(self, spacer,trace_ent->netname,"\n\nfriendly Piranha summoned by\n\n", trace_ent->real_owner->netname, "\n\n", st, " health");
				else
					centerprint(self, spacer,trace_ent->netname,"\n\nfriendly Piranha summoned by\n\n", trace_ent->real_owner->netname);
			}
			else
			{
				if (self->cutf_items & PR_CUTF_DEMONLORE)
					centerprint(self, spacer,trace_ent->netname,S_("\n\n^benemy^b Piranha summoned by\n\n "), trace_ent->real_owner->netname, "\n\n", st, " health");
				else
					centerprint(self, spacer,trace_ent->netname,S_("\n\n^benemy^b Piranha summoned by\n\n"), trace_ent->real_owner->netname);
			}
		}
		else if (trace_ent->classname == "monster_gremlin") //- OfN
		{
			//custom_demon_name(trace_ent);
			self->StatusRefreshTime = time + 1.5;
			st = ftos(trace_ent->health);
			if (self == trace_ent->real_owner)
				centerprint(self, spacer,"Your dirty gremlin ",trace_ent->netname, "\n\n", st, " health");
			else if (!teamplay)
				centerprint(self, spacer,"Gremlin under control of\n\n", trace_ent->real_owner->netname);
			else if (Teammate(trace_ent->real_owner, self))
			{
				if (self->cutf_items & PR_CUTF_DEMONLORE)
					centerprint(self, spacer,trace_ent->netname,"\n\nfriendly Gremlin summoned by\n\n", trace_ent->real_owner->netname, "\n\n", st, " health");
				else
					centerprint(self, spacer,trace_ent->netname,"\n\nfriendly Gremlin summoned by\n\n", trace_ent->real_owner->netname);
			}
			else
			{
				if (self->cutf_items & PR_CUTF_DEMONLORE)
					centerprint(self, spacer,trace_ent->netname,S_("\n\n^benemy^b Gremlin summoned by\n\n "), trace_ent->real_owner->netname, "\n\n", st, " health");
				else
					centerprint(self, spacer,trace_ent->netname,S_("\n\n^benemy^b Gremlin summoned by\n\n"), trace_ent->real_owner->netname);
			}
		}
		else if (trace_ent->classname == "monster_wizard") //- OfN
		{
			//custom_demon_name(trace_ent);
			self->StatusRefreshTime = time + 1.5;
			st = ftos(trace_ent->health);
			if (self == trace_ent->real_owner)
				centerprint(self, spacer,"Your flying Scrag ",trace_ent->netname, "\n\n", st, " health");
			else if (!teamplay)
				centerprint(self, spacer,"Scrag under control of\n\n", trace_ent->real_owner->netname);
			else if (Teammate(trace_ent->real_owner, self))
			{
				if (self->cutf_items & PR_CUTF_DEMONLORE)
					centerprint(self, spacer,trace_ent->netname,"\n\nfriendly Scrag summoned by\n\n", trace_ent->real_owner->netname, "\n\n", st, " health");
				else
					centerprint(self, spacer,trace_ent->netname,"\n\nfriendly Scrag summoned by\n\n", trace_ent->real_owner->netname);
			}
			else
			{
				if (self->cutf_items & PR_CUTF_DEMONLORE)
					centerprint(self, spacer,trace_ent->netname,S_("\n\n^benemy^b Scrag summoned by\n\n "), trace_ent->real_owner->netname, "\n\n", st, " health");
				else
					centerprint(self, spacer,trace_ent->netname,S_("\n\n^benemy^b Scrag summoned by\n\n"), trace_ent->real_owner->netname);
			}
		}

#ifdef PR_COOP_MODE_ENHANCED
		else if ( COOP_IsCoopMonster( trace_ent ) ) {
			string s1;
			string bullet, nail, exp, elec, fire, melee;
			string strong, weak;
			string ar;

			// Gizmo - wouldn't this be easier to do this one time before these if statements?
			self->StatusRefreshTime = time + 1.5;
			st = ftos (trace_ent->health);
			ar = ftos (trace_ent->armorvalue);

			// Gizmo - print more information
			bullet = ftos(rint(trace_ent->PR_npc_bulletmult * 100));
			nail = ftos(rint(trace_ent->PR_npc_spikemult * 100));
			exp = ftos(rint(trace_ent->PR_npc_explosionmult * 100));
			elec = ftos(rint(trace_ent->PR_npc_electricitymult * 100));
			fire = ftos(rint(trace_ent->PR_npc_firemult * 100));
			melee = ftos(rint(trace_ent->PR_npc_meleemult * 100));
			//bullet = colstr (bullet, #COLSTR_NUMBER);
			//nail = colstr (nail, #COLSTR_NUMBER);
			//exp = colstr (exp, #COLSTR_NUMBER);
			//elec = colstr (elec, #COLSTR_NUMBER);
			//fire = colstr (fire, #COLSTR_NUMBER);
			//melee = colstr (melee, #COLSTR_NUMBER);

			s1 = string_null;
			strong = "Strong";
			weak = "Weak";
			if (trace_ent->PR_npc_bulletmult) {
				if (trace_ent->PR_npc_bulletmult < 0.95)
					s1 = ConstructString (strong, S_("^b/^b"), bullet, "%", S_(" against ^bbullets^b\n"), string_null, string_null, string_null);
			}
			if (trace_ent->PR_npc_spikemult) {
				if (trace_ent->PR_npc_spikemult < 0.95)
					s1 = ConstructString (s1, strong, S_("^b/^b"), nail, "%", S_(" against ^bnails^b\n"), string_null, string_null);
			}
			if (trace_ent->PR_npc_explosionmult) {
				if (trace_ent->PR_npc_explosionmult < 0.95)
					s1 = ConstructString (s1, strong, S_("^b/^b"), exp, "%", S_(" against ^bexplosions^b\n"), string_null, string_null);
			}
			if (trace_ent->PR_npc_electricitymult) {
				if (trace_ent->PR_npc_electricitymult < 0.95)
					s1 = ConstructString (s1, strong, S_("^b/^b"), elec, "%", S_(" against ^belectricity^b\n"), string_null, string_null);
			}
			if (trace_ent->PR_npc_firemult) {
				if (trace_ent->PR_npc_firemult < 0.95)
					s1 = ConstructString (s1, strong, S_("^b/^b"), fire, "%", S_(" against ^bfire^b\n"), string_null, string_null);
			}
			if (trace_ent->PR_npc_meleemult) {
				if (trace_ent->PR_npc_meleemult < 0.95)
					s1 = ConstructString (s1, strong, S_("^b/^b"), melee, "%", S_(" against ^bmelee attacks^b\n"), string_null, string_null);
			}

			if (s1 != string_null)
				s1 = strcat (s1, "\n");

			if (trace_ent->PR_npc_bulletmult) {
				if (trace_ent->PR_npc_bulletmult > 1)
					s1 = ConstructString (s1, weak, S_("^b/^b"), bullet, "%", S_(" against ^bbullets^b\n"), string_null, string_null);
			}
			if (trace_ent->PR_npc_spikemult) {
				if (trace_ent->PR_npc_spikemult > 1)
					s1 = ConstructString (s1, weak, S_("^b/^b"), nail, "%", S_(" against ^bnails^b\n"), string_null, string_null);
			}
			if (trace_ent->PR_npc_explosionmult) {
				if (trace_ent->PR_npc_explosionmult > 1)
					s1 = ConstructString (s1, weak, S_("^b/^b"), exp, "%", S_(" against ^bexplosions^b\n"), string_null, string_null);
			}
			if (trace_ent->PR_npc_electricitymult) {
				if (trace_ent->PR_npc_electricitymult > 1)
					s1 = ConstructString (s1, weak, S_("^b/^b"), elec, "%", S_(" against ^belectricity^b\n"), string_null, string_null);
			}
			if (trace_ent->PR_npc_firemult) {
				if (trace_ent->PR_npc_firemult > 1)
					s1 = ConstructString (s1, weak, S_("^b/^b"), fire, "%", S_(" against ^bfire^b\n"), string_null, string_null);
			}
			if (trace_ent->PR_npc_meleemult) {
				if (trace_ent->PR_npc_meleemult > 1)
					s1 = ConstructString (s1, weak, S_("^b/^b"), melee, "%", S_(" against ^bmelee attacks^b\n"), string_null, string_null);
			}
			//delstr (bullet);
			//delstr (nail);
			//delstr (exp);
			//delstr (elec);
			//delstr (fire);
			//delstr (melee);

			if (self->cutf_items & PR_CUTF_DEMONLORE) {
				s1 = ConstructString (ar, " armor\n\n", s1, string_null, string_null, string_null, string_null, string_null);
				if ( Teammate(trace_ent, self) )
					centerprint( self, "\nfriendly ", trace_ent->netname, "\n\n", st, " health\n", s1 );
				else
					centerprint( self, S_("\nAn ^benemy^b "), trace_ent->netname, "\n\n", st, " health\n", s1 );
			} else {
				if ( Teammate(trace_ent, self) )
					centerprint( self, "\nfriendly ", trace_ent->netname, "\n\n", s1 );
				else
					centerprint( self, S_("\nAn ^benemy^b "), trace_ent->netname, "\n\n", s1 );
			}

			// for when an observer
			// Gizmo - wtf? why not use time + ... ? using every frame is unreliable
			self->menu_count = 0;
		}
#endif
	}
}

//=========================================================================
// Reloads the current weapon
void TeamFortress_ReloadCurrentWeapon()
{
	float rt;
	entity tWeapon;

	// OfN - Psionized guys can't reload
	if (self->tfstate & PR_TFSTATE_PSIONIZED)
	{
		sprint(self,PR_PRINT_HIGH,"You can't reload right now!\n");
		return;
	}

	//---------------- CLIP EXTENDER (same as below but with _EX values -------------//
	if (self->cutf_items & PR_CUTF_CLIPEXTEND)
	{
		if (self->current_weapon == PR_WEAP_SHOTGUN)
		{
			if (self->reload_shotgun == 0)
			{
				sprint(self, PR_PRINT_HIGH, "Clip full.\n");
				return;
			}

			if ( self->reload_shotgun < self->ammo_shells)
			{
				Attack_Finished(0.4);

				// Calculate the reload time needed
				rt = (PR_RE_SHOTGUN_EX - self->reload_shotgun) / PR_RE_SHOTGUN_EX;
				rt = PR_RE_SHOTGUN_TIME_EX - (PR_RE_SHOTGUN_TIME_EX * rt);

				if (self->PR_runes & PR_RUNE_SPEED)
					rt = rt * PR_SPEEDRUNE_RELOADMULT;

				self->reload_shotgun = 0;
				if (self->ammo_shells < PR_RE_SHOTGUN_EX)
					self->reload_shotgun = PR_RE_SHOTGUN_EX - self->ammo_shells;

				sprint(self, PR_PRINT_HIGH, "reloading...\n");
				self->tfstate = (self->tfstate | PR_TFSTATE_RELOADING);
				tWeapon = spawnServerSide(); // PZ: make it a server-side only entity
				tWeapon->netname = "reloadtimer"; //WK For Judoka code
				tWeapon->owner = self;
				tWeapon->classname = "timer";
				tWeapon->nextthink = time + rt;
				tWeapon->think = W_Reload_shotgun;

				self->weaponmodel = "";
				self->weaponframe = 0;
			}
			else
			{
				sprint(self, PR_PRINT_HIGH, "not enough ammo to reload\n");
			}
		}
		else if (self->current_weapon == PR_WEAP_SUPER_SHOTGUN)
		{
			if (self->reload_super_shotgun == 0)
			{
				sprint (self, PR_PRINT_HIGH, "Clip full.\n");
				return;
			}

			if ( self->reload_super_shotgun < self->ammo_shells)
			{
				Attack_Finished(0.7);

				// Calculate the reload time needed
				rt = (PR_RE_SUPER_SHOTGUN_EX - self->reload_super_shotgun) / PR_RE_SUPER_SHOTGUN_EX;
				rt = PR_RE_SUPER_SHOTGUN_TIME_EX - (PR_RE_SUPER_SHOTGUN_TIME_EX * rt);

				if (self->PR_runes & PR_RUNE_SPEED)
					rt = rt * PR_SPEEDRUNE_RELOADMULT;

				self->reload_super_shotgun = 0;
				if (self->ammo_shells < PR_RE_SUPER_SHOTGUN_EX)
					self->reload_super_shotgun = PR_RE_SUPER_SHOTGUN_EX - self->ammo_shells;

				sprint (self, PR_PRINT_HIGH, "reloading...\n");
				self->tfstate = (self->tfstate | PR_TFSTATE_RELOADING);
				tWeapon = spawnServerSide(); // PZ: make it a server-side only entity
				tWeapon->netname = "reloadtimer"; //WK For Judoka code
				tWeapon->owner = self;
				tWeapon->classname = "timer";
				tWeapon->nextthink = time + rt;
				tWeapon->think = W_Reload_super_shotgun;

				self->weaponmodel = "";
				self->weaponframe = 0;
			}
			else
			{
				sprint (self, PR_PRINT_HIGH, "not enough ammo to reload\n");
			}
		}
		else if (self->current_weapon == PR_WEAP_LASERCANNON)
		{
			if (self->reload_laser_cannon == 0)
			{
				sprint (self, PR_PRINT_HIGH, "Cannon already in full charge.\n");
				return;
			}

			if ( self->reload_laser_cannon < self->ammo_cells)
			{
				Attack_Finished(2);

				// Calculate the reload time needed
				rt = (PR_RE_LASER_CANNON_EX - self->reload_laser_cannon) / PR_RE_LASER_CANNON_EX;
				rt = PR_RE_LASER_CANNON_TIME_EX - (PR_RE_LASER_CANNON_TIME_EX * rt);

				if (self->PR_runes & PR_RUNE_SPEED)
					rt = rt * PR_SPEEDRUNE_RELOADMULT;

				self->reload_laser_cannon = 0;
				if (self->ammo_cells < PR_RE_LASER_CANNON_EX)
					self->reload_laser_cannon = PR_RE_LASER_CANNON_EX - self->ammo_cells;

				sprint (self, PR_PRINT_HIGH, "Charging cannon...\n");
				self->tfstate = (self->tfstate | PR_TFSTATE_RELOADING);
				tWeapon = spawnServerSide(); // PZ: make it a server-side only entity
				tWeapon->netname = "reloadtimer"; //WK For Judoka code
				tWeapon->owner = self;
				tWeapon->classname = "timer";
				tWeapon->nextthink = time + rt;
				tWeapon->think = W_Reload_laser_cannon;

				self->weaponmodel = "";
				self->weaponframe = 0;
			}
			else
			{
				sprint (self, PR_PRINT_HIGH, "not enough cells to recharge\n");
			}
		}
		else if (self->current_weapon == PR_WEAP_LIGHT_ASSAULT)
		{
			if (self->reload_light_assault == 0)
			{
				sprint (self, PR_PRINT_HIGH, "Clip full.\n");
				return;
			}

			if ( self->reload_light_assault < self->ammo_nails)
			{
				Attack_Finished(2);

				// Calculate the reload time needed
				rt = (PR_RE_LIGHT_ASSAULT_EX - self->reload_light_assault) / PR_RE_LIGHT_ASSAULT_EX;
				rt = PR_RE_LIGHT_ASSAULT_TIME_EX - (PR_RE_LIGHT_ASSAULT_TIME_EX * rt);

				if (self->PR_runes & PR_RUNE_SPEED)
					rt = rt * PR_SPEEDRUNE_RELOADMULT;

				self->reload_light_assault = 0;
				if (self->ammo_nails < PR_RE_LIGHT_ASSAULT_EX)
					self->reload_light_assault = PR_RE_LIGHT_ASSAULT_EX - self->ammo_nails;

				sprint (self, PR_PRINT_HIGH, "reloading...\n");
				self->tfstate = (self->tfstate | PR_TFSTATE_RELOADING);
				tWeapon = spawnServerSide(); // PZ: make it a server-side only entity
				tWeapon->netname = "reloadtimer"; //WK For Judoka code
				tWeapon->owner = self;
				tWeapon->classname = "timer";
				tWeapon->nextthink = time + rt;
				tWeapon->think = W_Reload_light_assault;

				self->weaponmodel = "";
				self->weaponframe = 0;
			}
			else
			{
				sprint (self, PR_PRINT_HIGH, "not enough ammo to reload\n");
			}
		}
		else if (self->current_weapon == PR_WEAP_GRENADE_LAUNCHER)
		{
			if (self->reload_grenade_launcher == 0)
			{
				sprint (self, PR_PRINT_HIGH, "Clip full.\n");
				return;
			}

			if (self->reload_grenade_launcher < self->ammo_rockets)
			{
				Attack_Finished(0.6);

				// Calculate the reload time needed
				rt = (PR_RE_GRENADE_LAUNCHER_EX - self->reload_grenade_launcher) / PR_RE_GRENADE_LAUNCHER_EX;
				rt = PR_RE_GRENADE_LAUNCHER_TIME_EX - (PR_RE_GRENADE_LAUNCHER_TIME_EX * rt);

				if (self->PR_runes & PR_RUNE_SPEED)
					rt = rt * PR_SPEEDRUNE_RELOADMULT;

				self->reload_grenade_launcher = 0;
				if (self->ammo_rockets < PR_RE_GRENADE_LAUNCHER_EX)
					self->reload_grenade_launcher = PR_RE_GRENADE_LAUNCHER_EX - self->ammo_rockets;

				sprint (self, PR_PRINT_HIGH, "reloading...\n");
				self->tfstate = (self->tfstate | PR_TFSTATE_RELOADING);
				tWeapon = spawnServerSide(); // PZ: make it a server-side only entity
				tWeapon->netname = "reloadtimer"; //WK For Judoka code
				tWeapon->owner = self;
				tWeapon->classname = "timer";
				tWeapon->nextthink = time + rt;
				tWeapon->think = W_Reload_grenade_launcher;

				self->weaponmodel = "";
				self->weaponframe = 0;
			}
			else
			{
				sprint (self, PR_PRINT_HIGH, "not enough ammo to reload\n");
			}
		}
		else if (self->current_weapon == PR_WEAP_ROCKET_LAUNCHER)
		{
			if (self->reload_rocket_launcher == 0)
			{
				sprint (self, PR_PRINT_HIGH, "Clip full.\n");
				return;
			}

			if (self->reload_rocket_launcher < self->ammo_rockets)
			{
				Attack_Finished(0.8);

				// Calculate the reload time needed
				rt = (PR_RE_ROCKET_LAUNCHER_EX - self->reload_rocket_launcher) / PR_RE_ROCKET_LAUNCHER_EX;
				rt = PR_RE_ROCKET_LAUNCHER_TIME_EX - (PR_RE_ROCKET_LAUNCHER_TIME_EX * rt);

				if (self->PR_runes & PR_RUNE_SPEED)
					rt = rt * PR_SPEEDRUNE_RELOADMULT;

				self->reload_rocket_launcher = 0;

				if (self->ammo_rockets < PR_RE_ROCKET_LAUNCHER_EX)
					self->reload_rocket_launcher = PR_RE_ROCKET_LAUNCHER_EX - self->ammo_rockets;

				sprint (self, PR_PRINT_HIGH, "reloading...\n");
				self->tfstate = (self->tfstate | PR_TFSTATE_RELOADING);
				tWeapon = spawnServerSide(); // PZ: make it a server-side only entity
				tWeapon->netname = "reloadtimer"; //WK For Judoka code
				tWeapon->owner = self;
				tWeapon->classname = "timer";
				tWeapon->nextthink = time + rt;
				tWeapon->think = W_Reload_rocket_launcher;

				self->weaponmodel = "";
				self->weaponframe = 0;
			}
		}



	} // CLIP EXTENDER
	else
	{
		if (self->current_weapon == PR_WEAP_SHOTGUN)
		{
			if (self->reload_shotgun == 0)
			{
				sprint(self, PR_PRINT_HIGH, "Clip full.\n");
				return;
			}

			if ( self->reload_shotgun < self->ammo_shells)
			{
				Attack_Finished(0.4);

				// Calculate the reload time needed
				rt = (PR_RE_SHOTGUN - self->reload_shotgun) / PR_RE_SHOTGUN;
				rt = PR_RE_SHOTGUN_TIME - (PR_RE_SHOTGUN_TIME * rt);

				if (self->PR_runes & PR_RUNE_SPEED)
					rt = rt * PR_SPEEDRUNE_RELOADMULT;

				self->reload_shotgun = 0;
				if (self->ammo_shells < PR_RE_SHOTGUN)
					self->reload_shotgun = PR_RE_SHOTGUN - self->ammo_shells;

				sprint(self, PR_PRINT_HIGH, "reloading...\n");
				self->tfstate = (self->tfstate | PR_TFSTATE_RELOADING);
				tWeapon = spawnServerSide(); // PZ: make it a server-side only entity
				tWeapon->netname = "reloadtimer"; //WK For Judoka code
				tWeapon->owner = self;
				tWeapon->classname = "timer";
				tWeapon->nextthink = time + rt;
				tWeapon->think = W_Reload_shotgun;

				self->weaponmodel = "";
				self->weaponframe = 0;
			}
			else
			{
				sprint(self, PR_PRINT_HIGH, "not enough ammo to reload\n");
			}
		}
		else if (self->current_weapon == PR_WEAP_SUPER_SHOTGUN)
		{
			if (self->reload_super_shotgun == 0)
			{
				sprint (self, PR_PRINT_HIGH, "Clip full.\n");
				return;
			}

			if ( self->reload_super_shotgun < self->ammo_shells)
			{
				Attack_Finished(0.7);

				// Calculate the reload time needed
				rt = (PR_RE_SUPER_SHOTGUN - self->reload_super_shotgun) / PR_RE_SUPER_SHOTGUN;
				rt = PR_RE_SUPER_SHOTGUN_TIME - (PR_RE_SUPER_SHOTGUN_TIME * rt);

				if (self->PR_runes & PR_RUNE_SPEED)
					rt = rt * PR_SPEEDRUNE_RELOADMULT;

				self->reload_super_shotgun = 0;
				if (self->ammo_shells < PR_RE_SUPER_SHOTGUN)
					self->reload_super_shotgun = PR_RE_SUPER_SHOTGUN - self->ammo_shells;

				sprint (self, PR_PRINT_HIGH, "reloading...\n");
				self->tfstate = (self->tfstate | PR_TFSTATE_RELOADING);
				tWeapon = spawnServerSide(); // PZ: make it a server-side only entity
				tWeapon->netname = "reloadtimer"; //WK For Judoka code
				tWeapon->owner = self;
				tWeapon->classname = "timer";
				tWeapon->nextthink = time + rt;
				tWeapon->think = W_Reload_super_shotgun;

				self->weaponmodel = "";
				self->weaponframe = 0;
			}
			else
			{
				sprint (self, PR_PRINT_HIGH, "not enough ammo to reload\n");
			}
		}
		else if (self->current_weapon == PR_WEAP_LASERCANNON)
		{
			if (self->reload_laser_cannon == 0)
			{
				sprint (self, PR_PRINT_HIGH, "Cannon already in full charge.\n");
				return;
			}

			if ( self->reload_laser_cannon < self->ammo_cells)
			{
				Attack_Finished(2);

				// Calculate the reload time needed
				rt = (PR_RE_LASER_CANNON - self->reload_laser_cannon) / PR_RE_LASER_CANNON;
				rt = PR_RE_LASER_CANNON_TIME - (PR_RE_LASER_CANNON_TIME * rt);

				if (self->PR_runes & PR_RUNE_SPEED)
					rt = rt * PR_SPEEDRUNE_RELOADMULT;

				self->reload_laser_cannon = 0;
				if (self->ammo_cells < PR_RE_LASER_CANNON)
					self->reload_laser_cannon = PR_RE_LASER_CANNON - self->ammo_cells;

				sprint (self, PR_PRINT_HIGH, "Charging cannon...\n");
				self->tfstate = (self->tfstate | PR_TFSTATE_RELOADING);
				tWeapon = spawnServerSide(); // PZ: make it a server-side only entity
				tWeapon->netname = "reloadtimer"; //WK For Judoka code
				tWeapon->owner = self;
				tWeapon->classname = "timer";
				tWeapon->nextthink = time + rt;
				tWeapon->think = W_Reload_laser_cannon;

				self->weaponmodel = "";
				self->weaponframe = 0;
			}
			else
			{
				sprint (self, PR_PRINT_HIGH, "not enough cells to recharge\n");
			}
		}
		else if (self->current_weapon == PR_WEAP_LIGHT_ASSAULT)
		{
			if (self->reload_light_assault == 0)
			{
				sprint (self, PR_PRINT_HIGH, "Clip full.\n");
				return;
			}

			if ( self->reload_light_assault < self->ammo_nails)
			{
				Attack_Finished(2);

				// Calculate the reload time needed
				rt = (PR_RE_LIGHT_ASSAULT - self->reload_light_assault) / PR_RE_LIGHT_ASSAULT;
				rt = PR_RE_LIGHT_ASSAULT_TIME - (PR_RE_LIGHT_ASSAULT_TIME * rt);

				if (self->PR_runes & PR_RUNE_SPEED)
					rt = rt * PR_SPEEDRUNE_RELOADMULT;

				self->reload_light_assault = 0;
				if (self->ammo_nails < PR_RE_LIGHT_ASSAULT)
					self->reload_light_assault = PR_RE_LIGHT_ASSAULT - self->ammo_nails;

				sprint (self, PR_PRINT_HIGH, "reloading...\n");
				self->tfstate = (self->tfstate | PR_TFSTATE_RELOADING);
				tWeapon = spawnServerSide(); // PZ: make it a server-side only entity
				tWeapon->netname = "reloadtimer"; //WK For Judoka code
				tWeapon->owner = self;
				tWeapon->classname = "timer";
				tWeapon->nextthink = time + rt;
				tWeapon->think = W_Reload_light_assault;

				self->weaponmodel = "";
				self->weaponframe = 0;
			}
			else
			{
				sprint (self, PR_PRINT_HIGH, "not enough ammo to reload\n");
			}
		}
		else if (self->current_weapon == PR_WEAP_GRENADE_LAUNCHER)
		{
			if (self->reload_grenade_launcher == 0)
			{
				sprint (self, PR_PRINT_HIGH, "Clip full.\n");
				return;
			}

			if (self->reload_grenade_launcher < self->ammo_rockets)
			{
				Attack_Finished(0.6);

				// Calculate the reload time needed
				rt = (PR_RE_GRENADE_LAUNCHER - self->reload_grenade_launcher) / PR_RE_GRENADE_LAUNCHER;
				rt = PR_RE_GRENADE_LAUNCHER_TIME - (PR_RE_GRENADE_LAUNCHER_TIME * rt);

				if (self->PR_runes & PR_RUNE_SPEED)
					rt = rt * PR_SPEEDRUNE_RELOADMULT;

				self->reload_grenade_launcher = 0;
				if (self->ammo_rockets < PR_RE_GRENADE_LAUNCHER)
					self->reload_grenade_launcher = PR_RE_GRENADE_LAUNCHER - self->ammo_rockets;

				sprint (self, PR_PRINT_HIGH, "reloading...\n");
				self->tfstate = (self->tfstate | PR_TFSTATE_RELOADING);
				tWeapon = spawnServerSide(); // PZ: make it a server-side only entity
				tWeapon->netname = "reloadtimer"; //WK For Judoka code
				tWeapon->owner = self;
				tWeapon->classname = "timer";
				tWeapon->nextthink = time + rt;
				tWeapon->think = W_Reload_grenade_launcher;

				self->weaponmodel = "";
				self->weaponframe = 0;
			}
			else
			{
				sprint (self, PR_PRINT_HIGH, "not enough ammo to reload\n");
			}
		}
		else if (self->current_weapon == PR_WEAP_ROCKET_LAUNCHER)
		{
			if (self->reload_rocket_launcher == 0)
			{
				sprint (self, PR_PRINT_HIGH, "Clip full.\n");
				return;
			}

			if (self->reload_rocket_launcher < self->ammo_rockets)
			{
				Attack_Finished(0.8);

				// Calculate the reload time needed
				rt = (PR_RE_ROCKET_LAUNCHER - self->reload_rocket_launcher) / PR_RE_ROCKET_LAUNCHER;
				rt = PR_RE_ROCKET_LAUNCHER_TIME - (PR_RE_ROCKET_LAUNCHER_TIME * rt);

				if (self->PR_runes & PR_RUNE_SPEED)
					rt = rt * PR_SPEEDRUNE_RELOADMULT;

				self->reload_rocket_launcher = 0;

				if (self->ammo_rockets < PR_RE_ROCKET_LAUNCHER)
					self->reload_rocket_launcher = PR_RE_ROCKET_LAUNCHER - self->ammo_rockets;

				sprint (self, PR_PRINT_HIGH, "reloading...\n");
				self->tfstate = (self->tfstate | PR_TFSTATE_RELOADING);
				tWeapon = spawnServerSide(); // PZ: make it a server-side only entity
				tWeapon->netname = "reloadtimer"; //WK For Judoka code
				tWeapon->owner = self;
				tWeapon->classname = "timer";
				tWeapon->nextthink = time + rt;
				tWeapon->think = W_Reload_rocket_launcher;

				self->weaponmodel = "";
				self->weaponframe = 0;
			}
		}
	} // CLIP EXTENDER (END ELSE)
}

//CH drops an item TF goal if the bits are set to allow it.
float TeamFortress_DropItems()
{
	entity tg;
	float counter;

	counter = 0;
	//local string temp;

	tg = find (world, "classname", "item_tfgoal");
	while (tg != world)
	{
		if (tg->owner == self && tg != world && tg->classname == "item_tfgoal")
		{
			if (tg->goal_activation & PR_TFGR_DROPITEMS) // Grievre fix: was goal_result
			{
				if (tg->netname != "")
					sprint (self, PR_PRINT_HIGH, "Dropping item: ", tg->netname, "\n");

				counter = counter + 1;

				if (counter > 1)
					tfgoalitem_RemoveFromPlayer(tg, self, 0);
				else
					tfgoalitem_RemoveFromPlayer(tg, self, 2); // last param was 0, OfN added 2, which tosses item in player dir, see tfortmap.qc
//CH it treats it as though you died.  And if bits are set, you throw it, or it gets removed etc.
			}
		}

		tg = find(tg, "classname", "item_tfgoal");
	}

	return counter;
}

} // END namespace Progs
