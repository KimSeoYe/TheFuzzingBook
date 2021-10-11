#ifndef CONFIG
#define CONFIG

#define PATH_MAX 4096
#define ARG_MAX 1024

typedef enum fuzopt { STD_IN = 0, ARGUMENT, FILE_CONTENTS } fuzopt_t ;
typedef enum fuztype { RANDOM = 0, MUTATION, GREYBOX } fuztype_t ;

typedef struct fuzarg {
    int f_min_len ;     // minimum length of fuzzer input
    int f_max_len ;     // maximum length of fuzzer input

    int f_char_start ;  // starting point of the input range
    int f_char_range ;  // range of input characters (ascii)
    char seed_dir[PATH_MAX] ;
} fuzarg_t ;

typedef struct runarg { 
    char binary_path[PATH_MAX] ;    // executable binary path
    char cmd_args[ARG_MAX] ;        // arguments in string (ex. "-b -e")
    int timeout ;                   // maximum time interval of each execution
} runarg_t ;

typedef struct covarg {
    int coverage_on ;               // turn on calculating coverage part if coverage_on is true
    char ** source_paths ;          // Q. array or pointer?
    int source_num ;    
    char source_dir[PATH_MAX] ;
    char csv_filename[PATH_MAX] ;   // result file name
} covarg_t ;

typedef struct test_config {   
    int trials ;                    // total trials
    fuztype_t fuzz_type ;

    fuzopt_t fuzz_option ;          // fuzzing fuzz_option : fuzz stdin | argument | file contents
    int fuzzed_args_num ;           // target program's argument # (default 0)

    char source_path[PATH_MAX] ;    // path of c source file (if is_source is true)

    fuzarg_t fuzargs ;              
    runarg_t runargs ;
    covarg_t covargs ;

    int (* oracle) (int return_code, int trial) ;
} test_config_t ;

void init_config (test_config_t * config) ;

#endif