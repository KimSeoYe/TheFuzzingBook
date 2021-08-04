#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "fileio.h" 

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

// this is for test -> TODO. split files!

int
write_fuzzer_data (char * path)
{            
    FILE * fp = fopen(path, "wb") ;
    if (fp == 0x0) {
        perror("fopen") ;
        return -1 ;
    }

    char * data = fuzzer(MAX_LEN, CHAR_START, CHAR_RANGE) ;
    int data_size = fwrite(data, 1, strlen(data), fp) ; 
    if (data_size != strlen(data)) {
        printf("Write to the end!\n") ; // TODO. to the end ?
    }

    free(data) ;
    fclose(fp) ;
    return data_size ;
}

void
read_fuzzer_data (char * path, int data_size)
{            
    FILE * fp = fopen(path, "rb") ;
    if (fp == 0x0) {
        perror("fopen") ;
        exit(1) ;
    }

    char * data = (char *) malloc(sizeof(char) * data_size) ;
    if (fread(data, 1, data_size, fp) != data_size) {
        perror("fread") ;
        return ;
    }
#ifdef DEBUG
    printf("data: %s\n", data) ;
#endif

    free(data) ;
    fclose(fp) ;
}

void
fuzzer_with_files ()
{
    char path[32] ;
    char dir_name[32] ;
    create_input_files(dir_name, path) ;
#ifdef DEBUG
    printf("path: %s\n", path) ;
    printf("dir_name: %s\n", dir_name) ;
#endif

    int data_size = write_fuzzer_data(path) ;
    if (data_size != -1) {
        read_fuzzer_data(path, data_size) ;
    }

    remove_input_files(dir_name, path) ;
}