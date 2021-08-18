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

void
get_c_file_name (char * dst, char * src)
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

    int i ;
    for (i = 0; i < src_len; i++) {
        dst[i] = reversed[idx - i - 1] ;
    }
    dst[i] = 0x0 ;
}

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
    else if (child_pid == 0x0) {
        perror("execute_program: fork") ;
        exit(1) ;
    }

    int exit_code ;
    wait(&exit_code) ;

    return exit_code ;
}

void
compile_with_gcc (char * target_path, char * target_path_c)
{
    char * compile_args[] = { "gcc", "--coverage", "-o", target_path, target_path_c, 0x0 } ;

    if (execute_program("/usr/bin/gcc", compile_args) != 0) {
        perror("compile_with_gcc: execute_program") ;
        exit(1) ;
    }
}

void
run_target(char * target_path, char * input)
{
    char * args[] = { target_path, input, 0x0 } ;
    if (execute_program(target_path, args) != 0) {
        perror("run_target: execute_program") ;
        exit(1) ;
    }
}

void
run_gcov (char * c_file_name)
{
    char * gcov_args[] = { "gcov", c_file_name, 0x0 } ;

    if (execute_program("/usr/bin/gcov", gcov_args) != 0) {
        perror("run_gcov: execute_program") ;
        exit(1) ;
    }
}

void
read_gcov_file (char * c_file_name) 
{
    char gcov_file[PATH_MAX] ;
    sprintf(gcov_file, "%s.gcov", c_file_name) ;

    int cov_max = 1024 ;
    int cov_idx = 0 ;
    int * coverage = (int *) malloc(sizeof(int) * cov_max) ; // TODO.

    FILE * fp = fopen(gcov_file, "rb") ;
    if (fp == 0x0) {
        perror("read_gcov_file: fopen") ;
        exit(1) ;
    }

    char * buf = (char *) malloc(sizeof(char) * LINE_MAX) ;
    size_t line_max = LINE_MAX ;
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

    free(buf) ;
    free(coverage) ;
    fclose(fp) ;
}


void 
remove_files (char * executable, char * c_file_name) {  
    if (remove(executable) == -1) {
        perror("remove_files: remove: executable") ;
    }

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

int
main (int argc, char * argv[])
{
    if (argc < 4) {
        perror("whitebox_testing: usage: ./whitebox_testing EXECUATBLE_PATH FILE_PATH INPUT") ;
        return 1 ;
    }

    char * executable_path = argv[1] ;
    char * target_c = argv[2] ;
    if (access(target_c, R_OK) == -1) {
        perror("access: readable file does not exist") ;
        return 1 ;
    }
    char * input = argv[3] ;

    char c_file_name[PATH_MAX] ;
    get_c_file_name(c_file_name, target_c) ;

    compile_with_gcc(executable_path, target_c) ;
    run_target(executable_path, input) ;
    run_gcov(c_file_name) ;

    read_gcov_file(c_file_name) ;
    // TODO.

    remove_files(executable_path, c_file_name) ;
    return 0 ;
}