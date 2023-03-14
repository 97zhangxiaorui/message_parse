#include "tls.h"

/*
*参数  ：pBytes - 应用层报文内容， pTls解析后的tls报文
*返回值：0 tls协议解析成功，-1 tls协议解析失败或者非tls协议
*/
int DecodeTls(unsigned char *pBytes, TlsHead *pTls)
{
	int nType = 0;

	nType = pBytes[0];
	//printf("--flag--%d :%d-%d\n", nType, ChangeCipherSpec, Alert);
	switch(nType){
		case ChangeCipherSpec:
		case Alert:
		case Handshake:
		case Application:
			//数据
			pTls = (TlsHead*)pBytes;
			break;
		default:
			return -1;
	}

	return 0;
}

void InitTlsStruct(TlsHead *pTls)
{
	if ( pTls ){
		pTls->Type = 0;
		pTls->Version = 0;
		pTls->Length = 0;
		pTls->pContent = NULL;
	}
}

int ParseTls(unsigned char *pBytes, FingerprintPtr pFinger, DeviceFingerprint *pDeviceFinger, int nPaylaodLen)
{
	TlsHead Tls;

	//InitTlsStruct(&Tls);
	memset(&Tls, 0, sizeof(TlsHead));
	if (0 == DecodeTls(pBytes, &Tls) ){
		if ( pFinger ){

			//比较内容
			if ( strstr(Tls.pContent, pFinger->Content) ){
				memcpy(pDeviceFinger->Describe, pFinger->Description, strlen(pFinger->Description));
			}
		}
		return 0;
	}

	return -1;
}