#ifndef __SCOKET_H_
#define __SCOKET_H_

/*
*nSocketType : UDP/TCP
*return     : socket handle or -1
*/
int Socket(const unsigned int nSocketType);

/*
*nSokcetFd  : socket handle 
*pServerIp  : server ip address
*nServerPort: server listen port
*return    : SocketFd or -1
*/
int Connect(const unsigned int nSocketFd, const unsigned char *pServerIp, const unsigned int nServerPort);


/*
*nSokcetFd   : socket handle
*pOutBuf    : the buffer for save recive data
*nLong      : recive data longth
*return     : recive number or -1
*/
int  ReciveData(const unsigned int nSokcetFd, unsigned char *pOutBuf,  const int nLong);


/*
*pData     : send data
*nSokcetFd : socket handle
*nDataLen  : send data length
*return    : send data number or -1
*/
int SendData(const unsigned char *pData, const unsigned int nDataLen, const unsigned int nSocketFd);


/*
*nSocketType   : socket type
*pServerIp    : TCP/UDP server ip address
*nServerPort  : server listen port
*return       : sokcet handle or -1
*/
int InitSocket(const unsigned int nSocketType, const unsigned char *pServerIp, const unsigned int nServerPort );

/*
*nSocketFd : socket handle 
*return    : 
*/
void CloseSocket(const unsigned int nSocketFd);


#endif