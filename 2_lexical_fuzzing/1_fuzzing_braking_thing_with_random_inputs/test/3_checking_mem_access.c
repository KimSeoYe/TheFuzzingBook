#include <stdlib.h>
#include <string.h>

// Compile : clang -fsanitize=address -g -o program program.c

int main(int argc, char** argv) {
    /* Create an array with 100 bytes, initialized with 42 */
    char *buf = malloc(100);
    memset(buf, 42, 100);

    /* Read the N-th element, with N being the first command-line argument */
    int index = atoi(argv[1]);
    char val = buf[index];

    /* Clean up memory so we don't leak */
    free(buf);
    return val;
}


/**
 * HeartBleed Bug
 * 
 * in OpenSSL library
 * if the client send the service a string like BIRd (500 letters), the server send BIRD + more payload, which can contains some secret data.
 * 
 * OpenSSL library를 memory sanitizer와 함께 compile 
 * >> fuzzer command로 test를 했을 때, out-of-bounds memory access를 확인하게 됨으로써 발견되었다.
*/