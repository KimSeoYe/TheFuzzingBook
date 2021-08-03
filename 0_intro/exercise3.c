#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

double
my_sqrt (double x)
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
my_sqrt_fixed(double x)
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
quadratic_solver (double a, double b, double c, double * solutions)
{
    if (a == 0) {
        if (b == 0) {
            if (c == 0) {
                // return 0 ? (for all x, true)
                solutions[0] = 0.0 ;
                solutions[1] = 0x0 ;
            }
            else {
                solutions = 0x0 ;
                return 0 ;
            }
        }
        else {
            solutions[0] = -c / b ;
            solutions[1] = 0x0 ;
        }
    }
    else {
        int q = b * b - 4 * a * c ;
        if (q < 0) {
            solutions = 0x0 ;
            return 0 ;
        }

        solutions[0] = (-1 * b + my_sqrt_fixed(q)) / (2 * a) ;
        solutions[1] = (-1 * b - my_sqrt_fixed(q)) / (2 * a) ;
    }

    return 1 ;
}

void
part2_fix_the_problem ()
{
    double solutions[2] ;

    if (quadratic_solver(3, 4, 1, solutions)){ 
        printf("%f %f\n", solutions[0], solutions[1]) ;
    } else {
        printf("There's no solution\n") ;
    }

    if (quadratic_solver(1, 1, 1, solutions)){ 
        printf("%f %f\n", solutions[0], solutions[1]) ;
    } else {
        printf("There's no solution\n") ;
    }

    if (quadratic_solver(0, 1, 1, solutions)){ 
        printf("%f %f\n", solutions[0], solutions[1]) ;
    } else {
        printf("There's no solution\n") ;
    }

    if (quadratic_solver(0, 0, 1, solutions)){ 
        printf("%f %f\n", solutions[0], solutions[1]) ;
    } else {
        printf("There's no solution\n") ;
    }

    if (quadratic_solver(0, 0, 0, solutions)){ 
        printf("%f %f\n", solutions[0], solutions[1]) ;
    } else {
        printf("There's no solution\n") ;
    }
}

void
part3_odd_and_ends ()
{
    // for the first condition, a == 0 && b == 0
    uint64_t combinations = pow(2, 63) ;  // Q. 
    uint64_t test_per_sec = 1000000000 ;
    uint64_t sec_per_year = 60 * 60 * 24 * 365 ;
    uint64_t test_per_year = test_per_sec * sec_per_year ;

    // Q. how about the second condition "b^2 - ac < 0" ?

    printf("%Lf years\n", ((long double)combinations / test_per_year) * 2) ;  
}

void
part4_to_infinity_and_beyond ()
{
    printf("%f\n", my_sqrt_fixed(__FLT_MAX__ * 1000)) ;
}

int
main ()
{  
    part2_fix_the_problem() ;
    part3_odd_and_ends() ;
    part4_to_infinity_and_beyond() ;
}