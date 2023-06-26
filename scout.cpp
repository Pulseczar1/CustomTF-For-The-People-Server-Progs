/*====================================================
	SCOUT.QC

	TeamFortress v2.5	 29/2/97
	Craig Hauser	 26/3/00
======================================================
Functions for the SCOUT class and associated weaponry
======================================================*/

#include "progs.h"
#include "ofndefs.h"
#include "debug.h"
#include "tfort.h"
#include "idmonsters/common.h"
#include "sprites.h"
#include "player.h"
#include "misc.h"
#include "cpstuff.h"
#include "custom.h"
#include "weapons.h"

namespace Progs {

// Functions outside this file

// Functions inside this file
// Concussion Grenade Functions
void ConcussionGrenadeTouch();
void ConcussionGrenadeExplode();
void ConcussionGrenadeTimer();
// Scanner Functions
void TeamFortress_Scan(float scanrange,float inAuto);
void T_RadiusBounce(entity inflictor, entity attacker, float bounce, entity ignore);
entity T_RadiusScan(entity scanner, float scanrange, float enemies, float friends);

//void(entity pl, string s1) CenterPrint;
void StatusPrint(entity pl, float fTime, const string& s1);

// OfN External
//void(entity tfield, float timedisable) DisableField;
void MuzzleFlash(entity ent);

#ifdef PR_OLD_FLASH

//=========================================================================
// Touch Function for Flash Grenade
void FlashGrenadeTouch()
{
	// If the Flash Grenade hits a player, it just bounces off
	sound (self, PR_CHAN_WEAPON, "weapons/bounce.wav", 1, PR_ATTN_NORM);
	if (self->velocity == V({0, 0, 0}))
		self->avelocity = V({0, 0, 0});
}

#ifdef PR_NET_SERVER
	#define PR_SMOOTH_FLASH 1
#else
	#define PR_SMOOTH_FLASH 0.1
#endif

void FlashTimer()
{
	entity te;

	te = self->owner;

	if (!te->is_connected) //WK Safety, now that we call this to clean up after death
		return;

	te->FlashTime = te->FlashTime - PR_SMOOTH_FLASH;

	if (te->FlashTime < 4)
	{
		te->FlashTime = 0;
		stuffcmd(te, "v_cshift 0\n");
		stuffcmd(te, "r_norefresh 0;wait;echo;wait;echo;wait;echo;wait;echo\n"); //WK
		// OfN- now we use the centerprint replacement
		//centerprint(te,"Your 	 eyes 	 finally 	 clear\n");
		StatusPrint(te,2,"Your \x09 eyes \x09 finally \x09 clear\n");
		return;
	}

	string st;

	st = ftos(te->FlashTime * 15); //WK Multiplier 10

	stuffcmd(te, "v_cshift ");
	stuffcmd(te, st);
	stuffcmd(te, " ");
	stuffcmd(te, st);
	stuffcmd(te, " ");
	stuffcmd(te, st);
	stuffcmd(te, " ");
	stuffcmd(te, st);
	stuffcmd(te, "\n");
	if (te->FlashTime > 15 || (te->FlashTime < 12 && te->FlashTime > 11) || (te->FlashTime < 9 && te->FlashTime > 7) || te->FlashTime < 5)
		if (self->heat != 1) {
			StatusPrint(te,2,"Your \x09 eyes \x09 burn\n\x09\x09 with \x09\x09 the \x09\x09 pain\n\x09 of \x09\n glowing phosphor\n\x09\x09\x09 \x09\x09\x09 \x09\x09\x09\n\x09 \x09 \x09 \x09 \x09\n");
			stuffcmd(te, "r_norefresh 1;wait;echo;wait;echo;wait;echo;wait;echo\n");
			self->heat = 1;
		}
	else
		if (self->heat != 0) {
			StatusPrint(te,2,"Your \x09 eyes \x09 water\n\x09\x09 and \x09\x09 momentarily \x09\x09 lessen\n\x09 the \x09\n Indescribable Pain\n\x09\x09\x09 \x09\x09\x09 \x09\x09\x09\n\x09 \x09 \x09 \x09 \x09\n");
			stuffcmd(te, "r_norefresh 0;wait;echo;wait;echo;wait;echo;wait;echo\n");
			self->heat = 0;
		}


	self->nextthink = time + PR_SMOOTH_FLASH;
}

//=========================================================================
// Flash Grenade explode function, for when the PRIMETIME runs out
void FlashGrenadeExplode()
{
	//local float expsize;
	entity te;//, oldself;

	self->effects = self->effects | PR_EF_BRIGHTLIGHT;

	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_TAREXPLOSION);
	WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
#ifdef PR_QUAKE_WORLD
	multicast (self->origin, PR_MULTICAST_PHS);
#endif

	// Find all people in area
	te = findradius(self->origin, 200);
	while (te != world)
	{
		// Player?
		if (te->classname == "player" && te->health > 0)
		{
			// Damage player and explode
			deathmsg = PR_DMSG_GREN_FLASH;
			TF_T_Damage(te, self, self->owner, 60, 0, PR_TF_TD_FIRE);

			if (te->health > 0)
			{
				if (te->FlashTime == 0)
				{
					// create flash timer
					newmis = spawnServerSide(); // PZ: make it a server-side only entity

					newmis->classname = "timer";
					newmis->netname = "flashtimer";
					newmis->team_no = self->owner->team_no;
					newmis->owner = te;
					newmis->think = FlashTimer;
					newmis->nextthink = time + 1;
					newmis->heat = 1;
				}

				te->FlashTime = 16; //WK 24 for non-owners

				string st;

				st = ftos(te->FlashTime * 15);
				stuffcmd(te, "v_cshift ");
				stuffcmd(te, st);
				stuffcmd(te, " ");
				stuffcmd(te, st);
				stuffcmd(te, " ");
				stuffcmd(te, st);
				stuffcmd(te, " ");
				stuffcmd(te, st);
				stuffcmd(te, "\n");
				stuffcmd(te, "r_norefresh 1;wait;echo;wait;echo;wait;echo;wait;echo\n"); //WK
				StatusPrint(te,2,"Your \x09 eyes \x09 burn\n\x09\x09 with \x09\x09 the \x09\x09 pain\n\x09 of \x09\n glowing phosphor\n\x09\x09\x09 \x09\x09\x09 \x09\x09\x09\n\x09 \x09 \x09 \x09 \x09\n");
			}
		}

		te = te->chain;
	}

#ifdef PR_DEMO_STUFF
	// Remove any camera's locks on this missile
	if (self->enemy != world)
		CamProjectileLockOff();
#endif

#ifdef PR_QUAKE_WORLD
	dremove(self);
#else
	BecomeExplosion();
#endif
}

#else // Psionic/Flash

//=========================================================================
// Touch Function for Psionic Grenade
void PsionicGrenadeTouch()
{
	// If the psionic Grenade hits a player, it just bounces off
	sound (self, PR_CHAN_WEAPON, "weapons/bounce.wav", 1, PR_ATTN_NORM);
	if (self->velocity == V({0, 0, 0}))
		self->avelocity = V({0, 0, 0});
}

void PsionicTimer()
{
	entity te;

	te = self->owner;

	if (!te->is_connected) //WK Safety, now that we call this to clean up after death
	{
		dremove(self);
		return;
	}

	if (te->tfstate & PR_TFSTATE_PSIONIZED)
	{
		te->tfstate = te->tfstate - PR_TFSTATE_PSIONIZED;

		if (te->health > 0)
		{
			if (te->classname == "player")
			{
				if (random()<0.5)
					sound(te,PR_CHAN_VOICE,"player/gasp1.wav",1,PR_ATTN_NORM);
				else
					sound(te,PR_CHAN_VOICE,"player/gasp2.wav",1,PR_ATTN_NORM);

				TeamFortress_SetSpeed(te);
				sprint(te,PR_PRINT_HIGH,"Your psionic state is now restored\n");
			}

#ifdef PR_COOP_MODE_ENHANCED
			else if ( COOP_IsCoopMonster( te ) ) {
				if ( random() < 0.5 )
					sound( te, PR_CHAN_VOICE, "player/gasp1.wav", 1, PR_ATTN_NORM );
				else
					sound( te, PR_CHAN_VOICE, "player/gasp2.wav", 1, PR_ATTN_NORM );
			}
#endif
		}
	}

	dremove(self);
}

//=========================================================================
// Psionic Grenade explode function, for when the PRIMETIME runs out
void PsionicGrenadeExplode()
{
	entity te, te2;
	float psionicstime;
	string desc;

	if (self->netname == "special_mine")
	{
		psionicstime = PR_MINE_PSIONICS_TIME;
		desc = "mine";
	}
	else
	{
		psionicstime = PR_PSIONICS_TIME;
		desc = "grenade";
	}

	self->effects = self->effects | PR_EF_BRIGHTLIGHT;

	WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
	WriteByte (PR_MSG_BROADCAST, PR_TE_TAREXPLOSION);
	WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
#ifdef PR_QUAKE_WORLD
	multicast (self->origin, PR_MULTICAST_PHS);
#endif

	// TODO: explosion sound?

	// Find all people/sentries in area
	te = findradius(self->origin, PR_PSIONIC_EXPLOSIONRANGE);
	while (te != world)
	{
		// Player?
		if (te->classname == "player")
		{
			if (te->health > 0)
			if (te->is_connected)
			if (!(te->PR_runes & PR_RUNE_RESIS))
			if (!(te->done_custom & PR_CUSTOM_BUILDING)) // skip ppl customizing
			if (te->playerclass != PR_PC_UNDEFINED) // skip observers
			if (te->invincible_finished <= time)
			{
				// report our success on hiting someone
				if (te == self->owner)
					sprint(self->owner,PR_PRINT_HIGH,"Your psionic ",desc," affects yourself!\n");
				else
					sprint(self->owner,PR_PRINT_HIGH,"Your psionic ",desc," affects ",te->netname,"!\n");

				if (random()<0.5)
					sound(te,PR_CHAN_VOICE,"player/drown1.wav",1,PR_ATTN_NORM);
				else
					sound(te,PR_CHAN_VOICE,"player/drown2.wav",1,PR_ATTN_NORM);

				te2 = SpawnSprite(1,PR_SPRITE_ABLAST,te->origin,V({0, 0, 0}),PR_SPRITEMOVE_UP,0.1);

				if (te2 != world)
				{
					te2->effects = PR_EF_DIMLIGHT;

					if (te->team_no == 1)
						te2->effects = PR_EF_DIMLIGHT | PR_EF_BLUE;
					else if (te->team_no == 2)
						te2->effects = PR_EF_DIMLIGHT | PR_EF_RED;
				}

				MuzzleFlash(te);

				if (te->tfstate & PR_TFSTATE_PSIONIZED) // already psionized
				{
					// find the timer for psionics
					entity psion;

					psion = find(world,"netname","psionictimer");

					while (psion != world)
					{
						if (psion->owner == te)
						{
							// reset time
							psion->nextthink = time + psionicstime;
						}

						psion = find(psion,"netname","psionictimer");
					}

					sprint(te,PR_PRINT_HIGH,"You receive another psionic discharge!\n");
				}
				else // not psionized yet
				{
					// create psionic timer
					newmis = spawnServerSide(); // PZ: make it a server-side only entity

					newmis->classname = "timer";
					newmis->netname = "psionictimer";
					newmis->team_no = self->owner->team_no;
					newmis->owner = te;
					newmis->think = PsionicTimer;
					newmis->nextthink = time + psionicstime;

					te->tfstate = te->tfstate | PR_TFSTATE_PSIONIZED;

					te->PR_BUTTON_FIRE = 0;
					te->PR_BUTTON_USE = 0;
					te->PR_BUTTON_JUMP = 0;
					te->fire_held_down = PR_FALSE;
					te->heat = 0;

					entity oself;
					oself = self;

					self = te;

					if (self->current_weapon == PR_WEAP_ASSAULT_CANNON && self->fire_held_down)
					{
						stuffcmd(self, "-attack;v_idlescale 0\n");
						self->tfstate = self->tfstate - (self->tfstate & PR_TFSTATE_CANT_MOVE);

						self->weaponframe = 0;
						self->count = 1;

						player_assaultcannondown1();
					}
					else
						player_run();

					self = oself;

					sprint(te,PR_PRINT_HIGH,"Your body accepts a psionic discharge!\n");

					TeamFortress_SetSpeed(te);
				}
			}
		}

#ifdef PR_COOP_MODE_ENHANCED
		else if ( COOP_IsCoopMonster( te ) ) {
			if ( te->health > 0 ) {
				// report our success on hiting someone
				sprint( self->owner, PR_PRINT_HIGH, "Your psionic ", desc, " affects ", te->netname, "!\n" );

				if ( random() < 0.5 )
					sound( te, PR_CHAN_VOICE, "player/drown1.wav", 1, PR_ATTN_NORM );
				else
					sound( te, PR_CHAN_VOICE, "player/drown2.wav", 1, PR_ATTN_NORM );

				te2 = SpawnSprite( 1, PR_SPRITE_ABLAST, te->origin, V({0, 0, 0}), PR_SPRITEMOVE_UP, 0.1 );

				if ( te2  != world) {
					te2->effects = PR_EF_DIMLIGHT;

					if ( te->team_no == 1 )
						te2->effects = PR_EF_DIMLIGHT | PR_EF_BLUE;
					else if ( te->team_no == 2 )
						te2->effects = PR_EF_DIMLIGHT | PR_EF_RED;
				}

				MuzzleFlash( te );

				if ( te->tfstate & PR_TFSTATE_PSIONIZED ) { // already psionized
					// find the timer for psionics
					entity psion;

					psion = find( world, "netname", "psionictimer" );

					while ( psion != world ) {
						if ( psion->owner == te ) {
							// reset time
							psion->nextthink = time + psionicstime;
						}

						psion = find( psion, "netname", "psionictimer" );
					}
				} else { // not psionized yet
					// create psionic timer
					newmis = spawnServerSide(); // PZ: make it a server-side only entity

					newmis->classname = "timer";
					newmis->netname = "psionictimer";
					newmis->team_no = self->owner->team_no;
					newmis->owner = te;
					newmis->think = PsionicTimer;
					newmis->nextthink = time + psionicstime;

					te->tfstate = te->tfstate | PR_TFSTATE_PSIONIZED;
				}
			}
		}
#endif

		te = te->chain;
	}

#ifdef PR_DEMO_STUFF
	// Remove any camera's locks on this missile
	if (self->enemy != world)
		CamProjectileLockOff();
#endif

#ifdef PR_QUAKE_WORLD
	dremove(self);
#else
	BecomeExplosion();
#endif
}


#endif // Psionic/Flash



//=========================================================================
// Touch function for a concussion grenade
void ConcussionGrenadeTouch()
{
	// concussion grenades bounce off other players now

	sound (self, PR_CHAN_WEAPON, "weapons/bounce.wav", 1, PR_ATTN_NORM);	// bounce sound
	if (self->velocity == V({0, 0, 0}))
		self->avelocity = V({0, 0, 0});
}

//=========================================================================
// Concussion grenade explosion function
void ConcussionGrenadeExplode()
{
	T_RadiusBounce (self, self->owner, 240, world);

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
	dremove(self);
#else
	BecomeExplosion ();
#endif
}

//=========================================================================
// Concussion grenade timer to remove idlescale
void ConcussionGrenadeTimer()
{
	string st;

	if (self->owner->invincible_finished > time)
	{
		stuffcmd(self->owner, "v_idlescale 0\n");
		self->owner->tfstate = self->owner->tfstate - (self->owner->tfstate & PR_TFSTATE_CONCUSSIONED); // PZ: added this flag for concussing bots
		dremove(self);
		return;
	}

	// Bubble
	newmis = spawn();
	setmodel (newmis, "progs/s_bubble.spr");
	setorigin (newmis, self->owner->origin);
	newmis->movetype = PR_MOVETYPE_NOCLIP;
	newmis->solid = PR_SOLID_NOT;
	newmis->velocity = V({0, 0, 15});
	newmis->nextthink = time + 0.5;
	newmis->think = bubble_bob;
	newmis->touch = bubble_remove;
	newmis->classname = "bubble";
	newmis->frame = 0;
	newmis->cnt = 0;
	setsize(newmis, V({-8, -8, -8}), V({8, 8, 8}));

	self->health = self->health - PR_GR_CONCUSS_DEC;

	// medic recovers twice as fast
	// WK ...and so do thieves and gymnasts
	if (self->owner->weapons_carried & PR_WEAP_MEDIKIT) //WK
		self->health = self->health - PR_GR_CONCUSS_DEC;
	if (self->owner->cutf_items & PR_CUTF_STEALTH)
		self->health = self->health - PR_GR_CONCUSS_DEC;
	if (self->owner->cutf_items & PR_CUTF_GYMNAST)
		self->health = self->health - PR_GR_CONCUSS_DEC;

	if (self->health < 0)
		self->health = 0;
	self->nextthink = time + PR_GR_CONCUSS_TIME;

	st = ftos(self->health);
	stuffcmd(self->owner, "v_idlescale ");
	stuffcmd(self->owner, st);
	stuffcmd(self->owner, "\n");

	if (self->health == 0)
	{
		self->owner->tfstate = self->owner->tfstate - (self->owner->tfstate & PR_TFSTATE_CONCUSSIONED); // PZ: added this flag for concussing bots
		dremove(self);
	}
}

//=========================================================================
// Handles the scanner function for Scouts
void TeamFortress_Scan(float scanrange,float inAuto)
{
	string power;
	entity list;
	float scen, scfr;
	vector lightningvec;
	float temp;

	// added in for the direction scanner code
	//float enemy_detected;
	float any_detected;

	//local vector vf, vr, e;  // transformed versions of v_forward, v_right and the enemy vector
	//local float res1, res2, res3; // for the vector work
	//local float vf_e_angle, vr_e_angle; // results

	// prevent scan impulse from triggering anything else
	self->impulse = 0;
	self->last_impulse = 0;

	if (self->classname == "player")
	{
		if (!(self->tf_items & PR_NIT_SCANNER))
			return;

		// If Impulse is #TF_SCAN_ENEMY, toggle Scanning for Enemies
		if (scanrange == PR_TF_SCAN_ENEMY)
		{
			if (self->tf_items_flags & PR_NIT_SCANNER_ENEMY)
			{
				sprint (self, PR_PRINT_HIGH, "Enemy Scanning disabled.\n");
				self->tf_items_flags = self->tf_items_flags - PR_NIT_SCANNER_ENEMY;
				return;
			}
			sprint (self, PR_PRINT_HIGH, "Enemy Scanning enabled.\n");
			self->tf_items_flags = self->tf_items_flags | PR_NIT_SCANNER_ENEMY;
			return;
		}

		// If Impulse is #TF_SCAN_FRIENDLY, toggle Scanning for Friendlies
		if (scanrange == PR_TF_SCAN_FRIENDLY)
		{
			if (self->tf_items_flags & PR_NIT_SCANNER_FRIENDLY)
			{
				sprint (self, PR_PRINT_HIGH, "Friendly Scanning disabled.\n");
				self->tf_items_flags = self->tf_items_flags - PR_NIT_SCANNER_FRIENDLY;
				return;
			}
			sprint (self, PR_PRINT_HIGH, "Friendly Scanning enabled.\n");
			self->tf_items_flags = self->tf_items_flags | PR_NIT_SCANNER_FRIENDLY;
			return;
		}

		// If the user doesn't have as many cells as he/she specified, just
		// use as many as they've got.

/*		local float scancost;

		scancost = ceil(scanrange / 20);

		if (scancost > self.ammo_cells)
		{
			scanrange = self.ammo_cells * 20;
			scancost = self.ammo_cells;
		}

		if (scanrange <= 0)
		{
			sprint(self, #PRINT_HIGH, "No cells.\n");
			return;
		}
*/
		if (scanrange > PR_NIT_SCANNER_MAXCELL)
			scanrange = PR_NIT_SCANNER_MAXCELL;

		scen = 0;
		scfr = 0;
		// Set the Scanner flags
		if (self->tf_items_flags & PR_NIT_SCANNER_ENEMY)
			scen = 1;
		if (self->tf_items_flags & PR_NIT_SCANNER_FRIENDLY)
			scfr = 1;

		// If no entity type is enabled, don't scan
		if ((scen == 0) && (scfr == 0))
		{
			sprint(self, PR_PRINT_HIGH, "All scanner functions are disabled.\nEnable with 'scane' or 'scanf'.\n");
			return;
		}

		// Use up cells to power the scanner
		// additions:
		// altered this so scanner could be more easily tested
//WK		self.ammo_cells = self.ammo_cells - scancost;
		scanrange = scanrange * PR_NIT_SCANNER_POWER;

		if (!inAuto) { //WK Only sprint() if not autoscanner
			sprint (self, PR_PRINT_HIGH, "Range: ");
			power = ftos(ceil(scanrange));
			sprint (self, PR_PRINT_HIGH, power);
			sprint (self, PR_PRINT_HIGH, ". Scanning...\n");
		}

		// Get the list of entities the scanner finds
		list = T_RadiusScan(self, scanrange, scen, scfr);
	} else
		list = world;
	// Base Defence scanning code here

	// Reset the entity counts
	scen = 0;
	scfr = 0;

	// the vectors v_forward and v_right are required to
	// 'triangulate' the enemies position
	makevectors(self->v_angle);

	// Walk the list
	// For now, just count the entities.
	// In the future, we'll display bearings :)
	// additions: the future is now!
	while (list != world)
	{
	if (list != self)
	{
		// sets the enemy_detected flag to #TRUE if not on your team, #FALSE if so
		any_detected = PR_TRUE; // this flag is set to false if bogie is moving
							 // too slow to be detected (and velocity checking is on)

		if (vlen(list->origin - self->origin) <= scanrange) //CH Secondary check NEEDED!!!
		{
			// If this scanner is a motion detector, don't record
			// object that don't have the required velocity to be detected.
			if (self->tf_items_flags & PR_NIT_SCANNER_MOVEMENT)
			{
				if (vlen(list->velocity) > PR_NIT_SCANNER_MIN_MOVEMENT)
				{
					// Gizmo - allow non demons and non army soldiers to be affected (coop monsters)
					if (IsMonster(list))
					{
						if (list->health > 0)
						{
							temp = GetTeam( list );
							if (Teammate(temp, self))
							{
								scfr = scfr + 1;
								//enemy_detected = PR_FALSE;
							}
							else
							{
								scen = scen + 1;
								//enemy_detected = PR_TRUE;
							}
						}
					}
					else if (list->classname == "item_tfgoal" && list->owned_by > 0 && list->team_no > 0 && self->team_no > 0) //Because they use owned_by
					{
						if (list->owned_by == self->team_no && list->team_no != self->team_no)
						{
							scfr = scfr + 1;
							//enemy_detected = PR_FALSE;
						}
						else if (list->owned_by == self->team_no && list->team_no == self->team_no)
						{
							scfr = scfr + 1;
							//enemy_detected = PR_FALSE;
						}
						else if (list->owned_by != self->team_no && list->team_no == self->team_no)
						{
							scen = scen + 1;
							//enemy_detected = PR_TRUE;
						}
						else
							any_detected = PR_FALSE;
					}
					else if ((list->classname == "player" || list->classname == "building_sentrygun" || list->classname == "building_tesla" || list->classname == "building_teleporter") && list->health > 0 && !(list->cutf_items & PR_CUTF_JAMMER))
					{
						if (Teammate(list, self))
						{
							scfr = scfr + 1;
							//enemy_detected = PR_FALSE;
						}
						else
						{
							scen = scen + 1;
							//enemy_detected = PR_TRUE;
						}
					}
					else
					{
						any_detected = PR_FALSE;
					}
				}
				else
				{
					any_detected = PR_FALSE;
				}
			}
			else
			{
				// Gizmo - allow non demons and non army soldiers to be affected (coop monsters)
				if (IsMonster(list))
				{
					if (list->health > 0)
					{
						temp = GetTeam( list );
						if (Teammate(temp, self))
						{
							scfr = scfr + 1;
							//enemy_detected = PR_FALSE;
						}
						else
						{
							scen = scen + 1;
							//enemy_detected = PR_TRUE;
						}
					}
				}
				else if (list->classname == "item_tfgoal" && list->owned_by > 0 && list->team_no > 0 && self->team_no > 0) //Because they use owned_by
				{
					if (list->owned_by == self->team_no && list->team_no != self->team_no)
					{
						scfr = scfr + 1;
						//enemy_detected = PR_FALSE;
					}
					else if (list->owned_by == self->team_no && list->team_no == self->team_no)
					{
						scfr = scfr + 1;
						//enemy_detected = PR_FALSE;
					}
					else if (list->owned_by != self->team_no && list->team_no == self->team_no)
					{
						scen = scen + 1;
						//enemy_detected = PR_TRUE;
					}
					else
						any_detected = PR_FALSE;
				}
				else if ((list->classname == "player" || list->classname == "building_sentrygun" || list->classname == "building_tesla" || list->classname == "building_teleporter") && list->health > 0 && !(list->cutf_items & PR_CUTF_JAMMER))
				{
					if (Teammate(list, self))
					{
						scfr = scfr + 1;
						//enemy_detected = PR_FALSE;
					}
					else
					{
						scen = scen + 1;
						//enemy_detected = PR_TRUE;
					}
				}
				else
				{
					any_detected = PR_FALSE;
				}
			}
		}
		else
		{
			any_detected = PR_FALSE;
		}

		// this displays the direction of the detected player
		// using the cosine rule to find the angle
		//	cos theta = A.B divided by |A||B|
		// it should return a value between 1 and -1
		if (any_detected)
		{
			// Gizmo - don't allow too many lightnings (if more than one actually does anything)
			// if too many get sent, they seem to get cut off in the middle and corrupt the protocol
			if ( scen + scfr <= 4 ) {
				// Get the unit vector
				lightningvec = normalize(list->origin - self->origin);
				lightningvec = lightningvec * (vlen(list->origin - self->origin) / 5);
				lightningvec = lightningvec + self->origin;

				// Create the Lightning
				msg_entity = self;
				WriteByte (PR_MSG_ONE, PR_SVC_TEMPENTITY);
				WriteByte (PR_MSG_ONE, PR_TE_LIGHTNING1);
				WriteEntity (PR_MSG_ONE, self);
				WriteCoord (PR_MSG_ONE, self->origin[X]);
				WriteCoord (PR_MSG_ONE, self->origin[Y]);
				WriteCoord (PR_MSG_ONE, self->origin[Z] + 8);
				WriteCoord (PR_MSG_ONE, lightningvec[X]);
				WriteCoord (PR_MSG_ONE, lightningvec[Y]);
				WriteCoord (PR_MSG_ONE, lightningvec[Z] + 8);
			}

			self->scaned = list; //CH for the sbar
			if (!inAuto)
			{
				self->StatusRefreshTime = time + 0.2;
				self->StatusBarScreen = 5;
			}
		} // end if(any_detected)
	}
		list = list->linked_list;
	}

	// Display the counts
	// For Base Defences, it will display the counts to all team members
	if ((scen == 0) && (scfr == 0) && (!inAuto))
	{
		sprint (self, PR_PRINT_HIGH, "No blips.\n");
		return;
	}

	// Update ammo levels
	//W_SetCurrentAmmo ();

	return;
}

//=========================================================================
// Acts just like T_RadiusDamage, but doesn't damage things, just pushes them away
// from the explosion at a speed relative to the distance from the explosion's origin.
void T_RadiusBounce(entity inflictor, entity attacker, float bounce, entity ignore)
{
	float	points;
	entity	head, te;
	vector	org;
	//local string	st;

	head = findradius(inflictor->origin, bounce+40);

	while (head != world)
	{
		if (head != ignore)
		{
			if (head->takedamage && head->health > 0)
			{
				org = head->origin + (head->mins + head->maxs)*0.5;
				points = 0.5*vlen (org - inflictor->origin);
				if (points < 0)
					points = 0;
				points = bounce - points;
				if (head->cutf_items & PR_CUTF_GYMNAST)
					points = points * 2;

				if (!IsBuilding(head) && points > 0)
				{
					// Bounce!!
					head->velocity = org - inflictor->origin;
					head->velocity = head->velocity * (points / 20);

					if (head->classname != "player")
					{
						if(head->flags & PR_FL_ONGROUND)
							head->flags = head->flags - PR_FL_ONGROUND;
					}
					else
					{
						if (!(head->PR_runes & PR_RUNE_RESIS))
						{
							//WK Add cheat immunity since they fly
							makeImmune(head,time+3);

							// Concuss 'em!!
							// If they are already concussed, set the concussion back up
							// Try to find a concusstimer entity for this player
							te = find(world, "classname", "timer");
							while (((te->owner != head) || (te->think != ConcussionGrenadeTimer)) && (te != world))
								te = find(te, "classname", "timer");
							if (te != world)
							{
								stuffcmd(head,"v_idlescale 100\n");
								te->health = 100;
								te->nextthink = time + PR_GR_CONCUSS_TIME;
							}
							else
							{
								stuffcmd(head,"v_idlescale 100\n");
								stuffcmd(head,"bf\n");
								// Create a timer entity
								te = spawnServerSide(); // PZ: make it a server-side only entity
								te->nextthink = time + PR_GR_CONCUSS_TIME;
								te->think = ConcussionGrenadeTimer;
								te->team_no = attacker->team_no;
								te->classname = "timer";
								te->owner = head;
								te->health = 100;
								head->tfstate = head->tfstate | PR_TFSTATE_CONCUSSIONED; // PZ: for some reason, this flag was not set here
							}
						}
					}
				}
			}
		}
		head = head->chain;
	}
}
//CH checks a player and returns True of False
float Scanner_Check_Player(entity scan, entity targ, float enemies, float friends)
{
		if (targ->playerclass == PR_PC_UNDEFINED) {
			return PR_FALSE;
		}
		else if (targ->done_custom & PR_CUSTOM_BUILDING) {
			return PR_FALSE;
		}
		else if (targ->health <= 0) {
			return PR_FALSE;
		}
		else if (!targ->is_connected) {
			return PR_FALSE;
		}
		else if (targ == scan) {
			return PR_FALSE;
		}
		else if (targ->flags & PR_FL_NOTARGET) {
			return PR_FALSE;
		}
		else if (targ->cutf_items & PR_CUTF_JAMMER)
			return PR_FALSE;
//CH ALL NEW CHECKS ABOVE THIS LINE
		if (teamplay)
		{
			if ( friends &&  Teammate(targ, scan) )
				return PR_TRUE;
			if ( enemies && !Teammate(targ, scan) )
				return PR_TRUE;
		}
		else
			return PR_TRUE;
	return PR_FALSE;
}
//=========================================================================
// Returns a list of players within a radius around the origin, like findradius,
// except that some parsing of the list can be done based on the parameters passed in.
// Make sure you check that the return value is not NULL b4 using it.
entity T_RadiusScan(entity scanner, float scanrange, float enemies, float friends)
{
	entity head;
	entity list_head;
	entity list;
	float gotatarget;
	float temp;

	list = world;
	list_head = world;

	head = findradius(scanner->origin, scanrange+40);

	while (head != world)
	{
		gotatarget = 0;
		if (head != scanner && (friends || enemies))	// Don't pick up the entity that's scanning
		{
			if (head->takedamage && head->health > 0) //item_tfgoal does not take dammage
			{
				if (head->classname == "player")
				{
					gotatarget = Scanner_Check_Player(scanner, head, enemies, friends);
				}
				if ((head->classname == "building_tesla" || head->classname == "building_sentrygun" || head->classname == "building_teleporter") && (head->health > 0)) //CH uses team_no :)
				{
					if (teamplay)
					{
						if ( friends &&  Teammate(head, scanner) )
							gotatarget = 1;
						if ( enemies && !Teammate(head, scanner) )
							gotatarget = 1;
					}
					else
						gotatarget = 1;
				}
				else if ( IsMonster( head ) && head->health > 0 ) {	// Gizmo - allow non demons and non army soldiers to be affected (coop monsters)
					if ( teamplay ) {
						temp = GetTeam( head );
						if ( friends && Teammate(temp, scanner) )
							gotatarget = 1;
						if ( enemies && !Teammate(temp, scanner) )
							gotatarget = 1;
					} else
						gotatarget = 1;
				}
			}
			else if (head->classname == "item_tfgoal") //CH flags used owned_by for what team it is
			{
				if (teamplay)
				{
					if ( friends && (head->team_no > 0) && (head->owned_by > 0) && (scanner->team_no > 0) && (head->team_no == scanner->team_no) && (head->owned_by == scanner->team_no) )
						gotatarget = 1;
					if ( friends && (head->team_no > 0) && (head->owned_by > 0) && (scanner->team_no > 0) && (head->team_no != scanner->team_no) && (head->owned_by == scanner->team_no) )
						gotatarget = 1;
					if ( enemies && (head->team_no > 0) && (head->owned_by > 0) && (scanner->team_no > 0) && (head->team_no == scanner->team_no) && (head->owned_by != scanner->team_no) )
						gotatarget = 1;
				}
				else
					gotatarget = 1;
			}
		}

		// Add this entity to the linked list if it matches the target criteria
		if (gotatarget)
		{
			if (list != world)
			{
				list->linked_list = head;
				list = list->linked_list;
			}
			else
			{
				list_head = head;
				list = head;
			}
		}

		head = head->chain;
	}

	return list_head;
}

//=========================================================================
// Caltrop Grenade explosion

#define PR_GR_TYPE_CALTROP_NO 5

void CreateCaltrop(const vector& org, entity shooter);
void CaltropTouch();

void CaltropGrenadeExplode()
{
	float i;

/*	deathmsg = DMSG_GREN_CALTROP;
	T_RadiusDamage (self, self.owner, 50, world);

	WriteByte (#MSG_BROADCAST, #SVC_TEMPENTITY);
	WriteByte (#MSG_BROADCAST, #TE_EXPLOSION);
	WriteCoord (#MSG_BROADCAST, self.origin_x);
	WriteCoord (#MSG_BROADCAST, self.origin_y);
	WriteCoord (#MSG_BROADCAST, self.origin_z);
	#ifdef QUAKE_WORLD
		multicast (self.origin, #MULTICAST_PHS);
	#endif

	self.solid = #SOLID_NOT;
	*/
	// Very well admiral. Engage the rebels.
	i = 0;
	while (i < PR_GR_TYPE_CALTROP_NO)
	{
		CreateCaltrop(self->origin + V({0, 0, -1}),self->owner);
		i = i + 1;
	}

	#ifdef PR_DEMO_STUFF
		// Remove any camera's locks on this missile
		if (self->enemy != world)
			CamProjectileLockOff();
	#endif

	//BecomeExplosion();
	dremove(self);
}

void CreateCaltrop(const vector& org, entity shooter)
{
	float xdir,ydir,zdir;

	xdir = 80 * random() - 40;
	ydir = 80 * random() - 40;
	zdir = 15 + 15 * random();

	newmis = spawn ();
	newmis->owner = shooter;
	newmis->movetype = PR_MOVETYPE_BOUNCE;
	newmis->solid = PR_SOLID_TRIGGER;
//	newmis.solid = #SOLID_BBOX;

	newmis->classname = "caltrop";
	newmis->weapon = PR_DMSG_CALTROP;

	newmis->touch = CaltropTouch;
	newmis->think = SUB_Remove;

	newmis->nextthink = time + 14 + random()*6; // was 7+random()
	newmis->heat = time + 2; // The caltrop doesn't activate for 2 seconds

	newmis->velocity[X] = xdir * 2;
	newmis->velocity[Y] = ydir * 2;
	newmis->velocity[Z] = zdir * 15;

//	newmis.avelocity = '500 500 500';
	newmis->avelocity[X] = crandom()*400;
	newmis->avelocity[Y] = crandom()*400;
	newmis->avelocity[Z] = crandom()*400;


	setmodel (newmis, "progs/caltrop.mdl");
	setsize (newmis, V({-10, -10, -9}), V({0, 0, 0}));
//	setsize (newmis, '-5 -5 -4', '5 5 5');

	/*#ifdef COOL_GIBS
	newmis.#coolflag = #STRFLAG_COOL;
	#endif*/

	setorigin (newmis, org);
}

// Gizmo - so they lay down correctly
void CaltropPostTouch()
{
	if (self->velocity == V({0, 0, 0})) {
		self->avelocity = V({0, 0, 0});
		self->angles = V({90, 90, 90});
	}

	self->nextthink = self->wait;
	self->think = SUB_Remove;

	self->wait = 0;
}

void CaltropTouch()
{
	// Gizmo - enable a post touch function since touch functions are called before velocity is changed
	if (!self->wait && other == world) {
		self->wait = self->nextthink;

		self->nextthink = time + 0.01;
		self->think = CaltropPostTouch;
	}

/*
	if (self.velocity == '0 0 0')
	{
		self.avelocity = '0 0 0';
		self.angles = '90 90 90';
	}
*/

	if (self->heat > time)
		return;

	if (other->takedamage && other->classname == "player")
	{
		if (Teammate(self->owner, other) && other != self->owner)
			return;

		if (self->velocity == V({0, 0, 0})) // supposedly on the ground..
		{
			sprint(other, PR_PRINT_HIGH, "Ouch! Ouch! Caltrops!\n");
			//other.leg_damage = other.leg_damage + 2;
			//TeamFortress_SetSpeed(other);
			HurtLegs(other,2);
			deathmsg = PR_DMSG_CALTROP;
			TF_T_Damage(other, self, self->owner, 16, 0, PR_TF_TD_OTHER);
		}
		else // if its moving...
		{
			sprint(other, PR_PRINT_HIGH, "Woah! Flying caltrops!\n");
			deathmsg = PR_DMSG_FLYCALTROP;
			TF_T_Damage(other, self, self->owner, 20 + random() * 9, 0, PR_TF_TD_OTHER);
		}

		dremove(self);
	}
#ifdef PR_COOP_MODE_ENHANCED
	if ( other->takedamage && COOP_IsCoopMonster( other ) ) {
		if (Teammate(self->owner, other))
			return;

		if (self->velocity == V({0, 0, 0})) // supposedly on the ground..
		{
			HurtLegs(other,2);
			deathmsg = PR_DMSG_CALTROP;
			TF_T_Damage(other, self, self->owner, 16, 0, PR_TF_TD_OTHER);
		}
		else // if its moving...
		{
			deathmsg = PR_DMSG_FLYCALTROP;
			TF_T_Damage(other, self, self->owner, 20 + random() * 9, 0, PR_TF_TD_OTHER);
		}

		dremove(self);
	}
#endif
}

} // END namespace Progs
