// Spectator functions
// Added Aug11'97 by Zoid <zoid@idsoftware.com>
//
// These functions are called from the server if they exist.
// Note that Spectators only have one think since they movement code doesn't
// track them much.  Impulse commands work as usual, but don't call
// the regular ImpulseCommand handler in weapons.qc since Spectators don't
// have any weapons and things can explode.
//
//	 --- Zoid.

#include "progs.h"
#include "spectate.h"
#include "ofndefs.h"
#include "debug.h"
#include "menu.h"
#include "cpstuff.h"
#include "tfort.h"
#include "environ.h"
#include "admin.h"
#include "vote.h"
#include "vote2.h"
#include "frikbot/bot_qw.h"

namespace Progs {

// OfteN -> I'm adding some stuff to spectator mode

// PZ: Moved defines to header file.

//-----------------------//
void Spec_ID();
void Player_Punish();
void Player_VoteMap();

// Resets to MENU_NONE
void SpecResetMenu();

// Local sounds functions
void PlayClientSound(entity player, const string& thesound)
{
	stuffcmd(player, "play ");
	stuffcmd(player,thesound);
	stuffcmd(player,"\n");
}

// used in vote.qc
void BroadcastSound(const string& thesound)
{
	entity te;

	te = find(world, "classname", "player");
	while (te != world)
	{
		if (te->is_connected)
			PlayClientSound(te, thesound);

		te = find(te, "classname", "player");
	}

	te = find(world, "classname", "spec");
	while (te != world)
	{
		if (te->is_connected)
			PlayClientSound(te, thesound);

		te = find(te, "classname", "spec");
	}
}

// PZ: This is used for the below function. Does the actual playing.
void PlayDelayedSound_Think()
{
    if (self->owner == world)         // broadcast to all players
		BroadcastSound(self->path);
	else                              // send to `owner`, only
		PlayClientSound(self->owner, self->path);
	// remove the timer entity that called this function
	dremove(self);
}

// PZ: This function allows you to play a sound at a delayed time. You can play it for a certain
//     `player`, or for all players (when `player` is `world`).
void PlayDelayedSound(entity player, const string& theSound, float delayDuration)
{
	// create timer entity to play the sound at a later time
	entity te = spawnServerSide();
	te->netname = string("DelayedSoundTimer_") + theSound;
	te->nextthink = time + delayDuration;
	te->path  = theSound;
	te->owner = player;
	te->think = PlayDelayedSound_Think;
}

/*void () menu_sound =
{
	stuffcmd(self, "play misc/menu3\nbf\n");
};*/

void chargesound()
{
	stuffcmd(self, "play weapons/guerset\nbf\n");
}

void MachineryScan();
void HelpersScan();
void ClassScan();
void GetTeamClassScan(float teamnum);

//


// Menu handlers

void Menu_Default_Inp(float inp);
void Menu_Default();
void Menu_Charging();


//============================================================

void sprintlineteam( float num);
void sprintlineb();

/*
===========
GetTeslaSpectatorScan
===========
*/
void GetTeslaSpectatorScan( float team_num) {
	entity	gun;
	string	st;
	float		numFound;

	numFound = 0;

	gun = find( world, "classname", "building_tesla" );
	while ( gun  != world) {
		if ( gun->real_owner->team_no == team_num ) {
			sprint( self, PR_PRINT_HIGH, "Tesla owner: ", gun->real_owner->netname, "\n" );

			// Gizmo - this is just copied from engineer.qc: Engineer_UseTesla()
			sprint(self, PR_PRINT_HIGH, S_("^bHealth^b:"));
			st = ftos(gun->health);
			sprint(self, PR_PRINT_HIGH, st);
			sprint(self, PR_PRINT_HIGH, S_("^b/^b"));
			st = ftos(gun->max_health);
			sprint(self, PR_PRINT_HIGH, st);
			sprint(self, PR_PRINT_HIGH, S_(" ^bCells^b:"));
			st = ftos(gun->ammo_cells);
			sprint(self, PR_PRINT_HIGH, st);
			sprint(self, PR_PRINT_HIGH, S_("^b/^b"));
			st = ftos(gun->maxammo_cells);
			sprint(self, PR_PRINT_HIGH, st);
			st = ftos(gun->has_sentry);
			sprint(self, PR_PRINT_HIGH, S_(" ^bNormal^b Ups Left:"));
			sprint(self, PR_PRINT_HIGH, st);
			st = ftos(gun->has_tesla);
			sprint(self, PR_PRINT_HIGH, S_(" ^bMisc^b Ups Left:"));
			sprint(self, PR_PRINT_HIGH, st);
			sprint(self, PR_PRINT_HIGH, "\n");

			sprint(self, PR_PRINT_HIGH, S_("^bVoltage^b:"));
			st = ftos(gun->ammo_shells);
			sprint(self, PR_PRINT_HIGH, st);
			sprint(self, PR_PRINT_HIGH, S_(" ^bAmperage^b:"));
			st = ftos(gun->ammo_nails);
			sprint(self, PR_PRINT_HIGH, st);
			sprint(self, PR_PRINT_HIGH, S_(" ^bPower Supply^b:"));
			st = ftos(gun->ammo_rockets);
			sprint(self, PR_PRINT_HIGH, st);
			sprint(self, PR_PRINT_HIGH, "\n");
			if (gun->tf_items) {
				float num;
				num = 0;
				if (gun->tf_items & PR_NIT_TURRET) {
					sprint(self, PR_PRINT_HIGH, "TeslaTurret(tm). ");
					num = num + 1;
				}
				if (gun->tf_items & PR_NIT_SCANNER) {
					sprint(self, PR_PRINT_HIGH, "Improved Targeter. ");
					num = num + 1;
				}
				if (gun->tf_items & PR_NIT_AUTOID) {
					sprint(self, PR_PRINT_HIGH, "Spy Detector. ");
					num = num + 1;
				}
				if (gun->tf_items & PR_NIT_KEVLAR) {
					sprint(self, PR_PRINT_HIGH, "Kevlar Armor. ");
					num = num + 1;
				}
				if (gun->tf_items & PR_NIT_BLAST) {
					sprint(self, PR_PRINT_HIGH, "Blast Armor. ");
					num = num + 1;
				}
				if (gun->tf_items & PR_NIT_ASBESTOS) {
					sprint(self, PR_PRINT_HIGH, "Asbestos Armor. ");
					num = num + 1;
				}
				if (gun->tf_items & PR_NIT_TELEPORTER) {
					sprint(self, PR_PRINT_HIGH, "Upgrades from frags. ");
					num = num + 1;
				}
				if (gun->tf_items & PR_NIT_TESLA_CLOAKING) {
					sprint(self, PR_PRINT_HIGH, "Cloaking Device. ");
					num = num + 1;
				}
				if (num != 0)
					sprint(self, PR_PRINT_HIGH, "\n");
			}

			sprint(self, PR_PRINT_HIGH, S_("Tesla Sentry ^bFRAGS^b: ")); //CH Displays kills of tesla
				st = ftos(gun->frags);
			sprint(self, PR_PRINT_HIGH, st);
			sprint(self, PR_PRINT_HIGH, "\n\n");

			numFound = numFound + 1;
		}

		gun = find( gun, "classname", "building_tesla" );
	}

	if ( !numFound )
		sprint( self, PR_PRINT_HIGH, "No tesla sentries on this team.\n" );
}

/*
===========
TeslaSpectatorScan

Shows details about all the teslas in the game
===========
*/
void TeslaSpectatorScan() {
	sprint(self, PR_PRINT_HIGH, S_("\n^(\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81^) Tesla Scan ^(\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81^)\n"));

	sprintlineteam( 1 );
	GetTeslaSpectatorScan( 1 );

	if ( number_of_teams > 1 ) {
		sprintlineteam( 2 );
		GetTeslaSpectatorScan( 2 );

		if ( number_of_teams > 2 ) {
			sprintlineteam( 3 );
			GetTeslaSpectatorScan( 3 );

			if ( number_of_teams > 3 ) {
				sprintlineteam( 4 );
				GetTeslaSpectatorScan( 4 );
			}
		}
	}

	sprintlineb();
}

/*
===========
GetSentryGunSpectatorScan
===========
*/
void GetSentryGunSpectatorScan( float team_num) {
	entity	gun;
	string	st;
	float		numFound;

	numFound = 0;

	gun = find( world, "classname", "building_sentrygun" );
	while ( gun  != world) {
		if ( gun->real_owner->team_no == team_num ) {
			sprint( self, PR_PRINT_HIGH, "Sentry Gun owner: ", gun->real_owner->netname, "\n" );

			// Gizmo - this is just copied from engineer.qc: Engineer_UseSentryGun()
			sprint(self, PR_PRINT_HIGH, S_("^bLevel^b "));
			st = Return_Colored_Num(gun->weapon);
			sprint(self, PR_PRINT_HIGH, st);
			sprint(self, PR_PRINT_HIGH, " Sentry Gun has ");
			st = ftos(gun->health);
			sprint(self, PR_PRINT_HIGH, st);
			sprint(self, PR_PRINT_HIGH, S_("^b/^b"));
			st = ftos(gun->max_health);
			sprint(self, PR_PRINT_HIGH, st);
			sprint(self, PR_PRINT_HIGH, S_(" ^bhealth^b, "));
			st = ftos(gun->ammo_shells);
			sprint(self, PR_PRINT_HIGH, st);
			sprint(self, PR_PRINT_HIGH, S_(" ^bshells^b"));
			if (gun->weapon == 3)
			{
				st = ftos(gun->ammo_rockets);
				sprint(self, PR_PRINT_HIGH, ", ");
				sprint(self, PR_PRINT_HIGH, st);
				sprint(self, PR_PRINT_HIGH, S_(" ^brockets^b"));
			}
			sprint(self, PR_PRINT_HIGH, "\n");

			sprint(self, PR_PRINT_HIGH, S_("Sentry Gun ^bFRAGS^b: ")); //CH Displays kills of sent
				st = ftos(gun->frags);
			sprint(self, PR_PRINT_HIGH, st);
			sprint(self, PR_PRINT_HIGH, "\n\n");

			numFound = numFound + 1;
		}

		gun = find( gun, "classname", "building_sentrygun" );
	}

	if ( !numFound )
		sprint( self, PR_PRINT_HIGH, "No sentry guns on this team.\n" );
}

/*
===========
SentryGunSpectatorScan

Shows details about all the sentry guns in the game
===========
*/
void SentryGunSpectatorScan() {
	sprint(self, PR_PRINT_HIGH, S_("\n^(\x81\x81\x81\x81\x81\x81\x81\x81^) SentryGun Scan ^(\x81\x81\x81\x81\x81\x81\x81\x81^)\n"));

	sprintlineteam( 1 );
	GetSentryGunSpectatorScan( 1 );

	if ( number_of_teams > 1 ) {
		sprintlineteam( 2 );
		GetSentryGunSpectatorScan( 2 );

		if ( number_of_teams > 2 ) {
			sprintlineteam( 3 );
			GetSentryGunSpectatorScan( 3 );

			if ( number_of_teams > 3 ) {
				sprintlineteam( 4 );
				GetSentryGunSpectatorScan( 4 );
			}
		}
	}

	sprintlineb();
}

//============================================================

/*
===========
GetHelperSpectatorScan
===========
*/
void GetHelperSpectatorScan( float team_num) {
	entity	e;
	float		numFound;

	numFound = 0;

	e = find( world, TO_STR(PR_monsterflag), PR_STRFLAG_MONSTER );
	while ( e  != world) {
		if ( e->takedamage ) {
			if ( e->health > 0 ) {
				if ( IsOwnedMonster( e ) ) {
					if ( e->real_owner->team_no == team_num ) {
						sprint( self, PR_PRINT_HIGH, "Helper owner: ", e->real_owner->netname, "\n" );
						PrintOwnedMonsterDetails( self, e );
						sprint( self, PR_PRINT_HIGH, "\n" );

						numFound = numFound + 1;
					}
				}
			}
		}

		e = find( e, TO_STR(PR_monsterflag), PR_STRFLAG_MONSTER );
	}

	if ( !numFound )
		sprint( self, PR_PRINT_HIGH, "No helpers on this team.\n" );
}

/*
===========
HelperSpectatorScan

Shows details about all the helpers in the game
===========
*/
void HelperSpectatorScan() {
	sprint(self, PR_PRINT_HIGH, S_("\n^(\x81\x81\x81\x81\x81\x81\x81\x81^) Helper Scan ^(\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81^)\n"));

	sprintlineteam( 1 );
	GetHelperSpectatorScan( 1 );

	if ( number_of_teams > 1 ) {
		sprintlineteam( 2 );
		GetHelperSpectatorScan( 2 );

		if ( number_of_teams > 2 ) {
			sprintlineteam( 3 );
			GetHelperSpectatorScan( 3 );

			if ( number_of_teams > 3 ) {
				sprintlineteam( 4 );
				GetHelperSpectatorScan( 4 );
			}
		}
	}

	sprintlineb();
}

//============================================================

/*
===========
SpectatorConnect

called when a spectator connects to a server
============
*/
void SpectatorConnect()
{
	//ClientFixRankings(); // FrikBot // PZ: C code handles this.

	// PZ: Mark him as human. This caused a lot of problems without this set. Specs couldn't see centerprints (menus).
	// They couldn't use commands, like "cmd admin login". Their impulses, below, also did not work. It broke many other things for spectators.
	self->ishuman = PR_TRUE;

	bprint (PR_PRINT_MEDIUM, "Spectator ");
	bprint (PR_PRINT_MEDIUM, self->netname);
	bprint (PR_PRINT_MEDIUM, " connected\n");

	stuffcmd(self, "bind 9 \"impulse 9\"\n");
	stuffcmd(self, "bind 0 \"impulse 10\"\n");

	TeamFortress_Alias("punish", PR_IMPULSE_PUNISH, 0);
	TeamFortress_Alias("votemap", PR_IMPULSE_VOTEMAP, 0);
	TeamFortress_Alias("votebot", PR_IMPULSE_VOTEBOT, 0);  // PZ: for "votebot"
	TeamFortress_Alias("voteyes", PR_IMPULSE_VOTEYES, 0);
	TeamFortress_Alias("voteno", PR_IMPULSE_VOTENO, 0);

	self->ex_skill_min = 0; // OfN - Used as password attempt count
	self->ex_skill_max = time; // OfN - Used on cmnd.qc
	self->option2 = time; // OfN - Used on cmnd.qc

	self->owned_by = 0; //#VOTED_JUSTJOINED;

	self->is_connected = PR_TRUE;

	if (spec_menu == 1)
	{
		stuffcmd(self, "cl_chasecam 0\n"); // see thru eyes (default state now)
		self->has_camera = 0;

		self->current_menu = PR_SPEC_MENU_DEFAULT;
		self->menu_count = PR_MENU_REFRESH_RATE;

		self->has_sensor = time;

		self->tf_items = self->tf_items | PR_NIT_AUTOSCANNER;
	}

	self->classname = "spec"; // spectators classname is always "spec", for admin code handling by now

	self->admin_kick = world;

	num_specs = num_specs + 1;

	if (num_specs == 1 && num_players == 0)
		World_FadeMid(3,0,2);

	//self.goalentity = world; // used for impulse 1 below
}

/*
===========
SpectatorDisconnect

called when a spectator disconnects from a server
============
*/
void SpectatorDisconnect()
{
	bprint (PR_PRINT_MEDIUM, "Spectator ");
	bprint (PR_PRINT_MEDIUM, self->netname);
	bprint (PR_PRINT_MEDIUM, " leaves the server\n");

	if (self->admin_flag)
		AdminLoggedOut(self);

	self->is_connected = PR_FALSE;
	self->classname = ""; // FIXED: if this isnt here, admin kick-cycle reports "phantom" spectators

	// PZ: Make it so that this client/player entity is cleared out, because when bots join and take an entity, it isn't currently clearing
	//     out the entity before giving it to the bot, and that can cause things like bots being 'cmd admin's on the server, if admin spec left.
	clearAllEntityFields(self);

	num_specs = num_specs - 1;

	if (num_specs < 0)
		num_specs = 0;

	if (num_specs == 0)
	{
		if (num_players == 0)
		{
			if (ceasefire)
			{
				ceasefire = PR_FALSE;
				bprint(PR_PRINT_HIGH,"No clients on server, ceasefire ends automatically..\n");
			}

			World_FadeOut(0,0,0);
		}
	}
}

/*
================
SpectatorImpulseCommand

Called by SpectatorThink if the spectator entered an impulse
================
*/
void SpectatorImpulseCommand()
{
	/*if (self.impulse == 1) {
		// teleport the spectator to the next spawn point
		// note that if the spectator is tracking, this doesn't do
		// much
		self.goalentity = find(self.goalentity, classname, "info_player_deathmatch");
		if (self.goalentity == world)
			self.goalentity = find(self.goalentity, classname, "info_player_deathmatch");
		if (self.goalentity != world) {
			setorigin(self, self.goalentity.origin);
			self.angles = self.goalentity.angles;
			self.fixangle = #TRUE;			 // turn this way immediately
		}
	}*/

	if (self->impulse == PR_IMPULSE_PUNISH)
	{
		Player_Punish();
	}
	else if (self->impulse == PR_IMPULSE_VOTEMAP)
	{
		Player_VoteMap();
	}
	else if (self->impulse == PR_IMPULSE_VOTEBOT) // PZ: for "votebot"
	{
		Player_VoteBot();
	}
	else if (self->impulse == PR_IMPULSE_VOTEYES)
	{
		Player_VoteYes();
	}
	else if (self->impulse == PR_IMPULSE_VOTENO)
	{
		Player_VoteNo();
	}
	else if (self->impulse == PR_TF_DISPLAYLOCATION)
	{
		display_location();
	}

	if (spec_menu == 0) // Return if spec menus are disabled..
	{
		self->impulse = 0;
		return;
	}

	// Are we inside a menu? then.. use a menu handler
	if (self->current_menu > PR_SPEC_MENU_NONE && self->impulse <= 10)
	{
		if (self->current_menu == PR_SPEC_MENU_DEFAULT && self->has_sensor <= time)
			Menu_Default_Inp(self->impulse);
		else if (self->current_menu == PR_SPEC_MENU_PUNISH)
			Menu_Punish_Input(self->impulse);
		else if (self->current_menu == PR_SPEC_MENU_VOTEMAP)
			Menu_VoteMap_Input(self->impulse);
		else if (self->current_menu == PR_SPEC_MENU_VOTEBOT) // PZ: for "votebot"
			Menu_VoteBot_Input(self->impulse);
	}
	else if (self->current_menu == PR_SPEC_MENU_NONE && self->impulse == 10)
	{
		self->current_menu = PR_SPEC_MENU_DEFAULT;
		self->menu_count = PR_MENU_REFRESH_RATE;
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
	}

	self->impulse = 0;
}

/*
================
SpectatorThink

Called every frame after physics are run
================
*/
void SpectatorThink()
{
	// self.origin, etc contains spectator position, so you could
	// do some neat stuff here

	if (self->impulse && !self->b_menu) // PZ: if we've made it into the waypoint editor menu, let the waypoint editor have the impulse
		SpectatorImpulseCommand();

	// PZ: call this for spectators so that they can use the waypoint editor menus
	if (!self->current_menu)
		if (!frik_botPreThink()) // FrikBot // PZ
			return;

	// TODO: motd

	if (spec_menu == 0)
	{
		return;
	}

	// Time to show the menu? (refresh)
	if (self->current_menu > PR_SPEC_MENU_NONE)
	{
		// menu refresh loop function
		if (self->menu_count > PR_MENU_REFRESH_RATE)
		{
			self->menu_count = 0;
		}
		else
		{
			self->menu_count = self->menu_count + 1;
			return; // exits without centerprint the menu
		}
	}
	else
	{
		if (self->tf_items & PR_NIT_AUTOSCANNER)
			Spec_ID();
	}

	// Menu Displayers
	if (self->current_menu == PR_SPEC_MENU_DEFAULT)
	{
		if (self->has_sensor <= time)
			Menu_Default();
		else
			Menu_Charging();
	}
	else if (self->current_menu == PR_SPEC_MENU_PUNISH)
	{
		Menu_Punish();
	}
	else if (self->current_menu == PR_SPEC_MENU_VOTEMAP)
	{
		Menu_VoteMap();
	}
	else if (self->current_menu == PR_SPEC_MENU_VOTEBOT) // PZ: for "votebot"
	{
		Menu_VoteBot();
	}
}

//=============================================================
// Resets menu to default state

void SpecResetMenu()
{
	self->menu_count = PR_MENU_REFRESH_RATE;
	self->current_menu = PR_SPEC_MENU_NONE;
	CenterPrint(self,"\n");
}

//=====================================================//
// MENUS - Display functions and menu impulse Handlers //
//-----------------------------------------------------//

void Menu_Default()
{
	string st2;

	if (self->has_camera != 1)
	{
		if (self->tf_items & PR_NIT_AUTOSCANNER)
			st2 = S_("^8.. ^bScanner^b state (^bon^b)           \n^9.. ^bChaseCam^b state (^bon^b)          \n\n");
		else
			st2 = S_("^8.. Scanner state (off)          \n^9.. ^bChaseCam^b state (^bon^b)          \n\n");
	}
	else
	{
		if (self->tf_items & PR_NIT_AUTOSCANNER)
			st2 = S_("^8.. ^bScanner^b state (^bon^b)           \n^9.. ChaseCam state (off)         \n\n");
		else
			st2 = S_("^8.. Scanner state (off)          \n^9.. ChaseCam state (off)         \n\n");
	}

	centerprint(self,S_("\x9D^(\x81\x81\x81\x81\x81\x81\x81^) ^bSpectator Menu^b: ^(\x81\x81\x81\x81\x81\x81\x81^)^>\n\n^1.. Machinery Battlefield Scan   \n^2.. Helpers Battlefield Scan     \n^3.. Player Classes Scan          \n^4.. Overall Balance (N/A)        \n^5.. SentryGun detail report      \n^6.. Tesla detail report          \n^7.. Helper detail report         \n"),st2,S_("^0^b..^b Hide Menu                    \n\n\x9D^(\x81\x81\x81\x81\x81\x81\x81^)^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^(\x81\x81\x81\x81\x81\x81\x81^)^>"));
}

void Menu_Default_Inp(float inp)
{
	if (inp == 10)
	{
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		SpecResetMenu();
		centerprint(self,S_("^bTurning off sensors^b...\n\nPress ^b\"^0\"^b to ^btoggle^b status"));
	}
	else if (inp == 9)
	{
		if (self->has_camera == 1)
		{
			stuffcmd(self, "cl_chasecam 0\n");
		}
		else
		{
			stuffcmd(self, "cl_chasecam 1\n");
		}

		self->has_camera = !self->has_camera;
		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		Menu_Default();
	}
	else if (inp == 8)
	{
		if (self->tf_items & PR_NIT_AUTOSCANNER)
			self->tf_items = self->tf_items - (self->tf_items & PR_NIT_AUTOSCANNER);
		else
			self->tf_items = self->tf_items | PR_NIT_AUTOSCANNER;

		CuTFMenuSound(PR_MENUSOUND_BROWSE);
		Menu_Default();
	}
	else if (inp == 1)
	{
		chargesound();
		MachineryScan();
		self->has_sensor = time + PR_CHARGE_SENSORS_TIME;
		Menu_Charging();
	}
	else if (inp == 2)
	{
		chargesound();
		HelpersScan();
		self->has_sensor = time + PR_CHARGE_SENSORS_TIME;
		Menu_Charging();
	}
	else if (inp == 3)
	{
		chargesound();
		ClassScan();
		self->has_sensor = time + PR_CHARGE_SENSORS_TIME;
		Menu_Charging();
	}

	// Gizmo - added the (N/A) items that have been that way for ages
	else if ( inp == 5 ) {
		chargesound();
		SentryGunSpectatorScan();
		self->has_sensor = time + PR_CHARGE_SENSORS_TIME;
		Menu_Charging();
	}
	else if ( inp == 6 ) {
		chargesound();
		TeslaSpectatorScan();
		self->has_sensor = time + PR_CHARGE_SENSORS_TIME;
		Menu_Charging();
	}

	// Gizmo - added helper scan
	else if ( inp == 7 ) {
		chargesound();
		HelperSpectatorScan();
		self->has_sensor = time + PR_CHARGE_SENSORS_TIME;
		Menu_Charging();
	}
}


//=========================================================================
// ID's the player in your sights, and if you're a medic or engineer,
// reports their health and armor.

void Spec_ID()
{
	// OfN Returns if any menu is being displayed
	if (self->current_menu != 0)
		return;

	//WK All the inAuto and spacer stuff I added
	vector src;
	string st, cls;
	string spacer;
	src = self->origin + v_forward*15; // was 10
	src[Z] = self->absmin[Z] + self->size[Z] * 0.7;

	//makevectors(self.angles); // Added - NEEDED?
	traceline (src, src + v_forward*2048, PR_TL_ANY_SOLID, self->goalentity);

	// Skip player beeing tracked by ourselves
	if (trace_ent == self->goalentity || trace_ent == world)
	{
		//src = self.goalentity.origin + v_forward*15; // was 10
		//src_z = self.absmin_z + self.size_z * 0.7;

		//traceline (src, src + v_forward*2048, #TL_ANY_SOLID, self.goalentity);
		//sprint(self,#PRINT_HIGH,self.goalentity.classname);
		return;
	}

	spacer = "\n";

	if (trace_ent != world && trace_ent->origin != world->origin )
	{
		if (trace_ent->classname == "player" && trace_ent->health > 0)
		{
			self->StatusRefreshTime = time + 1.5;

			if (trace_ent->playerclass == PR_PC_CUSTOM)
				cls = TeamFortress_GetJobName(trace_ent->job);
			else
				cls = TeamFortress_GetClassName(trace_ent->playerclass);

			st = ftos(trace_ent->health);

			centerprint(self, spacer, trace_ent->netname, "\n\n", cls, "\n\n", st, " health\n");
		}
		else if (trace_ent->classname == "building_dispenser")
		{
			self->StatusRefreshTime = time + 1.5;

			centerprint(self, spacer,"Dispenser made by\n\n", trace_ent->real_owner->netname);
		}
		else if (trace_ent->classname == "building_fieldgen")
		{
			self->StatusRefreshTime = time + 1.5;

			centerprint(self, spacer,"Field Generator made by\n\n", trace_ent->real_owner->netname);
		}

		else if (trace_ent->classname == "building_sentrygun" || trace_ent->classname == "building_sentrygun_base")
		{
			self->StatusRefreshTime = time + 1.5;

			centerprint(self, spacer,"Sentrygun made by\n\n", trace_ent->real_owner->netname,"\n");
		}
		else if (trace_ent->classname == "building_tesla")
		{
			self->StatusRefreshTime = time + 1.5;

			centerprint(self, spacer,"Tesla Sentry made by\n\n", trace_ent->real_owner->netname,"\n");
		}
		else if (trace_ent->classname == "building_camera")
		{
			self->StatusRefreshTime = time + 1.5;

			centerprint(self, spacer,"Security Camera made by\n\n", trace_ent->real_owner->netname,"\n");
		}
		else if (trace_ent->classname == "building_sensor")
		{
			self->StatusRefreshTime = time + 1.5;

			centerprint(self, spacer,"Motion Sensor made by\n\n", trace_ent->real_owner->netname,"\n");
		}
		else if (trace_ent->classname == "building_teleporter")
		{
			self->StatusRefreshTime = time + 1.5;

			centerprint(self, spacer,"Teleporter Pad made by\n\n", trace_ent->real_owner->netname,"\n");
		}
		else if (trace_ent->classname == "monster_demon1")
		{
			//custom_demon_name(trace_ent); //CH
			self->StatusRefreshTime = time + 1.5;
			st = ftos(trace_ent->health);

			centerprint(self, spacer,"Demon summoned by\n\n", trace_ent->real_owner->netname,"\n\n", st, " health");
		}
		else if (trace_ent->classname == "monster_army")
		{
			//custom_demon_name(trace_ent); //CH
			self->StatusRefreshTime = time + 1.5;
			st = ftos(trace_ent->health);

			centerprint(self, spacer,"Soldier in the pay of\n\n", trace_ent->real_owner->netname,"\n\n", st, " health");
		}
		else if (trace_ent->classname == "monster_shambler")
		{
			//custom_demon_name(trace_ent); //CH
			self->StatusRefreshTime = time + 1.5;
			st = ftos(trace_ent->health);

			centerprint(self, spacer,"Shambler under control of\n\n", trace_ent->real_owner->netname,"\n\n", st, " health");
		}
		else if (trace_ent->classname == "monster_wizard") //- OfN
		{
			//custom_demon_name(trace_ent);
			self->StatusRefreshTime = time + 1.5;
			st = ftos(trace_ent->health);

			centerprint(self, spacer,"Scrag under control of\n\n", trace_ent->real_owner->netname,"\n\n", st, " health");
		}
		else if (trace_ent->classname == "monster_fish") //- OfN
		{
			//custom_demon_name(trace_ent);
			self->StatusRefreshTime = time + 1.5;
			st = ftos(trace_ent->health);

			centerprint(self, spacer,"Piranha under control of\n\n", trace_ent->real_owner->netname,"\n\n", st, " health");
		}
		else if (trace_ent->classname == "monster_gremlin") //- OfN
		{
			//custom_demon_name(trace_ent);
			self->StatusRefreshTime = time + 1.5;
			st = ftos(trace_ent->health);

			centerprint(self, spacer,"Gremlin under control of\n\n", trace_ent->real_owner->netname,"\n\n", st, " health");
		}
	}

	// teamplay = oldteamplay;
}

#define PR_DISPENSER_DEF_POWER 	1
#define PR_CAMERA_DEF_POWER		0.25
#define PR_SENSOR_DEF_POWER		0.5 // spy 1.25 thief 1.60
	#define PR_SENSOR_DEF_PW2		  0.75
	#define PR_SENSOR_DEF_PW3		  0.35
#define PR_SENTRY_DEF_POWER		2.5 // base 3, 3.5 4
//#define TESLA_DEF_POWER		  3 // base 3, 4, 5
#define PR_FIELDGEN_DEF_POWER		2 // 2=1 field
#define PR_TELEPORTER_DEF_POWER	1.75 // 2=1 Route


//TODO: x Teleporter routes
//TODO: x Working force fields
//TODO: tesla upgrade ++ power, circuits ++ power
//TODO: sentry upgrade ++ power, circuits ++ power

#define PR_SCRAG_POWER 			1
#define PR_FIEND_POWER 			3
#define PR_SHAMBLER_POWER			6
#define PR_ARMY_POWER				4

#define PR_ARMY_POWER_INC			10

void Menu_Charging()
{
	string st2;

	if (self->has_camera == 1)
	{
		if (self->tf_items & PR_NIT_AUTOSCANNER)
			st2 = S_("    ^bScanner^b state (^bon^b)           \n    ^bChaseCam^b state (^bon^b)          \n\n");
		else
			st2 = S_("    Scanner state (off)          \n    ^bChaseCam^b state (^bon^b)          \n\n");
	}
	else
	{
		if (self->tf_items & PR_NIT_AUTOSCANNER)
			st2 = S_("    ^bScanner^b state (^bon^b)           \n    ChaseCam state (off)         \n\n");
		else
			st2 = "    Scanner state (off)          \n    ChaseCam state (off)         \n\n";
	}

	centerprint(self,S_("\x9D^(\x81\x81\x81\x81\x81\x81\x81^) ^bSpectator Menu^b: ^(\x81\x81\x81\x81\x81\x81\x81^)^>\n\n    Machinery Battlefield Scan   \n    Helpers Battlefield Scan     \n    Player Classes Scan          \n    Overall Balance              \n    Sentry gun detail report     \n    Tesla detail report          \n    Helper detail report         \n"),st2,S_("    Hide Menu                    \n\n\x9D^(\x81\x81\x81\x81\x81\x81\x81^)^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^-^(\x81\x81\x81\x81\x81\x81\x81^)^>\n\nCharging sensors..."));
	stuffcmd(self,"bf\n");
//	centerprint(self,"Charging sensors...\n");
}

void GetTeamMachineryScan(float team_num);

/*void ()sprintline =
{
	sprint(self, #PRINT_HIGH, "\n\{157}�����������������������������������\n\n");
};*/
void sprintlineb()
{
	sprint(self, PR_PRINT_HIGH, S_("\n^(\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81\x81^)\n\n"));
}
void sprintlineteam(float num)
{
	if (num == 1)
		sprint(self, PR_PRINT_HIGH, S_("\n\x9D^-^-^-^-^-^-^-^-^-^-^> Blue Team \x9D^-^-^-^-^-^-^-^-^-^-^-^>\n\n"));
	else if (num == 2)
		sprint(self, PR_PRINT_HIGH, S_("\n\x9D^-^-^-^-^-^-^-^-^-^-^-^> Red Team \x9D^-^-^-^-^-^-^-^-^-^-^-^>\n\n"));
	else if (num == 3)
		sprint(self, PR_PRINT_HIGH, S_("\n\x9D^-^-^-^-^-^-^-^-^-^> Yellow Team \x9D^-^-^-^-^-^-^-^-^-^-^>\n\n"));
	else if (num == 4)
		sprint(self, PR_PRINT_HIGH, S_("\n\x9D^-^-^-^-^-^-^-^-^-^-^> Green Team \x9D^-^-^-^-^-^-^-^-^-^-^>\n\n"));
}

void MachineryScan()
{
	sprint(self, PR_PRINT_HIGH, S_("\n^(\x81\x81\x81\x81\x81\x81\x81\x81^) Machinery Scan ^(\x81\x81\x81\x81\x81\x81\x81\x81^)\n"));

	sprintlineteam(1);
	GetTeamMachineryScan(1);

	// Gizmo - fixed this for maps with one team (like rj maps)
	if ( number_of_teams > 1 )
	{
		sprintlineteam(2);
		GetTeamMachineryScan(2);

		if (number_of_teams > 2)
		{
			sprintlineteam(3);
			GetTeamMachineryScan(3);

			if (number_of_teams > 3)
			{
				sprintlineteam(4);
				GetTeamMachineryScan(4);
			}
		}
	}

	sprintlineb();
}

void GetTeamMachineryScan(float team_num)
{
	entity te;
	float num, defpw;
	string str;

	defpw = 0;


	// Dispensers
	te = find(world,"classname","building_dispenser");

	num = 0;

	while (te != world)
	{
		if (te->real_owner->team_no == team_num) // Matches Team number
		{
			num = num + 1;
			defpw = defpw + PR_DISPENSER_DEF_POWER;
		}

		te = find (te,"classname","building_dispenser");
	}

	if (num > 0 )
	{
		sprint(self,PR_PRINT_HIGH,"Dispensers: ");
		str=ftos(num);
		sprint(self,PR_PRINT_HIGH,str);
		sprint(self,PR_PRINT_HIGH,"\n");
	}

	// Cameras
	te = find(world,"classname","building_camera");

	num = 0;

	while (te != world)
	{
		if (te->real_owner->team_no == team_num) // Matches Team number
		{
			num = num + 1;
			defpw = defpw + PR_CAMERA_DEF_POWER;
		}

		te = find (te,"classname","building_camera");
	}

	if (num >0)
	{
		sprint(self,PR_PRINT_HIGH,"Cameras: ");
		str=ftos(num);
		sprint(self,PR_PRINT_HIGH,str);
		sprint(self,PR_PRINT_HIGH,"\n");
	}

	// Motion Sensors
	te = find(world,"classname","building_sensor");

	num = 0;

	while (te != world)
	{
		if (te->real_owner->team_no == team_num) // Matches Team number
		{
			num = num + 1;
			defpw = defpw + PR_SENSOR_DEF_POWER;

			if (te->all_active & PR_IMPROVED_FOUR) // Spy detector
			{
				defpw = defpw + PR_SENSOR_DEF_PW2;
				if (te->all_active & PR_IMPROVED_SEVEN) // + thief detector
				{
					defpw = defpw + PR_SENSOR_DEF_PW3;
				}
			}
		}

		te = find (te,"classname","building_sensor");
	}

	if (num >0)
	{
		sprint(self,PR_PRINT_HIGH,"M.Sensors: ");
		str=ftos(num);
		sprint(self,PR_PRINT_HIGH,str);
		sprint(self,PR_PRINT_HIGH,"\n");
	}

	// Teleporter routes
	te = find(world,"classname","player");

	num = 0;

	while (te != world)
	{
		if (te->team_no == team_num) // Matches Team number
		{
			if (te->has_teleporter == 2)
			{
				num = num + 1;
				defpw = defpw + PR_TELEPORTER_DEF_POWER;
			}
		}

		te = find (te,"classname","player");
	}

	if (num >0)
	{
		sprint(self,PR_PRINT_HIGH,"Teleporter routes: ");
		str=ftos(num);
		sprint(self,PR_PRINT_HIGH,str);
		sprint(self,PR_PRINT_HIGH,"\n");
	}

	// Working force fields
	te = find(world,"classname","player");

	num = 0;

	while (te != world)
	{
		if (te->team_no == team_num) // Matches Team number
		{
			if (te->has_fieldgen == 2)
			{
				num = num + 1;
				defpw = defpw + PR_FIELDGEN_DEF_POWER;
			}
		}

		te = find (te,"classname","player");
	}

	if (num >0)
	{
		sprint(self,PR_PRINT_HIGH,"Working force fields: ");
		str=ftos(num);
		sprint(self,PR_PRINT_HIGH,str);
		sprint(self,PR_PRINT_HIGH,"\n");
	}

	// Sentry guns
	te = find(world,"classname","building_sentrygun");

	num = 0;

	while (te != world)
	{
		if (te->real_owner->team_no == team_num) // Matches Team number
		{
			num = num + 1;
			defpw = defpw + PR_SENTRY_DEF_POWER;

			if (te->weapon == 2) // level 2 sentry gun
				defpw = defpw + 0.5;

			if (te->weapon > 2) // level 3 sentry gun
				defpw = defpw + 1;

			if (te->all_active & PR_IMPROVED_FOUR) //enhanced circuits
				defpw = defpw + 0.5;
		}

		te = find (te,"classname","building_sentrygun");
	}

	if (num >0)
	{
		sprint(self,PR_PRINT_HIGH,"Sentry Guns: ");
		str=ftos(num);
		sprint(self,PR_PRINT_HIGH,str);
		sprint(self,PR_PRINT_HIGH,"\n");
	}

	// Tesla
	te = find(world,"classname","building_tesla");

	num = 0;

	while (te != world)
	{
		if (te->real_owner->team_no == team_num) // Matches Team number
		{
			num = num + 1;
			defpw = defpw + PR_SENTRY_DEF_POWER;

			if (te->real_owner->tf_items & PR_NIT_TESLA_UPGRADE)
				defpw = defpw + 1;

			if (te->all_active & PR_IMPROVED_FOUR) //enhanced circuits
				defpw = defpw + 0.5;
		}

		te = find (te,"classname","building_tesla");
	}

	if (num >0)
	{
		sprint(self,PR_PRINT_HIGH,"Teslas: ");
		str=ftos(num);
		sprint(self,PR_PRINT_HIGH,str);
		sprint(self,PR_PRINT_HIGH,"\n");
	}

	// Number of engineers
	te = find(world,"classname","player");

	num = 0;

	while (te != world)
	{
		if (te->real_owner->team_no == team_num && te->playerclass == PR_PC_ENGINEER) // Matches Team number
		{
			num = num + 1;
		}

		te = find (te,"classname","player");
	}

	if (num >0)
	{
		sprint(self,PR_PRINT_HIGH,"Reg.Engineers: ");
		str=ftos(num);
		sprint(self,PR_PRINT_HIGH,str);
		sprint(self,PR_PRINT_HIGH,"\n");
	}

	//Defensive power sprint
	sprint(self,PR_PRINT_HIGH,S_("\n^bDefensive Power^b: "));
	str=ftos(defpw);
	sprint(self,PR_PRINT_HIGH,str);
	sprint(self,PR_PRINT_HIGH,"\n");

}

void GetTeamHelpersScan(float team_num);

void HelpersScan()
{
	sprint(self, PR_PRINT_HIGH, S_("\n^(\x81\x81\x81\x81\x81\x81\x81\x81\x81^) Helpers Scan ^(\x81\x81\x81\x81\x81\x81\x81\x81\x81^)\n"));

	sprintlineteam(1);
	GetTeamHelpersScan(1);

	// Gizmo - fixed this for maps with one team (like rj maps)
	if ( number_of_teams > 1 )
	{
		sprintlineteam(2);
		GetTeamHelpersScan(2);

		if (number_of_teams > 2)
		{
			sprintlineteam(3);
			GetTeamHelpersScan(3);

			if (number_of_teams > 3)
			{
				sprintlineteam(4);
				GetTeamHelpersScan(4);
			}
		}
	}

	sprintlineb();

}

void GetTeamHelpersScan(float team_num)
{
	entity te;
	float num, defpw;
	string str;

	defpw = 0;


	// Scrags
	te = find(world,"classname","monster_wizard");

	num = 0;

	while (te != world)
	{
		if (te->real_owner->team_no == team_num) // Matches Team number
		{
			num = num + 1;
			defpw = defpw + PR_SCRAG_POWER;
		}

		te = find (te,"classname","monster_wizard");
	}

	if (num > 0 )
	{
		sprint(self,PR_PRINT_HIGH,"Scrags: ");
		str=ftos(num);
		sprint(self,PR_PRINT_HIGH,str);
		sprint(self,PR_PRINT_HIGH,"\n");
	}

	// Fiends
	te = find(world,"classname","monster_demon1");

	num = 0;

	while (te != world)
	{
		if (te->real_owner->team_no == team_num) // Matches Team number
		{
			num = num + 1;
			defpw = defpw + PR_FIEND_POWER;
		}

		te = find (te,"classname","monster_demon1");
	}

	if (num > 0 )
	{
		sprint(self,PR_PRINT_HIGH,"Fiends: ");
		str=ftos(num);
		sprint(self,PR_PRINT_HIGH,str);
		sprint(self,PR_PRINT_HIGH,"\n");
	}

	// Shamblers
	te = find(world,"classname","monster_shambler");

	num = 0;

	while (te != world)
	{
		if (te->real_owner->team_no == team_num) // Matches Team number
		{
			num = num + 1;
			defpw = defpw + PR_SHAMBLER_POWER;
		}

		te = find (te,"classname","monster_shambler");
	}

	if (num > 0 )
	{
		sprint(self,PR_PRINT_HIGH,"Shamblers: ");
		str=ftos(num);
		sprint(self,PR_PRINT_HIGH,str);
		sprint(self,PR_PRINT_HIGH,"\n");
	}

	// Soldiers
	te = find(world,"classname","monster_army");

	num = 0;

	while (te != world)
	{
		if (te->real_owner->team_no == team_num) // Matches Team number
		{
			num = num + 1;
			defpw = defpw + PR_ARMY_POWER;

			defpw = defpw + ((te->has_sensor/17)* PR_ARMY_POWER_INC);
		}

		te = find (te,"classname","monster_army");
	}

	if (num > 0 )
	{
		sprint(self,PR_PRINT_HIGH,"Soldiers: ");
		str=ftos(num);
		sprint(self,PR_PRINT_HIGH,str);
		sprint(self,PR_PRINT_HIGH,"\n");
	}

	// Warlocks
	te = find(world,"classname","player");

	num = 0;

	while (te != world)
	{
		if (te->job & PR_JOB_WARLOCK) // Matches Team number
		{
			num = num + 1;
		}

		te = find (te,"classname","player");
	}

	if (num >0)
	{
		sprint(self,PR_PRINT_HIGH,"Warlocks: ");
		str=ftos(num);
		sprint(self,PR_PRINT_HIGH,str);
		sprint(self,PR_PRINT_HIGH,"\n");
	}

	// Army members
	te = find(world,"classname","player");

	num = 0;

	while (te != world)
	{
		if (te->job & PR_JOB_ARMY) // Matches job
		{
			num = num + 1;
		}

		te = find (te,"classname","player");
	}

	if (num >0)
	{
		sprint(self,PR_PRINT_HIGH,"Army Officers: ");
		str=ftos(num);
		sprint(self,PR_PRINT_HIGH,str);
		sprint(self,PR_PRINT_HIGH,"\n");
	}

	//Defensive power sprint
	sprint(self,PR_PRINT_HIGH,S_("\n^bHelpers Power^b: "));
	str=ftos(defpw);
	sprint(self,PR_PRINT_HIGH,str);
	sprint(self,PR_PRINT_HIGH,"\n");

}

void ClassScan()
{
	sprint(self, PR_PRINT_HIGH, S_("\n^(\x81\x81\x81\x81\x81\x81\x81\x81\x81^) Classes Scan ^(\x81\x81\x81\x81\x81\x81\x81\x81\x81^)\n"));

	sprintlineteam(1);
	GetTeamClassScan(1);

	// Gizmo - fixed this for maps with one team (like rj maps)
	if (number_of_teams > 1)
	{
		sprintlineteam(2);
		GetTeamClassScan(2);

		if (number_of_teams > 2)
		{
			sprintlineteam(3);
			GetTeamClassScan(3);

			if (number_of_teams > 3)
			{
				sprintlineteam(4);
				GetTeamClassScan(4);
			}
		}
	}

	sprintlineb();

}

//====================================================================================
// same as in TeamFortress_TeamShowMemberClasses on TFortTM.qc, slightly modified

void GetTeamClassScan(float teamnum)
{
	/*local string st;
	st = infokey(world, "no_showmembers");	*/

	entity e;
	float found;

	found = PR_FALSE;

	e = find(world, "classname", "player");
	while (e != world)
	{
		if (e->team_no == teamnum)
		{
			if (e->model != string_null) // check if valid player
			{
				if (!found)
				{
					found = PR_TRUE;
					//sprint (self, #PRINT_HIGH, "The other members of your team are:\n");
				}

				sprint (self, PR_PRINT_HIGH, e->netname);
				sprint (self, PR_PRINT_HIGH, " : ");
				if (e->playerclass == PR_PC_CUSTOM)
					TeamFortress_PrintClassName(self,e->playerclass, (e->tfstate & PR_TFSTATE_RANDOMPC));
				else
					TeamFortress_PrintJobName(self,e->job);

			}
		}
		e = find(e, "classname", "player");
	}

	if (!found)
	{
		//sprint (Player, #PRINT_HIGH, "There are no other players on your team.\n");
		sprint(self,PR_PRINT_HIGH,"No players on this team.\n");
	}
}

} // END namespace Progs
