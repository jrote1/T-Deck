#include "screen_manager.h"
#include <MeshClient.h>

#include "../font.h"

struct ChannelMessage {
   uint32_t timestamp;

   String user;
   String text;
};

class ChatScreen : public screen_manager::Screen
{
public:
    ChatScreen()
    {
    }
    
    virtual void setup() override;
    virtual void loop() override;
    virtual void load(void* param) override;

    void renderMessage(String user, String text, bool sentBySelf);
private:
    void readKeyboard();

    std::vector<ChannelMessage> _channelMessages[MAX_GROUP_CHANNELS];
    int _channelIdx = -1;
    int _newChannelIdx;
    int _currentMsgOffset;
    int _nextMessagePos = 0;

    lv_obj_t* _messagesContainer;
    lv_obj_t * _ta;
    String _keyboardText = "";
};