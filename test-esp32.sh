#!/bin/bash

esphome -s mac_address B0:A7:32:13:57:72 -s external_components_source components ${1:-run} ${2:-esp32-ble-example-faker.yaml}
