#include "example1.h"

Pstate Exec;

#define toggle 9
#define led 13

void setup(){
  pinMode(led, OUTPUT);
}

void loop(){
  Exec();
}

State S1(){
  digitalWrite(led, HIGH);
  if(digitalRead(toggle)) Exec = S2;
}

State S2(){
  if(!digitalRead(toggle)) Exec = S3;
}

State S3(){
  digitalWrite(led, LOW);
  if(digitalRead(toggle)) Exec = S4;
}

State S4(){
  if(!digitalRead(toggle)) Exec = S1;
}
