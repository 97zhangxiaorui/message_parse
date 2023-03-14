#ifndef __UDP_H_
#define __UDP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/udp.h>

/*
#ifdef __FAVOR_BSD
struct udphdr
{
  u_int16_t uh_sport;           
  u_int16_t uh_dport;           
  u_int16_t uh_ulen;            
  u_int16_t uh_sum;         
};
#else
struct udphdr
{
  u_int16_t source;
  u_int16_t dest;
  u_int16_t len;
  u_int16_t check;
};
#endif
*/

/*
*pBytes  : in bytes stream
*pDesPort: out destination port
*pSrcPort: out source port
*return  :  
*/
void GetPortFromUdpBytes( u_char *pBytes, u_int16_t *pDesPort, u_int16_t *pSrcPort );

int GetUdpHeadSize();
#endif