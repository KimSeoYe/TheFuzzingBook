#ifndef MUTATE
#define MUTATE

#include "../include/config.h"

int flip_bits (char * dst, int dst_len, char * seed, int seed_len, int position) ;

int delete_bytes (char * dst, int dst_len, char * seed, int seed_len, int position) ;

int insert_random_bytes (char * dst, int dst_len, char * seed, int seed_len, int position) ;

int insert_known_constants (char * dst, int dst_len, char * seed, int seed_len, int position) ;

int flip_bytes (char * dst, int dst_len, char * seed, int seed_len, int position) ;

int flip_known_constants (char * dst, int dst_len, char * seed, int seed_len, int position) ;

int simple_arithmatic (char * dst, int dst_len, char * seed, int seed_len, int position) ;

int copy_another_offset (char * dst, int dst_len, char * seed, int seed_len, int position) ;

int mutate (char * dst, int dst_len, char * seed, int seed_len) ;

int mutate_input (char * dst, int dst_len, fuzarg_t * fuzargs, char * seed_filename) ;

#endif