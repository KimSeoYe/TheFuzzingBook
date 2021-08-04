#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "fuzzer.h"

int pipes[2] ;

void
child_proc (char * program, char * path)
{
    close(pipes[0]) ;
	// pipes[1], 0
    dup2(pipes[1], 1) ;
    dup2(pipes[1], 2) ;
    close(pipes[1]) ;

    execlp(program, program, path, 0x0) ;
	// get error code!

    exit(0) ;
}

void
parent_proc ()
{
    close(pipes[1]) ;

    char buf[1024] ;
    int s ;
    while ((s = read(pipes[0], buf, 1023)) > 0) {
        buf[s] = 0x0 ;
        printf("%s", buf) ;
	}
    close(pipes[0]) ;

    return ;
}

void
my_popen (char * program, char * path)
{
    pid_t child_pid = fork() ;
    if (child_pid == 0) {
        child_proc(program, path) ;
    }
    else if (child_pid > 0) {
        parent_proc() ;
    }
    else {
        perror("fork") ;
        return ;
    }

    int exit_code ;
    pid_t term_pid = wait(&exit_code) ;

#ifdef DEBUG
    printf("execution end %d %d\n", term_pid, exit_code) ;
#endif
}
