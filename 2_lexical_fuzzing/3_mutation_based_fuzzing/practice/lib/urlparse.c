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
urlparse (url_t * result, char * url)
{
    regex_t preg ;
    char * pattern = "^([a-z]+)://([a-z0-9.]+)/?([a-z0-9./]*)\\?\?([a-z0-9=]*)#?([a-z]*)" ;
    size_t nmatch = 6 ;
    regmatch_t pmatch[6] ;

    if (regcomp(&preg, pattern, REG_EXTENDED) != 0) {
        perror("urlparse: regcomp") ;
        return -1 ;
    }

    if (regexec(&preg, url, nmatch, pmatch, 0) != 0) {
        perror("urlparse: regexec: Invalid url") ;
        return -1 ;
    }

    printf("%s\n", url + pmatch[0].rm_so) ;

    memcpy(result->scheme, url + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so) ;
    result->scheme[pmatch[1].rm_eo - pmatch[1].rm_so] = 0x0 ;

    memcpy(result->netloc, url + pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so) ;
    result->netloc[pmatch[2].rm_eo - pmatch[2].rm_so] = 0x0 ;

    memcpy(result->path, url + pmatch[3].rm_so, pmatch[3].rm_eo - pmatch[3].rm_so) ;
    result->path[pmatch[3].rm_eo - pmatch[3].rm_so] = 0x0 ;

    memcpy(result->query, url + pmatch[4].rm_so, pmatch[4].rm_eo - pmatch[4].rm_so) ;
    result->query[pmatch[4].rm_eo - pmatch[4].rm_so] = 0x0 ;

    memcpy(result->fragment, url + pmatch[5].rm_so, pmatch[5].rm_eo - pmatch[5].rm_so) ;
    result->fragment[pmatch[5].rm_eo - pmatch[5].rm_so] = 0x0 ;

    return 0 ;
}

int
http_program (char * url)
{
    // TODO. supported_schemes = ["http", "https"]
    url_t result ;
    urlparse(&result, url) ;
    if (strcmp(result.scheme, "http") != 0 && strcmp(result.scheme, "https") != 0) {
        perror("http_program: scheme must be one of http, https") ;
        exit(1) ;
    }
    
    return 1 ;
}

int
main ()
{
    url_t result ;
    urlparse(&result, "https://www.naver.com/path?q=query#frag") ;
    printf("%s\n%s\n%s\n%s\n%s\n", result.scheme, result.netloc, result.path, result.query, result.fragment) ;

    return 0 ;
}