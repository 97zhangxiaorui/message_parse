#include "http.h"

/*
*参数  ：pBytes - 应用层报文内容， pHttp解析后的http报文
*返回值：0 http协议解析成功，-1 http协议解析失败或者不是http协议
*/
int DecodeHttp(unsigned char *pBytes, HttpProtocal *pHttp)
{
	if ( pBytes ){
		char flag[5] = { 0 };

		memcpy(flag, pBytes, 4);
		if (strcasestr(flag, "HTTP", strlen("HTTP")) ){
			//HTTP响应
			pHttp->Type = RESONSE;
			pHttp->ContentLines = StringDivide(pBytes, "\r\n", pHttp->Content, HTTP_CONTENT_MAX_LINES);
			return 0;
		}

		if ( strcasestr(flag, "POST", strlen("POST")) || strcasestr(flag, "GET", strlen("GET")) ){
			//HTTP请求
			pHttp->Type = REQUEST;
			pHttp->ContentLines = StringDivide(pBytes, "\r\n", pHttp->Content, HTTP_CONTENT_MAX_LINES);
			if (pHttp->ContentLines > 0){
				char *pTmp[32] = { NULL };
				int nNum = 0;

				//HTTP头部行，获取HTTP方法，版本等信息
				nNum = StringDivide(pHttp->Content[0], " ", pTmp, 3);
				if ( nNum >= 3 ){
					memcpy(pHttp->Methon, pTmp[0], strlen(pTmp[0]));
					memcpy(pHttp->Uri, pTmp[1], strlen(pTmp[1]));
					memcpy(pHttp->Version, pTmp[2], strlen(pTmp[2]));
				}
			}
			return 0;
		}
	}
	return -1;
}

void InitHttpStruct(HttpProtocal *pHttp)
{
	if ( pHttp ){
		pHttp->Type = -1;
		memset(pHttp->Methon, 0, sizeof(pHttp->Methon));
		memset(pHttp->Uri, 0, sizeof(pHttp->Uri));
		memset(pHttp->Version, 0, sizeof(pHttp->Version));
		pHttp->Content[HTTP_CONTENT_LINE_LONG] = NULL;
		pHttp->ContentLines = 0;
	}
}

int ParseHttp(unsigned char *pBytes, FingerprintPtr pFinger, DeviceFingerprint *pDeviceFinger, int nPaylaodLen){
	int nResult = -1;

	HttpProtocal Http;
	//InitHttpStruct(&Http);
	memset(&Http, 0, sizeof(HttpProtocal));

	//return DecodeHttp(pBytes, &Http);
	if ( 0 == DecodeHttp(pBytes, &Http) ){
		int i = 0;

		//命中指纹库
		if ( pFinger )
		{
			for ( ; i < Http.ContentLines; i++ ){
				if ( strstr(Http.Content[i], pFinger->Content) ){
					memcpy(pDeviceFinger->Describe, pFinger->Description, strlen(pFinger->Description));
					break;
				}
			}
			return 0;
		}

		//没有命中，或者命中指纹但是没有命中Content，则根据http报文的响应或者请求，判断设备描述信息
		if ( RESONSE == Http.Type ){
			memcpy(pDeviceFinger->Describe, "WEB 服务器", strlen("WEB 服务器"));
		}
		return 0;
	}

	return -1;
	//填充pFinger
}

