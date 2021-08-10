#include <stdio.h>

#include "../include/simulate_troff.h"

void
test_no_backslash_d ()
{
    no_backslash_d("Hello world!") ;
    no_backslash_d("It is for \\Dtesting") ;
    char test_input[10] = "TEST\\D" ;
    test_input[6] = (char) 1 ;
    test_input[7] = 0x0 ;
    no_backslash_d(test_input) ;
}

int
main ()
{
    test_no_backslash_d() ;
}