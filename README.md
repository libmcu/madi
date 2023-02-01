# MADI

![Build Status](https://github.com/libmcu/board-sdk/workflows/build/badge.svg)

MADI is a firmware development framework for platform-independent and
hardware-agnostic application development.

## Getting Started
Please refer to [the official documentation](https://docs.libmcu.org/quickstart/).

## Project Layout

```shell
.
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
| external  | External dependencies including all the third party libraries |
| include   | Public headers                                                |
| ports     | Adapter codes for a specific hardware or platform             |
| projects  | Build rules                                                   |
| src       | The application-specific code, including private headers      |
| tests     | Automated tests                                               |
| tools     | Tools and utilities                                           |

## Supported Boards
Supported boards can be found [here](https://docs.libmcu.org/boards/).

## Contributing
Contributions are welcome. Feel free to submit issues, enhancement requests and
Pull Request.
