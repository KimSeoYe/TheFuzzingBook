#ifndef PROG_RUNNER
#define PROG_RUNNER

#include "Runner.h"

typedef struct ProgramRunner {
    char * program ;

    char * sout ;   // as a file
    char * serr ;   // as a file
    char * outcome ;
    int return_code ;
    
    void (* run_process) (struct ProgramRunner *, char *, int) ;   // self, inp, length of inp
    void (* run) (struct ProgramRunner *, char *, int) ;
    void (* free_results) (struct ProgramRunner) ;
} ProgramRunner ;

void ProgramRunnerInit (ProgramRunner * runner, char * program) ;

#endif