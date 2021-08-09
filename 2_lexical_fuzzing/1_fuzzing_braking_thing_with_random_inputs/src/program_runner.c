#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../include/program_runner.h" 

// #define DEBUG

int pipes[2] ;

void
child_proc (char * program, char * inp) 
{
    dup2(pipes[0], 0) ;
    dup2(pipes[1], 1) ;
    dup2(pipes[1], 2) ;
    close(pipes[0]) ;

    execlp(program, program, inp, 0x0) ;
}

void
parent_proc (pr_ret_t * ret)
{
    close(pipes[1]) ;

    char buf[1024] ;
    int len = 0 ;
    int s = 0 ;

    ret->result = (char *) malloc(sizeof(char) * 1) ;   // TODO.
    ret->result[0] = 0x0 ;

    while ((s = read(pipes[0], buf, 1023)) > 0) {
        buf[s] = 0x0 ;
        len += s ;
        ret->result = realloc(ret->result, (len + 1)) ;
        strcat(ret->result, buf) ;

    #ifdef DEBUG
        printf("%s", buf) ;  // TODO.
    #endif
    }
    close(pipes[0]) ;
}

int
run_process (char * program, char * inp, pr_ret_t * ret)
{
    if (pipe(pipes) != 0) {
        perror("pipe") ;
        exit(1) ;
    }

    pid_t child_pid = fork() ;
    if (child_pid == 0) {
        child_proc(program, inp) ;
    }
    else if (child_pid > 0) {
        parent_proc(ret) ;
    }
    else {
        perror("fork") ;
        exit(1) ;
    }

    int exit_code ;
    pid_t term_pid = wait(&exit_code) ;
#ifdef DEBUG
    printf("%d terminated w/ exit code %d\n", term_pid, exit_code) ;
#endif

    return exit_code ;
}

void
program_runner_run (char * program, char * inp, pr_ret_t * ret)
{
    int exit_code = run_process(program, inp, ret) ;

    if (exit_code == 0) {
        ret->outcome = PR_PASS ;
    }
    else if (exit_code < 0) {
        ret->outcome = PR_FAIL ;
    }
    else {
        ret->outcome = PR_UNRESOLVED ;
    }

    return ;
}

void
binary_program_runner_run (char * program, int inp, pr_ret_t * ret)
{
    char str_inp[16] ;
    sprintf(str_inp, "%d", inp) ;
    program_runner_run(program, str_inp, ret) ;
}