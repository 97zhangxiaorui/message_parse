#ifndef __ASSETS_H__
#define __ASSETS_H__

#include "../comm/const.h"

#pragma pack(push)
#pragma pack(1)

typedef struct _AssetsFingerprint_{
	u_int32_t uIp;
	DeviceFingerprint Fingerpint;
}AssetsFingerprint;
#pragma pack(pop)

#define ASSETS_HASH_SIZE 1024

void PrintHash();
void UpdateFingerprint(DeviceFingerprint *pSrcFinger, DeviceFingerprint *pDstFinger);
char* UpdateAssets( AssetsFingerprint *pAassets, char *pOutFingerprint, int nOutLen);
#endif