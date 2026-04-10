#include "chat.h";

#include "../mesh_instance.h"
#include <cstdint> 
#include "../utilities.h"

void ChatScreen::readKeyboard(){
    char keyValue = 0;
    internal_wire.requestFrom(LILYGO_KB_SLAVE_ADDRESS, 1);
    while (internal_wire.available() > 0) {
        keyValue = internal_wire.read();
        if (keyValue != (char)0x00) {
            Serial.print("keyValue : ");
            Serial.println(keyValue);

            if (keyValue == '\b'){
              lv_textarea_delete_char(_ta);
              _keyboardText = _keyboardText.substring(0, _keyboardText.length() - 1);
            }else if (keyValue == '\n' || keyValue == '\r') {
              ChannelDetails dest;
              mesh_client.getChannel(_channelIdx, dest);
              mesh_client.sendGroupMessage(rtc_clock.getCurrentTime(), dest.channel, mesh_client.getNodeName(), _keyboardText.c_str(), _keyboardText.length());
              renderMessage(mesh_client.getNodeName(), _keyboardText, true);
              lv_textarea_set_text(_ta, "");
              _keyboardText = "";
            }else{
              lv_textarea_add_text(_ta, (String() + keyValue).c_str());
              _keyboardText += keyValue;
            }

            
        }
    }
}

void btn_event_cb(lv_event_t * e)
{
    screen_manager::switch_screen(screen_manager::ScreenId::SCREEN_CHANNEL_LIST, nullptr);
}

void ChatScreen::setup()
{
    _scr = lv_obj_create(NULL);

    lv_obj_t * btn = lv_button_create(_scr);     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 0, 0);                            /*Set its position*/
    lv_obj_set_size(btn, 30, 10);                          /*Set its size*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, 0);           /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(label, "back");                     /*Set the labels text*/
    lv_obj_center(label);

    _messagesContainer = lv_obj_create(_scr);
    lv_obj_set_pos(_messagesContainer, 0, 10);
    lv_obj_set_size(_messagesContainer, 320, 185);

    _ta = lv_textarea_create(_scr);
    lv_textarea_set_one_line(_ta, true);
    lv_obj_set_pos(_ta, 5, 200);
    lv_obj_set_width(_ta, 310);


    mesh_client.registerChannelMessageHandler([this](const mesh::GroupChannel &channel, mesh::Packet *pkt, uint32_t timestamp, const char *text) {
        auto idx = mesh_client.findChannelIdx(channel);

        String user_message = text;

        String s1, s2;

        size_t pos = user_message.indexOf(": ");
        s1 = user_message.substring(0, pos);
        s2 = user_message.substring(pos + 2, user_message.length());

        _channelMessages[idx].push_back({timestamp, s1, s2});

        Serial.printf("Received message for channel %d: %s\n", idx, text);
    });
}

void ChatScreen::renderMessage(String user, String text, bool sentBySelf) {
    int y = _nextMessagePos;

    auto label = lv_label_create(_messagesContainer);
    lv_label_set_text(label, user.c_str());
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_align(label, sentBySelf ? LV_ALIGN_TOP_RIGHT : LV_ALIGN_TOP_LEFT, 0, y);


    static lv_style_t chatBoxStyle;
    lv_style_init(&chatBoxStyle);

    /*Set a background color and a radius*/
    lv_style_set_radius(&chatBoxStyle, 10);
    lv_style_set_bg_opa(&chatBoxStyle, LV_OPA_COVER);
    lv_style_set_bg_color(&chatBoxStyle, lv_palette_lighten(sentBySelf ? LV_PALETTE_BLUE : LV_PALETTE_GREY, 1));



    
    label = lv_label_create(_messagesContainer);
    lv_obj_add_style(label, &chatBoxStyle, 0);
    lv_label_set_text(label, text.c_str());
    lv_obj_set_style_text_color(label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_align(label, sentBySelf ? LV_ALIGN_TOP_RIGHT : LV_ALIGN_TOP_LEFT, 0, y + 20);
    lv_obj_set_style_pad_all(label, 10, 0);

    lv_obj_refr_size(label);

    if(lv_obj_get_width(label) > 240) {
        lv_obj_set_width(label, 240);
    }

    lv_obj_update_layout(label);

    _nextMessagePos += (26 + lv_obj_get_height(label)); // TODO - calculate based on text height, and add spacing

    lv_obj_scroll_to_y(_messagesContainer, _nextMessagePos, LV_ANIM_OFF); // Scroll to bottom to show new message
}

void ChatScreen::loop()
{
    readKeyboard();

    int offset = 0;
    Serial.printf("Current message offset: %d\n", _currentMsgOffset);

    if (_channelMessages[_channelIdx].size() == 0) {
        return;
    }

    for (const ChannelMessage& msg : _channelMessages[_channelIdx]) {
        if (offset < _currentMsgOffset) {
            offset++;
            continue;
        }

        renderMessage(msg.user, msg.text, false);
        offset += 1;
    }
    _currentMsgOffset = offset;
    
}

void ChatScreen::load(void* param) {
    auto newChannelIdx = reinterpret_cast<uintptr_t>(param);
    if(newChannelIdx != _channelIdx) {
        _channelIdx = newChannelIdx;
        _currentMsgOffset = 0;
        _nextMessagePos = 0;
        lv_obj_clean(_messagesContainer); // Clear messages when switching channels
    }

    _channelIdx = newChannelIdx;
    
    int offset = 0;

    // if (_channelMessages[_channelIdx].size() == 0) {
    //     auto label = lv_label_create(_scr);
    //     lv_label_set_text(label, std::to_string(_channelIdx).c_str());
    //     lv_obj_set_style_text_color(_scr, lv_color_hex(0x000000), LV_PART_MAIN);
    //     lv_obj_align(label, LV_ALIGN_TOP_MID, 0, offset * 10);
    //     return;
    // }

    for (const ChannelMessage& msg : _channelMessages[_channelIdx]) {
        if (offset < _currentMsgOffset) {
            offset++;
            continue;
        }

        renderMessage(msg.user, msg.text, false);
        offset += 1;
    }
    _currentMsgOffset = offset;
    //_newChannelIdx = *((int*)param); // TODO - validate param
    // TODO - set current channel based on param (channel index or id), and refresh message list
}