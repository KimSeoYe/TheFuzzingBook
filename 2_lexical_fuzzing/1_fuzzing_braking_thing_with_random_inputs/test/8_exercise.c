#include <stdio.h>

#include "../include/simulate_troff.h"

void
test_no_backslash_d ()
{
    if (no_backslash_d("Hello world!")) printf("PASS!\n") ;
    if (no_backslash_d("It is for \\Dtesting")) printf("PASS!\n") ;
    if (no_backslash_d("troff like scenario\\D")) printf("PASS!\n") ;

    char test_input[10] = "TEST\\D" ;
    test_input[6] = (char) 1 ;
    test_input[7] = 0x0 ;
    if (no_backslash_d(test_input)) printf("PASS!\n") ;
}

int
main ()
{
    test_no_backslash_d() ;
}