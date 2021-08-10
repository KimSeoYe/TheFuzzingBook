#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../include/random_fuzzer.h"

void
random_fuzzer_init (fuzarg_t * dst, int min_length, int max_length, int char_start, int char_range)
{
    dst->min_length = min_length ;
    dst->max_length = max_length ;
    dst->char_start = char_start ;
    dst->char_range = char_range ;
}

char *
random_fuzzer_fuzz (fuzarg_t args)
{
    int string_length = rand() % (args.max_length - args.min_length + 1) + args.min_length ;
    
    char * out = (char *) malloc(sizeof(char) * string_length + 1) ;    // as an argument ?
    for (int i = 0; i < string_length; i++) {
        out[i] = rand() % args.char_range + args.char_start ;
    }
    out[string_length] = 0x0 ;

    return out ;
}

void 
random_fuzzer_run (char * program)
{
    fuzarg_t args ;
    random_fuzzer_init(&args, 20, 20, CHAR_START, CHAR_RANGE) ;
    char * inp = random_fuzzer_fuzz(args) ;

    pr_ret_t ret ;
    program_runner_run(&ret, program, inp) ;
    free(inp) ;

    // TODO. to get a stdout and stderr separately?
    printf("(CompletedProcess(args='%s', returncode=%d, stdout|stderr='%s')\n", program, ret.outcome, ret.result) ;
}

void 
random_fuzzer_runs (char * program, int trials)
{
    fuzarg_t args ;
    random_fuzzer_init(&args, 20, 20, CHAR_START, CHAR_RANGE) ;
    for (int i = 0; i < trials; i++) {
        char * inp = random_fuzzer_fuzz(args) ;

        pr_ret_t ret ;
        program_runner_run(&ret, program, inp) ;
        free(inp) ;

        printf("(CompletedProcess(args='%s', returncode=%d, stdout|stderr='%s')\n", program, ret.outcome, ret.result) ;
    }
}