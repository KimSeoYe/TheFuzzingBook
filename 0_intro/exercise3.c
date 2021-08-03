#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdint.h>

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

double
my_sqrt_fixed(int x)
{
    assert(x >= 0) ;
    if (x == 0) return 0 ;
    return my_sqrt(x) ;
}

/*
    part1. Find bug-trigerring inputs

    1. a division by zero : if a == 0
    2. violate the precondition of my_sqrt_fixed() : x(q) should be non-negative


    part3. Odds and Ends
    
    What are the chances of discovering these conditions with random inputs? 
    Assuming one can do a billion tests per second, 
    how long would one have to wait on average until a bug gets triggered?
*/

int
quadratic_solver (int a, int b, int c, double * solution_1, double * solution_2)
{
    if (a == 0) return 0 ;

    int q = b * b - 4 * a * c ;
    if (q < 0) return 0 ;

    *solution_1 = (-1 * b + my_sqrt_fixed(q)) / (double)(2 * a) ;
    *solution_2 = (-1 * b - my_sqrt_fixed(q)) / (double)(2 * a) ;

    return 1 ;
}

void
part3_odd_and_ends ()
{
    uint64_t combinations = pow(2, 32) ;  // Q.
    uint64_t test_per_sec = 1000000000 ;
    uint64_t sec_per_year = 60 * 60 * 24 * 365 ;
    uint64_t test_per_year = test_per_sec * sec_per_year ;

    printf("%Lf\n", (long double)combinations / test_per_year) ;    // cannot calculate the result
}

int
main ()
{
    double sol_1, sol_2 ;
    if (quadratic_solver(3, 4, 1, &sol_1, &sol_2)){ 
        printf("%f %f\n", sol_1, sol_2) ;
    } else {
        printf("There's no solution or it's not a quadratic.\n") ;
    }

    if (quadratic_solver(1, 1, 1, &sol_1, &sol_2)){ 
        printf("%f %f\n", sol_1, sol_2) ;
    } else {
        printf("There's no solution or it's not a quadratic.\n") ;
    }

    if (quadratic_solver(0, 1, 1, &sol_1, &sol_2)){ 
        printf("%f %f\n", sol_1, sol_2) ;
    } else {
        printf("There's no solution or it's not a quadratic.\n") ;
    }

    part3_odd_and_ends() ;
}