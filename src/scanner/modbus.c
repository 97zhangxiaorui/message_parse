#include "modbus.h"
#include "../net/socket.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct __MODBUS_MBAP
{
	unsigned short int TransactionId;  //事务处理标识码，请求和响应相同
	unsigned short int ProtocolId;     //modbus协议标识 0
	unsigned short int Len;            //数据长度+UnitId（识别码）
	char UnitId;              // 识别码
}ModbusMbapObj, *ModBusMbapPtr;

#define MODBUS_HEAD_LEN 7
#define MODBUS_REQUEST_PDU_LEN 4
#define MODBUS_RESPONS_PDU_LEN 7
#define MODBUS_OBJECT_HEAD_LEN 2

typedef struct __MODBUS_REQPDU
{
	//unsigned char ObjectId;       //vendorName (0x00至0xFF)
	char FunctionCode;   //功能码(0x2B 读设备识别码)
	char MEIType;        //Read Device Identification (0x0E)
	char ReadDevId;      //Base device identification (01/02/03/04)
	char ObjectId;       //vendorName (0x00至0xFF)
}ReqPDUObj, *ReqPDUPtr;


typedef struct __MODBUS_RESPDU
{
	char FunctionCode;		//0x2B
	char MEIType;
	char ReadDevId;
	char ConformityLevel;
	char MoreFollows;
	char NextObjectId;
	char ObjectNumber;
}ResPDUObj, *ResPDUPtr;

typedef struct __MODBUS_OBJECT
{
	char ObjectId;
	char ValueLen;
	char *Value;
}ObjectObj, *ObjectPtr;


int ModbusSanner(const unsigned char *pServerIp, const unsigned int nServerPort, unsigned char *pOutSannerResult, const unsigned int nOutLen)
{
	unsigned int nStream = 1;
	unsigned int nDgram  = 2;
			 int nSocketFd = 0; 

	//printf("%s:%d\n", pServerIp, nServerPort);
	nSocketFd = InitSocket(nStream, pServerIp, nServerPort);
	if ( nSocketFd > 0 )
	{
		unsigned char ReqBuf[1024] = { 0 };
		unsigned int nReqLen = 1024;

		int nPacketLen = EncodeModbus(ReqBuf, nReqLen);
		//printf("PacketLen = %d\n", nPacketLen);
		if ( -1 != nPacketLen )
		{
			//发送报文
			if( SendData(ReqBuf, nPacketLen, nSocketFd) < 0)
			{
				printf("send modbus packet failed \n");
				return -1;
			}

			//接收报文
			unsigned char ResBuf[2048] = { 0 };
			unsigned int  nResLen = 2048;

			int nRecvNumber = 0;
			nRecvNumber = ReciveData(nSocketFd, ResBuf, nResLen);
			if ( nRecvNumber > 0 )
			{
				return DecodeModbus(ResBuf, pOutSannerResult, nOutLen);
			}

		}
	}
}

int EncodeModbus(unsigned char *pOutPacket, const unsigned int nOutLen)
{
	int nHeadLen = 0;
	int nPDULen = 0;

	ModbusMbapObj ModbusHead;
	ReqPDUObj     RequestPDU;

	nHeadLen = sizeof(ModbusMbapObj);
	nPDULen = sizeof(ResPDUObj);
	if ( nOutLen >= nHeadLen + nPDULen )
	{
		ModbusHead.TransactionId = 0x00;
		ModbusHead.ProtocolId = 0x00;
		ModbusHead.Len = htons(0x05);
		ModbusHead.UnitId = 0x00;

		RequestPDU.FunctionCode = 0x2B;
		RequestPDU.MEIType = 0x0E;
		RequestPDU.ReadDevId = 0x01;
		RequestPDU.ObjectId  = 0x00; 

		memcpy(pOutPacket, &ModbusHead, MODBUS_HEAD_LEN);
		memcpy(pOutPacket + MODBUS_HEAD_LEN, &RequestPDU, MODBUS_REQUEST_PDU_LEN);
		return MODBUS_HEAD_LEN + MODBUS_REQUEST_PDU_LEN;
	}
	return -1;
}

int DecodeModbus(unsigned char *pPacket, unsigned char *pOutPacket, const unsigned int nOutLen)
{
	int nResPDULen = 0;
	int nHeadLen = 0;

	if ( NULL != pPacket )
	{
		unsigned short int nProtocol = 0;
		memcpy(&nProtocol, pPacket + 2, 2);
		//MODBUS PROTLCOL
		if ( 0 == nProtocol )
		{
			//check function code 
			if ( 43 == *(pPacket + MODBUS_HEAD_LEN) )
			{
				int nObjectNumber = 0, nObjectTmpNumber = 0;

				unsigned char *pObject = pPacket + MODBUS_HEAD_LEN + MODBUS_RESPONS_PDU_LEN;
				nObjectTmpNumber = pPacket[MODBUS_HEAD_LEN + MODBUS_RESPONS_PDU_LEN - 1];
				//printf("OJECT :%d \n", nObjectTmpNumber);

				//object 
				int nObjectOffset = 0, nOutTmpLen = 0;
				for ( ; nObjectNumber <  nObjectTmpNumber; nObjectNumber++)
				{
					char *pTmpObject = pObject + nObjectOffset;
					int nObjectLen = *(pTmpObject + 1);
					//printf("nObjectLen: %d \n", nObjectLen);
					if ( nOutLen - nOutTmpLen >= nObjectLen )
					{
						memcpy(pOutPacket + nOutTmpLen, pTmpObject + MODBUS_OBJECT_HEAD_LEN, nObjectLen);
						
						nObjectOffset = nObjectOffset + nObjectLen + MODBUS_OBJECT_HEAD_LEN; //
						nOutTmpLen = nOutTmpLen + nObjectLen;
						pOutPacket[nOutTmpLen] = '\n';
						//pOutPacket = pOutPacket + 1;
						nOutTmpLen = nOutTmpLen + 1;
					}
				} 
			}
			else
			{
				printf("The packet is not device function code packet");
				return -1;
			}
		}
		else
		{
			printf("The packet message is not modbus protocol, please check \n");
			return -1;
		}

	}
	return -1;
}
