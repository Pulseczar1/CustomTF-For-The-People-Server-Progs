#ifndef STATUS_H
#define STATUS_H

namespace Progs {

void CenterPrint(entity pl, const string& s1);
void CenterPrint2(entity pl, const string& s1, const string& s2);
void CenterPrint3(entity pl, const string& s1, const string& s2, const string& s3);
void CenterPrint4(entity pl, const string& s1, const string& s2, const string& s3, const string& s4);
void CenterPrint5(entity pl, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5);
void CenterPrint6(entity pl, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5, const string& s6);
void CenterPrint7(entity pl, const string& s1, const string& s2, const string& s3, const string& s4, const string& s5, const string& s6, const string& s7);
void StatusPrint(entity pl, float fTime, const string& s1);
void RefreshStatusBar(entity pl);
void RefreshStatusBar1(entity pl);
void RefreshStatusBar4(entity pl);
void RefreshStatusBar5(entity pl);
void RefreshStatusBar2(entity pl);
void RefreshStatusBar3(entity pl);
string GetStatusSize(entity pl);
void StatusRes(float res);
string ClipSizeToString100(entity pl);
string SentryDetailsToString(entity te, float devicenum);
string TeslaDetailsToString(entity te, float devicenum);

} // END namespace Progs

#endif // END STATUS_H

