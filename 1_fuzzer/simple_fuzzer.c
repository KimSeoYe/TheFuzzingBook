#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LEN 100
#define CHAR_START 32
#define CHAR_RANGE 32

#define DEBUG

char *
fuzzer (int max_length, int char_start, int char_range)
{
    srand(time(NULL)) ;

    int string_length = rand() % (max_length + 1) ;
    char * out = (char *) malloc(sizeof(char) * string_length) ;    // Q. Is it okay to use malloc() here?
    for (int i = 0; i < string_length; i++) {
        out[i] = rand() % char_range + char_start ;
    }
#ifdef DEBUG
    printf("fuzzer: %s\n", out) ;
#endif

    return out ;
}

void
create_input_files ()
{
    char * basename = "input.txt" ;
    char * template = "temp/XXXXXX" ;
    char * dir_name = mkdtemp(template) ;
    if (dir_name == 0x0) {
        perror("mkdtemp") ;
        exit(1) ;
    }
    printf("dir_name: %s\n", dir_name) ;
}

int
main ()
{
    // fuzzer(MAX_LEN, CHAR_START, CHAR_RANGE) ;
    // fuzzer(1000, 'a', 26) ;
    create_input_files() ;
}