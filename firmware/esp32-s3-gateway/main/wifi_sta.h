#ifndef WIFI_STA_H
#define WIFI_STA_H

#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define WIFI_CONNECTED_BIT BIT0

void wifi_sta_init(void);

bool wifi_sta_is_connected(void);

/* 提供给main等待 */
EventGroupHandle_t wifi_get_event_group(void);

#endif