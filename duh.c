

/*  duh.c  */  

/*                                                     
  Duh is a "dummy shell". Really just a do-while loop to test 
  reading strings and acting on them.  
  This is eventually aimed at testing a public-domain readline 
  utility.   
  This code is released to the public domain.                   
*/ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <ctype.h> 

void func1(void)
{  
   printf("Hey, you entered foo! \n"); 
} 

void func2(void) 
{  
   printf("Hey, you entered bar! \n"); 
} 
     


            
int main(void) 
{ 
  
  char word[80]; 
  char ch; 
  
  do { 
    puts("Enter some text :"); 
    scanf("%s", word);  
    
   if ( !strcmp(word, "foo") ) { 
      func1(); 
  } 
  
   else if (!strcmp(word, "bar") ) { 
      func2(); 
  } 
  
   else  { 
   printf("Nope - I do not recognise that phrase.... \n"); 
  }     
  
   printf("Try again? (y/n) : "); 
   scanf(" %c%*c", &ch);  
  } 
  
    while( toupper(ch) != 'N' );  
       
  return 0; 

} 


         
