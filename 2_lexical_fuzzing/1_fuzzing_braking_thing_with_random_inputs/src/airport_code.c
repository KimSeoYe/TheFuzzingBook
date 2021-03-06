#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "../include/airport_code.h"

Node *
make_node (char * code, char * city)
{
    Node * new_node = (Node *) malloc(sizeof(Node) * 1) ;
    strcpy(new_node->code, code) ;
    strcpy(new_node->city, city) ;
    new_node->next = 0x0 ;
    return new_node ;
}

Node *
make_header ()
{
    return make_node("", "") ;
}

void
append (Node * airport_code, char * code, char * city)
{
    Node * new_node = make_node(code, city) ;

    if (airport_code->next == 0x0) {
        airport_code->next = new_node ;
        return ;
    }

    Node * itr ;
    for (itr = airport_code->next; itr->next != 0x0; itr = itr->next) ;
    itr->next = new_node ;
    
    return ;
}

void
free_list (Node * airport_code)
{
    Node * itr ;
    Node * tmp ;
    for (itr = airport_code ; itr != 0x0; ) {
        tmp = itr->next ;
        free(itr) ;
        itr = tmp ;
    }

    return ;
}

void
print_list (Node * airport_code)
{
    Node * itr ;
    for (itr = airport_code->next; itr != 0x0; itr = itr->next) {
        printf("[%s] %s\n", itr->code, itr->city) ;
    }
    return ;
}

int
is_exist_code (Node * airport_code, char * code)
{
    Node * itr ;
    for (itr = airport_code->next; itr != 0x0; itr = itr->next) {
        if (strcmp(itr->code, code) == 0) return 1 ;
    }
    return 0 ;
}

int
find_code (Node * airport_code, char * code, char * ret)
{
    Node * itr ;
    for (itr = airport_code->next; itr != 0x0; itr = itr->next) {
        if (strcmp(itr->code, code) == 0) {
            strcpy(ret, itr->city) ;
            return 1 ;
        }
    }

    return 0 ;
}

int 
code_rep_ok (char * code) 
{
    if (strlen(code) != 3) {
        printf("Airport code must have three characters: '%s'\n", code) ;
        exit(1) ;   // Q.
    }

    for (int i = 0; i < 3; i++) {
        if (!isalpha(code[i])) {
            printf("Non-letter in airport code: '%s'\n", code) ;
            exit(1) ;
        }
        if (!isupper(code[i])) {
            printf("Lowercase letter in airport code: '%s'\n", code) ;
            exit(1) ;
        }
    }

    return 1 ;
}

int
airport_codes_rep_ok (Node * airport_code)
{
    Node * itr ;
    for (itr = airport_code->next; itr != 0x0; itr = itr->next) {
        assert(code_rep_ok(itr->code)) ;
    }
    return 1 ;
}

void
add_new_airport (Node * airport_code, char * code, char * city)
{
    assert(code_rep_ok(code)) ;
    assert(airport_codes_rep_ok(airport_code)) ;    // Q. overlaped assert ?
    append(airport_code, code, city) ;
    assert(airport_codes_rep_ok(airport_code)) ;
}