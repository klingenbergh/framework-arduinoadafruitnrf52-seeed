#ifndef _WM1110_ALMANAC_UPDATE_H_
#define _WM1110_ALMANAC_UPDATE_H_

#pragma once

#include <cstdio>
#include <cstdlib>
#include <bluefruit.h>

#include <Lbm_Modem_Common.hpp>


class WM1110_Almanac_Update
{
    private:
        static WM1110_Almanac_Update* instance_;

        uint32_t full_almanac_date_time;
        uint32_t stored_almanac_date_time;

    public:
        WM1110_Almanac_Update(void); 

        static WM1110_Almanac_Update& getInstance();

        void begin( void );

        /*!
        *  @brief  Updata WM1110 almanac
        */     
        void updateStoredAlmanac( void );

        /*!
        *  @brief  Get current almanac date time that Stored in WM1110
        *  @return timestamp
        */       
        uint32_t getStoredAlmanacDateTime( void );

        /*!
        *  @brief  Get full almanac date time from full_almanac(almanac.h)
        *  @return timestamp
        */   
        uint32_t getFullAlmanacDateTime( void );

    private:
    
        bool get_almanac_crc( const void* ral_context, uint32_t* almanac_crc ); 

        bool almanac_update( const void* ral_context, uint8_t *almanac_data, uint16_t almanac_len );

    public:
 
};

extern WM1110_Almanac_Update wm1110_almanac_update;

#endif /* _WM1110_ALMANAC_UPDATE_H_ */



