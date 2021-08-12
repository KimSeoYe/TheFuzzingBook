#ifndef CONFIG
#define CONFIG

#define PATH_MAX 4096

typedef struct test_config {
    /*
        1. fuzzer input config
        2. binary path
        3. oracle function
        4. command line config
    */

    int f_min_len ;
    int f_max_len ;
    int f_char_start ;
    int f_char_range ;

    char binary_path[PATH_MAX] ;    // Q. dinamic ?
    char cmd_args[PATH_MAX] ;   // Q.

    int trial ; // TODO. default 10 ?

    int timeout ; // TODO. for each loop. default ?

    int (* oracle) (char * dir_name) ;

} test_config_t ;

#define INIT_CONFIG(X) test_config_t X = { .f_min_len = 10, .f_max_len = 100, .f_char_start = 32, .f_char_range = 32, .binary_path = "", .cmd_args = "", .trial = 10, .timeout = 5, .oracle = 0x0 }

#endif