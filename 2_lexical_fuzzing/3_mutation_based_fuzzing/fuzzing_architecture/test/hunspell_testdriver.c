#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/fuzzer.h"

void
print_stderr (int trial)
{
    char err_path[RESULT_PATH_MAX] ;
    get_path(err_path, trial, 2) ;
    FILE * err_fp = fopen(err_path, "rb") ;

    int s ;
    char err_buf[1024] ;
    printf("stderr: ") ;
    while((s = fread(err_buf, 1, 1023, err_fp)) > 0) {
        err_buf[s] = 0x0 ;
        printf("%s", err_buf) ;
    }
    fclose(err_fp) ;
}

int 
hunspell_oracle (int return_code, int trial)
{
    if (return_code != 0) {
        print_stderr(trial) ;
        return -1 ;   
    }

    else return 0 ;
}

void
set_configs (test_config_t * config)
{
    strcpy(config->runargs.binary_path, "/usr/local/bin/hunspell") ;
    config->fuzargs.f_char_start = 32 ;
    config->fuzargs.f_char_range = 95 ;
    config->oracle = hunspell_oracle ;
}

int
main (int argc, char * argv[])
{
    test_config_t config ;
    init_config(&config) ;
    set_configs(&config) ;

    int opt ;
    while ((opt = getopt(argc, argv, "t:m:")) != -1) {
        switch(opt) {
            case 't':
                config.trials = atoi(optarg) ;
                break ;
            case 'm':
                config.fuzz_type = MUTATION ;
                strcpy(config.fuzargs.seed_dir, optarg) ;
                break ;
        }
    }

    fuzzer_main(&config) ;
}