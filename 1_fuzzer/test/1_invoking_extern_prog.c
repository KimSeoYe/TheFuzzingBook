#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/fuzzer.h"
#include "../include/fileio.h"

#define DEBUG

int pipes[2] ;
int error_pipes[2] ;

void
child_proc (char * program, char * path)
{
    close(pipes[0]) ;
    close(error_pipes[0]) ;
    dup2(pipes[1], 0) ; // Q. close(0) ?
    dup2(pipes[1], 1) ;
    dup2(error_pipes[1], 2) ;

    execlp(program, program, path, 0x0) ;
}

void
parent_proc ()
{
    int exit_code ;
    pid_t term_pid = wait(&exit_code) ;

#ifdef DEBUG
    printf("execution end %d %d\n", term_pid, exit_code) ;
#endif

    close(pipes[1]) ;
    close(error_pipes[1]) ;

    char buf[1024] ;
    int s ;
    while ((s = read(pipes[0], buf, 1023)) > 0) {
        buf[s] = 0x0 ;
        printf("%s", buf) ;
	}
    close(pipes[0]) ;

    while ((s = read(error_pipes[0], buf, 1023)) > 0) {
        buf[s] = 0x0 ;
        printf("%s", buf) ;
	}
    close(error_pipes[0]) ;

    return ;
}

int
my_popen (char * program, char * path)
{
    // TODO. position?
    if (pipe(pipes) != 0) {
        perror("pipe") ;
        exit(1) ;
    }
    if (pipe(error_pipes) != 0) {
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
        return -1 ;
    }

    return 1 ;
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

    for (int i = 0; i < trials; i++) {
        sprintf(path, "%s/%s%d", dir_name, "input", i) ;
        char * data = fuzzer(MAX_LEN, CHAR_START, CHAR_RANGE) ;
        if (write_data(path, data) == -1)  return ;
        int exit_code = my_popen(program, path) ;   // TODO. something with error code
        free(data) ;
    }
}

int
main ()
{
    srand(time(NULL)) ; 

    // invoking_extern_prog() ;
    long_running_fuzzing() ;
}
