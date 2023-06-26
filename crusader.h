#ifndef CRUSADER_H
#define CRUSADER_H

namespace Progs {

void JobCrusader();
void Menu_Crusader();
void Menu_Crusader_Input(float inp);
entity Crusader_FindIdiot(entity crusader);
void AddAura(float aur);
void CrusaderAddInvisibility();
void CrusaderMassHeal();
void CrusaderDispel();
void Aura_Regenerate();

} // END namespace Progs

#endif // END CRUSADER_H

