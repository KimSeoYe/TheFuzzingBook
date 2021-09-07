#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "../include/config.h"
#include "../include/mutate.h"
#include "../include/fuzzer.h"

#define DEBUG

#define INTERESTING_8 \
  -128,          /* Overflow signed 8-bit when decremented  */ \
  -1,            /*                                         */ \
   0,            /*                                         */ \
   1,            /*                                         */ \
   16,           /* One-off with common buffer size         */ \
   32,           /* One-off with common buffer size         */ \
   64,           /* One-off with common buffer size         */ \
   100,          /* One-off with common buffer size         */ \
   127           /* Overflow signed 8-bit when incremented  */

#define INTERESTING_16 \
  -32768,        /* Overflow signed 16-bit when decremented */ \
  -129,          /* Overflow signed 8-bit                   */ \
   128,          /* Overflow signed 8-bit                   */ \
   255,          /* Overflow unsig 8-bit when incremented   */ \
   256,          /* Overflow unsig 8-bit                    */ \
   512,          /* One-off with common buffer size         */ \
   1000,         /* One-off with common buffer size         */ \
   1024,         /* One-off with common buffer size         */ \
   4096,         /* One-off with common buffer size         */ \
   32767         /* Overflow signed 16-bit when incremented */

#define INTERESTING_32 \
  -2147483648LL, /* Overflow signed 32-bit when decremented */ \
  -100663046,    /* Large negative number (endian-agnostic) */ \
  -32769,        /* Overflow signed 16-bit                  */ \
   32768,        /* Overflow signed 16-bit                  */ \
   65535,        /* Overflow unsig 16-bit when incremented  */ \
   65536,        /* Overflow unsig 16 bit                   */ \
   100663045,    /* Large positive number (endian-agnostic) */ \
   2147483647    /* Overflow signed 32-bit when incremented */

static u_int8_t  interesting_8[] = { INTERESTING_8 } ;
static u_int16_t interesting_16[] = { INTERESTING_8, INTERESTING_16 } ;
static u_int32_t interesting_32[] = { INTERESTING_8, INTERESTING_16, INTERESTING_32 } ;

typedef enum mutator { DELETE = 0, INSERT, BIT_FLIP, BYTE_FLIP, ARITHMATIC, KNOWN_INT } mutator_t ;


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
bit_flip (char * dst, char * seed, int seed_len) 
{
    if (seed_len == 0) {
        dst[0] = 0x0 ;
        perror("bit_flip: seed_len == 0") ;
        return 0 ;
    } 

    int bit_size[3] = { 1, 3, 15 } ;
    int bit_index = rand() % 3 ;

    int position = rand() % seed_len ;
    int bit = bit_size[bit_index] << (rand() % 7) ;

    char new_char = (char) seed[position] ^ bit ;   // Q.
    
#ifdef DEBUG
    printf("Bit-flip %d in %d giving %c at %d\n", bit, seed[position], new_char, position) ;
#endif

    memcpy(dst, seed, position) ;
    dst[position] = new_char ;

    memcpy(dst + position + 1, seed + position + 1, seed_len - position - 1) ;
    dst[seed_len] = 0x0 ;

    return seed_len ;
}

int
byte_flip (char * dst, char * seed, int seed_len) 
{
    if (seed_len == 0) {
        dst[0] = 0x0 ;
        perror("byte_flip: seed_len == 0") ;
        return 0 ;
    } 

    int position ;
    char new_char ;

    int byte_size[3] = { 1, 2, 4 } ;
    for (int i = 2; i >= 0; i--) {
        if (seed_len >= byte_size[i]) {
            if (seed_len == byte_size[i]) position = 0 ;
            else position = rand() % (seed_len - byte_size[i]) ;
            new_char = (char) seed[position] ^ 0xff ;
            break ;
        }
    }

#ifdef DEBUG
    printf("Byte-flip: seed[position]=%d, new_char=%c, position=%d\n", seed[position], new_char, position) ;
#endif

    memcpy(dst, seed, position) ;
    dst[position] = new_char ;

    memcpy(dst + position + 1, seed + position + 1, seed_len - position - 1) ;
    dst[seed_len] = 0x0 ;

    return seed_len ;
}

int
simple_arithmatic (char * dst, char * seed, int seed_len) 
{
    if (seed_len == 0) {
        dst[0] = 0x0 ;
        perror("simple_arithmatic: seed_len == 0") ;
        return 0 ;
    } 

    int position = rand() % seed_len ;
    int add_operand = rand() % 71 - 35 ;
    char new_char = (char) seed[position] + add_operand ;

#ifdef DEBUG
    printf("Simple arithmatic: seed[position]=%d, new_char=%c, position=%d\n", seed[position], new_char, position) ;
#endif

    memcpy(dst, seed, position) ;
    dst[position] = new_char ;

    memcpy(dst + position + 1, seed + position + 1, seed_len - position - 1) ;
    dst[seed_len] = 0x0 ;

    return seed_len ;
}

int
known_integers (char * dst, char * seed, int seed_len) 
{
    if (seed_len == 0) {
        dst[0] = 0x0 ;
        perror("known_integers: seed_len == 0") ;
        return 0 ;
    } 

    int known_integers_num[] = { 9, 10, 8 } ;

    int position = rand() % seed_len ;

    int category = rand() % 3 ;
    int index = rand() % known_integers_num[category] ;
    char new_char ;

    switch (category) {
        case 0:
            new_char = interesting_8[index] ;
            break ;
        case 1:
            new_char = interesting_16[index] ;
            break ;
        case 2:
            new_char = interesting_32[index] ;
            break ;
    }

#ifdef DEBUG
    printf("Known integer: seed[position]=%d, new_char=%c, position=%d\n", seed[position], new_char, position) ;
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
    int (* mutator[6]) (char *, char *, int) = { delete_random_character, insert_random_character, bit_flip, byte_flip, simple_arithmatic, known_integers } ;

    int mutator_idx = rand() % 6 ;
    int new_len = 0 ;

    mutator[mutator_idx](dst, seed, seed_len) ; 

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