#include "WM1110_Cmd_Callback.h"

#include "lbm/smtc_modem_api/smtc_modem_api.h"
#include "common/apps_common_util/apps_common_util.h"



#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <WM1110_Storage.hpp>
#include <LbmWm1110.hpp>
#include <Lbmx.hpp>
#include <Lbm_Modem_Common.hpp>
#include <WM1110_BLE.hpp>
#include <Tracker_Peripheral.hpp>


#define tiny_sscanf sscanf 

#define SE_KEY_SIZE 16
#define SE_EUI_SIZE 8


void BLE_TRACE( const char* fmt, ... )
{
    va_list argp;
    va_start(argp, fmt);
    wm1110_ble.vprint(fmt, argp);
    va_end(argp);
}

/**
 * @brief  Print 4 bytes as %02x
 * 
 * @param  value containing the 4 bytes to print
 * 
 * @retval 
 */
static void print_uint32_as_02x(uint32_t value);

/**
 * @brief  Print 16 bytes as %02X
 * 
 * @param  pt pointer containing the 16 bytes to print
 * 
 * @retval 
 */
static void print_16_02x(uint8_t *pt);

/**
 * @brief  Print 8 bytes as %02X
 * 
 * @param  pt pointer containing the 8 bytes to print
 * 
 * @retval 
 */
static void print_8_02x(uint8_t *pt);

/**
 * @brief  Data analysis
 * 
 * @param  param 
 * 
 * @param  Buff 
 * 
 * @param  lenth 
 * 
 * @retval  int8_t
 */
static int8_t Data_Analysis(const char *param, uint8_t Buff[], uint8_t lenth) {
    uint8_t param_length = 0;
    int8_t result = 0;
    for (uint8_t j = 0; j < lenth; j++) {
        if (param_length > strlen(param)) {
            return -1;
        }
        result = ascii_4bit_to_hex(param[param_length++]);
        if (result == -1) {
            return -1;
        } else {
            Buff[j] = result << 4;
        }
        result = ascii_4bit_to_hex(param[param_length++]);
        if (result == -1) {
            return -1;
        } else {
            Buff[j] = Buff[j] | result;
        }
        param_length++; // The byte is a colon
    }
    return 0;
}

static void print_uint32_as_02x(uint32_t value) 
{
    AT_PRINTF("\"%02X:%02X:%02X:%02X\"",
        (unsigned)((unsigned char *)(&value))[3],
        (unsigned)((unsigned char *)(&value))[2],
        (unsigned)((unsigned char *)(&value))[1],
        (unsigned)((unsigned char *)(&value))[0]);
}

static void print_16_02x(uint8_t *pt) 
{
    AT_PRINTF("\"%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\"",
        pt[0], pt[1], pt[2], pt[3],
        pt[4], pt[5], pt[6], pt[7],
        pt[8], pt[9], pt[10], pt[11],
        pt[12], pt[13], pt[14], pt[15]);
}

static void print_8_02x(uint8_t *pt) 
{
    AT_PRINTF("\"%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\"",
        pt[0], pt[1], pt[2], pt[3], pt[4], pt[5], pt[6], pt[7]);
}


ATEerror_t AT_return_ok(const char *param) 
{
   return AT_OK;
}

ATEerror_t AT_return_error(const char *param) 
{
    return AT_ERROR;
}

ATEerror_t AT_DefEUI_get(const char *param) {
    uint8_t def_eui[8]={0};
    memset(def_eui,0,8);
    if(!wm1110_storage.getDefaultDevEui(def_eui))
    {
         return AT_READ_ERROR;
    }
    print_8_02x(def_eui); 
    return AT_OK;
}

/* --------------- General commands --------------- */

/*------------------------AT+VER=?\r\n-------------------------------------*/

ATEerror_t AT_version_get(const char *param) {
    smtc_modem_return_code_t modem_response_code = SMTC_MODEM_RC_OK;
    smtc_modem_lorawan_version_t lorawan_version;
    AT_PRINTF("\r\n{\r\n\t\"hw_ver\": \"V%d.%d\",", app_param.hardware_info.hw_ver&0xf0,app_param.hardware_info.hw_ver&0x0f);
    modem_response_code = smtc_modem_get_lorawan_version( &lorawan_version );
    if( modem_response_code == SMTC_MODEM_RC_OK )
    {
        AT_PRINTF("\r\n\t\"LoRaWAN\": \"V%X.%X.%X\"\r\n}", lorawan_version.major, lorawan_version.minor, lorawan_version.patch);
    }
    return AT_OK;
}
/*------------------------AT+VER=?\r\n-------------------------------------*/

/*------------------------AT+CONFIG=?\r\n -------------------------------------*/
ATEerror_t AT_Config_get(const char *param) {
    
    wm1110_storage.readCurrentParameters(&app_param);
    AT_PRINTF("\r\n{\r\n");    

    AT_PRINTF("\t\"devMdl\": ");
    if (AT_SensorType_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF(",\r\n\t\"deviceEui\": ");
    if (AT_DevEUI_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF(",\r\n\t\"defEui\": ");
    if (AT_DefEUI_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF(",\r\n\t\"appEui\": ");
    if (AT_JoinEUI_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF(",\r\n\t\"frequency\": ");
    if (AT_Region_get(param) != AT_OK) {
        return AT_ERROR;
    }
    AT_PRINTF(",\r\n\t\"subBand\": ");
    if (AT_ChannelGroup_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF(",\r\n\t\"joinType\": ");
    if (AT_ActivationType_get(param) != AT_OK) {
        return AT_ERROR;
    }
    
    AT_PRINTF(",\r\n\t\"appKey\": ");
    if (AT_AppKey_get(param) != AT_OK) {
        return AT_ERROR;
    }
    
    AT_PRINTF(",\r\n\t\"nwkSkey\": ");
    if (AT_NwkSKey_get(param) != AT_OK) {
        return AT_ERROR;
    }
    
    AT_PRINTF(",\r\n\t\"appSkey\": ");
    if (AT_AppSKey_get(param) != AT_OK) {
        return AT_ERROR;
    }
    
    AT_PRINTF(",\r\n\t\"devAddr\": ");
    if (AT_DevAddr_get(param) != AT_OK) {
        return AT_ERROR;
    }
    
    AT_PRINTF(",\r\n\t\"devCode\": ");
    if (AT_DevCODE_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF(",\r\n\t\"platform\": ");
    if (AT_Platform_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF(",\r\n\t\"devKey\": ");
    if (AT_DeviceKey_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF(",\r\n\t\"posInt\": ");
    if (AT_POS_INT_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF(",\r\n\t\"sampleInt\": ");
    if (AT_SAMPLE_INT_get(param) != AT_OK) {
        return AT_ERROR;
    }
    
    AT_PRINTF("\r\n}");
    return AT_OK;
}
/*------------------------AT+CONFIG=?\r\n -------------------------------------*/

/*------------------------AT+AT_DEFPARAM=?\r\n -------------------------------------*/
ATEerror_t AT_DefParam_get(const char *param) {
    
    wm1110_storage.readDefaultParameters(&app_param);
    AT_PRINTF("\r\n{\r\n");    

    AT_PRINTF("\t\"devMdl\": ");
    if (AT_SensorType_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF(",\r\n\t\"deviceEui\": ");
    if (AT_DevEUI_get(param) != AT_OK) {
        return AT_ERROR;
    }
    
    AT_PRINTF(",\r\n\t\"defEui\": ");
    if (AT_DefEUI_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF(",\r\n\t\"appEui\": ");
    if (AT_JoinEUI_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF(",\r\n\t\"frequency\": ");
    if (AT_Region_get(param) != AT_OK) {
        return AT_ERROR;
    }
    AT_PRINTF(",\r\n\t\"subBand\": ");
    if (AT_ChannelGroup_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF(",\r\n\t\"joinType\": ");
    if (AT_ActivationType_get(param) != AT_OK) {
        return AT_ERROR;
    }
    
    AT_PRINTF(",\r\n\t\"appKey\": ");
    if (AT_AppKey_get(param) != AT_OK) {
        return AT_ERROR;
    }
    
    AT_PRINTF(",\r\n\t\"nwkSkey\": ");
    if (AT_NwkSKey_get(param) != AT_OK) {
        return AT_ERROR;
    }
    
    AT_PRINTF(",\r\n\t\"appSkey\": ");
    if (AT_AppSKey_get(param) != AT_OK) {
        return AT_ERROR;
    }
    
    AT_PRINTF(",\r\n\t\"devAddr\": ");
    if (AT_DevAddr_get(param) != AT_OK) {
        return AT_ERROR;
    }
    
    AT_PRINTF(",\r\n\t\"devCode\": ");
    if (AT_DevCODE_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF(",\r\n\t\"platform\": ");
    if (AT_Platform_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF(",\r\n\t\"devKey\": ");
    if (AT_DeviceKey_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF(",\r\n\t\"posInt\": ");
    if (AT_POS_INT_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF(",\r\n\t\"sampleInt\": ");
    if (AT_SAMPLE_INT_get(param) != AT_OK) {
        return AT_ERROR;
    }

    AT_PRINTF("\r\n}");
    wm1110_storage.readCurrentParameters(&app_param);
    return AT_OK;
}
/*------------------------AT+AT_DEFPARAM=?\r\n -------------------------------------*/

/*------------------------AT+SN=?\r\n-------------------------------------*/
ATEerror_t AT_Sn_get(const char *param) {

    AT_PRINTF("\"%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\"", app_param.hardware_info.Sn[0], app_param.hardware_info.Sn[1],
        app_param.hardware_info.Sn[2], app_param.hardware_info.Sn[3],
        app_param.hardware_info.Sn[4], app_param.hardware_info.Sn[5],
        app_param.hardware_info.Sn[6], app_param.hardware_info.Sn[7],
        app_param.hardware_info.Sn[8]);


    return AT_OK;
}

ATEerror_t AT_Sn_set(const char *param) {
    uint8_t sn[9];

    if ((Data_Analysis(param, sn, 9) == -1) || (strlen(param) != 28)) {
        return AT_PARAM_ERROR;
    }
    if(memcmp(&app_param.hardware_info.Sn[0], sn, 9) != 0)
    {
        memcpy((uint8_t *)app_param.hardware_info.Sn, sn, 9);
    }
    return AT_OK;
}
/*------------------------AT+SN=?\r\n-------------------------------------*/

/*------------------------AT+HARDWAREVER=?\r\n-------------------------------------*/
ATEerror_t AT_HARDWAREVER_get(const char *param)
{    
    AT_PRINTF("%d.%d",app_param.hardware_info.hw_ver&0xf0,app_param.hardware_info.hw_ver&0x0f);
    return AT_OK; 
}
/*------------------------AT+HARDWAREVER=?\r\n-------------------------------------*/

/*------------------------AT+RESTORE\r\n-------------------------------------*/
ATEerror_t AT_Parameter_Reset(const char *param) 
{
    if (!wm1110_storage.restoreToFactoryParameters()) 
    {
        return AT_SAVE_FAILED;
    } 
    else 
    {
        app_append_param.position_interval = 5;
        app_append_param.sample_interval = 5;
        app_append_param.cache_en = true;
        app_append_param.Platform = IOT_PLATFORM_SENSECAP_TTN;
        app_append_param.Retry = 1;
        if(!wm1110_storage.readAppendParameters())
        {
            return AT_SAVE_FAILED;
        }
        return AT_OK;
    }
    return AT_OK;    
}
/*------------------------AT+RESTORE\r\n-------------------------------------*/

/*------------------------AT+POS_INT\r\n-------------------------------------*/
ATEerror_t AT_POS_INT_get(const char *param) 
{
    AT_PRINTF("%d",app_append_param.position_interval);
    return AT_OK;    
}

ATEerror_t AT_POS_INT_set(const char *param) 
{
    uint16_t pos_interval = 0;
    if (tiny_sscanf(param, "%hu", &pos_interval) != 1) {
        return AT_PARAM_ERROR;
    }
    if((pos_interval < 5)||(pos_interval > 1440))
    {
        return AT_PARAM_ERROR;    
    }
    if(pos_interval != app_append_param.position_interval)
    {
        app_append_param.position_interval = pos_interval;
        if(!wm1110_storage.readAppendParameters())
        {
            return AT_SAVE_FAILED;
        }
    }
    return AT_OK;    
}
/*------------------------AT+POS_INT\r\n-------------------------------------*/

/*------------------------AT+SAMPLE_INT\r\n-------------------------------------*/
ATEerror_t AT_SAMPLE_INT_get(const char *param) 
{
    AT_PRINTF("%d",app_append_param.sample_interval);
    return AT_OK;  
}

ATEerror_t AT_SAMPLE_INT_set(const char *param) 
{
    uint16_t sample_interval = 0;
    if (tiny_sscanf(param, "%hu", &sample_interval) != 1) {
        return AT_PARAM_ERROR;
    }
    if((sample_interval < 5)||(sample_interval > 1440))
    {
        return AT_PARAM_ERROR;    
    }
    if(sample_interval != app_append_param.sample_interval)
    {
        app_append_param.sample_interval = sample_interval;
        if(!wm1110_storage.readAppendParameters())
        {
            return AT_SAVE_FAILED;
        }
    }
    return AT_OK;     
}
/*------------------------AT+SAMPLE_INT\r\n-------------------------------------*/

/*------------------------AT+SENSOR=?\r\n -------------------------------------*/
ATEerror_t AT_SensorType_get(const char *param) {

    AT_PRINTF("%s", "\"Wio Tracker 1110\"");
    return AT_OK;
}
/*------------------------AT+SENSOR=?\r\n -------------------------------------*/

/*------------------------AT+PLATFORM=?\r\n-------------------------------------*/
ATEerror_t AT_Platform_get(const char *param) {
    AT_PRINTF("%d", app_append_param.Platform);

    return AT_OK;
}

ATEerror_t AT_Platform_set(const char *param) {

    uint16_t platform = 0;
    if (tiny_sscanf(param, "%hu", &platform) != 1) {
        return AT_PARAM_ERROR;
    }
    
    if (platform < IOT_PLATFORM_MAX) 
    {
        if(app_append_param.Platform != platform)
        {
            app_append_param.Platform = (platform_t)platform;
            if(!wm1110_storage.readAppendParameters())
            {
                return AT_SAVE_FAILED;
            }            
        }
        return AT_OK;
    } 
    else 
    {
        return AT_PARAM_ERROR;
    }
    return AT_OK;
}
/*------------------------AT+PLATFORM=?\r\n-------------------------------------*/

/*------------------------AT+MEA=?\r\n-------------------------------------*/
ATEerror_t AT_MeasurementValue_get(const char *param) {   
   
    int16_t temperture_temp = 0;
    uint8_t humidity_temp = 0;
    int16_t acc_x_temp = 0;
    int16_t acc_y_temp = 0;
    int16_t acc_z_temp = 0;
    float x,y,z,temperture,humid;
    if(tracker_peripheral.measureLIS3DHTRDatas(&x,&y,&z))
    {
        acc_x_temp = 100*x*9.80;
        acc_y_temp = 100*y*9.80;
        acc_z_temp = 100*z*9.80;            
    }
    if(tracker_peripheral.measureSHT4xDatas(&temperture,&humid))
    {
        temperture_temp = 10*temperture;
        humidity_temp = humid;        
    } 
    // single_fact_sensor_data_get(lis3dhtr_sensor_type);     
    // single_fact_sensor_data_get(sht4x_sensor_type);
    // temperture_temp = 10*sht4x_temperature;
    // humidity_temp = sht4x_humidity;
    // acc_x_temp = 100*lis3dhtr_x*9.80;
    // acc_y_temp = 100*lis3dhtr_y*9.80;
    // acc_z_temp = 100*lis3dhtr_z*9.80;    
    AT_PRINTF("\r\n{\r\n\t\"4097\":\t%d",(temperture_temp*100));   
    AT_PRINTF(",\r\n\t\"4098\":\t%d",humidity_temp*1000); 
    AT_PRINTF(",\r\n\t\"4150\":\t%d",acc_x_temp*10); 
    AT_PRINTF(",\r\n\t\"4151\":\t%d",acc_y_temp*10); 
    AT_PRINTF(",\r\n\t\"4152\":\t%d\r\n}\r\n",acc_z_temp*10); 

    return AT_OK;    
}
/*------------------------AT+MEA=?\r\n-------------------------------------*/

/*------------------------AT+POS_DEL\r\n-------------------------------------*/
ATEerror_t AT_POS_DEL_run(const char *param)
{
    uint16_t del_cnt = 0;
    del_cnt = wm1110_storage.getPositionDataCount();
    if(del_cnt > 0)
    {
        if(0 > wm1110_storage.deletePositionData(del_cnt) )
        {
            return AT_DELETE_ERROR;
        }
    }
    return AT_OK;
}
/*------------------------AT+POS_DEL\r\n-------------------------------------*/

/*------------------------AT+POS_MSG\r\n-------------------------------------*/
ATEerror_t AT_POS_MSG_run(const char *param)
{
    pos_msg_param_t pos_data_temp;
    uint16_t del_cnt = 0;
    uint8_t pos_data_buf[64] = {0};
    uint8_t pos_data_len = 0;  
    del_cnt = wm1110_storage.getPositionDataCount();
    if(del_cnt > 0)
    {
        for(uint16_t u16i = 1; u16i <= del_cnt; u16i ++)
        {
            wm1110_storage.readSinglePositionData(&pos_data_temp, u16i);
            wm1110_storage.convertPositionDatasToBuf(pos_data_temp, pos_data_buf, &pos_data_len);
            for(uint8_t u8j = 0; u8j < pos_data_len; u8j++)
            {
                AT_PRINTF("%2x",pos_data_buf[u8j]);
            }
            AT_PRINTF("\r\n");
        }
    }
    return AT_OK;
}
/*------------------------AT+POS_MSG\r\n-------------------------------------*/

/* ------------------------LoRaWAN network management commands ------------------------*/

/*------------------------AT+TYPE=?\r\n-------------------------------------*/
ATEerror_t AT_ActivationType_get(const char *param) {
//    const char *activationTypeStrings[] = {"NONE", "ABP", "OTAA"};

    if (app_param.lora_info.ActivationType > ACTIVATION_TYPE_OTAA) {
        return AT_PARAM_ERROR;
    }
    AT_PRINTF("%d", app_param.lora_info.ActivationType);
    return AT_OK;
}

ATEerror_t AT_ActivationType_set(const char *param) {
    uint16_t activetype;
    if (tiny_sscanf(param, "%hu", &activetype) != 1) {
        return AT_PARAM_ERROR;
    }
    if (activetype > ACTIVATION_TYPE_OTAA) {
        return AT_PARAM_ERROR;
    }
    if(app_param.lora_info.ActivationType != activetype)
    {
        app_param.lora_info.ActivationType = activetype == ACTIVATION_TYPE_OTAA ? ACTIVATION_TYPE_OTAA : ACTIVATION_TYPE_ABP;
    }
    return AT_OK;
}
/*------------------------AT+TYPE=?\r\n -------------------------------------*/

/*------------------------AT+BAND=?\r\n-------------------------------------*/
ATEerror_t AT_Region_get(const char *param) {

#ifdef LORAWAN_APP_PARAM_ENABLE
    //const char *regionStrings[] = {"AS923", "AU915", "CN470", "CN779", "EU433", "EU868", "KR920", "IN865", "US915", "RU864", "AS923_1", "AS923_2", "AS923_3", "AS923_4"};
    if (app_param.lora_info.ActiveRegion > LORAMAC_REGION_AS_GP_4 && app_param.lora_info.ActiveRegion != 0xFF) {
        return AT_PARAM_ERROR;
    }
#else
    // const char *regionStrings[] = { "AS923", "AU915", "CN470", "CN779", "EU433", "EU868", "KR920", "IN865", "US915", "RU864" };
    if (app_param.lora_info.ActiveRegion > LORAMAC_REGION_RU864) {
        return AT_PARAM_ERROR;
    }
#endif

    AT_PRINTF("%d", app_param.lora_info.ActiveRegion);

    return AT_OK;
}

ATEerror_t AT_Region_set(const char *param) {

    uint16_t region;
    if (tiny_sscanf(param, "%hu", &region) != 1) {
        return AT_PARAM_ERROR;
    }
    if (region >= 256) {
        return AT_PARAM_ERROR;
    }
#ifdef LORAWAN_APP_PARAM_ENABLE
    if (((region > LORAMAC_REGION_AS_GP_4)&& (region != LORAMAC_REGION_MAX)) || (region == LORAMAC_REGION_CN470) || (region == LORAMAC_REGION_CN779) || (region == LORAMAC_REGION_EU433)) {
        return AT_PARAM_ERROR;
    }
#else
    if (((region > LORAMAC_REGION_RU864)&& (region != LORAMAC_REGION_MAX)) || (region == LORAMAC_REGION_CN470) || (region == LORAMAC_REGION_CN779) || (region == LORAMAC_REGION_EU433)) {
        return AT_PARAM_ERROR;
    }
#endif
    if(app_param.lora_info.ActiveRegion != region)
    {
        app_param.lora_info.ActiveRegion = region;
    }
    return AT_OK;
}
/*------------------------AT+BAND=?\r\n-------------------------------------*/

/*------------------------AT+CHANNEL=?\r\n -------------------------------------*/
ATEerror_t AT_ChannelGroup_get(const char *param) {
    AT_PRINTF("%d", app_param.lora_info.ChannelGroup);
    return AT_OK;
}

ATEerror_t AT_ChannelGroup_set(const char *param) {

    uint16_t channelgroup = 0;
    if (tiny_sscanf(param, "%hu", &channelgroup) != 1) {
        return AT_PARAM_ERROR;
    }
    if ((channelgroup <= 7) && ((app_param.lora_info.ActiveRegion == LORAMAC_REGION_US915) || (app_param.lora_info.ActiveRegion == LORAMAC_REGION_AU915))) {
        if(app_param.lora_info.ChannelGroup != channelgroup)
        {
            app_param.lora_info.ChannelGroup = channelgroup;
        }
        return AT_OK;
    } 
    else 
    {
        return AT_PARAM_ERROR;
    }
    return AT_OK;
}
/*------------------------AT+CHANNEL=?\r\n -------------------------------------*/

/*------------------------AT+APPEUI=?\r\n-------------------------------------*/
ATEerror_t AT_JoinEUI_get(const char *param) {
    print_8_02x(app_param.lora_info.JoinEui);
    return AT_OK;
}

ATEerror_t AT_JoinEUI_set(const char *param) {
    uint8_t joineui[8];
    
    if ((Data_Analysis(param, joineui, 8) == -1) || (strlen(param) != 25)) {
        return AT_PARAM_ERROR;
    }
    if(memcmp(&app_param.lora_info.JoinEui[0],joineui,SE_EUI_SIZE) != 0)
    {
        memcpy((uint8_t *)app_param.lora_info.JoinEui, joineui, SE_EUI_SIZE);
    }
    return AT_OK;
}
/*------------------------AT+APPEUI=?\r\n-------------------------------------*/

/*------------------------AT+APPKEY=?\r\n-------------------------------------*/
ATEerror_t AT_AppKey_get(const char *param) {
    print_16_02x(app_param.lora_info.AppKey);

    return AT_OK;
}

ATEerror_t AT_AppKey_set(const char *param) {

    uint8_t appKey[16];
    if ((Data_Analysis(param, appKey, 16) == -1) || (strlen(param) != 49)) {
        return AT_PARAM_ERROR;
    }
    if(memcmp(&app_param.lora_info.AppKey[0], appKey, SE_KEY_SIZE) != 0)
    {
        memcpy((uint8_t *)app_param.lora_info.AppKey, appKey, SE_KEY_SIZE);
    }
    return AT_OK;
}
/*------------------------AT+APPKEY=?\r\n-------------------------------------*/

/*------------------------AT+DEUI=?\r\n-------------------------------------*/
ATEerror_t AT_DevEUI_get(const char *param) {
    print_8_02x(app_param.lora_info.DevEui);
    return AT_OK;
}
ATEerror_t AT_DevEUI_set(const char *param) {
    uint8_t devEui[8];

    if ((Data_Analysis(param, devEui, 8) == -1) || (strlen(param) != 25)) {
        return AT_PARAM_ERROR;
    }
    if(memcmp(&app_param.lora_info.DevEui[0], devEui, SE_EUI_SIZE)!= 0)
    {
        memcpy((uint8_t *)app_param.lora_info.DevEui, devEui, SE_EUI_SIZE);
    }
    return AT_OK;
}
/*------------------------AT+DEUI=?\r\n-------------------------------------*/

/*------------------------AT+NWKKEY=?\r\n-------------------------------------*/
ATEerror_t AT_NwkKey_get(const char *param) {
    print_16_02x(app_param.lora_info.NwkKey);

    return AT_OK;
}

ATEerror_t AT_NwkKey_set(const char *param) {
    uint8_t nwkKey[16];

    if ((Data_Analysis(param, nwkKey, 16) == -1) || (strlen(param) != 49)) {
        return AT_PARAM_ERROR;
    }
    if(memcmp(&app_param.lora_info.NwkKey[0],nwkKey,SE_KEY_SIZE) != 0)
    {
        memcpy((uint8_t *)app_param.lora_info.NwkKey, nwkKey, SE_KEY_SIZE);
    }
    return AT_OK;
}
/*------------------------AT+NWKKEY=?\r\n-------------------------------------*/

/*------------------------AT+NWKSKEY=?\r\n-------------------------------------*/
ATEerror_t AT_NwkSKey_get(const char *param) {
    print_16_02x(app_param.lora_info.NwkSKey);
    return AT_OK;
}

ATEerror_t AT_NwkSKey_set(const char *param) {
    uint8_t nwkSKey[16];

    if ((Data_Analysis(param, nwkSKey, 16) == -1) || (strlen(param) != 49)) {
        return AT_PARAM_ERROR;
    }
    if(memcmp(&app_param.lora_info.NwkSKey[0], nwkSKey, 16) != 0)
    {
        memcpy((uint8_t *)app_param.lora_info.NwkSKey, nwkSKey, 16);
    }
    return AT_OK;
}
/*------------------------AT+NWKSKEY=?\r\n-------------------------------------*/

/*------------------------AT+APPSKEY=?\r\n -------------------------------------*/
ATEerror_t AT_AppSKey_get(const char *param) {
    print_16_02x(app_param.lora_info.AppSKey);

    return AT_OK;
}

ATEerror_t AT_AppSKey_set(const char *param) {

    uint8_t appskey[16];

    if ((Data_Analysis(param, appskey, 16) == -1) || (strlen(param) != 49)) {
        return AT_PARAM_ERROR;
    }
    if(memcmp(&app_param.lora_info.AppSKey[0], appskey, SE_KEY_SIZE) != 0)
    {
        memcpy((uint8_t *)app_param.lora_info.AppSKey, appskey, SE_KEY_SIZE);
    }
    return AT_OK;
}
/*------------------------AT+APPSKEY=?\r\n -------------------------------------*/

/*------------------------AT+DKEY=?\r\n-------------------------------------*/
ATEerror_t AT_DeviceKey_get(const char *param) {
    print_16_02x(app_param.lora_info.DeviceKey);
    return AT_OK;
}

ATEerror_t AT_DeviceKey_set(const char *param) 
{
    uint8_t deviceKey[16];
    if ((Data_Analysis(param, deviceKey, 16) == -1) || (strlen(param) != 49)) {
        return AT_PARAM_ERROR;
    }
    if(memcmp(&app_param.lora_info.DeviceKey[0], deviceKey, SE_KEY_SIZE) != 0)
    {
        memcpy((uint8_t *)app_param.lora_info.DeviceKey, deviceKey, SE_KEY_SIZE);
    }
    return AT_OK;
}
/*------------------------AT+DKEY=?\r\n-------------------------------------*/

/*------------------------AT+DADDR=?\r\n -------------------------------------*/
ATEerror_t AT_DevAddr_get(const char *param) {
    print_uint32_as_02x(app_param.lora_info.DevAddr);

    return AT_OK;
}

ATEerror_t AT_DevAddr_set(const char *param) {

    uint32_t devAddr;
    uint8_t devAddrtmp[4];
    if ((Data_Analysis(param, devAddrtmp, 4) == -1) || (strlen(param) != 13)) {
        return AT_PARAM_ERROR;
    }

    devAddr = (devAddrtmp[0] << 24) | (devAddrtmp[1] << 16) | (devAddrtmp[2] << 8) | devAddrtmp[3];
    if(app_param.lora_info.DevAddr != devAddr)
    {
        app_param.lora_info.DevAddr = devAddr;
    }
    return AT_OK;
}
/*------------------------AT+DADDR=?\r\n -------------------------------------*/

/*------------------------AT+DCODE=?\r\n-------------------------------------*/
ATEerror_t AT_DevCODE_get(const char *param) {
    print_8_02x(app_param.lora_info.DeviceCode);

    return AT_OK;
}

ATEerror_t AT_DevCODE_set(const char *param) {

    uint8_t devCode[8];

    if ((Data_Analysis(param, devCode, 8) == -1) || (strlen(param) != 25)) {
        return AT_PARAM_ERROR;
    }
    if(memcmp(&app_param.lora_info.DeviceCode[0], devCode, SE_EUI_SIZE) != 0)
    {
        memcpy((uint8_t *)app_param.lora_info.DeviceCode, devCode, SE_EUI_SIZE);
    }

    return AT_OK;
}
/*------------------------AT+DCODE=?\r\n-------------------------------------*/

/* ------------------------ BLE connect commands ------------------------ */

/*------------------------AT+DISCONNECT\r\n-------------------------------------*/
ATEerror_t AT_Disconnect(const char *param) 
{
    wm1110_ble.controlDisconnectBle();
    return AT_OK;
}
/*------------------------AT+DISCONNECT\r\n-------------------------------------*/










