#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "cJSON.h"

int Parse_Print_Minify(const uint8_t* data, size_t size); /* required by C89 */

int Parse_Print_Minify(const uint8_t* data, size_t size)
{
	cJSON *json;
	unsigned char *copied;
	char *printed_json = NULL;

	if(data[size] != '\0'){
		printf("Buf should include NULL in the tail\n");  
		return 0;
	}
	
	// 1. Parsing
	json = cJSON_ParseWithOpts((const char*)data, NULL, 1);
	
	if(json == NULL) {
		printf("json is NULL \n");
		return 0;
	}

	// 2. Printing
	printed_json = cJSON_PrintBuffered(json, 1, 1);
	printf("[DEUBG] printed_json(buffered): %s\n", printed_json);

	if(printed_json != NULL) free(printed_json);

	// 3. Minifying
	copied = (unsigned char*)malloc(size);
	if(copied == NULL) return 3;
	memcpy(copied, data, size);
	cJSON_Minify((char*)copied);

	printf("[DEBUG] Minifyed: %s\n", copied);
	free(copied);

	// 4. Get

	printf("[DEBUT] Number_value: %lf\n", cJSON_GetNumberValue(json));
	printf("[DEBUT] String_value: %s\n", cJSON_GetStringValue(json));
		
	// 5. Add

	cJSON_Delete(json);
	return 0;
}

int main(int argc, char **argv)
{
	char* buf = (char*)malloc(sizeof(char) * 8196);
	size_t siz_buf = 0;
	memset(buf, 0, 8196);

	while((buf[siz_buf] = getc(stdin)) != EOF){
		siz_buf++;
	}
	buf[siz_buf] = '\0';

	int return_code;

	return_code = Parse_Print_Minify((uint8_t*)buf, (size_t)siz_buf);
	
	free(buf);
	return return_code;
}

