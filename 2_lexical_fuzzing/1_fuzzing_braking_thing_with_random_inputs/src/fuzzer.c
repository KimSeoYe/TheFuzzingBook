#include <stdio.h>

#include "../include/fuzzer.h"

char *
fuzz ()
{
    return "" ;
}

typedef void (* runners_p) (ret_t *, char *) ;

ret_t 
fuzzer_run (runners_p runner)
{
    ret_t ret ;
    runner(&ret, fuzz()) ;
    
    return ret ;
}

void
fuzzer_runs (ret_t * outcomes, int trials)
{
    for (int i = 0; i < trials; i++) {
        outcomes[i] = fuzzer_run(print_runner_run) ;
    }
}

