#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

// #include <cjson/cJSON.h>
#include "cJSON.h"

 
void test_type(cJSON * obj){
    printf("obj name : %s\n", obj->string );

    if( cJSON_IsInvalid(obj) ) printf("It's invalid\n") ;    
    if( cJSON_IsFalse(obj) ) printf("It's false\n") ;
    if( cJSON_IsTrue(obj) ) printf("It's true\n") ;
    if( cJSON_IsBool(obj) ) printf("It's boolean\n") ;
    if( cJSON_IsNull(obj) ) printf("It's null\n") ;
    if( cJSON_IsNumber(obj) ) printf("It's number\n") ;
    if( cJSON_IsString(obj) ) printf("It's string\n") ;
    if( cJSON_IsArray(obj) ) printf("It's array\n") ;
    if( cJSON_IsObject(obj) ) printf("It's object\n") ;
    if( cJSON_IsRaw(obj) ) printf("It's raw\n") ;

    return ;
}

// TEST PRINT
void test_print(cJSON * item, int size){
    // FORMATTED 
    char *formatted = cJSON_Print(item);
    if(formatted){
        printf("\nFORMATTED :\n%s\n", formatted);
        free(formatted);
    }

    formatted = cJSON_PrintBuffered(item, size, 1);
    if(formatted){
        printf("\nBUFFERED(formatted) :\n%s\n", formatted);
    }
    // UNFORNATTED
    cJSON_Minify(formatted);
    if(formatted){
        printf("\nMINIFIED :\n%s\n", formatted);
        free(formatted);
    }

    char *unformatted = cJSON_PrintUnformatted(item);
    if(unformatted){
        printf("\nUNFORMATTED :\n%s\n", unformatted);
        free(unformatted);
    }

    unformatted = cJSON_PrintBuffered(item, size, 0);
    if(unformatted){
        printf("\nBUFFERED(unformatted) :\n%s\n", unformatted);
        free(unformatted);
    }
}

int testcase1(const uint8_t* data, size_t size)
{
    cJSON *root = NULL;
    
    // TEST Parse 
    root = cJSON_Parse(data);
    if (root == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return 0;
    }
 
    test_type(root);

    test_print(root, size);

    cJSON_Delete(root);

    return 0;
}

int
main(int argc, char **argv)
{
    char * buf = (char*) calloc(1, sizeof(char) * 8192);
    size_t size_buf = 0;

    while((buf[size_buf] = getc(stdin)) != EOF){
        size_buf++;
    }
    buf[size_buf] = 0x0;

    testcase1((uint8_t *)buf, size_buf);
    free(buf);
    
    return 0;
}
