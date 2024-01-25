#include "esp32-wifi.h"
#include <WiFi.h>

void wifi_connect(const char *ssid, const char *passwd) {
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, passwd);
}

bool wifi_is_conneted(void) {
	if(WiFi.status() != WL_CONNECTED) return false;
	else return true;
}

String wifi_get_local_ip(void) {
	return WiFi.localIP().toString();
}