#include "apps_common_util.h"

bool buf_value_compare( uint8_t *a, uint8_t *b, uint8_t len )
{
    for( uint8_t i = 0; i < len; i++ )
    {
        if( a[i] != b[i] )
        {
            return false;
        }
    }
    return true;
}

void memcpyr( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    dst = dst + ( size - 1 );
    while( size-- )
    {
        *dst-- = *src++;
    }
}

void hexTonum(unsigned char *out_data, unsigned char *in_data, unsigned short Size) 
{
    for(unsigned char i = 0; i < Size; i++)
    {
        out_data[2*i] = (in_data[i]>>4);
        out_data[2*i+1] = (in_data[i]&0x0F);  
    }
    for(unsigned char i = 0; i < 2*Size; i++)
    {
        if (out_data[i] <= 9) 
        {
            out_data[i] = '0'+out_data[i];
        } 
        else if ((out_data[i] >= 0x0A) && (out_data[i] <= 0x0F)) 
        {
            out_data[i] = 'A'- 10 +out_data[i];
        } 
        else 
        {
            return;
        }
    }
}

void numTohex(unsigned char *out_data, unsigned char *in_data, unsigned short Size) 
{
    unsigned char temp;
    for(unsigned char i = 0; i < Size; i++)
    {
        temp = Char2Nibble(in_data[2*i]);
        temp = temp<<4;
        temp += Char2Nibble(in_data[2*i+1]);
        out_data[i] = temp;
    }
}

uint8_t Char2Nibble(char Char) 
{
    if (((Char >= '0') && (Char <= '9'))) 
    {
        return Char - '0';
    } 
    else if (((Char >= 'a') && (Char <= 'f'))) 
    {
        return Char - 'a' + 10;
    } 
    else if ((Char >= 'A') && (Char <= 'F')) 
    {
        return Char - 'A' + 10;
    } 
    else 
    {
        return 0xF0;
    }
}

int32_t isNum(char Char) 
{
    if ((Char >= '0') && (Char <= '9'))
    {
        return 0;
    } 
    else
    {
        return -1;
    }
}

int32_t isNums(const char *str, uint32_t Size) 
{
    for(uint8_t u8i = 0; u8i < Size; u8i++)
    {
        /*check if input is hex */
        if ((isNum(str[u8i]) == -1)) 
        {
            return -1;
        }
    }
    return 0;
}

int8_t ascii_4bit_to_hex(uint8_t ascii) {
    int8_t result = -1;
    if ((ascii >= '0') && (ascii <= '9')) {
        result = ascii - 0x30;
    } else if ((ascii >= 'a') && (ascii <= 'f')) {
        result = ascii - 'a' + 10;
    } else if ((ascii >= 'A') && (ascii <= 'F')) {
        result = ascii - 'A' + 10;
    }
    return result;
}



