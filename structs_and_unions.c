

/* structs_and_unions.c                         */ 
/* Playing around with structs and unions.      */ 
/* This code is released to the public domain.  */ 
/* "Share and enjoy...."  ;)                    */  


#include <stdio.h>
#include <stdlib.h> 
#include <stddef.h> 
#include <string.h>
#include <ctype.h> 
#include <malloc.h> 


/*  A union for token values.  */ 
typedef union tok_v { 
       char  *string_val; 
       int   int_val; 
       float float_val; 
       char  char_val;       
} value;    


/*  Tokens  */  
typedef struct tok_t { 
     char     *type; 
     value    myvalue;             
} token;   



int main() 
{ 
 
  token t ; 
  
  t.type = "Integer" ; 
  t.myvalue.int_val = 123 ; 
  printf("Type is : %s and value is : %d \n", t.type, t.myvalue.int_val);  
   
  t.type = "Now it's a float!" ; 
  t.myvalue.float_val = 42.567 ; 
  /*  The "%.3f" format rounds to 3 decimal places.  */  
  /*  Using %.2f will round to two places, giving you 42.57.  */    
  printf("Type is : %s and value is : %.3f \n", t.type, t.myvalue.float_val);   
   
  t.type = "Now it is a string!" ; 
  t.myvalue.string_val = "This is very cool..... :) "; 
  printf("Type is : %s and value is : %s \n", t.type, t.myvalue.string_val);   
  
  t.type = "Finally, a character!" ; 
  t.myvalue.char_val = '*' ; 
  printf("Type is : %s and value is : %c \n", t.type, t.myvalue.char_val);   
  
   
  /* Now, a pointer to a struct.  */  
  
  /*
  token t2 ; 
  token *t2ptr ; 
  t2ptr = &t2 ; 
  
  t2ptr->type = "Yet another string...." ; 
  t2ptr->myvalue->string_val = "Here it is!" ; 
  printf("Type is : %s and value is : %s \n", t2ptr->type, t2ptr->value->string_val); 
  
  t2ptr->type = "Now, an integer." ; 
  t2ptr->myvalue->int_val = 42 ; 
  printf("Type is : %s and value is : %d \n", t2ptr->type, t2ptr->value->int_val);    
  */    
    
     
  return 0;
 
}  










