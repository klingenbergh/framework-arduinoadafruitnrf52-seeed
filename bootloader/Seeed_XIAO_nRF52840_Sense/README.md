# Custom Bootloader for XIAO nRF52840 BLE Sense

This folder contains custom-compiled bootloaders based on the [Adafruit nRF52 Bootloader](https://github.com/adafruit/Adafruit_nRF52_Bootloader). The changes made to the original bootloader are detailed below.

The newly generated files in this folder are:

- `update-Seeed_XIAO_nRF52840_Sense_bootloader-0.9.2_nosd.uf2`: 
  - This can be flashed directly via UF2 but **will not take effect with the alterations** unless the `.hex` file in the same folder is flashed via JTAG or similar methods.
  
- `Seeed_XIAO_nRF52840_Sense_bootloader-0.9.2_s140_7.3.0.hex`: 
  - This file must be flashed using a JTAG or SWD programmer to apply the modifications.

- `Seeed_XIAO_nRF52840_Sense_bootloader-0.9.2_s140_7.3.0.zip`: 
  - A compressed package containing the bootloader and SoftDevice, useful for distribution.

The folder also contains the currently set bootloaders referenced in the `platform.txt` file.


---

## Key Modifications

### 1. Enabled NFC Support
- Added the following line to the `src/boards/xiao_nrf52840_ble_sense/board.mk` file:
  ```make
  USE_NFCT = yes
  ```

### 2. Updated `nrfx_config.h`
The `src/nrfx_config.h` file was modified to include and configure necessary drivers for NFC and other peripheral support. The final configuration is:

```c
#ifndef NRFX_CONFIG_H__
#define NRFX_CONFIG_H__

// Power
#define NRFX_POWER_ENABLED  1
#define NRFX_POWER_DEFAULT_CONFIG_IRQ_PRIORITY  7

#define NRFX_NFCT_ENABLED 1
#define NRFX_NFCT_DEFAULT_CONFIG_IRQ_PRIORITY 7
#define NRFX_NFCT_CONFIG_TIMER_INSTANCE_ID 4

#define NRFX_TIMER_ENABLED 1
#define NRFX_TIMER4_ENABLED 1

#define NRFX_CLOCK_ENABLED  0
#define NRFX_NVMC_ENABLED   1

#define NRFX_PRS_ENABLED    0

// PWM
#define NRFX_PWM_ENABLED    0
#define NRFX_PWM0_ENABLED   0
#define NRFX_PWM1_ENABLED   0
#define NRFX_PWM2_ENABLED   0
#define NRFX_PWM3_ENABLED   0

// UART
#ifdef NRF52832_XXAA
#define NRFX_UART_ENABLED   1
#define NRFX_UART0_ENABLED  1
#define NRFX_UART_DEFAULT_CONFIG_IRQ_PRIORITY 7
#define NRFX_UART_DEFAULT_CONFIG_HWFC NRF_UART_HWFC_DISABLED
#define NRFX_UART_DEFAULT_CONFIG_PARITY NRF_UART_PARITY_EXCLUDED
#define NRFX_UART_DEFAULT_CONFIG_BAUDRATE NRF_UART_BAUDRATE_115200
#endif

#endif
```

### 3. Modified the Makefile
The `Makefile` was updated to:

Removed the section:

```make
# Skip defining CONFIG_NFCT_PINS_AS_GPIOS if the device uses the NFCT.
ifneq ($(USE_NFCT),yes)
  CFLAGS += -DCONFIG_NFCT_PINS_AS_GPIOS
endif
```

---

## Building the Bootloader

1. Clone this repository and initialize submodules:
   ```bash
   git clone https://github.com/adafruit/Adafruit_nRF52_Bootloader
   cd Adafruit_nRF52_Bootloader
   git submodule update --init --recursive
   ```

2. Navigate to the folder containing the `Makefile`.

3. Build the bootloader using the `make` command:
   ```bash
   make BOARD=xiao_nrf52840_ble_sense
   ```

4. The output files (e.g., `.hex`, `.uf2`) will be located in the `_build` directory.

---

## Flashing the Bootloader

### Using `nrfjprog`
1. Erase the chip:
   ```bash
   nrfjprog --eraseall -f nrf52
   ```

2. Flash the bootloader:
   ```bash
   nrfjprog --program _build/xiao_nrf52840_ble_sense_bootloader.hex --verify -f nrf52 --reset
   ```

---

## Key Features
- Support for NFC communication with `NRFX_NFCT` driver enabled.
- Custom low-power and timer configurations tailored for the XIAO nRF52840 BLE Sense board.
- Full compatibility with the Adafruit nRF52 Bootloader structure.

---

## Notes
- Ensure you have the [Adafruit_nRF52_Bootloader](https://github.com/adafruit/Adafruit_nRF52_Bootloader) repository cloned if you need to modify or rebuild the bootloader.
- This bootloader is tailored specifically for the **XIAO nRF52840 BLE Sense** and may not work on other boards without modification.

---