#include "esp_log.h"
#include "esp_wifi.h"

#include "globals.h"

static int s_retry_num = 0;

static EventGroupHandle_t s_wifi_event_group;

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data) {
  const char *TAG = "WIFI";
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
    esp_wifi_connect();
  } else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED && !device_exit) {
    device_stopped = true;
    esp_wifi_connect();
    s_retry_num++;
    ESP_LOGI(TAG, "retry to connect to the AP");
    // } else {
    //   xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
    // }
    ESP_LOGI(TAG, "connect to the AP fail");
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "got ip: " IPSTR, IP2STR(&event->ip_info.ip));
    s_retry_num = 0;

    if (device_stopped) {
      ESP_LOGI(TAG, "Device stopped but then connected to the AP, so set "
                    "device stopped=false");
      device_stopped = false;
    }

    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

bool is_wifi_connected() {
  wifi_mode_t mode;
  wifi_ap_record_t ap_info;

  // Get the current WiFi mode
  esp_err_t mode_result = esp_wifi_get_mode(&mode);
  if (mode_result != ESP_OK) {
    ESP_LOGE("WiFi", "Failed to get WiFi mode");
    return false;
  }

  // Check if the mode is STATION or APSTA
  if (mode != WIFI_MODE_STA && mode != WIFI_MODE_APSTA) {
    return false;
  }

  // Try to get the AP info
  esp_err_t ap_result = esp_wifi_sta_get_ap_info(&ap_info);

  // If we can get the AP info, we're connected
  return (ap_result == ESP_OK);
}

void wifi_init_sta(void *pvArguments) {
  const char *TAG = "WIFI";

  s_wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(esp_netif_init());

  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

  wifi_config_t wifi_config = {
      .sta =
          {
              .ssid = ESP_WIFI_SSID, .password = ESP_WIFI_PASS,
              /* Authmode threshold resets to WPA2 as default if password
               * matches WPA2 standards (password len => 8). If you want to
               * connect the device to deprecated WEP/WPA networks, Please set
               * the threshold value to WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK and set
               * the password with length and format matching to
               * WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK standards.
               */
              /*.threshold.authmode = WIFI_SCAN_AUTH_MODE_THRESHOLD,*/
              /*.sae_pwe_h2e = WIFI_SAE_MODE,*/
              /*.sae_h2e_identifier = H2E_IDENTIFIER,*/
          },
  };
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

  while (!is_wifi_connected()) {
    ESP_ERROR_CHECK(esp_wifi_start());
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }

  ESP_LOGI(TAG, "wifi_init_sta finished.");

  /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or
   * connection failed for the maximum number of re-tries (WIFI_FAIL_BIT). The
   * bits are set by event_handler() (see above) */
  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                         pdFALSE, pdFALSE, portMAX_DELAY);

  /* xEventGroupWaitBits() returns the bits before the call returned, hence we
   * can test which event actually happened. */
  if (bits & WIFI_CONNECTED_BIT) {
    ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", ESP_WIFI_SSID,
             ESP_WIFI_PASS);
  } else if (bits & WIFI_FAIL_BIT) {
    ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", ESP_WIFI_SSID,
             ESP_WIFI_PASS);
  } else {
    ESP_LOGE(TAG, "UNEXPECTED EVENT");
  }
}
