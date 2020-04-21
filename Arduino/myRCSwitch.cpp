  /*  Includes */
#include "Project.h"
#include "ChargeOn.h"
#include "myRCSwitch.h"

  /* Module-specific defines */
#if defined(PRJ_DEBUGGING) && defined(MYRCS_DEBUGGING)      // Is debugging enabled for the whole project and for this module?
# define DEBUGGING                                          //  Yes, include DEBUGGING sections of this module in the compilation
#endif

#define RCS_REPEATS     6                                   // Number of times to repeat each transmission

//#define REMOTE_0304  // My ETEKCITY remotes
#define REMOTE_0307  // My DEWENWILS remotes

#if defined REMOTE_0304  // My ETEKCITY remotes
# define RCS_PULSELEN   181                                 // Pulse length in microseconds for Etekcity ZAP

# define OUTLET_1_ON    5330227
# define OUTLET_1_OFF   5330236               
# define OUTLET_2_ON    5330371
# define OUTLET_2_OFF   5330380
# define OUTLET_3_ON    5330691
# define OUTLET_3_OFF   5330700
# define OUTLET_4_ON    5332227
# define OUTLET_4_OFF   5332236
# define OUTLET_5_ON    5338371
# define OUTLET_5_OFF   5338380

#elif defined REMOTE_0307  // My DEWENWILS remotes
# define RCS_PULSELEN   173                                 // Pulse length in microseconds for Dewenwils

# define OUTLET_1_ON    87347
# define OUTLET_1_OFF   87356
# define OUTLET_2_ON    87491
# define OUTLET_2_OFF   87500
# define OUTLET_3_ON    87811
# define OUTLET_3_OFF   87820
# define OUTLET_4_ON    89347
# define OUTLET_4_OFF   89356
# define OUTLET_5_ON    95491
# define OUTLET_5_OFF   95500
#endif

  /* Static variables */
static RCSwitch  mySwitch        = RCSwitch();
                          // Offset:  0       1             2             3             4             5
static long      OutletOnCode[]  = { -1, OUTLET_1_ON,  OUTLET_2_ON,  OUTLET_3_ON,  OUTLET_4_ON,  OUTLET_5_ON  };
static long      OutletOffCode[] = { -1, OUTLET_1_OFF, OUTLET_2_OFF, OUTLET_3_OFF, OUTLET_4_OFF, OUTLET_5_OFF };

void RCSwitchSetup( void ) {
  mySwitch.enableTransmit( RCS_OUTPIN );                    // Assign output pin for transmitter data connection
#ifdef DEBUGGING
//  SerialDebug.print( "433MHz transmission enabled on pin ");
//  SerialDebug.println( RCS_OUTPIN );
#endif
  // mySwitch.setProtocol( 2 );                             // (Optional) Set protocol; default is 1, will work for most outlets
  mySwitch.setPulseLength( RCS_PULSELEN );                  // Set pulse length
  mySwitch.setRepeatTransmit( RCS_REPEATS );                // (Optional) Set number of transmission repetitions
}

void RCS_SendOnCode( int nOutlet ) {
  mySwitch.send( OutletOnCode[nOutlet], RCS_VALUELEN );
  delay(100);
#ifdef DEBUGGING
  SerialDebug.print( "  Sent ON code " );
  SerialDebug.print( OutletOnCode[nOutlet] );
  SerialDebug.print( " to outlet #" );
  SerialDebug.println( nOutlet );
#endif
}

void RCS_SendOffCode( int nOutlet ) {
  mySwitch.send( OutletOffCode[nOutlet], RCS_VALUELEN );
  delay(100);
#ifdef DEBUGGING
  SerialDebug.print( "  Sent OFF code " );
  SerialDebug.print( OutletOffCode[nOutlet] );
  SerialDebug.print( " to outlet #" );
  SerialDebug.println( nOutlet );
#endif
}
