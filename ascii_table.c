

/*  ascii_table.c  */  
/*  Draw an ascii table. This could be used for  */ 
/*  crosstabs.  */  
/*  This code is released to the public domain.  */  
/*  "Share and enjoy...."  ;)    */  



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <ctype.h> 


void hline(int width)
{  
   char *cnr; 
   char *mid; 
   
   cnr = '+' ; 
   mid = '-' ; 
   
   line = strcat(cnr, mid*width, cnr);      
   printf("%s", line); 
} 


void vline(int height)
{  
   char *cnr; 
   char *mid; 
   
   cnr = '+' ; 
   mid = '|' ; 
   
   line = strcat(cnr, mid*height, cnr);      
   printf("%s", line); 
} 
   

void table(int rows, int cols)  {  
    
  /*  Code to draw table   */  
  
}      



    
int main(void) 
{ 
  
  int rows; 
  int cols; 
  
  puts("Number of rows :"); 
  scanf("%d", rows); 
    
  puts("Number of cols :"); 
  scanf("%d", cols);   
    
  puts("Here is your table! \n");  
    
  table(rows, cols);  
           
  return 0; 

} 






 
