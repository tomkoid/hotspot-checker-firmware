| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 | Linux |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- | ----- |

# Hotspot Checker Firmware

This firmware is used to check hotspot availability. The firmware connects to a hotspot Wi-Fi and sends ping requests to a remote server. If the remote [backend server](https://codeberg.org/tomkoid/hotspot-checker-backend) has not received any message in an amount of time, it will send a notification to the phone sharing the hotspot through [Ntfy](https://ntfy.sh/). This is useful especially on iOS where hotspots are extremely unstable.

## How to use

To use this project, you need to have [esp-idf](https://github.com/espressif/esp-idf) installed on your system.
Select the instructions depending on Espressif chip installed on your development board:

- [ESP32 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html)
- [ESP32-S2 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html)

After a successful project clone, you'll likely want to configure the project with `idf.py menuconfig` to change the Wi-Fi SSID and password of the hotspot and to change the backend endpoint with the correct backend password.

These settings are located in `Wi-Fi Settings` and `Server Settings`.

### Build and flash

Before flashing on Linux, you should make sure that you've installed [esptool.py](https://github.com/espressif/esptool) and your user is in the `lock`, `dialout` or `uucp` group.

To build and flash the firmware, first run `idf.py -p PORT build` and then run `idf.py -p PORT flash`.

Also after flashing, you can run `idf.py -p PORT monitor` to see the output logs.

For more information on structure and contents of ESP-IDF projects, please refer to Section [Build System](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html) of the ESP-IDF Programming Guide.

## Roadmap

- [x] Add base functionality (e.g. ping requests)
- [x] LED indicator
- [x] Start and pause requests
- [x] Hold touch sensor to pause hotspot checking (press the restart button to resume hotspot checking)
- [x] Able to send ping requests through HTTPS

## Troubleshooting

* Program upload failure

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `idf.py menuconfig` menu, and try again.

# License

This project is licensed under the MIT License. See LICENSE for more information.

Copyright 2024 Tomkoid
