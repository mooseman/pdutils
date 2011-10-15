

/* nthword.c  */  

/* Print the nth "word" in a string.                          */ 
/* This may possibly be useful as a substitute for strtok().  */  
/* This code is released to the public domain. */ 
/* "Share and enjoy...."  ;)    */  


#include <stdio.h>
#include <string.h>
#include <ctype.h> 

void nthword(char *str, int num) 
{ 
  char ret[80] ; 
          
  int i=0;     
  
  /* Counter for the string number. */ 
  int strnum=0;    
  
  /* Pointer to character before *str. */ 
  /* Used to find first character of string. */  
  char *prev; 
  prev = NULL;   
          
  while ( *str != '\0' ) 
  {                      
     if ( *str != ' ' )               
     {          
        if ( ( prev == NULL ) || ( *prev == ' ' ) ) strnum++; 
        if ( strnum == num ) {ret[i] = *str; i++;}          
        str++; 
        prev = str - 1;         
     } 
     else if ( *str == ' ' ) 
     {                  
        str++; 
        prev = str - 1;                         
     }   
  }  
  
  ret[i] = '\0' ;     
  printf("%s \n", ret); 
                       
} 


int main() 
{ 
 
 char *test = "select * from mytable where city = \"Auckland\" ;" ;     
           
 nthword(test, 1) ;           
               
 nthword(test, 2) ;    
 
 nthword(test, 3) ;                                   
                       
 return 0;
 
 
}  



