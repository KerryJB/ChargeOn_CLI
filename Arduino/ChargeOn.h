#ifndef CHARGEON_H
#define CHARGEON_H

  // Module-specific defines
#define CO_DEBUGGING                                        // To debug the ChargeOn module (when PRJ_DEBUGGING is defined)

#define WAKE_SIGNAL   "<CO_WAKE>"
#define ACK_SIGNAL    "<CO_WAKE_OK>"

#define ON_SIGNAL     "<CO_ON>"
#define ON_OK_SIGNAL  "<CO_ON_OK>"

#define OFF_SIGNAL    "<CO_OFF>"
#define OFF_OK_SIGNAL "<CO_OFF_OK>"

#define OUTLET_NUM    2
  // Global variables
extern SoftwareSerial SerialDebug;

  // Global function prototypes

#endif   // #ifndef CHARGEON_H
