#ifndef DEMOMAN_H
#define DEMOMAN_H

namespace Progs {

void TeamFortress_DetonatePipebombs();
void PipebombTouch();
void MirvGrenadeTouch();
void MirvGrenadeExplode();
void MirvGrenadeLaunch(const vector& org, const vector& initVelocity, entity shooter);
void MirvGrenadeLaunch(const vector& org, entity shooter);
void TeamFortress_SetDetpack(float timer);
void TeamFortress_DetpackStop(float krac);
void TeamFortress_DetpackSet();
void TeamFortress_DetpackExplode();
void TeamFortress_DetpackTouch();
void TeamFortress_DetpackDisarm();
void TeamFortress_DetpackCountDown();

} // END namespace Progs

#endif // END DEMOMAN_H

