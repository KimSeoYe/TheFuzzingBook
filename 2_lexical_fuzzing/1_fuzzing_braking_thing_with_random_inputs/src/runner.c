#include <stdio.h>
#include <string.h>

#include "../include/runner.h"

void
runner_run (char * inp, ret_t * ret)
{
    strcpy(ret->inp, inp) ;
    ret->out = UNRESOLVED ;
    return ;
}

void
print_runner_run (char * inp, ret_t * ret)
{
    printf("%s\n", inp) ;
    runner_run(inp, ret) ;
    return ;
}
