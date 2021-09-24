#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// #define DEBUG

typedef struct _seed {
    char * seed_filename ;
    // char * data ; 
    int energy ;
} seed_t ;

void
init_energy (seed_t * seeds, int seeds_cnt)
{
    for (int i = 0; i < seeds_cnt; i++) {
        seeds[i].energy = 1 ;
    }
}

void
get_normalized_energy (float * norm_energies, seed_t * seeds, int seeds_cnt) 
{
    if (norm_energies == 0x0) {
        perror("get_normalized_energy: invalid norm_energies(0x0)") ;
        exit(1) ;
    }

    int sum_energy = 0 ;
    for (int i = 0; i < seeds_cnt; i++) {
        sum_energy += seeds[i].energy ;
    }

    for (int i = 0; i < seeds_cnt; i++) {
        norm_energies[i] = (float)seeds[i].energy / sum_energy ;
    }

#ifdef DEBUG
    printf("NORMALIZED ENERGIES\n") ;
    for (int i = 0; i < seeds_cnt; i++) {
        printf("[%d] %f\n", i, norm_energies[i]) ;
    }
#endif
}

int
choose_seed (seed_t * seeds, int seeds_cnt)
{  
    int sum_energy = 0 ;

    for (int i = 0; i < seeds_cnt; i++) {
        sum_energy += seeds[i].energy ;
    }

    int choice = rand() % sum_energy ;

    int accumulate = 0 ;
    for (int i = 0; i < seeds_cnt; i++) {
        accumulate += seeds[i].energy ;
        if (choice < accumulate) {
            return i ;
        }
    }

    return 0 ;
}

int 
main ()
{
    srand(time(NULL)) ;

    int seeds_cnt = 3 ;
    int trials = 1000 ;

    seed_t * seeds = (seed_t *) malloc(sizeof(seed_t) * seeds_cnt) ;
    init_energy(seeds, seeds_cnt) ;

    int * hits = (int *) malloc(sizeof(int) * seeds_cnt) ;
    for (int i = 0; i < seeds_cnt; i++) {
        hits[i] = 0 ;
    }

    for (int i = 0; i < trials; i++) {
        int choice = choose_seed(seeds, seeds_cnt) ;
        hits[choice]++ ;
    }

    for (int i = 0; i < seeds_cnt; i++) {
        printf("[%d] %d hits\n", i, hits[i]) ;
    }

    free(seeds) ;
}