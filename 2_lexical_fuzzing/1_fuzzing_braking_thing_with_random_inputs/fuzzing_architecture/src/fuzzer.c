#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/fuzzer.h"
#include "../include/input_fuzz.h"
#include "../include/run.h"

void
fuzzer_init (test_config_t * config, char * dir_name)
{
/*
    1. check some conditions
        - default values >> in config.h
        - binary path validity
        - check if the oracle function is not empty ?
    2. make result directory
*/

    if (access(config->binary_path, X_OK) == -1) {
        perror("Cannot access to the binary path") ;
        exit(1) ;
    }

    if (config->oracle == 0x0) {
        perror("Cannot access to the oracle tester") ;
        exit(1) ;
    }

    
}

void
fuzzer_main (test_config_t * config)
{
    char dir_name[32] ;
    fuzzer_init(config, dir_name) ;

    for (int i = 0; config->trial; i++) {
        input_fuzz(config) ;
        run(config) ;
        config->oracle(dir_name) ;
    }
}