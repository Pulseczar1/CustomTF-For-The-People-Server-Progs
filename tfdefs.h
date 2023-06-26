#ifndef TFDEFS_H
#define TFDEFS_H

namespace Progs {

//
// player only fields
//
#define PR_team_damage_dealt       cnt            // total amount of team damage this player has dealt
#define PR_enemy_damage_dealt      volume         // total amount of damage done to enemies only
#define PR_damage_taken            distance       // player that took the most amount of damage from enemies
#define PR_teleporter_teamkiller   trigger_field  // a teammate that teleported us into lava (requires teleporter.qc, client.qc: PlayerPostThink(), and obits.qc)

//
// grunty.qc
//
#define PR_allowed_weapons         done_custom    // weapons the grunt can use  (PZ: this was set to `wait`; needed to be an int)

//
// MedicRevive() flags
//
#define PR_REVIVE_PRINTMSGS        1              // display messages to the medic and dead player
#define PR_REVIVE_MEDIKITAMMO      2              // medikit ammo required

// Functions
float BitNumToValue( float bitNum);
float qc_droptofloor(entity e, float dist, float nodrop);
#ifdef PR_COOP_MODE_ENHANCED
void RemoveActiveSound();
#endif

} // END namespace Progs

#endif // END TFDEFS_H

