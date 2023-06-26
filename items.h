#ifndef ITEMS_H
#define ITEMS_H

namespace Progs {

float IsSingleplayerItem();
void SUB_regen();
void noclass();
void PlaceItem();
void StartItem();
float T_Heal(entity e, float healamount, float ignore);
void item_health();
void health_touch();
void item_megahealth_rot();
void armor_touch();
void item_armor1();
void item_armor2();
void item_armorInv();
void bound_other_ammo(entity p);
void Deathmatch_Weapon(float old, float newFlt);
void Respawn_Item(entity ritem, entity act);
void weapon_touch();
void weapon_supershotgun();
void weapon_nailgun();
void weapon_supernailgun();
void weapon_grenadelauncher();
void weapon_rocketlauncher();
void weapon_lightning();
void PrintGrenadeType(entity pl, float typ);
void ammo_touch();
void item_shells();
void item_spikes();
void item_rockets();
void item_cells();
void item_weapon();
void key_touch();
void key_setsounds();
void item_key1();
void item_key2();
void sigil_touch();
void item_sigil();
void powerup_touch();
void item_artifact_invulnerability();
void item_artifact_envirosuit();
void item_artifact_invisibility();
void item_artifact_super_damage();
void BackpackTouch();
void DropBackpack();

} // END namespace Progs

#endif // END ITEMS_H

