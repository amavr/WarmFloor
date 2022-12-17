#include <Arduino.h>

// GND --- термистор --- A0 --- 10к --- 5V
#include <GyverNTC.h>
GyverNTC therm(0, 10000, 3200); // пин, сопротивление при 25 градусах (R термистора = R резистора!), бета-коэффициент
// GyverNTC therm(0, 10000, 3435, 25, 10000);	// пин, R термистора, B термистора, базовая температура, R резистора
// серый 4300
// проводной 3950

#include <GyverOLED.h>
GyverOLED<SSD1306_128x64> oled;

#include "helpers.h"


bool isActive = false;
uint8_t activeRelay = 0;
uint16_t relayTimes = 0;
uint16_t maxTimes = 30;

uint8_t hyst_beg = 20;
uint8_t hyst = 6;


void setup()
{
    Serial.begin(115200);
    pinMode(A1, INPUT);
    oled.init(); // инициализация
}

void loop()
{
    float t = therm.getTempAverage();

    hyst_beg = readHystBeg(hyst);

    oled.clear();

    relayTimes++;
    if(relayTimes >= maxTimes){
        relayTimes = 0;
        activeRelay = activeRelay == 0 ? 1 : 0;
    }

    drawReleysAndTemp(activeRelay, t);
    drawScale();
    drawMode(hyst_beg, hyst);
    drawTemp(t);

    oled.update();

    // Serial.print("Temperature ");
    // Serial.print(t);
    // Serial.println(" *C");
    delay(300);
}

