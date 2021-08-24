#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define DEBUG

typedef enum mutator { DELETE = 0, INSERT, FLIP } mutator_t ;

int
delete_random_character (char * dst, char * seed, int seed_len) 
{
    if (strcmp(seed, "") == 0) {
        strncpy(dst, seed, seed_len) ;
        return 0 ;
    } 

    int position = rand() % seed_len ;
#ifdef DEBUG
    printf("Deleting %c at %d\n", seed[position], position) ;
#endif

    memcpy(dst, seed, position) ;
    memcpy(dst + position, seed + position + 1, seed_len - position - 1) ;
    dst[seed_len - 1] = 0x0 ;

    return seed_len - 1 ;
}

int
insert_random_character (char * dst, char * seed, int seed_len) 
{
    if (strcmp(seed, "") == 0) {
        strncpy(dst, seed, seed_len) ;
        return 0 ;
    } 

    int position = rand() % seed_len ;
    char rand_char = (char) (rand() % 96 + 32) ; 

#ifdef DEBUG
    printf("Inserting %c at %d\n", rand_char, position) ;
#endif

    memcpy(dst, seed, position) ;
    dst[position] = rand_char ;

    memcpy(dst + position + 1, seed + position, seed_len - position) ;
    dst[seed_len + 1] = 0x0 ;

    return seed_len + 1 ;
}

int
flip_random_character (char * dst, char * seed, int seed_len) 
{
    if (strcmp(seed, "") == 0) {
        dst[0] = 0x0 ;
        return 0 ;
    } 

    int position = rand() % seed_len ;
    int bit = 1 << (rand() % 7) ;
    char new_char = (char) seed[position] ^ bit ;   // Q.

#ifdef DEBUG
    printf("Flipping %d in %d giving %c at %d\n", bit, seed[position], new_char, position) ;
#endif

    memcpy(dst, seed, position) ;
    dst[position] = new_char ;

    memcpy(dst + position + 1, seed + position + 1, seed_len - position - 1) ;
    dst[seed_len] = 0x0 ;

    return seed_len ;
}

int
mutate (char * dst, char * seed, int seed_len)
{
    int mutator = rand() % 3 ;
    int new_len = 0 ;

    switch (mutator) {
        case DELETE: 
            new_len = delete_random_character(dst, seed, seed_len) ;
            break ;
        case INSERT:
            new_len = insert_random_character(dst, seed, seed_len) ;
            break ;
        case FLIP:
            new_len = flip_random_character(dst, seed, seed_len) ;
            break ;
    }

    return new_len ;
}

// temp
int
main ()
{
    srand(time(0x0)) ;

    char seed_input[] = "A quick brown fox" ;
    char dst[128] ;

    mutate(dst, seed_input, strlen(seed_input)) ;
    printf(">> %s\n", dst) ;
}