/*
    variables and functions used by this project
*/


// Heatbeat items
#define Heartbeat_Range   99                      // event count max value
#define Heatbeat    30000                         // heartbeat timer value, in mS

// Watchdog items
volatile int Watchdog_Timout = 1200;              // time in seconds before watchdog times out, 1200 = 1200 seconds or 20 minutes

// Heatbeat timer
int Heart_Value = 0;                              // timer
long LastMsg = 0;                                 // message sent counter

// Custom default values
String WiFi_SSID = "None";                        // SSID string
String My_MAC = "";                               // MAC address, tobe read from ESP8266
char MAC_array[13] = "000000000000";              // MAC definition
volatile int Report_Request = 0;                  // set to 1 if report required
String Event = "Boot";                            // default message event


// ********************************************************************************************************
// ************************ project specific variables ****************************************************
// ********************************************************************************************************

// humidity and temperature items
#define AM2301  2                                     // I/O pin with AM2301
float Humidity = 0;                                   // sensor value
float Temperature = 0;                                // sensor value
#define Sensor_Sample_Speed 5000                      // sample speed in mills
long Sensor_Sample = 0;                               // sensor sampling timer
String DHT_Value = "Null, Null";                      // default 
#define Mean_Samples 10                               // number of samples
float Mean_Temps[Mean_Samples];                       // array used for temperature samples


// ********************************************************************************************************
// ************************ watchdog items ****************************************************************
// ********************************************************************************************************

#include <Ticker.h>
Ticker secondTick;
volatile int watchdogCount = 0;

// watchdog timer function, watchdog is clear when the ststus report is requested by a GET request
void ISRwatchdog() {
  watchdogCount++;
  if (watchdogCount == Watchdog_Timout) {
    Serial.println();
    Serial.println("The watchdog bites !!!!!");
    ESP.reset();
  } // end of timeout test
} // end of watchdog count



