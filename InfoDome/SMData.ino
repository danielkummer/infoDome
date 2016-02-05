/**
 * Download data via WiFi from Temboo.
 */

State DataUpdate() {
  static bool initialized = false;
  //Console.println("SMData:DataStartup");  
  if(!initialized || SMData.Timeout(TEMBOO_UPDATE_INTERVAL)) {
    tube.setBackgroundColor(Magenta);
    tube.printf("%04d", 1010);  
    tube.display();
    getTembooData();
    initialized = true;
    SMData.Set(DataUpdate);    
  }    
}
