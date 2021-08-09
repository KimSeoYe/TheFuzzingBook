#include <stdio.h>

#include "../include/fuzzer.h"

char *
fuzz ()
{
    return "" ;
}

typedef void (* runners_p) (char *, ret_t *) ;

ret_t 
fuzzer_run (runners_p runner)
{
    ret_t ret ;
    runner(fuzz(), &ret) ;
    
    return ret ;
}

void
fuzzer_runs (int trials, ret_t * outcomes)
{
    for (int i = 0; i < trials; i++) {
        outcomes[i] = fuzzer_run(print_runner_run) ;
    }
}

