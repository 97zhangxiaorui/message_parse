#ifndef __MDNS_H__
#define __MDNS_H__

#include "../comm/const.h"

#define MDNS_CONTENT_MAX_LEN 512

#pragma pack(push)
#pragma pack(1)
//简单处理mdns报文，仅处理mdns响应报文，通过字符串匹配获取信息，不进行内容的详细解析
/*
*判断MNDS协议的标志：前两个字节为：00 00； 第三和第四字节为：84 00（响应），00 00 (响应)
*/
typedef struct __MDNS_HEAD_{
	int Type; //0,请求；1，响应
	char pPayload[MDNS_CONTENT_MAX_LEN]; //大于512的把字符串截断
}MdnsHead;
#pragma pack(pop)

int ParseMdns(unsigned char *pBytes, FingerprintPtr pFinger, DeviceFingerprint *pDeviceFinger, int nPayloadLen);

#endif