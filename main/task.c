#include "globals.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <sys/param.h>

#include "esp_http_client.h"

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048

esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
  const char *TAG = "HTTP_EVENT_HANDLER";

  switch (evt->event_id) {
  case HTTP_EVENT_ERROR:
    ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
    break;
  case HTTP_EVENT_ON_CONNECTED:
    ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
    break;
  case HTTP_EVENT_HEADER_SENT:
    ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
    break;
  case HTTP_EVENT_ON_HEADER:
    ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key,
             evt->header_value);
    break;
  case HTTP_EVENT_ON_DATA:
    ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
    if (!esp_http_client_is_chunked_response(evt->client)) {
      // Write out data
      printf("%.*s", evt->data_len, (char *)evt->data);
    }
    break;
  case HTTP_EVENT_ON_FINISH:
    ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
    break;
  case HTTP_EVENT_DISCONNECTED:
    ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
    break;
  case HTTP_EVENT_REDIRECT:
    ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
    break;
  }
  return ESP_OK;
}

void http_get_task(void *pvParameters) {
  const char *TAG = "HTTP_GET_TASK";

  while (1) {
    esp_http_client_config_t config = {
        .url = WEB_SUBMIT_URL,
        .method = HTTP_METHOD_GET,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
      int status_code = esp_http_client_get_status_code(client);
      int content_length = esp_http_client_get_content_length(client);
      ESP_LOGI(TAG, "HTTP Status = %d, content_length = %d", status_code,
               content_length);
      // read response:
      // char *buffer = malloc(content_length + 1);
      // int read_len = esp_http_client_read(client, buffer, content_length);
      // if (read_len <= 0) {
      //   ESP_LOGE(TAG, "Error reading response");
      // } else {
      //   buffer[read_len] = 0;
      //   ESP_LOGI(TAG, "Response: %s", buffer);
      // }
      // free(buffer);
    } else {
      ESP_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);

    gpio_set_level(BUILTIN_LED, 0);

    for (int countdown = 5; countdown >= 0; countdown--) {
      // ESP_LOGI(TAG, "%d... ", countdown);
      printf("%d... ", countdown);
      fflush(stdout);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("\n");
    ESP_LOGI(TAG, "Starting again!");

    gpio_set_level(BUILTIN_LED, 1);
  }
}
