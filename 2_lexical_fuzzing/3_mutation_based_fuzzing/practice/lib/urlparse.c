#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#define URL_MAX 2048 // URLs over 2,000 characters will not work in the most popular web browsers. (https://web.archive.org/web/20190902193246/https://boutell.com/newfaq/misc/urllength.html)

// Q. sizes ?
typedef struct url {
    char scheme[64] ;
    char netloc[128] ;
    char path[512] ;
    char query[512] ;
    char fragment[512] ;
} url_t ;

int
urlparse (url_t * result, char * input)
{
    regex_t preg ;
    char * pattern = "^([a-z]+)://([a-z0-9.]+)/?([a-z0-9./]*)\\?\?([a-z0-9=]*)#?([a-z]*)" ;
    size_t nmatch = 6 ;
    regmatch_t pmatch[6] ;

    if (regcomp(&preg, pattern, REG_EXTENDED) != 0) {
        perror("urlparse: regcomp") ;
        exit(1) ;
    }

    if (regexec(&preg, input, nmatch, pmatch, 0) != 0) {
        perror("urlparse: regexec") ;
        exit(1) ;
    }

    printf("%s\n", input + pmatch[0].rm_so) ;

    memcpy(result->scheme, input + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so) ;
    result->scheme[pmatch[1].rm_eo - pmatch[1].rm_so] = 0x0 ;

    memcpy(result->netloc, input + pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so) ;
    result->netloc[pmatch[2].rm_eo - pmatch[2].rm_so] = 0x0 ;

    memcpy(result->path, input + pmatch[3].rm_so, pmatch[3].rm_eo - pmatch[3].rm_so) ;
    result->path[pmatch[3].rm_eo - pmatch[3].rm_so] = 0x0 ;

    memcpy(result->query, input + pmatch[4].rm_so, pmatch[4].rm_eo - pmatch[4].rm_so) ;
    result->query[pmatch[4].rm_eo - pmatch[4].rm_so] = 0x0 ;

    memcpy(result->fragment, input + pmatch[5].rm_so, pmatch[5].rm_eo - pmatch[5].rm_so) ;
    result->fragment[pmatch[5].rm_eo - pmatch[5].rm_so] = 0x0 ;

    return 0 ;
}

int
main ()
{
    url_t result ;
    urlparse(&result, "https://www.naver.com/path?query#frag") ;
    printf("%s\n%s\n%s\n%s\n%s\n", result.scheme, result.netloc, result.path, result.query, result.fragment) ;

    return 0 ;
}