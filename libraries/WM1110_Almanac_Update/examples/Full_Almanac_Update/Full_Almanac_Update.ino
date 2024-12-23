#include <Arduino.h>

#include <LbmWm1110.hpp>
#include <Lbmx.hpp>
#include <WM1110_Almanac_Update.hpp>

/*
    tips:
        1.Make sure the almanac.h files are up to date,if not,please update it.
        2.Note that the updated almanac in the almanac.h file are not necessarily newer than the almanac stored on the chip
*/

// Instance
static LbmWm1110& lbmWm1110 = LbmWm1110::getInstance();

uint32_t local_almanac_date = 0;    // Local almanac date
uint32_t stored_almanac_date = 0;   // Almanac date stored in WM1110

// MyLbmxEventHandlers
class MyLbmxEventHandlers : public LbmxEventHandlers
{
protected:
    void reset(const LbmxEvent& event) override;

};

void MyLbmxEventHandlers::reset(const LbmxEvent& event)
{

}

static void ModemEventHandler()
{
    static LbmxEvent event;
    static MyLbmxEventHandlers handlers;

    while (event.fetch())
    {
        printf("----- %s -----\n", event.getEventString().c_str());

        handlers.invoke(event);
    }
}

void setup()
{
    // Initializes the debug output
    Serial.begin(115200);
    while (!Serial) delay(100);     // Wait for ready 

    printf("WM1110 Almanac Update Example\r\n");

    // Initialize WM1110 hardware peripherals
    lbmWm1110.begin();

    // Start runing
    LbmxEngine::begin(lbmWm1110.getRadio(), ModemEventHandler);

    // Initialize almanac update
    wm1110_almanac_update.begin();

    // Update the almanac
    wm1110_almanac_update.updateStoredAlmanac();

    // Get Local almanac date
    local_almanac_date = wm1110_almanac_update.getFullAlmanacDateTime();

    // Get almanac date stored in WM1110 after updated
    stored_almanac_date = wm1110_almanac_update.getStoredAlmanacDateTime();
}

void loop()
{    
    if(local_almanac_date != stored_almanac_date)
    {
        printf("Update WM1110 almanac fail\r\n");
    }
    else
    {
        printf("Update WM1110 almanac success,almanac date:%lu\r\n",local_almanac_date);        
    }
    delay(1000);
}

////////////////////////////////////////////////////////////////////////////////
