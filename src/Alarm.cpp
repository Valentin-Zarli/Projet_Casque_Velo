//
// Created by Antoine on 03/02/2025.
//

#include "Alarm.h"

Alarm::Alarm(const int pinBuzzer, const int pinLED, const double maxAngle, const double maxTime,
             const double angleAttenuation,
             const double timeAttenuation)
    : pinLED(pinLED), pinBuzzer(pinBuzzer), alarmState(false), silence(false), initialExceedTime(0),
      blinkState(false), lastBlinkChange(0), maxAngle(maxAngle), maxTime(maxTime),
      angleAttenuation(angleAttenuation), timeAttenuation(timeAttenuation)
{
}

void Alarm::init() const
{
    pinMode(pinLED, OUTPUT); // Configure le pin de la LED en sortie
    // pinMode(pinBuzzer, OUTPUT); // Configure le pin du buzzer en sortie

    // Buzzer test
    tone(pinBuzzer, 500);
    delay(50);
    noTone(pinBuzzer);

    // Initialise la LED état bas (éteinte)
    digitalWrite(pinLED, LOW);
}

boolean Alarm::update(const float (&ypr_diff)[3], const double speed)
{
    const std::pair<double, double> limits = getLimits(speed);
    const double limitAngle = limits.first;
    const double limitTime = limits.second;
    const double diffPitch = ypr_diff[2];

    // Log toutes les 3 secondes
    static unsigned long lastLog = 0;
    if (millis() - lastLog >= 1000){
        lastLog = millis();
        Serial.print("Speed: "); Serial.print(speed);
        Serial.print(" km/h | DiffPitch: "); Serial.print(diffPitch);
        Serial.print("° | LimitAngle: "); Serial.print(limitAngle);
        Serial.print("° | LimitTime: "); Serial.print(limitTime);
        Serial.print("s | AlarmState: "); Serial.println(alarmState ? "ON" : "OFF");
    }

    if (abs(diffPitch) > limitAngle)
    {
        if (initialExceedTime == 0)
        {
            initialExceedTime = millis();
            Serial.println(">>> Début dépassement angle !");
        }
        Serial.print("Temps dépassement: "); Serial.print((millis() - initialExceedTime) / 1000.0); Serial.println("s");
        if (millis() - initialExceedTime > limitTime * 1000)
        {
            alarmState = true;
            Serial.println(">>> ALARME DECLENCHEE !");
        }
    }
    else
    {
        initialExceedTime = 0;
        alarmState = false;
    }

    if (alarmState)
    {
        start();
        return true;
    }
    else
    {
        stop();
        return false;
    }
}

std::pair<double, double> Alarm::getLimits(const double v_kmh) const
{
    // Calcul des limites d'angle et de temps en fonction de la vitesse : fonction exponentielle décroissante
    double limitAngle = (maxAngle * std::exp(-(angleAttenuation * v_kmh)));
    double limitTime = (maxTime * std::exp((-timeAttenuation * v_kmh)));

    // Retourne les limites
    return std::make_pair(limitAngle, limitTime);
}

void Alarm::start()
{
    Serial.println("Alarm started");
    // Si l'alarme n'est pas en mode silence
    if (!silence)
    {
        // Active le buzzer
        tone(pinBuzzer, 500);
    }

    // Clignotement de la LED toutes les 500ms
    if (lastBlinkChange == 0 || millis() - lastBlinkChange > 500)
    {
        blinkState = !blinkState;
        lastBlinkChange = millis();
    }
    if (blinkState)
    {
        digitalWrite(pinLED, HIGH);
    }
    else
    {
        digitalWrite(pinLED, LOW);
    }
}

void Alarm::stop() const
{
    // On coupe le buzzer et on éteint la LED
    digitalWrite(pinLED, LOW);
    noTone(pinBuzzer);
}
