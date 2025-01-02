#ifndef _APPS_COMMON_UTIL_H_
#define _APPS_COMMON_UTIL_H_


#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/**
  * @brief  Compared of buf datas
  * 
  * @param  param a
  * 
  * @param  param b
  * 
  * @param  param len
  *  
  * @retval true: same  false: unlikeness
  */
bool buf_value_compare( uint8_t *a, uint8_t *b, uint8_t len );

/**
  * @brief  Copy data
  * 
  * @param  param dst
  * 
  * @param  param src
  * 
  * @param  param size
  *  
  * @retval 
  */
void memcpyr( uint8_t *dst, const uint8_t *src, uint16_t size );

/**
  * @brief  Hex to digital
  * 
  * @param  param out_data
  * 
  * @param  param in_data
  * 
  * @param  param Size
  *  
  * @retval 
  */
void hexTonum(unsigned char *out_data, unsigned char *in_data, unsigned short Size);

/**
  * @brief  digital to Hex
  * 
  * @param  param out_data
  * 
  * @param  param in_data
  * 
  * @param  param Size
  *  
  * @retval 
  */
void numTohex(unsigned char *out_data, unsigned char *in_data, unsigned short Size);

/**
  * @brief  Char to Nibble
  * 
  * @param  param Char
  * 
  * @retval uint8_t
  */
uint8_t Char2Nibble(char Char);

/**
 * @brief  Check if character in parameter is number
 * 
 * @param  Char for the number check
 *
 * @retval  int32_t
 */
int32_t isNum(char Char); 

/**
 * @brief  Check if character in parameter is number
 * 
 * @param  str for the number check
 * 
 * @param  Size for the number size we want check
 * 
 * @retval  int32_t
 */
int32_t isNums(const char *str, uint32_t Size);

/**
 * @brief  Change 4bit asscii to hex
 * 
 * @param  str for the number check
 * 
 * @param  Size for the number size we want check
 * 
 * @retval  int32_t
 */
int8_t ascii_4bit_to_hex(uint8_t ascii);



#ifdef __cplusplus
}
#endif


#endif
