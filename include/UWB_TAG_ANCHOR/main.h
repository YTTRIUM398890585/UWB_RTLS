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

// GLOBALS
// Linked list of known anchors
struct MyLink* uwb_data;

#endif

// We'll use a "short address" to make it easier to reference devices
char shortAddress[6];

// CALLBACK HANDLERS
void newRange();
void newDevice(DW1000Device* device);
void inactiveDevice(DW1000Device* device);