/*****************************************************************************
 * FILE: ChargeOn_Serial.h                                                   *
 * DESC: Definitions for serial communications                               *
 * AUTH: Kerry Burton                                                        *
 * INFO:                                                                     *
 *****************************************************************************
 * COPYRIGHT 2020 Kerry Burton. ALL RIGHTS RESERVED.                         *
 *****************************************************************************/

#ifndef CHARGEON_SERIAL_H
#define CHARGEON_SERIAL_H

 // Defines
#define MY_BAUDRATE CBR_9600

  // Typedefs
typedef enum { HANDSHAKE,                                  // 0
               TURN_ON,                                    // 1
               TURN_OFF,                                   // 2
               MAX_EXCH_TYPE                               // 3
             } SerialExchangeType;

#if 0
typedef enum { SUCCESS,                                    // 0
	           WRITE_ERROR,                                // 1
               READ_ERROR,                                 // 2
               BAD_RESPONSE,                               // 3
               MAX_ERR_TYPE                                // 4
             } SerialStatus;
#endif

typedef struct { char *signal;
	             char *expectedResponse;
                 char *errorMessage;
	           } TalkParams;
	      

  // Function prototypes
BOOL InitSerial(             HANDLE *hSerialPort, TCHAR *pPortName );
BOOL SendSignal_GetResponse( HANDLE hSerial,      SerialExchangeType talkType );

#endif
