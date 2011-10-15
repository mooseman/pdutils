

/*  simple_shell.c                                             */  
/*  An ultra-simple shell to test reading command history and  */ 
/*  simple line editing.                                       */  
/*  This code is released to the public domain.                */ 
/*  "Share and enjoy...."  ;)                                  */ 
 

#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <ctype.h>


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

/* Read 1 character */
/* If we read a Ctrl key, Alt key or arrow key, we do not echo. */ 
/* Otherwise, we do. */ 

char getch_(void) {
    char ch; 
    ch = getchar();  
    /* Test for the character type so we can decide whether to */ 
    /* echo or not. */ 
    if (isprint(ch) ) {initTermios(1);} 
    else {initTermios(0); }      
    resetTermios();
    return ch;
}



/*  If the key pressed is an arrow key, backspace or a */ 
/* function key, do not echo it. Otherwise, echo it.   */       
void isarrow(void)
{ 
  int key = getch_();  
  
  if(key == 10) 
     { puts("You pressed ENTER! \n"); }    
  
  if(key == 27)  
     { key = getch_(); 
         if(key == 91) 
             key = getch_(); 
             if(key == 65)                              
               { puts("You pressed up arrow! \n"); 		         
               }  
             else if(key == 66)                              
               { puts("You pressed down arrow! \n"); 		         
               }  
             else if(key == 67)                              
               { puts("You pressed right arrow! \n"); 		         
               }  
             else if(key == 68)                              
               { puts("You pressed left arrow! \n"); 		         
               }                                   
          }          
}     






int main() 
{               
            
    while (1) {
    /*  Get keystrokes here  */  
     
    printf("@> ");                         
                                    
    isarrow();                                                         
                
    }     
                         
    return 0; 
    
}


