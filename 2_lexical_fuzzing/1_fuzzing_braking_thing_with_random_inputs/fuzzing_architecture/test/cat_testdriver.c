#include <stdio.h>
#include <string.h>

#include "../include/fuzzer.h"

int 
cat_oracle (char * dir_name, int return_code, int trial)
{
    int result ;

    if (return_code != 0) {
        result = -1 ;   
    }

    char in_path[RESULT_PATH_MAX] ;
    char out_path[RESULT_PATH_MAX] ;

    FILE * in_fp ;
    FILE * out_fp ;

    
    get_path(in_path, trial, 0) ;
    in_fp = fopen(in_path, "rb") ;

    get_path(out_path, trial, 1) ;
    out_fp = fopen(out_path, "rb") ; 

    int s ;
    char in_buf[1024] ;
    while((s = fread(in_buf, 1, 1024, in_fp)) > 0) {
        
        int out_len = 0 ;
        char out_buf[1024] ;
        while(out_len < s || !feof(out_fp)) {
            out_len += fread(out_buf + out_len, 1, 1024, out_fp) ;
        }

        if (memcmp(in_buf, out_buf, s) == 0) result = 0 ;
        else result = -1 ;
    }        
    

    fclose(in_fp) ;
    fclose(out_fp) ;
    return result ;
}

void
set_configs (test_config_t * config)
{
    strcpy(config->runargs.binary_path, "/bin/cat") ;
    // strcpy(config->runargs.cmd_args, "-b -e") ;
    config->oracle = cat_oracle ;
}

int
main ()
{
    test_config_t config ;
    init_config(&config) ;
    set_configs(&config) ;

    fuzzer_main(&config) ;
}