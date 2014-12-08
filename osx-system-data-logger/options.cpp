//
//  options.cpp
//  osx-system-data-logger
//
//  Created by Karol Dzitkowski on 08.12.2014.
//  Copyright (c) 2014 Karol Dzitkowski. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include "options.h"
#include "files_management.h"

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

