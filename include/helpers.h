uint8_t readHystBeg(uint8_t hyst){
    int val = analogRead(A1);
    // Serial.println(val);
    return map(val, 0, 1023, 10, 40 - hyst);
}

void drawReleysAndTemp(uint8_t activeReley, float temp)
{
    uint8_t top = 8;
    oled.rect(4, top, 36, top + 7, activeReley == 0 ? OLED_FILL : OLED_STROKE);
    oled.rect(92, top, 124, top + 7, activeReley == 1 ? OLED_FILL : OLED_STROKE);
    oled.setScale(1);
    oled.setCursorXY(50, top + 1); // курсор в (пиксель X, пиксель Y)
    oled.print(temp, 1);
}

void drawScale()
{
    oled.setScale(1);
    const uint8_t base_y = 44;
    for (uint8_t i = 0; i < 31; i++)
    {
        uint8_t ofs = i * 4 + 4;
        if (i % 10 == 0)
        {
            oled.setCursorXY(i > 20 ? ofs - 8 : ofs - 4, 56); // курсор в (пиксель X, пиксель Y)
            oled.print(i + 10);
        }
        uint8_t y = (i % 5 == 0) ? base_y - 4 : base_y;
        oled.rect(ofs, y, ofs + 2, base_y + 2, OLED_FILL);
    }
}

void drawMode(uint8_t beg, uint8_t hyst)
{
    uint8_t x_beg = map(beg, 10, 40, 4, 124);
    uint8_t x_end = map(beg + hyst, 10, 40, 4, 124);
    oled.rect(x_beg, 50, x_end, 52, OLED_FILL);
}

void drawTemp(float val){
    Serial.println(val);
    uint8_t t = constrain(round(val), 10, 40);
    uint8_t x = map(t, 10, 40, 4, 124);
    oled.circle(x - 1, 33, 3, OLED_FILL);
}