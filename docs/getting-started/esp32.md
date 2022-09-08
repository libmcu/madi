# ESP32

In case of vscode, install [ESP-IDF extension](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension).

or build with CMake:

```shell
$ cmake -Bbuild -GNinja -DESP_PLATFORM=1 -DBOARD=esp32
$ cmake --build build
```

or Make:

```shell
$ make BOARD=esp32
```
