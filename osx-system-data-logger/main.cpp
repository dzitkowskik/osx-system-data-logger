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
#include "data.h"
#include "options.h"

volatile sig_atomic_t do_work = 1;

void sigintHandler(int sig)
{
    printf("Program captured SIGINT signal! - closing...\n");
    do_work=0;
}

options* initProgram(int argc, char** argv)
{
    // init, get options and init SMC
    if(setHandler(sigintHandler, SIGINT))
        FILE_ERR("Can't set sigint handler");
    options* opt = getOptions(argc, argv);
    CHECK_ERROR_WHEN_NULL( opt );
    smcInit();
    printOptions(opt);
    return opt;
}

void prepareFiles(int& info_fd, int& log_fd, options* opt)
{
    // Create dir and open info file
    CHECK_ERROR( CreateDirIfNotExists(opt->data_dir) );
    info_fd = OpenFile(opt->data_file, O_WRONLY | O_APPEND | O_CREAT);
    CHECK_ERROR( info_fd );
    
    // Open/Create log file and write column names if file does not exist
    bool logCreate = false;
    if(-1 == access(opt->log_file, F_OK)) logCreate = true;
    log_fd = OpenFile(opt->log_file, O_WRONLY | O_APPEND | O_CREAT);
    CHECK_ERROR( log_fd );
    if(logCreate)
    {
        char* data_labels = getDataLabels(',');
        size_t data_labels_size = strlen(data_labels)*sizeof(char);
        ssize_t data_labels_written = SafeWriteLine(log_fd, data_labels, data_labels_size);
        if (data_labels_written != data_labels_size)
            FILE_ERR("Not all data labels written to log file");
        free(data_labels);
    }
}

void terminateProgram(int info_fd, int log_fd, options* opt)
{
    // Close info file, SMC and clear resources
    CHECK_ERROR( CloseFile(info_fd) );
    CHECK_ERROR( CloseFile(log_fd) );
    smcClose();
    freeOptions(opt);
}

void work(int info_fd, int log_fd)
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
    
    free(pdDataText);
}

int main(int argc, char** argv)
{
    int info_fd;
    int log_fd;
    
    unsigned int speep_time_microseconds = 500*1000;
    
    options* opt = initProgram(argc, argv);
    prepareFiles(info_fd, log_fd, opt);
    
    int i = 1;
    while (do_work) // until sigint won't interrupt
    {
        work(info_fd, log_fd);
        usleep(speep_time_microseconds);
        printf("Appended %d entries\n", i++);
    }
    
    terminateProgram(info_fd, log_fd, opt);
    return 0;
}
