/*
    Title:        MQTT Sensor With AM2301
    Date:         13th February 2019
    Version:      1
    Description:  Sample code
    Device:       ESP8266
*/

/* ********************************** Compiler settings, un-comment to use ************************** */
//#define Fixed_IP                      // un-comment to use a fixed IP address to speed up development
//#define Print_Report_Level_1          // un-comment for option
#define Print_Report_Level_2          // un-comment for option, report received MQTT message
#define Print_Report_Level_3          // un-comment for option, DHT report
/* ************************************************************************************************** */

#include <ESP8266WiFi.h>              // needed for EPS8266
#include <WiFiClient.h>               // WiFi client

// Sensor items
#include <DHTesp.h>                   // see https://github.com/beegee-tokyo/DHTesp
DHTesp dht;

// custom settings files
#include "Wifi_Settings.h"            // custom Wifi settings
#include "MQTT_Settings.h"            // MQTT broker and topic
#include "Project_Settings.h"         // board specific details.

// incude WiFi and MQTT functions
WiFiClient espClient;                 // for ESP8266 boards
#include <PubSubClient.h>             // http://pubsubclient.knolleary.net/api.html
PubSubClient client(espClient);       // ESP pubsub client
#include "WiFi_Functions.h"           // read wifi data
#include "MQTT_Functions.h"           // MQTT Functions


void setup() {

  // setup the AM2301 sensor
  dht.setup(AM2301, DHTesp::DHT22);   // setup sensor on pin AM2301

  // watchdog items, comment out if not used
  secondTick.attach(1, ISRwatchdog);

  // connect to WiFi access point
  Get_Wifi();

  // connect to the MQTT broker
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // reset heartbeat timer
  LastMsg = millis();

  // fill the mean buffer with big values (easy to detect if buffer ready)
  for (int zz = 0; zz < Mean_Samples; zz++) {
    Mean_Temps[zz] = (Mean_Samples * 999);
  } // end of buffer flush

} // end of setup


void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();



  // timer for sampling sensor and sending result via MQTT
  if (millis() - Sensor_Sample > Sensor_Sample_Speed) {

    // sample timer timed out so sample the sensor values
    Sensor_Sample = millis();

    Temperature = dht.getTemperature();
    Humidity = dht.getHumidity();

    // test that the result from the sensor is valid, not nan (NaN stands for "not a number")
    if (isnan(Temperature)) {

#ifdef Print_Report_Level_3
      Serial.println("Failed to read from DHT sensor!");
#endif


      DHT_Value = "Null, Null";                  // Default
      return;
    } // end sensor valid test

    // sensor is valid
    else {


      // incrument array contents
      for (int zz = (Mean_Samples - 2); zz > -1; zz--) {
        float xx = Mean_Temps[zz];
        Mean_Temps[zz + 1] = xx;
      }

      // place new value in first position
      Mean_Temps[0] = Temperature;

      // display report of array
#ifdef Print_Report_Level_3
      Serial.println("Report the array contents");
#endif

      float Mean_Value = 0;
      for (int zz = 0; zz < Mean_Samples; zz++) {

#ifdef Print_Report_Level_3
        Serial.println(Mean_Temps[zz]);
#endif
        Mean_Value = Mean_Value + Mean_Temps[zz];
      }
      Temperature = (Mean_Value / Mean_Samples);
#ifdef Print_Report_Level_3
      Serial.print("Mean value = "); Serial.println(Temperature);
#endif

      DHT_Value = (String(Temperature) + "C, ") + (String(Humidity) + "%");

      // report null if mean buffer not yet filled
      if (Temperature > 100) {
        DHT_Value = "Null, Null";                  // Default
      } // end of null test

#ifdef Print_Report_Level_3
      Serial.println(DHT_Value);
#endif

      // publish report via MQTT
      Report_Request = 1;

    } // end of sensor read

  } // end of sensor sample timer and read





  // headbeat or report requested
  if (millis() - LastMsg > Heatbeat || Report_Request == 1) {

    LastMsg = millis();
    Report_Request = 0;

    // update event progress counter
    ++Heart_Value;
    if (Heart_Value > Heartbeat_Range) {
      Heart_Value = 1;
    }

    // heartbeat timed out or report message requested

    // get a Report Make and make as an array
    String Report = Status_Report();
    char Report_array[(Report.length() + 1)];
    Report.toCharArray(Report_array, (Report.length() + 1));

#ifdef Print_Report_Level_2
    // display a report when publishing
    Serial.print("Published To topic: "); Serial.print(InStatus); Serial.print("  -  Report Sent: "); Serial.println(Report_array);
#endif

    // send a status report
    client.publish(InStatus, Report_array);

  } // end of heartbeat timer


} // end of loop
