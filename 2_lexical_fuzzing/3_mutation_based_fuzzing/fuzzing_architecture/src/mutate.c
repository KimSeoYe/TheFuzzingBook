#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../include/config.h"
#include "../include/mutate.h"
#include "../include/fuzzer.h"

// #define DEBUG

typedef enum mutator { DELETE = 0, INSERT, FLIP } mutator_t ;

int
delete_random_character (char * dst, char * seed, int seed_len) 
{
    if (seed_len == 0) {
        dst[0] = 0x0 ;
        perror("delete_random_character: seed_len == 0") ;
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
    if (seed_len == 0) {
        dst[0] = 0x0 ;
        perror("insert_random_character: seed_len == 0") ;
        return 0 ;
    } 

    int position = rand() % seed_len ;
    char rand_char = (char) (rand() % 96 + 32) ; // TODO. range

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
    if (seed_len == 0) {
        dst[0] = 0x0 ;
        perror("flip_random_character: seed_len == 0") ;
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

void
get_seed_path (char * dst, char * dir_name, char * file_name)
{
    if (strlen(dir_name) + strlen(file_name) + 1 > PATH_MAX) {
        perror("get_seed_path: Too long path") ;
        exit(1) ;
    }
    strcpy(dst, dir_name) ;
    if (dst[strlen(dst) - 1] != '/') {
        strcat(dst, "/") ;
    }
    strcat(dst, file_name) ;
}

int
mutate_input (char * dst, fuzarg_t * fuzargs, char * seed_filename)
{ 
    char seed_path[PATH_MAX] ;
    get_seed_path(seed_path, fuzargs->seed_dir, seed_filename) ;

    FILE * fp = fopen(seed_path, "rb") ;
    if (fp == 0x0) {
        perror("mutate_input: fopen") ;
        exit(1) ;
    }

    int input_max = 1024 ;
    char * seed_input = (char *) malloc(sizeof(char) * input_max) ;

    int s, total_len = 0 ;
    char buf[1024] ;
    while ((s = fread(buf, 1, 1024, fp)) > 0) {
        if (total_len + s > 1024) {
            seed_input = realloc(seed_input, sizeof(char) * ((total_len + s) / 1024 + 1)) ;
        }

        memcpy(seed_input + total_len, buf, s) ;
        total_len += s ;
    }
    fclose(fp) ;

    int mutate_len = mutate(dst, seed_input, total_len) ;
    free(seed_input) ;
    
    return mutate_len ;
}