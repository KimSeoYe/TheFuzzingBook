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
int stderr_pipes[2] ;

void
child_proc (char * program, char * path)
{
    close(pipes[0]) ;
    close(stderr_pipes[0]) ;

    // dup2(pipes[1], 0) ; // ERROR (standard_in) 1: read() in flex scanner failed
    int dev_null = open("/dev/null", O_CLOEXEC) ;    // Q. ... close on exec
    dup2(dev_null, 0) ;
    dup2(pipes[1], 1) ;
    dup2(stderr_pipes[1], 2) ;

    execlp(program, program, path, 0x0) ;
}

int
parent_proc (char * dir_name, int i) 
{
    int exit_code ; 
    wait(&exit_code) ;

    close(pipes[1]) ;
    close(stderr_pipes[1]) ;

    char out_path[32] ;
    sprintf(out_path, "%s/%s%d", dir_name, "output", i) ;

    FILE * fp = fopen(out_path, "wb") ;
    if (fp == 0x0) {
        perror("fopen") ;
        exit(1) ;
    }   

    // Q. possible to get results from both stderr and stdout ? : >> if the fuzzer string has \n in the middle?

    int flag = 1 ;  // treat empty input as a normal

    char buf[1024] ;
    int s = 0 ;
    while ((s = read(pipes[0], buf, 1024)) > 0) {
        if (fwrite(buf, 1, s, fp) < s) {
            perror("fwrite") ;
        }
	}
    close(pipes[0]) ;

    while ((s = read(stderr_pipes[0], buf, 1024)) > 0) {
        flag = 2 ;  // TODO. inefficiant..
        if (fwrite(buf, 1, s, fp) < s) {
            perror("fwrite") ;
        }
	}
    close(stderr_pipes[0]) ;

    if (exit_code != 0) flag = -1 ;

    fclose(fp) ;

    return flag ;
}

int
my_popen (char * program, char * dir_name, char * in_path, int i)
{
    if (pipe(pipes) != 0) {
        perror("pipe") ;
        exit(1) ;
    }
    if (pipe(stderr_pipes) != 0) {
        perror("error pipe") ;
        exit(1) ;
    }

    int flag ;

    pid_t child_pid = fork() ;
    if (child_pid == 0) {
        child_proc(program, in_path) ;
    }
    else if (child_pid > 0) {
        flag = parent_proc(dir_name, i) ;
    }
    else {
        perror("fork") ;
        exit(1) ;
    }

    return flag ;
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

    int stdout_num = 0 ;
    int stderr_num = 0 ;
    int nonzero_ret_num = 0 ;
    int stdout_results[100] ; 
    int stderr_results[100] ;
    int nonzero_ret_results[100] ;

    for (int i = 0; i < trials; i++) {
        sprintf(in_path, "%s/%s%d", dir_name, "input", i) ;
        char * data = fuzzer(MAX_LEN, CHAR_START, CHAR_RANGE) ;
        data = realloc(data, strlen(data) + 1) ;
        data[strlen(data)] = '\n' ;

        if (write_data(in_path, data) == -1)  {
            exit(1) ;
        }
        free(data) ;

        int flag = my_popen(program, dir_name, in_path, i) ;
        if (flag == 1) {
            stdout_results[stdout_num] = i ;
            stdout_num++ ;
        }
        else if (flag == 2) { 
            stderr_results[stderr_num] = i ;
            stderr_num++ ;
        }
        else if (flag == -1) {
            nonzero_ret_results[nonzero_ret_num] = i ;
            nonzero_ret_num++ ;
        }
    }

    printf("STDOUT: %d\nSTDERR: %d\nCRASH: %d\n", stdout_num, stderr_num, nonzero_ret_num) ;
#ifdef DEBUG
    printf("STDOUT executions: \n") ;
    for (int j = 0; j < stdout_num; j++) {
        printf("%s/output%d\n", dir_name, stdout_results[j]) ;
    }
#endif
}

int
main ()
{
    srand(time(NULL)) ; 

    // invoking_extern_prog() ;
    long_running_fuzzing() ;
}
