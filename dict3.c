

/*  A simple dictionary in C.   */  
/*  We also define an "in" function here, to see if a value is 
    stored in a dict. */ 
/*  This code is released to the public domain.  */ 
/*  "Share and enjoy...."  ;)   */  


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 


/*  Define the array size  */  
#define arrsize 5  


/*  Our "dictionary" consists of an array of these elements.  */  
typedef struct dict {    
   char *key;
   char *data;   
}arr[arrsize] ;    

/*  Function to return an array of structs. It takes no arguments */ 
struct dict create(void);




     




    
    
/*  Main   */  
int main(void)  
{ 
    
 
mydict newdict ;    
    
    
/*  Store some data  */  
newdict[0].key = "foo" ; 
newdict[0].data = "123" ; 

newdict[1].key = "bar" ; 
newdict[1].data = "456" ;

newdict[2].key = "baz" ; 
newdict[2].data = "789" ; 
 
    
/*  Retrieve some data   */  

int i ; 

for(i=0; i<3; i++)  { 
   printf("mydict %d has key %s and value %s \n", 
      i, newdict[i].key, newdict[i].data); 
} 
    
return 0; 

}  

    




      


    



