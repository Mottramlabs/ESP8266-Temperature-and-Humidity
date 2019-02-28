# ESP-Temperature-and-Humidity
A simple Temperature and Humidity Sensor reporting via MQTT

This sample code uses AM2301 library https://github.com/beegee-tokyo/DHTesp. It reports the temperature and humidity every 5 seconds, the temperature is a mean value sampled 10 times. Therefore it takes 50 seconds to report the first value. This helps smooth out the noise from the AM2301 sensor.
The Wifi user name and password is set in the Wifi_Settings.h file and the MQTT broker etc in the MQTT_settings.h file. A watchdog is enabled that will re-boot the sensor if the remote PC does not pole the sensor at least once every 20 minutes, disable this it not required.

A simple case for this project https://www.thingiverse.com/thing:3454041

I have a few ready made PCB's here https://www.ebay.co.uk/sch/mlabs2018/m.html?_nkw=&_armrs=1&_ipg=&_from=
