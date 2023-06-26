//some of this by Cyto 3-27-00
//other by CH

#include "progs.h"
#include "debug.h"

namespace Progs {

//Used for testing tolerence, prints out various stuff
//#define SHOW_CHEAT_LEVELS

//If defined any cheat cmd results in kicking
//#define ENABLE_CHEAT_CMDS

void Kick_My_Owner()
{
	string ip;
      string oldname,nospeed;
	float banned,off;
	banned = PR_FALSE;

	//Don't kick if cheats are off
	off = PR_FALSE;
	nospeed = infokey(world,"nospeed");
	if (nospeed != string_null) {
		off = stof(nospeed);
	}
	if (off) return;


	oldname = self->owner->netname;
	sprint(self->owner, PR_PRINT_HIGH,S_("\nYou have been ^bKICKED^b for ^bCHEATING^b.\nIf you were not cheating, I apologize.\nHave a nice day.\n"));

	ip = infokey(self->owner,"ip");
	if (infokey(world,"infip") == ip) { // second infraction. ban.
		localcmd("addip ");
		localcmd(ip);
		localcmd("\n");
		localcmd("localinfo infip \"\"");
		banned = PR_TRUE;
	} else {
		localcmd("localinfo infip ");
		localcmd(ip);
		localcmd("\n");
	}

	stuffcmd(self->owner,"disconnect\n");
	bprint(PR_PRINT_HIGH, oldname);
	if (banned)
		bprint(PR_PRINT_HIGH, " was BANNED for cheating!\n");
	else
		bprint(PR_PRINT_HIGH, " was kicked for cheating.\n");

	self->think = SUB_Null;
	dremove(self);
}

#define PR_MAXPERCENT 35 //WK this looks good for wall strafing
//reasons for not checking
#define PR_IMMUNE 	1
#define PR_GRAPPLE	2
#define PR_FLYING 	4
#define PR_NOCLASS 	16
#define PR_BUILDING 	32
#define PR_DEAD	64

void TeamFortress_CheckForSpeed()
{
	vector tempvec;
	float max, dist, check;
	int immune;
	//local string ip;
	string nospeed;
	float off;

	off = PR_FALSE;
	nospeed = infokey(world,"nospeed");
	if (nospeed != string_null) {
		off = stof(nospeed);
	}
	if (off) return;

	immune = 0;

max = self->owner->maxspeed;
if ((self->owner->job & PR_JOB_RUNNER) && (self->owner->job & PR_JOB_ACTIVE))
	max = max + 200;
//make sure ok to check
if (self->owner->immune_to_chec > time)
	immune = immune | PR_IMMUNE; //its ok since #IMMUNE becomes 1
if (self->owner->hook_out)
	immune = immune | PR_GRAPPLE;
//WK We should boot if they have no set speed and are moving
//if (self.owner.maxspeed == 0) // no set speed
if (!(self->owner->flags & PR_FL_ONGROUND)) //WK People in air immune
	immune = immune | PR_FLYING;
if (self->owner->playerclass == PR_PC_UNDEFINED)
	immune = immune | PR_NOCLASS;
if (self->owner->done_custom & PR_CUSTOM_BUILDING)
	immune = immune | PR_BUILDING;
if (self->owner->health <= 0)
	immune = immune | PR_DEAD;
if (!self->owner->is_connected)
	dremove(self);

if (immune)
	check = PR_FALSE;
else
	check = PR_TRUE;

	//WK Lowered percent chance of restuffing because stuffcmds() cause overflows
	if (!self->lip || !(self->has_camera) || (random() <= 0.1)){ //periodically reset ping and gotwalls.
		self->has_camera = 0;
		self->has_camera = stof(infokey(self->owner,"ping")) / 20;
#ifdef PR_ENABLE_CHEAT_CMDS
		if (!self->lip || (random() <= 0.1)){
			stuffcmd(self->owner,"alias aa_enabled \"impulse " TO_STR(PR_I_CHEAT_ONE) ";wait;impulse " TO_STR(PR_I_CHEAT_TWO) ";wait;impulse " TO_STR(PR_I_CHEAT_THREE) "\"\n");
			stuffcmd(self->owner,"alias +aa \"aa_enabled\"\n");
			stuffcmd(self->owner,"alias -aa \"aa_enabled\"\n");
			stuffcmd(self->owner,"alias aa_teamlock \"aa_enabled\"\n");
			stuffcmd(self->owner,"alias aa_on \"aa_enabled\"\n");
			stuffcmd(self->owner,"alias aa_off \"aa_enabled\"\n");
			stuffcmd(self->owner,"alias +showradar \"aa_enabled\"\n");
			stuffcmd(self->owner,"alias -showradar \"aa_enabled\"\n");
			stuffcmd(self->owner,"alias cl_speed \"aa_enabled\"\n");
			stuffcmd(self->owner,"alias cl_gotwalls \"aa_enabled\"\n");
			stuffcmd(self->owner,"alias fullbright \"aa_enabled\"\n");
			self->lip = 1;
		}
#endif
	}

	// Ignore z part of vector
	tempvec = self->oldorigin - self->owner->origin;
	tempvec[Z] = 0;

	dist = vlen(tempvec);

	if (check && (max == 0) && (dist > 100)) { //Definitely cheating...
		self->owner->cheat_level = self->owner->cheat_level + 700;
	}
	if (check && (dist > (max * (1 + (PR_MAXPERCENT / 100))))) // Maybe cheating.
		self->owner->cheat_level = self->owner->cheat_level + 200;
	if (self->owner->cheat_level >= 900 && self->has_sentry == 0 && dist > max) {
		bprint(PR_PRINT_HIGH, self->owner->netname);
		bprint(PR_PRINT_HIGH, " may be cheating. (Current:");
		bprint(PR_PRINT_HIGH, ftos(dist));
		bprint(PR_PRINT_HIGH, "/Max:");
            bprint(PR_PRINT_HIGH, ftos(max));
		bprint(PR_PRINT_HIGH, ")\n");
		self->has_sentry = 10;
	} else {
		self->has_sentry = self->has_sentry - 1;
		if (self->has_sentry <= 0)
			self->has_sentry = 0;
	}
	if (self->owner->cheat_level >= 1800) { //bye....
		self->owner->cheat_level = 0; //WK Clear so we don't boot twice
		self->think = Kick_My_Owner;
		self->nextthink = time + 0.1;
	}
#ifdef PR_SHOW_CHEAT_LEVELS
	string st;
	st = ftos(max);
	bprint(PR_PRINT_HIGH, "Max: ");
	bprint(PR_PRINT_HIGH, st);
	bprint(PR_PRINT_HIGH, " Imm: ");
	st = ftos(immune);
	bprint(PR_PRINT_HIGH, st);
	bprint(PR_PRINT_HIGH, " Cht: ");
	st = ftos(self->owner->cheat_level);
	bprint(PR_PRINT_HIGH, st);
	bprint(PR_PRINT_HIGH, " Speed: (");
	st = ftos(dist);
	bprint(PR_PRINT_HIGH, st);
	bprint(PR_PRINT_HIGH, "/");
	st = ftos(max * (1 + (PR_MAXPERCENT / 100)));
	bprint(PR_PRINT_HIGH, st);
	bprint(PR_PRINT_HIGH, ")\n");
#endif
	self->oldorigin = self->owner->origin;
	self->nextthink = time + 1;
}
//This checks so that the impulse cmds were not by accident
//One, must be called before two, and two before three
void I_DID_CHEAT_ONE()
{
//	bprint(#PRINT_HIGH, "I did the cheat 1 cmd!\n");
	if (self->has_cheated == 0)
		self->has_cheated = 1;
}
void I_DID_CHEAT_TWO()
{
//	bprint(#PRINT_HIGH, "I did the cheat 2 cmd!\n");
	if (self->has_cheated >= 1) {
		self->has_cheated = 2;

	} else {
		self->has_cheated = 0;
	}
}
void I_DID_CHEAT_THREE()
{
//	bprint(#PRINT_HIGH, "I did the cheat 3 cmd!\n");
	if (self->has_cheated == 2) {
		//bprint(#PRINT_HIGH, "Making kick entity!\n");
		entity te;

		te = spawnServerSide(); // PZ: make it a server-side only entity         //CH
		te->nextthink = time + (random() * 5) + 5; //5-10 seconds
		self->cheat_level = 0; //WK Clear so we don't boot twice
		self->has_cheated = 0;
		te->think = Kick_My_Owner;
		te->owner = self;
		te->classname = "kicktimer";
	} else {
		self->has_cheated = 0;
	}
}

} // END namespace Progs
