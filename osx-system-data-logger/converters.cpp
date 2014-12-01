#include "converters.h"
#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UInt32 _strtoul(char *str, int size, int base)
{
    UInt32 total = 0;
    int i;
    
    for (i = 0; i < size; i++)
    {
        if (base == 16)
            total += str[i] << (size - 1 - i) * 8;
        else
            total += (unsigned char) (str[i] << (size - 1 - i) * 8);
    }
    return total;
}

void _ultostr(char *str, UInt32 val)
{
    str[0] = '\0';
    sprintf(str, "%c%c%c%c",
            (unsigned int) val >> 24,
            (unsigned int) val >> 16,
            (unsigned int) val >> 8,
            (unsigned int) val);
}

float _strtof(char *str, int size, int e)
{
    float total = 0;
    int i;
    
    for (i = 0; i < size; i++)
    {
        if (i == (size - 1))
            total += (str[i] & 0xff) >> e;
        else
            total += str[i] << (size - 1 - i) * (8 - e);
    }
    
    return total;
}

float FP1F_to_f(SMCVal_t val)
{
    return ntohs(*(UInt16*)val.bytes) / 32768.0;
}

float FP4C_to_f(SMCVal_t val)
{
    return ntohs(*(UInt16*)val.bytes) / 4096.0;
}

float FP5B_to_f(SMCVal_t val)
{
    return ntohs(*(UInt16*)val.bytes) / 2048.0;
}

float FP6A_to_f(SMCVal_t val)
{
    return ntohs(*(UInt16*)val.bytes) / 1024.0;
}

float FP79_to_f(SMCVal_t val)
{
    return ntohs(*(UInt16*)val.bytes) / 512.0;
}

float FP88_to_f(SMCVal_t val)
{
    return ntohs(*(UInt16*)val.bytes) / 256.0;
}

float FPA6_to_f(SMCVal_t val)
{
    return ntohs(*(UInt16*)val.bytes) / 64.0;
}

float FPC4_to_f(SMCVal_t val)
{
    return ntohs(*(UInt16*)val.bytes) / 16.0;
}

float FPE2_to_f(SMCVal_t val)
{
    return ntohs(*(UInt16*)val.bytes) / 4.0;
}

float SP1E_to_f(SMCVal_t val)
{
    return ((SInt16)ntohs(*(UInt16*)val.bytes)) / 16384.0;
}

float SP3C_to_f(SMCVal_t val)
{
    return ((SInt16)ntohs(*(UInt16*)val.bytes)) / 4096.0;
}

float SP4B_to_f(SMCVal_t val)
{
    return ((SInt16)ntohs(*(UInt16*)val.bytes)) / 2048.0;
}

float SP5A_to_f(SMCVal_t val)
{
    return ((SInt16)ntohs(*(UInt16*)val.bytes)) / 1024.0;
}

float SP69_to_f(SMCVal_t val)
{
    return ((SInt16)ntohs(*(UInt16*)val.bytes)) / 512.0;
}

float SP78_to_f(SMCVal_t val)
{
    return ((SInt16)ntohs(*(UInt16*)val.bytes)) / 256.0;
}

float SP87_to_f(SMCVal_t val)
{
    return ((SInt16)ntohs(*(UInt16*)val.bytes)) / 128.0;
}

float SP96_to_f(SMCVal_t val)
{
    return ((SInt16)ntohs(*(UInt16*)val.bytes)) / 64.0;
}

float SPB4_to_f(SMCVal_t val)
{
    return ((SInt16)ntohs(*(UInt16*)val.bytes)) / 16.0;
}

float SPF0_to_f(SMCVal_t val)
{
    return (float)ntohs(*(UInt16*)val.bytes);
}

int SI8_to_i(SMCVal_t val)
{
    return (signed char)*val.bytes;
}

int SI16_to_i(SMCVal_t val)
{
    return ntohs(*(SInt16*)val.bytes);
}

float PWM_to_f(SMCVal_t val)
{
    return ntohs(*(UInt16*)val.bytes) * 100 / 65536.0;
}

unsigned UInt_to_u(SMCVal_t val)
{
    return (unsigned int) _strtoul((char *)val.bytes, val.dataSize, 10);
}
