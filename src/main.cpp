/*
  Author: Benjamin Low (Lthben@gmail.com)
  Date: Oct 2019
  Description: 
      Teensy 3.2 with audio shield. 
      Sculpture 1, 2 and 3 have two buttons and one distance sensor each.
      The two buttons play back the two sets of air measurements readings translated into brightness values. 
      One button represents one set of reading and one strip of led.
      There is an idle mode pulsing light animation. Active mode is triggered by button and will show a sequence of brightness values. 
      The distance sensor changes the hue of the leds in real time. 
      There is a sound for idle mode and one for active playback mode. 
*/

#include <Arduino.h>
#include <Adafruit_VL53L0X.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>
#include <FastLED.h>

//-------------------- USER DEFINED SETTINGS --------------------//

//Uncomment one below
#define __CO2__
// #define __PM25__ 
// #define __VOC__

const int CO2_1[17] = { 1609, 577, 406, 419, 443, 414, 403, 413, 409, 411, 412, 409, 423, 414, 421, 434, 421 };
const int CO2_2[40] = { 1685, 642, 618, 698, 697, 778, 450, 664, 648, 676, 425, 504, 550, 481, 640, 942, 1791, 504, 733, 688, 592, 608, 850, 779, 1876, 646, 648, 659, 893, 422, 455, 701, 716, 892, 1046, 455, 483, 503, 448, 550 };

const int PM25_1[20] = { 118, 38, 34, 111, 125, 82, 178, 174, 43, 43, 42, 83, 63, 83, 85, 103, 68, 53, 54, 66 };
const int PM25_2[32] = { 65, 88, 44, 42, 73, 69, 70, 61, 54, 89, 86, 91, 60, 63, 92, 88, 95, 55, 85, 49, 48, 51, 35, 38, 49, 51, 21, 32, 28, 42, 21, 25 };

const int VOC_1[26] = { 8, 11, 5, 13, 16, 14, 15, 17, 15, 20, 29, 21, 22, 19, 14, 13, 19, 25, 17, 15, 13, 17, 16, 15, 20, 17 };
const int VOC_2[22] = { 122, 67, 24, 36, 46, 32, 29, 34, 27, 25, 22, 23, 19, 23, 21, 33, 26, 34, 41, 15, 25, 18 };

const int CO2band1 = 50, CO2band2 = 50, PM25band1 = 50, PM25band2 = 50, VOCband1 = 50, VOCband2 = 50; //num of pixels per strip. Each pixel is 10cm.

CHSV cblue(140,255,255);
const int BAND_DELAY = 500;   //controls led animation speed

//-------------------- Audio --------------------//

// GUItool: begin automatically generated code. See https://www.pjrc.com/teensy/gui/index.html
AudioPlaySdWav playSdWav1; //xy=416,186
AudioOutputI2S i2s1;       //xy=821,189
AudioConnection patchCord1(playSdWav1, 0, i2s1, 0);
AudioConnection patchCord2(playSdWav1, 1, i2s1, 1);
AudioControlSGTL5000 sgtl5000_1; //xy=615,336
// GUItool: end automatically generated code

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN 10
#define SDCARD_MOSI_PIN 7
#define SDCARD_SCK_PIN 14

float vol = 0.7; //master volume gain 0.0 - 1.0

const char *idleTrack = "DRONE1.WAV"; const char *activeTrack = "DRONE2.WAV";

//-------------------- Buttons and distance sensor --------------------//

Bounce button0 = Bounce(0, 15); // 15 = 15 ms debounce time
Bounce button1 = Bounce(1, 15);

bool isButton0Pressed, isButton1Pressed; //track response to button triggered

/*
Adafruit_VL53L0X lox = Adafruit_VL53L0X(); //SDL to 19 and SDA to 18
int rangeVal; //reading in mm
elapsedMillis loxmsec; //to track that it takes measurement at an interval of around 100ms instead of continuously
*/
//-------------------- Light --------------------//
#define STRIP1PIN 4
#define STRIP2PIN 5

#define LED_TYPE UCS1903
#define COLOR_ORDER GRB //Yes! GRB!

CHSV strip1Color = cblue;
CHSV strip2Color = cblue;

#if defined(__CO2__)
const int BAND1 = CO2band1, BAND2 = CO2band2;
const int SCULPTURE_ID = 1;
int readings1[17], readings2[40];

#elif defined(__PM25__)
const int BAND1 = PM25band1, BAND2 = PM25band2; 
const int SCULPTURE_ID = 2;
int readings1[20], readings2[32];

#elif defined(__VOC__)
const int BAND1 = VOCband1, BAND2 = VOCband2;
const int SCULPTURE_ID = 3;
int readings1[26], readings2[22];

#else
#error "invalid sculpture ID"
#endif

#define UPDATES_PER_SECOND 100 //speed of light animation
const int IDLE_MODE = 1, BUTTON_MODE = 2;
unsigned int strip1playMode = IDLE_MODE, strip2playMode = IDLE_MODE; 

CRGB leds0[BAND1]; CRGB leds1[BAND2];

int strip1brightness = 0, strip2brightness = 0; //band brightness
int strip1maxBrightLvl = 255, strip2maxBrightLvl = 255; //variable max brightness
bool strip1hasPlayModeChanged = false, strip2hasPlayModeChanged = false; //for audio track changes
int strip1activeLedState = 0, strip2activeLedState = 0;            //to track led animaton states, e.g. 0 - idle mode, start fade to black 1 - show brightness according to reading, 2 - has completed animations, fade to black and idle
bool strip1isMaxBrightness = false, strip2isMaxBrightness = false;      //to track idle animation direction
elapsedMillis strip1bandms, strip2bandms;              //multiple use time ellapsed counter
unsigned int strip1bandDelay = BAND_DELAY, strip2bandDelay = BAND_DELAY; //speed of fade animation
unsigned int strip1readingsCounter, strip2readingsCounter;                 //keeps track of indexing the readings array
unsigned int strip1prevBrightVal, strip1currBrightVal, strip2prevBrightVal, strip2currBrightVal;    //for comparing prev and current values for dimming and brightening

#include "myfunctions.h" //supporting functions

//-------------------- Setup --------------------//
void setup() 
{
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);

  Serial.begin(9600);
  
  /*
  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  */

  AudioMemory(8);

  sgtl5000_1.enable();
  sgtl5000_1.volume(vol);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN)))
  {
    while (1)
    {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  delay(2000); //power up safety delay

  FastLED.addLeds<LED_TYPE, STRIP1PIN, COLOR_ORDER>(leds0, BAND1);
  FastLED.addLeds<LED_TYPE, STRIP2PIN, COLOR_ORDER>(leds1, BAND2);

  FastLED.setBrightness(255);

  delay(10);

  register_readings(); //translate the air measurement data points into a readings[] brightness value array

  Serial.println("start");
}

//-------------------- Loop --------------------//
void loop() 
{
  read_console(); //gets input from dist sensor and buttons

  // do_colour_variation(); //changes hue of both strips according to dist sensor

  set_playMode();

  if (strip1playMode == IDLE_MODE)
  {
    strip1_idle_animation();
  }
  else if (strip1playMode == BUTTON_MODE)
  {
    strip1_playback_readings(); //play brightness sequence according to readings[] array
  }

  if (strip2playMode == IDLE_MODE)
  {
    strip2_idle_animation();
  } 
  else if (strip2playMode == BUTTON_MODE)
  {
    strip2_playback_readings(); //play brightness sequence according to readings[] array
  } 

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
  
  play_audio();
}

