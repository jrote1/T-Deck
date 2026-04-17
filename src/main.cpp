#include <Arduino.h>
#include <SPI.h>

#include "utilities.h"

#include "Wire.h"

#include <Mesh.h>

#include "helpers/sensors/EnvironmentSensorManager.h"
#include "helpers/sensors/MicroNMEALocationProvider.h"
#include <target.h>
#include "mesh_instance.h"

#include "screens/screen_manager.h"

#include "utilities/display.h"

void halt()
{
  while (1)
    ;
}

lv_obj_t *ta;

void setup()
{
  setup_initial();

  Serial.begin(115200);

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
  // SPI.begin(BOARD_SPI_SCK, BOARD_SPI_MISO, BOARD_SPI_MOSI); //SD

  if (!radio_init())
  {
    halt();
  }

  SPIFFS.begin(true);

  setup_mesh();

  display.setup();
  display.setBrightness(10);
  display.setSleepTimeout(30000); // Set display sleep timeout to 60 seconds
}

void loop()
{
  display.loop(true);
  mesh_client.loop();
  rtc_clock.tick();
}