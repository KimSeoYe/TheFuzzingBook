#include <stdio.h>
#include <stdlib.h>

#include "../include/runner.h"
#include "../include/program_runner.h"

void
print_runner_test ()
{
    ret_t ret ;
    print_runner_run("Some input", &ret) ;

    printf("inp: %s\n", ret.inp) ;
    printf("out: %d\n", ret.out) ;  // 2 for UNRESOLVED
}

void
program_runner_test ()
{
    pr_ret_t ret ;
    program_runner_run("cat", "hello world\n", &ret) ;
    printf("result:\n%s", ret.result) ;
    printf("out: %d\n", ret.outcome) ;
    free(ret.result) ;

    printf("-----------------------------\n") ;

    program_runner_run("bc", "2 + 2\n", &ret) ;
    printf("result:\n%s", ret.result) ;
    printf("out: %d\n", ret.outcome) ;
    free(ret.result) ;
}


int
main ()
{
    print_runner_test() ;
    printf("-----------------------------\n") ;
    program_runner_test() ;
    printf("-----------------------------\n") ;
}