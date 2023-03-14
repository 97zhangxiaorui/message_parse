#include "system.h"
#include "../comm/const.h"

int GetSystemByTTL(int nTtl)
{
	if ((128 >= nTtl && nTtl >= 113) || (17 <= nTtl && nTtl <= 32))
	{
		return WINDOWS;
	}

	if ((45 <= nTtl && nTtl <= 64) || (140 <= nTtl && nTtl <= 255))
	{
		return LINUX;
	}

	return 0;
}

int GetDeviceTypeByTtl(int nTtl, char *pVendor)
{
	if (strcasestr(pVendor, "huawei"))
	{
		//switch
		if (140 <= nTtl && nTtl <= 255)
			return 7; //网络交换设备
		if (45 <= nTtl && nTtl <= 64)
			return 3;//移动设备，手机或者pad
		if ((128 >= nTtl && nTtl >= 113) || (17 <= nTtl && nTtl <= 32))
			return 1;//笔记本或者台式机
	}

	if (strcasestr(pVendor, "xiaomi"))
	{
		if (140 <= nTtl && nTtl <= 255)
			return 1; //网络交换设备
		if (45 <= nTtl && nTtl <= 64)
			return 3;//移动设备，手机或者pad
		if ((128 >= nTtl && nTtl >= 113) || (17 <= nTtl && nTtl <= 32))
			return 2;//笔记本
	}
	return 0;
}

int GetDeviceTypeByVendor(char *pVendor)
{
	if (pVendor)
	{
		//PLC Schneider
		if (strcasestr(pVendor, "Rockwell") || strcasestr(pVendor, "General Electric") || strcasestr(pVendor, "Siemens") || \
			strcasestr(pVendor, "Schneider") || strcasestr(pVendor, "Omron") || strcasestr(pVendor, "Mitsubishi") || \
			strcasestr(pVendor, "Danfoss") || strcasestr(pVendor, "Unitronics") || strcasestr(pVendor, "HollySys"))
		{
			return 4;//PLC
		}

		//switch
		if (strcasestr(pVendor, "Ericsson") || strcasestr(pVendor, "Cisco") || strcasestr(pVendor, "zte") || \
			strcasestr(pVendor, "H3C") || strcasestr(pVendor, "TP-LINK") || strcasestr(pVendor, "D-Link") || strcasestr(pVendor, "3COM") || \
			strcasestr(pVendor, "Tenda") || strcasestr(pVendor, "Ruijie") || strcasestr(pVendor, "Kyland") || strcasestr(pVendor, "Raisecom") || \
			strcasestr(pVendor, "RADWIN") || strcasestr(pVendor, "SHENZHEN FAST"))
		{
			//|| strcasestr(vendor, "Huawei")
			return 7;//switch
		}

		//pc
		if (strcasestr(pVendor, "Dell") || strcasestr(pVendor, "Lenovo") || strcasestr(pVendor, "IBM") || strcasestr(pVendor, "acer") || \
			strcasestr(pVendor, "apple") || strcasestr(pVendor, "Hewlett") || strcasestr(pVendor, "Xteam") || \
			strcasestr(pVendor, "VMware") || strcasestr(pVendor, "Tyan Computer") || strcasestr(pVendor, "Universal Global Scientific")||\
			strcasestr(pVendor, "Intel Corporation"))
		{
			//|| strcasestr(vendor, "xiaomi")
			return 1; //台式机/笔记本/移动设备
		}

		//camera
		if (strcasestr(pVendor, "Dahua") || strcasestr(pVendor, "Hikvision") || strcasestr(pVendor, "uniview") || strcasestr(pVendor, "CODEX CORPORATION"))
		{
			return 6;//摄像头
		}

		//rtu
		if (strcasestr(pVendor, "Micro-Star"))
		{
			return 5;//RTU
		}

		if (strcasestr(pVendor, "TRANSMITTON"))
		{
			return 12;//ESD
		}

		if (strcasestr(pVendor, "Techno Trade"))
		{
			return 13;//网关设备
		}

		if (strcasestr(pVendor, "AEWIN") || strcasestr(pVendor, "LANNER"))
		{
			return 14;//网络安全设备
		}		
	}
}