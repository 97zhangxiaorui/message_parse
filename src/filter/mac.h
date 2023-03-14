#ifndef __MAC_H__
#define __MAC_H__

/*
*主要功能通过mac获取设备厂商信息，根据MAC地址获取设备类型
*/

#define MAC_HASH_LEN 1024

#pragma pack(push)
#pragma pack(1)
typedef struct _MacVendor_{
	char mac[24];
	char vendor[128];
	struct mac_vendor *next;
}MacVendor;
#pragma pack(pop)

MacVendor *MacHash[MAC_HASH_LEN];
unsigned int mac_hash(char *mac, char separator);
int add_mac_hash_node(char *mac, char *vendor, char separator );
MacVendor *hash_search(char *mac, char separator);
int init_hash();
int print_mac_hash();

#endif