# esphome-daly-bms

![GitHub actions](https://github.com/syssi/esphome-daly-bms/actions/workflows/ci.yaml/badge.svg)
![GitHub stars](https://img.shields.io/github/stars/syssi/esphome-daly-bms)
![GitHub forks](https://img.shields.io/github/forks/syssi/esphome-daly-bms)
![GitHub watchers](https://img.shields.io/github/watchers/syssi/esphome-daly-bms)
[!["Buy Me A Coffee"](https://img.shields.io/badge/buy%20me%20a%20coffee-donate-yellow.svg)](https://www.buymeacoffee.com/syssi)

ESPHome component to monitor a DALY Battery Management System via BLE

## Supported devices

* Daly Smart BMS K Series 100A advertised via BLE as `DL-xxxxxxxxxxxx` (e.g. `DL-40D63C3223A2`) or `DL-Fxxxxxxxxxxxx` (e.g. `DL-F28BBED000824`) where the MAC address is the `xxxxxxxxxxxx`
* Probably all models of the Daly Smart BMS H/K/M/S series (start of frame: `0xD2`, Modbus frames)

## Unsupported devices because of a different protocol

* Daly Smart BMS J/T/A/U/W/ND series (start of frame: `0xA5`)

## Alternative for wired communication

If you prefer a wired connection over Bluetooth, consider using the alternative project by @patagonaa: https://github.com/patagonaa/esphome-daly-hkms-bms

This custom component handles the DALY BMS serial protocol directly (which is similar to Modbus but incompatible with the official standard) and works with wired connections.

## Requirements

* [ESPHome 2024.6.0 or higher](https://github.com/esphome/esphome/releases)
* Generic ESP32 board

## Installation

You can install this component with [ESPHome external components feature](https://esphome.io/components/external_components.html) like this:
```yaml
external_components:
  - source: github://syssi/esphome-daly-bms@main
```

or just use the `esp32-ble-example.yaml` as proof of concept:

```bash
# Install esphome
pip3 install esphome

# Clone this external component
git clone https://github.com/syssi/esphome-daly-bms.git
cd esphome-daly-bms

# Create a secrets.yaml containing some setup specific secrets
cat > secrets.yaml <<EOF
bms0_mac_address: MY_BMS_MAC_ADDRESS

wifi_ssid: MY_WIFI_SSID
wifi_password: MY_WIFI_PASSWORD

mqtt_host: MY_MQTT_HOST
mqtt_username: MY_MQTT_USERNAME
mqtt_password: MY_MQTT_PASSWORD
EOF

# Validate the configuration, create a binary, upload it, and start logs
esphome run esp32-ble-example.yaml

```

## Example response all sensors enabled

```
[D][sensor:094]: 'daly-bms-ble cell voltage 1': Sending state 3.43800 V with 3 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble cell voltage 2': Sending state 3.43300 V with 3 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble cell voltage 3': Sending state 3.41700 V with 3 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble cell voltage 4': Sending state 3.54300 V with 3 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble cell voltage 5': Sending state 3.33900 V with 3 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble cell voltage 6': Sending state 3.33900 V with 3 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble cell voltage 7': Sending state 3.34000 V with 3 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble cell voltage 8': Sending state 3.33900 V with 3 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble min cell voltage': Sending state 3.33900 V with 3 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble max cell voltage': Sending state 3.54300 V with 3 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble max voltage cell': Sending state 4.00000  with 0 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble min voltage cell': Sending state 5.00000  with 0 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble average cell voltage': Sending state 3.39850 V with 4 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble temperature sensors': Sending state 1.00000  with 0 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble temperature 1': Sending state 21.00000 °C with 1 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble total voltage': Sending state 27.10000 V with 2 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble current': Sending state 0.00000 A with 2 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble state of charge': Sending state 100.00000 % with 0 decimals of accuracy
[I][daly_bms_ble:257]: Status: Idle
[D][sensor:094]: 'daly-bms-ble capacity remaining': Sending state 25.00000 Ah with 1 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble cell count': Sending state 8.00000  with 0 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble temperature sensors': Sending state 1.00000  with 0 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble charging cycles': Sending state 0.00000  with 0 decimals of accuracy
[11:50:17][D][binary_sensor:036]: 'daly-bms-ble balancing': Sending state ON
[11:50:17][D][binary_sensor:036]: 'daly-bms-ble charging': Sending state OFF
[11:50:17][D][binary_sensor:036]: 'daly-bms-ble discharging': Sending state ON
[D][sensor:094]: 'daly-bms-ble delta cell voltage': Sending state 0.204 V with 3 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble power': Sending state 0.00000 W with 0 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble charging power': Sending state 0.00000 W with 0 decimals of accuracy
[D][sensor:094]: 'daly-bms-ble discharging power': Sending state 0.00000 W with 0 decimals of accuracy
[W][component:237]: Component interval took a long time for an operation (298 ms).
[W][component:238]: Components should block for at most 30 ms.
```

## Protocol

See [dalyModbusProtocol.xlsx](docs/dalyModbusProtocol.xlsx)

## Known issues

None.

## Debugging

If this component doesn't work out of the box for your device please update your configuration to increase the log level to see details about the BLE communication and incoming traffic:

```
logger:
  level: VERY_VERBOSE
  logs:
    esp32_ble: DEBUG
    esp32_ble_tracker: VERY_VERBOSE
    daly_bms_ble: VERY_VERBOSE
    scheduler: DEBUG
    component: DEBUG
    sensor: DEBUG
    mqtt: INFO
    mqtt.idf: INFO
    mqtt.component: INFO
    mqtt.sensor: INFO
    mqtt.switch: INFO
    api.service: INFO
    api: INFO
    api: INFO
```

## References

* https://github.com/roccotsi2/esp32-smart-bms-simulation
* https://github.com/fl4p/batmon-ha/blob/master/bmslib/models/daly2.py
* https://github.com/patman15/BMS_BLE-HA/blob/main/custom_components/bms_ble/plugins/daly_bms.py
* https://github.com/patman15/BMS_BLE-HA/blob/main/tests/test_daly_bms.py
* https://esphome.io/components/sensor/daly_bms.html
* https://diysolarforum.com/threads/a-new-uart-protocol-for-a-daly-smart-150a-bms.86306/
