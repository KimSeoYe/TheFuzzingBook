#include <stdio.h>
#include <stdlib.h>

#include "../runner/Runner.h"
#include "../runner/PrintRunner.h"

void
print_runner_test ()
{
    Runner print_runner ;   // naming...
    PrintRunnerInit(&print_runner) ;
    
    char input[] = "Hello\0 World\n" ;
    print_runner.run(&print_runner, input, sizeof(input)) ;
    
    for (int i = 0; i < print_runner.result.input_size; i++) {
        putc(print_runner.result.input[i], stdout) ;
    }

    print_runner.free_input(print_runner) ;
}

int
main ()
{
    print_runner_test() ;
}