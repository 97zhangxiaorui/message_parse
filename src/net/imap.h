#ifndef __IMAP_H__
#define __IMAP_H__

#include "../comm/const.h"

#define IMAP_CONTENT_MAX_LINES	16
#define IMAP_CONTENT_LINE_LONG  1450

#pragma pack(push)
#pragma pack(1)
/*
*IMAP协议： 同行字段之间以空格分开，命令行之间通过\r\n风格，以\r\n为结尾
*协议标志 : 非命令行，第一个字符为*(如：* CAPABILITY IMAP4rev1 XLIST RM_MAIL_TYPE UIDPLUS\r\n)； 命令行，字符+数字+空格(如：C246559 OK CAPABILITY completed\r\n) 
*说明    ： IMAP协议内容可能是命令行，也可能非命令行
**/
typedef struct __IMAP_PROTOCAL_{
	int Type;  //0,命令行尾报文头; 1,非命令行为报文头
	char *pContent[IMAP_CONTENT_LINE_LONG];
	int ContentLines;
}ImapHead;
#pragma pack(pop)

int ParseImap(unsigned char *pBytes, FingerprintPtr pFinger, DeviceFingerprint *pDeviceFinger, int nPaylaodLen);

#endif