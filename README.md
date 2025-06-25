# ESP32-Deauth-Detector  
A lightweight deauth detector based on ESP32 boards  

Detector logic source: [Wi-PWN](https://github.com/samdenty/Wi-PWN)  

The code related to Deauth detection from the original project was extracted using `DeepSeek V3-0324` and adapted for the ESP32 (of course, you can also use it to convert this project's code for the ESP8266).  

## Main Features:  
- [x] Detection of `de-authentication` and `de-associate` packets initiated by pressing the Boot button, as these packets are transmitted unencrypted in the air under the 802.11b/g/n protocol.  
- [x] Scanning across channels (1-13), with a dwell time of 220ms per channel (adjustable) for detection. If a Deauth attack is detected, the LED will light up (each channel controls an LED independently. If a Deauth packet is detected on one channel but not on the next, the LED will first light up and then turn off). Therefore, when facing a single-channel Deauth attack, the LED will take some time to light up once.  
- [x] Adjustable LED brightness via PWM control.  
- [x] The device does not start detection immediately upon power-up. Detection begins only when the Boot button is pressed. To stop detection, press the Reset button or turn off the power.  
* Only supports the `2.4GHz` band.  

Compatible with: ESP32-WROOM32/ESP32-S3/ESP32-C3, etc.  

## Flash  
Install VS Code, add the PlatformIO plugin, and click `Upload` or `Upload and Monitor`.  

Not compatible with Arduino IDE.  
