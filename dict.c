

/*  A simple dictionary in C.   */  
/*  We also define an "in" function here, to see if a value is 
    stored in a dict. */ 
/*  This code is released to the public domain.  */ 
/*  "Share and enjoy...."  ;)   */  


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 

struct dict {    
   char *key ;
   char *data ;
};    

struct dict mydict[50] ;  
    
    
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

    




      


    



