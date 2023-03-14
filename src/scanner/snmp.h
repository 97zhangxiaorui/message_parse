#ifndef __SNMP_H_
#define __SNMP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push)
#pragma pack(1)
typedef struct _STTRING_FILTER_RULE_{
	char Name[32];
	char StringFlag[64];
	char Item[128];
	char ItemNumber[32];
	char DivisionMark[12];
}StringFilterRule;
#pragma pack(pop)

enum SnmpType{
	SNMP_GET,
	SNMP_WALK
};

void InitFilterRule(char *pRulePath);
char* GetFingerFromSnmp(char *pIpAddr, char *pVersion, char  *pKey, int nSnmpType, char *pOid, char* pOutBuffer, int nOutBufferLen);

#endif 