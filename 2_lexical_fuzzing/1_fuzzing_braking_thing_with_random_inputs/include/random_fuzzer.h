#ifndef RANDOM_FUZZER
#define RANDOM_FUZZER

#include "./runner.h"
#include "./program_runner.h"

#define MIN_LEN 10
#define MAX_LEN 100
#define CHAR_START 32
#define CHAR_RANGE 32

typedef void (* runners_p) (ret_t *, char *) ;

typedef struct fuzarg {
    int min_length ;
    int max_length ;
    int char_start ;
    int char_range ;
} fuzarg_t ;

void random_fuzzer_init (fuzarg_t * dst, int min_length, int max_length, int char_start, int char_range) ;

char * random_fuzzer_fuzz (fuzarg_t args) ;

void random_fuzzer_run (char * program) ;

void random_fuzzer_runs (char * program, int trials) ;

#endif