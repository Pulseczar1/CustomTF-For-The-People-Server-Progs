/*=======================================================//
// ofnents.QC - CustomTF 3.2.OfN           - 30/1/2001 - //
// by Sergio Fumaña Grunwaldt - OfteN [cp]               //
=========================================================//
 Additional map entities along with their spawn functions
=========================================================*/

#include "progs.h"
#include "ofndefs.h"
#include "subs.h"
#include "optimize.h"
#include "cpstuff.h"
#include "custom.h"

namespace Progs {

void model_solid()
{
    if (self->mdl != "")
        precache_model(self->mdl);

    self->movetype = PR_MOVETYPE_NONE;
	self->solid = PR_SOLID_BBOX;

    setmodel (self, self->mdl);
    setsize(self, self->mins, self->maxs);
    setorigin(self, self->origin);
}

void model_nonsolid()
{
    if (self->mdl != "")
    	precache_model(self->mdl);

    self->movetype = PR_MOVETYPE_NONE;
	self->solid = PR_SOLID_NOT;

    setmodel (self, self->mdl);
	setorigin(self, self->origin);

    if (self->option != 0) return;

    makestatic (self); //CRASH This used to have no args. Possible?
}

void InitOfNTrigger(void (*touch_function)())
{
    InitTrigger();

    self->touch = touch_function;

    if (self->noise != "")
    {
        precache_sound(self->noise);

        if (!self->volume)
            self->volume = 1;
    }
}

void TriggerNotifyAndEffects(entity targetplayer)
{
    if (self->message != "")
        sprint(targetplayer,PR_PRINT_HIGH,self->message);

    if (self->noise != "")
        sound(targetplayer,PR_CHAN_AUTO,self->noise,self->volume,PR_ATTN_NORM);
}

float PlayerCanTrigger(entity ent, float bastards)
{
    if (ent->classname != "player")
        return PR_FALSE;
    if (!ent->is_connected)
        return PR_FALSE;
    if (ent->health <= 0) // alive?
        return PR_FALSE;
    if (ent->done_custom & PR_CUSTOM_BUILDING) // ppl customizing
        return PR_FALSE;
    if (ent->playerclass == PR_PC_UNDEFINED) // observers
        return PR_FALSE;

    if (!bastards)
    if (other->penance_time > time)
        return PR_FALSE;

    return PR_TRUE;
}

void trigger_gravity_touch()
{
    if (!PlayerCanTrigger(other,PR_TRUE)) return;

    if (other->gravity != self->gravity)
    {
        TriggerNotifyAndEffects(other);
        other->gravity = self->gravity;
    }
}

void trigger_gravity()
{
    InitOfNTrigger(trigger_gravity_touch);
}

void trigger_alter_equip_touch()
{
    if (!PlayerCanTrigger(other,PR_FALSE)) return;

    float itemid, counter, all;
    entity oself;

    all = 0;
    counter = 0;

    if (self->PR_givenstuff1 | self->PR_givenstuff2 | self->PR_givenstuff3 | self->PR_givenstuff4 | self->PR_givenstuff5)
    {
        if (self->PR_givenstuff1 && ((self->PR_givenstuff1 & other->PR_givenstuff1)!=self->PR_givenstuff1))
        {
            itemid = 1;

            while (itemid < 22)
            {
                if (self->PR_givenstuff1 & itob(itemid))
                if (GiveSpecificItem(other,itemid,PR_TRUE,PR_FALSE))
                    counter = counter + 1;

                itemid = itemid + 1;
            }
        }

        if (self->PR_givenstuff2 && ((self->PR_givenstuff2 & other->PR_givenstuff2)!=self->PR_givenstuff2))
        {
            itemid = 25;

            while (itemid < 22 + 24)
            {
                if (self->PR_givenstuff2 & itob(itemid - 24))
                if (GiveSpecificItem(other,itemid,PR_TRUE,PR_FALSE))
                    counter = counter + 1;

                itemid = itemid + 1;
            }
        }

        if (self->PR_givenstuff3 && ((self->PR_givenstuff3 & other->PR_givenstuff3)!=self->PR_givenstuff3))
        {
            itemid = 25+24;

            while (itemid < 22 + 24 + 24)
            {
                if (self->PR_givenstuff3 & itob(itemid - (24+24)))
                if (GiveSpecificItem(other,itemid,PR_TRUE,PR_FALSE))
                    counter = counter + 1;

                itemid = itemid + 1;
            }
        }

        if (self->PR_givenstuff4 && ((self->PR_givenstuff4 & other->PR_givenstuff4)!=self->PR_givenstuff4))
        {
            itemid = 25+24+24;

            while (itemid < 22 + 24 + 24 + 24)
            {
                if (self->PR_givenstuff4 & itob(itemid - (24+24+24)))
                if (GiveSpecificItem(other,itemid,PR_TRUE,PR_FALSE))
                    counter = counter + 1;

                itemid = itemid + 1;
            }
        }

        if (self->PR_givenstuff5 && ((self->PR_givenstuff5 & other->PR_givenstuff5)!=self->PR_givenstuff5))
        {
            itemid = 25+24+24+24;

            while (itemid < 22 + 24 + 24 + 24 + 24)
            {
                if (self->PR_givenstuff5 & itob(itemid - (24+24+24+24)))
                if (GiveSpecificItem(other,itemid,PR_TRUE,PR_FALSE))
                    counter = counter + 1;

                itemid = itemid + 1;
            }
        }

        if (counter)
        {
            oself = self;
            self = other;
            UpdateWeaponItems();
            self = oself;
        }
    }

    if (self->PR_disabledstuff1 | self->PR_disabledstuff2 | self->PR_disabledstuff3 | self->PR_disabledstuff4 | self->PR_disabledstuff5)
    {
        all = counter;
        counter = 0;

        if (self->PR_disabledstuff1 && ((self->PR_disabledstuff1 & other->PR_disabledstuff1)!=self->PR_disabledstuff1))
        {
            itemid = 1;

            while (itemid < 22)
            {
                if (self->PR_disabledstuff1 & itob(itemid))
                if (StripSpecificItem(other,itemid,PR_TRUE,PR_FALSE))
                    counter = counter + 1;

                itemid = itemid + 1;
            }
        }

        if (self->PR_disabledstuff2 && ((self->PR_disabledstuff2 & other->PR_disabledstuff2)!=self->PR_disabledstuff2))
        {
            itemid = 25;

            while (itemid < 22 + 24)
            {
                if (self->PR_disabledstuff2 & itob(itemid - 24))
                if (StripSpecificItem(other,itemid,PR_TRUE,PR_FALSE))
                    counter = counter + 1;

                itemid = itemid + 1;
            }
        }

        if (self->PR_disabledstuff3 && ((self->PR_disabledstuff3 & other->PR_disabledstuff3)!=self->PR_disabledstuff3))
        {
            itemid = 25+24;

            while (itemid < 22 + 24 + 24)
            {
                if (self->PR_disabledstuff3 & itob(itemid - (24+24)))
                if (StripSpecificItem(other,itemid,PR_TRUE,PR_FALSE))
                    counter = counter + 1;

                itemid = itemid + 1;
            }
        }

        if (self->PR_disabledstuff4 && ((self->PR_disabledstuff4 & other->PR_disabledstuff4)!=self->PR_disabledstuff4))
        {
            itemid = 25+24+24;

            while (itemid < 22 + 24 + 24 + 24)
            {
                if (self->PR_disabledstuff4 & itob(itemid - (24+24+24)))
                if (StripSpecificItem(other,itemid,PR_TRUE,PR_FALSE))
                    counter = counter + 1;

                itemid = itemid + 1;
            }
        }

        if (self->PR_disabledstuff5 && ((self->PR_disabledstuff5 & other->PR_disabledstuff5)!=self->PR_disabledstuff5))
        {
            itemid = 25+24+24+24;

            while (itemid < 22 + 24 + 24 + 24 + 24)
            {
                if (self->PR_disabledstuff5 & itob(itemid - (24+24+24+24)))
                if (StripSpecificItem(other,itemid,PR_TRUE,PR_FALSE))
                    counter = counter + 1;

                itemid = itemid + 1;
            }
        }

        if (counter)
        {
            oself = self;
            self = other;
            self->current_weapon = W_BestWeapon ();
            W_SetCurrentAmmo ();
            UpdateWeaponItems();
            self = oself;
        }
    }

    if ((counter + all) > 0)
        TriggerNotifyAndEffects(other);
}

void trigger_alter_equip()
{
    InitOfNTrigger(trigger_alter_equip_touch);
}

void trigger_restore_equip_touch()
{
    if (!PlayerCanTrigger(other,PR_FALSE)) return;

    if (RestoreOriginalItems(other))
        TriggerNotifyAndEffects(other);
}

void trigger_restore_equip()
{
    InitOfNTrigger(trigger_restore_equip_touch);
}

// Spawnflags: 1 NO_BUILDS 2 NO_MONSTERS

void trigger_special_zone()
{

}

} // END namespace Progs
