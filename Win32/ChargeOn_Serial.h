/*****************************************************************************
 * FILE: ChargeOn_Serial.h                                                   *
 * DESC: Definitions for serial communications                               *
 * AUTH: Kerry Burton                                                        *
 * INFO:                                                                     *
 *****************************************************************************
 * COPYRIGHT 2020 Kerry Burton. ALL RIGHTS RESERVED.                         *
 *****************************************************************************/

#ifndef CHARGEON_SERIAL_H
#  define CHARGEON_SERIAL_H                      // Prevent items below from being processed more than once

    /* Defines */
//  #define MY_BAUDRATE   CBR_9600
//  #define MY_BAUDRATE   CBR_14400
//  #define MY_BAUDRATE   CBR_19200
//  #define MY_BAUDRATE   CBR_38400
//  #define MY_BAUDRATE   CBR_57600
  #define MY_BAUDRATE   CBR_115200

    /* Typedefs */
  typedef enum { HANDSHAKE,                      // 0
                 TURN_ON,                        // 1
                 TURN_OFF,                       // 2
                 MAX_EXCH_TYPE                   // 3
               } SerialExchangeType;

  typedef struct { char *signal;
                   char *expectedResponse;
                   char *errorMessage;
                 } TalkParams;
           

    /* Function prototypes */
  BOOL InitSerial            ( HANDLE *hSerialPort                              );
  BOOL SendSignal_GetResponse( HANDLE hSerial,      SerialExchangeType talkType );

#endif
