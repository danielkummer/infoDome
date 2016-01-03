/**
 *  Control the ambient leds based on the mode.
 *  Use the ambientSettings array to configure the state machine.
 */


State AmbientStartup() {  
  if(i2cModulesRunning) {
     if(SMAmbient.Timeout(50)){
        rainbow();    
        SMAmbient.Set(AmbientStartup);
      }    
  } else {
    //i2c problem - goto blink red state!
    SMAmbient.Set(AmbientBlinkRedOn);
  } 
}

State AmbientRun() {
  ambientSetting currentSetting = ambientSettings[currentMode];
  
  if(SMAmbient.Timeout(25)) {    
    if(currentSetting.pulse == 0) {
      strip.setBrightness(255);  
    } else {
      ambientPulse(currentSetting.pulse);  
    }
    setAmbientColor(currentSetting.color);    
    SMAmbient.Set(AmbientRun); 
  }
}

State AmbientBlinkRedOn() {
  Console.println(F("SMAmbient:AmbientBlinkRedOn"));
  setAmbientColor(strip.Color(255, 0, 0));
  if(SMAmbient.Timeout(400)) SMAmbient.Set(AmbientBlinkRedOff);  
}

State AmbientBlinkRedOff() {
  Console.println(F("SMAmbient:AmbientBlinkRedOff"));
  setAmbientColor(strip.Color(0, 0, 0));
  if(SMAmbient.Timeout(400)) SMAmbient.Set(AmbientBlinkRedOn);    
}

