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

void printBytesHex(SMCVal_t val)
{
    int i;
    
    printf("(bytes");
    for (i = 0; i < val.dataSize; i++)
        printf(" %02x", (unsigned char) val.bytes[i]);
    printf(")\n");
}

kern_return_t SMCOpen(io_connect_t *conn)
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

kern_return_t SMCClose(io_connect_t conn)
{
    return IOServiceClose(conn);
}

void smc_init()
{
    SMCOpen(&conn);
}

void smc_close()
{
    SMCClose(conn);
}

kern_return_t SMCCall(int index, SMCKeyData_t *inputStructure, SMCKeyData_t *outputStructure)
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

kern_return_t SMCReadKey(UInt32Char_t key, SMCVal_t *val)
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
    
    result = SMCCall(KERNEL_INDEX_SMC, &inputStructure, &outputStructure);
    if (result != kIOReturnSuccess)
        return result;
    
    val->dataSize = outputStructure.keyInfo.dataSize;
    _ultostr(val->dataType, outputStructure.keyInfo.dataType);
    inputStructure.keyInfo.dataSize = val->dataSize;
    inputStructure.data8 = SMC_CMD_READ_BYTES;
    
    result = SMCCall(KERNEL_INDEX_SMC, &inputStructure, &outputStructure);
    if (result != kIOReturnSuccess)
        return result;
    
    memcpy(val->bytes, outputStructure.bytes, sizeof(outputStructure.bytes));
    
    return kIOReturnSuccess;
}

kern_return_t SMCWriteKey(SMCVal_t writeVal)
{
    kern_return_t result;
    SMCKeyData_t  inputStructure;
    SMCKeyData_t  outputStructure;
    
    SMCVal_t      readVal;
    
    result = SMCReadKey(writeVal.key, &readVal);
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
    
    result = SMCCall(KERNEL_INDEX_SMC, &inputStructure, &outputStructure);
    if (result != kIOReturnSuccess)
        return result;
    
    return kIOReturnSuccess;
}

UInt32 SMCReadIndexCount(void)
{
    SMCVal_t val;
    
    SMCReadKey("#KEY", &val);
    return _strtoul(val.bytes, val.dataSize, 10);
}

kern_return_t SMCPrintAll(void)
{
    kern_return_t result;
    SMCKeyData_t  inputStructure;
    SMCKeyData_t  outputStructure;
    
    int           totalKeys, i;
    UInt32Char_t  key;
    SMCVal_t      val;
    
    totalKeys = SMCReadIndexCount();
    totalKeys = 500;
    for (i = 0; i < totalKeys; i++)
    {
        memset(&inputStructure, 0, sizeof(SMCKeyData_t));
        memset(&outputStructure, 0, sizeof(SMCKeyData_t));
        memset(&val, 0, sizeof(SMCVal_t));
        
        inputStructure.data8 = SMC_CMD_READ_INDEX;
        inputStructure.data32 = i;
        
        result = SMCCall(KERNEL_INDEX_SMC, &inputStructure, &outputStructure);
        if (result != kIOReturnSuccess)
            continue;
        
        _ultostr(key, outputStructure.key);
        
        result = SMCReadKey(key, &val);
        printVal(val);
    }
    
    return kIOReturnSuccess;
}

kern_return_t SMCPrintCpus(void)
{
    kern_return_t result;
    SMCVal_t      val;
    UInt32Char_t  key;
    int           totalCpus, i;
    return kIOReturnSuccess;
}

kern_return_t SMCPrintFans(void)
{
    kern_return_t result;
    SMCVal_t      val;
    UInt32Char_t  key;
    int           totalFans, i;
    
    result = SMCReadKey("FNum", &val);
    if (result != kIOReturnSuccess)
        return kIOReturnError;
    
    totalFans = _strtoul(val.bytes, val.dataSize, 10);
    printf("Total fans in system: %d\n", totalFans);
    
    for (i = 0; i < totalFans; i++)
    {
        printf("\nFan #%d:\n", i);
        sprintf(key, "F%dAc", i);
        SMCReadKey(key, &val);
        printf("    Actual speed : %.0f\n", _strtof(val.bytes, val.dataSize, 2));
        sprintf(key, "F%dMn", i);
        SMCReadKey(key, &val);
        printf("    Minimum speed: %.0f\n", _strtof(val.bytes, val.dataSize, 2));
        sprintf(key, "F%dMx", i);
        SMCReadKey(key, &val);
        printf("    Maximum speed: %.0f\n", _strtof(val.bytes, val.dataSize, 2));
        sprintf(key, "F%dSf", i);
        SMCReadKey(key, &val);
        printf("    Safe speed   : %.0f\n", _strtof(val.bytes, val.dataSize, 2));
        sprintf(key, "F%dTg", i);
        SMCReadKey(key, &val);
        printf("    Target speed : %.0f\n", _strtof(val.bytes, val.dataSize, 2));
        SMCReadKey("FS! ", &val);
        if ((_strtoul(val.bytes, 2, 16) & (1 << i)) == 0)
            printf("    Mode         : auto\n");
        else
            printf("    Mode         : forced\n");
    }
    
    return kIOReturnSuccess;
}

void usage(char* prog)
{
    printf("Apple System Management Control (SMC) tool %s\n", VERSION);
    printf("Usage:\n");
    printf("%s [options]\n", prog);
    printf("    -f         : fan info decoded\n");
    printf("    -h         : help\n");
    printf("    -k <key>   : key to manipulate\n");
    printf("    -l         : list all keys and values\n");
    printf("    -r         : read the value of a key\n");
    printf("    -w <value> : write the specified value to a key\n");
    printf("    -v         : version\n");
    printf("\n");
}

kern_return_t SMCWriteSimple(UInt32Char_t key, char *wvalue)
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
    result = SMCWriteKey(val);
    if (result != kIOReturnSuccess)
        printf("Error: SMCWriteKey() = %08x\n", result);
    
    
    return result;
}