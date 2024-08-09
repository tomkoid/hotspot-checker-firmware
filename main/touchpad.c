#include "FreeRTOSConfig.h"
#include "driver/touch_sensor.h"
#include "freertos/FreeRTOS.h"
#include "portmacro.h"
// #include "driver/touch_sensor_common.h"

#define TOUCH_PAD_NO_CHANGE (-1)
#define TOUCH_THRESH_NO_USE (0)
#define TOUCH_FILTER_MODE_EN (0)
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10)

static void tp_example_touch_pad_init(void) {
  for (int i = 0; i < TOUCH_PAD_MAX; i++) {
    touch_pad_config(i, TOUCH_THRESH_NO_USE);
  }
}

#include "esp_err.h"
static void touchpad_handle(void *pvParameters) {
  ESP_ERROR_CHECK(touch_pad_init());
  touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
  tp_example_touch_pad_init();

  uint16_t touch_value;
  printf("Touch Sensor normal mode read, the output format is: \nTouchpad "
         "num:[raw data]\n\n");
  while (1) {
    for (int i = 0; i < TOUCH_PAD_MAX; i++) {
      touch_pad_read(i, &touch_value);
      printf("T%d:[%4" PRIu16 "] ", i, touch_value);
    }
    printf("\n");
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }

  vTaskDelete(NULL);
}
