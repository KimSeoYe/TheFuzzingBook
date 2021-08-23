#ifndef CONFIG
#define CONFIG

#define PATH_MAX 4096
#define ARG_MAX 1024

typedef enum fuzopt { STD_IN = 0, ARGUMENT, FILE_CONTENTS } fuzopt_t ;

typedef struct fuzarg {
    int f_min_len ;     // minimum length of fuzzer input
    int f_max_len ;     // maximum length of fuzzer input
    int f_char_start ;  // starting point of the input range
    int f_char_range ;  // range of input characters (ascii)
} fuzarg_t ;

typedef struct runarg { 
    char binary_path[PATH_MAX] ;    // executable binary path
    char cmd_args[ARG_MAX] ;        // arguments in string (ex. "-b -e")
    int timeout ;                   // maximum time interval of each execution
} runarg_t ;

typedef struct test_config {   
    int trials ;                    // total trials
    fuzopt_t option ;               // fuzzing option : fuzz stdin | argument | file contents
    int fuzzed_args_num ;                  // target program's argument # (default 0)
    int is_source ;                 // is the target c source file ? (1: true, 0: false)
    char source_path[PATH_MAX] ;    // path of c source file (if is_source is true)
    fuzarg_t fuzargs ;              
    runarg_t runargs ;
    int (* oracle) (int return_code, int trial) ;
} test_config_t ;

void init_config (test_config_t * config) ;

#endif