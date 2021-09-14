#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "../include/mutate.h"

int
main ()
{
    srand(time(NULL)) ;

    char * seed = "Hi" ;
    char dst[32] ;
    int len = 0 ;
    
    len = bit_flip(dst, seed, strlen(seed), 0) ;
    printf(">> %s (%d)\n", dst, len) ;

    len = delete_bytes(dst, seed, strlen(seed), 0) ;
    printf(">> %s (%d)\n", dst, len) ;

    len = insert_bytes(dst, seed, strlen(seed), 0) ;
    printf(">> %s (%d)\n", dst, len) ;

    len = flip_bytes(dst, seed, strlen(seed), 0) ;
    printf(">> %s (%d)\n", dst, len) ;

    // len = simple_arithmatic(dst, seed, strlen(seed), 0) ;
    // printf(">> %s (%d)\n", dst, len) ;

    // len = known_integers(dst, seed, strlen(seed), 0) ;
    // printf(">> %s (%d)\n", dst, len) ;

    // len = mutate(dst, seed, strlen(seed)) ;
    // printf(">> %s (%d)\n", dst, len) ;
}