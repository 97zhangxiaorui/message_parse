#include "json.h"
#include "const.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_LEN 2048
#define SELECT_LINE_NUMBER 10000

//extern int StringDivide(char *input_string, char *seperate_flag, char *out_seperated_string[], int seperate_num);

int print_preallocated(cJSON *root)
{
    /* declarations */
    char *out = NULL;
    char *buf = NULL;
    char *buf_fail = NULL;
    size_t len = 0;
    size_t len_fail = 0;

    /* formatted print */
    out = cJSON_Print(root);

    /* create buffer to succeed */
    /* the extra 5 bytes are because of inaccuracies when reserving memory */
    len = strlen(out) + 5;
    buf = (char*)malloc(len);
    if (buf == NULL)
    {
		printf("Failed to allocate memory.\n");
        exit(1);
    }

    /* create buffer to fail */
    len_fail = strlen(out);
    buf_fail = (char*)malloc(len_fail);
    if (buf_fail == NULL)
    {
		printf("Failed to allocate memory.\n");
        exit(1);
    }

    /* Print to buffer */
    if (!cJSON_PrintPreallocated(root, buf, (int)len, 1)) {
		printf("cJSON_PrintPreallocated failed!\n");
        if (strcmp(out, buf) != 0) {
			printf("cJSON_PrintPreallocated not the same as cJSON_Print!\n");
			printf("cJSON_Print result:\n%s\n", out);
			printf("cJSON_PrintPreallocated result:\n%s\n", buf);
        }
        free(out);
        free(buf_fail);
        free(buf);
        return -1;
    }

    /* success */
    printf("%s\n", buf);
    //snprintf(out_buffer, buffer_len - 1, "%s", buf);
    //printf("%s\n", out_buffer);
    //printf("len: %d\n", buffer_len);

    /* force it to fail */
    if (cJSON_PrintPreallocated(root, buf_fail, (int)len_fail, 1)) {
		printf("cJSON_PrintPreallocated failed to show error with insufficient memory!\n");
		printf("cJSON_Print result:\n%s\n", out);
		printf("cJSON_PrintPreallocated result:\n%s\n", buf_fail);
        free(out);
        free(buf_fail);
        free(buf);
        return -1;
    }

    free(out);
    free(buf_fail);
    free(buf);
    return 0;
}


/* Create a bunch of objects as demonstration. */
//int print_preallocated(cJSON *root, char *out_buffer, const int buffer_len)
int json_2_string(cJSON *root, char *out_buffer, const int buffer_len)
{
    /* declarations */
    char *out = NULL;
    char *buf = NULL;
    char *buf_fail = NULL;
    size_t len = 0;
    size_t len_fail = 0;

    /* formatted print */
    out = cJSON_Print(root);

    /* create buffer to succeed */
    /* the extra 5 bytes are because of inaccuracies when reserving memory */
    len = strlen(out) + 5;
    buf = (char*)malloc(len);
    if (buf == NULL)
    {
		printf("Failed to allocate memory.\n");
        exit(1);
    }

    /* create buffer to fail */
    len_fail = strlen(out);
    buf_fail = (char*)malloc(len_fail);
    if (buf_fail == NULL)
    {
		printf("Failed to allocate memory.\n");
        exit(1);
    }

    /* Print to buffer */
    if (!cJSON_PrintPreallocated(root, buf, (int)len, 1)) {
		printf("cJSON_PrintPreallocated failed!\n");
        if (strcmp(out, buf) != 0) {
			printf("cJSON_PrintPreallocated not the same as cJSON_Print!\n");
			printf("cJSON_Print result:\n%s\n", out);
			printf("cJSON_PrintPreallocated result:\n%s\n", buf);
        }
        free(out);
        free(buf_fail);
        free(buf);
        return -1;
    }

    /* success */
    snprintf(out_buffer, buffer_len - 1, "%s", buf);

    /* force it to fail */
    if (cJSON_PrintPreallocated(root, buf_fail, (int)len_fail, 1)) {
		printf("cJSON_PrintPreallocated failed to show error with insufficient memory!\n");
		printf("cJSON_Print result:\n%s\n", out);
		printf("cJSON_PrintPreallocated result:\n%s\n", buf_fail);
        free(out);
        free(buf_fail);
        free(buf);
        return -1;
    }

    free(out);
    free(buf_fail);
    free(buf);
    return 0;
}

//json_data = {"a":"b", "c":"e", "d":"z"}
char *get_value_by_key( char *json_data, char *key, char *out_buffer, int buffer_len )
{
	cJSON *json = NULL, *item = NULL;
	//char out_buffer[BUFFER_LEN] = { 0 },
	char *value = NULL;

	//json_data 转成json
	json = cJSON_Parse(json_data);
	if ( NULL == json )
	{
		printf("parse string to json failed, please check string \n");
		return NULL;
	}

	if ( NULL == strstr(key,"/") )
	{
		get_value_from_json_by_key(json, key, out_buffer, buffer_len);
	}
	else
	{
		//get value by path(key) <data/key>
		char *key_array[JSON_PATH_DEEP] = { NULL };
		int key_number = 0, i = 0;

		item = json;
		key_number = StringDivide(key, "/", key_array, JSON_PATH_DEEP);
		for ( ; i < key_number - 1; i++ ) //find to next last
		{
			item = cJSON_GetObjectItem(item, key_array[i]);
		}
		value = cJSON_GetStringValue(item);
		memcpy(out_buffer, value, strlen(value));
		//get_value_from_json_by_key(item, key_array[key_number - 1], out_buffer);

	}

	cJSON_Delete(json);
	return out_buffer;
}

char * get_value_from_json_by_key(cJSON *json, char *key, char *out_buffer, int buffer_len)
{
	cJSON *item = NULL;
	char *value = NULL;

	item = cJSON_GetObjectItem(json, key);
	if ( NULL != item )
	{
		if ( cJSON_IsArray(item) ) //array type = [ ["a","b","c"],["a","b","c"] ] and ["a","b"]
		{
			int i = 0;
			for ( ; i < cJSON_GetArraySize(item); i++ )
			{
				cJSON *tmp = cJSON_GetArrayItem(item, i);
				if ( NULL != tmp )
				{
					if( cJSON_IsArray(tmp) )
					{
						int j = 0;
						for ( ; j < cJSON_GetArraySize(tmp); j++ )
						{
							int data_len = strlen(out_buffer);
							cJSON *child_tmp = cJSON_GetArrayItem(tmp, j);
							if ( cJSON_IsArray(child_tmp) )
							{
								//
								char buf[2048] = { 0 };
								json_array_2_string(child_tmp, buf);
								snprintf(out_buffer + data_len, buffer_len - data_len, "%s,", buf);
							}
							else
							{
								value = NULL;
								value = cJSON_GetStringValue(child_tmp);		
								snprintf(out_buffer + data_len, buffer_len - data_len, "%s,", value);								
							}
						}

						out_buffer[strlen(out_buffer) - 1] = '\n';
					}
					else if (cJSON_IsString(tmp))
					{
						int data_len = strlen(out_buffer);
						snprintf(out_buffer + data_len, buffer_len - data_len, "%s,", cJSON_GetStringValue(tmp));
					}
					else
					{
						printf("unkown json object type, please check json \n");
					}
				}
			}
			out_buffer[strlen(out_buffer) - 1] = '\0';
		}
		else if ( cJSON_IsString(item) )
		{
			value = cJSON_GetStringValue(item);
			memcpy(out_buffer, value, strlen(value));
		}
		else
		{
			printf("parase json only support string and array values \n");
		}
	}
	else
	{
		//printf("get value by key[%s] is null\n", key);
		//cJSON_Delete(json);
		return NULL;
	}

	//cJSON_Delete(json);
	return out_buffer;
}

/*
*key = { aa,bb,cc }
*value = { aa,bb,cc\naa,bb,cc\naa,bb,cc }
*/
cJSON *encode_data_json( cJSON *data, char *key, char *value, int line_count )
{
	const char *value_line[SELECT_LINE_NUMBER] = { NULL };
	const char *keys_fields[SELECT_LINE_NUMBER] = { NULL };
	int line_num = 0, i = 0, key_fields_num = 0, value_fields_num = 0;
	char tmp[16] = { 0 };

	//add key 
	//test key is null must
	key_fields_num = StringDivide(key, ",", (char**)keys_fields, SELECT_LINE_NUMBER);
	//cJSON_AddItemToObject(data, "key", cJSON_CreateStringArray(keys_fields, key_fields_num));
	cJSON_AddItemToObject(data, "key", cJSON_CreateStringArray(keys_fields, key_fields_num));
	//cJSON_AddItemToObject(key, "key", cJSON_CreateStringArray(pkey, 4));
	cJSON *vlaues_array = cJSON_CreateArray();
	line_num = StringDivide(value, "\n", (char**)value_line, SELECT_LINE_NUMBER);
	for ( i = 0; i < line_num; i++ )
	{
		cJSON *line_array = cJSON_CreateArray();
		const char *value_fields[SELECT_LINE_NUMBER] = { NULL };
		value_fields_num = StringDivide((char*)value_line[i], ",", (char**)value_fields, SELECT_LINE_NUMBER);
		//cJSON_AddItemToArray(vlaues_array, cJSON_CreateStringArray(value_fields, value_fields_num));

		int j = 0;
		for (j=0; j < value_fields_num; j++)
		{
			string_2_json_array(line_array,(char*)value_fields[j], "|");
		}

		cJSON_AddItemToArray(vlaues_array, line_array);
	}
	cJSON_AddItemToObject(data, "value", vlaues_array);

	//
	if ( -1 == line_count )
		line_count = 0;
	snprintf(tmp, sizeof(tmp) - 1, "%d", line_count);
	cJSON_AddStringToObject(data, "line_count", tmp);
	return data;
}

int get_keyvalue_from_json(cJSON *object, char *out_key, char *out_value)
{
	int key_len = 0, value_len = 0;
	
	//cJSON_Print(object);
	if ( NULL != object && cJSON_IsObject(object) )
	{
		int i = 0;
        for (; i < cJSON_GetArraySize(object); i++)
        {
                cJSON *p = cJSON_GetArrayItem(object, i);
                key_len = strlen(out_key);
                value_len = strlen(out_value);

                sprintf(out_key + key_len, "%s,", p->string);
                sprintf(out_value + value_len, "%s,", p->valuestring);
        }
        out_key[strlen(out_key) - 1] = '\0';
        out_value[strlen(out_value) - 1] = '\0';
        return 0;
	}
	return -1;
}

//"aa,bb,cc,aa|bb,dd"["aa","bb","cc",["aa","bb"],"dd"]
int string_2_json_array(cJSON *json, char *string, char *flag)
{
	char *string_array[64] = { 0 };
	int number = 0, i = 0;
	cJSON *child = NULL;
	number = StringDivide(string, flag, string_array, 64);
	if ( number > 1 )
	{
		cJSON *child = cJSON_CreateArray();
		for (; i < number; i ++)
		{
			if (strlen(string_array[i]) < 1) continue;//处理空的情况，也就是"aa|,"的情况
			cJSON_AddItemToArray(child, cJSON_CreateString(string_array[i]));
		} 
		//cJSON_AddItemToArray(json, cJSON_CreateStringArray(string_array,number))
		cJSON_AddItemToArray(json, child);
		return 0;
	}
	cJSON_AddItemToArray(json, cJSON_CreateString(string));
	return 0;
}

//parase array to string |example:["a","b","c"]="a|b|c" |
char *json_array_2_string( cJSON *json, char *out_buffer)
{
	if ( cJSON_IsArray(json) )
	{
		int i = 0, length = 0;
		for (; i < cJSON_GetArraySize(json); i++ )
		{
			cJSON *child = cJSON_GetArrayItem(json, i);
			char *tmp = cJSON_GetStringValue(child);
			sprintf(out_buffer + length,"%s|", tmp);
			length = length + 1 + strlen(tmp);
		}

		out_buffer[length - 1] = '\0';
		return out_buffer;
	}
	sprintf(out_buffer,"%s", cJSON_GetStringValue(json));
	return out_buffer;
}

int key_value_2_json_object(cJSON *json_obj, char *key, char *value)
{
	char *key_array[16] = { NULL }, *vlaues_array[ 16 ] = { NULL };
	int number = 0, i = 0;

	StringDivide(key, ",", key_array, 16);
	number = StringDivide(value, ",", vlaues_array, 16);
	for ( i=0; i < number; i++ )
	{
		cJSON_AddStringToObject(json_obj, (const char*)key_array[i], vlaues_array[i]);
	}

	return 0;
}