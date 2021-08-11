#include <stdio.h>
#include <stdlib.h>

#include "PrintRunner.h"

void
print_runner_run (Runner * self, char * input, int input_size)
{
    self->result.input = (char *) malloc(sizeof(char) * input_size) ;
    
    self->result.input_size = input_size ;

    for (int i = 0; i < input_size; i++) {
        self->result.input[i] = input[i] ;
        putchar(input[i]) ;
    }

    self->result.outcome = UNRESOLVED ;

    return ;
}

void
PrintRunnerInit (Runner * runner)
{
    runner->run = print_runner_run ;
    runner->free_input = free_input ;
}