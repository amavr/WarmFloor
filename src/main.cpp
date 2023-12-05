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
bool isForceMode = false;
uint8_t activeRelay = 0;

// цикл опроса датчика температуры + 500ms, т.е. реакции на изменение
unsigned long timeout_ms = 4500;
// переключение реле через 3 минуты
unsigned long switch_period_ms = 180000;
// начало отсчета периода переключения
unsigned long switched_time_ms = 0;

// начало интервала температур
uint8_t hyst_beg = 20;
// интервал заданных температур
uint8_t hyst = 3;

MicroDS18B20<TEMP_PIN> sensor;

void setup()
{
    Serial.begin(115200);
    pinMode(A1, INPUT);
    pinMode(RELAY0_PIN, OUTPUT);
    pinMode(RELAY1_PIN, OUTPUT);
    oled.init(); // инициализация

    switched_time_ms = millis();
}

void loop()
{
    // цикл опроса изменения температура (4.5 сек)
    // иначе отображения на экране с задержкой
    unsigned long beg_cycle = millis();
    while (millis() - beg_cycle < timeout_ms)
    {
        hyst_beg = readHystBeg(hyst);
        drawInterval(hyst_beg, hyst);
        oled.update();
        delay(500);
    }

    hyst_beg = readHystBeg(hyst);
    drawInterval(hyst_beg, hyst);

    // смена активного реле
    if (millis() - switched_time_ms > switch_period_ms)
    {
        // чтобы избежать работы только одной половины (с датчиком температуры)
        // смена происходит только когда реле включено
        if (isActive)
        {
            activeRelay = activeRelay == 0 ? 1 : 0;
        }
        switched_time_ms = millis();
    }

    sensor.requestTemp();
    delay(500);

    if (sensor.readTemp())
    {
        float t = sensor.getTemp();
        // Serial.println(t);

        // включение/выключение пола только при выходе за интервал
        // внутри интервала переключений нет

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

        // форс-мажор при снижении температуры ниже чем интервал на 2 градуса
        // но поскольку суммарная мощность в форс-мажоре большая,
        // то он в одном цикле включается, а в следующем выключается
        isForceMode = t < hyst_beg - 2 && !isForceMode;

        bool left = activeRelay == 0 || isForceMode;
        bool right = activeRelay == 1 || isForceMode;

        if (isActive)
        {
            digitalWrite(RELAY0_PIN, left ? HIGH : LOW);
            digitalWrite(RELAY1_PIN, right ? HIGH : LOW);
        }
        else
        {
            digitalWrite(RELAY0_PIN, LOW);
            digitalWrite(RELAY1_PIN, LOW);
        }

        showState(left, right, t, hyst_beg, hyst);
        // oled.clear();
        // drawReleysAndTemp(isActive ? activeRelay : 2, isForceMode, t);
        // drawScale();
        // drawInterval(hyst_beg, hyst);
        // drawTemp(t);
        // oled.update();
    }
    else
    {
        Serial.println("error");
    }
}
