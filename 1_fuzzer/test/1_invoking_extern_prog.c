#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#include "../include/fuzzer.h"
#include "../include/fileio.h"

#define DEBUG

int pipes[2] ;
int error_pipes[2] ;

void
child_proc (char * program, char * path)
{
    close(pipes[0]) ;
    close(error_pipes[0]) ;
    // dup2(pipes[1], 0) ; // ERROR (standard_in) 1: read() in flex scanner failed
    int dev_null = open("/dev/null", O_CLOEXEC) ;    // Q. ... close on exec
    dup2(dev_null, 0) ;
    dup2(pipes[1], 1) ;
    dup2(error_pipes[1], 2) ;

    execlp(program, program, path, 0x0) ;
}

void
parent_proc (char * dir_name, int i) 
{
    int exit_code ;
    wait(&exit_code) ;

#ifdef DEBUG
    printf("[%d] execution end with exit code %d\n", i, exit_code) ;
#endif

    close(pipes[1]) ;
    close(error_pipes[1]) ;

    char out_path[32] ;
    sprintf(out_path, "%s/%s%d", dir_name, "output", i) ;

    FILE * fp = fopen(out_path, "wb") ;
    if (fp == 0x0) {
        perror("fopen") ;
        return ;
    }   

    // Q. possible to get results from both stdout and stderr ?
    // Q. can i choose a pipe ...?
    // int w = fwrite(&exit_code, 1, sizeof(exit_code), fp) ;   // TODO. use it later
    char header[32] ;
    sprintf(header, "%d\n", exit_code) ;
    int w = fwrite(header, 1, strlen(header), fp) ;

    char buf[1024] ;
    int s ;
    while ((s = read(pipes[0], buf, 1024)) > 0) {
        if ((w = fwrite(buf, 1, s, fp)) != s) {
            perror("fwrite") ;
        }
	}
    close(pipes[0]) ;

    while ((s = read(error_pipes[0], buf, 1024)) > 0) {
        if ((w = fwrite(buf, 1, s, fp)) != s) {
            perror("fwrite") ;
        }
	}
    close(error_pipes[0]) ;

    fclose(fp) ;

    return ;
}

int
my_popen (char * program, char * dir_name, char * in_path, int i)
{
    if (pipe(pipes) != 0) {
        perror("pipe") ;
        exit(1) ;
    }
    if (pipe(error_pipes) != 0) {
        perror("error pipe") ;
        exit(1) ;
    }

    pid_t child_pid = fork() ;
    if (child_pid == 0) {
        child_proc(program, in_path) ;
    }
    else if (child_pid > 0) {
        parent_proc(dir_name, i) ;
    }
    else {
        perror("fork") ;
        return -1 ;
    }


    return 1 ;
}

void
invoking_extern_prog ()
{
    char * program = "bc" ;

    char dir_name[32] ;
    char path[32] ;
    create_input_dir(dir_name) ;
    sprintf(path, "%s/%s", dir_name, "input") ;

    if (write_data(path, "2 + 2\n") == -1)  return ;
    my_popen(program, dir_name, path, 0) ;
    
    if (remove(path) == -1) {
        perror("remove") ;
        exit(1) ;
    }
    // remove_input_dir(dir_name) ;
}

void
long_running_fuzzing ()
{
    int trials = 100 ;
    char * program = "bc" ;

    char dir_name[32] ;
    create_input_dir(dir_name) ;

    char in_path[32] ;

    // Q. 100 input files and 100 output files..?
    for (int i = 0; i < trials; i++) {
        sprintf(in_path, "%s/%s%d", dir_name, "input", i) ;
        char * data = fuzzer(MAX_LEN, CHAR_START, CHAR_RANGE) ;
        if (write_data(in_path, data) == -1)  return ;
        int exit_code = my_popen(program, dir_name, in_path, i) ;   // TODO. something with error code
        free(data) ;
    }
}

int
main ()
{
    srand(time(NULL)) ; 

    // invoking_extern_prog() ;
    long_running_fuzzing() ;
}
