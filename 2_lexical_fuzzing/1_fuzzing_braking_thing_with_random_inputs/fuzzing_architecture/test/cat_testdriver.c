#include <stdio.h>
#include <string.h>

#include "../include/fuzzer.h"

int 
oracle (char * dir_name)
{
    printf("ORACLE FUNCTION RUN\n") ;

    return 1 ;
}

void
set_configs (test_config_t * config)
{
    strcpy(config->runargs.binary_path, "/bin/cat") ;
    // strcpy(config->runargs.cmd_args, "-b -e") ;
    config->oracle = oracle ;
}

int
main ()
{
    test_config_t config ;
    init_config(&config) ;
    set_configs(&config) ;

    fuzzer_main(&config) ;
}