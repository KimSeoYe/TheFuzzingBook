#ifndef MUTATE
#define MUTATE

#include "../include/config.h"

int bit_flip (char * dst, char * seed, int seed_len, int position) ;

int delete_bytes (char * dst, char * seed, int seed_len, int position) ;

int insert_bytes (char * dst, char * seed, int seed_len, int position) ;

int flip_bytes (char * dst, char * seed, int seed_len, int position) ;

int simple_arithmatic (char * dst, char * seed, int seed_len, int position) ;

int known_integers (char * dst, char * seed, int seed_len, int position) ;

int mutate (char * dst, char * seed, int seed_len) ;

int mutate_input (char * dst, fuzarg_t * fuzargs, char * seed_filename) ;

#endif