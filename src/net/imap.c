#include "imap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
*参数  ：pBytes - 应用层报文内容， pImap解析后的tls报文
*返回值：0 imap协议解析成功，-1 imap协议解析失败或者非tls协议
*/
int DecodeImap(unsigned char *pBytes, ImapHead *pImap)
{
	//int nType = 0;

	//0x2a = '*', 0x20 = ' ', 判断开始为非命令行
	if ((pBytes[0] == 0x2a) && pBytes[1] == 0x20 ){
		pImap->Type = 1;
		//printf("发现IMAP协议-1： %s \n", pBytes);
		pImap->ContentLines = StringDivide(pBytes, "\r\n", pImap->pContent, IMAP_CONTENT_MAX_LINES);
		if ( pImap->ContentLines > 0 ){
			return 0;
		}
	}

	//判断为命令行
	char *pBuffer[16] = { NULL };
	if ( StringDivide(pBytes, " ", pBuffer, 2) ){
		if ( 0 == RegularExpression(pBuffer[0], "^[a-zA-Z]{1}[0-9]{3}") ){
			//printf("发现IMAP协议-0： %s \n", pBytes);
			pImap->Type = 0;

			pImap->ContentLines = StringDivide(pBytes, "\r\n", pImap->pContent, IMAP_CONTENT_MAX_LINES);
			if ( pImap->ContentLines > 0 ){
				return 0;
			}
		}
		//printf("非IMAP协议 \n");
	}

	return -1;
}

void InitImapStruct(ImapHead *pImap)
{
	if ( pImap ){
		pImap->Type = 0;
		pImap->pContent[IMAP_CONTENT_LINE_LONG] = NULL;
		pImap->ContentLines = 0;
	}
}

int ParseImap(unsigned char *pBytes, FingerprintPtr pFinger, DeviceFingerprint *pDeviceFinger, int nPaylaodLen)
{
	ImapHead Imap;

	//InitImapStruct(&Imap);
	memset(&Imap, 0, sizeof(ImapHead));
	if ( 0 == DecodeImap(pBytes, &Imap) ){
		if ( pFinger ){
			int i = 0;

			//比较内容
			for ( ; i < Imap.ContentLines; i++ ){
				if ( strstr(Imap.pContent[i], pFinger->Content) ){
					memcpy(pDeviceFinger->Describe, pFinger->Description, strlen(pFinger->Description));
					break;
				}
			}
		}
		return 0;
	}
	return -1;
}