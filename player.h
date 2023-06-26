#ifndef PLAYER_H
#define PLAYER_H

namespace Progs {

namespace PlayerFrames
{
	//
	// running
	//
	enum {FR_AXRUN1, FR_AXRUN2, FR_AXRUN3, FR_AXRUN4, FR_AXRUN5, FR_AXRUN6};

	enum {FR_ROCKRUN1 = 6, FR_ROCKRUN2, FR_ROCKRUN3, FR_ROCKRUN4, FR_ROCKRUN5, FR_ROCKRUN6};

	//
	// standing
	//
	enum {FR_STAND1 = 12, FR_STAND2, FR_STAND3, FR_STAND4, FR_STAND5};

	enum {FR_AXSTND1 = 17, FR_AXSTND2, FR_AXSTND3, FR_AXSTND4, FR_AXSTND5, FR_AXSTND6};
	enum {FR_AXSTND7 = 23, FR_AXSTND8, FR_AXSTND9, FR_AXSTND10, FR_AXSTND11, FR_AXSTND12};

	//
	// pain
	//
	enum {FR_AXPAIN1 = 29, FR_AXPAIN2, FR_AXPAIN3, FR_AXPAIN4, FR_AXPAIN5, FR_AXPAIN6};

	enum {FR_PAIN1 = 35, FR_PAIN2, FR_PAIN3, FR_PAIN4, FR_PAIN5, FR_PAIN6};

	//
	// death
	//
	enum {FR_AXDETH1 = 41, FR_AXDETH2, FR_AXDETH3, FR_AXDETH4, FR_AXDETH5, FR_AXDETH6};
	enum {FR_AXDETH7 = 47, FR_AXDETH8, FR_AXDETH9};

	enum {FR_DEATHA1 = 50, FR_DEATHA2, FR_DEATHA3, FR_DEATHA4, FR_DEATHA5, FR_DEATHA6, FR_DEATHA7, FR_DEATHA8};
	enum {FR_DEATHA9 = 58, FR_DEATHA10, FR_DEATHA11};

	enum {FR_DEATHB1 = 61, FR_DEATHB2, FR_DEATHB3, FR_DEATHB4, FR_DEATHB5, FR_DEATHB6, FR_DEATHB7, FR_DEATHB8};
	enum {FR_DEATHB9 = 69};

	enum {FR_DEATHC1 = 70, FR_DEATHC2, FR_DEATHC3, FR_DEATHC4, FR_DEATHC5, FR_DEATHC6, FR_DEATHC7, FR_DEATHC8};
	enum {FR_DEATHC9 = 78, FR_DEATHC10, FR_DEATHC11, FR_DEATHC12, FR_DEATHC13, FR_DEATHC14, FR_DEATHC15};

	enum {FR_DEATHD1 = 85, FR_DEATHD2, FR_DEATHD3, FR_DEATHD4, FR_DEATHD5, FR_DEATHD6, FR_DEATHD7};
	enum {FR_DEATHD8 = 92, FR_DEATHD9};

	enum {FR_DEATHE1 = 94, FR_DEATHE2, FR_DEATHE3, FR_DEATHE4, FR_DEATHE5, FR_DEATHE6, FR_DEATHE7};
	enum {FR_DEATHE8 = 101, FR_DEATHE9};

	//
	// attacks
	//
	enum {FR_NAILATT1 = 103, FR_NAILATT2};

	enum {FR_LIGHT1 = 105, FR_LIGHT2};

	enum {FR_ROCKATT1 = 107, FR_ROCKATT2, FR_ROCKATT3, FR_ROCKATT4, FR_ROCKATT5, FR_ROCKATT6};

	enum {FR_SHOTATT1 = 113, FR_SHOTATT2, FR_SHOTATT3, FR_SHOTATT4, FR_SHOTATT5, FR_SHOTATT6};

	enum {FR_AXATT1 = 119, FR_AXATT2, FR_AXATT3, FR_AXATT4, FR_AXATT5, FR_AXATT6};

	enum {FR_AXATTB1 = 125, FR_AXATTB2, FR_AXATTB3, FR_AXATTB4, FR_AXATTB5, FR_AXATTB6};

	enum {FR_AXATTC1 = 131, FR_AXATTC2, FR_AXATTC3, FR_AXATTC4, FR_AXATTC5, FR_AXATTC6};

	enum {FR_AXATTD1 = 137, FR_AXATTD2, FR_AXATTD3, FR_AXATTD4, FR_AXATTD5, FR_AXATTD6};
}

// Functions
void bubble_bob();
void W_FireAssaultCannon();
void W_FireLightAssault();
void Throw_Grapple();
void BioInfection_Decay();
void TeamFortress_RemoveTimers();
void T_Dispenser();
void MakeCorpse(entity deadone, float corpse_tolerance);
void SpawnBloodEx(const vector& where, float bloodtype, float bloodammount);
void player_touch();
void player_run();
void player_stand1();
void player_run();
void player_shot1();
void player_shot2();
void player_shot3();
void player_shot4();
void player_shot5();
void player_shot6();
void player_autorifle1();
void player_autorifle2();
void player_autorifle3();
void player_axe1();
void player_axe2();
void player_axe3();
void player_axe4();
void player_axeb1();
void player_axeb2();
void player_axeb3();
void player_axeb4();
void player_axec1();
void player_axec2();
void player_axec3();
void player_axec4();
void player_axed1();
void player_axed2();
void player_axed3();
void player_axed4();
void player_chain1();
void player_chain1a();
void player_chain2();
void player_chain2a();
void player_chain3();
void player_chain4();
void player_chain5();
void player_medikit1();
void player_medikit2();
void player_medikit3();
void player_medikit4();
void player_medikitb1();
void player_medikitb2();
void player_medikitb3();
void player_medikitb4();
void player_medikitc1();
void player_medikitc2();
void player_medikitc3();
void player_medikitc4();
void player_medikitd1();
void player_medikitd2();
void player_medikitd3();
void player_medikitd4();
void player_bioweapon1();
void player_bioweapon2();
void player_bioweapon3();
void player_bioweapon4();
void player_bioweaponb1();
void player_bioweaponb2();
void player_bioweaponb3();
void player_bioweaponb4();
void player_bioweaponc1();
void player_bioweaponc2();
void player_bioweaponc3();
void player_bioweaponc4();
void player_bioweapond1();
void player_bioweapond2();
void player_bioweapond3();
void player_bioweapond4();
void player_snail1();
void player_snail2();
void player_nail1();
void player_nail2();
void player_nail3();
void player_nail4();
void player_assaultcannonup1();
void player_assaultcannonup2();
void player_assaultcannon1();
void player_assaultcannon2();
void player_assaultcannondown1();
void player_light1();
void player_light2();
void player_light_assault1();
void player_light_assault2();
void player_rocket1();
void player_rocket2();
void player_rocket3();
void player_rocket4();
void player_rocket5();
void player_rocket6();
void DeathBubbles(float num_bubbles);
void PainSound();
void player_pain1();
void player_pain2();
void player_pain3();
void player_pain4();
void player_pain5();
void player_pain6();
void player_axpain1();
void player_axpain2();
void player_axpain3();
void player_axpain4();
void player_axpain5();
void player_axpain6();
void player_pain(entity attacker, float damage);
void player_diea1();
void player_dieb1();
void player_diec1();
void player_died1();
void player_diee1();
void player_die_ax1();
void DeathBubblesSpawn();
void DeathBubbles(float num_bubbles);
void NormalBubblesSpawn();
void SpawnBubbles(float num_bubbles, const vector& where);
void DeathSound();
void PlayerCorpse();
vector VelocityForDamage(float dm);
void GibPickSound(entity warlock);
float GetMeat(entity player);
void SetMeat(entity player, float numgibs);
void gib_touch();
void ThrowGib(const string& gibname, float dm, float makecool, float gibskin, float gibkgs, float randorg);
float GetHeads(entity player);
void SetHeads(entity player, float numheads);
void head_touch();
void ThrowHead(const string& gibname, float dm);
void ThrowHeadOut(const string& gibname, float dm);
void GibPlayerOut();
void KillPlayer();
void GibPlayer();
void PlayerDead();
void PlayerDie();
void set_suicide_frame();
void set_suicide_frame_disc();
void player_diea1();
void player_diea2();
void player_diea3();
void player_diea4();
void player_diea5();
void player_diea6();
void player_diea7();
void player_diea8();
void player_diea9();
void player_diea10();
void player_diea11();
void player_dieb1();
void player_dieb2();
void player_dieb3();
void player_dieb4();
void player_dieb5();
void player_dieb6();
void player_dieb7();
void player_dieb8();
void player_dieb9();
void player_diec1();
void player_diec2();
void player_diec3();
void player_diec4();
void player_diec5();
void player_diec6();
void player_diec7();
void player_diec8();
void player_diec9();
void player_diec10();
void player_diec11();
void player_diec12();
void player_diec13();
void player_diec14();
void player_diec15();
void player_died1();
void player_died2();
void player_died3();
void player_died4();
void player_died5();
void player_died6();
void player_died7();
void player_died8();
void player_died9();
void player_diee1();
void player_diee2();
void player_diee3();
void player_diee4();
void player_diee5();
void player_diee6();
void player_diee7();
void player_diee8();
void player_diee9();
void player_die_ax1();
void player_die_ax2();
void player_die_ax3();
void player_die_ax4();
void player_die_ax5();
void player_die_ax6();
void player_die_ax7();
void player_die_ax8();
void player_die_ax9();
void player_airgun1();
void player_airgun2();
void player_airgun3();
void player_airgun4();
void player_airgun5();
void player_airgun6();
void player_airgun7();
void player_airgun8();
void player_failedairgun1();
void player_failedairgun2();
void player_failedairgun3();
void player_laser1();
void player_laser2();
void player_laser3();
void player_laser4();

} // END namespace Progs

#endif // END PLAYER_H
