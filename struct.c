

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
   struct *next;  
};  


/*  Do a function to set the values of the struct  */  
dict insert(char *key, char *data) {            
    arr[x].key = key;
    arr[x].data = data;     
    return dict; 
} 
            

/*  Find data, given a key in the dict  */     
char find(char *key) {  
    if arr.key == NULL { 
       printf("Error: key not found"); 
    } 
    else { 
       return arr.data; 
    } 
} 

                              

/*  Main   */  
int main(void)  
{ 
         
/*  Store some data  */  
insert("foo", "123"); 
insert("bar", "456"); 
insert("baz", "789"); 

/*  Find some data  */  
find("bar");   
    
return 0; 

}  

    




      


    



