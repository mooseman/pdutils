/*
    getch() and getche() functionality for UNIX,
    based on termios (terminal handling functions)
   
    This code snippet was written by Wesley Stessens (wesley@ubuntu.com)
    It is released in the Public Domain. 
    * 
    testgetch() function added by Andy Elvey - also released to the 
    public domain.   
*/
 
#include <termios.h>
#include <stdio.h>
#include <ctype.h> 
#include <string.h> 
 
 
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


/*  See if it is possible to do a getch() function which only 
 *  echoes alphanumeric chars (but not, say, arrow keys, ctrl keys 
 *  and so on. ) 
 *  This should be possible, but we may need to do another version of 
 *  getch_ in order to do it. 
 */ 
      
      
char testgetch(void) { 
    char word[80];  
    int len;
    
    /* ch = getchar();   */  
    scanf("%s", word); 
    len = strlen(word); 
    
    if (len == 1) { 
       if ( isgraph(word) ) 
        { 
        return getch_(0); 
        } 
    else { 
        return getch_(1); 
         } 
    }      
    
    else { 
        return word;         
         } 
    
}      /*  testgetch()  */      


/* Let's test it out */
int main() {
    char c;
    printf("(getche example) please type a letter: ");
    c = getche();
    printf("\nYou typed: %c\n", c);
    printf("(getch example) please type a letter...");
    c = getch();
    printf("\nYou typed: %c\n", c); 
    c = testgetch();
    printf("\nYou typed: %c\n", c);     
    return 0; 
    
} 


