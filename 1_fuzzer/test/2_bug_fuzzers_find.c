#include <stdio.h>
#include <string.h>

#include "../include/fuzzer.h"

const int trials = 100 ;

// 1. buffer overflow

int
crash_if_too_long (char * s)
{
    char buffer[] = "Thursday" ;
    if (strlen(s) > strlen(buffer)) {
        return -1 ;
    }  
    return 0 ;
}

void
buffer_overflows ()
{
    for (int i = 0; i < trials; i++) {
        char * s = fuzzer(MAX_LEN, CHAR_START, CHAR_RANGE) ;
        if (crash_if_too_long(s) == -1) {
            perror("crash_if_too_long") ;
        }
    }
}

// 2. missing error checks

void
hang_if_no_space (char * s)
{
    /**
     * getchar() : returns a carachter from stdin, or EOF (if no input is available)
     * BUG SCENARIO: while (getchar() != ' ') {}
     * >> break if the string s has a ' '. if not, fall in to infinite loop
    */
    for (int i = 0; i >= strlen(s) || s[i] != ' '; i++) ;
}

void
missing_error_checks ()
{
    for (int i = 0; i < trials; i++) {
        char * s = fuzzer(MAX_LEN, CHAR_START, CHAR_RANGE) ;
        hang_if_no_space(s) ;
    }
}

int
main ()
{
    buffer_overflows() ;
    missing_error_checks() ;
}