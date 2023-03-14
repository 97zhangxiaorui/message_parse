#include "hash.h"

int init_hash_null(ListObj** hash, int hash_size)
{
	int i = 0;

	for(; i < hash_size; i++ ){
		hash[i] = NULL;
	}
}

u32 create_hash(u32 value, u32 hash_size )
{
	//return jhash_1words(value, VOICE_HASH_GOLDEN_INTERER) & (HASH_SIZE-1);
	return jhash_1words(value, VOICE_HASH_GOLDEN_INTERER) & (hash_size-1);
}

int add_hash( ListPtr *hash_name, u32 index_value, void *data, unsigned int data_len, unsigned int hash_size)
{
	u32 hash_key = create_hash(index_value, hash_size);
	ListPtr tmp_list = NULL;
	hash_node *node = NULL;
	//int data_len = sizeof(ip_policy);
	
	tmp_list = AddListNodes(&hash_name[hash_key], 1);

	//crete hash_node
	node = (hash_node*)malloc(sizeof(hash_node));
	memset(node, 0, sizeof(hash_node));
	node->key = index_value;

	//crete data
	node->data = malloc(data_len);
	memset(node->data, 0, data_len);
	memcpy(node->data, data, data_len);

	tmp_list->pContent = (void*)node;

	/*
	char str_ip[64] = { 0 };
	ip_policy *tmp_policy = NULL;//(ip_policy*)node->data;
	tmp_policy = (ip_policy*)data;
	*/
	//MANAGE_LOG_INFO("HASH: %u, %d, %d, %d, %u-%u \n", tmp_policy->ip, tmp_policy->regist, tmp_policy->device_type, tmp_policy->auth_cycle, tmp_policy->stop_ip_domain[0].begin_ip, tmp_policy->stop_ip_domain[0].end_ip);
	//MANAGE_LOG_INFO("HASH: %s, %d, %d, %d, %s-%s \n", int_ip_2_string(tmp_policy->ip, str_ip, sizeof(str_ip)), tmp_policy->regist, tmp_policy->device_type, tmp_policy->auth_cycle, int_ip_2_string(tmp_policy->stop_ip_domain[0].begin_ip, str_ip, sizeof(str_ip)), int_ip_2_string(tmp_policy->stop_ip_domain[0].end_ip, str_ip, sizeof(str_ip)));
	return 0;
}

void free_hash_data(hash_node *node)
{
	if ( node && node->data ){
		free(node->data);
	}
}

void free_hash_node(hash_node **node)
{
	if ( *node ){
		if ( *node ){
			free_hash_data(*node);
		}

		free(*node);
	}
	*node = NULL;
}

void *get_data_from_hash(ListPtr *hash_name, u32 index_value, unsigned int hash_size)
{
	u32 hash_key = create_hash(index_value, hash_size);
	ListPtr tmp_list = NULL;
	//MANAGE_LOG_INFO("----get_policy--[%u]---\n", hash_key);
	tmp_list = hash_name[hash_key];
	while(tmp_list)
	{
		hash_node *tmp_node = NULL;
		tmp_node = (hash_node *)tmp_list->pContent;
		if ( NULL == tmp_node ){
			return NULL;
		}
		//MANAGE_LOG_INFO(" commpare[%u-%u] \n", index_value, tmp_node->key);
		if ( index_value == tmp_node->key )
		{
			return tmp_node->data;
		}
		tmp_list = tmp_list->pNext;
	}
	return NULL;
}

void destroy(ListPtr *hash_name, int hash_size)
{
	int i = 0;
	for ( ; i < hash_size; i++ )
	{
		ListPtr tmp_list = NULL;

		tmp_list = hash_name[i];
		//tmp_list = list;
		while ( tmp_list )
		{
			hash_node *tmp_node = NULL;

			//free hash_node
			tmp_node = (hash_node*) tmp_list->pContent;
			if ( tmp_node )
			{
				/*
				//free hash data
				if ( tmp_node->data )
				{
					free(tmp_node->data);
				}
				free(tmp_node);
				*/
				free_hash_node(&tmp_node);
			}
			tmp_list->pContent = NULL;
			tmp_list = tmp_list->pNext;
		}

		//free list
		if ( tmp_list )
		{
			DestoryList(&tmp_list);
		}

		hash_name[i] = NULL;
	}
}