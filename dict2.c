

/*  A simple dictionary in C.   */  
/*  We also define an "in" function here, to see if a value is 
    stored in a dict. */ 
/*  This code is released to the public domain.  */ 
/*  "Share and enjoy...."  ;)   */  


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 

/*  Our "dictionary" consists of an array of these elements.  */  
typedef struct dict {    
   char *key;
   char *data;
   struct dict* next; 
} dict;    


struct mydict {  
   int size;      /*  Size of the dict.  */  
   int n;         /*  Number of elements currently used.  */  
   dict** table; 
};  


/*  Create a dictionary  */  
mydict create_dict(int size) 
{ 
    mydict* table = malloc(sizeof(mydict));
    dict** adict;            
    int i; 
    
    for(i=0; i<size; i++) {  
        adict[i] = NULL;
        table->key = NULL;
        table->data = NULL;
    }  
    
    return table;  
} 

        
/*  Destroy a dict.  */  
void destroy_dict(mydict* table) {  
  int i; 
  
  if (table == NULL)
        return;
  
  for (i = 0; i < table->size; ++i) {
        dict* adict;

        for (dict = (table->table)[i]; adict != NULL;) {
            dict* prev = adict;

            free(adict->key);
            adict = adict->next;
            free(prev);
        }
    }

    free(adict->adict);
    free(adict);         
};     
    
    
     




    
    
/*  Main   */  
int main(void)  
{ 
    
/*  Store some data  */  
mydict[0].key = "foo" ; 
mydict[0].data = "123" ; 

mydict[1].key = "bar" ; 
mydict[1].data = "456" ;

mydict[2].key = "baz" ; 
mydict[2].data = "789" ; 
 
    
/*  Retrieve some data   */  

int i ; 

for(i=0; i<3; i++)  { 
   printf("mydict %d has key %s and value %s \n", 
      i, mydict[i].key, mydict[i].data); 
} 
    
return 0; 

}  

    




      


    



