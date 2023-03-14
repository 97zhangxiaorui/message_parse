#include "mdns.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
*参数  ：pBytes(协议payload) - 应用层报文内容， pImap解析后的tls报文
*返回值：0 imap协议解析成功，-1 imap协议解析失败或者非tls协议
*/
int DecodeMdns(unsigned char *pBytes, MdnsHead *pMdns, int nPayloadLen)
{
	//int nType = 0;
	int nCopyLen = MDNS_CONTENT_MAX_LEN;

	if ( nPayloadLen < MDNS_CONTENT_MAX_LEN ){
		nCopyLen = nPayloadLen;
	}

	//MDNS响应报文标志 0x00 0x00 0x84 0x00(前两个字节为MDNS协议，后两个为响应报文<两个字段为Flag没有解析具体个字段>)，这种解析方式可能不包含所有的MDNS响应报文格式
	if ((pBytes[0] == 0x00) && (pBytes[1] == 0x00) && (pBytes[2] == 0x84) && (pBytes[3] == 0x00) ){

		pMdns->Type = 1;
		//printf("发现MDNS协议-1： %s \n", pBytes);

		memcpy(pMdns->pPayload, pBytes, nCopyLen);
	}

	//MDNS请求报文 0x00 0x00 0x00 0x00
	if ((pBytes[0] == 0x00) && (pBytes[1] == 0x00) && (pBytes[2] == 0x00) && (pBytes[3] == 0x00) ){
		pMdns->Type = 0;
		//printf("发现MDNS协议-0： %s \n", pBytes);
		memcpy(pMdns->pPayload, pBytes, nCopyLen);
	}

	return -1;
}

void InitMdnsStruct(MdnsHead *pMdns)
{
	if ( pMdns ){
		pMdns->Type = -1;
		memset(pMdns->pPayload, 0, MDNS_CONTENT_MAX_LEN);
	}
}

int ParseMdns(unsigned char *pBytes, FingerprintPtr pFinger, DeviceFingerprint *pDeviceFinger, int nPayloadLen)
{
	MdnsHead Mdns;
	InitMdnsStruct(&Mdns);
	if ( 0 == DecodeMdns(pBytes, &Mdns, nPayloadLen) ){
		if ( pFinger ){

			//比较内容
			if ( strstr(Mdns.pPayload, pFinger->Content) ){
				memcpy(pDeviceFinger->Describe, pFinger->Description, strlen(pFinger->Description));
			}
		}
		return 0;
	}

	return -1;
}