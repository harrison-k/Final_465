#include <Servo.h>
const int trigPin = 6;
const int echoPin = 5;
float angle;
Servo catapult;

void setup() {
  pinMode(trigPin,OUTPUT);
  catapult.attach(3);
  Serial.begin(9600);
  catapult.write(servoAngle(0));

}

void loop() {
  angle = rangeFinder();
  Serial.println(angle);
  catapult.write(servoAngle(5*angle));
  delay(100);

}

int servoAngle(float angle){
  int pulse = angle*10+900;
  if (pulse>2200) pulse = 2200;
  if (pulse<900) pulse = 900;
  return pulse;
}

float rangeFinder(){
  unsigned long timerStart;
  unsigned long timerStop;
  float distance;
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);
  while (digitalRead(echoPin) == 0);
  timerStart = micros();
  while(digitalRead(echoPin) == 1);
  timerStop = micros();
  distance = timerStop-timerStart;
  distance = distance/148; //distance in inches
  return distance;
}

