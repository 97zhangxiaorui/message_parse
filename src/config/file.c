#include "file.h"

int IsPath(char *pFilePath)
{
	DIR *pDir = NULL;

	pDir = opendir(pFilePath);
	if ( NULL == pDir )
	{
		printf("open dir path %s failed, error: %s\n", pFilePath, strerror(errno));
		return -1;
	}
	closedir(pDir);
	return 0;
}

int IsFile(char *pFileName)
{
	if(access(pFileName,F_OK)==0)
	{
		return 0;
   	}
   	else
   	{
   		return -1;
   	}
}

int GetFileList(char *pFilePath, ListPtr *pFileList)
{
	// not check param, user mast ensure params is not empty

	DIR *pDir = NULL;
	struct dirent *pDirent = NULL;
	int i = 0;
	char ChildPath[1024] = { 0 };
	//ListPtr pTmpList = *pFileList;
	ListPtr pTmpList = *pFileList; //gpFileList;
	//ListPtr pTmpNextList = NULL;


	pDir = opendir(pFilePath);
	if ( NULL == pDir )
	{
		printf("open dir path %s failed, error: %s\n", pFilePath, strerror(errno));
		return -1;
	}

	pTmpList = AddListNode(pFileList);
	while (NULL != (pDirent = readdir(pDir)))
	{
		//unsigned int nFileType = 0;
		//memcpy(&nFileType, &pDirent->d_type, sizeof( pDirent->d_type));
		if ( pDirent->d_type & DT_DIR )
		{
			/*
			if ( !strcmp(pDirent->d_name,".") || !strcmp(pDirent->d_name,"..") )
			{
				continue;
			}

			memset(ChildPath, 0, sizeof(ChildPath));
			sprintf(ChildPath, "%s/%s", pFilePath, pDirent->d_name);
			GetFileList(ChildPath, pFileList);
			*/
			//dir pass
			continue;
		}
		else
		{
			if ( NULL == pTmpList )
			{
				pTmpList = AddListNode(pFileList);
			}

			int nNum = strlen(pFilePath) + strlen(pDirent->d_name) + 2;
			pTmpList->pContent = Malloc(nNum);
			memset(pTmpList->pContent, 0, nNum);
			sprintf(pTmpList->pContent, "%s/%s", pFilePath, pDirent->d_name);
			pTmpList = NULL;
		}	
	}	

	closedir(pDir);
	return 0;
}