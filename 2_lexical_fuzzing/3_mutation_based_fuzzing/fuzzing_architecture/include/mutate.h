#ifndef MUTATE
#define MUTATE

#include "../include/config.h"

int delete_bytes (char * dst, char * seed, int seed_len) ;

int insert_bytes (char * dst, char * seed, int seed_len) ;

int bit_flip (char * dst, char * seed, int seed_len) ;

int byte_flip (char * dst, char * seed, int seed_len) ;

int simple_arithmatic (char * dst, char * seed, int seed_len) ;

int known_integers (char * dst, char * seed, int seed_len) ;

int mutate (char * dst, char * seed, int seed_len) ;

int mutate_input (char * dst, fuzarg_t * fuzargs, char * seed_filename) ;

#endif