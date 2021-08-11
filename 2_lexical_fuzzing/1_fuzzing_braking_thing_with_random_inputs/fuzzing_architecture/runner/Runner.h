#ifndef RUNNER
#define RUNNER

#define PASS "PASS"
#define FAIL "FAIL"
#define UNRESOLVED "UNRESOLVED"

// typedef struct result {
//     int input_size ;
//     char * input ;
//     char * outcome ;
// } result_t ;

typedef struct Runner {
    int input_size ;
    char * input ;
    char * outcome ;

    void (* run) (struct Runner *, char *, int) ;
    void (* free_input) (struct Runner) ;
} Runner ;

void run (Runner * self, char * input, int input_size) ;

void free_input (Runner runner) ;

void RunnerInit (Runner * runner) ;  // naming..?

#endif