#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/fuzzer.h"
#include "../include/fileio.h"


int pipes[2] ;

void
child_proc (char * program, char * path)
{
    close(pipes[0]) ;
    dup2(pipes[1], 0) ;
    dup2(pipes[1], 1) ;
    dup2(pipes[1], 2) ;
    // close(pipes[1]) ;

    execlp(program, program, path, 0x0) ;
	// get error code!
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
    if (pipe(pipes) != 0) {
        perror("pipe") ;
        exit(1) ;
    }

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

void
invoking_extern_prog ()
{
    char * program = "bc" ;

    char dir_name[32] ;
    char path[32] ;
    create_input_dir(dir_name) ;
    sprintf(path, "%s/%s", dir_name, "input") ;

    if (write_data(path, "2 + 2\n") == -1)  return ;
    my_popen(program, path) ;
    
    if (remove(path) == -1) {
        perror("remove") ;
        exit(1) ;
    }
    remove_input_dir(dir_name) ;
}

void
long_running_fuzzing ()
{
    int trials = 100 ;
    char * program = "bc" ;

    char dir_name[32] ;
    create_input_dir(dir_name) ;

    char path[32] ;
    char * basename = "input" ;

    for (int i = 0; i < trials; i++) {
        sprintf(path, "%s/%s%d", dir_name, basename, i) ;
        char * data = fuzzer(MAX_LEN, CHAR_START, CHAR_RANGE) ;
        if (write_data(path, data) == -1)  return ;
        my_popen(program, path) ;
        free(data) ;
    }
}

int
main ()
{
    srand(time(NULL)) ; 

    invoking_extern_prog() ;
    long_running_fuzzing() ;
}
