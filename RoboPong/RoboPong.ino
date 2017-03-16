#include <Servo.h>
const int trigPin = 6; //these two pins are for the ultrasonic thinger
const int echoPin = 5;
const float armLength = 13.75;  //catapult lever arm length in inches
const float g = 32.174; //American gravity
const float pi = 3.141592;
float angle; //launch angle
float dist;  //Distance from projectile release to target
float range; //Distance from servo axle to target
float height;//height from projectile release to target
int omega = 60; //servo angular rate. This is a total guess.
float deltaH = 3-2; //height from servo axle to target (Servo axle height minus cup lip height)
float error = 1; //used for some iterative stuff later
Servo catapult; //servo object named catapult

void setup() {
  pinMode(trigPin,OUTPUT);
  catapult.attach(3);
  Serial.begin(9600);
  catapult.write(servoAngle(0));

}

void loop() {
  if (Serial.available()){
    readAngle();
    range = rangeFinder()+3; //ultrasonic reading plus distance from sensor to servo axle
    dist = range; //initial guess for distance
    height = deltaH; //initial guess for height
    Serial.print("Distance (in): ");
    Serial.println(range);
    while (error>=1){ //Since launch angle and launch position depend on each other, iteration is needed to find a solution.
      angle = pi/2-atan((pow(omega*armLength,2)+sqrt(pow(omega*armLength,4)-pow(g*dist,2)+2*g*height*pow(omega*armLength,2)))/(g*dist)); //RADIANS. See https://en.wikipedia.org/wiki/Trajectory_of_a_projectile
      error = range+armLength*sin(angle)-dist; //This isn't used for anything other than deciding to stop the loop
      Serial.println(error); //debug line to make sure solution is converging. Delete once this code works.
      dist = range+armLength*cos(angle); //re-calculate distance with new launch x
      height = deltaH+armLength*sin(angle); //re-calculate height with new launch y
    }
    error = 1; //reset error for the next time something is launched
    catapult.write(servoAngle(angle*180/pi)); //fling catapult to desired angle, but in degrees
    Serial.print("Launch Angle (deg):");
    Serial.println(angle);
    delay(500);
    while (!Serial.available()); //wait for arbitrary input to reset. This and the line below it can be deleted after the system is debugged.
    readAngle();
    catapult.write(servoAngle(0));
  }
  else{ //this probably doesn't need to be here
    delay(10);
  }
}

int servoAngle(float angle){
  int pulse = 2200-angle*11; //convert angle in degrees to pulse width in microseconds
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
    delay(100);
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
