#ifndef GCOV_RUNNER
#define GCOV_RUNNER

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

int get_total_line_cnt (char * source_path) ;

void get_source_filename (char * dst, char * src) ;

void get_executable_real_path (char * executable_path, char * source_path) ;

int exec_program (char * program, char ** arguments) ;

void compile_with_coverage (char * target_path, char * target_path_c) ;

void run_gcov (char * source_path) ;

void remove_gcda (char * source_path) ;

coverage_t get_src_cnts (char * source_path) ;

int get_coverage (coverage_t * coverage, coverage_t * cov_set, int cov_set_len, char * source_path) ;

void remove_gcov_file (char * executable, char * source_path) ;

#endif
  