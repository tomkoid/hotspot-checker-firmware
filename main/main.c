#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_system.h"
#include "globals.h"
#include "nvs_flash.h"

#include "task.c"
#include "touchpad.c"
#include "wifi.c"

static void wifi_connection_handle(void *pvParameters) {
  while (true) {
    // wait until wifi is connected
    while (!is_wifi_connected()) {
      gpio_set_level(BUILTIN_LED, 0);
      vTaskDelay(100 / portTICK_PERIOD_MS);
      gpio_set_level(BUILTIN_LED, 1);
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
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

  xTaskCreate(&wifi_connection_handle, "wifi_connection_handle", 4096, NULL, 4,
              NULL);

  wifi_init_sta(NULL);

  // Prepare the server
  start_task();

  // Touchpad pause functionality
  xTaskCreate(&touchpad_handle, "touchpad_handle", 4096, NULL, 5, NULL);

  // Create the task
  submit_task();

  /* STOPPED PHASE: */
  stop_task();

  while (1) {
    gpio_set_level(BUILTIN_LED, 1);
    vTaskDelay(1500 / portTICK_PERIOD_MS);
    gpio_set_level(BUILTIN_LED, 0);
    vTaskDelay(1500 / portTICK_PERIOD_MS);
  }
}
