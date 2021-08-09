#ifndef RUNNER
#define RUNNER

#define INP_MAX 128

enum outs { PASS = 0, FAIL, UNRESOLVED } ;

typedef struct ret_t {
    char inp[INP_MAX] ;
    enum outs out ;
} ret_t ;

void runner_run (ret_t * ret, char * inp) ;

void print_runner_run (ret_t * ret, char * inp) ;

#endif