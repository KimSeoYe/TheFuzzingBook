#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "fuzzer.h"
#include "fileio.h"

void
execute_command (char * program, char * path)
{
    char * command = (char *) malloc(sizeof(char) * (strlen(path) + 32)) ;
    sprintf(command, "cat %s | %s", path, program) ;

    FILE * fp = popen(command, "r") ;
    if (fp == 0x0) {
        perror("popen") ;
        return ;
    }

    char buf[1024] ;
    while (fgets(buf, 1024, fp) != 0x0) {
        printf("%s", buf) ;
    }

    int exit_status = pclose(fp) ;
    printf("exit_status : %d\n", exit_status) ;

    free(command) ;
}

void
invoking_extern_prog ()
{
    char * program = "bc" ;

    char dir_name[32] ;
    char path[32] ;
    create_input_files(dir_name, path) ;

    if (write_data(path, "2 + 2\n") == -1)  return ;
    execute_command(program, path) ;
    
    remove_input_files(dir_name, path) ;
}

void
long_running_fuzzing ()
{
    // TODO. 
    char * program = "bc" ;

    char dir_name[32] ;
    char path[32] ;
    create_input_files(dir_name, path) ;

    char * data = fuzzer(MAX_LEN, CHAR_START, CHAR_RANGE) ;
    if (write_data(path, data) == -1)  return ;
    execute_command(program, path) ;

    remove_input_files(dir_name, path) ;
}

int
main ()
{
    invoking_extern_prog() ;
    long_running_fuzzing() ;
}