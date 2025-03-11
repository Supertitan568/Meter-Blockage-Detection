#include <wiringPi.h>
#include "sensor.h"

#define GPIO_PIN 28

void sensor_setup(){
   wiringPiSetup();
   pinMode(GPIO_PIN, INPUT);
}

bool is_movement(){
   return !digitalRead(GPIO_PIN);
}
