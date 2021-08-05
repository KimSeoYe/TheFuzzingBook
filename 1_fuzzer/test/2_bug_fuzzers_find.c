#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#include "../include/fuzzer.h"

const int trials = 100 ;

// 1. Buffer Overflow

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

// 2. Missing Error Checks

void
handler (int sig)
{
    if (sig == SIGALRM) {
        perror("timeout") ;
        exit(1) ;
    }
}

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
    struct itimerval t ;
    signal(SIGALRM, handler) ;

    t.it_value.tv_sec = 2 ;
    t.it_value.tv_usec = 0 ;
    t.it_interval = t.it_value ;

    setitimer(ITIMER_REAL, &t, 0x0) ;

    for (int i = 0; i < trials; i++) {
        char * s = fuzzer(MAX_LEN, CHAR_START, CHAR_RANGE) ;
        hang_if_no_space(s) ;
    }
}

// 3. Rogue Numbers



int
main ()
{
    buffer_overflows() ;
    missing_error_checks() ;
}