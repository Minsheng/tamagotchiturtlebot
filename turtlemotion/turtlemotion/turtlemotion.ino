#include <NewPing.h>
#include <VarSpeedServo.h> 
 
VarSpeedServo foreLeft;
VarSpeedServo foreRight;
VarSpeedServo rearLeft;
VarSpeedServo rearRight;

#define DEBUG 0
#define STOP 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

#define SONAR_NUM     1 // Number of sensors.
#define MAX_DISTANCE 200 // Maximum distance (in cm) to ping.
#define PING_INTERVAL 29 // Milliseconds between sensor pings (29ms is about the min to avoid cross-sensor echo).

unsigned long timer;
unsigned long elapsedTime;

unsigned long pingTimer[SONAR_NUM]; // Holds the times when the next ping should happen for each sensor.
unsigned int cm[SONAR_NUM];         // Where the ping distances are stored.
unsigned int lastCm[SONAR_NUM];
uint8_t currentSensor = 0;          // Keeps track of which sensor is active.

//NewPing sonar[SONAR_NUM] = {     // Sensor object array.
//  NewPing(12, 12, MAX_DISTANCE), // Each sensor's trigger pin, echo pin, and max distance to ping.
//  NewPing(11, 11, MAX_DISTANCE)
//};

NewPing sonar[SONAR_NUM] = {     // Sensor object array.
  NewPing(2, 2, MAX_DISTANCE)
};

const int foreLeftPin = 3;
const int foreRightPin = 5;
const int rearLeftPin = 7;
const int rearRightPin = 9;

const int fullSpeed = 255;
const int fastSpeed = 127;
const int regularSpeed = 90;
const int midSpeed = 60;
const int slowSpeed = 30;

const int maxAngle = 90;
const int minAngle = 90;

const int FEAR_THRESHOLD = 3;
const int CAUTIOUS_THRESHOLD = 3;
int fearLevel = 0; // 0-2: netural, >2: cautious
int becomeCautious = 0;
int cautiousCnt = 0;

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  if (!DEBUG) {
    pingTimer[0] = millis() + 75;           // First ping starts at 75ms, gives time for the Arduino to chill before starting.
    // Set the starting time for each sensor.
    for (uint8_t i = 1; i < SONAR_NUM; i++) {
      pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
    }
  }
  
  foreLeft.attach(foreLeftPin);
  foreRight.attach(foreRightPin);
  rearLeft.attach(rearLeftPin);
  rearRight.attach(rearRightPin);

  // set diagonal limbs to same direction
  // fore left and rear right point to tail
  // fore right and rear left point to head
  foreLeft.write(maxAngle,fullSpeed,false);
  rearRight.write(minAngle,fullSpeed,false);
  foreRight.write(maxAngle,fullSpeed,false);
  rearLeft.write(minAngle,fullSpeed,false);
} 

void loop() {
  if (DEBUG) {
    slideWithFore(170, 90);
  } else {
    for (uint8_t i = 0; i < SONAR_NUM; i++) { // Loop through all the sensors.
      if (millis() >= pingTimer[i]) {         // Is it this sensor's time to ping?
        lastCm[i] = cm[i]; // Set the last sensor value
        pingTimer[i] += PING_INTERVAL * SONAR_NUM;  // Set next time this sensor will be pinged.
  //      if (i == 0 && currentSensor == SONAR_NUM - 1) oneSensorCycle(); // Sensor ping cycle complete, do something with the results.
        sonar[currentSensor].timer_stop();          // Make sure previous timer is canceled before starting a new ping (insurance).
        currentSensor = i;                          // Sensor being accessed.
        cm[currentSensor] = 0;                      // Make distance zero in case there's no ping echo for this sensor.
        sonar[currentSensor].ping_timer(echoCheck); // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
      }
    }

    Serial.print("previous sonar1 ");
    Serial.print(lastCm[0]);
    Serial.println("cm ");
    Serial.print("current sonar1 ");
    Serial.print(cm[0]);
    Serial.println("cm ");
//    Serial.print("current sonar2 ");
//    Serial.print(cm[1]);
//    Serial.println("cm ");
    int prevSonar1 = lastCm[0];
    int currSonar1 = cm[0];
//    int sonar2 = cm[1];

    // when move cautiously for 10 times, reset 
    if (becomeCautious && (cautiousCnt > CAUTIOUS_THRESHOLD)) {
      Serial.println("resetting cautious behavior...");
      becomeCautious = 0; // reset behavior
      cautiousCnt = 0; // reset cautious count
    }
        
    // if fear level reaches cautious threshold, turtle behavior is changed to cautious
    if (fearLevel > FEAR_THRESHOLD) {
      Serial.println("resetting fear level...");
      becomeCautious = 1;
      cautiousCnt = 0; // reset cautious count because behavior is renewed
      fearLevel = 0; // reset fear level when behavior is changed
    }
    
    // if sonar1 detects obstacle,
    // execute scared strategy or brave strategy based on a random factor
    // scared strategy or taken strategy will increase fear level
    if ((currSonar1 > 0 && currSonar1 < 10) && (prevSonar1 > 0 && prevSonar1 < 10)) {
      fearLevel++;
      Serial.println("Committing taken strategy!");
      takenStrategy();
    } else if ((currSonar1 > 0 && currSonar1 < 40) && (prevSonar1 > 0 && prevSonar1 < 40)) {
      long randomStrategy = random(1, 4);
      if (randomStrategy == 3) {
        Serial.println("Committing brave strategy!");
        braveStrategy();
      } else {
        fearLevel++;
        Serial.println("Committing scared strategy!");
        scaredStrategy();
      }
    } else {
      goDirection(random(1,4), 48, 42);

      // if turtle is cautious, turn left and right to look around after every move
      if (becomeCautious) {
        Serial.print("Currenct cautious count: ");
        Serial.print(cautiousCnt);
        Serial.println();
        cautiousCnt++;
        lookAround();
      }
    }
  }
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

// locomotion pattern: diagonally opposite limbs move together
void goForward(int fastRate, int lowRate) {
  // fore left swing range:100
  // rear right swing range: 100
  // fore right swing range: 100
  // rear left swing range: 90
  foreLeft.write(30, fastRate, false);
  rearRight.write(160, fastRate, false);
  foreRight.write(50, lowRate, false);
  rearLeft.write(110, lowRate, true);
  
  foreRight.write(160, fastRate, false);
  rearLeft.write(20, fastRate, false);
  foreLeft.write(130, lowRate, false);
  rearRight.write(60, lowRate , true);
}

void goLeft(int fastRate, int lowRate) {
  // fore left swing range:60
  // rear right swing range: 115
  // fore right swing range: 115
  // rear left swing range: 60
  foreLeft.write(20, fastRate, false); // fore left moves forward
  rearRight.write(160, fastRate, false); // rear right moves forward
  rearLeft.write(80, lowRate, false); // rear left moves backward to lower degree
  foreRight.write(20, lowRate, true); // fore right moves backward
  
  foreRight.write(135, fastRate, false); // fore right moves forward
  rearLeft.write(20, fastRate, false); // rear left moves forward
  foreLeft.write(80, lowRate, false); // fore left moves backward to lower degree
  rearRight.write(45, lowRate, true); // rear right moves backward
}

void goRight(int fastRate, int lowRate) {
  // fore left swing range:115
  // rear right swing range: 55
  // fore right swing range: 45
  // rear left swing range: 115
  foreLeft.write(20, fastRate, false); // fore left moves forward
  rearRight.write(160, fastRate, false); // rear right moves forward
  foreRight.write(100, lowRate, false); // fore right moves backward to lower degree
  rearLeft.write(135, lowRate, true); // rear left moves backward
  
  foreRight.write(145, fastRate, false); // fore right moves forward
  rearLeft.write(20, fastRate, false); // rear left moves forward
  rearRight.write(105, lowRate, false); // rear right moves backward to lower degree
  foreLeft.write(135, lowRate, true); // fore left moves backward
}

void goDirection(int direct, int highSpeed, int lowSpeed) {
  switch (direct) {
    case FORWARD:
      goForward(highSpeed, lowSpeed);
      break;
    case LEFT:
      goLeft(highSpeed, lowSpeed);
      break;
    case RIGHT:
      goRight(highSpeed, lowSpeed);
      break;
    default:
      break;
  }
}

void slideWithFore(int fastRate, int lowRate) {
  // fore left swing range:100
  // fore right swing range: 100
  // rear left swing range: 100
  // rear right swing range: 100
  foreLeft.write(20, fastRate, false); // fore left moves forward
  foreRight.write(160, fastRate, false); // fore right moves forward
  rearLeft.write(120, lowRate, false); // rear left moves backward
  rearRight.write(60, lowRate, true); // rear right moves backward

  rearLeft.write(20, fastRate, false); // rear left moves forward
  rearRight.write(160, fastRate, false); // rear right moves backward
  foreLeft.write(120, lowRate, false); // fore left moves backward
  foreRight.write(60, lowRate, true); // fore right moves backward
}

void freeze() {
  foreLeft.write(180,fullSpeed,false);
  rearRight.write(180,fullSpeed,false);
  foreRight.write(0,fullSpeed,false);
  rearLeft.write(0,fullSpeed,false);
}

void braveStrategy() {
  freeze();
  delay(2000);
  
  long uTurnDirection = random(LEFT,RIGHT+1); // either turn left or right
  for (int i=0; i<6; i++) goDirection(uTurnDirection, 45, 30);
  
  if (uTurnDirection == LEFT) {
    uTurnDirection += 1; // turn to the opposite direction
  } else { // RIGHT
    uTurnDirection -=1; // turn to the opposite direction
  }
  freeze();
  delay(1500);
  
  for (int i=0; i<2; i++) goDirection(FORWARD, 130, 100);;
}

void scaredStrategy() {
  freeze();
  delay(2000);
  
  long uTurnDirection = random(LEFT,RIGHT+1); // either turn left or right
  for (int i=0; i<12; i++) goDirection(uTurnDirection, 45, 30);
  for (int i=0; i<6; i++) goDirection(FORWARD, 160, 120);
}

void takenStrategy() {
  freeze();
  delay(2000);
  for (int i=0; i<10; i++) slideWithFore(127, 95);
}

void mazeSolvedStrategy() {
  // to be implemented
}

void lookAround() {
  delay(1500);
  for (int i=0; i<2; i++) goDirection(LEFT, 75, 60);
  for (int i=0; i<4; i++) goDirection(RIGHT, 75, 60);
  for (int i=0; i<2; i++) goDirection(LEFT, 75, 60);
}

