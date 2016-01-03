/**
 * Executes all other state machines and handles the booting process.
 */

State Boot() {
  EXEC(SMDisplay);
  EXEC(SMAmbient);    

  if(SMMaster.Timeout(BOOT_TIMEOUT)) {    
    currentMode = TIME_MODE;
    isCycling = true;    
    SMMaster.Set(Run);
    SMAmbient.Set(AmbientRun);
    SMDisplay.Set(DisplayRun);
    SMData.Set(DataUpdate);
  }  
}

State Run() {
  EXEC(SMDetect);  
  EXEC(SMDisplay);
  EXEC(SMAmbient);
  EXEC(SMData);        

 if(isCycling) {    
    EXEC(SMCycler);
    if(SMCycler.Finished) { SMCycler.Restart(); }
  }
  
  if(!isCycling && !SMCycler.Finished) {
    SMCycler.Finish();  
  }  

  if(currentMode == JIRA_MODE || currentMode == GIT_MODE || currentMode == MAIL_MODE) {
    if(SMData.Finished) { SMData.Restart(); }
  }
}
