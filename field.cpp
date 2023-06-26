/*=======================================================//
// field.QC - CustomTF 3.2.OfN             - 30/1/2001 - //
// by Sergio Fuma�a Grunwaldt - OfteN [cp]               //
=========================================================//
 Field generator stuff - Written all by myself! :)
 I took the model and some sounds from deadlode mod
 One sound is from megaTF
=========================================================*/

// PZ NOTE: The reason field generators can't be positioned diagonally is because the Quake engine doesn't allow
//          for diagonal bounding boxes, which would be needed for the field entity. It doesn't allow diagonal
//          bounding boxes because a bounding box is always represented by only 2 points in space -- two opposite
//          corners of the box: .absmin and .absmax. We could change the server to support full 8-point bounding
//          boxes. Often said QuakeForge supports diagonal bounding boxes. I'm interested in diagonal ones, because
//          moving field generators with the gravity gun doesn't work right. For the time being, I will not allow
//          diagonal placement with the gravity gun. Instead, if you put a field generator close to in line with
//          its mate, it will position itself to be in perfect alignment, so that the field can be positioned
//          properly between the two generators.

#include "progs.h"
#include "field.h"
#include "ofndefs.h"
#include "debug.h"
#include "combat.h"
#include "cpstuff.h"
#include "custom.h"
#include "monsters.h"
#include "sprites.h"
#include "menu.h"

namespace Progs {

// field generator status flags, DO NOT MODIFY

#define PR_FIELDGEN_ISOFF      0 // no field, not linked, and its not trying to link (several possible reasons)
#define PR_FIELDGEN_ISIDLE     1 // no field, not linked, trying to link with other generator
#define PR_FIELDGEN_ISDISABLED 2 // no field, linked, its disabled temporary (teammate passing thru field)
#define PR_FIELDGEN_ISENABLED  3 // field, linked, and cells enough, waiting for shock, only hum sound
#define PR_FIELDGEN_ISWORKING  4 // field, linked, glowing and all the field visual/sound effects are on

// field generator settings

#define PR_FIELDGEN_WORKTIME       3   // seconds the generators remain "working", glowing and doing lightning after a shock
#define PR_FIELDGEN_WORKING_RATE   1   // interval in seconds for fieldgens to use cells
#define PR_FIELDGEN_LINKTIME       3.5 // seconds between tries to link with other generator (only visual)
#define PR_FIELDGEN_TIMEDISABLED   1.5 // time fieldgens remain disabled
#define PR_FIELDGEN_CELLSCOST      2   // cells cost for each "FIELDGEN_ISWORKING" pass

// force field settings

#define PR_FIELD_DMG               250//160 //was 80 - normal damag when touching
#define PR_FIELD_DMGINSIDE         500//420 //was 120 - damage when trapped inside field
#define PR_FIELD_SOUNDSRATE        1   // Rate in seconds at which hum/shield sound is played
#define PR_FIELD_VISUALFACTOR      0.5 // 1 = always generate lightning for visuals, when working. 0 = never

/*===============================================================================================

EXPLANATION OF HOW THE ENTITY FIELDS ARE USED (thnx? np.. :P)
---------------------------------------------

For field generator entity:
---------------------------

.has_holo        - Holds current status of every field generator, FIELDGEN_ISXXXX determines
.has_sensor      - Boolean value, determines if field generator is currently supporting a force field
.martyr_enemy    - This points to the force field, if none its always 'world'
.no_grenades_1   - Controls delay between tries to link (only affects sound/flash, it tries to link every frame)
.no_grenades_2   - Controls delay for field to go up again after being disabled
.tp_grenades_1   - Controls delay of the WORKING status
.has_teleporter  - Used to flash generators when field is activated
.has_camera      - Controls delay between cells take
.has_tesla       - Boolean, forced status.
.trigger_field   - PZ: whether the field generator was just dropped by a zero gravity gun; if so, by whom

For force field entity:
-----------------------

.demon_one      - Points to the first field generator
.demon_two      - Points to the 2nd generator
.has_holo       - Hum sound running, boolean
.has_sensor     - Shield sound running, boolean
.has_tesla      - Controls delay between hums
.has_sentry     - Controls delay between shield sounds
.cnt            - Orientation of field (x or y)
.dmg            - Next damage the field will do
.has_camera     - Used to control the rate at which the field touch sound/visual effects are done (4hz)

================================================================================================*/

void CheckDistance();
entity Find_OtherGen(entity fieldgen);
float FieldGens_CanLink(entity fieldgen1, entity fieldgen2);
float FieldGen_CanIdle(entity fieldgen);
float IsValidFieldGen(entity fieldgen);
float vis2orig(const vector& targ, const vector& check);
float IsValidField(entity field);
void Field_UpdateSounds(entity tfield, entity gen1);
void Field_StopSounds(entity tfield);
void Field_MakeVisual(entity tfield);
int FieldIsImproved(entity tfield);
int FieldIsMalfunctioning(entity tfield);
void Field_touch();
void Field_touch_SUB();
float EntsTouching2(entity e1, entity e2);
void FieldExplosion(entity tfield, const vector& where, entity thing);
void PutFieldWork(entity field);
float Field_ItCanPass(entity tfield, entity who);
float Field_ShouldDamage(entity tfield, entity who);

#ifdef PR_FIELD_FORCEMODE
entity GetMyFieldGen(entity myself);
float Field_GetForcedStatus(entity tfield);
void SetFieldForcedStatus(float value); // player function (self = player) cuts disabled time also
float GetFieldForcedStatus(); // player
#endif

//=========================================================================================
// field generator model and sounds

void Field_Precache()
{
  	precache_sound ("misc/null.wav");
    precache_sound2("misc/ffhum.wav");
    precache_sound2("misc/ffbeep.wav");
    precache_sound2("misc/ffield.wav");
    precache_sound2("misc/ffact.wav");
    precache_sound2("misc/fffail.wav");
    precache_model2("progs/ffgen2.mdl");
}

//==================================================================================================
// checks for field generators and removes itself if needed, checks for stuck entities inside field
// `self` : this field (not generators)
void Field_think()
{
    /* NOT NEEDED - if (self.classname != "force_field")
    {
        RPrint("BUG: Not a force field entity was in 'FieldThink()'!\n");
        return;
    }*/

    self->has_camera = PR_FALSE; // resets flag for visuals

    // check field generators, removal of this field if needed... (for safety only)
    if (!IsValidFieldGen(self->demon_one) || !IsValidFieldGen(self->demon_two))
    {
        if (IsValidFieldGen(self->demon_one))
        {
            self->demon_one->has_sensor = PR_FALSE;
            self->demon_one->martyr_enemy = world;
        }

        if (IsValidFieldGen(self->demon_two))
        {
            self->demon_two->has_sensor = PR_FALSE;
            self->demon_two->martyr_enemy = world;
        }

        self->demon_one = world;
        self->demon_two = world;

        dremove(self);

        // report this, as it shouldn't happen
        RPrint("Debug: Field entity removed in Field_think()\n");
        return;
    }
    else
        self->nextthink = time + 0.25; // 4hz rate

    // Make visuals if needed
    if (self->demon_one->has_holo == PR_FIELDGEN_ISWORKING)
        Field_MakeVisual(self);

    // checks for anything stuck in field :)
    entity te;
    float frange;

    frange = PR_FIELDGEN_RANGE;

    if (FieldIsImproved(self) & PR_IMPROVED_FOUR)
    {
        frange = PR_FIELDGEN_HACKEDRANGE; // at least 1 hacked for range

        if (self->demon_one->all_active & PR_IMPROVED_FOUR && self->demon_two->all_active & PR_IMPROVED_FOUR)
            frange = PR_FIELDGEN_HACKEDRANGE2; // both field generators hacked
    }

    te = findradius(self->origin, frange);
    while (te != world)
	{
		if (te != self)
        if (te != self->demon_one)
        if (te != self->demon_two)
        if (te->velocity == V({0, 0, 0}))
        if (te->classname != "force_field")
        if (EntsTouching2(te, self))
        {
            other = te;
            deathmsg = PR_DMSG_STUCK_FORCEFIELD;
            self->dmg = PR_FIELD_DMGINSIDE; // this gonna hurt
            Field_touch_SUB();
		}

		te = te->chain;
	}
}

//=============================================================================================
// is one entity actually intersecting the other one? (this avoids using a stupid trigger)
// PZ NOTE: I'm not sure why he didn't use bounding box collision detection that calls .touch().
//          I'm guessing it only works on solid entities that you can't walk through.
float EntsTouching2(entity e1, entity e2)
{
    if (e1->absmin[X] > e2->absmax[X])
        return PR_FALSE;
    if (e1->absmin[Y] > e2->absmax[Y])
        return PR_FALSE;
    if (e1->absmin[Z] > e2->absmax[Z])
        return PR_FALSE;

    if (e1->absmax[X] < e2->absmin[X])
        return PR_FALSE;
    if (e1->absmax[Y] < e2->absmin[Y])
        return PR_FALSE;
    if (e1->absmax[Z] < e2->absmin[Z])
        return PR_FALSE;

    return PR_TRUE;
}

//=================================================================================
// these 2 functions return the current hacks that should apply to the field

int FieldIsImproved(entity tfield)
{
    if (IsValidFieldGen(tfield->demon_one) && IsValidFieldGen(tfield->demon_two))
        return tfield->demon_one->all_active | tfield->demon_two->all_active;

    if (IsValidFieldGen(tfield->demon_one))
        return tfield->demon_one->all_active;

    if (IsValidFieldGen(tfield->demon_two))
        return tfield->demon_two->all_active;

    return 0;
}

int FieldIsMalfunctioning(entity tfield)
{
    if (IsValidFieldGen(tfield->demon_one) && IsValidFieldGen(tfield->demon_two))
        return tfield->demon_one->is_malfunctioning | tfield->demon_two->is_malfunctioning;

    if (IsValidFieldGen(tfield->demon_one))
        return tfield->demon_one->is_malfunctioning;

    if (IsValidFieldGen(tfield->demon_two))
        return tfield->demon_two->is_malfunctioning;

    return 0;
}

//=================================================================
// disables force field

void DisableField(entity tfield, float timedisable)
{
    if (IsValidFieldGen(tfield->demon_one))
    {
        tfield->demon_one->has_holo = PR_FIELDGEN_ISDISABLED;

        if (tfield->demon_one->no_grenades_2 < time + timedisable)
            tfield->demon_one->no_grenades_2 = time + timedisable;
    }

    if (IsValidFieldGen(tfield->demon_two))
    {
        tfield->demon_two->has_holo = PR_FIELDGEN_ISDISABLED;

        if (tfield->demon_two->no_grenades_2 < time + timedisable)
            tfield->demon_two->no_grenades_2 = time + timedisable;
    }

    sound (tfield, PR_CHAN_VOICE, "misc/ffbeep.wav", 0.4, PR_ATTN_IDLE);
}

//=========================================================================================
// applies damage and makes the sound and visual effect for the forcefield shock

void Field_touch_SUB()
{
    if (FieldIsMalfunctioning(self) & PR_SCREWUP_THREE) // reduce output
        self->dmg = 1;

    #ifdef PR_GIBABLE_CORPSES
    if (other->PR_corpseflag == PR_STRFLAG_CORPSE) // Corpses
    {
        if (Field_ItCanPass(self, other))
        {
            DisableField(self, PR_FIELDGEN_TIMEDISABLED);
            return;
        }
        else
        {
            if (Field_ShouldDamage(self, other))
                TF_T_Damage(other, self, self->real_owner, self->dmg, 0, PR_TF_TD_ELECTRICITY);
        }
    }
    else
    #endif
    if (other->classname == "player" && other->health > 0) // PLAYERS (alive)
    {
    	if (other->playerclass == PR_PC_UNDEFINED) // Observers (they have 1 hp)
	    	return;

        if (Field_ItCanPass(self, other))
        {
            DisableField(self, PR_FIELDGEN_TIMEDISABLED);
            return;
        }
        else
        {
            if (Field_ShouldDamage(self, other))
                TF_T_Damage(other, self, self->real_owner, self->dmg, PR_TF_TD_NOTTEAM, PR_TF_TD_ELECTRICITY);
        }
    }
    else // non player entities (or player heads)
    {
		// Gizmo - allow non demons and non army soldiers to be affected (coop monsters)
        if (IsMonster(other))
        {
            if (Field_ItCanPass(self, other))
            {
                DisableField(self, PR_FIELDGEN_TIMEDISABLED);
                return;
            }
            else if (Field_ShouldDamage(self, other))
                TF_T_Damage(other, self, self->real_owner, self->dmg, PR_TF_TD_NOTTEAM, PR_TF_TD_ELECTRICITY);
        }
        else // non-monsters non-alive-players entities (EVERYTHING ELSE)
        {
            // excludes entities that shouldnt be moved, doors plats etc..
			// PZ: Added movetype noclip to keep waypoints from being moved by fieldgens.
            if (other->movetype == PR_MOVETYPE_NONE || other->movetype == PR_MOVETYPE_PUSH || other->movetype == PR_MOVETYPE_NOCLIP ||
			    other == world)
                return;

            if (other->flags & PR_FL_ONGROUND)
                other->velocity[Z] = other->velocity[Z] + 100 + 160*random();

            other->flags = other->flags - (other->flags & PR_FL_ONGROUND);

            if (self->cnt) // Y Alignment
            {
                if (other->origin[X] < self->origin[X])
                    other->velocity[X] = (0 - other->velocity[X]) - (80 + 100 * random());
                else
                    other->velocity[X] = (0 - other->velocity[X]) + (80 + 100 * random());
            }
            else // X Alignment
            {
                if (other->origin[Y] < self->origin[Y])
                    other->velocity[Y] = (0 - other->velocity[Y]) - (80 + 100 * random());
                else
                    other->velocity[Y] = (0 - other->velocity[Y]) + (80 + 100 * random());
            }
        }
    }

    // Do the effects for shock
    FieldExplosion(self, other->origin, other);
    PutFieldWork(self);
}

#ifdef PR_FIELD_FORCEMODE

//==========================================================================
// gets one of our field generators

entity GetMyFieldGen(entity myself)
{
    entity te;
    float foundit;
    te = world;
    foundit = PR_FALSE;

    te = find(world, "classname", "building_fieldgen");
    while (te != world && foundit == PR_FALSE)
    {
        if (te->real_owner == myself) // is it ours?
            foundit = PR_TRUE; // yeah, found it

        if (foundit == PR_FALSE) // our search must continue...
            te = find(te, "classname", "building_fieldgen");
    }

    return te;
}

//=========================================================================
// these functions retrieve and set the current 'forced status' on a field

float Field_GetForcedStatus(entity tfield)
{
    if (IsValidFieldGen(tfield->demon_one) && IsValidFieldGen(tfield->demon_two))
    {
        if (tfield->demon_two->has_tesla || tfield->demon_one->has_tesla)
            return PR_TRUE;
    }
    else if (IsValidFieldGen(tfield->demon_one))
    {
        if (tfield->demon_one->has_tesla)
            return PR_TRUE;
    }
    else if (IsValidFieldGen(tfield->demon_two))
    {
        if (tfield->demon_two->has_tesla)
            return PR_TRUE;
    }

    return PR_FALSE;
}

//==============================================================================
// player functions called on menu.qc to disable/enable forced status on field

void SetFieldForcedStatus(float value)
{
    entity gen1, gen2;
    gen1 = GetMyFieldGen(self);
    gen2 = Find_OtherGen(gen1);

    if (IsValidFieldGen(gen1))
    {
        gen1->has_tesla = value;
        if (value)
            gen1->no_grenades_2 = time;
    }

    if (IsValidFieldGen(gen2))
    {
        gen2->has_tesla = value;
        if (value)
            gen2->no_grenades_2 = time;
    }
}

float GetFieldForcedStatus()
{
    entity gen1, gen2;
    gen1 = GetMyFieldGen(self);
    gen2 = Find_OtherGen(gen1);

    if (IsValidFieldGen(gen1) && IsValidFieldGen(gen2))
    {
        if (gen1->has_tesla || gen2->has_tesla)
            return PR_TRUE;
    }
    else if (IsValidFieldGen(gen1))
        return gen1->has_tesla;
    else if (IsValidFieldGen(gen2))
        return gen2->has_tesla;

    return PR_FALSE;

}

#endif

//=========================================================================
// returns TRUE if 'who' entity should be able to pass thru the field

float Field_ItCanPass(entity tfield, entity who)
{

#ifdef PR_FIELD_TEST
    return PR_FALSE;
#endif

    if (FieldIsMalfunctioning(tfield) & PR_SCREWUP_ONE) // Malfunctioning, always block
        return PR_FALSE;

    if (who == tfield->real_owner) // field owner - always pass
        return PR_TRUE;

#ifdef PR_FIELD_FORCEMODE

    if (Field_GetForcedStatus(tfield))
        return PR_FALSE;

#endif

    if (who->classname == "player") // PLAYERS
    {
        if (Teammate(who, tfield->real_owner)) // teammate
            return PR_TRUE;

        if (Teammate(who->undercover_team, tfield->real_owner)) // spies disguised as our team
            return PR_TRUE;
    }
    else if (IsMonster(who))  // MONSTERS/ARMY		// Gizmo - allow non demons and non army soldiers to be affected (coop monsters)
    {
		//float temp;
		//temp = GetTeam(who);
        if (Teammate(who, tfield->real_owner)) // team monster
            return PR_TRUE;
    }
#ifdef PR_GIBABLE_CORPSES
    else if (who->PR_corpseflag == PR_STRFLAG_CORPSE) // CORPSES
    {
        if (Teammate(who, tfield->real_owner)) // team corpse
            return PR_TRUE;
    }
#endif

    return PR_FALSE;
}

//=========================================================================
// returns TRUE if 'who' entity should be damaged by the field

float Field_ShouldDamage(entity tfield, entity who)
{
#ifdef PR_FIELD_TEST
    return PR_TRUE;
#endif

    if (FieldIsMalfunctioning(tfield) & PR_SCREWUP_ONE) // Malfunctioning, hurts always
        return PR_TRUE;

    if (who->classname == "player") // PLAYERS
    {
        if (who == tfield->real_owner) // field owner
            return PR_FALSE;

        if (Teammate(who, tfield->real_owner)) // teammate
            return PR_FALSE;

        if (Teammate(who->undercover_team, tfield->real_owner)) // spies disguised as our team
            return PR_FALSE;
    }
    else if (IsMonster(who))  // MONSTERS/ARMY		// Gizmo - allow non demons and non army soldiers to be affected (coop monsters)
    {
		//float temp;
		//temp = GetTeam(who);
        if (Teammate(who, tfield->real_owner)) // team monster
            return PR_FALSE;
    }
#ifdef PR_GIBABLE_CORPSES
    else if (who->PR_corpseflag == PR_STRFLAG_CORPSE) // CORPSES
    {
        if (Teammate(who, tfield->real_owner)) // team corpse
            return PR_FALSE;
    }
#endif

    return PR_TRUE;
}

//=============================================================================
// applies the particle effect and electric sound (at max 4hz rate)

void FieldExplosion(entity tfield, const vector& where, entity thing)
{
    if (!tfield->has_camera)
    {
        if (thing == world || thing->is_removed) return;

        vector whereFX;
        whereFX = where;
        whereFX[Z] = tfield->origin[Z];

        spawnFOG(whereFX);
        sound(tfield, PR_CHAN_BODY, "effects/crunch.wav", 0.5, PR_ATTN_NORM);

        tfield->has_camera = PR_TRUE; // cya soon (this gets reset on every field think)
    }
}

//================================================================
// Refresh working time for the force field

void PutFieldWork(entity field)
{
    if (IsValidFieldGen(field->demon_one))
        field->demon_one->tp_grenades_1 = time + PR_FIELDGEN_WORKTIME;
    if (IsValidFieldGen(field->demon_two))
        field->demon_two->tp_grenades_1 = time + PR_FIELDGEN_WORKTIME;
}

//==============================================================
// Force field touch function

void Field_touch()
{
    if (other->classname == "force_field") return; // avoid weird loops with other fields

    self->dmg = PR_FIELD_DMG;

    deathmsg = PR_DMSG_FORCEFIELD;
    Field_touch_SUB();
}

//===================================================================================
// creates the force field between the 2 generators (only if none currently on)

void Create_Field(entity gen1, entity gen2)
{
    // Check for existing force field, and abort if any
    if (gen1->has_sensor || gen2->has_sensor)
        return;

    if (gen1->martyr_enemy != world || gen2->martyr_enemy != world) // 2nd CHECK
        return;

    // already checked b4 on CanLink -> CanIdle
    /*if (!IsValidFieldGen(gen1) || !IsValidFieldGen(gen2))
        return;*/

    gen1->has_holo = PR_FIELDGEN_ISENABLED;
    gen2->has_holo = PR_FIELDGEN_ISENABLED;

    gen1->has_sensor = PR_TRUE;
    gen2->has_sensor = PR_TRUE;

    entity tfield;

    // generate field
    tfield = spawn();  // PZ: don't see a model getting set for this; might could get away with it being a server-only entity
    tfield->classname = "force_field";
    tfield->owner = world;
    tfield->real_owner = gen1->real_owner; // --> player

    tfield->think = Field_think;
    tfield->touch = Field_touch;
    tfield->nextthink = time + 0.25;

    // set pos and size
    tfield->origin = gen1->origin + V({0, 0, 32});
    tfield->absmin[Z] = gen1->origin[Z] - 32;
    tfield->absmax[Z] = gen1->origin[Z] + 32;

    tfield->mins[Z] = 0 - 32;
    tfield->maxs[Z] = 32;
    tfield->size[Z] = 64;

    float diff;

    if (gen1->origin[X] == gen2->origin[X])
    {
        tfield->cnt = 0;

        tfield->origin[X] = gen1->origin[X];
        tfield->absmin[X] = gen1->origin[X] - 2;
        tfield->absmax[X] = gen1->origin[X] + 2;

        tfield->maxs[X] = 2;
        tfield->mins[X] = 0 - 2;
        tfield->size[X] = 4;

        if (gen1->origin[Y] > gen2->origin[Y])
        {
            diff = (gen1->origin[Y] - gen2->origin[Y])/2;

            tfield->origin[Y] = gen1->origin[Y] - diff;
            tfield->absmin[Y] = gen2->origin[Y];
            tfield->absmax[Y] = gen1->origin[Y];

            tfield->maxs[Y] = diff;
            tfield->mins[Y] = 0 - diff;
            tfield->size[Y] = diff * 2;
        }
        else
        {
            diff = (gen2->origin[Y] - gen1->origin[Y])/2;

            tfield->origin[Y] = gen2->origin[Y] - diff;
            tfield->absmin[Y] = gen1->origin[Y];
            tfield->absmax[Y] = gen2->origin[Y];

            tfield->maxs[Y] = diff;
            tfield->mins[Y] = 0 - diff;
            tfield->size[Y] = diff * 2;
        }
    }
    else
    {
        tfield->cnt = 1;

        tfield->origin[Y] = gen1->origin[Y];
        tfield->absmin[Y] = gen1->origin[Y] - 2;
        tfield->absmax[Y] = gen1->origin[Y] + 2;

        tfield->maxs[Y] = 2;
        tfield->mins[Y] = 0 - 2;
        tfield->size[Y] = 4;

        if (gen1->origin[X] > gen2->origin[X])
        {
            diff = (gen1->origin[X] - gen2->origin[X])/2;

            tfield->origin[X] = gen1->origin[X] - diff;
            tfield->absmin[X] = gen2->origin[X];
            tfield->absmax[X] = gen1->origin[X];

            tfield->maxs[X] = diff;
            tfield->mins[X] = 0 - diff;
            tfield->size[X] = diff * 2;

        }
        else
        {
            diff = (gen2->origin[X] - gen1->origin[X])/2;

            tfield->origin[X] = gen2->origin[X] - diff;
            tfield->absmin[X] = gen1->origin[X];
            tfield->absmax[X] = gen2->origin[X];

            tfield->maxs[X] = diff;
            tfield->mins[X] = 0 - diff;
            tfield->size[X] = diff * 2;
        }
    }

    // apply stuff
    tfield->movetype = PR_MOVETYPE_NONE;
    tfield->solid = PR_SOLID_BBOX;
    setsize(tfield, tfield->mins, tfield->maxs);
    setorigin(tfield, tfield->origin);

    // assign the pointers on the field generators
    gen1->martyr_enemy = tfield;
    gen2->martyr_enemy = tfield;

    // assign the pointers to generators on ourselves
    tfield->demon_one = gen1;
    tfield->demon_two = gen2;

    // make activation sound
    sound (tfield, PR_CHAN_VOICE, "misc/ffact.wav", 0.2, PR_ATTN_NORM);

    // initialize sound flags on field
    tfield->has_sensor = PR_FALSE;
    tfield->has_holo = PR_FALSE;

    // flash generators
    gen1->effects = PR_EF_DIMLIGHT | PR_EF_RED;
    gen1->has_teleporter = PR_TRUE;
    gen1->skin = 2;
    gen2->effects = PR_EF_DIMLIGHT | PR_EF_RED;
    gen2->has_teleporter = PR_TRUE;
    gen2->skin = 2;
}

//=================================================================
// removes the force field (if any)

void Remove_Field(entity gen1, entity gen2)
{
    float soundsoff;
    soundsoff = PR_FALSE;

    if (IsValidFieldGen(gen1))
    {
        gen1->has_sensor = PR_FALSE;

        if (IsValidField(gen1->martyr_enemy))
        {
            Field_StopSounds(gen1->martyr_enemy); // Stops sounds on force field
            soundsoff = PR_TRUE;

            dremove(gen1->martyr_enemy);
            gen1->martyr_enemy = world;
        }
    }

    if (IsValidFieldGen(gen2))
    {
        gen2->has_sensor = PR_FALSE;

        if (IsValidField(gen2->martyr_enemy))
        {
            if (!soundsoff)
                Field_StopSounds(gen2->martyr_enemy); // Stops sounds on force field if not done

            dremove(gen2->martyr_enemy);
            gen2->martyr_enemy = world;
        }
    }
}

//======================================================================
// The following 2 functions are used for safety everywhere

float IsValidField(entity field)
{
    if (field == world)
        return PR_FALSE;

    if (field->classname != "force_field")
        return PR_FALSE;

    return PR_TRUE;
}

float IsValidFieldGen(entity fieldgen)
{
    if (fieldgen == world)
        return PR_FALSE;

    if (fieldgen->classname != "building_fieldgen")
        return PR_FALSE;

    return PR_TRUE;
}

//========================================================
// starts or removes sounds on the field

void Field_UpdateSounds(entity tfield, entity gen1)
{
    //.has_holo   : hum
    //.has_sensor : shield

    if (IsValidField(tfield)) // only if there is a field currently
    {
        float playhum, playshield;

        playhum = PR_FALSE;
        playshield = PR_FALSE;

        /*if (gen1.has_holo == #FIELDGEN_ISOFF) // for some reason we r not working
        {
            playhum = #FALSE;
            playshield = #FALSE;
        }
        else if (gen1.has_holo == #FIELDGEN_ISIDLE) // awaiting for link
        {
            playhum = #FALSE;
            playshield = #FALSE;
        }
        else if (gen1.has_holo == #FIELDGEN_ISDISABLED) // teammate passing thru the field?
        {
            playhum = #FALSE;
            playshield = #FALSE;
        }
        else*/

        if (gen1->has_holo == PR_FIELDGEN_ISENABLED)
        {
            playhum = PR_TRUE;
            playshield = PR_FALSE;
        }
        else if (gen1->has_holo == PR_FIELDGEN_ISWORKING)
        {
            playhum = PR_TRUE;
            playshield = PR_TRUE;
        }

        // ok, lets update the sounds if needed..
        if (!playhum)
        {
            if (tfield->has_holo)
            {
                sound(tfield,PR_CHAN_MISC,"misc/null.wav",0.5,PR_ATTN_NORM);
                tfield->has_holo = PR_FALSE;
            }
        }
        else
        {
            if (!tfield->has_holo || tfield->has_tesla < time)
            {
                sound(tfield,PR_CHAN_MISC,"misc/ffhum.wav",0.4,PR_ATTN_NORM);
                tfield->has_tesla = time + PR_FIELD_SOUNDSRATE;
                tfield->has_holo = PR_TRUE;
            }
        }

        if (!playshield)
        {
            if (tfield->has_sensor)
            {
                sound(tfield,PR_CHAN_ITEM,"misc/null.wav",0.5,PR_ATTN_NORM);
                tfield->has_sensor = PR_FALSE;
            }
        }
        else
        {
            if (!tfield->has_sensor || tfield->has_sentry < time)
            {
                //TODO?: lower volume as (FIELDGEN_WORKTIME - time) decreases

                sound(tfield,PR_CHAN_ITEM,"misc/ffield.wav",0.5,PR_ATTN_NORM);
                tfield->has_sentry = time + PR_FIELD_SOUNDSRATE;
                tfield->has_sensor = PR_TRUE;
            }
        }
    }
}

//====================================================================
// Called on each force field removal to stop any sounds from it

void Field_StopSounds(entity tfield)
{
    // We dont do any check because this function is called with everything already checked
    sound(tfield,PR_CHAN_ITEM,"misc/null.wav",0.5,PR_ATTN_NORM);
    sound(tfield,PR_CHAN_MISC,"misc/null.wav",0.5,PR_ATTN_NORM);
    tfield->has_sensor = PR_FALSE;
    tfield->has_holo = PR_FALSE;
}

//====================================================================
// do the lightning stuff while field is FIELDGEN_ISWORKING

void Field_MakeVisual(entity tfield)
{
    // To avoid some overhead, only do visual lightning effect sometimes
    if (random() < PR_FIELD_VISUALFACTOR)
        return;

    if (IsValidField(tfield))
    {
        float fx, fy;
        vector tmpvec;

        if (tfield->cnt)
        {
            fy = tfield->origin[Y];
            fx = tfield->origin[X] + (tfield->size[X]/2 - tfield->size[X] * random());
        }
        else
        {
            fx = tfield->origin[X];
            fy = tfield->origin[Y] + (tfield->size[Y]/2 - tfield->size[Y] * random());
        }

        tmpvec[X] = fx;
        tmpvec[Y] = fy;
        tmpvec[Z] = tfield->origin[Z] - 12;

        WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);

        if (random() > 0.5)
            WriteByte (PR_MSG_BROADCAST, PR_TE_LIGHTNING2);
        else
            WriteByte (PR_MSG_BROADCAST, PR_TE_LIGHTNING1);

	    WriteEntity (PR_MSG_BROADCAST, tfield);

        if (random() > 0.5)
        {
            WriteCoord (PR_MSG_BROADCAST, fx);
            WriteCoord (PR_MSG_BROADCAST, fy);
            WriteCoord (PR_MSG_BROADCAST, tfield->origin[Z] - 12);
            WriteCoord (PR_MSG_BROADCAST, fx);
            WriteCoord (PR_MSG_BROADCAST, fy);
            WriteCoord (PR_MSG_BROADCAST, tfield->origin[Z] + 12);
        }
        else
        {
            WriteCoord (PR_MSG_BROADCAST, fx);
            WriteCoord (PR_MSG_BROADCAST, fy);
            WriteCoord (PR_MSG_BROADCAST, tfield->origin[Z] + 22);
            WriteCoord (PR_MSG_BROADCAST, fx);
            WriteCoord (PR_MSG_BROADCAST, fy);
            WriteCoord (PR_MSG_BROADCAST, tfield->origin[Z] - 2);
        }

        #ifdef PR_QUAKE_WORLD
        multicast (tmpvec, PR_MULTICAST_PHS);
        #endif
    }
}

//==================================================
// called every frame by the field generators

void FieldGen_think()
{
    entity othergen = Find_OtherGen(self); // get our brother

    // Create the force field if it's possible, or remove it if not and exists
    if (FieldGens_CanLink(self, othergen))
        Create_Field(self,othergen); // checks redundancy itself
    else
        Remove_Field(self,othergen); // checks redundancy itself

    // field gens main loop (ai? heh.. my cat is smarter than these force fields)
    if (self->has_holo == PR_FIELDGEN_ISOFF) // for some reason we r not working
    {
        self->effects = 0;
        self->skin = 0;

        if (FieldGen_CanIdle(self)) // can we go idle?
            self->has_holo = PR_FIELDGEN_ISIDLE;
    }
    else if (self->has_holo == PR_FIELDGEN_ISIDLE) // trying to link
    {
        self->effects = 0;
        self->skin = 0;

        if (self->no_grenades_1 < time) // trying to link sound/flash time
        {
            sound (self, PR_CHAN_WEAPON, "misc/fffail.wav", 0.5, PR_ATTN_IDLE);
            self->skin = 1;
            self->effects = PR_EF_DIMLIGHT;
            self->no_grenades_1 = time + PR_FIELDGEN_LINKTIME;
        }
    }
    else if (self->has_holo == PR_FIELDGEN_ISDISABLED) // teammate passing thru the field?
    {
        self->effects = 0;
        self->skin = 0;

        // time check
        if (self->no_grenades_2 < time) // can we go idle?
        {
            self->has_holo = PR_FIELDGEN_ISIDLE;
            self->tp_grenades_1 = 0;
        }
    }
    else if (self->has_holo == PR_FIELDGEN_ISENABLED) // we r ready and linked
    {
        if (!self->has_teleporter)
        {
            self->effects = 0;
            self->skin = 1;
        }

        if (self->has_sensor == PR_FALSE)
            self->has_holo = PR_FIELDGEN_ISIDLE;

        if (self->tp_grenades_1 >= time)
            self->has_holo = PR_FIELDGEN_ISWORKING;
    }
    else if (self->has_holo == PR_FIELDGEN_ISWORKING) // hopefully after killing some1..
    {
        self->effects = PR_EF_DIMLIGHT;
        self->skin = 2;

        if (self->has_camera <= time)
        {
            self->ammo_cells = self->ammo_cells - PR_FIELDGEN_CELLSCOST;

            if (self->ammo_cells <= 24 && self->ammo_cells > 24 - PR_FIELDGEN_CELLSCOST)
                sprint(self->real_owner,PR_PRINT_HIGH,"Your field generator is low on cells\n");
            else if (self->ammo_cells < PR_FIELDGEN_CELLSCOST)
                sprint(self->real_owner,PR_PRINT_HIGH,"Your field generator has run out of cells\n");

            self->has_camera = time + PR_FIELDGEN_WORKING_RATE;
        }

        if (self->has_sensor == PR_FALSE)
            self->has_holo = PR_FIELDGEN_ISIDLE;
        else if (self->tp_grenades_1 <= time)
            self->has_holo = PR_FIELDGEN_ISENABLED;
    }

    if (!FieldGen_CanIdle(self)) // turn us off if needed
        self->has_holo = PR_FIELDGEN_ISOFF;

    Field_UpdateSounds(self->martyr_enemy,self); // update force field sounds

    self->has_teleporter = PR_FALSE; // resets 'flash' status bypass

    // PZ: If the field generator was just dropped by the zero gravity gun, and is now on the ground, make it
    //     auto-align with its mate field generator, as though it is being built.
    if (self->trigger_field != world && (self->flags & PR_FL_ONGROUND))
	{
		entity oldSelf = self;
		self = self->real_owner;
		vector pos = WhereGen(oldSelf->origin, PR_TRUE, oldSelf);
		self = oldSelf;
		setorigin(self, pos);
		// We don't need to update the position of the field between the two generators.
		// It updates itself, by removing and recreating itself, probably, when a fieldgen is moved.
		self->trigger_field = world; // handled the flag
	}

    self->nextthink = time + 0.1;
}

//=======================================================================
// returns TRUE if the generator could currently go to idle status

float FieldGen_CanIdle(entity fieldgen)
{
    if (!(IsValidFieldGen(fieldgen)))
        return PR_FALSE;

    if (fieldgen->ammo_cells >= PR_FIELDGEN_CELLSCOST &&
        !(fieldgen->is_malfunctioning & PR_SCREWUP_FOUR)
        && fieldgen->health > 0)
        return PR_TRUE;

    return PR_FALSE;
}

//=======================================================================
// returns TRUE if both generators could currently generate the field

float FieldGens_CanLink(entity fieldgen1, entity fieldgen2)
{
    if (!(IsValidFieldGen(fieldgen1)) || !(IsValidFieldGen(fieldgen2)))
        return PR_FALSE;

    if (!visible2(fieldgen1,fieldgen2))
        return PR_FALSE;

    float r;
    r = vlen(fieldgen1->origin - fieldgen2->origin); // get distance between generators

    // Both fieldgens hacked
    if ((fieldgen1->all_active & PR_IMPROVED_FOUR && fieldgen2->all_active & PR_IMPROVED_FOUR) && r > PR_FIELDGEN_HACKEDRANGE2)
        return PR_FALSE;

    // Only 1 fieldgen hacked
    if ( ((fieldgen1->all_active & PR_IMPROVED_FOUR && !(fieldgen2->all_active & PR_IMPROVED_FOUR)) || (fieldgen2->all_active & PR_IMPROVED_FOUR && !(fieldgen1->all_active & PR_IMPROVED_FOUR))) && r > PR_FIELDGEN_HACKEDRANGE)
        return PR_FALSE;

    // None hacked for range
    if (r > PR_FIELDGEN_RANGE && !(fieldgen1->all_active & PR_IMPROVED_FOUR || fieldgen2->all_active & PR_IMPROVED_FOUR))
        return PR_FALSE;

    if (fieldgen1->origin[Z] != fieldgen2->origin[Z]) // Different heights?
        return PR_FALSE;

	// PZ NOTE: Must line up due to inability to make diagonal bounding boxes. See my note at the top of the file.
    if (fieldgen1->origin[X] != fieldgen2->origin[X] && fieldgen1->origin[Y] != fieldgen2->origin[Y]) // Unaligned?
        return PR_FALSE;

    if (fieldgen1->has_holo == PR_FIELDGEN_ISDISABLED || fieldgen2->has_holo == PR_FIELDGEN_ISDISABLED)
        return PR_FALSE;

    if (fieldgen1->has_holo == PR_FIELDGEN_ISOFF || fieldgen2->has_holo == PR_FIELDGEN_ISOFF)
        return PR_FALSE;

    // Return TRUE only if basic conditions on CanIdle() are met
    if (FieldGen_CanIdle(fieldgen1) && FieldGen_CanIdle(fieldgen2))
        return PR_TRUE;

    return PR_FALSE;
}

//=============================================================================================
// initialize field generator stuff just after being built, called in engineer.qc

void FieldGen_Built(entity fieldgen)
{
    fieldgen->touch = SUB_Null;
    fieldgen->think = FieldGen_think;
    fieldgen->nextthink = time + 0.1;
    fieldgen->has_holo = PR_FIELDGEN_ISIDLE; // we start on IDLE status (searching for other gen to link)
    fieldgen->has_sensor = PR_FALSE;
    fieldgen->no_grenades_1 = time + 3;
    fieldgen->martyr_enemy = world;
}

//==============================================================
// returns our other generator (if any)

entity Find_OtherGen(entity fieldgen)
{
    entity te;
    float foundit;
    te = world;
    foundit = PR_FALSE;

    te = find(world, "classname", "building_fieldgen");
    while (te != world && foundit == PR_FALSE)
    {
        if (te->real_owner == fieldgen->real_owner) // is it ours?
        if (te != fieldgen)                       // and not the same generator..
            foundit = PR_TRUE; // yeah, found it

        if (foundit == PR_FALSE) // our search must continue...
            te = find(te, "classname", "building_fieldgen");
    }

    return te;
}

//=========================================================================================
// Returns the place where field gen will be built related to player current pos and yaw.
// Called in engineer.qc. `place` is the origin passed where other kinds of buildings are placed.
// PZ: Added `forGravityGun` for using this function when moving field generator with ZGG.
//     Added `fieldGen` for passing the field generator being moved by the ZGG.
// `self` is the player that owns the field generator.

vector WhereGen(const vector& place, bool forGravityGun /*= false*/, entity fieldGen /*= world*/)
{
    // if we have no field generator currently, it can be placed anywhere
    if (self->has_fieldgen == 0) return place;
    // PZ: Added this.
    if (forGravityGun && self->has_fieldgen <= 1) return place;

    vector retval;
    float r, distx, disty, foundit;
    entity te;

    foundit = PR_FALSE;

    // find the other generator (PZ NOTE: its mate)
    te = find(world, "classname", "building_fieldgen");
    while (te != world && foundit == PR_FALSE)
    {
        if (te->real_owner == self && te != fieldGen) // is it ours? // PZ: and not the fieldGen being moved
            foundit = PR_TRUE;      // yeah, found it

        if (foundit == PR_FALSE) // our search must continue...
            te = find(te, "classname", "building_fieldgen");
    }

    // check for error getting the other gen
    if (te == world || te->classname != "building_fieldgen" || foundit == PR_FALSE)
    {
        RPrint("BUG: Error in field generator placement routine. 'WhereGen()'\n");
        return place;
    }

    // calculate the new generator pos
    distx = fabs(place[X] - te->origin[X]);
    disty = fabs(place[Y] - te->origin[Y]);
    retval = place;

    if (distx < disty)
        retval[X] = te->origin[X]; // PZ NOTE: line up on the X axis
    else
        retval[Y] = te->origin[Y]; // PZ NOTE: line up on the Y axis

    // determine whether we are close enough to use lined up position
	float distanceAllowed; // PZ: Added
    if (!forGravityGun) distanceAllowed = 200;
    else                distanceAllowed = 4;   // make this too large, and players can't reposition stuff as finely; make it too small, and it's hard to relink
	if (!forGravityGun)                  // PZ: Added code for ZGG.
		r = vlen(self->origin - retval); // get distance from generator owner to the adjusted location
	else
		r = vlen(place - retval);        // PZ: get distance from the fieldgen to the adjusted location
	if (r > distanceAllowed)             // are we too far away?
		retval = place;                  // then bypass the calc

    // print message if they wont link   (PZ: Added "else"s to make it only print one message at a time.)
	r = vlen(te->origin - retval);       // get distance between generators
    if (!vis2orig(te->origin, retval))
        sprint(self, PR_PRINT_HIGH, "Your field generators won't link. There are obstacles between them!\n");
    else if (te->all_active & PR_IMPROVED_FOUR && r > PR_FIELDGEN_HACKEDRANGE)
        sprint(self, PR_PRINT_HIGH, "Your field generators are too far away to link, even hacked\n");
    else if (r > PR_FIELDGEN_RANGE && !(te->all_active & PR_IMPROVED_FOUR))
        sprint(self, PR_PRINT_HIGH, "Your field generators are too far away to link\n");
    /*else if (retval_z != te.origin_z)
        sprint(self, #PRINT_HIGH, "Your field generators are at different heights, they won't link\n");*/
    else if (retval[X] != te->origin[X] && retval[Y] != te->origin[Y])
        sprint(self, PR_PRINT_HIGH, "Your field generators are not lined up. They won't link!\n");

    // return the final building place
    return retval;
}

//======================================================================
// damn! our field generator was destroyed. Force field must go down..

void FieldGen_Die()
{
    self->real_owner->has_fieldgen = self->real_owner->has_fieldgen - 1;
    if (self->real_owner->has_fieldgen < 0) self->real_owner->has_fieldgen = 0;

    WriteByte (PR_MSG_BROADCAST, PR_SVC_TEMPENTITY);
    WriteByte (PR_MSG_BROADCAST, PR_TE_EXPLOSION);
	WriteCoord (PR_MSG_BROADCAST, self->origin[X]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Y]);
	WriteCoord (PR_MSG_BROADCAST, self->origin[Z]);
#ifdef PR_QUAKE_WORLD
	multicast (self->origin, PR_MULTICAST_PHS);
#endif

    // check if field should be removed..
    entity othergen;
    othergen = Find_OtherGen(self);

    if (IsValidFieldGen(othergen))
       Remove_Field(self, othergen);
    else
       Remove_Field(self, world); // extra removal, not needed i think...

    sprint(self->real_owner, PR_PRINT_HIGH, "Your field generator was destroyed.\n");

    entity sprite;

    sprite = SpawnSprite(1,PR_SPRITE_AIRBURST,self->origin,V({0, 0, 0}),PR_SPRITEMOVE_UPSLOW,0.1);

    if (sprite != world)
    {
        sprite->effects = PR_EF_DIMLIGHT;

        if (self->real_owner->team_no == 1)
            sprite->effects = PR_EF_DIMLIGHT | PR_EF_BLUE;
        else if (self->real_owner->team_no == 2)
            sprite->effects = PR_EF_DIMLIGHT | PR_EF_RED;
    }

	ThrowGib("progs/tesgib3.mdl", -70,PR_TRUE,0,0, PR_FALSE);

    dremove(self);
}

//=========================================================================
// Engineer has used a Spanner on the field generator

void Engineer_UseFieldGen(entity field)
{
	self->building = field;

	if (Teammate(self->building->real_owner, self) && self->building->is_malfunctioning & PR_SCREWUP_THREE)
	{
		sprint(self,PR_PRINT_HIGH,"Trapped field generator, have a nice day!\n");

		deathmsg = PR_DMSG_FGTRAP;
		sound (self, PR_CHAN_MISC, "effects/crunch.wav", 1, PR_ATTN_NONE);
		TF_T_Damage(self, self, self, self->health + 60, 0, PR_TF_TD_OTHER);
		return;
	}

	entity dist_checker;
	string st;

	sprint(self, PR_PRINT_HIGH, "Field Generator has ");
	st = ftos(field->health);
	sprint(self, PR_PRINT_HIGH, st);
    sprint(self, PR_PRINT_HIGH, S_("^b/^b"));
	st = ftos(field->max_health);
    sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_(" ^bhealth^b, "));
	st = ftos(field->ammo_cells);
	sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_("^b/^b"));
	st = ftos(field->maxammo_cells);
    sprint(self, PR_PRINT_HIGH, st);
	sprint(self, PR_PRINT_HIGH, S_(" ^bcells^b\n"));

    // Pop up the menu
	self->current_menu = PR_MENU_ENGINEER_FIX_FIELDGEN;
	self->menu_count = PR_MENU_REFRESH_RATE;

	//dodgy (PZ NOTE: this just makes enemies to the building automatically dismantle it upon spannering it)
	if (/*teamplay != 0 &&*/ !Teammate(self->building->real_owner, self)) {
		Menu_EngineerFix_FieldGen_Input(4);
		return;
	}

	// Start a Distance checker, which removes the menu if the player
	// gets too far away from the field generator.
	dist_checker = spawnServerSide(); // PZ: make it a server-side only entity
	dist_checker->classname = "timer";
	dist_checker->owner = self;
	dist_checker->enemy = field;
	dist_checker->think = CheckDistance;
	dist_checker->nextthink = time + 0.3;
}

//===========================================================================
// Any external code should use this function, where needed

void FieldEvent(entity tfield, const vector& where, entity thing)
{
	FieldExplosion(tfield, where, thing);
	PutFieldWork(tfield);
}

} // END namespace Progs
