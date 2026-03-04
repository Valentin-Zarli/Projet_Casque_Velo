#include <Arduino.h>

#define PIN_HAL 25
#define PIN_BUZZER 2

volatile bool triggered = false;
int tourCount = 0;

void IRAM_ATTR onPulse() {
    triggered = true;
}

void setup() {
    Serial.begin(115200);
    pinMode(PIN_HAL, INPUT_PULLUP);
    pinMode(PIN_BUZZER, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_HAL), onPulse, RISING);
}

void loop() {
    if (triggered) {
        triggered = false;
        tourCount++;
        Serial.print("Tour n°: ");
        Serial.println(tourCount);
        tone(PIN_BUZZER, 500);
        delay(50);
        noTone(PIN_BUZZER);
    }
}