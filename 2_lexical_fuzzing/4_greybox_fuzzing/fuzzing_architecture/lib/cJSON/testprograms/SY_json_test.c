#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// #include <cjson/cJSON.h>
#include "./cJSON.h"

#define BUF_SIZE 1024

void
simple_parse_and_print (char * input)
{
    cJSON * parsed_input = cJSON_Parse(input) ;
    if (parsed_input == 0x0) {
        perror("simple_parse_and_print: cJSON_Parse failed") ;
        return ;
    }

    printf("\n=========================================\n") ;
    printf("PARSE: FORMATTED\n") ;
    printf("=========================================\n") ;
    char * printed = cJSON_Print(parsed_input) ;
    printf("%s\n", printed) ;
    free(printed) ;

    printf("\n=========================================\n") ;
    printf("PARSE: UNFORMATTED\n") ;
    printf("=========================================\n") ;
    printed = cJSON_PrintUnformatted(parsed_input) ;
    printf("%s\n", printed) ;
    free(printed) ;

    cJSON_Delete(parsed_input) ;
}

void
parse_and_print_with_lenght (char * input, int input_len)
{
    cJSON * parsed_input = cJSON_ParseWithLength(input, input_len) ;
    if (parsed_input == 0x0) {
        perror("parse_and_print_with_lenght: cJSON_ParseWithLength failed") ;
        return ;
    }

    printf("\n=========================================\n") ;
    printf("PARSE_W_LEN: FORMATTED\n") ;
    printf("=========================================\n") ;
    char * printed = cJSON_PrintBuffered(parsed_input, input_len, 1) ;   // trun on fmt
    printf("%s\n", printed) ;
    free(printed) ;

    printf("\n=========================================\n") ;
    printf("PARSE_W_LEN: UNFORMATTED\n") ;
    printf("=========================================\n") ;
    printed = cJSON_PrintBuffered(parsed_input, input_len, 0) ;   // turn off fmt
    printf("%s\n", printed) ;
    free(printed) ;

    cJSON_Delete(parsed_input) ;
}

void
parse_and_print_with_opt (char * input)
{
    cJSON * parsed_input_w_null_terminated = cJSON_ParseWithOpts(input, 0x0, 1) ; 
    if (parsed_input_w_null_terminated == 0x0) {
        perror("parse_and_print_with_opt: cJSON_ParseWithOpts failed") ;
        return ;
    }

    printf("\n=========================================\n") ;
    printf("PARSE_W_NULL_TERM: FORMATTED\n") ;
    printf("=========================================\n") ;
    char * printed = cJSON_Print(parsed_input_w_null_terminated) ;
    printf("%s\n", printed) ;
    free(printed) ;

    printf("\n=========================================\n") ;
    printf("PARSE_W_NULL_TERM: UNFORMATTED\n") ;
    printf("=========================================\n") ;
    printed = cJSON_PrintUnformatted(parsed_input_w_null_terminated) ;
    printf("%s\n", printed) ;
    free(printed) ;

    cJSON_Delete(parsed_input_w_null_terminated) ;
}

void
parse_and_print_with_length_opt (char * input, int input_len)
{
    cJSON * parsed_input_w_null_terminated = cJSON_ParseWithLengthOpts(input, input_len, 0x0, 1) ;
    if (parsed_input_w_null_terminated == 0x0) {
        perror("parse_and_print_with_length_opt: cJSON_ParseWithLengthOpts failed") ;
        return ;
    } 

    printf("\n=========================================\n") ;
    printf("PARSE_LEN_W_NULL_TERM: FORMATTED\n") ;
    printf("=========================================\n") ;
    char * printed = cJSON_PrintBuffered(parsed_input_w_null_terminated, input_len, 1) ;
    printf("%s\n", printed) ;
    free(printed) ;

    printf("\n=========================================\n") ;
    printf("PARSE_LEN_W_NULL_TERM: UNFORMATTED\n") ;
    printf("=========================================\n") ;
    printed = cJSON_PrintBuffered(parsed_input_w_null_terminated, input_len, 0) ;
    printf("%s\n", printed) ;
    free(printed) ;

    cJSON_Delete(parsed_input_w_null_terminated) ;
}

void
parse_and_print_preallocated (char * input, int input_len) 
{
    cJSON * parsed_input = cJSON_ParseWithLength(input, input_len) ;
    if (parsed_input == 0x0) {
        perror("parse_and_print_preallocated: cJSON_ParseWithLength failed") ;
        return ;
    }

    char * buf = (char *) malloc(sizeof(char) * (input_len + 5)) ;

    printf("\n=========================================\n") ;
    printf("PRINT_PREALLOCATED: FORMATTED\n") ;
    printf("=========================================\n") ;
    int printed = cJSON_PrintPreallocated(parsed_input, buf, input_len + 5, 1) ;
    if (printed) {
        printf("%d\n%s", printed, buf) ;
        free(buf) ;
    }
    else {
        perror("parse_and_print_preallocated: cJSON_PrintPreallocated failed") ;
    }
    

    buf = (char *) malloc(sizeof(char) * (input_len + 5)) ;

    printf("\n=========================================\n") ;
    printf("PRINT_PREALLOCATED: UNFORMATTED\n") ;
    printf("=========================================\n") ;
    printed = cJSON_PrintPreallocated(parsed_input, buf, input_len + 5, 0) ;
    if (printed) {
        printf("%d\n%s", printed, buf) ;
        free(buf) ;
    }
    else {
        perror("parse_and_print_preallocated: cJSON_PrintPreallocated failed") ;
    }

    cJSON_Delete(parsed_input) ;  
}

void
duplicate_and_compare (char * input) 
{
    cJSON * parsed_input = cJSON_Parse(input) ;
    if (parsed_input == 0x0) {
        perror("parse_and_print_preallocated: cJSON_ParseWithLength failed") ;
        return ;
    }

    cJSON * duplicated = cJSON_Duplicate(parsed_input, 1) ;
    if (parsed_input == 0x0) {
        perror("parse_and_print_preallocated: cJSON_Duplicate failed") ;
        return ;
    }
    cJSON_bool is_same = cJSON_Compare(parsed_input, duplicated, 1);
    if (is_same) printf("duplicate success\n") ;
    else printf("duplicate failed\n") ;

    cJSON_Delete(parsed_input) ;
    cJSON_Delete(duplicated) ;
}

void
parse_and_minify (char * input, int input_len)
{
    cJSON * parsed_input = cJSON_Parse(input) ;
    if (parsed_input == 0x0) {
        perror("parse_and_print_preallocated: cJSON_ParseWithLength failed") ;
        return ;
    }

    char * copied = (char *) malloc(sizeof(char) * (input_len + 1)) ;
    memcpy(copied, input, input_len + 1) ;

    cJSON_Minify(copied) ;
    if (copied != 0x0) {
        printf("\n=========================================\n") ;
        printf("MINIFIED\n") ;
        printf("=========================================\n") ;
        printf("%s\n", copied) ;
    }

    free(copied) ;
    cJSON_Delete(parsed_input) ;
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

    simple_parse_and_print(input) ;
    parse_and_print_with_lenght(input, input_len) ;
    parse_and_print_with_opt(input) ;
    parse_and_print_with_length_opt(input, input_len) ;
    parse_and_print_preallocated(input, input_len) ;

    duplicate_and_compare(input) ;
    parse_and_minify(input, input_len) ;

    return 0 ;
}