//
//  options.h
//  osx-system-data-logger
//
//  Created by Karol Dzitkowski on 08.12.2014.
//  Copyright (c) 2014 Karol Dzitkowski. All rights reserved.
//

#ifndef __osx_system_data_logger__options__
#define __osx_system_data_logger__options__

#define DEFAULT_DATA_FOLDER     "data"      // default data folder will be created in current dir
#define DEFAULT_DATA_FILE       "info.inf"
#define DEFAULT_LOG_FILE        "info.log"

typedef struct
{
    char* data_dir;
    char* data_file;
    char* log_file;
} options;

options* initOptions();
void freeOptions(options* opt);
options* getOptions(int argc, char** argv);
void printOptions(options* opt);

#endif /* defined(__osx_system_data_logger__options__) */
