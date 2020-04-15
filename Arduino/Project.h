#ifndef PROJECT_H
#define PROJECT_H

#include <SoftwareSerial.h>

// Project-wide defines

//#define PRJ_TESTING                                         // ONLY activate this setting during testing! Comment it out before deployment.
#define PRJ_DEBUGGING                                       // When this is defined, each module can be configured for debugging separately
#define PRJ_BAUD_RATE     9600                              // For use with the one-and-only "Serial.begin" call ... in the setup() function
#define PRJ_DEBUG_RX_PIN  10
#define PRJ_DEBUG_TX_PIN  11

#endif  // #ifndef PROJECT_H
