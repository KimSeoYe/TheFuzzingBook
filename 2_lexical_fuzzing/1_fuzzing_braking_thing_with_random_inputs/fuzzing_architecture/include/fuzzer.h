#ifndef FUZZER
#define FUZZER

#include "config.h"

typedef enum result { PASS = 0, FAIL, UNRESOLVED } result_t ;
static char result_strings[3][16] = { "PASS", "FAIL", "UNRESOLVED" } ;

void fuzzer_main (test_config_t * config) ;

#endif