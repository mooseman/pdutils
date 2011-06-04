

/*  mylist.c  */  


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h> 


typedef struct node { 
   int val;    
   struct node *next;     
} list; 


void init(list *l) 
{ 
   list *nl = malloc(sizeof(list));   
   nl->val = nl->next = NULL;  
   list *head = nl->val;           
}     


list *add_val(list *l, int myint)
{         
   list *nl = malloc(sizeof(list)); 
   
   
      
   l->next = nl; 
   nl->val = myint; 
   nl->next = NULL; 
   
   return nl;      
}     


void print_list(list *l)
{     
  while (l != NULL)   
    printf("%d", l->val); 
    l = l->next ;         
}     





int main(int argc, char *argv[])
{
    
 list a;     
 a.val = 42;  
 a.next = NULL; 
  
 add_val(&a, 5); 
 add_val(&a, 7);  
 add_val(&a, 10); 
 int i; 
 
 for(i=0; i<4; i++)    
  {
     printf("Value: %d \n", a.val );  
  }
      
 return 0; 
    
}










