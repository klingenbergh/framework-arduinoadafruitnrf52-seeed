#ifndef _WM1110_STORAGE_H_
#define _WM1110_STORAGE_H_

#pragma once

#include <Arduino.h>

#include <ExternalFileSystem.h>
#include <WM1110_At_Config.hpp>


#if ( WM1110_STORAGE_DBG_ENABLE == 1)
#define WM1110_STORAGE_DBG_PRINTF( ... )  printf (  __VA_ARGS__ )
#else
#define WM1110_STORAGE_DBG_PRINTF( ... )  
#endif

#define FLASH_PAGE_SIZE (4096)

/**
 * @defgroup KEY_VALUE_FDS_ERROR_CODE Possible error codes
 * @{
 */
#define KEY_VALUE_FDS_ERR_OK             0   /**< No error. */
#define KEY_VALUE_FDS_ERR_NO_ENTRY      -2   /**< No entry. */
#define KEY_VALUE_FDS_ERR_IO            -5   /**< Error during device operation. */
#define KEY_VALUE_FDS_ERR_NO_MEM        -12  /**< No more memory available. */
#define KEY_VALUE_FDS_ERR_EXIST         -17  /**< Entry already exists . */
#define KEY_VALUE_FDS_ERR_INVAL         -22  /**< Invalid parameter. */
#define KEY_VALUE_FDS_ERR_VALUE_BIG     -27  /**< Value length too large. */
#define KEY_VALUE_FDS_ERR_NO_SPACE      -28  /**< No space left on device. */
#define KEY_VALUE_FDS_ERR_CORRUPT       -84  /**< Corrupted. */
/** @} */

#define KEY_VALUE_LFS_CACHE_SIZE        256


class WM1110_Storage
{
    private:
        static WM1110_Storage* instance_;

        //it is lorawan factory setting,be Be carefully don't erase
        static constexpr uintptr_t FLASH_FACT_END_ADDR = 0xE9000;    // Look at nrf52840_s140_v6.ld
        static const uint32_t FlashFactSettingPages = FLASH_FACT_END_ADDR / FLASH_PAGE_SIZE - 1;
        
        //use littlefs save parameter
        lfs_t    *s_op_instance_ptr;
        static constexpr uint16_t device_info_file = 0x100;
        static constexpr uint16_t device_default_info_file = 0x101;    
        static constexpr uint16_t pos_info_file = 0x104;
        static constexpr uint16_t group_id_info_file = 0x109;    
        static constexpr uint16_t append_param_info_file = 0x10A;
        static constexpr uint16_t pos_save_count_max = 120;    
        static constexpr uint8_t pos_save_in_file_max = 40;    // Multiple pieces of offline data are stored in a file 
        static constexpr uint16_t pos_file_id_start = 0x400;    
        static constexpr uint16_t pos_file_id_end = 0x400 + (pos_save_count_max);    

        uint8_t default_dev_eui[8];
        __attribute__ ((aligned (4))) uint8_t s_file_buffer[KEY_VALUE_LFS_CACHE_SIZE];


    private:
        uint8_t readLfsFile(uint16_t file_name, uint8_t *data, uint16_t *len) ;
        
        uint8_t writeLfsFile(uint16_t file_name, uint8_t *data, uint16_t len);
        
        uint8_t readLfsFileOne( uint16_t file_name, uint8_t *data, uint16_t *len );
        
        uint8_t writeLfsFileOne( uint16_t file_name, uint8_t *data, uint16_t len );

    public:
        WM1110_Storage(void);  
        static WM1110_Storage& getInstance();

        int begin(void);

        /*!
        *  @brief  Write key value to memory area
        *  @param key   
        *  @param p_value 
        *  @param length 
        *  @return KEY_VALUE_FDS_ERROR_CODE
        */        
        int writeStorageKeyValue(uint32_t key,const void *p_value,uint32_t length);
        
        /*!
        *  @brief  Read key value from memory area
        *  @param key   
        *  @param p_buffer 
        *  @param length 
        *  @return KEY_VALUE_FDS_ERROR_CODE
        */  
        int readStorageKeyValue(uint32_t key,void *p_buffer,uint32_t length);

        /*!
        *  @brief  Delete the value data corresponding to the key
        *  @param key   
        *  @return KEY_VALUE_FDS_ERROR_CODE
        */   
        int deleteStorageKeyValue(uint32_t key);
        
        /*!
        *  @brief  Get the value data size corresponding to the key
        *  @param key   
        *  @return value size
        */   
        int getStorageKeyValueSize(uint32_t key);
        
        /*!
        *  @brief  Write key value to memory offset area
        *  @param key   
        *  @param p_value 
        *  @param length  
        *  @param pos 
        *  @return KEY_VALUE_FDS_ERROR_CODE
        */ 
        int writeStorageKeyPosValue(uint32_t key,const void *p_value,uint32_t length,int32_t pos);

        /*!
        *  @brief  Read key value to memory offset area
        *  @param key   
        *  @param p_value 
        *  @param length  
        *  @param pos 
        *  @return KEY_VALUE_FDS_ERROR_CODE
        */         
        int readStorageKeyPosValue(uint32_t key,void *p_buffer,uint32_t length,int32_t pos);

        /*!
        *  @brief  Load the boot configuration parameters
        */     
        void loadBootConfigParameters(void);

        /*!
        *  @brief  Read the default configuration parameters
        *  @param config_param 
        *  @return bool       
        */   
        bool readDefaultParameters(app_param_t *config_param);

        /*!
        *  @brief  Write the current configuration parameters
        *  @return bool       
        */  
        bool writeCurrentParameters(void);

        /*!
        *  @brief  Read the current configuration parameters
        *  @return bool       
        */          
        bool readCurrentParameters(app_param_t *config_param);

        /*!
        *  @brief  Get the default network code DevEUI
        *  @param def_eui         
        *  @return bool       
        */  
        bool getDefaultDevEui(uint8_t *def_eui);

        /*!
        *  @brief  Restores the current parameters to their default values       
        *  @return bool       
        */  
        bool restoreToFactoryParameters(void);

        /*!
        *  @brief  Read the position file id info
        *  @param pos_info_param         
        *  @return bool       
        */ 
        bool readPositionFileIDInfo(pos_msg_file_id_t *pos_info_param);

        /*!
        *  @brief  Write the position datas      
        *  @return bool       
        */ 
        bool writePositionData(void);

        /*!
        *  @brief  Read the single position datas     
        *  @param pos_data   
        *  @param index                   
        *  @return bool       
        */  
        bool readSinglePositionData(pos_msg_param_t *pos_data, uint16_t index);
        
        /*!
        *  @brief  Delete the multiple position datas     
        *  @param del_cnt                  
        *  @return -1:Error     0:There's not that much data   large than 0: Number of successfully deleted items
        */          
        int8_t deletePositionData(uint16_t del_cnt);  

        /*!
        *  @brief  Get the position datas count                 
        *  @return count
        */          
        uint16_t getPositionDataCount(void);

        /*!
        *  @brief  Convert the single position datas  to buffer datas   
        *  @param pos_msg   
        *  @param buf  
        *  @param size                   
        */ 
        void convertPositionDatasToBuf(pos_msg_param_t pos_msg,uint8_t *buf,uint8_t *size);

        /*!
        *  @brief  Read the GNSS position group id   
        *  @return bool                
        */ 
        bool readGnssGroupID(void);

        /*!
        *  @brief  Write the GNSS position group id   
        *  @return 0:success                
        */ 
        uint8_t writeGnssGroupID(void);

        /*!
        *  @brief  Read the append parameters 
        *  @return bool             
        */ 
        bool readAppendParameters(void);

        /*!
        *  @brief  Write the append parameters 
        *  @return bool             
        */ 
        uint8_t writeAppendParameters(void);

    protected:

        //in flash
        void loadFactoryParametersFromFlash( void );

        int readFactoryParametersFromFlash(void *buffer, uint8_t size);

        int writeFactoryParametersToFlash(void *buffer, uint8_t size); 

        //in littlefs
        void loadDefaultParameters(void);    

        bool writeDefaultParameters(void);

        void loadCurrentParameters(void);        
        // position data info
        void loadPositionFileID();

        bool writePositionFileID();

        bool refreshPositionFileID();
        // gnss group id 
        void loadGnssGroupID(void);

        // append parameter 
        void loadAppendParameters(void);

};

extern WM1110_Storage wm1110_storage;

#endif /* _WM1110_STORAGE_H_ */
