#ifndef PROZAC_H
#define PROZAC_H

namespace Progs {

#define PR_REMINDER_RATE         2400 // 40 minutes

//WK 152 Default, must be > 6.
#define PR_MOTD_FINISHED         3000 // WK 1/7/7 420 //242 // This is in frames. Should be based on a timer, but I don't feel like changing it right now. -PZ

// PZ: added more pages to intro/MOTD
#define PR_NUMBER_OF_MOTD_PAGES  3    // PZ: ### need to set this based on how many pages you use below ###

// Functions
void PrintProzacMOTD();
void PrintProzacMOTD_Page2();
void PrintProzacMOTD_Page3();
void PrintProzacMOTD_Page4();
void PrintProzacMOTD_Page5();
void PrintProzacIntro();
void Reminder();

} // END namespace Progs

#endif // END PROZAC_H

