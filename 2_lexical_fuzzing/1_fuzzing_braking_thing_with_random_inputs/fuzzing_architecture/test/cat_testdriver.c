#include <stdio.h>

#include "../include/fuzzer.h"

void
set_configs (test_config_t * config)
{
    printf("SET CONFIGS\n") ;
}

int
main ()
{
    test_config_t config ;
    init_config(&config) ; // TODO. default values... >> 별로인 것 같다 (두번 세팅하니까..)
    set_configs(&config) ;

    fuzzer_main(&config) ;
}