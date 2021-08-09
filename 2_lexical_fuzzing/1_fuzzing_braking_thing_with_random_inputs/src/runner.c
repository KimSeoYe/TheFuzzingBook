#include <stdio.h>
#include <string.h>

#include "../include/runner.h"

void
runner_run (ret_t * ret, char * inp)
{
    strcpy(ret->inp, inp) ;
    ret->out = UNRESOLVED ;
    return ;
}

void
print_runner_run (ret_t * ret, char * inp)
{
    printf("%s\n", inp) ;
    runner_run(ret, inp) ;
    return ;
}
