#include <Arduino.h>

#include <WM1110_Geolocation.hpp>
#include <WM1110_BLE.hpp>

void setup()
{
    // Initializes the debug output
    Serial.begin(115200);
    while (!Serial) delay(100);     // Wait for ready  

    Serial.print("WM1110 BLE Advertising Example\r\n");  

    wm1110_ble.begin(); // Init BLE

    wm1110_ble.setName("Wio Tracker 1110"); // Set the  Bluetooth broadcast name "Wio Tracker 1110"

    // Set broadcast parameters
    // true: central,  false:peripheral
    wm1110_ble.setStartParameters(false);

}

void loop()
{    
    delay(1000);

    // Start broadcast
    printf("Starting adv---------\r\n");
    wm1110_ble.startAdv();

   // The duration of the broadcast is 10s
    delay(10000);
    
    // Stop broadcast
    wm1110_ble.stopAdv();
    printf("Stoped adv---------\r\n");

    delay(1000); 
}

////////////////////////////////////////////////////////////////////////////////
