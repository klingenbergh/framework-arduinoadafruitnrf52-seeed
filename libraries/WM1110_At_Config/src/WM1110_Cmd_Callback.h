#ifndef _WM1110_CMD_CALLBACK_H
#define _WM1110_CMD_CALLBACK_H 



#ifdef __cplusplus
extern "C" {
#endif


#include "WM1110_Cmd_List/WM1110_Cmd_List.h"

#include "WM1110_Param_Var/WM1110_Param_Var.h"

#include <stdio.h>



extern void BLE_TRACE( const char* fmt, ... );


#define AT_PRINTF(...)     do{  if(cmd_parse_type != 0)\
                                {\
                                    BLE_TRACE (  __VA_ARGS__ );\
                                }\
                                else\
                                {\
                                    printf (  __VA_ARGS__ );\
                                }\
                                hal_mcu_wait_ms(1);\
                            }while(0)
#define AT_PPRINTF(...)     do{  if(cmd_parse_type != 0)\
                                {\
                                    BLE_TRACE (  __VA_ARGS__ );\
                                }\
                                else\
                                {\
                                    printf (  __VA_ARGS__ );\
                                }\
                                 hal_mcu_wait_ms(1);\
                            }while(0)


/**
  * @brief  Return AT_OK in all cases
  * 
  * @param  param string of the AT command - unused
  * 
  * @retval AT_OK
  */
ATEerror_t AT_return_ok(const char *param);

/**
  * @brief  Return AT_ERROR in all cases
  * 
  * @param  param string of the AT command - unused
  * 
  * @retval AT_ERROR
  */
ATEerror_t AT_return_error(const char *param);

/**
  * @brief  Get default DevEui
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_DefEUI_get(const char *param);

/* --------------- General commands --------------- */

/**
  * @brief  Print the version of the AT_Slave FW
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_version_get(const char *param);

/**
  * @brief  Get device current paramenter
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_Config_get(const char *param);

/**
  * @brief  Get device default paramenter
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_DefParam_get(const char *param);

/**
  * @brief  Get device serial number
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_Sn_get(const char *param);

/**
  * @brief  Set device serial number
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_Sn_set(const char *param);

/**
  * @brief  Get hardware version
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_HARDWAREVER_get(const char *param);

/**
  * @brief  Restore the current parameters to default values
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_Parameter_Reset(const char *param); 

/**
  * @brief  Get position interval
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_POS_INT_get(const char *param);

/**
  * @brief  Set position interval
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_POS_INT_set(const char *param);

/**
  * @brief  Get sample interval
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_SAMPLE_INT_get(const char *param);

/**
  * @brief  Set sample interval
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_SAMPLE_INT_set(const char *param); 

/**
  * @brief  Get device type
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_SensorType_get(const char *param);

/**
  * @brief  Set lora join net platform
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_Platform_set(const char *param);
/**
  * @brief  Get lora join net platform
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_Platform_get(const char *param);

/**
  * @brief  Collect sensor data
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_MeasurementValue_get(const char *param);

/**
  * @brief  Delete history data
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_POS_DEL_run(const char *param);

/**
  * @brief  print position data 
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_POS_MSG_run(const char *param);

/* --------------- LoRaWAN network management commands --------------- */

/**
  * @brief  Print actual Active Type
  * 
  * @param  param string of the AT command
  * 
  * @retval AT_OK
  */
ATEerror_t AT_ActivationType_get(const char *param);

/**
  * @brief  Set Active Type
  * 
  * @param  param string of the AT command
  * 
  * @retval AT_OK if OK
  */
ATEerror_t AT_ActivationType_set(const char *param);

/**
  * @brief  Print actual Active Region
  * 
  * @param  param string of the AT command
  * 
  * @retval AT_OK
  */
ATEerror_t AT_Region_get(const char *param);

/**
  * @brief  Set Active Region
  * 
  * @param  param string of the AT command
  * 
  * @retval AT_OK if OK
  */
ATEerror_t AT_Region_set(const char *param);

/**
  * @brief  Print channel group
  * 
  * @param  param string of the AT command
  * 
  * @retval AT_OK
  */
ATEerror_t AT_ChannelGroup_get(const char *param);

/**
  * @brief  Set channel group
  * 
  * @param  param string of the AT command
  * 
  * @retval AT_OK if OK
  */
ATEerror_t AT_ChannelGroup_set(const char *param);

/**
  * @brief  Print Join Eui
  * 
  * @param  param string of the AT command
  * 
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_JoinEUI_get(const char *param);

/**
  * @brief  Set Join Eui
  * 
  * @param  param string of the AT command
  * 
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_JoinEUI_set(const char *param);

/**
  * @brief  Print Application Key
  * 
  * @param  param string of the AT command
  * 
  * @retval AT_OK
  */
ATEerror_t AT_AppKey_get(const char *param);

/**
  * @brief  Set Application Key
  * 
  * @param  param string of the AT command
  * 
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_AppKey_set(const char *param);

/**
  * @brief  Print Device EUI
  * 
  * @param  param string of the AT command
  * 
  * @retval AT_OK
  */
ATEerror_t AT_DevEUI_get(const char *param);

/**
  * @brief  Set Device EUI
  * 
  * @param  param string of the AT command
  * 
  * @retval AT_OK if OK
  */
ATEerror_t AT_DevEUI_set(const char *param);

/**
  * @brief  Print Network Key
  * 
  * @param  param string of the AT command
  * 
  * @retval AT_OK
  */
ATEerror_t AT_NwkKey_get(const char *param);

/**
  * @brief  Set Network Key
  * 
  * @param  param string of the AT command
  * 
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_NwkKey_set(const char *param);

/**
  * @brief  Print Network Session Key
  * 
  * @param  param string of the AT command
  * 
  * @retval AT_OK
  */
ATEerror_t AT_NwkSKey_get(const char *param);

/**
  * @brief  Set Network Session Key
  * 
  * @param  param String pointing to provided DevAddr
  * 
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_NwkSKey_set(const char *param);

/**
  * @brief  Print Application Session Key
  * 
  * @param  param string of the AT command
  * 
  * @retval AT_OK
  */
ATEerror_t AT_AppSKey_get(const char *param);

/**
  * @brief  Set Application Session Key
  * 
  * @param  param String pointing to provided DevAddr
  * 
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_AppSKey_set(const char *param);

/**
  * @brief  Set lora device key
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_DeviceKey_set(const char *param);

/**
  * @brief  Get lora device key
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_DeviceKey_get(const char *param);

/**
  * @brief  Print the DevAddr
  * 
  * @param  param String pointing to provided DevAddr
  * 
  * @retval AT_OK
  */
ATEerror_t AT_DevAddr_get(const char *param);

/**
  * @brief  Set DevAddr
  * 
  * @param  param String pointing to provided DevAddr
  * 
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_DevAddr_set(const char *param);

/**
  * @brief  Set device code
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_DevCODE_set(const char *param);
/**
  * @brief  Get device code
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_DevCODE_get(const char *param);

/* --------------- BLE connect commands --------------- */

/**
  * @brief  control ble disconnect 
  * 
  * @param  param String parameter
  * 
  * @retval AT_OK
  */
ATEerror_t AT_Disconnect(const char *param);




#ifdef __cplusplus
}
#endif
#endif