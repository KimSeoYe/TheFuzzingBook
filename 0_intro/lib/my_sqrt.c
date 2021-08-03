#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

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

void 
assert_equals (double x, double y, double epsilon)
{
    assert(fabs(x - y) < epsilon) ;
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