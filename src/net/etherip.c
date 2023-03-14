#include <string.h>

#include "etherip.h"

void GetMacFromBytes( u_char *pBytes, u_int8_t *pDesMac, u_int8_t *pSrcMac, u_int16_t *pEtherType, short int *pVlan)
{
	if ( NULL == pBytes || NULL == pDesMac || NULL == pSrcMac )
	{
		printf("The param of the function GetMacFromBytes is null, please check\n" );
		return;
	}

	struct ether_header *pEther = NULL;
	pEther = (struct ether_header *)pBytes;
	memcpy(pDesMac, pEther->ether_dhost, sizeof(pEther->ether_dhost));
	memcpy(pSrcMac, pEther->ether_shost, sizeof(pEther->ether_shost));


	//判断是不是带vlan信息0x8100
	if ( pEther->ether_type == 129 ){
		//int nVlan = 0;

		memcpy(pVlan, pBytes + sizeof(struct ether_header), 2);
		memcpy(pEtherType, pBytes + sizeof(struct ether_header) + 2, 2);
	}
	else{
		memcpy(pEtherType, &(pEther->ether_type), sizeof(pEther->ether_type));
		//printf("VlanId[%u] \n", (*pEtherType));
		//pBytes = pBytes + sizeof(struct ether_header);
	}

	/*
	char TmpBuf[128] = { 0 };
	CharMacToString(pEther->ether_dhost, TmpBuf);
	printf("MAC:%s\n", TmpBuf);
	memset(TmpBuf, 0, sizeof(TmpBuf));
	CharMacToString(pEther->ether_shost, TmpBuf);
	printf("MAC:%s\n", TmpBuf);
	*/	
	
}

//void GetIpFromBytes( u_char *pBytes, u_int32_t *pDesIp, u_int32_t *pSrcIp, u_int8_t *pIpType, u_int16_t * pTotalLen )
void GetIpFromBytes( u_char *pBytes, NetworkMessagePtr pMsg )
{
	if ( NULL == pBytes )
	{
		printf("The param error of function GetIpFromBytes, please check\n");
		return ;
	}

	int nEtherHeadSize = sizeof(struct ether_header);
	struct iphdr *pIpAddr = NULL;

	pIpAddr = (struct iphdr *)(pBytes + nEtherHeadSize);
	pMsg->DesIp = pIpAddr->daddr;
	pMsg->SrcIp = pIpAddr->saddr;
	//memcpy(pDesIp, &pIpAddr->daddr, sizeof(pIpAddr->daddr));	
	//memcpy(pSrcIp, &pIpAddr->saddr, sizeof(pIpAddr->saddr));
	//memcpy(pIpType, &pIpAddr->protocol, sizeof(pIpAddr->protocol));
	pMsg->ProtocalType = pIpAddr->protocol;
	//memcpy(pTotalLen, &pIpAddr->tot_len, sizeof(pIpAddr->tot_len));
	pMsg->PacketLen = pIpAddr->tot_len;
	pMsg->Ttl = pIpAddr->ttl;
}

char * CharMacToString( u_int8_t *pCharMac, char *pStringMac )
{
	if ( NULL != pCharMac )
	{
		sprintf( pStringMac,"%02X:%02X:%02X:%02X:%02X:%02X", pCharMac[0],pCharMac[1],pCharMac[2],pCharMac[3],pCharMac[4],pCharMac[5]);
		return pStringMac;
	}
	else
	{
		printf("The param of funtion CharMacToString is null, please check\n");
		return NULL;
	}

	return pStringMac;
}

char * IntIpToString( u_int32_t nIp, char *pIpBuffer)
{
	if ( nIp > 0 )
	{
		struct in_addr Addr;
		Addr.s_addr = nIp;
		sprintf(pIpBuffer, "%s", inet_ntoa(Addr));
	}
	return pIpBuffer;
}

/*
unsigned int StringIp2Int(char *pIp, unsigned int *nOutIp)
{
	if ( NULL != pIp )
	{
		nOutIp = inet_addr(pIp);
		return nOutIp;
	}
}
*/

int GetEtherHeadSize()
{
	return sizeof(struct ether_header);
}

int GetIpHeadSize()
{
	return sizeof(struct iphdr);
}