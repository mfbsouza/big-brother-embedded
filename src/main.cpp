#include <Arduino.h>
#include "drivers/lcd/lcd.h"
#include "drivers/rfid/rc522.h"
#include "drivers/keypad/key4x4.h"
#include "drivers/wifi/esp32-wifi.h"

#include <HTTPClient.h>
#include <ArduinoJson.h>

#define UART_SPEED 9600

enum State {
	PRINT_MENU = 0,
	WAIT_KEY_INPUT,
	PRINT_INSERT_RFID,
	PRINT_SELECT_EQUIP,
	WAIT_RFID_INPUT,
	GET_USER,
	WAIT_EQUIP_INPUT,
	CHECK_IF_IS_ADM,
	CHECK_IF_USER_EXISTS,
	POST_NEW_USER,
	GET_EQUIP,
	RENT_EQUIP,
	RETURN_EQUIP
};

unsigned char usr_tag[UID_MAX_SIZE];
String db_server_url = "http://192.168.197.166:3030";

void state_machine(void);
String bytearray_to_hex_string(const unsigned char *buf, size_t size);

void setup() {
	Serial.begin(UART_SPEED);
	lcd_init();
	rfid_init();
	wifi_connect("nao_pagou_ne", "aicomplica");
	Serial.print("Connecting to WIFI.");
	while(!wifi_is_conneted()) {
		Serial.print(".");
		delay(1000);
	}
	Serial.println("\nconnected with ip: " + wifi_get_local_ip());
}

void loop() {
	state_machine();
}

String bytearray_to_hex_string(const unsigned char *buf, size_t size) {
	String result = "";
	for (int i = 0; i < size; i++) {
		if(buf[i] < 0x10) result += '0';
		result += String(buf[i], HEX);
	}
	result.toUpperCase();
	return result;
}

void state_machine(void) {
	static enum State state = PRINT_MENU;
	static enum State last_state = PRINT_MENU;
	static char option, key;
	static unsigned char tag[UID_MAX_SIZE];
	static JsonDocument user, equip;
	static HTTPClient http;
	static int http_res_code;
	static String payload;
	static int try_cnt = 0;
	static String equip_input = "";
	bool is_adm, in_use;

	switch (state)
	{
	case PRINT_MENU:
		lcd_write("   1-Cadastro   2-Pegar 3-Voltar", 32);
	equip_input = "";
		last_state = state;
		state = WAIT_KEY_INPUT;
		break;
	
	case WAIT_KEY_INPUT:
		option = keypad_getkey();
		if (option) {
			last_state = state;
			state = PRINT_INSERT_RFID;
		}
		break;
	
	case PRINT_INSERT_RFID:
		lcd_write("Aproxime Cracha", 17);
		last_state = state;
		state = WAIT_RFID_INPUT;
		break;
	
	case WAIT_RFID_INPUT:
		if (rfid_available()) {
			rfid_read_tag(tag, UID_MAX_SIZE);
			lcd_write("Lido! Aguarde...", 16);
			delay(2000);
			if (last_state == PRINT_INSERT_RFID) {
				last_state = state;
				state = GET_USER;
			} else if (last_state == CHECK_IF_IS_ADM) {
				try_cnt = 0;
				last_state = state;
				state = CHECK_IF_USER_EXISTS;
			}
		}
		break;
	
	case GET_USER:
		http.begin(db_server_url + "/user/tag/" + bytearray_to_hex_string(tag, UID_MAX_SIZE));
		http_res_code = http.GET();
		if (http_res_code == 200) {
			payload = http.getString();
			deserializeJson(user, payload.c_str());
			switch (option)
			{
			case '1':
				try_cnt = 0;
				last_state = state;
				state = CHECK_IF_IS_ADM;
				option = 0;
				break;
				
			case '2':
				try_cnt = 0;
				last_state = state;
				state = PRINT_SELECT_EQUIP;
				break;

			case '3':
				try_cnt = 0;
				last_state = state;
				state = PRINT_SELECT_EQUIP;
				break;
				
			default:
				Serial.println("unknow option!");
				return;
				break;
			}
		} else if (http_res_code == 404) {
			lcd_write("Usuario nao cadastrado!", 23);
			last_state = state;
			state = PRINT_MENU;
			delay(3000);
		} else {
			Serial.println("erro no get!");
			delay(2000);
			if (++try_cnt == 3) {
				last_state = state;
				state = PRINT_MENU;
			}
		}
		break;

	case CHECK_IF_IS_ADM:
		is_adm = user["admin"];
		if (is_adm) {
			lcd_write("Aproxime nova   Tag RFID", 24);
			last_state = state;
			state = WAIT_RFID_INPUT;
		} else {
			lcd_write("Voce nao eh Adm!", 16);
			last_state = state;
			state = PRINT_MENU;
			delay(3000);
		}
		break;
	
	case CHECK_IF_USER_EXISTS:
		http.begin(db_server_url + "/user/tag/" + bytearray_to_hex_string(tag, UID_MAX_SIZE));
		http_res_code = http.GET();
		if (http_res_code == 404) {
			try_cnt = 0;
			last_state = state;
			state = POST_NEW_USER;
		} else if (http_res_code == 200) {
			lcd_write("Usuario ja cadastrado!", 22);
			last_state = state;
			state = PRINT_MENU;
			delay(3000);
		} else {
			Serial.println("erro no get!");
			delay(2000);
			if (++try_cnt == 3) {
				last_state = state;
				state = PRINT_MENU;
			}
		}
		break;
		break;

	case POST_NEW_USER:
		user["id"] = 0;
		user["name"] = "John Doe";
		user["admin"] = false;
		user["tag"] = bytearray_to_hex_string(tag, UID_MAX_SIZE);
		http.begin(db_server_url + "/user");
		http.addHeader("Content-Type", "application/json");
		serializeJson(user, payload);
		http_res_code = http.POST(payload);
		if (http_res_code == 200) {
			payload = http.getString();
			String msg = "Usuario cadastrado! Login: " + payload;
			lcd_write(msg.c_str(), msg.length());
			delay(10000);
			last_state = state;
			state = PRINT_MENU;
		} else {
			Serial.println("erro no post!");
			delay(2000);
			if (++try_cnt == 3) {
				last_state = state;
				state = PRINT_MENU;
			}
		}
		break;
	
	case PRINT_SELECT_EQUIP:
		lcd_write("Digite o numero do equipamento", 30);
		last_state = state;
		state = WAIT_EQUIP_INPUT;
		break;

	case WAIT_EQUIP_INPUT:
		key = keypad_getkey();
		if (key) {
			if (key == '*') {
				lcd_write("Lido! Aguarde...", 16);
				last_state = state;
				state = GET_EQUIP;
			} else if (key == '#' && equip_input.length() > 0) {
				equip_input = equip_input.substring(0, equip_input.length() - 1);
				lcd_write(equip_input.c_str(), equip_input.length());
			} else {
				equip_input += String(key);
				lcd_write(equip_input.c_str(), equip_input.length());
			}
		}
		delay(100);
		break;
	
	case GET_EQUIP:
		http.begin(db_server_url + "/equip/id/" + equip_input);
		http_res_code = http.GET();
		if (http_res_code == 200) {
			payload = http.getString();
			deserializeJson(equip, payload.c_str());
			switch (option)
			{
			case '2':
				try_cnt = 0;
				last_state = state;
				state = RENT_EQUIP;
				option = 0;
				break;
				
			case '3':
				try_cnt = 0;
				last_state = state;
				state = RETURN_EQUIP;
				break;
				
			default:
				Serial.println("unknow option, again!");
				return;
				break;
			}
		} else if (http_res_code == 404) {
			lcd_write("Equipamento nao existe!", 23);
			last_state = state;
			state = PRINT_MENU;
			delay(3000);
		} else {
			Serial.println("erro no get!");
			delay(2000);
			if (++try_cnt == 3) {
				last_state = state;
				state = PRINT_MENU;
			}
		}
		break;
	
	case RENT_EQUIP:
		in_use = equip["in_use"];
		if (!in_use) {
			http.begin(db_server_url + "/request/rent/" + equip_input);
			http.addHeader("Content-Type", "application/json");
			serializeJson(user, payload);
			http_res_code = http.POST(payload);
			if (http_res_code == 200) {
				lcd_write("Ok! equipamento  alugado!", 25);
				delay(10000);
				last_state = state;
				state = PRINT_MENU;
			} else {
				Serial.println("erro no post!");
				delay(2000);
				if (++try_cnt == 3) {
					last_state = state;
					state = PRINT_MENU;
				}
			}
		} else {
			lcd_write("Equipamento em  uso!", 20);
			last_state = state;
			state = PRINT_MENU;
			delay(3000);
		}
		break;
	
	case RETURN_EQUIP:
		in_use = equip["in_use"];
		if (in_use) {
			if (equip["user_id"] == user["id"]) {
				http.begin(db_server_url + "/request/return/" + equip_input);
				http.addHeader("Content-Type", "application/json");
				serializeJson(user, payload);
				http_res_code = http.POST(payload);
				if (http_res_code == 200) {
					lcd_write("Ok! equipamento  devolvido!", 27);
					delay(10000);
					last_state = state;
					state = PRINT_MENU;
				} else {
					Serial.println("erro no post!");
					delay(2000);
					if (++try_cnt == 3) {
						last_state = state;
						state = PRINT_MENU;
					}
				}
			} else {
				lcd_write("Equipamento nao esta com voce!", 30);
				last_state = state;
				state = PRINT_MENU;
				delay(3000);
			}
		} else {
			lcd_write("Equipamento nao esta em uso!", 28);
			last_state = state;
			state = PRINT_MENU;
			delay(3000);
		}
		break;

	default:
		Serial.println("nao devia estar aqui!");
		break;
	}
}
