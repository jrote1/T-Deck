#include "mesh_instance.h"
#include "utilities.h"

TwoWire internal_wire(0);

SerialBLEInterface serial_interface;

StdRNG fast_rng;
SimpleMeshTables tables;
DataStore store(SPIFFS, rtc_clock);

MeshClient mesh_client(radio_driver, fast_rng, rtc_clock, tables, store);

void setKeyboardBrightness(uint8_t value)
{
    internal_wire.beginTransmission(LILYGO_KB_SLAVE_ADDRESS);
    internal_wire.write(LILYGO_KB_BRIGHTNESS_CMD);
    internal_wire.write(value);
    internal_wire.endTransmission();
}

/*
* Set the default backlight brightness level. If the user sets the backlight to 0
* via setKeyboardBrightness, the default brightness is used when pressing ALT+B,
* rather than the backlight brightness level set by the user. This ensures that
* pressing ALT+B can respond to the backlight being turned on and off normally.
* Brightness Range: 30 ~ 255
* */
void setKeyboardDefaultBrightness(uint8_t value)
{
    internal_wire.beginTransmission(LILYGO_KB_SLAVE_ADDRESS);
    internal_wire.write(LILYGO_KB_ALT_B_BRIGHTNESS_CMD);
    internal_wire.write(value);
    internal_wire.endTransmission();
}

void setup_initial() {
    internal_wire.begin(BOARD_I2C_SDA, BOARD_I2C_SCL);


}

void setup_mesh(){

    fast_rng.begin(radio_get_rng_seed());
    
    store.begin();
    mesh_client.begin(SPIFFS);

    serial_interface.begin(BLE_NAME_PREFIX, mesh_client.getNodePrefs()->node_name, mesh_client.getBLEPin());

    Serial.println(mesh_client.getBLEPin());

    mesh_client.startInterface(serial_interface);

    internal_wire.requestFrom(LILYGO_KB_SLAVE_ADDRESS, 1);
    if (internal_wire.read() == -1) {
        while (1) {
            Serial.println("LILYGO Keyboard not online .");
            delay(1000);
        }
    }
    // Set the default backlight brightness level.
    setKeyboardDefaultBrightness(127);
}