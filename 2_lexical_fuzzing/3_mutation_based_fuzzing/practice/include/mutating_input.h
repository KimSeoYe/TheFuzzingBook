#ifndef MUTATE
#define MUTATE

int delete_random_character (char * dst, char * seed, int seed_len) ;

int insert_random_character (char * dst, char * seed, int seed_len) ;

int flip_random_character (char * dst, char * seed, int seed_len) ;

int mutate (char * dst, char * seed, int seed_len) ;
void multiple_mutate (char * seed, int seed_len, int trials) ;

#endif