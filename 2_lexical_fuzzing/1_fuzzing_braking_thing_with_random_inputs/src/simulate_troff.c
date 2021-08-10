#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../include/simulate_troff.h"

// #define DEBUG

int
no_backslash_d (char * inp)
{
    char pattern[] = "\\D" ;
    char * find = 0x0 ;
    find = strstr(inp, pattern) ;
    
    if (find == 0x0 || strlen(find) == strlen(pattern)) return 1 ;  // TODO. modify (it has a problem w/ \0)

    char * c = find + strlen(pattern) ; 
#ifdef DEBUG
    printf("> %d\n", (int) *c) ;
#endif
    assert(*c > 31 && *c < 128) ;

    return 1 ;
}

