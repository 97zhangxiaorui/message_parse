#include "const.h"
#include "list.h"

#include <net/if.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <regex.h>


/**
 * cutil_string_divide
 * @input_string:	待分割的字符串
 * @seperate_flag:	字符的分割标志
 * @out_seperated_string:	保存分割好的字符串
 * @seperate_num: 	指针数组的最大大小
 *
 *  将字符串分割为几个小的字符串(会修改源串)
 *
 *  Returns: 成功分割好的个数失败-1
 */
int StringDivide(char * pInputString, char *pSeperateFlag, char *OutSeperatedString[], int nSeperateNum)
{
	int i = 0;
	char * pszTemp = NULL;
	char * pszBreak = NULL;
	
	if((pInputString == NULL) || (pSeperateFlag == NULL) || (strlen(pInputString) == 0)
		|| (strlen(pSeperateFlag) == 0))
		return -1;
	pszTemp = pInputString;	
	for(i = 0; i < nSeperateNum; i ++)
	{
		pszBreak = strstr(pszTemp, pSeperateFlag);
		OutSeperatedString[i] = pszTemp;
		if(pszBreak == NULL)
			break;
		*pszBreak = '\0';
		pszTemp = pszBreak + strlen(pSeperateFlag);
	}
	if(i == nSeperateNum)
		OutSeperatedString[i] = pszTemp;
	
	return i + 1;
}

unsigned int StringIp2Int(char *pStringIp)
{
	unsigned int ip = 0;
	struct in_addr addr;
	if ( NULL != pStringIp )
	{
		//ip = inet_addr(string_ip);
		ip = inet_aton(pStringIp, &addr);
		return htonl(addr.s_addr);
		//return htonl(ip);
	}
}

char* IntIp2String( u_int32_t uIp, char *pStrIp, int nIpLen)
{
	memset(pStrIp, 0, nIpLen);
	if ( uIp > 0 )
	{
		struct in_addr Addr;
		memset(&Addr, 0, sizeof(struct in_addr));
		Addr.s_addr = uIp;//ntohl(uIp);
		sprintf(pStrIp, "%s", inet_ntoa(Addr));
	}
	return pStrIp;
}

char *FormatMacByFlag(unsigned char *pMac, char *pFlag, char *pBuffer)
{
	int len = 64;
	snprintf(pBuffer, len - 1, "%02x%s%02x%s%02x%s%02x%s%02x%s%02x", (unsigned char)pMac[0], pFlag, (unsigned char)pMac[1], pFlag, (unsigned char)pMac[2], pFlag, (unsigned char)pMac[3], pFlag, (unsigned char)pMac[4], pFlag, (unsigned char)pMac[5]);
	return pBuffer;
}

int GetLocalMacAndIp(char *pDeviceName, char *pOutMac, struct in_addr *pIpAddr)
{
	struct ifreq Ifreq;
    int   nSock = 0 , i = 0, mac_len = 6, ip_len = 4;

	if((nSock = socket(AF_INET,SOCK_STREAM,0)) < 0 )
	{
		printf( "Create socket failed , please check\n");
		return   -1;
	}

	strcpy(Ifreq.ifr_name, pDeviceName);
	if( (i = ioctl(nSock,SIOCGIFHWADDR,&Ifreq) ) < 0)
	{
		printf("I=%d\n", i);
		printf( "Ioctl failed, please check \n");
		printf("Get local mac address failed \n");
		return -1;
	}
	memcpy(pOutMac, Ifreq.ifr_hwaddr.sa_data, mac_len);

	if ( ioctl(nSock, SIOCGIFADDR, &Ifreq) < 0)
	{
		printf( "Create socket failed , please check\n");
		printf("Get local ip address failed \n");
		return   -1;
	}

	memcpy(pIpAddr, &((struct sockaddr_in *)&Ifreq.ifr_addr)->sin_addr, ip_len);

	close(nSock);
	return 0;
}

char *GetLocalTime(char *pOutTime, int nOutLen)
{
	time_t local_time = 0;
	struct tm *tm_time = NULL;

	time(&local_time);
	tm_time = localtime(&local_time);
	snprintf(pOutTime, nOutLen - 1, "%04d%02d%02d%02d%02d%02d", tm_time->tm_year+1900, tm_time->tm_mon + 1, tm_time->tm_mday, tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);
	return pOutTime;
}


char *Timestamp2String(time_t timestamp, char *pStringTime, int nStringLen)
{	
	struct tm *tm_time = NULL;
	tm_time = localtime(&timestamp);
	snprintf(pStringTime, nStringLen - 1, "%04d%02d%02d%02d%02d%02d", tm_time->tm_year+1900, tm_time->tm_mon + 1, tm_time->tm_mday, tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);
	return pStringTime;
}

time_t GetLocalTimestamp()
{
	time_t timestamp = 0;
	return time(&timestamp);
}

time_t StringTime2Timestamp(char *pStringTime)
{
	struct tm tmp_time;  
    strptime(pStringTime,"%Y-%m-%d %H:%M:%S", &tmp_time); //时间24时制
    return mktime(&tmp_time);  
}

int ExecShell(char *pShell, char *pResult, int nLen)
{
	//MANAGE_LOG_INFO("SHELL:%s \n", shell);
	FILE *fp = popen(pShell, "r");
	if (!fp)
	{
		printf("open [%s],failed ,please check \n", pShell);
		return -1;
	}


	char buffer[1024] = { 0 };

	while (fgets(buffer, sizeof(buffer) - 1, fp))
	{
		int out_len = strlen(pResult);
		int buffer_len = 0;

		LTrim(buffer);
		RTrim(buffer);

		buffer_len = strlen(buffer);
		//MANAGE_LOG_INFO("NAME:%s---\n", buffer);
		snprintf(pResult + out_len, nLen - out_len - 1, "%s", buffer);
		memset(buffer, 0, sizeof(buffer));

	}
	pResult[strlen(pResult) - 1] = '\0';
	pclose(fp);
	return 0;
}

int RTrim(char *p)
{
	char tmp[2048] = { 0 };
	int i = strlen(p);

	memcpy(tmp, p, strlen(p));
	while (tmp[i] == ' ')
		i--;

	memset(p, 0, strlen(p));
	memcpy(p, tmp, i);
	return 0;
}

int LTrim(char *p)
{
	char tmp[1024] = { 0 };
	int i = 0;

	memcpy(tmp, p, strlen(p));
	while (tmp[i] == ' ')
		i++;

	memset(p, 0, strlen(p));
	memcpy(p, tmp + i, strlen(tmp+i));
	return 0;
}

//对大于1024的字串会越界
int CleanQuotes(char *p)
{
	char tmp[1024] = { 0 };
	int i = 0, len = 0, j = 0;
	if (!p){
		return -1;
	}

	len = strlen(p);
	for ( ; i < len; i++){
		if ( p[i] == '\"' || p[i] == ',' )
		{
			continue;
		}

		tmp[j] = p[i];
		j++;
	}

	memset(p, 0, len);
	memcpy(p, tmp, j);
}

//去除左边的制表符
int CleanHT(char *p)
{
	char tmp[1024] = { 0 };
	int i = 0;

	memcpy(tmp, p, strlen(p) - 1); //去掉回车换行
	while (tmp[i] == '\t')
		i++;

	memset(p, 0, strlen(p));
	memcpy(p, tmp + i, strlen(tmp) - i);
}

void Uppercase(char p[])
{
        char *tmp = p;
        for ( ; *tmp; tmp++ ){
                if ( 'a' <= *tmp && 'z' >= *tmp ){
                        *tmp = *tmp - 32;
                }
        }
}

/*
**change paramp
*param: p is not const char *
*/
char *ReplaceChar(char *p, char src, char des)
{
	char *tmp = p;
	while (*tmp)
	{
		if (*tmp == src)
			*tmp = des;
		tmp++;
	}
	return p;
}

char *StrRpc(char *str, char *oldstr, char *newstr) 
{
	char bstr[1000];//转换缓冲区
	memset(bstr, 0, sizeof(bstr));

	for (int i = 0; i < strlen(str); i++) {
		if (!strncmp(str + i, oldstr, strlen(oldstr))) {//查找目标字符串
			strcat(bstr, newstr);
			i += strlen(oldstr) - 1;
		}
		else {
			strncat(bstr, str + i, 1);//保存一字节进缓冲区
		}
	}

	strcpy(str, bstr);
	return str;
}

int Ipv4Get(char *str, int len, char *out_ip) {
	int a, b, c, d;
	char *p = str;

	for (int i = 0; i < len; i++) {
		if ((*p >= '0') && (*p <= '9')) {
			if (4 == sscanf(p, "%d.%d.%d.%d", &a, &b, &c, &d)) {
				if (0 <= a && a <= 255
					&& 0 <= b && b <= 255
					&& 0 <= c && c <= 255
					&& 0 <= d && d <= 255) {
					sprintf(out_ip, "%d.%d.%d.%d", a, b, c, d);
					// MANAGE_LOG_INFO("IPv4: %d.%d.%d.%d\n",a,b,c,d);	
					return 1;
				}
			}
		}
		p++;
	}
	return 0;
}

/*
*功能	: 字符串，正则表达式校验
*参数	: pSrcStr 
*返回指	: 0，匹配；-1 不匹配
*/
int RegularExpression(char *pSrcStr, char *pRule)
{
	if ( pSrcStr && pRule ){
		regex_t Rgx;

		if ( 0 == regcomp(&Rgx, pRule, REG_EXTENDED) ){
			if( 0 == regexec( &Rgx, pSrcStr, 0, NULL, 0 ) ){
				return 0;
			}
		}
	}
	return -1;
}

extern ListPtr gpListPort;

/*
*功能：获取指纹链中的指纹节点，通过源端口、目的端口
*/
FingerprintPtr GetFingerprintByName(char *pName)
{
	if ( pName ){
		ListPtr pTmpList = NULL;

		pTmpList = gpListPort;
		while( pTmpList ){

			FingerprintPtr pTmpFinger = NULL;

			if ( NULL != pTmpList->pContent ){
				pTmpFinger = (FingerprintPtr) pTmpList->pContent;
				if ( strcasestr(pTmpFinger->Name, pName) ){
					//printf("finger name: %s \n", pTmpFinger->Name);
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