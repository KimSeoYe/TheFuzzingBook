#ifndef GCOV_RUNNER
#define GCOV_RUNNER

#define LINE_MAX 256 // Q.
#define COV_MAX 1024

typedef struct list {
    int * list ;
    int cnt ;
} list_t ;

void get_source_filename (char * dst, char * src) ;

void get_executable_real_path (char * executable_path, char * source_path) ;

int exec_program (char * program, char ** arguments) ;

void compile_with_coverage (char * target_path, char * target_path_c) ;

void run_gcov (char * source_filename) ;

int get_coverage (list_t * cov_set, char * source_filename) ;

void remove_files (char * executable, char * source_filename) ;

#endif
  