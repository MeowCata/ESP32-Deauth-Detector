# ESP32-Deauth-Detector  
A lightweight deauth detector based on ESP32 boards  

Original logic source: [Wi-PWN](https://github.com/samdenty/Wi-PWN)

This project is modified from `Wi-PWN`'s Detector

The code related to Deauth detection from the original project was extracted using `DeepSeek V3-0324` and adapted for the ESP32 (of course, you can also use it to convert this project's code for the ESP8266).  

## Main Features:  
- [x] Detection of `de-authentication` and `de-associate` packets initiated by pressing the Boot button, as these packets are transmitted unencrypted in the air under the 802.11b/g/n protocol.  
- [x] Scanning across channels (1-13), with a dwell time of 300ms per channel (adjustable) for detection. If a Deauth attack is detected, the LED will light up (Every channel manages the LED's status. That means, if Deauth packets are detected on one channel but not on the next, the LED will **first light up and then turn off** according to the original logic). Considering this issue, the introduction of `Grok 3` incorporates a *StayOnChannel* logic, where the detector stays on the channel where a Deauth attack was recently detected for a while to check for any continued Deauth packets. If such packets are detected, it continues to stay (thus ensuring continuous detection); if not, it proceeds to scan the subsequent channels. That works very nice.
- [x] Adjustable LED brightness via PWM control.  
- [x] The device does not start detection immediately upon power-up. Detection begins only when the Boot button is pressed. To stop detection, press the Reset button or turn off the power.  
* Only supports the `2.4GHz` band(But if someone performs a Deauth attack on the 5GHz band, they'll probably attack the 2.4GHz band as well lol).  

Compatible with: ESP32-WROOM32/ESP32-S3/ESP32-C3, etc.  

## Flash  
Install VS Code, add the PlatformIO plugin, and click `Upload` or `Upload and Monitor`.  

Not compatible with Arduino IDE.  
