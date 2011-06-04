
/*  followed_by.c                                     */  
/*  A utility which looks for a given token, and then */ 
/*  checks whether it is immediately followed by      */ 
/*  a given number of other tokens.                   */     
/*  This code is released to the public domain.       */
/*  "Share and enjoy..."  ;)                          */  


#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>

#define ARRAYLEN 10  


/* A function to compare two arrays for equality. */ 
/* This will be the second part of the followed-by test. */ 
int comp_arrays(char arr1[ARRAYLEN], char arr2[ARRAYLEN]) 
{ 
   int i;       
   
   for (i=0; i<ARRAYLEN; i++) 
   { 
     if (!strcmp(arr1[i], arr2[i]) ) 
                
       
   }      
    
    
}       


int followed_by(char *tok,  char tokarray[]) 
{ 
    
    
    
}     




int main()
{
    
    int myarray[] = {32, 67, 25, 48, 5, 17, 23}; 
    
    int length = sizeof(myarray) / sizeof(*myarray);    
    
    printf("length of myarray is %d \n", length); 
    
    return 0; 
    
}





