# 16 channel LED PWM controller

Each PCA9685 has 16 independent channels. If you want to use LED strip or other high-power lamps you must use power transistors on each channel. In this project i used two ULN2803A darlington transistor arrays to have 12V 500-mA for each lamp.
There are 5 states for each lamp:
- Off
- Smoothly turns on
- On
- Smoothly turns off
- Sleep (sleep time is bigger then off time)

Each state has random time interval and ratio (you can change it in code)

[Video](https://yadi.sk/i/EYHFzLRHz2qZLA)

## Development platform
PlatformIO, but as the whole code is in a single file it can be easily copied into a .ino file and used with the Arduino IDE

## Used hardware
- Arduino Nano ATmega328
- ULN2803A
- [Adafruit 16-Channel 12-bit PWM/Servo Driver](https://www.adafruit.com/product/815)
	

## Library dependencies		
- [PCA9685](https://github.com/TOppenhoff/PCA9685)