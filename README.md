# Firmware Project Layout

## Directory Structure

```shell
.
├── common
├── docs
├── drivers
├── external
│   ├── nRF5_Mesh_SDK
│   └── tinyusb
├── include
├── ports
│   ├── esp-idf
│   ├── freertos
│   ├── nrf52
│   ├── nrf9160
│   └── stm32
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

### A33G
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

## Supported Microcontrollers

| Manufacturer | Device Family | Core       | Note |
| ------------ | ------------- | ---------- | ---- |
| ABOV         | A33G5         | Cortex-M3  |      |
| Espressif    | ESP32         | Xtensa LX6 |      |
| Nordic       | nRF52         | Cortex-M4  |      |
| ST           | STM32F1       | Cortex-M3  |      |
| ST           | STM32F4       | Cortex-M4  |      |
