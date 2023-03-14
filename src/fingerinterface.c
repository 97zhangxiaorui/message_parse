#include "fingerinterface.h"
//#include "fingerprint.h"
#include "pcap/pcap.h"
#include "filter/mac.h"
#include "scanner/snmp.h"

int InitFinger(char *pFingerprintPath, char *pRulePath){
	//初始化字符处理规则
	if ( !pRulePath ){
		pRulePath = "../conf/rule/rule.conf";
	}
	InitFilterRule(pRulePath);

	//初始化xml指纹
	if ( !pFingerprintPath ){
		pFingerprintPath = "../conf/fingerprint";
	}
	
	InitFingerprint(pFingerprintPath);

	//初始化mac地址hash
	init_hash();

	//初始化yara库
	InitYara();
}

int GetFingerprintFromNetwork( FingerHandle *pFingerHandle, char* pDeviceNetworkName, char *pFingerFilePath)
{
	//init
	if ( NULL != pFingerHandle && NULL != pDeviceNetworkName && NULL != pFingerFilePath )
	{
		InitPcap(pDeviceNetworkName);
		InitFingerprint(pFingerFilePath);
		InitFingerCallback(pFingerHandle);
	}
}

int DetectIndustryProtocal(char *pIpAddress, char *pProtocolType, int nProtocolPort, char* pOutBuf, int nOutLen, char *pSnmpKey, char *pSnmpVersion)
{
	/*
	SnmpSanner(pIpAddress, nProtocolPort, pOutBuf, sizeof(pOutBuf) - 1, pSnmpKey, pSnmpVersion); 
	if ( strlen(pOutBuf) > 0 && NULL == strstr( pOutBuf, "error" ) )
	{
		return pOutBuf;
	}
	ModbusSanner(pIpAddress, nProtocolPort, pOutBuf, sizeof(pOutBuf) - 1);
	if ( strlen(pOutBuf) > 0 && NULL == strstr( pOutBuf, "error" ) )
	{
		return pOutBuf;
	}
	S7commSanner(pIpAddress, nProtocolPort, pOutBuf, sizeof(pOutBuf) - 1);
	{
		return pOutBuf;
	}*/
}
char* DetectSnmp(char *pIpAddress, char *pSnmpKey, char *pSnmpVersion, char* pOutBuf, int nOutLen)
{	
	if ( pIpAddress && pSnmpKey && pSnmpVersion ){
		SnmpSanner(pIpAddress, pSnmpKey, pSnmpVersion, pOutBuf, nOutLen);
	}

	return NULL;
}

//nProtocolType = 1-SNMP| 2-MODBUS|3-S7COMM
//nProtocolPort = 102/502
int GetFingerprintFromDetection(char *pIpAddress, int nProtocolType, int nProtocolPort, char *pSnmpKey, char *pSnmpVersion, char *pOutBuffer, int nBufferLen)
{
	char FingerBuf[2048] = { 0 };

	switch(nProtocolType)
	{
		case 1 :
			SnmpSanner(pIpAddress, nProtocolPort, FingerBuf, sizeof(FingerBuf) - 1, pSnmpKey, pSnmpVersion);
			break;
		case 2 :
			ModbusSanner(pIpAddress, nProtocolPort, FingerBuf, sizeof(FingerBuf) - 1);
			//printf("Data:%s\n", FingerBuf);
			break;
		case 3 :
			S7commSanner(pIpAddress, nProtocolPort, FingerBuf, sizeof(FingerBuf) - 1);
			//printf("Data:%s\n", FingerBuf);
			break;
		default:
			if ( 0 == nProtocolType )
			{
				DetectIndustryProtocal(pIpAddress, nProtocolType, nProtocolPort, FingerBuf, sizeof(FingerBuf) - 1, pSnmpKey, pSnmpVersion);
			}
			else
			{
				printf("Unknown detection protocal type , please check \n");
			}
		//return FingerBuf;
	}
	if ( strlen(FingerBuf) > 0 )
	{
		snprintf(pOutBuffer, nBufferLen - 1, "%s", FingerBuf);
		return pOutBuffer;
	}
	return NULL;
}

int ScanDeivceFingerprinter(char *pIp, char *pProtocol, int nPort, char *pSnmpKey, char *pSnmpVersion, char *pOutBuffer, int nBufferLen){
	if ( !pIp ){
		printf("Ip 地址为空，主动探测设备指纹失败\n");
		return -1;
	}

	if ( strcasestr(pProtocol, "snmp") ){
		char *pTmpSnmpKey = pSnmpKey, *pTmpSnmpVersion = pSnmpVersion;

		if ( !pSnmpKey ){
			pTmpSnmpKey = "public";
		}

		if ( !pSnmpVersion ){
			pTmpSnmpVersion = "2c";
		}
		DetectSnmp(pIp, pTmpSnmpKey, pTmpSnmpVersion, pOutBuffer, nBufferLen);
	}
	else{
		char TmpBuffer[124] = { 0 };

		snprintf(TmpBuffer, sizeof(TmpBuffer) - 1, "../third/atk/active_fingerprint %s %s %d", pProtocol, pIp, nPort);
		ExecShell(TmpBuffer, pOutBuffer, nBufferLen);
	}
	return 0;
}

int GetFingerprintFromPcapFile(char* pPcapFileName, FingerHandle *pFingerHandle)
{
	//InitFingerprint(pFingerConfigPath);
	//init_hash();
	//print_mac_hash();
	InitFinger(NULL, NULL);
	InitFingerCallback(pFingerHandle);
	InitReadPcapFile(pPcapFileName);
	//print_list();
}

char* GetFingerprintFromByts(unsigned char *pBytes, char *pOutFingerprint, int nOutFingerLen)
{
	if ( pBytes ){
		NetworkPacketHandle(pBytes, pOutFingerprint, nOutFingerLen);
	}
	return pOutFingerprint;
}
