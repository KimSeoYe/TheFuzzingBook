#include <stdio.h>
#include <unistd.h>
#include "../include/config.h"

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


void
get_executable_real_path (char * executable_path, char * source_path)
{
    char copied_src[PATH_MAX] ;
    strcpy(copied_src, source_path) ;

    char * tok_ptr = strtok(copied_src, ".") ;
    strcpy(executable_path, tok_ptr) ;
    printf("executable: %s\n", executable_path) ;
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