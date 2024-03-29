/*
	TeamFortress V2.1 22/12/96

	TeamFortress Software
*/

#include "progs.h"

namespace Progs {

float CheckExistence();
entity Findgoal(float gno);

//- OfN -
void RPrint(const string& msg);
void MakeMeDebug(entity who);

//==============================================================
// A remove function which makes sure the entity hasn't already
// been removed, and that it isn't the world object.
void dremove(entity te)
{
	if (te == world)
	{
		RPrint("***BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG***\n");
		RPrint("WORLD has nearly been removed. Don't worry!\n");
		RPrint("***BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG***\n");
		return;
	}

    if (te->classname == "player") //- OfN -
	{
		RPrint("***BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG***\n");
		RPrint("Player entity was going to be removed. Don't worry!\n");
		RPrint("***BUG BUG BUG BUG BUG BUG BUG BUG BUG BUG***\n");
		return;
	}

	if (te->is_removed == PR_TRUE)
	{
	/*
		RPrint("***BUG BUG BUG BUG BUG BUG BUG***\n");
		RPrint("  Entity has been removed twice. \n");
		RPrint("***BUG BUG BUG BUG BUG BUG BUG***\n");
	*/
		return;
	}

	te->is_removed = PR_TRUE;
	remove(te);
}

//==============================================================
// A command which just dumps your current location to the screen
void display_location()
{
	string st;

	st = vtos(self->origin);
	sprint (self, PR_PRINT_HIGH, st);
	sprint (self, PR_PRINT_HIGH, "\n");
}

//- OfN
// any client can remotely debug the server after using impulse 195 if "allow_debug" is set to 1
// and its an admin
// then RPrints will be sprinted to him also
// i coded this cause i dont have access to console in prozac server and wanted to see the dprints
// all the dprints in the code were replaced with RPrints
void RPrint(const string& msg)
{
    dprint(msg);

    if (debug_target==world)
        return;

    sprint(debug_target,PR_PRINT_HIGH,msg);
}

//float(entity person) Is_Admin;
//void(entity person) Check_Admin_Password;

//====================================================================//
// called when an impulse 195
void MakeMeDebug(entity who)
{
    if (debug_target==who)
    {
        sprint(debug_target,PR_PRINT_HIGH,"You are already the remote debugger!\n");
        return;
    }

    if (!who->admin_flag)
    {
        RPrint(who->netname);
        RPrint(" requests remote debugging without beeing admin!\n");
        return;
    }

    RPrint("Debug: ");
    RPrint(who->netname);
    RPrint(" requests remote debugging. Debug (dprints) messages will be sent to him/her too.\n");

    if (debug_target!=world)
    {
        sprint(debug_target,PR_PRINT_HIGH,"Debug: You are not the remote debugger anymore.\n");
        sprint(debug_target,PR_PRINT_HIGH," New remote debugger is: '");
        sprint(debug_target,PR_PRINT_HIGH,who->netname); // PZ: why was this missing second parm? and why does CP compiler just now catch it?
        sprint(debug_target,PR_PRINT_HIGH,"'\n");
    }

    debug_target=who;
    sprint(debug_target,PR_PRINT_HIGH,"Debug: Server debug messages (dprints) will be sent to you.\n");
}

} // END namespace Progs
