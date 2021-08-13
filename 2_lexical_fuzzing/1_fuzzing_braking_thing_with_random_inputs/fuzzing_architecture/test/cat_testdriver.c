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
compare_in_and_out (FILE * in_fp, FILE * out_fp)
{
    int result = 0 ;

    int s ;
    char in_buf[1024] ;
    while((s = fread(in_buf, 1, 1024, in_fp)) > 0) {
        
        int out_len = 0 ;
        char out_buf[1024] ;
        while(out_len < s || !feof(out_fp)) {
            out_len += fread(out_buf + out_len, 1, 1024, out_fp) ;
        }

        if (memcmp(in_buf, out_buf, s) != 0) {
            result = -1 ;
            break ;
        }
    }        

    return result ;
}

int 
cat_oracle (int return_code, int trial)
{
    int result ;

    if (return_code != 0) {
        print_stderr(trial) ;
        result = -1 ;   
    }

    char in_path[RESULT_PATH_MAX] ;  
    get_path(in_path, trial, 0) ;
    FILE * in_fp = fopen(in_path, "rb") ;
    
    char out_path[RESULT_PATH_MAX] ;
    get_path(out_path, trial, 1) ;
    FILE * out_fp = fopen(out_path, "rb") ; 

    result = compare_in_and_out(in_fp, out_fp) ;
    
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