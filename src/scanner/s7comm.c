#include "s7comm.h"
#include "../net/socket.h"

#pragma pack(1)

char gTmpFingerprint[1024];

typedef struct __TPKT_
{
	unsigned char Version;
	unsigned char Reserverd;
	unsigned short Length;
}TpktObj, *TpktPtr;

typedef struct __COTP_DATA_
{
	unsigned char Length;
	unsigned char PduType;
	unsigned char DesRef;
}CotpDataObj, *CotpDataPtr;

typedef struct __COTP_CONNECT_
{
	unsigned char Length;
	unsigned char PduType;
	unsigned short int DesRef;
	unsigned short int SrcRef;
	unsigned char Class;
	unsigned char SrcTsap;
	unsigned char SrcTsapLen;
	unsigned short int SrcTsapData;
	unsigned char DstTsap;
	unsigned char DstTsapLen;
	unsigned short int DstTsapData;
	unsigned char TpduSize;
	unsigned char TpduSizeLen;
	unsigned char TpduSizeData;
}CotpConnectObj, *CotpConnectPtr;


typedef struct __S7_COMM_HEAD_
{
	unsigned char ProtocolId;
	unsigned char MsgType;  //[1,7]
	unsigned short int Reserverd;
	unsigned short int ReqId;
	unsigned short int ParamLen;
	unsigned short int DataLen;
}S7commHeadObj, *S7commHeadPtr;

typedef struct __S7_COMM_PARAM_
{
	unsigned char Head[3];
	unsigned char Length;
	unsigned char Method; //0x11 request
	unsigned char TypeFunction;
	unsigned char Function;
	unsigned char Sequence;
}S7commParamObj, *S7commParamPtr;

typedef struct __S7_COMM_DATA_
{
	unsigned char ReturnCode;
	unsigned char TransportSize;
	unsigned short Length;
	unsigned short SzlId;
	unsigned short SzlIndex;
}S7commDataObj, *S7commDataPtr;

int DecodeTpkt(char *pInMsg)
{
	if ( TPKT_PROTOCAL == *pInMsg )
	{
		pInMsg = pInMsg + TPKT_LENGTH;
		//printf("in function %s \n", __FUNCTION__);
		return 0;
	}
	printf("packet is not Tpkt \n");
	return -1;
}

int EncodeTpkt(char *pOutMsg, int nBufLen, int nDataLen)
{
	TpktObj TmpTpkt;
	if ( nBufLen >= TPKT_LENGTH )
	{
		//printf("in function %s \n", __FUNCTION__);
		TmpTpkt.Version = 3;
		TmpTpkt.Reserverd = 0;
		TmpTpkt.Length = htons(TPKT_LENGTH + nDataLen);
		memcpy(pOutMsg, &TmpTpkt, TPKT_LENGTH);
		return TPKT_LENGTH;
	}

	return -1;
}

int DecodeIso(char *pInMsg)
{
	printf("in function %s \n", __FUNCTION__);
	/*
	int nTmp = 0;
	nTmp = *pInMsg + 1;
	pInMsg = pInMsg + nTmp;
	*/
	pInMsg = pInMsg + ISO_LENGTH;
	return ISO_LENGTH;
}

int EncodeIso(char *pOutMsg, int nBufLen)
{
	if ( nBufLen >= ISO_LENGTH )
	{
		printf("in function %s \n", __FUNCTION__);
		CotpDataObj TmpCotpData;
		TmpCotpData.Length = 2;
		TmpCotpData.PduType = 0xF0; 
		TmpCotpData.DesRef = 0x80;
		memcpy(pOutMsg, &TmpCotpData, ISO_LENGTH);
		return ISO_LENGTH;
	}
	return -1;
}

int S7Error(unsigned short nErrorCode )
{
	switch (nErrorCode)
	{
		case 0x05 :
			printf("Address error \n");
			break;
		case 0x0A :
			printf("Item not available\n");
			break;
		case 0x8014 :
			printf("Context not supported \n");
			break;
		case 0x8500 :
			printf("Wrong PDU  size\n");
			break;
		default:
			printf("Unkown error\n");
	}
}

int DecodeS7Comm(char *pInMsg)
{
	unsigned short nS7HeadLen = 0;
	unsigned short nParamLen = 0, nDataLen = 0;
	unsigned short nError = 0;
	pInMsg = pInMsg + ISO_LENGTH + TPKT_LENGTH;
	//printf("TYPE: %02x \n", pInMsg[1]);
	//printf("S7COMM:%02x - %02x -%02x- %02x -%02x -%02x \n", pInMsg[0], pInMsg[1], pInMsg[2], pInMsg[3], pInMsg[4], (unsigned char)pInMsg[5] );
	if ( pInMsg[1] == 3 || 2 == pInMsg[1] )
	{
		nS7HeadLen = 12;
		unsigned char TmpBuf[16] = { 0 };

		memcpy(&nError, pInMsg + 10, 2);
		S7Error(nError);
		return -1;

	}
	else if ( pInMsg[1] == 7 || 1 == pInMsg[1]  )
	{
		nS7HeadLen = 10;
		unsigned char tmp[6] = { 0 };

		//printf("len==: %02x-%02x-%02x-%02x \n", (unsigned char)pInMsg[6],(unsigned char)pInMsg[7],(unsigned char)pInMsg[8],(unsigned char)pInMsg[9]);
		//memcpy(&nParamLen, pInMsg + 6, 2);
		memcpy(tmp, pInMsg + 6, 2);
		memcpy(&nParamLen, tmp, 2);
		nParamLen = ntohs(nParamLen);

		memset(tmp, 0, sizeof(tmp));
		//memcpy(&nDataLen, pInMsg + 8, 2);
		memcpy(tmp, pInMsg + 8, 2);
		memcpy(&nDataLen, tmp, 2);
		nDataLen = ntohs(nDataLen);

		//printf("LEN[%d-%d] \n", nParamLen, nDataLen);
	}
	else
	{
		printf("resopnse packet is error, please check \n");
		return -1;
	}

	unsigned char MsgBuf[ 8192 ] = { 0 };
	memcpy(MsgBuf, pInMsg + nS7HeadLen + nParamLen, nDataLen);
	DeviceMsg(MsgBuf);
	return 0;
}

char* DeviceMsg(unsigned char *pMsg)
{
	unsigned char Code, TransportSize;
	unsigned short DataLen = 0;

	//Code = *pMsg;
	TransportSize = *(pMsg+1);
	memcpy(&DataLen, pMsg + 2, 2);
	DataLen = ntohs(DataLen);

	//printf("data==: %02x-%02x-%02x-%02x \n", (unsigned char)pMsg[0],(unsigned char)pMsg[1],(unsigned char)pMsg[2],(unsigned char)pMsg[3]);
	if ( pMsg[0] != 0xff )
	{
		S7Error(Code);
		return NULL;
	}

	int nLen = 4;
	unsigned short nSZLId = 0, nSZLIndex = 0, nEleMentSize = 0, nElementCount = 0;

	memcpy(&nSZLId, pMsg + nLen, 2);
	nSZLId = ntohs(nSZLId);
	memcpy(&nSZLIndex, pMsg + nLen + 2, 2);
	nSZLIndex = ntohs(nSZLIndex);
	memcpy(&nEleMentSize, pMsg + nLen + 4, 2);
	nEleMentSize = ntohs(nEleMentSize);
	memcpy(&nElementCount, pMsg + nLen + 6, 2);
	nElementCount = ntohs(nElementCount);

	int nHeadLen = 8;
	char TmpMsg [2048] = { 0 };
	//snprintf(TmpMsg, sizeof(TmpMsg) - 1,"%s", MsgInfo(pMsg + nLen + nHeadLen, nEleMentSize, nSZLId));
	MsgInfo(TmpMsg, sizeof(TmpMsg), pMsg + nLen +  nHeadLen, DataLen - nLen - nLen, nEleMentSize, nSZLId);
	//printf("DEVICE MESSAGE: %s \n", TmpMsg);

	int nTmpLen = strlen(gTmpFingerprint);
	snprintf(gTmpFingerprint + nTmpLen, sizeof(gTmpFingerprint) - 1 - nTmpLen, "%s", TmpMsg);
	return TmpMsg;
}

char* MsgInfo(char *pMsgBuf, int buffer_len, unsigned char *pInMsg, unsigned int MsgLen, unsigned int nStep, unsigned int nSzlId )
{
	unsigned int i = 0, nInMsgLen = 0, nX11 = 0X11, nX1c = 0X1C, nMsgLen = 0;
	//char MsgBuf[4096] = { 0 };

	if( NULL != pInMsg )
	{
		nInMsgLen = MsgLen;
		//printf("LLLLLLLLLLLLen-[%d][%d] [%d]\n", nInMsgLen, nStep, nSzlId);
		if ( nX11 == nSzlId )
		{

			for ( ; i <  nInMsgLen ; i = i + nStep )
			{
				unsigned int nIndex = 0, nLen = 0;
				memcpy(&nIndex, pInMsg + i, 2);
				nIndex = ntohs(nIndex);
				char Tmp[128] = { 0 };
				//printf("InDex= %d \n", nIndex);
				char *pMsg = MsgSzlId0X11(Tmp, sizeof(Tmp), nIndex, pInMsg + i + 2);
				if ( NULL != pMsg)
				{
					nLen = strlen(pMsg);
					snprintf(pMsgBuf + nMsgLen, buffer_len - nMsgLen - 1, "%s\n", pMsg);
					nMsgLen = nMsgLen + nLen + 1;
				}
			}
		}
		else if ( nX1c == nSzlId )
		{
			for ( ; i <  nInMsgLen ; i = i + nStep )
			{
				unsigned int nIndex = 0, nLen = 0;
				memcpy(&nIndex, pInMsg + i, 2);
				nIndex = ntohs(nIndex);
				//printf("InDex= %d \n", nIndex);
				char Tmp[128] = { 0 };
				char *pMsg = MsgSzlId0X1c(Tmp, sizeof(Tmp), nIndex, pInMsg + i + 2);
				if ( NULL != pMsg)
				{
					nLen = strlen(pMsg);
					snprintf(pMsgBuf + nMsgLen, buffer_len - nMsgLen - 1, "%s\n", pMsg);
					nMsgLen = nMsgLen + nLen + 1;
				}
			}
		}
		else
		{
			printf("Unknow Response [%d]\n", nSzlId);
			return NULL;
		}

		return pMsgBuf;
	}
	return NULL;
}

char* MsgSzlId0X11(char *pOutBuffer, int BufferLen, int nIndex, char *pInMsg)
{
	//Module Identification
	int nDesLen = 20;

	char DesBuf[128] = { 0 };
	unsigned short nVersion1 = 0, nVersion2 = 0, nVersion3 = 0;
	//char DeviceBuf[1024] = { 0 }; 
	switch(nIndex)
	{
		case 1 :
			memcpy(DesBuf, pInMsg, nDesLen);
			memcpy(&nVersion1, pInMsg + nDesLen + 2, 2);
			nVersion1 = ntohs(nVersion1);
			memcpy(&nVersion2, pInMsg + nDesLen + 4, 2);
			nVersion2 = ntohs(nVersion2);

			//memcpy(&nVersion2, pInMsg + nDesLen + 6, 2);
			snprintf(pOutBuffer, BufferLen - 1,"Module:%s v.%d.%d", DesBuf, nVersion1, nVersion2);
			printf("pOutBuffer:%s \n", pOutBuffer);
			break;
		case 6 :
			memcpy(DesBuf, pInMsg, nDesLen);
			memcpy(&nVersion1, pInMsg + nDesLen + 2, 2);
			nVersion1 = ntohs(nVersion1);
			memcpy(&nVersion2, pInMsg + nDesLen + 4, 2);
			nVersion2 = ntohs(nVersion2);
			snprintf(pOutBuffer, BufferLen - 1,"Basic Hardware:%s v.%d.%d", DesBuf, nVersion1, nVersion2);
			break;
		case 7 :
			memcpy(DesBuf, pInMsg, nDesLen);
			memcpy(&nVersion1, pInMsg + nDesLen + 2, 2);
			nVersion1 = ntohs(nVersion1);
			memcpy(&nVersion2, pInMsg + nDesLen + 4, 2);
			nVersion2 = ntohs(nVersion2);
			memcpy(&nVersion3, pInMsg + nDesLen + 6, 2);
			nVersion3 = ntohs(nVersion3);
			snprintf(pOutBuffer, BufferLen - 1,"Basic Firmware:%s v.%d.%d.%d", DesBuf, nVersion1, nVersion2, nVersion3);
			break;
		default :
			memcpy(pOutBuffer, "unknown", sizeof("unknown"));
	}
	return pOutBuffer;
}

char* MsgSzlId0X1c(char *pOutBuffer, int BufferLen, int nIndex, char *pInMsg)
{
	//Component Identification
	int nLen1 = 24, nLen2 = 32, nLen3 = 26;
	char Version[128] = { 0 };
	char DeviceBuf[1024] = { 0 };
	switch(nIndex)
	{
		case 1 :
			memcpy(Version, pInMsg, nLen1);
			snprintf(pOutBuffer, BufferLen - 1, "Name of the PLC: %s", Version);
			break;
		case 2 :
			memcpy(Version, pInMsg, nLen1);
			snprintf(pOutBuffer, BufferLen - 1, "Name of the module: %s", Version);
			break;
		case 3 :
			memcpy(Version, pInMsg, nLen2);
			snprintf(pOutBuffer, BufferLen - 1, "Plant identification: %s", Version);
			break;
		case 4 :
			memcpy(Version, pInMsg, nLen3);
			snprintf(pOutBuffer, BufferLen - 1, "Copyright: %s", Version);
			break;
		case 5 :
			memcpy(Version, pInMsg, nLen1);
			snprintf(pOutBuffer, BufferLen - 1, "Serial number of module: %s", Version);
			break;
		case 6 :
			strcpy(pOutBuffer, "Reserved for operating system:");
			break;
		case 7 :
			memcpy(Version, pInMsg, nLen2);
			snprintf(pOutBuffer, BufferLen - 1, "Module type name: %s", Version);
			break;
		case 8 :
			memcpy(Version, pInMsg, nLen2);
			snprintf(pOutBuffer, BufferLen - 1, "Serial number of memory card: %s", Version);
			break;
		case 9 :
			strcpy(pOutBuffer, "Manufacturer and profile of a CPU module:");
			break;
		case 10 :
			strcpy(pOutBuffer, "OEM ID of a module:");
			break;
		case 11 :
			strcpy(pOutBuffer, "Location designation of a module:");
			break;
		default:
			strcpy(pOutBuffer, "Unknown:");
	}
	return pOutBuffer;
}

//szlId=0x11,0x1c
int EncodeS7Comm(unsigned short SzlId, char *pOutMsg, int nBufLen)
{
	int nParamLen = 8, nDataLen = 8, nHeadLen = 10;
	int nParamHeadLen = 3;
	unsigned int nHead = 0x120100;

	//memset(pOutMsg, 0, nBufLen);

	if ( nBufLen >= nParamLen + nDataLen + nHeadLen + TPKT_LENGTH + ISO_LENGTH )
	{

		S7commDataObj TmpS7Data;
		TmpS7Data.ReturnCode = 0xFF;
		TmpS7Data.TransportSize = 0x09;
		TmpS7Data.Length = htons(0x04);
		TmpS7Data.SzlId = htons(SzlId);
		//TmpS7Data.SzlId = SzlId;
		TmpS7Data.SzlIndex = htons(0x01);

		S7commParamObj TmpS7Param;
		memset(TmpS7Param.Head, 0, nParamHeadLen);
		memcpy(TmpS7Param.Head, &nHead, nParamHeadLen);
		TmpS7Param.Length = 0x4;
		TmpS7Param.Method = 0x11; //0x11 request
		TmpS7Param.TypeFunction = 0x44;
		//memcpy(&TmpS7Param.TypeFunction, &(0x44), 1);
		TmpS7Param.Function = 0x01;
		TmpS7Param.Sequence = 0x00;

		S7commHeadObj TmpS7Head;
		TmpS7Head.ProtocolId = 0x32;
		TmpS7Head.MsgType = 0x07;
		TmpS7Head.Reserverd = 0x00;
		TmpS7Head.ReqId = 0x00;
		TmpS7Head.ParamLen = htons(nParamLen);
		//TmpS7Head.ParamLen = nParamLen;
		//TmpS7Head.DataLen = nDataLen;
		TmpS7Head.DataLen = htons(nDataLen);

		unsigned int nTpktLen = 0;
		if ( EncodeTpkt(pOutMsg, nBufLen, nParamLen + nDataLen + nHeadLen + ISO_LENGTH ) > 0 )
		{
			if ( EncodeIso(pOutMsg + TPKT_LENGTH, nBufLen - TPKT_LENGTH ) > 0 )
			{
				memcpy(pOutMsg + TPKT_LENGTH + ISO_LENGTH, &TmpS7Head, nHeadLen);
				memcpy(pOutMsg + TPKT_LENGTH + ISO_LENGTH + nHeadLen, &TmpS7Param, nParamLen);
				memcpy(pOutMsg + TPKT_LENGTH + ISO_LENGTH + nHeadLen + nParamLen, &TmpS7Data, nDataLen);
			}
		}
		char *pA = (char *)&TmpS7Param;
		//printf("4444:%d\n", TmpS7Param.TypeFunction);
		return nParamLen + nDataLen + nHeadLen + TPKT_LENGTH + ISO_LENGTH;
	}
	return -1;
}

int EncodeS7Param( S7commParamPtr pS7Param, unsigned char S7Type, unsigned char S7Group, unsigned char S7Function )
{
	int nParamLen = 0;
	if ( NULL != pS7Param )
	{
		unsigned int nHead = 0x120100;
		int nParamHeadLen = 3;

		memset(pS7Param->Head, 0, nParamHeadLen);
		memcpy(pS7Param->Head, &nHead, nParamHeadLen);
		pS7Param->Length = 0x4;
		pS7Param->Method = 0x11; //0x11 request
		pS7Param->TypeFunction = S7Type*0x10+S7Group;//0x44;
		pS7Param->Function = S7Function;//0x01;
		pS7Param->Sequence = 0x00;

		nParamLen = 8;
	}
	return nParamLen;
}

int EncodeS7Data(S7commDataPtr pS7Data, unsigned short int SzlId)
{
	int nDataLen = 0;
	if ( NULL != pS7Data )
	{
		pS7Data->ReturnCode = 0xFF;
		pS7Data->TransportSize = 0x09;
		pS7Data->Length = htons(0x04);
		pS7Data->SzlId = htons(SzlId);
		pS7Data->SzlIndex = htons(0x01);
		nDataLen = 8;
	}

	return nDataLen;
}

int EncodeS7Head( S7commHeadPtr pS7Head, unsigned char S7Type, unsigned short int S7ParamLen, unsigned short int S7DataLne)
{
	int nHeadLen = 0;
	if ( NULL != pS7Head)
	{
		pS7Head->ProtocolId = 0x32;
		pS7Head->MsgType = S7Type;
		pS7Head->Reserverd = 0x00;
		pS7Head->ReqId = 0x00;
		pS7Head->ParamLen = htons(S7ParamLen);
		pS7Head->DataLen = htons(S7DataLne);
		nHeadLen = 10;
	}

	return nHeadLen;
}

int DecodeCotp(char *pInMsg)
{
	if ( pInMsg > 0 )
	{
		//printf("DATA: %x %x\n", pInMsg[0], pInMsg[1] );
		if ( (unsigned char)pInMsg[1] != 0xd0 )
		{
			return -1;
		}
		return 0;
	}
	return -1;
}

//SrcTsap = 0x100,0x200
//DstTsap = 0x102,0x200,0x201
int EncodeCotp(char *pOutMsg, int nBufLen, unsigned short int nSrcTsap, unsigned short int nDstTsap)
{
	CotpConnectObj TmpCotpConn;
	//unsigned int nIsoLen = 

	TmpCotpConn.Length = 17;
	if ( nBufLen > TmpCotpConn.Length + TPKT_LENGTH + 1 )
	{
		TmpCotpConn.PduType = 0xE0;
		TmpCotpConn.DesRef = 0;
		//TmpCotpConn.SrcRef = 0x04;
		TmpCotpConn.SrcRef = htons(0x10);
		TmpCotpConn.Class =  0;
		TmpCotpConn.SrcTsap = 0xc1;
		TmpCotpConn.SrcTsapLen = 2;
		TmpCotpConn.SrcTsapData = htons(nSrcTsap);
		TmpCotpConn.DstTsap = 0xc2;
		TmpCotpConn.DstTsapLen = 2;
		TmpCotpConn.DstTsapData = htons(nDstTsap);
		TmpCotpConn.TpduSize = 0xc0;
		TmpCotpConn.TpduSizeLen = 1;
		TmpCotpConn.TpduSizeData = 0x0a;

		unsigned int nTpktLen = 0;
		nTpktLen = EncodeTpkt(pOutMsg, nBufLen, TmpCotpConn.Length + 1);
		if ( nTpktLen > 0 )
		{
			memcpy(pOutMsg + nTpktLen, &TmpCotpConn, sizeof(TmpCotpConn));//TmpCotpConn.Length + 1);
		}

		return TmpCotpConn.Length + TPKT_LENGTH + 1;
	}
	return -1;
}

int NegotiatePDU(unsigned char *pOutBuf, unsigned int nOutBufLen )
{
	int nParamLen = 0;
	if (  NULL != pOutBuf )
	{
		unsigned short int nJobs1 = htons(0x01), nPdu = htons(480);

		pOutBuf[0] = 0xF0;
		pOutBuf[1] = 0x00;
		memcpy(pOutBuf + 2, &nJobs1, 2);
		memcpy(pOutBuf + 4, &nJobs1, 2);
		memcpy(pOutBuf + 6, &nPdu, 2);

		nParamLen = 8;
	}

	return nParamLen;

}

int EncodeS7Job(unsigned char *pOutBuf, unsigned int nBufLen)
{
	int nParamLen = 0, nHeadLen = 0, nDataLen = 0;
	if ( NULL != pOutBuf )
	{
		unsigned char TmpBuf[24] = { 0 };

		nParamLen = NegotiatePDU(TmpBuf, sizeof(TmpBuf));
		if ( nParamLen > 0 )
		{
			S7commHeadObj S7Head;

			nHeadLen = EncodeS7Head(&S7Head, 0x01, nParamLen, 0);
			if ( nHeadLen > 0 && nBufLen >= nHeadLen + nParamLen )
			{
				//int nTptkLen = 0, nIsoLen = 0;;
				if ( EncodeTpkt(pOutBuf, nBufLen, nParamLen + nDataLen + nHeadLen + ISO_LENGTH ) > 0 )
				{
					if ( EncodeIso(pOutBuf + TPKT_LENGTH, nBufLen - TPKT_LENGTH ) > 0 )
					{
						memcpy(pOutBuf + TPKT_LENGTH + ISO_LENGTH, &S7Head, nHeadLen);
						memcpy(pOutBuf + TPKT_LENGTH + ISO_LENGTH + nHeadLen, TmpBuf, nParamLen);
						return nParamLen + nHeadLen + TPKT_LENGTH + ISO_LENGTH;
					}
				}
			}
		}
	}
	return 0;
}

int EncodeUserdata(unsigned char *pOutBuf, unsigned int nOutBufLen, unsigned short int SzlId)
{
	return 0;
}

int Encode( unsigned char *pIp, unsigned int nPort, unsigned short int nSrcTsap, unsigned short int nDstTsap )
{
	unsigned int nStream = 1;
	unsigned int nDgram  = 2;
	int nSocketFd = 0;

	nSocketFd = InitSocket(nStream, pIp, nPort);
	if ( -1 == nSocketFd )
	{
		printf("TCP connect error,server ip %s:%d\n", pIp, nPort);
		return 0;
	}

	//encode cotp 
	unsigned char TmpBuf[2048] = { 0 };
	unsigned int nLength = 0;
	nLength = EncodeCotp(TmpBuf, sizeof(TmpBuf), nSrcTsap, nDstTsap);
	if ( nLength > 0 )
	{
		int nSendNum = 0;

		//send 
		nSendNum = SendData(TmpBuf, nLength, nSocketFd);
		if ( nSendNum > 0 )
		{	
			unsigned char RecvBuf[2048] = { 0 };
			int nRecvLen = 0;

			nRecvLen = ReciveData(nSocketFd, RecvBuf, sizeof(RecvBuf));
			if ( nRecvLen > 0 )
			{
				char *pTmpData = RecvBuf;
				//decode 
				if ( -1 == DecodeTpkt(pTmpData) )
				{
					printf("decode tpkt head error, please check\n");
					CloseSocket(nSocketFd);
					return -1;
				} 

				//decode cotp
				//printf("%02x - %02x -%02x- %02x -%02x -%02x \n", pTmpData[0], pTmpData[1], pTmpData[2], pTmpData[3], pTmpData[4], (unsigned char)pTmpData[5] );
				if ( -1 == DecodeCotp(pTmpData + TPKT_LENGTH) )
				{
					printf("decode cotp error, please check\n");
					CloseSocket(nSocketFd);
					return -1;
				}

				//Send Job
				int nJobLen = 0;
				unsigned char JobBuf[64] = { 0 };
				nJobLen = EncodeS7Job(JobBuf, sizeof(JobBuf));
				//printf("===:%d \n", JobBuf[0]);
				if ( nJobLen <= 0 )
				{
					printf("send job failed, please check\n");
					CloseSocket(nSocketFd);
					return -1;
				}

				int len = 0;
				len = SendData(JobBuf, nJobLen, nSocketFd);
				//printf("SEND-LEN: %d-%d \n", nJobLen, len);
				ReciveData(nSocketFd, RecvBuf, sizeof(RecvBuf));


				//encode s7
				unsigned short Szl[2] = { 0x11,0x1C };
				unsigned int nNum = 0;
				for ( ; nNum < 2; nNum++ )
				{
					memset(TmpBuf, 0, sizeof(TmpBuf));
					unsigned int nS7Num = 0;
					nS7Num = EncodeS7Comm(Szl[nNum], TmpBuf, sizeof(TmpBuf));
					if ( nS7Num > 0 )
					{
						//send 
						if ( SendData(TmpBuf, nS7Num, nSocketFd) > 0)
						{
							memset(RecvBuf, 0, sizeof(RecvBuf));
							if ( ReciveData(nSocketFd, RecvBuf, sizeof(RecvBuf)) > 0 )
							{
								unsigned char *pResonse = RecvBuf;

								//decode 
								//DecodeTpkt(pResonse);
								//DecodeIso(pResonse);
								DecodeS7Comm(pResonse);
							}
						}
					}
					else
					{	
						CloseSocket(nSocketFd);
						return -1;
					}

				}
				return 0;
			}
		}
	}
	CloseSocket(nSocketFd);
	return 0;
}

int S7commSanner(const unsigned char *pServerIp, const unsigned int nServerPort, unsigned char *pOutSannerResult, const unsigned int nOutLen)
{

	unsigned short int SrcTsap[2] = {0x100,0x200};
	unsigned short int DstTsap[3] = {0x102,0x200,0x201};

	int nSrc = 0;
	int nDst = 0;
	
	memset(pOutSannerResult, 0, nOutLen);
	memset(gTmpFingerprint, 0, sizeof(gTmpFingerprint));
	for ( ; nSrc < 2; nSrc++ )
	{
		for ( ; nDst < 3; nDst++ )
		{
			if ( 0 != Encode( pServerIp, nServerPort, SrcTsap[nSrc], DstTsap[nDst] ) )
			{
				return -1;
			}
		}
	}
	
	//
	snprintf(pOutSannerResult, nOutLen - 1, "%s", gTmpFingerprint);

	return 0;
}

/*
int main()
{

	unsigned char SrcTsap[2] = {0x100,0x200};
	unsigned char DstTsap[3] = {0x102,0x200,0x201};

	int nSrc = 0;
	int nDst = 0;
	for ( ; nSrc < 2; nSrc++ )
	{
		for ( ; nDst < 3; nDst++ )
		{
			if ( !Encode )
			{
				return 0;
			}
		}
	}
}
*/