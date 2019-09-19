/*
 * Description: Ardupilot external i2c leds fake NCP5623 device
 *
 * Author: ninja
 *
 * Date: 2019-09-19
 */
#include <Arduino.h>
#include <Wire.h>
#include "FastLED.h"

#define NCP5623_I2C_ADD   0x38

#define NUM_LEDS 3

#define DATA_PIN 9

#define TIMEOUT  1000   // 1s

CRGB leds[NUM_LEDS];

typedef struct _rgb_t{
  uint8_t r;
  uint8_t g;
  uint8_t b;
}rgb_t;

rgb_t Colors;
bool update;          // is update
uint32_t last_update; // last i2c communicate time


void receiveEvent(int x);
void led_loop();

void setup() {
  Wire.begin(NCP5623_I2C_ADD); 
  Wire.onReceive(receiveEvent);
  Serial.begin(57600); 
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  for(uint8_t i=0; i<NUM_LEDS; i++){
    leds[i] = CRGB::Green;
    FastLED.show();
  }
  delay(100);
  for(uint8_t i=0; i<NUM_LEDS; i++){
    leds[i] = CRGB::Black;
    FastLED.show();
  }
  memset(&Colors, 0, sizeof(rgb_t));
  update = false;
  last_update = 0;
}

void loop() {
  led_loop();
}

void led_loop(){
  if(millis() - last_update < TIMEOUT){
    if(update){
      update = false;
      for(uint8_t i=0; i<NUM_LEDS; i++){
        leds[i] = CRGB(Colors.r, Colors.g, Colors.b);
        FastLED.show();
      }
    }else{
      FastLED.show();
    }
  }else{
    for(uint8_t i=0; i<NUM_LEDS; i++){
      leds[i] = CRGB::Black;
      FastLED.show();
    }
  }
}

void receiveEvent(int x) {
  last_update = millis();
  update = true;
  while(Wire.available()){
    uint8_t val = Wire.read();
    switch(val & 0xE0){
      case 0:
        #if (DEBUG==1)
          Serial.println("off");
        #endif
          break;
      case 0x20:
        #if (DEBUG==1)
          Serial.println("max");
        #endif
          break;
      case 0x40:
        #if (DEBUG==1)
          Serial.print("PWM1 : ");
          Serial.println(val & 0x1F, HEX);
        #endif
          Colors.r = ((val & 0x1F) == 0xF)? 0xFF : 0 ;
          break;
      case 0x60:
        #if (DEBUG==1)
          Serial.print("PWM2 : ");
          Serial.println(val & 0x1F, HEX);
        #endif
          Colors.g = ((val & 0x1F) == 0xF)? 0xFF : 0 ;
          break;
      case 0x80:
        #if (DEBUG==1)
          Serial.print("PWM3 : ");
          Serial.println(val & 0x1F, HEX);
        #endif
          Colors.b = ((val & 0x1F) == 0xF)? 0xFF : 0 ;
          break;
      case 0xA0:
          Serial.println("Upward");
          break;
      case 0xC0:
          Serial.println("Downward");
          break;
      case 0xE0:
          Serial.println("Dim");
          break;
    }
  }
}