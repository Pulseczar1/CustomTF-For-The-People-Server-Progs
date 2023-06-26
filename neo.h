#pragma once

namespace Progs {

class Neo
{
  private:
	// Variables for 'localinfo' variables
	bool   modeActive;                     // just on or off (requires 'localinfo neo 1' to be on)
	float  neo_base_dmg;                   // Neo's damage factor when there are 0 people in the game
	float  neo_dmg_factor;                 // how much damage factor increases with each player past playercount of 0
	float  neo_base_res;                   // Neo's resistance factor when there are 0 people in the game
	float  neo_res_factor;                 // how much resistance factor increases with each player past playercount of 0
	float  neo_base_regen;                 // Neo's regeneration factor when there are 0 people in the game
	float  neo_regen_factor;               // how much regeneration factor increases with each player past playercount of 0

	// Normal Variables
	entity neoExists;                      // Is there currently a Neo? If so, who?
	float  oldTeamplayValue;               // used in teamplay mode changing
	float  lastNadeRegen;                  // last time Neo had a grenade regenerated
	float  lastDetRegen;                   // last time Neo had a detpack (tossable or not) regenerated
	float  neoDamageFactor;                // final damage factor (sorry about name redundancy)
	float  neoResistFactor;                // final resist factor
	float  neoRegenFactor;                 // final regen factor
	float  timeLastLightningDisplayed;     // Timer for lightning direction indicator.
	bool   neoCanChangeClass;              // Neo can use commands to initiate class change and then use 'kill' to make the change.
	int    neoFragsAtLastClassChange;      // Neo can change class every `NEO_KILLS_FOR_CLASS_CHANGE` kills.
	float  neoTimeAtLastClassChange;       // Neo can change class every `NEO_MINUTES_FOR_CLASS_CHANGE` kills.

	// Constants
	// TODO: Make this so that Neo has to gib the player, or knock them a long distance, to say "Whoa..".
	const float WHOA_LIKELINESS = 0.25;    // how likely Neo is to say 'Whoa..'; set between 0 and 1
	                                       // not a 'true' percentage; look at how it's used

	const string CUTF_URL          = "https://discord.gg/fp9sSZ5";   // Taco Bell's old CuTF forum: "customtf.net/forum/"
	const string NEO_ERROR_MESSAGE = "Neo Mode error detected!\nPlease report bugs to " + CUTF_URL + ".\n";

	const int    NEO_KILLS_FOR_CLASS_CHANGE   = 50;
	const float  NEO_MINUTES_FOR_CLASS_CHANGE = 10;

  public:
	// Accessors and Mutators
	bool   isModeActive() { return modeActive; }
	entity getNeo() { return neoExists; }
	float  getLastNadeRegen() { return lastNadeRegen; }
	void   setLastNadeRegen(float x) { lastNadeRegen = x; }
	float  getLastDetRegen() { return lastDetRegen; }
	void   setLastDetRegen(float x) { lastDetRegen = x; }
	float  getNeoDamageFactor() { return neoDamageFactor; }
	float  getNeoResistFactor() { return neoResistFactor; }
	float  getNeoRegenFactor()  { return neoRegenFactor; }
	bool   getNeoCanChangeClass() {return neoCanChangeClass; }

	void   initialize();
	void   mainThink();
	void   deathThink(entity targ, entity attacker);
	void   introducePlayer();
	void   removeNeo(entity player);
	float  teammate(entity a, entity b);
	void   invPrint();
	void   centerPrint();

  private:
	void   calcNeoFactors();
	void   assignNeo(entity attacker);
	void   possessionsChangeTeam(entity player, float teamNumber);
	void   errorCheck();
};

extern Neo neo;

} // END namespace Progs
