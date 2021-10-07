#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <cjson/cJSON.h>

#define BUF_SIZE 4096
#define NAME_SIZE 128

int Type_check(cJSON* json,int level,cJSON* arr){

    int size = cJSON_GetArraySize(json);

    for(int i = 0; i < level; i++){
        printf(" ");
    }
        if(cJSON_IsNumber(json))
        {   
            cJSON* dup = cJSON_Duplicate(json,1);
            if( cJSON_AddItemToArray(arr,dup) == 0){
                perror("add number failed\n");
            }

            printf("(Number)");
            printf("%f\n",cJSON_GetNumberValue(json));
        }else if(cJSON_IsString(json))
        {   
            cJSON* dup = cJSON_Duplicate(json,1);
            if( cJSON_AddItemToArray(arr,dup) == 0){
                perror("add string failed\n");
            }
          
            printf("(String)");
            printf("%s\n",cJSON_GetStringValue(json));
            
        }else if(cJSON_IsArray(json))
        {   
            printf("(Array)\n");
            printf("%s\n",json->string);

        }else if(cJSON_IsObject(json))
        {
            printf("(Object)\n");
        }
    cJSON* child;

    if(size > 0){
        child = json->child;
    }

    for(int i = 0; i < size; i++){
        Type_check(child,level+1,arr);
        child = child->next;
    }
}

void create_array_check(cJSON* json){


    cJSON* arr = cJSON_CreateArray();

    if(arr == NULL){
        perror("create string array failed\n");
        exit(1);
    }

    Type_check(json,0,arr);

    cJSON* child;

    int size = cJSON_GetArraySize(arr);
    printf("before size: %d\n",size);
    if(size > 0){
        child = arr->child;
        for(int i = 0; i < size; i++){
            if(child->type == 8) //number
            {   
                printf("Delete : %0.1f\n",child->valuedouble);
                child = child->next;
                cJSON_DetachItemViaPointer(arr,child->prev);
            }else{
                child = child->next;
            }
        }
    }
    printf("=============\n");
    printf("PRINT ARRAY\n");
    
    size = cJSON_GetArraySize(arr);
    printf("after size: %d\n",size);
    child = arr->child;

    for(int i = 0; i < size; i++)
    {
        child = arr->child;
        printf("Type: : ");
        switch (child->type)
        {
        case 0:
            printf("invalid\n");
            break;
        case 1:
            printf("False\n");
            break;
        case 2:
            printf("True\n");
            break;
        case 4:
            printf("NULL\n");
            break;
        case 8:
            perror("Number type should be delete\n");
            exit(1);
            break;
        case 16:
            printf("String\n");
            break;
        case 32:
            printf("Array\n");
            break;
        case 64:
            printf("Object\n");
            break;
        default:
            printf("None\n");
            break;
        }
    }
    cJSON_Print(arr);

    cJSON_Delete(arr);
}

int Parse_check(const char* data,size_t data_length,cJSON* json_parse)
{   
    int error_flag = 0;

    if(data == NULL || data_length  == 0)
    {
        perror("data is NULL\n");
        return 1;
    }

    /*parsing same input data*/
    ///////////////////////////////////////
    cJSON *json_parse2,*json_parse3;

    json_parse2 = cJSON_ParseWithLength(data,data_length);

    if(json_parse == NULL)
    {
        perror("parse1 failed\n");
        return 1;
    }

    if(json_parse2 == NULL)
    {
        perror("parse2 failed\n");
        return 1;
    }
    
    if(cJSON_Compare(json_parse,json_parse2,1) != 1)
    {
        perror("json1 and json2 are not equal!(sensitive)\n");
        error_flag = 1;
        goto fail;
    }

    if(cJSON_Compare(json_parse,json_parse2,0) != 1)
    {
        printf("json1 and json2 are not equal!(insensitive)\n");
        error_flag = 1;
        goto fail;
    }

    /* Duplicate */
    //////////////////////////////////////
    json_parse3 = cJSON_Duplicate(json_parse,1);

    if(json_parse3 == NULL)
    {
        perror("duplicate failed\n");
        return 1;
    }

    if(cJSON_Compare(json_parse,json_parse3,1) != 1)
    {
        perror("json1 and json2 are not equal!(sensitive && duplicate)\n");
        error_flag = 1;
        goto fail;
    }

    if(cJSON_Compare(json_parse,json_parse3,0) != 1)
    {
        perror("json1 and json2 are not equal!(insensitive && duplicate)\n");
        error_flag = 1;
        goto fail;
    }

fail:
    if(json_parse2 != NULL)
    {
        cJSON_Delete(json_parse2);
    }

    if(json_parse3 != NULL)
    {
        cJSON_Delete(json_parse3);
    }

    if(error_flag == 1)
    {
        return 1;
    }

    return 0;    
}


int main()
{

    char* buf = (char*)malloc(sizeof(char)*BUF_SIZE);
    
    if(buf == NULL)
    {
        perror("buffer malloc failed\n");
        exit(1);
    }
    
    long size_buf = 0;
    char c;

    while((c = getchar()) != EOF)
    {
        if(size_buf + 1 % BUF_SIZE == 0)
        {
            buf = realloc(buf,sizeof(char)*(((size_buf + 1)/ BUF_SIZE) + 1)*BUF_SIZE);

            if(buf == NULL)
            {
                perror("buffer realloc failed\n");
                exit(1);
            }
        }
        buf[size_buf] = c;
        size_buf++;
    }
    buf[size_buf] = '\0';

    cJSON* json = cJSON_Parse(buf);

    if(json == NULL){
        perror("json parse failed\n");
        goto err;
    }

    if(Parse_check(buf,size_buf,json)){
        goto err;
    }

    create_array_check(json);

    cJSON_Delete(json);

err:
    free(buf);

    return 0;
}
