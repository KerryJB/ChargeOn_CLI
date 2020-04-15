/*****************************************************************************
 * FILE: ChargeOn_Serial.c                                                   *
 * DESC: Communications module for ChargeOn (Laptop Battery Conditioner)     *
 * AUTH: Kerry Burton                                                        *
 * INFO:                                                                     *
 *****************************************************************************
 * COPYRIGHT 2020 Kerry Burton. ALL RIGHTS RESERVED.                         *
 *****************************************************************************/

#define WIN32_LEAN_AND_MEAN

  /* Includes */
#include "ChargeOn.h"
#include "ChargeOn_Serial.h"

  /* Defines */
#define WAKE_SIGNAL       "<CO_WAKE>"
#define ACK_SIGNAL        "<CO_WAKE_OK>"
#define HANDSHAKE_ERROR   "during initial handshake"
#define HANDSHAKE_SUCCESS 

#define ON_SIGNAL         "<CO_ON>"
#define ON_OK_SIGNAL      "<CO_ON_OK>"
#define TURN_ON_ERROR     "while turning outlet ON"

#define OFF_SIGNAL        "<CO_OFF>"
#define OFF_OK_SIGNAL     "<CO_OFF_OK>"
#define TURN_OFF_ERROR    "while turning outlet OFF"

  /* Function prototypes */
static BOOL SetPortState(  HANDLE hSerial );


/************************************************************************************
 * FUNC: InitSerial                                                                 *
 * DESC: Loop through COM ports until one is found that:                            *
 *         1) We can open and configure successfully                                *
 *         2) Sends the pre-arranged ACK signal in response to our "wake up" signal *
 *       If such a port is found, populate the handle & port name, return "Success" *
 *       Otherwise, return "Failure"                                                *
 * ARGS: phSerialPort = Address of port handle to be populated                      *
 *       pPortName    = String to be populated with port name                       *
 * RET:  TRUE  = Port was opened and configured correctly                           *
 *       FALSE = Failed to open/configure port                                      *
 ************************************************************************************/
BOOL InitSerial( HANDLE *phSerialPort, TCHAR *pPortName )
{
  UINT       uPortNum;
  NAMESTRING pTempPortName;
  BOOL       bStatus        = FALSE;
 
  for( uPortNum = 1; uPortNum < MAX_PORT_NUM; uPortNum++ ) {    
    HANDLE hSerial;

    sprintf( pTempPortName, TEXT("\\\\.\\COM%u"), uPortNum );
    hSerial = CreateFile( pTempPortName,                   // Name of the Port to be Opened
                          GENERIC_READ | GENERIC_WRITE,    // Read/Write Access
                          0,                               // No Sharing, ports cant be shared
                          NULL,                            // No Security
                          OPEN_EXISTING,                   // Open existing port only
                          FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH,
                                                           // Non Overlapped, Non-Buffered I/O
                          NULL);                           // Null for Comm Devices

    if( hSerial == INVALID_HANDLE_VALUE ) {                // Handle is invalid?
      continue;                                            //  Yes, try the next COM port
    }
    else {                                                 //  No...
      printf( "Opened port %s\n", pTempPortName );         //   Print the port name
      if( !SetPortState(hSerial) ) {                       //   Able to configure the desired settings for the port?
        printf( "** ERROR: Unable to configure port %s... **\n", pTempPortName );
				                                           //    No, print error message
        CloseHandle( hSerial );                            //     Close the handle
        printf( "Closing port %s...\n\n", pTempPortName ); //     Print "closing" message
        continue;                                          //     Try the next COM port
      }
      else if( !SendSignal_GetResponse(hSerial, HANDSHAKE) ) {
                                                           //    Yes, able to "wake up" the COM port?
        CloseHandle( hSerial );                            //     No, close the handle
        printf( "Closing port %s...\n\n", pTempPortName ); //      Print "closing" message
        continue;                                          //      Try the next COM port
      }
      else {                                               //     Yes...
        *phSerialPort = hSerial;                           //      Populate caller's handle for the port
        strcpy( pPortName, pTempPortName );                //      Populate caller's port name string
        bStatus = TRUE;                                    //      Set status flag to indicate Success
        printf( "** SUCCESS: Received expected response from port %s during initial handshake **\n\n", pTempPortName );
        break;
      }
    }

  } /* for */
  if (!bStatus ) {
    printf( "\n*** Checked %d ports ***\n\n", uPortNum );  // Report on the number of ports checked
  }

  return bStatus;                                          // Return the final status
}

/*************************************************************************************
 * FUNC: SetPortState                                                                *
 * DESC: Set up DCB (Device Control Block) and timeouts for current port of interest *
 * ARGS: hSerial = Handle for port to be configured                                  *
 * RET:  TRUE  = Port was successfully configured                                    *
 *       FALSE = Failed to configure port                                            *
 *************************************************************************************/

static BOOL SetPortState( HANDLE hSerial )
{
  DCB dcbSerialParams = { 0 };                             // Initialize DCB structure

  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
  if( !GetCommState(hSerial, &dcbSerialParams) ) {         // Able to retrieve  the current settings?
    printf("  ERROR in GetCommState()\n");                 //  No, print error message
    return FALSE;                                          //  and FAIL
  }
  else {                                                   //  Yes...
    dcbSerialParams.BaudRate = MY_BAUDRATE;                //   Set DCB values that we care about
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;

    if( !SetCommState(hSerial, &dcbSerialParams) ) {       //   Able to configure the port according to settings in DCB?
      printf("  ERROR setting DCB structure\n");           //    No, print error message
      return FALSE;                                        //     and FAIL
    }
    else {                                                 //   Yes, print configured settings
      printf( "  Baud Rate = %d\n", dcbSerialParams.BaudRate );
      printf( "  Byte Size = %d\n", dcbSerialParams.ByteSize );
      printf( "  Stop Bits = " );
      switch( dcbSerialParams.StopBits ) {
        case ONESTOPBIT:   printf( "1\n" );
                           break;
        case ONE5STOPBITS: printf( "1.5\n" );
                           break;
        case TWOSTOPBITS:  printf( "2\n" );
                           break;
      }
      printf( "   Parity   = %d\n", dcbSerialParams.Parity );
		
      COMMTIMEOUTS timeouts = { 0 };                       //    Assign desired timeout settings
      timeouts.ReadIntervalTimeout         = 50;
      timeouts.ReadTotalTimeoutConstant    = 50;
      timeouts.ReadTotalTimeoutMultiplier  = 10;
      timeouts.WriteTotalTimeoutConstant   = 50;
      timeouts.WriteTotalTimeoutMultiplier = 10;

      if( !SetCommTimeouts(hSerial, &timeouts) ) {         //    Able to configure the port with the desired timeout settings?
        printf("  ERROR setting timeouts\n");              //     No, print error message
        return FALSE;                                      //     and FAIL
      }
    }
  }

  return TRUE;                                             // Configured the port successfully!
}


/*************************************************************************************
 * FUNC: SendSignal_GetResponse                                                      *
 * DESC: Send specific signal to microcontroller, expect (one of) certain response(s)*
 * ARGS: hSerial  = Handle for microcontroller serial port                           *
 *       talkType = HANDSHAKE to establish initial contact with microcontroller      *
 *                = TURN_ON   to send signal for MC to turn outlet ON                *
 *                = TURN_OFF  to send signal for MC to turn outlet OFF               *
 * RET:  TRUE  = Successfully wrote to port and received good response               *
 *       FALSE = Error while writing/reading, or received unexpected response        *
 *************************************************************************************/

BOOL SendSignal_GetResponse( HANDLE hSerial, SerialExchangeType talkType )
{
  TalkParams chat[] = { {WAKE_SIGNAL, ACK_SIGNAL,    HANDSHAKE_ERROR},
                        {ON_SIGNAL,   ON_OK_SIGNAL,  TURN_ON_ERROR},
                        {OFF_SIGNAL,  OFF_OK_SIGNAL, TURN_OFF_ERROR} };
  char  *OutBuffer        = chat[talkType].signal;         // OutBuffer should be char or byte array, otherwise write will fail
  DWORD dNoOfBytesToWrite = strlen(OutBuffer);             // Number of bytes to write to the port
  DWORD dNoOfBytesWritten = 0;                             // Number of bytes actually written to the port

  char  InBuffer[15];
  DWORD dNoOfBytesToRead  = strlen( chat[talkType].expectedResponse );
                                                           // Number of bytes to read from the port
  DWORD dNoOfBytesRead    = 0;                             // Number of bytes actually read from the port

  if( !WriteFile(hSerial,
                 OutBuffer,
                 dNoOfBytesToWrite,
                 &dNoOfBytesWritten,
                 NULL)
    ) {                                                    // Able to write "wake up" signal to serial port?
    printf("** ERROR writing to serial port %s **\n", chat[talkType].errorMessage );
		                                                   //  No, print error message
    return FALSE;                                          //  and FAIL
  }
  else {                                                   //  Yes...
    Sleep( 300 );                                          //   Allow time for microcontroller to send response
    if( !ReadFile(hSerial,
                  InBuffer,
                  dNoOfBytesToRead,
                  &dNoOfBytesRead,
                  NULL) ) {                                //   Able to read response from serial port?
      printf("** ERROR reading from serial port %s **\n", chat[talkType].errorMessage );
		                                                   //    No, print error message
      return FALSE;                                        //    and FAIL
    }
    else if( strncmp(InBuffer, chat[talkType].expectedResponse, dNoOfBytesToRead) ) {
                                                           //    Yes, did we get the *expected* response?
      printf("** ERROR: Received unexpected response \"%s\" from serial port %s **\n", InBuffer, chat[talkType].errorMessage );
		                                                   //     No, print error message
      return FALSE;                                        //     and FAIL
    }
    else {
      return TRUE;                                         //     Yes, success!
    }
  }
}
