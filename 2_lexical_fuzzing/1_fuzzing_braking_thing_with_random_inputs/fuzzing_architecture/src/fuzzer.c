#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "../include/fuzzer.h"
#include "../include/fuzz_input.h"

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
fuzzer_init (test_config_t * config, char * dir_name)
{
    /*
        1. check some conditions
            - default values >> in config.h
            - binary path validity
            - check if the oracle function is not empty ?
        2. make result directory
    */

    if (access(config->binary_path, X_OK) == -1) {
        perror("Cannot access to the binary path") ;
        exit(1) ;
    }
    /*
        Q. exec에 상대 경로를 넣었을 경우 ?
        Q. command line argument를 어떤 식으로 넘겨줄지 (exec 함수들 중 무엇을 선택할지)    
    */

    if (config->oracle == 0x0) {
        perror("Cannot access to the oracle tester") ;
        exit(1) ;
    }

    create_input_dir(dir_name) ;
}


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

void
fuzzer_main (test_config_t * config)
{
    char dir_name[32] ;
    fuzzer_init(config, dir_name) ;

    srand(time(NULL)) ;

    for (int i = 0; config->trial; i++) {
        char * input = (char *) malloc(sizeof(char) * (config->f_max_len + 1)) ; // Q.
        int input_len = fuzz_input(config, input) ;

        run(config, input, input_len) ;

        config->oracle(dir_name) ;

        free(input) ;
    }
}