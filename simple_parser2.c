

/*  simple_parser.c  */  

/*                                                     
  This is a very simple parser which uses strtok() to parse the 
  p entered by the user. It breaks up the p into tokens and 
  takes action accordingly.    
  This code is released to the public domain.                   
*/ 


#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

    
int main(void) 
{ 
    
  char *p ; 
  
  p = strtok("The quick brown fox", " ");   
  p = strtok('\0', " "); 
  printf(p); 
  
  /*
  do {     
  p = strtok('\0', " "); 
  if(p) printf("|%s", p);                                                   
  }  while(p);      
    
  */    
            
  return 0; 

} 


     


