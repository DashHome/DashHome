# DashHome
DashHome: A HomeSpan based HomeKit implementation with Web Dashboard for the ESP32

## Current version has basic support for :
- Switch (HIGH/LOW)
- Servo motor triggered Switch
- Controlling Tuya v3.3 (WiFi only) Devices using [MicroTuya](https://github.com/kwankiu/MicroTuya) on a secondary ESP32 device via UART communication
- Sending Ultrasonic Sensor Distance from an ESP8266 using ESPNOW

## Future Plans
- I will evaluate whether this is feasible, but I would probably move from HomeSpan to using Matter library which brings Arduino-IDE-based Smart Home projects to not only HomeKit but using a more flexible protocol Matter which allows us to support HomeKit, Google Assistant, Amazon Alexa, and more. (or even creating your own Smart Home ecosystem, probably either link up with a Linux based software, or with an external web service, or even just using multiple ESP32 to form an ecosystem without requiring another device as a Home Hub)

## Known Issue (to be fixed) :
- Currently, this firmware can only add one type of devices at a time, need some code fix (which I do this for hobby so I dont know when I will be able to fix this, I will be more than happy if you submit a pull request (if you know how to fix that)

## Library :
- [Modified Version of HomeSpan](https://github.com/kwankiu/HomeSpan)
- ESPNow, Servo, ArduinoJSON, etc (details will be updated soon)

## Contributors are welcomed 
