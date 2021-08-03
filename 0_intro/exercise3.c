#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

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


// need to fix !
double *
quadratic_solver (int a, int b, int c)
{
    // if (a == 0) return 0 ;
    double * solutions = (double *) malloc(sizeof(double) * 2) ;
    if (a == 0) {
        if (b == 0) {
            if (c == 0) {
                // return 0 ?
                solutions[0] = 0.0 ;
                solutions[1] = 0.0 ;
            }
            else {
                return 0x0 ;
            }
        }
        else {
            solutions[0] = -c / (double)b ;
            solutions[1] = 0x0 ;
        }
    }
    else {
        int q = b * b - 4 * a * c ;
        if (q < 0) return 0x0 ;

        solutions[0] = (-1 * b + my_sqrt_fixed(q)) / (double)(2 * a) ;
        solutions[1] = (-1 * b - my_sqrt_fixed(q)) / (double)(2 * a) ;
    }

    return solutions ;
}

void
part3_odd_and_ends ()
{
    uint64_t combinations = pow(2, 32) ;  // Q. 2^32 * 2^32 ?
    uint64_t test_per_sec = 1000000000 ;
    uint64_t sec_per_year = 60 * 60 * 24 * 365 ;
    uint64_t test_per_year = test_per_sec * sec_per_year ;

    printf("%Lf\n", (long double)combinations / test_per_year) ;    // cannot calculate the result
}

int
main ()
{
    double * solutions ;
    if ((solutions = quadratic_solver(3, 4, 1)) != 0x0){ 
        printf("%f %f\n", solutions[0], solutions[1]) ;
    } else {
        printf("There's no solution\n") ;
    }
    free(solutions) ;

    if ((solutions = quadratic_solver(1, 1, 1)) != 0x0){ 
        printf("%f %f\n", solutions[0], solutions[1]) ;
    } else {
        printf("There's no solution\n") ;
    }
    free(solutions) ;

    if ((solutions = quadratic_solver(0, 1, 1)) != 0x0){ 
        printf("%f %f\n", solutions[0], solutions[1]) ;
    } else {
        printf("There's no solution\n") ;
    }
    free(solutions) ;

    if ((solutions = quadratic_solver(0, 0, 1)) != 0x0){ 
        printf("%f %f\n", solutions[0], solutions[1]) ;
    } else {
        printf("There's no solution\n") ;
    }
    free(solutions) ;

    if ((solutions = quadratic_solver(0, 0, 0)) != 0x0){ 
        printf("%f %f\n", solutions[0], solutions[1]) ;
    } else {
        printf("There's no solution\n") ;
    }
    free(solutions) ;

    part3_odd_and_ends() ;
}