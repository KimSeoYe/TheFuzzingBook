#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "../include/fuzzer.h"
#include "../include/fuzz_input.h"

#define DEBUG

static int trial ;
static fuzarg_t fuzargs ;
static runarg_t runargs ;
static int (* oracle) (char * dir_name) ;

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
copy_status (test_config_t * config)
{
    trial = config->trial ;
    fuzargs = config->fuzargs ;
    
    strcpy(runargs.binary_path, config->runargs.binary_path) ;
    if (config->runargs.cmd_args != 0x0) strcpy(runargs.cmd_args, config->runargs.cmd_args) ;
    runargs.timeout = config->runargs.timeout ;

    oracle = config->oracle ; // Q.
}

void
fuzzer_init (test_config_t * config, char * dir_name)
{
    copy_status(config) ;

    if (access(runargs.binary_path, X_OK) == -1) {
        perror("Cannot access to the binary path") ;
        exit(1) ;
    }
    /*
        Q. exec에 상대 경로를 넣었을 경우 ?
        Q. command line argument를 어떤 식으로 넘겨줄지 (exec 함수들 중 무엇을 선택할지)    
    */

    if (oracle == 0x0) {
        perror("Cannot access to the oracle tester") ;  // TODO. default oracle
        exit(1) ;
    }

    create_input_dir(dir_name) ;
}

static int stdin_pipes[2] ;
static int stdout_pipes[2] ;
static int stderr_pipes[2] ;

void 
execute_target(runarg_t * runargs, char * input, int input_len)
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
save_results(runarg_t * runargs, char * dir_name, char * input, int input_len)
{

}

void 
run (runarg_t * runargs, char * dir_name, char * input, int input_len)
{
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
        execute_target(runargs, input, input_len) ;
    }
    else if (child_pid > 0) {
        save_results(runargs, dir_name, input, input_len) ;
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

    for (int i = 0; i < trial; i++) {
        char * input = (char *) malloc(sizeof(char) * (fuzargs.f_max_len + 1)) ;
        int input_len = fuzz_input(&fuzargs, input) ;
    #ifdef DEBUG
        printf("FUZZ INPUT: (%d)%s\n", input_len, input) ;
    #endif

        run(&runargs, dir_name, input, input_len) ;
        free(input) ;

        // oracle check
    }

    // after loop ends, some works ?
}