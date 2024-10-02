// This program calibrates an ESP32_UWB module intended for use as a fixed anchor point
// uses binary search to find anchor antenna delay to calibrate against a known distance
//
// modified version of Thomas Trojer's DW1000 library is required!

// Remote tag (at origin) must be set up with default antenna delay (library default = 16384)

// user input required, possibly unique to each tag:
// 1) accurately measured distance from anchor to tag
// 2) address of anchor
//
// output: antenna delay parameter for use in final anchor setup.
// S. James Remington 2/20/2022

// INCLUDES
#include <Arduino.h>

#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"

// Include the C string library to convert macro defined string to char array
#include <cstring>

#include "ANCHOR_CALIBRATION/define.h"
#include "ANCHOR_CALIBRATION/main.h"

// this is not used, but included so that the same environment can be used for cal and normal operation, wifi.h dependancy bullshits
#include <micro_ros_platformio.h>

void setup()
{
	Serial.begin(115200);
	while (!Serial)
		;
	// init the configuration
	SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
	DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); // Reset, CS, IRQ pin

	Serial.print("Starting Adelay ");
	Serial.println(this_anchor_Adelay);
	Serial.print("Measured distance ");
	Serial.println(this_anchor_target_distance);

	DW1000.setAntennaDelay(this_anchor_Adelay);

	DW1000Ranging.attachNewRange(newRange);
	DW1000Ranging.attachNewDevice(newDevice);
	DW1000Ranging.attachInactiveDevice(inactiveDevice);
	// Enable the filter to smooth the distance
	// DW1000Ranging.useRangeFilter(true);

	// convert macro defined string to char array
	// 24 is 23 characters + null terminator
	char DEVICE_ADD_CHAR[24];
	strcpy(DEVICE_ADD_CHAR, DEVICE_ADDRESS);

    // this is for localisation, so for calibration just fill in dummy values
    float anchor_coords[] = {0, 0, 0};

    // start the module as anchor, don't assign random short address
	DW1000Ranging.startAsAnchor(DEVICE_ADD_CHAR, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false, anchor_coords);
}

void loop()
{
	DW1000Ranging.loop();
}

void newRange()
{
    Serial.print(F("Device range updated: "));
    Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);

    if (calibrated) {
        Serial.print("\t");
        Serial.print(DW1000Ranging.getDistantDevice()->getRange());
        Serial.print(" m\t");
        Serial.print(DW1000Ranging.getDistantDevice()->getRXPower());
        Serial.print(" dBm");
        Serial.print(" final Adelay ");
		Serial.print(this_anchor_Adelay);
        Serial.print(" final Adelay stored ");
		Serial.println(DW1000.getAntennaDelay());
    } else {
        static float last_delta = 0.0;

        float dist = DW1000Ranging.getDistantDevice()->getRange();

        Serial.print(" ,");
        Serial.print(dist);
        if (Adelay_delta == 0) {
            // Serial.print(" m, final Adelay ");
            // Serial.println(this_anchor_Adelay);
            calibrated = true;
        }

        float this_delta = dist - this_anchor_target_distance; // error in measured distance

        if (this_delta * last_delta < 0.0) Adelay_delta = Adelay_delta - 1; // sign changed, reduce step size
        last_delta = this_delta;

        if (this_delta > 0.0)
            this_anchor_Adelay += Adelay_delta; // new trial Adelay
        else
            this_anchor_Adelay -= Adelay_delta;

        Serial.print(", Adelay = ");
        Serial.println(this_anchor_Adelay);
        //  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
        DW1000.setAntennaDelay(this_anchor_Adelay);
    }
}

void newDevice(DW1000Device* device)
{
	Serial.print("Device added: ");
	Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device* device)
{
	Serial.print("delete inactive device: ");
	Serial.println(device->getShortAddress(), HEX);
}