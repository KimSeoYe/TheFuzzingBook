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
    
    len = flip_bits(dst, seed, strlen(seed), 0) ;
    printf(">> %s (%d)\n", dst, len) ;

    len = delete_bytes(dst, seed, strlen(seed), 0) ;
    printf(">> %s (%d)\n", dst, len) ;

    len = insert_random_bytes(dst, seed, strlen(seed), 0) ;
    printf(">> %s (%d)\n", dst, len) ;

    len = insert_known_constants(dst, seed, strlen(seed), 0) ;
    printf(">> %s (%d)\n>>", dst, len) ;
    for (int i = 0; i < len; i++) {
        printf(" %x", dst[i]) ;
    }
    printf("\n") ;

    len = flip_bytes(dst, seed, strlen(seed), 0) ;
    printf(">> %s (%d)\n", dst, len) ;

    // len = simple_arithmatic(dst, seed, strlen(seed), 0) ;
    // printf(">> %s (%d)\n", dst, len) ;

    // len = known_integers(dst, seed, strlen(seed), 0) ;
    // printf(">> %s (%d)\n", dst, len) ;

    // len = mutate(dst, seed, strlen(seed)) ;
    // printf(">> %s (%d)\n", dst, len) ;
}