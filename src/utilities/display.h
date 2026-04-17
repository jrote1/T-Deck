#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <gt911_lite.h>
#include <lvgl.h>
#include <ctime>
#include <TFT_eSPI.h>

#define TOUCH_INT  16

class Display
{
public:
    Display(TwoWire &wire) : _wire(wire) {}

    void setup();
    void loop(bool full);
    void wakeup();
    void setBrightness(uint8_t brightness);
    void setSleepTimeout(uint32_t timeout_ms);

    void touchRead(lv_indev_t * indev, lv_indev_data_t * data);

private:

    TwoWire _wire;
    GT911_Lite _tp;
    std::time_t _lastWakeTime;
    int _sleepTimeoutMs = 30000; // default sleep timeout of 30 seconds
};