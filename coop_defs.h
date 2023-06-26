/*
	coop_defs.qc
*/

#ifndef PR_COOP_MODE_ENHANCED
//#pragma DONT_COMPILE_THIS_FILE
#else

#ifndef COOP_DEFS_H
#define COOP_DEFS_H

namespace Progs {

// misc monster stuff
#define PR_NPC_TRANQ_FACTOR			0.6				// how much monsters slow down when tranqed

// .spawnflags values
#define PR_SPAWN_USETARGETSONRESPAWN		4				// if present, monsters that respawn will re-fire their triggers upon death
#define PR_SPAWN_DONTCOUNT				8				// if present, monster doesn't count towards the total number of monsters or killed monsters
#define PR_SPAWN_RUN					16				// if present on a path_corner, any monster using that path_corner will run to the next path_corner; if present on a monster, the monster always runs

// alternate teams
#define PR_TEAMNO_MONSTERS				100				// team number for monsters

// monster weights, a monster can be inbetween these
#define PR_MWEIGHT_SMALL				80				// small monster
#define PR_MWEIGHT_NORMAL				160
#define PR_MWEIGHT_MEDIUM				290
#define PR_MWEIGHT_LARGE				500
#define PR_MWEIGHT_HUGE				1000

// monster flags
#define PR_MFLAG_HASWEAPON				1				// used for judo, determines if he can be stunned
#define PR_MFLAG_HUMANOID				2				// dogs and other similar monsters won't have this flag
#define PR_MFLAG_ALMOSTHUMANOID			4				// almost looks like a humanoid
#define PR_MFLAG_BITER				8				// the monster can bite (IE: dogs)
#define PR_MFLAG_NOHEART				16				// mainly for warlock heart grabbing
#define PR_MFLAG_NOBREATHING			32				// monster doesn't breathe, (IE: shambler, enforcer (has suit))
#define PR_MFLAG_NOLEGDAMAGE			64				// monster has legs but doesn't take leg damage (tarbabys)
#define PR_MFLAG_NOLEGS				128				// monster doesn't even have legs

// more attack states
#define PR_AS_PREDICT_MISSILE			10				// used to tell the monster to face the direction where he's going to shoot a predicted shot before he shoots
#define PR_AS_BLIND_MISSILE			11				// shooting randomly near enemy's last know location (if they entered water, player thiefed, or ducked behind cover)

// macros for certain variables so we can reuse unused variables and still give it a unique name
#define PR_npc_speedfactor				speed
#define PR_npc_noticedelay				invisible_finished
#define PR_npc_spawnfunc				think1			// safe to use?
//#define npc_spawndelay				walkframe			// was gravity
#define PR_npc_originalloc				pos1
#define PR_npc_attackoffset			pos2				// where the npc shoots from, IE: '0 0 40' for shams
#define PR_npc_enemyoffset				dest				// offset that our enemy is visible
#define PR_npc_inpain				demon_one			// if the monster is currently in pain
#define PR_npc_originalAngles			finalangle			// original angles when first spawned
#define PR_npc_lastEnemyOrigin			dest1				// last visible origin our enemy was spotted at
#define PR_npc_lastEnemyTime			lip				// time enemy was last spotted at npc_lastEnemyOrigin
#define PR_npc_isCoopMonster			nextpc			// used for determining a coop monster from a normal monster
#define PR_npc_useTargetsOnRespawn		StatusBarSize		// if true then monster should call SUB_UseTargets() again when he dies after respawning

// damage multipliers
#define PR_npc_bulletmult				money
#define PR_npc_spikemult				custom_speed
#define PR_npc_explosionmult			ff_count
#define PR_npc_electricitymult			hover_time
#define PR_npc_firemult				penance_time
#define PR_npc_meleemult				has_cheated
#define PR_npc_othermult				has_cheated			// FIXME: right now melee works by other, we should make all melee weapons use TF_TD_MELEE

// used for smooth movement of monsters, FIXME: change vars I just made up some crap to see if it compiles
FIELD(".void npc_oldthink();")

#define PR_npc_oldthink				npc_oldthink
#define PR_npc_smoothtime				maxspeed			// this is the time when we need to give back control to monster and stop lerping
#define PR_npc_smoothdist				demon_blood			// total distance we need to go
#define PR_npc_lastruntime				axhitme			// last time our lerp function was called, BUG: RECUSTOMIZE
#define PR_npc_attemptednextthink		cnt

// coop monster fields
FIELD(".float monsterweight;")
FIELD(".int monsterflags;")								// MFLAG_*** flags

// coop only player fields
#define PR_is_dead_observer			monsterweight

} // END namespace Progs

#endif // END COOP_DEFS_H

#endif // END PR_COOP_MODE_ENHANCED
