#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "snmp.h"
#include "../comm/json.h"

#define RULE_NUMBER 1024
#define STRING_FLAGS 16

#define SWITCH_OID_NUM 1
#define SWITCH_OID_LEN 64
//华三获取交换机信息的OID
char SWITCH_OID[SWITCH_OID_NUM][SWITCH_OID_LEN] = {"iso.3.6.1.2.1.1.1.0"};

static StringFilterRule gRule[RULE_NUMBER];

char* RegexFingerString(char* pString)
{
	char *pRule = "xx*";
	int nRet = 0, nFlag = 0;
	regex_t Reg;
	char ErrorBuf[1024] = { 0 };

	nFlag = REG_EXTENDED | REG_ICASE | REG_NOSUB;

	nRet = regcomp(&Reg, pRule, nFlag);
	if ( nRet )
	{
		regerror(nRet, &Reg, ErrorBuf, sizeof(ErrorBuf) - 1);
		printf("regex error: %s \n", ErrorBuf);
		regfree(&Reg);
		return NULL;
	}

	nRet = regexec(&Reg, pString, 0, NULL, 0);
	if ( nRet )
	{
		regerror(nRet, &Reg, ErrorBuf, sizeof(ErrorBuf) - 1);
		printf("regex error: %s \n", ErrorBuf);
		regfree(&Reg);
		return NULL;
	}

	regerror(nRet, &Reg, ErrorBuf, sizeof(ErrorBuf) - 1);
	regfree(&Reg);
	if ( NULL != strstr(ErrorBuf, "Success") )
	{
		return pString;
	}
	else
	{
		return NULL;
	}
}

char *ParseString2Finger(char *pFingerString)
{
	//find real finger
	printf("Device Finger: %s \n", pFingerString); 
}


int ReadConfFile(const char *pFilePath)
{
	int nNum = 0;

	FILE *pFile = fopen(pFilePath, "r+");
	if (NULL == pFile)
	{
		printf("配置文件[%s]打开失败 \n", pFilePath);
		return;
	}

	char buf[1024] = { 0 };
	int l = 0;
	while (fgets(buf, 1024, pFile) != NULL)
	{
		//去除行两边的空格
		RTrim(buf);
		LTrim(buf);

		if ( l >= RULE_NUMBER ){
			printf("规则条数太多，系统仅支持规则%d数 \n", RULE_NUMBER);
			return -1;
		}

		//每行的第一个字符必须是'{'否则跳过
		if ( buf[0] == '{' ){
			int nSize = 128, nLineOk = 0;
			char TmpBuf[128] = { 0 };

			if ( buf[strlen(buf) - 1] != '}' ){
				buf[strlen(buf) - 1] = '\0';
			}

			if ( !get_value_by_key(buf, "Name", TmpBuf, nSize) ){
				continue;
			}
			snprintf(gRule[l].Name, sizeof(gRule[l].Name) - 1, "%s", TmpBuf);
			memset(&TmpBuf, 0, nSize);

			if ( !get_value_by_key(buf, "StringFlag", TmpBuf, nSize) ){
				memset(&gRule[l], 0, sizeof(StringFilterRule));
				continue;
			}

			snprintf(gRule[l].StringFlag, sizeof(gRule[l].StringFlag) - 1, "%s", TmpBuf);
			memset(TmpBuf, 0, nSize);

			if ( !get_value_by_key(buf, "Item", TmpBuf, nSize) ){;
				memset(&gRule[l], 0, sizeof(StringFilterRule));
				continue;
			}
			snprintf(gRule[l].Item, sizeof(gRule[l].Item) - 1, "%s", TmpBuf);
			memset(TmpBuf, 0, nSize);

			if ( !get_value_by_key(buf, "DivisionMark", TmpBuf, nSize) ){
				memset(&gRule[l], 0, sizeof(StringFilterRule));
				continue;
			}
			snprintf(gRule[l].DivisionMark, sizeof(gRule[l].DivisionMark) - 1, "%s", TmpBuf);
			memset(TmpBuf, 0, nSize);

			if ( !get_value_by_key(buf, "ItemNumber", TmpBuf, nSize) ){
				memset(&gRule[l], 0, sizeof(StringFilterRule));
				continue;
			}
			snprintf(gRule[l].ItemNumber, sizeof(gRule[l].ItemNumber) - 1, "%s", TmpBuf);

			l++;
		}
	}

	//关闭文件
	fclose(pFile);
}

//
void PrintRule(){
	int i = 0;

	for ( ; i < RULE_NUMBER; i++ ){
		if ( strlen(gRule[i].Name) <= 0 ){
			return -1;;
		}

		printf("[%s-%s-%s-%s-%s] \n", gRule[i].Name, gRule[i].StringFlag, gRule[i].ItemNumber, gRule[i].Item, gRule[i].DivisionMark);
	}
}

void InitFilterRule(char *pRulePath)
{
	//
	ReadConfFile(pRulePath);

	//print rule
	//PrintRule();
}

int StringFilterByRule(char *pSrcStr, char*pItem, int nItemLen)
{
	//
	if ( pSrcStr && pItem ){
		int i = 0;
		for ( ; i < RULE_NUMBER; i++ ){
			if ( strlen(gRule[i].Name) <= 0 ){
				return -1;;
			}

			//获取字符串标志
			if ( strlen(gRule[i].StringFlag) > 0 ){
				char *pFlags[STRING_FLAGS] = { NULL };
				int nFlagNumber = 0, j = 0;
				char TmpBuf[128] = { 0 };

				snprintf(TmpBuf, sizeof(TmpBuf) - 1, "%s", gRule[i].StringFlag);
				//分割字符串
				nFlagNumber = StringDivide(TmpBuf, "|", pFlags, STRING_FLAGS);
				for (; j < nFlagNumber; j++ ){
					if ( strstr(pSrcStr, pFlags[j]) ){
					//if ( strstr(pSrcStr, TmpBuf) ){
						//获取字符串
						char *pLineItem[128] = { NULL }, *pRuleItemNumber[64] = { NULL }, *pRuleItem[64] = { NULL };
						int nLineItemNumber = 128, nRuleItemNumber = 64, nRuleItem = 64;
						char RuleItem[128] = { NULL }, RuleItemNumber[64] = { NULL };

						snprintf(RuleItem, sizeof(RuleItem) -1, "%s", gRule[i].Item);
						snprintf(RuleItemNumber, sizeof(RuleItemNumber) -1, "%s", gRule[i].ItemNumber);
						//
						char t1[1024] = { 0 };
						snprintf(t1, sizeof(t1) - 1, "%s", pSrcStr);
						nLineItemNumber = StringDivide(t1, gRule[i].DivisionMark, pLineItem, 128);
						nRuleItem = StringDivide(RuleItem, "|", pRuleItem, 64);
						nRuleItemNumber = StringDivide(RuleItemNumber, "|", pRuleItemNumber, 64);
						//规则字段值与规则字段名数量一致，Rule初始化时已经进行过校验，不符个规范的会剔除
						{
							int t = 0;
							for ( ; t < nRuleItem; t++ ){
								int ItemNum = 0, Len = 0;
								char tmp[512] = { 0 };

								ItemNum = atoi(pRuleItemNumber[t]);
								Len = strlen(pItem);

								if (pLineItem[ItemNum]){
									memcpy(tmp, pLineItem[ItemNum], strlen(pLineItem[ItemNum]));
									CleanQuotes(tmp);
									if ( ItemNum < nLineItemNumber ){
										snprintf(pItem + Len, nItemLen - Len, "\"%s\":\"%s\",",pRuleItem[t], tmp);
									}
								}
							}
						}
					}
					//
					if ( strlen(pItem) > 0 ){
						pItem[strlen(pItem) - 1] = '\0';
					}
				}
			}
		}
	}
	return -1;
}

/*pVersion = 1|2c|3;default 2c*/
char* GetFingerFromSnmp(char *pIpAddr, char *pVersion, char  *pKey, int nSnmpType, char *pOid, char* pOutBuffer, int nOutBufferLen)
{
	char TmpBuf[1024] = { 0 };
	FILE *pFile = NULL;
	char RecvBuf[1024] = { 0 };

	if ( NULL == pKey)																																							
	{
		pKey = "public";
	}

	if ( NULL == pVersion )
	{
		pVersion = "2c";
	}

	if ( nSnmpType == SNMP_WALK ){
		snprintf(TmpBuf, sizeof(TmpBuf) - 1, "snmpwalk %s -c %s -v %s system", pIpAddr, pKey, pVersion);
	}

	if ( nSnmpType == SNMP_GET ){
		if ( !pOid ){
			printf("SNMP 采用Get方式，但是该Oid为空,程序返回，请检查参数 \n");
			return NULL;
		}
		snprintf(TmpBuf, sizeof(TmpBuf) - 1, "snmpget -c %s -v %s %s %s", pKey, pVersion, pIpAddr, pOid);
	}

	//open pipe
	pFile = popen(TmpBuf, "r");
	if ( !pFile ){
		printf("----------popen 打开失败\n");
		return NULL;
	}

	while( fread(RecvBuf, 1, sizeof(RecvBuf) -1, pFile ) > 0 )
	{
		//char *p = "SNMPv2-MIB::sysDescr.0 = STRING: Siemens, SIMATIC NET, SCALANCE XM408-8C, 6GK5 408-8GS00-2AM2, HW: Version 2, FW: Version V06.01.00, SVPK2138923";
		char buf[1024] = { 0 };
		StringFilterByRule(RecvBuf, buf, sizeof(buf) - 1);
		if ( strlen(buf) > 6 ){
			snprintf(pOutBuffer + strlen(pOutBuffer), nOutBufferLen - strlen(pOutBuffer), "%s,",buf);
		}
	}
	pclose(pFile);

	int nLen = strlen(pOutBuffer);
	if ( nLen > 1 ){
		char *pTmp = (char*)malloc(nLen - 1);
		memset(pTmp, 0, nLen -1);
		memcpy(pTmp, pOutBuffer, nLen - 1);
		snprintf(pOutBuffer, nOutBufferLen - 1, "{%s}", pTmp);
		free(pTmp);
		//pOutBuffer[ nLen - 1 ] = '\0';

	}


	return pOutBuffer;
}

int SnmpSanner(char *pIpAddress, char *pSnmpKey, char *pSnmpVersion, char *pOutBuf, int nOutBufLen)
{
	//SNMP 先进行GET，如果没有获取到数据则进行walk
	int i = 0, nDataLen = 6;
	for ( ; i < SWITCH_OID_NUM; i++){
		if ( !GetFingerFromSnmp(pIpAddress, pSnmpVersion, pSnmpKey, SNMP_GET, SWITCH_OID[i], pOutBuf, nOutBufLen) || strlen(pOutBuf) <= nDataLen ){
			continue;
		}
	}

	if ( strlen(pOutBuf) <= nDataLen ){
		GetFingerFromSnmp(pIpAddress, pSnmpVersion, pSnmpKey, SNMP_WALK, NULL, pOutBuf, nOutBufLen);
	}
	return 0;
}