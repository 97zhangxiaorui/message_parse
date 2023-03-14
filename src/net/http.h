#ifndef __HTTP_H_
#define __HTTP_H_

#include "../comm/const.h"
/*
HTTP GET： Methond URI Version\r\n(0d0a)Host\r\nConnection\r\nAccept\r\nUserId\r\nAgent\r\nToken\r\nContentType\r\nReferer\r\nEncoding\r\nLanguage\r\nCookie
HTTP POST: Methond URI Version\r\n(0d0a)\r\nHost\r\nAcceipt\r\nLength\r\nContentType\r\n
*/

#define HTTP_CONTENT_MAX_LINES	16
#define HTTP_CONTENT_LINE_LONG  1450

#pragma pack(push)
#pragma pack(1)

typedef struct _HTTP_PROTOCAL{
	int  Type;
	char Methon[16];
	char Uri[64]; //标识字串
	char Version[16];
	char *Content[HTTP_CONTENT_LINE_LONG];
	int  ContentLines;
}HttpProtocal;

enum HttpType{
	REQUEST,
	RESONSE
};

#pragma pack(pop)

int ParseHttp(unsigned char *pBytes, FingerprintPtr pFinger, DeviceFingerprint *pDeviceFinger, int nPaylaodLen);

//HTTP 内容类型字段
/*
Host:
Connection:
Conten-Length:
timeout:
Origin:
User-Agent:
Content-Type:
Accept:
Referer:
Accept-Encoding:
Accept-Language:
Cookie:
Member:
Server:
*/

#endif