#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/config.h"
#include "../include/gcov_runner.h"

// #define DEBUG

int
get_total_line_cnt (char * source_path)
{
    int cnt = 0 ;

    FILE * fp = fopen(source_path, "rb") ;
    char * buf = 0x0 ;
    size_t line_max = 0 ;
    while(getline(&buf, &line_max, fp) > 0) {   // Q.
        cnt++ ;
    }
    fclose(fp) ;

    return cnt ;
}

void
get_source_filename (char * dst, char * src)
{
    char copied_src[1024] ;
    strcpy(copied_src, src) ;

    int src_len = strlen(src) ;
    int idx = 0 ;
    char reversed[1024] ;
    for (int i = src_len - 1; i >= 0; i--) {
        if (src[i] == '/') break ;
        reversed[idx++] = src[i] ;
    }
    reversed[idx] = 0x0 ;

    if (reversed[0] != 'c' || reversed[1] != '.') {
        perror("get_source_filename: not a c source file") ;
        exit(1) ;
    }

    int i ;
    for (i = 0; i < src_len; i++) {
        dst[i] = reversed[idx - i - 1] ;
    }
    dst[i] = 0x0 ;
}


void
get_executable_real_path (char * executable_path, char * source_path)
{
    char copied_src[PATH_MAX] ;
    strcpy(copied_src, source_path) ;

    if (copied_src[0] == '.') {
        perror("get_executable_real_path: Source is not a absolute path") ;
        exit(1) ;
    }

    char * tok_ptr = strtok(copied_src, ".") ;
    strcpy(executable_path, tok_ptr) ;
}

int
exec_program (char * program, char ** arguments)
{
    pid_t child_pid = fork() ;

    if (child_pid == 0) {
        if (execv(program, arguments) == -1) {
            perror("exec_program: execv") ;
            exit(1) ;
        }
    }
    else if (child_pid < 0) {
        perror("exec_program: fork") ;
        exit(1) ;
    }

    int exit_code ;
    wait(&exit_code) ;
    
    return exit_code ;
}

void
compile_with_coverage (char * target_path, char * target_path_c)
{
    char * compile_args[] = { "/usr/bin/gcc", "--coverage", "-o", target_path, target_path_c, 0x0 } ;
    
    if (exec_program("/usr/bin/gcc", compile_args) != 0) {
        perror("compile_with_coverage: exec_program") ;
        exit(1) ;
    }
}

void
run_gcov (char * source_path)
{
    char * gcov_args[] = { "/usr/bin/gcov", "-b", source_path, 0x0 } ;

    if (exec_program("/usr/bin/gcov", gcov_args) != 0) {
        perror("run_gcov: exec_program") ;
        exit(1) ;
    }
}

void 
remove_gcda (char * source_path)
{
    char copied_path[PATH_MAX] ;
    strcpy(copied_path, source_path) ;
    copied_path[strlen(source_path) - 2] = 0x0 ;

    char gcda_file[PATH_MAX] ;
    sprintf(gcda_file, "%s.gcda", copied_path) ;
    printf("GCDA: %s\n", gcda_file) ;

    if (access(gcda_file, F_OK) == 0) {
        if (remove(gcda_file) == -1) {
            perror("remove_gcda: remove") ;
        }
    }
    else {
        perror("remove_gcda: access: gcda file not exist") ;
    }
}

coverage_t
get_src_cnts (char * source_path)
{

    run_gcov(source_path) ;
    char source_filename[PATH_MAX] ;
    get_source_filename(source_filename, source_path) ;
    
    char gcov_file[PATH_MAX] ;
    sprintf(gcov_file, "%s.gcov", source_filename) ;

    coverage_t cnt ;
    cnt.line = 0 ;
    cnt.branch = 0 ;

    FILE * fp = fopen(gcov_file, "rb") ;
    char * buf = 0x0 ;
    size_t line_max = 0 ;
    while(getline(&buf, &line_max, fp) > 0) {   // Q.
        if (strncmp(buf, "branch", 6) == 0) cnt.branch++ ;
        else if (strstr(buf, "#####") != 0x0 || atoi(buf) > 0) cnt.line++ ;
    }
    fclose(fp) ;

    return cnt ;
}

int
read_gcov_file (coverage_t * cov, coverage_t * cov_set, int cov_set_len, char * source_path) 
{
    char source_filename[PATH_MAX] ;
    get_source_filename(source_filename, source_path) ;

    char gcov_file[PATH_MAX] ;
    sprintf(gcov_file, "%s.gcov", source_filename) ;
    
    FILE * fp = fopen(gcov_file, "r") ;
    if (fp == 0x0) {
        perror("read_gcov_file: fopen") ;
        exit(1) ;
    }

    cov->line = 0 ;
    cov->branch = 0 ;

    int * line_result = (int *) malloc(sizeof(int) * cov_set_len) ;  
    int * branch_result = (int *) malloc(sizeof(int) * cov_set_len) ;
    
    char * buf = (char *) malloc(sizeof(char) * LINE_MAX) ;
    size_t line_max = LINE_MAX ;

    for (int branch_num = 0 ; getline(&buf, &line_max, fp) > 0; ) {
        if (strncmp(buf, "branch", 6) == 0) {
            if (strstr(buf, "taken") != 0x0) {
                branch_result[cov->branch++] = branch_num ;
            }
            branch_num++ ;
        }

        char * covered = strtok(buf, ":") ; 
        if (atoi(covered) > 0) { 
            char * line_number = strtok(0x0, ":") ;
            line_result[cov->line++] = atoi(line_number) ;
        }
    }

    int is_cov_grow = 0 ;

    /**
     * Question
     * 
     * Currently, it consider both line and branch coverage
     * when update is_cov_grow.
     * Need to separate it?
    */

    for (int i = 0; i < cov->line; i++) {
        if (cov_set[line_result[i]].line == 0) {
            is_cov_grow = 1 ;
        }
        cov_set[line_result[i]].line = 1 ;
    }

    for (int i = 0; i < cov->branch; i++) {
        if (cov_set[branch_result[i]].branch == 0) {
            is_cov_grow = 1 ; 
        }
        cov_set[branch_result[i]].branch = 1 ;
    }

    free(buf) ;
    fclose(fp) ;
    free(line_result) ;
    free(branch_result) ;

    return is_cov_grow ;
}

int
get_coverage (coverage_t * coverage, coverage_t * cov_set, int cov_set_len, char * source_path)
{
    run_gcov(source_path) ;
    int is_cov_grow = read_gcov_file(coverage, cov_set, cov_set_len, source_path) ;
    remove_gcda(source_path) ;
    
    return is_cov_grow ;
}

void 
remove_gcov_file (char * executable, char * source_path) 
{  
    char source_filename[PATH_MAX] ;
    get_source_filename(source_filename, source_path) ;

    char gcov_file[PATH_MAX] ;
    sprintf(gcov_file, "%s.gcov", source_filename) ;
    if (remove(gcov_file) == -1) {
        perror("remove_gcov_file: remove: gcov") ;
    }
}