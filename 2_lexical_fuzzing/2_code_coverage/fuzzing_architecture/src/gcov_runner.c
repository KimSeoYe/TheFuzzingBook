#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/config.h"
#include "../include/gcov_runner.h"

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
    else if (child_pid == 0x0) {
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
    char * compile_args[] = { "gcc", "--coverage", "-o", target_path, target_path_c, 0x0 } ;

    if (exec_program("/usr/bin/gcc", compile_args) != 0) {
        perror("compile_with_coverage: exec_program") ;
        exit(1) ;
    }
}

void
run_gcov (char * source_filename)
{
    char * gcov_args[] = { "gcov", source_filename, 0x0 } ;

    if (exec_program("/usr/bin/gcov", gcov_args) != 0) {
        perror("run_gcov: exec_program") ;
        exit(1) ;
    }
}

void
read_gcov_file (coverset_t * coverage, char * source_filename) 
{
    char gcov_file[PATH_MAX] ;
    sprintf(gcov_file, "%s.gcov", source_filename) ;

    int cov_idx = 0 ;
    
    FILE * fp = fopen(gcov_file, "r") ;
    if (fp == 0x0) {
        perror("read_gcov_file: fopen") ;
        exit(1) ;
    }

    coverage->line_nums = (int *) malloc(sizeof(int) * COV_MAX) ;

    char * buf = (char *) malloc(sizeof(char) * LINE_MAX) ;
    size_t line_max = LINE_MAX ;
    while(getline(&buf, &line_max, fp) > 0) {
        char * covered = strtok(buf, ":") ; 
        if (atoi(covered) > 0) { 
            char * line_number = strtok(0x0, ":") ;

            if (cov_idx != 0 && cov_idx % COV_MAX == 0) {
                coverage->line_nums = realloc(coverage, cov_idx + COV_MAX) ;
            }
            coverage->line_nums[cov_idx++] = atoi(line_number) ;
        #ifdef DEBUG
            printf("('%s', %d)\n", source_filename, coverage->line_nums[cov_idx - 1]) ;
        #endif
        }
    }

    free(buf) ;
    fclose(fp) ;

    coverage->line_cnt = cov_idx ;
}

void 
remove_gcda (char * source_filename)
{
    char copied_filename[PATH_MAX] ;
    strcpy(copied_filename, source_filename) ;

    char gcda_file[PATH_MAX] ;
    char * ptr = strtok(copied_filename, ".") ;
    sprintf(gcda_file, "%s.gcda", ptr) ;
    if (remove(gcda_file) == -1) {
        perror("get_coverage: remove: gcda") ;
    }
}

void
get_coverage (coverset_t * coverage, char * source_filename)
{
    run_gcov(source_filename) ;
    // read gcov & save result
    read_gcov_file(coverage, source_filename) ;

    remove_gcda(source_filename) ;
}

void 
remove_files (char * executable, char * source_filename) {  
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