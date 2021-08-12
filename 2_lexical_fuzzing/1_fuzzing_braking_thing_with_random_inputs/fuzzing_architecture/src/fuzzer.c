#include <stdio.h>

#include "../include/fuzzer.h"
#include "../include/input_fuzz.h"
#include "../include/run.h"

void
fuzzer_init (test_config_t config, char * dir_name)
{
    /*
        1. check some conditions
            - binary path validity
            - default values
            - check if the oracle function is not 0x0
        2. make result directory
    */
   
   printf("FUZZER INIT\n") ;
   printf("MAKE DIRECTORY\n") ;
}

void
fuzzer_main (test_config_t config)
{
    char dir_name[32] ;
    fuzzer_init(config, dir_name) ;

    for (int i = 0; config.trial; i++) {
        input_fuzz(config.f_min_len, config.f_max_len, config.f_char_start, config.f_char_range) ;
        run(dir_name, config.binary_path, config.cmd_args, config.timeout) ;
        config.oracle(dir_name) ;
    }
}