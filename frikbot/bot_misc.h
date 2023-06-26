#ifndef BOT_MISC_H
#define BOT_MISC_H

namespace Progs {

string frik_getNameForBotGivenNumber(float r);
string frik_getRandomNameForBot();
void frik_botConsiderStartingConversation(float topic, float chance);
void frik_botChat();
void frik_removeABot();
vector frik_getRealOriginOfEntity(entity ent);
float frik_isVisible_Simple(entity targ);
float frik_isVisible_Complex(entity targ);
float frik_isVisible_Waypoint(entity targ1, entity targ2);
float frik_angleSubtraction(float y1, float y2);
float frik_isInFovOfSelf(entity targ);
float frik_getAngleMod360(float v);
float frik_getAngleMod180(float v);
entity LinkedList_addNodeToEndOf(entity firstNode, const string& LinkedList_description);
entity LinkedList_removeNode(entity firstNode, entity nodeToRemove);
void LinkedList_deleteList(entity firstNode);
entity LinkedList_swapNodes(entity firstNode, entity e1, entity e2);
void timerThink_botQuit();
void timerThink_botFinishTypingMsg();
void timerThink_botEnsureWeaponSelection();
void timerCleanup_botEnsureWeaponSelection();
float roundFloat(float x);

} // END namespace Progs

#endif // END BOT_MISC_H

