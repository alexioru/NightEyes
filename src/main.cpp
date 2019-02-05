#include <Arduino.h>
#include <PCA9685.h>
#include <Wire.h>

// Config
#define EYES_COUNT 8 // 0...16
#define PWM_STEP 5 // One of 4095
#define ON_2_OFF_RATIO 1 // For initial setup
#define OFF_2_SLEEP_RATIO 10 // Means OFF.count / SLEEP.cout = X

// Times random MIN...MAX in seconds
#define MIN_ON_TIME 5
#define MAX_ON_TIME 15

#define MIN_OFF_TIME 2
#define MAX_OFF_TIME 8

#define MIN_SLEEP_TIME 20
#define MAX_SLEEP_TIME 30

PCA9685 driver = PCA9685(0x0, PCA9685_MODE_LED_DIRECT, 1500.0);

enum State {OFF, GO_ON, ON, GO_OFF};

struct Eye {
  State state = OFF;
  int value = 0;
  unsigned long onEndTime = 0;
  unsigned long offEndTime = 0;
};

Eye eyes[EYES_COUNT];

void setup() {
  Wire.begin();
  driver.setup();
  randomSeed(analogRead(0)); // For random values

  // Initial state
  for(int i = 0; i < EYES_COUNT; i++) {
    Eye &eye = eyes[i];
    if (random(ON_2_OFF_RATIO + 1)) {
      eye.state = ON;
      eye.value = 0;
      eye.onEndTime = millis() + 1000 * random(MIN_ON_TIME, MAX_ON_TIME);
      driver.getPin(i).fullOffAndWrite(); // On

    } else {
      eye.state = OFF;
      eye.value = PCA9685_MAX_VALUE;
      eye.onEndTime = millis() + 1000 * random(MIN_ON_TIME, MAX_ON_TIME);
      driver.getPin(i).fullOnAndWrite(); // Off
    }

  }

  // All others pins
  for(int i = EYES_COUNT; i < 16; i++) {
    driver.getPin(i).fullOnAndWrite(); // Off
  }

}

void loop() {
  unsigned long currentTime = millis(); 
  for(int i = 0; i < EYES_COUNT; i++) {
    Eye &eye = eyes[i];
    switch (eye.state) {
      case OFF: {
        if (currentTime >= eye.offEndTime) {
          eye.state = GO_ON;
        }
        break;
      }

      case GO_ON: {
        int &value = eye.value;
        if (value > 0) {
          value = value - PWM_STEP;
          driver.getPin(i).setValueAndWrite(value);
        
        } else {
          eye.state = ON;
          eye.onEndTime = millis() + 1000 * random(MIN_ON_TIME, MAX_ON_TIME);
        }
        break;
      }
        
      case ON: {
        if (currentTime >= eye.onEndTime) {
          eye.state = GO_OFF;
        }
        break;
      }
        
      case GO_OFF: {
        int &value = eye.value;
        if (value < PCA9685_MAX_VALUE) {
          value = value + PWM_STEP;
          driver.getPin(i).setValueAndWrite(value);
        
        } else {
          eye.state = OFF;

          if (random(OFF_2_SLEEP_RATIO + 1)) {
            eye.offEndTime = millis() + 1000 * random(MIN_OFF_TIME, MAX_OFF_TIME);
            
          } else {
            eye.offEndTime = millis() + 1000 * random(MIN_SLEEP_TIME, MAX_SLEEP_TIME);

          }

        }
        break;
      }
    }
  }

}