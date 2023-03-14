#include "list.h"
#include "malloc.h"

ListPtr AddListNode(ListPtr *pDestList)
{
	ListPtr pTmpList = Malloc(sizeof(ListObj));
	pTmpList->pNext = NULL;
	pTmpList->pPre = NULL;
	pTmpList->pContent = NULL;
	if ( NULL == *pDestList )
	{
		*pDestList = pTmpList;
	}
	else
	{
		ListPtr pListEnd = *pDestList;
		while ( NULL != pListEnd->pNext )
		{
			pListEnd = pListEnd->pNext;
		}
		pListEnd->pNext = pTmpList;
		pTmpList->pPre = pListEnd;
	}

	return pTmpList;
}

int AddListNodes(ListPtr *pDestList, int nNodeNumber)
{
	int i = 0;
	ListPtr pList = NULL;

	if ( NULL != pDestList )
	{
		pList = *pDestList;
		while ( NULL != pList->pNext )
		{
			pList = pList->pNext;
		}
	}

	for ( ; i < nNodeNumber; i++ )
	{
		ListPtr pTmpList = (ListPtr) Malloc(sizeof(ListObj));
		pTmpList->pNext = NULL;
		pTmpList->pPre = NULL;
		pTmpList->pContent = NULL;

		if ( NULL != pList )
		{
			pList->pNext = pTmpList;
			pTmpList->pPre = pList;
			pList = pList->pNext;
			continue;
		}

		*pDestList = pTmpList;
		pList = *pDestList;
	}
	return 0;
}

int DestoryList(ListPtr pSrcList)
{
	while ( NULL != pSrcList )
	{
		if ( NULL != pSrcList->pNext )
		{
			pSrcList = pSrcList->pNext;
			Free(pSrcList->pPre);
			pSrcList->pPre = NULL;
			continue;
		}
		Free(pSrcList);
		pSrcList = NULL;
	}

	return 0;
}
