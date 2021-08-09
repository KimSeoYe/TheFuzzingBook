#include <stdio.h>
#include <stdlib.h>

#include "../include/random_fuzzer.h"

void
random_fuzzer_init (fuzarg_t * dst, int min_length, int max_length, int char_start, int char_range)
{
    // -1 >> default    // Q.

    if (min_length == -1) dst->min_length = MIN_LEN ;
    else dst->min_length = min_length ;

    if (max_length == -1) dst->max_length = MAX_LEN ;
    else dst->max_length = max_length ;

    if (char_start == -1) dst->char_start = CHAR_START ;
    else dst->char_start = char_start ;

    if (char_range == -1) dst->char_range = CHAR_RANGE ;
    else dst->char_range = char_range ;
}

char *
random_fuzzer_fuzz (fuzarg_t args)
{
    int string_length = rand() % (args.max_length - args.min_length + 1) + args.min_length ;
    
    char * out = (char *) malloc(sizeof(char) * string_length + 1) ;
    for (int i = 0; i < string_length; i++) {
        out[i] = rand() % args.char_range + args.char_start ;
    }
    out[string_length] = 0x0 ;

    return out ;
}

ret_t 
random_fuzzer_run (runners_p runner, fuzarg_t args)
{
    ret_t ret ;
    runner(random_fuzzer_fuzz(args), &ret) ;
    
    return ret ;
}