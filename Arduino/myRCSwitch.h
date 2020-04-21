#include <RCSwitch.h>

#ifndef MYRCSWITCH_H
#  define MYRCSWITCH_H

    /* Module-specific defines */
#  define MYRCS_DEBUGGING                                     // To debug the myRCSwitch module (when PRJ_DEBUGGING is defined)

      // For ETEKCITY ZAP 5LX remote 120V outlets
#  define RCS_OUTPIN       7         // Arduino pin for transmitter data connection
#  define RCS_VALUELEN    24         // Number of data bits

      /* Global variables */

      /* Public function prototypes */
  void RCSwitchSetup(   void );
  void RCS_SendOnCode(  int nOutlet );
  void RCS_SendOffCode( int nOutlet );

#endif   // #ifndef MYRCSWITCH_H
