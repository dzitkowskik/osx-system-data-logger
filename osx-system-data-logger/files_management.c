//
//  files_management.c
//  osx-system-data-logger
//
//  Created by Karol Dzitkowski on 01.12.2014.
//  Copyright (c) 2014 Karol Dzitkowski. All rights reserved.
//

#include "files_management.h"
#include "errors.h"
#define N 1024

int SafeRead(int fd, char* buf, size_t size)
{
	int offset = 0;
	int charRead = 0;
	do
	{
		charRead = (int)TEMP_FAILURE_RETRY(read(fd, buf, size));
		if(charRead < 0)
		{
			fprintf(stderr, "Błąd podczas czytania pliku:\n Error = %s\n", strerror(errno));
			return charRead;
		}
		if(charRead == 0) return offset;
		buf += charRead;
		offset += charRead;
		size -= charRead;
	}
	while(size > 0);
	return offset;
}

ssize_t SafeWrite(int fd, char* buf, size_t size)
{
	int charWritten;
	size_t offset;
	do
	{
		charWritten = (int)TEMP_FAILURE_RETRY(write(fd, buf, size));
		if(charWritten < 0)
		{
			fprintf(stderr, "Error in SafeWrite: Error = %s\n", strerror(errno));
			return charWritten;
		}
		buf += charWritten;
		offset += charWritten;
		size -= charWritten;
	}
	while(size > 0);
	return charWritten;
}

ssize_t SafeWriteLine(int fd, char* buf, size_t size)
{
    ssize_t charWritten = SafeWrite(fd, buf, size);
    ssize_t end_line = TEMP_FAILURE_RETRY(write(fd, "\n", 1));
    if(end_line < 0)
    {
        fprintf(stderr, "Error in SafeWriteLine: Error = %s\n", strerror(errno));
        return charWritten;
    }
    return charWritten;
}

int OpenFile(char* path, int flags)
{
    //flags: O_RDONLY, O_WRONLY, O_RDWR
	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH;
	int fd = (int)TEMP_FAILURE_RETRY(open(path, flags, mode));
	if(-1 == fd)
	{
		fprintf(stderr, "Error occured when opening a file: %s\n Error = %s\n", path, strerror(errno));
		if(errno == EACCES)
		{
			fprintf(stderr, "- permission denied!\n");
		} 
		return -1;
	}
	return fd;
}

int CreateDir(char* dirName)
{
	// Permissions read/write/search permissions for owner and group, and with read/search permissions for others
	mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
	if(-1 == mkdir(dirName, mode))
	{
		fprintf(stderr, "Error occured when creating directory: %s\n Error code = %d\n", dirName, errno);
		if(errno == EEXIST) 
		{
			fprintf(stderr, "- file with name [%s] already exists\n", dirName);
		}
		else if(errno == EACCES)
		{
			fprintf(stderr, "- permission denied!\n");
		} 
		return -1;
	}
	return 0;
}

int CreateDirIfNotExists(char* dirName)
{
    if (0 != access(dirName, F_OK))
    {
        if (ENOENT == errno) { // if file does not exist
            if(-1 == CreateDir(dirName)) return -1;
        }
        if (ENOTDIR == errno) {
            fprintf(stderr, "Not a directory!\n");
            return -1;
        }
    }
    return 0;
}

int CreateDirInPathAndBackToCurrentDir(char* dirName, char* path, char* currentDir)
{
    if(-1 == ChangeDirectory(path)) return -1;
    if (0 != access(dirName, F_OK))
    {
        if (ENOENT == errno) { // if file does not exist
            if(-1 == CreateDir(dirName)) return -1;
        }
        if (ENOTDIR == errno) {
            fprintf(stderr, "Not a directory!\n");
            return -1;
        }
    }
    if(-1 == ChangeDirectory(currentDir)) return -1;
    return 0;
}

int CreateDirInPath(char* dirName, char* path)
{
    int returnValue = 0;
    char* currentDir = GetCurrentPath();
    if(currentDir)
    {
        returnValue = CreateDirInPathAndBackToCurrentDir(dirName, path, currentDir);
        free(currentDir);
        return returnValue;
    }
    return -1;
}

int CreateDirRecursive(const char *dir)
{
    if(NULL == dir) return -1;
    char *tmp = (char*)malloc(strlen(dir)*sizeof(char));
    if(NULL == tmp) return -1;
    
    char *p = NULL;
    size_t len;
    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);
    if(tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for(p = tmp + 1; *p; p++)
        if(*p == '/')
        {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    mkdir(tmp, S_IRWXU);
    return 0;
}

char* GetCurrentPath()
{
    size_t size = 100;
    while(1)
    {
        char *buffer = (char *)malloc(size);
        if (getcwd (buffer, size) == buffer)
            return buffer;
        free(buffer);
        if (errno != ERANGE)
            return 0;
        size *= 2;
    }
}

char* AllocCombinedPath(const char *path1, const char *path2)
{
    size_t size = 1;
    if (path1 && *path1) size += strlen(path1);
    if (path2 && *path2) size += strlen(path2);
    return malloc(size*sizeof(char));
}

void CombinePaths(char *destination, const char *path1, const char *path2)
{
    if (path1 && *path1)
    {
        size_t len = strlen(path1);
        strcpy(destination, path1);
        
        if (destination[len - 1] == DIR_SEPARATOR)
        {
            if (path2 && *path2)
            {
                strcpy(destination + len, (*path2 == DIR_SEPARATOR) ? (path2 + 1) : path2);
            }
        }
        else
        {
            if (path2 && *path2)
            {
                if (*path2 == DIR_SEPARATOR)
                    strcpy(destination + len, path2);
                else
                {
                    destination[len] = DIR_SEPARATOR;
                    strcpy(destination + len + 1, path2);
                }
            }
        }
    }
    else if (path2 && *path2)
        strcpy(destination, path2);
    else
        destination[0] = '\0';
}

int CreateFile(char* name)
{
	//Permissions Owner - Read, Write, Execute; Group - Read, Execute; Others - Read
	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH;
	int fd = (int)TEMP_FAILURE_RETRY(open(name, O_WRONLY | O_CREAT | O_EXCL, mode));
	if(-1 == fd)
	{
		fprintf(stderr, "Error occured when creating a file: %s\n Error code = %d\n", name, errno);
		if(errno == EEXIST)
		{
			fprintf(stderr, "- file with name [%s] already exists\n", name);
		}
		else if(errno == EACCES)
		{
			fprintf(stderr, "- permission denied!\n");
		} 
		return -1;
	}
	return fd;
}

int CloseFile(int fd)
{
	if(-1 == TEMP_FAILURE_RETRY(close(fd)))
	{
		fprintf(stderr, "Error occured when closing file:\n Error code = %d\n", errno);
		return -1;
	}
	return 0;
}

int CreateFileWithSizeUsingTruncate(char* name, off_t size)
{
	int fd = CreateFile(name);
	if(-1 == fd) {return -1;}
	if(-1 == SetFileSizeWithTruncate(fd, size)) {return -1;}
	if(-1 == CloseFile(fd)) {return -1;}
	return 0;
}

int CreateFileWithSizeUsingLseek(char* name, off_t size)
{
	int fd = CreateFile(name);
	if(-1 == fd) {return -1;}
	if(-1 == SetFileSizeWithLseek(fd, size)) {return -1;}
	if(-1 == CloseFile(fd)) {return -1;}
	return 0;
}

int SetFileSizeWithTruncate(int fd, off_t size)
{
	//Setting the size of file
	if(-1 == TEMP_FAILURE_RETRY(ftruncate(fd, size)))
	{
		fprintf(stderr, "Error occured when setting file size:\n Error code = %d\n", errno);
		return -1;
	}
	return 0;
}

int SetFileSizeWithLseek(int fd, off_t size)
{
	char buf[1];
	buf[0] = '\0';
    
	//Setting the size of file
	if(-1 == lseek(fd, size-1, SEEK_SET))
	{
		fprintf(stderr, "Error occured when setting file size:\n Error code = %d\n", errno);
		return -1;
	}
	if(-1 == TEMP_FAILURE_RETRY(write(fd, buf, 1)))
	{
		fprintf(stderr, "Cannot put last byte the new file using write:\n Error = %s\n", strerror(errno));
		return -1;
	}	
	return 0;
}

int CreateSymbolicLink(char* nameOfSymbolicLink, char* pathToFile)
{
	if(-1 == symlink(pathToFile, nameOfSymbolicLink))
	{
		fprintf(stderr, "Error occured when creating symbolic link called [%s]:\n", nameOfSymbolicLink);
		fprintf(stderr, "Error = %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

int ChangeDirectory(char* path)
{
	// Posix spec. doesn't say anything about EINTR error in chdir function
	// But it is safer to use TEMP_FAILURE_RETRY anyway
	if(-1 == TEMP_FAILURE_RETRY(chdir(path)))
	{
		fprintf(stderr, "Cannot change working directory:\n Error = %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

char* ReadLine(int fd)
{
	char buf[N];
	char* result;

	int i = 0, bytesRead = 0;
	for(; i<N; i++)
	{
		buf[i] = 0;
	}
	bytesRead = SafeRead(fd, buf, N);
	if(bytesRead <= 0) return NULL;
	
	for(i=0; i<bytesRead; i++)
		if(buf[i]=='\n')
		{
			buf[i]=0;
			break;
		}
	result = malloc(i+1);
	if(result==NULL)
        FILE_ERR("malloc failed!");
	result[i]=0;
	memcpy(result, buf, i);
	if(-1 == lseek(fd, i - bytesRead + 1, SEEK_CUR))
		FILE_ERR("Error occured using lseek in ReadLine function!");
	return result;
}

FILE* OpenStream(char* fileName, char* mode)
{
	FILE* result = TEMP_FAILURE_RETRY_WHEN_NULL(fopen(fileName, mode));
	if(NULL == result)
	{
		fprintf(stderr, "Error occured in OpenFile function!\n");
		fprintf(stderr, "Cannot open file\n Error = %s\n", strerror(errno));
		return NULL;
	}
	return result;
}

int CloseStream(FILE* file)
{
    if(0 != fclose(file))
    {
        fprintf(stderr, "Can't close a file stream!\n Error = %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

DIR* OpenDirectory(char* path)
{
	DIR* result = opendir(path);
	if(NULL == result)
	{
		fprintf(stderr, "Cannot open directory %s\n Error = %s\n", path, strerror(errno));
		return NULL;
	}
	return result;
}

int CloseDirectory(DIR* dirp)
{
	if(-1 == TEMP_FAILURE_RETRY(closedir(dirp)))
	{
		fprintf(stderr, "Cannot close directory\n Error = %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

struct dirent* ReadDirectory(DIR *dirp)
{
	struct dirent *dp;
	errno = 0;
	dp = readdir(dirp);
	if (errno != 0)
	{
		fprintf(stderr, "Error occured while reading directory\n Error = %s\n", strerror(errno));
	}
	return dp;
}

void PrintfFileStats(struct dirent *dp)
{
    //STATS of file
    //struct stat contains:
    //st_mode - permissions
    //st_nlink - count of hard links
    //st_uid - owners id
    //st_gid - group id
    //st_size - size of file
    // and more... see: http://en.wikipedia.org/wiki/Stat_(system_call)
	struct stat     statbuf;
	struct passwd  *pwd;
	struct group   *grp;
	struct tm      *tm;
	char            datestring[256];

	if(-1 == stat(dp->d_name, &statbuf))
	{
		fprintf(stderr, "Cannot obtain information about file %s\n Error = %s\n", dp->d_name, strerror(errno));
	}
    /* Print out type, permissions, and number of links. */
	printf("%10.10d", statbuf.st_mode);
	printf("%4d", statbuf.st_nlink);
	fflush(stdout);
	/* Print out owner's name if it is found using getpwuid(). */
	if ((pwd = getpwuid(statbuf.st_uid)) != NULL)
	  printf(" %-8.8s", pwd->pw_name);
	else
	  printf(" %-8d", statbuf.st_uid);

	/* Print out group name if it is found using getgrgid(). */
	if ((grp = getgrgid(statbuf.st_gid)) != NULL)
	  printf(" %-8.8s", grp->gr_name);
	else
	  printf(" %-8d", statbuf.st_gid);

	/* Print size of file. */
	printf(" %9jd", (intmax_t)statbuf.st_size);

	tm = localtime(&statbuf.st_mtime);

	/* Get localized date string. */
	strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tm);

	printf(" %s %s\n", datestring, dp->d_name);
	return;
}

int LockWholeFile(int fileDesc)
{
	struct flock fl =
	{
		F_WRLCK, //l_type - lock type: F_WRLCK, F_RDLCK, F_UNLCK
		SEEK_SET, //l_whence - SEEK_SET, SEEK_CUR, SEEK_END
		0, //l_start - offset from l_whence
		0, //l_len - length - 0 = EOF
		0 //l_pid - pid of current process
	};
	fl.l_pid = getpid();	// set pid to current proces pid
	if(-1 == TEMP_FAILURE_RETRY(fcntl(fileDesc, F_SETLKW, &fl)))
	{
		fprintf(stderr, "Error occured when trying to lock the file\n Error = %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

int UnlockWholeFile(int fileDesc)
{
	struct flock fl =
	{
		F_UNLCK, //l_type - lock type: F_WRLCK, F_RDLCK, F_UNLCK
		SEEK_SET, //l_whence - SEEK_SET, SEEK_CUR, SEEK_END
		0, //l_start - offset from l_whence
		0, //l_len - length - 0 = EOF
		0 //l_pid - pid of current process
	};
	fl.l_pid = getpid();	// set pid to current proces pid
	if(-1 == fcntl(fileDesc, F_SETLK, &fl))
	{
		fprintf(stderr, "Error occured when trying to unlock the file\n Error = %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

char* ReadLineFromStream(FILE* file)
{
	char line[N];
	errno = 0;
	if(fgets(line, N, file) == NULL)
	{
		if(feof(file)) return NULL;
		else if(ferror(file))
		{
			fprintf(stderr, "Error occured while reading from file\n Error = %s\n", strerror(errno));
		}
	}
	return strdup(line);
}

struct dirent* GetFileDirent(char* path, char* name)
{
	DIR* dirp = OpenDirectory(path);
	struct dirent* dp;
	if(NULL == dirp) return NULL;
	do {
		errno = 0;
		if ((dp = ReadDirectory(dirp)) != NULL) 
		{
			if (strcmp(dp->d_name, name) == 0)
			{
				CloseDirectory(dirp);
				return dp;
			}	
		}	
	} while (dp != NULL);

	if (errno != 0)
	{
		fprintf(stderr, "Error while searching for file %s in directory %s\n", name, path);
		fprintf(stderr, "Error = %s\n", strerror(errno));
	}
	CloseDirectory(dirp);
	return NULL;
}


































