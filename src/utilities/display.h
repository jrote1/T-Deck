#pragma once

#include <Arduino.h>
#include <Wire.h>

class Display
{
public:
    Display(TwoWire &wire) : _wire(wire) {}

    void setup();
    void loop(bool full);
    void wakeup();
    void setBrightness(uint8_t brightness);
    void setSleepTimeout(uint32_t timeout_ms);

private:
    TwoWire _wire;
};