#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "../../include/fuzzer.h"

void
set_configs (test_config_t * config)
{
    config->covargs.coverage_on = 1 ;

    strcpy(config->runargs.binary_path, "../../lib/cJSON/JY/JY_cjson_test") ;
    config->covargs.source_num = 1 ;
    strcpy(config->covargs.source_dir, "../../lib/cJSON/JY/") ;

    config->covargs.source_paths = (char **) malloc(sizeof(char *) * config->covargs.source_num) ;
    config->covargs.source_paths[0] = (char *) malloc(sizeof(char) * PATH_MAX) ;
    strcpy(config->covargs.source_paths[0], "../../lib/cJSON/JY/cJSON.c") ;

    strcpy(config->fuzargs.seed_dir, "./testprogram_seeds/seed_JY") ;
}

void
free_conf_source_paths (test_config_t * config)
{
    for (int i = 0; i < config->covargs.source_num; i++) {
        free(config->covargs.source_paths[i]) ;
    }
    free(config->covargs.source_paths) ;
}

void
execute_fuzzer (int trials, fuztype_t fuzz_type, char * csv_filename)
{
    test_config_t config ;
    init_config(&config) ;
    set_configs(&config) ;

    config.trials = trials ;
    config.fuzz_type = fuzz_type ;
    strcpy(config.covargs.csv_filename, csv_filename) ;

    fuzzer_main(&config) ;

    free_conf_source_paths(&config) ;
}

int
main (int argc, char * argv[])
{
    int opt ;
    int trials = 10 ;
    int loop_trials = 10 ;
    fuztype_t fuzz_type = RANDOM ;
    char csv_filename[PATH_MAX] ;
    strcpy(csv_filename, "result.csv") ;

    while ((opt = getopt(argc, argv, "t:l:mg")) != -1) {
        switch(opt) {
            case 't':
                trials = atoi(optarg) ;
                break ;
            case 'l':
                loop_trials = atoi(optarg) ;
                break ;
            case 'm':
                fuzz_type = MUTATION ;
                strcpy(csv_filename, "result_jy_mut.csv") ;
                break ;
            case 'g':
                fuzz_type = GREYBOX ;
                strcpy(csv_filename, "result_jy_grey.csv") ;
                break ;
        }
    }
 
    execute_fuzzer(trials, fuzz_type, csv_filename) ;
    
    return 0 ;
}