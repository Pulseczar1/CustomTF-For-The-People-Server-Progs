/*=======================================================//
// sprites.QC - CustomTF 3.2.OfN           - 18/4/2004 - //
// by Sergio Fumaña Grunwaldt - OfteN [cp]               //
=========================================================//
 Sprites stuff
---------------------------------------------------------//
 Animation of sprites and functions related to them
=========================================================*/

#include "progs.h"
#include "sprites.h"

namespace Progs {

/*===============================================================================================

EXPLANATION OF HOW THE ENTITY FIELDS ARE USED (thnx? np.. :P)
---------------------------------------------

For sprite entities:
--------------------

.has_fieldgen - Sprite animation rate

================================================================================================*/

// these are the only sprites still in the game...

/*$spritename s_explod
$type vp_parallel
$load /raid/quake/id1/gfx/sprites/explod03.lbm
$frame	24	24	56	56
$frame	120	24	56	56
$frame	216	24	56	56
$frame	24	88	56	56
$frame	120	88	56	56
$frame	216	88	56	56


$spritename s_bubble
$type vp_parallel
$load /raid/quake/id1/gfx/sprites/bubble.lbm
$frame	16	16	16	16
$frame	40	16	16	16


$spritename s_light
$type vp_parallel
$load /raid/quake/id1/gfx/sprites/light.lbm
$frame	104	32	32	32
*/

void ab_exp1(); // AirFist.qc
void run_ablast1(); // mtfents.qc
float crandom();

entity SpawnSprite(float num, float type, const vector& org, vector direction, float animtype, float animrate)
{
    entity sprite = world, oself;
    float counter;

    counter = 0;

    oself = self;

    while (counter < num)
    {
        if (type == PR_SPRITE_ABLAST)
        {
            sprite = spawn();

            sprite->movetype = PR_MOVETYPE_NOCLIP;
            sprite->solid = PR_SOLID_NOT;

            setmodel(sprite, "progs/s_ablast.spr");
            setorigin(sprite, org);

            if (animtype == PR_SPRITEMOVE_UP)
            {
                direction[Z] = 240 + crandom()*100;
                direction[X] = crandom()*120;
                direction[Y] = crandom()*120;
            }
            else if (animtype == PR_SPRITEMOVE_UPSLOW)
            {
                direction[Z] = 50 + crandom()*50;
                direction[X] = crandom()*20;
                direction[Y] = crandom()*20;
            }

            sprite->velocity = direction;

            if (animrate <= 0)
                sprite->has_fieldgen = 0.1;
            else
                sprite->has_fieldgen = animrate;

            self = sprite;
            run_ablast1();
        }
        else if (type == PR_SPRITE_AIRBURST)
        {
            sprite = spawn();

            sprite->movetype = PR_MOVETYPE_NOCLIP;
            sprite->solid = PR_SOLID_NOT;

            setmodel (sprite, "progs/s_explod.spr");
            setorigin(sprite, org);

            if (animtype == PR_SPRITEMOVE_UP)
            {
                direction[Z] = 150 + crandom()*100;
                direction[X] = crandom()*120;
                direction[Y] = crandom()*120;
            }
            else if (animtype == PR_SPRITEMOVE_UPSLOW)
            {
                direction[Z] = 50 + crandom()*50;
                direction[X] = crandom()*20;
                direction[Y] = crandom()*20;
            }

            sprite->velocity = direction;

            if (animrate <= 0)
                sprite->has_fieldgen = 0.1;
            else
                sprite->has_fieldgen = animrate;

            self = sprite;
            ab_exp1();
        } else {
			error ("unknown sprite type");
			sprite = world;		// shutup warnings
		}

        counter = counter + 1;
    }

    self = oself;

    return sprite;
}

//=====================================================
// Sets default rate on a sprite or custom rate

void SetSpriteThink()
{
    if (!self->has_fieldgen)
        self->nextthink = time + 0.1;
    else
        self->nextthink = time + self->has_fieldgen;
}

} // END namespace Progs
