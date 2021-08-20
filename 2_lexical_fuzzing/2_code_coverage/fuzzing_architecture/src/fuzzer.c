#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>

#include "../include/fuzzer.h"
#include "../include/fuzz_input.h"
#include "../include/gcov_runner.h"

// #define DEBUG

///////////////////////////////////// Fuzzer Status /////////////////////////////////////

static int trials ;
fuzopt_t option ;
static int fuzzed_args_num = 0 ;
static int is_source ;
static char source_path[PATH_MAX] ;
static char source_filename[PATH_MAX] ;
static fuzarg_t fuzargs ;
static runarg_t runargs ;
static int (* oracle) (int return_code, int trial) ;

// Q. static global ?
static char dir_name[RESULT_PATH_MAX] ;  
static char ** parsed_args ; // TODO. as a local var.
static int arg_num = 2 ;


///////////////////////////////////// Fuzzer Init /////////////////////////////////////

void
copy_status (test_config_t * config)
{
    trials = config->trials ;
    fuzargs = config->fuzargs ; 

    option = config->option ;
    if (option == ARGUMENT) {
        fuzzed_args_num = config->fuzzed_args_num ;
    }

    is_source = config->is_source ;
    if (is_source) {
        if (realpath(config->source_path, source_path) == 0x0) {
            perror("copy_status: realpath: source_path") ;
            exit(1) ;
        }
    }  
    else {
        if (realpath(config->runargs.binary_path, runargs.binary_path) == 0x0) {
            perror("copy_status: realpath: runargs.binary_path") ;
            exit(1) ;
        }  
    }   

    strcpy(runargs.cmd_args, config->runargs.cmd_args) ;
    runargs.timeout = config->runargs.timeout ;

    oracle = config->oracle ; // Q.
}

void
parse_args ()
{   
    parsed_args = (char **) malloc(sizeof(char *) * ARG_N_MAX) ;

    parsed_args[0] = (char *) malloc(sizeof(char) * (strlen(runargs.binary_path) + 1)) ;
    strcpy(parsed_args[0], runargs.binary_path) ;   // TODO. filename ?

    int i ;
    char * tok_ptr = strtok(runargs.cmd_args, " ") ; 
    for (i = 1; tok_ptr != NULL; i++) { 
        arg_num++ ;
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

    if (is_source) {
        if (access(source_path, R_OK) == -1) {
            perror("copy_status: access: cannot access to the source path") ;
            exit(1) ;
        }

        get_source_filename(source_filename, source_path) ;
        get_executable_real_path(runargs.binary_path, source_path) ;
        compile_with_coverage(runargs.binary_path, source_path) ;
    }

    if (access(runargs.binary_path, X_OK) == -1) {
        perror("fuzzer_init: access: cannot access to the binary path") ;
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
    for (int i = 0; i < arg_num; i++) {
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

    if (option == STD_IN) {
        if (write(stdin_pipes[1], input, input_len) != input_len) {
            perror("execute_target: write") ;
        }
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


///////////////////////////////////// Fuzzer Loop /////////////////////////////////////

void
fuzz_argument (fuzarg_t * fuzargs)
{
    int i ;
    for (i = arg_num - 1; i < arg_num + fuzzed_args_num - 1; i++) {
        parsed_args[i] = (char *) malloc(sizeof(char) * (fuzargs->f_max_len + 1)) ;
        fuzz_input(fuzargs, parsed_args[i]) ;
    }
    parsed_args[i] = 0x0 ;
    arg_num += fuzzed_args_num ;
}

double
fuzzer_loop (int * return_codes, result_t * results, char ** stdout_contents, char ** stderr_contents, int * coverages, char * cov_set, int total_line_cnt) 
{
    clock_t start = clock() ;

    for (int i = 0; i < trials; i++) {
        char * input = (char *) malloc(sizeof(char) * (fuzargs.f_max_len + 1)) ;
        
        int input_len = 0 ;
        if (option == STD_IN) input_len = fuzz_input(&fuzargs, input) ;
        else if (option == ARGUMENT) fuzz_argument(&fuzargs) ;

    #ifdef DEBUG
        printf("FUZZER INPUT : %s\n", input) ;
    #endif

        return_codes[i] = run(stdout_contents, stderr_contents, input, input_len, i) ;
        free(input) ;

        if (is_source) {
            coverages[i] = get_coverage(cov_set, total_line_cnt, source_filename) ;
        }

        results[i] = oracle_run(return_codes[i], i) ;
    }

    clock_t end = clock() ;
    double exec_time_ms = (double) end - start ;

    return exec_time_ms ;
}

///////////////////////////////////// Fuzzer Summary /////////////////////////////////////

void
fuzzer_summary (int * return_codes, result_t * results, char ** stdout_contents, char ** stderr_contents, int * coverages, char * cov_set, int cov_set_len, double exec_time_ms)
{
    int pass_cnt = 0 ;
    int fail_cnt = 0 ;
    int unresolved_cnt = 0 ;

    for (int i = 0; i < trials; i++) {
        // TODO. exec_time
        printf("(CompletedProcess(target='%s', args='%s', coverage='%d', returncode='%d', stdout='%s', stderr='%s', result='%s'))\n", runargs.binary_path, runargs.cmd_args, coverages[i], return_codes[i], stdout_contents[i], stderr_contents[i], result_strings[results[i]]) ;
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

    int total_coverage = 0 ;
    if (is_source) {
        printf("\n=======================================================\n") ;
        printf("LINE COVERAGES\n") ;
        printf("=======================================================\n") ;
        for (int i = 0; i < trials; i++) {
            printf("%d : ", coverages[i]) ;
            for (int j = 0; j < coverages[i]; j++) printf("#") ;
            printf("\n") ;
        }
        printf("=======================================================\n") ;

        for (int i = 0; i < cov_set_len; i++) {
            total_coverage += cov_set[i] - '0' ;
        }
    }

    printf("\n=======================================================\n") ;
    printf("TOTAL SUMMARY\n") ;
    printf("=======================================================\n") ;
    printf("# TRIALS : %d\n", trials) ;
    printf("# EXEC TIME : %.f ms\n", exec_time_ms) ;
    printf("# LINE COVERED : %d / %d\n", total_coverage, cov_set_len) ;
    printf("# PASS : %d\n", pass_cnt) ;
    printf("# FAIL : %d\n", fail_cnt) ;
    printf("# UNRESOLVED : %d\n", unresolved_cnt) ;
    printf("=======================================================\n") ;
}


///////////////////////////////////// Fuzzer Completion /////////////////////////////////////

void
free_parsed_args ()
{
    for (int i = 0; i < arg_num; i++) {
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
            if (remove(path) == -1) {
                perror("remove_temp_dir: remove") ;
                exit(1) ;
            }
        }
    }
    
    if (rmdir(dir_name) == -1) {
        perror("rmove_temp_dir: rmdir") ;
        exit(1) ;
    }
}

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


///////////////////////////////////// Fuzzer Main /////////////////////////////////////

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

    int total_line_cnt = 0 ;
    int * coverages = 0x0 ;
    char * cov_set ;

    if (is_source) {
        coverages = (int *) malloc(sizeof(int) * trials) ;
        total_line_cnt = get_total_line_cnt(source_path) ;
        cov_set = (char *) malloc(sizeof(char) * total_line_cnt) ; 
        memset(cov_set, '0', total_line_cnt) ;
    }

    double exec_time_ms = fuzzer_loop (return_codes, results, stdout_contents, stderr_contents, coverages, cov_set, total_line_cnt) ;

    fuzzer_summary(return_codes, results, stdout_contents, stderr_contents, coverages, cov_set, total_line_cnt, exec_time_ms) ;

    free(return_codes) ;
    free(results) ;
    free_parsed_args() ;
    free_contents(stdout_contents, stderr_contents) ;
    remove_temp_dir() ;

    if (is_source) {
        remove_files(runargs.binary_path, source_filename) ;
        free(coverages) ;
        free(cov_set) ;
    }
}