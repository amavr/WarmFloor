#include <Arduino.h>

// GND --- термистор --- A0 --- 10к --- 5V
#include <GyverNTC.h>
GyverNTC therm(0, 10000, 3200); // пин, сопротивление при 25 градусах (R термистора = R резистора!), бета-коэффициент
// GyverNTC therm(0, 10000, 3435, 25, 10000);	// пин, R термистора, B термистора, базовая температура, R резистора
// серый 4300
// проводной 3950

#include <GyverOLED.h>
GyverOLED<SSD1306_128x64> oled;
// GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;

#define TEMP_PIN 3
#define RELAY0_PIN 2
#define RELAY1_PIN 4

#include <microDS18B20.h>
#include "helpers.h"

bool isActive = false;
bool isActiveBak = false;

uint8_t activeRelay = 0;
uint8_t activeRelayBak = 3;

uint16_t relayTimes = 0;
uint16_t maxTimes = 180;

uint8_t hyst_beg = 20;
uint8_t hyst = 3;

MicroDS18B20<TEMP_PIN> sensor;

void setup()
{
    Serial.begin(115200);
    pinMode(A1, INPUT);
    pinMode(RELAY0_PIN, OUTPUT);
    pinMode(RELAY1_PIN, OUTPUT);
    oled.init(); // инициализация
}

void loop()
{
    sensor.requestTemp();
    delay(1000);
    if (sensor.readTemp())
    {
        float t = sensor.getTemp();
        Serial.println(t);

        hyst_beg = readHystBeg(hyst);

        isActiveBak = isActive;
        // температура выше чем заданный режим
        if (t > hyst_beg + hyst)
        {
            isActive = false;
        }
        // температура меньше заданного режима
        else if (t < hyst_beg)
        {
            isActive = true;
        }

        relayTimes++;
        if (relayTimes >= maxTimes)
        {
            relayTimes = 0;
            activeRelayBak = activeRelay;
            activeRelay = activeRelay == 0 ? 1 : 0;
        }

        if (isActive)
        {
            if (activeRelay == 0)
            {
                if (activeRelayBak != activeRelay)
                {
                    digitalWrite(RELAY0_PIN, HIGH);
                    digitalWrite(RELAY1_PIN, LOW);
                    activeRelayBak = activeRelay;
                }
            }
            else
            {
                if (activeRelayBak != activeRelay)
                {
                    digitalWrite(RELAY0_PIN, LOW);
                    digitalWrite(RELAY1_PIN, HIGH);
                    activeRelayBak = activeRelay;
                }
            }
        }
        else
        {
            if (isActiveBak != isActive)
            {
                digitalWrite(RELAY0_PIN, LOW);
                digitalWrite(RELAY1_PIN, LOW);
                isActiveBak = isActive;
            }
        }

        oled.clear();
        drawReleysAndTemp(isActive ? activeRelay : 2, t);
        drawScale();
        drawMode(hyst_beg, hyst);
        drawTemp(t);
        oled.update();
    }
    else
    {
        Serial.println("error");
    }

    // float t = therm.getTempAverage();
    // float t = 20.0;
    // hyst_beg = readHystBeg(hyst);

    // delay(300);
}
