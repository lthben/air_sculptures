/*
  Author: Benjamin Low (Lthben@gmail.com)
  Date: Oct 2019
  Description: 
      Teensy 3.2 with audio shield. 
      Sculpture 1, 2 and 3 have two buttons and one distance sensor each.
      The two buttons play back the two sets of air measurements readings translated into brightness values. 
      There is one strip of leds per set of reading, therefore two strips per sculpture.
      The distance sensor changes the hue of the leds in real time. 
      There are sounds associated with all light animations, including a pulsing idle animation. 
*/

#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce.h>
#include <FastLED.h>

//-------------------- USER DEFINED SETTINGS --------------------//

//Uncomment one below
// #define __PM25__ 
// #define __CO2__
// #define __VOC__

const int pm25weslyn[20] = { 118, 38, 34, 111, 125, 82, 178, 174, 43, 43, 42, 83, 63, 83, 85, 103, 68, 53, 54, 66 };
const int pm25susan[32] = { 65, 88, 44, 42, 73, 69, 70, 61, 54, 89, 86, 91, 60, 63, 92, 88, 95, 55, 85, 49, 48, 51, 35, 38, 49, 51, 21, 32, 28, 42, 21, 25 };
const int co2chan[17] = { 1609, 577, 406, 419, 443, 414, 403, 413, 409, 411, 412, 409, 423, 414, 421, 434, 421 };
const int co2charlene[40] = { 1685, 642, 618, 698, 697, 778, 450, 664, 648, 676, 425, 504, 550, 481, 640, 942, 1791, 504, 733, 688, 592, 608, 850, 779, 1876, 646, 648, 659, 893, 422, 455, 701, 716, 892, 1046, 455, 483, 503, 448, 550 };

void setup() {
}

void loop() {
}