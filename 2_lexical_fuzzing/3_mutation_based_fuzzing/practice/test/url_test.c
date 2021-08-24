#include <stdio.h>
#include <string.h>

#include "../include/mutating_input.h"
#include "../include/urlparse.h"

int
main ()
{
    char * seed = "http://www.google.com/search?q=fuzzing" ;
    multiple_mutate(seed, strlen(seed), 50) ;
}