
/*--------------------------------------------------------------------------------
  Reads the two buttons and distance sensor. Dist sensor changes hue of both led strips.
--------------------------------------------------------------------------------*/
void read_console()
{
  if (strip1playMode == IDLE_MODE)
  {
    button0.update();

    if (button0.fallingEdge())
    {
      isButton0Pressed = true;
      Serial.println("button0 pressed");
    }
  }
  if (strip2playMode == IDLE_MODE)
  {
    button1.update();

    if (button1.fallingEdge())
    {
      isButton1Pressed = true;
      Serial.println("button1 pressed");
    }
  }

  if (loxmsec > 100)
  {
    VL53L0X_RangingMeasurementData_t measure;

    // Serial.print("Reading a measurement... ");
    lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

    if (measure.RangeStatus != 4)
    { // phase failures have incorrect data
      rangeVal = measure.RangeMilliMeter;

      if (rangeVal > 1000)
      {
        isUserPresent = false;
      }
      else
      {
        isUserPresent = true;
      }
      // Serial.print("Distance (mm): "); Serial.println(rangeVal);
    }
    else
    {
      // Serial.println(" out of range ");
      isUserPresent = false;
    }

    loxmsec = 0; //refresh timer for next reading
  }
}

/*--------------------------------------------------------------------------------
  Changes both led strips colour in real time according to the dist sensor
--------------------------------------------------------------------------------*/
void do_colour_variation()
{
  if (isUserPresent == true)
  {
    strip1Color.hue = map(rangeVal, 20, 400, 76, 204);
    strip2Color.hue = map(rangeVal, 20, 400, 76, 204);
  }
  else
  {
    if (strip1playMode == IDLE_MODE)
    {
      strip1Color = idleColor;
    }
    else if (strip1playMode == BUTTON_MODE)
    {
      strip1Color = activeColor;
    }
    if (strip2playMode == IDLE_MODE)
    {
      strip2Color = idleColor;
    }
    else if (strip2playMode == BUTTON_MODE)
    {
      strip2Color = activeColor;
    }
  }
}

/*--------------------------------------------------------------------------------
  Done once during setup(). Translates the raw data readings into brightness values.
--------------------------------------------------------------------------------*/
void register_readings()
{
  if (SCULPTURE_ID == 1)
  {
    for (int i = 0; i < 17; i++)
    {
      readings1[i] = int(map(CO2_1[i], 0, 1800, 64, 255));
    }
    for (int i = 0; i < 40; i++)
    {
      readings2[i] = int(map(CO2_2[i], 0, 1800, 64, 255));
    }
  }
  else if (SCULPTURE_ID == 2)
  {
    for (int i = 0; i < 20; i++)
    {
      readings1[i] = int(map(PM25_1[i], 0, 125, 64, 255));
    }
    for (int i = 0; i < 32; i++)
    {
      readings2[i] = int(map(PM25_2[i], 0, 125, 64, 255));
    }
  }
  else //sculpture 3
  {
    for (int i = 0; i < 26; i++)
    {
      readings1[i] = int(map(VOC_1[i], 0, 130, 80, 255));
    }
    for (int i = 0; i < 22; i++)
    {
      readings2[i] = int(map(VOC_2[i], 0, 130, 80, 255));
    }
  }
}

/*--------------------------------------------------------------------------------
  Toggles the playMode according to button press
--------------------------------------------------------------------------------*/
void set_playMode()
{
  if (isButton0Pressed == true) //process button press
  {
    isButton0Pressed = false; //listen again for button presses
    strip1playMode = BUTTON_MODE;
    strip1hasPlayModeChanged = true; //trigger sound change
    Serial.println("strip1 : BUTTON MODE");

    strip1activeLedState = 0;         //reset the led if currently active
    strip1bandDelay = BAND_DELAY / 4; //speed up the fade animation
  }

  if (isButton1Pressed == true) //process button press
  {
    isButton1Pressed = false; //listen again for button presses
    strip2playMode = BUTTON_MODE;
    strip2hasPlayModeChanged = true; //trigger sound change
    Serial.println("strip2 : BUTTON MODE");

    strip2activeLedState = 0;         //reset the led if currently active
    strip2bandDelay = BAND_DELAY / 4; //speed up the fade animation
  }
}

/*--------------------------------------------------------------------------------
  led strip support functions
--------------------------------------------------------------------------------*/
void strip1_fade()
{
  for (int i = 0; i < BAND1; i++)
  {
    leds0[i].fadeToBlackBy(8);
  }
}

void strip2_fade()
{
  for (int i = 0; i < BAND2; i++)
  {
    leds1[i].fadeToBlackBy(8);
  }
}

void strip1_set_brightLevel(int brightlvl)
{
  strip1Color.val = brightlvl;

  for (int i = 0; i < BAND1; i++)
  {
    leds0[i] = strip1Color;
  }
}

void strip2_set_brightLevel(int brightlvl)
{
  strip2Color.val = brightlvl;

  for (int i = 0; i < BAND2; i++)
  {
    leds1[i] = strip2Color;
  }
}

bool strip1_has_fade()
{
  if (leds0[0].getAverageLight() == 0 && leds0[BAND1 - 1].getAverageLight() == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool strip2_has_fade()
{
  if (leds1[0].getAverageLight() == 0 && leds1[BAND2 - 1].getAverageLight() == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/*--------------------------------------------------------------------------------
  Tracks fade animation bright levels
--------------------------------------------------------------------------------*/
int strip1_get_brightness(int _brightness)
{
  if (!strip1isMaxBrightness)
  {
    _brightness++;
    if (_brightness > strip1maxBrightLvl)
      _brightness = strip1maxBrightLvl;
    return _brightness;
  }
  else //reached max brightness
  {
    _brightness--;
    if (_brightness < 0)
      _brightness = 0;
    return _brightness;
  }
}

int strip2_get_brightness(int _brightness)
{
  if (!strip2isMaxBrightness)
  {
    _brightness++;
    if (_brightness > strip2maxBrightLvl)
      _brightness = strip2maxBrightLvl;
    return _brightness;
  }
  else //reached max brightness
  {
    _brightness--;
    if (_brightness < 0)
      _brightness = 0;
    return _brightness;
  }
}

/*--------------------------------------------------------------------------------
  Idle fade animation
--------------------------------------------------------------------------------*/
void strip1_idle_animation()
{
  int brightlevel = strip1_get_brightness(strip1brightness);
  strip1Color.val = strip1brightness = brightlevel;

  for (int i = 0; i < BAND1; i++)
  {
    leds0[i] = strip1Color;
  }
  if (brightlevel == strip1maxBrightLvl)
  {
    strip1isMaxBrightness = true;
  }
  else if (brightlevel == 0)
  {
    strip1isMaxBrightness = false;
  }
}

void strip2_idle_animation()
{
  int brightlevel = strip2_get_brightness(strip2brightness);
  strip2Color.val = strip2brightness = brightlevel;

  for (int i = 0; i < BAND2; i++)
  {
    leds1[i] = strip2Color;
  }
  if (brightlevel == strip2maxBrightLvl)
  {
    strip2isMaxBrightness = true;
  }
  else if (brightlevel == 0)
  {
    strip2isMaxBrightness = false;
  }
}

/*--------------------------------------------------------------------------------
  Prep to go to idle mode
--------------------------------------------------------------------------------*/
void strip1_go_idle()
{
  strip1activeLedState = 0; //go to idle state
  strip1playMode = IDLE_MODE;
  strip1hasPlayModeChanged = true; //trigger sound change
  strip1bandDelay = BAND_DELAY;
  strip1maxBrightLvl = 255;
  Serial.println("strip1 : IDLE MODE");
  strip1isMaxBrightness = false;
  strip1brightness = 0;
  strip1bandms = 0;
  // sgtl5000_1.volume(0.5); //uncomment when audio added
}

void strip2_go_idle()
{
  strip2activeLedState = 0; //go to idle state
  strip2playMode = IDLE_MODE;
  strip2hasPlayModeChanged = true; //trigger sound change
  strip2bandDelay = BAND_DELAY;
  strip2maxBrightLvl = 255;
  Serial.println("strip 2: IDLE MODE");
  strip2isMaxBrightness = false;
  strip2brightness = 0;
  strip2bandms = 0;
  // sgtl5000_1.volume(0.5); //uncomment when audio added
}

/*--------------------------------------------------------------------------------
  plays back readings
--------------------------------------------------------------------------------*/
void strip1_playback_readings()
{
  if (strip1activeLedState == 0) //dim the lights
  {
    strip1_fade();

    if (strip1_has_fade() == true)
    {
      strip1activeLedState = 1;
      strip1bandms = 0;
      strip1readingsCounter = 0;
      strip1currBrightVal = strip1prevBrightVal = 0;
      strip1Color.val = 0;
    }
  }
  else if (strip1activeLedState == 1)
  {
    if (strip1bandms < BAND_DELAY * 2) //control the speed of the fade animation here
    {
      strip1currBrightVal = readings1[strip1readingsCounter];

      if (strip1currBrightVal > strip1prevBrightVal)
      {
        if (strip1Color.val < strip1currBrightVal)
        {
          strip1Color.val += 10; //brighten
        }
      }
      else //curr is < prev bright val
      {
        if (strip1Color.val > strip1currBrightVal)
        {
          strip1Color.val -= 10; //dim
        }
      }

      strip1_set_brightLevel(strip1Color.val);
    }
    else //go to next bright value
    {
      strip1prevBrightVal = strip1currBrightVal;
      strip1readingsCounter++;

      Serial.print("strip1readingsCounter: ");
      Serial.print(strip1readingsCounter);
      Serial.print("\t strip1currBrightVal: ");
      Serial.println(strip1currBrightVal);

      strip1bandms = 0; //need to reset here

      unsigned int numElements = sizeof(readings1) / sizeof(readings1[0]);

      if (strip1readingsCounter == numElements)
      {
        strip1activeLedState = 2; //go to next state
      }
    }
  }
  else if (strip1activeLedState == 2)
  {
    strip1_fade();

    if (strip1_has_fade())
    {
      strip1_go_idle();
    }
  }
}

void strip2_playback_readings()
{
  if (strip2activeLedState == 0) //dim the lights
  {
    strip2_fade();

    if (strip2_has_fade() == true)
    {
      strip2activeLedState = 1;
      strip2bandms = 0;
      strip2readingsCounter = 0;
      strip2currBrightVal = strip2prevBrightVal = 0;
      strip2Color.val = 0;
    }
  }
  else if (strip2activeLedState == 1)
  {
    if (strip2bandms < BAND_DELAY * 2) //control the speed of the fade animation here
    {
      strip2currBrightVal = readings2[strip2readingsCounter];

      if (strip2currBrightVal > strip2prevBrightVal)
      {
        if (strip2Color.val < strip2currBrightVal)
        {
          strip2Color.val += 10; //brighten
        }
      }
      else //curr is < prev bright val
      {
        if (strip2Color.val > strip2currBrightVal)
        {
          strip2Color.val -= 10; //dim
        }
      }

      strip2_set_brightLevel(strip2Color.val);
    }
    else //go to next bright value
    {
      strip2prevBrightVal = strip2currBrightVal;
      strip2readingsCounter++;

      Serial.print("strip2readingsCounter: ");
      Serial.print(strip2readingsCounter);
      Serial.print("\t strip2currBrightVal: ");
      Serial.println(strip2currBrightVal);

      strip2bandms = 0; //need to reset here

      unsigned int numElements = sizeof(readings2) / sizeof(readings2[0]);

      if (strip2readingsCounter == numElements)
      {
        strip2activeLedState = 2; //go to next state
      }
    }
  }
  else if (strip2activeLedState == 2)
  {
    strip2_fade();

    if (strip2_has_fade())
    {
      strip2_go_idle();
    }
  }
}

/*--------------------------------------------------------------------------------
  Selects the tracks according to playMode
--------------------------------------------------------------------------------*/
void play_audio()
{
  if (strip1playMode == IDLE_MODE && strip2playMode == IDLE_MODE)
  {
    if ((strip1hasPlayModeChanged == true || strip2hasPlayModeChanged) && playSdWav1.isPlaying() == true)
    {
      playSdWav1.stop();
      playSdWav1.play(idleTrack);
      delay(10);
      Serial.print("Start playing ");
      Serial.println(idleTrack);
      strip1hasPlayModeChanged = strip2hasPlayModeChanged = false;
    }
    else if (playSdWav1.isPlaying() == false)
    {
      playSdWav1.play(idleTrack);
      delay(10);
      Serial.print("Start playing ");
      Serial.println(idleTrack);
    }
  }
  else if (strip1playMode == BUTTON_MODE || strip2playMode == BUTTON_MODE)
  {
    if ((strip1hasPlayModeChanged == true || strip2hasPlayModeChanged == true) && playSdWav1.isPlaying() == true)
    {
      playSdWav1.stop();
      playSdWav1.play(activeTrack);
      delay(10);
      Serial.print("Start playing ");
      Serial.println(activeTrack);
      strip1hasPlayModeChanged = strip2hasPlayModeChanged = false;
    }
    else if (playSdWav1.isPlaying() == false)
    {
      playSdWav1.play(activeTrack);
      delay(10);
      Serial.print("Start playing ");
      Serial.println(activeTrack);
    }
  }
}

/*--------------------------------------------------------------------------------
  add glitter
--------------------------------------------------------------------------------*/
void add_glitter()
{
  if (SCULPTURE_ID == 3)
  {
    if (random8() < 55) //random8() returns a rand num from 0 - 255
    {
      if (strip1playMode == IDLE_MODE)
        leds0[random16(VOCband1)] += CRGB::White;
      if (strip2playMode == IDLE_MODE)
        leds1[random16(VOCband2)] += CRGB::White;
    }
  }
}
