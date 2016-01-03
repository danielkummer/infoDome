#include <SM.h>


SM m1(m1s1h, m1s1b);
SM m2(m2s1h, m2s1b);
SM m3(m3s1h, m3s1b);
SM Blink(On);

static int m2c = 0;//counter for machine2

int ledPin = 13;// LED connected to digital pin 13

void setup(){
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);      // sets the digital pin as output
}//Setup()

void loop(){
  EXEC(m1);
  EXEC(Blink);
}//loop()

State m1s1h(){
  Serial.println("Machine1:State1");
}//m1s1h()

State m1s1b(){
  if(m1.Timeout(300)){
    m1.Set(m1s2h, m1s2b);
    Serial.println("chaging to 2");
  };
}//m1s1b()

State m1s2h(){
  Serial.println("Machine1:State2:Splitting execution");
}//m1s2h()

State m1s2b(){
  EXEC(m2);
  EXEC(m3);
  if(m2.Finished && m3.Finished){
    Serial.println("Split completed");
    m1.Finish();
  };
}//m1s2()

State m2s1h(){
    Serial.print("Machine2:State1:Count");
    Serial.println(m2c++);
}//m2s1h()

State m2s1b(){
  if(m2.Timeout(200)) m2.Set(m2s1h, m2s1b);
  if(m2c>10) m2.Finish();
}//m2s1b()

State m3s1h(){
  Serial.println("Machine3:state1");
}//m3s1h()

State m3s1b(){
  if(m3.Timeout(250)) m3.Set(m3s2h, m3s2b);
}//m3s1b()

State m3s2h(){
  Serial.println("Machine3:state2");
}//m2s2h()

State m3s2b(){
  if(m3.Timeout(250)) m3.Set(m3s3h, m3s3b);
}//m3s2b()

State m3s3h(){
  Serial.println("Machine3:state3");
}

State m3s3b(){
  if(m3.Timeout(250)) m3.Finish();
}//s3m1()

State On(){
  digitalWrite(ledPin, HIGH);// sets the LED on
  if(Blink.Timeout(400)) Blink.Set(Off);
}

State Off(){
  digitalWrite(ledPin, LOW);   // sets the LED on
  if(Blink.Timeout(400)) Blink.Set(On);
}
