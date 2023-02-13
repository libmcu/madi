![Build Status](https://github.com/libmcu/firmware-project-layout/workflows/build/badge.svg)
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=libmcu_firmware-project-layout&metric=coverage)](https://sonarcloud.io/summary/new_code?id=libmcu_firmware-project-layout)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=libmcu_firmware-project-layout&metric=bugs)](https://sonarcloud.io/summary/new_code?id=libmcu_firmware-project-layout)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?project=libmcu_firmware-project-layout&metric=vulnerabilities)](https://sonarcloud.io/summary/new_code?id=libmcu_firmware-project-layout)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=libmcu_firmware-project-layout&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=libmcu_firmware-project-layout)
[![Duplicated Lines (%)](https://sonarcloud.io/api/project_badges/measure?project=libmcu_firmware-project-layout&metric=duplicated_lines_density)](https://sonarcloud.io/summary/new_code?id=libmcu_firmware-project-layout)

This project is my attempt at developing platform independent and hardware
agnostic firmware, providing a standard project layout.

## Getting Started
Please refer to [Quickstart guide](https://docs.libmcu.org/quickstart/).

## Project Layout

```shell
.
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

## Supported Boards
Supported boards can be found [here](https://docs.libmcu.org/boards/).

## Contributing
Contributions are welcome. Feel free to submit issues, enhancement requests and
Pull Request.
