

/*  pdmenu.c                                                */  
/*  This code is aimed at helping those who would like to   */ 
/*  create menus without using the curses library.          */ 
/*  It creates a count variable (which could be used to     */ 
/*  keep track of which item is highlighted). Using the     */ 
/*  up and down arrow keys increments and decrements the    */ 
/*  count variable. Finally, when you press Enter, the      */ 
/*  value of the count variable is printed and the program  */ 
/*  exits.                                                  */       
/*  This code is released to the public domain.             */ 
/*  "Share and enjoy...."  ;)                               */  


#include <string.h>   
#include <stdio.h> 
#include <termios.h>  /* For getch()  */  

/* This implementation of getch() is from here - */ 
/* http://wesley.vidiqatch.org/                  */ 
/* Thanks, Wesley!                               */  
static struct termios old, new;

/* Initialize new terminal i/o settings */
void initTermios(int echo) {
    tcgetattr(0, &old); /* grab old terminal i/o settings */
    new = old; /* make new settings same as old settings */
    new.c_lflag &= ~ICANON; /* disable buffered i/o */
    new.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
    tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) {
    tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) {
    char ch;
    initTermios(echo);
    ch = getchar();
    resetTermios();
    return ch;
}

/* Read 1 character without echo */
char getch(void) {
    return getch_(0);
}

/* Read 1 character with echo */
char getche(void) {
    return getch_(1);
} 



int main(void)
{
	
  printf("Public Domain Menu Program \n");              
  printf("\nUse the up and down arrow keys then press Enter\n"); 
  
  int menunum = 0;            
  
  while(1) 
    {  
                   
      int key = getch(); 
                                                                       
      /* Up arrow is 27, 91, 65.    ( ESC [ A )   */   
      /* Down arrow is 27, 91, 66.  ( ESC [ B )   */ 
      /* Right arrow is 27, 91, 67. ( ESC [ C )   */ 
      /* Left arrow is 27, 91, 68.  ( ESC [ D )   */              
      if(key == 27)  
          {   key = getch(); 
              if(key == 91) 
              key = getch(); 
              if(key == 65)                              
               { puts("You pressed up arrow! \n"); 
		         menunum-=1; 
                 printf("Menunum is now %d \n", menunum);  
               }  
              else if(key == 66)                              
               { puts("You pressed down arrow! \n"); 
		         menunum+=1; 
                 printf("Menunum is now %d \n", menunum);  
               }  
              else if(key == 67)                              
               { puts("You pressed right arrow! \n"); 		         
               }  
              else if(key == 68)                              
               { puts("You pressed left arrow! \n"); 		         
               }                
          }
                                                          
    /* The Enter key exits. Enter is 10 decimal */	  
        else if(key == 10)  
		{ printf("You pressed ENTER! You chose item %d \n", menunum);  
          break; }  
    }                
                                      
	return 0;
}  




