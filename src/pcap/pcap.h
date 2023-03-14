#ifndef __PCAP_H_
#define __PCAP_H_

#include <stdlib.h>
#include <stdio.h>
#include <pcap/pcap.h>

/*
*pDeviceName: 网卡名称
*return：0 ok， -1 failed
*/
int InitPcap(char *pDeviceName);

/*
*pFileName:file  name (文件名称)
*return: 0 sucessful, -1 failed
*/
int InitReadPcapFile(char *pFileName);

/*
*pUser  : user data
*pPkthdr: package time ，length， data length
*u_char : data
*return : 
*/
void PcapHandler(u_char *pUser, const struct pcap_pkthdr *pPkthdr,const u_char *pBytes);

#endif