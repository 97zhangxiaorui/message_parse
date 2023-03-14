#ifndef __FILE_H_
#define __FILE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <error.h>
#include <errno.h>

#include "../comm/list.h"

#define CONTENT_LENGTH 1024

/*
*pFilePath : file path, not empty
*return    : 0 ok -1 error
*/
int IsPath(char *pFilePath);

/*
*pFileName : file name , not empty
*return    : 0 ok -1 error
*/
int IsFile(char *pFileName);

/*
*pFilePath : FILE PATH , not empty
*pFileList : out files list, not empty
*return    : 0 ok , -1 error
*/
int GetFileList(char *pFilePath, ListPtr *pFileList);

#endif
