#include <stdio.h>
#include <stdlib.h>

#include <cjson/cJSON.h>

#define BUF_SIZE 1024

#define DEBUG

/*
JSON STRUCTURE
{
    "name" : "<filename>",
    "line_number" : <int>,
    "current_version" : <double>,
    "version_logs" : [
        {
            "version": <double>,
            "editor" : "<editor name>"
        }, 
        {
            "version": <double>,
            "editor" : "<editor name>"
        }
    ]
}
*/

int 
get_code_info (const char * const code)
{
    const cJSON * name = 0x0 ;
    const cJSON * line_number = 0x0 ;
    const cJSON * current_version = 0x0 ;
    const cJSON * version_logs = 0x0 ;
    const cJSON * version_log = 0x0 ;

    cJSON * code_json = cJSON_Parse(code) ;
    if (code_json == 0x0) {
        const char * error_ptr = cJSON_GetErrorPtr() ;
        if (error_ptr != 0x0) {
            fprintf(stderr, "get_code_info: %s\n", error_ptr) ;
        }
        goto end ;
    }

    name = cJSON_GetObjectItemCaseSensitive(code_json, "name") ;
    if (name != 0x0 && cJSON_IsString(name) && name->valuestring != 0x0) {
        printf("NAME: %s\n", name->valuestring) ;
    }
    else {
        perror("get_code_info: name") ;
        goto end ;
    }

    line_number = cJSON_GetObjectItemCaseSensitive(code_json, "line_number") ;
    if (line_number != 0x0 && cJSON_IsNumber(line_number)) {
        printf("LINE NUMBER: %d\n", line_number->valueint) ;
    }
    else {
        perror("get_code_info: line_number") ;
        goto end ;
    }

    current_version = cJSON_GetObjectItemCaseSensitive(code_json, "current_version") ;
    if (current_version != 0x0 && cJSON_IsNumber(current_version)) {
        printf("CURRENT VERSION: %.2f\n", current_version->valuedouble) ;
    }
    else {
        perror("get_code_info: current_version") ;
        goto end ;
    }

    if ((version_logs = cJSON_GetObjectItemCaseSensitive(code_json, "version_logs")) != 0x0) {
        printf("VERSION LOGS:\n") ;
        cJSON_ArrayForEach(version_log, version_logs) {
            cJSON * version = cJSON_GetObjectItemCaseSensitive(version_log, "version") ;
            cJSON * editor = cJSON_GetObjectItemCaseSensitive(version_log, "editor") ;
            
            if (version != 0x0 && cJSON_IsNumber(version)) {
                printf("\t%.2f ", version->valuedouble) ;
            }
            else {
                perror("get_code_info: version_logs: version") ;
                goto end ;
            }

            if (editor != 0x0 && cJSON_IsString(editor) && editor->valuestring != 0x0) {
                printf("%s\n", editor->valuestring) ;
            }
            else {
                perror("get_code_info: version_logs: editor") ;
                goto end ;
            }
        }
    }
    else {
        perror("get_code_info: version_logs") ;
        goto end ;
    }


    return 1 ;

end:
    cJSON_Delete(code_json) ;
    return 0 ;
}

int
main (int argc, char * argv[])
{
    char * input = (char *) malloc(sizeof(char) * BUF_SIZE) ;
    char c ;
    int input_len ;
    for (input_len = 0; (c = getchar()) != EOF && c != 4; input_len++) {    // 3 : ctrl+D
        if ((input_len + 1) % BUF_SIZE == 0 && input_len != 0) {
            input = realloc(input, sizeof(char) * BUF_SIZE * (BUF_SIZE / (input_len + 1) + 1)) ;
        }
        input[input_len] = c ;
    }
    input[input_len] = 0x0 ;
    
#ifdef DEBUG
    printf("%s", input) ;
#endif

    get_code_info(input) ;    

    return 0 ;
}