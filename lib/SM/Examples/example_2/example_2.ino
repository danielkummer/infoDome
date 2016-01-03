#include <SM.h>


SM Simple(S1);//create simple statemachine

#define toggle 9//pin to toggle led on and off
#define led 13

void setup(){
  pinMode(led, OUTPUT);
}

void loop(){
  EXEC(Simple);//run statemachine
}

State S1(){
  digitalWrite(led, HIGH);//turn led on
  if(digitalRead(toggle)) Simple.Set(S2);//wait for toggle pin to go high and change state to S2
}

State S2(){
  if(!digitalRead(toggle)) Simple.Set(S3);//wait for toggle pin to go low and change state to S3
}

State S3(){
  digitalWrite(led, LOW);//turn led off
  if(digitalRead(toggle)) Simple.Set(S4);//wait for toggle pin to go high and change state to S4
}

State S4(){
  if(!digitalRead(toggle)) Simple.Set(S1);//wait for toggle pin to go low and change state to S1
}
