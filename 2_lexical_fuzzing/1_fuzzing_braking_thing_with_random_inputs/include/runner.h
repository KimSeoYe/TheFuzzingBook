#define INP_MAX 2048

enum outs { PASS = 0, FAIL, UNRESOLVED } ;

typedef struct ret_t {
    char inp[INP_MAX] ;
    enum outs out ;
} ret_t ;

void runner_run (char * inp, ret_t * ret) ;

void print_runner_run (char * inp, ret_t * ret) ;