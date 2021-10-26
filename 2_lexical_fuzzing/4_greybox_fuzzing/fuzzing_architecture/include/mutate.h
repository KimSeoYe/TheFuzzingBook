#ifndef MUTATE
#define MUTATE

#include "../include/config.h"

#define INITIAL_E 100

typedef struct _seed {
    char seed_filename[PATH_MAX] ;
    int energy ;
} seed_t ;

int flip_bits (char * dst, int dst_len, char * seed, int seed_len, int position) ;

int delete_bytes (char * dst, int dst_len, char * seed, int seed_len, int position) ;

int insert_random_bytes (char * dst, int dst_len, char * seed, int seed_len, int position) ;

int insert_known_constants (char * dst, int dst_len, char * seed, int seed_len, int position) ;

int flip_bytes (char * dst, int dst_len, char * seed, int seed_len, int position) ;

int flip_known_constants (char * dst, int dst_len, char * seed, int seed_len, int position) ;

int simple_arithmatic (char * dst, int dst_len, char * seed, int seed_len, int position) ;

int copy_another_offset (char * dst, int dst_len, char * seed, int seed_len, int position) ;

int mutate (char * dst, int dst_len, char * seed, int seed_len) ;

void get_seed_path (char * dst, char * dir_name, char * file_name) ;

int mutate_input (char ** dst, int dst_len, fuzarg_t * fuzargs, char * seed_filename, int is_initial) ;

#endif