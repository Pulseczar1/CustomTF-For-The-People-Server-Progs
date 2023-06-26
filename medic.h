#ifndef MEDIC_H
#define MEDIC_H

namespace Progs {

float BioInfect(entity targ, entity attacker, float prob, entity inflictor);
void BioGrenadeTouch();
void BioGrenadeExplode();
void CureAdverseEffects(entity doc, entity patient, const vector& org);
void ClientRevived();
float MedicRevive( entity medic, entity deadPlayer, int reviveFlags);

} // END namespace Progs

#endif // END MEDIC_H

