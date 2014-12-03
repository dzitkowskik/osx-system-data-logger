//
//  main.cpp
//  osx-system-data-logger
//
//  Created by Karol Dzitkowski on 02.12.2014.
//  Copyright (c) 2014 Karol Dzitkowski. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <libgen.h>
#include <unistd.h>
#include <string>
#include "smc.h"
#include "errors.h"
#include "converters.h"
#include "files_management.h"
#include "signals_library.h"

#define DEFAULT_DATA_FOLDER     "data"      // default data folder will be created in current dir
#define DEFAULT_DATA_FILE       "info.inf"
#define DEFAULT_LOG_FILE        "info.log"

volatile sig_atomic_t do_work = 1;

typedef struct
{
    char* data_dir;
    char* data_file;
    char* log_file;
} options;

typedef struct
{
    float core_voltage;
    float cpu_temp;
    float gpu_temp;
} data;

const char* data_labels = "CoreVoltage,CpuTemp,GpuTemp";

options* initOptions()
{
    options* opt = (options*)malloc(sizeof(options));
    if(NULL == opt) return NULL;
    else
    {
        opt->data_dir = NULL;
        opt->data_file = NULL;
        opt->log_file = NULL;
    }
    return opt;
}

void freeOptions(options* opt)
{
    free(opt->data_dir);
    free(opt->data_file);
    free(opt->log_file);
    free(opt);
}

options* getOptions(int argc, char** argv)
{
    int c;
    opterr = 0;
    options* opt = initOptions();
    if (NULL == opt) return NULL;
    
    while ((c = getopt (argc, argv, "nd:")) != -1)
        switch (c)
        {
            case 'f':
                opt->data_file = strdup(optarg);
                break;
            case 'd':
                opt->data_dir = strdup(optarg);
                break;
            case 'l':
                opt->log_file = strdup(optarg);
                break;
            case '?':
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return NULL;
            default:
                abort();
        }

    if(NULL == opt->data_dir)
    {
        char* working_dir = GetCurrentPath();
        opt->data_dir = AllocCombinedPath(working_dir, DEFAULT_DATA_FOLDER);
        CombinePaths(opt->data_dir, working_dir, DEFAULT_DATA_FOLDER);
        free(working_dir);
    }
    if(NULL == opt->data_file)
    {
        opt->data_file = AllocCombinedPath(opt->data_dir, DEFAULT_DATA_FILE);
        CombinePaths(opt->data_file, opt->data_dir, DEFAULT_DATA_FILE);
    }
    if(NULL == opt->log_file)
    {
        opt->log_file = AllocCombinedPath(opt->data_dir, DEFAULT_LOG_FILE);
        CombinePaths(opt->log_file, opt->data_dir, DEFAULT_LOG_FILE);
    }
    return opt;
}

void printOptions(options* opt)
{
    printf("Data folder dir: %s\n", opt->data_dir);
    printf("Info file path: %s\n", opt->data_file);
    printf("Log file path: %s\n", opt->log_file);
}

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

char* dataToString(data* data)
{
    std::string var = std::to_string(data->core_voltage) + ","
        + std::to_string(data->cpu_temp) + ","
        + std::to_string(data->gpu_temp);
    return strdup(var.c_str());
}

int readData(options* opt, data* d)
{
    int fd = OpenFile(opt->data_file, O_RDONLY);
    ssize_t charRead = SafeRead(fd, (char*)d, sizeof(data));
    if (charRead != sizeof(data))
        return -1;
    return 0;
}

void sigintHandler(int sig)
{
    printf("Program captured SIGINT signal! - closing...\n");
    do_work=0;
}

void initProgram()
{
    if(setHandler(sigintHandler, SIGINT)) FILE_ERR("Can't set sigint handler");
}

int main(int argc, char** argv)
{
    printf("PROGRAM RUNS\n");
    
    // init, get options and init SMC
    initProgram();
    options* opt = getOptions(argc, argv);
    CHECK_ERROR_WHEN_NULL( opt );
    smcInit();
    printOptions(opt);
    
    // Create dir and open info file
    CHECK_ERROR( CreateDirIfNotExists(opt->data_dir) );
    int info_fd = OpenFile(opt->data_file, O_WRONLY | O_APPEND | O_CREAT);
    CHECK_ERROR( info_fd );
    
    // Open/Create log file and write column names if file does not exist
    bool logCreate = false;
    if(-1 == access(opt->log_file, F_OK)) logCreate = true;
    int log_fd = OpenFile(opt->log_file, O_WRONLY | O_APPEND | O_CREAT);
    CHECK_ERROR( log_fd );
    if(logCreate)
    {
        size_t data_labels_size = strlen(data_labels)*sizeof(char);
        ssize_t data_labels_written = SafeWriteLine(log_fd, (char*)data_labels, data_labels_size);
        if (data_labels_written != data_labels_size)
            FILE_ERR("Not all data labels written to log file");
    }
    
    int i = 0;
    while (do_work) // until sigint won't interrupt
    {
        data pcData = getData();
        // Write to info file as binary serialized data
        ssize_t charWritten = SafeWriteLine(info_fd, (char*)&pcData, sizeof(data));
        if (charWritten != sizeof(data))
            FILE_ERR("Not all data written to info file");
        
        // Write to log file in csv format
        char* pdDataText = dataToString(&pcData);
        size_t expected_size = strlen(pdDataText)*sizeof(char);
        charWritten = SafeWriteLine(log_fd, pdDataText, expected_size);
        if (charWritten != expected_size)
            FILE_ERR("Not all data written to log file");
        
        sleep(1);
        printf("Created %d entries\n", i++);
    }
    
    // Close info file, SMC and clear resources
    CHECK_ERROR( CloseFile(info_fd) );
    CHECK_ERROR( CloseFile(log_fd) );
    smcClose();
    freeOptions(opt);
    printf("CLOSED\n");
    return 0;
}
