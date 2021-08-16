
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#define DEBUG

int
main ()
{  
    char input[1024] ;
    char c ;
    int input_len ;
    for (input_len = 0; (c = getchar()) != EOF && input_len < 1023; input_len++) {
        input[input_len] = c ;
    }
    input[input_len] = 0x0 ;
    
#ifdef DEBUG
    printf("Input: %s", input) ;
#endif

    // case1
    char pattern[] = "\\D" ;
    char * find = 0x0 ;
    find = strstr(input, pattern) ; // ...
    
    if (find != 0x0 && strlen(find) != strlen(pattern)) {   // strstr...
        char * following = find + strlen(pattern) ; 
    #ifdef DEBUG
        printf("> %d\n", (int) *following) ;
    #endif

        // assert(*following > 31 && *following < 128) ;
        if (*following < 32 || *following > 127) {
            perror("\\D") ;
            exit(1) ;
        }
    }

    // case2
    for (int i = 0; i < strlen(input); i++) {
        // assert((unsigned int)input[i] < 128 || input[i + 1] != '\n') ;
        if ((unsigned int)input[i] >= 128 && input[i + 1] == '\n') {
            perror("8bit") ;
            exit(1) ;
        }
    }

    // case3
    for (int i = 0; i < input_len - 1 ; i++) {
        // assert(strcmp(input + i, ".\n") != 0) ;
        if (strcmp(input + i, ".\n") == 0) {
            perror("single .") ;
            exit(1) ;
        }
    }

    return 0 ;
}
