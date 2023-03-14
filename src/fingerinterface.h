#ifndef __FINGERINTERFACE_H_
#define __FINGERINTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fingerprint.h"

int InitFinger(char *pFingerprintPath, char *pRulePath);

int GetFingerprintFromNetwork(FingerHandle *pFingerHandle, char* pDeviceNetworkName, char *pFingerFilePath);

int GetFingerprintFromDetection(char *pIpAddress, int nProtocolType, int nProtocolPort, char *pSnmpKey, char *pSnmpVersion, char *pOutBuffer, int nBufferLen);

//int GetFingerprintFromPcapFile(char* pPcapFileName, char *pFingerConfigPath, FingerHandle *pFingerHandle);
int GetFingerprintFromPcapFile(char* pPcapFileName, FingerHandle *pFingerHandle);

char* GetFingerprintFromByts(unsigned char *pBytes, char *pOutFingerprint, int nOutFingerLen);

#endif 