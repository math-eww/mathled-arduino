/*
 * Math's LED kit
 */

//Library
#include <FastLED.h>
//Color definitions
#include "WS2812_Definitions.h"
#include "PaletteDefinitions.h"
//Globals
#include "Globals.h"
//Serial Communication
#include "SerialComm.h"

//Constants
#define LED_COUNT 60
#define DATA_PIN 4
#define BRIGHTNESS  128

#define qsubd(x, b) ((x>b)?b:0)                               // Clip. . . . A digital unsigned subtraction macro. if result <0, then x=0. Otherwise, x=b.
#define qsuba(x, b) ((x>b)?x-b:0)                             // Level shift. . . Unsigned subtraction macro. if result <0, then x=0. Otherwise x=x-b.

//MATHLED Global Variables
CRGB leds[LED_COUNT];
unsigned long lastMillis = 0;
CRGBPalette16 currentPalette = CRGBPalette16(CRGB::Black);
CRGBPalette16 targetPalette = GMT_hot_gp;//RainbowColors_p;
TBlendType    currentBlending = LINEARBLEND;
uint8_t index = 0;

//Globals initialization
//LED
bool onoff = true;
uint8_t now_playing = 0;
uint16_t delayMillis = 50;
uint8_t delta = 1;
uint8_t rate = 10;
uint8_t hue = 190;
uint8_t saturation = 255;
uint8_t value = 255;

void setup() {
  //Serial
  Serial.begin(57600);
  //Serial.println("MATHLED Online");
  //debugToPC("MATHLED Online");
  //LEDs
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, LED_COUNT);
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.setMaxPowerInVoltsAndMilliamps(5,500);
}

void loop() {
  //First, get instructions from serial
  readSerialComm();
  //Then, check if should update LEDs based on lastMillis compared to current
  unsigned long currentMillis = millis();
  if (currentMillis - lastMillis >= delayMillis) {
    lastMillis = currentMillis;
    if (onoff) {
      switch (now_playing) {
        case 0:
          solid();
          break;
        case 1:
          rainbow();
          break;
        case 2:
          pulse();
          break;
        case 3:
          twinkle();
          break;
        case 4:
          plasma();
          break;
        case 5:
          dot_beat();
          break;
        case 6:
          inoise8_fire();
          break;
        default: 
          rainbow();
          break;
      }
    }
    FastLED.show();
    //debugToPC(LEDS.getFPS());
  }
  //FASTLED function to run code every x ms
  EVERY_N_MILLISECONDS(100) {
    nblendPaletteTowardPalette(currentPalette, targetPalette, 24);   // AWESOME palette blending capability.
  }
}

//Serial Communication - calls functions in SerialComm
void readSerialComm() {
  getSerialData();
  processData();
}

// Prints a colour on the ENTIRE LED strip.
void solid() {
  fill_solid (leds, LED_COUNT, CHSV(hue,saturation,value));
}

// Prints a rainbow on the ENTIRE LED strip.
// The rainbow begins at a specified hue. 
// ROY G BIV!
void rainbow() 
{
  fill_rainbow(leds, LED_COUNT, hue, delta);
  hue++;
}

// Prints a 'pulse' that moves up and down
// the LED strip.
void pulse() {
  fadeToBlackBy( leds, LED_COUNT, 32);
  int pos1 = beatsin16(rate,0,LED_COUNT);
  leds[pos1] += CHSV(hue,saturation,value); //ColorFromPalette(currentPalette, delta, 255, currentBlending);
}

// Lights random leds like twinkling stars
void twinkle() {
  if (random8() < rate) leds[random16(LED_COUNT)] += CHSV(hue,saturation,value); //ColorFromPalette(currentPalette, delta, 255, currentBlending);
  fadeToBlackBy(leds, LED_COUNT, 8);
}

// Plasma wave thing
void plasma() {                                                 // This is the heart of this program. Sure is short. . . and fast.
  int thisPhase = beatsin8(rate,-64,64);                           // Setting phase change for a couple of waves.
  int thatPhase = beatsin8(rate+1,-64,64);
  for (int k=0; k<LED_COUNT; k++) {                              // For each of the LED's in the strand, set a brightness based on a wave as follows:
    int colorIndex = cubicwave8((k*23)+thisPhase)/2 + cos8((k*15)+thatPhase)/2;           // Create a wave and add a phase change and add another wave with its own phase change.. Hey, you can even change the frequencies if you wish.
    int thisBright = qsuba(colorIndex, beatsin8(7,0,96));                                 // qsub gives it a bit of 'black' dead space by setting sets a minimum value. If colorIndex < current value of beatsin8(), then bright = 0. Otherwise, bright = colorIndex..
    leds[k] = ColorFromPalette(currentPalette, colorIndex, thisBright, currentBlending);  // Let's now add the foreground colour.
  }
}

//Bouncing dots
void dot_beat() {
  uint8_t inner = beatsin8(rate, LED_COUNT/4, LED_COUNT/4*3);    // Move 1/4 to 3/4
  uint8_t outer = beatsin8(rate, 0, LED_COUNT-1);               // Move entire length
  uint8_t middle = beatsin8(rate, LED_COUNT/3, LED_COUNT/3*2);   // Move 1/3 to 2/3
  leds[middle] = CHSV(hue - delta, saturation, value);
  leds[inner] = CHSV(hue, saturation, value);
  leds[outer] = CHSV(hue + delta, saturation, value);
  nscale8(leds,LED_COUNT,200);                             // Fade the entire array. Or for just a few LED's, use  nscale8(&leds[2], 5, fadeval);
}

//Fire
void inoise8_fire() {
  for(int i = 0; i < LED_COUNT; i++) {
    index = inoise8(i*20,millis()*3*LED_COUNT/255);                                           // X location is constant, but we move along the Y at the rate of millis()
    leds[i] = ColorFromPalette(currentPalette, min(i*(index)>>6, 255), i*255/LED_COUNT, LINEARBLEND);  // With that value, look up the 8 bit colour palette value and assign it to the current LED.
  }                                                                                              // Also, the higher the value of i => the brighter the LED.
}
