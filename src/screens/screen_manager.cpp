

#include "screen_manager.h"

#include "home.h"
#include "channel_list.h"
#include "chat.h"

namespace screen_manager {

static constexpr ScreenId kInitialScreen = SCREEN_HOME;

struct ScreenDesc {
  ScreenId screen_id;
  Screen* screen_ptr;
};

static HomeScreen home_screen;
static ChannelListScreen channel_list_screen;
static ChatScreen chat_screen;

// Order here determines screen 'next/previous' order.
static const ScreenDesc screen_table[] = {
    {SCREEN_HOME, &home_screen},
    {SCREEN_CHANNEL_LIST, &channel_list_screen},
    {SCREEN_CHAT, &chat_screen},
};

constexpr int kNumScreens = sizeof(screen_table) / sizeof(screen_table[0]);

static const ScreenDesc* current_screen_desc = nullptr;

static const ScreenDesc* find_screen_desc(ScreenId screen_id) {
  // Try to match to the sequential screens.
  for (int i = 0; i < kNumScreens; i++) {
    if (screen_id == screen_table[i].screen_id) {
      const ScreenDesc* new_screen_desc = &screen_table[i];
      return new_screen_desc;
    }
  }
  return nullptr;
}

void switch_screen(ScreenId screen_id, void* param) {
  //const ScreenId current_screen_id = current_screen_desc->screen_id;
  const ScreenDesc* new_screen_desc = find_screen_desc(screen_id);
//   if (new_screen_desc == nullptr ||
//       new_screen_desc->screen_id == current_screen_id) {
//     return;
//   }
  // current_screen_desc->screen_ptr->on_unload();
  if (!new_screen_desc->screen_ptr->is_setup()) {
    new_screen_desc->screen_ptr->setup();
  }
  new_screen_desc->screen_ptr->load(param);
  current_screen_desc = new_screen_desc;
  lv_scr_load(current_screen_desc->screen_ptr->lv_scr());
}

void loop() {
  // Service the current screen.
  current_screen_desc->screen_ptr->loop();
};
}