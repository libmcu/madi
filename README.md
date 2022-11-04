![Build Status](https://github.com/libmcu/firmware-project-layout/workflows/build/badge.svg)
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=libmcu_firmware-project-layout&metric=coverage)](https://sonarcloud.io/summary/new_code?id=libmcu_firmware-project-layout)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=libmcu_firmware-project-layout&metric=bugs)](https://sonarcloud.io/summary/new_code?id=libmcu_firmware-project-layout)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?project=libmcu_firmware-project-layout&metric=vulnerabilities)](https://sonarcloud.io/summary/new_code?id=libmcu_firmware-project-layout)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=libmcu_firmware-project-layout&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=libmcu_firmware-project-layout)
[![Duplicated Lines (%)](https://sonarcloud.io/api/project_badges/measure?project=libmcu_firmware-project-layout&metric=duplicated_lines_density)](https://sonarcloud.io/summary/new_code?id=libmcu_firmware-project-layout)

## Project Layout

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
├── stubs
└── tests
```

| Directory | Description                                                                    |
| --------- | -----------                                                                    |
| common    | Reusable components invoked by various systems. non-application-specific code  |
| docs      | Project documentation, possibly auto-generated                                 |
| drivers   | Device drivers                                                                 |
| external  | External dependencies including all the third party libraries                  |
| include   | Public headers                                                                 |
| ports     | Adapter codes for a specific hardware or platform                              |
| projects  | Build rules                                                                    |
| src       | The application-specific code, including private headers                       |
| stubs     | Non-functional glue code for build to be done without error                    |
| tests     | Automated tests                                                                |
| tools     | Tools and utilities                                                            |

## Build
In case you go with [Zephyr](https://zephyrproject.org/), use `west` which is
Zephyr's meta tool. Please refer to [Zephyr's Getting Started
Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html).
Zephyr kernel configurations and devicetree overlays for a board can be found
under [docs/boards](docs/boards).

Please check out our getting started guide under [docs/boards](docs/boards) for
other RTOSs or bear metal firmwares.
