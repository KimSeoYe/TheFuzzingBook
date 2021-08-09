#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#include "../include/simplefuzzer.h"

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
            return ;
        }
    }
}

/**
 * 2. Missing Error Checks
 * 
 * getchar() : returns a carachter from stdin, or EOF (if no input is available)
 * BUG SCENARIO: while (getchar() != ' ') {}
 * >> break if the string s has a ' '. if not, fall in to infinite loop
*/

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
        free(s) ;
    }
}

/**
 * 3. Rogue Numbers
 * 
 * char *read_input() {
 *   size_t size = read_buffer_size();
 *   char *buffer = (char *)malloc(size);
 *   // fill buffer
 *   return (buffer);
 * }
 * 
 * >> What if the size is less than the # of characters following?
 * >> What if the size is negative ?
*/

int
collapse_if_too_large (char * s)
{
    int s_num = atoi(s) ;
    if (s_num > 1000) return -1 ;
    return 0 ;
}

void
rogue_numbers ()
{
    char * long_number = fuzzer(10, '0', 10) ;
    printf("%s\n", long_number) ;

    if (collapse_if_too_large(long_number) == -1) {
        perror("collapse_if_too_large") ;
        return ;
    }

    free(long_number) ;
}


int
main ()
{
    buffer_overflows() ;
    rogue_numbers() ;
    missing_error_checks() ;
}
