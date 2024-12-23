#include <Arduino.h>

#include <WM1110_Geolocation.hpp>
#include <WM1110_BLE.hpp>

// The buffer array used to get the Bluetooth name
char ble_name[24] = {0};

void setup()
{
    uint8_t len = 23;
    // Initializes the debug output
    Serial.begin(115200);
    while (!Serial) delay(100);     // Wait for ready

    Serial.print("WM1110 BLE Scan Example\r\n");  

    wm1110_ble.begin(); // Init BLE

    wm1110_ble.setName("Wio Tracker 1110"); // Set the  Bluetooth broadcast name "Wio Tracker 1110"

    wm1110_ble.getName(ble_name,len); // Get the  Bluetooth broadcast name

    // Set scan parameters
    // true: central,  false:peripheral   empty:both
    wm1110_ble.setStartParameters(true);

}

void loop()
{    
    // Start scanning ibeacon
    printf("Starting scan---------\r\n");
    wm1110_ble.startScan();
    delay(5000);

    // Stop scanning ibeacon
    wm1110_ble.stopScan();
    printf("Stoped scan---------\r\n");

    // Sort the scan results
    wm1110_ble.sortScanedResults();

    // Display
    printf("Scaned results: \r\n");  
    wm1110_ble.displayScanedDatas( );  

    delay(1000); 
}

////////////////////////////////////////////////////////////////////////////////
