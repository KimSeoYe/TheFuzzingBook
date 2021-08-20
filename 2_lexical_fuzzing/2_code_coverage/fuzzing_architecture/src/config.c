#include <stdio.h>
#include <string.h>

#include "../include/config.h"

void 
init_config (test_config_t * config)
{
    config->trials = 10 ; 

    config->fuzargs.f_min_len = 10 ;
    config->fuzargs.f_max_len = 100 ;
    config->fuzargs.f_char_start = 32 ;
    config->fuzargs.f_char_range = 32 ;

    config->option = STD_IN ;

    config->is_source = 0 ;
    strcpy(config->source_path, "") ;
    strcpy(config->runargs.binary_path, "") ;
    strcpy(config->runargs.cmd_args, "") ;
    config->runargs.timeout = 5 ;
    config->fuzzed_args_num = 0 ;

    config->oracle = 0x0 ;
}