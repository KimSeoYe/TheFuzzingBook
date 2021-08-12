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
    strcpy(config->runargs.cmd_args, "-b -e") ;
    config->runargs.num_args = 2 ;

    config->oracle = oracle ;
}

int
main ()
{
    test_config_t config ;
    init_config(&config) ; // TODO. default values... >> 별로인 것 같다 (두번 세팅하니까..)
    set_configs(&config) ;

    fuzzer_main(&config) ;
}