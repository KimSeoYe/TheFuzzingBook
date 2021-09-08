#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/fuzzer.h"

void
set_configs (test_config_t * config)
{
    strcpy(config->source_path, "../lib/bc-1.07.1/bc/bc.c") ;
    strcpy(config->runargs.binary_path, "../lib/bc-1.07.1/bc/bc") ;
    // absolute : /Users/kimseoye/Developer/git/TheFuzzingBook/2_lexical_fuzzing/3_mutation_based_fuzzing/fuzzing_architecture/lib/bc-1.07.1/bc

    config->is_source = 0 ;
    config->coverage_on = 1 ;
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