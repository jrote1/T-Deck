#include "display.h"
#include "../utilities.h"
#include <screens/screen_manager.h>
#include "../mesh_instance.h"

static bool touch_swap_xy = false;
static int16_t touch_map_x1 = -1;
static int16_t touch_map_x2 = -1;
static int16_t touch_map_y1 = -1;
static int16_t touch_map_y2 = -1;

static int16_t touch_max_x = 0, touch_max_y = 0;
static int16_t touch_raw_x = 0, touch_raw_y = 0;
static int16_t touch_last_x = 0, touch_last_y = 0;

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

typedef struct {
    uint8_t cmd;
    uint8_t data[14];
    uint8_t len;
} lcd_cmd_t;

lcd_cmd_t lcd_st7789v[] = {
    {0x01, {0}, 0 | 0x80},
    {0x11, {0}, 0 | 0x80},
    {0x3A, {0X05}, 1},
    {0x36, {0x55}, 1},
    {0xB2, {0x0C, 0x0C, 0X00, 0X33, 0X33}, 5},
    {0xB7, {0X75}, 1},
    {0xBB, {0X1A}, 1},
    {0xC0, {0X2C}, 1},
    {0xC2, {0X01}, 1},
    {0xC3, {0X13}, 1},
    {0xC4, {0X20}, 1},
    {0xC6, {0X0F}, 1},
    {0xD0, {0XA4, 0XA1}, 2},
    {0xD6, {0XA1}, 1},
    {0xE0, {0XD0, 0X0D, 0X14, 0X0D, 0X0D, 0X09, 0X38, 0X44, 0X4E, 0X3A, 0X17, 0X18, 0X2F, 0X30}, 14},
    {0xE1, {0XD0, 0X09, 0X0F, 0X08, 0X07, 0X14, 0X37, 0X44, 0X4D, 0X38, 0X15, 0X16, 0X2C, 0X3E}, 14},
    {0x21, {0}, 0}, //invertDisplay
    {0x29, {0}, 0},
    {0x2C, {0}, 0},
};

static Display *_display_instance = nullptr;

void touchRead_callback(lv_indev_t *indev, lv_indev_data_t *data)
{
    _display_instance->touchRead(indev, data);
}

GT911_Lite tp;

void Display::touchRead(lv_indev_t *indev, lv_indev_data_t *data)
{
    if (tp.read())
    {
        for (int i = 0; i < tp.touches; i++)
        {
            data->state = LV_INDEV_STATE_PRESSED;
            wakeup();

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

typedef struct {
    TFT_eSPI * tft;
} lv_tft_espi_t;

#define DRAW_BUF_SIZE (TFT_WIDTH * TFT_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

// static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
// {
//     lv_tft_espi_t * dsc = (lv_tft_espi_t *)lv_display_get_driver_data(disp);

//     uint32_t w = (area->x2 - area->x1 + 1);
//     uint32_t h = (area->y2 - area->y1 + 1);

//     dsc->tft->startWrite();
//     dsc->tft->setAddrWindow(area->x1, area->y1, w, h);
//     dsc->tft->pushColors((uint16_t *)px_map, w * h, false);
//     dsc->tft->endWrite();

//     lv_display_flush_ready(disp);

// }

void Display::setup()
{
    tp.begin(&internal_wire);
    touch_init(TFT_WIDTH, TFT_HEIGHT, 2);

    lv_init();

    // /* Set a tick source so that LVGL will know how much time elapsed. */
    lv_tick_set_cb((lv_tick_get_cb_t)millis);

    // /* Create a display using the inbuilt LVGL support for TFT_eSPI*/
    lv_display_t *disp = lv_tft_espi_create(TFT_WIDTH, TFT_HEIGHT, draw_buf, sizeof(draw_buf));
    //lv_display_set_flush_cb(disp, flush_cb);

    lv_tft_espi_t * dsc = (lv_tft_espi_t *)lv_display_get_driver_data(disp);
    //dsc->tft->invertDisplay(1);

    for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++) {
        dsc->tft->writecommand(lcd_st7789v[i].cmd);
        for (int j = 0; j < (lcd_st7789v[i].len & 0x7f); j++) {
            dsc->tft->writedata(lcd_st7789v[i].data[j]);
        }

        if (lcd_st7789v[i].len & 0x80) {
            delay(120);
        }
    }

    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90);

    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);

    _display_instance = this; // Set the global instance pointer for touch callback access
    lv_indev_set_read_cb(indev, touchRead_callback);

    Serial.println("LVGL initialized");

    screen_manager::switch_screen(screen_manager::ScreenId::SCREEN_HOME, nullptr);

    pinMode(BOARD_BL_PIN, OUTPUT);
    for (int i = 0; i <= 16; ++i)
    {
        setBrightness(i);
        delay(30);
    }

    wakeup();
}

void Display::loop(bool full)
{
    if(full){
        if(_lastWakeTime != 0 && (std::time(0) - _lastWakeTime) * 1000 >= _sleepTimeoutMs) {
            setBrightness(0);
        }else{
            setBrightness(16);
        }
    }
    screen_manager::loop();
    lv_task_handler();
    // TODO - implement display loop handling (e.g. for sleep/wake)
}

void Display::wakeup()
{
    _lastWakeTime = std::time(0);
}

void Display::setBrightness(uint8_t brightness)
{
    static uint8_t level = 0;
    static uint8_t steps = 16;
    if (brightness == 0)
    {
        digitalWrite(BOARD_BL_PIN, 0);
        delay(3);
        level = 0;
        return;
    }
    if (level == 0)
    {
        digitalWrite(BOARD_BL_PIN, 1);
        level = steps;
        delayMicroseconds(30);
    }
    int from = steps - level;
    int to = steps - brightness;
    int num = (steps + to - from) % steps;
    for (int i = 0; i < num; i++)
    {
        digitalWrite(BOARD_BL_PIN, 0);
        digitalWrite(BOARD_BL_PIN, 1);
    }
    level = brightness;
}
void Display::setSleepTimeout(uint32_t timeout_ms)
{
    _sleepTimeoutMs = timeout_ms;
}