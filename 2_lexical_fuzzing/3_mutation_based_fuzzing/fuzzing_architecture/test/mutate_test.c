#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "../include/mutate.h"

int
main ()
{
    srand(time(NULL)) ;

    char * seed = "Hello World!\n" ;
    char dst[32] ;
    
    delete_bytes(dst, seed, strlen(seed)) ;
    printf(">> %s\n", dst) ;

    insert_bytes(dst, seed, strlen(seed)) ;
    printf(">> %s\n", dst) ;

    bit_flip(dst, seed, strlen(seed)) ;
    printf(">> %s\n", dst) ;

    byte_flip(dst, seed, strlen(seed)) ;
    printf(">> %s\n", dst) ;

    simple_arithmatic(dst, seed, strlen(seed)) ;
    printf(">> %s\n", dst) ;

    known_integers(dst, seed, strlen(seed)) ;
    printf(">> %s\n", dst) ;

    mutate(dst, seed, strlen(seed)) ;
    printf(">> %s\n", dst) ;
}