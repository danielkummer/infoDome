/**
 * Detect and debounce hand movements.
 */

State Detect() {
  static bool detected = false;  
  
  if(SMDetect.Timeout(DETECTION_INTERVAL)) {
    if(detected && detect()) {
      Console.print("SMDetect:Detected :: ");
      detected = false;    
      unsigned short newMode = ++currentMode % BOOT_MODE;              
      blockingAmbientBlink(strip.Color(0,255,0), newMode + 1); //blink at least once
      
      if(isCycling) {
        isCycling = false;
        newMode = TIME_MODE;
      }

      if(newMode == CYCLING_MODE) {
        isCycling = true;      
        newMode = TIME_MODE;
      }      

      currentMode = newMode;   

      //small bugfix for tubes being off incorrectly when fast switching between the modes...
      tube.setBrightness(0x20);
    }    
    detected = detect();    
    SMDetect.Set(Detect);
  }  
}
