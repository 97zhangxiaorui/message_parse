#ifndef __LIST_H_
#define __LIST_H_

#pragma pack(push)
#pragma pack(1)
typedef struct List
{
	void *pContent;
	struct List *pNext;
	struct List *pPre;
}ListObj, *ListPtr;
#pragma pack(pop)

/*
*funtion   : add node for pDestList
*pSrcList  : source list
*return    : NULL failed , ListPtr ok
*/
ListPtr AddListNode(ListPtr *pDestList);

/*
*funtion    : add node for pDestList
*nNodeNumber: number of the list node
*pSrcList   : source list
*return     : -1 failed , 0 ok
*/
int AddListNodes(ListPtr *pDestList, int nNodeNumber);

/*
*funtion   : destory list
*pSrcList  : the name of destory list 
*return    : -1 failed , 0 ok
*/
int DestoryList(ListPtr pSrcList);

#endif