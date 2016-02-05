/**
 * Display values on the nixie tubes based on the current mode.
 */

State DisplayStartup() {  
  if(SMDisplay.Timeout(100)) {
    displayRandomNumbers();
    SMDisplay.Set(DisplayStartup);
  }    
}

State DisplayRun() {
  short mainTemp;
  DateTime now;    
  
  if(SMDisplay.Timeout(100)) {    
    now = rtc.now();  
    switch(currentMode) {      
      case TIME_MODE:                
        tube.setBackgroundColor(Cyan);
        //animate for 30 secs every full hour
        if(now.minute() == 0 && now.second() < 15) {
          displayRandomNumbers();
        } else {
          tube.printf("%02d:%02d", now.hour(), now.minute());    
        }
        break;
      case DATE_MODE:                
        tube.setBackgroundColor(Yellow);        
        tube.printf("%02d.%02d", now.day(), now.month());  
        break;
      case TEMP_MODE:
        bmp.getTemperature(&temperature);  
        tube.setBackgroundColor(Blue);
        mainTemp = (short)temperature;
        tube.printf("%02d.%02d'", mainTemp, (short)((temperature - mainTemp) * 100) );  
        break;
      case JIRA_MODE:
        pulseTubeIfChanged(&lastJiraCount, jiraCount);
        tube.setBackgroundColor(Green);
        tube.printf("%04d", jiraCount);  
        break;
      case GIT_MODE:
        pulseTubeIfChanged(&lastGitCount, jiraCount);
        tube.setBackgroundColor(Red);
        tube.printf("%04d", gitCount); 
        break;
      case MAIL_MODE:
        pulseTubeIfChanged(&lastMailCount, jiraCount);
        tube.setBackgroundColor(White);
        tube.printf("%04d", mailCount);  
        break;
      default:
        Console.print("SMDisplay:DisplayRun :: Unknown mode: ");
        Console.println(currentMode);
        tube.printf("%04d", (short)9999);  
        break;
    } 
    tube.display();
    SMDisplay.Set(DisplayRun);
  }
}



