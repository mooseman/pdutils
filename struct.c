

/*  A simple dictionary in C.   */  
/*  We also define an "in" function here, to see if a value is 
    stored in a dict. */ 
/*  This code is released to the public domain.  */ 
/*  "Share and enjoy...."  ;)   */  


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 


/*  Our "dictionary" consists of a linked list. */  
typedef struct dict {    
   char *key;
   char *data;   
   struct dict *next;  
}list;  


/*  Insert data into the dict.  */  
list *dict_insert(list **l, char *mykey, char *mydata) {            
    if (l == NULL)  return NULL; 
    
    list *n = malloc(sizeof(list)); 
    if (n == NULL) return NULL; 
    
    n->next = *l; 
    *l = n; 
    
    /*  Add the key and data  */  
    n->key = mykey; 
    n->data = mydata; 
    
    return *l ; 
} 


/*  Delete a dict  */  
void dict_delete(list **l) 
{ 
   if (l != NULL && *l != NULL) 
     {  
        list *n = *l; 
        *l = (*l)->next; 
        free(n); 
     } 
}  


/*  Find data, given a key   */  
list **dict_find(list **n, char *mykey) { 
   if (n == NULL)  return NULL; 
   
   while (*n != NULL) 
   { 
      if ( (*n)->key == mykey ) 
         { 
            return n;               
         }     
      n = &(*n)->next;  
   } 
   return NULL; 
} 


/*  Print the contents of a dict.  */  
void dict_print(list *n) {  
    if (n == NULL)  
    { 
       printf("No data in the dict.\n");    
    }      
    
    while (n != NULL) 
    { 
       printf("Dict %p %p %p %p\n", n, n->next, n->key, n->data);
	   n = n->next;
	}
}
 
                               

/*  Main   */  
int main(void)  
{ 
       
  list *n = NULL;      
         
/*  Store some data  */  
dict_insert(&n, "foo", "123"); 
dict_insert(&n, "bar", "456"); 
dict_insert(&n, "baz", "789"); 

/*  Find some data  */  
dict_find(&n, "bar");   
    
dict_print(n);     
    
return 0; 

}  

    




      


    



