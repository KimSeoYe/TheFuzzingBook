#include <stdio.h>
#include <string.h>

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
troff_oracle (int return_code, int trial)
{
    if (return_code != 0) {
        // print_stderr(trial) ;
        return -1 ;   
    }

    else return 0 ;
}

void
set_configs (test_config_t * config)
{
    // strcpy(config->runargs.binary_path, "../lib/troff_bug_simulate") ;
    config->is_source = 1 ;
    strcpy(config->source_path, "../lib/troff_bug_simulate.c") ;

    config->trials = 100 ;
    
    config->fuzargs.f_char_start = 0 ;
    config->fuzargs.f_char_range = 255 ;
    config->oracle = troff_oracle ;
}

int
main (int argc, char * argv[])
{
    test_config_t config ;
    init_config(&config) ;
    set_configs(&config) ;

    fuzzer_main(&config) ;
}