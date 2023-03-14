#ifndef __JSON_H__
#define __JSON_H__

#include "cJSON.h"

//#include <stdio.h>
//#include <string.h>

#define JSON_PATH_DEEP	12

//int print_preallocated(cJSON *root);
char *get_value_by_key( char *json_data, char *key, char *out_buffer, int buffer_len);
cJSON *encode_data_json( cJSON *data, char *key, char *value , int line_count);
cJSON *encode_response_json(char *data_type, char *result_code, char *keys, char *values, int line_count);
char *json_array_2_string( cJSON *json, char *out_buffer);
int string_2_json_array(cJSON *json, char *string, char *flag);
int key_value_2_json_object(cJSON *json_obj, char *key, char *value);
char * get_value_from_json_by_key(cJSON *json, char *key, char *out_buffer, int buffer_len);
int json_2_string(cJSON *root, char *out_buffer, const int buffer_len);

#endif