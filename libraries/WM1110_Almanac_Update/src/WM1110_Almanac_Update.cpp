#include "WM1110_Almanac_Update.hpp"

#include <LbmWm1110.hpp>
#include <Lbmx.hpp>

#include <Lbm_Modem_Common.hpp>

#include "common/smtc_board/smtc_board.h"
#include "common/apps_modem_common.h"
#include "common/apps_modem_event.h"

#include <lbm/smtc_modem_api/smtc_modem_utilities.h>
#include <lbm/smtc_modem_core/radio_drivers/lr11xx_driver/src/lr11xx_types.h>
#include <lbm/smtc_modem_core/radio_drivers/lr11xx_driver/src/lr11xx_gnss_types.h>
#include <lbm/smtc_modem_core/radio_drivers/lr11xx_driver/src/lr11xx_gnss.h>
#include "almanac.h"


#define OFFSET_BETWEEN_GPS_EPOCH_AND_UNIX_EPOCH 315964800

lr11xx_gnss_almanac_full_read_bytestream_t lr11xx_almanac;

static LbmWm1110& lbmWm1110 = LbmWm1110::getInstance();
ralf_t* modem_radio = lbmWm1110.getRadio();


WM1110_Almanac_Update* WM1110_Almanac_Update::instance_ = nullptr;


#if 1 // public function
WM1110_Almanac_Update& WM1110_Almanac_Update::getInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = new WM1110_Almanac_Update();
    }

    return *instance_;
}

WM1110_Almanac_Update::WM1110_Almanac_Update(void)
{

}

void WM1110_Almanac_Update::begin( void )
{

}

void WM1110_Almanac_Update::updateStoredAlmanac( void )
{
    uint32_t almanac_date = getStoredAlmanacDateTime( );
    full_almanac_date_time = getFullAlmanacDateTime();
    printf("LR11XX almanac date: %u, Local almanac date: %u\r\n", almanac_date, full_almanac_date_time );

    if( almanac_date < full_almanac_date_time )
    {
        bool ret = almanac_update( modem_radio->ral.context, full_almanac, sizeof( full_almanac ));
    }
    else if(almanac_date > full_almanac_date_time)
    {
        printf("Please run get_full_almanac.py to update full almanac buffer firstly\r\n");
    }
    else
    {
        printf( "Local almanac matches LR11XX almanac -> no update\n" );
    }
}

uint32_t WM1110_Almanac_Update::getStoredAlmanacDateTime( void )
{
    if( lr11xx_gnss_read_almanac( modem_radio->ral.context , lr11xx_almanac ) == LR11XX_STATUS_OK )
    {
        // printf( "almanac_date_raw: %02x%02x, %02x%02x\r\n", lr11xx_almanac[2], lr11xx_almanac[1], lr11xx_almanac[24], lr11xx_almanac[23] );
        // uint16_t almanac_date_raw = ( uint16_t )( ( lr11xx_almanac[2] << 8 ) | lr11xx_almanac[1] );
        uint16_t almanac_date_raw = ( uint16_t )( ( lr11xx_almanac[24] << 8 ) | lr11xx_almanac[23] );
        uint32_t almanac_date = ( OFFSET_BETWEEN_GPS_EPOCH_AND_UNIX_EPOCH + 24 * 3600 * ( 2048 * 7 + almanac_date_raw ) );
        return almanac_date;
    }
    else
    {
        return 0;
    }
}

bool WM1110_Almanac_Update::get_almanac_crc( const void* ral_context, uint32_t* almanac_crc )
{
    lr11xx_status_t                         err;
    lr11xx_gnss_context_status_bytestream_t context_status_bytestream;
    lr11xx_gnss_context_status_t            context_status;

    err = lr11xx_gnss_get_context_status( ral_context, context_status_bytestream );
    if( err != LR11XX_STATUS_OK )
    {
        printf( "Failed to get gnss context status\n" );
        return false;
    }

    err = lr11xx_gnss_parse_context_status_buffer( context_status_bytestream, &context_status );
    if( err != LR11XX_STATUS_OK )
    {
        printf( "Failed to parse gnss context status to get almanac status\n" );
        return false;
    }

    *almanac_crc = context_status.global_almanac_crc;

    return true;
}

#endif


#if 1 // private function
bool WM1110_Almanac_Update::almanac_update( const void* ral_context, uint8_t *almanac_data, uint16_t almanac_len )
{
    uint32_t global_almanac_crc, local_almanac_crc;
    local_almanac_crc = ( almanac_data[6] << 24 ) + ( almanac_data[5] << 16 ) + ( almanac_data[4] << 8 ) + ( almanac_data[3] );

    if( get_almanac_crc( ral_context, &global_almanac_crc ) == false )
    {
        printf( "Failed to get almanac CRC before update\n" );
        return false;
    }
    if( global_almanac_crc != local_almanac_crc )
    {
        printf( "Local almanac doesn't match LR11XX almanac -> start update\n" );

        /* Load almanac in flash */
        uint16_t almanac_idx = 0;
        while( almanac_idx < almanac_len )
        {
            if( lr11xx_gnss_almanac_update( ral_context, almanac_data + almanac_idx, 1 ) != LR11XX_STATUS_OK )
            {
                printf( "Failed to update almanac\n" );
                return false;
            }
            almanac_idx += LR11XX_GNSS_SINGLE_ALMANAC_WRITE_SIZE;
        }

        /* Check CRC again to confirm proper update */
        if( get_almanac_crc( ral_context, &global_almanac_crc ) == false )
        {
            printf( "Failed to get almanac CRC after update\n" );
            return false;
        }
        if( global_almanac_crc != local_almanac_crc )
        {
            printf( "0x%08x, 0x%08x\r\n", global_almanac_crc, local_almanac_crc );
            printf( "Local almanac doesn't match LR11XX almanac -> update failed\n" );
            return false;
        }
        else
        {
            uint32_t almanac_date = getStoredAlmanacDateTime( );
            printf("LR11XX new almanac date: %u\r\n", almanac_date );
            printf( "Almanac update succeeded\r\n" );
        }
    }
    else
    {
        printf( "Local almanac matches LR11XX almanac -> no update\n" );
    }

    return true;
}

uint32_t WM1110_Almanac_Update::getFullAlmanacDateTime( void )
{
    //current
    uint16_t almanac_date_raw = ( uint16_t )( ( full_almanac[2] << 8 ) | full_almanac[1] );
    uint32_t almanac_date = ( OFFSET_BETWEEN_GPS_EPOCH_AND_UNIX_EPOCH + 24 * 3600 * ( 2048 * 7 + almanac_date_raw ) ); 
    
    full_almanac_date_time = almanac_date;
    return full_almanac_date_time;
}

#endif

WM1110_Almanac_Update wm1110_almanac_update;




