#include <stdio.h>

#include "../include/cgi_decode.h"

int
main ()
{
    char dst[32] ;
    char src[32] = "Hello%2c+world%21" ;
    cgi_decode(dst, src) ;
    printf("%s\n", dst) ;
}