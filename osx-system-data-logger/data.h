//
//  data.h
//  osx-system-data-logger
//
//  Created by Karol Dzitkowski on 08.12.2014.
//  Copyright (c) 2014 Karol Dzitkowski. All rights reserved.
//

#ifndef osx_system_data_logger_data_h
#define osx_system_data_logger_data_h

typedef struct
{
    time_t timestamp;
    float core_voltage;
    float cpu_temp;
    float gpu_temp;
} data;

data getData();
char* getDataLabels(const char sep);
char* dataToString(data* data);
int readData(char* data_file, data* d);

#endif
