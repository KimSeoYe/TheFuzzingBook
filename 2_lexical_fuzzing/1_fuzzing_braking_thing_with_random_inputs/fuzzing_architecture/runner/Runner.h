#define PASS "PASS"
#define FAIL "FAIL"
#define UNRESOLVED "UNRESOLVED"

typedef struct result {
    int input_size ;
    char * input ;
    char * outcome ;
} result_t ;

typedef struct Runner {
    result_t result ;
    void (* run) (result_t *, char *, int) ;
} Runner ;

void RunnerInit (Runner * runner) ;  // naming..?