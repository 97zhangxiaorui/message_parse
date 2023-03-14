#include "assets.h"
#include "../comm/hash.h"

ListObj* pGlobalAssetsHash[ASSETS_HASH_SIZE];

int CompareFingerprint(DeviceFingerprint *pSrcFinger, DeviceFingerprint *pDstFinger){
	//如果发现资产指纹中有任何一个属性更新，则判断指纹需要更新
	int nResult = 0;

	if ( strlen(pSrcFinger->Vendor) > 0 && !strcasestr( pSrcFinger->Vendor, pDstFinger->Vendor) ){
		nResult = 1;
	}
	if ( strlen(pSrcFinger->Type) > 0 && !strcasestr( pSrcFinger->Type, pDstFinger->Type) ){
		nResult = 1;
	}
	if ( strlen(pSrcFinger->ProductName) > 0 && strcasestr( pSrcFinger->ProductName, pDstFinger->ProductName) ){
		nResult = 1;
	}
	if ( strlen(pSrcFinger->SystemType) > 0 && strcasestr( pSrcFinger->SystemType, pDstFinger->SystemType) ){
		nResult = 1;
	}
	if ( strlen(pSrcFinger->SystemVersion) > 0 && strcasestr( pSrcFinger->SystemVersion, pDstFinger->SystemVersion) ){
		nResult = 1;
	}
	if ( strlen(pSrcFinger->HostName) > 0 && strcasestr( pSrcFinger->HostName, pDstFinger->HostName) ){
		nResult = 1;
	}

	return nResult;
}

void ClearFingerprint(DeviceFingerprint *pFinger){

	memset(pFinger, 0, sizeof(DeviceFingerprint));
	/*
	memset(pFinger->Ip, 0, sizeof(pFinger->Ip));
	memset(pFinger->Mac, 0, sizeof(pFinger->Mac));
	memset(pFinger->Vendor, 0, sizeof(pFinger->Vendor));
	memset(pFinger->Type, 0, sizeof(pFinger->Type));
	memset(pFinger->SystemType, 0, sizeof(pFinger->SystemType));
	memset(pFinger->SystemVersion, 0, sizeof(pFinger->SystemVersion));
	memset(pFinger->ProductName, 0, sizeof(pFinger->ProductName));
	memset(pFinger->Describe, 0, sizeof(pFinger->Describe));
	memset(pFinger->HostName, 0, sizeof(pFinger->HostName));
	*/
}

void UpdateFingerprint(DeviceFingerprint *pSrcFinger, DeviceFingerprint *pDstFinger){
	//ClearFingerprint();
	if ( strlen(pSrcFinger->Vendor) > 0 ){
		memset(pDstFinger->Vendor, 0, sizeof(pDstFinger->Vendor));
		memcpy(pDstFinger->Vendor, pSrcFinger->Vendor, strlen(pSrcFinger->Vendor) );
	}

	if ( strlen(pSrcFinger->Type) > 0 ){
		memset(pDstFinger->Type, 0, sizeof(pDstFinger->Type));
		memcpy(pDstFinger->Type, pSrcFinger->Type, strlen(pSrcFinger->Type) );
	}

	if ( strlen(pSrcFinger->SystemType) > 0 ){
		memset(pDstFinger->SystemType, 0, sizeof(pDstFinger->SystemType));
		memcpy(pDstFinger->SystemType, pSrcFinger->SystemType, strlen(pSrcFinger->SystemType) );
	}

	if ( strlen(pSrcFinger->SystemVersion) > 0 ){
		memset(pDstFinger->SystemVersion, 0, sizeof(pDstFinger->SystemVersion));
		memcpy(pDstFinger->SystemVersion, pSrcFinger->SystemVersion, strlen(pSrcFinger->SystemVersion) );
	}

	if ( strlen(pSrcFinger->ProductName) > 0 ){
		memset(pDstFinger->ProductName, 0, sizeof(pDstFinger->ProductName));
		memcpy(pDstFinger->ProductName, pSrcFinger->ProductName, strlen(pSrcFinger->ProductName) );
	}

	if ( strlen(pSrcFinger->Describe) > 0 ){
		memset(pDstFinger->Describe, 0, sizeof(pDstFinger->Describe));
		memcpy(pDstFinger->Describe, pSrcFinger->Describe, strlen(pSrcFinger->Describe) );
	}

	if ( strlen(pSrcFinger->HostName) > 0 ){
		memset(pDstFinger->HostName, 0, sizeof(pDstFinger->HostName));
		memcpy(pDstFinger->HostName, pSrcFinger->HostName, strlen(pSrcFinger->HostName) );
	}
}

void PrintFingerprint(DeviceFingerprint *pFingerprint){
	printf("{\"ip\":\"%s\",\"mac\":\"%s\",\"vendor\":\"%s\",\"device_type\":\"%s\",\"system_type\":\"%s\","\
		"\"version\":\"%s\",\"describe\":\"%s\",\"host_name\":\"%s\", \"device_name\":\"%s\"}", \
		pFingerprint->Ip, pFingerprint->Mac, pFingerprint->Vendor, pFingerprint->Type, pFingerprint->SystemType, \
		pFingerprint->SystemVersion, pFingerprint->Describe, pFingerprint->HostName, pFingerprint->ProductName);
}

void PrintHash()
{
	int i = 0;
	for ( ; i < ASSETS_HASH_SIZE; i++ )
	{
		ListPtr list = NULL, tmp_list = NULL;

		tmp_list = list = pGlobalAssetsHash[i];
		//tmp_list = global_tmp_ip_hash[i];
		while ( tmp_list )
		{
			hash_node *tmp_node = NULL;
			tmp_node = (hash_node*) tmp_list->pContent;
			if ( tmp_node )
			{
				if ( tmp_node->data )
				{
					char str_ip[32] = { 0 };
					DeviceFingerprint *pFinger = (DeviceFingerprint *)tmp_node->data;

					if (pFinger == NULL) continue;

					PrintFingerprint(pFinger);
				}
			}
			tmp_list = tmp_list->pNext;
		}
	}
}

char* UpdateAssets( AssetsFingerprint *pAassets, char *pOutFingerprint, int nOutLen){
	AssetsFingerprint *pTmpFinger = NULL;
	int nIsNew = 0;

	pTmpFinger = (AssetsFingerprint *)get_data_from_hash(pGlobalAssetsHash, pAassets->uIp, ASSETS_HASH_SIZE);
	if ( !pTmpFinger ){
		//添加新的资产到HASH
		add_hash( pGlobalAssetsHash, pAassets->uIp, (void *)pAassets, sizeof(AssetsFingerprint), ASSETS_HASH_SIZE);
		nIsNew = 1;
	}
	else{
		//更新资产信息，如果没有更新，则什么都不做，输出为空
		if (CompareFingerprint( &(pAassets->Fingerpint), &(pTmpFinger->Fingerpint)) ){
			UpdateFingerprint(&(pAassets->Fingerpint), &(pTmpFinger->Fingerpint));
			nIsNew = 1;
		}
	}

	if ( nIsNew ){
		pTmpFinger = NULL;
		//char TmpBuf[1024] = 0;

		pTmpFinger = (AssetsFingerprint *)get_data_from_hash(pGlobalAssetsHash, pAassets->uIp, ASSETS_HASH_SIZE);
		if ( !pTmpFinger ){
			return;
		}
		//snprintf(TmpBuf, sizeof(TmpBuf) -1, "{\"ip\":\"%s\",\"mac\":\"%s\",\"vendor\":\"%s\",\"device_type\":\"%s\",\"system_type\":\"%s\","\
											 "\"version\":\"%s\",\"describe\":\"%s\",\"host_name\":\"%s\", \"device_name\":\"%s\"}", \
											 pAassets->Fingerpint.Ip, pAassets->Fingerpint.Mac, pAassets->Fingerpint.Vendor, pAassets->Fingerpint.Type, pAassets->Fingerpint.SystemType, \
											 pAassets->Fingerpint.SystemVersion, pAassets->Fingerpint.Describe, pAassets->Fingerpint.HostName, pAassets->Fingerpint.ProductName);

		//printf("{\"ip\":\"%s\",\"mac\":\"%s\",\"vendor\":\"%s\",\"device_type\":\"%s\",\"system_type\":\"%s\","\
				"\"version\":\"%s\",\"describe\":\"%s\",\"host_name\":\"%s\", \"device_name\":\"%s\"}", \
				pAassets->Fingerpint.Ip, pAassets->Fingerpint.Mac, pAassets->Fingerpint.Vendor, pAassets->Fingerpint.Type, pAassets->Fingerpint.SystemType, \
				pAassets->Fingerpint.SystemVersion, pAassets->Fingerpint.Describe, pAassets->Fingerpint.HostName, pAassets->Fingerpint.ProductName);
	}
}