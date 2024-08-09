#include "globals.h"
#include <stdio.h>
#include <string.h>

const char *get_web_url(char *endpoint) {
  char *url = malloc(strlen(WEB_SUBMIT_URL) + strlen(endpoint) + 1);
  sprintf(url, "%s%s", WEB_SUBMIT_URL, endpoint);

  return url;
}
