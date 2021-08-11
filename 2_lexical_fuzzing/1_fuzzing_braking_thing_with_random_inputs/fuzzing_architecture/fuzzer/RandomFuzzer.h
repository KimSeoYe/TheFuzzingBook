#ifndef RAND_FUZZER
#define RAND_FUZZER

#include "Fuzzer.h"

typedef struct fuzarg {
    int min_length ;
    int max_length ;
    int char_start ;
    int char_range ;
} fuzarg_t ;

void fuzarg_init (fuzarg_t * fuzargs) ;
void RandomFuzzerInit (Fuzzer * fuzzer) ;

#endif