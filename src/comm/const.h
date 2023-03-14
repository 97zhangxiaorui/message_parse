#ifndef __CONST_H_
#define __CONST_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define FINGERPRINT_LEN 64
#define STRING_IP_LEN 16
#define STRING_MAC_LEN 18
#define COMMON_LEN 64
#define MAC_LEN	6
#define NAME_LEN 128
#define DESCRIPTION_LEN 128
#define PRODUCTOR_LEN 256
#define PRODUCT_LEN 256
#define FILTER_CONTENT_LEN 256
#define COMMENT_LEN 16
#define COMMENTEX_LEN 128

#pragma pack(push)
#pragma pack(1)

typedef struct NetworkMessage
{
	unsigned int  SrcPort;
	unsigned int  DesPort;
	unsigned char DesMac[MAC_LEN];
	unsigned char SrcMac[MAC_LEN]; 
	unsigned int  SrcIp;
	unsigned int  DesIp;
	unsigned int  Ttl;
	unsigned int  ProtocalType;
	unsigned char *pBytes;  //packet data no etherip and mac head
	unsigned int  PacketLen; //uDP or TCP data len 
	unsigned int Vlan;
}NetworkMessageObj,*NetworkMessagePtr;

typedef struct Fingerprint
{
	unsigned char Name[NAME_LEN];
	unsigned char Description[DESCRIPTION_LEN];
	unsigned char Category[COMMENTEX_LEN];
	unsigned char Productor[NAME_LEN];
	unsigned char ProductName[NAME_LEN];
	unsigned char Role[COMMENT_LEN];
	unsigned char ProtocalType[COMMENTEX_LEN];
	unsigned char SoftwareVersion[NAME_LEN];
	unsigned char MessageTag[COMMENTEX_LEN*3];
	unsigned char Direction[MAC_LEN];

	unsigned int  MaxLen;
	unsigned char TransportProtocal;
	unsigned int  SrcPort;
	unsigned int  DesPort;
	unsigned char MoveOffset[COMMENTEX_LEN]; //support multiple offsets and separate them with "|"
	unsigned char Content[FILTER_CONTENT_LEN]; //support multiple contents and separate them with "|"
}FingerprintObj,*FingerprintPtr;

enum SystemType
{
	WINDOWS = 1,
	LINUX,
	IOS,
	ANDROID
};

//device type
enum DeviceType
{
	PC = 1,
	NOTEBOOK,
	MOBILE_DEVICE,
	PLC,
	RTU,
	CAMERA,
	SWITCH,
	SERVER,
	WIRELESS_ROUTER,
	AI_INTELLIGENCE,
	PRINTER,
	ESD,
	GATEWAY,
	NETWORK_SECURITY_DEVICE
};

typedef struct _DeviceSoft_{
	char Name[COMMON_LEN];
	int port;
	//DeviceSoft *pNext;
}DeviceSoft;

typedef struct _DeviceFingerprint_
{
	char Ip[STRING_IP_LEN];
	char Mac[STRING_MAC_LEN];
	char Vendor[COMMON_LEN*2];
	char Type[COMMON_LEN];
	char SystemType[STRING_IP_LEN];
	char SystemVersion[STRING_IP_LEN];
	char ProductName[COMMON_LEN];
	char Describe[DESCRIPTION_LEN];
	char HostName[COMMON_LEN];
}DeviceFingerprint;

#pragma pack(pop)

int StringDivide(char * pInputString, char *pSeperateFlag, char *OutSeperatedString[], int nSeperateNum);
unsigned int StringIp2Int(char *pStringIp);
char* IntIp2String( u_int32_t uIp, char *pStrIp, int nIpLen);
char *FormatMacByFlag(unsigned char *pMac, char *pFlag, char *pBuffer);
int GetLocalMacAndIp(char *pDeviceName, char *pOutMac, struct in_addr *pIpAddr);
char *GetLocalTime(char *pOutTime, int nOutLen);
char *Timestamp2String(time_t timestamp, char *pStringTime, int nStringLen);
time_t GetLocalTimestamp();
time_t StringTime2Timestamp(char *pStringTime);
int ExecShell(char *pShell, char *pResult, int nLen);
int RTrim(char *p);
int LTrim(char *p);
void Uppercase(char p[]);
char *ReplaceChar(char *p, char src, char des);
char *StrRpc(char *str, char *oldstr, char *newstr);
int Ipv4Get(char *str, int len, char *out_ip);
int RegularExpression(char *pSrcStr, char *pRule);

#endif