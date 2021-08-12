#ifndef FUZZER
#define FUZZER

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

void fuzzer_main (test_config_t config) ;

#endif