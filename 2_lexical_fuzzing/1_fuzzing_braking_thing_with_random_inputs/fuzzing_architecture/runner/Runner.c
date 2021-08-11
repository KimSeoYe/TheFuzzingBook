#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Runner.h"

void
runner_run (result_t * result, char * input, int input_size)
{
    result->input = (char *) malloc(sizeof(char) * input_size) ;
    
    result->input_size = input_size ;

    // Q. how to check validity ?
    for (int i = 0; i < input_size; i++) {
        result->input[i] = input[i] ;
    }

    result->outcome = UNRESOLVED ;

    return ;
}

void
RunnerInit (Runner * runner)
{
    runner->run = runner_run ;
    return ;
}