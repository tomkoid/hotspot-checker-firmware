#include "FreeRTOSConfig.h"
#include "driver/gpio.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "portmacro.h"
#include "sdkconfig.h"
#include <inttypes.h>
#include <stdio.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include <string.h>

#include "lwip/err.h"
#include "lwip/sys.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "lwip/dns.h"
#include "lwip/err.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "sdkconfig.h"

#include "globals.h"
#include "task.c"
#include "wifi.c"

#define FOO_BAR_ENABLE CONFIG_FOO_ENABLE_BAR

static void wifi_led_handle() {
  while (!wifi_connected && !wifi_error) {
    gpio_set_level(BUILTIN_LED, 0);
    vTaskDelay(350 / portTICK_PERIOD_MS);
    gpio_set_level(BUILTIN_LED, 1);
    vTaskDelay(350 / portTICK_PERIOD_MS);
  }

  vTaskDelete(NULL);
}

void app_main(void) {
  gpio_set_direction(BUILTIN_LED, GPIO_MODE_OUTPUT);

  gpio_set_level(BUILTIN_LED, 1);

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_LOGI("MAIN", "ESP_WIFI_MODE_STA");

  xTaskCreate(&wifi_led_handle, "wifi_led_handle", 4096, NULL, 4, NULL);
  wifi_init_sta();

  // Create the task
  http_get_task(NULL);

  printf("Restarting now.\n");
  fflush(stdout);
  esp_restart();
}
