#ifndef CONFIG
#define CONFIG

#define PATH_MAX 4096
#define ARG_MAX 1024

typedef struct fuzarg {
    int f_min_len ;
    int f_max_len ;
    int f_char_start ;
    int f_char_range ;
} fuzarg_t ;

typedef struct runarg {
    char binary_path[PATH_MAX] ;
    char cmd_args[ARG_MAX] ;
    int timeout ; // TODO. for each loop. default ?
} runarg_t ;

typedef struct test_config {   
    int trials ;
    fuzarg_t fuzargs ;
    runarg_t runargs ;
    int (* oracle) (int return_code, int trial) ;
} test_config_t ;

void init_config (test_config_t * config) ;

#endif