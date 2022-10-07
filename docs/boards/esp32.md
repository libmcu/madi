# ESP32

In case of vscode, install [ESP-IDF extension](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension).

or build with `idf.py` command line tool:

```shell
$ idf.py build
```

or build with CMake:

```shell
$ cmake -Bbuild -GNinja -DESP_PLATFORM=1 -DBOARD=esp32
$ cmake --build build
```

or Make:

```shell
$ make BOARD=esp32
```

## Zephyr

```
# prj.conf

CONFIG_NETWORKING=y
CONFIG_NET_L2_ETHERNET=y
CONFIG_NET_LOG=y

CONFIG_WIFI=y
CONFIG_WIFI_ESP32=y
CONFIG_WIFI_LOG_LEVEL_DBG=y

CONFIG_NET_IPV4=y
CONFIG_NET_IPV6=n
CONFIG_NET_DHCPV4=y

CONFIG_ESP32_WIFI_STA_AUTO_DHCPV4=n
CONFIG_ESP32_WIFI_STA_RECONNECT=n

CONFIG_NET_SHELL=y
```

```
# app.overlay

&wifi {
	status = "okay";
};
```
