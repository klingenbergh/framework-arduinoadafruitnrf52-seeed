#include <Arduino.h>

#include <WM1110_Storage.hpp>


uint32_t test_data_key = 0x3000;    // Key
uint32_t test_data_value = 0x01;    // Key value
uint32_t test_data_size = 0x4;      // Key value size


void setup()
{
    // Initializes the debug output
    Serial.begin(115200);
    while (!Serial) delay(100);   // Wait for ready

    printf("WM1110 Cache User Data Example\r\n");

    test_data_size = sizeof(test_data_value);

    // Initializes the storage area
    wm1110_storage.begin();

    // Try to read the value corresponding to the key 
    int ret = wm1110_storage.readStorageKeyValue(test_data_key,&test_data_value,test_data_size);
    if(ret >= 0 ) // Success,use this value as the initial value
    {
        printf("booting test data value:%lu\r\n",test_data_value);
    }
    else // Fail 
    {
        // Write an initial value
        ret = wm1110_storage.writeStorageKeyValue(test_data_key,&test_data_value,test_data_size);
        if(ret < 0)
        {
            printf("write test data value error:%d\r\n",ret);
        }
        else
        {
            printf("write default test data value:%lu\r\n",test_data_value);
        }
    }
}

void loop()
{
    // Changes the value,and Overlay write
    test_data_value++;
    int ret = wm1110_storage.writeStorageKeyValue(test_data_key,&test_data_value,test_data_size);
    if(ret < 0)
    {
        printf("write test data value error:%d\r\n",ret);
    }
    else
    {
        // Write success,read back and check
        int ret = wm1110_storage.readStorageKeyValue(test_data_key,&test_data_value,test_data_size);
        if(ret >= 0 )
        {
            printf("test data value:%lu\r\n",test_data_value);
        }
    }
    // Wait
    delay(1000);      
}







