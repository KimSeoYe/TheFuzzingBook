#ifndef FUZZER
#define FUZZER

#include "config.h"

#define RESULT_PATH_MAX 32
#define CONTENTS_MAX 32
#define ARG_N_MAX 64

typedef enum result { PASS = 0, FAIL, UNRESOLVED } result_t ;
static char result_strings[3][16] = { "PASS", "FAIL", "UNRESOLVED" } ;

typedef struct content {
    char ** input_contents ;
    char ** stdout_contents ;
    char ** stderr_contents ;
} content_t ;

void get_path (char * path, int trial, int fd) ;
void fuzzer_main (test_config_t * config) ;

#endif