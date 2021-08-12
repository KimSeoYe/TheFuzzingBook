#include <stdio.h>
#include <unistd.h>

#include "../include/run.h"

void 
execute_binary(char * input, int * stdin_pipes, int * stdout_pipes, int * stderr_pipes)
{
    
}

void
save_results(char * input, int * stdin_pipes, int * stdout_pipes, int * stderr_pipes)
{

}

void 
run (test_config_t * config, char * input)
{
    int stdin_pipes[2] ;
    int stdout_pipes[2] ;
    int stderr_pipes[2] ;

    if (pipe(stdin_pipes) != 0) {
        perror("pipe") ;
        exit(1) ;
    }
    if (pipe(stdout_pipes) != 0) {
        perror("pipe") ;
        exit(1) ;
    }
    if (pipe(stderr_pipes) != 0) {
        perror("pipe") ;
        exit(1) ;
    }

    pid_t child_pid = fork() ;
    if (child_pid == 0) {
        execute_binary(input, stdin_pipes, stdout_pipes, stderr_pipes) ;
    }
    else if (child_pid > 0) {
        save_results(input, stdin_pipes, stdout_pipes, stderr_pipes) ;
    }
    else {
        perror("fork") ;
        exit(1) ;
    }
}