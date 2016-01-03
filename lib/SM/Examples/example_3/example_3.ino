#include <SM.h>

SM M(S1H, S1B);//create statemchine with initial head and body state functions

void setup(){
  Serial.begin(115200);
 }//setup()

void loop(){
  EXEC(M);//run statemachine
}//loop()

State S1H(){//state head function, run only once at each entry
  Serial.println("S1 head");//print message on each re-entry
}

State S1B(){//state body function run constantly
  if(M.Timeout(500)) M.Set(S1H, S1B);//re-enter state after 0,5s
}//
