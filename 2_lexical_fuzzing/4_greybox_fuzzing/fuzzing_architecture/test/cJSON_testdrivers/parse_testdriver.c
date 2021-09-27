#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../include/fuzzer.h"

void
set_configs (test_config_t * config)
{
    strcpy(config->runargs.binary_path, "../../lib/cJSON/parse_test") ;
    char sources[1][PATH_MAX] = { "../../lib/cJSON/cJSON.c"} ;
    config->covargs.source_num = 1 ;
    config->covargs.source_paths = (char **) malloc(sizeof(char *) * config->covargs.source_num) ;
    for (int i = 0; i < config->covargs.source_num; i++) {
        config->covargs.source_paths[i] = (char *) malloc(sizeof(char) * PATH_MAX) ;
        strcpy(config->covargs.source_paths[i], sources[i]) ;
    }
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