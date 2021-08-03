#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

// #define DEBUG
#define EPSILON 1e-8

double
my_sqrt (int x)
{
    // Compute the sqare root of x, using the Newton-Raphson method
    double approx = 0.0 ;
    double guess = x / 2.0 ;

    while (approx != guess) {
    #ifdef DEBUG
        printf("approx = %f\n", approx) ;
    #endif
        approx = guess ;
        guess = (approx + x / approx) / 2 ; 
    }
#ifdef DEBUG
    printf("%f\n", approx) ;
#endif
    return approx ;
}

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
assert_equals (double x, double y, double epsilon)
{
    assert(fabs(x - y) < epsilon) ;
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

float
my_sqrt_checked (int x)
{
    double root = my_sqrt(x) ;
    assert_equals(root * root, x, EPSILON) ;
    return root ;
}

int
is_number(char * arg)
{
    for(int i = 0; arg[i] != 0x0; i++) {
        if (arg[i] == '-' || arg[i] == '+') continue ;
        if (isdigit(arg[i]) == 0) return 0 ;
    }

    return 1 ;
}

void
sqrt_program(char * arg)
{
    if (!is_number(arg)) 
        goto err_exit ;
    
    int x = atoi(arg) ;
    if (x < 0)
        goto err_exit ;
    else
        printf("The root of %d is %f\n", x, my_sqrt(x)) ;
    
    return ;

err_exit:
    printf("Illegal Input\n") ;
}

double
my_sqrt_fixed(int x)
{
    assert(x >= 0) ;
    if (x == 0) return 0 ;
    return my_sqrt(x) ;
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