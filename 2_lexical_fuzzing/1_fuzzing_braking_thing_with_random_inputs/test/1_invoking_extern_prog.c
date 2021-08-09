#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "../include/simplefuzzer.h"
#include "../include/fileio.h"

#define DEBUG

int stdout_pipes[2] ;
int stderr_pipes[2] ;
int stdin_pipes[2] ;

/*
    http://csit.udc.edu/~byu/COSC4740-01/Lecture6add1.pdf

    pipe : 
    single channel이고, read end와 write end가 있다. (files)
    child에서, write하는 파이프를 close하지 않으면 read하는 파이프로 eof가 넘어오지 않는다.

    * kernel에 의해 manage되는 버퍼
    * 프로세스가 pipe가 비었을 때 read()를 호출하면, 데이터가 전송되거나 write end가 close될 때 까지 프로세스가 block된다.

    이 코드에서 만약 pipes[0]에 stdin을 연결한다면,
    child에서 stdout에 연결된 파이프에 write를 마치고 close를 해줘야 stdin에 연결된 파이프(read end)에 eof가 넘어올 수 있다 (read from stdin!)
    (현재 코드의 경우, /dev/null을 읽으면 eof가 넘어오기 때문에 bc가 잘 종료되는 상태 >> 파이프로도 같은 효과를 내야 한다)
    (parent process에서 close하는 것은 관련 X - file descriptor table is per-process)
    * 아까의 경우, pipes[0]을 close한 후 pipe[0]에 stdin을 연결해서 동작하지 않았을 것이다.
    
    execlp :
    현재의 프로세스 이미지를 새로운 프로세스 이미지로 replace한다.
    따라서 이 코드에서 execlp 아래 있는 코드는 실행되지 않는다.
    * Process image : 프로세스를 실행하는 동안 필요한 executable file으로, 프로세스의 실행과 관련된 몇몇 segment들로 구성되어 있다.
*/

void
child_proc (char * program, char * path)
{
    dup2(stdin_pipes[0], 0) ;
    close(stdin_pipes[0]) ;
    close(stdin_pipes[1]) ;

    close(stdout_pipes[0]) ;
    close(stderr_pipes[0]) ;

    // int dev_null = open("/dev/null", O_RDONLY) ;    // /dev/null : eof... >> check man4 null
    // dup2(dev_null, 0) ;

    dup2(stdout_pipes[1], 1) ;
    dup2(stderr_pipes[1], 2) ;

    execlp(program, program, path, 0x0) ;
}

int
parent_proc (char * dir_name, int i) 
{
    close(stdin_pipes[1]) ;
    close(stdout_pipes[1]) ;
    close(stderr_pipes[1]) ;

    char out_path[32] ;
    sprintf(out_path, "%s/%s%d", dir_name, "output", i) ;

    FILE * fp = fopen(out_path, "wb") ;
    if (fp == 0x0) {
        perror("fopen") ;
        exit(1) ;
    }   

    // Q. possible to get results from both stderr and stdout ? : >> if the fuzzer string has \n in the middle?

    int flag = 1 ;  // treat empty output or comments as a normal

    char buf[1024] ;
    int s = 0 ;
    while ((s = read(stdout_pipes[0], buf, 1024)) > 0) {
        if (fwrite(buf, 1, s, fp) < s) {
            perror("fwrite") ;
        }
	}
    close(stdout_pipes[0]) ;

    while ((s = read(stderr_pipes[0], buf, 1024)) > 0) {
        flag = 2 ;  // TODO. inefficiant..
        if (fwrite(buf, 1, s, fp) < s) {
            perror("fwrite") ;
        }
	}
    close(stderr_pipes[0]) ;
    fclose(fp) ;

    return flag ;
}

int
my_popen (char * program, char * dir_name, char * in_path, int i)
{
    if (pipe(stdin_pipes) != 0) {
        perror("pipe") ;
        exit(1) ;
    }
    if (pipe(stdout_pipes) != 0) {
        perror("pipe") ;
        exit(1) ;
    }
    if (pipe(stderr_pipes) != 0) {
        perror("error pipe") ;
        exit(1) ;
    }

    int flag ;

    pid_t child_pid = fork() ;
    if (child_pid == 0) {
        child_proc(program, in_path) ;
    }
    else if (child_pid > 0) {
        flag = parent_proc(dir_name, i) ;
    }
    else {
        perror("fork") ;
        exit(1) ;
    }

    int exit_code ; 
    wait(&exit_code) ;

    if (exit_code == -1) flag = -1 ;

    return flag ;
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
    my_popen(program, dir_name, path, 0) ;
}

void
long_running_fuzzing ()
{
    int trials = 100 ;
    char * program = "bc" ;

    char dir_name[32] ;
    create_input_dir(dir_name) ;

    char in_path[32] ;

    // Q. 100 input files and 100 output files..?

    int stdout_num = 0 ;
    int stderr_num = 0 ;
    int nonzero_ret_num = 0 ;
    int stdout_results[100] ; 
    int stderr_results[100] ;
    int nonzero_ret_results[100] ;

    for (int i = 0; i < trials; i++) {
        sprintf(in_path, "%s/%s%d", dir_name, "input", i) ;
        char * data = fuzzer(MAX_LEN, CHAR_START, CHAR_RANGE) ;
        data = realloc(data, strlen(data) + 1) ;
        data[strlen(data)] = '\n' ;

        if (write_data(in_path, data) == -1)  {
            exit(1) ;
        }
        free(data) ;

        int flag = my_popen(program, dir_name, in_path, i) ;
        if (flag == 1) {
            stdout_results[stdout_num] = i ;
            stdout_num++ ;
        }
        else if (flag == 2) { 
            stderr_results[stderr_num] = i ;
            stderr_num++ ;
        }
        else if (flag == -1) {
            nonzero_ret_results[nonzero_ret_num] = i ;
            nonzero_ret_num++ ;
        }
    }

    printf("STDOUT: %d\nSTDERR: %d\nCRASH: %d\n", stdout_num, stderr_num, nonzero_ret_num) ;
#ifdef DEBUG
    printf("STDOUT executions: \n") ;
    for (int j = 0; j < stdout_num; j++) {
        printf("%s/output%d\n", dir_name, stdout_results[j]) ;
    }
#endif
}

int
main ()
{
    srand(time(NULL)) ; 

    // invoking_extern_prog() ;
    long_running_fuzzing() ;
}
