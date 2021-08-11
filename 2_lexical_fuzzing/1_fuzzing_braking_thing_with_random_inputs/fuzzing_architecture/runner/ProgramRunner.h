#ifndef PROG_RUNNER
#define PROG_RUNNER

#include "Runner.h"

typedef struct prog_result {
    char * sout ;
    char * serr ;
    char * outcome ;
    int return_code ;
} prog_result_t ;

typedef struct ProgramRunner {
    prog_result_t result ;
    char * program ;
    
    void (* run_process) (prog_result_t *, char *, char *, int) ;   // ret, prog, inp, length of inp
    void (* run) (result_t *, char *, char *, int) ;
} ProgramRunner ;

void ProgramRunnerInit (ProgramRunner * runner, char * program) ;

#endif