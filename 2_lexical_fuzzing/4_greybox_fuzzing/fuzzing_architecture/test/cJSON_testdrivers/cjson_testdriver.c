#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../include/fuzzer.h"

void
set_configs (test_config_t * config)
{
    strcpy(config->runargs.binary_path, "../../lib/cJSON/json_test") ;
    config->covargs.source_num = 1 ;
    strcpy(config->covargs.source_dir, "../../lib/cJSON/") ;

    config->covargs.source_paths = (char **) malloc(sizeof(char *) * config->covargs.source_num) ;
    config->covargs.source_paths[0] = (char *) malloc(sizeof(char) * PATH_MAX) ;
    strcpy(config->covargs.source_paths[0], "../../lib/cJSON/cJSON.c") ;

    strcpy(config->fuzargs.seed_dir, "./seed_cjson") ;
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
    while ((opt = getopt(argc, argv, "t:m:")) != -1) {
        switch(opt) {
            case 't':
                config.trials = atoi(optarg) ;
                break ;
        }
    }

    config.covargs.coverage_on = 1 ;
   
    strcpy(config.covargs.csv_filename, "random.csv") ;
    config.fuzz_type = RANDOM ;
    // for (int i = 0; i < 10; i++) {
        fuzzer_main(&config) ;
    // }

    strcpy(config.covargs.csv_filename, "mutation.csv") ;
    config.fuzz_type = MUTATION ;
    // for (int i = 0; i < 10; i++) {
        fuzzer_main(&config) ;
    // }
    
    free_conf_source_paths(&config) ;
}