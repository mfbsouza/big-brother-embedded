#ifndef __ESP32_WIFI_DRIVER_H__
#define __ESP32_WIFI_DRIVER_H__

#include <Arduino.h>

void wifi_connect(const char *ssid, const char *passwd);
bool wifi_is_conneted(void);
String wifi_get_local_ip(void);

#endif /* __ESP32_WIFI_DRIVER_H__ */