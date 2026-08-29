# esp32-wifi6-twt-micropython
Experimental solution for using TWT (Target Wait Time) mechanism from WiFi 6 and not implemented yet ESP32 feature in MicroPython. It's saves power consumption for IoT devices based on ESP32 with WiFi 6.

This solution could save a lot of capacity for you IoT battery based IoT device.
For example, this extension made sleeping WiFi and CPU stack, so you can bring 200 uA power consumption for long time and only Keep-Alive peeks with ~150mA for miliseconds occurs.
It means, that you no need to keep WiFi connection in real-time, becouse TWT on your access point take care about it.

Warninig!
You need to test and run it for WiFi6 router, with support of TWT feature - spec. `Wi-Fi 6 network (2.4GHz 802.11ax compatible router)`.



## ESP32-C6 TWT MicroPython Module Compilation Guide

This guide explains how to compile the custom `esp32_twt` C module into the MicroPython firmware for the **ESP32-C6** using a Linux terminal.

---

### Prerequisites

Before starting, ensure your Linux system has the required build tools installed:

```bash
sudo apt update
sudo apt install build-essential libffi-dev git pkg-config python3-pip python3-virtualenv
```

---

### Step-by-Step Compilation

### Step 1: Clone MicroPython Repository
Clone the official MicroPython repository and fetch its essential submodules:

```bash
git clone --recursive https://github.com/micropython/micropython
cd micropython
```

#### Step 2: Build the MicroPython Cross-Compiler
Navigate to the `mpy-cross` directory and compile the cross-compiler tool:

```bash
make -C mpy-cross
```

#### Step 3: Set Up ESP-IDF (Espressif IoT Development Framework)
MicroPython requires a specific version of ESP-IDF. Move to the ESP32 port directory to install and activate it:

```bash
cd ports/esp32
# Install the matching ESP-IDF version
make idf_version
```
*Note: Follow the terminal prompts provided by the command above to source the IDF environment variables (usually by running `source ../../esp-idf/export.sh`).*

#### Step 4: Prepare Your Module Directory
Ensure your custom module files are organized in a dedicated directory outside or inside the repository (e.g., `~/esp32_twt`):

```text
~/esp32_twt/
├── esp32_twt.c
├── micropython.cmake
└── micropython.mk
```

#### Step 5: Compile the Firmware with the Module
Compile the firmware specifically for the **ESP32-C6** board (`ESP32_GENERIC_C6`) and inject your custom C module using the `USER_C_MODULES` flag:

```bash
make BOARD=ESP32_GENERIC_C6 USER_C_MODULES=~/esp32_twt
```

---

### Flashing the Firmware

Once the compilation succeeds, connect your ESP32-C6 development board to your Linux PC via USB and flash the new firmware:

1. **Erase the existing flash memory:**
   ```bash
   make BOARD=ESP32_GENERIC_C6 erase
   ```

2. **Deploy the new build:**
   ```bash
   make BOARD=ESP32_GENERIC_C6 deploy
   ```

---

### Verification
After flashing, open your favorite serial terminal (e.g., `screen`, `picocom`, or `Thonny`) at **115200 baud**, enter the REPL interface, and verify the installation:

```python
>>> import esp32_twt
>>> dir(esp32_twt)
['__name__', 'setup']
```
If you see the `setup` function listed, your custom TWT compilation was successful!
