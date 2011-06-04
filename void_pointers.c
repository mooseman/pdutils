

/*  void_pointers.c  */  
/*  Playing around with void pointers and structs.  */ 
/*  This code is released to the public domain.  */ 
/*  "Share and enjoy..."  ;)                     */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef struct 
{ 
  char name[30]; 
  int  intval; 
  char charval[40];      
} data;      


/*  A data "object". This has a value.  */  
void makedata(void *val) 
{    
   data *dp = (data*)val ; 
   printf("%s \n%d \n%s \n", dp->name, dp->intval, dp->charval);                  
}  


int main()
{
    
	data d;  
      
	strcpy(d.name, "Just a test");
    d.intval = 123;  
	strcpy(d.charval, "More stuff");
    
	makedata(&d);
                
	return 0;
    
}









