#include <Arduino.h>

#include <WM1110_Geolocation.hpp>
#include <WM1110_BLE.hpp>
#include <WM1110_Storage.hpp>
#include <WM1110_At_Config.hpp>

// Receive command data buffer
uint8_t cmd_data_buf[244] = {0};

// Receive command data size
uint8_t cmd_data_size = 0;

void setup()
{
    // Initializes the debug output
    Serial.begin(115200);
    while (!Serial) delay(100);     // Wait for ready   

    printf("WM1110 AT Config Example\r\n");

    // Initializes the storage area    
    wm1110_storage.begin();
    wm1110_storage.loadBootConfigParameters(); // Load all parameters (WM1110_Param_Var.h)

    // Initialize command parsing
    wm1110_at_config.begin();

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
    // Get the data received by Serial  
    if(wm1110_at_config.receiveSerialCmd(cmd_data_buf,&cmd_data_size))
    {
        // Set received data from serial
        cmd_parse_type = 0;
    
        // Parsing command 
        wm1110_at_config.parseCmd((char *)cmd_data_buf,cmd_data_size);

        // Clear buffer & size
        memset(cmd_data_buf, 0, cmd_data_size);
        cmd_data_size = 0;
    }
    // Get the data received by Bluetooth
    if(wm1110_ble.getBleRecData(cmd_data_buf,&cmd_data_size))
    {
        // Set received data from BLE
        cmd_parse_type = 1;

        // Parsing command 
        wm1110_at_config.parseCmd((char *)cmd_data_buf,cmd_data_size);

        // Clear buffer & size
        memset(cmd_data_buf,0,cmd_data_size);
        cmd_data_size = 0;

        // Commands are received from the serial port by default
        cmd_parse_type = 0;
    }
}







