#ifndef FUZZER
#define FUZZER

#include "../runner/ProgramRunner.h"

typedef struct Fuzzer {
    char * (* fuzz) (int *) ; // int * length
    void (* run) (void (* runner) (Runner *, char *, int)) ; // CHECK
    void (* runs) (void (* runner) (Runner *, char *, int), int) ;
} Fuzzer ;

#endif