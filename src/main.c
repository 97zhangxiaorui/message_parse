 #include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "fingerinterface.h"
#include "scanner/snmp.h"

int test(char *pFinger){
	printf("测试函数\n");
	return 0;
}

static void usage(void)
{
	fprintf(stderr,
		"scan device fingerprint program: 通过主动发送探测报文，获取设备指纹（指纹：设备类型、设备厂商、操作系统类型、软件版本、设备型号、MAC地址等）\n"
		"    usage: ScanDeviceFingerprint [-i interface] [-f pcap file] [ -d ipaddr ] [-p port] [-L] [-t protocol(snmp、s7、modbus、eip等)]\n"
		"	 		[-v snmp version ] [-P snmp password] [-h help]\n"
		);
	exit(1);
}

/*
*IP,协议,端口
*/
int main(int argc, char **argv)
{
	char TmpBuffer[1024] = { 0 };
	char ch;
	char *pIp = NULL, *pProtocol = NULL, *pInterface = NULL, *pSnmpPwd = NULL, *pSnmpVersion = NULL, *pPcapFile = NULL;
	int nPort = 0;

	while ((ch = getopt(argc, argv, "hi:f:d:t:p:v:P")) != -1) {
		switch (ch) {
			case 'h':
				usage();
				break;
			case 'i':	/* interface */
				pInterface = optarg;
				break;
			case 'f':
				pPcapFile = optarg;
				break;
			case 'd':
				pIp = optarg;
				break;
			case 'p':
				nPort = atoi(optarg);
				break;
			case 't':
				pProtocol = optarg;
				break;
			case 'v':
				pSnmpVersion = optarg;
				break;
			case 'P':
				pSnmpPwd = optarg;
				break;
			default:
				return 0;
				/* fallthrough */
		}
	}

	//初始化函数
	//InitFinger("../fingerprint", "conf/rule/rule.conf");
	InitFinger(NULL, NULL);
	//return 0;
	//如果是读取配置pcap文件
	if ( pPcapFile ){
		GetFingerprintFromPcapFile(pPcapFile, test);
		PrintHash();
		return 0;
	}

	if ( argc < 3 )
	{
		usage();
		return -1;
	}

	
	//printf( "%s\n", GetDeviceFingerFromDetection(argv[1], 2, atoi(argv[2]),  NULL, NULL) );
	//GetDeviceFingerFromDetection(argv[1], atoi(argv[3]), atoi(argv[2]),  NULL, NULL);
	//GetFingerprintFromDetection(argv[1], atoi(argv[3]), atoi(argv[2]),  NULL, NULL, TmpBuffer, sizeof(TmpBuffer));
	//printf("%s", TmpBuffer);
	//GetDeviceFingerFromPcapFile(argv[1], argv[2], test);
	//GetFingerprintFromPcapFile(argv[1], argv[2], test);
	if ( pIp && pProtocol && nPort > 0 ){
		ScanDeivceFingerprinter(pIp, pProtocol, nPort, pSnmpPwd, pSnmpVersion, TmpBuffer, sizeof(TmpBuffer));
		if ( strlen(TmpBuffer) > 5 ){
			printf("%s", TmpBuffer);
		}
	}
}