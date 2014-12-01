//
//  files_management.h
//  osx-system-data-logger
//
//  Created by Karol Dzitkowski on 01.12.2014.
//  Copyright (c) 2014 Karol Dzitkowski. All rights reserved.
//

#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include <grp.h>
#include <langinfo.h>
#include <stdint.h>
#include <pwd.h>

#ifndef FILE_ERR
#define FILE_ERR(source) (fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     perror(source),kill(0,SIGKILL),\
                         exit(EXIT_FAILURE))
#endif
#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression) \
    ({ \
        long int _result; \
        do _result = (long int) (expression); \
        while (_result == -1L && errno == EINTR); \
        _result; \
     })
#endif
#ifndef CHECK_ERROR
#define CHECK_ERROR(expression) \
    ({ \
        if(-1 == expression){ \
            fprintf(stderr,"%s:%d\n",__FILE__,__LINE__); \
            kill(0,SIGKILL); \
            exit(EXIT_FAILURE);} \
    })
#endif
#ifndef TEMP_FAILURE_RETRY_WHEN_NULL
#define TEMP_FAILURE_RETRY_WHEN_NULL(expression) \
    ({ \
        void* _result; \
        do _result = (void*) (expression); \
        while (_result == NULL && errno == EINTR); \
        _result; \
     })
#endif
#ifndef CHECK_ERROR_WHEN_NULL
#define CHECK_ERROR_WHEN_NULL(expression) \
    ({ \
        if(NULL == (void*)(expression) ){ \
            fprintf(stderr,"%s:%d\n",__FILE__,__LINE__); \
            kill(0,SIGKILL); \
            exit(EXIT_FAILURE);} \
    })
#endif

int OpenFile(char* path, int flags);
int CreateDir(char* nameOfDirectory);
int CreateFile(char* nameOfFile);
int CloseFile(int fileDescriptor);
int SetFileSizeWithTruncate(int fileDescriptor, off_t size);
int SetFileSizeWithLseek(int fileDescriptor, off_t size);
int CreateFileWithSizeUsingTruncate(char* nameOfFile, off_t size);
int CreateFileWithSizeUsingLseek(char* nameOfFile, off_t size);
int CreateSymbolicLink(char* nameOfSymbolicLink, char* pathToFile);
int ChangeDirectory(char* path);
int SafeRead(int fd, char* buf, size_t size);
ssize_t SafeWrite(int fd, char* buf, size_t size);
char* ReadLine(int fd);
FILE* OpenStream(char* fileName, char* mode);
int CloseStream(FILE* file);
DIR* OpenDirectory(char* path);
int CloseDirectory(DIR* dirp);
struct dirent* ReadDirectory(DIR *dirp);
void PrintfFileStats(struct dirent *dp);
int LockWholeFile(int fileDesc);
int UnlockWholeFile(int fileDesc);
char* ReadLineFromStream(FILE* file);
struct dirent* GetFileDirent(char* path, char* name);