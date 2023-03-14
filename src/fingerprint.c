//#include "comm/const.h"
#include "fingerprint.h"
#include "config/file.h"
#include "xml2/xml2.h"
#include "net/etherip.h"
#include "net/tcp.h"
#include "net/udp.h"
#include "pcap/pcap.h"
#include "net/http.h"
#include "net/tls.h"
#include "net/imap.h"
#include "net/mdns.h"
#include "filter/mac.h"
#include "yara/layara.h"
#include "assets/assets.h"

#include <arpa/inet.h>

char SystemString[4][10] = { "OTHER", "WINDOWS", "LINUX", "IOS", "ANDROID" };
char DeviceTypeString[][24] = { "PC电脑", "笔记本电脑", "移动设备", "PLC设备", "RTU设备", "摄像头", "网络交换设备", "服务器", "无线路由设备", "AI智能", \
						"打印机", "ESD设备", "网关设备", "网络安全设备" };

char XmlPath[][COMMENTEX_LEN] = { "//Fingerprint/Name", 
								  "//Fingerprint/Description", 
								  "/Fingerprint/Category", 
								  "//Fingerprint/Productor", 
								  "//Fingerprint/ProductName",
								  "//Fingerprint/Role",
								  "//Fingerprint/MessageMaxLen",
								  "//Fingerprint/ProtocalType",
								  "//Fingerprint/Filter/TransportProtocal",
								  "//Fingerprint/Filter/SrcPort",
								  "//Fingerprint/Filter/DesPort",
								  "//Fingerprint/Filter/MoveOffset",
								  "//Fingerprint/Filter/Content",
								  "//Fingerprint/Direction",
								};

//define globle fingerprint pointer
ListPtr gpListPort = NULL;

//callback typdef
//typedef int (FingerHandle) (char *pFinger);

FingerHandle *gpFingerFuntion = NULL;

void InitFingerCallback( FingerHandle *pFingerHandle )
{
	gpFingerFuntion = pFingerHandle;
}

void InitNework(NetworkMessageObj *p){
	if (p){
		p->pBytes = NULL;
		p->DesPort = 0;
		p->DesPort = 0;
		p->SrcIp = 0;
		p->DesIp = 0;
		p->ProtocalType = 0;
		p->PacketLen = 0;
		p->Ttl = 0;
		memset(p->DesMac, 0, MAC_LEN);
		memset(p->SrcMac, 0, MAC_LEN);
	}
}

void NetworkPacketHandle( unsigned char *pBytes, char *pOutFingerprint, int nOutFingerLen)
{
	NetworkMessageObj Msg;
	DeviceFingerprint DevFinger;
	FingerprintPtr	  pFinger = NULL;
	int nPacketLength = 54;

	InitNework(&Msg);
	memset(&DevFinger, 0, sizeof(DeviceFingerprint));
	// memset(DevFinger.Ip, 0, sizeof(DevFinger.Ip));
	// memset(DevFinger.Mac, 0, sizeof(DevFinger.Mac));
	// memset(DevFinger.Vendor, 0, sizeof(DevFinger.Vendor));
	// memset(DevFinger.Type, 0, sizeof(DevFinger.Type));
	// memset(DevFinger.SystemType, 0, sizeof(DevFinger.SystemType));
	// memset(DevFinger.SystemVersion, 0, sizeof(DevFinger.SystemVersion));
	// memset(DevFinger.Describe, 0, sizeof(DevFinger.Describe));
	// memset(DevFinger.HostName, 0, sizeof(DevFinger.HostName));

	//仅处理TCP或者UDP协议
	if ( 0 == GetNetworkMessage(pBytes, &Msg) )
	{
		char Ip1[32] = { 0 }, Mac1[64] = { 0 };

		//printf("test: %02X %02X %02X \n", (unsigned char)Msg.pBytes[0],(unsigned char)Msg.pBytes[1],(unsigned char)Msg.pBytes[2]);
		IntIp2String((Msg.SrcIp),Ip1, sizeof(Ip1));
		FormatMacByFlag(Msg.SrcMac,"-",Mac1);

		memcpy( DevFinger.Ip, Ip1, strlen(Ip1) );
		memcpy( DevFinger.Mac, Mac1, strlen(Mac1) );

		/*处理UDP和TCP协议，其它协议暂不处理,*/
		pFinger = GetFingerByYara(Msg.pBytes, Msg.PacketLen);
		if ( !pFinger ){
			//如果通过yara没有命中指纹库，则尝试通过通过端口, 命中设备指纹库
			pFinger = GetFingerprintByPort(ntohs(Msg.SrcPort), ntohs(Msg.DesPort));
		}
		else{
			//判断指纹是针对源IP还是目的IP地址
			if ( strlen(pFinger->Direction) > 0 && strcasestr(pFinger->Direction, "dst"))
			{
				char Ip2[32] = { 0 }, Mac2[64] = { 0 };

				memset(DevFinger.Ip, 0, sizeof(DevFinger.Ip));
				memset(DevFinger.Mac, 0, sizeof(DevFinger.Mac));
				IntIp2String((Msg.DesIp),Ip2, sizeof(Ip2));
				FormatMacByFlag(Msg.DesMac,"-",Mac2);

				memcpy( DevFinger.Ip, Ip2, strlen(Ip2) );
				memcpy( DevFinger.Mac, Mac2, strlen(Mac2) );
			} 
		}

		if ( pFinger ){
			//开始封装指纹,命中指纹则更新输出指纹信息
			if ( strlen(pFinger->Productor) > 0 ){
				memcpy( DevFinger.Vendor, pFinger->Productor, strlen(pFinger->Productor) );
			}

			memcpy( DevFinger.Type, pFinger->Category, strlen(pFinger->Category) );
			memcpy( DevFinger.ProductName, pFinger->ProductName, strlen(pFinger->ProductName));
			memcpy( DevFinger.Describe, pFinger->Description, strlen(pFinger->Description));
		}

		//获取操作系统类型和生产厂商
		int nSystemType = 0;

		//如果指纹防线为dst那么用TTL获取系统类型，则为错误的。
		//if ( strlen(pFinger->Direction) <= 0 || strcasestr(pFinger->Direction, "src"))
		{
			nSystemType = GetSystemByTTL(Msg.Ttl);
			memcpy(DevFinger.SystemType, SystemString[nSystemType], strlen(SystemString[nSystemType]) );
		}

		//通过MAC地址获取厂商
		MacVendor *pMacVendor = NULL;
		char TmpMac[12] = { 0 };

		//snprintf(TmpMac, sizeof(TmpMac) - 1, "%02x-%02x-%02x", (unsigned char)Msg.SrcMac[0], \
			(unsigned char)Msg.SrcMac[1], (unsigned char)Msg.SrcMac[2]);
		snprintf(TmpMac, sizeof(TmpMac) - 1, "%02x-%02x-%02x", (unsigned char)DevFinger.Mac[0], \
			(unsigned char)DevFinger.Mac[1], (unsigned char)DevFinger.Mac[2]);
		Uppercase(TmpMac);
		TmpMac[strlen(TmpMac)] = '\0';
		pMacVendor = hash_search(TmpMac, '-');

		//通过获取的厂商，并且在指纹库中没有命中，或者命中的指纹没有厂商信息
		if ( pMacVendor && strlen(DevFinger.Vendor) <= 0 ){
			int nVendorLen = strlen(pMacVendor->vendor);

			if ( nVendorLen > COMMON_LEN ){
				nVendorLen = COMMON_LEN;
			}
			memcpy(DevFinger.Vendor, pMacVendor->vendor, nVendorLen);

			//通过TTL和厂商获取设备类型(粗略判断)或者通过厂商判断设备类型
			if ( strlen(DevFinger.Type) <= 0 && Msg.Ttl > 0 ){
				int nType = 0;

				nType = GetDeviceTypeByTtl(Msg.Ttl, pMacVendor->vendor);
				if ( 0 == nType ){
					nType = GetDeviceTypeByVendor(pMacVendor->vendor);
				}

				if ( nType > 0 ){
					nType = nType - 1;
				}

				//printf("SRC [%s] Dst [%s] [%s] Type [%d]\n", DevFinger.Type, DeviceTypeString[7], DeviceTypeString[nType], nType);
				memcpy( DevFinger.Type, DeviceTypeString[nType], strlen(DeviceTypeString[nType]) );
			}
		}

		//DeviceFingerprint TmpFinger;
		//memset(&TmpFinger, 0, sizeof(DeviceFingerprint));
		//ParseHttp(Msg.pBytes, &TmpFinger);

		//printf("===len: %d \n", ntohs(Msg.PacketLen));
		//非连接报文
		if ( ntohs(Msg.PacketLen) > nPacketLength ){
			if ( ParseHttp(Msg.pBytes, pFinger, &DevFinger, Msg.PacketLen) ){
				//printf("----------no http protocal---------\n");
				if ( ParseTls(Msg.pBytes, pFinger,&DevFinger, Msg.PacketLen) ){
					//printf("------------no tls protocal----------\n");
					if ( ParseImap(Msg.pBytes, pFinger, &DevFinger, Msg.PacketLen) ){
						//printf("------------no IMAP protocal----------\n");
						if ( ParseMdns(Msg.pBytes, pFinger, &DevFinger, Msg.PacketLen) ){
							//printf("------------no Ndns protocal----------\n");
						}
					}
				}
			}

			//指纹过滤
		}

		//printf("IP[%s] MAC[%s] Vendor[%s] Category[%s] System[%s] Describe[%s] \n", DevFinger.Ip, DevFinger.Mac, \
			DevFinger.Vendor, DevFinger.Type, DevFinger.SystemType, DevFinger.Describe);
		memset(pOutFingerprint, 0, nOutFingerLen);
		snprintf(pOutFingerprint, nOutFingerLen -1, "{\"ip\":\"%s\",\"mac\":\"%s\",\"vendor\":\"%s\",\"device_type\":\"%s\",\"system_type\":\"%s\","\
													 "\"version\":\"%s\",\"describe\":\"%s\",\"host_name\":\"%s\", \"device_name\":\"%s\"}", \
													 DevFinger.Ip, DevFinger.Mac, DevFinger.Vendor, DevFinger.Type, DevFinger.SystemType, \
													 DevFinger.SystemVersion, DevFinger.Describe, DevFinger.HostName, DevFinger.ProductName);
		//add hash
		AssetsFingerprint TmpAssets;
		TmpAssets.uIp = Msg.SrcIp;
		memcpy(&(TmpAssets.Fingerpint), &DevFinger, sizeof(DeviceFingerprint));
		UpdateAssets( &TmpAssets, pOutFingerprint, nOutFingerLen);

		//如果是连接报文，仅进行设备发现，以及操作系统预判
		/*
		if ( ( ntohs(Msg.PacketLen) - GetIpHeadSize() - GetTcpHeadSize() - 4) > 0 )
		{
			pFinger = FilterFingerprint(&Msg);
			if ( NULL != pFinger )
			{
				struct in_addr Addr;
				Addr.s_addr = Msg.SrcIp;

				char TmpBuf[128] = { 0 };
				CharMacToString(Msg.SrcMac, TmpBuf);
	
				if ( NULL != gpFingerFuntion )
				{
					char Finger[1024] = { 0 };

					snprintf(Finger, sizeof(Finger) - 1, "ProtocalType:%s\nIP:%s\nMAC:%s\nCategory:%s\nProductor:%s\nRole:%s\n",\
															pFinger->ProtocalType,
															inet_ntoa(Addr),
															TmpBuf,
															pFinger->Category,
															pFinger->Productor,
															pFinger->Role);
					gpFingerFuntion(Finger);
				}

				//IntIpToString(Msg.SrcIp, TmpBuf);
				
				printf("IP:%s\n", inet_ntoa(Addr));
				printf("MAC:%s\n", TmpBuf);
				printf("Category: %s\n", pFinger->Category);
				printf("Productor: %s\n", pFinger->Productor);
				printf("Role: %s\n", pFinger->Role);
				printf("ProtocalType: %s\n", pFinger->ProtocalType);
				
			}
		}
		*/
	}
	else
	{
		//非UDP、TCP报文
	}
}

FingerprintPtr GetFingerprintFromBytes(unsigned char *pBytes, NetworkMessagePtr pNetworkMsg)
{
	NetworkMessageObj Msg;
	FingerprintPtr	  pFinger = NULL;
	int nLvLen = 4;

	//check pNetworkMsg value is NULL, then get network message form btyes. 
	if ( NULL == pNetworkMsg )
	{
		if( 0 == GetNetworkMessage(pBytes, &Msg) )
		{
			pNetworkMsg = &Msg;
			struct in_addr Addr;
			Addr.s_addr = pNetworkMsg->SrcIp;

			char TmpBuf[128] = { 0 };
			CharMacToString(pNetworkMsg->SrcMac, TmpBuf);
			//printf("IP:%s\n", inet_ntoa(Addr));
			//printf("MAC:%s\n", TmpBuf);
		}
		else
		{
			return NULL;
		}
	}

	if ( ( ntohs(pNetworkMsg->PacketLen) - GetIpHeadSize() - GetTcpHeadSize() - nLvLen) > 0 )
	{
		pFinger = FilterFingerprint(&Msg);
		if ( NULL != pFinger )
		{
			struct in_addr Addr;
			Addr.s_addr = Msg.SrcIp;

			char TmpBuf[128] = { 0 };
			CharMacToString(Msg.SrcMac, TmpBuf);

			/*
			printf("IP:%s\n", inet_ntoa(Addr));
			printf("MAC:%s\n", TmpBuf);
			printf("Category: %s\n", pFinger->Category);
			printf("Productor: %s\n", pFinger->Productor);
			printf("Role: %s\n", pFinger->Role);
			printf("ProtocalType: %s\n", pFinger->ProtocalType);
			*/

			return pFinger;
		}
	}
	return NULL;
}

void GetPortFromBytes(unsigned char *pBytes, NetworkMessagePtr pMsg)
{
	short int nTcpType = 6;
	short int nUdpType = 17;

	pMsg->pBytes = NULL;
	if ( nTcpType == pMsg->ProtocalType )
	{
		GetPortFromTcpBytes((u_char *)pBytes, (u_int16_t *)&pMsg->DesPort, (u_int16_t *)&pMsg->SrcPort);
		pMsg->pBytes = pBytes + GetEtherHeadSize() + GetIpHeadSize() + GetTcpHeadSize();
	}
	else if ( nUdpType == pMsg->ProtocalType )
	{
		GetPortFromUdpBytes((u_char *)pBytes, (u_int16_t *)&pMsg->DesPort, (u_int16_t *)&pMsg->SrcPort);
		pMsg->pBytes = pBytes + GetEtherHeadSize() + GetIpHeadSize() + GetUdpHeadSize();
	}
	else
	{
		return;
	}
}

int GetNetworkMessage(unsigned char *pBytes, NetworkMessagePtr pMsg)
{
	short int nEtherType = 0;
	short int nIpType = 8;
	short int nVlan = 0;

	GetMacFromBytes(pBytes, pMsg->DesMac, pMsg->SrcMac, &nEtherType, &nVlan);
	pMsg->Vlan = nVlan;
	if ( nIpType == nEtherType )
	{
		short int nTransType = 0;
		short int nTcpType = 6;
		short int nUdpType = 17;
		unsigned char *pTmpBytes = NULL;

		if ( pMsg->Vlan > 0){
			pTmpBytes = pBytes + 4; //加上vlan头 长度
		} 
		else{
			pTmpBytes = pBytes;
		}

		//GetIpFromBytes(pBytes, (u_int32_t *)&pMsg->DesIp, (u_int32_t*)&pMsg->SrcIp, (u_int8_t*)&pMsg->ProtocalType, (u_int16_t *)&pMsg->PacketLen );
		GetIpFromBytes(pTmpBytes, pMsg);
		nTransType = pMsg->ProtocalType;
		// can use switch if support more protocal
		if ( nTcpType == nTransType || nUdpType == nTransType )
		{
			GetPortFromBytes(pTmpBytes, pMsg);
			return 0;
		}
	}

	return -1;
}

int Bytes2String(char *pBytes, int nBytesLen, unsigned char *pStringBuf)
{
	int i = 0;

	for (; i < nBytesLen; i++ )
	{
		sprintf(pStringBuf + i*2, "%02X", (unsigned char)pBytes[i]);
	}
}

int CompareContentByHex( NetworkMessagePtr pMsg, FingerprintPtr pFinger )
{
	char *pFlag = "|";
	int nOffset = atoi(pFinger->MoveOffset);
	int nContent = strlen(pFinger->Content);
	int nBytesLen =  ntohs(pMsg->PacketLen);

	int nBytesMinLen = GetIpHeadSize() + GetTcpHeadSize() + nOffset + nContent/2;

	if ( nBytesLen < nBytesMinLen )
	{
		printf("pbytes length is to short \n");
		return -1;
	}

	//check normal string -- no |
	if ( NULL == strstr( pFinger->MoveOffset, pFlag )  && NULL == strstr( pFinger->Content, pFlag ) )
	{
		unsigned char TmpBuf[1024] = { 0 };

		Bytes2String(pMsg->pBytes + nOffset, nContent/2, TmpBuf );

		if ( strlen(pFinger->Content) > 0 )
		{
			if ( 0 != strncmp( TmpBuf, pFinger->Content, strlen(pFinger->Content) ))
			{
				return -1;
			}
		}
		return 0;
	}

	// offset may be string 1|2|12
	char *pOffset[100];
	int nOffsetNum = 0;

	nOffsetNum = SplitStrByFlag(pFinger->MoveOffset, pFlag, pOffset, 100);

	//content may be string 0002|2A3D 
	char *pContent[100];
	int nContNum = 0;

	nContNum = SplitStrByFlag(pFinger->Content, pFlag, pContent, 100);

	if ( nContNum != nOffsetNum )
	{
		printf("The MoveOffset and Content do not match in the %s file\n", pFinger->Name);
	}

	//
	int nNum = nOffsetNum >= nContNum ? nContNum : nOffsetNum;
	int i = 0;
	for ( ; i < nNum; i++ )
	{
		unsigned char TmpBuf[1024] = { 0 };

		Bytes2String(pMsg->pBytes + atoi(pOffset[i]), strlen(pContent[i])/2, TmpBuf);
		if ( 0 != strncmp( TmpBuf, pContent[i], strlen(pContent[i]) ))
		{
			return -1;
		}
	}

	return 0;
}

FingerprintPtr FilterFingerprint(NetworkMessagePtr pMsg)
{
	ListPtr pTmpPortList = NULL;
	pTmpPortList = gpListPort;
	while(pTmpPortList)
	{
		PortPtr pTmpPort = (PortPtr) pTmpPortList->pContent;
		if ( pTmpPort->DesPort == ntohs(pMsg->DesPort) || pTmpPort->SrcPort == ntohs(pMsg->SrcPort) )
		{
			//finger list 
			ListPtr pTmpFingerList = pTmpPort->pList;
			while(pTmpFingerList)
			{
				// file data
				FingerprintPtr pFinger = NULL;
				pFinger = (FingerprintPtr) pTmpFingerList->pContent;
				if ( 0 == CompareContentByHex(pMsg, pFinger) )
				{
					return pFinger;
				}

				pTmpFingerList = pTmpFingerList->pNext;
			}
		}
		pTmpPortList = pTmpPortList->pNext; 
	}

}

int SplitStrByFlag(char* pInputString, char *pDivide, char* pSeperateStr[], int nSeperateNum)
{
	int i = 0;
	char * pTemp = NULL;
	char * pBreak = NULL;
	
	if((pInputString == NULL) || (pDivide == NULL) || (strlen(pInputString) == 0)
		|| (strlen(pDivide) == 0))
		return -1;
				
	pTemp = pInputString;	
	for(i = 0; i < nSeperateNum; i++)
	{
		pBreak = strstr(pTemp, pDivide);
		pSeperateStr[i] = pTemp;
		if(pBreak == NULL)
			break;
		*pBreak = '\0';
		pTemp = pBreak + strlen(pDivide);
	}
	if(i == nSeperateNum)
		pSeperateStr[i] = pTemp;
	return i + 1;
}

/*
*功能：获取指纹链中的指纹节点，通过源端口、目的端口
*/
FingerprintPtr GetFingerprintByPort(int nSrcPort, int nDstPort)
{
	if ( (nSrcPort <= 65535 && nSrcPort > 0) && ( (nDstPort <= 65535 && nDstPort > 0) ) ){
		ListPtr pTmpList = NULL;

		pTmpList = gpListPort;
		while( pTmpList ){

			FingerprintPtr pTmpFinger = NULL;

			if ( NULL != pTmpList->pContent ){
				pTmpFinger = (FingerprintPtr) pTmpList->pContent;
				//if ( pTmpFinger->SrcPort == nSrcPort || pTmpFinger->DesPort == nDstPort ){
				/*
				if ( pTmpFinger->SrcPort > 0 && pTmpFinger->DesPort > 0 ){
					if ( pTmpFinger->SrcPort == nSrcPort && pTmpFinger->DesPort == nDstPort ){
						return pTmpFinger;
					}
				}
				else if ( pTmpFinger->SrcPort > 0 && pTmpFinger->DesPort <= 0 ){
					if ( pTmpFinger->SrcPort == nSrcPort ){
						return pTmpFinger;
					}
				}
				else if ( pTmpFinger->SrcPort <= 0 && pTmpFinger->DesPort > 0 ){
					if ( pTmpFinger->DesPort == nDstPort ){
						return pTmpFinger;
					}
				}*/
				if ( pTmpFinger->SrcPort == nSrcPort && pTmpFinger->DesPort == nDstPort ){
					return pTmpFinger;
				}
				
				if ( pTmpFinger->SrcPort == nSrcPort || pTmpFinger->DesPort == nDstPort ){
					return pTmpFinger;
				}
			}
			else{
				break;
			}
			pTmpList = pTmpList->pNext;
		}

	}
	return NULL;
}

void InitFingerprint(char *pFingerprintPath)
{
	//check pFingerprintPath is path
	ListPtr pFileList = NULL;
	ListPtr pTmpFileList = NULL;
	ListPtr pTmpPortList = NULL, pPortList = NULL;
	//gpListPort = NULL;

	/*
	if ( NULL != gpListPort )
	{
		printf("Fingerprints have been read!!\n");
		return;
	}*/

	GetFileList(pFingerprintPath, &pFileList);
	
	pTmpFileList = pFileList;
	while(pTmpFileList)
	{
		//printf("FILE: %s\n", pTmpFileList->pContent);

		//Get fingerprint
		FingerprintPtr pTmpFinger = NULL;
		ListPtr pFingerList = NULL;

		pTmpFinger = (FingerprintPtr) Malloc(sizeof(FingerprintObj));
		memset(pTmpFinger, 0, sizeof(FingerprintObj));
		if ( -1 == GetFingerprint(pTmpFileList->pContent, pTmpFinger) )
		{
			Free(pTmpFinger);
			continue;
		}
	
		//free content memery
		Free(pTmpFileList->pContent);
		pTmpFileList = pTmpFileList->pNext;

		//find same port 
		/*
		pPortList = gpListPort;
		while ( pPortList )
		{
			PortPtr pPort = (PortPtr) pPortList->pContent;
			if ( pPort->DesPort == pTmpFinger->DesPort && pPort->SrcPort == pTmpFinger->SrcPort )
			{

				pFingerList = AddListNode(&(pPort->pList));
				break;
			}
			pPortList = pPortList->pNext;
		}

		//same port  list
		if ( NULL != pFingerList )
		{
			pFingerList->pContent = pTmpFinger;
			continue;
		}
		*/
		//new port list
		pTmpPortList = AddListNode(&gpListPort);

		/*
		//port node malloc
		PortPtr pTmpPortPtr = NULL;
		pTmpPortPtr = (PortObj*) Malloc(sizeof(PortObj));
		memset(pTmpPortPtr, 0, sizeof(PortObj));
		pTmpPortPtr->DesPort = pTmpFinger->DesPort;
		pTmpPortPtr->SrcPort = pTmpFinger->SrcPort;
		pTmpPortPtr->pList = NULL;
	
		//add fingerprint list to port
		pFingerList = AddListNode(&(pTmpPortPtr->pList));
		pFingerList->pContent = pTmpFinger;
		*/
		// add port to port list
		//pTmpPortList->pContent = pTmpPortPtr;
		pTmpPortList->pContent = pTmpFinger;

		pTmpPortList = NULL;
	}

	//free file list memery
	DestoryList(pFileList);
}

int GetFingerprint(char *pFileName, FingerprintPtr pFinger)
{
	xmlDocPtr pDoc = NULL;
	int nXmlPathNum = 0;

	pDoc = ReadXmlFile(pFileName);
	if ( NULL != pDoc )
	{
		nXmlPathNum = sizeof(XmlPath)/COMMENTEX_LEN;
		int i = 0, nTmpBufLen = 128;
		char TmpBuf[1024] = { 0 };

		for (; i < 14; i++)
		{
			memset(TmpBuf, 0, sizeof(TmpBuf));
			GetXmlNodeValueByXpath(pDoc, XmlPath[i], 1, TmpBuf);
			printf("tmpbuf: %s \n", TmpBuf);
			FillFingerprint(i, pFinger, TmpBuf);
		}

		xmlFreeDoc (pDoc);
        xmlCleanupParser ();
        xmlMemoryDump ();
	}
	else
	{
		printf("open xml file [%s] failed \n", pFileName);
		return -1;
	}
	return 0;
}

int FillFingerprint(int nIndex, FingerprintPtr pFinger, char *pSrc)
{
	switch(nIndex)
	{
		case 0:
			memcpy(pFinger->Name, pSrc, strlen(pSrc));
			break;
		case 1:
			memcpy(pFinger->Description, pSrc, strlen(pSrc));
			break;
		case 2:
			memset(pFinger->Category, 0, sizeof(pFinger->Category));
			memcpy(pFinger->Category, pSrc, strlen(pSrc));
			break;
		case 3:
			memcpy(pFinger->Productor, pSrc, strlen(pSrc));
			break;
		case 4:
			memcpy(pFinger->ProductName, pSrc, strlen(pSrc));
			break;
		case 5:
			memcpy(pFinger->Role, pSrc, strlen(pSrc));
			break;
		case 7:
			memcpy(pFinger->ProtocalType, pSrc, strlen(pSrc));
			break;
		case 6:
			pFinger->MaxLen = atoi(pSrc);
			break;
		case 8:
			pFinger->TransportProtocal = pSrc[0];
			break;
		case 9:
			pFinger->SrcPort = atoi(pSrc);
			break;
		case 10:
			pFinger->DesPort = atoi(pSrc);
			break;
		case 11:
			memcpy(pFinger->MoveOffset, pSrc, strlen(pSrc));
			break;
		case 12:
			memcpy(pFinger->Content, pSrc, strlen(pSrc));
			break;
		case 13:
		memcpy(pFinger->Direction, pSrc, strlen(pSrc));
			break;
		default:
			return -1;
	}
}

void print_list()
{
	ListPtr tmp = NULL;

	tmp = gpListPort;
	while(tmp)
	{	
		if ( tmp->pContent ){

			FingerprintPtr fig = (FingerprintPtr) (tmp->pContent);
			//printf("fingerprint:name[%s] ProtocalType[%s] Content[%s]\n", fig->Name, fig->ProtocalType, fig->Content);
			printf("fingerprint:name[%s] SourcePort[%d] DestPort[%d]\n", fig->Name, fig->SrcPort, fig->DesPort);
		}

		tmp = tmp->pNext;
	}
}
