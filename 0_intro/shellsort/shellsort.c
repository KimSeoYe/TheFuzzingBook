int
shellsort (int * elems, int n, int * sorted_elems)
{
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

    return n ;
}