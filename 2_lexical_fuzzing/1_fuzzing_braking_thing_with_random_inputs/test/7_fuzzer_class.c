#include <stdio.h>
#include <stdlib.h>

#include "../include/fuzzer.h"
#include "../include/random_fuzzer.h"

void
fuzzer_test ()
{
    ret_t outcomes[10] ;
    fuzzer_runs(10, outcomes) ;
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

int
main ()
{
    // fuzzer_test() ;
    rand_fuzzer_test() ;

    return 0 ;
}