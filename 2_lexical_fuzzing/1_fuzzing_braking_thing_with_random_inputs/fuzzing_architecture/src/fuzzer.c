#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "../include/fuzzer.h"
#include "../include/fuzz_input.h"
#include "../include/run.h"

void
create_input_dir (char * dir_name)
{
    char template[] = "tmp.XXXXXX" ;
    char * temp_dir = mkdtemp(template) ;
    if (temp_dir == 0x0) {
        perror("mkdtemp") ;
        exit(1) ;
    }
    strcpy(dir_name, temp_dir) ;
}

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
    /*
        Q. exec에 상대 경로를 넣었을 경우 ?
        Q. command line argument를 어떤 식으로 넘겨줄지 (exec 함수들 중 무엇을 선택할지)    
    */

    if (config->oracle == 0x0) {
        perror("Cannot access to the oracle tester") ;
        exit(1) ;
    }

    create_input_dir(dir_name) ;
}

void
fuzzer_main (test_config_t * config)
{
    char dir_name[32] ;
    fuzzer_init(config, dir_name) ;

    srand(time(NULL)) ;

    for (int i = 0; config->trial; i++) {
        char * input = (char *) malloc(sizeof(char) * (config->f_max_len + 1)) ; // Q.
        fuzz_input(config, input) ;

        run(config, input) ;

        config->oracle(dir_name) ;

        free(input) ;
    }
}