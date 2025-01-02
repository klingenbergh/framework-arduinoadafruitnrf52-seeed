#ifndef _WM1110_BLE_H_
#define _WM1110_BLE_H_

#pragma once

#include <bluefruit.h>

#include <Lbm_Modem_Common.hpp>


#if ( WM1110_BLE_DBG_ENABLE == 1)
#define WM1110_BLE_DBG_PRINTF( ... )  printf (  __VA_ARGS__ )
#else
#define WM1110_BLE_DBG_PRINTF( ... )  
#endif


#define BLE_SCAN_INTERVAL               160      /**< Determines scan interval(in units of 0.625 ms). */
#define BLE_SCAN_WINDOW                 80       /**< Determines scan window(in units of 0.625 ms). */
#define BLE_SCAN_DURATION               0        /**< Duration of the scanning(in units of 10 ms). */

#define BEACON_DATA_LEN     0x15
#define BEACON_DATA_TYPE    0x02
#define COMPANY_IDENTIFIER  0x004C        // Apple's company ID  

#define BLE_BEACON_BUF_MAX      16
#define BLE_BEACON_SEND_MUM     3
#define BLE_BEACON_FILTER       -80

#define IBEACON_MAC_ADDRESS_SIZE    6
#define IBEACON_RSSI_SIZE           1

typedef struct sWM1110_Beacons
{
    uint32_t utc;               //time 
    uint16_t company_id;        //ibeacon company id
    uint8_t uuid[16];           //ibeacon uuid
    uint16_t major;
    uint16_t minor;
    int8_t rssi;                 
    int8_t rssi_;
    uint8_t mac[8];             
}WM1110_Beacons_t;

enum class BleRunState
{
    StateUnkown,
    StateInit,
    StateScaning,
    StateStopedScan,
    StateStartAdv,
    StateStopedAdv,
    StateConnect,
    StateDisconnect,
};

extern BLEComm ble_comm_service;

class WM1110_BLE
{
    private:
        static WM1110_BLE* instance_;

        static uint8_t scaned_beacon_num;  // Number of ibeacons scanned,range:0~BLE_BEACON_BUF_MAX
        static WM1110_Beacons_t scaned_beacon_buf[BLE_BEACON_BUF_MAX];     //ibeacons data buffer
        static uint8_t scaned_beacon_rssi_array[BLE_BEACON_BUF_MAX];   //ibeacons rssi data buffer

        char  tx_buf[244];      // ble tx buffer
        static BleRunState run_status;   
        static constexpr uint16_t SenseCAP_SCAN_UUID1 = 0x2886;
        static constexpr uint16_t SenseCAP_SCAN_UUID2 = 0xA886;

        BLEUuid uuid1 = BLEUuid(SenseCAP_SCAN_UUID1);
        BLEUuid uuid2 = BLEUuid(SenseCAP_SCAN_UUID2);

        static uint16_t ble_connect_handle;
    public:
        char rx_buf[244] = {0};            //ble rx data buffer    
        uint8_t rx_length = 0;                //ble rx data len 

        WM1110_Beacons_t beacon_results_buf[3] = { 0 }; // Number of ibeacons need to be send
        uint8_t beacon_results_num = 0;             // Number of ibeacons need to be send,range:0~3

    private:

        void setScanParameters(void);

        void setAdvParameters(void);

        static void ble_connect_cb(uint16_t conn_handle);

        static void ble_disconnect_cb(uint16_t conn_handle, uint8_t reason);

        void clearRecbuf( void );
      
        static void ble_scanner_evt_handler( ble_gap_evt_adv_report_t* report );

    public:
        WM1110_BLE(void);  

        static WM1110_BLE& getInstance();

        void begin(void); 

        /*!
        *  @brief  Set Ble name 
        *  @param buf   
        */      
        void setName(const char *buf = NULL);

        /*!
         *  @brief Get Ble name 
         *  @param name    
         *  @param bufsize    
         *  @return uint8_t
         */
        uint8_t getName(char* name, uint16_t bufsize);

        /*!
        *  @brief  Set the Bluetooth parameters
        */  
        void setStartParameters( void );

        /*!
        *  @brief  Set the Bluetooth parameters
        *  @param central true: central false:peripheral  
        */  
        void setStartParameters( bool central );

        /*!
        *  @brief  Start the Bluetooth scan 
        *  @return bool
        */           
        bool startScan( void );

        /*!
        *  @brief  Get the Bluetooth scan results
        *  @return bool
        */    
        bool getScanedResults(void);

        /*!
        *  @brief  Sort the scan results according to rssi
        */    
        void sortScanedResults(void);

        /*!
        *  @brief  Get the Bluetooth scaned data
        *  @param result   
        *  @param size      
        *  @return bool
        */   
        bool getScanedDatas( uint8_t *result, uint8_t *size );

        /*!
        *  @brief  Stop the Bluetooth scan
        */  
        void stopScan( void );

        /*!
        *  @brief  Display the Bluetooth scaned data
        */ 
        void displayScanedDatas( void );

        /*!
        *  @brief  Start Bluetooth Advertising
        *  @return bool        
        */ 
        bool startAdv( void );

        /*!
        *  @brief  Stop Bluetooth Advertising
        *  @return bool        
        */         
        void stopAdv( void );
        
        /*!
        *  @brief  print datas
        *  @param fmt  
        *  @param argp      
        */   
        void vprint(const char *fmt, va_list argp); 

        /*!
        *  @brief  print datas
        *  @param fmt  
        *  @param argp      
        */   
        void ble_trace_print(const char *fmt, ...);

        /*!
        *  @brief  Detect whether Bluetooth receives data
        *  @return       
        */   
        bool getBleRecData(void);

        /*!
        *  @brief  Get Bluetooth receives data
        *  @param buf  
        *  @param size        
        *  @return       
        */  
        bool getBleRecData(uint8_t *buf,uint8_t *size);

        /*!
        *  @brief  Control disconnect Bluetooth      
        */  
        void controlDisconnectBle( void );

        /*!
        *  @brief  Get ble running status
        *  @return BleRunState            
        */  
        BleRunState getBleStatus( void );
};

extern WM1110_BLE wm1110_ble;

#endif /* _WM1110_BLE_H_ */
