#ifndef SPRITES_H
#define SPRITES_H

namespace Progs {

// Sprite types
#define PR_SPRITE_ABLAST     1
#define PR_SPRITE_AIRBURST   2

// Sprite move/animation types
//#define SPRITEMOVE_USEDEF 0
#define PR_SPRITEMOVE_UP     1
#define PR_SPRITEMOVE_UPSLOW 2

entity SpawnSprite(float num, float type, const vector& org, vector direction, float animtype, float animrate);
void SetSpriteThink();

} // END namespace Progs

#endif // END SPRITES_H

