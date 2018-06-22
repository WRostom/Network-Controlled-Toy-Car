# Network Controlled Toy Car
Turning a normal RC car into a car that can be controller over the network (technically still RC, but you get it).

## Hardware:
- ESP8266-12E — "Car Brains"

- L2932D Half-H Drivers — Used to control toy car motors (front & back)

## Libraries:
- Mosquitto MQTT — for switching between upload and boot mode

- Arduino OTA update — for uploading the code to the ESP8266 wirelessly

Hindsight: I could've used the MQTT to send the commands directly to the arduino. This requires the OTA file to be hosted somewhere so that the ESP8266 can access and flash it.
