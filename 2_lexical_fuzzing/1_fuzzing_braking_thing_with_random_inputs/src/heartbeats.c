#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "../include/simplefuzzer.h"
#include "../include/heartbeats.h"

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

