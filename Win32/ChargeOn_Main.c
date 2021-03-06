/*****************************************************************************
 * FILE: ChargeOn_Main.c                                                     *
 * DESC: Core module for ChargeOn (Laptop Battery Conditioner)               *
 * AUTH: Kerry Burton                                                        *
 * INFO:                                                                     *
 *****************************************************************************
 * COPYRIGHT 2020 Kerry Burton. ALL RIGHTS RESERVED.                         *
 *****************************************************************************/

  /* Includes */
#include "ChargeOn.h"
#include "ChargeOn_Serial.h"

#if 0
typedef struct _SYSTEM_POWER_STATUS {
  BYTE  ACLineStatus;         //   0 = Offline
                              //   1 = Online
                              // 255 = Unknown status
  BYTE  BatteryFlag;          //   0 = Capacity is 33%-66% and battery is not being charged
                              //   1 = High; more than 66%
                              //   2 = Low; less than 33%
                              //   4 = Critical; less than 5%
                              //   8 = Charging
                              // 128 = No system battery
                              // 255 = Unknown status; unable to read battery flag info
  BYTE  BatteryLifePercent;   // 0-100 = Percentage of full battery charge remaining
                              //   255 = Status is unknown
  BYTE  SystemStatusFlag;     // [Introduced in Windows 10]
                              // 0 = Battery saver is OFF
                              // 1 = Battery saver is ON
  DWORD BatteryLifeTime;      // The number of seconds of battery life remaining
                              //   �1 = Unknown, or the device is connected to AC power
  DWORD BatteryFullLifeTime;  // The number of seconds of battery life when at full charge
                              //   �1 = Unknown, or the device is connected to AC power
} SYSTEM_POWER_STATUS, *LPSYSTEM_POWER_STATUS;
#endif

  /* Defines */
#define UNKNOWN_STATUS  255
#define UNKNOWN_PERCENT 255

#define MAXIMUM_PERCENT 96
#define MINIMUM_PERCENT 30
#define WAIT_INTERVAL   20000

  /* Function prototypes */
static BOOL CollectBatteryInfo( SYSTEM_POWER_STATUS *pSPS );
static void ProcessBatteryInfo( SYSTEM_POWER_STATUS *pSPS, BOOL bInfoIsGood, HANDLE hSerialPort );
static void EnableCharging    ( HANDLE hController );
static void DisableCharging   ( HANDLE hController );


/*****************************************************************************
 * FUNC: main                                                                *
 * DESC: The application entry point.                                        *
 *       Oversee the initialization and continuing operation of the app.     *
 *****************************************************************************/
int main( int argc, char **argv )
{
  HANDLE              hComPort;
  SYSTEM_POWER_STATUS SysPowStat;                          // Windows-defined structure to hold battery-related info
  BOOL                bCollectedInfoOK;                    // Indicates whether battery-related info was collected successfully

  if( !InitSerial(&hComPort) ) {
    printf( "ERROR: Could not find an available / suitable COM port.\n\nPress any key to quit..." );
    _getch();
    return 1;
  }

    /* MAIN LOOP */
  while( 1 ) {                                             // Until the app is "killed"...
    bCollectedInfoOK = CollectBatteryInfo( &SysPowStat );  //  Get details about the battery's current state
    ProcessBatteryInfo( &SysPowStat, bCollectedInfoOK, hComPort );
                                                           //  Make decisions, take actions (if any) based on battery state
    Sleep( WAIT_INTERVAL );                                //  Avoid too-tight loop (may not be necessary?)
  }
}  // main


/*****************************************************************************
 * FUNC: CollectBatteryInfo                                                  *
 * DESC: Collect information about the battery's charge and status           *
 *****************************************************************************/
static BOOL CollectBatteryInfo( SYSTEM_POWER_STATUS *pSPS )
{
  BOOL bCallSuccess = FALSE;

  bCallSuccess = GetSystemPowerStatus( pSPS );
  if( bCallSuccess ) {
    printf( "Battery %scharging at %d percent\n", (pSPS->ACLineStatus == 0) ? "dis" : "", pSPS->BatteryLifePercent );
  }
  else {
    printf( "Could  not collect info about the battery\n" );
  }
  return bCallSuccess;
}


/*****************************************************************************
 * FUNC: ProcessBatteryInfo                                                  *
 * DESC: Make decisions and take actions (if any) based on battery state     *
 *****************************************************************************/
static void ProcessBatteryInfo( SYSTEM_POWER_STATUS *pSPS, BOOL bInfoIsGood, HANDLE hSerialPort )
{
  if(    !bInfoIsGood                                      // Couldn't get battery info
      || pSPS->BatteryLifePercent == UNKNOWN_PERCENT       // OR battery status is unknown
      || pSPS->ACLineStatus == UNKNOWN_STATUS              // OR AC line status is unknown
      || (   pSPS->ACLineStatus == 0                       // OR currently discharging 
          && pSPS->BatteryLifePercent <= MINIMUM_PERCENT)  // at or below the minimum charge allowed?
    ) {
    EnableCharging( hSerialPort );                         //  Yes, enable charging
    if( pSPS->BatteryLifePercent < MINIMUM_PERCENT-2 ) {   //   Have we continued to discharge past the target percentage?
      MessageBox( 0,                                       //    Yes, alert the user
                  "Outlet has not turned on as expected. Try repositioning the controller box, or turn on the outlet manually.",
                  "Outlet did not turn ON",
                  MB_OK | MB_ICONWARNING
                );
    }
  }
  else if(   pSPS->ACLineStatus == 1                       //  No, currently charging
          && pSPS->BatteryLifePercent >= MAXIMUM_PERCENT){ //  at or above the maximum charge allowed?
    DisableCharging( hSerialPort );                        //   Yes, disable charging
    if( pSPS->BatteryLifePercent > MAXIMUM_PERCENT+2 ) {   //    Have we continued to charge past the target percentage?
      MessageBox( 0,                                       //     Yes, alert the user
                  "Outlet has not turned off as expected. Try repositioning the controller box, or turn off the outlet manually.",
                  "Outlet did not turn OFF",
                  MB_OK | MB_ICONINFORMATION
                );
    }

  }
  else {                                                   //   No, allow charging (or discharging) to continue as-is.
    ;
  }
}


/*****************************************************************************
 * FUNC: EnableCharging                                                      *
 * DESC: Send signal to turn ON the AC line (to charge battery)              *
 *****************************************************************************/
static void EnableCharging( HANDLE hController )
{
  if( SendSignal_GetResponse(hController, TURN_ON) ) {
    printf( "  Turning smart outlet ON\n" );
  }
}


/*****************************************************************************
 * FUNC: DisableCharging                                                     *
 * DESC: Send signal to turn OFF the AC line (to allow battery to discharge) *
 *****************************************************************************/
static void DisableCharging( HANDLE hController )
{
  if( SendSignal_GetResponse(hController, TURN_OFF) ) {
    printf( "  Turning smart outlet OFF\n" );
  }
}
