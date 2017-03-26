
/**
 * Check if i2c modules are up and running
 */
bool checkI2cModules() {
  return (rtc.isrunning() && bmp.begin());
}

/**
 * Set an ambient color for all leds at once
 * @param color The color to use
 */
void setAmbientColor(uint32_t color) {
  static uint32_t oldColor = 1;
  static byte j = 0;
  byte i = 0;

  //display rainbow for cycler mode in bottom led
  if (isCycling) {
    i = 1;
    strip.setPixelColor(0, Wheel((j) & 255));
    j++ % 256;
  }  
  for (i; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

/**
 * Blink while blocking everything else.
 *
 * @param color The color to use
 * @param times Number of times to blink
 */
void blockingAmbientBlink(uint32_t color, byte times) {
  for (byte i = 0; i < times; i++) {
    setAmbientColor(color);
    delay(150);
    setAmbientColor(strip.Color(0, 0, 0));
    delay(150);
  }
}

/**
 *  Pulse the ambient light, the pulse is linear
 *  @pulseDuration The total duration of one pulse consisting of a 50% on and a 50% off cycle
 */
void ambientPulse(unsigned short pulseDuration) {
  static unsigned short brightness = 0;
  static unsigned int elapsedDuration = 0;
  unsigned int halfDuration = pulseDuration >> 1;  

  if(elapsedDuration > pulseDuration) {
    elapsedDuration = 0;
  } else if (elapsedDuration < halfDuration ) {
    //raising
    brightness = map(elapsedDuration, 0, halfDuration, 0, 255);
  } else {
    //falling
    brightness = map(elapsedDuration, halfDuration, pulseDuration, 255, 0);
  }

  elapsedDuration += SMAmbient.Statetime(); //last enter or re-enter
  strip.setBrightness(brightness);
  strip.show();
}

/**
 * Pulse the tube light, the pulse is binary, e.g. on/off 
 * @param pulseDuration The total duration of one pulse consisting of a 50% on and a 50% off cycle
 */
void tubePulse(unsigned short pulseDuration) {
  static unsigned short brightness = 0x20;
  static unsigned int elapsedDuration = 0;
  unsigned int halfDuration = pulseDuration >> 1;  

  if(elapsedDuration > pulseDuration) {
    elapsedDuration = 0;
  } else if (elapsedDuration < halfDuration ) {
    //raising
    brightness = 0x20;
  } else {
    //falling
    brightness = 0;
  }

  elapsedDuration += SMDisplay.Statetime(); //last enter or re-enter  
  tube.setBrightness(brightness);
}

/**
 * Pulse the tubes if the values changed for a duration of 600ms.
 * Note: The oldValue pointer gets updated to newValue after the timeout period.
 * @param oldValue The old value to watch
 * @param newValue The new value
 */
void pulseTubeIfChanged(short *oldValue, short newValue) {
  static unsigned int elapsedDuration = 0;
  if(*oldValue != newValue) {      
      tubePulse(600);
      elapsedDuration += SMDisplay.Statetime();
      if(elapsedDuration > PULSE_ON_NEW_VALUES_TIMEOUT) {
        elapsedDuration = 0;
        *oldValue = newValue;
        tube.setBrightness(0x20);
      }     
  }  
}

/**
 * Colorwheel
 * Input a value 0 to 255 to get a color value.
 * The colours are a transition r - g - b - back to r.
 * @param wheelPos The position of the wheel
 */
uint32_t Wheel(byte wheelPos) {
  wheelPos = 255 - wheelPos;
  if (wheelPos < 85) {
    return strip.Color(255 - wheelPos * 3, 0, wheelPos * 3);
  }
  if (wheelPos < 170) {
    wheelPos -= 85;
    return strip.Color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
  wheelPos -= 170;
  return strip.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
}

/**
 * Display a rainbow, call repeatedly for each iteration
 */
void rainbow() {
  static byte j = 0;
  
  for (byte i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel((i + j) & 255));
  }
  j++ % 256;
  strip.show();
}

/**
 * Get data from temboo - all data is collected running the combined choreo
 */
void getTembooData() {  
  TembooChoreo GetCombinedChoreo;

  GetCombinedChoreo.begin();
  GetCombinedChoreo.setAccountName(TEMBOO_ACCOUNT);
  GetCombinedChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  GetCombinedChoreo.setAppKey(TEMBOO_APP_KEY);
  GetCombinedChoreo.setChoreo(F("/Choreos/CollectedChoreos"));
  GetCombinedChoreo.run();
  while (GetCombinedChoreo.available()) {
    String name = GetCombinedChoreo.readStringUntil('\x1F');
    name.trim(); // use “trim” to get rid of newlines
    String data = GetCombinedChoreo.readStringUntil('\x1E');
    data.trim(); // use “trim” to get rid of newlines
    if (name == F("totalEmailCount")) {      
      mailCount = data.toInt();
    } else if (name == F("totalJiraTickets")) {      
      jiraCount = data.toInt();
    } else if (name == F("totalMergeRequests")) {      
      gitCount = data.toInt();
    } else if (name == F("currentTime")) {          
      updateTime(data);      
    }
  }
  GetCombinedChoreo.close();
}

/**
 * Update the rtc clock to adjust for daylight saving time
 * @param data the current date as string in the format "HH:mm yy-MM-dd"

 */
void updateTime(String data) {
  int hour = data.substring(0,2).toInt();      
  int minute = data.substring(3,2).toInt();  
  int year = data.substring(6,2).toInt();  
  int month = data.substring(9,2).toInt();  
  int day = data.substring(12,2).toInt();  
      
  DateTime now = rtc.now();

  //adjust if HH::mm or yy-MM-dd mismatch
  // the folowing addition was skipped because of size: || (now.year() != year && now.month() != month && day != now.day()) 
  if (now.hour() != hour && now.minute() != minute ) {               
    Console.println(data);
    rtc.adjust(DateTime(year, month, day, hour, minute, 0));
  }  
}

/**
 * Check if a user interaction occured. This is based on a detection threshold.
 *
 * @return bool true if threshold met or exeeded
 */
bool detect() {
  if (analogRead(DETECTION_PIN) >= DETECTION_DISTANCE_THRESHOLD) {
    return true;
  }
  return false;
}

/**
 * Display a random number and color in each tube.
 */
void displayRandomNumbers() {
  for (byte i = 0; i < NIXIE_COUNT; i++)
  {    
    tube.setBackgroundColor(i, (Color)random(0, 7));
    tube.setNumber(i, random(0, 9));
  }
  tube.display();
}
