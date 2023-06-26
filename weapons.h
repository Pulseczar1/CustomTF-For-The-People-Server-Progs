#ifndef WEAPONS_H
#define WEAPONS_H

namespace Progs {

void SniperSight_Update2();
void I_DID_CHEAT_ONE();
void I_DID_CHEAT_TWO();
void I_DID_CHEAT_THREE();
void ThrowGib(const string& gibname, float dm, float makecool, float gibskin, float gibkgs, float randorg);
void T_Damage(entity targ, entity inflictor, entity attacker, float damage);
void TF_T_Damage(entity targ, entity inflictor, entity attacker, float damage, int T_flags, float T_AttackType);
void player_run();
void T_RadiusDamage(entity bomb, entity attacker, float rad, entity ignore);
void TeamFortress_DisplayDetectionItems();
float crossproduct(const vector& veca, const vector& vecb);
void SpawnBlood(const vector& org, float damage);
void Reset_Grapple(entity rhook);
void SuperDamageSound();
float W_BestWeapon();
void ConcussionGrenadeTimer();
void W_PrintWeaponMessage();
void button_touch();
void button_fire();
void MauserRecoilThink();
float ReturnWeaponVelocity();
vector Grunty_LeadShot();
void TeamFortress_ChangeClass();
void TeamFortress_DisplayLegalClasses();
void TeamFortress_Inventory();
void TeamFortress_SaveMe();
void TeamFortress_ID(float inAuto);
void TeamFortress_ShowTF();
void TeamFortress_SniperWeapon();
void TeamFortress_AssaultWeapon();
void TeamFortress_IncendiaryCannon();
void TeamFortress_FlameThrower();
void TeamFortress_PrimeGrenade();
void TeamFortress_ThrowGrenade();
void TeamFortress_Discard();
void TeamFortress_SetSpeed(entity p);
void TeamFortress_DetonatePipebombs();
void PipebombTouch();
void TeamFortress_DetpackStop(float foo);
void SniperSight_Create(float type);
void TF_zoom(float zoom_level);
void TeamFortress_ReloadCurrentWeapon();
void TeamFortress_AutoZoomToggle();
void TeamFortress_StatusQuery();
void TeamFortress_SpyGoUndercover();
void TeamFortress_SpyFeignDeath(float type);
void TeamFortress_EngineerBuild();
void DropKey();
void UseSpecialSkill();
void UseJobSkill();
void GuerillaMineSweep(const vector& startpos);
void makeImmune(entity foo,float bar);
float CheckForReload();
void SBBuildSensor();
void SBFireInterface();
void W_FireMauser();
void W_FireDaedalus();
void TeamFortress_Scan(float scanrange,float inAuto);
void TeamFortress_SetDetpack(float timer);
float TeamFortress_TeamSet(float tno, float performChecks);
void TeamFortress_TeamShowScores(float all, float teamscored, float scorepoints);
void TeamFortress_TeamShowMemberClasses(entity Player);
void CamLock();
void CamDistLock();
void CamVecLock();
void CamAngleLock();
void CamRevAngleLock();
void CamProjectileLock();
void CamProjectileZoom();
void CamProjectileLockOn();
void CamProjectileLockOff();
void CamOffset();
void CamDrop();
void fadetoblack();
void fadefromblack();
void fadetowhite();
void fadefromwhite();
void Engineer_UseDispenser(entity disp);
void Engineer_UseSensor(entity cam);
void Engineer_UseSentryGun(entity gun);
void Engineer_UseTesla(entity gun);
void Engineer_UseCamera(entity cam);
void Engineer_UseTeleporter(entity tele);
void Engineer_UseFieldGen(entity field);
void Spy_RemoveDisguise(entity spy);
void TeamFortress_MOTD();
void TeamFortress_HelpMap();
void StatusRes(float res);
void BioInfection_Decay();
void BioInfection_MonsterDecay();
void W_FireFlame();
void W_FireIncendiaryCannon();
void W_FireTranq();
void W_FireLaser();
void HallucinationTimer();
void TranquiliserTimer();
void TeamFortress_CTF_FlagInfo();
void TF_MovePlayer();
void custom_demon_precache();
void custom_lay();
float TeamFortress_DropItems();
void ActivateHolo(entity player);
void MakeMeDebug(entity who);
void grunty_spike(const vector& org, const vector& dir);
void launch_horn();
void player_laser1();
void FieldEvent(entity tfield, const vector& where, entity thing);
void Player_Punish();
void Player_VoteMap();
void Player_VoteYes();
void Player_VoteNo();
float GoodIntermissionImpulse(float imp);
string GetMonsterName(entity themonster);
void teamsprint6(entity ignore, const string& st, const string& st2, const string& st3, const string& st4, const string& st5, const string& st6);
void SpawnDmgFX(entity targetent, const vector& where, float spiketype, float bloodtype, float bloodammount);
void BulletTraceline(const vector& src, const vector& endpos);
void W_Precache();
float crandom();
void Attack_Finished(float att_delay);
void W_FireAxe();
void W_FireSpanner();
void teamsprintbi(entity ignore, entity ignore2, const string& st, const string& st2, const string& st3, const string& st4, const string& st5, const string& st6);
void teamprefixsprintbi(float tno, entity ignore, entity ignore2);
float MedicRevive( entity medic, entity deadPlayer, int reviveFlags);
void W_FireMedikit(float inAuto);
void W_FireBioweapon();
vector wall_velocity();
void SpawnMeatSpray(const vector& org, const vector& vel);
void SpawnBlood(const vector& org, float damage);
void spawn_touchblood(float damage);
void SpawnChunk(const vector& org, const vector& vel);
void ClearMultiDamage();
void ApplyMultiDamage();
void AddMultiDamage(entity hit, float damage);
void Multi_Finish();
void CoolPush(const vector& org, const vector& dir,float intensity,float radius);
void TraceAttack(float damage, const vector& dir);
void BulletTraceline(const vector& src, const vector& endpos);
void FireBullets(float shotcount, const vector& dir, const vector& spread, float dist);
void W_FireShotgun();
void W_FireSuperShotgun();
void FireSniperBullet(const vector& direction, float damage);
void W_FireSniperRifle();
void W_FireAutoRifle();
void W_FireAssaultCannon();
void W_FireLightAssault();
void BecomeExplosion();
void T_MissileTouch();
void Rocket_Track_Dot();
float RocketSpeedMult();
void W_FireRocket();
void LightningHit(entity from, float damage);
void LightningDamage(const vector& p1, const vector& p2, entity from, float damage);
void W_FireLightning();
float num_team_pipebombs(float tno);
void ExplodeOldPipebomb(float tno);
void increment_team_pipebombs(float tno);
void decrement_team_pipebombs(float tno);
void GrenadeExplode();
void GrenadeTouch();
void W_FireGrenade();
void spike_touch();
void launch_spike(const vector& org, const vector& dir);
void launch_directspike(const vector& org, const vector& dir);
void superspike_touch();
void W_FireSuperSpikes();
void W_FireSpikes(float ox);
void spike_touch();
void W_SetCurrentAmmo();
float W_BestWeapon();
float W_CheckNoAmmo();
void W_Reload_shotgun();
void W_Reload_light_assault();
void W_Reload_super_shotgun();
void W_Reload_grenade_launcher();
void W_Reload_rocket_launcher();
void W_Reload_laser_cannon();
float CheckForReload();
void player_axe1();
void player_axeb1();
void player_axec1();
void player_axed1();
void player_shot1();
void player_nail1();
void player_snail1();
void player_light1();
void player_light_assault1();
void player_rocket1();
void player_autorifle1();
void player_assaultcannon1();
void player_assaultcannonup1();
void player_assaultcannondown1();
void player_medikit1();
void player_medikitb1();
void player_medikitc1();
void player_medikitd1();
void player_bioweapon1();
void player_bioweaponb1();
void player_bioweaponc1();
void player_bioweapond1();
void player_chain1();
void player_chain2();
void player_chain3();
void player_chain4();
void player_chain5();
void W_Attack();
void W_PrintWeaponMessage();
void W_ChangeWeapon();
void CycleWeaponCommand();
void ChangeToWeapon( const string& weaponName );
void QuadCheat();
void DeadImpulses();
float BuildingImpulses(float inp);
float NoCooldownImpulseCommands();
void ImpulseCommands();
void DeadImpulses();
void W_WeaponFrame();
void T_DaedalusTouch();
void W_FireDaedalus();
void W_FireMauser();
void MauserRecoilThink();
void SuperDamageSound();
void HIP_LaserTouch();
void HIP_LaserThink();
void HIP_LaunchLaser(const vector& org, vector vec, float light);
void HIP_FireLaser(float stat);
float W_HasWeapon( entity e, float weaponNum);
void W_AddWeapon( entity e, float weaponNum);
void W_RemoveWeapon( entity e, float weaponNum);
void W_SetCurrentWeapon( entity e, float weaponNum);
void W_Sprint( entity e, const string& msg);
void W_KickPlayer( float psize, entity p);
void W_MakeEntityVectors( entity e);
vector W_Aim( entity e, float missilespeed);
float Weapon_Shotgun( entity e, float func);
void s_explode1();
void s_explode2();
void s_explode3();
void s_explode4();
void s_explode5();
void s_explode6();

} // END namespace Progs

#endif // END WEAPONS_H

