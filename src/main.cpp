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
    switch (eyes[i].state) {
      case OFF: {
        if (currentTime >= eyes[i].offEndTime) {
          eyes[i].state = GO_ON;
          Serial.print(i);
          Serial.println(" GO_ON");
        }
        break;
      }

      case GO_ON: {
        int value = eyes[i].value;
        if (value > 0) {
          value = value - PWM_STEP;
          eyes[i].value = value;
          driver.getPin(i).setValueAndWrite(value);
        
        } else {
          eyes[i].state = ON;
          eyes[i].onEndTime = millis() + 10000;
          Serial.print(i);
          Serial.println(" ON");
        }
        break;
      }
        
      case ON: {
        if (currentTime >= eyes[i].onEndTime) {
          eyes[i].state = GO_OFF;
          Serial.print(i);
          Serial.println(" GO_OFF");
        }
        break;
      }
        
      case GO_OFF: {
        int value = eyes[i].value;
        if (value < PCA9685_MAX_VALUE) {
          value = value + PWM_STEP;
          eyes[i].value = value;
          driver.getPin(i).setValueAndWrite(value);
        
        } else {
          eyes[i].state = OFF;
          eyes[i].offEndTime = millis() + 2000; 
          Serial.print(i);
          Serial.println(" OFF");
        }
        break;
      }
    }
  }

}