#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

int *
shellsort (int * elems, int n)
{
    int * sorted_elems = (int *) malloc (sizeof(int) * n) ;
    for (int i = 0; i < n; i++) {
        sorted_elems[i] = elems[i] ;
    }

    int gaps[] = { 701, 301, 132, 57, 23, 10, 4, 1 } ;
    for (int gap = 0; gap < 8; gap++) {
        for (int i = gap; i < n; i++) {
            int temp = sorted_elems[i] ;
            int j = i ;
            while (j >= gap && sorted_elems[j - gap] > temp) {
                sorted_elems[j] = sorted_elems[j - gap] ;
                j -= gap ;
            }
            sorted_elems[j] = temp ;
        }
    }
    
    return sorted_elems ;
}

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
    int case1[] = { 3, 2, 1 } ;
    int case1_sol[] = { 1, 2, 3 } ;
    assert(is_same_array(shellsort(case1, 3), case1_sol, 3, 3)) ;

    int case2[] = { 2, 3, 1 } ;
    int case2_sol[] = { 1, 2, 3 } ;
    assert(is_same_array(shellsort(case2, 3), case2_sol, 3, 3)) ;

    int case3[] = { 1, 2, 3 } ;
    int case3_sol[] = { 1, 2, 3 } ;
    assert(is_same_array(shellsort(case3, 3), case3_sol, 3, 3)) ;

    int case4[] = {2, 1, 2, 2} ;
    int case4_sol[] = {1, 2, 2, 2} ;
    assert(is_same_array(shellsort(case4, 4), case4_sol, 4, 4)) ;

    int case5[] = { 0 } ;
    int case5_sol[] = { 0 } ;
    assert(is_same_array(shellsort(case5, 1), case5_sol, 1, 1)) ;

    int case6[] = { } ;
    int case6_sol[] = { } ;
    assert(is_same_array(shellsort(case6, 0), case6_sol, 0, 0)) ;

    // Q. any other case...?
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
is_permutation (int * a, int * b, int n)
{
    // Q. how to check the length of array...?

    for (int elem = 0; elem < n; elem++) {
        int a_count = 0 ;
        int b_count = 0 ;

        for (int i = 0; i < n; i++) {
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
        int length = rand() % 10 ;
        int * list = (int *) malloc(sizeof(int) * length) ;

        for (int i = 0; i < length; i++) {
            list[i] = rand() % 10 ;
        }

        int * sorted_list = shellsort(list, length) ;
        assert(is_sorted(sorted_list, length)) ;
        assert(is_permutation(list, sorted_list, length)) ;

        free(list) ;
        free(sorted_list) ;
    }
}

int
main ()
{
    part1_manual_test_cases() ;

    // int test[] = { 3, 5, 9 } ;
    // printf("%d\n", is_sorted(test, 3)) ;

    // int a[] = { 3, 2, 1 } ;
    // int b[] = { 1, 3, 2 } ;
    // printf("%d\n", is_permutation(a, b, 3, 3)) ;

    part2_random_inputs() ;

    return 0 ;
}