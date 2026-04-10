#pragma once

#include <lvgl.h>


namespace screen_manager {

class Screen {
  public:
    virtual void setup(){};
    virtual void loop(){};
    virtual void load(void* param){};

    lv_obj_t* lv_scr() { return _scr; }
    inline bool is_setup() { return _scr != nullptr; }
  protected:
    lv_obj_t* _scr;
};

enum ScreenId
{
    SCREEN_NONE,
    SCREEN_HOME,
    SCREEN_CHANNEL_LIST,
    SCREEN_CHAT,
};


void switch_screen(ScreenId screen_id, void* param);
void loop();

}

