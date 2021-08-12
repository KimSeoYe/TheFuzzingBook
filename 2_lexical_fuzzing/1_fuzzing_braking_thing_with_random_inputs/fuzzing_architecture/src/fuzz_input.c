#include <stdio.h>
#include <stdlib.h>

#include "../include/fuzz_input.h"

void
fuzz_input (test_config_t * config, char * fuzz_string)
{
    int string_len = rand() % (config->f_max_len - config->f_min_len + 1) + config->f_min_len ;
    
    for (int i = 0; i < string_len; i++) {
        fuzz_string[i] = rand() % (config->f_char_range + 1) + config->f_char_start ;
    }
    fuzz_string[string_len] = 0x0 ;

    return ;
}