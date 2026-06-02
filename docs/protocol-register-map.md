# Daly BMS BLE – Register Map: D2 vs. p81

Both protocols expose the same physical data points but encode them at
**different register addresses**. The reason is straightforward: the p81
protocol reserves 48 cell slots (0x0000–0x002F), while D2 reserves only
32 (0x0000–0x001F). Every register **after** the cell block is shifted by
the same constant amount.

---

## Real-time Block 1 – Cells, Temperatures, Voltage, Current, SOC

D2 reads this block in a single frame (62 or 80 registers from 0x0000).
p81 reads the same block as "RT1" (64 registers from 0x0000).

| Data point | D2 register | p81 register | Offset | Unit / factor |
|---|---|---|---|---|
| Cell voltage 1 | 0x0000 | 0x0000 | — | raw · 0.001 → V |
| Cell voltage 2 | 0x0001 | 0x0001 | — | raw · 0.001 → V |
| … | … | … | — | |
| Cell voltage 32 | 0x001F | 0x001F | — | raw · 0.001 → V |
| Cell voltage 33 | — | 0x0020 | — | raw · 0.001 → V |
| … | — | … | — | |
| Cell voltage 48 | — | 0x002F | — | raw · 0.001 → V |
| Temperature 1 | **0x0020** | **0x0030** | **+0x10** | raw − 40 → °C |
| Temperature 2 | 0x0021 | 0x0031 | +0x10 | raw − 40 → °C |
| Temperature 3 | 0x0022 | 0x0032 | +0x10 | raw − 40 → °C |
| Temperature 4 | 0x0023 | 0x0033 | +0x10 | raw − 40 → °C |
| Temperature 5 | 0x0024 | 0x0034 | +0x10 | raw − 40 → °C |
| Temperature 6 | 0x0025 | 0x0035 | +0x10 | raw − 40 → °C |
| Temperature 7 | 0x0026 | 0x0036 | +0x10 | raw − 40 → °C |
| Temperature 8 | 0x0027 | 0x0037 | +0x10 | raw − 40 → °C |
| Total voltage | **0x0028** | **0x0038** | **+0x10** | raw · 0.1 → V |
| Current | **0x0029** | **0x0039** | **+0x10** | (raw − 30000) · 0.1 → A |
| SOC | **0x002A** | **0x003A** | **+0x10** | raw · 0.1 → % |
| Max. cell voltage (value) | 0x002B | 0x003E | +0x13 | raw · 0.001 → V |
| Max. cell voltage (index) | — | 0x003F | — | |
| Heartbeat counter | — | 0x003B | — | |
| Cell count | 0x0031 | 0x003C | +0x0B | |
| Temperature sensor count | 0x0032 | 0x003D | +0x0B | |

> **Why +0x10?** p81 reserves 48 cell slots (0x0000–0x002F), D2 only 32
> (0x0000–0x001F). The 16 extra registers shift the entire subsequent block
> by exactly +0x10. Temperatures, voltage, current, and SOC all carry this
> constant offset. After SOC the offset diverges because p81 inserts
> additional fields (heartbeat counter, max-index) that D2 does not have.

---

## Real-time Block 2 – Status, Capacity, MOSFETs, Temperatures

D2 reads this block in the same 80-register frame (continuing from 0x002B).
p81 reads it as a separate "RT2" frame (62 registers from 0x0041).

| Data point | D2 register | p81 register | Offset | Unit / factor |
|---|---|---|---|---|
| Max. battery temperature (value) | 0x002D | 0x0043 | +0x16 | raw − 40 → °C |
| Max. battery temperature (index) | — | 0x0044 | — | |
| Min. battery temperature (value) | 0x002E | 0x0045 | +0x17 | raw − 40 → °C |
| Min. battery temperature (index) | — | 0x0046 | — | |
| Charge / discharge status | **0x002F** | **0x0048** | **+0x19** | 0=Idle 1=Chg 2=Dis |
| Remaining capacity | **0x0030** | **0x004B** | **+0x1B** | raw · 0.1 → Ah |
| Cell count | 0x0031 | — | — | (in RT1 for p81) |
| Charging cycles | **0x0033** | **0x004C** | **+0x19** | |
| Balancer state | **0x0034** | **0x004D** | **+0x19** | 0=off 1=passive 2=active |
| Balance current | **0x0040** | **0x004E** | **+0x0E** | (raw − 30000) · 0.001 → A |
| Charging MOSFET | **0x0035** | **0x0052** | **+0x1D** | bool |
| Discharging MOSFET | **0x0036** | **0x0053** | **+0x1D** | bool |
| Pre-charge MOSFET | — | 0x0054 | — | bool |
| Heating MOSFET | — | 0x0055 | — | bool |
| Average cell voltage | 0x0037 | 0x0057 | +0x20 | raw · 0.001 → V |
| Power | 0x0039 | 0x0058 | +0x1F | W |
| Energy counter | — | 0x0059 | — | Wh |
| MOSFET temperature | **0x0042** | **0x005A** | **+0x18** | raw − 40 → °C |
| Board temperature | **0x0043** | **0x005B** | **+0x18** | raw − 40 → °C |
| Delta cell voltage | 0x0038 | — | — | raw · 0.001 (p81: calculated) |
| Alarm flags (4 registers) | 0x003A–0x003D | 0x006D–0x006E | — | bitmask |
| Cell balance bitmask 1–16 | 0x003E | 0x004F | +0x11 | |
| Cell balance bitmask 17–32 | 0x003F | 0x0050 | +0x11 | |
| Cell balance bitmask 33–48 | — | 0x0051 | — | |

> The offset in block 2 is **not constant** because p81 inserts several new
> fields (pre-charge/heating MOSFETs, energy counter, max/min temperature
> indices) that do not exist in D2. The relative order of the shared fields
> is nonetheless the same in both protocols.

---

## Settings Registers

| Data point | D2 register | p81 register | Offset |
|---|---|---|---|
| Rated capacity | 0x0080 | 0x0100 | **+0x0080** |
| Cell reference voltage | 0x0081 | 0x0101 | +0x0080 |
| Acquisition board count | 0x0082 | 0x0102 | +0x0080 |
| Board 1 cell count | 0x0083 | 0x0103 | +0x0080 |
| Board 2 cell count | 0x0084 | 0x0104 | +0x0080 |
| Board 3 cell count | 0x0085 | 0x0105 | +0x0080 |
| Board 1 temperature sensor count | 0x0086 | 0x0106 | +0x0080 |
| Board 2 temperature sensor count | 0x0087 | 0x0107 | +0x0080 |
| Board 3 temperature sensor count | 0x0088 | 0x0108 | +0x0080 |
| Battery type | 0x0089 | 0x0109 | +0x0080 |
| Sleep wait time | 0x008A | 0x010A | +0x0080 |
| Warning: cell overvoltage | 0x008B | 0x010B | +0x0080 |
| Alarm: cell overvoltage | 0x008C | 0x010C | +0x0080 |
| Warning: cell undervoltage | 0x008D | 0x010D | +0x0080 |
| Alarm: cell undervoltage | 0x008E | 0x010E | +0x0080 |
| Warning: total overvoltage | 0x008F | 0x010F | +0x0080 |
| Alarm: total overvoltage | 0x0090 | 0x0110 | +0x0080 |
| Warning: total undervoltage | 0x0091 | 0x0111 | +0x0080 |
| Alarm: total undervoltage | 0x0092 | 0x0112 | +0x0080 |
| Warning: charge overcurrent | 0x0093 | 0x0113 | +0x0080 |
| Alarm: charge overcurrent | 0x0094 | 0x0114 | +0x0080 |
| Warning: discharge overcurrent | 0x0095 | 0x0115 | +0x0080 |
| Alarm: discharge overcurrent | 0x0096 | 0x0116 | +0x0080 |
| Warning: charge overtemperature | 0x0097 | 0x0117 | +0x0080 |
| Alarm: charge overtemperature | 0x0098 | 0x0118 | +0x0080 |
| Warning: charge undertemperature | 0x0099 | 0x0119 | +0x0080 |
| Alarm: charge undertemperature | 0x009A | 0x011A | +0x0080 |
| Warning: discharge overtemperature | 0x009B | 0x011B | +0x0080 |
| Alarm: discharge overtemperature | 0x009C | 0x011C | +0x0080 |
| Warning: discharge undertemperature | 0x009D | 0x011D | +0x0080 |
| Alarm: discharge undertemperature | 0x009E | 0x011E | +0x0080 |
| Balancer activation voltage | 0x00A3 | 0x0123 | +0x0080 |
| Balancer voltage difference | 0x00A4 | 0x0124 | +0x0080 |
| Charging MOS switch | 0x00A5 | — | — |
| Discharging MOS switch | 0x00A6 | — | — |
| SOC setting | 0x00A7 | 0x0127 | +0x0080 |
| MOS overtemperature alarm | 0x00A8 | 0x0128 | +0x0080 |

> **The settings offset is exactly +0x0080 (128) throughout** — no
> exceptions among the shared parameters. The settings blocks are
> structurally identical; only the base address differs.

---

## Other Commands

| Command | D2 address | p81 address | Register count |
|---|---|---|---|
| Balancer switch (read / write) | 0x00CF | 0x00CF | 1 |
| Firmware / hardware version | 0x00A9 | 0x0178 | 32 (D2) / 74 (p81) |
| Password (D2 only) | 0x00C9 | — | 3 |

> Register **0x00CF is identical in both protocols** — the only register
> outside the cell block that carries no offset.

---

## Summary

| Section | Offset D2 → p81 | Reason |
|---|---|---|
| Cells (0x0000–) | — | same base address, p81 has more slots |
| Temperatures, voltage, current, SOC | **+0x10** | 16 extra cell slots in p81 |
| Status, capacity, MOSFETs | variable (+0x0E–+0x20) | p81 inserts additional fields |
| Settings | **+0x0080** | different base address |
| Balancer switch | 0 | identical |
| Version | different | different address and frame size |
