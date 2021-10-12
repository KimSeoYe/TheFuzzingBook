#include "cJSON.h" 
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 

#define BUFF_SIZE 4096

void copyItems(cJSON * json, cJSON * new_json) { 
    int cJSON_array_size ; 
    cJSON * item = json; 

    if((cJSON_array_size = cJSON_GetArraySize(item)) != 0) {
        cJSON * array = cJSON_CreateArray() ; 

        cJSON * ci ; 
        for (int i = 0 ; i < cJSON_array_size ; i++) {
            cJSON * new_obj = cJSON_CreateObject() ; 
            ci = cJSON_GetArrayItem(item, i) ;

            if (cJSON_HasObjectItem(item, ci->string)) { 
                int string_length = strlen(ci->string);  
                new_obj->string = (char *)cJSON_malloc(sizeof(char) * string_length);  
                
                memcpy(new_obj->string, ci->string, string_length) ; new_obj->string[string_length] = 0x0 ;   
                cJSON_Minify(new_obj->string); 
                cJSON * obj_item = cJSON_GetObjectItemCaseSensitive(item, ci->string); 
                
                printf("[item]:%s ", new_obj->string) ; 
                if (obj_item != NULL) {
                    if (cJSON_IsString(obj_item) && obj_item->valuestring != NULL) {
                        int valuestring_length = strlen(obj_item->valuestring);  
                        new_obj->valuestring = (char *)cJSON_malloc(sizeof(char) * valuestring_length) ;
                        
                        memcpy(new_obj->valuestring, ci->valuestring, valuestring_length) ; new_obj->valuestring[valuestring_length] = 0x0 ;   
                        printf("[ValueString]: %s, \n", new_obj->valuestring) ; 
                    } else if(cJSON_IsNumber(obj_item)) { 
                        new_obj->valueint = ci->valueint ; 
                        new_obj->valuedouble = ci->valuedouble ; 

                        printf("[value]: %f ", new_obj->valuedouble) ; 
                    }else {
                        printf("\n");
                    }
                    if ( !cJSON_AddItemToArray(array, new_obj) ) {
                        printf("Cannot add the new_obj to array!\n"); 
                    } 
                }
            }
            copyItems(ci, new_json); 
        }
        if ( !cJSON_AddItemToArray(new_json, array) ) { 
            printf("Cannot add the array to new_json!\n"); 
        } 
    }  
}

int main(void) {
    char * text = (char *)malloc(sizeof(char) * BUFF_SIZE) ; 
    char c ; 
    int idx = 0, n = 1 ; 
    cJSON *json ; 
    
    // INPUT 
    while( (c = getchar()) != EOF) {
        if (idx >= BUFF_SIZE) { 
            text = (char *)realloc(text, sizeof(char) * BUFF_SIZE * (++n)) ;
        }
        text[idx++] = c; 
    }
    text[idx] = '\0'; 

    // Parse 
    json = cJSON_Parse(text) ;
    if (!json) {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
    } else { 
        printf("[JSON file structure]\n"); 
        printf("%s\n", cJSON_Print(json));
        
        char * copy_text = NULL ; 
        cJSON * created_cJSON = cJSON_CreateStringReference(text); 

        if (!created_cJSON) {
            printf("Cannot create the cJSON: [%s]\n",cJSON_GetErrorPtr());
        } else {    
            copy_text = (char *)cJSON_malloc(sizeof(char) * idx) ;
            // If the content of entity is correctly copied to copy_text, true
            if (cJSON_PrintPreallocated(json, copy_text, idx , cJSON_True)) { 
                printf("%s\n", copy_text); 

                if( !strcmp(text, copy_text)) {
                    printf("The content of text is same with copy_text!\n"); 
                }else {
                    printf("The content of text is not same with copy_text!\n"); 
                }
            }
            if ( copy_text != NULL ) free(copy_text); 

            // copy the content of object
            copyItems(json, created_cJSON) ;
            printf("<<< COPY ITEMS >>>\n");
            printf("%s\n", cJSON_PrintUnformatted(created_cJSON));
            
            cJSON_free(created_cJSON); 
        }

        // cJSON Duplicate
        cJSON * json_dup = cJSON_Duplicate(json, cJSON_True); 

        // Compare original cJSON to duplicated cJSON. 
        if (cJSON_Compare(json, json_dup, cJSON_False)) {
            printf("Copy is correct!\n"); 
        }
        cJSON_free(json_dup) ;

        /* Minify a strings, remove blank characters(such as ' ', '\t', '\r', '\n') from strings.*/
        cJSON_Minify(text); 
        json_dup = cJSON_Parse(text) ; 
        if (!json_dup) {
            printf("Error before: [%s]\n",cJSON_GetErrorPtr());
        }else {
            printf("%s\n", cJSON_PrintUnformatted(json_dup)); ;
        }

        if (cJSON_Compare(json, json_dup, cJSON_False)) {
            printf("Structure of minified text is same with the structure of original text!\n"); 
        }else{
            printf("Structure of minified text is not same with the structure of original text!\n");
        }
        
        /* Delete a cJSON entity and all subentities. */
        cJSON_Delete(json_dup);
        cJSON_Delete(json); 
    }
    
    free(text); 

    return 0;
}


