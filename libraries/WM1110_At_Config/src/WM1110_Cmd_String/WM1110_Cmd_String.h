#ifndef _WM1110_CMD_STRING_H
#define _WM1110_CMD_STRING_H


/* AT Command strings. Commands start with AT */
/* General commands */

#define AT_VER                	"+VER"        		// Version
#define AT_CONFIG             	"+CONFIG" 			// All current configurations parameter
#define AT_DEFPARAM           	"+DEFPARAM" 		// All default configurations parameter

#define AT_SN                 	"+SN"				// Device Serial number

#define AT_HARDWAREVER			"+HARDWAREVER"		// Board hardware version 

#define AT_RESTORE          	"+RESTORE"			// Restore the current parameters to default values
#define AT_POS_INT          	"+POS_INT"			// Positioning interval
#define AT_SAMPLE_INT       	"+SAMPLE_INT"		// Default sensor sampling interval
#define AT_SENSOR           	"+SENSOR" 			// Equipment model name
#define AT_PLATFORM         	"+PLATFORM"			// Data access platform
#define AT_MEA              	"+MEA"  			// Measurement sensor data
#define AT_POS_DEL          	"+POS_DEL"			// Delete cached data
#define AT_POS_MSG          	"+POS_MSG"			// Query cache data

/* LoRaWAN network management commands */
#define AT_TYPE       "+TYPE"       				// LoRaWAN access mode (OTAA or ABP)
#define AT_BAND       "+BAND"      					// LoRaWAN region
#define AT_CHANNEL    "+CHANNEL"   					// LoRaWAN Sub-band for US915&AU915

#define AT_JOINEUI    "+APPEUI"      				// LoRaWAN AppEUI
#define AT_APPKEY     "+APPKEY"						// LoRaWAN AppKey
#define AT_DEUI       "+DEUI" 						// LoRaWAN DevEUI

#define AT_NWKKEY     "+NWKKEY"      				// LoRaWAN NwkKey
#define AT_NWKSKEY    "+NWKSKEY"					// LoRaWAN NwksKey     
#define AT_APPSKEY    "+APPSKEY"					// LoRaWAN AppsKey  
  
#define AT_DKEY       "+DKEY"						// Device Key
#define AT_DADDR      "+DADDR"      				// Device Addr
#define AT_DCODE      "+DCODE" 						// Device Code


//BLE commands
#define AT_DISCONNECT       "+DISCONNECT" 			// Disconnect ble

//User commands



#endif
