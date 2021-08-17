#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../include/cgi_decode.h"

/**
 * Specifications
 * 1. replacement of '+'
 * 2. replacement of "%xx"
 * 3. non-replacement
 * 4. illegal inputs
*/

int
main ()
{
    char dst[32] ;
    
    cgi_decode(dst, "+") ;
    assert(strcmp(dst, " ") == 0) ;

    cgi_decode(dst, "%20") ;
    assert(strcmp(dst, " ") == 0) ;

    cgi_decode(dst, "abc") ;
    assert(strcmp(dst, "abc") == 0) ;

    assert(cgi_decode(dst, "%?a") == -1) ;
    assert(cgi_decode(dst, "%a?") == -1) ;
}