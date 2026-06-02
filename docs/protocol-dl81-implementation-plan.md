# Implementierungsplan: 0x81-Protokoll-Unterstützung (DL-WB01 und ähnliche Geräte)

## Hintergrund

Nutzer berichten, dass Geräte wie das **F16S48V200A DL-WB01** nicht funktionieren.
Die Analyse der Stock-Android-App (`/tmp/out/sources/`) ergibt: Die App unterstützt
**zwei unterschiedliche Protokoll-Varianten** über die selben BLE-Charakteristika.

Die aktuelle ESPHome-Komponente implementiert nur das **0xD2-Protokoll** (H/K/M/S-Serie).
Das **0x81-Protokoll** (DL-Serie, vermutlich WB01 und andere neuere Modelle) fehlt.

---

## Protokoll-Übersicht

### Gemeinsames BLE-Fundament (beide Protokolle identisch)

| | UUID |
|---|---|
| Service | `0000fff0-0000-1000-8000-00805f9b34fb` |
| Notify/Read | `0000fff1-0000-1000-8000-00805f9b34fb` |
| Write | `0000fff2-0000-1000-8000-00805f9b34fb` |

Modbus-RTU-Framing ist in beiden Protokollen gleich:
```
[slave_addr] [func] [addr_hi] [addr_lo] [val_hi] [val_lo] [crc_lo] [crc_hi]
```
CRC: Standard Modbus CRC16 (bereits implementiert, unverändert).

### Geräteerkennung (aus `BleDeviceActivity.java`)

Die App liest Manufacturer-Specific-Data aus dem BLE-Advertisement-Scan-Record.
Bytes 9 und 10 bestimmen das Routing:

```
Manufacturer Data [9] == 0x59 ('Y') && [10] == 0x43 ('C')  →  YC-Protokoll (0xD2 + andere Register)
Manufacturer Data [9] == 0x4C ('L') && [10] == 0x43 ('C')
                         && [11] == 0x44 ('D')              →  Screen-Protokoll (kein BMS)
Alles andere                                                →  DL-Protokoll (0x81 oder 0xD2 auto-detect)
```

Für die ESPHome-Komponente ist die Geräteerkennung **nicht automatisch möglich** –
der Nutzer muss das Protokoll explizit konfigurieren (`protocol: dl` oder `protocol: d2`).

### Protokoll-Unterschiede im Überblick

| Eigenschaft | 0xD2 (aktuell, H/K/M/S-Serie) | 0x81 (neu, DL-Serie / WB01) |
|---|---|---|
| Slave-Adresse Frame-Byte[0] | `0xD2` | `0x81` |
| Status-Abfrage | 1 Read-Befehl (0x41 oder 0x4E Register ab `0x0000`) | **2 Read-Befehle** (0x40 ab `0x0000`, dann 0x3E ab `0x0041`) |
| Settings-Abfrage | 1 Read-Befehl (0x29 Register ab `0x0080`) | **Mehrere** Read-Befehle ab `0x0100` |
| Balancer-Register | `0x00CF` | `0x00CF` (gleich) |
| Max. Zellen | 32 | 48 |
| Max. Temperatursensoren | 8 | 8 |

---

## Protokolldetails: 0x81 Echtzeit-Daten

### Befehl 1: Register 0x0000–0x003F (0x40 Register = 64 Register)

```
TX: [0x81, 0x03, 0x00, 0x00, 0x00, 0x40, CRC_LO, CRC_HI]
```

Antwort-Frame: `[0x81, 0x03, 0x80, data...(128 Bytes), CRC_LO, CRC_HI]`
Gesamt: 3 + 128 + 2 = **133 Bytes**

Byte-Layout (data[] ab Index 0 = Byte[3] des Frames):

```
Register  Byte   Beschreibung                              Einheit   Faktor
0x0000    3–4    Zellspannung 1                            mV        1
0x0001    5–6    Zellspannung 2                            mV        1
...
0x0000+N  3+N*2  Zellspannung N+1 (max. 48 Zellen)        mV        1
...
0x0030    99–100 Temperatursensor 1 (offset -40)           °C        1
0x0031    101–102 Temperatursensor 2 (offset -40)          °C        1
...
0x0030+N  99+N*2 Temperatursensor N+1 (max. 8)             °C        1
...
0x0038    115–116 Gesamtspannung                           0.1 V     0.1
0x0039    117–118 Strom (offset -30000)                    0.1 A     0.1
0x003A    119–120 SOC                                      0.1 %     0.1
0x003B    121–122 Life/Heartbeat-Counter                   -         -
0x003C    123–124 Anzahl Zellen                            -         1
0x003D    125–126 Anzahl Temperatursensoren                -         1
0x003E    127–128 Maximale Zellspannung                    mV        1
0x003F    129–130 Index maximale Zellspannung (1-basiert)  -         1
```

**Wichtig:** Zellspannung und Temperatursensoranzahl stehen in Register 0x003C/0x003D
und müssen ausgelesen werden, bevor die Zellen/Temperaturen geparst werden.

Konkrete Byte-Offsets (Frame-absolut):
- `data[3 + reg*2]` / `data[4 + reg*2]` für Register `reg` (0x0000-based)

### Befehl 2: Register 0x0041–0x007E (0x3E Register = 62 Register)

```
TX: [0x81, 0x03, 0x00, 0x41, 0x00, 0x3E, CRC_LO, CRC_HI]
```

Antwort-Frame: `[0x81, 0x03, 0x7C, data...(124 Bytes), CRC_LO, CRC_HI]`
Gesamt: 3 + 124 + 2 = **129 Bytes**

Byte-Layout (data[] ab Index 0 = Register 0x0041, Byte[3] des Frames):

```
Register  Offset  Beschreibung                                  Einheit  Faktor
0x0041    0       (reserviert / Min-Zellspannung-Index?)        -        -
0x0042    1       (reserviert)                                  -        -
0x0043    2       Maximale Batterietemperatur (offset -40)      °C       1
0x0044    3       Index maximale Batterietemperatur             -        1
0x0045    4       Minimale Batterietemperatur (offset -40)      °C       1
0x0046    5       Index minimale Batterietemperatur             -        1
0x0047    6       Max-Min Temperaturdifferenz                   °C       1
0x0048    7       Lade-/Entladestatus (0=Idle,1=Charge,2=Dis)   -        -
0x0049    8       Ladegerät erkannt (0=nein, 1=ja)              -        -
0x004A    9       Last erkannt (0=nein, 1=ja)                   -        -
0x004B    10      Verbleibende Kapazität                        0.1 Ah   0.1
0x004C    11      Ladezyklen                                    -        1
0x004D    12      Balancer-Status (0=aus,1=passiv,2=aktiv)      -        -
0x004E    13      Balancer-Strom (offset -30000)                0.001 A  0.001
0x004F    14      (Alarm-Flags Teil 1)                          -        -
0x0050    15      (Alarm-Flags Teil 2)                          -        -
0x0051    16      (reserviert)                                  -        -
0x0052    17      Lade-MOSFET-Status (0=aus, 1=an)              -        -
0x0053    18      Entlade-MOSFET-Status (0=aus, 1=an)           -        -
0x0054    19      Pre-Charge-MOSFET-Status (0=aus, 1=an)        -        -
0x0055    20      Heizungs-MOSFET-Status (0=aus, 1=an)          -        -
0x0056    21      Lüfter-MOSFET-Status (0=aus, 1=an)            -        -
0x0057    22      Durchschnittliche Zellspannung                mV       1
0x0058    23      Leistung                                      W        1
0x0059    24      Energie                                       Wh       1
0x005A    25      MOSFET-Temperatur (offset -40)                °C       1
0x005B    26      Umgebungstemperatur / Board-Temp (offset -40) °C       1
0x005C    27      Heizungstemperatur (offset -40)               °C       1
0x005D    28      Heizungsstrom                                 A/Bit    1
0x005E    29      Verbleibende Reichweite                       0.1 km   0.1
0x005F    30      Strombegrenzungs-Status (0=aktiv,1=inaktiv)   -        -
0x0060    31      Strombegrenzungswert (offset -30000)          0.1 A    0.1
0x0061–63 32–34   RTC Zeit (Jahr/Monat/Tag als gepackte Bytes)  -        -
0x0064    35      Verbleibende Ladezeit                         0.1 min  0.1
0x0065    36      DI-Name (Byte)                                -        -
0x0066    37      DO-Name (Byte)                                -        -
...
0x006A    41      Backup-Strommessung (offset -30000)           0.1 A    0.1
0x006B    42      Wake-Up-Quelle                                -        -
0x006C    43      R-SOC                                         0.1 %    0.1
0x006D    44      Neue Fehler-Flags 1                           bitmask  -
0x006E    45      Neue Fehler-Flags 2                           bitmask  -
...
0x0075    52      Fehler-Flags (alte Encoding, Byte A0)         bitmask  -
0x0076    53      Fehler-Flags (alte Encoding, Byte A1)         bitmask  -
```

Byte-Offsets in Frame-absoluten Koordinaten: `data[3 + offset*2]` / `data[4 + offset*2]`

**Mapping auf vorhandene ESPHome-Sensoren (0x81 Teil 2 → aktuelle Sensornamen):**

| ESPHome-Sensor | 0xD2 Frame-Bytes | 0x81 Teil2 Register-Offset |
|---|---|---|
| `battery_status` (Idle/Charging/Discharging) | Byte 98 | Offset 7 |
| `capacity_remaining_sensor_` | Bytes 99–100 | Offset 10 |
| `charging_cycles_sensor_` | Bytes 105–106 | Offset 11 |
| `balancing_binary_sensor_` | Bytes 107–108 | Offset 12 |
| `charging_binary_sensor_` | Bytes 109–110 | Offset 17 |
| `discharging_binary_sensor_` | Bytes 111–112 | Offset 18 |
| `mosfet_temperature_sensor_` | Bytes 135–136 | Offset 25 |
| `board_temperature_sensor_` | Bytes 137–138 | Offset 26 |
| `balance_current_sensor_` | Bytes 131–132 | Offset 13 |

---

## Protokolldetails: 0x81 Settings-Daten

Die Settings-Daten werden in **mehreren Lesebefehlen** abgefragt (analog zur App, die
mehrere `_analysisVersion*` Funktionen hat). Für die ESPHome-Komponente ist primär
der erste Block relevant, da er die `Number`-Entities (Schwellwerte) enthält.

### Settings Block 1: Register 0x0100–0x0150 (0x51 = 81 Register)

```
TX: [0x81, 0x03, 0x01, 0x00, 0x00, 0x51, CRC_LO, CRC_HI]
```

Antwort: `[0x81, 0x03, 0xA2, data...(162 Bytes), CRC_LO, CRC_HI]`
Gesamt: **167 Bytes**

```
Register  Offset  Beschreibung                                     Einheit  Faktor
0x0100    0       Nennkapazität                                    0.1 Ah   0.1
0x0101    1       Zell-Referenzspannung                            mV       1
0x0102    2       Anzahl Erfassungsboards                          -        1
0x0103    3       Zellen Board 1                                   -        1
0x0104    4       Zellen Board 2                                   -        1
0x0105    5       Zellen Board 3                                   -        1
0x0106    6       Temperatursensoren Board 1                       -        1
0x0107    7       Temperatursensoren Board 2                       -        1
0x0108    8       Temperatursensoren Board 3                       -        1
0x0109    9       Batterietyp (0=LiFePO4, 1=Li-Ion, 2=LTO)        -        1
0x010A    10      Sleep-Wartezeit                                  s        1
0x010B    11      Warnung: Zellspannung zu hoch                    mV       1
0x010C    12      Alarm: Zellspannung zu hoch                      mV       1
0x010D    13      Warnung: Zellspannung zu niedrig                 mV       1
0x010E    14      Alarm: Zellspannung zu niedrig                   mV       1
0x010F    15      Warnung: Gesamtspannung zu hoch                  0.1 V    0.1
0x0110    16      Alarm: Gesamtspannung zu hoch                    0.1 V    0.1
0x0111    17      Warnung: Gesamtspannung zu niedrig               0.1 V    0.1
0x0112    18      Alarm: Gesamtspannung zu niedrig                 0.1 V    0.1
0x0113    19      Warnung: Ladestrom zu hoch (offset -30000)       0.1 A    0.1
0x0114    20      Alarm: Ladestrom zu hoch (offset -30000)         0.1 A    0.1
0x0115    21      Warnung: Entladestrom zu hoch (offset -30000)    0.1 A    0.1
0x0116    22      Alarm: Entladestrom zu hoch (offset -30000)      0.1 A    0.1
0x0117    23      Warnung: Ladetemperatur zu hoch (offset -40)     °C       1
0x0118    24      Alarm: Ladetemperatur zu hoch (offset -40)       °C       1
0x0119    25      Warnung: Ladetemperatur zu niedrig (offset -40)  °C       1
0x011A    26      Alarm: Ladetemperatur zu niedrig (offset -40)    °C       1
0x011B    27      Warnung: Entladetemperatur zu hoch (offset -40)  °C       1
0x011C    28      Alarm: Entladetemperatur zu hoch (offset -40)    °C       1
0x011D    29      Warnung: Entladetemperatur zu niedrig (offset -40)°C      1
0x011E    30      Alarm: Entladetemperatur zu niedrig (offset -40) °C       1
...
(0x011F–0x0150: weitere Schutzparameter, für MVP nicht zwingend notwendig)
```

### Settings Block 2: Register 0x0151–0x0177 (0x27 = 39 Register)

```
TX: [0x81, 0x03, 0x01, 0x51, 0x00, 0x27, CRC_LO, CRC_HI]
```

Enthält weitere Schutzparameter (SOC-Grenzen, Zellbalancer-Schwellwerte usw.).
Für MVP nicht zwingend nötig.

### Settings Block 3: Register 0x0178–0x01C2 (0x4A = 74 Register) — Version/Firmware

```
TX: [0x81, 0x03, 0x01, 0x78, 0x00, 0x4A, CRC_LO, CRC_HI]
```

Enthält Firmware-Versionsinformationen (Software- und Hardware-Version als ASCII-Strings).

### Balancer-Register (identisch mit 0xD2-Protokoll)

```
TX: [0x81, 0x03, 0x00, 0xCF, 0x00, 0x01, CRC_LO, CRC_HI]
Antwort: [0x81, 0x03, 0x02, val_hi, val_lo, CRC_LO, CRC_HI]
```

---

## Implementierungsschritte

### Schritt 1: YAML-Konfigurationsoption `protocol`

**Datei: `components/daly_bms_ble/__init__.py`**

```python
# Neue Konstante
CONF_PROTOCOL = "protocol"

# In CONFIG_SCHEMA einfügen:
cv.Optional(CONF_PROTOCOL, default="d2"): cv.one_of("d2", "dl", lower=True),

# In to_code():
cg.add(var.set_protocol(config[CONF_PROTOCOL] == "dl"))
```

Alternativ als boolean `use_dl_protocol` statt enum – je nach Geschmack.

**Beispiel-YAML nach der Implementierung:**
```yaml
daly_bms_ble:
  ble_client_id: my_bms
  protocol: dl    # neu: "dl" für DL-WB01 und ähnliche, "d2" (default) wie bisher
  update_interval: 10s
```

### Schritt 2: C++ – Neue Konstanten und Member-Variable

**Datei: `components/daly_bms_ble/daly_bms_ble.cpp`** – Neue Konstanten oben:

```cpp
// --- 0x81 DL-Protokoll Konstanten ---
static const uint8_t  DALY_FRAME_START_DL      = 0x81;

// Echtzeit-Daten: zwei Lesebefehle
static const uint16_t DALY_CMD_DL_RT1_START    = 0x0000;  // Register 0x0000
static const uint16_t DALY_CMD_DL_RT1_COUNT    = 0x0040;  // 64 Register
static const uint16_t DALY_CMD_DL_RT2_START    = 0x0041;  // Register 0x0041
static const uint16_t DALY_CMD_DL_RT2_COUNT    = 0x003E;  // 62 Register

// Antwort-Byte-Count (Byte[2] des Frames)
static const uint8_t  DALY_FRAME_DL_RT1_LEN    = 0x80;    // 128 Bytes = 64 Reg * 2
static const uint8_t  DALY_FRAME_DL_RT2_LEN    = 0x7C;    // 124 Bytes = 62 Reg * 2

// Settings Block 1
static const uint16_t DALY_CMD_DL_SET1_START   = 0x0100;
static const uint16_t DALY_CMD_DL_SET1_COUNT   = 0x0051;  // 81 Register
static const uint8_t  DALY_FRAME_DL_SET1_LEN   = 0xA2;    // 162 Bytes

// Settings Block 2 (optional für MVP)
static const uint16_t DALY_CMD_DL_SET2_START   = 0x0151;
static const uint16_t DALY_CMD_DL_SET2_COUNT   = 0x0027;

// Version/Firmware
static const uint16_t DALY_CMD_DL_VER_START    = 0x0178;
static const uint16_t DALY_CMD_DL_VER_COUNT    = 0x004A;
static const uint8_t  DALY_FRAME_DL_VER_LEN    = 0x94;    // 148 Bytes

// Balancer-Register (identisch mit D2)
// DALY_COMMAND_REQ_BALANCER_SWITCH = 0x00CF  →  bleibt unverändert
```

**Datei: `components/daly_bms_ble/daly_bms_ble.h`** – Neues Member und Setter:

```cpp
// In der public-Sektion:
void set_protocol_dl(bool use_dl) { this->use_dl_protocol_ = use_dl; }

// In der protected-Sektion (neben status_registers_):
bool use_dl_protocol_{false};

// Neue private Decode-Funktionen:
void decode_dl_rt1_data_(const std::vector<uint8_t> &data);
void decode_dl_rt2_data_(const std::vector<uint8_t> &data);
void decode_dl_settings_data_(const std::vector<uint8_t> &data);

// cells_[32] → cells_[48] wegen 0x81-Protokoll (max. 48 Zellen)
struct Cell {
  sensor::Sensor *cell_voltage_sensor_{nullptr};
} cells_[48];  // War 32, jetzt 48
```

### Schritt 3: `build_frame_()` anpassen

**Datei: `daly_bms_ble.cpp`**

```cpp
std::array<uint8_t, 8> DalyBmsBle::build_frame_(uint8_t function, uint16_t address, uint16_t value) const {
  std::array<uint8_t, 8> frame;
  frame[0] = this->use_dl_protocol_ ? 0x81 : 0xD2;  // Einzige Änderung
  frame[1] = function;
  frame[2] = address >> 8;
  frame[3] = address >> 0;
  frame[4] = value >> 8;
  frame[5] = value >> 0;
  auto crc = crc16(frame.data(), 6);
  frame[6] = crc >> 0;
  frame[7] = crc >> 8;
  return frame;
}
```

### Schritt 4: `update()` anpassen

```cpp
void DalyBmsBle::update() {
  if (this->use_dl_protocol_) {
    // 0x81 DL-Protokoll: zwei Echtzeit-Reads + Settings + Balancer
    this->queue_command_(DALY_FUNCTION_READ, DALY_CMD_DL_RT1_START, DALY_CMD_DL_RT1_COUNT);
    this->queue_command_(DALY_FUNCTION_READ, DALY_CMD_DL_RT2_START, DALY_CMD_DL_RT2_COUNT);
    this->queue_command_(DALY_FUNCTION_READ, DALY_CMD_DL_SET1_START, DALY_CMD_DL_SET1_COUNT);
    this->queue_command_(DALY_FUNCTION_READ, DALY_COMMAND_REQ_BALANCER_SWITCH, 1);
  } else {
    // 0xD2-Protokoll: wie bisher
    this->queue_command_(DALY_FUNCTION_READ, DALY_COMMAND_REQ_STATUS_START, this->status_registers_);
    this->queue_command_(DALY_FUNCTION_READ, DALY_COMMAND_REQ_SETTINGS_START, DALY_COMMAND_REQ_SETTINGS_COUNT);
    this->queue_command_(DALY_FUNCTION_READ, DALY_COMMAND_REQ_BALANCER_SWITCH, 1);
  }
  this->send_next_command_();
}
```

### Schritt 5: `on_daly_bms_ble_data()` Dispatcher erweitern

```cpp
void DalyBmsBle::on_daly_bms_ble_data(const std::vector<uint8_t> &data) {
  const uint8_t expected_start = this->use_dl_protocol_ ? 0x81 : 0xD2;

  if (data[0] != expected_start || data.size() > MAX_RESPONSE_SIZE) {
    ESP_LOGW(TAG, "Invalid response ...");
    return;
  }

  // CRC-Check: unverändert
  // ...

  this->advance_command_queue_();

  if (data[1] == DALY_FUNCTION_WRITE) { /* ... */ return; }
  if (data[1] != 0x03) { /* ... */ return; }

  uint8_t frame_len = data[2];  // Byte-Count

  if (this->use_dl_protocol_) {
    switch (frame_len) {
      case DALY_FRAME_DL_RT1_LEN:    this->decode_dl_rt1_data_(data); break;
      case DALY_FRAME_DL_RT2_LEN:    this->decode_dl_rt2_data_(data); break;
      case DALY_FRAME_DL_SET1_LEN:   this->decode_dl_settings_data_(data); break;
      case DALY_FRAME_LEN_BALANCER_SWITCH: this->decode_balancer_switch_data_(data); break;
      default:
        ESP_LOGW(TAG, "Unhandled DL response (len=0x%02X)", frame_len);
    }
  } else {
    // Bestehender D2-Switch: unverändert
    switch (frame_len) { ... }
  }
}
```

**Achtung:** `MAX_RESPONSE_SIZE` muss ggf. erhöht werden.
- Aktuell: 165 Bytes
- 0x81 Settings Block 1: 3 + 162 + 2 = **167 Bytes** → auf mindestens 170 setzen

### Schritt 6: `decode_dl_rt1_data_()` implementieren

Liest Register 0x0000–0x003F (Zellspannungen, Temperaturen, Spannung, Strom, SOC).

```cpp
void DalyBmsBle::decode_dl_rt1_data_(const std::vector<uint8_t> &data) {
  auto get16 = [&](uint8_t reg) -> uint16_t {
    // reg ist 0-basierter Register-Index (0x0000-based)
    size_t i = 3 + reg * 2;
    return (uint16_t(data[i]) << 8) | data[i + 1];
  };

  // Anzahl Zellen und Sensoren zuerst lesen (für Loop-Grenzen)
  uint8_t cells = std::min((uint8_t) get16(0x3C), (uint8_t) 48);
  uint8_t temp_sensors = std::min((uint8_t) get16(0x3D), (uint8_t) 8);

  // Zellspannungen: Register 0x0000 + i, bis zu 48 Zellen
  float min_cell_v = 100.0f, max_cell_v = -100.0f, avg_cell_v = 0.0f;
  uint8_t min_cell = 0, max_cell = 0;
  for (uint8_t i = 0; i < cells; i++) {
    float v = get16(i) * 0.001f;
    avg_cell_v += v;
    if (v > 0 && v < min_cell_v) { min_cell_v = v; min_cell = i + 1; }
    if (v > max_cell_v) { max_cell_v = v; max_cell = i + 1; }
    this->publish_state_(this->cells_[i].cell_voltage_sensor_, v);
  }
  avg_cell_v /= cells;
  this->publish_state_(this->min_cell_voltage_sensor_, min_cell_v);
  this->publish_state_(this->max_cell_voltage_sensor_, max_cell_v);
  this->publish_state_(this->average_cell_voltage_sensor_, avg_cell_v);
  this->publish_state_(this->min_voltage_cell_sensor_, (float) min_cell);
  this->publish_state_(this->max_voltage_cell_sensor_, (float) max_cell);

  // Temperaturen: Register 0x0030 + i (offset -40)
  this->publish_state_(this->temperature_sensors_sensor_, (float) temp_sensors);
  for (uint8_t i = 0; i < temp_sensors; i++) {
    this->publish_state_(this->temperatures_[i].temperature_sensor_,
                         (get16(0x30 + i) - 40) * 1.0f);
  }

  // Gesamtspannung: Register 0x0038
  float total_v = get16(0x38) * 0.1f;
  this->publish_state_(this->total_voltage_sensor_, total_v);

  // Strom: Register 0x0039, Offset -30000, Faktor 0.1
  float current = (get16(0x39) - 30000) * 0.1f;
  this->publish_state_(this->current_sensor_, current);

  // Leistung berechnen (kein eigenes Power-Register in Teil 1)
  float power = total_v * current;
  this->publish_state_(this->power_sensor_, power);
  this->publish_state_(this->charging_power_sensor_, std::max(0.0f, power));
  this->publish_state_(this->discharging_power_sensor_, std::abs(std::min(0.0f, power)));

  // SOC: Register 0x003A
  this->publish_state_(this->state_of_charge_sensor_, get16(0x3A) * 0.1f);

  // Zellanzahl
  this->publish_state_(this->cell_count_sensor_, (float) cells);

  // Max/Min-Zellspannung aus Register (für Logging, Sensoren bereits oben gesetzt)
  // Register 0x003E = Max-Zellspannung, 0x003F = Index, 0x0040 im nächsten Read
}
```

### Schritt 7: `decode_dl_rt2_data_()` implementieren

Liest Register 0x0041–0x007E (Status, Kapazität, MOSFET, Temperaturen, Alarme).

```cpp
void DalyBmsBle::decode_dl_rt2_data_(const std::vector<uint8_t> &data) {
  auto get16 = [&](uint8_t offset) -> uint16_t {
    // offset = 0 entspricht Register 0x0041 (data[3])
    size_t i = 3 + offset * 2;
    return (uint16_t(data[i]) << 8) | data[i + 1];
  };

  // Offset 2: Max-Temp, 3: Max-Temp-Index, 4: Min-Temp, 5: Min-Temp-Index
  // (für ESPHome-Sensoren aktuell nicht einzeln publiziert, nur in D2-Version)

  // Offset 7: Lade-/Entladestatus
  uint16_t status = get16(7);
  this->publish_state_(this->battery_status_text_sensor_,
                       status == 0 ? "Idle" : status == 1 ? "Charging" : "Discharging");

  // Offset 10: Verbleibende Kapazität
  this->publish_state_(this->capacity_remaining_sensor_, get16(10) * 0.1f);

  // Offset 11: Ladezyklen
  this->publish_state_(this->charging_cycles_sensor_, (float) get16(11));

  // Offset 12: Balancer-Status (0=aus, 1=passiv, 2=aktiv)
  this->publish_state_(this->balancing_binary_sensor_, get16(12) != 0);

  // Offset 13: Balancer-Strom (offset -30000, Faktor 0.001)
  this->publish_state_(this->balance_current_sensor_, (get16(13) - 30000) * 0.001f);

  // Offset 17: Lade-MOSFET
  this->publish_state_(this->charging_binary_sensor_, get16(17) == 1);

  // Offset 18: Entlade-MOSFET
  this->publish_state_(this->discharging_binary_sensor_, get16(18) == 1);

  // Offset 25: MOSFET-Temperatur (offset -40)
  this->publish_state_(this->mosfet_temperature_sensor_, (get16(25) - 40) * 1.0f);

  // Offset 26: Board-/Umgebungstemperatur (offset -40)
  this->publish_state_(this->board_temperature_sensor_, (get16(26) - 40) * 1.0f);

  // Delta Zellspannung: fehlt in Teil 2, entweder aus Teil 1 berechnen oder weglassen
  // (Im D2-Protokoll: Byte 115–116)

  // Alarm-Bitmask: Offset 44–47 (neue Fehler-Flags)
  // Mapping auf ERRORS[] noch zu klären – zunächst nur error_bitmask_sensor_ setzen
  uint64_t alarm = ((uint64_t) get16(44) << 48) | ((uint64_t) get16(45) << 32)
                 | ((uint64_t) get16(46) << 16) |  (uint64_t) get16(47);
  this->publish_state_(this->error_bitmask_sensor_, (float) alarm);
  this->publish_state_(this->errors_text_sensor_,
                       this->bitmask_to_string_(ERRORS, ERRORS_SIZE, alarm));
}
```

### Schritt 8: `decode_dl_settings_data_()` implementieren

Analog zur bestehenden `decode_settings_data_()`, aber mit anderen Byte-Offsets.
Die Register-Adressen für Schwellwerte sind in Settings Block 1 (0x0100+).

Settings-Nummern (`register_settings_number`) im YAML bleiben kompatibel, wenn
der Python-Code die Register-Adresse direkt übergibt. Die Adressen unterscheiden
sich zwischen D2 (0x008B = Warnung Zellspannung zu hoch) und 0x81 (0x010B).

**Option A (empfohlen für MVP):** Settings zunächst nur loggen (wie in `decode_settings_data_`
für viele Werte). Number-Entities für 0x81 in einem späteren PR.

**Option B:** Vollständige Implementierung mit angepassten Register-Adressen im YAML,
wobei der Nutzer beim 0x81-Protokoll andere Adressen angeben muss.

### Schritt 9: Sensor-Array-Größe anpassen

In `sensor.py` und `__init__.py`:
- `cells_`: 32 → 48 (0x81 unterstützt bis zu 48 Zellen)
- Die Sensor-Konfiguration im YAML bleibt kompatibel

---

## Offene Fragen / Unsicherheiten

1. **Alarm-Bitmask-Mapping:** Das ERRORS[]-Array ist auf das 0xD2-Protokoll zugeschnitten
   (Register 0x3A–0x3D). Das 0x81-Protokoll hat Fehler-Flags in Register 0x006D–0x006E
   mit anderem Bit-Layout. Entweder separates ERRORS_DL[]-Array oder zunächst nur
   Rohdaten-Sensor ohne Text-Mapping.

2. **Delta-Zellspannung:** Im 0xD2-Protokoll direkt aus Register verfügbar (Byte 115–116).
   Im 0x81-Protokoll nicht explizit – muss aus max/min-Zellspannung berechnet werden.
   Einfache Lösung: `max_cell_voltage - min_cell_voltage` in `decode_dl_rt1_data_()`.

3. **MAX_RESPONSE_SIZE:** Aktuell 165, Settings-Antwort ist 167 Bytes → auf 170 erhöhen.

4. **`set_status_registers()`:** Diese Option ist für 0x81 irrelevant (feste Befehlsfolge).
   Im YAML kann sie ignoriert werden wenn `protocol: dl`. Alternativ: Validierung in `__init__.py`
   die warnt wenn `status_registers` + `protocol: dl` kombiniert werden.

5. **Verifikation mit echtem DL-WB01:** Die Register-Adressen basieren auf der App-Analyse.
   Es empfiehlt sich, mit einem ESP32-BLE-Scanner (z.B. `esp32-ble-scanner.yaml`) einen
   vollständigen Kommunikationsmitschnitt vom DL-WB01 aufzunehmen und die Byte-Offsets
   zu verifizieren, bevor die Decoder-Funktionen finalisiert werden.

---

## Reihenfolge für MVP

1. `__init__.py`: `protocol`-Option hinzufügen
2. `.h`: `use_dl_protocol_`, Setter, neue Decode-Funktionen deklarieren, `cells_[48]`
3. `.cpp`: Neue Konstanten, `build_frame_()`, `update()`, Dispatcher
4. `.cpp`: `decode_dl_rt1_data_()` – alle Sensoren außer Alarme
5. `.cpp`: `decode_dl_rt2_data_()` – Status, MOSFET, Kapazität (ohne Alarm-Text)
6. `.cpp`: `decode_dl_settings_data_()` – zunächst nur Logging (kein Number-Support)
7. Testen mit DL-WB01

Settings-Number-Support für 0x81 kann in einem separaten Folge-PR implementiert werden.
