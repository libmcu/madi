# Firmware Project Layout

## Directory Structure

```shell
.
├── common
├── docs
├── drivers
├── external
│   ├── cbor
│   ├── libmcu
│   └── tinyusb
├── include
├── ports
│   ├── esp-idf
│   ├── freertos
│   ├── nrf52
│   ├── stm32
│   └── zephyr
├── projects
├── src
└── tests
```

| Directory | Description                                                   |
| --------- | -----------                                                   |
| common    |                                                               |
| docs      | Project documentation, possibly auto-generated                |
| drivers   |                                                               |
| external  | External dependencies including all the third party libraries |
| include   | Public headers                                                |
| ports     |                                                               |
| projects  |                                                               |
| src       |                                                               |
| tests     | Automated tests                                               |
| tools     | Tools and utilities                                           |

## Getting Started
In case you go with [Zephyr](https://zephyrproject.org/), use `west` which is Zephyr's meta tool.

Instructions below are for other RTOSs or bear metal firmwares.

### A33G
### AT32F403A
### ESP32
In case of vscode, install [ESP-IDF extension](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension).

or build with CMake:

```shell
$ cmake -Bbuild -GNinja -DESP_PLATFORM=1 -DDEVICE=esp32
$ cmake --build build
```

or with Make:

```shell
$ make DEVICE=esp32
```

### nRF52
#### Prerequisite

Download the [nRF5 SDK](https://www.nordicsemi.com/Products/Development-software/nRF5-SDK/Download#infotabs).

In case of Mesh, Download the [nRF5 SDK for Mesh](https://www.nordicsemi.com/Products/Development-software/nRF5-SDK-for-Mesh/Download?lang=en#infotabs).

#### Build

```shell
$ make DEVICE=nrf52
```

### RP2040

### STM32
#### Prerequisite

Download the proper STM32Cube firmware package for your device:
- https://github.com/STMicroelectronics/STM32CubeF0.git
- https://github.com/STMicroelectronics/STM32CubeF1.git
- https://github.com/STMicroelectronics/STM32CubeF2.git
- https://github.com/STMicroelectronics/STM32CubeF3.git
- https://github.com/STMicroelectronics/STM32CubeF4.git
- https://github.com/STMicroelectronics/STM32CubeF7.git
- https://github.com/STMicroelectronics/STM32CubeH7.git
- https://github.com/STMicroelectronics/STM32CubeL0.git
- https://github.com/STMicroelectronics/STM32CubeL1.git
- https://github.com/STMicroelectronics/STM32CubeL4.git
- https://github.com/STMicroelectronics/STM32CubeL5.git
- https://github.com/STMicroelectronics/STM32CubeG0.git
- https://github.com/STMicroelectronics/STM32CubeG4.git
- https://github.com/STMicroelectronics/STM32CubeWL.git
- https://github.com/STMicroelectronics/STM32CubeWB.git
- https://github.com/STMicroelectronics/STM32CubeU5.git
- https://github.com/STMicroelectronics/STM32CubeMP1.git

```shell
$ git clone https://github.com/STMicroelectronics/STM32CubeF1.git external/STM32CubeF1
```

or add it as a submodule to your project:

```shell
$ git submodule add https://github.com/STMicroelectronics/STM32CubeF1.git external/STM32CubeF1
```

#### Build

```shell
$ make DEVICE=stm32f1
```

## Supported Boards

| Device  | Board              | Core       | Device Family | Manufacturer |
| ------- | ------------------ | ---------- | ------------- | ------------ |
| esp32   | ESP32-DevKitC      | Xtensa LX6 | ESP32         | Espressif    |
| nrf52   | Redbear Nano v2    | Cortex-M4F | nRF52         | Nordic       |
| rp2040  | Raspberry Pi Pico  | Cortex-M0+ | RP2040        | Raspberry Pi |
| stm32f1 | STM32F103 BluePill | Cortex-M3  | STM32F1       | ST           |
| stm32f4 | STM32F429I-DISC1   | Cortex-M4F | STM32F4       | ST           |
| stm32f4 | myCortex-STM32F4   | Cortex-M4F | STM32F4       | ST           |
|         | StarterKit A33G526 | Cortex-M3  | A33G5         | ABOV         |
|         | WeAct BlackPill    | Cortex-M4F | AT32F403A     | Artery       |
