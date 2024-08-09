#ifndef WIFI_H
#define WIFI_H

#include <stdbool.h>

extern bool wifi_connected;
extern bool wifi_error;

void wifi_init_sta(void);

#endif // !WIFI_H
