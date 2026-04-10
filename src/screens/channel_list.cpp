#include "channel_list.h"

static void btn_event_cb(lv_event_t * e)
{
    screen_manager::switch_screen(screen_manager::ScreenId::SCREEN_CHAT, lv_event_get_user_data(e));
}

void ChannelListScreen::setup() {
    _scr = lv_obj_create(NULL);

    int offset = 0;
    for(const ChannelDetails& ch : mesh_client.getChannels()) {   

        if(ch.name == nullptr || strlen(ch.name) == 0) {
            continue;
        }

        lv_obj_t * btn = lv_button_create(_scr);     /*Add a button the current screen*/
        lv_obj_set_pos(btn, 10, 10 + (offset * 60));                            /*Set its position*/
        lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
        lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, (void*)offset);           /*Assign a callback to the button*/

        lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
        lv_label_set_text(label, String(ch.name).c_str());                     /*Set the labels text*/
        lv_obj_center(label);

        offset += 1;
    }
  // TODO - show list of channels, with option to view details or leave channel
}

void ChannelListScreen::loop() {
  // TODO - show list of channels, with option to view details or leave channel
}

void ChannelListScreen::load(void* param) {
  // TODO - show list of channels, with option to view details or leave channel
}