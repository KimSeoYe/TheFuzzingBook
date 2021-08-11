#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../runner/Runner.h"
#include "../runner/PrintRunner.h"

void
print_runner_test ()
{
    Runner print_runner ;   // naming...
    PrintRunnerInit(&print_runner) ;
    
    char input[] = "Hello\0 World\n" ;
    print_runner.run(&print_runner, input, sizeof(input)) ;

    write(1, print_runner.input, print_runner.input_size) ;

    print_runner.free_input(print_runner) ;
}

int
main ()
{
    print_runner_test() ;
}