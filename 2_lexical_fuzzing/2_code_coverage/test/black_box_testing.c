#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../include/cgi_decode.h"

/**
 * Black-Box Testing to find errors in the specified behavior.
 * 
 * Specifications
 * 1. replacement of '+'
 * 2. replacement of "%xx"
 * 3. non-replacement
 * 4. illegal inputs
*/

int
main ()
{
    char dst[16] ;

    cgi_decode(dst, "+") ;
    assert(strcmp(dst, " ") == 0) ;

    cgi_decode(dst, "%20") ;
    assert(strcmp(dst, " ") == 0) ;

    cgi_decode(dst, "%20%2c") ;
    assert(strcmp(dst, " ,") == 0) ;

    cgi_decode(dst, "a%20b%20c") ;
    assert(strcmp(dst, "a b c") == 0) ;

    cgi_decode(dst, "abc") ;
    assert(strcmp(dst, "abc") == 0) ;

    assert(cgi_decode(dst, "%?a") == -1) ;
    assert(cgi_decode(dst, "%a?") == -1) ;
}