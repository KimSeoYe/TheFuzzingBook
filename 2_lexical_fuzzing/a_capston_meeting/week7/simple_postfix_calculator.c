#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_CAPACITY 100

typedef 
   struct {
      void * buffer ;
      int capacity ;
      int top ;
   } 
   stack ;

int is_empty (stack * st) 
{
   return (st->top == 0) ;
}

int is_full (stack * st) 
{
   return (st->top == st->capacity) ;
}

stack * 
create_stack () 
{
   stack * st = malloc(sizeof(stack)) ;

   st->capacity = MAX_CAPACITY;
   st->top = 0 ;
   st->buffer = calloc(MAX_CAPACITY, sizeof(int)) ;
   return st ;
}


void delete_stack (stack * st) 
{
   if (st->buffer != 0x0)

      free(st->buffer) ;

   free(st) ;
}



int push (stack * st, int elem)
{   
   //stack이 가득 찰 경우
   if (is_full(st))
      return 1 ;

   memcpy(st->buffer + ((st->top) * (sizeof(int))), &elem,sizeof(int)) ;
   
   st->top += 1 ;
   return 0 ;
}



int pop (stack * st, int* elem)
{   
   //stack이 빈 경우
   if (is_empty(st)) 
      return 1 ;
   
   memcpy(elem, st->buffer + (st->top - 1) * (sizeof(int)), sizeof(int)) ;
   st->top -= 1 ;
   return 0;
}
int get_size (stack * st) 
{
   return st->top ;
}



int get_element (stack * st, int index, int* elem)
{
   if (st->top <= index)
      return 1 ;

   memcpy(elem, st->buffer + index * (sizeof(int)), (sizeof(int)));
   return 0 ;
}

void
print_stack (stack * st)
{
   int i ;
   for (i = 0 ; i < get_size(st) ; i++) {
      int num ;
      get_element(st, i, &num) ;
      printf("[%d]", num) ;
   }
   printf(".\n") ;
}


int
main (){
   stack * st ;
   st = create_stack() ;

   char tok;
   int flag = 0;

   do{
      tok = getc(stdin) ;

      if(tok == ' ') 
      {
         continue;
      }else if(isdigit(tok))
      {
         int num = atoi(&tok) ;
         push(st, num) ;

      }else if(tok == '+' || tok == '-' || tok == '*' || tok == '/' || tok == '^' || tok == '%'){
         if(get_size(st) >= 2){
            int n1, n2, res ;
            pop(st, &n2) ;
            pop(st, &n1) ;
            if(tok == '+'){
               res = n1 + n2;
            }else if(tok == '-'){
               res = n1 - n2;
            }else if(tok == '*'){
               res = n1 * n2 ;
            }else if(tok == '/'){
               res = n1 / n2 ;
            }else if(tok == '^'){
               if(n2 >= 0)
               {
                  res = 1;
                  for(int i = 0; i < n2 ; i++)
                  {
                     res = res*n1 ;
                  }
               }
            }else if(tok == '%'){
               res = n1 % n2;
            }
            if(flag != 1){
               push(st, res) ;
            }
         }else{
            flag = 1;
         }
      }else{
         if( tok != ';'){
            flag = 1;
         }
      }

      if(flag == 1){
         break;
      }
   }while(tok != ';');

   if(flag == 0){
      int result ;
      pop(st, &result) ;
      printf("%d\n", result) ;
   }else{
      printf("invalid\n");
   }
   
   delete_stack(st) ;
   return 0 ;
}