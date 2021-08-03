#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "lib/shellsort.h"

int
is_same_array (int * x, int * y, int x_n, int y_n)
{
    if (x_n != y_n) return 0 ;

    for (int i = 0; i < x_n; i++) {
        if (x[i] != y[i]) return 0 ;
    }

    return 1 ;
}

void
part1_manual_test_cases ()
{
    int sorted_elems[10] ;

    int case1[] = { 3, 2, 1 } ;
    int case1_sol[] = { 1, 2, 3 } ;
    shellsort(case1, 3, sorted_elems) ;
    assert(is_same_array(sorted_elems, case1_sol, 3, 3) == 1) ;

    int case2[] = { 2, 3, 1 } ;
    int case2_sol[] = { 1, 2, 3 } ;
    shellsort(case2, 3, sorted_elems) ;
    assert(is_same_array(sorted_elems, case2_sol, 3, 3)) ;

    int case3[] = { 1, 2, 3 } ;
    int case3_sol[] = { 1, 2, 3 } ;
    shellsort(case3, 3, sorted_elems) ;
    assert(is_same_array(sorted_elems, case3_sol, 3, 3)) ;

    int case4[] = { 2, 1, 2, 2 } ;
    int case4_sol[] = { 1, 2, 2, 2 } ;
    shellsort(case4, 4, sorted_elems) ;
    assert(is_same_array(sorted_elems, case4_sol, 4, 4)) ;

    int case5[] = { 0 } ;
    int case5_sol[] = { 0 } ;
    shellsort(case5, 1, sorted_elems) ;
    assert(is_same_array(sorted_elems, case5_sol, 1, 1)) ;

    int case6[] = { } ;
    int case6_sol[] = { } ;
    shellsort(case6, 0, sorted_elems) ;
    assert(is_same_array(sorted_elems, case6_sol, 0, 0)) ;

    // Q. any other case...?

    int case7[] = { 100, 300, -250, 700, -10, 0 } ;
    int case7_sol[] = { -250, -10, 0, 100, 300, 700 } ;
    shellsort(case7, 6, sorted_elems) ;
    assert(is_same_array(sorted_elems, case7_sol, 6, 6)) ;
}

int
is_sorted (int * elems, int n)
{
    for (int i = 0; i < n - 1; i++) {
        if (elems[i] > elems[i+1]) return 0 ;
    }

    return 1 ;
}

int
is_permutation (int * a, int * b, int a_n, int b_n)
{
    // Q. how to check the length of array...?
    if (a_n != b_n) return 0 ;

    for (int elem = 0; elem < a_n; elem++) {
        int a_count = 0 ;
        int b_count = 0 ;

        for (int i = 0; i < a_n; i++) {
            if (a[i] == a[elem]) a_count++ ;
            if (b[i] == a[elem]) b_count++ ;
        }

        if (a_count != b_count) return 0 ;
    }

    return 1 ;
}

void
part2_random_inputs ()
{
    srand(time(NULL)) ;

    for (int c = 0; c < 1000; c++) {   
        int length = rand() % 1000 ;
        int * list = (int *) malloc(sizeof(int) * length) ;

        for (int i = 0; i < length; i++) {
            list[i] = rand() % 1000 ;
        }

        int * sorted_list = (int *) malloc(sizeof(int) * length) ;
        int sorted_length = shellsort(list, length, sorted_list) ;
        assert(is_sorted(sorted_list, length)) ;
        assert(is_permutation(list, sorted_list, length, sorted_length)) ;

        free(list) ;
        free(sorted_list) ;
    }
}

int
main ()
{
    part1_manual_test_cases() ;
    part2_random_inputs() ;

    return 0 ;
}