#pragma once

#ifdef IS_TAG
// CONSTANTS
// The tag will update a server with its location information
// allowing it to be remotely tracked
// Wi-Fi credentials
const char* ssid = "INSERT SSID HERE";
const char* password = "INSERT WIFI PASSWORD HERE";
// IP address of server to send location information to
const char* host = "192.168.0.155";
uint16_t portNum = 50000;

unsigned long last_pub = 0;

#endif

#ifdef DEBUG
	unsigned long last_print = 0;
#endif