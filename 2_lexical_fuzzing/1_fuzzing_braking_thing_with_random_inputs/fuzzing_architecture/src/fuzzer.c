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
static char ** parsed_args ;
static int (* oracle) (char * dir_name) ;

void
create_temp_dir (char * dir_name)
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
    fuzargs = config->fuzargs ; // Q.
    
    strcpy(runargs.binary_path, config->runargs.binary_path) ;  // Q.
    strcpy(runargs.cmd_args, config->runargs.cmd_args) ;
    runargs.num_args = config->runargs.num_args ;
    runargs.timeout = config->runargs.timeout ;

    oracle = config->oracle ; // Q.
}

void
parse_args ()
{
    parsed_args = (char **) malloc(sizeof(char *) * (runargs.num_args + 2)) ; // path + null

    parsed_args[0] = (char *) malloc(sizeof(char) * (strlen(runargs.binary_path) + 1)) ;
    strcpy(parsed_args[0], runargs.binary_path) ;

    int i ;
    char * tok_ptr = strtok(runargs.cmd_args, " ") ; 
    for (i = 1; tok_ptr != NULL; i++) {
        parsed_args[i] = (char *) malloc(sizeof(char) * (strlen(tok_ptr) + 1)) ;
        strcpy(parsed_args[i], tok_ptr) ;
        tok_ptr = strtok(0x0, " ") ;
    }
    parsed_args[i] = (char *) malloc(sizeof(char) * 1) ;
    parsed_args[i] = 0x0 ;

    // if num_args is less than the real number of args..? >> seg fault
    if (runargs.num_args != i - 1) {
        perror("Invalid number of argument") ;
        exit(1) ; 
    }
}

void
fuzzer_init (test_config_t * config, char * dir_name)
{
    copy_status(config) ;

    if (fuzargs.f_min_len > fuzargs.f_max_len) {
        perror("Invalid fuzzer arguments:\n\tf_min_len is bigger than f_max_len") ;
        exit(1) ;
    }

    if (access(runargs.binary_path, X_OK) == -1) {
        perror("Cannot access to the binary path") ;
        exit(1) ;
    }

    parse_args() ;

#ifdef DEBUG
    for (int i = 0; i < runargs.num_args + 2; i++) {
        if (parsed_args[i] != 0x0) printf("parsed_args[%d] %s\n", i, parsed_args[i]) ;
        else printf("parsed_args[%d] 0x0\n", i) ;
    }
#endif

    if (oracle == 0x0) {
        perror("Cannot access to the oracle tester") ;  // TODO. default oracle
        exit(1) ;
    }

    create_temp_dir(dir_name) ;
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

    execv(runargs->binary_path, parsed_args) ;
}

void
save_results(runarg_t * runargs, char * dir_name, char * input, int input_len)
{
    close(stdin_pipes[1]) ;
    close(stdout_pipes[1]) ;
    close(stderr_pipes[1]) ;

    // FILE * input_fp = fopen()

    char buf[1024] ;
    int len = 0 ;
    int s = 0 ;

    while ((s = read(stdout_pipes[0], buf, 1024)) > 0) {
        len += s ;

    }
}

void 
run (runarg_t * runargs, char * dir_name, char * input, int input_len)
{
    if (pipe(stdin_pipes) != 0) goto pipe_err ;
    if (pipe(stdout_pipes) != 0) goto pipe_err ;
    if (pipe(stderr_pipes) != 0) goto pipe_err ;

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

    return ;

pipe_err:
    perror("pipe") ;
    exit(1) ;
}

void
free_parsed_args ()
{
    for (int i = 0; i < runargs.num_args + 2; i++) {
        free(parsed_args[i]) ;
    }
    free(parsed_args) ;
}

void
remove_temp_dir (char * dir_name)
{
    // TODO. remove files
    if (rmdir(dir_name) == -1) {
        perror("rmdir") ;
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
    free_parsed_args() ;
    remove_temp_dir(dir_name) ;
}