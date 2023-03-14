#ifndef __TLS_H__
#define __TLS_H__

#include "../comm/const.h"

#pragma pack(push)
#pragma pack(1)
typedef struct __TLS_PROTOCAL_{
	char Type;					//0x14(20) ChangeCipherSpec, 0x15(21)Alert, 0x16(22)Handshake, 0x17(23)Application
	short int Version;          //主版本+子版本，3+0=SSLv3, 3+1=TLS1.0, 3+2=TLS1.1, 3+3=TLS1.2
	short int Length;
	char *pContent;
}TlsHead;

enum TlsType
{
	ChangeCipherSpec = 20,
	Alert,
	Handshake,
	Application
};
#pragma pack(pop)

int ParseTls(unsigned char *pBytes, FingerprintPtr pFinger, DeviceFingerprint *pDeviceFinger, int nPaylaodLen);

#endif