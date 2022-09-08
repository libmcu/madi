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
### Build
In case you go with [Zephyr](https://zephyrproject.org/), use `west` which is
Zephyr's meta tool. Please refer to [Zephyr's Getting Started
Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html).

For other RTOSs or bear metal firmwares, please check out our getting started
guide under [docs/getting-started](docs/getting-started).

## Supported Boards

| Board                                                | Desc.              | Core       | Device Family | Manufacturer | Note        |
| ---------------------------------------------------- | ------------------ | ---------- | ------------- | ------------ | ----------- |
| a33g526_starter                                      | StarterKit A33G526 | Cortex-M3  | A33G5         | ABOV         | In progress |
| blackpill_at32f403a                                  | WeAct BlackPill    | Cortex-M4F | AT32F403A     | Artery       | In progress |
| [esp32](docs/getting-started/esp32.md)               | ESP32-DevKitC      | Xtensa LX6 | ESP32         | Espressif    |             |
| [nrf52_blenano2](docs/getting-started/nrf52.md)      | Redbear Nano v2    | Cortex-M4F | nRF52         | Nordic       | In progress |
| [rpi_pico](docs/getting-started/rpi.md)              | Raspberry Pi Pico  | Cortex-M0+ | RP2040        | Raspberry Pi | In progress |
| [stm32_min_dev_blue](docs/getting-started/stm32.md)  | STM32F103 BluePill | Cortex-M3  | STM32F1       | ST           |             |
| [stm32f429i_disc1](docs/getting-started/stm32.md)    | STM32F429I-DISC1   | Cortex-M4F | STM32F4       | ST           | In progress |
| [stm32f407v_mycortex](docs/getting-started/stm32.md) | myCortex-STM32F4   | Cortex-M4F | STM32F4       | ST           | In progress |
