#pragma once

#include <Arduino.h>
#include <Wire.h>

class Keyboard
{
public:
    Keyboard(TwoWire& wire) : _wire(wire) {}

    void setup();
    void loop(bool full);
    void setBrightness(uint8_t value);
    void registerKeyEventHandler(void (*handler)(char key));

private:
    TwoWire _wire;
};