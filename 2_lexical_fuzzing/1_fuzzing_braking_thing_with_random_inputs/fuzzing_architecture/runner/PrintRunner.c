#include <stdio.h>
#include <stdlib.h>

#include "PrintRunner.h"

void
print_runner_run (Runner * self, char * input, int input_size)
{
    if (input_size > 0) {
        self->input = (char *) malloc(sizeof(char) * input_size) ;
        if (self->input == 0x0) {
            perror("Could not allocate memory") ;
            return ;
        }
        
        self->input_size = input_size ;

        for (int i = 0; i < input_size; i++) {
            self->input[i] = input[i] ;
            putc(input[i], stdout) ;
        }

        self->outcome = UNRESOLVED ;
    }
    else if (input_size == 0) {
        self->input_size = 0 ;
        self->input = 0x0 ;
        self->outcome = UNRESOLVED ;
    }
    else {
        perror("Input size must be non-negative") ;
    }

    return ;
}

void
PrintRunnerInit (Runner * self)
{
    self->run = print_runner_run ;
    self->free_input = free_input ;
}