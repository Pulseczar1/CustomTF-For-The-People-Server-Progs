/*
	TeamFortress 1.38	-	11/12/96

	TeamFortress Software
	Functions for CTF Support
*/

#include "progs.h"
#include "tfortmap.h"

namespace Progs {

// Functions in this file
void TeamFortress_CTF_FlagInfo();

//=========================================================================
// Display the whereabouts of the flags
void TeamFortress_CTF_FlagInfo()
{
	entity te;

	// Flag 1
	te = Finditem(PR_CTF_FLAG1);
	if (te->goal_state == PR_TFGS_ACTIVE)
	{
		if (self == te->owner)
		{
			sprint(self, PR_PRINT_HIGH, "You have the enemy flag. ");
		}
		else
		{
			sprint(self, PR_PRINT_HIGH, te->owner->netname);
			sprint(self, PR_PRINT_HIGH, " has");
			if (self->team_no == 1)
				sprint(self, PR_PRINT_HIGH, " your flag. ");
			else
				sprint(self, PR_PRINT_HIGH, " the enemy flag. ");
		}
	}
	else if (te->origin != te->oldorigin)
	{
		if (self->team_no == 1)
			sprint(self, PR_PRINT_HIGH, "Your flag is lying about. ");
		else
			sprint(self, PR_PRINT_HIGH, "The enemy flag is lying about. ");
	}
	else
	{
		if (self->team_no == 1)
			sprint(self, PR_PRINT_HIGH, "Your flag is in your base. ");
		else
			sprint(self, PR_PRINT_HIGH, "The enemy flag is in their base. ");
	}

	// Flag 2
	te = Finditem(PR_CTF_FLAG2);
	if (te->goal_state == PR_TFGS_ACTIVE)
	{
		if (self == te->owner)
			sprint(self,  PR_PRINT_HIGH, "You have the enemy flag.\n");
		else
		{
			sprint(self, PR_PRINT_HIGH, te->owner->netname);
			sprint(self, PR_PRINT_HIGH, " has");
			if (self->team_no == 2)
				sprint(self, PR_PRINT_HIGH, " your flag.\n");
			else
				sprint(self, PR_PRINT_HIGH, " the enemy flag.\n");
		}
	}
	else if (te->origin != te->oldorigin)
	{
		if (self->team_no == 2)
			sprint(self, PR_PRINT_HIGH, "Your flag is lying about.\n");
		else
			sprint(self, PR_PRINT_HIGH, "The enemy flag is lying about.\n");
	}
	else
	{
		if (self->team_no == 2)
			sprint(self, PR_PRINT_HIGH, "Your flag is in your base.\n");
		else
			sprint(self, PR_PRINT_HIGH, "The enemy flag is in their base.\n");
	}
}

} // END namespace Progs
