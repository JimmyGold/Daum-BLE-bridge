# Daum-BLE-bridge
Connects to a Daum Ergobike TRS 3 via RS232 and provide the data via BLE to bike computer.
Additionaly it is reading in Heart Rate from a BLE device and provide the pulses to the ear clip input of the Daum.


Hardware: ESP32, Adafruit Bluefruit LE SPI friend module, MAX3232 module, male DP9 connector

Software: Visual Studio Code + PlatformIO

The Bluefruit module is necessary because with the ESP32 built in BLE device it was not possible for me to send the CSC and Power data to the Garmin device (Edge 1030). 


