#include "home.h"


static void btn_event_cb(lv_event_t * e)
{
    screen_manager::switch_screen(screen_manager::ScreenId::SCREEN_CHANNEL_LIST, nullptr);
}

void HomeScreen::setup() {
    _scr = lv_obj_create(NULL);

    lv_obj_t * btn = lv_button_create(_scr);     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);           /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(label, "Channels");                     /*Set the labels text*/
    lv_obj_center(label);
}

void HomeScreen::loop() {
    // TODO - show home screen with options to navigate to other screens (e.g. channel list, chat, settings, etc.)
}

void HomeScreen::load(void* param) {
    // TODO - show home screen with options to navigate to other screens (e.g. channel list, chat, settings, etc.)
}