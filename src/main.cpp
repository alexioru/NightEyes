#include <Arduino.h>
#include <PCA9685.h>
#include <Wire.h>

// Config
#define EYES_COUNT 1
#define PWM_STEP 8

PCA9685 driver = PCA9685(0x0, PCA9685_MODE_LED_DIRECT, 800.0);

enum State {OFF, GO_ON, ON, GO_OFF};

struct Eye {
  State state = OFF;
  int value = 0;
    
  unsigned long offEndTime = millis() + 2000;
  unsigned long onEndTime = 0;  
};

Eye eyes[EYES_COUNT];

void setup() {
  Serial.begin(9600);
  Wire.begin();
  driver.setup();
  
  // All off
  for(int i = 0; i < 16; i++) {
    driver.getPin(i).fullOnAndWrite(); // Inversed
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
          Serial.print(i);
          Serial.println(" GO_ON");
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
          eye.onEndTime = millis() + 10000;
          Serial.print(i);
          Serial.println(" ON");
        }
        break;
      }
        
      case ON: {
        if (currentTime >= eye.onEndTime) {
          eye.state = GO_OFF;
          Serial.print(i);
          Serial.println(" GO_OFF");
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
          eye.offEndTime = millis() + 2000; 
          Serial.print(i);
          Serial.println(" OFF");
        }
        break;
      }
    }
  }

}