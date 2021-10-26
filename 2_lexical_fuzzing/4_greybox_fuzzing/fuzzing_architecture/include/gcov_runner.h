#ifndef GCOV_RUNNER
#define GCOV_RUNNER

#include "config.h"

#define LINE_MAX 256 // Q.
#define COV_MAX 1024

typedef struct list {
    int * list ;
    int cnt ;
} list_t ;

typedef struct coverage {
    int line ;
    int branch ;
} coverage_t ;

typedef struct covset {
    char filepath[PATH_MAX] ;
    int len ;
    coverage_t * set ;
} covset_t ;

typedef struct idset {
    int len ;
    coverage_t * set ;
} idset_t ;

int get_total_line_cnt (char * source_path) ;

void get_source_filename (char * dst, char * src) ;

void get_executable_real_path (char * executable_path, char * source_path) ;

int exec_program (char * program, char ** arguments, covarg_t * covargs) ;

void run_gcov (char * source_path, covarg_t * covargs) ;

void remove_gcda (char * source_path) ;

coverage_t get_src_cnts (char * source_path, covarg_t * covargs) ;

int get_coverage (coverage_t * coverage, covset_t * cov_sets, idset_t * id_set, covarg_t * covargs) ;

void remove_gcov_file (char * executable, char * source_path) ;

#endif
  