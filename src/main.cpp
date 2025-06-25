/*
  Deauth Detector for ESP32-C3
  Based on Wi-PWN deauth detection code
*/

#include <Arduino.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_event.h"
#include "driver/ledc.h"

// Settings
const int detectorChannel = 1;  // Default channel to monitor
const int alertPin = 8;         // GPIO pin for alert output (GPIO8 on ESP32-C3)
const int bootPin = 9; //Onboard boot button on GPIO x(GPIO9 on ESP32-C3)
const unsigned long scanTime = 300; // Detection window in ms
const bool invertAlertPin = false;   // Invert alert pin logic (true=LED off while deauthing; false=LED on while deauthing)
const unsigned long stayOnChannelTime = 700; // Time to stay on a channel after detecting deauth (ms)
const int LED_brightness = 80;   // The brightness of LED(0-255)

// Detection variables
bool detecting = false;
unsigned long deauthCount = 0;
unsigned long prevTime = 0;
unsigned long curTime = 0;
int curChannel = detectorChannel;
unsigned long lastDetectionTime = 0; // Last time a deauth frame was detected
bool stayOnChannel = false; // Flag to stay on current channel after detection

void dSniffer(void* buf, wifi_promiscuous_pkt_type_t type) {
  // Detect deauthentication (0xA0) and disassociation (0xC0) frames
  wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
  if (pkt->rx_ctrl.sig_len >= 12 && (pkt->payload[0] == 0xA0 || pkt->payload[0] == 0xC0)) {
    deauthCount++;
  }
}

void setBrightness(int brightness) {
  // Constrain brightness to 0-255 range
  brightness = constrain(brightness, 0, 255);
  ledcWrite(0, brightness); // Write to PWM channel 0
}

void startDetector() {
  Serial.println("Starting Deauth Detector...");
  
  // Setup WiFi in monitor mode
  WiFi.mode(WIFI_MODE_NULL);
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  
  // Set packet sniffer callback
  esp_wifi_set_promiscuous_rx_cb(dSniffer);
  esp_wifi_set_channel(curChannel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(true);

  detecting = true;
  
  Serial.println("Detector started on channel " + String(curChannel));
}

void setup() {
  Serial.begin(115200);
  Serial.println("Deauth Detector Initializing...");
  
  // Setup PWM for LED brightness control
  ledcSetup(0, 5000, 8); // Channel 0, 5kHz, 8-bit resolution
  ledcAttachPin(alertPin, 0); // Attach alertPin to PWM channel 0
  ledcWrite(0, 255); // Start with LED off
  
  pinMode(bootPin, INPUT);
  delay(1000);
}

void loop() {
  bool bootBtnPressed = (digitalRead(bootPin) == LOW);
  if (bootBtnPressed) {
    // Blink LED for 50ms
    setBrightness(255 - LED_brightness);
    delay(50);
    setBrightness(255);
    
    if (!detecting) startDetector();
  }
  if (detecting) {
    while (true) {
      curTime = millis();
    
      // Check detection window
      if (curTime - prevTime >= scanTime) {
        prevTime = curTime;
      
        // Print detection count
        Serial.println("Deauth frames: " + String(deauthCount) + " - Channel: " + String(curChannel));
      
        // Trigger alert if threshold reached
        if (deauthCount >= 1) { // Lowered threshold for higher sensitivity
          if (invertAlertPin) setBrightness(255);
          else setBrightness(255 - LED_brightness);
          lastDetectionTime = curTime;
          stayOnChannel = true; // Stay on this channel after detection
        } else {
          if (invertAlertPin) setBrightness(255 - LED_brightness);
          else setBrightness(255);
        }
      
        // Reset counter
        deauthCount = 0;
      
        // Channel hopping with conditional stay
        if (stayOnChannel) {
          if (curTime - lastDetectionTime < stayOnChannelTime) {
            Serial.println("Staying on channel " + String(curChannel) + " due to recent detection");
          } else {
            stayOnChannel = false;
            curChannel++;
            if (curChannel > 13) curChannel = 1; // ESP32-C3 supports channels 1-13
            esp_wifi_set_channel(curChannel, WIFI_SECOND_CHAN_NONE);
            Serial.println("Switching to channel " + String(curChannel));
          }
        } else {
          curChannel++;
          if (curChannel > 13) curChannel = 1; // ESP32-C3 supports channels 1-13
          esp_wifi_set_channel(curChannel, WIFI_SECOND_CHAN_NONE);
          Serial.println("Switching to channel " + String(curChannel));
        }
      }
    }
  }
}
