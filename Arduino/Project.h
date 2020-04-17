#ifndef PROJECT_H
#define PROJECT_H

#include <SoftwareSerial.h>

  // Project-wide defines
#define PRJ_DEBUGGING                                       // When this is defined, each module can be configured for debugging separately

//#define PRJ_BAUD_RATE     96000
//#define PRJ_BAUD_RATE     14400
//#define PRJ_BAUD_RATE     19200
//#define PRJ_BAUD_RATE     38400
//#define PRJ_BAUD_RATE     57600
#define PRJ_BAUD_RATE     115200                            // For use with the one-and-only "SerialDebug.begin" call ... in the setup() function

#define PRJ_DEBUG_RX_PIN  10
#define PRJ_DEBUG_TX_PIN  11

#endif  // #ifndef PROJECT_H
