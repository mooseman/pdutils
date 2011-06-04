

/*  toktest.c  */  





#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>


/* To do - Try creating a struct for each token.  */ 
/* This would have a token type and value.        */   

enum keywords { SELECT, FROM, WHERE, AND, OR, NOT, 
  IN, IS, _NULL_ };  

enum toktype { KEYWORD, VARNAME, INTEGER, STRING, 
  OP, COMMA, LPAREN, RPAREN, SEMI }; 
  
/* A function to extract a string  */ 
char *getstring(char ch) 
{ 
  
  char *mystr="" ; 
  while (ch != '"') 
  { 
      strcat(mystr, ch);           
  }         
  return mystr;            
}       


/*  A function to extract data up to a delimiter */ 
char *getstuff(char *mystr, char delim)
{ 
  char *retval; 
  int i=0; 
  
  while (ch != delim) 
  {
    retval[i] = ch;     
    i++;   
  }    
  return retval;           
}    


toktype test(char *testval)
{ 
   if (testval == KEYWORD) return KEYWORD; 
   else if (testval == VARNAME) return VARNAME; 
   else if (testval == INTEGER) return INTEGER; 
   /* ......  */ 
}      







int main() { 

char *mystuff[80] ;    
int i=0;  
char *token ; 
  
char text[80] = "Peter Piper picked a peck of pickled peppers." ;  

token = strtok(text, " ");
       
 while (token != NULL) {

    printf("Token = %s\n", token);
    mystuff[i] = token; 
    
    token = strtok(NULL, " ");    
        
    printf("Array element %d is %s\n", i, mystuff[i]);  
    
    i++; 
            
    }      
        
  return 0;   
  
}








 

