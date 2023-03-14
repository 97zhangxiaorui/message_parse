#ifndef __SYSTEM_H__
#define __SYSTEM_H__

/*
*支持通过地方工具获取操作系统信息
*支持通过自分析类型获取操作系统信息
*支持主动方式、被动方式识别操作系统信息
*/

int GetSystemByTTL(int nTtl);
int GetDeviceTypeByVendor(char *pVendor);
int GetDeviceTypeByTtl(int nTtl, char *pVendor);

#endif