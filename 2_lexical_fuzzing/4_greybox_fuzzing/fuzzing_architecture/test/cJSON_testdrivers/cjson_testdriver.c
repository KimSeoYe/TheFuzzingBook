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
    while ((opt = getopt(argc, argv, "t:l:")) != -1) {
        switch(opt) {
            case 't':
                trials = atoi(optarg) ;
                break ;
            case 'l':
                loop_trials = atoi(optarg) ;
                break ;
        }
    }

    clock_t rand_start = clock() ;
    for (int i = 0; i < loop_trials; i++) {
        execute_fuzzer(trials, RANDOM, "random.csv") ;
    }
    clock_t rand_end = clock() ;

    clock_t mut_start = clock() ;
    for (int i = 0; i < loop_trials; i++) {
        execute_fuzzer(trials, MUTATION, "mutation.csv") ;
    }
    clock_t mut_end = clock() ;

    printf("\n=======================================================\n") ;
    printf("TOTAL TIME\n") ;
    printf("=======================================================\n") ;
    printf("RANDOM: %.3f s\n", (double)(rand_end - rand_start) / CLOCKS_PER_SEC) ;
    printf("MUTATION: %.3f s\n", (double)(mut_end - mut_start) / CLOCKS_PER_SEC) ;
    printf("=======================================================\n") ;

    return 0 ;
}