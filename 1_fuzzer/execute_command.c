void
execute_command (char * program, char * path)
{
    char * command = (char *) malloc(sizeof(char) * (strlen(path) + 32)) ;  // TODO. use local variable 
    sprintf(command, "cat %s | %s", path, program) ;    // TODO. modify -> "bc <file path>" -> how to check & count error?

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
    // TODO. handle error !

    free(command) ;
}