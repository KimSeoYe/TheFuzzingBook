#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "../include/fuzzer.h"

#define BUF_MAX 2048
// #define DEBUG

const char * uninitialized_memory_marker = "deadbeef" ;

char *
heartbeats (char * reply, int length, char * memory, char * s) 
{
    strncpy(memory, reply, strlen(reply)) ;
#ifdef DEBUG
    printf("MEMORY: %s\n", memory) ;
#endif

    strncpy(s, memory, length) ;
    s[length] = 0x0 ;

    return s ;    
}

void
make_secrets_sentence (char * secrets)
{
    char ** fuzzer_data = (char **) malloc(sizeof(char *) * 3) ;
    for (int i = 0; i < 3; i++) {
        fuzzer_data[i] = fuzzer(100, CHAR_START, CHAR_RANGE) ;
    }
    sprintf(secrets, "<space-for-reply>%s<secret-certificate>%s<secret-key>%s<other-secrets>", fuzzer_data[0], fuzzer_data[1], fuzzer_data[2]) ;
    
    int marker_len = strlen(uninitialized_memory_marker) ;
    int secrets_len = strlen(secrets) ;
    while (secrets_len + marker_len + 1 < BUF_MAX) {
        strcat(secrets, uninitialized_memory_marker) ;
        secrets_len += marker_len ;
    }

    for (int i = 0; i < 3; i++) {
        free(fuzzer_data[i]) ;
    }
    free(fuzzer_data) ;
}

void
execute_heartbeats (char * reply, int length, char * result)
{
    char secrets[BUF_MAX] ;
    make_secrets_sentence(secrets) ;

    heartbeats(reply, length, secrets, result) ;

    printf("REPLY: %s\n", result) ;
    return ;
}

void
manual_test ()
{
    char result[512] ;
    execute_heartbeats("potato", 6, result) ;
    execute_heartbeats("bird", 4, result) ;
    execute_heartbeats("hat", 500, result) ;
}

void
random_test_with_fuzzer ()
{
    for (int i = 0; i < 10; i++) {
        char * fuzzer_data = fuzzer(MAX_LEN, CHAR_START, CHAR_RANGE) ;

        char result[512] = "" ;
        execute_heartbeats(fuzzer_data, rand() % 500 + 1, result) ;
        free(fuzzer_data) ;

        assert(strstr(result, uninitialized_memory_marker) == NULL) ;
        assert(strstr(result, "secret") == NULL) ;
        /**
         * Cannot detect a BUG case 
         * if the length of fuzzer_data is bigger then the randomly generated length (rand() % 500 + 1),
         * the result does not have both "deadbeef" and "secret".
         * however, it is okay now because it is for testing the information leaks from sender...
        */
    }
}

int
main ()
{
    srand(time(NULL)) ;
    manual_test() ;
    random_test_with_fuzzer() ;

    return 0 ;
}