#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#include "lib/my_sqrt.h"

// #define DEBUG

int
automating_test_execution ()
{
    double result = my_sqrt(4) ;
    double expected_result = 2.0 ;
    if (result == expected_result) {
        printf("Test passed.\n") ;
        return 1 ;
    }
    else {
        printf("Test failed.\n") ;
        return 0 ;
    }
}

void
generating_tests ()
{
    clock_t start, end ;
    start = clock() ;
    srand(time(NULL)) ;

    for (int i = 0; i < 10000; i++) {
        int x = rand() % 1000000;
        assert_equals(my_sqrt(x) * my_sqrt(x), x, EPSILON) ;
    }

    end = clock() ;
    printf("%lu ms\n", end - start) ;
}

int
main ()
{
    // my_sqrt(4) ;
    // my_sqrt(2) ;
    // my_sqrt(9) ;

    // double square = my_sqrt(2) * my_sqrt(2) ;
    // printf("square: %f\n", square) ;

    // automating_test_execution() ;

    // assert_equals(my_sqrt(4), 2, 1e-8) ;
    // assert_equals(my_sqrt(9), 3, 1e-8) ;
    // assert_equals(my_sqrt(100), 10, 1e-8) ;

    generating_tests() ;

    sqrt_program("4") ;
    sqrt_program("-4") ;
    sqrt_program("xjfdsa") ;

    return 0 ;
}