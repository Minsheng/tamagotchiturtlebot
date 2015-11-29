/*
  SweepTwoServos
  By Philip van Allen <philvanallen.com> for the VarSpeedServo.h library (October 2013)
  This example code is in the public domain
  
  Sweep two servos from 0-180, 180-0 in unison
  Uses the wait feature of the 2013 version of VarSpeedServo to start the first servo moving in the background
  and immediately starting a second servo moving and waiting for the second one to finish.
  
  Note that two servos will require more power than is available from the USB port - use an external power supply!
*/

#include <VarSpeedServo.h> 
 
VarSpeedServo foreLeft;
VarSpeedServo foreRight;
VarSpeedServo backLeft;
VarSpeedServo backRight;
//VarSpeedServo myservo5;
//VarSpeedServo myservo6;
//VarSpeedServo myservo7;
//VarSpeedServo myservo8;
 
const int foreLeftPin = 3;
const int foreRightPin = 5;
const int backLeftPin = 7;
const int backRightPin = 9;
//const int servoPin5 = 10;
//const int servoPin6 = 11;
//const int servoPin7 = 12;
//const int servoPin8 = 13;
int fastSpeed = 127;
int regularSpeed = 90;
int slowSpeed = 30;

void setup() { 
  foreLeft.attach(foreLeftPin);
  foreLeft.write(0,255,false);
  foreRight.attach(foreRightPin);
  foreRight.write(0,255,false);
  backLeft.attach(backLeftPin);
  backLeft.write(0,255,false);
  backRight.attach(backRightPin);
  backRight.write(0,255,true);
//  myservo5.attach(servoPin5);
//  myservo5.write(0,255,false);
//  myservo6.attach(servoPin6);
//  myservo6.write(0,255,false);
//  myservo7.attach(servoPin7);
//  myservo7.write(0,255,false);
//  myservo8.attach(servoPin8);
//  myservo8.write(0,255,true);
} 

void loop() {
//  goSRoute();
  goFoward();
  goLeft();
  goFoward();
  goLeft();
//  myservo5.write(180,127,false);
//  myservo6.write(180,127,false);
//  myservo7.write(180,127,false);
//  myservo8.write(180,127,true);
    
//  myservo5.write(0,30,false);
//  myservo6.write(0,30,false);
//  myservo7.write(0,30,false);
//  myservo8.write(0,30,true);
}

void goSRoute() {
  goFoward();
  goLeft();
  goFoward();
  goLeft();
  goFoward();
  goRight();
  goFoward();
  goRight();
  goFoward();
  goLeft();
  goFoward();
  goLeft();
}

void goFoward() {
  foreLeft.write(180,fastSpeed,false);
  foreRight.write(180,fastSpeed,false);
  backLeft.write(0,fastSpeed,false);
  backRight.write(0,fastSpeed,true);

  foreLeft.write(0,regularSpeed,false);
  foreRight.write(0,regularSpeed,false);
  backLeft.write(180,regularSpeed,false);
  backRight.write(180,regularSpeed,true);
}

void goLeft() {
  foreLeft.write(180,regularSpeed,false);
  foreRight.write(180,fastSpeed,false);
  backLeft.write(0,regularSpeed,false);
  backRight.write(0,fastSpeed,true);

  foreLeft.write(0,slowSpeed,false);
  foreRight.write(0,regularSpeed,false);
  backLeft.write(180,slowSpeed,false);
  backRight.write(180,regularSpeed,true);
}

void goRight() {
  foreLeft.write(180,fastSpeed,false);
  foreRight.write(180,regularSpeed,false);
  backLeft.write(0,fastSpeed,false);
  backRight.write(0,regularSpeed,true);

  foreLeft.write(0,regularSpeed,false);
  foreRight.write(0,slowSpeed,false);
  backLeft.write(180,regularSpeed,false);
  backRight.write(180,slowSpeed,true);
}

void goBack() {
  foreLeft.write(0,fastSpeed,false);
  foreRight.write(0,fastSpeed,false);
  backLeft.write(180,fastSpeed,false);
  backRight.write(180,fastSpeed,true);
  
  foreLeft.write(180,regularSpeed,false);
  foreRight.write(180,regularSpeed,false);
  backLeft.write(0,regularSpeed,false);
  backRight.write(0,regularSpeed,true);
}

