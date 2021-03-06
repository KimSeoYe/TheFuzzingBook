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

void get_source_filename (char * dst, char * src) ;

void get_executable_real_path (char * executable_path, char * source_path) ;

int exec_program (char * program, char ** arguments) ;

void compile_with_coverage (char * target_path, char * target_path_c) ;

void run_gcov (char * source_filename) ;

coverage_t get_src_cnts (char * source_filename) ;

coverage_t get_coverage (coverage_t * cov_set, coverage_t src_cnts, char * source_filename) ;

void remove_files (char * executable, char * source_filename) ;

#endif
  