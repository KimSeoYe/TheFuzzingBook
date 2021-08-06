#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "../include/fuzzer.h"
#include "../include/heartbeats.h"

// #define DEBUG

void
manual_test ()
{
    char result[512] ;
    execute_heartbeats("potato", 6, result) ;
    execute_heartbeats("bird", 4, result) ;
    execute_heartbeats("hat", 500, result) ;
}

void
random_test_with_fuzzer ()
{
    for (int i = 0; i < 10; i++) {
        char * fuzzer_data = fuzzer(MAX_LEN, CHAR_START, CHAR_RANGE) ;

        char result[512] = "" ;
        execute_heartbeats(fuzzer_data, rand() % 500 + 1, result) ;
        free(fuzzer_data) ;

        assert(strstr(result, uninitialized_memory_marker) == NULL) ;
        assert(strstr(result, "secret") == NULL) ;
        /**
         * Cannot detect a BUG case 
         * if the length of fuzzer_data is bigger then the randomly generated length (rand() % 500 + 1),
         * the result does not have both "deadbeef" and "secret".
         * however, it is okay now because it is for testing the information leaks from sender...
        */
    }
}

int
main ()
{
    srand(time(NULL)) ;
    manual_test() ;
    random_test_with_fuzzer() ;

    return 0 ;
}