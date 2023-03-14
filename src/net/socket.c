#include "socket.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

int Socket(const unsigned int nSocketType)
{
    if ( nSocketType == SOCK_STREAM || SOCK_DGRAM == nSocketType )
    {
        int nSocketFd = socket(AF_INET, SOCK_STREAM, 0);
        if( nSocketFd < 0 )
        {
            printf("socket create errror: %s, errno: %d \n", strerror(errno), errno);
        }
        return nSocketFd;
    }

    return -1;
}

int Connect(const unsigned int nSocketFd, const unsigned char *pServerIp, const unsigned int nServerPort)
{
    int nResult = 0;
    struct sockaddr_in ServerAddr;
    memset(&ServerAddr, 0, sizeof(struct sockaddr_in));

    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(nServerPort);
    //ServerAddr.sin_port = nServerPort;
    //printf("Port: %d\n", nServerPort);
    inet_pton(AF_INET, pServerIp, &ServerAddr.sin_addr);
    nResult = connect(nSocketFd, (struct sockaddr*)&ServerAddr, sizeof(struct sockaddr));
    if( nResult < 0 ) {
        printf("connect error %s errno: %d\n", strerror(errno), errno);
    }

    return nResult;
}

int  ReciveData(const unsigned int nSokcetFd, unsigned char *pOutBuf,  const int nLong)
{
    int nRecvLen = 0;
    char Buf[1024] = { 0 };

    nRecvLen = recv(nSokcetFd, pOutBuf, nLong, MSG_CMSG_CLOEXEC);
    if ( nRecvLen <= 0 )
    {
        printf("recv error %s errno: %d\n", strerror(errno), errno);
    }

    return nRecvLen;
}

int SendData(const unsigned char *pData, const unsigned int nDataLen, const unsigned int nSocketFd)
{
    int nSendNumber = 0;

    nSendNumber = send(nSocketFd, pData, nDataLen, 0);
    if ( nSendNumber < 0 )
    {
        printf("send message error: %s errno : %d", strerror(errno), errno);
    }
    return nSendNumber;
}

void CloseSocket(const unsigned int nSocketFd)
{
    close(nSocketFd);
}

int InitSocket(const unsigned int nSocketType, const unsigned char *pServerIp, const unsigned int nServerPort)
{
    int nSocketFd = 0;

    nSocketFd = Socket(nSocketType);
    if ( nSocketFd > 0 )
    {   
        int nConnectStat = 0;
	
	//printf("DD:%d\n", nServerPort);
        nConnectStat = Connect(nSocketFd, pServerIp, nServerPort);
        if ( nConnectStat < 0 )
        {
            return -1;
        }
    }
    return nSocketFd;
}
 
