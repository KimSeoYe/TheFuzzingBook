#include <stdio.h>
#include <string.h>

#include "../include/mutating_input.h"
#include "../include/urlparse.h"

int
main ()
{
    char * seed = "http://www.google.com/search?q=fuzzing" ;
    // multiple_mutate(seed, strlen(seed), 50) ;
    char dst[1024] ;
    for (int i = 0; i < 10; i++) {
        insert_random_character(dst, seed, strlen(seed)) ;
        printf("[%d] %s\n", i, dst) ;
    }
}
