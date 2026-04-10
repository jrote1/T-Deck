#pragma once

#include "screen_manager.h"
#include <MeshClient.h>

#include <lvgl.h>

class HomeScreen : public screen_manager::Screen {
  public:

    HomeScreen() {
    }

    virtual void setup() override;
    virtual void loop() override;
    virtual void load(void* param) override;
};