#include <stdio.h>
#include <stdlib.h>

#include "../runner/Runner.h"
#include "../runner/PrintRunner.h"

void
print_runner_test ()
{
    Runner print_runner ;
    PrintRunnerInit(&print_runner) ;
    
    char input[] = "Hello\0 World\n" ;
    print_runner.run(&print_runner.result, input, sizeof(input)) ;
    if (print_runner.result.input != 0x0)
        free(print_runner.result.input) ;
}

int
main ()
{
    print_runner_test() ;
}