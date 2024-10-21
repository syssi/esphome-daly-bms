# esphome-daly-bms

![GitHub actions](https://github.com/syssi/esphome-daly-bms/actions/workflows/ci.yaml/badge.svg)
![GitHub stars](https://img.shields.io/github/stars/syssi/esphome-daly-bms)
![GitHub forks](https://img.shields.io/github/forks/syssi/esphome-daly-bms)
![GitHub watchers](https://img.shields.io/github/watchers/syssi/esphome-daly-bms)
[!["Buy Me A Coffee"](https://img.shields.io/badge/buy%20me%20a%20coffee-donate-yellow.svg)](https://www.buymeacoffee.com/syssi)

ESPHome component to monitor a DALY Battery Management System via BLE

## Supported devices

* Daly Smart BMS K-Series 100A advertised via BLE as `DL-xxxxxxxxxxxx` f.e. `DL-40D63C3223A2`

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
TBD.
```

## Protocol

TBD.

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
