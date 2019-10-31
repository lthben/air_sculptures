
/*--------------------------------------------------------------------------------
  Reads the two buttons and distance sensor 
--------------------------------------------------------------------------------*/
void read_console()
{
  button0.update();
  button1.update();

  if (button0.fallingEdge())
  {
    isButtonPressed = true;
    Serial.println("button0 pressed");
  }
  if (button1.fallingEdge())
  {
    isButtonPressed = true;
    Serial.println("button1 pressed");
  }

  if (loxmsec > 100)
  {
    VL53L0X_RangingMeasurementData_t measure;

    Serial.print("Reading a measurement... ");
    lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

    if (measure.RangeStatus != 4) 
    {  // phase failures have incorrect data
      int rangeVal = measure.RangeMilliMeter;
      Serial.print("Distance (mm): "); Serial.println(rangeVal);
    } 
    else 
    {
      Serial.println(" out of range ");
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
    for (int i = 0; i < 20; i++)
    {
      readings1[i] = PM25_1[i];
    }
    for (int i = 0; i < 32; i++)
    {
      readings2[i] = PM25_2[i];
    }
  }
  else if (SCULPTURE_ID == 2)
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