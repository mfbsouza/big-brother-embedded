#include "InternetManager.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define WIFI_TIMEOUT 5000
#define TIME_STEP 500

String net_wifi_connect(const char *ssid, const char *passwd)
{
	size_t timeout = 0;

	WiFi.begin(ssid, passwd);
	while (WiFi.status() !=  WL_CONNECTED || timeout >= WIFI_TIMEOUT) {
		delay(TIME_STEP);
		timeout += TIME_STEP;
	}
	if (WiFi.status() == WL_CONNECTED) {
		return WiFi.localIP().toString();
	}
	else {
		return "";
	}
}

bool net_get_wifi_status()
{
	if (WiFi.status() == WL_CONNECTED) {
		return true;
	}
	else {
		return false;
	}
}

HttpResponse net_http_get(const char *url)
{
	HTTPClient http;
	HttpResponse Response = { .Payload = "{}" };

	http.begin(url);
	Response.Code = http.GET();
	if (Response.Code > 0) {
		Response.Payload = http.getString();
	}
	http.end();

	return Response;
}

HttpResponse net_http_post(const char *url, const char *payload, const char *type)
{
	HTTPClient http;
	HttpResponse Response = { .Payload = "{}" };

	http.begin(url);
	http.addHeader("Content-Type", type);
	Response.Code = http.POST(payload);
	if (Response.Code > 0) {
		Response.Payload = http.getString();
	}
	http.end();

	return Response;
}

HttpResponse net_http_put(const char *url, const char *payload, const char *type)
{
	HTTPClient http;
	HttpResponse Response = { .Payload = "{}" };

	http.begin(url);
	http.addHeader("Content-Type", type);
	Response.Code = http.PUT(payload);
	if (Response.Code > 0) {
		Response.Payload = http.getString();
	}
	http.end();

	return Response;
}
