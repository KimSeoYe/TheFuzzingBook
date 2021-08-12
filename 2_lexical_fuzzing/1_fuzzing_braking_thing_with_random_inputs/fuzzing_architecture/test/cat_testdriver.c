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
    INIT_CONFIG(config) ;
    set_configs(&config) ;

    fuzzer_main(&config) ;
}