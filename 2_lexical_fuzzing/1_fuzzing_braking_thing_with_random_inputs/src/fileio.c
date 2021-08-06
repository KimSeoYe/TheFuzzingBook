#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../include/fileio.h"

int
write_data (char * path, char * data)
{            
    FILE * fp = fopen(path, "wb") ;
    if (fp == 0x0) {
        perror("fopen") ;
        return -1 ;
    }

    int data_size = fwrite(data, 1, strlen(data), fp) ; 
    if (data_size != strlen(data)) {
        printf("Write to the end!\n") ; // TODO. to the end ?
    }

    fclose(fp) ;
    return data_size ;
}


char *
read_data (char * path, int data_size)
{            
    FILE * fp = fopen(path, "rb") ;
    if (fp == 0x0) {
        perror("fopen") ;
        exit(1) ;
    }

    char * data = (char *) malloc(sizeof(char) * data_size) ;
    if (fread(data, 1, data_size, fp) != data_size) {
        perror("fread") ;
        return 0x0 ;
    }
#ifdef DEBUG
    printf("data: %s\n", data) ;
#endif

    fclose(fp) ;
    return data ;
}


void
create_input_dir (char * dir_name)
{
    char template[] = "tmp.XXXXXX" ;
    char * temp_dir = mkdtemp(template) ;
    if (temp_dir == 0x0) {
        perror("mkdtemp") ;
        exit(1) ;
    }
    strcpy(dir_name, temp_dir) ;
}

void
remove_input_dir (char * dir_name)
{
    if (rmdir(dir_name) == -1) {
        perror("rmdir") ;
        exit(1) ;
    }
}
