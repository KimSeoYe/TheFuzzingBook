#include <stdio.h>
#include <stdlib.h>

#include "Runner.h"

void
run (Runner * self, char * input, int input_size)
{
    self->result.input = (char *) malloc(sizeof(char) * input_size) ;
    
    self->result.input_size = input_size ;

    // Q. how to check validity ?
    for (int i = 0; i < input_size; i++) {
        self->result.input[i] = input[i] ;
    }

    self->result.outcome = UNRESOLVED ;

    return ;
}

void
free_input (Runner runner) 
{
    if (runner.result.input != 0x0) free(runner.result.input) ;
    return ;
}

void
RunnerInit (Runner * runner)
{
    runner->run = run ;
    runner->free_input = free_input ;
    return ;
}