#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PATH_MAX 2048
#define LINE_MAX 256 // Q.

/**
 * White-Box Testing : derive tests from the implementation.
 * 
 * Coverage criteria
 * 1. Statement coverage
 * 2. Branch coverage
*/

int
execute_program (char * program, char ** arguments)
{
    pid_t child_pid = fork() ;
    if (child_pid == 0) {
        if (execv(program, arguments) == -1) {
            perror("execute_program: execv") ;
            exit(1) ;
        }
    }

    int exit_code ;
    wait(&exit_code) ;

    return exit_code ;
}

void
compile_with_gcc (char * target, char * target_c)
{
    char ** compile_args = (char **) malloc(sizeof(char *) * 6) ;
    for (int i = 0; i < 6; i++) compile_args[i] = (char *) malloc(sizeof(char) * PATH_MAX) ;
    
    strcpy(compile_args[0], "gcc") ;
    strcpy(compile_args[1], "--coverage") ;
    strcpy(compile_args[2], "-o") ;
    strcpy(compile_args[3], target) ;
    strcpy(compile_args[4], target_c) ;
    compile_args[5] = 0x0 ;

    if (execute_program("/usr/bin/gcc", compile_args) != 0) {
        perror("compile_with_gcc: execute_program") ;
        exit(1) ;
    }

    for (int i = 0; i < 6; i++) free(compile_args[i]) ;
    free(compile_args) ;
}

void
run_target(char * target, char * input)
{
    char ** args = (char **) malloc(sizeof(char *) * 3) ;
    for (int i = 0; i < 3; i++) args[i] = (char *) malloc(sizeof(char) * PATH_MAX) ;

    strcpy(args[0], target) ;
    strcpy(args[1], input) ;
    args[2] = 0x0 ;

    if (execute_program(target, args) != 0) {
        perror("run_target: execute_program") ;
        exit(1) ;
    }

    for (int i = 0; i < 3; i++) free(args[i]) ;
    free(args) ;
}

void
run_gcov (char * c_file_name)
{
    char ** gcov_args = (char **) malloc(sizeof(char *) * 3) ;
    for(int i = 0; i < 3; i++) gcov_args[i] = (char *) malloc(sizeof(char) * PATH_MAX) ;

    strcpy(gcov_args[0], "gcov") ;
    strcpy(gcov_args[1], c_file_name) ;
    gcov_args[2] = 0x0 ;

    if (execute_program("/usr/bin/gcov", gcov_args) != 0) {
        perror("run_gcov: execute_program") ;
        exit(1) ;
    }

    for(int i = 0; i < 3; i++) free(gcov_args[i]) ;
    free(gcov_args) ;
}

void
read_gcov_file (char * c_file_name) 
{
    char gcov_file[PATH_MAX] ;
    sprintf(gcov_file, "%s.gcov", c_file_name) ;

    int cov_max = 1024 ;
    int cov_idx = 0 ;
    int * coverage = (int *) malloc(sizeof(int) * cov_max) ;

    FILE * fp = fopen(gcov_file, "rb") ;
    if (fp == 0x0) {
        perror("read_gcov_file: fopen") ;
    }

    char * buf = (char *) malloc(sizeof(char) * LINE_MAX) ;
    size_t line_max = 0 ;
    while(getline(&buf, &line_max, fp) > 0) {
        char * covered = strtok(buf, ":") ; 
        if (atoi(covered) > 0) { 
            char * line_number = strtok(0x0, ":") ;

            if (cov_idx != 0 && cov_idx % cov_max == 0) {
                coverage = realloc(coverage, cov_idx + cov_max) ;
            }

            coverage[cov_idx++] = atoi(line_number) ;
            printf("('%s', %d)\n", c_file_name, coverage[cov_idx - 1]) ;
        }
    }

    free(coverage) ;
    fclose(fp) ;
}

void 
remove_files (char * c_file_name) {
    
    char gcov_file[PATH_MAX] ;
    sprintf(gcov_file, "%s.gcov", c_file_name) ;
    if (remove(gcov_file) == -1) {
        perror("remove_files: remove: gcov") ;
    }

    char gcda_file[PATH_MAX] ;
    char * ptr = strtok(c_file_name, ".") ;
    sprintf(gcda_file, "%s.gcda", ptr) ;
    if (remove(gcda_file) == -1) {
        perror("remove_files: remove: gcda") ;
    }
    
    char gcno_file[PATH_MAX] ;
    sprintf(gcno_file, "%s.gcno", ptr) ;
    if (remove(gcno_file) == -1) {
        perror("remove_files: remove: gcno") ;
    }
}

void
get_c_file_name (char * dst, char * src)
{
    char cpied_src[1024] ;
    strcpy(cpied_src, src) ;

    char * tmp_ptr = src ;
    for (char * ptr = strtok(cpied_src, "/"); ptr != 0x0; ptr = strtok(0x0, "/")) {
        tmp_ptr = ptr ;
    }
    strcpy(dst, tmp_ptr) ;
}

int
main (int argc, char * argv[])
{
    if (argc != 4) {
        perror("whitebox_testing: usage: ./whitebox_testing EXECUATBLE_PATH FILE_PATH INPUT") ;
        return 1 ;
    }

    char * target = argv[1] ;
    char * target_c = argv[2] ;
    if (access(target_c, R_OK) == -1) {
        perror("access: readable file does not exist") ;
        return 1 ;
    }
    char * input = argv[3] ;

    char c_file_name[PATH_MAX] ;
    get_c_file_name(c_file_name, target_c) ;

    compile_with_gcc(target, target_c) ;
    run_target(target, input) ;
    run_gcov(c_file_name) ;

    read_gcov_file(c_file_name) ;

    remove_files(c_file_name) ;
    return 0 ;
}