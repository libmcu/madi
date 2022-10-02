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
└── tests
```

| Directory | Description                                                                    |
| --------- | -----------                                                                    |
| common    | Reusable components invoked by various systems, like middleware and subsystem  |
| docs      | Project documentation, possibly auto-generated                                 |
| drivers   | Device drivers                                                                 |
| external  | External dependencies including all the third party libraries                  |
| include   | Public headers                                                                 |
| ports     | Adapter codes for a specific hardware or platform                              |
| projects  | Build rules                                                                    |
| src       | Main sources representing the business logic, including private headers        |
| tests     | Automated tests                                                                |
| tools     | Tools and utilities                                                            |

## Build
In case you go with [Zephyr](https://zephyrproject.org/), use `west` which is
Zephyr's meta tool. Please refer to [Zephyr's Getting Started
Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html).

Please check out our getting started guide under [docs/boards](docs/boards) for
other RTOSs or bear metal firmwares.
