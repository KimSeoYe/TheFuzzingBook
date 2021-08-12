#include <stdio.h>
#include <stdlib.h>

#include "../include/fuzz_input.h"

int
fuzz_input (fuzarg_t * fuzargs, char * fuzz_string)
{
    int string_len = rand() % (fuzargs->f_max_len - fuzargs->f_min_len + 1) + fuzargs->f_min_len ;
    
    for (int i = 0; i < string_len; i++) {
        fuzz_string[i] = rand() % (fuzargs->f_char_range + 1) + fuzargs->f_char_start ;
    }
    fuzz_string[string_len] = 0x0 ;

    return string_len ;
}