#pragma once


#include "MeshClient.h"

#include <SPIFFS.h>

#include <helpers/esp32/SerialBLEInterface.h>
#include <Wire.h>

extern StdRNG fast_rng;
extern SimpleMeshTables tables;
extern DataStore store;
extern MeshClient mesh_client;
extern TwoWire internal_wire;

extern void setup_initial();
extern void setup_mesh();