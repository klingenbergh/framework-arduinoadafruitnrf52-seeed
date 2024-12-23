#include <Arduino.h>

#include <WM1110_Geolocation.hpp>
#include <WM1110_BLE.hpp>

// Ble receives data buffer
uint8_t ble_rec_buf[244] = {0};

// Ble receives data size
uint8_t ble_rec_size = 0;

void setup()
{
    // Initializes the debug output
    Serial.begin(115200);
    while (!Serial) delay(100);     // Wait for ready  

    Serial.print("WM1110 BLE Transmit & Receive Example\r\n");  

    wm1110_ble.begin(); // Init BLE

    wm1110_ble.setName("Wio Tracker 1110"); // Set the  Bluetooth broadcast name "Wio Tracker 1110"

    // Set broadcast parameters
    // true: central,  false:peripheral   empty:both 
    wm1110_ble.setStartParameters();

    // Start broadcast
    wm1110_ble.startAdv();
}

void loop()
{    
    // Get the data received by Bluetooth
    if(wm1110_ble.getBleRecData(ble_rec_buf,&ble_rec_size)) 
    {
        // Print the data received by Bluetooth
        printf("%s",ble_rec_buf);

        // The received data is circuited back
        wm1110_ble.ble_trace_print("%s",ble_rec_buf);

        // Clear buffer & size
        memset(ble_rec_buf,0,ble_rec_size);
        ble_rec_size = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
