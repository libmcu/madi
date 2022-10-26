# nRF52

## Common prerequisite
Download the [nRF5 SDK](https://www.nordicsemi.com/Products/Development-software/nRF5-SDK/Download#infotabs).

In case of Mesh, Download the [nRF5 SDK for Mesh](https://www.nordicsemi.com/Products/Development-software/nRF5-SDK-for-Mesh/Download?lang=en#infotabs).


## Redbear Nano v2
### Build

#### Make
```shell
$ make BOARD=nrf52_blenano2
```

#### CMake
```shell
$ cmake -S . -B build -DBOARD=nrf52_blenano2 -DCMAKE_TOOLCHAIN_FILE=projects/arm-none-eabi-gcc.cmake
$ cmake --build build
```

## Zephyr

```
# prj.conf

CONFIG_BOOTLOADER_MCUBOOT=y
CONFIG_MCUBOOT_SIGNATURE_KEY_FILE="bootloader/mcuboot/root-rsa-2048.pem"
CONFIG_MCUBOOT_ENCRYPTION_KEY_FILE="bootloader/mcuboot/enc-rsa2048-priv.pem"

CONFIG_BT=y
CONFIG_BT_OBSERVER=y
CONFIG_BT_EXT_ADV=y

CONFIG_BT_PERIPHERAL=y
CONFIG_BT_DEVICE_NAME="fpl"
CONFIG_BT_DEVICE_APPEARANCE=833
CONFIG_BT_MAX_CONN=1
CONFIG_BT_MAX_PAIRED=1
```

```
# nrf52_blenano2_flash.overlay

/delete-node/ &storage_partition;

&boot_partition {
	label = "mcuboot";
	reg = <0x00000000 0x10000>;
};
&slot0_partition {
	 label = "image-0";
	 reg = <0x00010000 0x34000>;
};
&slot1_partition {
	 label = "image-1";
	 reg = <0x00044000 0x34000>;
};
&scratch_partition {
	   label = "image-scratch";
	   reg = <0x00078000 0x8000>;
};
```
