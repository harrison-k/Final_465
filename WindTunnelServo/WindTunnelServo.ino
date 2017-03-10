// WindTunnelServo
// Harrison King
// 2/9/2017
// This code reads an input angle in ASCII from the serial line, then writes that angle to a BlueBird 616DMG+HS servo.

#include <Servo.h>

//_____________Global Variables_____________
Servo myservo;  // create servo object to control a servo
int val;        // input angle

//_____________Setup Function_____________
void setup() {
  Serial.begin(9600);
  myservo.attach(3);  // attaches the servo on pin 3 to the servo object
}

//_____________Main Loop_____________
void loop() {
  if (Serial.available()){
  val = readAngle();
  Serial.println(val);
  val = val*10+1500;    //10 us per degree, zero point at 1500 us. This gives range of +69 to -70.
  myservo.write(val);   // sets the servo position according to the scaled value
  delay(15);            // waits for the servo to get there
  } 
}

//_____________Support Functions_____________
int readAngle(){
  int angle = 0;
  if (!Serial.available()) return 0;    //exit if called erroniously
  delay(100);                           //wait for all characters to pile up in buffer
  bool negative = (Serial.peek()==45);  //check for minus sign
  if (negative) Serial.read();          //clear minus sign from buffer
  while (Serial.available()){
    angle = angle*10+Serial.read()-48;  //read digits
  }
  if (negative) angle = angle*-1;       //flip sign if number is supposed to be negative
  return angle;
}

