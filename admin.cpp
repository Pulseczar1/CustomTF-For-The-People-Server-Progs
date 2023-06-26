/*

	Functions handling TeamFortress Administration for Net Servers
	!NOTE! parm15 is used to pass admin status across maps
*/

#include "progs.h"
#include "debug.h"
#include "ofndefs.h"  // PZ: Needed for definition of PR_ADMIN_CMND_ALLOWED.
#include "custom.h"
#include "tforttm.h"
#include "gweapons.h"
#include "combat.h"
#include "neo.h"

namespace Progs {

void RemoveHolo(entity player);
float TeamFortress_TeamGetLives(float tno);
void SetTeamName(entity p);
void kill_my_demons();
void DetonateAllGunsForced();
float TeamFortress_DropItems();

void World_FadeIn(float delaytime, float fadetime, float factor);
void World_FadeOut(float delaytime, float fadetime, float factor);
void World_FadeMid(float delaytime, float fadetime, float factor);
entity FadeWorld(float brightness, float fadetime, float delaytime, float factor);
float ServerUpdating();
void NextLevel();
void BroadcastSound(const string& thesound);

void ArmyRatingJoin(entity player);
void ArmyRatingLeave(entity player);

string GetClientDescription(entity client);
void PlayerFinal(entity player, const string& st, float action, float predefined);

string GetTrueTeamName(float tno);

float HasValidAdminTarget(entity theAdmin)
{
	if (theAdmin->admin_kick->classname != "player" && theAdmin->admin_kick->classname != "spec") {
	//if (theAdmin.admin_kick.classname != "player") {
		sprint(theAdmin, PR_PRINT_HIGH, "No user selected!\n");
		theAdmin->admin_kick = world;
		return PR_FALSE;
	}
	if (!theAdmin->admin_kick->is_connected) {
		sprint(theAdmin, PR_PRINT_HIGH, "User has disconnected!\n");
		theAdmin->admin_kick = world;
		return PR_FALSE;
	}

	return PR_TRUE;
}

//=====================================================================
// same thing but doesn't display messages, used in vote.qc

float HasValidAdminTarget2(entity theAdmin)
{
	if (theAdmin->admin_kick->classname != "player" && theAdmin->admin_kick->classname != "spec") {
		theAdmin->admin_kick = world;
		return PR_FALSE;
	}
	if (!theAdmin->admin_kick->is_connected) {
		theAdmin->admin_kick = world;
		return PR_FALSE;
	}

	return PR_TRUE;
}

float ValidClientState(entity player)
{
	if (player->playerclass == PR_PC_UNDEFINED)
	{
		sprint(self,PR_PRINT_HIGH,"Player ");
		sprint(self,PR_PRINT_HIGH,self->admin_kick->netname);
		sprint(self,PR_PRINT_HIGH," is observing the game\n");

		return PR_FALSE;
	}
	if (player->done_custom & PR_CUSTOM_BUILDING)
	{
		sprint(self,PR_PRINT_HIGH,"Player ");
		sprint(self,PR_PRINT_HIGH,self->admin_kick->netname);
		sprint(self,PR_PRINT_HIGH," is still customizing\n");

		return PR_FALSE;
	}

	return PR_TRUE;
}

void Admin_Kick_Cycle()
{
	entity te;
	float num;
	string st;

	num = PR_FALSE;

	te = find(self->admin_kick, "classname", "player");
	while (te != world && num == PR_FALSE)
	{
		num = PR_TRUE;

		if (!te->is_connected)
			num = PR_FALSE;

		if (self->admin_kick==te)
			num = PR_FALSE;

		if (num == PR_FALSE) te = find(te, "classname", "player");
	}

	if (te == world) // if out of players on our search lets scan for spectators
	{
		te = find(self->admin_kick, "classname", "spec");
		while (te != world && num == PR_FALSE)
		{
			num = PR_TRUE;

			if (!te->is_connected)
				num = PR_FALSE;

			if (self->admin_kick==te)
				num = PR_FALSE;

			if (num == PR_FALSE) te = find(te, "classname", "spec");
		}
	}

	if (te == world)
	{
		sprint(self, PR_PRINT_HIGH, S_("No Clients Found! ^b(^bend of list^b)^b\n"));
		self->admin_kick=world;
	}
	else
	{
		self->admin_kick = te; //Set current selected person
		//sprint(self, #PRINT_HIGH, "You can type ���� or ��� to throw them out. Type ��� again to select someone else.");

		if (self->admin_kick->classname == "spec")
			sprint(self, PR_PRINT_HIGH, S_("^bClient (^bSpectator^b)^b: "));
		else
			sprint(self, PR_PRINT_HIGH, S_("^bClient^b: "));

		sprint(self, PR_PRINT_HIGH, self->admin_kick->netname);
		sprint(self, PR_PRINT_HIGH, S_(" selected ^b(^b"));

		st = ftos(getuid(self->admin_kick));
		st = colstr(st,PR_COLSTR_NUMBER);

		sprint(self, PR_PRINT_HIGH, S_("^bUserID:^b"),st,S_(" ^bIP:^b"));
		st = infokey(self->admin_kick,"ip");
		st = colstr(st,PR_COLSTR_NUMBER);
		sprint(self,PR_PRINT_HIGH,st,S_("^b)^b\n"));
	}
}
void Admin_Kick_Person()
{
	if (self->admin_kick != world) //Bad
	{
		if (!HasValidAdminTarget(self))
			return;

		RPrint(self->netname);
		RPrint(" kicks ");
		RPrint(self->admin_kick->netname);
		RPrint("\n");

		bprint(PR_PRINT_HIGH, self->admin_kick->netname);
		bprint(PR_PRINT_HIGH, S_(" has been ^bKICKED^b by the admin "));
		bprint(PR_PRINT_HIGH, self->netname);
		bprint(PR_PRINT_HIGH, "\n");

		sprint(self->admin_kick,PR_PRINT_HIGH,S_("\nYou have been ^bKICKED^b from the server!\n"));

		stuffcmd(self->admin_kick, "disconnect\n"); //Kick them!

		BroadcastSound("player/teledth1");

		self->admin_kick = world; //Clear it //WK BUG! Used to be ==
	}
	else
		sprint(self, PR_PRINT_HIGH, "No target client selected!\n");
}
//WK Same code as Bloggy's, but with some happy code for banning
void Admin_Ban_Person()
{
	string foo;

	if (self->admin_kick != world) //Bad
	{
		//WK Add checks so that it doesn't crash the server!
		if (!HasValidAdminTarget(self))
			return;

		RPrint(self->netname);
		RPrint(" bans ");
		RPrint(self->admin_kick->netname);
		RPrint("\n");

		bprint(PR_PRINT_HIGH, self->admin_kick->netname);
		bprint(PR_PRINT_HIGH, S_(" has been ^bBANNED^b by the admin "));
		bprint(PR_PRINT_HIGH, self->netname);
		bprint(PR_PRINT_HIGH, "\n");

		sprint(self->admin_kick,PR_PRINT_HIGH,S_("\nYou have been ^bBANNED^b from the server!\n"));

		foo = infokey(self->admin_kick,"ip");
		localcmd("addip ");
		localcmd(foo);
		localcmd("\n");

		stuffcmd(self->admin_kick, "disconnect\n"); //Kick them!

		BroadcastSound("player/teledth1");

		self->admin_kick = world; //Clear it //WK BUG! Used to be ==
	}
	else
		sprint(self, PR_PRINT_HIGH, "No target client selected!\n");
}

void Admin_Curse()
{
	if (self->admin_kick != world) //Bad
	{
		//WK Add checks so that it doesn't crash the server!
		if (!HasValidAdminTarget(self))
			return;

		if (self->admin_kick->classname != "player")
		{
			sprint(self,PR_PRINT_HIGH,"You can only curse players!\n");
			return;
		}

		if (!ValidClientState(self->admin_kick))
			return;

		RPrint(self->netname);
		RPrint(" curses the player ");
		RPrint(self->admin_kick->netname);
		RPrint("\n");

		bprint(PR_PRINT_HIGH, self->admin_kick->netname);
		bprint(PR_PRINT_HIGH, S_(" has been ^bCURSED^b by the admin "));
		bprint(PR_PRINT_HIGH, self->netname);
		bprint(PR_PRINT_HIGH, "\n");

		float tf;
		string st2;

		tf = 0;
		st2 = infokey(world, "curse");

		if (st2 == string_null)
			tf = stof(st2);

		createBastard(self->admin_kick,tf);
	}
	else
		sprint(self, PR_PRINT_HIGH, "No target client selected!\n");
}

// PZ: Added the "changeteam" command. It will piggyback on this function, which used to only be invoked by "cmnd admin team".
//     'votebot' menu is also using this function to move players and bots from team to team.
//     `self` = the admin; `self->admin_kick` = player to assign; PZ: `silent` keeps it from printing messages.
void Admin_Assign(float targetteam, bool usedChangeTeamCommand, bool silent /*= false*/)
{
	string st;
	float tc;

	// In Neo Mode, we want team assignments to be done silently.
	if (neo.isModeActive()) silent = true;

	if (self->admin_kick == world) // PZ: made this condition simpler to avoid having everything indented for this condition
	{
		sprint(self, PR_PRINT_HIGH, "No target client selected!\n");
		return;
	}

	// PZ: if player used "changeteam", we've already done these checks
	if (!usedChangeTeamCommand)
	{
		//WK Add checks so that it doesn't crash the server!
		if (!HasValidAdminTarget(self))
			return;

		targetteam = floor(targetteam);

		if (targetteam == 3)
		{
			if (number_of_teams < 3)
			{
				sprint(self,PR_PRINT_HIGH,"No team 3 on this map!\n");
				return;
			}

			targetteam = 3;
		}
		else if (targetteam == 4)
		{
			if (number_of_teams < 4)
			{
				sprint(self,PR_PRINT_HIGH,"No team 4 on this map!\n");
				return;
			}

			targetteam = 4;
		}
		else if (targetteam < 1 || targetteam > 4)
		{
			sprint(self,PR_PRINT_HIGH,"Invalid team number!\n");
			return;
		}

		if (self->admin_kick->team_no == targetteam)
		{
			st = ftos(targetteam);
			sprint(self,PR_PRINT_HIGH,"Player ");
			sprint(self,PR_PRINT_HIGH,self->admin_kick->netname);
			sprint(self,PR_PRINT_HIGH," is already on team ");
			sprint(self,PR_PRINT_HIGH,st);
			sprint(self,PR_PRINT_HIGH,"!\n");
			return;
		}

		// if our target is observer or he's building a class, return
		if (!ValidClientState(self->admin_kick))
			return;

		if (!silent)
		{
			bprint(PR_PRINT_HIGH,"The admin ",self->netname," performs team adjustment:\n");

			st = ftos(targetteam);
			RPrint(self->netname);
			RPrint(" puts the player ");
			RPrint(self->admin_kick->netname);
			RPrint(" on team ");
			RPrint(st);
			RPrint("\n");
		}
	}

	ArmyRatingLeave(self->admin_kick);

	entity oself;
	if (!neo.isModeActive())
	{
		oself = self;

		self = self->admin_kick;

		// Detonate and kill all our stuff
		if (self->has_holo > 0) RemoveHolo(self);
		kill_my_demons();
		DetonateMines(self);
		DetonateAllGunsForced();
		TeamFortress_DropItems();
		self = oself;
	}

	if (!usedChangeTeamCommand) // PZ: already did this stuff in calling TeamFortress_TeamSet(), for "changeteam"
	{
		// Set the player's color
		// PZ: We are no longer using stuffcmd() to set team, color, or skin. Players will no longer be able to change these things,
		//     themselves, to keep from having to kick them when they do.
		//stuffcmd(self.admin_kick, "color ");
		tc = TeamFortress_TeamGetColor(targetteam) - 1;
		st = ftos(tc);
		tc = self->admin_kick->team_no;
		// Nice colors
		if (nicecolors==1) st =TeamGetNiceColor(targetteam);
		// PZ WARNING! Everywhere that I used setinfo() with TeamGetNiceColor() is wrong because TeamGetNiceColor() returns two colors,
		// not one. This is okay, though, because 'nicecolors' isn't being used. It's turned on with a serverinfo or localinfo variable
		// 'nicecolors', being set to '1' or 'on'. And I don't have it on.
		setinfo(self->admin_kick, "topcolor", st);    // PZ
		setinfo(self->admin_kick, "bottomcolor", st); // PZ
		//stuffcmd(self.admin_kick, st);
		//stuffcmd(self.admin_kick, "\n");

		self->admin_kick->team_no = targetteam;
		makeImmune(self->admin_kick,time+15);

		self->admin_kick->lives = TeamFortress_TeamGetLives(targetteam);

		SetTeamName(self->admin_kick);

		if (!silent) bprint(PR_PRINT_HIGH,"Player ",self->admin_kick->netname," is assigned to team ");
		// PZ - modified for AGR.. changes playersOnTeam#
		if (targetteam == 1)
		{
			playersOnTeam1 = playersOnTeam1 + 1;
			if (!silent) bprint(PR_PRINT_HIGH,"1");
		}
		else if (targetteam == 2)
		{
			playersOnTeam2 = playersOnTeam2 + 1;
			if (!silent) bprint(PR_PRINT_HIGH,"2");
		}
		else if (targetteam == 3)
		{
			playersOnTeam3 = playersOnTeam3 + 1;
			if (!silent) bprint(PR_PRINT_HIGH,"3");
		}
		else
		{
			playersOnTeam4 = playersOnTeam4 + 1;
			if (!silent) bprint(PR_PRINT_HIGH,"4");
		}
		if      (tc == 1) playersOnTeam1 = playersOnTeam1 - 1; // tc is old team number
		else if (tc == 2) playersOnTeam2 = playersOnTeam2 - 1;
		else if (tc == 3) playersOnTeam3 = playersOnTeam3 - 1;
		else              playersOnTeam4 = playersOnTeam4 - 1;
		// PZ - end AGR mod
		if (!silent)
		{
			st = GetTrueTeamName(targetteam);
			bprint(PR_PRINT_HIGH, " (", st, " <= ");
			st = GetTrueTeamName(tc);
			bprint(PR_PRINT_HIGH, st,")\n");
		}
	}

	ArmyRatingJoin(self->admin_kick);

	if (!usedChangeTeamCommand && !neo.isModeActive()) // Neo Mode: Don't make them respawn.
	{
		// Make him/her respawn away
		oself = self;
		self = self->admin_kick;
		PutClientInServer();
		self = oself;
	}
}

//======================================================================
// The admin wants to type on a client console

void Admin_Cmd(const string& cmd)
{
	if (self->admin_kick != world) //Bad
	{
		if (!HasValidAdminTarget(self))
			return;

		#ifdef PR_ADMIN_CMND_ALLOWED
		stuffcmd(self->admin_kick,cmd); // execute command
		stuffcmd(self->admin_kick,"\n");
		sprint(self,PR_PRINT_HIGH, "Command \"");
		sprint(self,PR_PRINT_HIGH, cmd);
		sprint(self,PR_PRINT_HIGH, "\" is executed for ");
		sprint(self, PR_PRINT_HIGH, self->admin_kick->netname);
		sprint(self, PR_PRINT_HIGH, "\n");
		// PZ: something this powerful needs transparency
		bprint(PR_PRINT_HIGH, "Admin, ");
		bprint(PR_PRINT_HIGH, self->netname);
		bprint(PR_PRINT_HIGH, ", executes the following command on ");
		bprint(PR_PRINT_HIGH, self->admin_kick->netname);
		bprint(PR_PRINT_HIGH, "'s client: \"");
		bprint(PR_PRINT_HIGH, cmd);
		bprint(PR_PRINT_HIGH, "\"\n");
		BroadcastSound("doors/runeuse");
		#else
		sprint(self,PR_PRINT_HIGH, "Use of CMND's is not allowed.\n");
		#endif
	}
	else
		sprint(self, PR_PRINT_HIGH, "No target client selected!\n");
}

void Admin_Cuff(const string& arg)
{
	if (self->admin_kick != world) //Bad
	{
		if (!HasValidAdminTarget(self))
			return;

		string tmps;
		float tmpf,tmpf2;

		tmpf = getuid(self->admin_kick);

		if (tmpf) // We got a valid user ID?
		{
			tmps = ftos(tmpf);
			tmpf2 = stof(arg); // Get the argument as float

			if (tmpf2 != 0) // Are we cuffing? or uncuffing?
			{
				RPrint(self->netname);
				RPrint(" cuffs the client ");
				RPrint(self->admin_kick->netname);
				RPrint("\n");

				bprint(PR_PRINT_HIGH,self->admin_kick->netname);
				bprint(PR_PRINT_HIGH,S_(" has been ^bCUFFED^b by the admin "));
				bprint(PR_PRINT_HIGH,self->netname);
				bprint(PR_PRINT_HIGH,"\n");
			}
			else
			{
				RPrint(self->netname);
				RPrint(" uncuffs ");
				RPrint(self->admin_kick->netname);
				RPrint("\n");

				bprint(PR_PRINT_HIGH,"The admin ");
				bprint(PR_PRINT_HIGH,self->netname);
				bprint(PR_PRINT_HIGH," uncuffs ");
				bprint(PR_PRINT_HIGH,self->admin_kick->netname);
				bprint(PR_PRINT_HIGH,"\n");
			}

			// Perform the command
			localcmd("cuff ");
			localcmd(tmps);
			localcmd(" ");
			localcmd(arg);
			localcmd("\n");
		}
		else
			sprint(self,PR_PRINT_HIGH,"ERROR: Can't get the user ID!\n");

	}
	else
		sprint(self, PR_PRINT_HIGH, "No target client selected!\n");
}

void Admin_Mute(const string& arg)
{
	if (self->admin_kick != world) //Bad
	{
		if (!HasValidAdminTarget(self))
			return;

		string tmps;
		float tmpf,tmpf2;

		tmpf = getuid(self->admin_kick);

		if (tmpf) // We got a valid user ID?
		{
			tmps = ftos(tmpf);
			tmpf2 = stof(arg); // Get the argument as float

			if (tmpf2 != 0) // Are we muting? or unmuting?
			{
				RPrint(self->netname);
				RPrint(" mutes the client ");
				RPrint(self->admin_kick->netname);
				RPrint("\n");

				bprint(PR_PRINT_HIGH,self->admin_kick->netname);
				bprint(PR_PRINT_HIGH,S_(" has been ^bMUTED^b by the admin "));
				bprint(PR_PRINT_HIGH,self->netname);
				bprint(PR_PRINT_HIGH,"\n");
			}
			else
			{
				RPrint(self->netname);
				RPrint(" allows ");
				RPrint(self->admin_kick->netname);
				RPrint(" to speak\n");

				bprint(PR_PRINT_HIGH,"The admin ");
				bprint(PR_PRINT_HIGH,self->netname);
				bprint(PR_PRINT_HIGH," allows ");
				bprint(PR_PRINT_HIGH,self->admin_kick->netname);
				bprint(PR_PRINT_HIGH," to speak\n");
			}

			// Perform the command
			localcmd("mute ");
			localcmd(tmps);
			localcmd(" ");
			localcmd(arg);
			localcmd("\n");
		}
		else
			sprint(self,PR_PRINT_HIGH,"ERROR: Can't get the user ID!\n");

	}
	else
		sprint(self, PR_PRINT_HIGH, "No target client selected!\n");
}


void Admin_Call_Ceasefire()
{
	if (intermission_running) return;

	if (ceasefire)
	{
		ceasefire = PR_FALSE;
		bprint(PR_PRINT_HIGH, "The game resumes now.\n");

		RPrint(self->netname);
		RPrint(" ends the ceasefire\n");

		if (num_players)
			World_FadeIn(0,0,0);
		else if (num_specs)
			World_FadeMid(0,0,0);
		else World_FadeOut(0,0,0);
	}
	else
	{
		ceasefire = PR_TRUE;
		bprint(PR_PRINT_HIGH, self->netname);
		bprint(PR_PRINT_HIGH, " forced a ceasefire.\n");

		RPrint(self->netname);
		RPrint(" forces a ceasefire\n");

		World_FadeMid(0,0,0);
	}
}

void Admin_KillStuff()
{
	if (self->admin_kick != world) //Bad
	{
		if (!HasValidAdminTarget(self))
			return;

		entity oself;
		oself = self;
		self = self->admin_kick;

		// Detonate and kill all his/her stuff
		if (self->has_holo > 0 ) RemoveHolo(self);
		kill_my_demons();
		DetonateMines(self);
		DetonateAllGunsForced();
		TeamFortress_DropItems();
		self = oself;

		bprint(PR_PRINT_HIGH,"The admin ");
		bprint(PR_PRINT_HIGH,self->netname);
		bprint(PR_PRINT_HIGH," detonates all the stuff belonging to ");
		bprint(PR_PRINT_HIGH,self->admin_kick->netname);
		bprint(PR_PRINT_HIGH,"\n");
	}
	else
		sprint(self, PR_PRINT_HIGH, "No target client selected!\n");
}

//contains the list of impulses that can be used during ceasefire
//returns TRUE if its good
float GoodCeasefireImpulse(float inp)
{
	if (inp >= 0 && inp <= 10) // Allow them for menus
		return PR_TRUE;

	if (inp == PR_TF_MEDIC_HELPME ||
	inp == PR_TF_TAUNT ||
	inp == PR_TF_TAUNT2 ||
	inp == PR_TF_TAUNT3 ||
	inp == PR_TF_TAUNT4 ||
	inp == PR_TF_TAUNT5 ||
	inp == PR_TF_STATUS_QUERY ||
	inp == PR_TF_DISPLAYLOCATION   ||
	inp == PR_TF_STATUS_QUERY ||
	inp == PR_TF_HELP_MAP ||
	inp == PR_TF_INVENTORY ||
	inp == PR_TF_SHOWTF ||
	inp == PR_FLAG_INFO ||
	inp == PR_I_CHEAT_ONE ||
	inp == PR_I_CHEAT_TWO ||
	inp == PR_I_CHEAT_THREE ||
	inp == PR_IMPULSE_PUNISH ||
	inp == PR_IMPULSE_VOTEMAP ||
	inp == PR_IMPULSE_VOTEYES ||
	inp == PR_IMPULSE_VOTENO ||
	inp == PR_IMPULSE_TESTRANGE) //- OfN -
		return PR_TRUE;

	return PR_FALSE;

}

//contains the list of impulses that can be used during ceasefire
//returns TRUE if its good

/* OfN UNUSED
float(float inp) Good_Impulse_OnMenu =
{
	if (inp == #TF_MEDIC_HELPME ||
	inp == #TF_TAUNT ||
	inp == #TF_TAUNT2 ||
	inp == #TF_TAUNT3 ||
	inp == #TF_TAUNT4 ||
	inp == #TF_TAUNT5 ||*//*
	inp == #TF_STATUS_QUERY ||
	inp == #TF_DISPLAYLOCATION   ||
	inp == #TF_STATUS_QUERY ||
	inp == #TF_HELP_MAP ||
	inp == #TF_INVENTORY ||
	inp == #TF_SHOWTF ||
	inp == #FLAG_INFO ||
	inp == #I_CHEAT_ONE ||
	inp == #I_CHEAT_TWO ||
	inp == #I_CHEAT_THREE ||
	inp == #IMPULSE_PUNISH ||
	inp == #IMPULSE_VOTEMAP ||
	inp == #IMPULSE_VOTEYES ||
	inp == #IMPULSE_VOTENO) //- OfN -
		return #TRUE;

	return #FALSE;

};*/

//=================================================================================
// Prints a list of current admins, if param is TRUE print a "none found" message

void PrintAdmins(float none, float isadmin)
{
	float counter, tmpf;
	entity te;
	string tmps;

	counter = 0;

	te = find(world, "classname", "player");
	while (te != world)
	{
		if (te->admin_flag)
		if (te->is_connected)
		{
			if (!counter)
				sprint(self,PR_PRINT_HIGH,S_("Current admins logged on server^b:^b\n"));

			counter = counter + 1;

			tmps = ftos(counter);
			tmps = colstr(tmps,PR_COLSTR_NUMBER);

			sprint(self,PR_PRINT_HIGH,tmps);
			sprint(self,PR_PRINT_HIGH,S_("^b:^b "));
			sprint(self,PR_PRINT_HIGH,te->netname);

			if (isadmin || self->admin_flag)
			{
				tmps = ftos(getuid(te));
				tmps = colstr(tmps,PR_COLSTR_NUMBER);

				sprint(self, PR_PRINT_HIGH, S_(" ^b(UserID:^b"),tmps,S_(" ^bIP:^b"));
				tmps = infokey(te,"ip");
				tmps = colstr(tmps,PR_COLSTR_NUMBER);
				sprint(self,PR_PRINT_HIGH,tmps,S_("^b)^b\n"));
			}
			else
			{
				sprint(self,PR_PRINT_HIGH,S_(" ^b(^bUserID^b:^b"));

				tmpf = getuid(te);
				tmps = ftos(tmpf);
				sprint(self,PR_PRINT_HIGH,tmps,S_("^b)^b\n"));
			}
		}

		te = find(te, "classname", "player");
	}

	te = find(world, "classname", "spec");
	while (te != world)
	{
		if (te->admin_flag)
		if (te->is_connected)
		{
			if (!counter)
				sprint(self,PR_PRINT_HIGH,S_("Current admins logged on server^b:^b\n"));

			counter = counter + 1;

			tmps = ftos(counter);
			tmps = colstr(tmps,PR_COLSTR_NUMBER);

			sprint(self,PR_PRINT_HIGH,tmps);
			sprint(self,PR_PRINT_HIGH,S_("^b:^b "));
			sprint(self,PR_PRINT_HIGH,te->netname);
			sprint(self,PR_PRINT_HIGH,S_(" ^b(^bspectator^b)^b"));

			if (isadmin || self->admin_flag)
			{
				tmps = ftos(getuid(te));
				tmps = colstr(tmps,PR_COLSTR_NUMBER);

				sprint(self, PR_PRINT_HIGH, S_(" ^b(UserID:^b"),tmps,S_(" ^bIP:^b"));
				tmps = infokey(te,"ip");
				tmps = colstr(tmps,PR_COLSTR_NUMBER);
				sprint(self,PR_PRINT_HIGH,tmps,S_("^b)^b\n"));
			}
			else
			{
				sprint(self,PR_PRINT_HIGH,S_(" ^b(^bUserID^b:^b"));
				tmpf = getuid(te);
				tmps = ftos(tmpf);
				sprint(self,PR_PRINT_HIGH,tmps,S_("^b)^b\n"));
			}
		}

		te = find(te, "classname", "spec");
	}

	if (!counter && none)
		sprint(self,PR_PRINT_HIGH,"No admins currently logged on server.\n");
}

//========================================================================
// Prints given text to all admins

void NotifyAdmins(const string& text)
{
	entity te;

	te = find(world, "classname", "player");
	while (te != world)
	{
		if (te->admin_flag)
		if (te->is_connected)
			sprint(te,PR_PRINT_HIGH,text);

		te = find(te, "classname", "player");
	}

	te = find(world, "classname", "spec");
	while (te != world)
	{
		if (te->admin_flag)
		if (te->is_connected)
			sprint(te,PR_PRINT_HIGH,text);

		te = find(te, "classname", "spec");
	}
}

void PrintToAdmins(const string& s1, const string& s2, const string& s3, const string& s4, const string& s5, const string& s6, const string& s7, const string& s8)
{
	entity te;

	te = find(world, "classname", "player");
	while (te != world)
	{
		if (te->admin_flag)
		if (te->is_connected)
		{
			sprint(te,PR_PRINT_HIGH,s1);
			sprint(te,PR_PRINT_HIGH,s2);
			sprint(te,PR_PRINT_HIGH,s3);
			sprint(te,PR_PRINT_HIGH,s4);
			sprint(te,PR_PRINT_HIGH,s5);
			sprint(te,PR_PRINT_HIGH,s6);
			sprint(te,PR_PRINT_HIGH,s7);
			sprint(te,PR_PRINT_HIGH,s8);
		}

		te = find(te, "classname", "player");
	}

	te = find(world, "classname", "spec");
	while (te != world)
	{
		if (te->admin_flag)
		if (te->is_connected)
		{
			sprint(te,PR_PRINT_HIGH,s1);
			sprint(te,PR_PRINT_HIGH,s2);
			sprint(te,PR_PRINT_HIGH,s3);
			sprint(te,PR_PRINT_HIGH,s4);
			sprint(te,PR_PRINT_HIGH,s5);
			sprint(te,PR_PRINT_HIGH,s6);
			sprint(te,PR_PRINT_HIGH,s7);
			sprint(te,PR_PRINT_HIGH,s8);
		}

		te = find(te, "classname", "spec");
	}
}

//===============================================================
// Admin logged in event

void AdminLoggedIn(entity admin)
{
	string text;

	text = strcat("User ",admin->netname);
	text = strcat(text," logged in as admin.\n");

	RPrint(text);
	NotifyAdmins(text);
}

//===============================================================
// Admin logged out event

void AdminLoggedOut(entity admin)
{
	string text;

	text = strcat("User ",admin->netname);
	text = strcat(text," logged out as admin.\n");

	RPrint(text);
	NotifyAdmins(text);
}

//================================================================
// Tells a message from an user to all admins logged in

float Admin_Tell(entity user, const string& str)
{
	float numadmins;
	entity admin;
	string st;

	if (mutedtime(user))
	{
		sprint(user,PR_PRINT_HIGH,"You are muted!\n");
		return 99;
	}

	st = colstr(str,PR_COLSTR_RED);

	numadmins = 0;

	admin = find(world,"classname","player");

	while (admin != world)
	{
		if (admin->admin_flag)
		if (admin->is_connected)
		{
			numadmins = numadmins + 1;
			sprint(admin,PR_PRINT_HIGH,"To Admins\x8D (",user->netname,"): ",st,"\n");
			stuffcmd(admin,"play misc/talk\n");
		}

		admin = find(admin,"classname","player");
	}

	admin = find(world,"classname","spec");

	while (admin != world)
	{
		if (admin->admin_flag)
		if (admin->is_connected)
		{
			numadmins = numadmins + 1;
			sprint(admin,PR_PRINT_HIGH,"To Admins\x8D (",user->netname,"): ",st,"\n");
			stuffcmd(admin,"play misc/talk\n");
		}

		admin = find(admin,"classname","spec");
	}

	putsaytime(user);

	if (!user->admin_flag && numadmins > 0)
	{
		sprint(user,PR_PRINT_HIGH,"To Admins\x8D (",user->netname,"): ",st,"\n");
		stuffcmd(user,"play misc/talk\n");
	}

	dprint("To Admins\x8D (");
	dprint(user->netname);
	dprint("): ");
	dprint(str);
	dprint("\n");

	if (!numadmins)
		sprint(user,PR_PRINT_HIGH,"No admins logged in server currently!\n");
	else
	{
		if (numadmins == 1)
			sprint(user,PR_PRINT_HIGH,S_("^b[^1^b admin reading this^b]^b\n"));
		else
		{
			st = ftos(numadmins);
			st = colstr(st,PR_COLSTR_NUMBER);

			sprint(user,PR_PRINT_HIGH,S_("^b[^b"),st,S_(" admins reading this^b]^b\n"));
		}
	}

	return numadmins;
}

//=======================================================================
// Ends current map if possible

void Admin_EndGame()
{
	if (intermission_running)
	{
		sprint(self,PR_PRINT_HIGH,"The map already ended!\n");
		return;
	}

	if (ServerUpdating())
	{
		sprint(self,PR_PRINT_HIGH,"Unable to end the map during a server update!\n");
		return;
	}

	if (time < 20)
	{
		sprint(self,PR_PRINT_HIGH,"There must pass 20 seconds at least to be able to end a map!\n");
		return;
	}

	ceasefire = PR_FALSE;

	bprint(PR_PRINT_HIGH,"The admin ",self->netname," ends the current game.\n");

	NextLevel ();
}

//==========================================================
// Prints a sermon on a player client

void Admin_Sermon(const string& st, float action)
{
	float predef;

	predef = 0;

	if (st == "1") // TK
		predef = 1;
	else if (st == "2") // Cheating
		predef = 2;

	if (self->admin_kick == world || self->admin_kick->classname != "player" || !self->admin_kick->is_connected)
	{
		sprint(self,PR_PRINT_HIGH,"Bad player selected as target for sermon!\n");
		return;
	}

	if (self->flags & PR_FL_FINALIZED)
	{
		sprint(self,PR_PRINT_HIGH,"Already showing a sermon for that player!\n");
		return;
	}

	if (st == "")
	{
		sprint(self,PR_PRINT_HIGH,"You should specify a string for the sermon or a number (1=tk or 2=cheats)!\n");
		return;
	}

	PlayerFinal(self->admin_kick,st,action,predef);

	if (action == 2)
		bprint(PR_PRINT_HIGH,"The admin ",self->netname, " decides to ban ",self->admin_kick->netname," after a sermon");
	else if (action == 1)
		bprint(PR_PRINT_HIGH,"The admin ",self->netname, " decides to kick ",self->admin_kick->netname," after a sermon");
	else
		bprint(PR_PRINT_HIGH,"The admin ",self->netname, " sermonizes ",self->admin_kick->netname);

	if (predef == 1)
		bprint(PR_PRINT_HIGH," about teamkilling\n");
	else if (predef == 2)
		bprint(PR_PRINT_HIGH," about cheating\n");
	else
		bprint(PR_PRINT_HIGH,"\n");
}

//==============================================================================
// Isn't the bitching of kids annoying sometimes while playing?

float Admin_Silence(float mode, float mutetime)
{
	entity te;
	string st, st2;
	float fl, counter;

	te = world;
	counter = 0;

	if (mode == 0 || mode == 1) // Players
	{
		te = find(te,"classname","player");

		while (te != world)
		{
			if (te->is_connected)
			if (!te->admin_flag)
			{
				fl = getuid(te);
				st = ftos(fl);
				st2 = ftos(mutetime);

				localcmd("mute 00");
				localcmd(st);
				localcmd(" ");
				localcmd(st2);
				localcmd("\n");

				counter = counter + 1;
			}

			te = find(te,"classname","player");
		}
	}

	if (mode == 2 || mode == 0) // Spectators
	{
		te = find(te,"classname","spec");

		while (te != world)
		{
			if (te->is_connected)
			if (!te->admin_flag)
			{
				fl = getuid(te);
				st = ftos(fl);
				st2 = ftos(mutetime);

				localcmd("mute 00");
				localcmd(st);
				localcmd(" ");
				localcmd(st2);
				localcmd("\n");

				counter = counter + 1;
			}

			te = find(te,"classname","spec");
		}
	}

	if (mutetime > 0)
	{
		bprint(PR_PRINT_HIGH,"The admin ",self->netname," imposes silence ");
		if (mode == 0)
			bprint(PR_PRINT_HIGH,"among all clients\n");
		else if (mode == 1)
			bprint(PR_PRINT_HIGH,"among players\n");
		else if (mode == 2)
			bprint(PR_PRINT_HIGH,"among spectators\n");
	}
	else
	{
		bprint(PR_PRINT_HIGH,"The admin ",self->netname," allows all ");
		if (mode == 0)
			bprint(PR_PRINT_HIGH,"clients");
		else if (mode == 1)
			bprint(PR_PRINT_HIGH,"players");
		else if (mode == 2)
			bprint(PR_PRINT_HIGH,"spectators");
		bprint(PR_PRINT_HIGH," to speak\n");
	}

	return counter;
}

//=======================================================================
// Admin requests info about a client

void Admin_Check()
{
	string st;
	float tmpf;

	if (self->admin_kick != world) //Bad
	{
		if (!HasValidAdminTarget(self))
			return;

		sprint(self,PR_PRINT_HIGH,S_("\x9D\x9E\x9E\x9F ^bClient Information Checks^b: \x9D\x9E\x9E\x9F\n"));
		tmpf = getuid(self->admin_kick);
		st = ftos(tmpf);
		st = colstr(st,PR_COLSTR_NUMBER);
		sprint(self, PR_PRINT_HIGH,S_("^bName^b: "),self->admin_kick->netname,S_("\n^bUserID^b: "),st,S_(" \x9C ^bIP^b: "));
		st = infokey(self->admin_kick,"ip");
		st = colstr(st,PR_COLSTR_NUMBER);
		sprint(self, PR_PRINT_HIGH,st,S_("\n^bClaimed^b: "));
		st = GetClientDescription(self->admin_kick);
		sprint(self, PR_PRINT_HIGH,st,"\n\x9D\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9E\x9F\n");
	}
	else
		sprint(self, PR_PRINT_HIGH, "No target client selected!\n");
}


//==================================================


// Gizmo
float IsValidUseObject( entity object);
string GetUseObjectAction( entity object);
string GetUseObjectName( entity object);
void UseObject( entity object);
void Admin_Use()
{
	string objectName, objectAction;

	makevectors( self->v_angle );
	traceline( self->origin + V({0, 0, 16}), ( self->origin + V({0, 0, 16}) ) + v_forward * 4096, PR_TL_ANY_SOLID, self );

	if ( !IsValidUseObject( trace_ent ) ) {
		sprint( self, PR_PRINT_HIGH, trace_ent->classname );
		sprint( self, PR_PRINT_HIGH, " is not a valid object to use\n" );
		return;
	}

	objectName = GetUseObjectName( trace_ent );
	objectAction = GetUseObjectAction( trace_ent );

	bprint( PR_PRINT_HIGH, "The admin ", self->netname, " forces a ", objectName, " to ", objectAction, "\n" );
	UseObject( trace_ent );
}


//==================================================


#ifdef PR_COOP_MODE_ENHANCED
void monster_zombie();
void monster_shambler();
void monster_knight();
void monster_enforcer();
void monster_demon1();
void monster_army();
void monster_ogre();
void monster_shalrath();
void monster_wizard();
void monster_dog();
void monster_fish();
void monster_hell_knight();
void monster_tarbaby();

/*
===============
Admin_SpawnEntityAtLocation

This is a local function used to spawn an admin spawnable
entity at the specified location.
NOTE: does not check if the location is valid.
===============
*/
entity Admin_SpawnEntityAtLocation( const string& entity_classname, const vector& pos) {
	entity	e, oldself;

	e = spawn();
	setorigin( e, pos );

	oldself = self;
	self = e;
	if ( entity_classname == "monster_zombie" )
		monster_zombie ();
	else if ( entity_classname == "monster_shambler" )
		monster_shambler ();
	else if ( entity_classname == "monster_knight" )
		monster_knight ();
	else if ( entity_classname == "monster_enforcer" )
		monster_enforcer ();
	else if ( entity_classname == "monster_demon1" )
		monster_demon1 ();
	else if ( entity_classname == "monster_army" )
		monster_army ();
	else if ( entity_classname == "monster_ogre" )
		monster_ogre ();
	else if ( entity_classname == "monster_shalrath" )
		monster_shalrath ();
	else if ( entity_classname == "monster_wizard" )
		monster_wizard ();
	else if ( entity_classname == "monster_dog" )
		monster_dog ();
	else if ( entity_classname == "monster_fish" )
		monster_fish ();
	else if ( entity_classname == "monster_hell_knight" )
		monster_hell_knight ();
	else if ( entity_classname == "monster_tarbaby" )
		monster_tarbaby ();
	else {
		self = oldself;
		remove( e );
		return world;
	}
	self = oldself;

	e->lives = 0;
	e->endtime = 5;		// remove corpses and gibs after around 5 seconds

	return e;
}

/*
===============
Admin_Spawn

Tries to spawn the the specified type of entity near self's origin.
===============
*/
void Admin_Spawn( const string& entity_classname, float entity_team) {
	entity	e;
	vector	vec;
	string	tmps;

	vec = self->v_angle;
	vec[Z] = 0;
	makevectors( vec );
	traceline( self->origin + V({0, 0, 16}), ( self->origin + V({0, 0, 16}) ) + v_forward * 128, PR_TL_ANY_SOLID, self );

	// spawn the entity at the location
	e = Admin_SpawnEntityAtLocation( entity_classname, trace_endpos );
	if ( e  == world) {
		sprint( self, PR_PRINT_HIGH, "Unknown entity classname\n" );
		return;
	}

	if ( InSolid( e ) ) {
		remove( e );
		sprint( self, PR_PRINT_HIGH, "Entity spawn position not valid!\n" );
		return;
	}

	if ( entity_team )
		e->team_no = entity_team;

	if ( entity_team ) {
		tmps = ftos( entity_team );
		bprint( PR_PRINT_HIGH, "The admin ", self->netname, " spawns a ", entity_classname, " on team ", tmps, "\n" );
	} else
		bprint( PR_PRINT_HIGH, "The admin ", self->netname, " spawns a ", entity_classname, "\n" );
}

/*
===============
Admin_MassSpawn

Tries to spawn the specified amount of the specified type of
entity near self's origin.
===============
*/
void Admin_MassSpawn( const string& entity_classname, float entity_team, float ecount) {
	entity	e;
	vector	vec;
	vector	tempOrigin;
	string	tmps;
	float		biggestSide, ftmp;
	float		numSpawned;
	float		maxCount;

	// don't allow too many to prevent invisible monsters
	if ( ecount > 50 ) {
		sprint( self, PR_PRINT_HIGH, "You cannot spawn more than 50 entities at a time!\n" );
		return;
	}

	// default to 4 if 0 is specified
	if ( !ecount )
		ecount = 4;

	// only allow up to 200 tries to find a good spot
	maxCount = 200;

	e = world;
	biggestSide = numSpawned = 0;

	// set the initial vectors
	vec = self->v_angle;
	vec[Z] = 0;
	makevectors( vec );

	// set the initial origin
	tempOrigin = self->origin + v_forward * 128;

	// make the vectors flat from here on in
	vec[Z] = vec[X] = 0;
	vec[Y] = self->v_angle[Y];
	makevectors( vec );
	vec = v_forward;

	// spawn as many as possible
	while ( numSpawned < ecount && maxCount ) {
		// spawn the entity at the location
		if ( e  == world) {
			e = Admin_SpawnEntityAtLocation( entity_classname, tempOrigin );
			if ( e  == world) {
				sprint( self, PR_PRINT_HIGH, "Unknown entity classname\n" );
				return;
			}
		} else
			setorigin( e, tempOrigin );

		// get the biggest side
		if ( !biggestSide ) {
			biggestSide = ( e->mins[X] * -1 ) + e->maxs[X];
			ftmp = ( e->mins[Y] * -1 ) + e->maxs[Y];
			if ( ftmp > biggestSide )
				biggestSide = ftmp;

			biggestSide = biggestSide + biggestSide / 2;
		}

		// check for invalid positions
		traceline( e->origin, e->origin + v_up * -8192, PR_TL_BSP_ONLY, e );

		// if the entity's current position isn't valid, then keep it and try it somewhere else
		if ( trace_fraction == 1 || InSolid( e ) ) {
			// stop trying if the first monster isn't valid
			if ( !numSpawned ) {
				remove( e );
				sprint( self, PR_PRINT_HIGH, "Entity spawn position not valid!\n" );
				return;
			}

			// backup and try a new direction
			tempOrigin = tempOrigin - vec * biggestSide;

			// randomize the direction a little
			ftmp = random();
			if ( ftmp <= 0.25 )
				vec = v_forward;
			else if ( ftmp <= 0.5 )
				vec = v_forward * -1;
			else if ( ftmp <= 0.75 )
				vec = v_right;
			else
				vec = v_right * -1;
		} else {
			if ( entity_team )
				e->team_no = entity_team;

			e = world;					// spawn a new one now
			numSpawned = numSpawned + 1;
		}

		// update origin and ecount
		tempOrigin = tempOrigin + vec * biggestSide;
		maxCount = maxCount - 1;
	}

	// remove the last monster if it didn't spawn
	if ( e  != world)
		remove( e );

	tmps = ftos( numSpawned );
	sprint( self, PR_PRINT_HIGH, tmps, " entities spawned\n" );

	if ( entity_team ) {
		bprint( PR_PRINT_HIGH, "The admin ", self->netname, " spawns ", tmps, " " );
		tmps = ftos( entity_team );
		bprint( PR_PRINT_HIGH, entity_classname, " entities on team ", tmps, "\n" );
	} else
		bprint( PR_PRINT_HIGH, "The admin ", self->netname, " spawns ", tmps, " ", entity_classname, " entities\n" );
}
#endif

//==================================================

/*
===============
Admin_ValidEntityToKill
===============
*/
float Admin_ValidEntityToKill( entity te) {
	if ( te  == world)
		return PR_FALSE;
	if ( !te->takedamage )
		return PR_FALSE;

	if ( te->classname == "misc_explobox" )
		return PR_TRUE;
	if ( te->classname == "misc_explobox2" )
		return PR_TRUE;
	if ( te->classname == "player" )
		return PR_TRUE;
#ifdef PR_COOP_MODE_ENHANCED
	if ( COOP_IsCoopMonster( te ) )
		return PR_TRUE;
#endif

	return PR_FALSE;
}

/*
===============
Admin_Kill

Kills the entity directly infront of self.
===============
*/
void Admin_Kill() {
	makevectors( self->v_angle );
	traceline( self->origin + V({0, 0, 16}), ( self->origin + V({0, 0, 16}) ) + v_forward * 4096, PR_TL_ANY_SOLID, self );

	if ( !Admin_ValidEntityToKill( trace_ent ) ) {
		sprint( self, PR_PRINT_HIGH, "Invalid entity to kill\n" );
		return;
	}

	if ( trace_ent->netname  != "")
		bprint( PR_PRINT_HIGH, "The admin ", self->netname, " deems ", trace_ent->netname, " dead\n" );
	else
		bprint( PR_PRINT_HIGH, "The admin ", self->netname, " deems the entity ", trace_ent->classname, " dead\n" );

	deathmsg = PR_DMSG_ADMINKILL;
	TF_T_Damage( trace_ent, self, self, trace_ent->health + 10000, PR_TF_TD_IGNOREARMOUR | PR_TF_TD_NOPAIN, PR_TF_TD_OTHER );
}

} // END namespace Progs
