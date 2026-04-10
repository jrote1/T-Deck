#pragma once

#include "screen_manager.h"
#include <MeshClient.h>

#include <lvgl.h>

#include "../mesh_instance.h"

class ChannelListScreen : public screen_manager::Screen {
  public:

    ChannelListScreen() {
    }
    
    virtual void setup() override;
    virtual void loop() override;
    virtual void load(void* param) override;
};