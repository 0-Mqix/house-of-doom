#include "led.h"

#define DEBOUNCE_DELAY 300

const int red_button = 2;

volatile RGB status_led(24, 23, 22);

void setup() {
  pinMode(red_button, INPUT_PULLUP);
  pinMode(24, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(red_button), red_button_handler, FALLING);
  
  Serial.begin(9600);
  Serial.println("----------------");
  Serial.println("house-of-doom");
  Serial.println("");
  Serial.println("----------------");

  status_led.begin();
}

void loop() {

}

void red_button_handler() {
  status_led.toggle();
  
  if (status_led.is_active()) {
    status_led.random_color();
  }

  status_led.show();
}