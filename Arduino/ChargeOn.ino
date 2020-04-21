  // Includes
#include "Project.h"
#include "ChargeOn.h"
#include "myRCSwitch.h"

  // Module-specific defines
#if defined(PRJ_DEBUGGING)                                  // Is debugging enabled for the project?
# if defined(CO_DEBUGGING) || defined(MYRCS_DEBUGGING)      //  Yes, is debugging enabled for at least one module?
#  define DO_SERIAL_DEBUG                                   //   Yes, include SerialDebug.begin() in the compilation
# endif
#endif

#if defined(PRJ_DEBUGGING) && defined(CO_DEBUGGING)         // Is debugging enabled for the whole project and for this module?
# define DEBUGGING                                          //  Yes, include DEBUGGING sections of this module in the compilation
#endif

  /* Global variables */
#ifdef DO_SERIAL_DEBUG
SoftwareSerial SerialDebug( PRJ_DEBUG_RX_PIN, PRJ_DEBUG_TX_PIN );
#endif

  /* Local variables */
const byte numChars                 = 32;                   // This must be at least 2
      char receivedChars[numChars];
      bool bNewData;

  /*  Static function prototypes */

/***********************/
/* Project entry point */
/***********************/
void setup() {
  Serial.begin( PRJ_BAUD_RATE );                            // For communicating with Windows "ChargeOn" program
#ifdef DO_SERIAL_DEBUG
  SerialDebug.begin( PRJ_BAUD_RATE );                       // Start serial communications (for debugging) with project-defined baud rate
#endif
  RCSwitchSetup();                                          // Initialize RF Transmitter
}

void loop() {
#if 0
  RCS_SendOnCode( OUTLET_NUM );
  Serial.println( ON_OK_SIGNAL );
  delay(4000);
  RCS_SendOffCode( OUTLET_NUM );
  Serial.println( OFF_OK_SIGNAL );
  delay(4000);

#else

  recvWithStartEndMarkers();
  if( bNewData ) {
    if( !strcmp(receivedChars, WAKE_SIGNAL) ) {
      Serial.print( WAKE_OK_SIGNAL );
#ifdef DEBUGGING
      SerialDebug.print( "  Sending reply: " );
      SerialDebug.println( WAKE_OK_SIGNAL );
#endif
    }
    else if( !strcmp(receivedChars, ON_SIGNAL) ) {
      Serial.print( ON_OK_SIGNAL );
#ifdef DEBUGGING
      SerialDebug.print( "  Sending reply: " );
      SerialDebug.println( ON_OK_SIGNAL );
#endif
      RCS_SendOnCode( OUTLET_NUM );
    }
    else if( !strcmp(receivedChars, OFF_SIGNAL) ) {
      Serial.print( OFF_OK_SIGNAL );
#ifdef DEBUGGING
      SerialDebug.print( "  Sending reply: " );
      SerialDebug.println( OFF_OK_SIGNAL );
#endif
      RCS_SendOffCode( OUTLET_NUM );
    }
    receivedChars[0] = '\0';                                // "Clear out" the latest input string
    bNewData         = false;                               // We no longer have an "active" input string
  }
#endif
}

void recvWithStartEndMarkers() {
  const char    startMarker    = '<';
  const char    endMarker      = '>';
        boolean recvInProgress = false;
        byte    ndx            = 0;
        char    rc;
 
  while( Serial.available() ) {                             // While data is waiting...
    rc = Serial.read();                                     //  Read the next byte
                                 /* KJB: Not sure why the following delay seems to be necessary? */
                                 /* KJB: Without it, the received characters do not seem to "register" ... */
    delay(1);
    if( !recvInProgress && (rc != startMarker) ) {          //  Have we seen the start marker yet?
      continue;                                             //   No, throw away the new byte
    }
    else {                                                  //   Yes...
      receivedChars[ndx++] = rc;                            //    Append new byte to the input buffer
      if( (rc == startMarker) && (ndx == 1) ) {             //    New byte IS the (first) start marker?
        recvInProgress = true;                              //     Yes, we're now between the start marker and end marker
      }
      else if( (ndx == numChars) || (rc == endMarker) ) {   //     No, is buffer full, or is new byte the end marker?
        receivedChars[ndx] = '\0';                          //      Yes, null-terminate the buffer
        bNewData = true;                                    //       We now have a complete input string
        break;                                              //       Exit the loop - we're done!
      }
    }
  }  // while loop
}
