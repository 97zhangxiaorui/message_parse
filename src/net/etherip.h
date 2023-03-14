#ifndef __ETHERIP_H_
#define __ETHERIP_H_

#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <netinet/ip.h>

#include "../comm/const.h"

/* struct ethhdr {
 * unsigned char h_dest[ETH_ALEN];
 * unsigned char h_source[ETH_ALEN];
 * __be16 h_proto;
 * } __attribute__((packed));
 *
 * struct ether_header
 * {
 * u_int8_t ether_dhost[ETH_ALEN];      // destination eth addr
 * u_int8_t ether_shost[ETH_ALEN];      // source ether addr
 * u_int16_t ether_type;                 // packet type ID field
 * } __attribute__ ((__packed__));
 *
 *
 *struct iphdr
 *{
 *#if __BYTE_ORDER == __LITTLE_ENDIAN
 *    unsigned int ihl:4;
 *    unsigned int version:4;
 *#elif __BYTE_ORDER == __BIG_ENDIAN
 *    unsigned int version:4;
 *    unsigned int ihl:4;
 *#else
 *# error "Please fix "
 *#endif
 *    u_int8_t tos;
 *    u_int16_t tot_len;
 *    u_int16_t id;
 *    u_int16_t frag_off;
 *    u_int8_t ttl;
 *    u_int8_t protocol;
 *    u_int16_t check;
 *    u_int32_t saddr;
 *    u_int32_t daddr;
 *};
*/

/*
*pBytes    : net package
*pDesMac   : out destination mac
*pSrcMac   : out source mac
*pEtherType: The protocol of next layer 
*return    :
*/
//void GetMacFromBytes( u_char *pBytes, u_int8_t *pDesMac, u_int8_t *pSrcMac, u_int16_t *pEtherType);
void GetMacFromBytes( u_char *pBytes, u_int8_t *pDesMac, u_int8_t *pSrcMac, u_int16_t *pEtherType, short int *pVlan);

/*
*pBytes : net package
*pDesIP : destination ip address
*pSrcMac: source  ip addresss
*pIpType: The protocol of next layer 
*return :
*/
//void GetIpFromBytes( u_char *pBytes, u_int32_t *pDesIp, u_int32_t *pSrcIp, u_int8_t *pIpType, u_int16_t * pTotalLen );
void GetIpFromBytes( u_char *pBytes, NetworkMessagePtr pMsg );

/*
*pCharMac  : ungined char mac addr
*pStringMac: out string mac address
*return    : return string mac address
*/
char * CharMacToString( u_int8_t *pCharMac, char *pStringMac );

/*
*nIp    : Int type ip address
*pIp    : Out the string ip address
*return : return string address
*/
//char * IntIpToString( u_int32_t nIp, char *pIpBuffer);

int GetEtherHeadSize();
int GetIpHeadSize();

#endif
