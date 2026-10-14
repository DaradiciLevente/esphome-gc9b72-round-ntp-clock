# ESPHome Round NTP Clock with GC9B72 Display & ESP32-C3

An analog-style NTP clock built with **ESPHome**, running on an **ESP32-C3** board (SuperMini / DevKit) with a round **GC9B72** (360x360) TFT display driven by the `Arduino_GFX` library.

Since the **GC9B72** display controller is currently undocumented and not natively supported in ESPHome, this repository provides a full custom implementation using external C++ code.

---

## 🌟 Features

* **Undocumented Display Support**: Custom integration of the GC9B72 display controller into ESPHome.
* **High Performance**: Direct frame updates with smooth 1-second ticks using C++ class integration.
* **Aviation-Style Dial**: Analog clock face featuring hour, minute, and second hands, numbers, and minute ticks.
* **Partial Area Clearing**: Optimized redraw logic clearing only hand sectors and restoring intersecting dial elements to prevent screen flickering.
* **Dynamic Timezones**: Global timezone selector drop-down directly in Home Assistant (UTC-12 to UTC+14) with DST support.
* **PWM Backlight Control**: Adjustable screen brightness exposed as a standard light entity in Home Assistant.
* **NVS Persistence**: Preserves last selected timezone and brightness settings across reboots.

---

## 🛠 Hardware Required

* **Microcontroller**: ESP32-C3 SuperMini (or ESP32-C3 DevKitM-1)
* **Display**: GC9B72 Round TFT Display (360x360 resolution, SPI interface)
* **Connectivity**: Wi-Fi (2.4 GHz)

### Pinout Configuration

| Component | ESP32-C3 Pin | Function |
| :--- | :--- | :--- |
| **TFT SCLK** | GPIO0 | SPI Clock |
| **TFT MOSI** | GPIO1 | SPI MOSI |
| **TFT RST** | GPIO2 | Reset |
| **TFT DC** | GPIO3 | Data/Command |
| **TFT CS** | GPIO10 | Chip Select |
| **Backlight (BL)**| GPIO4 | PWM Brightness |
| **I2C SCL** | GPIO6 | Optional Sensor Bus |
| **I2C SDA** | GPIO5 | Optional Sensor Bus |

---

## 📁 Repository Structure

```
├── roundclock.yaml       # Main ESPHome configuration file
├── gc9b72_display.h     # C++ Header for clock rendering & redraw logic
├── Arduino_GC9B72.h     # Custom GC9B72 display driver header
├── Arduino_GC9B72.cpp   # Custom GC9B72 display driver implementation
└── README.md            # Project documentation
```

# ⚙️ Installation & Setup
## Prepare ESPHome:
Ensure you have ESPHome running via Home Assistant Add-on or CLI.

## Copy Files:
Copy all repository files (roundclock.yaml, gc9b72_display.h, Arduino_GC9B72.h, and Arduino_GC9B72.cpp) into your ESPHome configuration directory (e.g., /config/esphome/).

## Configure Wi-Fi:
Open roundclock.yaml and update the Wi-Fi credentials with your own:

```
wifi:
  ssid: "YOUR_WIFI_SSID"
  password: "YOUR_WIFI_PASSWORD"
```

## Compile & Flash:
Run compilation and flash your ESP32-C3:
```
esphome run roundclock.yaml
```

##🔌 Home Assistant Integration
Once connected to Wi-Fi, ESPHome will automatically discover the device in Home Assistant (or you can add it manually via the Native API integration).

Entities created:
```
light.luminozitate_display: Controls display backlight brightness (0–100%).
```

```
select.selectare_fus_orar: Timezone selector providing real-time clock adjustment.
```

## 👏 Credits & Acknowledgments

MaliosDark / Arduino_GC9B72 - Special thanks for the custom GC9B72 display driver written for Arduino_GFX.

## 📄 License
This project is licensed under the MIT License - see the LICENSE file for details.
