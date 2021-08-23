#include <stdio.h>
#include <string.h>

#define URL_MAX 2048 // URLs over 2,000 characters will not work in the most popular web browsers. (https://web.archive.org/web/20190902193246/https://boutell.com/newfaq/misc/urllength.html)

// Q. sizes ?
typedef struct url {
    char scheme[32] ;
    char netloc[128] ;
    char path[128] ;
    char query[256] ;
    char fragment[256] ;
} url_t ;

int
urlparse (url_t * result, char * input)
{
    char copied_input[URL_MAX] ;
    strcpy(copied_input, input) ;
    
    char * toked = 0x0 ;
    if (strstr(copied_input, "://") != 0x0) {
        toked = strtok(copied_input, "://") ;
        strcpy(result->scheme, toked) ;
    }
    else {
        perror("urlparse: Invalid url") ;
        return -1 ;
    }
    
    // continue strtok
    
    return 0 ;   
}

int
main ()
{

    return 0 ;
}