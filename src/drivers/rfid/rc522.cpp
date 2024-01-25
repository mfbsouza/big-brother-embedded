#include "rc522.h"
#include <MFRC522.h>
#include <SPI.h>

#define SS_PIN 5
#define RST_PIN 2

MFRC522 rfid_reader(SS_PIN, RST_PIN);

void rfid_init()
{
	SPI.begin();
	rfid_reader.PCD_Init();
}

bool rfid_available()
{
	if (!rfid_reader.PICC_IsNewCardPresent()) return false;
	if (!rfid_reader.PICC_ReadCardSerial()) return false;
	return true;
}

uint8_t rfid_read_tag(unsigned char *buf, size_t size)
{
	size_t tocpy = (UID_MAX_SIZE >= size) ? size : UID_MAX_SIZE;
	memcpy(buf, rfid_reader.uid.uidByte, tocpy);
	rfid_reader.PICC_HaltA();
	return (uint8_t)tocpy;
}