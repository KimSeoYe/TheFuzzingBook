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

// void
// make_new_union (int * new_union, int * set_a, int * set_b)
// {
//     int * longer ;
//     int * shorter ;
//     if (set_a->cnt > set_b->cnt) {
//         longer = set_a ;
//         shorter = set_b ;
//     }
//     else {
//         longer = set_b ;
//         shorter = set_a ;
//     }

//     // TODO. realloc
//     int i ;
//     for (i = 0; i < shorter->cnt; i++) {
//         if (longer->line_nums[i] == shorter->line_nums[i]) {
//             new_union->line_nums[new_union->cnt] = longer->line_nums[i] ;
//             new_union->cnt += 1 ;
//         }
//         else {
//             if (longer->line_nums[i] > shorter->line_nums[i]) {
//                 new_union->line_nums[new_union->cnt] = shorter->line_nums[i] ;
//                 new_union->line_nums[new_union->cnt + 1] = longer->line_nums[i] ;
//             }
//             else {
//                 new_union->line_nums[new_union->cnt] = longer->line_nums[i] ;
//                 new_union->line_nums[new_union->cnt + 1] = shorter->line_nums[i] ;
//             }
//             new_union->cnt += 2 ;
//         }
//     }
//     for (; i < longer->cnt; i++) {
//         new_union->line_nums[new_union->cnt] = longer->line_nums[i] ;
//         new_union->cnt += 1 ;
//     }
// }

// void 
// union_coverages (int * cov_set, int * coverages, int coverages_size)
// {
//     if (coverages[0].cnt > COV_MAX) {
//         cov_set->line_nums = realloc(cov_set->line_nums, COV_MAX * (coverages[0].cnt/COV_MAX + 1)) ;
//         if (cov_set->line_nums == 0x0) {
//             perror("union_coverage: realloc") ;
//             exit(1) ; 
//         }
//     }
//     memcpy(cov_set->line_nums, coverages[0].line_nums, coverages[0].cnt) ;
//     cov_set->cnt = coverages[0].cnt ;

//     for (int i = 1; i < coverages_size; i++) {
//         int new_union ;
//         new_union.line_nums = (int *) malloc(sizeof(int) * (cov_set->cnt + coverages[i].cnt)) ;
//         make_new_union (&new_union, cov_set, &coverages[i]) ;

//         memcpy(cov_set->line_nums, new_union.line_nums, new_union.cnt) ;
//         cov_set->cnt = new_union.cnt ;

//         free(new_union.line_nums) ;
//     }
// }


int
read_gcov_file (list_t * cov_set, char * source_filename) 
{
    char gcov_file[PATH_MAX] ;
    sprintf(gcov_file, "%s.gcov", source_filename) ;

    int cov_idx = 0 ;
    
    FILE * fp = fopen(gcov_file, "r") ;
    if (fp == 0x0) {
        perror("read_gcov_file: fopen") ;
        exit(1) ;
    }

    list_t gcov_result ;
    gcov_result.list = (int *) malloc(sizeof(int) * COV_MAX) ;

    int idx = 0 ;

    char * buf = (char *) malloc(sizeof(char) * LINE_MAX) ;
    size_t line_max = LINE_MAX ;
    while(getline(&buf, &line_max, fp) > 0) {
        char * covered = strtok(buf, ":") ; 
        if (atoi(covered) > 0) { 
            char * line_number = strtok(0x0, ":") ;

            if (idx != 0 && idx % COV_MAX == 0) {
                gcov_result.list = realloc(gcov_result.list, idx + COV_MAX) ;
            }
            gcov_result.list[idx++] = atoi(line_number) ;
        #ifdef DEBUG
            printf("('%s', %d)\n", source_filename, line_nums[idx - 1]) ;
        #endif
        }
    }

    gcov_result.size = idx ;
    // union_coverage(cov_set, gcov_result) ;

    free(buf) ;
    fclose(fp) ;
    free(gcov_result.list) ;

    return idx ;
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

int
get_coverage (list_t * cov_set, char * source_filename)
{
    run_gcov(source_filename) ;
    int coverage = read_gcov_file(cov_set, source_filename) ;
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