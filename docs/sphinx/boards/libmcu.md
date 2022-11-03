# libmcu Development Board

## Core

### ESP32

![Pinout](https://user-images.githubusercontent.com/20197999/198325717-b572a618-983e-4156-8259-81021f6312df.png)

| Name                  | Desc.                                               |
| --------------------- | --------------------------------------------------- |
| ESP32-WROOM-32E-N4    | 240MHz dual core, 600 DMIPS, 520KB SRAM, Wi-Fi, BLE |
| Flash                 | 4MB                                                 |
| PSRAM                 | -                                                   |
| Input Voltage         | 2.7 ~ 5.5VDC @ 1.1A                                 |
| Interface             | USB Type-C                                          |
| LED                   | Green power indicator light                         |
| Button                | Reset button, User button                           |
| PMU                   | BQ25180YBGR                                         |
| USB Chip              | CH9102F                                             |
| Antenna               | 2.4GHz PCB antenna                                  |
| Operating temperature | -30&deg; ~ 85&deg;C                                 |
| Net Weight            |                                                     |
| Gross Weight          |                                                     |
| Product Size          | 39mm x 31.75mm                                      |

#### USB Type-C Connector

| Pin  | Func. |
| ---- | ----- |
| GND  |       |
| TX1+ | TDO   |
| TX1- | TCLK  |
| VBUS |       |
| CC1  |       |
| D+   |       |
| D-   |       |
| SBU1 |       |
| VBUS |       |
| RX2- | TDI   |
| RX2+ | TMS   |
| GND  |       |

#### LED
A user green LED : `IO2` low active

#### PMU

#### Operating Temperature

| 파트명              | Min. | Max. | Unit   |
| ------------------- | ---- | ---- | ------ |
| TCR3RM33A           | -40  | 85   | &deg;C |
| BQ25180             | -40  | 125  | &deg;C |
| SN74LV4066ARGYR     | -40  | 85   | &deg;C |
| CP2102N             | -40  | 85   | &deg;C |
| KUSBX-SMT-CS2-B30TR | -30  | 85   | &deg;C |
| ESP32-WROOM-32E     | -40  | 85   | &deg;C |

## Adapter

## Sensor

## Base
