#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../include/fuzzer.h"
#include "../include/random_fuzzer.h"
#include "../include/program_runner.h"

void
fuzzer_test ()
{
    ret_t outcomes[10] ;
    fuzzer_runs(outcomes, 10) ;
    for (int i = 0; i < 10; i++) {
        printf("[%d] %d\n", i, outcomes[i].out) ; 
    }
}

void
rand_fuzzer_test ()
{
    fuzarg_t args ;
    random_fuzzer_init(&args, 20, 20, CHAR_START, CHAR_RANGE) ;
    for (int i = 0; i < 10; i++) {
        char * result = random_fuzzer_fuzz(args) ;
        printf("%s\n", result) ;
        free(result) ;
    }    
}

void
rand_fuzzer_test_with_prog_runner ()
{
    fuzarg_t args ;
    random_fuzzer_init(&args, 20, 20, CHAR_START, CHAR_RANGE) ;
    for (int i = 0; i < 10; i++) {
        char * inp = random_fuzzer_fuzz(args) ;

        pr_ret_t ret ;
        program_runner_run(&ret, "cat", inp) ;

        assert(strcmp(ret.result, inp) == 0) ;
        assert(ret.outcome == PASS) ;

        free(inp) ;
    }
}

int
main ()
{
    // fuzzer_test() ;
    rand_fuzzer_test() ;
    rand_fuzzer_test_with_prog_runner() ;
    random_fuzzer_run("cat") ;
    random_fuzzer_runs("cat", 10) ;

    return 0 ;
}