#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
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
static fuzarg_t fuzargs ;
static runarg_t runargs ;
static covarg_t covargs ;
static int (* oracle) (int return_code, int trial) ;

static char dir_name[RESULT_PATH_MAX] = "" ;  

static char ** parsed_args = 0x0 ; // Q. local var ?
static int * parsed_args_lengths = 0x0 ;
static int cmd_args_num = 2 ;

static char ** seed_filenames ;
static int seed_files_num ;
static int initial_seeds_num ;

static coverage_t accumulated_cov = { 0, } ;
static coverage_t * accumulated_cov_list = 0x0 ;

///////////////////////////////////// Fuzzer Init /////////////////////////////////////

void
print_status ()
{
    printf("=======================================================\n") ;
    printf("FUZZER STATUS\n") ;
    printf("=======================================================\n") ;
    printf("# TRIALS: %d\n", trials) ;
    printf("# FUZZ TYPE (0: RANDOM, 1: MUTATION) : %d\n", fuzz_type) ;
    printf("# FUZZ OPTION (0: STD_IN, 1: ARGUMENT, 2: FILE_CONTENTS): %d\n", fuzz_option) ;
    printf("# FUZZED ARGS NUM: %d\n", fuzzed_args_num) ;
    printf("# FUZARGS\n") ;
    printf("\t- f_min_len: %d / f_max_len: %d\n", fuzargs.f_min_len, fuzargs.f_max_len) ;
    printf("\t- f_char_start: %d / f_char_range: %d\n", fuzargs.f_char_start, fuzargs.f_char_range) ;
    if (fuzz_type == 1) {
        printf("\t- seed_dir: %s\n", fuzargs.seed_dir) ;
    }
    printf("# RUNARGS\n") ;
    printf("\t- binary_path: %s\n", runargs.binary_path) ;
    printf("\t- cmd_args: %s\n", runargs.cmd_args) ;
    printf("\t- timeout: %d\n", runargs.timeout) ;
    printf("# COVARGS\n") ;
    printf("\t- coverage_on: %d\n", covargs.coverage_on) ;
    printf("\t- csv_filename: %s\n", covargs.csv_filename) ;
    printf("\t- source_dir: %s\n", covargs.source_dir) ;
    printf("\t- source_num: %d\n", covargs.source_num) ;
    for (int i = 0; i < covargs.source_num; i++) {
        printf("\t---> [%d] %s\n", i, covargs.source_paths[i]) ;
    }
    printf("=======================================================\n\n") ;
}

void
copy_status (test_config_t * config)
{
    trials = config->trials ;

    fuzz_type = config->fuzz_type ;
    if (fuzz_type == MUTATION) {
        if (realpath(config->fuzargs.seed_dir, fuzargs.seed_dir) == 0x0) {
            perror("copy_status: realpath: fuzargs.seed_dir") ;
            exit(1) ;
        }

        struct stat st_seed_dir ;
        if (stat(fuzargs.seed_dir, &st_seed_dir) == -1) {
            perror("copy_status: stat") ;
            exit(1) ;
        }

        if (!S_ISDIR(st_seed_dir.st_mode)) {
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

    if (strcmp(config->runargs.binary_path, "") != 0) { // MODIFIED HERE
        if (realpath(config->runargs.binary_path, runargs.binary_path) == 0x0) {
            perror("copy_status: realpath: runargs.binary_path") ;
            exit(1) ;
        }  
    }   

    covargs.coverage_on = config->covargs.coverage_on ;
    covargs.source_num = config->covargs.source_num ;

    if (realpath(config->covargs.source_dir, covargs.source_dir) == 0x0) {
        perror("copy_status: realpath: covargs.source_dir") ;
        exit(1) ;
    }  

    covargs.source_paths = (char **) malloc(sizeof(char *) * covargs.source_num) ;
    for (int i = 0; i < covargs.source_num; i++) {
        covargs.source_paths[i] = (char *) malloc(sizeof(char) * PATH_MAX) ;
        if (realpath(config->covargs.source_paths[i], covargs.source_paths[i]) == 0x0) {
            perror("copy_status: realpath: covargs.source_paths[i]") ;
            exit(1) ;
        }
    }
    strcpy(covargs.csv_filename, config->covargs.csv_filename) ;

    strcpy(runargs.cmd_args, config->runargs.cmd_args) ;
    runargs.timeout = config->runargs.timeout ;

    oracle = config->oracle ; 

    print_status() ;
}

void
parse_args ()
{   
    parsed_args = (char **) malloc(sizeof(char *) * ARG_N_MAX) ;    // Q.
    parsed_args_lengths = (int *) malloc(sizeof(int) * ARG_N_MAX) ;

    parsed_args[0] = (char *) malloc(sizeof(char) * (strlen(runargs.binary_path) + 1)) ;
    parsed_args_lengths[0] = strlen(runargs.binary_path) ;
    strncpy(parsed_args[0], runargs.binary_path, parsed_args_lengths[0]) ;
    parsed_args[0][parsed_args_lengths[0]] = 0x0 ;

    int i ;
    char * tok_ptr = strtok(runargs.cmd_args, " ") ; 
    for (i = 1; tok_ptr != 0x0; i++) { 
        cmd_args_num++ ;
        parsed_args[i] = (char *) malloc(sizeof(char) * (strlen(tok_ptr) + 1)) ;
        parsed_args_lengths[i] = strlen(tok_ptr) ;
        strcpy(parsed_args[i], tok_ptr) ;
        parsed_args[i][parsed_args_lengths[i]] = 0x0 ;

        tok_ptr = strtok(0x0, " ") ;
    }
    
    parsed_args[i] = (char *) malloc(sizeof(char) * 1) ;
    parsed_args_lengths[i] = 0 ;
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
    strncpy(dir_name, temp_dir, strlen(temp_dir)) ;
    dir_name[strlen(temp_dir)] = 0x0 ;
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
            seed_filenames = realloc(seed_filenames, sizeof(char *) * (num_files / SEED_CNT_MAX + 1) * SEED_CNT_MAX) ;
            if (seed_filenames == 0x0) {
                perror("read_seed_dir: realloc") ;
                break ;
            }
        } 

        if (entry->d_name[0] != '.') {  // TODO. more condition ?
            seed_filenames[num_files] = (char *) malloc(sizeof(char) * (strlen(entry->d_name) + 1)) ;
            strncpy(seed_filenames[num_files], entry->d_name, strlen(entry->d_name)) ;
            seed_filenames[num_files][strlen(entry->d_name)] = 0x0 ;
            num_files++ ;
        }
    }
    seed_files_num = num_files ;
    initial_seeds_num = num_files ;

    closedir(dir_ptr) ;
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

    if (covargs.coverage_on) {
        for (int i = 0; i < covargs.source_num; i++) {
            if (access(covargs.source_paths[i], R_OK) == -1) {
                perror("fuzzer_init: access: cannot access to the source path") ;
                exit(1) ;
            }
        }
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
        if (parsed_args[i] != 0x0) printf("parsed_args[%d] %s(%d)\n", i, parsed_args[i], parsed_args_lengths[i]) ;
        else printf("parsed_args[%d] 0x0\n", i) ;
    }
#endif

    if (fuzz_type == MUTATION) read_seed_dir() ;

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
    char in_path[RESULT_PATH_MAX] = "" ;
    get_path(in_path, trial, 0) ;

    if (input_len >= CONTENTS_MAX - 1) {
        memcpy(contents.input_contents[trial], input, CONTENTS_MAX - 1) ;
        contents.input_contents[trial][CONTENTS_MAX - 1] = 0x0 ;
    }
    else {
        memcpy(contents.input_contents[trial], input, input_len) ;
        contents.input_contents[trial][input_len] = 0x0 ;
    }

    FILE * in_fp = fopen(in_path, "wb") ;
    if (in_fp == 0x0) {
        perror("fopen") ;
        exit(1) ;
    }

    if (fwrite(input, 1, input_len, in_fp) != input_len) {  
        perror("fwrite: write_input_files") ;
    }

    fclose(in_fp) ;
}

void 
execute_target(content_t contents, char * input, int input_len, int trial)
{
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
    char path[RESULT_PATH_MAX] = "" ;
    get_path(path, trial, fd) ;

    FILE * fp = fopen(path, "wb") ;
    if (fp == 0x0) {
        perror("fopen: write_output_files") ;
        exit(1) ;
    }

    char buf[1024] = "" ;
    int s = 0 ;
    int is_first = 1 ;
    
    if (fd == 1) {
        while ((s = read(stdout_pipes[0], buf, 1024)) > 0) {
            if (is_first) {
                if (s >= CONTENTS_MAX - 1) {
                    memcpy(contents.stdout_contents[trial], buf, CONTENTS_MAX - 1) ;
                    contents.stdout_contents[trial][CONTENTS_MAX - 1] = 0x0 ;
                }
                else {
                    memcpy(contents.stdout_contents[trial], buf, s) ;
                    contents.stdout_contents[trial][s] = 0x0 ;
                }
                is_first = 0 ;
            }
            if (fwrite(buf, 1, s, fp) != s) {
                perror("fwrite: save_results: stdout") ;
            }
        }
        close(stdout_pipes[0]) ;
    }
    else if (fd == 2) {
        while ((s = read(stderr_pipes[0], buf, 1024)) > 0) {
            if (is_first) {
                if (s >= CONTENTS_MAX - 1) {
                    memcpy(contents.stderr_contents[trial], buf, CONTENTS_MAX - 1) ;
                    contents.stderr_contents[trial][CONTENTS_MAX - 1] = 0x0 ;
                }
                else {
                    memcpy(contents.stderr_contents[trial], buf, s) ;
                    contents.stderr_contents[trial][s] = 0x0 ;
                }
                is_first = 0 ;
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
    close(stdin_pipes[0]) ;
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

    if (fuzz_option == STD_IN) write_input_files(contents, input, input_len, trial) ;

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
write_fuzzed_args_to_file (FILE * fp)
{
    for (int i = 1; i <= fuzzed_args_num; i++) {
        if (fwrite(parsed_args[cmd_args_num - i], 1, parsed_args_lengths[cmd_args_num - i], fp) !=  parsed_args_lengths[cmd_args_num - i]) {
            perror("write_input_ars_file: fwrite") ;
        }
        if (fwrite(" ", 1, 1, fp) != 1) {     
            perror("write_input_ars_file: fwrite") ;
        }
    }
}

void
write_input_args_file (content_t contents, int trial)
{
    char in_path[RESULT_PATH_MAX] = "" ;
    get_path(in_path, trial, 0) ;

    if (parsed_args_lengths[cmd_args_num - 1] >= CONTENTS_MAX - 1) { 
        memcpy(contents.input_contents[trial], parsed_args[cmd_args_num - 1], CONTENTS_MAX - 1) ;  
        contents.input_contents[trial][CONTENTS_MAX - 1] = 0x0 ;
    }
    else {
        memcpy(contents.input_contents[trial], parsed_args[cmd_args_num - 1], parsed_args_lengths[cmd_args_num - 1]) ;
        contents.input_contents[trial][parsed_args_lengths[cmd_args_num - 1]] = 0x0 ;
    }

    FILE * in_fp = fopen(in_path, "wb") ;
    if (in_fp == 0x0) {
        perror("fopen") ;
        exit(1) ;
    }

    write_fuzzed_args_to_file (in_fp) ;

    fclose(in_fp) ;
}

void
fuzz_argument (content_t contents, fuzarg_t * fuzargs, int trial)
{
    int i ;
    for (i = cmd_args_num - 1; i < cmd_args_num + fuzzed_args_num - 1; i++) {
        int args_size = fuzargs->f_max_len + 1 ; // TODO. mutation ?
        parsed_args[i] = (char *) malloc(sizeof(char) * args_size) ; 
    
        int is_initial = 0 ;
        switch (fuzz_type) {
            case RANDOM: 
                parsed_args_lengths[i] = fuzz_input(fuzargs, parsed_args[i]) ;
                break ;
            case MUTATION:
                if (trial < initial_seeds_num) is_initial = 1 ;
                parsed_args_lengths[i] = mutate_input(&parsed_args[i], args_size, fuzargs, seed_filenames[trial % seed_files_num], is_initial) ;   
                break ;
        }
    }
    parsed_args[i] = 0x0 ;
    parsed_args_lengths[i] = 0 ;

#ifdef DEBUG
    printf("TOTAL ARGS NUM: %d\n", cmd_args_num + fuzzed_args_num) ;
    printf("ARGS AFTER FUZZ\n") ;
    for (i = 0; i < cmd_args_num + fuzzed_args_num; i++) {
        printf("[%d] %s(%d)\n", i, parsed_args[i], parsed_args_lengths[i]) ;
    }
#endif

    write_input_args_file (contents, trial) ;
}

void
get_accumulated_covs (covset_t * cov_sets, int trial)
{
    int total_line_coverage = 0 ;
    int total_branch_coverage = 0 ;

    for (int i = 0; i < covargs.source_num; i++) {
        for (int t = 0; t < cov_sets[i].len; t++) {
            total_line_coverage += cov_sets[i].set[t].line ;
            total_branch_coverage += cov_sets[i].set[t].branch ;
        }
    }

    accumulated_cov_list[trial].line = total_line_coverage ;
    accumulated_cov_list[trial].branch = total_branch_coverage ;
}


void
update_corpus (char * input, int input_len)
{
    char new_seed_file[PATH_MAX] = "" ;
    int rand_num = rand() % 1000000 ;
    sprintf(new_seed_file, "mutated_%d", rand_num) ;
    int new_filename_len = strlen(new_seed_file) ;

    if (seed_files_num != 0 && seed_files_num % SEED_CNT_MAX == 0) {
        seed_filenames = realloc(seed_filenames, sizeof(char *) * (seed_files_num / SEED_CNT_MAX + 1) * SEED_CNT_MAX) ;
        if (seed_filenames == 0x0) {
            perror("update_corpus: realloc") ;
            exit(1) ;
        }
    }

    seed_filenames[seed_files_num] = (char *) malloc(sizeof(char) * (new_filename_len + 1)) ;
    strncpy(seed_filenames[seed_files_num], new_seed_file, new_filename_len) ;
    seed_filenames[seed_files_num][new_filename_len] = 0x0 ;
    seed_files_num++ ;

    char new_seed_path[PATH_MAX] = "" ;
    strncpy(new_seed_path, fuzargs.seed_dir, strlen(fuzargs.seed_dir)) ;
    new_seed_path[strlen(fuzargs.seed_dir)] = 0x0 ;
    if (new_seed_path[strlen(new_seed_path) - 1] != '/') {
        strcat(new_seed_path, "/") ;
    }
    strcat(new_seed_path, new_seed_file) ;

    FILE * fp = fopen(new_seed_path, "wb") ;
    if (fp == 0x0) {
        perror("update_corpus: fopen") ;
        exit(1) ;
    }

    if (fuzz_option == STD_IN) {
        if (fwrite(input, 1, input_len, fp) != input_len) {
            perror("write_mutated_input: fwrite") ;
        }
    }
    else if (fuzz_option == ARGUMENT) {
        write_fuzzed_args_to_file(fp) ;
    }

    fclose(fp) ;
}

double
fuzzer_loop (int * return_codes, result_t * results, content_t contents, covset_t * cov_sets) 
{
    char * input = (char *) malloc(sizeof(char) * BUF_PAGE_UNIT) ;    

    clock_t start = clock() ;
    
    for (int i = 0; i < trials; i++) {
        int input_len = 0 ;
        int is_initial = 0 ;

        if (fuzz_option == STD_IN) {
            switch (fuzz_type) {
                case RANDOM: 
                    input_len = fuzz_input(&fuzargs, input) ;   // TODO. input as a first param.
                    break ;
                case MUTATION:
                    if (i < initial_seeds_num) is_initial = 1 ;
                    input_len = mutate_input(&input, BUF_PAGE_UNIT, &fuzargs, seed_filenames[i % seed_files_num], is_initial) ;
                    break ;
            }
        }
        else if (fuzz_option == ARGUMENT) fuzz_argument(contents, &fuzargs, i) ;

        return_codes[i] = run(contents, input, input_len, i) ;

        int is_cov_grow = 0 ;
        if (covargs.coverage_on) {
            coverage_t cov ;
            is_cov_grow = get_coverage(&cov, cov_sets, &covargs) ; // TODO. covargs to pointer

            get_accumulated_covs(cov_sets, i) ;

            if (is_cov_grow && fuzz_type == MUTATION) { // TODO. if not mutation
                update_corpus(input, input_len) ;
            }
        }

        results[i] = oracle_run(return_codes[i], i) ;
    }

    clock_t end = clock() ;
    double exec_time = (double) end - start ;
    
    free(input) ;

    return exec_time ;
}

///////////////////////////////////// Fuzzer Summary /////////////////////////////////////

void
write_csv_file (coverage_t total_src_cnts)
{
#ifdef DEBUG
    printf("\n\nACCUMULATED RESULT : LINE\n") ;
    for (int i = 0; i < trials; i++) {
        printf("%d ", accumulated_cov_list[i].line) ;
    }
    printf("\n\nACCUMULATED RESULT : BRANCH\n") ;
    for (int i = 0; i < trials; i++) {
        printf("%d ", accumulated_cov_list[i].branch) ;
    }
#endif

    FILE * fp = fopen(covargs.csv_filename, "ab") ;
    if (fp == 0x0) {
        perror("write_csv_file: fopen") ;
    }

    fprintf(fp, "line_cov") ;
    for (int i = 0; i < trials; i++) {
        fprintf(fp, ",%f", (double)accumulated_cov_list[i].line / total_src_cnts.line * 100) ;
    }
    fprintf(fp, "\nbranch_cov") ;
    for (int i = 0; i < trials; i++) {
        fprintf(fp, ",%f", (double)accumulated_cov_list[i].branch / total_src_cnts.branch * 100) ;
    }
    fprintf(fp, "\n") ;

    fclose(fp) ;
}

void
fuzzer_summary (int * return_codes, result_t * results, content_t contents, covset_t * cov_sets, double exec_time)
{
    int pass_cnt = 0 ;
    int fail_cnt = 0 ;
    int unresolved_cnt = 0 ;

    coverage_t total_src_cnts ;
    total_src_cnts.line = 0 ;
    total_src_cnts.branch = 0 ;

    if (covargs.coverage_on) {
        for (int i = 0; i < covargs.source_num; i++) {
            coverage_t src_cnts = get_src_cnts(covargs.source_paths[i], &covargs) ;
            total_src_cnts.line += src_cnts.line ;
            total_src_cnts.branch += src_cnts.branch ;
        }
    }

    for (int i = 0; i < trials; i++) {
        printf("(CompletedProcess(target='%s', args='%s', ", runargs.binary_path, runargs.cmd_args) ;
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

    if (covargs.coverage_on) {
        for (int i = 0; i < covargs.source_num; i++) {
            for (int t = 0; t < cov_sets[i].len; t++) {
                total_line_coverage += cov_sets[i].set[t].line ;
                total_branch_coverage += cov_sets[i].set[t].branch ;
            }
        }
    }
    
    printf("\n=======================================================\n") ;
    printf("TOTAL SUMMARY\n") ;
    printf("=======================================================\n") ;
    printf("# TRIALS : %d\n", trials) ;
    printf("# EXEC TIME : %.3f s\n", exec_time / CLOCKS_PER_SEC) ;
    if (covargs.coverage_on) {
        printf("# LINE COVERED : %d / %d = %.1f%%\n", total_line_coverage, total_src_cnts.line, (double)total_line_coverage * 100.0 / total_src_cnts.line) ;
        printf("# BRANCH COVERED : %d / %d = %.1f%%\n", total_branch_coverage, total_src_cnts.branch, (double)total_branch_coverage * 100.0 / total_src_cnts.branch) ;
    }
    printf("# PASS : %d\n", pass_cnt) ;
    printf("# FAIL : %d\n", fail_cnt) ;
    printf("# UNRESOLVED : %d\n", unresolved_cnt) ;
    printf("=======================================================\n") ;

    if (covargs.coverage_on) {
        write_csv_file(total_src_cnts) ;
    }
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
            char path[RESULT_PATH_MAX] = "" ;
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

void
free_source_paths ()
{
    for (int i = 0; i < covargs.source_num; i++) {
        free(covargs.source_paths[i]) ;
    }
    free(covargs.source_paths) ;
}

void
reset_seeds ()
{
    for (int i = 0; i < seed_files_num; i++) {
        if (strncmp(seed_filenames[i], "mutated_", 8) == 0) {
            char seed_path[PATH_MAX] ;
            get_seed_path(seed_path, fuzargs.seed_dir, seed_filenames[i]) ; 
            if (remove(seed_path) == -1) {
                perror("reset_seeds: remove") ;
            }
        }
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

    covset_t * cov_sets ;

    if (covargs.coverage_on) {
        accumulated_cov_list = (coverage_t *) malloc(sizeof(coverage_t) * trials) ;

        cov_sets = (covset_t *) malloc(sizeof(covset_t) * covargs.source_num) ;

        for (int i = 0; i < covargs.source_num; i++) {
            strncpy(cov_sets[i].filepath, covargs.source_paths[i], strlen(covargs.source_paths[i])) ;
            cov_sets[i].filepath[strlen(covargs.source_paths[i])] = 0x0 ;
            
            cov_sets[i].len = get_total_line_cnt(covargs.source_paths[i]) ;
            cov_sets[i].set = (coverage_t *) malloc(sizeof(coverage_t) * cov_sets[i].len) ;

            memset(cov_sets[i].set, 0, sizeof(coverage_t) * cov_sets[i].len) ;
        }        
    }

    double exec_time = fuzzer_loop(return_codes, results, contents, cov_sets) ;
    fuzzer_summary(return_codes, results, contents, cov_sets, exec_time) ;

    if (covargs.coverage_on) {  
        for (int i = 0; i < covargs.source_num; i++) {
            remove_gcov_file(runargs.binary_path, covargs.source_paths[i]) ;
            free(cov_sets[i].set) ;
        }
        free(cov_sets) ;
    }
    else {
        if (covargs.source_num != 0) {
            for (int i = 0; i < covargs.source_num; i++) {
                remove_gcda(covargs.source_paths[i]) ;
            }
        }
    }

    if (fuzz_type == MUTATION) {
        free_seed_filenames() ;
        if (covargs.coverage_on) {
            // reset_seeds() ;
        }
    }

    if (covargs.source_num != 0) free_source_paths() ;

    free(return_codes) ;
    free(results) ;
    free_parsed_args() ;
    free(parsed_args_lengths) ;
    remove_temp_dir() ;  
}