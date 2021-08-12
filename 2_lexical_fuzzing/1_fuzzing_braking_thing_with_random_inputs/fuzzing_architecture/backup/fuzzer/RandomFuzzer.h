#ifndef RAND_FUZZER
#define RAND_FUZZER

#include "Fuzzer.h"

typedef struct fuzarg {
    int min_length ;
    int max_length ;
    int char_start ;
    int char_range ;
} fuzarg_t ;

typedef struct RandomFuzzer {
    char * (* fuzz) (int *) ; // int * length
    void (* run) (void (* runner) (ProgramRunner *, char *, int)) ; // CHECK
    void (* runs) (void (* runner) (ProgramRunner *, char *, int), int) ;
} RandomFuzzer ;

void fuzarg_init (fuzarg_t * fuzargs) ;
void RandomFuzzerInit (RandomFuzzer * fuzzer) ;

#endif