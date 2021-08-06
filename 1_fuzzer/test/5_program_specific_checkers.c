#include <stdio.h>
#include <stdlib.h>

#include "../include/airport_code.h"

#define DEBUG

void
initializer (Node * airport_code)
{
    append(airport_code, "YVR", "Vancouver") ;
    append(airport_code, "JFK", "New York-JFK") ;
    append(airport_code, "CDG", "Paris-Charles de Gaulle") ;
    append(airport_code, "CAI", "Cairo") ;
    append(airport_code, "LED", "St. Petersburg") ;
    append(airport_code, "PEK", "Beijing") ;
    append(airport_code, "HND", "Tokyo-Haneda") ;
    append(airport_code, "AKL", "Auckland") ;
#ifdef DEBUG
    print_list(airport_code) ;
#endif
}

void
_test_find_code (Node * airport_code, char * code)
{
    char target_city[CITY_MAX] ;
    if (!find_code(airport_code, code, target_city)) {
        printf("code not exist\n") ;
    }
    else printf("%s\n", target_city) ;
}

void
program_specific_checkers ()
{
    Node * airport_code = make_header() ;
    initializer(airport_code) ;

#ifdef DEBUG
    _test_find_code(airport_code, "YVR") ;
#endif

    free_list(airport_code) ;
}

int
main ()
{
    program_specific_checkers() ;
    return 0 ;
}