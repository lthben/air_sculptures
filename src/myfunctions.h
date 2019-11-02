
/*--------------------------------------------------------------------------------
  Reads the two buttons and distance sensor 
--------------------------------------------------------------------------------*/
void read_console()
{
  button0.update();
  button1.update();

  if (button0.fallingEdge())
  {
    isButton0Pressed = true;
    Serial.println("button0 pressed");
  }
  if (button1.fallingEdge())
  {
    isButton1Pressed = true;
    Serial.println("button1 pressed");
  }

  if (loxmsec > 100)
  {
    VL53L0X_RangingMeasurementData_t measure;

    // Serial.print("Reading a measurement... ");
    lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

    if (measure.RangeStatus != 4) 
    {  // phase failures have incorrect data
      int rangeVal = measure.RangeMilliMeter;
      // Serial.print("Distance (mm): "); Serial.println(rangeVal);
    } 
    else 
    {
      // Serial.println(" out of range ");
    }

    loxmsec = 0; //refresh timer for next reading
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
      readings1[i] = CO2_1[i];
    }
    for (int i = 0; i < 40; i++)
    {
      readings2[i] = CO2_2[i];
    }
  }
  else if (SCULPTURE_ID == 2)
  {
    for (int i = 0; i < 20; i++)
    {
      readings1[i] = PM25_1[i];
    }
    for (int i = 0; i < 32; i++)
    {
      readings2[i] = PM25_2[i];
    }
  }
  else //sculpture 3
  {
    for (int i = 0; i < 26; i++)
    {
      readings1[i] = VOC_1[i];
    }
    for (int i = 0; i < 22; i++)
    {
      readings2[i] = VOC_2[i];
    }
  }
}

/*--------------------------------------------------------------------------------
  Toggles the playMode according to button press
--------------------------------------------------------------------------------*/
void check_playMode() 
{
  if (isButton0Pressed == true) //process button press
  {
    isButton0Pressed = false; //listen again for button presses
    strip1playMode = BUTTON_MODE;
    strip1hasPlayModeChanged = true; //trigger sound change
    Serial.println("strip1 : BUTTON MODE");

    strip1activeLedState = 0;          //reset the led if currently active
    strip1bandDelay = BAND_DELAY / 4; //speed up the fade animation
  }
}

/*--------------------------------------------------------------------------------
  led strip functions
--------------------------------------------------------------------------------*/
void fade_strip1()
{
  for (int i = 0; i<BAND1; i++)
  {
    leds0[i].fadeToBlackBy(8);
  }
}

void fade_strip2()
{
  for (int i = 0; i<BAND2; i++)
  {
    leds1[i].fadeToBlackBy(8);
  }
}

void set_strip1_brightLevel (int brightlvl)
{
  strip1Color.val = brightlvl;

  for (int i=0; i<BAND1; i++)
  {
    leds0[i] = strip1Color;
  }
}

void set_strip2_brightLevel (int brightlvl)
{
  strip2Color.val = brightlvl;

  for (int i=0; i<BAND2; i++)
  {
    leds1[i] = strip2Color;
  }
}

bool has_strip1_fade()
{
  if (leds0[0].getAverageLight() == 0 && leds0[BAND1].getAverageLight() == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool has_strip2_fade()
{
  if (leds1[0].getAverageLight() == 0 && leds1[BAND2].getAverageLight() == 0)
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

/*--------------------------------------------------------------------------------
  Idle fade animation
--------------------------------------------------------------------------------*/
void strip1_fade_animation()
{
  int brightlevel = strip1_get_brightness(strip1brightness);
  strip1Color.val = brightlevel;
  strip1brightness = brightlevel;
  for (int i=0; i<BAND1; i++)
  {
    leds0[i] = strip1Color;
  }
  if (brightlevel == strip1maxBrightLvl)
  {
    strip1isMaxBrightness = true;
    strip1bandms = 0;
  }
  if (brightlevel == 0)
  {
    strip1isMaxBrightness = false;
    strip1bandms = 0;
  }
  // Serial.println(strip1Color.val);
}

void strip2_fade_animation()
{

}

/*--------------------------------------------------------------------------------
  plays back readings
--------------------------------------------------------------------------------*/
void strip1_playback_readings()
{

}