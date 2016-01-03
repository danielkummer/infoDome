/**
 * If turned on, cycle to a new mode after a specific interval until turned off.
 */

State CyclerOn() {  
  static int cyclerMode = 0;
       
  if(SMCycler.Timeout(LOOPER_INTERVAL)) {        
      cyclerMode = ++cyclerMode % CYCLING_MODE;
      Console.print(F("SMCycler:CyclerOn :: cycle to next mode "));
      Console.println(cyclerMode);
      currentMode = cyclerMode;
      SMCycler.Set(CyclerOn);
  }
}
