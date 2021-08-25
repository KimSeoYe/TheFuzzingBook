#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "../include/fuzzer.h"
#include "../include/fuzz_input.h"
#include "../include/gcov_runner.h"
#include "../include/mutate.h"

// #define DEBUG

///////////////////////////////////// Fuzzer Status /////////////////////////////////////

# define SEED_CNT_MAX 512

static int trials ;
fuztype_t fuzz_type ;
fuzopt_t fuzz_option ;
static int fuzzed_args_num = 0 ;
static int is_source ;
static char source_path[PATH_MAX] ;
static char source_filename[PATH_MAX] ;
static fuzarg_t fuzargs ;
static runarg_t runargs ;
static int (* oracle) (int return_code, int trial) ;

// Q. static global ?
static char dir_name[RESULT_PATH_MAX] ;  

static char ** parsed_args ; // Q. local var ?
static int cmd_args_num = 2 ;

static char ** seed_filenames ;
static int seed_files_num ;

///////////////////////////////////// Fuzzer Init /////////////////////////////////////

void
copy_status (test_config_t * config)
{
    trials = config->trials ;

    fuzz_type = config->fuzz_type ;
    if (fuzz_type == MUTATION) {
        struct stat st_seed_dir ;
        if (stat(config->fuzargs.seed_dir, &st_seed_dir) == -1) {
            perror("copy_status: stat") ;
            exit(1) ;
        }

        if (S_ISDIR(st_seed_dir.st_mode)) {
            strcpy(fuzargs.seed_dir, config->fuzargs.seed_dir) ;
        }
        else {
            perror("copy_status: S_ISDIR: Invalid seed_dir") ;
            exit(1) ;
        }
    }

    fuzz_option = config->fuzz_option ;
    if (fuzz_option == ARGUMENT) {
        fuzzed_args_num = config->fuzzed_args_num ;
        if (fuzzed_args_num == 0) {
            perror("copy_status: Invalid fuzzed_args_num for fuzz_option ARGUMENT") ;
            exit(1) ;
        }
    }

    fuzargs.f_max_len = config->fuzargs.f_max_len ;
    fuzargs.f_min_len = config->fuzargs.f_min_len ;

    fuzargs.f_char_start = config->fuzargs.f_char_start ;
    fuzargs.f_char_range = config->fuzargs.f_char_range ;

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

    oracle = config->oracle ; 
}

void
parse_args ()
{   
    parsed_args = (char **) malloc(sizeof(char *) * ARG_N_MAX) ;

    parsed_args[0] = (char *) malloc(sizeof(char) * (strlen(runargs.binary_path) + 1)) ;
    strcpy(parsed_args[0], runargs.binary_path) ;

    int i ;
    char * tok_ptr = strtok(runargs.cmd_args, " ") ; 
    for (i = 1; tok_ptr != NULL; i++) { 
        cmd_args_num++ ;
        parsed_args[i] = (char *) malloc(sizeof(char) * (strlen(tok_ptr) + 1)) ;
        strcpy(parsed_args[i], tok_ptr) ;

        tok_ptr = strtok(0x0, " ") ;
    }
    
    parsed_args[i] = (char *) malloc(sizeof(char) * 1) ;
    parsed_args[i] = 0x0 ;
}

int
default_oracle (int return_codes, int trial)
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
read_seed_dir ()
{
    int num_files = 0 ;

    DIR * dir_ptr = 0x0 ;
    struct dirent * entry = 0x0 ;

    if ((dir_ptr = opendir(fuzargs.seed_dir)) == 0x0) {
        perror("read_seed_dir: opendir") ;
        exit(1) ;
    }

    seed_filenames = (char **) malloc(sizeof(char *) * SEED_CNT_MAX) ;

    while ((entry = readdir(dir_ptr)) != 0x0) {
        if (num_files != 0 && num_files % SEED_CNT_MAX == 0) {
            seed_filenames = realloc(seed_filenames, sizeof(char *) * (num_files / SEED_CNT_MAX + 1)) ;
            if (seed_filenames == 0x0) {
                perror("read_seed_dir: realloc") ;
                break ;
            }
        }

        if (entry->d_name[0] != '.') {  // TODO. more condition ?
            seed_filenames[num_files] = (char *) malloc(sizeof(char) * (strlen(entry->d_name) + 1)) ;
            strcpy(seed_filenames[num_files], entry->d_name) ;
            num_files++ ;
        }
    }
    seed_files_num = num_files ;

    closedir(dir_ptr) ;
}

void
fuzzer_init (test_config_t * config)
{
    copy_status(config) ;

    if (fuzz_type == MUTATION) read_seed_dir() ;

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
    for (int i = 0; i < cmd_args_num; i++) {
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
write_input_files (content_t contents, char * input, int input_len, int trial)
{
    char in_path[RESULT_PATH_MAX] ;
    get_path(in_path, trial, 0) ;
    printf("[DEBUG] %d get_path : %s\n", __LINE__, in_path) ;

    if (input_len >= CONTENTS_MAX - 1) {
        memcpy(contents.input_contents[trial], input, CONTENTS_MAX - 1) ;
        contents.input_contents[trial][CONTENTS_MAX - 1] = 0x0 ;
    }
    else {
        memcpy(contents.input_contents[trial], input, input_len) ;
    }

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
execute_target(content_t contents, char * input, int input_len, int trial)
{
    if (fuzz_option == STD_IN) write_input_files(contents, input, input_len, trial) ;

    alarm(runargs.timeout) ;

    if (fuzz_option == STD_IN) {
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
write_output_files (content_t contents, int trial, int fd){
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
            if (s >= CONTENTS_MAX - 1) {
                memcpy(contents.stdout_contents[trial], buf, CONTENTS_MAX - 1) ;
                contents.stdout_contents[trial][CONTENTS_MAX - 1] = 0x0 ;
            }
            else {
                memcpy(contents.stdout_contents[trial], buf, s) ;
            }
            

            if (fwrite(buf, 1, s, fp) != s) {
                perror("fwrite: save_results: stdout") ;
            }
        }
        close(stdout_pipes[0]) ;
    }
    else if (fd == 2) {
        while ((s = read(stderr_pipes[0], buf, 1024)) > 0) {
            if (s >= CONTENTS_MAX - 1) {
                memcpy(contents.stderr_contents[trial], buf, CONTENTS_MAX - 1) ;
                contents.stderr_contents[trial][CONTENTS_MAX - 1] = 0x0 ;
            }
            else {
                memcpy(contents.stderr_contents[trial], buf, s) ;
            }

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
save_results(content_t contents, int trial)
{
    close(stdin_pipes[1]) ;
    close(stdout_pipes[1]) ;
    close(stderr_pipes[1]) ;
    write_output_files(contents, trial, 1) ;
    write_output_files(contents, trial, 2) ;
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
run (content_t contents, char * input, int input_len, int trial)
{    
    if (pipe(stdin_pipes) != 0) goto pipe_err ;
    if (pipe(stdout_pipes) != 0) goto pipe_err ;
    if (pipe(stderr_pipes) != 0) goto pipe_err ;

    child_pid = fork() ;
    if (child_pid == 0) {
        execute_target(contents, input, input_len, trial) ;
    }
    else if (child_pid > 0) {
        save_results(contents, trial) ;
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
write_input_args_file (content_t contents, int first_input_len, int trial)
{
    char in_path[RESULT_PATH_MAX] ;
    get_path(in_path, trial, 0) ;

    if (strlen(parsed_args[cmd_args_num - 1]) >= CONTENTS_MAX - 1) {  // TODO. strlen, strcpy
        strncpy(contents.input_contents[trial], parsed_args[cmd_args_num - 1], CONTENTS_MAX - 1) ;  
        contents.input_contents[trial][CONTENTS_MAX - 1] = 0x0 ;
    }
    else {
        strcpy(contents.input_contents[trial], parsed_args[cmd_args_num - 1]) ;
    }

    FILE * in_fp = fopen(in_path, "wb") ;
    if (in_fp == 0x0) {
        perror("fopen") ;
        exit(1) ;
    }

    if (fwrite(parsed_args[cmd_args_num - 1], 1, first_input_len, in_fp) != first_input_len) {  
        perror("write_input_ars_file: fwrite") ;
    }

    fclose(in_fp) ;
}

void
fuzz_argument (content_t contents, fuzarg_t * fuzargs, int trial)
{
    int first_input_len ;

    int i ;
    for (i = cmd_args_num - 1; i < cmd_args_num + fuzzed_args_num - 1; i++) {
        parsed_args[i] = (char *) malloc(sizeof(char) * (fuzargs->f_max_len + 1)) ;
        switch (fuzz_type) {
            case RANDOM: 
                first_input_len = fuzz_input(fuzargs, parsed_args[i]) ;
                break ;
            case MUTATION:
                first_input_len = mutate_input(parsed_args[i], fuzargs, seed_filenames[trial % seed_files_num]) ;
                break ;
        }
    }
    parsed_args[i] = 0x0 ;

#ifdef DEBUG
    printf("TOTAL ARGS NUM: %d\n", cmd_args_num + fuzzed_args_num) ;
    printf("ARGS AFTER FUZZ\n") ;
    for (i = 0; i < cmd_args_num + fuzzed_args_num; i++) {
        printf("[%d] %s\n", i, parsed_args[i]) ;
    }
#endif

    write_input_args_file (contents, first_input_len, trial) ;
}

double
fuzzer_loop (int * return_codes, result_t * results, content_t contents, coverage_t * coverages, coverage_t * cov_set, coverage_t src_cnts) 
{
    clock_t start = clock() ;

    char * input = (char *) malloc(sizeof(char) * (fuzargs.f_max_len + 1)) ;    // TODO -> mutation ?
    
    for (int i = 0; i < trials; i++) {
        int input_len = 0 ;
        if (fuzz_option == STD_IN) {
            switch (fuzz_type) {
                case RANDOM: 
                    input_len = fuzz_input(&fuzargs, input) ;
                    break ;
                case MUTATION:
                    input_len = mutate_input(input, &fuzargs, seed_filenames[i % seed_files_num]) ;
                    break ;
            }
        }
        else if (fuzz_option == ARGUMENT) fuzz_argument(contents, &fuzargs, i) ;

        return_codes[i] = run(contents, input, input_len, i) ;
        printf("[DEBUG] %d run end w/ return code %d\n", i, return_codes[i]) ;

        if (is_source) {
            coverage_t cov = get_coverage(cov_set, src_cnts, source_filename) ;
            printf("[DEBUG] %d get_coverage end w/ ", i) ;
            coverages[i].line = cov.line ;
            coverages[i].branch = cov.branch ;
            printf("line %d, branch %d\n", coverages[i].line, coverages[i].branch) ;
        }

        results[i] = oracle_run(return_codes[i], i) ;
    }

    clock_t end = clock() ;
    double exec_time_ms = (double) end - start ;
    
    free(input) ;

    return exec_time_ms ;
}

///////////////////////////////////// Fuzzer Summary /////////////////////////////////////

void
fuzzer_summary (int * return_codes, result_t * results, content_t contents, coverage_t * coverages, coverage_t * cov_set, int cov_set_len, coverage_t src_cnts, double exec_time_ms)
{
    int pass_cnt = 0 ;
    int fail_cnt = 0 ;
    int unresolved_cnt = 0 ;

    for (int i = 0; i < trials; i++) {
        printf("(CompletedProcess(target='%s', args='%s', ", runargs.binary_path, runargs.cmd_args) ;
        if (is_source) printf("line_coverage='%d', branch_coverage='%d', ", coverages[i].line, coverages[i].branch) ;
        printf("returncode='%d', input='%s', stdout='%s', stderr='%s', result='%s'))\n", return_codes[i], contents.input_contents[i], contents.stdout_contents[i], contents.stderr_contents[i], result_strings[results[i]]) ;
        
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

    int total_line_coverage = 0 ;
    int total_branch_coverage = 0 ;
    if (is_source) {
        for (int i = 0; i < cov_set_len; i++) {
            total_line_coverage += cov_set[i].line ;
            total_branch_coverage += cov_set[i].branch ;
        }
        printf("\n") ;
    }

    printf("\n=======================================================\n") ;
    printf("TOTAL SUMMARY\n") ;
    printf("=======================================================\n") ;
    printf("# TRIALS : %d\n", trials) ;
    printf("# EXEC TIME : %.f ms\n", exec_time_ms) ;
    if (is_source) {
        printf("# LINE COVERED : %d / %d = %.1f%%\n", total_line_coverage, src_cnts.line, (double)total_line_coverage * 100.0 / src_cnts.line) ;
        printf("# BRANCH COVERED : %d / %d = %.1f%%\n", total_branch_coverage, src_cnts.branch, (double)total_branch_coverage * 100.0 / src_cnts.branch) ;
    }
    printf("# PASS : %d\n", pass_cnt) ;
    printf("# FAIL : %d\n", fail_cnt) ;
    printf("# UNRESOLVED : %d\n", unresolved_cnt) ;
    printf("=======================================================\n") ;
}


///////////////////////////////////// Fuzzer Completion /////////////////////////////////////

void
free_parsed_args ()
{
    for (int i = 0; i < cmd_args_num + fuzzed_args_num; i++) {
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
            }
        }
    }
    
    if (rmdir(dir_name) == -1) {
        perror("rmove_temp_dir: rmdir") ;
        exit(1) ;
    }
}

void
free_contents (content_t contents)
{
    for (int i = 0; i < trials; i++) {
        free(contents.input_contents[i]) ;
        free(contents.stdout_contents[i]) ;
        free(contents.stderr_contents[i]) ;
    }
    free(contents.input_contents) ;
    free(contents.stdout_contents) ;
    free(contents.stderr_contents) ;
}

void
free_seed_filenames ()
{
    for (int i = 0; i < seed_files_num; i++) {
        free(seed_filenames[i]) ;
    }
    free(seed_filenames) ;
}


///////////////////////////////////// Fuzzer Main /////////////////////////////////////

void
allocate_contents (content_t * contents)
{
    contents->input_contents = (char **) malloc(sizeof(char *) * trials) ;
    contents->stdout_contents = (char **) malloc(sizeof(char *) * trials) ;
    contents->stderr_contents = (char **) malloc(sizeof(char *) * trials) ;
    for (int i = 0; i < trials; i++) {
        contents->input_contents[i] = (char *) malloc(sizeof(char) * CONTENTS_MAX) ;
        contents->stdout_contents[i] = (char *) malloc(sizeof(char) * CONTENTS_MAX) ;
        contents->stderr_contents[i] = (char *) malloc(sizeof(char) * CONTENTS_MAX) ;
    }
}

void
fuzzer_main (test_config_t * config)
{
    srand(time(NULL)) ;
    signal(SIGALRM, timeout_handler) ;
    
    fuzzer_init(config) ;

    int * return_codes = (int *) malloc(sizeof(int) * trials) ;
    result_t * results = (result_t *) malloc(sizeof(result_t) * trials) ;
    
    content_t contents ;
    allocate_contents(&contents) ;

    coverage_t * coverages ;
    coverage_t * cov_set ;
    int cov_set_len ;
    coverage_t src_cnts ;

    if (is_source) {
        coverages = (coverage_t *) malloc(sizeof(coverage_t) * trials) ;

        src_cnts = get_src_cnts(source_filename) ;
        if (src_cnts.line >= src_cnts.branch) cov_set_len = src_cnts.line ;
        else cov_set_len = src_cnts.branch ;

        cov_set = (coverage_t *) malloc(sizeof(coverage_t) * cov_set_len) ;
        memset(cov_set, 0, sizeof(coverage_t) * cov_set_len) ;
    }

    double exec_time_ms = fuzzer_loop (return_codes, results, contents, coverages, cov_set, src_cnts) ;
    fuzzer_summary(return_codes, results, contents, coverages, cov_set, cov_set_len, src_cnts, exec_time_ms) ;

    if (is_source) {
        remove_files(runargs.binary_path, source_filename) ;
        free(coverages) ;
        free(cov_set) ;
    }

    free(return_codes) ;
    free(results) ;
    free_parsed_args() ;
    free_seed_filenames() ;
    remove_temp_dir() ;  
}