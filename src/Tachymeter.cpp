#include "Tachymeter.h"

// Initialisation de l'instance statique
Tachymeter *Tachymeter::instance = nullptr;


Tachymeter::Tachymeter(const int sensorPin, const int buzzerPin, const float wheelRadius, const float stationarySpeed)
    : stationarySpeed(stationarySpeed), sensorPin(sensorPin), buzzerPin(buzzerPin), wheelRadius(wheelRadius),
      pulseCounter(0), eventFlag(false), previousMillis(0), currentSpeed(0)
{
    stationaryPeriod = (wheelRadius * 2 * PI) / stationarySpeed;
}

// Initialisation
void Tachymeter::initialize() {
    // Configurer les broches
    pinMode(sensorPin, INPUT_PULLUP);
    pinMode(buzzerPin, OUTPUT);
    // Attacher l'interruption pour gérer les impulsions
    attachInterrupt(digitalPinToInterrupt(sensorPin), []() { instance->pulseEvent(); }, RISING);
    previousMillis = millis();
}

void Tachymeter::update() {
    if (eventFlag) {
        eventFlag = false;

        // Faire sonner le buzzer de façon non bloquante
        tone(buzzerPin, 500, 50); // 3ème paramètre = durée automatique, pas de delay !

        // Calculer la vitesse
        const unsigned long currentMillis = millis();
        const float period = (currentMillis - previousMillis) / 1000.0;
        if (period > 0) {
            currentSpeed = (wheelRadius * 2 * PI) / period;
        }
        else {
            currentSpeed = 0;
        }
        previousMillis = currentMillis;
    }
    else {
        unsigned long currentMillis = millis();
        float period = (currentMillis - previousMillis) / 1000.0;
        if (period > stationaryPeriod) {
            currentSpeed = 0;
        }
    }
}

// Méthode pour récupérer la vitesse actuelle
float Tachymeter::getSpeed() const {
    return currentSpeed; // Retourner la vitesse calculée
}

// Méthode privée : gestion de l'interruption
void Tachymeter::pulseEvent() {
    pulseCounter++;
    eventFlag = true; // Signaler qu'un événement a eu lieu
}
