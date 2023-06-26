#ifndef FIELD_H
#define FIELD_H

namespace Progs {

void Field_Precache();
void Field_think();
float EntsTouching2(entity e1, entity e2);
int FieldIsImproved(entity tfield);
int FieldIsMalfunctioning(entity tfield);
void DisableField(entity tfield, float timedisable);
void Field_touch_SUB();
entity GetMyFieldGen(entity myself);
float Field_GetForcedStatus(entity tfield);
void SetFieldForcedStatus(float value);
float GetFieldForcedStatus();
float Field_ItCanPass(entity tfield, entity who);
float Field_ShouldDamage(entity tfield, entity who);
void FieldExplosion(entity tfield, const vector& where, entity thing);
void PutFieldWork(entity field);
void Field_touch();
void Create_Field(entity gen1, entity gen2);
void Remove_Field(entity gen1, entity gen2);
float IsValidField(entity field);
float IsValidFieldGen(entity fieldgen);
void Field_UpdateSounds(entity tfield, entity gen1);
void Field_StopSounds(entity tfield);
void Field_MakeVisual(entity tfield);
void FieldGen_think();
float FieldGen_CanIdle(entity fieldgen);
float FieldGens_CanLink(entity fieldgen1, entity fieldgen2);
void FieldGen_Built(entity fieldgen);
entity Find_OtherGen(entity fieldgen);
vector WhereGen(const vector& place, bool forGravityGun = false, entity fieldGen = world);
void FieldGen_Die();
void Engineer_UseFieldGen(entity field);
void FieldEvent(entity tfield, const vector& where, entity thing);

} // END namespace Progs

#endif // END FIELD_H

