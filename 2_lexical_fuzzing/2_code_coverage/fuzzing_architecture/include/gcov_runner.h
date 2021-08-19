#ifndef GCOV_RUNNER
#define GCOV_RUNNER

void get_c_file_name (char * dst, char * src) ;

void get_executable_real_path (char * executable_path, char * source_path) ;

int exec_program (char * program, char ** arguments) ;

void compile_with_coverage (char * target_path, char * target_path_c) ;

void run_gcov (char * c_file_name) ;

void remove_files (char * executable, char * c_file_name) ;

#endif
  