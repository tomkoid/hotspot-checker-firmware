#include "FreeRTOSConfig.h"
#include "driver/gpio.h"
#include "driver/touch_sensor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "globals.h"
#include "portmacro.h"

#define TOUCH_PAD_NO_CHANGE (-1)
#define TOUCH_THRESH_NO_USE (0)
#define TOUCH_FILTER_MODE_EN (0)
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10)

static void tp_example_touch_pad_init(void) {
  for (int i = 0; i < TOUCH_PAD_MAX; i++) {
    if (i == TOUCH_PAD_NUM2) {
      continue;
    }
    touch_pad_config(i, TOUCH_THRESH_NO_USE);
  }
}

void touchpad_handle(void *pvParameters) {
  const char *TAG = "touchpad_handle";

  ESP_ERROR_CHECK(touch_pad_init());
  touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
  tp_example_touch_pad_init();

  uint16_t touch_value;
  // printf("Touch Sensor normal mode read, the output format is: \nTouchpad "
  //        "num:[raw data]\n\n");

  int touchpad_confidence = 0;
  int touchpad_confidence_minimal = 10;

  while (1) {
    int tmp_touchpad_confident = touchpad_confidence;
    for (int i = 0; i < TOUCH_PAD_MAX; i++) {
      touch_pad_read(i, &touch_value);

      if (touch_value < 400 && touch_value != 0) {
        ESP_LOGI(TAG, "confidence: %d\n", touchpad_confidence);
        touchpad_confidence++;
      }

      // printf("T%d:[%4" PRIu16 "] ", i, touch_value);
    }

    // if touchpad is confident it is actually pressed
    if (touchpad_confidence >= touchpad_confidence_minimal) {
      device_stopped = true;
      break;
    }

    if (touchpad_confidence == tmp_touchpad_confident) {
      touchpad_confidence = 0;
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  vTaskDelete(NULL);
}
