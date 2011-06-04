

/*  toktest.c  */  





#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>
#include "toktest2.h" 


/* To do - Try creating a struct for each token.  */ 
/* This would have a token type and value.        */   

                  
void select(char *tok) 
{ 
  char *token;   
  if (!strcmp(tok, "select")) 
     { 
        token = strtok(NULL, " ");   
        from(token);   
     } 
  else puts("Select error! \n");         
}           


void from(char *tok) 
{ 
  char *token;     
  if (!strcmp(tok, "from")) 
    { 
      token = strtok(NULL, " ");   
      where(token);     
    } 
  else puts("From error! \n");      
}


void where(char *tok) 
{   
  if (!strcmp(tok, "where"))  puts("Success! \n"); 
  else puts("Where error! \n");   
}



int main() { 

char *token ; 
  
char text[80] = "select from where" ;  

token = strtok(text, " ");     
printf("Token = %s\n", token);
select(token);            
   
char text2[80] = "select foo where" ;    
token = strtok(text2, " ");     
printf("Token = %s\n", token);
select(token);            
   
    
return 0;   
  
}








 

