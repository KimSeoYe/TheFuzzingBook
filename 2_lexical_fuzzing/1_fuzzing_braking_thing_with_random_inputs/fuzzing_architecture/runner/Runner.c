#include <stdio.h>
#include <stdlib.h>

#include "Runner.h"

void
run (Runner * self, char * input, int input_size)
{
    if (input_size > 0) {
        self->input = (char *) malloc(sizeof(char) * input_size) ;
        if (self->input == 0x0) {
            perror("Could not allocate memory") ;
            return ;
        }
        
        self->input_size = input_size ;

        // Q. how to check validity ?
        for (int i = 0; i < input_size; i++) {
            self->input[i] = input[i] ;
        }

        self->outcome = UNRESOLVED ;
    }
    else if (input_size == 0) {
        self->input_size = 0 ;
        self->input = 0x0 ;
        self->outcome = UNRESOLVED ;
    }
    else {
        perror("Negative input size") ;
    }

    return ;
}

void
free_input (Runner self) 
{
    if (self.input != 0x0) free(self.input) ;
    return ;
}

void
RunnerInit (Runner * self)
{
    self->run = run ;
    self->free_input = free_input ;
    return ;
}