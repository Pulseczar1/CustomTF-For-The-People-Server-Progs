/*======================================================
	COMBAT.QC			Custom TeamFortress v3.2

	(c) TeamFortress Software Pty Ltd 	29/2/97
	(c) William Kerney			5/19/00
========================================================
All the functions pertaining to killing people
======================================================*/

#include "progs.h"
#include "coop_defs.h"
#include "idmonsters/common.h"
#include "custom.h"
#include "debug.h"
#include "frikbot/bot_qw.h"
#include "frikbot/bot_misc.h"
#include "frikbot/bot_ai.h"
#include "combat.h"
#include "ofndefs.h"
#include "tfortmap.h"
#include "jobs.h"
#include "tfdefs.h"
#include "invade.h"
#include "neo.h"

namespace Progs {

//void(string gibname, float dm) ThrowGib;
void ThrowGib(const string& gibname, float dm, float makecool, float gibskin, float gibkgs, float randorg);
void T_MissileTouch();
void info_player_start();
void ClientObituary(entity targ, entity attacker);

// TeamFortress Prototypes
void DoResults(entity Goal, entity AP, float addb);
float Activated(entity Goal, entity AP);
void monster_death_use();
float TeamEqualiseDamage(entity targ, entity attacker, float damage);
void createBastard(entity bastard,float threshold);
void RevealThief(entity targ,float pain);

//WK
void GuerillaExplode();
float IsBuilding(entity tester);
//float(entity happyboy,float newtime) makeImmune;

//- OfN -
float IsMonster(entity thing);
float IsOwnedMonster( entity thing);
void ExpBody(entity body);
string GetBuildingName(entity thebuilding);
string GetEnemyName(entity thething);
void Napalm_touch();

void FieldEvent(entity tfield, const vector& where, entity thing);

float GetDamageInFactor(entity player);
float GetDamageOutFactor(entity player);

void DamageCorpse(entity corpse, float damage, vector where, entity attacker, float directdmg);

//void(entity tfield, vector where, entity thing) FieldExplosion;
//void(entity field) PutFieldWork;

#ifndef PR_COOP_MODE

/*================
monster_death_use (from monsters.qc)

When a monster dies, it fires all of its targets with the current
enemy as activator.
================
*/
void monster_death_use()
{
	//local entity 	ent, otemp, stemp;

	// fall to ground
	if (self->flags & PR_FL_FLY)
		self->flags = self->flags - PR_FL_FLY;
	if (self->flags & PR_FL_SWIM)
		self->flags = self->flags - PR_FL_SWIM;

	if (self->target == "")
		return;
#ifdef PR_COOP_MODE_ENHANCED
	if ( COOP_IsCoopMonster( self ) && !self->PR_npc_useTargetsOnRespawn )
		return;
#endif

	activator = self->enemy;
	SUB_UseTargets ();
}
#endif


//============================================================================

/*
============
CanDamage

Returns true if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/
float CanDamage(entity targ, entity inflictor)
{
    // bmodels need special checking because their origin is 0,0,0
	if (targ->movetype == PR_MOVETYPE_PUSH)
	{
		traceline(inflictor->origin, 0.5 * (targ->absmin + targ->absmax), PR_TL_BSP_ONLY, self);
		if (trace_fraction == 1)
			return PR_TRUE;
		if (trace_ent == targ)
			return PR_TRUE;
		return PR_FALSE;
	}

    vector soffset; // - OfN - hackish fix for turretized sentry guns
    soffset=V({0, 0, 0});

    if (targ->classname == "building_sentrygun" && (targ->tf_items & PR_NIT_TURRET))
        soffset=V({0, 0, -20});

    // OfN - Force field
    traceline(inflictor->origin, targ->origin + soffset, PR_TL_ANY_SOLID, self);
    if (trace_ent->classname == "force_field")
    {
        //FieldExplosion(trace_ent,trace_endpos,trace_ent);
        //PutFieldWork(trace_ent);
        FieldEvent(trace_ent,trace_endpos,trace_ent);
        return PR_FALSE;
    }

	traceline(inflictor->origin, targ->origin + soffset, PR_TL_BSP_ONLY, self);
	if (trace_fraction == 1)
		return PR_TRUE;
	/*traceline(inflictor.origin, targ.origin + '15 15 0' + soffset, #TL_BSP_ONLY, self);
	if (trace_fraction == 1)
		return #TRUE;
	traceline(inflictor.origin, targ.origin + '-15 -15 0' + soffset, #TL_BSP_ONLY, self);
	if (trace_fraction == 1)
		return #TRUE;
	traceline(inflictor.origin, targ.origin + '-15 15 0' + soffset, #TL_BSP_ONLY, self);
	if (trace_fraction == 1)
		return #TRUE;
	traceline(inflictor.origin, targ.origin + '15 -15 0' + soffset, #TL_BSP_ONLY, self);
	if (trace_fraction == 1)
		return #TRUE;*/ // OfN - Fixes dets thru wall?

	return PR_FALSE;
}


/*
============
Killed
============
*/
#ifdef PR_COOP_MODE_ENHANCED
void COOP_SetupRespawn( entity e);
void COOP_MonsterDead( entity e);
#endif
void Killed(entity targ, entity attacker)
{
	string st;

    // SOLVES BUG ???? // Stack overflow?
    if (targ->is_killed == PR_TRUE)
    {
        if (IsBuilding(targ))
        {
            RPrint("(OfN WARNING:) Building: '");
            st=GetBuildingName(targ);
            RPrint(st);
            RPrint("' was going to be Killed() again!\n");
            RPrint("Attacker: '");
            st=GetEnemyName(attacker);
            RPrint(st);
            RPrint("\n");
            return;
        }

        if (targ->classname == "player")
        {
            RPrint("(OfN WARNING:) Player: '");
            RPrint(targ->netname);

            if (targ->cutf_items & PR_CUTF_EXPBODY)
            {
                RPrint(" (with expbody) ");
            }
            else
            {
                RPrint(" (without expbody) ");
            }


            RPrint("' is Killed() again!\n");
            RPrint("Attacker: '");
            st=GetEnemyName(attacker);
            RPrint(st);

            if (attacker->classname == "player" && attacker->cutf_items & PR_CUTF_EXPBODY)
            {
                RPrint(" (with expbody) ");
            }
            else
            {
                RPrint(" (without expbody) ");
            }
            RPrint("\n");
        }
        else
        {
            // NOT BUGS, HAPPEN OFTEN WITH BUTTONS ETC..

            /*RPrint("(OfN WARNING:) Object: '");
            RPrint(targ.classname);
            RPrint("' is Killed() again!\n");
            RPrint("Attacker: '");
            st=GetEnemyName(attacker);
            RPrint(st);
            if (attacker.classname == "player" && attacker.cutf_items & #CUTF_EXPBODY)
            {
                RPrint(" (with expbody) ");
            }
            else
            {
                RPrint(" (without expbody) ");
            }
            RPrint("\n");            */
        }
    }

    targ->is_killed = PR_TRUE;
    /////////////////////////////////////

    entity oself;
	//local string db;

	//WK Have cursed person respawn immediately
	if (targ->classname == "player") {
		if (targ->penance_time > time) {
			targ->real_frags = targ->real_frags - 1;
		 	if (!(toggleflags & PR_TFLAG_TEAMFRAGS))
				targ->frags = targ->real_frags;
			targ->health = 50; //Give em a little so they can die again
            targ->is_killed = PR_FALSE;
			return;
		}
	}

	oself = self;
	self = targ;

	// don't let sbar look bad if a player
	// Modified for Neo mode: I don't want players limited to -99 health for really cool gib explosions.
	if (self->health < -99 && !neo.isModeActive())
		self->health = -99;

	// doors, triggers, etc // - ofn added sensor, it now calls clientobituary when dead
	if ((self->movetype == PR_MOVETYPE_PUSH || self->movetype == PR_MOVETYPE_NONE) && self->classname != "building_camera" && self->classname!="building_sensor") //CH stop bug.
	{
#ifdef PR_COOP_MODE_ENHANCED
		if ( !COOP_IsCoopMonster( self ) ) {
#endif
			self->th_die ();
			self = oself;
			return;
#ifdef PR_COOP_MODE_ENHANCED
		}
#endif
	}

	self->enemy = attacker;

	// bump the monster counter
#ifdef PR_COOP_MODE_ENHANCED
	if ( self->flags & PR_FL_MONSTER && COOP_IsCoopMonster( self ) )
#else
	if (self->flags & PR_FL_MONSTER)
#endif
	{
#ifdef PR_COOP_MODE_ENHANCED
		if ( !( self->spawnflags & PR_SPAWN_DONTCOUNT ) )
		if ( number_of_teams > 1 || !Teammate(self, 1 ) ) {		// don't count friendly monsters
#endif
			killed_monsters = killed_monsters + 1;
			WriteByte (PR_MSG_ALL, PR_SVC_KILLEDMONSTER);
#ifdef PR_COOP_MODE_ENHANCED
		}
#endif
	}

	ClientObituary(self, attacker);

	self->takedamage = PR_DAMAGE_NO;
	self->touch = SUB_Null;

	monster_death_use();

#ifdef PR_COOP_MODE_ENHANCED
	// setup respawn
	//void (float respawntime, void() spawn_func) COOP_SetupRespawn
	if ( COOP_IsCoopMonster( self ) ) {
		// make sure we call this before any cleanup is done
		COOP_SetupRespawn( self );
		// this is called to allow transparent monster AI without affecting the monster's actual code
		COOP_MonsterDead( self );
	}
#endif

	self->th_die();

	// PZ: for bots
	if (!self->ishuman)
	{
		// [remove any of its timers set for removal on death of the bot]
		entity e;
		e = nextent(world);
		while (e != world)
		{
			if (e->owner == self)
			{
				if (e->deadflag & PR_REMOVE_TIMER_ON_DEATH)
				{
					if (strstr(e->classname, "timer") >= 0)
					{
						//bprint(2, "found entity for removal: ", e.classname, ", ", e.netname, "\n");
						if (e->th_die == SUB_Null) // is there not a special cleanup function?
							dremove(e);
						else
							e->th_die(); // we need to call cleanup function (do more than just delete the timer)
					}
				}
			}
			e = nextent(e);
		}

		// [Give a small chance that the bot will rage quit. This allows a new bot to join with a different skill level, class, and team, allowing for the balance
		// of a game to change over time.]
		float rnum;
		rnum = random();
		if (rnum >= 0.983) // 1 in 60 bot deaths
		//if (1)
		{
			/*rnum = random();
			// PZ TODO: use the "start topic" FrikBot system, instead?
			if      (rnum >= 0.75) frik_botSay("fcuk this\n");
			else if (rnum >= 0.50) frik_botSay("i'm out\n");
			else if (rnum >= 0.25) frik_botSay("cyas\n"); // else, says nothing*/
			self->respawn_time = time + 10; // PZ: he's leaving, so don't let him respawn
			frik_botConsiderStartingConversation(PR_TOPIC_SELF_LEAVES_GAME, 1);
			// PZ TODO: also, delay this with a timer, so that bots don't quit before message above is sent
			// right here is the old way
			//removeBot(self); // PZ: makes SV_DropClient() (C code) get called for the bot (this is going to call ClientDisconnect())
			//lastTimeBotCountAdjusted = time; // delay next bot joining
			//self.ishuman = #TRUE;

			entity timer;
			timer = spawnServerSide();
			timer->classname = "bot_quit_timer";
			timer->owner = self;
			timer->think = timerThink_botQuit;
			timer->nextthink = time + 3 + random();
		}
	}

	// PZ: increment/decrement `collectiveHumanScore` appropriately
	if ((attacker->classname == "player" || attacker->real_owner->classname == "player") &&
	    (    self->classname == "player" ||     self->real_owner->classname == "player") &&
	    !Teammate(attacker, self))
	{
		// a human or a human's belonging killed a bot or a bot's belonging   (TODO: may want to disallow changing points over destroyed cameras and such)
		if      (( attacker->ishuman ||  attacker->real_owner->ishuman) && (!self->ishuman || !self->real_owner->ishuman))
			collectiveHumanScore = collectiveHumanScore + 1;
		// a bot or a bot's belonging killed a human or a human's belonging
		else if ((!attacker->ishuman || !attacker->real_owner->ishuman) && ( self->ishuman ||  self->real_owner->ishuman))
			collectiveHumanScore = collectiveHumanScore - 1;
	}

	self = oself;
}


/*
============
T_Damage

The damage is coming from inflictor, but get mad at attacker
This should be the only function that ever reduces health.
============
*/
void T_Damage(entity targ, entity inflictor, entity attacker, float damage)
{
	//WK Just call TF_T_Damage instead, so no updating two functions
	//WK that do exactly the same thing.
	TF_T_Damage(targ,inflictor,attacker,damage,0,0);
}

 /*
 ============
 TF_T_Damage
 same thing as T_Damage (see above), just with some more details

 T_Flags:
	#TF_TD_IGNOREARMOUR: bypasses the armour of the target
	#TF_TD_NOTTEAM: doesn't damage a team member
	#TF_TD_NOTSELF: doesn't damage self
    #TF_TD_NOKNOCK: Impides knockation on players (OfN)
    #TF_TD_DONTGIB: Impides the damage to result in a health of target below -1
    #TF_TD_NOPAIN : Don't do pain frames

 The following is used to determine whether this attack is affected
 the type of armor the defender is wearing.
 T_AttackType:
	#TF_TD_OTHER		:	type ignored
	#TF_TD_SHOT			: 	bullet damage
	#TF_TD_NAIL			:	nailgun damage
	#TF_TD_EXPLOSION	:	explosion damage
	#TF_TD_ELECTRICITY	:	electricity damage
	#TF_TD_FIRE			:	fire damage

	#TF_TD_NOSOUND		:	Special Value. Health is adjusted without
						    any sound, painframe, etc
							Health is _set_ to damage, not altered.

============
*/

#ifdef PR_COOP_MODE_ENHANCED
float COOP_CalcEntityDamage(entity targ, entity attacker, float indmg, float attacktype);
float COOP_IsCoopMonster(entity test);
void COOP_PrePain( entity attacker);
#endif
// PZ NOTE: `inflictor` is the object that literally does the damage (like a detpack); `attacker` is the entity responsible for the damage
void TF_T_Damage(entity targ, entity inflictor, entity attacker, float damage, int T_flags, float T_AttackType)
{
	vector	dir;
	entity	te; //<-- oldself,
	//local	float	save; --> knockem
	float	take;
	//local float mirror; --> knockem
	//local float dist; --> knockem
	//local string output; (?)
	float knockem;

	//WK -- For LPB calculation
	string st;
	//local float ping; --> knockem


	//mirror = 0;
#ifdef PR_QUAKE_WORLD // culled by KK. was QUAKE_WORLD
    if (ceasefire)
        return;
#endif

	if (targ == world)
		return;

	if (!targ->takedamage)
		return;

#ifdef PR_COOP_MODE_ENHANCED
	// certain monsters are weaker/stronger against certain types of attack
	if ( !deathmatch )
		damage = COOP_CalcEntityDamage (targ, attacker, damage, T_AttackType);
#endif

	//WK Set off land mines
	if (targ->classname == "grenade" && targ->netname == "land_mine")
	{
		targ->think = GuerillaExplode;
		targ->nextthink = time + 0.1;
		return;
	}

	//BOOKKEEPING
	//WK Store last person who shot martyr into .enemy
	//(This conflicts with cameraman, but hey...)
	//Remember last person who shot us, so we can give him a frag
	if (targ->cutf_items & PR_CUTF_EXPBODY && !targ->is_abouttodie)
	{
		//if (targ.classname == "player" && attacker.classname == "player" && !Teammate(targ.team_no, attacker.team_no)) {
		targ->martyr_enemy = attacker;
		targ->stored_deathmsg = deathmsg; //- OfN - UNUSED?
	}
	if (targ->classname == "player") //WK Time holding the last time we've been shot
		targ->last_attacked_time = time; //WK For chaplan healing purposes


	//BUTTON TRIGGERING
	if (attacker->classname == "player")
	{
		// Gizmo - allow non demons and non army soldiers to be excluded (coop monsters)
		if (targ->classname != "player" && !IsBuilding(targ) && !IsMonster(targ) && targ->PR_corpseflag != PR_STRFLAG_CORPSE)
		{
			if (!Activated(targ,attacker))
			{
				// If an else goal should be activated, activate it
				if (targ->else_goal != 0)
				{
					te = Findgoal(targ->else_goal);
					if (te != world)
						DoResults(te, attacker, (targ->goal_result & PR_TFGR_ADD_BONUSES));
				}

				return;
			}
		}
	}

	// used by buttons and triggers to set activator for target firing
	damage_attacker = attacker;

	// INVINCIBILITY
	if (targ->invincible_finished >= time)
	{
		if (targ->invincible_sound < time)
		{
			sound (targ, PR_CHAN_ITEM, "items/protect3.wav", 1, PR_ATTN_NORM);
			targ->invincible_sound = time + 2;
		}
		return;
	}

	//CHAPLAIN CALCULATIONS
	// WK See if they're close enough to chap to get protection
	if (attacker->classname == "player" && attacker->tfstate & PR_TFSTATE_INSPIRED) {
		knockem = vlen(attacker->origin - attacker->inspirator->origin);
		//output = ftos(dist);
		if (knockem > PR_CHAPLAN_RADIUS) { //We've strayed from the flock
			sprint(attacker,PR_PRINT_HIGH,"You have strayed from the flock\n");
			attacker->tfstate = attacker->tfstate - PR_TFSTATE_INSPIRED;
			attacker->super_damage_finished = 0;
			attacker->items = attacker->items - (attacker->items & PR_IT_QUAD);
			attacker->effects = attacker->effects - (attacker->effects & PR_EF_DIMLIGHT);
		}
	}
	if (targ->classname == "player" && targ->tfstate & PR_TFSTATE_INSPIRED) {
		knockem = vlen(targ->origin - targ->inspirator->origin);
		if (knockem > PR_CHAPLAN_RADIUS) { //We've strayed from the flock
			sprint(targ,PR_PRINT_HIGH,"You have strayed from the flock\n");
			targ->tfstate = targ->tfstate - PR_TFSTATE_INSPIRED;
			targ->super_damage_finished = 0;
			targ->items = targ->items - (targ->items & PR_IT_QUAD);
			targ->effects = targ->effects - (targ->effects & PR_EF_DIMLIGHT);
		}
	}

    // OfN - Apply damage factors of attacker and target if any
    damage = damage * GetDamageOutFactor(attacker);

	// ugly, but QC does multiple if statements faster than || or &&
	// this counts total damage dealt in general, without regard to armor protection
	if (targ != attacker) {
		if (attacker->classname == "player") {
			// Gizmo - we do this even if team damage isn't enabled to make fun of people who can't hit the enemy
			if (Teammate(targ, attacker)) {
				if (!IsBuilding(targ))
					attacker->PR_team_damage_dealt = attacker->PR_team_damage_dealt + damage;
			} else
				attacker->PR_enemy_damage_dealt = attacker->PR_enemy_damage_dealt + damage;
		}

		if (targ->classname == "player") {
			if (attacker != world) {
				if (!Teammate(targ, attacker))
					targ->PR_damage_taken = targ->PR_damage_taken + damage;
			}
		}
	}

    if (!(T_flags & PR_TF_TD_IGNORERESIST))
        damage = damage * GetDamageInFactor(targ);

    // OfN - Corpses are handled in their own routine
    if (targ->PR_corpseflag == PR_STRFLAG_CORPSE)
    {
        DamageCorpse(targ,damage,targ->origin,attacker,PR_TRUE);
        return;
    }

	if (targ->aura == PR_AURA_RESIS || targ->PR_runes & PR_RUNE_RESIS)
	{
		if (targ->invincible_sound < time) {
			sound (targ, PR_CHAN_ITEM, "auras/aura1b.wav", 1, PR_ATTN_NORM);
			targ->invincible_sound = time + 1.5;
		}
	}

	//DAMAGE ADJUSTMENT
    if (deathmsg != PR_DMSG_MARTYR && !IsMonster(targ))	// Gizmo - allow non demons and non army soldiers to be excluded (coop monsters)
	{
#ifdef PR_PING_FAIRNESS    // PZ
		//WK Ping fairness code. LPB is < 200 ping
		if (attacker->classname == "player" && targ->classname == "player" && attacker != targ)
		{
			st = infokey(attacker,"ping");
			knockem = 200;
			if (st != string_null)
				knockem = stof(st);
			knockem = (knockem + 1000) / 1200;
			if (knockem < 0.8) knockem = 0.8;
			if (knockem > 1.2) knockem = 1.2;
			damage = damage * knockem;
		}
#endif
		if (teamplay & (PR_TEAMPLAY_LESSSCOREHELP | PR_TEAMPLAY_LESSPLAYERSHELP))
			damage = TeamEqualiseDamage(targ, attacker, damage);
	}

	//MIRROR DAMAGE
    knockem = 0;
	//WK - Check to see if we hit a friend
	if (Teammate(targ, attacker) && targ != attacker && attacker->classname == "player" && targ->classname == "player")  {
		//We just hit someone on our team!
		if (T_flags & PR_TF_TD_NOTTEAM)
		{
			//Direct damage
			if (teamplay & PR_TEAMPLAY_FULLMIRRORDIRECT)
				knockem = knockem + damage;
			if (teamplay & PR_TEAMPLAY_HALFMIRRORDIRECT)
				knockem = knockem + damage / 2;
		}
		else
		{
			//Explosive damage
			if (teamplay & PR_TEAMPLAY_FULLMIRROREXPLOSIVE)
				knockem = knockem + damage;
			if (teamplay & PR_TEAMPLAY_HALFMIRROREXPLOSIVE)
				knockem = knockem + damage / 2;
		}
		//Don't inflict damage on a (teamkiller or spy)-hitter
		if (knockem > 0)
        if (!(targ->is_feigning || targ->is_undercover) && !(targ->penance_time > time)) //Hurt the jerk!
			TF_T_Damage (attacker, attacker, attacker, knockem, 0, PR_TF_TD_OTHER);
	}
	//WK Slight mirror demon protection
	//Do 2 points of damage to a friendly teammate shooting a friendly demon
	//SB 2 damage? no way, we're doing the full mirror damage
	if ( IsOwnedMonster( targ ) ) {
		// PZ (9/17/14): just like with buildings, I don't want friendly fire killing summons
		if (Teammate(targ->real_owner, attacker) && targ->real_owner != attacker && attacker != targ)
			return;
		// PZ: END
        targ->armorvalue = 1;
		if (T_flags & PR_TF_TD_NOTTEAM)
		{
			//Direct damage
			if (teamplay & PR_TEAMPLAY_FULLMIRRORDIRECT)
				knockem = knockem + damage;
			if (teamplay & PR_TEAMPLAY_HALFMIRRORDIRECT)
				knockem = knockem + damage / 2;
		}
		else
		{
			//Explosive damage
			if (teamplay & PR_TEAMPLAY_FULLMIRROREXPLOSIVE)
				knockem = knockem + damage;
			if (teamplay & PR_TEAMPLAY_HALFMIRROREXPLOSIVE)
				knockem = knockem + damage / 2;
		}
        if (knockem > 0) {
			if (Teammate(targ->real_owner, attacker) && targ->real_owner != attacker && attacker->classname == "player")
				TF_T_Damage(attacker, attacker, attacker, knockem, 0, PR_TF_TD_OTHER);
		}
	}
	//WK Friendly Sentrygun protection - you can't hurt friendly sentries
	if (IsBuilding(targ)) {
		if (Teammate(targ, attacker) && targ != attacker && !(targ->tf_items & PR_NIT_SECURITY_CAMERA)) {
			return;
		}
		targ->armorvalue = 1; //CH so that the armors are counted
	}

	// SPECIAL ARMOR CALCULATIONS
	if ((targ->armorclass != 0) && (T_AttackType != 0))
	{
		if ((targ->armorclass & PR_AT_SAVESHOT) && (T_AttackType == PR_TF_TD_SHOT)) {
			damage = floor(damage * 0.5);
			//WK Cap max damage you can take with kevlar on, like in Real Life(tm)
			//WK The purpose being to cut down on the power of snipers
			if(targ->classname == "player") // SB kevlar fixed so it caps damage now, also spacing much nicer :)
			{
				//WK 1/7/7 Fixed SB's spacing, pfft. Removed caps now that sniper damage is nerfed.
/*
				if (light_damage)
				{
					if (damage > 125)
						damage = 125;
				}
				else
					if (damage > 150) damage = 150;
*/
				if (attacker->classname == "player" && (deathmsg == PR_DMSG_SNIPERRIFLE || deathmsg == PR_DMSG_SNIPERHEADSHOT || deathmsg == PR_DMSG_SNIPERLEGSHOT))
				    sprint(attacker,PR_PRINT_HIGH,"You hit kevlar\n");
	            }
		}
		else if ((targ->armorclass & PR_AT_SAVEMELEE) && (T_AttackType == PR_TF_TD_MELEE || deathmsg == PR_DMSG_JUDOKA || deathmsg == PR_DMSG_AXE || deathmsg == PR_DMSG_BACKSTAB || deathmsg == PR_DMSG_SPANNER))
		{
			damage = floor(damage * 0.5);
			if ((targ->velocity[X] > 0 || targ->velocity[Y] > 0 || targ->velocity[Z] > 0) && random() > 0.6)
				return;
			if (deathmsg == PR_DMSG_BACKSTAB)
				T_flags = T_flags - (T_flags & PR_TF_TD_IGNOREARMOUR);
		}
		else if ((targ->armorclass & PR_AT_SAVEEXPLOSION) && (T_AttackType == PR_TF_TD_EXPLOSION))
			damage = floor(damage * 0.5);
		else if ((targ->armorclass & PR_AT_SAVEELECTRICITY) && (T_AttackType == PR_TF_TD_ELECTRICITY || T_AttackType == PR_TF_TD_NAIL))
			damage = floor(damage * 0.5);
		else if ((targ->armorclass & PR_AT_SAVEFIRE) && (T_AttackType == PR_TF_TD_FIRE))
			damage = floor(damage * 0.5);
	}

	//ARMOR PROTECTION CALCULATION
	// save damage based on the target's armor level
	if (T_flags & PR_TF_TD_IGNOREARMOUR)
	{
		take = damage;
		knockem = 0;
	}
	else
	{
		knockem = ceil(targ->armortype*damage);
		if (knockem >= targ->armorvalue)
		{
			knockem = targ->armorvalue;
			if (!neo.isModeActive() || targ != neo.getNeo()) // modded for Neo mode
			{
				targ->armortype = 0; // lost all armor
				//WK 5-15-08 No longer strip special armors targ.armorclass = 0; // lost special armor  (PZ: I pulled this in from Shaka's latest code.)
				targ->items = targ->items - (targ->items & (PR_IT_ARMOR1 | PR_IT_ARMOR2 | PR_IT_ARMOR3));
			}
		}
		//WK Flags prevent armor damage too
		if (T_flags & PR_TF_TD_NOTTEAM)
		{
			if (Teammate(targ, attacker) && targ != attacker)
			{
				if (teamplay & PR_TEAMPLAY_NOARMORDIRECT)
					targ->armorvalue = targ->armorvalue;
				else if (teamplay & PR_TEAMPLAY_HALFARMORDIRECT)
					targ->armorvalue = targ->armorvalue - knockem / 2;
				else
					targ->armorvalue = targ->armorvalue - knockem;
			}
			else
				if (!Invade_buffPlayer(targ))
					targ->armorvalue = targ->armorvalue - knockem;
		}
		else
		{
			if (Teammate(targ, attacker) && targ != attacker)
			{
				if (teamplay & PR_TEAMPLAY_NOARMOREXPLOSIVE)
					targ->armorvalue = targ->armorvalue;
				else if (teamplay & PR_TEAMPLAY_HALFARMOREXPLOSIVE)
					targ->armorvalue = targ->armorvalue - knockem / 2;
				else
					targ->armorvalue = targ->armorvalue - knockem;
			}
			else
				if (!Invade_buffPlayer(targ))
					targ->armorvalue = targ->armorvalue - knockem;
		}
		//		targ.armorvalue = targ.armorvalue - knockem;
		take = ceil(damage-knockem);
	}
	// add to the damage total for clients, which will be sent as a single
	// message at the end of the frame
	// FIXME: remove after combining shotgun blasts?
	if (targ->flags & PR_FL_CLIENT)
	{
		targ->dmg_take = targ->dmg_take + take;
		targ->dmg_save = targ->dmg_save + knockem;
		targ->dmg_inflictor = inflictor;
	}

	// KNOCKATION CALCULATION
	if (!(T_flags & PR_TF_TD_NOKNOCK)) // OfN - No knockation if the appropiate flag is set
    		if ( (inflictor != world) && (targ->movetype == PR_MOVETYPE_WALK) )
		{
			// Nail Gren doesn't knock ppl
			//WK People with Aspect of HWGUY are immune to knockation
			//WK Martyrs are immune to bullet damage, so AC's and Turrets
			// can't be annoying and just simply stop them.
			//SB That's annoying. They can now :)
			knockem = 1;
			if (targ->cutf_items & PR_CUTF_ASPECT_OF_HWGUY) knockem = 0;
			//if (targ.job & #JOB_MARTYR && targ.job & #JOB_ACTIVE && T_AttackType != #TF_TD_SHOT) knockem = 1;
			if (Invade_buffPlayer(targ)) knockem = 0; // PZ: don't knock D players during Invade Prematch
			if (deathmsg == PR_DMSG_GREN_NAIL) knockem = 0;
			if (deathmsg == PR_DMSG_CLUSTER_ROCKET) knockem = 0; //WK 1/7/7 Clusters are ineffective since they push people away
			if (knockem)
			{
				// give them some immunity to cheat check
				//targ.immune_to_check = time + (damage / 20);
				makeImmune(targ,time+(damage/20));

				dir = targ->origin - (inflictor->absmin + inflictor->absmax) * 0.5;
				dir = normalize(dir);

			#ifdef PR_QUAKE_WORLD
				// Increase kickback for smaller weapons
				if ( (damage < 60) && ((attacker->classname == "player") && (targ->classname == "player")) && ( attacker->netname != targ->netname))
					targ->velocity = targ->velocity + dir * damage * 11;
				else
					targ->velocity = targ->velocity + dir*damage*8;
				if ( (rj > 1) && ((attacker->classname == "player") && (targ->classname == "player")) && ( attacker->netname == targ->netname))
					targ->velocity = targ->velocity + dir * damage * rj;
				if (targ->classname == "player" && targ->cutf_items & PR_CUTF_GYMNAST)
					targ->velocity = targ->velocity + dir * damage * 4; //WK 1/7/7 Gymnast Nerf: 8
			#else
				targ->velocity = targ->velocity + dir*damage*8;
			#endif
		}
	}

	// TEAMPLAY FLAGS
	// Gizmo - monsters are now affected by teamplay flags for no team damage
	// if there's an idiot blocking with his summon on purpose, we can always vote kick/ban
	if ( ( attacker->classname == "player" && targ->classname == "player" )
		|| ( attacker->classname == "player" && IsMonster( targ ) )
//		|| ( IsMonster( attacker ) && targ.classname == "player" )
		|| ( IsBuilding( attacker ) && IsMonster( targ ) ) )  {
		if (T_flags & PR_TF_TD_NOTTEAM)  {
			if (Teammate(targ, attacker) && targ != attacker) {
				if (teamplay & PR_TEAMPLAY_NODIRECT) return;
				else if (teamplay & PR_TEAMPLAY_HALFDIRECT) take = take / 2;
			}
		}
		else {
			if (Teammate(targ, attacker) && targ != attacker)	{
				if (teamplay & PR_TEAMPLAY_NOEXPLOSIVE) return;
				else if (teamplay & PR_TEAMPLAY_HALFEXPLOSIVE) take = take / 2;
			}
		}
	}

	if (T_flags & PR_TF_TD_NOTSELF) {
		if (targ == attacker) return;
	}

	// do the damage, min 1
	if (take < 1) take = 1;

	//Make thief visible when he's shot
	if (take > 1 && targ != attacker && (targ->job & PR_JOB_THIEF && (targ->job & PR_JOB_ACTIVE || targ->job & PR_JOB_FULL_HIDE))) {
		if (!Teammate(targ, attacker))
			RevealThief(targ, PR_TRUE);
	}

	// for Invade Prematch
	if (invade && invade_gameState == InvadeGameState::IN_PREMATCH && targ->team_no == invade_teamOnDef)
	{
		if (IsBuilding(targ))
			take = take * PR_INVADE_PM_BUILD_PROTECT;
		else if (targ->classname == "player")
			take = take * PR_INVADE_PM_PLAYER_PROTECT;
	}

	targ->health = targ->health - take;

	// PZ: So that we can make bots react to being hurt, like making bots with spy kit feign sometimes when getting shot by a player.
	if (!targ->ishuman) if (targ->classname == "player")
		callback_botTakesDamage(targ, take, inflictor, attacker, T_flags, T_AttackType);

	if (targ->armorvalue < 1)
	{
		//targ.armorclass = 0; // lost special armor // PZ: seems dumb to me to take away armor class when armor reaches 0 (maybe this is a bad idea, though, to take this out)
		targ->armorvalue = 0;
	}

	if (targ->health < 1 && targ->health > 0) //WK Stop the scoreboard coming up
		targ->health = 1;

	if (targ->health <= 0) {

	// If we dont have to gib the target, just set its HP to -1
	// Gizmo - if in coop 3 mode then don't gib
#ifdef PR_COOP_MODE_ENHANCED
	if ( T_flags & PR_TF_TD_DONTGIB || ( coop == 3 && !deathmatch && targ->classname == "player" ) )
#else
	if ( T_flags & PR_TF_TD_DONTGIB )
#endif
		targ->health = -1;

        if (inflictor->classname == "detpack" && inflictor->weaponmode == 1 && inflictor->enemy == targ)
			deathmsg = PR_DMSG_DETPACK_DIS;

		//WK Autotrigger martyr - OfN - Now exp.body
		if ((targ->cutf_items & PR_CUTF_EXPBODY) && attacker != world && targ->is_abouttodie == PR_FALSE)
		{
			//oldself = self;
			//self = targ;
			targ->health = 1;
            targ->martyr_enemy=attacker;
			ExpBody(targ);
			//self = oldself;
            targ->is_abouttodie = PR_TRUE;
			return;
		}// /////// OFTEN ///////////

		//WK Make sure the world doesn't have a bastard level (<- OfN this is funny)
		if (attacker->classname != "player" || targ->classname != "player") {
			//bprint(#PRINT_MEDIUM,"Non player death or kill\n");
			Killed(targ, attacker);
			return;
		}

#ifdef PR_QUAKE_WORLD
		//WK Handle Total Bastard Checking
		//If they kill more than a certain number of friends in a certain period, they are booted
		// Neo mode: Don't count as teamkill when there's no Neo in the game.
		if (!(neo.isModeActive() && neo.getNeo() == world) && Teammate(targ, attacker) && targ != attacker && !(targ->penance_time > time) && !targ->is_undercover && prematch < time)
		{
			//A teamkill
			float threshold;

			threshold = 0;
			st = infokey(world, "curse");
			if (st != string_null)
				threshold = stof(st);

			attacker->ff_count = attacker->ff_count + 1; //Increase their bastard rating
			if (threshold >= 1) {
				if (attacker->ff_count >= threshold) createBastard(attacker,threshold);
				if ((attacker->ff_count == threshold - 1) || (attacker->ff_count == threshold - 0.5)) {
					sprint (attacker, PR_PRINT_MEDIUM, "One more teamkill and you will be cursed.\n");
				}
			}
		}
		if (!Teammate(targ, attacker) || targ->penance_time > time) {
			attacker->ff_count = attacker->ff_count - 0.5;
			if (attacker->ff_count < 0) attacker->ff_count = 0;
		}
#endif

		if (targ->penance_time > time) //A penitent loses frags
			Killed(targ, targ);
		else
			Killed(targ, attacker);
		return;

	}

    // Should we do any pain stuff?
    if (T_flags & PR_TF_TD_NOPAIN)
        return; // Just end

	// react to the damage
	te = self;
	self = targ;

#ifdef PR_COOP_MODE_ENHANCED
	// Gizmo - call the default AI pain function
	if ( COOP_IsCoopMonster( self ) )
		COOP_PrePain( attacker );
#endif

	if ((void (*)())self->th_pain != SUB_Null)
	{
		self->th_pain (attacker, take);

#ifdef PR_COOP_MODE_ENHANCED
		// Gizmo - prevent the monster lerping from getting messed up when pausing a bit for pain
		if ( COOP_IsCoopMonster( self ) )
			self->PR_npc_lastruntime = 0;
#endif
	}

	self = te;
}

#ifdef PR_COOL_GIBS
void CoolExplosion(entity inflictor, float radius, float extent);
float cool_gibs;
#endif

/* OfN - Same as radius damage but with fire, used for napalm grens TODO: Mindmg, maxdmg
============
T_RadiusFireDamage
============
*/
void T_RadiusFireDamage(const vector& place, entity inflictor, entity attacker, float damage, float fireprob)
{
    entity head;
    float finaldmg;

    head = findradius(place, damage+50);

    while (head != world)
    {
        //WK Set off all land mines in blast radius
        if (head->classname == "grenade" && head->netname == "land_mine")
        {
            head->think = GuerillaExplode;
            head->nextthink = time + 0.1;
        }
        else if (head->takedamage)// && head.health > 0)
        {
            finaldmg = damage - vlen(place - head->origin);

            if (finaldmg > 0)
            {
                TF_T_Damage(head, inflictor, attacker, finaldmg, PR_TF_TD_NOKNOCK | PR_TF_TD_DONTGIB, PR_TF_TD_FIRE);

                if (random() <= fireprob)
                {
                    // set 'em on fire
                    other = head;
                    Napalm_touch();
                    if (other->classname == "player")
                        stuffcmd(other, "bf\nbf\n");
                }
            }
        }

        head = head->chain;
    }

    // Push gibs and heads if enabled
    // (you may think this could be optimized by pushing the stuff in the loop above
    // and its true, but doing the scan again lets the gibs/heads that just appeared to be
    // pushed by its first explosion and thats cool)
    #ifdef PR_COOL_GIBS
    CoolExplosion(inflictor, damage,20);
    #endif
}

/*
============
T_RadiusDamage
============
*/
void T_RadiusDamage(entity inflictor, entity attacker, float damage, entity ignore)
{
	float 	points;
	entity	head, te;
	vector	org;

	head = findradius(inflictor->origin, damage+40);

	while (head != world)
	{
		if (head != ignore)
		{
            //WK Set off all land mines in blast radius
            if (head->classname == "grenade" && head->netname == "land_mine")
			{
				if (deathmsg != PR_DMSG_LAND_MINE) {
                    //head.has_tesla = 2; //- ofn - ur mine explodes better than is destroyed
					head->think = GuerillaExplode;
					head->nextthink = time + 0.1;
				}
			}
			// Check for TeamFortress Goals that can be triggered by Detpacks
			else if (head->classname == "info_tfgoal")
			{
				if (inflictor->classname == "detpack")
				{
					// Don't activate timer goals
					if ((head->goal_activation & PR_TFGA_TOUCH_DETPACK) && (head->search_time == 0))
					{
						traceline (inflictor->origin, head->origin, PR_TL_BSP_ONLY, inflictor);

						if (trace_fraction == 1)
						{
							// Does the AP match the AP Criteria?
							if (Activated(head,attacker))
							{
								DoResults(head, attacker, PR_TRUE);
							}
							else
							{
								// If an else goal should be activated, activate it
								if (head->else_goal != 0)
								{
									te = Findgoal(head->else_goal);
									if (te != world)
										DoResults(te, attacker, (head->goal_result & PR_TFGR_ADD_BONUSES));
								}

								return;
							}
						}
					}
				}
			}
			else if (head->takedamage)// && head.health > 0)
			{
				org = head->origin + (head->mins + head->maxs)*0.5;
				points = 0.5*vlen (inflictor->origin - org);
				if (points < 0)
					points = 0;
				points = damage - points;
				if (head == attacker)
					points = rint(points * 0.75);
				if (head->classname == "building_camera")
					points = 10;

				if (points > 0)
				{
					if (CanDamage (head, inflictor))
					{
		                TF_T_Damage (head, inflictor, attacker, points, PR_TF_TD_NOTTEAM, PR_TF_TD_EXPLOSION);
					}
					//Turrets have fucked up bounding boxes due to their natures
					//So we have to make them susceptible to blast damage here
					//But only 50% damage, since they are turrets...
					else if ((head->classname == "building_sentrygun" || head->classname == "building_tesla") && head->tf_items & PR_NIT_TURRET) {
							points = rint(points / 2);
							TF_T_Damage (head, inflictor, attacker, points, PR_TF_TD_NOTTEAM, PR_TF_TD_EXPLOSION);
					}
				}
			}
		}
		head = head->chain;
	}

    // Push gibs and heads if enabled
    // (you may think this could be optimized by pushing the stuff in the loop above
    // and its true, but doing the scan again lets the gibs/heads that just appeared to be
    // pushed by its first explosion and thats cool)
    #ifdef PR_COOL_GIBS
    CoolExplosion(inflictor, damage,50);
    #endif
}

#ifdef PR_COOL_GIBS
float crandom();

void CoolExplosion(entity inflictor, float radius, float extent)
{
    // If this is disabled, go away..
    if (!cool_gibs)
        return;

    entity head;
    vector dir;
    float dist, intensity;

    head = findradius(inflictor->origin, radius+extent);

    while (head != world)
    {
        if (head->PR_coolflag == PR_STRFLAG_COOL)
        if (CanDamage(head,inflictor))
        {
            // Get distance between the explosion and this thing
            dist = vlen(head->origin - inflictor->origin);

            // Calculate the intensity based on proximity of explosion (1 = same place, 0 = as far away it shouldnt push this)
            intensity = 1 - ((radius+extent) - ((radius+extent) - dist)) / (radius+extent);

            // Calculate the direction the thing will be pushed to
            dir = head->origin - (inflictor->absmin + inflictor->absmax) * 0.5;
			dir = normalize(dir);

            // Wheeeeeee, throw it away!
            head->velocity = head->velocity + dir*radius*intensity*4;

            // Special: if this is set onground use a randomized*intensity positive velocity_z instead
            if (head->flags & PR_FL_ONGROUND)
            {
                head->flags = head->flags - PR_FL_ONGROUND;
                head->velocity[Z] = 140*intensity + radius*2*intensity + random()*230 + intensity*random()*120;
            }

            // Do an appropiate spin movement if we arent spinning currently and sometimes even if we are
            if (head->avelocity == V({0, 0, 0}) || random() < 0.2)
            {
                // is this a head?
                if (head->classname != "gib")
                    head->avelocity = crandom() * V({0, 600, 0});
                else // should be a gib then
                {
                    head->avelocity[X] = crandom()*600;
                    head->avelocity[Y] = crandom()*600;
                    head->avelocity[Z] = crandom()*600;
                }
            }
        }

        head = head->chain;
    }
}
#endif

/*
============
T_BeamDamage // OfN - Used by bio grens
============
*/

void T_BeamDamage(entity inflictor, entity attacker, float damage)
{
	float 	points;
	entity	head;

	head = findradius(inflictor->origin, damage+40);

	while (head != world)
	{
		if (head->takedamage && head->health > 0)
		{
			points = 0.5*vlen (inflictor->origin - head->origin);
			if (points < 0)
				points = 0;
			points = damage - points;
			/*if (head == attacker)
				points = points * 0.5;*/
			if (points > 0)
			{
				if (CanDamage (head, inflictor))
				{

					//if (head.classname == "monster_shambler")
					//	T_Damage (head, attacker, attacker, points*0.5);
					//else

						TF_T_Damage (head, inflictor, attacker, points,PR_TF_TD_NOKNOCK,0);

				}
			}
		}
		head = head->chain;
	}
}

} // END namespace Progs
