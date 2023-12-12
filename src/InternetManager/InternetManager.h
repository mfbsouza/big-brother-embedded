#ifndef __INTERNET_MANAGER_H__
#define __INTERNET_MANAGER_H__

#include <Arduino.h>

typedef struct {
	int Code;
	String Payload;
} HttpResponse;

String net_wifi_connect(const char *ssid, const char *passwd);
bool net_get_wifi_status(void);
HttpResponse net_http_get(const char *url);
HttpResponse net_http_post(const char *url, const char *payload, const char *type);
HttpResponse net_http_put(const char *url, const char *payload, const char *type);

#endif /* __INTERNET_MANAGER_H__ */
