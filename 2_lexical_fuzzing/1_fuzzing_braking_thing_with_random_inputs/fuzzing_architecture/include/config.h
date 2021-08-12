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
    char ** cmd_args ;   // Q. argument들을 string 하나로 받아서 parsing VS array of string ..

    int trial ; // TODO. default 10 ?

    int timeout ; // TODO. for each loop. default ?

    int (* oracle) (char * dir_name) ;

} test_config_t ;

void init_config (test_config_t * config) ;

#endif