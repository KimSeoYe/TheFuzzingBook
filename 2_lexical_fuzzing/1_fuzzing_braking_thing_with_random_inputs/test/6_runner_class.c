#include <stdio.h>

#include "../include/runner.h"

void
print_runner_test ()
{
    ret_t ret ;
    print_runner_run("Some input", &ret) ;

    printf("inp: %s\n", ret.inp) ;
    printf("out: %d\n", ret.out) ;  // 2 for UNRESOLVED
}

int
main ()
{
    print_runner_test() ;
}