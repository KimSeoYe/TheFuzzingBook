#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../include/fuzzer.h"

#define BUF_MAX 2048
// #define DEBUG

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
    sprintf(secrets, "<space-for-reply>%s<secret-certificate>%s<secret-key>%s<other-secrets>", fuzzer(100, CHAR_START, CHAR_RANGE), fuzzer(100, CHAR_START, CHAR_RANGE), fuzzer(100, CHAR_START, CHAR_RANGE)) ;

    char * uninitialized_memory_marker = "deadbeef" ;
    int marker_len = strlen(uninitialized_memory_marker) ;
    while (strlen(secrets) + marker_len + 1 < BUF_MAX) {    // TODO. strlen(secrets)
        strcat(secrets, uninitialized_memory_marker) ;
    }
}

void
execute_heartbeats (char * reply, int length)
{
    char secrets[BUF_MAX] ;
    make_secrets_sentence(secrets) ;

    char * result = (char *) malloc(sizeof(char) * (length + 1)) ;
    heartbeats(reply, length, secrets, result) ;
    printf("REPLY: %s\n", result) ;
    free(result) ;
}

void
information_leaks ()
{
    execute_heartbeats("potato", 6) ;
    execute_heartbeats("bird", 4) ;
    execute_heartbeats("hat", 500) ;
}

int
main ()
{
    srand(time(NULL)) ;
    information_leaks() ;

    return 0 ;
}