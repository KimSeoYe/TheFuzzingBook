#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "../include/config.h"
#include "../include/gcov_runner.h"

// #define DEBUG

int
get_total_line_cnt (char * source_path)
{
    int cnt = 0 ;

    FILE * fp = fopen(source_path, "rb") ;
    if (fp == 0x0) {
        perror("get_total_line_cnt: fopen") ;
        exit(1) ;
    }
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
exec_program (char * program, char ** arguments, covarg_t * covargs)
{
    pid_t child_pid = fork() ;

    if (child_pid == 0) {
        chdir(covargs->source_dir) ;

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
run_gcov (char * source_path, covarg_t * covargs)
{
    char * gcov_args[] = { "/usr/bin/gcov", "-b", source_path, 0x0 } ;

    if (exec_program("/usr/bin/gcov", gcov_args, covargs) != 0) {
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
get_src_cnts (char * source_path, covarg_t * covargs)
{

    run_gcov(source_path, covargs) ;
    
    char gcov_file[PATH_MAX] ;
    sprintf(gcov_file, "%s.gcov", source_path) ;

    coverage_t cnt ;
    cnt.line = 0 ;
    cnt.branch = 0 ;

    FILE * fp = fopen(gcov_file, "rb") ;
    if (fp == 0x0) {
        perror("get_src_cnts: fopen") ;
        exit(1) ;
    }
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
read_gcov_file (coverage_t * cov, covset_t * cov_set, char * source_path) 
{
    char gcov_file[PATH_MAX] ;
    sprintf(gcov_file, "%s.gcov", source_path) ;
    printf("gcov_path: %s\n", gcov_file) ;
    
    FILE * fp = fopen(gcov_file, "r") ;
    if (fp == 0x0) {
        perror("read_gcov_file: fopen") ;
        exit(1) ;
    }

    cov->line = 0 ;
    cov->branch = 0 ; 

    int * line_result = (int *) malloc(sizeof(int) * cov_set->len) ;  
    int * branch_result = (int *) malloc(sizeof(int) * cov_set->len) ;
    
    char buf[LINE_MAX] ;
    for (int branch_num = 0 ; fgets(buf, LINE_MAX, fp) > 0; ) { // TODO. what if, branch num is more than line num?
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
 
    fclose(fp) ;

    int is_cov_grow = 0 ;

    /**
     * Question
     * 
     * Currently, it consider both line and branch coverage
     * when update is_cov_grow.
     * Need to separate it?
    */

    for (int i = 0; i < cov->line; i++) {
        if (cov_set->set[line_result[i]].line == 0) {
            is_cov_grow = 1 ;
        }
        cov_set->set[line_result[i]].line = 1 ;
    }

    for (int i = 0; i < cov->branch; i++) {
        if (cov_set->set[branch_result[i]].branch == 0) {
            is_cov_grow = 1 ; 
        }
        cov_set->set[branch_result[i]].branch = 1 ;
    }

    free(line_result) ;
    free(branch_result) ;
    return is_cov_grow ;
}

int
get_coverage (coverage_t * coverage, covset_t * cov_sets, covarg_t * covargs)
{
    int is_total_cov_grow = 0 ;

    coverage->line = 0 ;
    coverage->branch = 0 ;

    for (int i = 0; i < covargs->source_num; i++) {
        run_gcov(covargs->source_paths[i], covargs) ;

        coverage_t coverage_per_src ;
        int is_cov_grow = read_gcov_file(&coverage_per_src, &cov_sets[i], covargs->source_paths[i]) ;
        if (!is_total_cov_grow && is_cov_grow) is_total_cov_grow = 1 ;
        
        coverage->line += coverage_per_src.line ;
        coverage->branch += coverage_per_src.branch ;
        
        remove_gcda(covargs->source_paths[i]) ;
    }
    
    return is_total_cov_grow ;
}

void 
remove_gcov_file (char * executable, char * source_path) 
{  
    char gcov_file[PATH_MAX] ;
    sprintf(gcov_file, "%s.gcov", source_path) ;
    if (remove(gcov_file) == -1) {
        perror("remove_gcov_file: remove: gcov") ;
    }
}
