
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#define DEBUG

char *
find_pattern(char * input, int input_len, char * pattern)
{
    char * find = 0x0 ;
    int flag = 0 ;

    for (int i = 0; i < input_len; i++) {
        if (input[i] == pattern[0]) {
            for (int j = 1; j < strlen(pattern); j++) {
                if(input[i+j] == pattern[j]) flag = 1 ;
                else {
                    flag = 0 ;
                    break ;
                }
            }
            if (flag) {
                find = input + i ;
                break ;
            }
        }
    }

    return find ;
}

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
    find = find_pattern(input, input_len, pattern) ;
    
    if (find != 0x0) {
        char * following = find + strlen(pattern) ; 
    #ifdef DEBUG
        printf("> %d\n", (int) *following) ;
    #endif

        if (*following < 32 || *following > 127) {
            perror("\\D") ;
            exit(1) ;
        }
    }

    // case2
    for (int i = 0; i < input_len; i++) {
        if (((int)input[i] >= 128 || (int)input[i] < 0) && input[i + 1] == '\n') {
        #ifdef DEBUG
            printf("%u\n", input[i]) ;
        #endif
            perror("8bit") ;
            exit(1) ;
        }
    }

    // case3
    for (int i = 0; i < input_len ; i++) {
        if (memcmp(input + i, ".\n", 2) == 0) {
            perror("single .") ;
            exit(1) ;
        }
    }

    return 0 ;
}
