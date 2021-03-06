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
bc_oracle (int return_code, int trial)
{
    if (return_code != 0) return -1 ;   
    else {
        char err_path[RESULT_PATH_MAX] ;
        get_path(err_path, trial, 2) ;
        FILE * err_fp = fopen(err_path, "rb") ;

        char err_buf[1024] ;

        if (fread(err_buf, 1, 1023, err_fp) > 0) {
            return -1 ;
        }
        fclose(err_fp) ;        
    }
    return 0 ;
}

void
set_configs (test_config_t * config)
{
    // strcpy(config->source_path, "../lib/bc-1.07.1/bc/bc.c") ;
    strcpy(config->covargs.source_dir, "../lib/bc-1.07.1/bc/") ;
    char sources[7][PATH_MAX] = { "../lib/bc-1.07.1/bc/bc.c", "../lib/bc-1.07.1/bc/execute.c", "../lib/bc-1.07.1/bc/load.c", "../lib/bc-1.07.1/bc/main.c", "../lib/bc-1.07.1/bc/scan.c", "../lib/bc-1.07.1/bc/storage.c", "../lib/bc-1.07.1/bc/util.c"} ;
    config->covargs.source_num = 7 ;
    config->covargs.source_paths = (char **) malloc(sizeof(char *) * config->covargs.source_num) ;
    for (int i = 0; i < config->covargs.source_num; i++) {
        config->covargs.source_paths[i] = (char *) malloc(sizeof(char) * PATH_MAX) ;
        strcpy(config->covargs.source_paths[i], sources[i]) ;
    }

    strcpy(config->runargs.binary_path, "../lib/bc-1.07.1/bc/bc") ;

    config->oracle = bc_oracle ;
}

void
free_conf_source_paths (test_config_t * config)
{
    for (int i = 0; i < config->covargs.source_num; i++) {
        free(config->covargs.source_paths[i]) ;
    }
    free(config->covargs.source_paths) ;
}


int
main (int argc, char * argv[])
{
    test_config_t config ;
    init_config(&config) ;
    set_configs(&config) ;

    int opt ;
    while ((opt = getopt(argc, argv, "t:m:c")) != -1) {
        switch(opt) {
            case 't':
                config.trials = atoi(optarg) ;
                break ;
            case 'm':
                config.fuzz_type = MUTATION ;
                strcpy(config.fuzargs.seed_dir, optarg) ;
                break ;
            case 'c':
                config.covargs.coverage_on = 1 ;
                break ;
        }
    }

    fuzzer_main(&config) ;
    
    free_conf_source_paths(&config) ;
}