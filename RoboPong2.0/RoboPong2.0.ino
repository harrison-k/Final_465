#include <Servo.h>
const int trigPin = 6; //these two pins are for the ultrasonic thinger
const int echoPin = 5;
const float armLength = 13.75;  //catapult lever arm length in inches
const float g = 32.174*12; //gravity in in/s^2
const float pi = 3.141592;
float angle; //launch angle
float angles[2];
float dist;  //Distance from projectile release to target
float range; //Distance from servo axle to target
float testrange;
float height;//height from projectile release to target
float v; //Marble release velocity
float deltaH = 4.25-2; //height from servo axle to target (Servo axle height minus cup lip height)
float error = 1; //used for some iterative stuff later
int counter = 0; //also used for iterations
bool manual; //manual vs automatic mode
Servo catapult; //servo object named catapult

void setup() {
  pinMode(trigPin,OUTPUT);
  catapult.attach(3);
  Serial.begin(9600);
  catapult.write(servoAngle(0));
  Serial.println("Enter m for manual mode, or press any key to continue.");
  while (!Serial.available());
  if (Serial.read()=='m'){
    manual = 1;
    Serial.println("Manual mode");
  }
  else Serial.println("Automatic mode");
}

void loop() {
  if (Serial.available()&&manual){ //Manual mode: writes user inputted angle to catapult
    catapult.write(servoAngle(readAngle()));
  }
  else if (Serial.available()){ //Automatic mode: hits target automatically. Maybe.
    readAngle(); //clear serial buffer
    range = rangeFinder()+2.25; //ultrasonic reading plus distance from sensor to servo axle
    angles[0] = 0;
    angles[1] = 60*pi/180;
    testrange = 0;
    Serial.print("Distance (in): ");
    Serial.println(range);
    while ((abs(error)>=1)&&(counter<100)){ //Since launch angle and launch position depend on each other, iteration is needed to find a solution.
      angle = (angles[0]+angles[1])/2;
      height = deltaH+armLength*sin(angle);
      dist = range+armLength*cos(angle);
      v = 32.98*angle+60.56;
      testrange = v*cos(angle)/g*(v*sin(angle)+sqrt(pow(v*sin(angle),2)+2*g*height));
      error = dist-testrange; //This isn't used for anything other than deciding to stop the loop
      Serial.println(error); //debug line to make sure solution is converging. Delete once this code works.
      if (error>0){
        angles[0] = angle;
      }
      else{
        angles[1] = angle;
      }
      counter++;
    }
    counter = 0;
    error = 1; //reset error for the next time something is launched
    catapult.write(servoAngle(angle*180/pi)); //fling catapult to desired angle, but in degrees
    Serial.print("Launch Angle (deg):");
    Serial.println(angle*180/pi);
    delay(500);
    catapult.write(servoAngle(0));
  }
  else{ //this probably doesn't need to be here
    delay(10);
  }
}

int servoAngle(float angle){
  int pulse = 2170-angle*10.8; //convert angle in degrees to pulse width in microseconds
  if (pulse>2200) pulse = 2200; //make sure we don't exceed the max or min servo positions
  if (pulse<900) pulse = 900;
  return pulse;
}

float rangeFinder(){
  unsigned long timerStart;
  unsigned long timerStop;
  float distance = 0;
  for (int i = 0; i<5; i++){  //collect five distance measurements
    digitalWrite(trigPin,HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin,LOW);
    while (digitalRead(echoPin) == 0);
    timerStart = micros();
    while(digitalRead(echoPin) == 1);
    timerStop = micros();
    distance = distance+timerStop-timerStart;
    delay(200);
  }
  distance = distance/5; //divide sum of 5 measurements by 5
  distance = distance/148; //distance in inches
  return distance;
}

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
