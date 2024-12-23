#include "WM1110_Storage.hpp"


#include <Adafruit_LittleFS.h>
#include <ExternalFileSystem.h>

#include <LbmWm1110.hpp>
#include <Lbmx.hpp>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <Lbm_Modem_Common.hpp>
#include "internal/nrf_hal/system.hpp"
#include "internal/nrf_hal/trace.hpp"
#include "internal/Wm1110Hardware.hpp"
#include <WM1110_Geolocation.hpp>


using namespace Adafruit_LittleFS_Namespace;

WM1110_Storage* WM1110_Storage::instance_ = nullptr;

/*------------------------------------------------------------------*/
/* INTERNAL FUNCTION
 *------------------------------------------------------------------*/

static char     s_key_char[10] = {0};
#define KEY_VALUE_FDS_VERIFY(ret)     \
do                                    \
{                                     \
    if (ret)                          \
    {                                 \
        return ret;                   \
    }                                 \
} while(0)

static char * key_value_fds_int_key_convert(uint32_t key)
{
    uint16_t digit = 0;
    uint32_t temp = key;

    do
    {
        temp /= 10;
        digit++;
    } while (temp > 0);

    s_key_char[digit] = '\0';

    do {
        digit--;
        s_key_char[digit] = key % 10 + '0';
        key /= 10;
    } while (key > 0);

    return s_key_char;
}

static uint8_t crc8_poly(uint8_t *data, uint8_t data_len)
{
	uint8_t data_in;
	uint8_t i;
	uint8_t crc = 0x00;//CRC
	uint8_t crc_poly = 0x07;//CRC Poly
	while (data_len--)
	{
		data_in = *data++;
		crc = crc ^ data_in;//CRC
		for (i = 0; i < 8; i++)
		{
			if (crc & 0x80)//
			{
				crc = (crc << 1) ^ crc_poly;
			}
			else //
			{
				crc = crc << 1;
			}
		}
	}
	
	return crc ^0x00;
}

#if 1	//public
WM1110_Storage& WM1110_Storage::getInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = new WM1110_Storage();
    }

    return *instance_;
}

WM1110_Storage::WM1110_Storage(void)
{

}

int WM1110_Storage::begin(void)
{
    if(ExternalFS.begin())
    {
        s_op_instance_ptr = ExternalFS._getFS();  

        return KEY_VALUE_FDS_ERR_OK;
    }
    return KEY_VALUE_FDS_ERR_IO;    
}

int WM1110_Storage::writeStorageKeyValue(uint32_t key,const void *p_value,uint32_t length)
{
    int ret;

    lfs_file_t              file_id;
    struct lfs_file_config  file_config = {0};

    file_config.buffer = s_file_buffer;
 

    ret = lfs_file_opencfg(s_op_instance_ptr, &file_id, key_value_fds_int_key_convert(key), LFS_O_RDWR | LFS_O_CREAT, &file_config);
    KEY_VALUE_FDS_VERIFY(ret);

    ret = lfs_file_rewind(s_op_instance_ptr, &file_id);
    if (ret)
    {
        lfs_file_close(s_op_instance_ptr, &file_id);
        return ret;
    }

    ret = lfs_file_write(s_op_instance_ptr, &file_id, p_value, length);
    if (ret)
    {
        lfs_file_close(s_op_instance_ptr, &file_id);
        return ret;
    }

    ret = lfs_file_close(s_op_instance_ptr, &file_id);
    KEY_VALUE_FDS_VERIFY(ret);

    return KEY_VALUE_FDS_ERR_OK;
}

int WM1110_Storage::readStorageKeyValue(uint32_t key,void *p_buffer,uint32_t length)
{
    int ret;

    lfs_file_t              file_id;
    struct lfs_file_config  file_config = {0};

    file_config.buffer = s_file_buffer;
 
    ret = lfs_file_opencfg(s_op_instance_ptr, &file_id, key_value_fds_int_key_convert(key), LFS_O_RDONLY, &file_config);
    KEY_VALUE_FDS_VERIFY(ret);

    ret = lfs_file_rewind(s_op_instance_ptr, &file_id);
    if (ret)
    {
        lfs_file_close(s_op_instance_ptr, &file_id);
        return ret;
    }

    ret = lfs_file_read(s_op_instance_ptr, &file_id, p_buffer, length);
    if (ret)
    {
        lfs_file_close(s_op_instance_ptr, &file_id);
        return ret;
    }

    ret = lfs_file_close(s_op_instance_ptr, &file_id);
    KEY_VALUE_FDS_VERIFY(ret);

    return KEY_VALUE_FDS_ERR_OK;
}

int WM1110_Storage::deleteStorageKeyValue(uint32_t key)
{
    return lfs_remove(s_op_instance_ptr, key_value_fds_int_key_convert(key));
}

int WM1110_Storage::getStorageKeyValueSize(uint32_t key)
{
    int ret;
    lfs_file_t              file_id;
    struct lfs_file_config  file_config = {0};
    file_config.buffer = s_file_buffer;

    ret = lfs_file_opencfg(s_op_instance_ptr, &file_id, key_value_fds_int_key_convert(key), LFS_O_RDONLY, &file_config);
    KEY_VALUE_FDS_VERIFY(ret);

    ret = lfs_file_size(s_op_instance_ptr, &file_id);
    lfs_file_close(s_op_instance_ptr, &file_id);

    return ret;
}

int WM1110_Storage::writeStorageKeyPosValue(uint32_t key,const void *p_value,uint32_t length,int32_t pos)
{
    int ret;

    lfs_file_t              file_id;
    struct lfs_file_config  file_config = {0};

    file_config.buffer = s_file_buffer;
 
    ret = lfs_file_opencfg(s_op_instance_ptr, &file_id, key_value_fds_int_key_convert(key), LFS_O_RDWR | LFS_O_CREAT, &file_config);
    KEY_VALUE_FDS_VERIFY(ret);
    
    ret = lfs_file_seek(s_op_instance_ptr, &file_id, pos, LFS_SEEK_SET);
    if (ret < 0)
    {
        lfs_file_close(s_op_instance_ptr, &file_id);
        return ret;
    }

    ret = lfs_file_write(s_op_instance_ptr, &file_id, p_value, length);
    if (ret)
    {
        lfs_file_close(s_op_instance_ptr, &file_id);
        return ret;
    }

    ret = lfs_file_close(s_op_instance_ptr, &file_id);
    KEY_VALUE_FDS_VERIFY(ret);

    return KEY_VALUE_FDS_ERR_OK;
}

int WM1110_Storage::readStorageKeyPosValue(uint32_t key,void *p_buffer,uint32_t length,int32_t pos)
{
    int ret;

    lfs_file_t              file_id;
    struct lfs_file_config  file_config = {0};

    file_config.buffer = s_file_buffer;
 
    ret = lfs_file_opencfg(s_op_instance_ptr, &file_id, key_value_fds_int_key_convert(key), LFS_O_RDONLY, &file_config);
    KEY_VALUE_FDS_VERIFY(ret);
    
    ret = lfs_file_seek(s_op_instance_ptr, &file_id, pos, LFS_SEEK_SET);
    if (ret < 0)
    {
        lfs_file_close(s_op_instance_ptr, &file_id);
        return ret;
    }

    ret = lfs_file_read(s_op_instance_ptr, &file_id, p_buffer, length);
    if (ret)
    {
        lfs_file_close(s_op_instance_ptr, &file_id);
        return ret;
    }

    ret = lfs_file_close(s_op_instance_ptr, &file_id);
    KEY_VALUE_FDS_VERIFY(ret);

    return KEY_VALUE_FDS_ERR_OK;
}
#endif


#if 1	//private
uint8_t WM1110_Storage::readLfsFile(uint16_t file_name, uint8_t *data, uint16_t *len)
{
    int ret = 0;
    ret = readStorageKeyValue(file_name, data, *len );
    if( ret >= 0 )
    {
        // *len = ret;
        return 0;    
    }
    WM1110_STORAGE_DBG_PRINTF("readLfsFile error,ret:%02x\r\n",ret);
    return ret;
}

uint8_t WM1110_Storage::writeLfsFile(uint16_t file_name, uint8_t *data, uint16_t len)
{
    int ret = 0;
    ret = writeStorageKeyValue( file_name, data, len );
    if( ret >= 0 )
    {
        return 0;    
    }
    WM1110_STORAGE_DBG_PRINTF("key_value_fds_value_write error,ret:%02x\r\n",ret);
    return ret;
}

uint8_t WM1110_Storage::readLfsFileOne( uint16_t file_name, uint8_t *data, uint16_t *len )
{
    int ret = 0xff;
    uint16_t file_name_start = 0;
    uint32_t file_name_offset = 0;

    if( file_name >= pos_file_id_start && file_name <= pos_file_id_end )
    {
        file_name_start = pos_file_id_start + ( file_name - pos_file_id_start ) / pos_save_in_file_max;
        file_name_offset = (( file_name - pos_file_id_start ) % pos_save_in_file_max ) * ( *len );
    }
    else
    {
        return ret;
    }

    ret = readStorageKeyPosValue( file_name_start, data, *len, file_name_offset );
    if( ret >= 0 )
    {
        return 0;    
    }
    WM1110_STORAGE_DBG_PRINTF("readLfsFileOne error,ret:%02x\r\n",ret);
    return ret;
}

uint8_t WM1110_Storage::writeLfsFileOne( uint16_t file_name, uint8_t *data, uint16_t len )
{
    int ret = 0xff;
    uint16_t file_name_start = 0;
    uint32_t file_name_offset = 0;

    if( file_name >= pos_file_id_start && file_name <= pos_file_id_end )
    {
        file_name_start = pos_file_id_start + ( file_name - pos_file_id_start ) / pos_save_in_file_max;
        file_name_offset = (( file_name - pos_file_id_start ) % pos_save_in_file_max ) * ( len );
    }
    else
    {
        return ret;
    }

    ret = writeStorageKeyPosValue( file_name_start, data, len, file_name_offset );
    if( ret >= 0 )
    {
        return 0;    
    }
    WM1110_STORAGE_DBG_PRINTF("writeLfsFileOne error,ret:%02x\r\n",ret);
    return ret;
}

void WM1110_Storage::loadBootConfigParameters(void)
{
    loadDefaultParameters();
    loadCurrentParameters();

    loadPositionFileID();

    loadGnssGroupID();
    loadAppendParameters();

}

bool WM1110_Storage::readDefaultParameters(app_param_t *config_param)
{
    uint16_t r_len = sizeof(app_param_t);
    if (readLfsFile(device_default_info_file, (uint8_t *)config_param, &r_len))
        return false;
    return true;
}

bool WM1110_Storage::writeCurrentParameters(void)
{
    int ret = 0;

    app_param.param_len = ((uint8_t *)&app_param.crc-(uint8_t *)&app_param.param_len);
    app_param.crc = crc8_poly((uint8_t *)&app_param, app_param.param_len);    

    ret = writeLfsFile(device_info_file,(uint8_t *)&app_param,sizeof(app_param_t));
    if(ret >= 0)
    {
        return true;
    }
    return false;
}

bool WM1110_Storage::readCurrentParameters(app_param_t *config_param)
{
    uint16_t r_len = sizeof(app_param_t);
    if (readLfsFile(device_info_file, (uint8_t *)config_param, &r_len))
        return false;
    return true;
}

bool WM1110_Storage::getDefaultDevEui(uint8_t *def_eui)
{
    memcpy(def_eui,default_dev_eui,8);
    return true;
}

bool WM1110_Storage::restoreToFactoryParameters(void)
{
    app_param_t config_param_temp = {0};

    if(readDefaultParameters(&config_param_temp))
    {
        memcpy(&app_param,&config_param_temp,sizeof(app_param_t));
        if(writeCurrentParameters())
        {
            return true;
        }
    }
    else
    {
        return false;
    }
    return false;
}

bool WM1110_Storage::readPositionFileIDInfo(pos_msg_file_id_t *pos_info_param)
{
    uint16_t r_len = sizeof(pos_msg_file_id_t);
    if (readLfsFile(pos_info_file, (uint8_t *)pos_info_param, &r_len))
        return false;
    return true;
}

bool WM1110_Storage::writePositionData()
{
    uint8_t rc = 0;
    uint16_t app_tag_id= 0;
    uint16_t pos_data_len= 0;  

    //on the basis of  cur file id to save log data
    app_tag_id = pos_msg_file_id.file_id_cur;

    //update log data
    pos_data_len = sizeof(pos_msg_param_t);
    rc = writeLfsFileOne(app_tag_id, (uint8_t *)&pos_msg_param, pos_data_len);
    if (rc != 0)
    {
        WM1110_STORAGE_DBG_PRINTF("writePositionData error,code:%d\r\n",rc);
        return false;
    }
    if(!refreshPositionFileID())
    {
        WM1110_STORAGE_DBG_PRINTF("refreshPositionFileID error,code:%d\r\n",rc);
        return false;
    }  
    return true;      
}

bool WM1110_Storage::readSinglePositionData(pos_msg_param_t *pos_data, uint16_t index)
{
    uint8_t rc = 0;
    uint16_t app_tag_id = 0;
    uint16_t rlen = sizeof(pos_msg_param_t);

    if((pos_msg_file_id.log_cnt == 0)||(index > pos_msg_file_id.log_cnt)||index == 0) 
    {
        return false;  
    }  

    if(pos_file_id_end-pos_msg_file_id.file_id_start + 1 >= index)
    {
        app_tag_id = pos_msg_file_id.file_id_start + index-1;
    }
    else
    {
        app_tag_id = index-(pos_file_id_end-pos_msg_file_id.file_id_start+1)+pos_file_id_start-1;
    }
    rc = readLfsFileOne(app_tag_id, (uint8_t*)pos_data, &rlen);   
    if (rc == 0)  
    {
        return true;
    }
    return false;    
}

int8_t WM1110_Storage::deletePositionData(uint16_t del_cnt)
{
    uint16_t app_tag_id = 0; 
    uint16_t rlen1 = sizeof(pos_msg_file_id_t);
    pos_msg_file_id_t pos_msg_file_temp;
    memset(&pos_msg_file_temp,0,rlen1);
    
    if(pos_msg_file_id.log_cnt < del_cnt) return 0;    

    WM1110_STORAGE_DBG_PRINTF("delete pos msg count:%d,%d\r\n",del_cnt,pos_msg_file_id.log_cnt); 

    WM1110_STORAGE_DBG_PRINTF("start:%04x,end:%04x,log_cnt:%d\r\n",pos_msg_file_id.file_id_start,pos_msg_file_id.file_id_cur,pos_msg_file_id.log_cnt); 
    
    app_tag_id = pos_msg_file_id.file_id_start;

    //Change index points directly
    if(pos_file_id_end-pos_msg_file_id.file_id_start >= del_cnt)
    {
        app_tag_id = pos_msg_file_id.file_id_start + del_cnt;
    }
    else
    {
        app_tag_id = del_cnt - (pos_file_id_end-pos_msg_file_id.file_id_start)+pos_file_id_start-1; 
    }
    pos_msg_file_id.log_cnt = pos_msg_file_id.log_cnt - del_cnt;
    pos_msg_file_id.file_id_start = app_tag_id;

    if(writePositionFileID() == false)
    {
        WM1110_STORAGE_DBG_PRINTF("deletePositionData error\r\n");    
        readPositionFileIDInfo(&pos_msg_file_id);
        return -1;
    }
    return del_cnt;    
}

uint16_t WM1110_Storage::getPositionDataCount(void)
{
    return pos_msg_file_id.log_cnt;
}

void WM1110_Storage::convertPositionDatasToBuf(pos_msg_param_t pos_msg,uint8_t *buf,uint8_t *size)
{
    uint8_t offine_data_len_temp = 0;
    buf[0] = pos_msg.pos_type;

    if( pos_msg.pos_type != DATA_ID_UP_PACKET_GNSS_RAW )
    {
        memcpyr( buf + 1, ( uint8_t * )( &pos_msg.pos_status ), 4 );
        memcpyr( buf + 5, ( uint8_t * )( &pos_msg.utc_time ), 4 );
    }

    switch( pos_msg.pos_type )
    {
        case DATA_ID_UP_PACKET_WIFI_RAW:
        {
            for( uint8_t i = 0; i < pos_msg.context_count; i++ )
            {
                memcpy( buf + 9 + i * 7, &pos_msg.context.wifi_context[i].wifi_mac, 6 );
                memcpy( buf + 9 + i * 7 + 6, &pos_msg.context.wifi_context[i].cur_rssi, 1 );
            }
            offine_data_len_temp = 9 + pos_msg.context_count * 7;
        }
        break;

        case DATA_ID_UP_PACKET_BLE_RAW:
        {
            for( uint8_t i = 0; i < pos_msg.context_count; i++ )
            {
                memcpy( buf + 9 + i * 7, &pos_msg.context.beac_context[i].beac_mac, 6 );
                memcpy( buf + 9 + i * 7 + 6, &pos_msg.context.beac_context[i].cur_rssi, 1 );
            }
            offine_data_len_temp = 9 + pos_msg.context_count * 7;
        }
        break;

        case DATA_ID_UP_PACKET_GNSS_RAW:
        {
            buf[1] = pos_msg.context.gps_context.zone_flag; // fragment data
            buf[4] = pos_msg.context.gps_context.gnss_len; // GNSS raw lenght
            memcpyr( buf + 2, ( uint8_t * )( &pos_msg.context.gps_context.group_id ), 2 ); // group id
            memcpy( buf + 5, pos_msg.context.gps_context.gnss_res, pos_msg.context.gps_context.gnss_len ); // GNSS raw data
            offine_data_len_temp = 5 + pos_msg.context.gps_context.gnss_len;
        }
        break;

        case DATA_ID_UP_PACKET_GNSS_END:
        {
            buf[9] = pos_msg.context.gps_context.zone_flag; // fragment data
            memcpyr( buf + 10, ( uint8_t * )( &pos_msg.context.gps_context.group_id ), 2 ); // group id
            offine_data_len_temp = 12;
        }
        break;

        case DATA_ID_UP_PACKET_POS_STATUS:
        {
            offine_data_len_temp = 9;
        }
        break;
        case DATA_ID_UP_PACKET_USER_SENSOR:
        {
            memcpy( &buf[1], ( uint8_t * )( pos_msg.context.sensor_context.sensor_data ), pos_msg.context.sensor_context.len );
            offine_data_len_temp = pos_msg.context.sensor_context.len+1;
        }
        break;
        case DATA_ID_UP_PACKET_FACT_SENSOR:
        {
            memcpy( &buf[1], ( uint8_t * )( pos_msg.context.sensor_context.sensor_data ), pos_msg.context.sensor_context.len );
            offine_data_len_temp = pos_msg.context.sensor_context.len+1;
        }
        break;            
        default:
            offine_data_len_temp = 0;
            break;
    }
    *size = offine_data_len_temp;
}

bool WM1110_Storage::readGnssGroupID(void)
{
    uint8_t ret = 0;
    uint16_t test_data_len= 0;  
    test_data_len = sizeof(gnss_group_param_t);
    ret = readLfsFile(group_id_info_file,(uint8_t *)&app_gnss_group_param,&test_data_len);
    if(ret == 0)
    {
        track_gnss_group_id = app_gnss_group_param.group_id;
        return true;        
    }    
    else
    {
        WM1110_STORAGE_DBG_PRINTF("readGnssGroupID error\r\n"); 
    }
    return false;
}

uint8_t WM1110_Storage::writeGnssGroupID(void)
{
    uint8_t rc = 0;
    uint16_t test_data_len= 0;  
    //
    app_gnss_group_param.group_id = track_gnss_group_id;
    test_data_len = sizeof(gnss_group_param_t);
    rc = writeLfsFile(group_id_info_file, (uint8_t *)&app_gnss_group_param, test_data_len);
    if (rc != 0)
    {
        WM1110_STORAGE_DBG_PRINTF("write_gnss_group_id_param error\r\n");
        return rc;
    } 
    return 0;  
}

bool WM1110_Storage::readAppendParameters(void)
{
    uint8_t ret = 0;
    uint16_t test_data_len= 0;  
    test_data_len = sizeof(append_param_t);
    ret = readLfsFile(append_param_info_file,(uint8_t *)&app_append_param,&test_data_len);
    if(ret == 0)
    {
        return true;        
    }  
    else
    {
        WM1110_STORAGE_DBG_PRINTF("readAppendParameters error\r\n");    
    }  
    return false;
}

uint8_t WM1110_Storage::writeAppendParameters(void)
{
    uint8_t rc = 0;
    uint16_t test_data_len= 0;  
    //
    test_data_len = sizeof(append_param_t);
    rc = writeLfsFile(append_param_info_file, (uint8_t *)&app_append_param, test_data_len);
    if (rc != 0)
    {
        WM1110_STORAGE_DBG_PRINTF("writeAppendParameters error\r\n");
        return rc;
    } 
    return 0;  
}
#endif


#if 1	//protected
void WM1110_Storage::loadFactoryParametersFromFlash( void )
{
    app_param_t param_temp;
    uint8_t check_crc = 0;
    uint8_t check_len = 0;

    memset(&param_temp,0,sizeof(app_param_t));

    readFactoryParametersFromFlash(&param_temp,sizeof(app_param_t));
    // check length
    check_len = ((uint8_t *)&app_param.crc-(uint8_t *)&app_param.param_len);

    // check crc
    check_crc = crc8_poly((uint8_t *)&param_temp, check_len);
    if((param_temp.param_len != check_len)||(param_temp.crc != check_crc ))  // Factory setting is null
    {
        app_param.param_len = ((uint8_t *)&app_param.crc-(uint8_t *)&app_param.param_len);
        app_param.crc = crc8_poly((uint8_t *)&app_param, app_param.param_len);
        writeFactoryParametersToFlash((uint8_t*)&app_param,sizeof(app_param_t));
        return;
    }
    memcpy(&app_param,&param_temp,check_len);
    memcpy(default_dev_eui,app_param.lora_info.DevEui,8);
}

int WM1110_Storage::readFactoryParametersFromFlash (void *buffer, uint8_t size)
{
    const uint32_t page = FlashFactSettingPages;

    Wm1110Hardware::getInstance().flash.read(page, buffer, size);
    return 0;
}

int WM1110_Storage::writeFactoryParametersToFlash (void *buffer, uint8_t size)
{
    const uint32_t page = FlashFactSettingPages;
    Wm1110Hardware::getInstance().flash.write(page, buffer, size);
    return 0;
}

void WM1110_Storage::loadDefaultParameters(void)
{
    uint8_t ret = 0;    
    uint16_t test_data_len= 0;  
    app_param_t app_param_temp = {0};
    uint8_t u8_crc=0;

    test_data_len = sizeof(app_param_t );

    loadFactoryParametersFromFlash();

    ret = readLfsFile(device_default_info_file,(uint8_t *)&app_param_temp,&test_data_len);
    if(ret != 0)
    {
        app_param.param_len = ((uint8_t *)&app_param.crc-(uint8_t *)&app_param.param_len);
        app_param.crc = crc8_poly((uint8_t *)&app_param, app_param.param_len);    
        ret = writeLfsFile(device_default_info_file, (uint8_t *)&app_param, sizeof(app_param_t));
        if(ret != 0)
        {
            WM1110_STORAGE_DBG_PRINTF("loadDefaultParameters error\r\n"); 
        }         
    } 
    else //check
    {
        //check 
        uint8_t check_len = ((uint8_t *)&app_param.crc-(uint8_t *)&app_param.param_len);
        u8_crc = crc8_poly((uint8_t *)&app_param_temp, check_len);
        if((u8_crc != app_param.crc) || (app_param_temp.crc != u8_crc))
        {    
            app_param.param_len = ((uint8_t *)&app_param.crc-(uint8_t *)&app_param.param_len);
            app_param.crc = crc8_poly((uint8_t *)&app_param, app_param.param_len);
            ret = writeLfsFile(device_default_info_file, (uint8_t *)&app_param, sizeof(app_param_t));
            if(ret != 0)
            {
                WM1110_STORAGE_DBG_PRINTF("app_param,len:%02x,crc:%02x\r\n",app_param.param_len,app_param.crc);
            }
        }        
    } 
}

bool WM1110_Storage::writeDefaultParameters(void)
{
    int ret = 0;

    app_param.param_len = ((uint8_t *)&app_param.crc-(uint8_t *)&app_param.param_len);
    app_param.crc = crc8_poly((uint8_t *)&app_param, app_param.param_len);    

    ret = writeLfsFileOne(device_default_info_file,(uint8_t *)&app_param,sizeof(app_param_t));
    if(ret >= 0)
    {
        return true;
    }
    return false;
}

void WM1110_Storage::loadCurrentParameters(void)
{
    uint8_t ret = 0;    
    uint16_t test_data_len= 0;  
    app_param_t app_param_temp = {0};
    uint8_t u8_crc=0;

    test_data_len = sizeof(app_param_t );

    ret = readLfsFile(device_info_file,(uint8_t *)&app_param_temp,&test_data_len);
    if(ret != 0)
    {
        app_param.param_len = ((uint8_t *)&app_param.crc-(uint8_t *)&app_param.param_len);
        app_param.crc = crc8_poly((uint8_t *)&app_param, app_param.param_len);    
        ret = writeLfsFile(device_info_file, (uint8_t *)&app_param, sizeof(app_param_t));
        if(ret != 0)
        {
            WM1110_STORAGE_DBG_PRINTF("loadAppendParameters error\r\n"); 
        }         
    } 
    else //check
    {
        //check length 
        uint8_t check_len = ((uint8_t *)&app_param.crc-(uint8_t *)&app_param.param_len);
        u8_crc = crc8_poly((uint8_t *)&app_param_temp, app_param_temp.param_len);
        if((u8_crc != app_param_temp.crc) || (app_param_temp.param_len != check_len))
        {
            WM1110_STORAGE_DBG_PRINTF("param crc error,u8_crc:%02x,crc:%02x\r\n",u8_crc,app_param_temp.crc);       
            app_param.param_len = ((uint8_t *)&app_param.crc-(uint8_t *)&app_param.param_len);
            app_param.crc = crc8_poly((uint8_t *)&app_param, app_param.param_len);
            ret = writeLfsFile(device_info_file, (uint8_t *)&app_param, sizeof(app_param_t));
            if(ret != 0)
            {
                WM1110_STORAGE_DBG_PRINTF("app_param,len:%02x,crc:%02x\r\n",app_param.param_len,app_param.crc);
            }
        }
        else
        {
            memcpy(&app_param,&app_param_temp,test_data_len);         
        }           
    }

}  

void WM1110_Storage::loadPositionFileID()
{
    uint16_t pos_info_len = 0; 
    uint8_t ret=0;
    //pos msg info
    pos_info_len = sizeof(pos_msg_file_id_t);
    ret = readLfsFile(pos_info_file,(uint8_t *)&pos_msg_file_id,&pos_info_len);
    if( ret != 0 || pos_msg_file_id.log_cnt > pos_save_count_max || pos_msg_file_id.file_id_start > pos_file_id_end || pos_msg_file_id.file_id_cur > pos_file_id_end )
    {
        //init default log file id
        pos_msg_file_id.reboot_cnt = 0;
        pos_msg_file_id.log_cnt = 0;
        pos_msg_file_id.file_id_start = pos_file_id_start;        
        pos_msg_file_id.file_id_cur = pos_file_id_start;
        ret = writeLfsFile(pos_info_file, (uint8_t *)&pos_msg_file_id, sizeof(pos_msg_file_id_t));
        if(ret != 0)
        {
            WM1110_STORAGE_DBG_PRINTF("loadPositionFileID,code:%02x\r\n",ret); 
        }          
    }
    else
    {
        if(pos_msg_file_id.file_id_start < pos_file_id_start ||pos_msg_file_id.file_id_cur<pos_file_id_start)
        {
            pos_msg_file_id.file_id_start = pos_file_id_start;
            pos_msg_file_id.file_id_cur = pos_file_id_start;
            pos_msg_file_id.reboot_cnt = 0;
            pos_msg_file_id.log_cnt = 0;
        }    
        pos_msg_file_id.reboot_cnt++;   
        ret = writeLfsFile(pos_info_file, (uint8_t *)&pos_msg_file_id, sizeof(pos_msg_file_id_t));
        if(ret != 0)
        {
            WM1110_STORAGE_DBG_PRINTF("loadPositionFileID,code:%02x\r\n",ret); 
        }           
    }    
}

bool WM1110_Storage::writePositionFileID()
{
    uint8_t rc = 0;
    uint16_t rlen = 0;
    rc = writeLfsFile(pos_info_file, (uint8_t *)&pos_msg_file_id, sizeof(pos_msg_file_id_t));
    if (rc != 0)
    {
        rlen = sizeof(pos_msg_file_id_t);
        rc = readLfsFile(pos_info_file,(uint8_t *)&pos_msg_file_id, &rlen);   
        WM1110_STORAGE_DBG_PRINTF("writePositionFileID error,code:%d\r\n",rc); 
        return false;       
    }
    return true;
}

bool WM1110_Storage::refreshPositionFileID()
{

    pos_msg_file_id.file_id_cur++;
    pos_msg_file_id.log_cnt++;

    if(pos_msg_file_id.file_id_cur > pos_file_id_end) //judge file id range
    {
        pos_msg_file_id.file_id_cur = pos_file_id_start;
    }
    if(pos_msg_file_id.log_cnt > pos_save_count_max)
    {
        pos_msg_file_id.log_cnt = pos_save_count_max;
        pos_msg_file_id.file_id_start++;
        if(pos_msg_file_id.file_id_start > pos_file_id_end) //judge file id range
        {
            pos_msg_file_id.file_id_start = pos_file_id_start;
        }
    }
    if(!writePositionFileID())
    {
        WM1110_STORAGE_DBG_PRINTF("refreshPositionFileID error\r\n"); 
        return false;
    }
    return true;    
}

void WM1110_Storage::loadGnssGroupID(void)
{
    uint8_t ret = 0;    
    uint16_t test_data_len= 0;  

    test_data_len = sizeof(gnss_group_param_t );
    ret = readLfsFile(group_id_info_file,(uint8_t *)&app_gnss_group_param,&test_data_len);
    if(ret != 0)
    {
        ret = writeLfsFile(group_id_info_file, (uint8_t *)&app_gnss_group_param, sizeof(gnss_group_param_t));
        if(ret != 0)
        {
            WM1110_STORAGE_DBG_PRINTF("gnss_group_id_param_init error\r\n"); 
        }  
        track_gnss_group_id = app_gnss_group_param.group_id;            
    } 
    else
    {
        track_gnss_group_id = app_gnss_group_param.group_id;  
    }    
}

void WM1110_Storage::loadAppendParameters(void)
{
    uint8_t ret = 0;    
    uint16_t test_data_len= 0;  

    test_data_len = sizeof(append_param_t );
    ret = readLfsFile(append_param_info_file,(uint8_t *)&app_append_param,&test_data_len);
    if(ret != 0)
    {
        ret = writeLfsFile(append_param_info_file, (uint8_t *)&app_append_param, sizeof(append_param_t));
        if(ret != 0)
        {
            WM1110_STORAGE_DBG_PRINTF("loadAppendParameters error\r\n"); 
        }         
    }       
}

#endif

WM1110_Storage wm1110_storage;

