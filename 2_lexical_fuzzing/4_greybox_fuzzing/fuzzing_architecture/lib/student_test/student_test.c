#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 

char * substring(char * email, int start, int count)  { 
    char * substr = (char *)malloc(sizeof(char) * 128); 
    int idx = 0 ;

    for (int i = start ; i < start + count ; i++) {
        substr[idx++] = email[i] ; 
    }   
    substr[idx] = '\0'; 

    return substr; 
}

int main(int argc, char * argv[]) {
    char email[128] ;
    char * id = NULL ;
    int check = 1  ; 
    int idx = 0 ;

    strcpy(email, argv[1]) ;
    printf("INPUT: %s\n", email) ;

    for (int i = 0 ; i < 8; i++) {
        if (email[i] - '0' < 0  && email[i] - '0' > 9) {
            check = 0 ; 
            break ;
        }
    }

    if (check == 1) {
        id = substring(email, 0, 3); 
        int student_id = atoi(id); 
        
        if (student_id >= 195 && student_id <= 221) {
            printf("Student ID: %d\n", student_id); 
        } else { 
            fprintf(stderr, "Studnet ID is not valid!\n"); 
            return 1 ;
        }
    }
    free(id) ;
    return 0;
}