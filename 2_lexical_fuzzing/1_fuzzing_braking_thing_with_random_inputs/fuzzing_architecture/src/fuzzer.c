#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "../include/fuzzer.h"
#include "../include/fuzz_input.h"

#define DEBUG

static int trials ;
static fuzarg_t fuzargs ;
static runarg_t runargs ;
static int (* oracle) (char * dir_name) ;

// Q. static global ?
static char dir_name[32] ;  
static char ** parsed_args ;
static int arg_num = 0 ;

void
create_temp_dir ()
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
    trials = config->trials ;
    fuzargs = config->fuzargs ; // Q.
    
    strcpy(runargs.binary_path, config->runargs.binary_path) ;  // Q.
    strcpy(runargs.cmd_args, config->runargs.cmd_args) ;
    runargs.timeout = config->runargs.timeout ;

    oracle = config->oracle ; // Q.
}

void
parse_args ()
{   
    parsed_args = (char **) malloc(sizeof(char *) * 2) ; // path + null

    parsed_args[0] = (char *) malloc(sizeof(char) * (strlen(runargs.binary_path) + 1)) ;
    strcpy(parsed_args[0], runargs.binary_path) ;

    int i ;
    char * tok_ptr = strtok(runargs.cmd_args, " ") ; 
    for (i = 1; tok_ptr != NULL; i++) { 
        if (arg_num < i) parsed_args = realloc(parsed_args, sizeof(char *) * (++arg_num + 2)) ;
        
        parsed_args[i] = (char *) malloc(sizeof(char) * (strlen(tok_ptr) + 1)) ;
        strcpy(parsed_args[i], tok_ptr) ;

        tok_ptr = strtok(0x0, " ") ;
    }
    
    parsed_args[i] = (char *) malloc(sizeof(char) * 1) ;
    parsed_args[i] = 0x0 ;
}

void
fuzzer_init (test_config_t * config)
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
    for (int i = 0; i < arg_num + 2; i++) {
        if (parsed_args[i] != 0x0) printf("parsed_args[%d] %s\n", i, parsed_args[i]) ;
        else printf("parsed_args[%d] 0x0\n", i) ;
    }
#endif

    if (oracle == 0x0) {
        perror("Cannot access to the oracle tester") ;  // TODO. default oracle
        exit(1) ;
    }

    create_temp_dir() ;
}

static int stdin_pipes[2] ;
static int stdout_pipes[2] ;
static int stderr_pipes[2] ;

void
get_path (char * path, int trial, int fd)
{
    if (fd == 0) {
        sprintf(path, "%s/input%d", dir_name, trial) ;
    }
    else if (fd == 1) {
        sprintf(path, "%s/stdout%d", dir_name, trial) ;
    }
    else if (fd == 2) {
        sprintf(path, "%s/stderr%d", dir_name, trial) ;
    }
    else {
        perror("get_path: wrong fd") ;
        exit(1) ;
    }
}

void
write_input_files (char * input, int input_len, int trial)
{
    char in_path[32] ;
    get_path(in_path, trial, 0) ;

    FILE * in_fp = fopen(in_path, "wb") ;
    if (in_fp == 0x0) {
        perror("fopen") ;
        exit(1) ;
    }

    if (fwrite(input, 1, input_len, in_fp) != input_len) {  
        perror("fwrite: write_input_files") ; // TODO
    }

    fclose(in_fp) ;
}

void 
execute_target(char * input, int input_len, int trial)
{
    write_input_files(input, input_len, trial) ;

    if (write(stdin_pipes[1], input, input_len) != input_len) {
        perror("write: execute_target") ;
    }
    close(stdin_pipes[1]) ;

    dup2(stdin_pipes[0], 0) ;
    
    close(stdin_pipes[0]) ;
    close(stdout_pipes[0]) ;
    close(stderr_pipes[0]) ;

    dup2(stdout_pipes[1], 1) ;
    dup2(stderr_pipes[1], 2) ;

    execv(runargs.binary_path, parsed_args) ;
}

void
write_output_files (int trial, int fd){
    char path[32] ;
    get_path(path, trial, fd) ;

    FILE * fp = fopen(path, "wb") ;
    if (fp == 0x0) {
        perror("fopen: write_output_files") ;
        exit(1) ;
    }

    char buf[1024] ;
    int s = 0 ;
    
    if (fd == 1) {
        while ((s = read(stdout_pipes[0], buf, 1024)) > 0) {
            if (fwrite(buf, 1, s, fp) != s) {
                perror("fwrite: save_results: stdout") ;
            }
        }
        close(stdout_pipes[0]) ;
    }
    else if (fd == 2) {
        while ((s = read(stderr_pipes[0], buf, 1024)) > 0) {
            if (fwrite(buf, 1, s, fp) != s) {
                perror("fwrite: save_results: stderr") ;
            }
        }
        close(stderr_pipes[0]) ;
    }
    else {
        perror("write_output_files: wrong fd") ;
        exit(1) ;
    }

    fclose(fp) ;
}

void
save_results(int trial)
{
    close(stdin_pipes[1]) ;
    close(stdout_pipes[1]) ;
    close(stderr_pipes[1]) ;

    write_output_files(trial, 1) ;
    write_output_files(trial, 2) ;
}

void 
run (char * input, int input_len, int trial)
{
    if (pipe(stdin_pipes) != 0) goto pipe_err ;
    if (pipe(stdout_pipes) != 0) goto pipe_err ;
    if (pipe(stderr_pipes) != 0) goto pipe_err ;

    pid_t child_pid = fork() ;
    if (child_pid == 0) {
        execute_target(input, input_len, trial) ;
    }
    else if (child_pid > 0) {
        save_results(trial) ;
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
    for (int i = 0; i < arg_num + 2; i++) {
        free(parsed_args[i]) ;
    }
    free(parsed_args) ;
}

void
remove_temp_dir ()
{
    for (int fd = 0; fd < 3; fd++) {
        for (int i = 0; i < trials; i++) {
            char path[32] ;
            get_path(path, i, fd) ;
            remove(path) ;
        }
    }
    
    if (rmdir(dir_name) == -1) {
        perror("rmdir") ;
        exit(1) ;
    }
}

void
fuzzer_main (test_config_t * config)
{
    fuzzer_init(config) ;

    srand(time(NULL)) ;

    for (int i = 0; i < trials; i++) {
        char * input = (char *) malloc(sizeof(char) * (fuzargs.f_max_len + 1)) ;
        int input_len = fuzz_input(&fuzargs, input) ;

        run(input, input_len, i) ;
        free(input) ;

        // oracle check
    }

    // after loop ends, some works ?
    free_parsed_args() ;
    remove_temp_dir() ;
}