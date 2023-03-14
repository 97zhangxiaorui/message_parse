#include "tcp.h"
#include "etherip.h"

void GetPortFromTcpBytes( u_char *pBytes, u_int16_t *pDesPort, u_int16_t *pSrcPort )
{
	if ( NULL == pBytes || NULL == pDesPort || NULL == pSrcPort )
	{
		printf(" The param of funtion GetPortFromBytes is null , please check \n");
		return ;
	}

	struct tcphdr *pTcpAddr = NULL;
	int nEtherHeadLen = GetEtherHeadSize();
	int nIpHeadLen = GetIpHeadSize();

	pTcpAddr = (struct tcphdr *)(pBytes + nEtherHeadLen + nIpHeadLen );
	memcpy(pSrcPort, &pTcpAddr->source, sizeof(u_int16_t));
	memcpy(pDesPort, &pTcpAddr->dest, sizeof(u_int16_t));

	return ;
}

int GetTcpHeadSize()
{
	return sizeof(struct tcphdr);
}