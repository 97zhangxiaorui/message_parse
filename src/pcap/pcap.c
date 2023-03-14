#include "pcap.h"
#include "../fingerprint.h"

#define SNAP_LEN 65536

int InitPcap(char *pDeviceName)
{
	if ( NULL != pDeviceName )
	{
		pcap_t *pPcapHandle = NULL;
		char ErrorBuf[PCAP_ERRBUF_SIZE] = { 0 };
		struct bpf_program Fp; /* compiled filter program (expression) */
		char FilterExp[] = "tcp";        /* filter expression [3] */

		//检查网卡，开始监听报文
		pPcapHandle = pcap_open_live(pDeviceName, SNAP_LEN, 1, 1000, ErrorBuf);
		if ( pPcapHandle )
		{
			if (pcap_datalink(pPcapHandle) != DLT_EN10MB)
			{
				printf("%s is not an Ethernet\n", pDeviceName);
				return -1;
			}

			if (pcap_compile(pPcapHandle, &Fp, FilterExp, 0, 24) == -1)
			{
				printf("Couldn't parse filter %s: %s\n",FilterExp, pcap_geterr(pPcapHandle));
				return -1;
			}

			if (pcap_setfilter(pPcapHandle, &Fp) == -1)
			{
				printf("Couldn't install filter %s: %s\n", FilterExp, pcap_geterr(pPcapHandle));
				return -1;
			}

			pcap_loop(pPcapHandle, -1, PcapHandler, NULL);

			pcap_freecode(&Fp);
			pcap_close(pPcapHandle);
		}
		else
		{
			printf("Couldn't open device %s: %s\n", pDeviceName, ErrorBuf);
			return -1;
		}

		printf("pcap init ok \n");
		return 0;
	}
	else
	{
		printf("Device name is null （网卡名称为空），please check device \n");
		return -1;
	}
}

int InitReadPcapFile(char *pFileName)
{
	if ( NULL != pFileName )
	{
		pcap_t *pPcapFile = NULL;
		char ErrorBuf[1024] = { 0 };

		pPcapFile = pcap_open_offline(pFileName, ErrorBuf);
		if ( NULL != pPcapFile )
		{
			int nResult = 0;
			nResult = pcap_loop(pPcapFile, -1, PcapHandler, NULL);
			switch (nResult)
			{
				case 0 :
					printf("pcap_loop is sucessful [%d]\n", nResult);
					break;
				case -1:
					printf("pcap_loop an error occurs[%d]\n", nResult);
					break;
				case -2:
					printf("pcap_loop an error occurs[%d] of the loop terminated due to a call to pcap_breakloop() before any packets were processed\n", nResult);
					break;
				default:
					printf("pcap_loop return unkone error number [%d]\n", nResult);
			}

			pcap_close(pPcapFile);
		}
		else
		{
			printf("Pcap file open failed, init pcap file interface failed\n");
			return -1;
		}

		printf("Init read pcap file interface sucessful!\n");
		return 0;
	}
	else
	{
		printf("File name is null, please check!\n");
		return -1;
	}
}

void PcapHandler(u_char *pUser, const struct pcap_pkthdr *pPkthdr,const u_char *pBytes)
{
	if ( NULL != pBytes )
	{
		char buf[1024] = { 0 };

		NetworkPacketHandle((unsigned char *)pBytes, buf, sizeof(buf) -1 );
		printf("buf[%s]\n", buf);
	}
	else
	{
		printf(" The network package of function PcapHandler param is null, nothing to do\n");
	}
}

