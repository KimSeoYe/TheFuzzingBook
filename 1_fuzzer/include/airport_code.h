#define CITY_MAX 128

typedef struct _Node {
    char code[4] ;
    char city[CITY_MAX] ;
    struct _Node * next ;
} Node ;

Node * make_node (char * code, char * city) ;

Node * make_header () ;

void append (Node * airport_code, char * code, char * city) ;

void free_list (Node * airport_code) ;

void print_list (Node * airport_code) ;

int is_exist_code (Node * airport_code, char * code) ;

int find_code (Node * airport_code, char * code, char * ret) ;