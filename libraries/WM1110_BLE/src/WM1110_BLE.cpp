#include "WM1110_BLE.hpp"


#include <WM1110_At_Config.hpp>


WM1110_BLE* WM1110_BLE::instance_ = nullptr;

BLEComm ble_comm_service;

uint8_t WM1110_BLE::scaned_beacon_num;  //Number of ibeacons scanned
WM1110_Beacons_t WM1110_BLE::scaned_beacon_buf[BLE_BEACON_BUF_MAX];     //ibeacons data buffer
uint8_t WM1110_BLE::scaned_beacon_rssi_array[BLE_BEACON_BUF_MAX];   //ibeacons rssi data buffer

BleRunState WM1110_BLE::run_status = BleRunState::StateUnkown;  

uint16_t WM1110_BLE::ble_connect_handle = 0;

WM1110_BLE& WM1110_BLE::getInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = new WM1110_BLE();
    }

    return *instance_;
}

WM1110_BLE::WM1110_BLE(void)
{

}
#if 1 // public function
void WM1110_BLE::begin(void)
{
    static bool begin_flag = false;
    if(begin_flag == true)
    {
        return;
    }
    // Config the peripheral connection with maximum bandwidth 
    // more SRAM required by SoftDevice
    // Note: All config***() function must be called before begin()
    Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
    Bluefruit.begin(1,1);
    Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values

    begin_flag = true;
}

void WM1110_BLE::setName(const char *buf)
{
    char adv_device_name[24] = {0}; 
    uint8_t len = 0;
    if(buf == NULL)
    {
        hexTonum((unsigned char *)adv_device_name, app_param.hardware_info.Sn, 9);
        uint8_t templen = strlen("-1110");
        memcpy(&adv_device_name[18],"-1110",templen);  
        len = 23;
    }
    else
    {
        len = strlen(buf);
        if(len>23)
        {
            len = 23;
        }
        memcpy(adv_device_name,buf,len);

    }
    Bluefruit.setName(adv_device_name);
}

uint8_t WM1110_BLE::getName(char* name, uint16_t bufsize)
{
    return Bluefruit.getName(name,bufsize);
}

void WM1110_BLE::setStartParameters( void )
{
    //scan
    setScanParameters();

    //adv 
    setAdvParameters();

    run_status = BleRunState::StateInit;
}

void WM1110_BLE::setStartParameters( bool central )
{
    if(central == true)
    {
        setScanParameters();
    }
    else
    {
        //adv 
        setAdvParameters();
    }
    run_status = BleRunState::StateInit;  
}

bool WM1110_BLE::startScan( void )
{
    if(getBleStatus() == BleRunState::StateScaning)
    {
        return true;
    }
    for( uint8_t i = 0; i < BLE_BEACON_BUF_MAX; i++ )
    {
        memset(( uint8_t *)( &scaned_beacon_buf[i] ), 0, sizeof( WM1110_Beacons_t ));    
    }
    
    scaned_beacon_num = 0;
    Bluefruit.Scanner.start(0);

    run_status = BleRunState::StateScaning;
    return true;
}

bool WM1110_BLE::getScanedResults(void)
{
    if(scaned_beacon_num > 0)
    {
        sortScanedResults();
        return true;
    }
    else
    {
        return false;
    }
    return false;
}

void WM1110_BLE::sortScanedResults(void)
{
    int8_t rssi_max_index = 0;
    for( uint8_t i = 0; i < scaned_beacon_num; i++ )
    {
        scaned_beacon_rssi_array[i] = i;
    }

    //sort
    for( uint8_t i = 0; i < scaned_beacon_num; i++ )
    {
        for( uint8_t j = i; j < scaned_beacon_num; j++ )
        {
            if( scaned_beacon_buf[scaned_beacon_rssi_array[i]].rssi_ < scaned_beacon_buf[scaned_beacon_rssi_array[j]].rssi_ )
            {
                rssi_max_index = scaned_beacon_rssi_array[i];
                scaned_beacon_rssi_array[i] = scaned_beacon_rssi_array[j];
                scaned_beacon_rssi_array[j] = rssi_max_index;
            }
        }
    }
}

bool WM1110_BLE::getScanedDatas( uint8_t *result, uint8_t *size )
{
    uint8_t beacon_num = 0;

    if( result && size )
    {
        *size = 0;
        if( scaned_beacon_num == 0 ) return false;
        
        sortScanedResults();

        memcpyr( result, ( uint8_t * )( &scaned_beacon_buf[0].utc ), 4 );
        *size += 4;

        // The number that exceeds the maximum number sent must be truncated
        if( scaned_beacon_num > BLE_BEACON_SEND_MUM )
        {
            beacon_num = BLE_BEACON_SEND_MUM;
        }
        else
        {
            beacon_num = scaned_beacon_num;
        }

        //fill in mac&rssi
        for( uint8_t i = 0; i < beacon_num; i ++ )
        {
            memcpyr( result + 4 + i * 7, ( uint8_t *)( &scaned_beacon_buf[scaned_beacon_rssi_array[i]].mac ), 6 );
            memcpy( result + 4 + i * 7 + 6, &scaned_beacon_buf[scaned_beacon_rssi_array[i]].rssi_, 1 );
            *size += 7;
        }
        beacon_results_num = beacon_num;
        // if Not enough, fill in 0xff
        if( beacon_num < BLE_BEACON_SEND_MUM )
        {
            for( uint8_t i = 0; i < ( BLE_BEACON_SEND_MUM - beacon_num ); i++ )
            {
                memset( result + 4 + beacon_num * 7 + i * 7, 0xff, 7 );
                *size += 7;

            }
        }
        
        if( beacon_num ) return true;
        else return false;
    }
    return false;
}

void WM1110_BLE::stopScan( void )
{
    if(getBleStatus() == BleRunState::StateStopedScan)
    {
        return;
    }
    Bluefruit.Scanner.stop();
    bool ret = getScanedResults();
    if(ret == true )
    {
        sortScanedResults();   
        uint8_t beacon_num = 0;

        beacon_num = scaned_beacon_num > BLE_BEACON_SEND_MUM ? BLE_BEACON_SEND_MUM:scaned_beacon_num;

        for( uint8_t i = 0; i < beacon_num; i ++ )
        {
            memcpy(&beacon_results_buf[i],&scaned_beacon_buf[scaned_beacon_rssi_array[i]],sizeof(WM1110_Beacons_t));
        }  
        beacon_results_num = beacon_num;
    }

    run_status = BleRunState::StateStopedScan;

}

void WM1110_BLE::displayScanedDatas( void )
{
    printf( "iBeacon: %d\r\n", scaned_beacon_num );
    for( uint8_t i = 0; i < scaned_beacon_num; i ++ )
    {
        printf("%04x, ", scaned_beacon_buf[scaned_beacon_rssi_array[i]].company_id );
        for( uint8_t j = 0; j < 16; j++ )
        {
            printf( "%02x ", scaned_beacon_buf[scaned_beacon_rssi_array[i]].uuid[j] );
        }

        uint16_t major = 0, minor = 0;
        memcpyr(( uint8_t *)( &major ), ( uint8_t *)( &scaned_beacon_buf[scaned_beacon_rssi_array[i]].major ), 2 );
        memcpyr(( uint8_t *)( &minor ), ( uint8_t *)( &scaned_beacon_buf[scaned_beacon_rssi_array[i]].minor ), 2 );
        printf(", %04x, ", major );
        printf("%04x, ", minor );
        printf("%d/%d dBm, ", scaned_beacon_buf[scaned_beacon_rssi_array[i]].rssi, scaned_beacon_buf[scaned_beacon_rssi_array[i]].rssi_ );

        for( uint8_t j = 0; j < 5; j++ )
        {
            printf( "%02x:", scaned_beacon_buf[scaned_beacon_rssi_array[i]].mac[5 - j] );
        }
        printf( "%02x\r\n", scaned_beacon_buf[scaned_beacon_rssi_array[i]].mac[0] );
    }    
}

bool WM1110_BLE::startAdv( void )
{
    if(getBleStatus() == BleRunState::StateStartAdv)
    {
        return true;
    }
    clearRecbuf();
    Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
   
    run_status = BleRunState::StateStartAdv;   

    return true;
}

void WM1110_BLE::stopAdv( void )
{
    Bluefruit.Advertising.stop(); 

    run_status = BleRunState::StateStopedAdv;   
             
}

void WM1110_BLE::ble_trace_print(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);
    WM1110_BLE::vprint(fmt, argp);
    va_end(argp);
}

bool WM1110_BLE::getBleRecData(void)
{
    rx_length = ble_comm_service.available();
    if(rx_length > 0 )
    {
        ble_comm_service.read(rx_buf,rx_length);
        ble_comm_service.flush(); // empty rx fifo
        return true;
    }
    return false;
}

bool WM1110_BLE::getBleRecData(uint8_t *buf,uint8_t *size)
{
    rx_length = ble_comm_service.available();
    if(rx_length > 0 )
    {
        ble_comm_service.read(rx_buf,rx_length);
        ble_comm_service.flush(); // empty rx fifo
        memcpy(buf,rx_buf,rx_length);
        *size = rx_length;
        return true;
    }
    return false;
}

void WM1110_BLE::controlDisconnectBle( void )
{ 
    Bluefruit.disconnect(ble_connect_handle); 
}
#endif

#if 1 // private function

void WM1110_BLE::setScanParameters(void)
{
    /* Set the LED interval for blinky pattern on BLUE LED */
    // Bluefruit.setConnLedInterval(250);
    /* Start Central Scanning
    * - Enable auto scan if disconnected
    * - Filter out packet with a min rssi
    * - Interval = 100 ms, window = 50 ms
    * - Use active scan (used to retrieve the optional scan response adv packet)
    * - Start(0) = will scan forever since no timeout is given
    */
    Bluefruit.Scanner.setRxCallback(&WM1110_BLE::ble_scanner_evt_handler);
    Bluefruit.Scanner.restartOnDisconnect(true);
    Bluefruit.Scanner.filterRssi(BLE_BEACON_FILTER);
    Bluefruit.Scanner.setInterval(BLE_SCAN_INTERVAL, BLE_SCAN_WINDOW);       // in units of 0.625 ms
    Bluefruit.Scanner.useActiveScan(true);        // Request scan response data
}

void WM1110_BLE::setAdvParameters(void)
{
    Bluefruit.Periph.setConnectCallback(&WM1110_BLE::ble_connect_cb);
    Bluefruit.Periph.setDisconnectCallback(&WM1110_BLE::ble_disconnect_cb);

    // Configure and Start BLE Uart Service
    ble_comm_service.begin();

    // Set up and start advertising
    Bluefruit.Advertising.addUuid(uuid1,uuid2);
    Bluefruit.Advertising.addName();

    /* Start Advertising
    * - Enable auto advertising if disconnected
    * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
    * - Timeout for fast mode is 30 seconds
    * - Start(timeout) with timeout = 0 will advertise forever (until connected)
    * 
    * For recommended advertising interval
    * https://developer.apple.com/library/content/qa/qa1931/_index.html   
    */
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
    Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
}

void WM1110_BLE::ble_connect_cb(uint16_t conn_handle)
{
    char central_name[32] = { 0 };
    ble_gap_addr_t ble_gap_addr;

    ble_connect_handle = conn_handle;
    // Get the reference to current connection
    BLEConnection* connection = Bluefruit.Connection(conn_handle);
    WM1110_BLE_DBG_PRINTF("conn_handle:%d\r\n",conn_handle);

    ble_gap_addr = connection->getPeerAddr(); 
    connection->getPeerName(central_name, sizeof(central_name));
    WM1110_BLE_DBG_PRINTF("Connected to %s\r\n",central_name);
    WM1110_BLE_DBG_PRINTF("mac:%02x:%02x:%02x:%02x:%02x:%02x\r\n",ble_gap_addr.addr[5],ble_gap_addr.addr[4],ble_gap_addr.addr[3],\
                                            ble_gap_addr.addr[2],ble_gap_addr.addr[1],ble_gap_addr.addr[0]);

    // request to update data length
    connection->requestDataLengthUpdate();

    // // request mtu exchange
    // connection->requestMtuExchange(247);

    run_status = BleRunState::StateConnect;


}

void WM1110_BLE::ble_disconnect_cb(uint16_t conn_handle, uint8_t reason)
{
    (void) conn_handle;
    (void) reason;
    WM1110_BLE_DBG_PRINTF("Disconnected, reason = 0x%0x\r\n",reason);

    run_status = BleRunState::StateDisconnect;
   
}

void WM1110_BLE::vprint(const char *fmt, va_list argp) 
{
    uint8_t ble_tx_len = 0;
    uint8_t ble_tx_size = 0;
    bool notify_status = ble_comm_service.notifyEnabled();
    if(notify_status == true)
    {
        if (0 < vsprintf(tx_buf, fmt, argp)) // build string
        {
            ble_tx_len = strlen(tx_buf);
            ble_tx_size = ble_comm_service.write( tx_buf, ble_tx_len );
            uint8_t retry_cnt = 0;
            while((ble_tx_size == 0)&&(ble_tx_len>0))
            {
                ble_tx_size = ble_comm_service.write( tx_buf, ble_tx_len );    
                retry_cnt++;
                if(retry_cnt >=3)
                {
                    break;
                }
            }
        }    
    }  
}

void WM1110_BLE::clearRecbuf( void )
{
    memset(rx_buf,0,244);
    rx_length = 0;
}

void WM1110_BLE::ble_scanner_evt_handler( ble_gap_evt_adv_report_t* report )
{
    PRINT_LOCATION();
    uint8_t len = 0;
    uint8_t buffer[32];
    memset(buffer, 0, sizeof(buffer));
    len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, buffer, sizeof(buffer));
    if (len)    
    {
        uint8_t beacon_data_len = 0;
        uint8_t beacon_data_type = 0;
        uint16_t company_identifier = 0;
        beacon_data_len = buffer[3];
        beacon_data_type = buffer[2];
        memcpy(( uint8_t * )( &company_identifier ), buffer, 2 );
        if( beacon_data_type == BEACON_DATA_TYPE )
        {
            if( company_identifier == COMPANY_IDENTIFIER )
            {
                if( beacon_data_len == BEACON_DATA_LEN )
                {
                    bool res0 = true, res1 = true, res2 = true, res3 = true, res4 = true, res5 = true;
                    for( uint8_t j = 0; j < BLE_BEACON_BUF_MAX; j++ )
                    {
                        res1 = buf_value_compare( scaned_beacon_buf[j].uuid, ( uint8_t *)( buffer + 4 ), 16 );
                        res2 = buf_value_compare(( uint8_t *)( &scaned_beacon_buf[j].major ), ( uint8_t *)( buffer + 20 ), 2 );
                        res3 = buf_value_compare(( uint8_t *)( &scaned_beacon_buf[j].minor ), ( uint8_t *)( buffer + 22 ), 2 );
                        // res4 = buf_value_compare(( uint8_t *)( &ble_beacon_buf[j].rssi ), report->rssi, 1 );
                        res5 = buf_value_compare(( uint8_t *)( &scaned_beacon_buf[j].mac ), ( uint8_t *)( report->peer_addr.addr ), 6 );
                        if( res1 && res2 && res3 && res4 && res5 ) // all is same, don't save the scan result
                        {
                            res0 = false;
                            break;
                        }

                    }
                    if( res0 )
                    {
                        if(( scaned_beacon_num < BLE_BEACON_BUF_MAX ) && ( scaned_beacon_buf[scaned_beacon_num].company_id == 0 ))
                        {
                            uint32_t utc = 0;
                            // TODO - get ble scan utc
                            scaned_beacon_buf[scaned_beacon_num].utc = utc;
                            scaned_beacon_buf[scaned_beacon_num].company_id = company_identifier;
                            memcpy( scaned_beacon_buf[scaned_beacon_num].uuid, buffer + 4, 16 );
                            memcpy(( uint8_t *)( &scaned_beacon_buf[scaned_beacon_num].major ), buffer + 20, 2 );
                            memcpy(( uint8_t *)( &scaned_beacon_buf[scaned_beacon_num].minor ), buffer + 22, 2 );
                            memcpy(( uint8_t *)( &scaned_beacon_buf[scaned_beacon_num].rssi ), buffer + 24, 1 );
                            memcpy(( uint8_t *)( &scaned_beacon_buf[scaned_beacon_num].rssi_ ), ( uint8_t *)( &report->rssi ), 1 );
                            memcpy(( uint8_t *)( &scaned_beacon_buf[scaned_beacon_num].mac ), ( uint8_t *)( report->peer_addr.addr ), 6 );
                            scaned_beacon_num ++;
                        }   
                    }
                }
            }
        }
    }
    Bluefruit.Scanner.resume();
}

BleRunState WM1110_BLE::getBleStatus( void )
{
    return run_status;
}

#endif


WM1110_BLE wm1110_ble;

