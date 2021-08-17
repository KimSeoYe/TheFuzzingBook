#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>

#include "../include/fuzzer.h"
#include "../include/fuzz_input.h"

// #define DEBUG


///////////////////////////////////// Fuzzer Status /////////////////////////////////////

static int trials ;
static fuzarg_t fuzargs ;
static runarg_t runargs ;
static int (* oracle) (int return_code, int trial) ;

// Q. static global ?
static char dir_name[RESULT_PATH_MAX] ;  
static char ** parsed_args ;
static int arg_num = 0 ;


///////////////////////////////////// Fuzzer Init /////////////////////////////////////

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
    fuzargs = config->fuzargs ; 

    if (realpath(config->runargs.binary_path, runargs.binary_path) == 0x0) {
        perror("realpath: copy_status") ;
        exit(1) ;
    }

    strcpy(runargs.cmd_args, config->runargs.cmd_args) ;
    runargs.timeout = config->runargs.timeout ;

    oracle = config->oracle ; // Q.
}

void
parse_args ()
{   
    parsed_args = (char **) malloc(sizeof(char *) * 2) ; // path + null

    parsed_args[0] = (char *) malloc(sizeof(char) * (strlen(runargs.binary_path) + 1)) ;
    strcpy(parsed_args[0], runargs.binary_path) ;   // TODO. filename ?

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

int
default_oracle (int return_codes, int trial)    // Q.
{
    if (return_codes == 0) return 0 ;
    else return -1 ;
}

void
fuzzer_init (test_config_t * config)
{
    copy_status(config) ;

    if (fuzargs.f_min_len > fuzargs.f_max_len) {
        perror("fuzzer_init: invalid fuzzer arguments:\n\tf_min_len is bigger than f_max_len") ;
        exit(1) ;
    }

    if (fuzargs.f_char_start < 0 || fuzargs.f_char_start > 127) {
        perror("fuzzer_init: invalid f_char_start value") ;
        exit(1) ;
    }

    if (fuzargs.f_char_start + fuzargs.f_char_range > 255) {
        perror("fuzzer_init: invalid range") ;
        exit(1) ;
    }

    if (access(runargs.binary_path, X_OK) == -1) {
        perror("fuzzer_init: cannot access to the binary path") ;
        exit(1) ;
    }

    if (runargs.timeout < 0) {
        perror("fuzzer_init: invalid value for timeout") ;
        exit(1) ;
    }

    if (oracle == 0x0) {
        oracle = default_oracle ;
    }

    parse_args() ;
#ifdef DEBUG
    for (int i = 0; i < arg_num + 2; i++) {
        if (parsed_args[i] != 0x0) printf("parsed_args[%d] %s\n", i, parsed_args[i]) ;
        else printf("parsed_args[%d] 0x0\n", i) ;
    }
#endif

    create_temp_dir() ;
}


///////////////////////////////////// Fuzzer Run /////////////////////////////////////

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
    char in_path[RESULT_PATH_MAX] ;
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

    alarm(runargs.timeout) ;

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

    if (execv(runargs.binary_path, parsed_args) == -1) {
        perror("execv: execute_target") ;
        exit(1) ;
    }
}

void
write_output_files (char ** stdout_contents, char ** stderr_contents, int trial, int fd){
    char path[RESULT_PATH_MAX] ;
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
            strncpy(stdout_contents[trial], buf, 15) ;
            stdout_contents[trial][15] = 0x0 ;

            if (fwrite(buf, 1, s, fp) != s) {
                perror("fwrite: save_results: stdout") ;
            }
        }
        close(stdout_pipes[0]) ;
    }
    else if (fd == 2) {
        while ((s = read(stderr_pipes[0], buf, 1024)) > 0) {
            strncpy(stderr_contents[trial], buf, 15) ;
            stderr_contents[trial][15] = 0x0 ;

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
save_results(char ** stdout_contents, char ** stderr_contents, int trial)
{
    close(stdin_pipes[1]) ;
    close(stdout_pipes[1]) ;
    close(stderr_pipes[1]) ;
    write_output_files(stdout_contents, stderr_contents, trial, 1) ;
    write_output_files(stdout_contents, stderr_contents, trial, 2) ;
}

static pid_t child_pid ;

void
timeout_handler (int sig)
{
    if (sig == SIGALRM) {
        perror("timeout") ;
        if (kill(child_pid, SIGINT) == -1) {
            perror("kill: timeout_handler") ;
            exit(1) ;
        }
    }
}

int
run (char ** stdout_contents, char ** stderr_contents, char * input, int input_len, int trial)
{    
    if (pipe(stdin_pipes) != 0) goto pipe_err ;
    if (pipe(stdout_pipes) != 0) goto pipe_err ;
    if (pipe(stderr_pipes) != 0) goto pipe_err ;

    child_pid = fork() ;
    if (child_pid == 0) {
        execute_target(input, input_len, trial) ;
    }
    else if (child_pid > 0) {
        save_results(stdout_contents, stderr_contents, trial) ;
    }
    else {
        perror("fork") ;
        exit(1) ;
    }

    int exit_code ;
    pid_t term_pid = wait(&exit_code) ;
    // TODO. process termination check
#ifdef DEBUG
    printf("%d terminated w/ exit code %d\n", term_pid, exit_code) ;
#endif

    return exit_code ;

pipe_err:
    perror("pipe") ;
    exit(1) ;
}


///////////////////////////////////// Fuzzer Oracle /////////////////////////////////////

result_t
oracle_run (int return_code, int trial)   // Q. useless..?
{
    result_t result ;

    int ret = oracle(return_code, trial) ;
    switch(ret) {
        case 0:
            result = PASS ;
            break ;
        case -1:
            result = FAIL ;
            break ;
        default:
            result = UNRESOLVED ;
    }
    return result ;
}


///////////////////////////////////// Fuzzer Summary /////////////////////////////////////

void
fuzzer_summary (int * return_codes, result_t * results, char ** stdout_contents, char ** stderr_contents)
{
    int pass_cnt = 0 ;
    int fail_cnt = 0 ;
    int unresolved_cnt = 0 ;

    for (int i = 0; i < trials; i++) {
        printf("(CompletedProcess(target='%s', args='%s', returncode='%d', stdout='%s', stderr='%s', result='%s'))\n", runargs.binary_path, runargs.cmd_args, return_codes[i], stdout_contents[i], stderr_contents[i], result_strings[results[i]]) ;
        switch(results[i]) {
            case PASS:
                pass_cnt++ ;
                break ;
            case FAIL:
                fail_cnt++ ;
                break ;
            default:
                unresolved_cnt++ ;
        }
    }

    printf("=======================================================\n") ;
    printf("TOTAL SUMMARY\n") ;
    printf("=======================================================\n") ;
    printf("# TRIALS : %d\n", trials) ;
    printf("# PASS : %d\n", pass_cnt) ;
    printf("# FAIL : %d\n", fail_cnt) ;
    printf("# UNRESOLVED : %d\n", unresolved_cnt) ;
    printf("=======================================================\n") ;
}


///////////////////////////////////// Fuzzer Completion /////////////////////////////////////

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
            char path[RESULT_PATH_MAX] ;
            get_path(path, i, fd) ;
            remove(path) ;
        }
    }
    
    if (rmdir(dir_name) == -1) {
        perror("rmdir") ;
        exit(1) ;
    }
}

///////////////////////////////////// Fuzzer Main /////////////////////////////////////

void
free_contents (char ** stdout_contents, char ** stderr_contents)
{
    for (int i = 0; i < trials; i++) {
        free(stdout_contents[i]) ;
        free(stderr_contents[i]) ;
    }

    free(stdout_contents) ;
    free(stderr_contents) ;
}

void
fuzzer_main (test_config_t * config)
{
    srand(time(NULL)) ;
    signal(SIGALRM, timeout_handler) ;
    
    fuzzer_init(config) ;

    int * return_codes = (int *) malloc(sizeof(int) * trials) ;
    result_t * results = (result_t *) malloc(sizeof(result_t) * trials) ;

    char ** stdout_contents = (char **) malloc(sizeof(char *) * trials) ;
    char ** stderr_contents = (char **) malloc(sizeof(char *) * trials) ;
    for (int i = 0; i < trials; i++) {
        stdout_contents[i] = (char *) malloc(sizeof(char) * 16) ;
        stderr_contents[i] = (char *) malloc(sizeof(char) * 16) ;
    }

    for (int i = 0; i < trials; i++) {
        char * input = (char *) malloc(sizeof(char) * (fuzargs.f_max_len + 1)) ;
        int input_len = fuzz_input(&fuzargs, input) ;

        return_codes[i] = run(stdout_contents, stderr_contents, input, input_len, i) ;
        free(input) ;

        results[i] = oracle_run(return_codes[i], i) ;
    }

    fuzzer_summary(return_codes, results, stdout_contents, stderr_contents) ;

    free(return_codes) ;
    free(results) ;
    free_parsed_args() ;
    free_contents(stdout_contents, stderr_contents) ;
    remove_temp_dir() ;
}