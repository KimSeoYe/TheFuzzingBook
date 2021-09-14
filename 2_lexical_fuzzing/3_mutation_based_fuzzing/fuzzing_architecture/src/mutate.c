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
flip_bits (char * dst, char * seed, int seed_len, int position) 
{
    if (seed_len == 0) {
        dst[0] = 0x0 ;
        perror("flip_bits: seed_len is 0") ;
        return 0 ;
    } 

    int bit_size[3] = { 0b0001, 0b0011, 0b1111 } ;
    int bit_idx = rand() % 3 ;

    int bit = bit_size[bit_idx] << (rand() % 7) ;

    char new_char = (char) seed[position] ^ bit ;
    
#ifdef DEBUG
    printf("flip_bits: byte-size=%d, position=%d\n", bit_size[bit_idx], position) ;
#endif

    memcpy(dst, seed, position) ;
    dst[position] = new_char ;

    memcpy(dst + position + 1, seed + position + 1, seed_len - position - 1) ;
    dst[seed_len] = 0x0 ;

    return seed_len ;
}

int
delete_bytes (char * dst, char * seed, int seed_len, int position) 
{
    if (seed_len == 0) {
        dst[0] = 0x0 ;
        perror("delete: seed_len is 0") ;
        return 0 ;
    } 

    int byte_size[3] = { 1, 2, 4 } ;
    int byte_idx = rand() % 3 ;

#ifdef DEBUG
    printf("delete_bytes: byte-size=%d, position=%d\n", byte_size[byte_idx], position) ;
#endif

    memcpy(dst, seed, position) ;

    if (byte_size[byte_idx] > seed_len - position) {
        dst[position] = 0x0 ;
        return position ;
    }

    memcpy(dst + position, seed + position + byte_size[byte_idx], seed_len - position - byte_size[byte_idx]) ;
    dst[seed_len - byte_size[byte_idx]] = 0x0 ;

    return seed_len - byte_size[byte_idx] ;
}

int
insert_random_bytes (char * dst, char * seed, int seed_len, int position) 
{   
    int byte_size[3] = { 1, 2, 4 } ;
    int byte_idx = rand() % 3 ;

    memcpy(dst, seed, position) ;
    
    for (int i = 0; i < byte_size[byte_idx]; i++) {
        char rand_char = (char) (rand() % 96 + 32) ; // TODO. range
        dst[position + i] = rand_char ;
    }

#ifdef DEBUG
    printf("insert_random_bytes: byte-size=%d, position=%d\n", byte_size[byte_idx], position) ;
#endif

    memcpy(dst + position + byte_size[byte_idx], seed + position, seed_len - position) ;
    dst[seed_len + byte_size[byte_idx]] = 0x0 ;

    return seed_len + byte_size[byte_idx] ;
}

int
insert_known_constants (char * dst, char * seed, int seed_len, int position) 
{   
    int byte_size[3] = { 1, 2, 4 } ;
    int byte_idx = rand() % 3 ;
    
    int known_constant_num[] = { 9, 10, 8 } ;
    int constant_idx ;
    switch (byte_idx) {
        case 0:
            constant_idx = rand() % known_constant_num[0] ;
            * (u_int8_t *)(dst + position) = interesting_8[constant_idx] ;
        case 1:
            constant_idx = rand() % known_constant_num[1] ;
            * (u_int16_t *)(dst + position) = interesting_8[constant_idx] ;
        case 2:
            constant_idx = rand() % known_constant_num[2] ;
            * (u_int32_t *)(dst + position) = interesting_8[constant_idx] ;
    }  

#ifdef DEBUG
    printf("insert_known_constants: byte-size=%d, position=%d\n", byte_size[byte_idx], position) ;
#endif

    memcpy(dst, seed, position) ;
    memcpy(dst + position + byte_size[byte_idx], seed + position, seed_len - position) ;
    dst[seed_len + byte_size[byte_idx]] = 0x0 ;

    return seed_len + byte_size[byte_idx] ;
}

int
flip_bytes (char * dst, char * seed, int seed_len, int position) 
{
    if (seed_len == 0) {
        dst[0] = 0x0 ;
        perror("flip_bytes: seed_len is 0") ;
        return 0 ;
    } 

    int byte_size[3] = { 1, 2, 4 } ;
    int byte_idx = rand() % 3 ;

    memcpy(dst, seed, position) ;

    char new_char ;
    if (seed_len - position <= byte_size[byte_idx]) {  // flip all bytes   
        for (int i = 0; i < seed_len - position; i++) {
            new_char = (char) seed[position + i] ^ 0xff ;
            dst[position + i] = new_char ;
        }
    }
    else {
        for (int i = 0; i < byte_size[byte_idx]; i++) {
            new_char = (char) seed[position + i] ^ 0xff ;
            dst[position + i] = new_char ;
        }

        memcpy(dst + position + byte_size[byte_idx], seed + position + byte_size[byte_idx], seed_len - position - byte_size[byte_idx]) ;
    }

    dst[seed_len] = 0x0 ;

#ifdef DEBUG
    printf("flip_bytes: byte-size=%d, position=%d\n", byte_size[byte_idx], position) ;
#endif

    return seed_len ;
}

int
flip_known_constants (char * dst, char * seed, int seed_len, int position) 
{
    if (seed_len == 0) {
        dst[0] = 0x0 ;
        perror("flip_known_constants: seed_len is 0") ;
        return 0 ;
    }
    
    int byte_size[3] = { 1, 2, 4 } ;
    int byte_idx = rand() % 3 ;

    if (seed_len - position < byte_size[byte_idx]) {
        memcpy(dst, seed, seed_len + 1) ;   // contain 0x0
        perror("flip_known_constatns: Range overflow") ;
        return seed_len ;
    }
    
    int known_constant_num[] = { 9, 10, 8 } ;
    int constant_idx ;
    switch (byte_idx) {
        case 0:
            constant_idx = rand() % known_constant_num[0] ;
            * (u_int8_t *)(dst + position) = interesting_8[constant_idx] ;
        case 1:
            constant_idx = rand() % known_constant_num[1] ;
            * (u_int16_t *)(dst + position) = interesting_8[constant_idx] ;
        case 2:
            constant_idx = rand() % known_constant_num[2] ;
            * (u_int32_t *)(dst + position) = interesting_8[constant_idx] ;
    }  

#ifdef DEBUG
    printf("flip_known_constants: byte-size=%d, position=%d\n", byte_size[byte_idx], position) ;
#endif

    memcpy(dst, seed, position) ;
    memcpy(dst + position + byte_size[byte_idx], seed + position + byte_size[byte_idx], seed_len - position - byte_size[byte_idx]) ;
    dst[seed_len] = 0x0 ;

    return seed_len ;
}

int
simple_arithmatic (char * dst, char * seed, int seed_len, int position) 
{
    if (seed_len == 0) {
        dst[0] = 0x0 ;
        perror("simple_arithmatic: seed_len is 0") ;
        return 0 ;
    } 

    int byte_size[3] = { 1, 2, 4 } ;
    int byte_idx = rand() % 3 ;

    memcpy(dst, seed, position) ;

    char new_char ;
    int add_operand ;
    if (seed_len - position <= byte_size[byte_idx]) {
        for (int i = 0; i < seed_len - position; i++) {
            add_operand = rand() % 71 - 35 ;
            new_char = (char) seed[position] + add_operand ;
            dst[position + i] = new_char ;
        }
    }
    else {
        for (int i = 0; i < byte_size[byte_idx]; i++) {
            add_operand = rand() % 71 - 35 ;
            new_char = (char) seed[position] + add_operand ;
            dst[position + i] = new_char ;
        }

        memcpy(dst + position + byte_size[byte_idx], seed + position + byte_size[byte_idx], seed_len - position - byte_size[byte_idx]) ;
    }

    dst[seed_len] = 0x0 ;

#ifdef DEBUG
    printf("simple_arithmatic: byte-size=%d, position=%d\n", byte_size[byte_idx], position) ;
#endif

    return seed_len ;
}

int
copy_another_offset (char * dst, char * seed, int seed_len, int position) 
{
    if (seed_len == 0) {
        dst[0] = 0x0 ;
        perror("copy_another_offset: seed_len is 0") ;
        return 0 ;
    } 

    int byte_size[3] = { 1, 2, 4 } ;
    int byte_idx = rand() % 3 ;

    if (seed_len - position <= byte_size[byte_idx]) {
        memcpy(dst, seed, seed_len + 1) ; 
        perror("copy_another_offset: Range overflow") ;
        return seed_len ;
    }

    int src_position = rand() % (seed_len - byte_size[byte_idx]) ;
    
    memcpy(dst, seed, position) ;
    memcpy(dst + position, seed + src_position, byte_size[byte_idx]) ;
    memcpy(dst + position + byte_size[byte_idx], seed + position + byte_size[byte_idx], seed_len - position - byte_size[byte_idx]) ;
    
    dst[seed_len] = 0x0 ;

#ifdef DEBUG
    printf("copy_another_offset: byte-size=%d, position=%d\n", byte_size[byte_idx], position) ;
#endif

    return seed_len ;
}

int
mutate (char * dst, char * seed, int seed_len) 
{
    int (* mutator[]) (char *, char *, int, int) = { flip_bits, delete_bytes, insert_random_bytes, insert_known_constants, flip_bytes, flip_known_constants, simple_arithmatic, copy_another_offset } ;

    int mutator_idx = rand() % 6 ;
    int new_len = 0 ;

    int position ;
    if (seed_len != 0) position = rand() % seed_len ;
    else position = 0 ;

    new_len = mutator[mutator_idx](dst, seed, seed_len, position) ; 

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