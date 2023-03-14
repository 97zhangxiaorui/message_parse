#include "mac.h"
#include "../comm/const.h"

#include <stdlib.h>
#include <stdio.h>

/*
*说明：此文件hash仅用于此项目，并且仅适用于处理oui文件中的数据，如有其他用途，请根据业务修改
*此文件中所有函数，仅用于通过MAC地址OUI文件中寻找生产厂商
*/


/*
*功能：  mac地址生成hash值
*参数：  mac地址，mac分隔符
*返回值：mac地址hash值
*/
unsigned int mac_hash(char *mac, char separator)
{
	int mac_len = 0, hash = 0, i = 0;
	char *p = NULL;

    mac_len = strlen(mac);
	if ( !mac || mac_len < 6 ){
		return 0;
	}

    for ( ; i < mac_len; i++ ){
        if ( mac[i] == separator ){
            continue;
        }
        hash+= mac[i];
    }

	return hash;
}


/*
*功能：		添加一个MAC HASH节点 
*参数：		HASH节点信息（mac，vendor）
*返回值：    0 成功， -1 失败
*/
int add_mac_hash_node(char *mac , char *vendor, char separator )
{
	MacVendor *temp;
    int i = 0, hash = 0;

    temp=(MacVendor*)malloc(sizeof(MacVendor));
    if(temp == NULL){
		printf("Malloc error!\n");
        exit(1);
    }

    memset(temp, 0, sizeof(MacVendor));
    snprintf(temp->mac, sizeof(temp->mac) - 1, "%s", mac);
    snprintf(temp->vendor, sizeof(temp->vendor) - 1, "%s", vendor);
    temp->next=NULL;
    hash = mac_hash( mac, separator);
    if(MacHash[hash] == NULL){
        MacHash[hash]=temp;
    }
    else{
        temp->next= MacHash[hash];
        MacHash[hash]=temp;
    }

}

/*
*功能：		查找mac地址HASH节点值
*参数：		mac地址，分割符
*返回值：    NULL， hash节点
*/
MacVendor *hash_search(char *mac, char separator)
{
	unsigned int hash = 0;
    MacVendor *p = NULL;

    hash=mac_hash(mac,separator);

    if( MacHash[hash] == NULL){
        printf("No record of this MAC!\n");
        return NULL;
    }
    else{
        p=MacHash[hash];
        while(p != NULL){
            if(strcmp(p->mac,mac) == 0)
                return p;
            else
                p=p->next;
        }
    }
    return NULL;
}

int init_hash()
{
    int i = 0, result = 0, mac_len = strlen("AA-AA-AA");
    char *p = NULL, *cmd = NULL;
    unsigned int hash = 0;
 
    //cmd = "cat ../conf/oui.txt | grep hex | awk -F \"(\" '{print $1,$2}' |  awk -F \"hex)\" '{print $1,$2}' | awk -F \"\t\" '{print $1,$3}'";
    cmd = "cat ../conf/oui.txt | grep hex | awk -F \"(\" '{print $1,$2}' | awk -F \")\" '{print $1,$2}' | awk -F \"\t\" '{print $1,$3}' | awk -F \"hex\" '{print $1,$2}'";
    FILE *fp = popen(cmd, "r");
    if (!fp){
        printf("打开文件句柄失败\n");
        return 0;
    }

    char buffer[1024] = { 0 };
    while( fgets(buffer,sizeof(buffer) - 1,fp) ){
        char mac[12] = { 0 };
        char vendor[128] = { 0 };
        char *tmp = NULL;
        //MANAGE_LOG_INFO("buf:%s \n", buffer);
        memcpy(mac, buffer, mac_len);
        RTrim(mac);

        if ( strlen(mac) != mac_len ){
            //printf("Tmp:[%s] [%d-%d]\n", buffer, mac_len, strlen(mac));
            continue;
        }

        mac[ mac_len ] = '\0';
        tmp = buffer + mac_len;
        RTrim(tmp);
        LTrim(tmp);
        //CleanHT(tmp);
        memcpy(vendor, tmp, strlen(tmp));
        vendor[strlen(vendor) - 1] = '\0';

        //新增节点
        add_mac_hash_node(mac, vendor, '-');
    }
     
    pclose(fp);
    return 0;
}

int print_mac_hash()
{
    int i = 0, count = 0;
    for ( ; i < MAC_HASH_LEN; i++ ){
        if( NULL != MacHash[i]){
            MacVendor *tmp = NULL;
            tmp = MacHash[i];
            while(tmp){
                printf("HASH[%u] MAC[%s] Vendor[%s] \n", i, tmp->mac, tmp->vendor);
                tmp = tmp->next;
            }
        }
        else{
            count++;
        }
    }
    printf("未命中数量：%d \n", count);
}