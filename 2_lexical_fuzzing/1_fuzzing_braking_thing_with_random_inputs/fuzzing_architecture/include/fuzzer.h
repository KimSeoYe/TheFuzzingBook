#ifndef FUZZER
#define FUZZER

#include "config.h"

typedef enum results { PASS = 0, FAIL, UNRESOLVED } results_t ;

void fuzzer_main (test_config_t * config) ;

#endif