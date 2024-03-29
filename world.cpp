//================================================================//
//- CustomTF Worldspawn and related stuff			   OfteN	  //
//- 30-01-2001----------------------------------------------------//

#include "progs.h"
#include "ofndefs.h"
#include "coop_defs.h"
#include "spectate.h"
#include "client.h"
#include "prozac.h"
#include "weapons.h"
#include "agr.h"
#include "invade.h"
#include "neo.h"
#include "gweapons.h"
#include "environ.h"
#include "frikbot/bot_qw.h"
#include "vote2.h"

namespace Progs {

//-- OFTEN --//
void Wiz_Precache();
void Fish_Precache();
void Gremlin_Precache();
void Field_Precache();
void MenuPrecache();
void UpdateInfos(float startup);
void ResetVoteEnt();
void PrecacheRuneModels();
//==============//

void InitBodyQue();
entity GetQueEntry();

void Main()
{
// Gizmo - commented this out since it's not needed
/*
	dprint ("main function\n");

// these are just commands the the prog compiler to copy these files

	precache_file ("progs.dat");
	precache_file ("gfx.wad");
	precache_file ("quake.rc");
	precache_file ("default.cfg");

	precache_file ("end1.bin");
	precache_file2 ("end2.bin");

	precache_file ("demo1.dem");
	precache_file ("demo2.dem");
	precache_file ("demo3.dem");

//
// these are all of the lumps from the cached.ls files
//
	precache_file ("gfx/palette.lmp");
	precache_file ("gfx/colormap.lmp");

	precache_file2 ("gfx/pop.lmp");

	precache_file ("gfx/complete.lmp");
	precache_file ("gfx/inter.lmp");

	precache_file ("gfx/ranking.lmp");
	precache_file ("gfx/vidmodes.lmp");
	precache_file ("gfx/finale.lmp");
	precache_file ("gfx/conback.lmp");
	precache_file ("gfx/qplaque.lmp");

	precache_file ("gfx/menudot1.lmp");
	precache_file ("gfx/menudot2.lmp");
	precache_file ("gfx/menudot3.lmp");
	precache_file ("gfx/menudot4.lmp");
	precache_file ("gfx/menudot5.lmp");
	precache_file ("gfx/menudot6.lmp");

	precache_file ("gfx/menuplyr.lmp");
	precache_file ("gfx/bigbox.lmp");
	precache_file ("gfx/dim_modm.lmp");
	precache_file ("gfx/dim_drct.lmp");
	precache_file ("gfx/dim_ipx.lmp");
	precache_file ("gfx/dim_tcp.lmp");
	precache_file ("gfx/dim_mult.lmp");
	precache_file ("gfx/mainmenu.lmp");

	precache_file ("gfx/box_tl.lmp");
	precache_file ("gfx/box_tm.lmp");
	precache_file ("gfx/box_tr.lmp");

	precache_file ("gfx/box_ml.lmp");
	precache_file ("gfx/box_mm.lmp");
	precache_file ("gfx/box_mm2.lmp");
	precache_file ("gfx/box_mr.lmp");

	precache_file ("gfx/box_bl.lmp");
	precache_file ("gfx/box_bm.lmp");
	precache_file ("gfx/box_br.lmp");

	precache_file ("gfx/sp_menu.lmp");
	precache_file ("gfx/ttl_sgl.lmp");
	precache_file ("gfx/ttl_main.lmp");
	precache_file ("gfx/ttl_cstm.lmp");

	precache_file ("gfx/mp_menu.lmp");

	precache_file ("gfx/netmen1.lmp");
	precache_file ("gfx/netmen2.lmp");
	precache_file ("gfx/netmen3.lmp");
	precache_file ("gfx/netmen4.lmp");
	precache_file ("gfx/netmen5.lmp");

	precache_file ("gfx/sell.lmp");

	precache_file ("gfx/help0.lmp");
	precache_file ("gfx/help1.lmp");
	precache_file ("gfx/help2.lmp");
	precache_file ("gfx/help3.lmp");
	precache_file ("gfx/help4.lmp");
	precache_file ("gfx/help5.lmp");

	precache_file ("gfx/pause.lmp");
	precache_file ("gfx/loading.lmp");

	precache_file ("gfx/p_option.lmp");
	precache_file ("gfx/p_load.lmp");
	precache_file ("gfx/p_save.lmp");
	precache_file ("gfx/p_multi.lmp");

// sounds loaded by C code
	precache_sound ("misc/menu1.wav");
	precache_sound ("misc/menu2.wav");
	precache_sound ("misc/menu3.wav");

	precache_sound ("ambience/water1.wav");
	precache_sound ("ambience/wind2.wav");

// shareware
	precache_file ("maps/start.bsp");

	precache_file ("maps/e1m1.bsp");
	precache_file ("maps/e1m2.bsp");
	precache_file ("maps/e1m3.bsp");
	precache_file ("maps/e1m4.bsp");
	precache_file ("maps/e1m5.bsp");
	precache_file ("maps/e1m6.bsp");
	precache_file ("maps/e1m7.bsp");
	precache_file ("maps/e1m8.bsp");

// registered
	precache_file2 ("gfx/pop.lmp");

	precache_file2 ("maps/e2m1.bsp");
	precache_file2 ("maps/e2m2.bsp");
	precache_file2 ("maps/e2m3.bsp");
	precache_file2 ("maps/e2m4.bsp");
	precache_file2 ("maps/e2m5.bsp");
	precache_file2 ("maps/e2m6.bsp");
	precache_file2 ("maps/e2m7.bsp");

	precache_file2 ("maps/e3m1.bsp");
	precache_file2 ("maps/e3m2.bsp");
	precache_file2 ("maps/e3m3.bsp");
	precache_file2 ("maps/e3m4.bsp");
	precache_file2 ("maps/e3m5.bsp");
	precache_file2 ("maps/e3m6.bsp");
	precache_file2 ("maps/e3m7.bsp");

	precache_file2 ("maps/e4m1.bsp");
	precache_file2 ("maps/e4m2.bsp");
	precache_file2 ("maps/e4m3.bsp");
	precache_file2 ("maps/e4m4.bsp");
	precache_file2 ("maps/e4m5.bsp");
	precache_file2 ("maps/e4m6.bsp");
	precache_file2 ("maps/e4m7.bsp");
	precache_file2 ("maps/e4m8.bsp");

	precache_file2 ("maps/end.bsp");

	precache_file2 ("maps/dm1.bsp");
	precache_file2 ("maps/dm2.bsp");
	precache_file2 ("maps/dm3.bsp");
	precache_file2 ("maps/dm4.bsp");
	precache_file2 ("maps/dm5.bsp");
	precache_file2 ("maps/dm6.bsp");
*/
}

#ifdef PR_COOP_MODE_ENHANCED
/*
=============
InsaneCoop_CheckDeadPlayers
=============
*/
void InsaneCoop_CheckDeadPlayers() {
	entity	ent;
	string	str;

	if ( coop != 3 || deathmatch || !num_players ) {
		self->nextthink = time + 5;
		return;
	}

	// check if there's any players alive
	ent = find( world, "classname", "player" );
	while ( ent  != world) {
		// we found a player that's alive, so stop here
		if ( ent->health > 0 ) {
			// if they're an observer then we need additional checks before we can stop
			if ( ent->playerclass == PR_PC_UNDEFINED ) {
				// ok he's not an observer from being dead
				if ( !ent->PR_is_dead_observer ) {
					self->nextthink = time + 5;
					return;
				}
			} else {
				self->nextthink = time + 5;
				return;
			}
		}

		ent = find( ent, "classname", "player" );
	}

	// at this point there's either no one in the game or they're all dead
	str = colstr( "Your team has been eliminated.\n", PR_COLSTR_RED );
	bprint( PR_PRINT_HIGH, str );
	BroadcastSound( "misc/runekey" );

	// give them time to read defeat
	self->think = execute_changelevel;
	self->nextthink = time + 5;
}
#endif

void spawn_think() {
	server_spawned = PR_TRUE;
	remove( self );
}


entity	lastspawn;

//=======================
/*QUAKED worldspawn (0 0 0) ?
Only used for the world entity.
Set message to the level name.
Set sounds to the cd track to play.

World Types:
0: medieval
1: metal
2: base
*/
// PZ NOTE: I'm pretty sure this function is called once on each map load.
//=======================
void worldspawn()
{
	entity	e;
	string	temps;
	float 	tempf;

	//frik_initializeFrikBotModule();  // FrikBot  // PZ: moved to StartFrame(). Needs to run after all map entities are created, like doors, teleporters, etc.

	// PZ: Initialize the votebot menu data on every map change.
	initVoteBotMenuData();

	// PZ: make sure to execute any <mapname>.cfg file in the cycledir
	// localcmd() doesn't immediately execute, so I made flushcmd().
	string cyc;
	cyc = infokey (world, "cycledir");
	if (cyc == string_null)
		localcmd("exec " TO_STR(PR_DEFAULT_CYCLEDIR) "/");
	else
	{
		localcmd("exec ");
		localcmd(cyc);
		localcmd("/");
	}
	localcmd(mapname);
	localcmd(".cfg\n");

#ifdef PR_COOP_MODE_ENHANCED
	// the coop and skill cvars are removed in QW, in NQ SV_SpawnServer() updated the prog's coop and deathmatch globals from the cvar and skill was updated in StartFrame()
	// so we'll emulate that by doing it here (deathmatch is in StartFrame())
	// Gizmo - I'd like to set coop based on if it's a coop map or not, but there's no telling when a monster or the tf detect entity will spawn
	// and we may need to know if it's coop before either of those spawn
	temps = infokey( world, "coop" );
	if ( temps == "" && deathmatch )	// if deathmatch is set and coop isn't defined then turn coop off
		coop = 0;
	else if ( temps == "" )			// if deathmatch is zero and coop isn't defined then turn coop on (you must explicitly set coop to 0 to play single-player)
		coop = 1;
	else {
		coop = stof( temps );

		// Gizmo - force deathmatch off if coop is present
		if ( coop ) {
			deathmatch = 0;
			cvar_set( "deathmatch", "0" );
		}
	}

	// restore previous coop and deathmatch
	temps = infokey( world, "_saved_coop" );
	if ( temps != "" ) {
		coop = stof( temps );
		localcmd( "localinfo _saved_coop \"\"\n" );
	}
	temps = infokey( world, "_saved_deathmatch" );
	if ( temps != "" ) {
		deathmatch = stof( temps );
		cvar_set( "deathmatch", temps );
		localcmd( "localinfo _saved_deathmatch \"\"\n" );
	}

	// Gizmo - check if we voted for coop or tf mode
	temps = infokey( world, "_voted_coop" );
	if ( temps != "" ) {
		// save off the previous stuff
		localcmd( "localinfo _saved_coop " );
		localcmd( ftos( coop ) );
		localcmd( "\n" );
		coop = stof( temps );
		localcmd( "localinfo _voted_coop \"\"\n" );
	}
	temps = infokey( world, "_voted_deathmatch" );
	if ( temps != "" ) {
		// save off the previous stuff
		localcmd( "localinfo _saved_deathmatch " );
		localcmd( ftos( deathmatch ) );
		localcmd( "\n" );
		deathmatch = stof( temps );
		cvar_set( "deathmatch", temps );
		localcmd( "localinfo _voted_deathmatch \"\"\n" );
	}

	// PZ: Force deathmatch off if coop is present. This was done above, for a certain condition. I'm not sure why it
	//     wasn't done for all instances of being in coop mode. If this doesn't go to 0, special damage calculations
	//     for coop aren't done. It results in not being able to kill zombies, because nothing does enough damage to
	//     gib them, even rockets. Set `deathmatch` to 0 and all of a sudden you can gib zombies.
	if (coop)
	{
		deathmatch = 0;
		cvar_set("deathmatch", "0");
	}

	temps = infokey( world, "skill" );
	if ( temps == "" )
		skill = 5;
	else
		skill = stof( temps );

	// Gizmo - if singleplayer or coop disable grapple by default
	if ( !deathmatch ) {
		if ( infokey( world, "g" ) == string_null )
			localcmd( "localinfo g off\n" );
	}

	// Gizmo - check for insane coop mode
	if ( coop == 3 && !deathmatch ) {
		e = spawnServerSide(); // PZ: make it a server-side only entity
		e->think = InsaneCoop_CheckDeadPlayers;
		e->nextthink = time + 10;
	}
#endif

	// Gizmo - create an entity that will let us know when everything's finished spawning
	e = spawnServerSide();      // PZ: make it a server-side only entity
	e->nextthink = time + 0.2;
	e->think = spawn_think;

	lastspawn = world;

// custom map attributes
	// Gizmo - allowed this to work without always forcing gravity to 800 when not on e1m8
	// this now works on coop and non-coop as it should
	if ( self->model == "maps/e1m8.bsp" ) {
		// save off the server's gravity setting
		tempf = cvar( "sv_gravity" );
		temps = ftos( tempf );
		localcmd( "localinfo _saved_gravity " );
		localcmd( temps );
		localcmd( "\n" );

		cvar_set( "sv_gravity", "100" );
	} else if ( infokey( world, "_saved_gravity" ) != string_null ) {
		// restore server's gravity setting if changing from e1m8
		temps = infokey( world, "_saved_gravity" );
		cvar_set( "sv_gravity", temps );
		localcmd( "localinfo _saved_gravity \"\"\n" );
	}

#ifdef PR_QUAKE_WORLD
/*	if (infokey(world, "*gamedir") != "fortress")
		objerror("QW TF must be run with a gamedir of \"fortress\".\n");*/
#endif

	//- OfteN globals -//
	// OfN - Init global vars
	num_players = 0;
	num_specs = 0;

	// Retrieve prozac main localinfos
	UpdateInfos(PR_TRUE);

#ifdef PR_COOP_MODE_ENHANCED
	// turn off grapple and those sort of things by default if nothing is already set if in singleplayer or coop
	if ( !deathmatch ) {
		if (infokey(world, "no_grapple") == string_null) {
			localcmd ("localinfo no_grapple 1\n");
			no_grapple = 1;
		}
		if (infokey(world, "allow_watermonsters") == string_null) {
			localcmd ("localinfo allow_watermonsters 1\n");
			allow_watermonsters = 1;
		}
	}
#endif

	// Init the body queue for corpses and heads of players
	InitBodyQue ();

	// Clear this local used for map cycle error reporting (see client.qc)
	localcmd("localinfo tmap \"\"\n");
	localcmd("localinfo specialmap \"\"\n");

	current_voteent = world;
	votesleeping = PR_FALSE;

	debug_target=world;
	already_chosen_map = PR_FALSE;
	triggered_cycle = PR_FALSE;

	ceasefire = PR_FALSE;
	////////////////////

	// We set up Chris' Teamplay Plus Mode here cos that's fun
	/*st = infokey(world, "chris");
	if (st == "on")
		chris = #TRUE;
	else
		chris = stof(st);

	st = infokey(world, "roundtime");
	roundtime = stof(st);
	if (!roundtime)
		roundtime = 300;

	roundstowin = 5;*/

	////////////////////////////////

	team_with_flag = 0; 	//KK initial global sets

	// PZ - I'll stick my global initializers here too, I guess. This is done on map change.
	playersOnTeam1 = 0; playersOnTeam2 = 0;
	playersOnTeam3 = 0; playersOnTeam4 = 0;
	agrTimerRunning = 0; agrTimeOfLastSound = 0;
	agrFlagRespawnWait = 0; agrTimeToRewardBlue = 0;

	//friends1_mask = stof(infokey(world,"friends1_mask"));
	//friends2_mask = stof(infokey(world,"friends2_mask"));
	//friends3_mask = stof(infokey(world,"friends3_mask"));
	//friends4_mask = stof(infokey(world,"friends4_mask"));

	// PZ: if we're in Invade mode, randomly pick a team to play defense
	if (invade)
	{
		if (random() < 0.5)
			invade_teamOnDef = 1; // blue
		else
			invade_teamOnDef = 2; // red
		invade_gameState = InvadeGameState::STOPPED;
		invade_timeOfLastSound = 0;
		invade_timeToRewardDef = 0;
	}

	// PZ: create timer to remind players to give feedback
	entity te;
	te = spawnServerSide(); // PZ: make it a server-side-only entity
	te->classname = "reminder_timer";
	te->nextthink = time + PR_REMINDER_RATE;
	te->think = Reminder;

// the area based ambient sounds MUST be the first precache_sounds

// player precaches
	W_Precache ();			// get weapon precaches

// sounds used from C physics code
	precache_sound ("demon/dland2.wav");		// landing thud
	precache_sound ("misc/h2ohit1.wav");		// landing splash

// setup precaches always needed
	precache_sound ("items/itembk2.wav");		// item respawn sound
	precache_sound ("player/plyrjmp8.wav"); 	// player jump
	precache_sound ("player/land.wav"); 		// player landing
	precache_sound ("player/land2.wav");		// player hurt landing
	precache_sound ("player/drown1.wav");		// drowning pain
	precache_sound ("player/drown2.wav");		// drowning pain
	precache_sound ("player/gasp1.wav");		// gasping for air
	precache_sound ("player/gasp2.wav");		// taking breath
	precache_sound ("player/h2odeath.wav"); 	// drowning death

	precache_sound ("misc/talk.wav");			// talk
	precache_sound ("player/teledth1.wav"); 	// telefrag
	precache_sound ("misc/r_tele1.wav");		// teleport sounds
	precache_sound ("misc/r_tele2.wav");
	precache_sound ("misc/r_tele3.wav");
	precache_sound ("misc/r_tele4.wav");
	precache_sound ("misc/r_tele5.wav");
	precache_sound ("weapons/lock4.wav");		// ammo pick up
	precache_sound ("weapons/pkup.wav");		// weapon up
	precache_sound ("items/armor1.wav");		// armor up
	precache_sound ("weapons/lhit.wav");		//lightning
	precache_sound ("weapons/lstart.wav");		//lightning start
	precache_sound ("items/damage3.wav");

	precache_sound ("misc/power.wav");			//lightning for boss

// player gib sounds
	precache_sound ("player/gib.wav");			// player gib sound
	precache_sound ("player/udeath.wav");		// player gib sound
	precache_sound ("player/tornoff2.wav"); 	// gib sound

// player pain sounds

	precache_sound ("player/pain1.wav");
	precache_sound ("player/pain2.wav");
	precache_sound ("player/pain3.wav");
	precache_sound ("player/pain4.wav");
	precache_sound ("player/pain5.wav");
	precache_sound ("player/pain6.wav");

// player death sounds
	precache_sound ("player/death1.wav");
	precache_sound ("player/death2.wav");
	precache_sound ("player/death3.wav");
	precache_sound ("player/death4.wav");
	precache_sound ("player/death5.wav");

// ax sounds
	precache_sound ("weapons/ax1.wav"); 		// ax swoosh
	precache_sound ("player/axhit1.wav");		// ax hit meat
	precache_sound ("player/axhit2.wav");		// ax hit world

	precache_sound ("player/h2ojump.wav");		// player jumping into water
	precache_sound ("player/slimbrn2.wav"); 	// player enter slime
	precache_sound ("player/inh2o.wav");		// player enter water
	precache_sound ("player/inlava.wav");		// player enter lava
	precache_sound ("misc/outwater.wav");		// leaving water sound

	precache_sound ("player/lburn1.wav");		// lava burn
	precache_sound ("player/lburn2.wav");		// lava burn

	precache_sound ("misc/water1.wav"); 		// swimming
	precache_sound ("misc/water2.wav"); 		// swimming

	precache_model ("progs/player.mdl");
	precache_model ("progs/eyes.mdl");
	precache_model ("progs/h_player.mdl");
	precache_model ("progs/gib1.mdl");
	precache_model ("progs/gib2.mdl");
	precache_model ("progs/gib3.mdl");

	precache_model ("progs/s_bubble.spr");	// drowning bubbles
	precache_model ("progs/s_explod.spr");	// sprite explosion
	precache_model ("progs/s_light.spr");

	precache_model ("progs/v_axe.mdl");
	precache_model ("progs/v_shot.mdl");
	precache_model ("progs/v_nail.mdl");
	precache_model ("progs/v_rock.mdl");
	precache_model ("progs/v_shot2.mdl");
	precache_model ("progs/v_nail2.mdl");
	precache_model ("progs/v_rock2.mdl");
	//UNUSED_TEST precache_model ("progs/grenade.mdl");

	//precache_model ("progs/v_chain.mdl"); // OfN - Light assault

	precache_model ("progs/bolt.mdl");		// for lightning gun
	precache_model ("progs/bolt2.mdl"); 	// for lightning gun
	precache_model ("progs/bolt3.mdl"); 	// for boss shock
	precache_model ("progs/lavaball.mdl");	// for testing

	precache_model ("progs/missile.mdl");
	precache_model ("progs/spike.mdl");
	precache_model ("progs/s_spike.mdl");

	precache_model ("progs/backpack.mdl");

	precache_model ("progs/zom_gib.mdl");
	precache_model ("progs/v_light.mdl");

	precache_model2 ("progs/laser.mdl");
	precache_sound2 ("enforcer/enfire.wav");
	precache_sound2 ("enforcer/enfstop.wav");
	precache_sound2 ("hknight/attack1.wav");

	//- OfN - Flare exp sound
	precache_sound2 ("items/flare1.wav");

	// Team Fortress Weapon MDLs
	//precache_model2("progs/sight.spr"); // OfN Moved to megatf source
//	precache_model2("progs/cross1.mdl");
	precache_model2("progs/v_medi.mdl");
	//precache_model2("progs/v_bio.mdl");
	precache_model2("progs/hgren2_orig.mdl");
	precache_model2("progs/biggren.mdl");

	//precache_model2("progs/lndmine.mdl");
	precache_model2("progs/flare.mdl");

	precache_model2("progs/v_srifle.mdl");
	precache_model2("progs/v_asscan.mdl");
	precache_model2("progs/detpack.mdl");
	precache_model2("progs/ammobox.mdl");
	precache_model2("progs/v_knife.mdl");	  // Spy's Knife
	precache_model2("progs/v_knife2.mdl");	  // Spy's Knife with Blood on it
	precache_model2("progs/v_span.mdl");	  // Engineer's Spanner
	precache_model2("progs/e_spike1.mdl");	  // Engineer's RailGun
	precache_model2("progs/e_spike2.mdl");	  // Engineer's RailGun
	precache_model2("progs/v_rail.mdl");	  // Engineer's RailGun

	//precache_model2("progs/turrgun.mdl");   // Sentry Gun

	//////////////////////////////////////////////////////
	precache_model2("progs/trrgn2.mdl");
	precache_sound2 ("weapons/sntr666.wav");
	precache_sound2 ("weapons/trr2lost.wav");

	//precache_model2("progs/turrbase.mdl");	  // Sentry Gun
	precache_model2("progs/trrbs2.mdl");	  // Sentry Gun

	//air-fist models and sounds--------//
	precache_model2("progs/v_airgun.mdl");
	precache_sound2("weapons/agfail.wav");
	precache_sound2("weapons/agwfail.wav");
	precache_sound2("weapons/agwater.wav");
	precache_sound2("weapons/agfire.wav");
	precache_model2("progs/s_ablast.spr");
	//----------------------------------//

	//laser cannon models and sounds---//
	precache_model2("progs/v_laserg.mdl");
	precache_model2("progs/lasrspik.mdl");
	precache_sound2("weapons/laserric.wav");
	precache_sound2("weapons/laserg.wav");
	//-----------------------------------//

	//field gen precaches-------------//
	Field_Precache();
	//---------------------------------//

	precache_sound("gibs/gibpick.wav");
	/*precache_sound("gibs/gibpick2.wav");
	precache_sound("gibs/gibpick3.wav");*/
	//precache_sound2("blob/hit1.wav"); // For psionic grens

	#ifdef PR_KICKABLE_HEADS
	precache_sound("zombie/z_miss.wav");
	#endif

	MenuPrecache();

	PrecacheRuneModels();

	//- OfN -
	precache_model2("progs/flarefly.mdl");
	precache_model2("progs/tesgib4.mdl");
	precache_model ("progs/caltrop.mdl");		// A big, bad, caltrop

	precache_sound2("weapons/gren_emp.wav");

	precache_model2("progs/t2gib1.mdl");		  // Sentry Gun Gib
	precache_model2("progs/t2gib2.mdl");		  // Sentry Gun Gib
	precache_model2("progs/t2gib3.mdl");		  // Sentry Gun Gib

	//- extra gib
	precache_model2("progs/t2gib4.mdl");		  // Sentry Gun Gib
///////////////////////////////////////

	precache_model2("progs/disp.mdl");		  // Tmp dispenser mdl
	precache_model2("progs/dgib1.mdl"); 	  // Dispenser Gib
	precache_model2("progs/dgib2.mdl"); 	  // Dispenser Gib
	precache_model2("progs/dgib3.mdl"); 	  // Dispenser Gib

	precache_model2("progs/grenade2.mdl");	  // New grenade
	precache_model2("progs/v_grap.mdl");	  // Grapple gun
	precache_model2("progs/hook.mdl");		  // Grapple hook
//#ifndef QUAKE_WORLD
	/*if (headless==1)
	   precache_model2("progs/headless.mdl");	  // Player with no head*/
//#endif hehe why??

	precache_model2("progs/sencer.mdl");

	precache_model2("progs/grunty.mdl");

	precache_model("progs/flame.mdl"); // PZ: torch
	precache_sound("ambience/fire1.wav"); // PZ: torch

	//- OfN
	Wiz_Precache();
	Fish_Precache();
	#ifndef PR_GREMLIN_NOTDONE // PZ - let's not let these occupy resources if grem is disabled
		Gremlin_Precache();
	#endif

	precache_sound2("weapons/detpack.wav");
	precache_sound2("sub/locked.wav");        // PZ: ZGG triggers detpack to explode sound
	precache_sound2("shaka/martyr.wav");

	precache_sound2("speech/saveme1.wav");	  // Saveme Sound
	precache_sound2("speech/saveme2.wav");	  // Saveme Sound

	//- OfN sounds for flares and holo
	//precache_sound2("effects/bodyhit1.wav");
	//precache_sound2("effects/bodyhit2.wav");

	//- OfN - Aura sounds
	precache_sound2("auras/aura1b.wav");
	precache_sound2("auras/aura2b.wav");
	precache_sound2("auras/aura3b.wav");
	precache_sound2("auras/aura4b.wav");

	//precache_sound2("weapons/pinpull.wav");
	//precache_sound2("weapons/throw.wav");

#ifdef PR_SPEECH
	precache_sound2("speech/scout.wav");
	precache_sound2("speech/sniper.wav");
	precache_sound2("speech/soldier.wav");
	precache_sound2("speech/demoman.wav");
	precache_sound2("speech/medic.wav");
	precache_sound2("speech/hvyweap.wav");
	precache_sound2("speech/pyro.wav");
	precache_sound2("speech/spy.wav");
	precache_sound2("speech/engineer.wav");

	precache_sound2("speech/demo_dp.wav");
#endif

	// PZ - AGR mode
	if (agr) AGR_precacheSounds();

	// PZ - Invade mode
	if (invade) Invade_precacheSounds();

	// PZ: for the Zero Gravity Gun
#ifndef PR_NO_ZEROGRAVGUN
	ZGG_precache();
#endif

//
// Setup light animation tables. 'a' is total darkness, 'z' is maxbright.
//

	// 0 normal
	//lightstyle(0, "f"); // default starting settin was "m" (half bright)
	//SetWorldBrightness(#ENVIRO_DARKNESS); // Done on InitEnvironment()

	// 1 FLICKER (first variety)
	lightstyle(1, "mmnmmommommnonmmonqnmmo");

	// 2 SLOW STRONG PULSE
	lightstyle(2, "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba");

	// 3 CANDLE (first variety)
	lightstyle(3, "mmmmmaaaaammmmmaaaaaabcdefgabcdefg");

	// 4 FAST STROBE
	lightstyle(4, "mamamamamama");

	// 5 GENTLE PULSE 1
	lightstyle(5,"jklmnopqrstuvwxyzyxwvutsrqponmlkj");

	// 6 FLICKER (second variety)
	lightstyle(6, "nmonqnmomnmomomno");

	// 7 CANDLE (second variety)
	lightstyle(7, "mmmaaaabcdefgmmmmaaaammmaamm");

	// 8 CANDLE (third variety)
	lightstyle(8, "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa");

	// 9 SLOW STROBE (fourth variety)
	lightstyle(9, "aaaaaaaazzzzzzzz");

	// 10 FLUORESCENT FLICKER
	lightstyle(10, "mmamammmmammamamaaamammma");

	// 11 SLOW PULSE NOT FADE TO BLACK
	lightstyle(11, "abcdefghijklmnopqrrqponmlkjihgfedcba");

	// styles 32-62 are assigned by the light program for switchable lights

	// 63 testing
	lightstyle(63, "a");

	// Init environment stuff, in environ.qc
	InitEnvironment();
}

void TeamFortress_TeamIncreaseScore(float tno, float scoretoadd, entity playerThatTriggeredScore);

float StartFrame_initialized; // PZ: need a static variable

// PZ NOTE: This is called every frame, by the C code.
void StartFrame()
{
	num_clients = getNumberOfTotalClients(); // PZ: added

	// this stuff runs one time at the start, either on the first or the second frame (need the 'initialized' flag, because framecount will eventually loop around)
	if (!StartFrame_initialized)
	{
		if      (framecount == 0) // run this stuff on the very first frame (first SV_Frame() execution)
		{

		}
		else if (framecount == 1) // run this stuff on the very second SV_Frame() execution
		{
			frik_initializeFrikBotModule(); // PZ: This needs to run after all map entities are created, like doors, teleporters, etc.

			StartFrame_initialized = PR_TRUE; // we are now done with initialization, at least for this function
		}
	}

	frik_mainRoutine(); // PZ: FrikBot's main routine
	teamplay = cvar("teamplay");
	timelimit = cvar("timelimit") * 60;
	fraglimit = cvar("fraglimit");
	deathmatch = cvar("deathmatch");

	// PZ: Attackers Go Red mode
	if (agr) AGR_think();
	
	// PZ: Invade mode
	if (invade) Invade_think();
	
	// PZ: Neo Mode
	if (neo.isModeActive()) 
	{
		neo.mainThink();
	}

	// PZ NOTE: I'd always make sure this frame incrementer stays the last statement in this function.
	framecount = framecount + 1;
}

/*
==============================================================================

BODY QUE

==============================================================================
*/

entity	bodyque_head;

void bodyque()
{	// just here so spawn functions don't complain after the world
	// creates bodyques
}

void InitBodyQue()
{
	//local entity	e; UNUSED

/*	OfN - Original code
	bodyque_head = spawn();
	bodyque_head.classname = "bodyque";
	bodyque_head.owner = spawn();
	bodyque_head.owner.classname = "bodyque";
	bodyque_head.owner.owner = spawn();
	bodyque_head.owner.owner.classname = "bodyque";
	bodyque_head.owner.owner.owner = spawn();
	bodyque_head.owner.owner.owner.classname = "bodyque";
	bodyque_head.owner.owner.owner.owner = bodyque_head;*/

	// OfN - Now the serious "pro" code :)
	entity te;

	float counter;
	counter = 1;

	te = spawn();
	te->ltime = 0;
	te->classname = "bodyque";
	te->modelindex = modelindex_null;
	bodyque_head = te;

	while (counter < bodyque_size) // i wouldnt recommend more than 16
	{
		te->owner = spawn();
		te->owner->ltime = 0;
		te->owner->classname = "bodyque";
		te->owner->modelindex = modelindex_null;

		te = te->owner;

		counter = counter + 1;
	}

	te->owner = bodyque_head;
}

// make a body que entry for the given ent so the ent can be
// respawned elsewhere
void CopyToBodyQue(entity ent)
{
	entity que_item;

	if (ent->modelindex == modelindex_null)
		return; // OfN Dont let invisible stuff on bodyque

	// Get an appropiate item on que
	que_item = GetQueEntry();

	que_item->angles = ent->angles;
	que_item->model = ent->model;
	que_item->skin = ent->skin;
	que_item->modelindex = ent->modelindex;
	que_item->frame = ent->frame;
	que_item->colormap = ent->colormap;
	que_item->movetype = ent->movetype;
	que_item->velocity = ent->velocity;

	// OfN
	que_item->takedamage = ent->takedamage;
	que_item->avelocity = ent->avelocity;
	if (ent->modelindex == modelindex_player) // stop any spin movement if a corpse
		que_item->avelocity = V({0, 0, 0});

	if (ent->flags & PR_FL_FINDABLE_NONSOLID)
		que_item->flags = PR_FL_FINDABLE_NONSOLID;
	else
		que_item->flags = 0; // <-- original was always this

	que_item->ltime = time;
	// OfN end

	setorigin (que_item, ent->origin);
	setsize (que_item, ent->mins, ent->maxs);

	// OfN
	que_item->touch = ent->touch;
	que_item->solid = ent->solid;
	que_item->team_no = ent->team_no;
	que_item->message = ent->message;

	#ifdef PR_KICKABLE_HEADS
	que_item->t_length = ent->t_length;
	que_item->t_width = ent->t_width;
	que_item->oldenemy = ent->oldenemy;
	#endif

	que_item->PR_corpse_HP = ent->PR_corpse_HP;
	// OfN end

	// Cycle-advance que pointer if current was used
	/*if (que_item == bodyque_head)
		bodyque_head = bodyque_head.owner;*/
}

entity GetQueEntry()
{
	float counter;
	entity te, oldest;
	counter = 0;
	te = oldest = bodyque_head;

	// Check all items in que
	while (counter < bodyque_size)
	{
		// Empty one?
		if (te->modelindex == modelindex_null)
			return te; // Ok, return it for use..

		// Check how old is this one
		if (te->ltime < oldest->ltime)
			oldest = te; // set it as the oldest found

		// Cycle-advance to continue checks
		te = te->owner;
		counter = counter + 1;
	}

	// No empty que items found, so return oldest one..
	return oldest;
}

} // END namespace Progs
