#include <assert.h>

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