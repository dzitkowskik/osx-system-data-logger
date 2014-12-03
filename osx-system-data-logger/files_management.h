//
//  files_management.h
//  osx-system-data-logger
//
//  Created by Karol Dzitkowski on 01.12.2014.
//  Copyright (c) 2014 Karol Dzitkowski. All rights reserved.
//

#ifndef FILES_MANAGEMENT_H
#define FILES_MANAGEMENT_H

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <grp.h>
#include <langinfo.h>
#include <stdint.h>
#include <pwd.h>

#if defined(WIN32)
#  define DIR_SEPARATOR '\\'
#else
#  define DIR_SEPARATOR '/'
#endif

#if defined(__cplusplus)
extern "C" {
#endif

char* GetCurrentPath();
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
ssize_t SafeWriteLine(int fd, char* buf, size_t size);
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
char* AllocCombinedPath(const char *path1, const char *path2);
void CombinePaths(char *destination, const char *path1, const char *path2);
int CreateDirInPathAndBackToCurrentDir(char* dirName, char* path, char* currentDir);
int CreateDirInPath(char* dirName, char* path);
int CreateDirIfNotExists(char* dirName);
    
#if defined(__cplusplus)
}
#endif

#endif // FILES_MANAGEMENT_H