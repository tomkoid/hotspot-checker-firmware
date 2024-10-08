#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>

#define DEBUG CONFIG_ESP_REQUEST_DEBUG

#define ESP_WIFI_SSID CONFIG_ESP_WIFI_SSID
#define ESP_WIFI_PASS CONFIG_ESP_WIFI_PASSWORD
// #define ESP_WIFI_CHANNEL 9
#define MAX_STA_CONN CONFIG_ESP_MAX_STA_CONN

#define ESP_MAXIMUM_RETRY 10

#define BUILTIN_LED 2

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

#define WEB_SUBMIT_URL CONFIG_ESP_SUBMIT_URL
#define WEB_SUBMIT_PASSWORD CONFIG_ESP_SUBMIT_PASSWORD
#define WEB_SUBMIT_INTERVAL CONFIG_ESP_SUBMIT_INTERVAL

#define WEB_PATH "/"

bool device_stopped = false;
bool device_exit = false;

#endif
