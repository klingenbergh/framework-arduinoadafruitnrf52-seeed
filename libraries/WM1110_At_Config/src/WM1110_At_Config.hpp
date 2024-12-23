#ifndef _WM1110_AT_CONFIG_H_
#define _WM1110_AT_CONFIG_H_

#pragma once

#include <Arduino.h>

#include "WM1110_Cmd_String/WM1110_Cmd_String.h"
#include "WM1110_Param_Var/WM1110_Param_Var.h"
#include "WM1110_Cmd_List/WM1110_Cmd_List.h"
#include "WM1110_Cmd_Callback.h"



class WM1110_AT_Config
{
    private:
        static WM1110_AT_Config* instance_;

    public:
        WM1110_AT_Config(void);  

        static WM1110_AT_Config& getInstance();

        void begin( void );

        /*!
        *  @brief  Parse AT cmd
        *  @param cmd   
        *  @param length 
        */  
        void parseCmd(const char *cmd, uint16_t length);

        /*!
        *  @brief  Receive serial cmd
        *  @param buf   
        *  @param size 
        *  @return bool
        */  
        bool receiveSerialCmd(uint8_t *buf, uint8_t *size);

    protected:

};

extern WM1110_AT_Config wm1110_at_config;

#endif /* _WM1110_AT_CONFIG_H_ */
