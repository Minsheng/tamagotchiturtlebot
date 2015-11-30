/*
  SweepTwoServos
  By Philip van Allen <philvanallen.com> for the VarSpeedServo.h library (October 2013)
  This example code is in the public domain
  
  Sweep two servos from 0-180, 180-0 in unison
  Uses the wait feature of the 2013 version of VarSpeedServo to start the first servo moving in the background
  and immediately starting a second servo moving and waiting for the second one to finish.
  
  Note that two servos will require more power than is available from the USB port - use an external power supply!
*/
#include <NewPing.h>
#include <VarSpeedServo.h> 
 
VarSpeedServo foreLeft;
VarSpeedServo foreRight;
VarSpeedServo backLeft;
VarSpeedServo backRight;

#define SONAR_NUM     2 // Number of sensors.
#define MAX_DISTANCE 200 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 33 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).

unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.

NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  NewPing(12, 12, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping.
  NewPing(11, 11, MAX_DISTANCE)
};

const int foreLeftPin = 3;
const int foreRightPin = 5;
const int backLeftPin = 7;
const int backRightPin = 9;

int fastSpeed = 127;
int regularSpeed = 90;
int slowSpeed = 30;

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  pingTimer[0] = millis() + 75;           // First ping starts at 75ms, gives time for the Arduino to chill before starting.
  // Set the starting time for each sensor.
  for (uint8_t i = 1; i < SONAR_NUM; i++) {
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
  }
  
  foreLeft.attach(foreLeftPin);
  foreLeft.write(0,255,false);
  foreRight.attach(foreRightPin);
  foreRight.write(0,255,false);
  backLeft.attach(backLeftPin);
  backLeft.write(0,255,false);
  backRight.attach(backRightPin);
  backRight.write(0,255,true);
} 

void loop() {
  for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through all the sensors.
    if (millis() >= pingTimer[i]) {         // Is it this sensor's time to ping?
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;  // Set next time this sensor will be pinged.
      if (i == 0 && currentSensor == SONAR_NUM - 1) oneSensorCycle(); // Sensor ping cycle complete, do something with the results.
      sonar[currentSensor].timer_stop();          // Make sure previous timer is canceled before starting a new ping (insurance).
      currentSensor = i;                          // Sensor being accessed.
      cm[currentSensor] = 0;                      // Make distance zero in case there's no ping echo for this sensor.
      sonar[currentSensor].ping_timer(echoCheck); // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
    }
  }
  
//  goFoward();
}

void echoCheck() { // If ping received, set the sensor distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}

void oneSensorCycle() { // Sensor ping cycle complete, do something with the results.
  // The following code would be replaced with your code that does something with the ping results.
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    Serial.print(i);
    Serial.print("=");
    Serial.print(cm[i]);
    Serial.print("cm ");
  }
  Serial.println();
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

