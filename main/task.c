#include "driver/gpio.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "globals.h"
#include <esp_log.h>
#include <esp_tls.h>
#include <stdlib.h>

#include "url.c"

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

esp_http_client_handle_t get_http_client(const char *url) {
  const char *TAG = "GET_HTTP_CLIENT";

  esp_http_client_config_t config = {
    .url = url,
    .method = HTTP_METHOD_POST,
#if CONFIG_ESP_REQUEST_DEBUG
    .event_handler = _http_event_handler, // debug info
#endif
    .crt_bundle_attach = esp_crt_bundle_attach,
  };

  esp_http_client_handle_t client = esp_http_client_init(&config);
  if (client == NULL) {
    ESP_LOGE(TAG, "Failed to initialise HTTP connection");
    return NULL;
  }

  return client;
}

esp_err_t http_post(esp_http_client_handle_t client) {
  const char *TAG = "HTTP_POST";

  const char *post_data = WEB_SUBMIT_PASSWORD;
  esp_http_client_set_header(client, "Content-Type", "text/plain");
  esp_http_client_set_post_field(client, post_data, strlen(post_data));

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
  }

  return err;
}

void submit_task() {
  const char *TAG = "SUBMIT_TASK";

  esp_http_client_handle_t client = get_http_client(WEB_SUBMIT_URL);

  while (1) {
    esp_err_t err = http_post(client);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
      vTaskDelay(3000 / portTICK_PERIOD_MS);
      continue;
    }

    gpio_set_level(BUILTIN_LED, 0);

    int interval = WEB_SUBMIT_INTERVAL;

    for (int countdown = interval; countdown > 0; countdown--) {
      // ESP_LOGI(TAG, "%d... ", countdown);
      printf("%d... ", countdown);
      fflush(stdout);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("\n");
    ESP_LOGI(TAG, "Starting again!");

    gpio_set_level(BUILTIN_LED, 1);

    // if device is physically stopped, stop submitting
    if (device_stopped) {
      break;
    }
  }

  esp_http_client_cleanup(client);
}

void start_task() {
  const char *TAG = "START_TASK";

  esp_http_client_handle_t client = get_http_client(get_web_url("/start"));

  int attempts = 0;
  int max_attempts = 5;
  while (1) {
    if (attempts > max_attempts) {
      ESP_LOGE(TAG, "Failed to start after %d attempts", max_attempts);
      exit(1);
    }
    esp_err_t err = http_post(client);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
      vTaskDelay(3000 / portTICK_PERIOD_MS);
      attempts++;
      continue;
    } else {
      break;
    }
    attempts++;
  }

  esp_http_client_cleanup(client);
}

void stop_task() {
  const char *TAG = "STOP_TASK";

  esp_http_client_handle_t client = get_http_client(get_web_url("/stop"));

  int attempts = 0;
  int max_attempts = 5;
  while (1) {
    if (attempts > max_attempts) {
      ESP_LOGE(TAG, "Failed to stop after %d attempts", max_attempts);
      exit(1);
    }
    esp_err_t err = http_post(client);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "HTTP request failed: %s", esp_err_to_name(err));
      vTaskDelay(3000 / portTICK_PERIOD_MS);
      attempts++;
      continue;
    } else {
      break;
    }
    attempts++;
  }

  esp_http_client_cleanup(client);
}
