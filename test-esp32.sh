#!/bin/bash

if [[ $2 == tests/* ]]; then
  C="../components"
else
  C="components"
fi

esphome -s mac_address B0:A7:32:13:57:72 -s external_components_source $C ${1:-run} ${2:-esp32-ble-example-faker.yaml}
