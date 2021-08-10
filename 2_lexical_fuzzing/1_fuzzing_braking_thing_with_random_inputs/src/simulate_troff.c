#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../include/simulate_troff.h"

#define DEBUG

int
no_backslash_d (char * inp)
{
    char pattern[] = "\\D" ;
    char * find = 0x0 ;
    find = strstr(inp, pattern) ;
    
    if (find == 0x0 || strlen(find) == strlen(pattern)) return 1 ;

    char * c = find + strlen(pattern) ;
#ifdef DEBUG
    printf("> %d\n", (int) *c) ;
#endif
    assert(*c > 31 && *c < 256) ;

    return 1 ;
}