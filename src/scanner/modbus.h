#ifndef __MODBUS_H_
#define __MODBUS_H_


/*
*pServerIp        : scanner server iip
*nServerPORT      : scanner server port
*pOutSannerResult : save sanner result buffer
*nOutLen          : buffer length
*result           : scanner result length or -1
*/
int ModbusSanner(const unsigned char *pServerIp, const unsigned int nServerPort, unsigned char *pOutSannerResult, const unsigned int nOutLen );

/*
*pOutPacket : buffer for encode modebus
*nOutLen    : buffer len
*return     : Packet length or -1
*/
int EncodeModbus(unsigned char *pOutPacket, const unsigned int nOutLen);

/*
*pPacket    : moubus packet message
*pOutPacket : buffer for encode modebus
*buffer len : buffer len
*return     : 0 or -1
*/
int DecodeModbus(unsigned char *pPacket, unsigned char *pOutPacket, const unsigned int nOutLen);


#endif