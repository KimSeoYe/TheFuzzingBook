#ifndef RANDOM_FUZZER
#define RANDOM_FUZZER

#include "./runner.h"

#define MIN_LEN 10
#define MAX_LEN 100
#define CHAR_START 32
#define CHAR_RANGE 32

typedef void (* runners_p) (char *, ret_t *) ;

typedef struct fuzarg {
    int min_length ;
    int max_length ;
    int char_start ;
    int char_range ;
} fuzarg_t ;

void random_fuzzer_init (fuzarg_t * dst, int min_length, int max_length, int char_start, int char_range) ;

char * random_fuzzer_fuzz (fuzarg_t args) ;

ret_t random_fuzzer_run (runners_p runner, fuzarg_t args) ;

#endif