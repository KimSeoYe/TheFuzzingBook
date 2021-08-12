#include <stdio.h>
#include <unistd.h>

#include "../include/run.h"

void 
execute_binary(test_config_t * config, char * input, int input_len, int * stdin_pipes, int * stdout_pipes, int * stderr_pipes)
{
    write(stdin_pipes[1], input, input_len) ;
    close(stdin_pipes[1]) ;

    dup2(stdin_pipes[0], 0) ;

    
    close(stdin_pipes[0]) ;
    close(stdout_pipes[0]) ;
    close(stderr_pipes[0]) ;

    dup2(stdout_pipes[1], 1) ;
    dup2(stderr_pipes[1], 2) ;

    // execute
}

void
save_results(test_config_t * config, char * input, int input_len, int * stdin_pipes, int * stdout_pipes, int * stderr_pipes)
{

}

void 
run (test_config_t * config, char * input, int input_len)
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
        execute_binary(config, input, input_len, stdin_pipes, stdout_pipes, stderr_pipes) ;
    }
    else if (child_pid > 0) {
        save_results(config, input, input_len, stdin_pipes, stdout_pipes, stderr_pipes) ;
    }
    else {
        perror("fork") ;
        exit(1) ;
    }
}