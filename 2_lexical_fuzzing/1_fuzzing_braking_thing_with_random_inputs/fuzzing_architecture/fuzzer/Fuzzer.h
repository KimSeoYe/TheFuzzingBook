#ifndef FUZZER
#define FUZZER

#include "../runner/ProgramRunner.h"

typedef struct Fuzzer {
    char * (* fuzz) (int *) ; // int * length
    void (* run) (void (* runner) (result_t *, char *, int)) ; // CHECK
    void (* runs) (void (* print_runner) (result_t *, char *, int), int) ;
} Fuzzer ;

#endif