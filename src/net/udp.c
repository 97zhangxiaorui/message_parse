#include "etherip.h"
#include "udp.h"

void GetPortFromUdpBytes( u_char *pBytes, u_int16_t *pDesPort, u_int16_t *pSrcPort )
{
	if ( NULL == pBytes || NULL == pDesPort || NULL == pSrcPort )
	{
		printf(" The param of funtion GetPortFromBytes is null , please check \n");
		return ;
	}

	struct udphdr *pUdpAddr = NULL;
	int nEtherHeadLen = GetEtherHeadSize();
	int nIpHeadLen = GetIpHeadSize();

	pUdpAddr = (struct udphdr *)(pBytes + nEtherHeadLen + nIpHeadLen );
	memcpy(pSrcPort, &pUdpAddr->source, sizeof(u_int16_t));
	memcpy(pDesPort, &pUdpAddr->dest, sizeof(u_int16_t));
	return ;
}

int GetUdpHeadSize()
{
	return sizeof(struct udphdr);
}