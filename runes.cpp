/*=======================================================//
// runes.QC - CustomTF 3.2.OfN             - 28/9/2004 - //
// by Sergio Fuma�a Grunwaldt - OfteN [cp]               //
=========================================================//
 This file contains all the runes-specific code
---------------------------------------------------------//
 Runes are randomly scattered on map start if the server
 has enabled it with the info key named "runes".
 The possible choices for this setting are:
 0 = no runes, 1 to 4 a single player can hold upto this
 number of runes simultaneously, for himself.
 I recommend a setting of 1 or 2 for ovbious reasons! :)
 ________________________________________________________

 Every rune gives to the player that owns it a special
 efficiency increase, each rune in a different area.
 There are ALWAYS 4 runes anytime:

  1.- Triad Rune           (like quad but does x3 damage)
  2.- Resist Rune   (protects from damage and bad states)
  3.- Speed Rune (faster shoting + player speed increase)
  4.- Regen Rune      (regenerates health/armor and ammo)

 --------------------------------------------------------
 The effects of any rune are accumulated with other items
 or ability the player already has, like Quad, Auras etc.
 --------------------------------------------------------

 The Resist Rune cuts incoming damage for player that
 holds it, and also protects from judo, infection and
 most affections a player usually is vulnerable to.

 The Speed Rune increases player movement speed and acts
 as the aura of haste too (making player shot faster).
 It also cuts down reload times a bit.

 The Regen Rune refills health and armor at a greater
 rate than the regeneration aura, and it also refills
 all basic ammo's (shells, nails, rockets and cells).

=========================================================*/

#include "progs.h"
#include "ofndefs.h"
#include "debug.h"
#include "weapons.h"
#include "sprites.h"
#include "cpstuff.h"
#include "neo.h"

namespace Progs {

// Rune type flags - DO NOT MODIFY

/* Defined in ofndefs.qc
#define RUNE_TRIAD  1
#define RUNE_RESIS  2
#define RUNE_SPEED  4
#define RUNE_REGEN  8
*/

// Rune state possible values

//#define RUNESTATE_SPAWN       0 // Default, just spawned
#define PR_RUNESTATE_READY       1 // Everything assumed to be right
#define PR_RUNESTATE_WRONG       2 // Invalid rune, need to respawn
#define PR_RUNESTATE_RESPAWN     3 // Although probably a correct rune, need to respawn
#define PR_RUNESTATE_DROPED      4 // Just droped by a player, we should check stuff

// Miscellaneous settings for runes

#define PR_RUNES_DROPDISABLE_TIME   4 // Seconds for a player to not be able to pick up a rune after droping it
#define PR_RUNES_DECLAREDEAD_TIME  10 // Minutes to pass until a rune is considered unreachable and respawns
#define PR_RUNESPOT_ATTEMPTS      250 // Attempts for GetRuneSpot to do before failing
#define PR_COLOURED_RUNEFLASH         // If defined rune's flash will get a purple color (on gl clients only)

// Regeneration rune settings

#define PR_REGENRUNE_RATE           0.5
#define PR_REGENRUNE_NADE_RATE      15    // only currently used in Neo mode
#define PR_REGENRUNE_DET_RATE       60    // only currently used in Neo mode
#define PR_REGENRUNE_HP_AMMOUNT     2.25
#define PR_REGENRUNE_ARMOR_AMMOUNT  2.25
#define PR_REGENRUNE_AMMO_AMMOUNT   1

/*===============================================================================================

EXPLANATION OF HOW THE ENTITY FIELDS ARE USED (thnx? np.. :P)
---------------------------------------------

For a rune entity:
------------------

.has_sentry     - Kind of rune
.enemy          - Last person that droped this rune
.pain_finished  - Time at which this rune has been droped the last time
.tfstate        - Status of rune, see the rune state defines
.ltime          - Indicates the time this rune was spawned at

For the player entity:
----------------------

.#runes   - Indicates what runes the player is carrying, if 0 then none
            (field used defined on ofndefs.qc, currently we use a dedicated field called runes_owned)

================================================================================================*/

// Function declarations

void StartRunes();
void EndRunes();
void UpdateRuneStuff(float oldrunes);
entity SpawnRune(float runetype);
void SpawnNewRune(float runetype);
float GetPlayerNoRunes(entity player);
void PlayerTakesRune(entity player, entity rune);
void PlayerDropRunes(entity player);
vector GetRuneSpot();
float CheckRuneSpot(const vector& spot);
void ReportRuneTaken(entity player, float runetype);
void ReportRuneDrop(entity player, float runetype);
void RuneDisappear(entity rune);
void RuneAppear(entity rune);
string GetRuneString(float runetype);
void PlayerDropsRune(entity player, float runetype, float throwRune);
void RuneEndEffects(entity player, int runetype);
void RuneStartEffects(entity player, int runetype);
void SpawnRegenRuneTimer(entity player);
void RemoveRegenRuneTimer(entity player);
void InvalidateRune(entity rune);
float RuneOnSky(entity rune);
void RespawnRune(entity rune, float nice);

//==================================================
// Rune models get precacheed in this function

void PrecacheRuneModels()
{
    precache_model("progs/end1.mdl");
    precache_model("progs/end2.mdl");
    precache_model("progs/end3.mdl");
    precache_model("progs/end4.mdl");
}

//==============================================================================
// Retrieves server setting about runes

void GetRunesGlobal()
{
    string st;

    // Get rune setting for map, if any
    st = infokey(world,"runes");

    if (st != "")
    {
        runes = floor(stof(st)); // we use the custom setting..

        // Cap minimum/maximum's
        if (runes < 0)
            runes = 0;

        if (runes > 4)
            runes = 4;
    }
}

//===============================================================================
// Applies any changes on the rune setting

void UpdateRuneStuff(float oldrunes)
{
    if (oldrunes == 0 && runes != 0)
        StartRunes(); // We should start the party
    else if (oldrunes != 0 && runes == 0)
        EndRunes(); // We must close the store

    // TODO: Make players drop runes if needed after a runes setting change to less runes per player
}

//=========================================================================
// Spawns the 4 runes and places them randomly on the map

void StartRunes()
{
    if (runes == 0) return;

    // Spawn our 4 runes from scratch
    SpawnNewRune(PR_RUNE_TRIAD);
    SpawnNewRune(PR_RUNE_RESIS);
    SpawnNewRune(PR_RUNE_SPEED);
    SpawnNewRune(PR_RUNE_REGEN);

    // Notify to everybody!
    bprint(PR_PRINT_HIGH,"Environment\x8D Ancient Runes located around the zone!\n");
}

//=========================================================================
// Deletes the runes including any runes affecting players (not entities)

void EndRunes()
{
    entity te;
    float tmp;

    // Find and remove any rune entities
    te = find(world,"classname","rune");

    while (te != world)
    {
        // Make it disappear nicely
        RuneDisappear(te);

        te = find(te,"classname","rune");
    }

    // Find and remove any wrong rune entities
    te = find(world,"classname","runewrong");

    while (te != world)
    {
        // Remove it like anything, this entity is not even visible
        dremove(te);

        te = find(te,"classname","runewrong");
    }

    // Remove any runes "carried" by the players
    te = find(world,"classname","player");

    while (te != world)
    {
        if (te->is_connected)
        {
            if (te->PR_runes != 0)
            {
                if (!neo.isModeActive()) // modded for Neo mode
                {
                    if (GetPlayerNoRunes(te) > 1)
                        sprint(te,PR_PRINT_HIGH,"Your runes have lost their power.\n");
                    else
                        sprint(te,PR_PRINT_HIGH,"You have lost the power of your rune.\n");
                }

                stuffcmd(te,"bf\nplay items/itembk2.wav\n");
                tmp = te->PR_runes;
                te->PR_runes = 0;
                RuneEndEffects(te,tmp);
            }
        }

        te = find(te,"classname","player");
    }

    // Notify to everybody the party is over!
    if (!neo.isModeActive()) // modded for Neo mode
        bprint(PR_PRINT_HIGH,"Environment\x8D Ancient Runes don't exist anymore!\n");
}

//===========================================================================
// Returns a random valid point inside level to spawn a rune on

vector GetRuneSpot()
{
    vector retvec;
    float num;

    num = PR_RUNESPOT_ATTEMPTS;

    while (num > 0)
    {
        retvec[X] = (random() * 8192) - 4096;
        retvec[Y] = (random() * 8192) - 4096;
        retvec[Z] = (random() * 8192) - 4096;

        if (CheckRuneSpot(retvec))
            return retvec;

        num = num - 1;
    }

    // Return an always-invalid vector to identify the fail of this function
    return V({4096, 4096, 4096});
}

//========================================================================================
// Returns TRUE if the origin is a valid place for a rune (no solid, sky, slime or lava)

float CheckRuneSpot(const vector& spot)
{
    float content;

    // This gonna be fun to do
    content = pointcontents(spot);
    if (content == PR_CONTENT_EMPTY || content == PR_CONTENT_WATER)
    {
        content = pointcontents(spot + V({-16, -16, -24}));
        if (content == PR_CONTENT_EMPTY || content == PR_CONTENT_WATER)
        {
            content = pointcontents(spot + V({16, 16, 32}));
            if (content == PR_CONTENT_EMPTY || content == PR_CONTENT_WATER)
            {
                content = pointcontents(spot + V({-16, 16, 32}));
                if (content == PR_CONTENT_EMPTY || content == PR_CONTENT_WATER)
                {
                    content = pointcontents(spot + V({16, -16, 32}));
                    if (content == PR_CONTENT_EMPTY || content == PR_CONTENT_WATER)
                    {
                        content = pointcontents(spot + V({16, 16, -24}));
                        if (content == PR_CONTENT_EMPTY || content == PR_CONTENT_WATER)
                        {
                            content = pointcontents(spot + V({16, -16, -24}));
                            if (content == PR_CONTENT_EMPTY || content == PR_CONTENT_WATER)
                            {
                                content = pointcontents(spot + V({-16, 16, -24}));
                                if (content == PR_CONTENT_EMPTY || content == PR_CONTENT_WATER)
                                {
                                    content = pointcontents(spot + V({-16, -16, 32}));
                                    if (content == PR_CONTENT_EMPTY || content == PR_CONTENT_WATER)
                                        return PR_TRUE; // ok, spot *seems* fine (not 100% sure)
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Spot isn't a valid place for a rune
    return PR_FALSE;
}

//=====================================================================================
// Checks if the rune has landed on sky (seriously.. may happen)

float RuneOnSky(entity rune)
{
    if (pointcontents(rune->origin - V({0, 0, 24.1})) == PR_CONTENT_SKY)
        return PR_TRUE;

    return PR_FALSE;
}

//=====================================================================================
// Main think routine for runes, does visual/sound stuff, checks for any needed respawn

void Rune_think()
{
    // Check if we need to relocate/respawn this rune
    if (self->tfstate == PR_RUNESTATE_WRONG) // Invalid rune?
    {
        // Respawn a new one of same kind
        RespawnRune(self, PR_FALSE);
        return;
    }
    else if (self->tfstate == PR_RUNESTATE_RESPAWN) // Unreachable rune?
    {
        // Reallocate another rune of same kind
        RespawnRune(self, PR_TRUE);
        return;
    }
    // also check if this has been droped and got into a valid location or not
    else if (self->tfstate == PR_RUNESTATE_DROPED)
    {
        if (!CheckRuneSpot(self->origin)) // Invalid current position?
        {
            // Reallocate another rune of same kind
            RespawnRune(self, PR_TRUE);
            return;
        }

        // If the last check was done with the rune already on ground we can stop checking
        if (self->flags & PR_FL_ONGROUND)
        {
            if (RuneOnSky(self)) // Landed on sky? lol
            {
                // Replace the rune elsewhere
                RespawnRune(self, PR_TRUE);
                return;
            }

            // Everything seems fine then, apparently we have been dropped well..
            self->tfstate = PR_RUNESTATE_READY; // so reset state
        }
    }

    // Check if we should declare this rune in dead/unreachable spot to respawn it
    if (time > (self->ltime + (PR_RUNES_DECLAREDEAD_TIME * 60)))
        self->tfstate = PR_RUNESTATE_RESPAWN;

    // If on ground, do a little hop with light, sprite and sound, for players to be able to notice this rune easily
    if (self->flags & PR_FL_ONGROUND)
    {
        // Make it hop a little
        self->velocity[Z] = 190 + crandom()*50;

        // Make it do a magic sound
        sound(self,PR_CHAN_BODY,"player/slimbrn2.wav",1,PR_ATTN_NORM);

        // Spawn a magic sprite..
        entity te;

        // (the kind of sprite depends on content this rune is on)
        if (pointcontents(self->origin) ==  PR_CONTENT_EMPTY)
            te = SpawnSprite(1,PR_SPRITE_AIRBURST,self->origin + V({0, 0, 40}),V({0, 0, 0}),PR_SPRITEMOVE_UPSLOW,0.1);
        else
            te = SpawnSprite(1,PR_SPRITE_ABLAST,self->origin + V({0, 0, 40}),V({0, 0, 0}),PR_SPRITEMOVE_UPSLOW,0.2);

        //..and lit it
        #ifdef PR_COLOURED_RUNEFLASH
        te->effects = PR_EF_DIMLIGHT | PR_EF_PURPLE;
        #else
        te->effects = PR_EF_DIMLIGHT;
        #endif

        // flash the rune also
        MuzzleFlash(self);
    }

    // Set next think time
    self->nextthink = time + 3 + random()*7;
}

//======================================================================
// Touch function for runes, checks if able to be picked up

void Rune_touch()
{
    if (!ceasefire && !intermission_running) // not allowed during ceasefire or intermission
    if (prematch < time) // Only if not in prematch
    if (other->classname == "player")
    if (other->is_connected)
    if (other->health > 0) // alive?
    if (!(other->done_custom & PR_CUSTOM_BUILDING)) // skip ppl customizing
    if (other->playerclass != PR_PC_UNDEFINED) // skip observers
    if (other->penance_time < time) // Don't let lame teamkillers take runes
	{
        // First of all check if too early for this person to take it again
        if (self->enemy == other)
        if ((time - self->pain_finished) < PR_RUNES_DROPDISABLE_TIME)
            return; // too early still, dont take it

        // Then check if allowed to take another rune
        float numrunes;
        numrunes = GetPlayerNoRunes(other);

        // Does this player should take another one?
        if ((numrunes + 1) > runes)
            return; // nope.. so just ignore touch

        // Ok, he/she picks it up!
        PlayerTakesRune(other, self);
    }
}

//====================================================================
// Spawns and sets the basic stuff on a rune entity

entity SpawnRune(float runetype)
{
    entity rune;

    // Spawn a new entity for it
    rune = spawn();

    // This is the safest entity spawn ever on a quakeworld mod! :-)
    // Critical entity settings
    #ifndef PR_IDABLE_RUNES
    rune->flags = PR_FL_ITEM;
    #else
    rune->flags = PR_FL_ITEM | PR_FL_FINDABLE_NONSOLID;
    #endif
    rune->classname = "rune";
    rune->movetype = PR_MOVETYPE_TOSS;
    rune->solid = PR_SOLID_TRIGGER;
    rune->takedamage = PR_DAMAGE_NO;
    rune->owner = world;

    // Choose the correspoding model
    if (runetype == PR_RUNE_TRIAD)
        rune->mdl = "progs/end1.mdl";
    else if (runetype == PR_RUNE_RESIS)
        rune->mdl = "progs/end2.mdl";
    else if (runetype == PR_RUNE_SPEED)
        rune->mdl = "progs/end3.mdl";
    else if (runetype == PR_RUNE_REGEN)
        rune->mdl = "progs/end4.mdl";
    else
    {
        RPrint("BUG: Unknown rune type on SpawnRune()!\n");
        dremove(rune);
        return world;
    }

    // Assign model
    setmodel(rune,rune->mdl);

    // Use standard item size
    setsize (rune, V({-16, -16, -24}), V({16, 16, 32}));

    // Indicate which kind of rune this one is
    rune->has_sentry = runetype;

    // Assign description to it
    rune->netname = GetRuneString(runetype);

    // Set its main subroutines
    rune->think = Rune_think;
    rune->touch = Rune_touch;

    // First "rune-hop" or think
    rune->nextthink = time + 6 + random()*6;

    // Set our "spawned at" time flag
    rune->ltime = time;

    // Return a pointer to our rune
    return rune;
}

//=========================================================================
// Creates from scratch a rune of the specified kind and places it

void SpawnNewRune(float runetype)
{
    entity rune, oself;

    // Spawn our new rune
    rune = SpawnRune(runetype);

    // Check for error doing it
    if (rune == world)
    {
        RPrint("BUG: Unable to create entity on SpawnNewRune()!\n");
        return;
    }

    // Initialize with neutral values, our pointer and time for disable time after drop
    rune->enemy = world;
    rune->pain_finished = time;

    // Get a valid random start origin on the map
    rune->origin = GetRuneSpot();

    // Check if success getting the random point
    if (rune->origin == V({4096, 4096, 4096})) // nope?
    {
        InvalidateRune(rune); // another one will spawn and appear elsewhere
        return; // do nothing else
    }

    // And set it
    setorigin(rune,rune->origin);

    // Adjust it on ground and set ready flag if everything seems ok
    oself = self;
    self = rune;

    if (!droptofloor()) // didn't land?
        InvalidateRune(self); // another one will spawn and appear elsewhere
    else
    {
        // Do a content check on our final position
        if (CheckRuneSpot(self->origin))
        {
            // Landed on sky? lol
            if (RuneOnSky(self))
                InvalidateRune(self); // another one will spawn and appear elsewhere
            else
            {
                // Ok, everything seems correct, flag rune as valid and perform appearing effects
                self->tfstate = PR_RUNESTATE_READY;
                if (!neo.isModeActive()) // modified for Neo mode
                    RuneAppear(self);
            }
        }
        else // invalid location?
            InvalidateRune(self); // another rune will spawn and appear elsewhere
    }

    self = oself;
}

//============================================================================
// Removes inapropiate stuff from a wrong rune, and prepares it for respawn

void InvalidateRune(entity rune)
{
    // Flag it as wrong rune, which will make next think to respawn it
    rune->tfstate = PR_RUNESTATE_WRONG;

    // Make it invisible
    rune->modelindex = modelindex_null;

    // Stop it, and make it not trigger anything
    rune->classname = "runewrong";
    rune->movetype = PR_MOVETYPE_NONE;
    rune->velocity = V({0, 0, 0});
    rune->solid = PR_SOLID_NOT;

    // Remove touch function
    rune->touch = SUB_Null;

    // IMPORTANT: Do not RESET or CHANGE the think function of this entity, it's NEEDED FOR IT TO RESPAWN
}

//============================================================================
// A player is taking a rune (already checked if possible on rune touch)

void PlayerTakesRune(entity player, entity rune)
{
    // Check for error first
    if (player->PR_runes & rune->has_sentry) // Already has it? wtf..
    {
        RPrint("BUG: Player double picking up a rune!\n");
        return;
    }

    // Update our C+quakeC field for rune indication flag which will
    // update client status bar rune items also (done on C)
    player->PR_runes = player->PR_runes | rune->has_sentry;

    // Make rune sound as it's picked up
    sound (player, PR_CHAN_ITEM, "auras/aura3b.wav", 1, PR_ATTN_NORM);

    // Flash player screen
    stuffcmd(player,"bf;bf\n");

    // Flash player entity
    MuzzleFlash(player);

    // Report the rune has been taken by this bastard
    if (!neo.isModeActive()) // modded for Neo mode
        ReportRuneTaken(player,rune->has_sentry);

    // Starts any rune effects needed on player
    RuneStartEffects(player,rune->has_sentry);

    // Remove rune entity
    dremove(rune);
}

//===============================================================================
// A player drops a rune, spawn entity and set its velocity

void PlayerDropsRune(entity player, int runetype, float throwRune)  // PZ: `throwRune` was called "throw"; changed for C++
{
    // Check for error first
    if (!(player->PR_runes & runetype)) // Doesn't have it? wtf..
    {
        RPrint("BUG: Player dropping a buggy rune!\n");
        return;
    }

    // Remove our rune type flag and update client runes in status bar items (done by server code)
    player->PR_runes = player->PR_runes - runetype;

    // Spawn rune entity and launch it away
    entity rune;
    rune = SpawnRune(runetype);

    if (rune != world)
    {
        rune->origin = player->origin;
        setorigin(rune,rune->origin);

        if (throwRune) // Use player aiming, as we have thrown this rune on purpose
        {
            makevectors(player->v_angle);
            rune->velocity = player->velocity + v_forward * 256 + V({0, 0, 340});
        }
        else // Thrown not on purpose, so in a random direction, mainly above player
        {
            rune->velocity[Z] = 320+random()*30;
            rune->velocity[X] = crandom()*60;
            rune->velocity[Y] = crandom()*60;

            // Add player velocity for extra realism :P
            rune->velocity = rune->velocity + player->velocity;
        }

        // Set last drop pointer and time, that avoids picking it up repeatedly by same player
        rune->enemy = player;
        rune->pain_finished = time;

        // Flag this rune as a droped one, which will make the think sub to check for valid position
        rune->tfstate = PR_RUNESTATE_DROPED;
    }
    else
        RPrint("BUG: Unable to spawn rune entity on PlayerDropsRune()!\n");

    // Report this rune drop
    ReportRuneDrop(player,runetype);

    // End rune effects on player if needed
    RuneEndEffects(player,runetype);
}

//================================================================================
// Called whenever a player is unable to keep his runes (dies, quits game etc..)

void PlayerDropRunes(entity player)
{
	if (player->PR_runes == 0) return; // no runes to drop
	if (neo.isModeActive() && player == neo.getNeo()) return;   // PZ: Don't drop runes during Neo mode.

	// Perform "drop" of any runes if needed
	if (player->PR_runes & PR_RUNE_TRIAD)
		PlayerDropsRune(player,PR_RUNE_TRIAD,PR_FALSE);

	if (player->PR_runes & PR_RUNE_RESIS)
		PlayerDropsRune(player,PR_RUNE_RESIS,PR_FALSE);

	if (player->PR_runes & PR_RUNE_SPEED)
		PlayerDropsRune(player,PR_RUNE_SPEED,PR_FALSE);

	if (player->PR_runes & PR_RUNE_REGEN)
		PlayerDropsRune(player,PR_RUNE_REGEN,PR_FALSE);

	// Reset our rune indicator to 0 for safety
	player->PR_runes = 0;
}

//==============================================================================
// Gets number of runes a player is carrying

float GetPlayerNoRunes(entity player)
{
    float numrunes;
    numrunes = 0;

    if (player->PR_runes & PR_RUNE_TRIAD)
        numrunes = numrunes + 1;
    if (player->PR_runes & PR_RUNE_RESIS)
        numrunes = numrunes + 1;
    if (player->PR_runes & PR_RUNE_SPEED)
        numrunes = numrunes + 1;
    if (player->PR_runes & PR_RUNE_REGEN)
        numrunes = numrunes + 1;

    return numrunes;
}

//====================================================================
// Gets the string describing a rune

string GetRuneString(float runetype)
{
    if (runetype == PR_RUNE_TRIAD)
        return S_("^bTriad Rune^b");
    if (runetype == PR_RUNE_RESIS)
        return S_("^bResist Rune^b");
    if (runetype == PR_RUNE_SPEED)
        return S_("^bSpeed Rune^b");
    if (runetype == PR_RUNE_REGEN)
        return S_("^bRegen Rune^b");

    return "ERROR!";
}

//==================================================================
// Prints any messages needed when a rune is taken

void ReportRuneTaken(entity player, float runetype)
{
    string st, st2;
    float rnum;
    rnum = random();
    st = GetRuneString(runetype);

    if (rnum < 0.2)
        st2 = " got";
    else if (rnum < 0.4)
        st2 = " has";
    else if (rnum < 0.6)
        st2 = " took";
    else if (rnum < 0.8)
        st2 = " gets";
    else
        st2 = " takes";

    bprint(PR_PRINT_MEDIUM,player->netname,st2," the ",st,"!\n");

    sprint(player,PR_PRINT_HIGH,"The ",st," is now yours!\n");
}

//=================================================================
// Prints any messages needed when a rune is droped

void ReportRuneDrop(entity player, float runetype)
{
    string st;
    st = GetRuneString(runetype);
    bprint(PR_PRINT_MEDIUM,player->netname," dropped the ",st,"\n");

    sprint(player,PR_PRINT_HIGH,"You drop the ",st,".\n");
}

//=================================================================
// Does visual/sound effects and removes the rune

void RuneDisappear(entity rune)
{
    spawnFOG(rune->origin);
    SpawnSprite(1,PR_SPRITE_ABLAST,rune->origin,V({0, 0, 0}),PR_SPRITEMOVE_UPSLOW,0.1);
    sound(rune,PR_CHAN_BODY,"items/itembk2.wav",1,PR_ATTN_NORM);
    dremove(rune);
}

//=================================================================
// Does visual/sound effects stuff when a rune appears

void RuneAppear(entity rune)
{
    spawnFOG(rune->origin);
    MuzzleFlash(rune);
    sound(rune,PR_CHAN_BODY,"items/itembk2.wav",1,PR_ATTN_NORM);
}

//================================================================================
// Drops the specified rune number (1-4, not bitfield) from a player if possible

float DropSpecificRune(entity player, float runenum)
{
    runenum = floor(runenum);

    if (runenum < 1 || runenum > 4)
        return PR_FALSE;

    if (runenum == 1)
    {
        if (player->PR_runes & PR_RUNE_TRIAD)
        {
            PlayerDropsRune(player,PR_RUNE_TRIAD,PR_TRUE);
            return PR_TRUE;
        }

        return PR_FALSE;
    }
    else if (runenum == 2)
    {
        if (player->PR_runes & PR_RUNE_RESIS)
        {
            PlayerDropsRune(player,PR_RUNE_RESIS,PR_TRUE);
            return PR_TRUE;
        }

        return PR_FALSE;
    }
    else if (runenum == 3)
    {
        if (player->PR_runes & PR_RUNE_SPEED)
        {
            PlayerDropsRune(player,PR_RUNE_SPEED,PR_TRUE);
            return PR_TRUE;
        }

        return PR_FALSE;
    }
    else if (runenum == 4)
    {
        if (player->PR_runes & PR_RUNE_REGEN)
        {
            PlayerDropsRune(player,PR_RUNE_REGEN,PR_TRUE);
            return PR_TRUE;
        }

        return PR_FALSE;
    }

    return PR_FALSE;
}

//=============================================================
// Used to start any stuff needed by a rune/s on player

void RuneStartEffects(entity player, int runetype)
{
    // NOTE: For resist and triad runes, no action is needed

    if (runetype & PR_RUNE_SPEED)
    {
        // Update player speed
        TeamFortress_SetSpeed(player);
    }

    if (runetype & PR_RUNE_REGEN)
    {
        // Spawn regen timer
        SpawnRegenRuneTimer(player);
    }
}

//=============================================================
// Cleans up the effect of a rune/s on a player

void RuneEndEffects(entity player, int runetype)
{
    // NOTE: For resist and triad runes, no action is needed

    if (runetype & PR_RUNE_SPEED)
    {
        // Update player speed
        TeamFortress_SetSpeed(player);
    }

    if (runetype & PR_RUNE_REGEN)
    {
        // Remove regen timer
        RemoveRegenRuneTimer(player);
    }
}

//===============================================================
// Regen Rune timer main think

void RegenRuneTimer_think()
{
	if (!self->owner->is_connected || self->owner->classname != "player" || !(self->owner->PR_runes & PR_RUNE_REGEN))
	{
		dremove(self);
		return;
	}

	if (self->owner->is_abouttodie || self->owner->health <= 0)
	{
		dremove(self);
		return;
	}

	// Begin Neo mod...
	// calculate regen multiplier for Neo mode scaling
	float neoFactor = 1;
	if (neo.isModeActive()) neoFactor = neo.getNeoRegenFactor();

	// Regenerate health
	if (self->owner->health < self->owner->max_health) // Check
	{
		if (self->owner->health + (PR_REGENRUNE_HP_AMMOUNT * neoFactor) > self->owner->max_health)
			self->owner->health = self->owner->max_health;
		else
			self->owner->health = self->owner->health + (PR_REGENRUNE_HP_AMMOUNT * neoFactor);
	}

	// Regenerate armor, but only if any left
	// PZ: I don't understand the point of this. I'm making it always regen armor in Neo mode.
	if (self->owner->armorvalue > 0 || neo.isModeActive())
	{
		if (self->owner->armorvalue + (PR_REGENRUNE_ARMOR_AMMOUNT * neoFactor) > self->owner->maxarmor)
			self->owner->armorvalue = self->owner->maxarmor;
		else
			self->owner->armorvalue = self->owner->armorvalue + (PR_REGENRUNE_ARMOR_AMMOUNT * neoFactor);
	}

	// Regenerate all the ammo
	if (self->owner->ammo_shells + (PR_REGENRUNE_AMMO_AMMOUNT * neoFactor) > self->owner->maxammo_shells)
		self->owner->ammo_shells = self->owner->maxammo_shells;
	else
		self->owner->ammo_shells = self->owner->ammo_shells + (PR_REGENRUNE_AMMO_AMMOUNT * neoFactor);

	if (self->owner->ammo_nails + (PR_REGENRUNE_AMMO_AMMOUNT * neoFactor) > self->owner->maxammo_nails)
		self->owner->ammo_nails = self->owner->maxammo_nails;
	else
		self->owner->ammo_nails = self->owner->ammo_nails + (PR_REGENRUNE_AMMO_AMMOUNT * neoFactor);

	if (self->owner->ammo_rockets + (PR_REGENRUNE_AMMO_AMMOUNT * neoFactor) > self->owner->maxammo_rockets)
		self->owner->ammo_rockets = self->owner->maxammo_rockets;
	else
		self->owner->ammo_rockets = self->owner->ammo_rockets + (PR_REGENRUNE_AMMO_AMMOUNT * neoFactor);

	if (self->owner->ammo_cells + (PR_REGENRUNE_AMMO_AMMOUNT * neoFactor) > self->owner->maxammo_cells)
		self->owner->ammo_cells = self->owner->maxammo_cells;
	else
		self->owner->ammo_cells = self->owner->ammo_cells + (PR_REGENRUNE_AMMO_AMMOUNT * neoFactor);

	// Neo Mode:
	// Neo can't pick up resupply packs. So, we're going to allow him to regen grenades and detpacks.
	if (neo.isModeActive())
	{
		float maxAmmo, increase;

		// Grenades
		if (neo.getLastNadeRegen() + (PR_REGENRUNE_NADE_RATE / neoFactor) <= time)
		{
			neo.setLastNadeRegen(time);
			if (self->owner->tp_grenades_1 != PR_GR_TYPE_NONE)
			{
				maxAmmo = GetMaxGrens(self->owner, 1);
				if (self->owner->no_grenades_1 + (PR_REGENRUNE_AMMO_AMMOUNT * neoFactor) > maxAmmo)
					self->owner->no_grenades_1 = maxAmmo;
				else
				{
					increase = PR_REGENRUNE_AMMO_AMMOUNT * neoFactor;
					if (increase < 1) increase = 1;
					else increase = floor(increase);
					self->owner->no_grenades_1 += increase;
				}
			}
			if (self->owner->tp_grenades_2 != PR_GR_TYPE_NONE)
			{
				maxAmmo = GetMaxGrens(self->owner, 2);
				if (self->owner->no_grenades_2 + (PR_REGENRUNE_AMMO_AMMOUNT * neoFactor) > maxAmmo)
					self->owner->no_grenades_2 = maxAmmo;
				else
				{
					increase = PR_REGENRUNE_AMMO_AMMOUNT * neoFactor;
					if (increase < 1) increase = 1;
					else increase = floor(increase);
					self->owner->no_grenades_2 += increase;
				}
			}
		}
		// Detpacks
		if (neo.getLastDetRegen() + (PR_REGENRUNE_DET_RATE / neoFactor) <= time)
		{
			neo.setLastDetRegen(time);
			if (self->owner->cutf_items & PR_CUTF_DETPACK)
			{
				maxAmmo = self->owner->maxammo_detpack;
				if (self->owner->ammo_detpack + (PR_REGENRUNE_AMMO_AMMOUNT * neoFactor) > maxAmmo)
					self->owner->ammo_detpack = maxAmmo;
				else
				{
					increase = PR_REGENRUNE_AMMO_AMMOUNT * neoFactor;
					if (increase < 1) increase = 1;
					else increase = floor(increase);
					self->owner->ammo_detpack += increase;
				}
			}
			if (self->owner->cutf_items & PR_CUTF_TOSSABLEDET)
			{
				maxAmmo = self->owner->maxammo_detpack;
				if (self->owner->ammo_c4det + (PR_REGENRUNE_AMMO_AMMOUNT * neoFactor) > maxAmmo)
					self->owner->ammo_c4det = maxAmmo;
				else
				{
					increase = PR_REGENRUNE_AMMO_AMMOUNT * neoFactor;
					if (increase < 1) increase = 1;
					else increase = floor(increase);
					self->owner->ammo_c4det += increase;
				}
			}
		}
	}
	// Update current ammo if needed
	if (self->owner->items & PR_IT_SHELLS)
		self->owner->currentammo = self->owner->ammo_shells;
	else if (self->owner->items & PR_IT_NAILS)
		self->owner->currentammo = self->owner->ammo_nails;
	else if (self->owner->items & PR_IT_ROCKETS)
		self->owner->currentammo = self->owner->ammo_rockets;
	else if (self->owner->items & PR_IT_CELLS)
		self->owner->currentammo = self->owner->ammo_cells;
	else if (self->owner->current_weapon == PR_WEAP_SPANNER)
		self->owner->currentammo = self->owner->ammo_cells;

	// Cya soon!
	self->nextthink = time + (PR_REGENRUNE_RATE / neoFactor);
	// ...End Neo mod
}

//==============================================================
// Clean up any regen rune timer for player

void RemoveRegenRuneTimer(entity player)
{
    // Find any regen rune timers for this player..
    entity te;

    te = find(world,"classname","regenrune_timer");

    while (te != world)
    {
        // .. and remove them if ours
        if (te->owner == player)
            dremove(te);

        te = find(te,"classname","regenrune_timer");
    }
}

//===============================================================
// Spawns a new regen rune timer for a player

void SpawnRegenRuneTimer(entity player)
{
    entity regenrune_timer;

    regenrune_timer = spawnServerSide(); // PZ: make it a server-side only entity
    regenrune_timer->owner = player;
    regenrune_timer->classname = "regenrune_timer";
    regenrune_timer->think = RegenRuneTimer_think;
    regenrune_timer->nextthink = time + PR_REGENRUNE_RATE;
}

//==========================================================================================
// Respawns a rune elsewhere, doing nice effects for the rune that disappears if specified

void RespawnRune(entity rune, float nice)
{
    // Create a new rune of same kind elsewhere
    SpawnNewRune(rune->has_sentry);

    // Should we do any effect for the disappearing rune?
    if (nice)
        RuneDisappear(rune); // Disappear nicely and remove entity
    else
        dremove(rune); // Just remove the rune entity
}

} // END namespace Progs
