#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "../include/fuzzer.h"

// #define DEBUG

char *
fuzzer (int max_length, int char_start, int char_range)
{
    int string_length = rand() % (max_length + 1) ;
    char * out = (char *) malloc(sizeof(char) * string_length + 1) ;    // Q. Is it okay to use malloc() here?
    for (int i = 0; i < string_length; i++) {
        out[i] = rand() % char_range + char_start ;
    }
    out[string_length] = '\0' ;

#ifdef DEBUG
    printf("fuzzer: %s\n", out) ;
#endif

    return out ;
}
