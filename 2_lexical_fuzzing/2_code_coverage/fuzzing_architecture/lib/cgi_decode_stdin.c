#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

#define BUFMAX 1024

int 
is_hex_value (char val)
{
    if ((val - '0' >= 0 && val - '0' < 10) || (val >= 'A' && val <= 'Z') || (val >= 'a' && val <= 'z'))
        return 1 ;
    else return 0 ;
}

int
hex_to_dec (char val_char)
{
    if (!is_hex_value(val_char)) {
        perror("hex_to_dec: is not hex value") ;
        return -1 ;
    }

    int dec = 0 ;
    int val_int = (int) val_char ;
    if (val_int - '0' >= 0 && val_int - '0' < 10) {
        dec = val_int - '0' ;
    }
    if (val_int >= 'A' && val_int <= 'Z') {
        dec = val_int - 'A' + 10 ;
    }
    if (val_int >= 'a' && val_int <= 'z') {
        dec = val_int - 'a' + 10 ;
    }
    return dec ;
}

/**
 * Decode the CGI-encoded string `s`:
 * replace "+" by " "
 * replace "%xx" by the character with hex number xx.
 * Return the decoded string.  Raise `ValueError` for invalid inputs.
*/
int
cgi_decode (char * decoded, char * input)
{
    int idx = 0 ;
    for (int i = 0; i < strlen(input); i++) {   // Q. possible to have 0x0 in the middle ?
        if (input[i] == '+') {
            decoded[idx++] = ' ' ;
        }
        else if (input[i] == '%') {
            if (is_hex_value(input[i + 1]) && is_hex_value(input[i + 2])) {
                int dec_value = hex_to_dec(input[i + 1]) * 16 + hex_to_dec(input[i + 2]) ;
                decoded[idx++] = (char) dec_value ;
                i += 2 ;
            }
            else {
                perror("cgi_decode: Invalid encoding") ;
                return -1 ;
            }
        }
        else if (input[i] == ' ') {
            perror("cgi_decode: Invalid encoding: No whitespace is allowed") ;
            return -1 ;
        }
        else {
            decoded[idx++] = input[i] ;
        }
    }
    decoded[idx] = 0x0 ;

    return 0 ;
}

int
main (int argc, char * argv[])
{
    char * input = (char *) malloc(sizeof(char) * (BUFMAX + 1)) ;
    char c ;
    int input_len = 0 ;
    for (input_len = 0; (c = getchar()) != EOF ; input_len++) {
        if (input_len != 0 && input_len % BUFMAX == 0) {
            input = realloc(input, sizeof(char) * (input_len / BUFMAX + 1)) ;
            if (input == 0x0) {
                perror("realloc") ;
                exit(1) ;
            }
        }
        input[input_len] = c ;
    }
    input[input_len] = 0x0 ;

    char * dst = (char *) malloc(sizeof(char) * (input_len + 1)) ;
    int ret = cgi_decode(dst, input) ;
    if (ret == -1) {
        free(input) ;
        free(dst) ;
        return 1 ;
    }

    printf("%s\n", dst) ;
    free(input) ;
    free(dst) ;
    
    return ret ;
}