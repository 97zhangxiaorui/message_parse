#ifndef __FINGERPRINT_H_
#define __FINGERPRINT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comm/list.h"
#include "comm/const.h"

typedef struct Port
{
	unsigned int DesPort;
	unsigned int SrcPort;
	//FingerprintPtr pFingerprint;
	ListPtr pList;
}PortObj,*PortPtr;

//callback typdef
typedef int (FingerHandle) (char *pFinger);

void InitFingerCallback( FingerHandle *pFingerHandle );

/*
*pBytes      : network package
*pNetworkMsg : network message
*return      : fingerprint or null
*/
FingerprintPtr GetFingerprintFromBytes(unsigned char *pBytes, NetworkMessagePtr pNetworkMsg);

/*
*pBytes : net package
*return : 
*/
void NetworkPacketHandle(unsigned char *pBytes, char *pOutFingerprint, int nOutFingerLen);

/*
*pBytes : net package
*return : -1 error 0 ok
*/
int GetNetworkMessage(unsigned char *pBytes, NetworkMessagePtr pMsg);

/*
*pBytes            : network package
*pNetworkQuintuple : ip,mac,transport protocol type
*/
FingerprintPtr FilterFingerprint(NetworkMessagePtr pMsg);

/*
*pFingerprintPath : fingerprint file path
*return           : 
*/
void InitFingerprint(char *pFingerprintPath);

/*
*pFileName: fingerprint file name
*return   : -1 failed , 0 ok
*/
int GetFingerprint(char *pFileName, FingerprintPtr pFinger);

/*
*function : fill fingerprint struct
*nIndex   : fingerprint member index
*pFinger  : fingerprint 
*pSrc     : data
*/
int FillFingerprint(int nIndex, FingerprintPtr pFinger, char *pSrc);

/*
*pBytes      : network package
*pNetworkMsg : network message
*return      : void
*/
void GetPortFromBytes(unsigned char *pBytes, NetworkMessagePtr pMsg);

FingerprintPtr GetFingerprintByPort(int nSrcPort, int nDstPort);

#endif
