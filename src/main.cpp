#include <Arduino.h>
#include <SPI.h>

#include <TFT_eSPI.h>
#include "utilities.h"
#include <lvgl.h>

#include <gt911_lite.h>
#include "Wire.h"

#include <Mesh.h>

#include "helpers/sensors/EnvironmentSensorManager.h"
#include "helpers/sensors/MicroNMEALocationProvider.h"
#include <target.h>
#include "mesh_instance.h"

#include "screens/screen_manager.h"

#define TOUCH_INT  16

#define DRAW_BUF_SIZE (TFT_WIDTH * TFT_HEIGHT / 10 * (LV_COLOR_DEPTH/8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

//TFT_eSPI  tft;

GT911_Lite  tp;




// LilyGo  T-Deck  control backlight chip has 16 levels of adjustment range
// The adjustable range is 0~15, 0 is the minimum brightness, 16 is the maximum brightness
void setBrightness(uint8_t value)
{
    static uint8_t level = 0;
    static uint8_t steps = 16;
    if (value == 0) {
        digitalWrite(BOARD_BL_PIN, 0);
        delay(3);
        level = 0;
        return;
    }
    if (level == 0) {
        digitalWrite(BOARD_BL_PIN, 1);
        level = steps;
        delayMicroseconds(30);
    }
    int from = steps - level;
    int to = steps - value;
    int num = (steps + to - from) % steps;
    for (int i = 0; i < num; i++) {
        digitalWrite(BOARD_BL_PIN, 0);
        digitalWrite(BOARD_BL_PIN, 1);
    }
    level = value;
}

static void drag_event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);

    lv_indev_t * indev = lv_indev_active();
    if(indev == NULL)  return;

    lv_point_t vect;
    lv_indev_get_vect(indev, &vect);

    int32_t x = lv_obj_get_x_aligned(obj) + vect.x;
    int32_t y = lv_obj_get_y_aligned(obj) + vect.y;
    lv_obj_set_pos(obj, x, y);
}




bool touch_swap_xy = false;
int16_t touch_map_x1 = -1;
int16_t touch_map_x2 = -1;
int16_t touch_map_y1 = -1;
int16_t touch_map_y2 = -1;

int16_t touch_max_x = 0, touch_max_y = 0;
int16_t touch_raw_x = 0, touch_raw_y = 0;
int16_t touch_last_x = 0, touch_last_y = 0;

void touch_init(int16_t w, int16_t h, uint8_t r)
{
  touch_max_x = w - 1;
  touch_max_y = h - 1;
  if (touch_map_x1 == -1)
  {
    switch (r)
    {
    case 3:
      touch_swap_xy = true;
      touch_map_x1 = touch_max_x;
      touch_map_x2 = 0;
      touch_map_y1 = 0;
      touch_map_y2 = touch_max_y;
      break;
    case 2:
      touch_swap_xy = false;
      touch_map_x1 = touch_max_x;
      touch_map_x2 = 0;
      touch_map_y1 = touch_max_y;
      touch_map_y2 = 0;
      break;
    case 1:
      touch_swap_xy = true;
      touch_map_x1 = 0;
      touch_map_x2 = touch_max_x;
      touch_map_y1 = touch_max_y;
      touch_map_y2 = 0;
      break;
    default: // case 0:
      touch_swap_xy = false;
      touch_map_x1 = 0;
      touch_map_x2 = touch_max_x;
      touch_map_y1 = 0;
      touch_map_y2 = touch_max_y;
      break;
    }
}
}

void touch_read(lv_indev_t * indev, lv_indev_data_t * data)
{
  if (tp.read())
  {
    for (int i = 0; i < tp.touches; i++) {
        data->state = LV_INDEV_STATE_PRESSED;

        if (touch_swap_xy)
        {
            touch_last_x = map(tp.points[i].y, touch_map_x1, touch_map_x2, 0, touch_max_x);
            touch_last_y = map(tp.points[i].x, touch_map_y1, touch_map_y2, 0, touch_max_y);
        }
        else
        {
            touch_last_x = map(tp.points[i].x, touch_map_x1, touch_map_x2, 0, touch_max_x);
            touch_last_y = map(tp.points[i].y, touch_map_y1, touch_map_y2, 0, touch_max_y);
        }

        data->point.x = touch_last_x;
        data->point.y = touch_last_y;
        return;
    }
  }
  else
  {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void halt() {
  while (1) ;
}

lv_obj_t * ta;

void setup()
{
    setup_initial();

    Serial.begin(115200);

    tp.begin(&internal_wire);
    touch_init(TFT_WIDTH, TFT_HEIGHT, 2);

    Serial.println("T-DECK factory");

    //! The board peripheral power control pin needs to be set to HIGH when using the peripheral
    pinMode(BOARD_POWERON, OUTPUT);
    digitalWrite(BOARD_POWERON, HIGH);

    delay(500);

    
    pinMode(P_LORA_MISO, INPUT_PULLUP);

    //! Set CS on all SPI buses to high level during initialization
    pinMode(BOARD_SDCARD_CS, OUTPUT);
    pinMode(RADIO_CS_PIN, OUTPUT);
    pinMode(BOARD_TFT_CS, OUTPUT);

    digitalWrite(BOARD_SDCARD_CS, HIGH);
    digitalWrite(RADIO_CS_PIN, HIGH);
    digitalWrite(BOARD_TFT_CS, HIGH);

    pinMode(BOARD_SPI_MISO, INPUT_PULLUP);
    //SPI.begin(BOARD_SPI_SCK, BOARD_SPI_MISO, BOARD_SPI_MOSI); //SD

    lv_init();

    // /* Set a tick source so that LVGL will know how much time elapsed. */
    lv_tick_set_cb((lv_tick_get_cb_t)millis);

    // /* Create a display using the inbuilt LVGL support for TFT_eSPI*/
    lv_display_t *disp = lv_tft_espi_create(TFT_WIDTH, TFT_HEIGHT, draw_buf, sizeof(draw_buf));

    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90);

    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touch_read);
    
    Serial.println("LVGL initialized");

    pinMode(BOARD_BL_PIN, OUTPUT);
    for (int i = 0; i <= 16; ++i) {
         setBrightness(i);
         delay(30);
    }

    if (!radio_init()) { halt(); }

    SPIFFS.begin(true);

    setup_mesh();
    
    screen_manager::switch_screen(screen_manager::ScreenId::SCREEN_HOME, nullptr);
}

void loop()
{
    screen_manager::loop();
    mesh_client.loop();
    lv_task_handler();
    rtc_clock.tick();
}