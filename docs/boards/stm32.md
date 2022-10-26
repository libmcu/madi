# STM32

## Common prerequisite

Download the proper STM32Cube firmware package for your board:

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

## STM32F103 BluePill
### Build

#### Make
```shell
$ make BOARD=stm32_min_dev_blue
```

#### CMake
```shell
$ cmake -S . -B build -DBOARD=stm32_min_dev_blue -DCMAKE_TOOLCHAIN_FILE=projects/arm-none-eabi-gcc.cmake
$ cmake --build build
```
