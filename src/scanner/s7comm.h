#ifndef __S7COMM_H_
#define __S7COMM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TPKT_PROTOCAL	3
#define TPKT_LENGTH		4
#define ISO_LENGTH		3

/*
*function : clear Tpkt from pInMsg (change source pointer)
*pInMsg   : network packet 
*return   : 0 or -1
*/
int DecodeTpkt(char *pInMsg);

/*
*funtion     : encode tpkt packet 
*pOutMsg     : the buffer of save encode message
*nBufLen     : the buffer length
*nDataLen    : tpkt data length
*return      : Tpkt length or -1
*/
int EncodeTpkt(char *pOutMsg, int nBufLen, int nDataLen);

/*
*funtion : clear iso head from pInMsg (change source pointer)
*pInMsg  : network packet
*return  : 0 or -1
*/
int DecodeIso(char *pInMsg);

/*
*function : encode iso packet 
*pOutMsg  : the buffer of save encode message
*nBufLen  : the size of pOutMsg 
*return   : iso length or -1
*/
int EncodeIso(char *pOutMsg, int nBufLen);

/*
*function    : clear S7 head from pInMsg (change source pointer)
*pInMsg      : network packet
*return      : 0 or -1
*/
int DecodeS7Comm(char *pInMsg);

/*
*function : encode S7 protocol packet
*pParam   : S7comm param 
*pData    : S7comm content
*pOutMsg  : the buffer of save encode S7 message
*nBufLen  : the size of pOutMsg
*return   : s7comm length or -1
*/
int EncodeS7Comm(unsigned short SzlId, char *pOutMsg, int nBufLen);

/*
*function     : 
*pInMsg       :
*pOutSrcTsap  :
*pOutDstTsap  :
*pOutDesRef   :
*pOutSrcRef   :
*/
int DecodeCotp(char *pInMsg);

/*
* 
*
*/
int EncodeCotp(char *pOutMsg, int nBufLen, unsigned short nSrcTsap, unsigned short nDstTsap);


char* DeviceMsg(unsigned char *pMsg);

char* MsgInfo(char *pMsgBuf, int buffer_len, unsigned char *pInMsg, unsigned int MsgLen, unsigned int nStep, unsigned int nSzlId );

char* MsgSzlId0X11(char *pOutBuffer, int BufferLen, int nIndex, char *pInMsg);

char* MsgSzlId0X1c(char *pOutBuffer, int BufferLen, int nIndex, char *pInMsg);

#endif