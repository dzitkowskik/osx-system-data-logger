/*
 * Apple System Management Control (SMC) Tool
 * Copyright (C) 2006 devnull
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smc.h"
#include "converters.h"

io_connect_t conn;

void printVal(SMCVal_t val)
{
    printf("  %-4s  [%-4s]  ", val.key, val.dataType);
    if (val.dataSize > 0)
    {
        if ((strcmp(val.dataType, DATATYPE_UINT8) == 0) ||
            (strcmp(val.dataType, DATATYPE_UINT16) == 0) ||
            (strcmp(val.dataType, DATATYPE_UINT32) == 0))
            printf("%u", UInt_to_u(val));
        else if (strcmp(val.dataType, DATATYPE_FP1F) == 0)
            printf("%f", FP1F_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_FP4C) == 0)
            printf("%f", FP4C_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_FP5B) == 0)
            printf("%f", FP5B_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_FP6A) == 0)
            printf("%f", FP6A_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_FP79) == 0)
            printf("%f", FP79_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_FP88) == 0)
            printf("%f", FP88_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_FPA6) == 0)
            printf("%f", FPA6_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_FPC4) == 0)
            printf("%f", FPC4_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_FPE2) == 0)
            printf("%f", FPE2_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_SP1E) == 0)
            printf("%f", SP1E_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_SP3C) == 0)
            printf("%f", SP3C_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_SP4B) == 0)
            printf("%f", SP4B_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_SP5A) == 0)
            printf("%f", SP5A_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_SP69) == 0)
            printf("%f", SP69_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_SP78) == 0)
            printf("%f", SP78_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_SP87) == 0)
            printf("%f", SP87_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_SP96) == 0)
            printf("%f", SP96_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_SPB4) == 0)
            printf("%f", SPB4_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_SPF0) == 0)
            printf("%f", SPF0_to_f(val));
        else if (strcmp(val.dataType, DATATYPE_SI16) == 0)
            printf("%d", SI16_to_i(val));
        else if (strcmp(val.dataType, DATATYPE_SI8) == 0)
            printf("%d", SI8_to_i(val));
        else if (strcmp(val.dataType, DATATYPE_PWM) == 0)
            printf("%f", PWM_to_f(val));
        else
            printBytesHex(val);
    }
    else
    {
        printf("no data\n");
    }
}

int getIntValue(SMCVal_t val, int* result)
{
    if (val.dataSize > 0)
    {
        if (strcmp(val.dataType, DATATYPE_SI16) == 0)
            *result = SI16_to_i(val);
        else if (strcmp(val.dataType, DATATYPE_SI8) == 0)
            *result = SI8_to_i(val);
        else return -1;
    }
    else return -1;
    return 0;
}

unsigned int getUIntValue(SMCVal_t val, unsigned int* result)
{
    if (val.dataSize > 0)
    {
        if ((strcmp(val.dataType, DATATYPE_UINT8) == 0) ||
            (strcmp(val.dataType, DATATYPE_UINT16) == 0) ||
            (strcmp(val.dataType, DATATYPE_UINT32) == 0))
            *result = UInt_to_u(val);
        else return -1;
    }
    else return -1;
    return 0;
}

int getFloatValue(SMCVal_t val, float* result)
{
    if (val.dataSize > 0)
    {
        if (strcmp(val.dataType, DATATYPE_FP1F) == 0)
            *result = FP1F_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_FP4C) == 0)
            *result = FP4C_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_FP5B) == 0)
            *result = FP5B_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_FP6A) == 0)
            *result = FP6A_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_FP79) == 0)
            *result = FP79_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_FP88) == 0)
            *result = FP88_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_FPA6) == 0)
            *result = FPA6_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_FPC4) == 0)
            *result = FPC4_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_FPE2) == 0)
            *result = FPE2_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_SP1E) == 0)
            *result = SP1E_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_SP3C) == 0)
            *result = SP3C_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_SP4B) == 0)
            *result = SP4B_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_SP5A) == 0)
            *result = SP5A_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_SP69) == 0)
            *result = SP69_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_SP78) == 0)
            *result = SP78_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_SP87) == 0)
            *result = SP87_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_SP96) == 0)
            *result = SP96_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_SPB4) == 0)
            *result = SPB4_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_SPF0) == 0)
            *result = SPF0_to_f(val);
        else if (strcmp(val.dataType, DATATYPE_PWM) == 0)
            *result = PWM_to_f(val);
        else return -1;
        }
    else return -1;
    return 0;
}

void printBytesHex(SMCVal_t val)
{
    int i;
    
    printf("(bytes");
    for (i = 0; i < val.dataSize; i++)
        printf(" %02x", (unsigned char) val.bytes[i]);
    printf(")\n");
}

kern_return_t _smcOpen(io_connect_t *conn)
{
    kern_return_t result;
    mach_port_t   masterPort;
    io_iterator_t iterator;
    io_object_t   device;
    
    result = IOMasterPort(MACH_PORT_NULL, &masterPort);
    
    CFMutableDictionaryRef matchingDictionary = IOServiceMatching("AppleSMC");
    result = IOServiceGetMatchingServices(masterPort, matchingDictionary, &iterator);
    if (result != kIOReturnSuccess)
    {
        printf("Error: IOServiceGetMatchingServices() = %08x\n", result);
        return 1;
    }
    
    device = IOIteratorNext(iterator);
    IOObjectRelease(iterator);
    if (device == 0)
    {
        printf("Error: no SMC found\n");
        return 1;
    }
    
    result = IOServiceOpen(device, mach_task_self(), 0, conn);
    IOObjectRelease(device);
    if (result != kIOReturnSuccess)
    {
        printf("Error: IOServiceOpen() = %08x\n", result);
        return 1;
    }
    
    return kIOReturnSuccess;
}

kern_return_t _smcClose(io_connect_t conn)
{
    return IOServiceClose(conn);
}

void smcInit()
{
    _smcOpen(&conn);
}

void smcClose()
{
    _smcClose(conn);
}

kern_return_t smcCall(int index, SMCKeyData_t *inputStructure, SMCKeyData_t *outputStructure)
{
    size_t   structureInputSize;
    size_t   structureOutputSize;
    
    structureInputSize = sizeof(SMCKeyData_t);
    structureOutputSize = sizeof(SMCKeyData_t);
    
#if MAC_OS_X_VERSION_10_5
    return IOConnectCallStructMethod( conn, index,
                                     // inputStructure
                                     inputStructure, structureInputSize,
                                     // ouputStructure
                                     outputStructure, &structureOutputSize );
#else
    return IOConnectMethodStructureIStructureO( conn, index,
                                               structureInputSize, /* structureInputSize */
                                               &structureOutputSize,   /* structureOutputSize */
                                               inputStructure,        /* inputStructure */
                                               outputStructure);       /* ouputStructure */
#endif
    
}

kern_return_t smcReadKey(UInt32Char_t key, SMCVal_t *val)
{
    kern_return_t result;
    SMCKeyData_t  inputStructure;
    SMCKeyData_t  outputStructure;
    
    memset(&inputStructure, 0, sizeof(SMCKeyData_t));
    memset(&outputStructure, 0, sizeof(SMCKeyData_t));
    memset(val, 0, sizeof(SMCVal_t));
    
    inputStructure.key = _strtoul(key, 4, 16);
    sprintf(val->key, key);
    inputStructure.data8 = SMC_CMD_READ_KEYINFO;
    
    result = smcCall(KERNEL_INDEX_SMC, &inputStructure, &outputStructure);
    if (result != kIOReturnSuccess)
        return result;
    
    val->dataSize = outputStructure.keyInfo.dataSize;
    _ultostr(val->dataType, outputStructure.keyInfo.dataType);
    inputStructure.keyInfo.dataSize = val->dataSize;
    inputStructure.data8 = SMC_CMD_READ_BYTES;
    
    result = smcCall(KERNEL_INDEX_SMC, &inputStructure, &outputStructure);
    if (result != kIOReturnSuccess)
        return result;
    
    memcpy(val->bytes, outputStructure.bytes, sizeof(outputStructure.bytes));
    
    return kIOReturnSuccess;
}

kern_return_t smcWriteKey(SMCVal_t writeVal)
{
    kern_return_t result;
    SMCKeyData_t  inputStructure;
    SMCKeyData_t  outputStructure;
    
    SMCVal_t      readVal;
    
    result = smcReadKey(writeVal.key, &readVal);
    if (result != kIOReturnSuccess)
        return result;
    
    if (readVal.dataSize != writeVal.dataSize)
        return kIOReturnError;
    
    memset(&inputStructure, 0, sizeof(SMCKeyData_t));
    memset(&outputStructure, 0, sizeof(SMCKeyData_t));
    
    inputStructure.key = _strtoul(writeVal.key, 4, 16);
    inputStructure.data8 = SMC_CMD_WRITE_BYTES;
    inputStructure.keyInfo.dataSize = writeVal.dataSize;
    memcpy(inputStructure.bytes, writeVal.bytes, sizeof(writeVal.bytes));
    
    result = smcCall(KERNEL_INDEX_SMC, &inputStructure, &outputStructure);
    if (result != kIOReturnSuccess)
        return result;
    
    return kIOReturnSuccess;
}

UInt32 smcReadIndexCount(void)
{
    SMCVal_t val;
    smcReadKey("#KEY", &val);
    return _strtoul(val.bytes, val.dataSize, 10);
}

kern_return_t smcPrintAll(void)
{
    kern_return_t result;
    SMCKeyData_t  inputStructure;
    SMCKeyData_t  outputStructure;
    
    int           totalKeys, i;
    UInt32Char_t  key;
    SMCVal_t      val;
    
    totalKeys = smcReadIndexCount();
    totalKeys = 500;
    for (i = 0; i < totalKeys; i++)
    {
        memset(&inputStructure, 0, sizeof(SMCKeyData_t));
        memset(&outputStructure, 0, sizeof(SMCKeyData_t));
        memset(&val, 0, sizeof(SMCVal_t));
        
        inputStructure.data8 = SMC_CMD_READ_INDEX;
        inputStructure.data32 = i;
        
        result = smcCall(KERNEL_INDEX_SMC, &inputStructure, &outputStructure);
        if (result != kIOReturnSuccess)
            continue;
        
        _ultostr(key, outputStructure.key);
        
        result = smcReadKey(key, &val);
        printVal(val);
    }
    
    return kIOReturnSuccess;
}

kern_return_t smcWriteSimple(UInt32Char_t key, char *wvalue)
{
    kern_return_t result;
    SMCVal_t   val;
    int i;
    char c[3];
    for (i = 0; i < strlen(wvalue); i++)
    {
        sprintf(c, "%c%c", wvalue[i * 2], wvalue[(i * 2) + 1]);
        val.bytes[i] = (int) strtol(c, NULL, 16);
    }
    val.dataSize = i / 2;
    sprintf(val.key, key);
    result = smcWriteKey(val);
    if (result != kIOReturnSuccess)
        printf("Error: SMCWriteKey() = %08x\n", result);
    
    
    return result;
}