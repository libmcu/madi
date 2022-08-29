# Firmware Project Layout

## Getting Started
### Directory Structure

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

### Build your project

```shell
$ DEVICE=esp32 make
```

#### A33G
#### ESP32
#### nRF52
#### STM32

## Supported Microcontrollers

| Manufacturer | Device Family | Core       | Note |
| ------------ | ------------- | ---------- | ---- |
| ABOV         | A33G5         | Cortex-M3  |      |
| Espressif    | ESP32         | Xtensa LX6 |      |
| Nordic       | nRF52         | Cortex-M4  |      |
| ST           | STM32F1       | Cortex-M3  |      |
| ST           | STM32F4       | Cortex-M4  |      |
