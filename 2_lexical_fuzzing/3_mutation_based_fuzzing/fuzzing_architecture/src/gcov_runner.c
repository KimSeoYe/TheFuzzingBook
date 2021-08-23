#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/config.h"
#include "../include/gcov_runner.h"

// #define DEBUG

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
run_gcov (char * source_filename)
{
    char * gcov_args[] = { "/usr/bin/gcov", "-b", source_filename, 0x0 } ;

    if (exec_program("/usr/bin/gcov", gcov_args) != 0) {
        perror("run_gcov: exec_program") ;
        exit(1) ;
    }
}

void 
remove_gcda (char * source_filename)
{
    char copied_filename[PATH_MAX] ;
    strcpy(copied_filename, source_filename) ;

    char gcda_file[PATH_MAX] ;
    char * ptr = strtok(copied_filename, ".") ;
    sprintf(gcda_file, "%s.gcda", ptr) ;

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
get_src_cnts (char * source_filename)
{
    run_gcov(source_filename) ;
    
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

    remove_gcda(source_filename) ;

    return cnt ;
}

coverage_t
read_gcov_file (coverage_t * cov_set, coverage_t src_cnts, char * source_filename) 
{
    char gcov_file[PATH_MAX] ;
    sprintf(gcov_file, "%s.gcov", source_filename) ;
    
    FILE * fp = fopen(gcov_file, "r") ;
    if (fp == 0x0) {
        perror("read_gcov_file: fopen") ;
        exit(1) ;
    }

    coverage_t cov ;
    cov.line = 0 ;
    cov.branch = 0 ;

    int * line_result = (int *) malloc(sizeof(int) * src_cnts.line) ;
    int * branch_result = (int *) malloc(sizeof(int) * src_cnts.branch) ;
    
    char * buf = (char *) malloc(sizeof(char) * LINE_MAX) ;
    size_t line_max = LINE_MAX ;

    for (int branch_num = 0 ; getline(&buf, &line_max, fp) > 0; ) {
        if (strncmp(buf, "branch", 6) == 0) {
            if (strstr(buf, "taken") != 0x0) {
                branch_result[cov.branch++] = branch_num ;
            }
            branch_num++ ;
        }

        char * covered = strtok(buf, ":") ; 
        if (atoi(covered) > 0) { 
            char * line_number = strtok(0x0, ":") ;
            line_result[cov.line++] = atoi(line_number) ;
        }
    }

    for (int i = 0; i < cov.line; i++) {
        cov_set[line_result[i]].line = 1 ;
    }
    for (int i = 0; i < cov.branch; i++) {
        cov_set[branch_result[i]].branch = 1 ;
    }

    free(buf) ;
    fclose(fp) ;
    free(line_result) ;
    free(branch_result) ;

    return cov ;
}

coverage_t
get_coverage (coverage_t * cov_set, coverage_t src_cnts, char * source_filename)
{
    run_gcov(source_filename) ;
    coverage_t coverage = read_gcov_file(cov_set, src_cnts, source_filename) ;
    remove_gcda(source_filename) ;
    
    return coverage ;
}

void 
remove_files (char * executable, char * source_filename) 
{  
    if (remove(executable) == -1) {
        perror("remove_files: remove: executable") ;
    }

    char gcov_file[PATH_MAX] ;
    sprintf(gcov_file, "%s.gcov", source_filename) ;
    if (remove(gcov_file) == -1) {
        perror("remove_files: remove: gcov") ;
    }
    
    char copied_filename[PATH_MAX] ;
    strcpy(copied_filename, source_filename) ;

    char gcno_file[PATH_MAX] ;
    char * ptr = strtok(copied_filename, ".") ;
    sprintf(gcno_file, "%s.gcno", ptr) ;
    if (remove(gcno_file) == -1) {
        perror("remove_files: remove: gcno") ;
    }
}