//
//  data.cpp
//  osx-system-data-logger
//
//  Created by Karol Dzitkowski on 08.12.2014.
//  Copyright (c) 2014 Karol Dzitkowski. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "data.h"
#include "smc.h"
#include "errors.h"
#include "converters.h"
#include "files_management.h"

using namespace std;

data getData()
{
    data d;
    kern_return_t result = kIOReturnSuccess;
    SMCVal_t val;
    result = smcReadKey(const_cast<char*>(SMC_KEY_CORE_VOLTAGE), &val);
    CHECK_ERROR( getFloatValue(val, &d.core_voltage) );
    result = smcReadKey(const_cast<char*>(SMC_KEY_CPU_TEMP), &val);
    CHECK_ERROR( getFloatValue(val, &d.cpu_temp) );
    result = smcReadKey(const_cast<char*>(SMC_KEY_GPU_TEMP), &val);
    CHECK_ERROR( getFloatValue(val, &d.gpu_temp) );
    return d;
}

char* getDataLabels(const char sep)
{
    string var =
        string(SMC_NAME_CORE_VOLTAGE) + sep +
        string(SMC_NAME_CPU_TEMP) + sep +
        string(SMC_NAME_GPU_TEMP);
    
    return strdup(var.c_str());
}

char* dataToString(data* data)
{
    string var =
        to_string(data->core_voltage) + "," +
        to_string(data->cpu_temp) + "," +
        to_string(data->gpu_temp);
    return strdup(var.c_str());
}

int readData(char* data_file, data* d)
{
    int fd = OpenFile(data_file, O_RDONLY);
    ssize_t charRead = SafeRead(fd, (char*)d, sizeof(data));
    if (charRead != sizeof(data))
        return -1;
    return 0;
}