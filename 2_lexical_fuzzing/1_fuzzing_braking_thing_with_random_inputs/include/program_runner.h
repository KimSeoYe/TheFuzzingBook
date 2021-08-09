#ifndef PROG_RUNNER
#define PROG_RUNNER

enum pr_outs { PR_PASS = 0, PR_FAIL, PR_UNRESOLVED } ;

typedef struct pr_ret_t {
    enum pr_outs outcome ;
    char * result ;
} pr_ret_t ;

void program_runner_run (char * program, char * inp, pr_ret_t * ret) ;

#endif