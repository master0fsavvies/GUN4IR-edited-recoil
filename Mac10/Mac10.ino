#include "Arduino.h"
#include "Transformation.h"
#include "DFRobotIRPosition.h"
#include "AbsMouse.h"
#include <Keyboard.h>
#include "Wire.h"

#define triggerA 5
#define triggerB 10
#define mag 0
#define lid 16
#define fireLED 9
#define warningLED 0

const int joystickXPin = A1; 
const int joystickYPin = A0; 


const int joystickThreshold = 50; 


int prevJoystickStateX = 0;
int prevJoystickStateY = 0;


DFRobotIRPosition camera;
int positionX[] = {1023, 1023, 1023, 1023};
int positionY[] = {1023, 1023, 1023, 1023};

int cornersX[] = {1023, 1023, 1023, 1023};
int cornersY[] = {1023, 1023, 1023, 1023};

boolean dataComplete = false;
int autoFire = 2;
int totalFire = 2;

int button_triggerA = 0;
int button_triggerB = 0;
int button_mag = 0;
int button_lid = 0;



int warningLEDState = 0;

int screenW = 1920;
//int screenW = 2560;
int screenH = 1080; //DONT NORMALLY CHANGE

// X GOES HIGHER, cursor moves left
// Y GOES HIGHER, cursor moves down
//int gunCenterX = 560;   //TV
//int gunCenterY = 280;   //TV
//int gunCenterX = 518;   //WIDESCREEN
//int gunCenterY = 314;   //WIDESCREEN
int gunCenterX = 528;   //PC
int gunCenterY = 300;   //PC

void setup() {
  pinMode(fireLED, OUTPUT);
  pinMode(warningLED, OUTPUT);
  pinMode(triggerA, INPUT_PULLUP);
  pinMode(triggerB, INPUT_PULLUP);
  pinMode(mag, INPUT_PULLUP);
  pinMode(lid, INPUT_PULLUP);

  Serial.begin(19200);
  AbsMouse.init(screenW, screenH);
  camera.begin();
  Keyboard.begin();
  digitalWrite(fireLED, LOW);
  digitalWrite(warningLED, LOW);
}

void loop() {
  handleButtons();
  autoRecoil();
  getCameraData();
  sortPoints();
  moveCursor();
}

void handleButtons() {
  int triggerA_now = digitalRead(triggerA);
  int triggerB_now = digitalRead(triggerB);
  int mag_now = digitalRead(mag);
  int lid_now = digitalRead(lid);

  if (triggerA_now != button_triggerA) {
    button_triggerA = triggerA_now;
    if (button_triggerA == 0) {
      AbsMouse.press(MOUSE_LEFT);
      if(autoFire == 1){
        digitalWrite(7, HIGH);
        digitalWrite(8, HIGH);
        digitalWrite(9, HIGH);
        delay(19); 
        digitalWrite(7, LOW);
        digitalWrite(8, LOW);
        digitalWrite(9, LOW);
      }
    } else {
      AbsMouse.release(MOUSE_LEFT);
      digitalWrite(7, LOW);
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);
    }
  } 

  if (triggerB_now != button_triggerB) {
    button_triggerB = triggerB_now;
    if (button_triggerB == 0) {
      Keyboard.press(KEY_RETURN);
    } else {
      Keyboard.release(KEY_RETURN);
    }
  }

  if (lid_now != button_lid) {
    button_lid = lid_now;
    if (button_lid == 0) {
      autoFire = (autoFire % totalFire) + 1;
      delay(1000);
    } else {
    }
  }

  if (mag_now != button_mag) { 
    button_mag = mag_now;
    if (button_mag == 0) {
    } else {
    }
  } 

  int joystickXValue = analogRead(joystickXPin);
  int joystickYValue = analogRead(joystickYPin);
  
  if (joystickXValue > (512 + joystickThreshold) && prevJoystickStateX <= (512 + joystickThreshold)) {
    Keyboard.press(KEY_RIGHT_ARROW);
    prevJoystickStateX = joystickXValue;
  }
  else if (joystickXValue < (512 - joystickThreshold) && prevJoystickStateX >= (512 - joystickThreshold)) {
    Keyboard.press(KEY_LEFT_ARROW);
    prevJoystickStateX = joystickXValue;
  }
  else if (joystickXValue >= (512 - joystickThreshold) && joystickXValue <= (512 + joystickThreshold)) {
    Keyboard.release(KEY_RIGHT_ARROW);
    Keyboard.release(KEY_LEFT_ARROW);
    prevJoystickStateX = joystickXValue;
  }
  if (joystickYValue > (512 + joystickThreshold) && prevJoystickStateY <= (512 + joystickThreshold)) {
    Keyboard.press(KEY_DOWN_ARROW);
    prevJoystickStateY = joystickYValue;
  }
  else if (joystickYValue < (512 - joystickThreshold) && prevJoystickStateY >= (512 - joystickThreshold)) {
    Keyboard.press(KEY_UP_ARROW);
    prevJoystickStateY = joystickYValue;
  }
  else if (joystickYValue >= (512 - joystickThreshold) && joystickYValue <= (512 + joystickThreshold)) {
    Keyboard.release(KEY_DOWN_ARROW);
    Keyboard.release(KEY_UP_ARROW);
    prevJoystickStateY = joystickYValue;
  }
}

void autoRecoil() {
  if(autoFire == 3 && !digitalRead(triggerA)){
    digitalWrite(7, HIGH);
    delay(8);
    digitalWrite(8, HIGH);
    delay(8);
    digitalWrite(9, HIGH);
    delay(8);
    digitalWrite(7, LOW);
    delay(8);
    digitalWrite(8, LOW);
    delay(8);
    digitalWrite(9, LOW);
    delay(8);
  }
  if(autoFire == 2 && !digitalRead(triggerA)){
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
    digitalWrite(9, HIGH);
    delay(15); 
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);
    delay(25);
  }
  
  
}

void getCameraData() {
  camera.requestPosition();
  if (camera.available()) {
    for (int i = 0; i < 4; i++) {
      positionX[i] = camera.readX(i);
      positionY[i] = camera.readY(i);
      //Serial.println(i + " Position X: ");
      //Serial.print(positionX[i]); // Print any index you want
      //Serial.println(i + " Position Y: ");
      //Serial.print(positionY[i]);
    }
    
  }
  
  if (positionX[3] == 1023 && positionY[3] == 1023) {
    dataComplete = false;
    setWarningLED(1);
  } else {
    dataComplete = true;
    setWarningLED(0);
  }
}

void sortPoints() {
  if (!dataComplete)
    return;

  int orderedX[] = {0, 1, 2, 3};

  for (int i = 0; i < 3 ; i++) {
    for (int j = i + 1; j < 4; j++) {
      if (positionX[orderedX[i]] < positionX[orderedX[j]]) {
        int temp = orderedX[i];
        orderedX[i] = orderedX[j];
        orderedX[j] = temp;
      }
    }
  }

  if (positionY[orderedX[0]] < positionY[orderedX[1]]) {
    cornersX[0] = positionX[orderedX[0]];
    cornersY[0] = positionY[orderedX[0]];
    cornersX[2] = positionX[orderedX[1]];
    cornersY[2] = positionY[orderedX[1]];
  } else {

    cornersX[0] = positionX[orderedX[1]];
    cornersY[0] = positionY[orderedX[1]];
    cornersX[2] = positionX[orderedX[0]];
    cornersY[2] = positionY[orderedX[0]];
  }

  if (positionY[orderedX[2]] < positionY[orderedX[3]]) {
    cornersX[1] = positionX[orderedX[2]];
    cornersY[1] = positionY[orderedX[2]];
    cornersX[3] = positionX[orderedX[3]];
    cornersY[3] = positionY[orderedX[3]];
  } else {

    cornersX[1] = positionX[orderedX[3]];
    cornersY[1] = positionY[orderedX[3]];
    cornersX[3] = positionX[orderedX[2]];
    cornersY[3] = positionY[orderedX[2]];
  }
}
void moveCursor() {
  if (!dataComplete)
    return;

  Transformation trans(cornersX, cornersY, screenW, screenH, gunCenterX, gunCenterY);
  AbsMouse.move(trans.u(),trans.v());
}

void setWarningLED(int x) {
  if (x == 1) {
    digitalWrite(warningLED, HIGH);
  } else {
    digitalWrite(warningLED, LOW);
  }
}
