#include <stdio.h>

#include "PrintRunner.h"

void
print_runner_run (result_t * result, char * input, int input_size)
{
    result->input = (char *) malloc(sizeof(char) * input_size) ;
    
    result->input_size = input_size ;

    for (int i = 0; i < input_size; i++) {
        result->input[i] = input[i] ;
        putchar(input[i]) ;
    }

    result->outcome = UNRESOLVED ;

    return ;
}

void
PrintRunnerInit (Runner * runner)
{
    runner->run = print_runner_run ;
}