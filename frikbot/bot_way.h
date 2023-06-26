#ifndef BOT_WAY_H
#define BOT_WAY_H

namespace Progs {

float frik_doesWaypointLinkToWaypoint(entity waypoint1, entity waypoint2);
float frik_linkWaypointToWaypoint(entity waypoint1, entity waypoint2);
float frik_telelinkWaypointToWaypoint(entity waypoint1, entity waypoint2);
void frik_unlinkWaypointToWaypoint(entity waypoint1, entity waypoint2);
entity frik_findWaypointNearestAndVisibleToSelf(entity startingWaypoint);
entity frik_findWaypointToRoamTo();
entity frik_createBlankWaypointHere(const vector& position);
void frik_createWaypointVisualMarker(entity waypoint);
void frik_deleteWaypointVisualMarker(entity waypoint);
void frik_updateWaypointVisualMarkers();
void frik_dynamicWaypointing();
void frik_deleteAllWaypoints();
entity frik_retrieveWaypointByNumber(float num);
void frik_updateWaypointLinkPointers(entity waypoint);
void frik_updateWaypointLinkNumbers(entity waypoint);
void frik_fixThisWaypoint();
void frik_fixAllWaypoints();
void frik_deleteWaypoint(entity waypoint);
entity frik_findNearestEntityToSelfWithThisClassname(const string& s);
entity frik_findNextWaypointInOurRoute(entity lastone);
void frik_clearRouteTable();
void frik_clearRouteForSelf();
void frik_storeSelfsRoute(entity waypointStart/*this*/);
void FollowLink(entity linkedToWaypoint, int teleLink);
void WaypointThink();
float frik_startRouteFinding();
void frik_findRouteToObject(entity object/*this*/, float direct);
void frik_createWaypoint(const vector& position, const vector& link1_2_and_3, float link4, float AI_flags);
entity frik_copyWaypoint(entity waypoint);
void frik_createTemporaryWaypoint(const vector& org);

} // END namespace Progs

#endif // END BOT_WAY_H

