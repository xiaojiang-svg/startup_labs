#include "main.h"


void LED_ON(void* arg) {
  while (1) {
    LED1_ON;
    vTaskDelay(1000);
  }
}

void LED_OFF(void* arg) {
  while (1) {
    LED1_OFF;
    vTaskDelay(1500);
  }
}